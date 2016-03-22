#include "DrawingToolsTPCECal.hxx"
#include <iomanip>
#include <iostream>
#include <fstream>

#include "TF1.h"
#include "TMultiGraph.h"
#include "TGraphErrors.h"
#include "TSystem.h"
#include "TPaveText.h"

double GetBinomialUncertainty(double numer, double denom)
{
   if(denom == 0)
   {
      return 0;
   }

   double frac = numer / denom;
   return sqrt(frac * (1 - frac) / denom);
}

DrawingToolsTPCECal::DrawingToolsTPCECal(const std::string& file,
   bool useT2Kstyle): DrawingTools(file, useT2Kstyle)
{
   _treeForSystErrors = NULL;
   _range = false;
}

DrawingToolsTPCECal::DrawingToolsTPCECal(Experiment& exp, bool useT2Kstyle):
   DrawingTools(exp.GetFilePath(), useT2Kstyle)
{
   _treeForSystErrors = NULL;
   _range = false;
}

void DrawingToolsTPCECal::PlotEfficiency(DataSample& data,
   const std::string& variable, const std::string& signal,
   const std::string& cut, int numBins, double* bins, TH1F& histogram,
   const std::string& options, const std::string& legend,
   std::vector<double>* errors)
{
   std::vector<double> fillvar(numBins);
   std::vector<double> staterrors(numBins);

   fillvar = GetEfficiency(data, variable, signal, cut, numBins, bins,
      &staterrors);

   for(int i = 1; i < numBins + 1; i++)
   {
      histogram.SetBinContent(i, fillvar.at(i - 1));
      // Add systematic errors to statistical errors.
      if(!isinf(staterrors.at(i - 1)))
      {
         histogram.SetBinError(i, staterrors.at(i - 1));
      }
      else
      {
         histogram.SetBinError(i, 0);
      }
      if(errors)
      {
         histogram.SetBinError(i, histogram.GetBinError(i) + errors->at(i - 1));
      }
   }

   histogram.SetAxisRange(0,1.1,"Y");

   Plot(histogram, options, legend);
   gPad->Update();
}

std::vector<double> DrawingToolsTPCECal::GetEfficiency(DataSample& data,
   const std::string& variable, const std::string& signal,
   const std::string& cut, int numBins, double* bins, std::vector<double>* lerr,
   std::vector<double>* herr)
{
   if(lerr)
   {
      lerr->resize(numBins);
   }
   if(herr)
   {
      herr->resize(numBins);
   }

   std::vector<double> efficiencies(numBins);
   string sel = cut + " && " + signal;

   TH1F* selec = DrawingToolsBase::GetHisto(data.GetTree(), "selec", variable,
      numBins, bins, sel, "", "", 1);
   TH1F* total = DrawingToolsBase::GetHisto(data.GetTree(), "total", variable,
      numBins, bins, signal, "", "", 1);

   for(int i = 1; i < numBins + 1; i++)
   {
      double selecContent = selec->GetBinContent(i);
      double totalContent = total->GetBinContent(i);
      if(totalContent != 0)
      {
         efficiencies.at(i - 1) = selecContent / totalContent;
      }

      // Sort out errors.
      if(!lerr && !herr)
      {
         continue;
      }
      // Uncertainty in fraction
      double uncertainty = GetBinomialUncertainty(selecContent, totalContent);

      if(lerr && !herr)
      {
         lerr->at(i - 1) = uncertainty;
         continue;
      }

      double efficiency = efficiencies.at(i - 1);
      // Efficiency cannot be > 1 or < 0 so we limit the uncertainties.
      if((uncertainty + efficiency) > 1)
      {
         herr->at(i - 1) = 1 - efficiency;
      }
      else
      {
         herr->at(i - 1) = uncertainty;
      }

      if((uncertainty - efficiency) < 0)
      {
         lerr->at(i - 1) = efficiency;
      }
      else
      {
         lerr->at(i - 1) = uncertainty;
      }
   }

   delete total;
   delete selec;

   return efficiencies;
}

void DrawingToolsTPCECal::PlotSystematic(DataSample& rdp, DataSample& mcp,
   const std::string& variable, const std::string& signal,
   const std::string& cut, int numBins, double* bins, TH1F& histogram,
   const std::string& options, const std::string& legend)
{
   std::vector<double> data_errs(numBins);
   std::vector<double> mc_errs(numBins);

   std::vector<double> data_eff = GetEfficiency(rdp, variable, signal, cut, numBins,
      bins, &data_errs);
   std::vector<double> mc_eff = GetEfficiency(mcp, variable, signal, cut, numBins,
      bins, &mc_errs);

   for(int i = 0; i < numBins; i++)
   {
      double rdpEff = data_eff.at(i);
      double mcpEff = mc_eff.at(i);
      double rdpErr = data_errs.at(i);
      double mcpErr = mc_errs.at(i);
      // Difference in data and MC squared + errors squared.
      double systematic = sqrt((rdpEff - mcpEff) * (rdpEff - mcpEff) +
         rdpErr * rdpErr + mcpErr * mcpErr);

      // Not -nan or inf
      if(!isnan(systematic) && !isinf(systematic))
      {
         histogram.SetBinContent(i + 1, systematic);
         // Don't want errors drawn to systematics plots.
         histogram.SetBinError(i + 1, 0.0000001);
      }
      else
      {
         cout << "Error in bin " << bins[i] << " - " << bins[i + 1] <<
            " inf or nan propagated!" << endl;
         histogram.SetBinContent(i + 1, 0);
         histogram.SetBinError(i + 1, 0.0000001);
      }
   }

   histogram.SetMinimum(0);
   Plot(histogram, options, legend);
   gPad->Update();
}

void DrawingToolsTPCECal::Plot(TH1& histogram, const std::string& options,
   const std::string& legend)
{
   bool same = (ToUpper(options).find("SAME") != string::npos);

   histogram.GetXaxis()->SetTitle(_titleX.c_str());
   histogram.GetYaxis()->SetTitle(_titleY.c_str());
   histogram.SetTitle(_title.c_str());
   if(_range)
   {
      histogram.SetAxisRange(_min, _max, "Y");
   }

   if(same)
   {
      _same_level++;
   }
   else
   {
      _same_level = 0;
   }

   histogram.SetLineColor(_auto_colors[_same_level]);
   histogram.SetFillColor(_auto_colors[_same_level]);
   histogram.SetMarkerColor(_auto_colors[_same_level]);
   histogram.SetMarkerStyle(_auto_markers[_same_level]);
   gStyle->SetOptStat(0);

   histogram.Draw(options.c_str());

   if(legend != "")
   {
      if(!same)
      {
         CreateLegend();
      }
      _legends.back()->AddEntry(&histogram, legend.c_str(), "LE1P");
      _legends.back()->Draw();
   }
}
