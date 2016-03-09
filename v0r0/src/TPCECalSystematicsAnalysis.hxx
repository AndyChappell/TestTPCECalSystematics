#ifndef TPCECalSystematicsAnalysis_h
#define TPCECalSystematicsAnalysis_h

#include "baseTrackerAnalysis.hxx"
#include "AnalysisUtils.hxx"

class TPCECalSystematicsAnalysis: public baseTrackerAnalysis {
 public:
  TPCECalSystematicsAnalysis(AnalysisAlgorithm* ana=NULL);
  virtual ~TPCECalSystematicsAnalysis(){}

  //---- These are mandatory functions
  bool Initialize();

  void DefineSelections();
  void DefineCorrections();
  void DefineSystematics();
  void DefineConfigurations();
  void DefineMicroTrees(bool addBase=true);
  void DefineTruthTree();

  void FillMicroTrees(bool addBase=true);
  void FillToyVarsInMicroTrees(bool addBase=true);

  bool CheckFillTruthTree(const AnaTrueVertex& vtx);

  void FillTruthTree(const AnaTrueVertex& vtx);
  //--------------------

  void FillCategories();

public:

  /* Here we create an enum with all variables we want to add to the trees, starting at the last variable it was added by used analyses, 
     in this case the last of the nueCCAnalysis
   */

  enum enumStandardMicroTrees_TPCECalSystematicsAnalysis{
    entersBarrel=enumStandardMicroTreesLast_baseTrackerAnalysis + 1,
    entersDownstream,
    isMuonLike,
    isProtonLike,
    truemu_mom,
    truemu_costheta,
    enumStandardMicroTreesLast_TPCECalSystematicsAnalysis
  };  
};

#endif
