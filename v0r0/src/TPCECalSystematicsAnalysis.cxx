#include "TPCECalSystematicsAnalysis.hxx"
#include "FiducialVolumeDefinition.hxx"
#include "Parameters.hxx"
#include "TPCECalSelection.hxx"
#include "CategoriesUtils.hxx"
#include "BasicUtils.hxx"
#include "baseToyMaker.hxx"

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
  sel().AddSelection("TPCECal",  "TPC/ECal selection",     new TPCECalSelection(false));

  if (!ND::params().GetParameterI("TPCECalSystematicsAnalysis.Selections.RunSelectionWithBranches"))
    sel().DisableSelection("TPCECal");

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
   AddVarI(output(), isMuonLike, "is muon candidate");
   AddVarI(output(), isProtonLike, "is proton candidate");
   AddVarI(output(), entersBarrel, "appears to enter the barrel ECal");
   AddVarI(output(), entersDownstream, "appears to enter the downstream ECal");
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
   if (tpcECalBox->MainTrack)
   {
      output().FillVar(isMuonLike, tpcECalBox->isMuonLike);
      output().FillVar(isProtonLike, tpcECalBox->isProtonLike);
      output().FillVar(entersBarrel, tpcECalBox->entersBarrel);
      output().FillVar(entersDownstream, tpcECalBox->entersDownstream);
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

void TPCECalSystematicsAnalysis::FillCategories(){
  // For the muon candidate
  anaUtils::FillCategories(_event, static_cast<AnaTrack*>(box().MainTrack),"", SubDetId::kFGD1);
}
