{
   //char* mcfile = getenv("TPCECALMCFILE");
   char* mcfile = getenv("TPCECALANTIMCFILE");

   if(!mcfile)
   {
      std::cerr << "Error: Must set $TPCECALMCFILE. Exiting." << std::endl;
      exit(1);
   }

   // Read the data file.
   DataSample mc1(mcfile);
   DataSample mc2(mcfile);
   // Pass the file to drawing tools to provide configuration information
   DrawingToolsTPCECal draw(mcfile);

   // Check validity of file.
   if(mc1.GetTree()->GetEntries() == 0 || mc2.GetTree()->GetEntries() == 0)
   {
      std::cerr << "Error: Tree has no entries. Exiting." << std::endl;
      exit(1);
   }

   // Print out the POT.
   std::cout << std::endl << "Neutrino MC POT" << std::endl;
   draw.DumpPOT(mc1);
   std::cout << std::endl << "Neutrino MC2 POT" << std::endl;
   draw.DumpPOT(mc2);

   draw.SetDifferentStackFillStyles();
   draw.ApplyRange(false);
   gStyle->SetOptStat(0);

   // DS ECal binning
   const int nds_mom = 6;
   const double ds_bins_mom[7] = {0, 400, 800, 1200, 2000, 3600, 5000};
//   const int nds_mom = 12;
//   const double ds_bins_mom[13] = {0, 400, 800, 1200, 1600, 2000, 2400, 2800, 3200, 3600, 4000, 4400, 4800};
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

   // PID tags
   std::string isElectron = "isElectronLike==1";
   std::string isPositron = "isPositronLike==1";
   std::string isMuon = "isMuonLike==1";
   std::string isAntiMuon = "isAntiMuonLike==1";
   std::string isProton = "isProtonLike==1";

   //std::string isEl = "&& pullelec>-1 && pullelec<2.5 && (pullmuon>2.5 || pullmuon<-2.5) && (pullpion>2 || pullpion<-2)";
   //std::string isMu = "&& pullmuon>-2 && pullmuon<2 && (pullelec>2 || pullelec<-1)";
   //std::string isProt = "&& pullprot>-2 && pullprot<2 && (pullelec>2 || pullelec<-1)";

   std::string momentum = "momentum";
   std::string angle = "direction[2]";
   std::string isNeg = "charge<0";
   std::string isPos = "charge>0";

   draw.SetLegendSize(0.2, 0.4);

   draw.SetLegendPos("tr");
   draw.SetTitleX("Track Momentum (MeV)");
   draw.SetTitleY("Counts/Bin");
   draw.Draw(mc1, mc2, momentum, nds_mom, ds_bins_mom, "particle", isDownstream + " && " + isElectron);
   c1->Print("temp_sel_ds_ele.png", "png");
   draw.Draw(mc1, mc2, momentum, nbr_mom, br_bins_mom, "particle", isBarrel + " && " + isElectron);
   c1->Print("temp_sel_br_ele.png", "png");
   draw.Draw(mc1, mc2, momentum, nds_mom, ds_bins_mom, "particle", isDownstream + " && " + isMuon);
   c1->Print("temp_sel_ds_mu.png", "png");
   draw.Draw(mc1, mc2, momentum, nbr_mom, br_bins_mom, "particle", isBarrel + " && " + isMuon);
   c1->Print("temp_sel_br_mu.png", "png");
   draw.Draw(mc1, mc2, momentum, nds_mom, ds_bins_mom, "particle", isDownstream + " && " + isProton);
   c1->Print("temp_sel_ds_p.png", "png");
   draw.Draw(mc1, mc2, momentum, nbr_mom, br_bins_mom, "particle", isBarrel + " && " + isProton);
   c1->Print("temp_sel_br_p.png", "png");
   draw.Draw(mc1, mc2, momentum, nds_mom, ds_bins_mom, "particle", isDownstream + " && " + isAntiMuon);
   c1->Print("temp_sel_ds_mubar.png", "png");
   draw.Draw(mc1, mc2, momentum, nbr_mom, br_bins_mom, "particle", isBarrel + " && " + isAntiMuon);
   c1->Print("temp_sel_br_mubar.png", "png");
   draw.Draw(mc1, mc2, momentum, nds_mom, ds_bins_mom, "particle", isDownstream + " && " + isPositron);
   c1->Print("temp_sel_ds_ebar.png", "png");
   draw.Draw(mc1, mc2, momentum, nbr_mom, br_bins_mom, "particle", isBarrel + " && " + isPositron);
   c1->Print("temp_sel_br_ebar.png", "png");

   draw.SetLegendPos("tl");
   draw.SetTitleY("Purity and Efficiency");

/*   std::cout << "DS Mu Purity" << std::endl;
   draw.DrawEffPurVSCut(mc1, 0, 0, "particle==13");
   c1->Print("temp_pur_ds_mu.png", "png");

   std::cout << "Barrel Mu Purity" << std::endl;
   draw.DrawEffPurVSCut(mc1, 0, 1, "particle==13");
   c1->Print("temp_pur_br_mu.png", "png");*/

/*   std::cout << "DS Ele Purity" << std::endl;
   draw.DrawEffPurVSCut(mc1, 1, 0, "particle==11", "");
   c1->Print("temp_pur_ds_ele.png", "png");

   std::cout << "Barrel Ele Purity" << std::endl;
   draw.DrawEffPurVSCut(mc1, 1, 1, "particle==11", "");
   c1->Print("temp_pur_br_ele.png", "png");*/

//   draw.DrawEventsVSCut(mc1, 0, "", 2)

/*   std::cout << "DS Prot Purity" << std::endl;
   draw.DrawEffPurVSCut(mc1, 2, 0, "particle==2212", "");
   c1->Print("temp_pur_ds_p.png", "png");

   std::cout << "Barrel Prot Purity" << std::endl;
   draw.DrawEffPurVSCut(mc1, 2, 1, "particle==2212", "");
   c1->Print("temp_pur_br_p.png", "png");*/

/*  std::cout << "DS Mu Purity" << std::endl;
   draw.DrawEffPurVSCut(mc1, 3, 0, "particle==-13");
   c1->Print("temp_pur_ds_mubar.png", "png");

   std::cout << "Barrel Mu Purity" << std::endl;
   draw.DrawEffPurVSCut(mc1, 3, 1, "particle==-13");
   c1->Print("temp_pur_br_mubar.png", "png");*/

   std::cout << "DS Ele Purity" << std::endl;
   draw.DrawEffPurVSCut(mc1, 4, 0, "particle==-11", "");
   c1->Print("temp_pur_ds_ebar.png", "png");

   std::cout << "Barrel Ele Purity" << std::endl;
   draw.DrawEffPurVSCut(mc1, 4, 1, "particle==-11", "");
   c1->Print("temp_pur_br_ebar.png", "png");
}
