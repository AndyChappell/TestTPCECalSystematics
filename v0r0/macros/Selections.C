{
//   std::string mcfile = std::string(getenv("TPCECALMCFILE"));
//   std::string antimcfile = std::string(getenv("TPCECALANTIMCFILE"));

   std::string rdp_e_file = std::string(getenv("E_RDP_FILE"));
   std::string mcp_e_file = std::string(getenv("E_MCP_FILE"));
   std::string rdp_mu_file = std::string(getenv("MU_RDP_FILE"));
   std::string mcp_mu_file = std::string(getenv("MU_MCP_FILE"));
   std::string rdp_p_file = std::string(getenv("P_RDP_FILE"));
   std::string mcp_p_file = std::string(getenv("P_MCP_FILE"));
   std::string rdp_ebar_file = std::string(getenv("EBAR_RDP_FILE"));
   std::string mcp_ebar_file = std::string(getenv("EBAR_MCP_FILE"));
   std::string rdp_mubar_file = std::string(getenv("MUBAR_RDP_FILE"));
   std::string mcp_mubar_file = std::string(getenv("MUBAR_MCP_FILE"));

   if(rdp_e_file.length() == 0 ||
      mcp_e_file.length() == 0 ||
      rdp_mu_file.length() == 0 ||
      mcp_mu_file.length() == 0 ||
      rdp_p_file.length() == 0 ||
      mcp_p_file.length() == 0 ||
      rdp_ebar_file.length() == 0 ||
      mcp_ebar_file.length() == 0 ||
      rdp_mubar_file.length() == 0 ||
      mcp_mubar_file.length() == 0)
   {
      std::cerr << "Error: Environment variables for files not correctly set"
         ". Exiting." << std::endl;
      exit(1);
   }

   // Read the microtree files.
   DataSample rdp_e(rdp_e_file);
   DataSample mcp_e(mcp_e_file);
   DataSample rdp_mu(rdp_mu_file);
   DataSample mcp_mu(mcp_mu_file);
   DataSample rdp_p(rdp_p_file);
   DataSample mcp_p(mcp_p_file);
   DataSample rdp_ebar(rdp_ebar_file);
   DataSample mcp_ebar(mcp_ebar_file);
   DataSample rdp_mubar(rdp_mubar_file);
   DataSample mcp_mubar(mcp_mubar_file);
   // Pass a file to drawing tools to provide configuration information
   DrawingToolsTPCECal draw(mcp_e_file);

   // Check validity of file.
   if(rdp_e.GetTree()->GetEntries() == 0 ||
      mcp_e.GetTree()->GetEntries() == 0 ||
      rdp_mu.GetTree()->GetEntries() == 0 ||
      mcp_mu.GetTree()->GetEntries() == 0 ||
      rdp_p.GetTree()->GetEntries() == 0 ||
      mcp_p.GetTree()->GetEntries() == 0 ||
      rdp_ebar.GetTree()->GetEntries() == 0 ||
      mcp_ebar.GetTree()->GetEntries() == 0 ||
      rdp_mubar.GetTree()->GetEntries() == 0 ||
      mcp_mubar.GetTree()->GetEntries() == 0)
   {
      std::cerr << "Error: Tree has no entries. Exiting." << std::endl;
      exit(1);
   }

   // Print out the POT.
   std::cout << std::endl << "RDP POT" << std::endl;
   draw.DumpPOT(rdp_e);
   std::cout << std::endl << "ARDP POT" << std::endl;
   draw.DumpPOT(rdp_ebar);
   std::cout << std::endl << "MCP POT" << std::endl;
   draw.DumpPOT(mcp_e);
   std::cout << std::endl << "AMCP POT" << std::endl;
   draw.DumpPOT(mcp_ebar);

   draw.SetDifferentStackFillStyles();
   draw.ApplyRange(false);
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

   // Appears to enter barrel based on cuts
   std::string isBarrel = "entersBarrel==1";
   // Appears to enter downstream based on cuts
   std::string isDownstream = "entersDownstream==1";

   // Reconstructed as entering the barrel (SubDetId::SubDetEnum::kTECAL == 6)
   std::string recoBr = "ecalDetector==6";
   // Reconstructed as entering the downstream (SubDetId::SubDetEnum::kDSECAL == 23)
   std::string recoDS = "ecalDetector==23";

   std::string momentum = "momentum";
   std::string angle = "direction[2]";

   draw.SetLegendSize(0.2, 0.4);

   draw.SetLegendPos("tr");
   draw.SetTitleX("Track Momentum (MeV)");
   draw.SetTitleY("Counts/Bin");
   draw.Draw(rdp_e, mcp_e, momentum, nds_mom, ds_bins_mom, "particle", isDownstream);
   c1->Print("temp_sel_ds_e.png", "png");
   draw.Draw(rdp_e, mcp_e, momentum, nbr_mom, br_bins_mom, "particle", isBarrel);
   c1->Print("temp_sel_br_e.png", "png");
   draw.Draw(rdp_mu, mcp_mu, momentum, nds_mom, ds_bins_mom, "particle", isDownstream);
   c1->Print("temp_sel_ds_mu.png", "png");
   draw.Draw(rdp_mu, mcp_mu, momentum, nbr_mom, br_bins_mom, "particle", isBarrel);
   c1->Print("temp_sel_br_mu.png", "png");
   draw.Draw(rdp_p, mcp_p, momentum, nds_mom, ds_bins_mom, "particle", isDownstream);
   c1->Print("temp_sel_ds_p.png", "png");
   draw.Draw(rdp_p, mcp_p, momentum, nbr_mom, br_bins_mom, "particle", isBarrel);
   c1->Print("temp_sel_br_p.png", "png");
   draw.Draw(rdp_mubar, mcp_mubar, momentum, nds_mom, ds_bins_mom, "particle", isDownstream);
   c1->Print("temp_sel_ds_mubar.png", "png");
   draw.Draw(rdp_mubar, mcp_mubar, momentum, nbr_mom, br_bins_mom, "particle", isBarrel);
   c1->Print("temp_sel_br_mubar.png", "png");
   draw.Draw(rdp_ebar, mcp_ebar, momentum, nds_mom, ds_bins_mom, "particle", isDownstream);
   c1->Print("temp_sel_ds_ebar.png", "png");
   draw.Draw(rdp_ebar, mcp_ebar, momentum, nbr_mom, br_bins_mom, "particle", isBarrel);
   c1->Print("temp_sel_br_ebar.png", "png");

   draw.SetLegendPos("tl");
   draw.SetTitleY("Purity and Efficiency");

   std::cout << "DS e Purity" << std::endl;
   draw.DrawEffPurVSCut(mcp_e, 0, 0, "particle==11", "");
   c1->Print("temp_pur_ds_e.png", "png");

   std::cout << "Barrel e Purity" << std::endl;
   draw.DrawEffPurVSCut(mcp_e, 0, 1, "particle==11", "");
   c1->Print("temp_pur_br_e.png", "png");

   std::cout << "DS mu Purity" << std::endl;
   draw.DrawEffPurVSCut(mcp_mu, 1, 0, "particle==13");
   c1->Print("temp_pur_ds_mu.png", "png");

   std::cout << "Barrel mu Purity" << std::endl;
   draw.DrawEffPurVSCut(mcp_mu, 1, 1, "particle==13");
   c1->Print("temp_pur_br_mu.png", "png");

   std::cout << "DS p Purity" << std::endl;
   draw.DrawEffPurVSCut(mcp_p, 2, 0, "particle==2212", "");
   c1->Print("temp_pur_ds_p.png", "png");

   std::cout << "Barrel p Purity" << std::endl;
   draw.DrawEffPurVSCut(mcp_p, 2, 1, "particle==2212", "");
   c1->Print("temp_pur_br_p.png", "png");

   std::cout << "DS ebar Purity" << std::endl;
   draw.DrawEffPurVSCut(mcp_ebar, 3, 0, "particle==-11", "");
   c1->Print("temp_pur_ds_ebar.png", "png");

   std::cout << "Barrel ebar Purity" << std::endl;
   draw.DrawEffPurVSCut(mcp_ebar, 3, 1, "particle==-11", "");
   c1->Print("temp_pur_br_ebar.png", "png");

  std::cout << "DS mubar Purity" << std::endl;
   draw.DrawEffPurVSCut(mcp_mubar, 4, 0, "particle==-13");
   c1->Print("temp_pur_ds_mubar.png", "png");

   std::cout << "Barrel mubar Purity" << std::endl;
   draw.DrawEffPurVSCut(mcp_mubar, 5, 1, "particle==-13");
   c1->Print("temp_pur_br_mubar.png", "png");

//   draw.DrawEventsVSCut(mc1, 0, "", 2)
}
