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
      return 1;
   }

   double frac = numer / denom;
   return sqrt(frac * (1 - frac) / denom);
}

double GetSystematic(double rdpEfficiency, double mcpEfficiency,
   double rdpError, double mcpError)
{
   return sqrt((rdpEfficiency - mcpEfficiency) * (rdpEfficiency - mcpEfficiency) +
      rdpError * rdpError + mcpError * mcpError);
}

double GetSystematicUncertainty(double rdpEfficiency, double mcpEfficiency)
{
   return sqrt((rdpEfficiency - mcpEfficiency) * (rdpEfficiency - mcpEfficiency));
}

double GetSystematicError(double rdpEffErr, double mcpEffErr)
{
   return sqrt(rdpEffErr * rdpEffErr + mcpEffErr * mcpEffErr);
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
   const std::string& cut, const int numBins, double* bins, TH1F& histogram,
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
      if((efficiency + uncertainty) > 1)
      {
         herr->at(i - 1) = 1 - efficiency;
      }
      else
      {
         herr->at(i - 1) = uncertainty;
      }

      if((efficiency - uncertainty) < 0)
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

std::vector<double> DrawingToolsTPCECal::GetEfficiency(DataSample& data1,
   DataSample& data2, const std::string& variable, const std::string& signal,
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

   TH1F* selec1 = DrawingToolsBase::GetHisto(data1.GetTree(), "selec", variable,
      numBins, bins, sel, "", "", 1);
   TH1F* total1 = DrawingToolsBase::GetHisto(data1.GetTree(), "total", variable,
      numBins, bins, signal, "", "", 1);
   TH1F* selec2 = DrawingToolsBase::GetHisto(data2.GetTree(), "selec", variable,
      numBins, bins, sel, "", "", 1);
   TH1F* total2 = DrawingToolsBase::GetHisto(data2.GetTree(), "total", variable,
      numBins, bins, signal, "", "", 1);

   selec1->Sumw2();
   selec2->Sumw2();
   TH1F selec = (*selec1) + (*selec2);

   total1->Sumw2();
   total2->Sumw2();
   TH1F total = (*total1) + (*total2);

   for(int i = 1; i < numBins + 1; i++)
   {
      double selecContent = selec.GetBinContent(i);
      double totalContent = total.GetBinContent(i);
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
      if((efficiency + uncertainty) > 1)
      {
         herr->at(i - 1) = 1 - efficiency;
      }
      else
      {
         herr->at(i - 1) = uncertainty;
      }

      if((efficiency - uncertainty) < 0)
      {
         lerr->at(i - 1) = efficiency;
      }
      else
      {
         lerr->at(i - 1) = uncertainty;
      }
   }

   delete total1;
   delete selec1;
   delete total2;
   delete selec2;

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
/*      double systematic = GetSystematic(data_eff.at(i), mc_eff.at(i),
         data_errs.at(i), mc_errs.at(i));

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
      }*/
      double systematic = GetSystematicUncertainty(data_eff.at(i), mc_eff.at(i));
      double error = GetSystematicError(data_errs.at(i), mc_errs.at(i));

      // Not -nan or inf
      if(!isnan(systematic) && !isinf(systematic))
      {
         histogram.SetBinContent(i + 1, systematic);
         histogram.SetBinError(i + 1, error);
      }
      else
      {
         cout << "Error in bin " << bins[i] << " - " << bins[i + 1] <<
            " inf or nan propagated!" << endl;
         histogram.SetBinContent(i + 1, 0);
         histogram.SetBinError(i + 1, error);
      }
   }

   histogram.SetMinimum(0);
   Plot(histogram, options, legend);
   gPad->Update();
}

void DrawingToolsTPCECal::PlotSystematic(DataSample& nuRdp, DataSample& nubarRdp,
   DataSample& nuMcp, DataSample& nubarMcp,
   const std::string& variable, const std::string& signal,
   const std::string& cut, int numBins, double* bins, TH1F& histogram,
   const std::string& options, const std::string& legend)
{
   std::vector<double> data_errs(numBins);
   std::vector<double> mc_errs(numBins);

   std::vector<double> data_eff = GetEfficiency(nuRdp, nubarRdp, variable, signal,
      cut, numBins, bins, &data_errs);
   std::vector<double> mc_eff = GetEfficiency(nuMcp, nubarMcp, variable, signal,
      cut, numBins, bins, &mc_errs);

   for(int i = 0; i < numBins; i++)
   {
/*      double systematic = GetSystematic(data_eff.at(i), mc_eff.at(i),
         data_errs.at(i), mc_errs.at(i));

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
      }*/
      double systematic = GetSystematicUncertainty(data_eff.at(i), mc_eff.at(i));
      double error = GetSystematicError(data_errs.at(i), mc_errs.at(i));

      // Not -nan or inf
      if(!isnan(systematic) && !isinf(systematic))
      {
         histogram.SetBinContent(i + 1, systematic);
         histogram.SetBinError(i + 1, error);
      }
      else
      {
         cout << "Error in bin " << bins[i] << " - " << bins[i + 1] <<
            " inf or nan propagated!" << endl;
         histogram.SetBinContent(i + 1, 0);
         histogram.SetBinError(i + 1, error);
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

void DrawingToolsTPCECal::Plot(TMultiGraph& graph,
   TGraphAsymmErrors& rdpGraph, TGraphAsymmErrors& mcpGraph,
   const std::string& options, const std::vector<std::string>& legend)
{
   graph.Add(&rdpGraph);
   graph.Add(&mcpGraph);

   graph.Draw(options.c_str());
 
   graph.GetXaxis()->SetTitle(_titleX.c_str());
   graph.GetYaxis()->SetTitle(_titleY.c_str());
   graph.SetTitle(_title.c_str());
   if(_range)
   {
      graph.SetMinimum(_min);
      graph.SetMaximum(_max);
   }
   else
   {
      graph.SetMinimum(0);
      graph.SetMaximum(1.1);
   }

   rdpGraph.SetMarkerColor(_auto_colors[0]);
   rdpGraph.SetLineColor(_auto_colors[0]);
   rdpGraph.SetMarkerStyle(21);

   CreateLegend();
   _legends.back()->AddEntry(&rdpGraph, legend[0].c_str(), "LE1P");
   _legends.back()->Draw();

   mcpGraph.SetMarkerColor(_auto_colors[1]);
   mcpGraph.SetLineColor(_auto_colors[1]);
   mcpGraph.SetMarkerStyle(21);

   _legends.back()->AddEntry(&mcpGraph, legend[1].c_str(), "LE1P");
   _legends.back()->Draw();

   gStyle->SetOptStat(0);

   gPad->Modified();
}
