{
   std::string nuRdpEnvVars[] = {"E_RDP_FILE", "MU_RDP_FILE", "P_RDP_FILE"};
   std::string nuMcpEnvVars[] = {"E_MCP_FILE", "MU_MCP_FILE", "P_MCP_FILE"};
   std::string nubarRdpEnvVars[] = {"EBAR_RDP_FILE", "MUBAR_RDP_FILE"};
   std::string nubarMcpEnvVars[] = {"EBAR_MCP_FILE", "MUBAR_MCP_FILE"};

   std::vector<std::string> nu_rdp_files;
   std::vector<std::string> nu_mcp_files;
   for(int i = 0; i < 3; ++i)
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

   std::vector<std::string> nubar_rdp_files;
   std::vector<std::string> nubar_mcp_files;
   for(int i = 0; i < 2; ++i)
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
   for(int i = 0; i < nu_rdp_files.size(); ++i)
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
   for(int i = 0; i < nubar_rdp_files.size(); ++i)
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
   const int nds_mom = 6;
   const double ds_bins_mom[7] = {0, 400, 800, 1200, 2000, 3600, 5000};
   const int nds_ang = 5;
   const double ds_bins_ang[6] = {0.75, 0.8, 0.85, 0.925, 0.975, 1.0};

   // Barrel ECal binning
   const int nbr_mom = 5;
   double br_bins_mom[6] = {0, 400, 800, 2000, 3600, 5000};
   const int nbr_ang = 4;
   double br_bins_ang[5] = {-0.3, 0.1, 0.4, 0.7, 0.825};

   // Cut variables
   std::string isBarrel = "entersBarrel==1";
   std::string isDownstream = "entersDownstream==1";
   std::string recoBr = "ecalDetector==6";
   std::string recoDS = "ecalDetector==23";

   // Independent variables
   std::string momentum = "momentum";
   std::string angle = "direction[2]";
   
   // nu mode
   std::string nu_particle[] = {"e", "mu", "p"};
   for(int i = 0; i < nu_rdp_files.size(); ++i)
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
         isDownstream);
      ss.str(""); ss.clear();
      ss << "sel_ang_br_" << nu_particle[i] << ".png";
      c1->Print(ss.str().c_str(), "png");

      draw.Draw(*rdp, *mcp, angle, nbr_ang, br_bins_ang, "particle",
         isBarrel);
      ss.str(""); ss.clear();
      ss << "sel_ang_br_" << nu_particle[i] << ".png";
      c1->Print(ss.str().c_str(), "png");
   }  

   // nubar mode
   std::string nubar_particle[] = {"ebar", "mubar"};
   for(int i = 0; i < nubar_rdp_files.size(); ++i)
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
   }  

/*   draw.SetLegendPos("tl");
   draw.SetTitleY("Purity and Efficiency");

   std::cout << "DS e Purity" << std::endl;
   draw.DrawEffPurVSCut(mcp_e, 0, 0, "particle==11", "");
   c1->Print("pur_ds_e.png", "png");

   std::cout << "Barrel e Purity" << std::endl;
   draw.DrawEffPurVSCut(mcp_e, 0, 1, "particle==11", "");
   c1->Print("pur_br_e.png", "png");

   std::cout << "DS mu Purity" << std::endl;
   draw.DrawEffPurVSCut(mcp_mu, 1, 0, "particle==13");
   c1->Print("pur_ds_mu.png", "png");

   std::cout << "Barrel mu Purity" << std::endl;
   draw.DrawEffPurVSCut(mcp_mu, 1, 1, "particle==13");
   c1->Print("pur_br_mu.png", "png");

   std::cout << "DS p Purity" << std::endl;
   draw.DrawEffPurVSCut(mcp_p, 2, 0, "particle==2212", "");
   c1->Print("pur_ds_p.png", "png");

   std::cout << "Barrel p Purity" << std::endl;
   draw.DrawEffPurVSCut(mcp_p, 2, 1, "particle==2212", "");
   c1->Print("pur_br_p.png", "png");

   std::cout << "DS ebar Purity" << std::endl;
   draw.DrawEffPurVSCut(mcp_ebar, 3, 0, "particle==-11", "");
   c1->Print("pur_ds_ebar.png", "png");

   std::cout << "Barrel ebar Purity" << std::endl;
   draw.DrawEffPurVSCut(mcp_ebar, 3, 1, "particle==-11", "");
   c1->Print("pur_br_ebar.png", "png");

  std::cout << "DS mubar Purity" << std::endl;
   draw.DrawEffPurVSCut(mcp_mubar, 4, 0, "particle==-13");
   c1->Print("pur_ds_mubar.png", "png");

   std::cout << "Barrel mubar Purity" << std::endl;
   draw.DrawEffPurVSCut(mcp_mubar, 4, 1, "particle==-13");
   c1->Print("pur_br_mubar.png", "png");

//   draw.DrawEventsVSCut(mc1, 0, "", 2)*/

   for(int i = 0; i < nu_rdp_files.size(); ++i)
   {
      delete rdp_nu[i];
      delete mcp_nu[i];
   }

   for(int i = 0; i < nubar_rdp_files.size(); ++i)
   {
      delete rdp_nubar[i];
      delete mcp_nubar[i];
   }

   delete[] rdp_nu;
   delete[] rdp_nubar;
   delete[] mcp_nu;
   delete[] mcp_nubar;
}
