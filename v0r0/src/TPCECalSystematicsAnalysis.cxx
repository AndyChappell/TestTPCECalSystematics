#include "TPCECalSystematicsAnalysis.hxx"
#include "FiducialVolumeDefinition.hxx"
#include "Parameters.hxx"
#include "TPCECalElectronSelection.hxx"
#include "TPCECalMuonSelection.hxx"
#include "TPCECalProtonSelection.hxx"
#include "TPCECalAntiMuonSelection.hxx"
#include "TPCECalPositronSelection.hxx"
#include "CategoriesUtils.hxx"
#include "BasicUtils.hxx"
#include "baseToyMaker.hxx"
#include "SubDetId.hxx"

TPCECalSystematicsAnalysis::TPCECalSystematicsAnalysis(AnalysisAlgorithm* ana) : baseTrackerAnalysis(ana) {
  // Add the package version (to be stored in the "config" tree)
  ND::versioning().AddPackage("TPCECalSystematicsAnalysis", anaUtils::GetSoftwareVersionFromPath((std::string)getenv("TPCECALSYSTEMATICSANALYSISROOT")));
}

bool TPCECalSystematicsAnalysis::Initialize(){
  // Initialize the base class
  if (!baseTrackerAnalysis::Initialize()) return false;

  // Define categories
  ND::categ().AddStandardCategories();

  // Minimum accum level to save event into the output Micro-trees
  SetMinAccumCutLevelToSave(ND::params().GetParameterI("TPCECalSystematicsAnalysis.MinAccumLevelToSave"));

  return true;
}

void TPCECalSystematicsAnalysis::DefineSelections(){
   // Add a more complicated selection with branches
   sel().AddSelection("TPCECalMuon",  "TPC/ECal muon selection", new TPCECalMuonSelection(false));
   sel().AddSelection("TPCECalElectron",  "TPC/ECal electron selection", new TPCECalElectronSelection(false));
   sel().AddSelection("TPCECalProton",  "TPC/ECal proton selection", new TPCECalProtonSelection(false));
   sel().AddSelection("TPCECalAntiMuon",  "TPC/ECal antimuon selection", new TPCECalAntiMuonSelection(false));
   sel().AddSelection("TPCECalPositron",  "TPC/ECal positron selection", new TPCECalPositronSelection(false));

   if(!ND::params().GetParameterI(
      "TPCECalSystematicsAnalysis.Selections.RunMuonSelection"))
   {
      sel().DisableSelection("TPCECalMuon");
      std::cout << "Disable muon" << std::endl;
   }
   if(!ND::params().GetParameterI(
      "TPCECalSystematicsAnalysis.Selections.RunElectronSelection"))
   {
      sel().DisableSelection("TPCECalElectron");
      std::cout << "Disable electron" << std::endl;
   }
   if(!ND::params().GetParameterI(
      "TPCECalSystematicsAnalysis.Selections.RunProtonSelection"))
   {
      sel().DisableSelection("TPCECalProton");
      std::cout << "Disable proton" << std::endl;
   }
   if(!ND::params().GetParameterI(
      "TPCECalSystematicsAnalysis.Selections.RunAntiMuonSelection"))
   {
      sel().DisableSelection("TPCECalAntiMuon");
      std::cout << "Disable antimuon" << std::endl;
   }
   if(!ND::params().GetParameterI(
      "TPCECalSystematicsAnalysis.Selections.RunPositronSelection"))
   {
      sel().DisableSelection("TPCECalPositron");
      std::cout << "Disable positron" << std::endl;
   }
}

void TPCECalSystematicsAnalysis::DefineCorrections(){
  // Some corrections are defined in baseTrackerAnalysis (have a look at baseTrackerAnalysis/vXrY/src/baseTrackerAnalysis.cxx)
  baseTrackerAnalysis::DefineCorrections();
}

void TPCECalSystematicsAnalysis::DefineSystematics(){
  // Some systematics are defined in baseTrackerAnalysis (have a look at baseTrackerAnalysis/vXrY/src/baseTrackerAnalysis.cxx)
  baseTrackerAnalysis::DefineSystematics();
}

void TPCECalSystematicsAnalysis::DefineConfigurations(){
  // Some configurations are defined in baseTrackerAnalysis (have a look at baseTrackerAnalysis/vXrY/src/baseTrackerAnalysis.cxx)
  baseTrackerAnalysis::DefineConfigurations();
}

void TPCECalSystematicsAnalysis::DefineMicroTrees(bool addBase)
{
   // Variables from baseTrackerAnalysis (run, event, ..., tracker related stuff, ...)
   if(addBase)
   {
      baseTrackerAnalysis::DefineMicroTrees(addBase);
   }

   // --- Single variables -------
   AddVarI(output(), entersBarrel, "appears to enter the barrel ECal");
   AddVarI(output(), entersDownstream, "appears to enter the downstream ECal");
   AddVarI(output(), ecalDetector, "Number identifying the part of the ECal that"
      "the track appears to enter. 9 == DS, 5 - 8 == BR");
   AddVarI(output(), isMuonLike, "is muon candidate");
   AddVarI(output(), isAntiMuonLike, "is antimuon candidate");
   AddVarI(output(), isElectronLike, "is electron candidate");
   AddVarI(output(), isPositronLike, "is positron candidate");
   AddVarI(output(), isProtonLike, "is proton candidate");
   AddVarF(output(), charge, "Reconstructed charge of the selected track");
   AddVarF(output(), momentum, "Reconstructed momentum of the selected track");

   // --- Vectir variables -------

   AddVar3VF(output(), direction, "End direction of the selected track");
}

void TPCECalSystematicsAnalysis::DefineTruthTree(){
  // Variables from baseTrackerAnalysis (run, event, ...)
  baseTrackerAnalysis::DefineTruthTree();

  //--- muon variables -------
  AddVarF(  output(), truemu_mom,      "true muon momentum");
  AddVarF(  output(), truemu_costheta, "true muon cos(theta)");
}

void TPCECalSystematicsAnalysis::FillMicroTrees(bool addBase)
{
   // Variables from baseTrackerAnalysis (run, event, ...)
   if (addBase)
   {
      baseTrackerAnalysis::FillMicroTreesBase(addBase);
   }

   const ToyBoxTPCECal* tpcECalBox = static_cast<const ToyBoxTPCECal*>(&box());

   // Muon candidate variables
   AnaTrackB* track = tpcECalBox->selectedTrack;
   if (track)
   {
      output().FillVar(entersBarrel, tpcECalBox->entersBarrel);
      output().FillVar(entersDownstream, tpcECalBox->entersDownstream);

      int det = SubDetId::kInvalid;
      if(IsDSECal(track->Detector))
      {  // DS ECal
         det = SubDetId::kDSECAL;
      }
      else if(IsBarrelECal(track->Detector))
      {  // Barrel ECal
         det = SubDetId::kTECAL;
      }

      output().FillVar(ecalDetector, det);
      output().FillVar(isMuonLike, tpcECalBox->isMuonLike);
      output().FillVar(isAntiMuonLike, tpcECalBox->isAntiMuonLike);
      output().FillVar(isElectronLike, tpcECalBox->isElectronLike);
      output().FillVar(isPositronLike, tpcECalBox->isPositronLike);
      output().FillVar(isProtonLike, tpcECalBox->isProtonLike);

      AnaTpcTrack* backTpc = static_cast<AnaTpcTrack*>(
         anaUtils::GetTPCBackSegment(track));

      output().FillVar(charge, backTpc->Charge);
      output().FillVar(momentum, backTpc->Momentum);
      output().FillVectorVarFromArray(direction, backTpc->DirectionEnd, 3);
   }   
}

void TPCECalSystematicsAnalysis::FillToyVarsInMicroTrees(bool addBase){
  // Fill the common variables
  if (addBase) baseTrackerAnalysis::FillToyVarsInMicroTreesBase(addBase);
}

bool TPCECalSystematicsAnalysis::CheckFillTruthTree(const AnaTrueVertex& vtx){
  // In this case we only save numu (NuPDG=14) charged current  (0<ReacCode<30) interactions in the FGD1 FV
  bool numuCC=vtx.ReacCode>0 && vtx.ReacCode<30 && vtx.NuPDG==14;// && vtx.LeptonPDG==13;  
  return (anaUtils::InFiducialVolume(SubDetId::kFGD1, vtx.Position, FVDef::FVdefminFGD1,FVDef::FVdefmaxFGD1) && numuCC);
}

void TPCECalSystematicsAnalysis::FillTruthTree(const AnaTrueVertex& vtx){
  // Fill the common variables defined in baseTrackerAnalysis/vXrY/src/baseTrackerAnalysis.cxx
  baseTrackerAnalysis::FillTruthTreeBase(vtx);
  
  // ---- Fill the extra variables ------
  output().FillVar(truemu_costheta,(Float_t)cos(utils::ArrayToTVector3(vtx.LeptonDir).Angle(utils::ArrayToTVector3(vtx.NuDir))));
  output().FillVar(truemu_mom,vtx.LeptonMom);
}

void TPCECalSystematicsAnalysis::FillCategories()
{
   const ToyBoxTPCECal* tpcECalBox = static_cast<const ToyBoxTPCECal*>(&box());
   anaUtils::FillCategories(_event, static_cast<AnaTrack*>(
      tpcECalBox->selectedTrack), "", SubDetId::kFGD1);
}

bool TPCECalSystematicsAnalysis::IsBarrelECal(const unsigned long detector)
{
   bool barrelLeft = (detector & (1 << SubDetId::kLeftTECAL)) ? true : false;
   bool barrelRight = (detector & (1 << SubDetId::kRightTECAL)) ? true : false;
   bool barrelTop = (detector & (1 << SubDetId::kTopTECAL)) ? true : false;
   bool barrelBottom = (detector & (1 << SubDetId::kBottomTECAL)) ? true : false;
      
   return (barrelLeft || barrelRight || barrelTop || barrelBottom);
}

bool TPCECalSystematicsAnalysis::IsDSECal(const unsigned long detector)
{   
   return (detector & (1 << SubDetId::kDSECAL)) ? true : false;
}

