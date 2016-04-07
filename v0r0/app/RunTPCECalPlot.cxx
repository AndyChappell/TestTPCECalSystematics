#include "DrawingToolsTPCECal.hxx"
#include "Bins.hxx"
#include "Detector.hxx"
#include "Particle.hxx"
#include "AnalysisVariable.hxx"

using TPCECalSystematics::Bins;
using TPCECalSystematics::Detector;
using TPCECalSystematics::Particle;
using TPCECalSystematics::AnalysisVariable;

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
   DataSample& rdp, DataSample& mcp, const AnalysisVariable& variable, Bins& bins,
   const Detector& detector, const Particle& particle)
{
   draw.SetLegendSize(0.12, 0.35);
   draw.SetLegendPos("tr");
   draw.SetTitleX(variable.GetDescription());
   draw.SetTitleY("Counts/Bin");
   std::ostringstream ss;

   int n = bins.GetNumBins();
   double* boundaries = bins.GetBoundaries();
   draw.Draw(rdp, mcp, variable.GetMicrotreeVariable(), n, boundaries,
      "particle", detector.GetSignal());
   ss << "sel_" << variable.GetName() << "_" << detector.GetName() << "_" <<
      particle.GetName() << ".png";
   c1->Print(ss.str().c_str(), "png");
}

void DrawEfficiencies(DrawingToolsTPCECal& draw, TCanvas* c1,
   DataSample& rdp, DataSample& mcp, const AnalysisVariable& variable, Bins& bins,
   const Detector& detector, const Particle& particle)
{
   c1->Clear();
   draw.SetLegendSize(0.12, 0.1);
   draw.SetLegendPos("tr");
   draw.SetTitleY("Matching Efficiency");
   draw.SetTitleX(variable.GetDescription());
   std::ostringstream ss;
 
   draw.PlotEfficiency(rdp, mcp, variable.GetMicrotreeVariable(),
      detector.GetSignal(), detector.GetCut(), bins.GetNumBins(),
      bins.GetBoundaries(), particle.GetName().find("bar") != std::string::npos);

   ss << "eff_" << variable.GetName() << "_" << detector.GetName() << "_" <<
      particle.GetName() << ".png";
   c1->Print(ss.str().c_str(), "png");
   c1->Clear();
}

void DrawCombinedEfficiencies(DrawingToolsTPCECal& draw, TCanvas* c1,
   DataSample& nuRdp, DataSample& nubarRdp, DataSample& nuMcp,
   DataSample& nubarMcp, const AnalysisVariable& variable, Bins& bins,
   const Detector& detector, const Particle& particle)
{
   c1->Clear();
   draw.SetLegendSize(0.12, 0.1);
   draw.SetLegendPos("tr");
   draw.SetTitleY("Matching Efficiency");
   draw.SetTitleX(variable.GetDescription());
   std::ostringstream ss;

   draw.PlotEfficiency(nuRdp, nubarRdp, nuMcp, nubarMcp,
      variable.GetMicrotreeVariable(), detector.GetSignal(), detector.GetCut(),
      bins.GetNumBins(), bins.GetBoundaries());
 
   ss << "eff_" << variable.GetName() << "_" << detector.GetName() << "_" <<
      particle.GetName() << "like" << ".png";
   c1->Print(ss.str().c_str(), "png");
   c1->Clear();
}

void DrawSystematics(DrawingToolsTPCECal& draw, TCanvas* c1,
   DataSample& rdp, DataSample& mcp, const AnalysisVariable& variable, Bins& bins,
   const Detector& detector, const Particle& particle)
{
   draw.SetLegendSize(0.12, 0.06);
   draw.SetLegendPos("tr");
   draw.SetTitleX(variable.GetDescription());
   draw.SetTitleY("Systematic Uncertainty");
   std::ostringstream ss;
   
   draw.PlotSystematic(rdp, mcp, variable.GetMicrotreeVariable(),
      detector.GetSignal(), detector.GetCut(), bins.GetNumBins(),
      bins.GetBoundaries(), "e1", particle.GetName().find("bar") == std::string::npos);
   ss << "syst_" << variable.GetName() << "_" << detector.GetName() << "_" <<
      particle.GetName() << ".png";
   c1->Print(ss.str().c_str(), "png");
}

void DrawCombinedSystematics(DrawingToolsTPCECal& draw, TCanvas* c1,
   DataSample& nuRdp, DataSample& nubarRdp, DataSample& nuMcp,
   DataSample& nubarMcp, const AnalysisVariable& variable, Bins& bins,
   const Detector& detector, const Particle& particle)
{
   draw.SetLegendSize(0.12, 0.05);
   draw.SetLegendPos("tr");
   draw.SetTitleX(variable.GetDescription());
   draw.SetTitleY("Systematic Uncertainty");
   std::ostringstream ss;

   draw.PlotSystematic(nuRdp, nubarRdp, nuMcp, nubarMcp,
      variable.GetMicrotreeVariable(), detector.GetSignal(), detector.GetCut(),
      bins.GetNumBins(), bins.GetBoundaries(), "e1");
   ss << "syst_" << variable.GetName() << "_" << detector.GetName() << "_" <<
      particle.GetName() << "like" << ".png";
   c1->Print(ss.str().c_str(), "png");
}

void DrawPurity(DrawingToolsTPCECal& draw, TCanvas* c1, DataSample& mcp,
   const Detector& detector, const Particle& particle, const int selection)
{
   draw.SetLegendSize(0.12, 0.1);
   draw.SetLegendPos("tr");
   draw.SetTitleX("Cut");
   draw.SetTitleY("Purity/Efficiency");
   std::ostringstream ss;

   ss << "particle==" << particle.GetPDG();
   draw.DrawEffPurVSCut(mcp, selection, (detector.GetName() == "ds") ? 0 : 1,
      ss.str(), "");
   ss.str(""); ss.clear();
   ss << "pur_" << detector.GetName() << "_" << particle.GetName() << ".png";
   c1->Print(ss.str().c_str(), "png");
}

void PrintSummaryDataUnbinned(DrawingToolsTPCECal& draw, DataSample& rdp,
   DataSample& mcp, const AnalysisVariable& variable, const Detector& detector,
   const Particle& particle)
{
   double bins[2] = {0, 100000};
   std::vector<double> errors(1);

   std::cout.setf(ios::fixed,ios::floatfield);
   std::cout.precision(3);

   double rdpEff = draw.GetEfficiency(rdp, variable.GetMicrotreeVariable(),
      detector.GetSignal(), detector.GetCut(), 1, bins, &errors).at(0);
   double rdpErr = errors.at(0);
   double mcpEff = draw.GetEfficiency(mcp, variable.GetMicrotreeVariable(),
      detector.GetSignal(), detector.GetCut(), 1, bins, &errors).at(0);
   double mcpErr = errors.at(0);
   std::cout << particle.GetName() << " : " << detector.GetDescription() <<
      std::endl;
   std::cout << "rdp eff    = " << rdpEff << " +/- " << rdpErr << std::endl;
   std::cout << "mcp eff    = " << mcpEff << " +/- " << mcpErr << std::endl;

   double systematic = GetSystematicUncertainty(rdpEff, mcpEff);
   double error = GetSystematicError(rdpErr, mcpErr);
   std::cout << "systematic = " << systematic << " +/- " << error <<
      std::endl << std::endl;
}

void PrintSummaryDataBinned(DrawingToolsTPCECal& draw, DataSample& rdp,
   DataSample& mcp, const AnalysisVariable& variable, Bins& bins,
   const Detector& detector, const Particle& particle)
{
   const int n = bins.GetNumBins();
   std::vector<double> mcpErr(n);
   std::vector<double> rdpErr(n);
   
   std::cout.setf(ios::fixed, ios::floatfield);
   std::cout.precision(3);

   std::vector<double> rdpEff = draw.GetEfficiency(rdp,
      variable.GetMicrotreeVariable(), detector.GetSignal(), detector.GetCut(),
      n, bins.GetBoundaries(), &rdpErr);
   std::vector<double> mcpEff = draw.GetEfficiency(mcp,
      variable.GetMicrotreeVariable(), detector.GetSignal(), detector.GetCut(),
      n, bins.GetBoundaries(), &mcpErr);

   std::cout << particle.GetName() << " : " << detector.GetDescription() <<
      " " << variable.GetDescription() << std::endl;
   for(int i = 0; i < n; i++)
   {
      std::cout << "For bin: " << bins[i] << " - " << bins[i + 1] << std::endl;
      std::cout << "rdp eff    = " << rdpEff[i] << " +/- " << rdpErr[i] <<
         std::endl;
      std::cout << "mcp eff    = " << mcpEff[i] << " +/- " << mcpErr[i] <<
         std::endl;

      double systematic = GetSystematicUncertainty(rdpEff[i], mcpEff[i]);
      std::cout << "systematic = " << systematic << std::endl << std::endl;
   }
}

void PrintLaTeXSummaryDataUnbinned(DrawingToolsTPCECal& draw, DataSample& rdp,
   DataSample& mcp, const AnalysisVariable& variable, const Detector& detector,
   const Particle& particle)
{
   double bins[2] = {0, 100000};
   std::vector<double> errors(1);

   std::cout.setf(ios::fixed,ios::floatfield);
   std::cout.precision(3);

   double rdpEff = draw.GetEfficiency(rdp, variable.GetMicrotreeVariable(),
      detector.GetSignal(), detector.GetCut(), 1, bins, &errors).at(0);
   double rdpErr = errors.at(0);
   double mcpEff = draw.GetEfficiency(mcp, variable.GetMicrotreeVariable(),
      detector.GetSignal(), detector.GetCut(), 1, bins, &errors).at(0);
   double mcpErr = errors.at(0);
   double systematic = 100 * GetSystematicUncertainty(rdpEff, mcpEff);
   double error = 100 * GetSystematicError(rdpErr, mcpErr);
   std::cout << detector.GetDescription() << " & " << particle.GetName() <<
      " & " << rdpEff << " & " << rdpErr << " & " << mcpEff << " & " <<
      mcpErr << " & " << systematic << " & " << error << "\\\\" << std::endl;   
}

void PrintLaTeXSummaryDataBinned(DrawingToolsTPCECal& draw, DataSample& rdp,
   DataSample& mcp, const AnalysisVariable& variable, Bins& bins,
   const Detector& detector, const Particle& particle)
{
   const int n = bins.GetNumBins();
   std::vector<double> mcpErr(n);
   std::vector<double> rdpErr(n);
   
   std::cout.setf(ios::fixed, ios::floatfield);
   std::cout.precision(3);

   std::vector<double> rdpEff = draw.GetEfficiency(rdp,
      variable.GetMicrotreeVariable(), detector.GetSignal(), detector.GetCut(),
      n, bins.GetBoundaries(), &rdpErr);
   std::vector<double> mcpEff = draw.GetEfficiency(mcp,
      variable.GetMicrotreeVariable(), detector.GetSignal(), detector.GetCut(),
      n, bins.GetBoundaries(), &mcpErr);

   std::cout << particle.GetName() << " : " << detector.GetDescription() <<
      " - " << variable.GetDescription() << std::endl;
   for(int i = 0; i < n; i++)
   {
      double systematic = 100 * GetSystematicUncertainty(rdpEff[i], mcpEff[i]);
      double error = 100 * GetSystematicError(rdpErr[i], mcpErr[i]);
      std::cout << bins[i] << " - " << bins[i + 1] << " & " << rdpEff[i] <<
         " & " << rdpErr[i] << " & " << mcpEff[i] << " & " << mcpErr[i] <<
         " & " << systematic << " & " << error << "\\\\" << std::endl;
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

   // Detector signal and cut details
   Detector barrel("br", "Barrel", "entersBarrel==1", "ecalDetector==23");
   Detector downstream("ds", "Downstream", "entersDownstream==1", "ecalDetector==6");

   // Analysis variables
   AnalysisVariable momentum("momentum", "mom", "Track Momentum (MeV)");
   AnalysisVariable angle("direction[2]", "ang", "cos(Track Angle)");

   TCanvas* c1 = new TCanvas("c", "c");

   gPad->SetLeftMargin(0.12);
   gPad->SetBottomMargin(0.12);
   gPad->SetRightMargin(0.18);
   
   //vecstr particle;
   std::vector<const Particle*> particle;
   const Particle* e = new Particle("e", 11);
   const Particle* mu = new Particle("mu", 13);
   const Particle* p = new Particle("p", 2212);
   const Particle* ebar = new Particle("ebar", -11);
   const Particle* mubar = new Particle("mubar", -13);
   particle.push_back(e);
   particle.push_back(mu);
   particle.push_back(p);
   particle.push_back(ebar);
   particle.push_back(mubar);
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
      DrawSelection(draw, c1, rdp, mcp, momentum, dsMomBins[i], downstream,
         *(particle[i]));
      DrawSelection(draw, c1, rdp, mcp, momentum, brMomBins[i], barrel,
         *(particle[i]));

      // Track angle selections
      DrawSelection(draw, c1, rdp, mcp, angle, dsAngBins[i], downstream,
         *(particle[i]));
      DrawSelection(draw, c1, rdp, mcp, angle, brAngBins[i], barrel,
         *(particle[i]));

      // Momentum effiencies
      DrawEfficiencies(draw, c1, rdp, mcp, momentum, dsMomBins[i], downstream,
         *(particle[i]));
      DrawEfficiencies(draw, c1, rdp, mcp, momentum, brMomBins[i], barrel,
         *(particle[i]));

      // Track angle effiencies
      DrawEfficiencies(draw, c1, rdp, mcp, angle, dsAngBins[i], downstream,
         *(particle[i]));
      DrawEfficiencies(draw, c1, rdp, mcp, angle, brAngBins[i], barrel,
         *(particle[i]));

      // Momentum systematics
      DrawSystematics(draw, c1, rdp, mcp, momentum, dsMomBins[i], downstream,
         *(particle[i]));
      DrawSystematics(draw, c1, rdp, mcp, momentum, brMomBins[i], barrel,
         *(particle[i]));

      // Track angle systematics
      DrawSystematics(draw, c1, rdp, mcp, angle, dsAngBins[i], downstream,
         *(particle[i]));
      DrawSystematics(draw, c1, rdp, mcp, angle, brAngBins[i], barrel,
         *(particle[i]));
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
         momentum, dsMomBins[i], downstream, *(particle[i]));
      DrawCombinedEfficiencies(draw, c1, nuRdp, nubarRdp, nuMcp, nubarMcp,
         momentum, brMomBins[i], barrel, *(particle[i]));

      // Track angle effiencies
      DrawCombinedEfficiencies(draw, c1, nuRdp, nubarRdp, nuMcp, nubarMcp,
         angle, dsAngBins[i], downstream, *(particle[i]));
      DrawCombinedEfficiencies(draw, c1, nuRdp, nubarRdp, nuMcp, nubarMcp,
         angle, brAngBins[i], barrel, *(particle[i]));

      // Momentum systematics
      DrawCombinedSystematics(draw, c1, nuRdp, nubarRdp, nuMcp, nubarMcp,
         momentum, dsMomBins[i], downstream, *(particle[i]));
      DrawCombinedSystematics(draw, c1, nuRdp, nubarRdp, nuMcp, nubarMcp,
         momentum, brMomBins[i], barrel, *(particle[i]));

      // Track angle systematics
      DrawCombinedSystematics(draw, c1, nuRdp, nubarRdp, nuMcp, nubarMcp,
         angle, dsAngBins[i], downstream, *(particle[i]));
      DrawCombinedSystematics(draw, c1, nuRdp, nubarRdp, nuMcp, nubarMcp,
         angle, brAngBins[i], barrel, *(particle[i]));
   }

   // Print unbinned summary
   for(unsigned int i = 0; i < rdpFiles.size(); ++i)
   {
      DataSample rdp = GetDataSample(rdpFiles[i]);
      DataSample mcp = GetDataSample(mcpFiles[i]);

      DrawingToolsTPCECal draw(mcpFiles[i]);

      PrintSummaryDataUnbinned(draw, rdp, mcp, momentum, downstream,
         *(particle[i]));
      PrintSummaryDataUnbinned(draw, rdp, mcp, momentum, barrel,
         *(particle[i]));
   }

   // Print binned summary
   for(unsigned int i = 0; i < rdpFiles.size(); ++i)
   {
      DataSample rdp = GetDataSample(rdpFiles[i]);
      DataSample mcp = GetDataSample(mcpFiles[i]);

      DrawingToolsTPCECal draw(mcpFiles[i]);

      PrintSummaryDataBinned(draw, rdp, mcp, momentum, dsMomBins[i],
         downstream, *(particle[i]));
      PrintSummaryDataBinned(draw, rdp, mcp, momentum, brMomBins[i],
         barrel, *(particle[i]));
      PrintSummaryDataBinned(draw, rdp, mcp, angle, dsAngBins[i],
         downstream, *(particle[i]));
      PrintSummaryDataBinned(draw, rdp, mcp, angle, brAngBins[i],
         barrel, *(particle[i]));
   }

   // Print LaTeX-friendly unbinned summary
   for(unsigned int i = 0; i < rdpFiles.size(); ++i)
   {
      DataSample rdp = GetDataSample(rdpFiles[i]);
      DataSample mcp = GetDataSample(mcpFiles[i]);

      DrawingToolsTPCECal draw(mcpFiles[i]);

      PrintLaTeXSummaryDataUnbinned(draw, rdp, mcp, momentum, downstream,
         *(particle[i]));
      PrintLaTeXSummaryDataUnbinned(draw, rdp, mcp, momentum, barrel,
         *(particle[i]));

      PrintLaTeXSummaryDataBinned(draw, rdp, mcp, momentum, dsMomBins[i],
         downstream, *(particle[i]));
      PrintLaTeXSummaryDataBinned(draw, rdp, mcp, momentum, brMomBins[i],
         barrel, *(particle[i]));
      PrintLaTeXSummaryDataBinned(draw, rdp, mcp, angle, dsAngBins[i],
         downstream, *(particle[i]));
      PrintLaTeXSummaryDataBinned(draw, rdp, mcp, angle, brAngBins[i],
         barrel, *(particle[i]));
   }

   // Draw purities
   for(unsigned int i = 0; i < rdpFiles.size(); ++i)
   {
      DataSample mcp = GetDataSample(mcpFiles[i]);

      DrawingToolsTPCECal draw(mcpFiles[i]);

      DrawPurity(draw, c1, mcp, downstream, *(particle[i]), i);
      DrawPurity(draw, c1, mcp, barrel, *(particle[i]), i);
   }

   delete c1;
   delete e;
   delete mu;
   delete p;
   delete ebar;
   delete mubar;
}
