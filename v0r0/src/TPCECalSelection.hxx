#ifndef TPCECalSelection_h
#define TPCECalSelection_h

#include <list>
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
      isAntiMuonLike = false;
      isProtonLike = false;
      entersBarrel = false;
      entersDownstream = false;
      downstreamTrack = nullptr;
      barrelTrack = nullptr;
      selectedTrack = nullptr;
      negativeTracks.clear();
      positiveTracks.clear();
   }

   virtual void Reset()
   {
      isElectronLike = false;
      isMuonLike = false;
      isAntiMuonLike = false;
      isProtonLike = false;
      entersBarrel = false;
      entersDownstream = false;
      downstreamTrack = nullptr;
      barrelTrack = nullptr;
      selectedTrack = nullptr;
      negativeTracks.clear();
      positiveTracks.clear();
   }
  
   virtual ~ToyBoxTPCECal(){ }

   /// Describes whether this track is thought to be an electron
   bool isElectronLike;
   /// Describes whether this track is thought to be a muon
   bool isMuonLike;
   /// Describes whether this track is thought to be an antimuon
   bool isAntiMuonLike;
   /// Describes whether this track is thought to be a proton
   bool isProtonLike;
   /// Describes whether this track appears to enter the barrel ECal
   bool entersBarrel;
   /// Describes whether this track appears to enter the downstream ECal
   bool entersDownstream;

   /// All negative tracks
   std::list<AnaTrackB*> negativeTracks;
   /// All positive tracks
   std::list<AnaTrackB*> positiveTracks;

   /// Track appearing to enter the downstream ECal.
   AnaTrackB* downstreamTrack;
   /// Track appearing to enter the barrel ECal.
   AnaTrackB* barrelTrack;
   /// The selected track.
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

class TPC
{
public:
   static const int MinimumNodes = 19;
};

///---- Define all steps -------
class NegativeMultiplicityCut: public StepBase{
 public:
  using StepBase::Apply;
  bool Apply(AnaEventB& event, ToyBoxB& box) const;
  StepBase* MakeClone(){return new NegativeMultiplicityCut();}
};

class PositiveMultiplicityCut: public StepBase{
 public:
  using StepBase::Apply;
  bool Apply(AnaEventB& event, ToyBoxB& box) const;
  StepBase* MakeClone(){return new PositiveMultiplicityCut();}
};

/// Negative leading tracks with good quality in FGD1
class FindNegativeLeadingTracksAction: public StepBase{
 public:
  using StepBase::Apply;
  bool Apply(AnaEventB& event, ToyBoxB& box) const;
  StepBase* MakeClone(){return new FindNegativeLeadingTracksAction();}
};

/// Highest momentum negative track
class FindHighestMomentumNegativeTrackAction: public StepBase{
 public:
  using StepBase::Apply;
  bool Apply(AnaEventB& event, ToyBoxB& box) const;
  StepBase* MakeClone(){return new FindHighestMomentumNegativeTrackAction();}
};

/// Highest momentum positive track
class FindHighestMomentumPositiveTrackAction: public StepBase{
 public:
  using StepBase::Apply;
  bool Apply(AnaEventB& event, ToyBoxB& box) const;
  StepBase* MakeClone(){return new FindHighestMomentumPositiveTrackAction();}
};

/// Positive leading tracks with good quality in FGD1
class FindPositiveLeadingTracksAction: public StepBase{
 public:
  using StepBase::Apply;
  bool Apply(AnaEventB& event, ToyBoxB& box) const;
  StepBase* MakeClone(){return new FindPositiveLeadingTracksAction();}
};

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

class TPCTrackQualityCut: public StepBase{
 public:
  using StepBase::Apply;
  bool Apply(AnaEventB& event, ToyBoxB& box) const;
  StepBase* MakeClone(){return new TPCTrackQualityCut();}
};

class TotalMultiplicityCut: public StepBase{
 public:
  using StepBase::Apply;
  bool Apply(AnaEventB& event, ToyBoxB& box) const;
  StepBase* MakeClone(){return new TotalMultiplicityCut();}
};

class ExternalVetoCut: public StepBase{
 public:
  using StepBase::Apply;
  bool Apply(AnaEventB& event, ToyBoxB& box) const;
  StepBase* MakeClone(){return new ExternalVetoCut();}
};

class ExternalFGD1lastlayersCut: public StepBase{
 public:
  using StepBase::Apply;
  bool Apply(AnaEventB& event, ToyBoxB& box) const;
  StepBase* MakeClone(){return new ExternalFGD1lastlayersCut();}
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

class SelectTrackAction: public StepBase
{
   public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new SelectTrackAction(); }
};

#endif
