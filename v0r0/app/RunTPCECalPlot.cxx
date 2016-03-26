#include "DrawingToolsTPCECal.hxx"
#include "Bins.hxx"

using TPCECalSystematics::Bins;

typedef std::vector<std::string> vecstr;
typedef std::vector<Bins> BinsVector;

const vecstr GetEnvironmentVariables(bool mcp)
{
   vecstr envVars;
   if(mcp)
   {  // MCP data
      envVars.push_back("E_MCP_FILE");
      envVars.push_back("MU_MCP_FILE");
      envVars.push_back("P_MCP_FILE");
      envVars.push_back("EBAR_MCP_FILE");
      envVars.push_back("MUBAR_MCP_FILE");
   }
   else
   {  // RDP data
      envVars.push_back("E_RDP_FILE");
      envVars.push_back("MU_RDP_FILE");
      envVars.push_back("P_RDP_FILE");
      envVars.push_back("EBAR_RDP_FILE");
      envVars.push_back("MUBAR_RDP_FILE");
   }
   
   return envVars;
}

void CreateBins(BinsVector& dsMomBins, BinsVector& brMomBins,
   BinsVector& dsAngBins, BinsVector& brAngBins)
{
   // e-like bins
   double ebins1[8] = {0., 100., 200., 300., 400., 500., 800., 1500.};
   Bins elikeDSMom(ebins1, 7);
   double ebins2[5] = {0., 50., 100., 300., 600.};
   Bins elikeBrMom(ebins2, 4);
   double ebins3[6] = {0.75, 0.8, 0.85, 0.925, 0.975, 1.0};
   Bins elikeDSAng(ebins3, 5);
   double ebins4[6] = {-0.3, 0.3, 0.45, 0.65, 0.75, 0.825};
   Bins elikeBrAng(ebins4, 5);

   // mu-like bins
   double mubins1[13] = {0., 250., 500., 750., 1000., 1250., 1500., 1750., 2000.,
      2500., 3000., 3500., 4000.};
   Bins mulikeDSMom(mubins1, 12);
   double mubins2[10] = {0., 200., 300., 400., 500., 600., 700., 800., 1200.,
      1600.};
   Bins mulikeBrMom(mubins2, 9);
   double mubins3[9] = {0.75, 0.8, 0.85, 0.875, 0.9, 0.925, 0.95, 0.975, 1.0};
   Bins mulikeDSAng(mubins3, 8);
   double mubins4[6] = {-0.3, 0.3, 0.5, 0.6, 0.7, 0.825};
   Bins mulikeBrAng(mubins4, 5);

   // Proton bins
   double pbins1[7] = {0., 300., 600., 900., 1200., 1500., 2000.};
   Bins plikeDSMom(pbins1, 6);
   double pbins2[9] = {0., 300., 400., 500., 600., 700., 800., 900., 1000.};
   Bins plikeBrMom(pbins2, 8);
   double pbins3[8] = {0.75, 0.85, 0.875, 0.9, 0.925, 0.95, 0.975, 1.0};
   Bins plikeDSAng(pbins3, 7);
   double pbins4[6] = {-0.3, 0.3, 0.5, 0.6, 0.7, 0.825};
   Bins plikeBrAng(pbins4, 5);

   dsMomBins.push_back(elikeDSMom);
   dsMomBins.push_back(mulikeDSMom);
   dsMomBins.push_back(plikeDSMom);
   dsMomBins.push_back(elikeDSMom);
   dsMomBins.push_back(mulikeDSMom);

   brMomBins.push_back(elikeBrMom);
   brMomBins.push_back(mulikeBrMom);
   brMomBins.push_back(plikeBrMom);
   brMomBins.push_back(elikeBrMom);
   brMomBins.push_back(mulikeBrMom);

   dsAngBins.push_back(elikeDSAng);
   dsAngBins.push_back(mulikeDSAng);
   dsAngBins.push_back(plikeDSAng);
   dsAngBins.push_back(elikeDSAng);
   dsAngBins.push_back(mulikeDSAng);

   brAngBins.push_back(elikeBrAng);
   brAngBins.push_back(mulikeBrAng);
   brAngBins.push_back(plikeBrAng);
   brAngBins.push_back(elikeBrAng);
   brAngBins.push_back(mulikeBrAng);
}

void GetFilenames(vecstr& rdpFiles, vecstr& mcpFiles)
{
   vecstr rdpEnvVars = GetEnvironmentVariables(false);
   vecstr mcpEnvVars = GetEnvironmentVariables(true);
   
   for(unsigned int i = 0; i < rdpEnvVars.size(); ++i)
   {
      std::string filename = std::string(getenv(rdpEnvVars[i].c_str()));
      if(filename.length() == 0)
      {
         std::cerr << "Error: Environment variable" << rdpEnvVars[i] <<
            " not set. Exiting." << std::endl;
         exit(1);
      }
      rdpFiles.push_back(filename);

      filename = std::string(getenv(mcpEnvVars[i].c_str()));
      if(filename.length() == 0)
      {
         std::cerr << "Error: Environment variable" << mcpEnvVars[i] <<
            " not set. Exiting." << std::endl;
         exit(1);
      }
      mcpFiles.push_back(filename);
   }
}

DataSample GetDataSample(std::string filename)
{
   DataSample data(filename.c_str());
   if(data.GetTree()->GetEntries() == 0)
   {
      std::cerr << "Error: Tree " << filename << " has no entries. Exiting." <<
         std::endl;
      exit(1);
   }
   return data;
}

void DrawSelection(DrawingToolsTPCECal& draw, TCanvas* c1,
   DataSample& rdp, DataSample& mcp, std::string& variable, Bins& bins,
   const std::string& signal, const std::string& detector,
   const std::string& particle, const std::string& varstr,
   const std::string& axis, const std::string& legpos)
{
   draw.SetLegendSize(0.2, 0.4);
   draw.SetLegendPos(legpos);
   draw.SetTitleX(axis);
   draw.SetTitleY("Counts/Bin");
   std::ostringstream ss;

   int n = bins.GetNumBins();
   double* boundaries = bins.GetBoundaries();
   draw.Draw(rdp, mcp, variable, n, boundaries, "particle", signal);
   ss << "sel_" << varstr << "_" << detector << "_" << particle << ".png";
   c1->Print(ss.str().c_str(), "png");
}

void DrawEfficiencies(DrawingToolsTPCECal& draw, TCanvas* c1,
   DataSample& rdp, DataSample& mcp, std::string& variable, Bins& bins,
   const std::string& signal, const std::string& cut,
   const std::string& detector, const std::string& particle,
   const std::string& varstr, const std::string& axis, const std::string& legpos)
{
   const int n = bins.GetNumBins();
   c1->Clear();
   draw.SetLegendSize(0.15, 0.1);
   draw.SetLegendPos(legpos);
   draw.SetTitleY("Matching Efficiency");
   draw.SetTitleX(axis);
   std::ostringstream ss;
 
   std::vector<double> lerrs(n);
   std::vector<double> herrs(n);
   std::vector<double> effs = draw.GetEfficiency(rdp, variable, signal, cut,
      bins.GetNumBins(), bins.GetBoundaries(), &lerrs, &herrs); 

   double x1[n];
   double y1[n];
   double xlerrs1[n];
   double xherrs1[n];
   double ylerrs1[n];
   double yherrs1[n];
   for(int i = 0; i < n; i++)
   {
      x1[i] = (bins[i] + bins[i + 1]) / 2.0;
      y1[i] = effs[i];
      xlerrs1[i] = x1[i] - bins[i];
      xherrs1[i] = xlerrs1[i];
      ylerrs1[i] = lerrs[i];
      yherrs1[i] = herrs[i];
   }
   TGraphAsymmErrors rdpGraph(n, x1, y1, xlerrs1, xherrs1, ylerrs1, yherrs1);
   
   effs = draw.GetEfficiency(mcp, variable, signal, cut, bins.GetNumBins(),
      bins.GetBoundaries(), &lerrs, &herrs); 

   double x2[n];
   double y2[n];
   double xlerrs2[n];
   double xherrs2[n];
   double ylerrs2[n];
   double yherrs2[n];
   for(int i = 0; i < n; i++)
   {
      x2[i] = (bins[i] + bins[i + 1]) / 2.0;
      y2[i] = effs[i];
      xlerrs2[i] = x2[i] - bins[i];
      xherrs2[i] = xlerrs2[i];
      ylerrs2[i] = lerrs[i];
      yherrs2[i] = herrs[i];
   }
   TGraphAsymmErrors mcpGraph(n, x2, y2, xlerrs2, xherrs2, ylerrs2, yherrs2);

   vecstr legend;
   legend.push_back((particle.find("bar") == std::string::npos) ? "#nu Data" : "#bar{#nu} Data");
   legend.push_back((particle.find("bar") == std::string::npos) ? "#nu MC" : "#bar{#nu} MC");
   TMultiGraph graph;
   draw.Plot(graph, rdpGraph, mcpGraph, "AP", legend);
   gPad->Update();

   ss << "eff_" << varstr << "_" << detector << "_" << particle << ".png";
   c1->Print(ss.str().c_str(), "png");
   c1->Clear();
}

void DrawCombinedEfficiencies(DrawingToolsTPCECal& draw, TCanvas* c1,
   DataSample& nuRdp, DataSample& nubarRdp, DataSample& nuMcp,
   DataSample& nubarMcp, std::string& variable, Bins& bins,
   const std::string& signal, const std::string& cut,
   const std::string& detector, const std::string& particle,
   const std::string& varstr, const std::string& legpos)
{
   int n = bins.GetNumBins();
   c1->Clear();
   draw.SetLegendSize(0.15, 0.1);
   draw.SetLegendPos("br");
   draw.SetTitleY("Matching Efficiency");
   draw.SetTitleX("Track Momentum (MeV)");
   std::ostringstream ss;
 
   std::vector<double> lerrs(n);
   std::vector<double> herrs(n);
   std::vector<double> effs = draw.GetEfficiency(nuRdp, nubarRdp, variable,
      signal, cut, bins.GetNumBins(), bins.GetBoundaries(), &lerrs, &herrs); 

   double x1[n];
   double y1[n];
   double xlerrs1[n];
   double xherrs1[n];
   double ylerrs1[n];
   double yherrs1[n];
   for(int i = 0; i < n; i++)
   {
      x1[i] = (bins[i] + bins[i + 1]) / 2.0;
      y1[i] = effs[i];
      xlerrs1[i] = x1[i] - bins[i];
      xherrs1[i] = xlerrs1[i];
      ylerrs1[i] = lerrs[i];
      yherrs1[i] = herrs[i];
   }
   TGraphAsymmErrors rdpGraph(n, x1, y1, xlerrs1, xherrs1, ylerrs1, yherrs1);
   
   effs = draw.GetEfficiency(nuMcp, nubarMcp, variable, signal, cut,
      bins.GetNumBins(), bins.GetBoundaries(), &lerrs, &herrs); 

   double x2[n];
   double y2[n];
   double xlerrs2[n];
   double xherrs2[n];
   double ylerrs2[n];
   double yherrs2[n];
   for(int i = 0; i < n; i++)
   {
      x2[i] = (bins[i] + bins[i + 1]) / 2.0;
      y2[i] = effs[i];
      xlerrs2[i] = x2[i] - bins[i];
      xherrs2[i] = xlerrs2[i];
      ylerrs2[i] = lerrs[i];
      yherrs2[i] = herrs[i];
   }
   TGraphAsymmErrors mcpGraph(n, x2, y2, xlerrs2, xherrs2, ylerrs2, yherrs2);

   vecstr legend;
   legend.push_back("Data");
   legend.push_back("MC");
   TMultiGraph graph;
   draw.Plot(graph, rdpGraph, mcpGraph, "AP", legend);
   gPad->Update();

   ss << "eff_" << varstr << "_" << detector << "_" << particle << ".png";
   c1->Print(ss.str().c_str(), "png");
   c1->Clear();
}

void DrawSystematics(DrawingToolsTPCECal& draw, TCanvas* c1,
   DataSample& rdp, DataSample& mcp, std::string& variable, Bins& bins,
   const std::string& signal, const std::string& cut,
   const std::string& detector, const std::string& particle,
   const std::string& varstr, const std::string& axis)
{
   draw.SetLegendSize(0.1, 0.05);
   draw.SetLegendPos("br");
   draw.SetTitleX(axis);
   draw.SetTitleY("Systematic Uncertainty");
   std::ostringstream ss;

   TH1F histogram("", "", bins.GetNumBins(), bins.GetBoundaries());
   draw.PlotSystematic(rdp, mcp, variable, signal, cut, bins.GetNumBins(),
      bins.GetBoundaries(), histogram, "e1", 
      (particle.find("bar") == std::string::npos) ? "#nu" : "#bar{#nu}");
   ss << "syst_" << varstr << "_" << detector << "_" << particle << ".png";
   c1->Print(ss.str().c_str(), "png");
}

void DrawDualSystematics(DrawingToolsTPCECal& draw, TCanvas* c1,
   DataSample& rdp, DataSample& ardp, DataSample& mcp, DataSample& amcp,
   std::string& variable, Bins& bins,
   const std::string& signal, const std::string& cut,
   const std::string& detector, const std::string& particle,
   const std::string& varstr)
{
   c1->Clear();
   draw.SetLegendSize(0.1, 0.05);
   draw.SetLegendPos("br");
   draw.SetTitleX("Track Momentum (MeV)");
   draw.SetTitleY("Systematic Uncertainty");
   std::ostringstream ss;

   TH1F histogram1("", "", bins.GetNumBins(), bins.GetBoundaries());
   draw.PlotSystematic(rdp, mcp, variable, signal, cut, bins.GetNumBins(),
      bins.GetBoundaries(), histogram1, "e1", "#nu");
   TH1F histogram2("", "", bins.GetNumBins(), bins.GetBoundaries());
   draw.PlotSystematic(ardp, amcp, variable, signal, cut, bins.GetNumBins(),
      bins.GetBoundaries(), histogram2, "same", "#bar{#nu}");
   ss << "syst_" << varstr << "_" << detector << "_" << particle << ".png";
   c1->Print(ss.str().c_str(), "png");
   c1->Clear();
}

void DrawCombinedSystematics(DrawingToolsTPCECal& draw, TCanvas* c1,
   DataSample& nuRdp, DataSample& nubarRdp, DataSample& nuMcp,
   DataSample& nubarMcp, std::string& variable, Bins& bins,
   const std::string& signal, const std::string& cut,
   const std::string& detector, const std::string& particle,
   const std::string& varstr)
{
   draw.SetLegendSize(0.1, 0.05);
   draw.SetLegendPos("br");
   draw.SetTitleX("Track Momentum (MeV)");
   draw.SetTitleY("Systematic Uncertainty");
   std::ostringstream ss;

   TH1F histogram("", "", bins.GetNumBins(), bins.GetBoundaries());
   draw.PlotSystematic(nuRdp, nubarRdp, nuMcp, nubarMcp, variable, signal,
      cut, bins.GetNumBins(), bins.GetBoundaries(), histogram, "e1", "");
   ss << "syst_" << varstr << "_" << detector << "_" << particle << ".png";
   c1->Print(ss.str().c_str(), "png");
}

void DrawPurity(DrawingToolsTPCECal& draw, TCanvas* c1, DataSample& mcp,
   const std::string& detector, const std::string& particle,
   const std::string& trueParticle, const int selection)
{
   draw.SetLegendSize(0.15, 0.1);
   draw.SetLegendPos("tl");
   draw.SetTitleX("Cut");
   draw.SetTitleY("Purity/Efficiency");
   std::ostringstream ss;

   ss << "particle==" << trueParticle;
   draw.DrawEffPurVSCut(mcp, selection, (detector == "ds") ? 0 : 1, ss.str(), "");
   ss.str(""); ss.clear();
   ss << "pur_" << detector << "_" << particle << ".png";
   c1->Print(ss.str().c_str(), "png");
}

void PrintSummaryDataUnbinned(DrawingToolsTPCECal& draw, DataSample& rdp,
   DataSample& mcp, const std::string& momentum, const std::string& signal,
   const std::string& cut, const std::string& detector,
   const std::string& particle)
{
   double bins[2] = {0, 100000};
   std::vector<double> errors(1);

   std::cout.setf(ios::fixed,ios::floatfield);
   std::cout.precision(3);

   double rdpEff = draw.GetEfficiency(rdp, momentum, signal, cut, 1, bins,
      &errors).at(0);
   double rdpErr = errors.at(0);
   double mcpEff = draw.GetEfficiency(mcp, momentum, signal, cut, 1, bins,
      &errors).at(0);
   double mcpErr = errors.at(0);
   std::cout << particle << " : " << detector << std::endl;
   std::cout << "rdp eff    = " << rdpEff << " +/- " << rdpErr << std::endl;
   std::cout << "mcp eff    = " << mcpEff << " +/- " << mcpErr << std::endl;

   //double systematic = GetSystematic(rdpEff, mcpEff, rdpErr, mcpErr);
   double systematic = GetSystematicUncertainty(rdpEff, mcpEff);
   double error = GetSystematicError(rdpErr, mcpErr);
   std::cout << "systematic = " << systematic<< " +/- " << error << std::endl << std::endl;
}

void PrintSummaryDataBinned(DrawingToolsTPCECal& draw, DataSample& rdp,
   DataSample& mcp, const std::string& variable, Bins& bins,
   const std::string& signal, const std::string& cut,
   const std::string& detector, const std::string& particle)
{
   const int n = bins.GetNumBins();
   std::vector<double> mcpErr(n);
   std::vector<double> rdpErr(n);
   
   std::cout.setf(ios::fixed, ios::floatfield);
   std::cout.precision(3);

   std::vector<double> rdpEff = draw.GetEfficiency(rdp, variable, signal, cut,
      n, bins.GetBoundaries(), &rdpErr);
   std::vector<double> mcpEff = draw.GetEfficiency(mcp, variable, signal, cut,
      n, bins.GetBoundaries(), &mcpErr);

   std::cout << particle << " : " << detector << std::endl;
   for(int i = 0; i < n; i++)
   {
      std::cout << "For bin: " << bins[i] << " - " << bins[i + 1] << std::endl;
      std::cout << "rdp eff    = " << rdpEff[i] << " +/- " << rdpErr[i] <<
         std::endl;
      std::cout << "mcp eff    = " << mcpEff[i] << " +/- " << mcpErr[i] <<
         std::endl;

      /*double systematic = GetSystematic(rdpEff[i], mcpEff[i], rdpErr[i],
         mcpErr[i]);*/
      double systematic = GetSystematicUncertainty(rdpEff[i], mcpEff[i]);
      std::cout << "systematic = " << systematic << std::endl << std::endl;
   }
}

void PrintLaTeXSummaryDataUnbinned(DrawingToolsTPCECal& draw, DataSample& rdp,
   DataSample& mcp, const std::string& momentum, const std::string& signal,
   const std::string& cut, const std::string& detector,
   const std::string& particle)
{
   double bins[2] = {0, 100000};
   std::vector<double> errors(1);

   std::cout.setf(ios::fixed,ios::floatfield);
   std::cout.precision(3);

   double rdpEff = draw.GetEfficiency(rdp, momentum, signal, cut, 1, bins,
      &errors).at(0);
   double rdpErr = errors.at(0);
   double mcpEff = draw.GetEfficiency(mcp, momentum, signal, cut, 1, bins,
      &errors).at(0);
   double mcpErr = errors.at(0);
   //double systematic = GetSystematic(rdpEff, mcpEff, rdpErr, mcpErr);
   double systematic = GetSystematicUncertainty(rdpEff, mcpEff);
   double error = GetSystematicError(rdpErr, mcpErr);
   std::cout << detector << " & " << particle << " & " << rdpEff << " & " <<
      rdpErr << " & " << mcpEff << " & " << mcpErr << " & " <<
      (systematic * 100) << " & " << (error * 100) << "\\\\" << std::endl;   
}

void PrintLaTeXSummaryDataBinned(DrawingToolsTPCECal& draw, DataSample& rdp,
   DataSample& mcp, const std::string& variable, Bins& bins,
   const std::string& signal, const std::string& cut,
   const std::string& detector, const std::string& particle)
{
   const int n = bins.GetNumBins();
   std::vector<double> mcpErr(n);
   std::vector<double> rdpErr(n);
   
   std::cout.setf(ios::fixed, ios::floatfield);
   std::cout.precision(3);

   std::vector<double> rdpEff = draw.GetEfficiency(rdp, variable, signal, cut,
      n, bins.GetBoundaries(), &rdpErr);
   std::vector<double> mcpEff = draw.GetEfficiency(mcp, variable, signal, cut,
      n, bins.GetBoundaries(), &mcpErr);

   std::cout << particle << " : " << detector << std::endl;
   for(int i = 0; i < n; i++)
   {
      double systematic = GetSystematicUncertainty(rdpEff[i], mcpEff[i]);
      std::cout << bins[i] << " - " << bins[i + 1] << " & " << rdpEff[i] <<
         " & " << rdpErr[i] << " & " << mcpEff[i] << " & " << mcpErr[i] <<
         " & " << (systematic * 100) << "\\\\" << std::endl;
   }
}

int main(int argc, char *argv[])
{
   vecstr rdpFiles;
   vecstr mcpFiles;
   GetFilenames(rdpFiles, mcpFiles);

   gStyle->SetOptStat(0);

   BinsVector dsMomBins;
   BinsVector brMomBins;
   BinsVector dsAngBins;
   BinsVector brAngBins;
   CreateBins(dsMomBins, brMomBins, dsAngBins, brAngBins);

   // Cut variables
   std::string isBarrel = "entersBarrel==1";
   std::string isDownstream = "entersDownstream==1";
   std::string recoBr = "ecalDetector==23";
   std::string recoDS = "ecalDetector==6";

   // Independent variables
   std::string momentum = "momentum";
   std::string angle = "direction[2]";

   TCanvas* c1 = new TCanvas("c", "c");
   
   vecstr particle;
   particle.push_back("e");
   particle.push_back("mu");
   particle.push_back("p");
   particle.push_back("ebar");
   particle.push_back("mubar");
   for(unsigned int i = 0; i < rdpFiles.size(); ++i)
   {
      DataSample rdp = GetDataSample(rdpFiles[i]);
      DataSample mcp = GetDataSample(mcpFiles[i]);

      DrawingToolsTPCECal draw(mcpFiles[i]);

      draw.SetDifferentStackFillStyles();
      draw.ApplyRange(false);
      draw.DumpPOT(rdp);
      draw.DumpPOT(mcp);

      // Momentum selections
      DrawSelection(draw, c1, rdp, mcp, momentum, dsMomBins[i],
         isDownstream, "ds", particle[i], "mom", "Track Momentum (MeV)", "tr");
      DrawSelection(draw, c1, rdp, mcp, momentum, brMomBins[i],
         isBarrel, "br", particle[i], "mom", "Track Momentum (MeV)", "tr");

      // Track angle selections
      DrawSelection(draw, c1, rdp, mcp, angle, dsAngBins[i],
         isDownstream, "ds", particle[i], "ang", "cos(Track Angle)", "tl");
      DrawSelection(draw, c1, rdp, mcp, angle, brAngBins[i],
         isBarrel, "br", particle[i], "ang", "cos(Track Angle)", "tl");

      // Momentum effiencies
      DrawEfficiencies(draw, c1, rdp, mcp, momentum, dsMomBins[i],
         isDownstream, recoDS, "ds", particle[i], "mom", "Track Momentum (MeV)", "br");
      DrawEfficiencies(draw, c1, rdp, mcp, momentum, brMomBins[i],
         isBarrel, recoBr, "br", particle[i], "mom", "Track Momentum (MeV)", "tl");

      // Track angle effiencies
      DrawEfficiencies(draw, c1, rdp, mcp, angle, dsAngBins[i],
         isDownstream, recoDS, "ds", particle[i], "ang", "cos(Track Angle)", "br");
      DrawEfficiencies(draw, c1, rdp, mcp, angle, brAngBins[i],
         isBarrel, recoBr, "br", particle[i], "ang", "cos(Track Angle)", "tl");

      // Momentum systematics
      DrawSystematics(draw, c1, rdp, mcp, momentum, dsMomBins[i],
         isDownstream, recoDS, "ds", particle[i], "mom", "Track Momentum (MeV)");
      DrawSystematics(draw, c1, rdp, mcp, momentum, brMomBins[i],
         isBarrel, recoBr, "br", particle[i], "mom", "Track Momentum (MeV)");

      // Track angle systematics
      DrawSystematics(draw, c1, rdp, mcp, angle, dsAngBins[i],
         isDownstream, recoDS, "ds", particle[i], "ang", "cos(Track Angle)");
      DrawSystematics(draw, c1, rdp, mcp, angle, brAngBins[i],
         isBarrel, recoBr, "br", particle[i], "ang", "cos(Track Angle)");
   }

   for(unsigned int i = 0; i < 2; ++i)
   {
      DataSample nuRdp = GetDataSample(rdpFiles[i]);
      DataSample nubarRdp = GetDataSample(rdpFiles[i + 3]);
      DataSample nuMcp = GetDataSample(mcpFiles[i]);
      DataSample nubarMcp = GetDataSample(mcpFiles[i + 3]);

      DrawingToolsTPCECal draw(mcpFiles[i]);

      draw.SetDifferentStackFillStyles();
      draw.ApplyRange(false);

      // Momentum effiencies
      DrawCombinedEfficiencies(draw, c1, nuRdp, nubarRdp, nuMcp, nubarMcp,
         momentum, dsMomBins[i], isDownstream, recoDS, "ds",
         particle[i] + "like", "mom", "br");
      DrawCombinedEfficiencies(draw, c1, nuRdp, nubarRdp, nuMcp, nubarMcp,
         momentum, brMomBins[i], isBarrel, recoBr, "br",
         particle[i] + "like", "mom", "tl");

      // Track angle effiencies
      DrawCombinedEfficiencies(draw, c1, nuRdp, nubarRdp, nuMcp, nubarMcp,
         angle, dsAngBins[i], isDownstream, recoDS, "ds", particle[i] + "like",
         "ang", "br");
      DrawCombinedEfficiencies(draw, c1, nuRdp, nubarRdp, nuMcp, nubarMcp,
         angle, brAngBins[i], isBarrel, recoBr, "br", particle[i] + "like",
         "ang", "tl");

      // Momentum systematics
      DrawCombinedSystematics(draw, c1, nuRdp, nubarRdp, nuMcp, nubarMcp,
         momentum, dsMomBins[i], isDownstream, recoDS, "ds",
         particle[i] + "like", "mom");
      DrawCombinedSystematics(draw, c1, nuRdp, nubarRdp, nuMcp, nubarMcp,
         momentum, brMomBins[i], isBarrel, recoBr, "br", particle[i] + "like",
         "mom");

      // Track angle systematics
      DrawCombinedSystematics(draw, c1, nuRdp, nubarRdp, nuMcp, nubarMcp,
         angle, dsAngBins[i], isDownstream, recoDS, "ds", particle[i] + "like",
         "ang");
      DrawCombinedSystematics(draw, c1, nuRdp, nubarRdp, nuMcp, nubarMcp,
         angle, brAngBins[i], isBarrel, recoBr, "br", particle[i] + "like",
         "ang");

      // Momentum systematics - dual plot
//      DrawDualSystematics(draw, c1, nuRdp, nubarRdp, nuMcp, nubarMcp,
//         momentum, dsMomBins[i], isDownstream, recoDS, "ds", particle[i], "mom");
//      DrawDualSystematics(draw, c1, nuRdp, nubarRdp, nuMcp, nubarMcp,
//         momentum, brMomBins[i], isBarrel, recoBr, "br", particle[i], "mom");

      // Track angle systematics - dual plot
//      DrawDualSystematics(draw, c1, nuRdp, nubarRdp, nuMcp, nubarMcp,
//         angle, dsAngBins[i], isDownstream, recoDS, "ds", particle[i], "ang");
//      DrawDualSystematics(draw, c1, nuRdp, nubarRdp, nuMcp, nubarMcp,
//         angle, brAngBins[i], isBarrel, recoBr, "br", particle[i], "ang");
   }

   // Print unbinned summary
   for(unsigned int i = 0; i < rdpFiles.size(); ++i)
   {
      DataSample rdp = GetDataSample(rdpFiles[i]);
      DataSample mcp = GetDataSample(mcpFiles[i]);

      DrawingToolsTPCECal draw(mcpFiles[i]);

      PrintSummaryDataUnbinned(draw, rdp, mcp, momentum, isDownstream, recoDS,
         "Downstream", particle[i]);
      PrintSummaryDataUnbinned(draw, rdp, mcp, momentum, isBarrel, recoBr,
         "Barrel", particle[i]);
   }

   // Print binned summary
   for(unsigned int i = 0; i < rdpFiles.size(); ++i)
   {
      DataSample rdp = GetDataSample(rdpFiles[i]);
      DataSample mcp = GetDataSample(mcpFiles[i]);

      DrawingToolsTPCECal draw(mcpFiles[i]);

      std::cout << "***** " << particle[i] << " *****" << std::endl;
      std::cout << "*** Downstream Mom ***" << std::endl;
      PrintSummaryDataBinned(draw, rdp, mcp, momentum, dsMomBins[i],
         isDownstream, recoDS, "Downstream", particle[i]);
      std::cout << "*** Barrel Mom ***" << std::endl;
      PrintSummaryDataBinned(draw, rdp, mcp, momentum, brMomBins[i],
         isBarrel, recoBr, "Barrel", particle[i]);
      std::cout << "*** Downstream Ang ***" << std::endl;
      PrintSummaryDataBinned(draw, rdp, mcp, angle, dsAngBins[i],
         isDownstream, recoDS, "Downstream", particle[i]);
      std::cout << "*** Barrel Ang ***" << std::endl;
      PrintSummaryDataBinned(draw, rdp, mcp, angle, brAngBins[i],
         isBarrel, recoBr, "Barrel", particle[i]);
   }

   // Print LaTeX-friendly unbinned summary
   for(unsigned int i = 0; i < rdpFiles.size(); ++i)
   {
      DataSample rdp = GetDataSample(rdpFiles[i]);
      DataSample mcp = GetDataSample(mcpFiles[i]);

      DrawingToolsTPCECal draw(mcpFiles[i]);

      std::cout << "***** " << particle[i] << " *****" << std::endl;
      std::cout << "*** Downstream ***" << std::endl;
      PrintLaTeXSummaryDataUnbinned(draw, rdp, mcp, momentum, isDownstream, recoDS,
         "Downstream", particle[i]);
      std::cout << "*** Barrel ***" << std::endl;
      PrintLaTeXSummaryDataUnbinned(draw, rdp, mcp, momentum, isBarrel, recoBr,
         "Barrel", particle[i]);

      std::cout << "***** " << particle[i] << " *****" << std::endl;
      std::cout << "*** Downstream Mom ***" << std::endl;
      PrintLaTeXSummaryDataBinned(draw, rdp, mcp, momentum, dsMomBins[i],
         isDownstream, recoDS, "Downstream", particle[i]);
      std::cout << "*** Barrel Mom ***" << std::endl;
      PrintLaTeXSummaryDataBinned(draw, rdp, mcp, momentum, brMomBins[i],
         isBarrel, recoBr, "Barrel", particle[i]);
      std::cout << "*** Downstream Ang ***" << std::endl;
      PrintLaTeXSummaryDataBinned(draw, rdp, mcp, angle, dsAngBins[i],
         isDownstream, recoDS, "Downstream", particle[i]);
      std::cout << "*** Barrel Ang ***" << std::endl;
      PrintLaTeXSummaryDataBinned(draw, rdp, mcp, angle, brAngBins[i],
         isBarrel, recoBr, "Barrel", particle[i]);
   }

   // Draw purities
/*   vecstr trueParticle;
   trueParticle.push_back("11");
   trueParticle.push_back("13");
   trueParticle.push_back("2212");
   trueParticle.push_back("-11");
   trueParticle.push_back("-13");
   for(unsigned int i = 0; i < rdpFiles.size(); ++i)
   {
      DataSample mcp = GetDataSample(mcpFiles[i]);

      DrawingToolsTPCECal draw(mcpFiles[i]);

      DrawPurity(draw, c1, mcp, "ds", particle[i], 
         trueParticle[i], i);
      DrawPurity(draw, c1, mcp, "br", particle[i], 
         trueParticle[i], i);
   }*/

   delete c1;
}
