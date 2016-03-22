#include "DrawingToolsTPCECal.hxx"

typedef std::vector<std::string> vecstr;

const vecstr GetEnvironmentVariables(bool mcp, bool nubarMode)
{
   vecstr envVars;
   if(nubarMode)
   {  // nubar mode data
      if(mcp)
      {  // MCP data
         envVars.push_back("EBAR_MCP_FILE");
         envVars.push_back("MUBAR_MCP_FILE");
      }
      else
      {  // RDP data
         envVars.push_back("EBAR_RDP_FILE");
         envVars.push_back("MUBAR_RDP_FILE");
      }
   }
   else
   {  // nu mode data
      if(mcp)
      {  // MCP data
         envVars.push_back("E_MCP_FILE");
         envVars.push_back("MU_MCP_FILE");
         envVars.push_back("P_MCP_FILE");
      }
      else
      {  // RDP data
         envVars.push_back("E_RDP_FILE");
         envVars.push_back("MU_RDP_FILE");
         envVars.push_back("P_RDP_FILE");
      }
   }
   
   return envVars;
}

void GetNuModeFilenames(vecstr& rdpFiles, vecstr& mcpFiles)
{
   vecstr nuRdpEnvVars = GetEnvironmentVariables(false, false);
   vecstr nuMcpEnvVars = GetEnvironmentVariables(true, false);
   
   for(unsigned int i = 0; i < nuRdpEnvVars.size(); ++i)
   {
      std::string filename = std::string(getenv(nuRdpEnvVars[i].c_str()));
      if(filename.length() == 0)
      {
         std::cerr << "Error: Environment variable" << nuRdpEnvVars[i] <<
            " not set. Exiting." << std::endl;
         exit(1);
      }
      rdpFiles.push_back(filename);

      filename = std::string(getenv(nuMcpEnvVars[i].c_str()));
      if(filename.length() == 0)
      {
         std::cerr << "Error: Environment variable" << nuMcpEnvVars[i] <<
            " not set. Exiting." << std::endl;
         exit(1);
      }
      mcpFiles.push_back(filename);
   }
}

void GetNubarModeFilenames(vecstr& rdpFiles, vecstr& mcpFiles)
{
   vecstr nubarRdpEnvVars = GetEnvironmentVariables(false, true);
   vecstr nubarMcpEnvVars = GetEnvironmentVariables(true, true);
   
   for(unsigned int i = 0; i < nubarRdpEnvVars.size(); ++i)
   {
      std::string filename = std::string(getenv(nubarRdpEnvVars[i].c_str()));
      if(filename.length() == 0)
      {
         std::cerr << "Error: Environment variable" << nubarRdpEnvVars[i] <<
            " not set. Exiting." << std::endl;
         exit(1);
      }
      rdpFiles.push_back(filename);

      filename = std::string(getenv(nubarMcpEnvVars[i].c_str()));
      if(filename.length() == 0)
      {
         std::cerr << "Error: Environment variable" << nubarMcpEnvVars[i] <<
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
   DataSample& rdp, DataSample& mcp, std::string& momentum, int n, double* bins,
   const std::string& signal, const std::string& cut,
   const std::string& detector, const std::string& particle)
{
   draw.SetLegendSize(0.15, 0.1);
   draw.SetLegendPos("br");
   draw.SetTitleY("Matching Efficiency");
   draw.SetTitleX("Track Momentum (MeV)");
   std::ostringstream ss;

   TH1F rdpHistogram("", "", n, bins);
   draw.PlotEfficiency(rdp, momentum, signal, cut, n, bins, rdpHistogram, "",
      "#nu Data");
   TH1F mcpHistogram("", "", n, bins);
   draw.PlotEfficiency(mcp, momentum, signal, cut, n, bins, mcpHistogram,
      "same", "#nu MC");         
   ss << "eff_mom_" << detector << "_" << particle << ".png";
   c1->Print(ss.str().c_str(), "png");
}

void DrawTrackAngleEfficiencies(DrawingToolsTPCECal& draw, TCanvas* c1,
   DataSample& rdp, DataSample& mcp, std::string& angle, int n, double* bins,
   const std::string& signal, const std::string& cut,
   const std::string& detector, const std::string& particle)
{
   draw.SetLegendSize(0.15, 0.1);
   draw.SetLegendPos("br");
   draw.SetTitleY("Matching Efficiency");
   draw.SetTitleX("cos(Track Angle)");
   std::ostringstream ss;

   TH1F rdpHistogram("", "", n, bins);
   draw.PlotEfficiency(rdp, angle, signal, cut, n, bins, rdpHistogram, "",
      "#nu Data");
   TH1F mcpHistogram("", "", n, bins);
   draw.PlotEfficiency(mcp, angle, signal, cut, n, bins, mcpHistogram, "same",
      "#nu MC");         
   ss << "eff_ang_" << detector << "_" << particle << ".png";
   c1->Print(ss.str().c_str(), "png");
}

void DrawMomentumSystematics(DrawingToolsTPCECal& draw, TCanvas* c1,
   DataSample& rdp, DataSample& mcp, std::string& momentum, int n, double* bins,
   const std::string& signal, const std::string& cut,
   const std::string& detector, const std::string& particle)
{
   draw.SetLegendSize(0.15, 0.1);
   draw.SetLegendPos("br");
   draw.SetTitleX("Track Momentum (MeV)");
   draw.SetTitleY("Systematic Uncertainty");
   std::ostringstream ss;

   TH1F histogram("", "", n, bins);
   draw.PlotSystematic(rdp, mcp, momentum, signal, cut, n, bins, histogram,
      "e1", "#nu");
   ss << "syst_mom_" << detector << "_" << particle << ".png";
   c1->Print(ss.str().c_str(), "png");
}

void DrawTrackAngleSystematics(DrawingToolsTPCECal& draw, TCanvas* c1,
   DataSample& rdp, DataSample& mcp, std::string& angle, int n, double* bins,
   const std::string& signal, const std::string& cut,
   const std::string& detector, const std::string& particle)
{
   draw.SetLegendSize(0.15, 0.1);
   draw.SetLegendPos("br");
   draw.SetTitleX("cos(Track Angle)");
   draw.SetTitleY("Systematic Uncertainty");
   std::ostringstream ss;

   TH1F histogram("", "", n, bins);
   draw.PlotSystematic(rdp, mcp, angle, signal, cut, n, bins, histogram,
      "e1", "#nu");
   ss << "syst_ang_" << detector << "_" << particle << ".png";
   c1->Print(ss.str().c_str(), "png");
}

int main(int argc, char *argv[])
{
   vecstr nu_rdp_files;
   vecstr nu_mcp_files;
   GetNuModeFilenames(nu_rdp_files, nu_mcp_files);

   vecstr nubar_rdp_files;
   vecstr nubar_mcp_files;
   GetNubarModeFilenames(nubar_rdp_files, nubar_mcp_files);
 
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

   TCanvas* c1 = new TCanvas("c", "c", 600, 400);
   
   // nu mode
   vecstr nu_particle;
   nu_particle.push_back("e");
   nu_particle.push_back("mu");
   nu_particle.push_back("p");
   for(unsigned int i = 0; i < nu_rdp_files.size(); ++i)
   {
      DataSample rdp = GetDataSample(nu_rdp_files[i]);
      DataSample mcp = GetDataSample(nu_mcp_files[i]);

      DrawingToolsTPCECal draw(nu_mcp_files[i]);

      draw.SetDifferentStackFillStyles();
      draw.ApplyRange(false);
      draw.DumpPOT(rdp);
      draw.DumpPOT(mcp);

      std::ostringstream ss;

      // Momentum selections
      DrawMomentumSelection(draw, c1, rdp, mcp, momentum, nds_mom, ds_bins_mom,
         isDownstream, "ds", nu_particle[i]);
      DrawMomentumSelection(draw, c1, rdp, mcp, momentum, nbr_mom, br_bins_mom,
         isBarrel, "br", nu_particle[i]);

      // Track angle selections
      DrawTrackAngleSelection(draw, c1, rdp, mcp, angle, nds_ang, ds_bins_ang,
         isDownstream, "ds", nu_particle[i]);
      DrawTrackAngleSelection(draw, c1, rdp, mcp, angle, nbr_ang, br_bins_ang,
         isBarrel, "br", nu_particle[i]);

      // Momentum effiencies
      DrawMomentumEfficiencies(draw, c1, rdp, mcp, momentum, nds_mom, ds_bins_mom,
         isDownstream, recoDS, "ds", nu_particle[i]);
      DrawMomentumEfficiencies(draw, c1, rdp, mcp, momentum, nbr_mom, br_bins_mom,
         isBarrel, recoBr, "br", nu_particle[i]);

      // Track angle effiencies
      DrawTrackAngleEfficiencies(draw, c1, rdp, mcp, angle, nds_ang, ds_bins_ang,
         isDownstream, recoDS, "ds", nu_particle[i]);
      DrawTrackAngleEfficiencies(draw, c1, rdp, mcp, angle, nbr_ang, br_bins_ang,
         isBarrel, recoBr, "br", nu_particle[i]);

      // Momentum systematics
      DrawMomentumSystematics(draw, c1, rdp, mcp, momentum, nds_mom, ds_bins_mom,
         isDownstream, recoDS, "ds", nu_particle[i]);
      DrawMomentumSystematics(draw, c1, rdp, mcp, momentum, nbr_mom, br_bins_mom,
         isBarrel, recoBr, "br", nu_particle[i]);

      // Track angle systematics
      DrawTrackAngleSystematics(draw, c1, rdp, mcp, angle, nds_ang, ds_bins_ang,
         isDownstream, recoDS, "ds", nu_particle[i]);
      DrawTrackAngleSystematics(draw, c1, rdp, mcp, angle, nbr_ang, br_bins_ang,
         isBarrel, recoBr, "br", nu_particle[i]);
   }  

   // nubar mode
   vecstr nubar_particle;
   nubar_particle.push_back("ebar");
   nubar_particle.push_back("mubar");
   for(unsigned int i = 0; i < nubar_rdp_files.size(); ++i)
   {
      DataSample rdp = GetDataSample(nubar_rdp_files[i]);
      DataSample mcp = GetDataSample(nubar_mcp_files[i]);

      DrawingToolsTPCECal draw(nu_mcp_files[i]);
      draw.SetDifferentStackFillStyles();
      draw.ApplyRange(false);
      draw.DumpPOT(rdp);
      draw.DumpPOT(mcp);

      std::ostringstream ss;

      // Momentum selections
      DrawMomentumSelection(draw, c1, rdp, mcp, momentum, nds_mom, ds_bins_mom,
         isDownstream, "ds", nubar_particle[i]);
      DrawMomentumSelection(draw, c1, rdp, mcp, momentum, nbr_mom, br_bins_mom,
         isBarrel, "br", nubar_particle[i]);

      // Track angle selections
      DrawTrackAngleSelection(draw, c1, rdp, mcp, angle, nds_ang, ds_bins_ang,
         isDownstream, "ds", nubar_particle[i]);
      DrawTrackAngleSelection(draw, c1, rdp, mcp, angle, nbr_ang, br_bins_ang,
         isBarrel, "br", nubar_particle[i]);

      // Momentum effiencies
      DrawMomentumEfficiencies(draw, c1, rdp, mcp, momentum, nds_mom, ds_bins_mom,
         isDownstream, recoDS, "ds", nubar_particle[i]);
      DrawMomentumEfficiencies(draw, c1, rdp, mcp, momentum, nbr_mom, br_bins_mom,
         isBarrel, recoBr, "br", nubar_particle[i]);

      // Track angle effiencies
      DrawTrackAngleEfficiencies(draw, c1, rdp, mcp, angle, nds_ang, ds_bins_ang,
         isDownstream, recoDS, "ds", nubar_particle[i]);
      DrawTrackAngleEfficiencies(draw, c1, rdp, mcp, angle, nbr_ang, br_bins_ang,
         isBarrel, recoBr, "br", nubar_particle[i]);

      // Momentum systematics
      DrawMomentumSystematics(draw, c1, rdp, mcp, momentum, nds_mom, ds_bins_mom,
         isDownstream, recoDS, "ds", nubar_particle[i]);
      DrawMomentumSystematics(draw, c1, rdp, mcp, momentum, nbr_mom, br_bins_mom,
         isBarrel, recoBr, "br", nubar_particle[i]);

      // Track angle systematics
      DrawTrackAngleSystematics(draw, c1, rdp, mcp, angle, nds_ang, ds_bins_ang,
         isDownstream, recoDS, "ds", nubar_particle[i]);
      DrawTrackAngleSystematics(draw, c1, rdp, mcp, angle, nbr_ang, br_bins_ang,
         isBarrel, recoBr, "br", nubar_particle[i]);
   }

   delete c1;
}
