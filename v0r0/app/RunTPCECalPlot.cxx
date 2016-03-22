#include "DrawingToolsTPCECal.hxx"

typedef std::vector<std::string> vecstr;

int main(int argc, char *argv[])
{
   vecstr nuRdpEnvVars;
   nuRdpEnvVars.push_back("E_RDP_FILE");
   nuRdpEnvVars.push_back("MU_RDP_FILE");
   nuRdpEnvVars.push_back("P_RDP_FILE");
   vecstr nuMcpEnvVars;
   nuMcpEnvVars.push_back("E_MCP_FILE");
   nuMcpEnvVars.push_back("MU_MCP_FILE");
   nuMcpEnvVars.push_back("P_MCP_FILE");
   vecstr nubarRdpEnvVars;
   nubarRdpEnvVars.push_back("EBAR_RDP_FILE");
   nubarRdpEnvVars.push_back("MUBAR_RDP_FILE");
   vecstr nubarMcpEnvVars;
   nubarMcpEnvVars.push_back("EBAR_MCP_FILE");
   nubarMcpEnvVars.push_back("MUBAR_MCP_FILE");

   vecstr nu_rdp_files;
   vecstr nu_mcp_files;
   for(unsigned int i = 0; i < nuRdpEnvVars.size(); ++i)
   {
      std::string filename = std::string(getenv(nuRdpEnvVars[i].c_str()));
      if(filename.length() == 0)
      {
         std::cerr << "Error: Environment variable" << nuRdpEnvVars[i] <<
            " not set. Exiting." << std::endl;
         exit(1);
      }
      nu_rdp_files.push_back(filename);

      filename = std::string(getenv(nuMcpEnvVars[i].c_str()));
      if(filename.length() == 0)
      {
         std::cerr << "Error: Environment variable" << nuMcpEnvVars[i] <<
            " not set. Exiting." << std::endl;
         exit(1);
      }
      nu_mcp_files.push_back(filename);
   }

   vecstr nubar_rdp_files;
   vecstr nubar_mcp_files;
   for(unsigned int i = 0; i < nubarRdpEnvVars.size(); ++i)
   {
      std::string filename = std::string(getenv(nubarRdpEnvVars[i].c_str()));
      if(filename.length() == 0)
      {
         std::cerr << "Error: Environment variable" << nubarRdpEnvVars[i] <<
            " not set. Exiting." << std::endl;
         exit(1);
      }
      nubar_rdp_files.push_back(filename);

      filename = std::string(getenv(nubarMcpEnvVars[i].c_str()));
      if(filename.length() == 0)
      {
         std::cerr << "Error: Environment variable" << nubarMcpEnvVars[i] <<
            " not set. Exiting." << std::endl;
         exit(1);
      }
      nubar_mcp_files.push_back(filename);
   }
 
   // Read the microtree files.
   DataSample** rdp_nu = new DataSample*[nu_rdp_files.size()];
   DataSample** mcp_nu = new DataSample*[nu_mcp_files.size()];
   for(unsigned int i = 0; i < nu_rdp_files.size(); ++i)
   {
      rdp_nu[i] = new DataSample(nu_rdp_files[i].c_str());
      if(rdp_nu[i]->GetTree()->GetEntries() == 0)
      {
         std::cerr << "Error: Tree " << nu_rdp_files[i] <<
            " has no entries. Exiting." << std::endl;
         exit(1);
      }
      mcp_nu[i] = new DataSample(nu_mcp_files[i].c_str());
      if(mcp_nu[i]->GetTree()->GetEntries() == 0)
      {
         std::cerr << "Error: Tree " << nu_mcp_files[i] <<
            " has no entries. Exiting." << std::endl;
         exit(1);
      }
   }

   DataSample** rdp_nubar = new DataSample*[nubar_rdp_files.size()];
   DataSample** mcp_nubar = new DataSample*[nubar_mcp_files.size()];
   for(unsigned int i = 0; i < nubar_rdp_files.size(); ++i)
   {
      rdp_nubar[i] = new DataSample(nubar_rdp_files[i].c_str());
      if(rdp_nubar[i]->GetTree()->GetEntries() == 0)
      {
         std::cerr << "Error: Tree " << nubar_rdp_files[i] <<
            " has no entries. Exiting." << std::endl;
         exit(1);
      }
      mcp_nubar[i] = new DataSample(nubar_mcp_files[i].c_str());
      if(mcp_nubar[i]->GetTree()->GetEntries() == 0)
      {
         std::cerr << "Error: Tree " << nubar_mcp_files[i] <<
            " has no entries. Exiting." << std::endl;
         exit(1);
      }
   }

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
      DataSample* rdp = rdp_nu[i];
      DataSample* mcp = mcp_nu[i];

      DrawingToolsTPCECal draw(nu_mcp_files[i]);

      draw.SetDifferentStackFillStyles();
      draw.ApplyRange(false);
      draw.DumpPOT(*rdp);
      draw.DumpPOT(*mcp);

      std::ostringstream ss;

      // Momentum selections
      draw.SetLegendSize(0.2, 0.4);
      draw.SetLegendPos("tr");
      draw.SetTitleX("Track Momentum (MeV)");
      draw.SetTitleY("Counts/Bin");

      draw.Draw(*rdp, *mcp, momentum, nds_mom, ds_bins_mom, "particle",
         isDownstream);    
      ss << "sel_mom_ds_" << nu_particle[i] << ".png";
      c1->Print(ss.str().c_str(), "png");
      
      draw.Draw(*rdp, *mcp, momentum, nbr_mom, br_bins_mom, "particle",
         isBarrel);
      ss.str(""); ss.clear();
      ss << "sel_mom_br_" << nu_particle[i] << ".png";
      c1->Print(ss.str().c_str(), "png");

      // Track angle selections
      draw.SetLegendPos("tl");
      draw.SetTitleX("cos(Track Angle)");
      draw.SetTitleY("Counts/Bin");

      draw.Draw(*rdp, *mcp, angle, nds_ang, ds_bins_ang, "particle",
         isDownstream + " && " + recoDS);
      ss.str(""); ss.clear();
      ss << "sel_ang_br_" << nu_particle[i] << ".png";
      c1->Print(ss.str().c_str(), "png");

      draw.Draw(*rdp, *mcp, angle, nbr_ang, br_bins_ang, "particle",
         isBarrel);
      ss.str(""); ss.clear();
      ss << "sel_ang_br_" << nu_particle[i] << ".png";
      c1->Print(ss.str().c_str(), "png");

      // Momentum effiencies
      draw.SetLegendSize(0.15, 0.1);
      draw.SetLegendPos("br");
      draw.SetTitleY("Matching Efficiency");
      draw.SetTitleX("Track Momentum (MeV)");

      draw.DrawEfficiency(*rdp, momentum, isDownstream, recoDS, nds_mom,
         ds_bins_mom, "", "#nu Data");
      draw.DrawEfficiency(*mcp, momentum, isDownstream, recoDS, nds_mom,
         ds_bins_mom, "same", "#nu MC");         
      ss.str(""); ss.clear();
      ss << "eff_mom_ds_" << nu_particle[i] << ".png";
      c1->Print(ss.str().c_str(), "png");

      std::cout << "********** Barrel Efficiency **********" << std::endl;
      std::cout << "Data\n";
      draw.DrawEfficiency(*rdp, momentum, isBarrel, recoBr, nbr_mom,
         br_bins_mom, "", "#nu Data");
      std::cout << "MC\n";
      draw.DrawEfficiency(*mcp, momentum, isBarrel, recoBr, nbr_mom,
         br_bins_mom, "same", "#nu MC");         
      ss.str(""); ss.clear();
      ss << "eff_mom_br_" << nu_particle[i] << ".png";
      c1->Print(ss.str().c_str(), "png");
      std::cout << "********** End Barrel Efficiency **********" << std::endl;

      // Track angle effiencies
      draw.SetTitleX("cos(Track Angle)");
      
      draw.DrawEfficiency(*rdp, angle, isDownstream, recoDS, nds_ang,
         ds_bins_ang, "", "#nu Data");
      draw.DrawEfficiency(*mcp, momentum, isDownstream, recoDS, nds_ang,
         ds_bins_ang, "same", "#nu MC");
      ss.str(""); ss.clear();
      ss << "eff_ang_ds_" << nu_particle[i] << ".png";
      c1->Print(ss.str().c_str(), "png");

      draw.DrawEfficiency(*rdp, momentum, isBarrel, recoBr, nbr_ang,
         br_bins_ang, "", "#nu Data");
      draw.DrawEfficiency(*mcp, momentum, isBarrel, recoBr, nbr_ang,
         br_bins_ang, "same", "#nu MC");         
      ss.str(""); ss.clear();
      ss << "eff_ang_br_" << nu_particle[i] << ".png";
      c1->Print(ss.str().c_str(), "png");
   }  

   // nubar mode
   vecstr nubar_particle;
   nubar_particle.push_back("ebar");
   nubar_particle.push_back("mubar");
   for(unsigned int i = 0; i < nubar_rdp_files.size(); ++i)
   {
      DataSample* rdp = rdp_nubar[i];
      DataSample* mcp = mcp_nubar[i];

      DrawingToolsTPCECal draw(nu_mcp_files[i]);
      draw.SetDifferentStackFillStyles();
      draw.ApplyRange(false);
      draw.DumpPOT(*rdp);
      draw.DumpPOT(*mcp);

      std::ostringstream ss;

      // Momentum selections
      draw.SetLegendSize(0.2, 0.4);
      draw.SetLegendPos("tr");
      draw.SetTitleX("Track Momentum (MeV)");
      draw.SetTitleY("Counts/Bin");

      draw.Draw(*rdp, *mcp, momentum, nds_mom, ds_bins_mom, "particle",
         isDownstream);    
      ss << "sel_mom_ds_" << nubar_particle[i] << ".png";
      c1->Print(ss.str().c_str(), "png");
      
      draw.Draw(*rdp, *mcp, momentum, nbr_mom, br_bins_mom, "particle",
         isBarrel);
      ss.str(""); ss.clear();
      ss << "sel_mom_br_" << nubar_particle[i] << ".png";
      c1->Print(ss.str().c_str(), "png");

      // Track angle selections
      draw.SetLegendPos("tl");
      draw.SetTitleX("cos(Track Angle)");
      draw.SetTitleY("Counts/Bin");

      draw.Draw(*rdp, *mcp, angle, nds_ang, ds_bins_ang, "particle",
         isDownstream);
      ss.str(""); ss.clear();
      ss << "sel_ang_br_" << nubar_particle[i] << ".png";
      c1->Print(ss.str().c_str(), "png");

      draw.Draw(*rdp, *mcp, angle, nbr_ang, br_bins_ang, "particle",
         isBarrel);
      ss.str(""); ss.clear();
      ss << "sel_ang_br_" << nubar_particle[i] << ".png";
      c1->Print(ss.str().c_str(), "png");

      // Momentum effiencies
      draw.SetLegendSize(0.15, 0.1);
      draw.SetLegendPos("br");
      draw.SetTitleY("Matching Efficiency");
      draw.SetTitleX("Track Momentum (MeV)");

      draw.DrawEfficiency(*rdp, momentum, isDownstream, recoDS, nds_mom,
         ds_bins_mom, "", "#nu Data");
      draw.DrawEfficiency(*mcp, momentum, isDownstream, recoDS, nds_mom,
         ds_bins_mom, "same", "#nu MC");         
      ss.str(""); ss.clear();
      ss << "eff_mom_ds_" << nubar_particle[i] << ".png";
      c1->Print(ss.str().c_str(), "png");

      draw.DrawEfficiency(*rdp, momentum, isBarrel, recoBr, nbr_mom,
         br_bins_mom, "", "#nu Data");
      draw.DrawEfficiency(*mcp, momentum, isBarrel, recoBr, nbr_mom,
         br_bins_mom, "same", "#nu MC");         
      ss.str(""); ss.clear();
      ss << "eff_mom_br_" << nubar_particle[i] << ".png";
      c1->Print(ss.str().c_str(), "png");

      // Track angle effiencies
      draw.SetTitleX("cos(Track Angle)");
      
      draw.DrawEfficiency(*rdp, angle, isDownstream, recoDS, nds_ang,
         ds_bins_ang, "", "#nu Data");
      draw.DrawEfficiency(*mcp, momentum, isDownstream, recoDS, nds_ang,
         ds_bins_ang, "same", "#nu MC");
      ss.str(""); ss.clear();
      ss << "eff_ang_ds_" << nubar_particle[i] << ".png";
      c1->Print(ss.str().c_str(), "png");

      draw.DrawEfficiency(*rdp, momentum, isBarrel, recoBr, nbr_ang,
         br_bins_ang, "", "#nu Data");
      draw.DrawEfficiency(*mcp, momentum, isBarrel, recoBr, nbr_ang,
         br_bins_ang, "same", "#nu MC");         
      ss.str(""); ss.clear();
      ss << "eff_ang_br_" << nubar_particle[i] << ".png";
      c1->Print(ss.str().c_str(), "png");
   }

   delete c1;

   for(unsigned int i = 0; i < nu_rdp_files.size(); ++i)
   {
      delete rdp_nu[i];
      delete mcp_nu[i];
   }

   for(unsigned int i = 0; i < nubar_rdp_files.size(); ++i)
   {
      delete rdp_nubar[i];
      delete mcp_nubar[i];
   }

   delete[] rdp_nu;
   delete[] rdp_nubar;
   delete[] mcp_nu;
   delete[] mcp_nubar;
}
