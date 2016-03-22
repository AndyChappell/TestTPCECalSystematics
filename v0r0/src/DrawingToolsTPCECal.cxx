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

DrawingToolsTPCECal::DrawingToolsTPCECal(const string& file, bool useT2Kstyle):
   DrawingTools(file, useT2Kstyle)
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

void DrawingToolsTPCECal::DrawEfficiency(DataSample& data, const string var,
   const string& signal, const string& cut, int nx, double *xbins, string opt,
   string leg, vector<double> *errors)
{
   vector<double> fillvar(nx);
   vector<double> staterrors(nx);

   fillvar = GetEfficiencyVsBin(data, var, signal, cut, nx, xbins, &staterrors);

   TH1F *hist = new TH1F("", "", nx, xbins);

   for(int i = 1; i < nx + 1; i++)
   {
      hist->SetBinContent(i, fillvar.at(i-1));
      // Add systematic errors to statistical errors.
      if(!isinf(staterrors.at(i-1)))
      {
         hist->SetBinError(i, staterrors.at(i-1));
      }
      else
      {
         hist->SetBinError(i, 0);
      }
      if(errors)
      {
         hist->SetBinError(i, hist->GetBinError(i) + errors->at(i-1));
      }
   }

   hist->SetAxisRange(0,1.1,"Y");

   DrawPlot(hist, opt, leg);
   gPad->Update();
}

vector<double> DrawingToolsTPCECal::GetEfficiencyVsBin(DataSample& data,
   const string var, const string& signal, const string& cut, int nx,
   double *xbins, vector<double> *lerr, vector<double> *herr)
{
   if(lerr)
   {
      lerr->resize(nx);
   }
   if(herr)
   {
      herr->resize(nx);
   }

   vector<double> efficiencies(nx);
   double temp = 0;
   string sel = cut + " && " + signal;

   TH1F* selec = DrawingToolsBase::GetHisto(data.GetTree(), "selec", var, nx,
      xbins, sel, "", "", 1);
   TH1F* total = DrawingToolsBase::GetHisto(data.GetTree(), "total", var, nx,
      xbins, signal, "", "", 1);

   for(int i = 1; i < nx + 1; i++)
   {
      if(total->GetBinContent(i) != 0)
      {
         efficiencies.at(i-1) = selec->GetBinContent(i) /
            total->GetBinContent(i);
      }

      // Sort out errors.
      if(!lerr && !herr)
      {
         continue;
      }
      // Uncertainty in fraction
      temp = GetBinomialUncertainty(selec->GetBinContent(i),
         total->GetBinContent(i));

      if(lerr && !herr)
      {
         lerr->at(i-1) = temp;
         continue;
      }

      // Efficiency cannot be > 1 or < 0 so we limit the uncertainties.
      if(temp + efficiencies.at(i-1) > 1)
      {
         herr->at(i-1) = 1 - efficiencies.at(i-1);
      }
      else
      {
         herr->at(i-1) = temp;
      }

      if(temp - efficiencies.at(i-1) < 0)
      {
         lerr->at(i-1) = efficiencies.at(i-1);
      }
      else
      {
         lerr->at(i-1) = temp;
      }
   }

   delete total;
   delete selec;

   return efficiencies;
}

void DrawingToolsTPCECal::CalculateSystematic(DataSample& rdp, DataSample& mcp,
   const std::string& var, const std::string& signal, const std::string& cut,
   int nx, double* xbins, const string& opt, const string& leg)
{
   vector<double> data_errs(nx);
   vector<double> mc_errs(nx);

   vector<double> data_eff = GetEfficiencyVsBin(rdp, var, signal, cut, nx,
      xbins, &data_errs);
   vector<double> mc_eff = GetEfficiencyVsBin(mcp, var, signal, cut, nx, xbins,
      &mc_errs);

   double FillVar = 0;

   TH1F *hist = new TH1F("", "", nx, xbins);
   for(int i = 0; i < nx; i++)
   {
      // Difference in data and MC squared + errors squared.
      FillVar = sqrt((data_eff.at(i) - mc_eff.at(i)) *
         (data_eff.at(i) - mc_eff.at(i)) + mc_errs.at(i) * mc_errs.at(i) +
         data_errs.at(i)*data_errs.at(i));

      // Not -nan or inf
      if(FillVar == FillVar && !isinf(FillVar))
      {
         hist->SetBinContent(i+1, FillVar);
         // Don't want errors drawn to systematics plots.
         hist->SetBinError(i+1, 0.0000001);
         cout << "For bin: " << xbins[i] << " - " << xbins[i + 1] <<
            ", uncertainty = " << FillVar << endl;
      }
      else
      {
         cout << "Error in bin " << xbins[i] << " - " << xbins[i + 1] <<
            " inf or nan propagated!" << endl;
         cout << "data eff = " << data_eff.at(i) << " +/- " <<
            data_errs.at(i) << ", mc eff = " << mc_eff.at(i) << " +/- " <<
            mc_errs.at(i) << endl;
         hist->SetBinContent(i+1, 0);
         hist->SetBinError(i+1, 0.0000001);
      }
   }

   hist->SetMinimum(0);
   DrawPlot(hist, opt, leg);
   gPad->Update();
}

void DrawingToolsTPCECal::DrawPlot(TH1* hist,
   const std::string& opt, const std::string& leg)
{
   bool same = (ToUpper(opt).find("SAME") != string::npos);

   hist->GetXaxis()->SetTitle(_titleX.c_str());
   hist->GetYaxis()->SetTitle(_titleY.c_str());
   hist->SetTitle(_title.c_str());
   if(_range)
   {
      hist->SetAxisRange(_min, _max, "Y");
   }

   if(same)
   {
      _same_level++;
   }
   else
   {
      _same_level = 0;
   }

   hist->SetLineColor(_auto_colors[_same_level]);
   hist->SetFillColor(_auto_colors[_same_level]);
   hist->SetMarkerColor(_auto_colors[_same_level]);
   hist->SetMarkerStyle(_auto_markers[_same_level]);
   gStyle->SetOptStat(0);

   hist->Draw(opt.c_str());

   if(leg != "")
   {
      if(!same)
      {
         CreateLegend();
      }
      _legends.back()->AddEntry(hist, leg.c_str(), "LE1P");
      _legends.back()->Draw();
   }
}
