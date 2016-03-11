#ifndef TPCECalSelection_h
#define TPCECalSelection_h

#include "SelectionBase.hxx"
#include "Parameters.hxx"

//---- Define an specific box for this selection -------
class ToyBoxTPCECal: public ToyBoxB
{
public:
   ToyBoxTPCECal()
   {
      isElectronLike = false;
      isMuonLike = false;
      isProtonLike = false;
      entersBarrel = false;
      entersDownstream = false;
      fgdFVTracks.clear();
      downstreamTracks.clear();
      barrelTracks.clear();
      selectedTrack = nullptr;
   }

   virtual void Reset()
   {
      isElectronLike = false;
      isMuonLike = false;
      isProtonLike = false;
      entersBarrel = false;
      entersDownstream = false;
      fgdFVTracks.clear();
      downstreamTracks.clear();
      barrelTracks.clear();
      selectedTrack = nullptr;
   }
  
   virtual ~ToyBoxTPCECal(){ }

   /// Describes whether this track is thought to be an electron
   bool isElectronLike;
   /// Describes whether this track is thought to be a muon
   bool isMuonLike;
   /// Describes whether this track is thought to be a proton
   bool isProtonLike;
   /// Describes whether this track appears to enter the barrel ECal
   bool entersBarrel;
   /// Describes whether this track appears to enter the downstream ECal
   bool entersDownstream;

   /// Tracks with TPC that start in the FGD FV.
   std::vector<AnaTrackB*> fgdFVTracks;
   /// Tracks appearing to enter the downstream ECal.
   std::vector<AnaTrackB*> downstreamTracks;
   /// Tracks appearing to enter the barrel ECal.
   std::vector<AnaTrackB*> barrelTracks;
   /// The track to use - if more than one surviving track, pick highest momentum
   AnaTrackB* selectedTrack;
};

class Barrel
{
public:
   static const float TpcXMin = -890;
   static const float TpcXMax = +890;
   static const float TpcYMin = -980;
   static const float TpcYMax = +1085;
   static const float TpcZMin = +600;
   static const float TpcZMax = +2600;
   static const float TpcAzimuthAbs = +160;
   static const float TpcAngleMin = +35;
};

class DS
{
public:
   static const float TpcXMin = -920;
   static const float TpcXMax = +920;
   static const float TpcYMin = -910;
   static const float TpcYMax = +930;
   static const float TpcZMin = +2665;
   static const float TpcAngleMax = +40;
};

//---- Define the class for the new selection, which should inherit from SelectionBase or from another existing selection -------
class TPCECalSelection: public SelectionBase{
 public:
  TPCECalSelection(bool forceBreak=true);
  virtual ~TPCECalSelection(){}


  ///========= These are mandatory functions ==================

  /// In this method all steps are added to the selection
  void DefineSteps();
  
  /// Set detector FV
  void DefineDetectorFV();

  /// Create a proper instance of the box (ToyBoxB) to store all relevant 
  /// information to be passed from one step to the next
  ToyBoxB* MakeToyBox() {return new ToyBoxTPCECal();}

  /// Fill the EventBox with the objects needed by this selection
  void InitializeEvent(AnaEventB& event);

  //---- These are optional functions, needed by FITTERS but not by highland2 analyses --------------

  bool FillEventSummary(AnaEventB& event, Int_t allCutsPassed[]);
  nd280Samples::SampleEnum GetSampleEnum(){return nd280Samples::kFGD1NuMuCC;}
};

///---- Define all steps -------
class FillSummaryAction_TPCECal: public StepBase{
public:
  using StepBase::Apply;
  bool Apply(AnaEventB& event, ToyBoxB& box) const;  
  StepBase* MakeClone(){return new FillSummaryAction_TPCECal();}
};

class TrackQualityFiducialCut: public StepBase{
 public:
  using StepBase::Apply;
  bool Apply(AnaEventB& event, ToyBoxB& box) const;
  StepBase* MakeClone(){return new TrackQualityFiducialCut();}
};

class TotalMultiplicityCut: public StepBase{
 public:
  using StepBase::Apply;
  bool Apply(AnaEventB& event, ToyBoxB& box) const;
  StepBase* MakeClone(){return new TotalMultiplicityCut();}
};

class FindMuonPIDAction: public StepBase
{
public:
   FindMuonPIDAction()
   {
      _prod5Cut = false;
   }
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new FindMuonPIDAction(); }
private:
   bool _prod5Cut;
};

class FindProtonPIDAction: public StepBase
{
public:
   FindProtonPIDAction()
   {
   }
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new FindProtonPIDAction(); }
};

class ExternalVetoCut: public StepBase{
 public:
  using StepBase::Apply;
  bool Apply(AnaEventB& event, ToyBoxB& box) const;
  StepBase* MakeClone(){return new ExternalVetoCut();}
};

class DeltaZCut: public StepBase{
 public:
  using StepBase::Apply;
  bool Apply(AnaEventB& event, ToyBoxB& box) const;
  StepBase* MakeClone(){return new DeltaZCut();}
};

class ExternalFGD1lastlayersCut: public StepBase{
 public:
  using StepBase::Apply;
  bool Apply(AnaEventB& event, ToyBoxB& box) const;
  StepBase* MakeClone(){return new ExternalFGD1lastlayersCut();}
};

/// Leading tracks with good quality in FGD1
class FindLeadingTracksAction: public StepBase{
 public:
  using StepBase::Apply;
  bool Apply(AnaEventB& event, ToyBoxB& box) const;
  StepBase* MakeClone(){return new FindLeadingTracksAction();}
};

/// Find the Vertex. For the moment it's just the Star position of the HM track
class FindVertexAction: public StepBase{
 public:
  using StepBase::Apply;
  bool Apply(AnaEventB& event, ToyBoxB& box) const;
  StepBase* MakeClone(){return new FindVertexAction();}
};


class FindVetoTrackAction: public StepBase{
 public:
  using StepBase::Apply;
  bool Apply(AnaEventB& event, ToyBoxB& box) const;
  StepBase* MakeClone(){return new FindVetoTrackAction();}
};

class FindOOFVTrackAction: public StepBase{
 public:
  using StepBase::Apply;
  bool Apply(AnaEventB& event, ToyBoxB& box) const;
  StepBase* MakeClone(){return new FindOOFVTrackAction();}
};

class OneTPCTrackCut: public StepBase{  
    public:
        using StepBase::Apply;
        bool Apply(AnaEventB& event, ToyBoxB& box) const;
        StepBase* MakeClone(){return new OneTPCTrackCut();}
};

class TwoTPCTracksCut: public StepBase{  
    public:
        using StepBase::Apply;
        bool Apply(AnaEventB& event, ToyBoxB& box) const;
        StepBase* MakeClone(){return new TwoTPCTracksCut();}
};

class MoreThanTwoTPCTracksCut: public StepBase{  
    public:
        using StepBase::Apply;
        bool Apply(AnaEventB& event, ToyBoxB& box) const;
        StepBase* MakeClone(){return new MoreThanTwoTPCTracksCut();}
};

class MoreThanTwoLongTPCTracksCut: public StepBase{  
    public:
        using StepBase::Apply;
        bool Apply(AnaEventB& event, ToyBoxB& box) const;
        StepBase* MakeClone(){return new MoreThanTwoLongTPCTracksCut();}
};

class FindLongTPCTracks: public StepBase{
    public:
        using StepBase::Apply;
        bool Apply(AnaEventB& event, ToyBoxB& box) const;  
        StepBase* MakeClone(){return new FindLongTPCTracks();}
};

class FindDownstreamTracksAction: public StepBase
{
   public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new FindDownstreamTracksAction(); }
};

class DownstreamTracksCut: public StepBase
{
   public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new DownstreamTracksCut(); }
};

class FindBarrelTracksAction: public StepBase
{
   public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new FindBarrelTracksAction(); }
};

class BarrelTracksCut: public StepBase
{
   public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new BarrelTracksCut(); }
};

class FindFGDFVTracksAction: public StepBase
{
   public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new FindFGDFVTracksAction(); }
};

class FGDFVTracksCut: public StepBase
{
   public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new FGDFVTracksCut(); }
};

class SelectTrackAction: public StepBase
{
   public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new SelectTrackAction(); }
};

class MuonPIDCut: public StepBase
{
public:
   MuonPIDCut()
   {
      _prod5Cut = false;
   }
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new MuonPIDCut(); }
private:
   bool _prod5Cut;
};

#endif
