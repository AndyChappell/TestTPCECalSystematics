#include "DrawingToolsTPCECal.hxx"

typedef std::vector<std::string> vecstr;

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

void DrawMomentumSelection(DrawingToolsTPCECal& draw, TCanvas* c1,
   DataSample& rdp, DataSample& mcp, std::string& momentum, int n, double* bins,
   const std::string& signal, const std::string& detector,
   const std::string& particle)
{
   draw.SetLegendSize(0.2, 0.4);
   draw.SetLegendPos("tr");
   draw.SetTitleX("Track Momentum (MeV)");
   draw.SetTitleY("Counts/Bin");
   std::ostringstream ss;

   draw.Draw(rdp, mcp, momentum, n, bins, "particle", signal);    
   ss << "sel_mom_" << detector << "_" << particle << ".png";
   c1->Print(ss.str().c_str(), "png");
}

void DrawTrackAngleSelection(DrawingToolsTPCECal& draw, TCanvas* c1,
   DataSample& rdp, DataSample& mcp, std::string& angle, int n, double* bins,
   const std::string& signal, const std::string& detector,
   const std::string& particle)
{
   draw.SetLegendSize(0.2, 0.4);
   draw.SetLegendPos("tl");
   draw.SetTitleX("cos(Track Angle)");
   draw.SetTitleY("Counts/Bin");
   std::ostringstream ss;

   draw.Draw(rdp, mcp, angle, n, bins, "particle", signal);    
   ss << "sel_ang_" << detector << "_" << particle << ".png";
   c1->Print(ss.str().c_str(), "png");
}

void DrawMomentumEfficiencies(DrawingToolsTPCECal& draw, TCanvas* c1,
   DataSample& rdp, DataSample& mcp, std::string& momentum, const int n, double* bins,
   const std::string& signal, const std::string& cut,
   const std::string& detector, const std::string& particle)
{
   c1->Clear();
   draw.SetLegendSize(0.15, 0.1);
   draw.SetLegendPos("br");
   draw.SetTitleY("Matching Efficiency");
   draw.SetTitleX("Track Momentum (MeV)");
   std::ostringstream ss;
 
   std::vector<double> lerrs(n);
   std::vector<double> herrs(n);
   std::vector<double> effs = draw.GetEfficiency(rdp, momentum, signal, cut, n,
      bins, &lerrs, &herrs); 

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
   
   effs = draw.GetEfficiency(mcp, momentum, signal, cut, n, bins, &lerrs, &herrs); 

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

   ss << "eff_mom_" << detector << "_" << particle << ".png";
   c1->Print(ss.str().c_str(), "png");
   c1->Clear();
}

void DrawTrackAngleEfficiencies(DrawingToolsTPCECal& draw, TCanvas* c1,
   DataSample& rdp, DataSample& mcp, std::string& angle, const int n, double* bins,
   const std::string& signal, const std::string& cut,
   const std::string& detector, const std::string& particle)
{
   c1->Clear();
   draw.SetLegendSize(0.15, 0.1);
   draw.SetLegendPos("br");
   draw.SetTitleY("Matching Efficiency");
   draw.SetTitleX("cos(Track Angle)");
   std::ostringstream ss;

   std::vector<double> lerrs(n);
   std::vector<double> herrs(n);
   std::vector<double> effs = draw.GetEfficiency(rdp, angle, signal, cut, n,
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
      if((y[i] + yherrs[i]) > 1)
      {
         yherrs[i] = 1 - y[i];
      }
      if((y[i] - ylerrs[i]) < 0)
      {
         ylerrs[i] = y[i];
      }
   }
   TGraphAsymmErrors rdpGraph(n, x, y, xlerrs, xherrs, ylerrs, yherrs);
   
   effs = draw.GetEfficiency(mcp, angle, signal, cut, n, bins, &lerrs, &herrs); 

   for(int i = 0; i < n; i++)
   {
      x[i] = (bins[i] + bins[i + 1]) / 2.0;
      y[i] = effs[i];
      xlerrs[i] = x[i] - bins[i];
      xherrs[i] = xlerrs[i];
      ylerrs[i] = lerrs[i];
      yherrs[i] = herrs[i];
      if((y[i] + yherrs[i]) > 1)
      {
         yherrs[i] = 1 - y[i];
      }
      if((y[i] - ylerrs[i]) < 0)
      {
         ylerrs[i] = y[i];
      }
   }
   TGraphAsymmErrors mcpGraph(n, x, y, xlerrs, xherrs, ylerrs, yherrs);

   vecstr legend;
   legend.push_back((particle.find("bar") == std::string::npos) ? "#nu Data" : "#bar{#nu} Data");
   legend.push_back((particle.find("bar") == std::string::npos) ? "#nu MC" : "#bar{#nu} MC");
   TMultiGraph graph;
   draw.Plot(graph, rdpGraph, mcpGraph, "AP", legend);
   gPad->Update();

   ss << "eff_ang_" << detector << "_" << particle << ".png";
   c1->Print(ss.str().c_str(), "png");
   c1->Clear();
}

void DrawMomentumSystematics(DrawingToolsTPCECal& draw, TCanvas* c1,
   DataSample& rdp, DataSample& mcp, std::string& momentum, int n, double* bins,
   const std::string& signal, const std::string& cut,
   const std::string& detector, const std::string& particle)
{
   draw.SetLegendSize(0.1, 0.05);
   draw.SetLegendPos("br");
   draw.SetTitleX("Track Momentum (MeV)");
   draw.SetTitleY("Systematic Uncertainty");
   std::ostringstream ss;

   TH1F histogram("", "", n, bins);
   draw.PlotSystematic(rdp, mcp, momentum, signal, cut, n, bins, histogram,
      "e1", (particle.find("bar") == std::string::npos) ? "#nu" : "#bar{#nu}");
   ss << "syst_mom_" << detector << "_" << particle << ".png";
   c1->Print(ss.str().c_str(), "png");
}

void DrawTrackAngleSystematics(DrawingToolsTPCECal& draw, TCanvas* c1,
   DataSample& rdp, DataSample& mcp, std::string& angle, int n, double* bins,
   const std::string& signal, const std::string& cut,
   const std::string& detector, const std::string& particle)
{
   draw.SetLegendSize(0.1, 0.05);
   draw.SetLegendPos("br");
   draw.SetTitleX("cos(Track Angle)");
   draw.SetTitleY("Systematic Uncertainty");
   std::ostringstream ss;

   TH1F histogram("", "", n, bins);
   draw.PlotSystematic(rdp, mcp, angle, signal, cut, n, bins, histogram,
      "e1", (particle.find("bar") == std::string::npos) ? "#nu" : "#bar{#nu}");
   ss << "syst_ang_" << detector << "_" << particle << ".png";
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

   double systematic = GetSystematic(rdpEff, mcpEff, rdpErr, mcpErr);
   std::cout << "systematic = " << systematic << std::endl << std::endl;
}

void PrintSummaryDataBinned(DrawingToolsTPCECal& draw, DataSample& rdp,
   DataSample& mcp, const std::string& variable, int n, double* bins,
   const std::string& signal, const std::string& cut,
   const std::string& detector, const std::string& particle)
{
   std::vector<double> mcpErr(n);
   std::vector<double> rdpErr(n);
   
   std::cout.setf(ios::fixed, ios::floatfield);
   std::cout.precision(3);

   std::vector<double> rdpEff = draw.GetEfficiency(rdp, variable, signal, cut,
      n, bins, &rdpErr);
   std::vector<double> mcpEff = draw.GetEfficiency(mcp, variable, signal, cut,
      n, bins, &rdpErr);

   std::cout << particle << " : " << detector << std::endl;
   for(int i = 0; i < n; i++)
   {
      std::cout << "For bin: " << bins[i] << " - " << bins[i + 1] << std::endl;
      std::cout << "rdp eff    = " << rdpEff[i] << " +/- " << rdpErr[i] <<
         std::endl;
      std::cout << "mcp eff    = " << mcpEff[i] << " +/- " << mcpErr[i] <<
         std::endl;

      double systematic = GetSystematic(rdpEff[i], mcpEff[i], rdpErr[i],
         mcpErr[i]);
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
   double systematic = GetSystematic(rdpEff, mcpEff, rdpErr, mcpErr);
   std::cout << detector << " & " << particle << " & " << rdpEff << " & " <<
      rdpErr << " & " << mcpEff << " & " << mcpErr << " & " << systematic <<
      "\\\\" << std::endl;   
}

int main(int argc, char *argv[])
{
   vecstr rdpFiles;
   vecstr mcpFiles;
   GetFilenames(rdpFiles, mcpFiles);

   gStyle->SetOptStat(0);

   // DS ECal binning
   int nds_mom = 6;
   double ds_bins_mom[7] = {0, 400, 800, 1200, 2000, 3600, 5000};
   int nds_ang = 5;
   double ds_bins_ang[6] = {0.75, 0.8, 0.85, 0.925, 0.975, 1.0};

   // Barrel ECal binning
   int nbr_mom = 5;
   double br_bins_mom[6] = {0, 400, 800, 2000, 3600, 5000};
   int nbr_ang = 4;
   double br_bins_ang[5] = {-0.3, 0.1, 0.4, 0.7, 0.825};

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
      DrawMomentumSelection(draw, c1, rdp, mcp, momentum, nds_mom, ds_bins_mom,
         isDownstream, "ds", particle[i]);
      DrawMomentumSelection(draw, c1, rdp, mcp, momentum, nbr_mom, br_bins_mom,
         isBarrel, "br", particle[i]);

      // Track angle selections
      DrawTrackAngleSelection(draw, c1, rdp, mcp, angle, nds_ang, ds_bins_ang,
         isDownstream, "ds", particle[i]);
      DrawTrackAngleSelection(draw, c1, rdp, mcp, angle, nbr_ang, br_bins_ang,
         isBarrel, "br", particle[i]);

      // Momentum effiencies
      DrawMomentumEfficiencies(draw, c1, rdp, mcp, momentum, nds_mom, ds_bins_mom,
         isDownstream, recoDS, "ds", particle[i]);
      DrawMomentumEfficiencies(draw, c1, rdp, mcp, momentum, nbr_mom, br_bins_mom,
         isBarrel, recoBr, "br", particle[i]);

      // Track angle effiencies
      DrawTrackAngleEfficiencies(draw, c1, rdp, mcp, angle, nds_ang, ds_bins_ang,
         isDownstream, recoDS, "ds", particle[i]);
      DrawTrackAngleEfficiencies(draw, c1, rdp, mcp, angle, nbr_ang, br_bins_ang,
         isBarrel, recoBr, "br", particle[i]);

      // Momentum systematics
      DrawMomentumSystematics(draw, c1, rdp, mcp, momentum, nds_mom, ds_bins_mom,
         isDownstream, recoDS, "ds", particle[i]);
      DrawMomentumSystematics(draw, c1, rdp, mcp, momentum, nbr_mom, br_bins_mom,
         isBarrel, recoBr, "br", particle[i]);

      // Track angle systematics
      DrawTrackAngleSystematics(draw, c1, rdp, mcp, angle, nds_ang, ds_bins_ang,
         isDownstream, recoDS, "ds", particle[i]);
      DrawTrackAngleSystematics(draw, c1, rdp, mcp, angle, nbr_ang, br_bins_ang,
         isBarrel, recoBr, "br", particle[i]);
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

      PrintSummaryDataBinned(draw, rdp, mcp, momentum, nds_mom, ds_bins_mom,
         isDownstream, recoDS, "Downstream", particle[i]);
      PrintSummaryDataBinned(draw, rdp, mcp, momentum, nbr_mom, br_bins_mom,
         isBarrel, recoBr, "Barrel", particle[i]);
      PrintSummaryDataBinned(draw, rdp, mcp, angle, nds_ang, ds_bins_ang,
         isDownstream, recoDS, "Downstream", particle[i]);
      PrintSummaryDataBinned(draw, rdp, mcp, angle, nbr_ang, br_bins_ang,
         isBarrel, recoBr, "Barrel", particle[i]);
   }

   // Print LaTeX-friendly unbinned summary
   for(unsigned int i = 0; i < rdpFiles.size(); ++i)
   {
      DataSample rdp = GetDataSample(rdpFiles[i]);
      DataSample mcp = GetDataSample(mcpFiles[i]);

      DrawingToolsTPCECal draw(mcpFiles[i]);

      PrintLaTeXSummaryDataUnbinned(draw, rdp, mcp, momentum, isDownstream, recoDS,
         "Downstream", particle[i]);
      PrintLaTeXSummaryDataUnbinned(draw, rdp, mcp, momentum, isBarrel, recoBr,
         "Barrel", particle[i]);
   }

   // Draw purities
   vecstr trueParticle;
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
   }

   delete c1;
}
