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
   _multigraph = nullptr;
   _histogram1 = nullptr;
}

DrawingToolsTPCECal::DrawingToolsTPCECal(Experiment& exp, bool useT2Kstyle):
   DrawingTools(exp.GetFilePath(), useT2Kstyle)
{
   _treeForSystErrors = NULL;
   _range = false;
   _multigraph = nullptr;
   _histogram1 = nullptr;
}

DrawingToolsTPCECal::~DrawingToolsTPCECal()
{
   if(_multigraph)
   {
      delete _multigraph;
   }
   if(_histogram1)
   {
      delete _histogram1;
   }
}

TGraphAsymmErrors* DrawingToolsTPCECal::CreateEfficiencyGraph(DataSample& data,
   const std::string& variable, const std::string& signal,
   const std::string& cut, int n, double* bins)
{
   std::vector<double> lerrs(n);
   std::vector<double> herrs(n);
   std::vector<double> effs = GetEfficiency(data, variable, signal, cut, n,
      bins, &lerrs, &herrs); 

   double x[n];
   double y[n];
   double xlerrs[n];
   double xherrs[n];
   double ylerrs[n];
   double yherrs[n];
   for(int i = 0; i < n; i++)
   {
      x[i] = (bins[i] + bins[i + 1]) / 2.0;
      y[i] = effs[i];
      xlerrs[i] = x[i] - bins[i];
      xherrs[i] = xlerrs[i];
      ylerrs[i] = lerrs[i];
      yherrs[i] = herrs[i];
   }
   
   return new TGraphAsymmErrors(n, x, y, xlerrs, xherrs, ylerrs, yherrs);
}

TGraphAsymmErrors* DrawingToolsTPCECal::CreateEfficiencyGraph(DataSample& data,
   DataSample& antidata, const std::string& variable, const std::string& signal,
   const std::string& cut, int n, double* bins)
{
   std::vector<double> lerrs(n);
   std::vector<double> herrs(n);
   std::vector<double> effs = GetEfficiency(data, antidata, variable, signal,
      cut, n, bins, &lerrs, &herrs); 

   double x[n];
   double y[n];
   double xlerrs[n];
   double xherrs[n];
   double ylerrs[n];
   double yherrs[n];
   for(int i = 0; i < n; i++)
   {
      x[i] = (bins[i] + bins[i + 1]) / 2.0;
      y[i] = effs[i];
      xlerrs[i] = x[i] - bins[i];
      xherrs[i] = xlerrs[i];
      ylerrs[i] = lerrs[i];
      yherrs[i] = herrs[i];
   }
   
   return new TGraphAsymmErrors(n, x, y, xlerrs, xherrs, ylerrs, yherrs);
}

void DrawingToolsTPCECal::PlotEfficiency(DataSample& rdp, DataSample& mcp,
   const std::string& variable, const std::string& signal,
   const std::string& cut, const int numBins, double* bins, bool isAnti)
{
   if(_multigraph)
   {
      delete _multigraph;
      _multigraph = nullptr;
   }

   TGraphAsymmErrors* graph1 = CreateEfficiencyGraph(rdp, variable, signal, cut,
      numBins, bins);
   TGraphAsymmErrors* graph2 = CreateEfficiencyGraph(mcp, variable, signal, cut,
      numBins, bins);

   std::vector<std::string> legend;
   legend.push_back(isAnti ? "#bar{#nu} Data" : "#nu Data");
   legend.push_back(isAnti ? "#bar{#nu} MC" : "#nu MC");
   _multigraph = new TMultiGraph();
   Plot(*_multigraph, *graph1, *graph2, "AP", legend);
   gPad->Update();
}

void DrawingToolsTPCECal::PlotEfficiency(DataSample& rdp, DataSample& rdpbar,
   DataSample& mcp, DataSample& mcpbar, const std::string& variable,
   const std::string& signal, const std::string& cut, const int numBins,
   double* bins)
{
   if(_multigraph)
   {
      delete _multigraph;
      _multigraph = nullptr;
   }

   TGraphAsymmErrors* graph1 = CreateEfficiencyGraph(rdp, rdpbar, variable,
      signal, cut, numBins, bins);
   TGraphAsymmErrors* graph2 = CreateEfficiencyGraph(mcp, mcpbar, variable,
      signal, cut, numBins, bins);

   std::vector<std::string> legend;
   legend.push_back("Data");
   legend.push_back("MC");
   _multigraph = new TMultiGraph();
   Plot(*_multigraph, *graph1, *graph2, "AP", legend);
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
   const std::string& cut, int numBins, double* bins,
   const std::string& options, bool isAnti)
{
   if(_histogram1)
   {
      delete _histogram1;
      _histogram1 = nullptr;
   }
   _histogram1 = new TH1F("", "", numBins, bins);
   
   std::vector<double> data_errs(numBins);
   std::vector<double> mc_errs(numBins);

   std::vector<double> data_eff = GetEfficiency(rdp, variable, signal, cut, numBins,
      bins, &data_errs);
   std::vector<double> mc_eff = GetEfficiency(mcp, variable, signal, cut, numBins,
      bins, &mc_errs);

   for(int i = 0; i < numBins; i++)
   {
      double systematic = GetSystematicUncertainty(data_eff.at(i), mc_eff.at(i));
      double error = GetSystematicError(data_errs.at(i), mc_errs.at(i));

      // Not -nan or inf
      if(!isnan(systematic) && !isinf(systematic))
      {
         _histogram1->SetBinContent(i + 1, systematic);
         _histogram1->SetBinError(i + 1, error);
      }
      else
      {
         cout << "Error in bin " << bins[i] << " - " << bins[i + 1] <<
            " inf or nan propagated!" << endl;
         _histogram1->SetBinContent(i + 1, 0);
         _histogram1->SetBinError(i + 1, error);
      }
   }

   _histogram1->SetMinimum(0);
   std::string legend = isAnti ? "#bar{#nu}" : "#nu";
   Plot(*_histogram1, options, legend);
   gPad->Update();
}

void DrawingToolsTPCECal::PlotSystematic(DataSample& nuRdp, DataSample& nubarRdp,
   DataSample& nuMcp, DataSample& nubarMcp,
   const std::string& variable, const std::string& signal,
   const std::string& cut, int numBins, double* bins,
   const std::string& options)
{
   if(_histogram1)
   {
      delete _histogram1;
      _histogram1 = nullptr;
   }
   _histogram1 = new TH1F("", "", numBins, bins);
   
   std::vector<double> data_errs(numBins);
   std::vector<double> mc_errs(numBins);

   std::vector<double> data_eff = GetEfficiency(nuRdp, nubarRdp, variable, signal,
      cut, numBins, bins, &data_errs);
   std::vector<double> mc_eff = GetEfficiency(nuMcp, nubarMcp, variable, signal,
      cut, numBins, bins, &mc_errs);

   for(int i = 0; i < numBins; i++)
   {
      double systematic = GetSystematicUncertainty(data_eff.at(i), mc_eff.at(i));
      double error = GetSystematicError(data_errs.at(i), mc_errs.at(i));

      // Not -nan or inf
      if(!isnan(systematic) && !isinf(systematic))
      {
         _histogram1->SetBinContent(i + 1, systematic);
         _histogram1->SetBinError(i + 1, error);
      }
      else
      {
         cout << "Error in bin " << bins[i] << " - " << bins[i + 1] <<
            " inf or nan propagated!" << endl;
         _histogram1->SetBinContent(i + 1, 0);
         _histogram1->SetBinError(i + 1, error);
      }
   }

   _histogram1->SetMinimum(0);
   Plot(*_histogram1, options, "");
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
