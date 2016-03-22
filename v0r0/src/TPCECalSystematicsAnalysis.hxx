#ifndef TPCECalSystematicsAnalysis_h
#define TPCECalSystematicsAnalysis_h

#include "baseAnalysis.hxx"
#include "AnalysisUtils.hxx"

class TPCECalSystematicsAnalysis: public baseAnalysis {
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

   /*
      Create an enum with all the variables we want to add to the trees,
      starting with the final value used by the analysis that we build on, in
      this case baseTrackerAnalysis
   */
   enum enumStandardMicroTrees_TPCECalSystematicsAnalysis
   {
      entersBarrel=enumStandardMicroTreesLast_baseAnalysis + 1,
      entersDownstream,
      ecalDetector,
      isMuonLike,
      isAntiMuonLike,
      isElectronLike,
      isPositronLike,
      isProtonLike,
      charge,
      momentum,
      direction,
      truemu_mom,
      truemu_costheta,
      enumStandardMicroTreesLast_TPCECalSystematicsAnalysis
   };

private:
   /**
      Extracts the bits that identify parts of the barrel ECal and checks if
      they are set.
      \param detector   The bitfield containing all of the subdetectors
                        intersected by a track. This value comes from the field
                        AnaTrackB::Detector.
      \return  True if the track intersects part of the barrel ECal, False
               otherwise.
   */
   bool IsBarrelECal(const unsigned long detector);

   /**
      Extracts the bit that identifies the DS ECal and checks if it is set.
      \param detector   The bitfield containing all of the subdetectors
                        intersected by a track. This value comes from the field
                        AnaTrackB::Detector.
      \return  True if the track intersects the DS ECal, False otherwise.
   */
   bool IsDSECal(const unsigned long detector);
};

#endif
