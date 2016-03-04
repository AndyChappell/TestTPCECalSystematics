#include "TestTPCECalSystematics.hxx"
#include "TPCECalSelection.hxx"
#include "FiducialVolumeDefinition.hxx"
#include "Parameters.hxx"
#include "CategoriesUtils.hxx"
#include "BasicUtils.hxx"
#include "baseToyMaker.hxx"

TestTPCECalSystematics::TestTPCECalSystematics(AnalysisAlgorithm* ana) :
   baseTrackerAnalysis(ana)
{
   // Add the package version (to be stored in the "config" tree)
   std::cout << std::string(getenv("TESTTPCECALSYSTEMATICSROOT")) << std::endl;
   ND::versioning().AddPackage("TestTPCECalSystematics",
      anaUtils::GetSoftwareVersionFromPath(
         std::string(getenv("TESTTPCECALSYSTEMATICSROOT"))));
}

bool TestTPCECalSystematics::Initialize()
{
   // Initialize the base class
   if(!baseTrackerAnalysis::Initialize())
   {
      return false;
   }

   // Define categories
   ND::categ().AddStandardCategories();

   // Minimum accum level to save event into the output micro-trees
   SetMinAccumCutLevelToSave(
      ND::params().GetParameterI("TestTPCECalSystematics.MinAccumLevelToSave"));

   return true;
}

void TestTPCECalSystematics::DefineSelections()
{
   // Add selections to the SelectionManager provided:
   // - (Selection Name, Selection Title, Selection)
   // - The argument in the selection constructor (false) indicates the step
   //   sequence is not broken when a cut is not passed.
   sel().AddSelection("kTPCECalSelection", "TPC/ECal selection",
      new TPCECalSelection(false));

   // Disable any selections not enabled in the parameters file
   if(!ND::params().GetParameterI(
      "TestTPCECalSystematics.Selections.RunSelection"))
   {
      sel().DisableSelection("kTPCECalSelection");
   }
}

void TestTPCECalSystematics::DefineCorrections()
{
   // Corrections defined in baseTrackerAnalysis
   baseTrackerAnalysis::DefineCorrections();
}

void TestTPCECalSystematics::DefineSystematics()
{
   // Systematics defined in baseTrackerAnalysis
   baseTrackerAnalysis::DefineSystematics();
}

void TestTPCECalSystematics::DefineConfigurations()
{
   // Configurations defined in baseTrackerAnalysis
   baseTrackerAnalysis::DefineConfigurations();
}

void TestTPCECalSystematics::DefineMicroTrees(bool addBase)
{
   // -------- Add variables to the analysis tree ----------------------

   // Variables from baseTrackerAnalysis
   if(addBase)
   {
      baseTrackerAnalysis::DefineMicroTrees(addBase);
   }
}

void TestTPCECalSystematics::DefineTruthTree()
{
   // Variables from baseTrackerAnalysis
   baseTrackerAnalysis::DefineTruthTree();
}

void TestTPCECalSystematics::FillMicroTrees(bool addBase)
{
   // Variables from baseTrackerAnalysis
   if(addBase)
   {
      baseTrackerAnalysis::FillMicroTreesBase(addBase);
   }
}

void TestTPCECalSystematics::FillToyVarsInMicroTrees(bool addBase)
{
   // Fill the common variables
   if(addBase)
   {
      baseTrackerAnalysis::FillToyVarsInMicroTreesBase(addBase);
   }
}

bool TestTPCECalSystematics::CheckFillTruthTree(const AnaTrueVertex& vtx)
{
   (void)vtx;

   // Truth tree not needed
   return false;
}

void TestTPCECalSystematics::FillTruthTree(const AnaTrueVertex& vtx)
{
   // Fill the common variables defined in baseTrackerAnalysis
   baseTrackerAnalysis::FillTruthTreeBase(vtx);
}

void TestTPCECalSystematics::FillCategories()
{
   baseTrackerAnalysis::FillCategories();
}
