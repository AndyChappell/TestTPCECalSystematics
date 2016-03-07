#ifndef TPCECalSelection_h
#define TPCECalSelection_h

#include "SelectionBase.hxx"
#include "SubDetId.hxx"

/**
   The ToyBox for this selection
 */
class ToyBoxTPCECal: public ToyBoxB
{
public:
   ToyBoxTPCECal()
   {
      // Initialize any variables for the selection.
   }

   /**
      Resets the ToyBox variables.
    */
   virtual void Reset()
   {
      fgdFVTracks.clear();
      tpcQualityTracks.clear();
      barrelTracks.clear();
      downstreamTracks.clear();
   }
  
   virtual ~ToyBoxTPCECal()
   {
   }

   /// Tracks with more than 18 TPC nodes.
   std::vector<AnaTrackB*> tpcQualityTracks;

   /// Tracks with TPC that start in the FGD FV.
   std::vector<AnaTrackB*> fgdFVTracks;

   /// Tracks which appear to enter the Barrel ECal.
   std::vector<AnaTrackB*> barrelTracks;

   /// Tracks which appear to enter the Downstream ECal.
   std::vector<AnaTrackB*> downstreamTracks;
};

/**
   The TPC/ECal selection
 */
class TPCECalSelection: public SelectionBase
{
public:
   /**
      Constructs a TPCECalSelection object.
      \param forceBreak Whether or not to cut the sequence when a cut is not
                        passed.
    */
   TPCECalSelection(bool forceBreak=true);
   virtual ~TPCECalSelection(){}

   //---- These are mandatory functions ----

   /**
      Defines the steps to be added to this selection.
    */
   void DefineSteps();

   /**
      Set the detector fiducial volume in which the selection is applied.
      
      If different branches use different fiducial volumes the branch can be
      specified via SetDetectorFV(SubDetId::kFGD1, branch).
    */
   void DefineDetectorFV();

   /**
      Creates a proper instance of the box (ToyBoxB) to store all relevent
      information to be passed from one step to the next.
    */
   ToyBoxB* MakeToyBox() { return new ToyBoxTPCECal(); }

   /**
      Fills the EventBox with the objects needed by this selection.

      \param event The event from which the event box will be filled
    */
   void InitializeEvent(AnaEventB& event);

   //----

   //---- These are optional functions, needed by fitters but not by highland2
   // analyses --------------

   /**
      Fill the event summary information, which is needed by the fitters.
    */
   bool FillEventSummary(AnaEventB& event, Int_t allCutsPassed[]);

   /**
      Return the appropriate sample type (only needed by fitters)
    */
   nd280Samples::SampleEnum GetSampleEnum()
   {
      return nd280Samples::kFGD1NuMuCC0Pi;
   }

   //---- These are optional functions, but recommended to customize systematics
   // and increase speed --------------
   bool IsRelevantTrackForSystematic(const AnaEventB&, AnaTrackB*,
      Int_t syst_index, Int_t branch=0) const
   {
      (void)syst_index;
      (void)branch;

      return true;
   }
   
   bool IsRelevantTrueTrackForSystematic(const AnaEventB&, AnaTrueTrackB*,
      Int_t syst_index, Int_t branch=0) const
   {
      (void)syst_index;
      (void)branch;

      return true;
   }

   bool IsRelevantTrackForSystematicInToy(const AnaEventB&, const ToyBoxB&,
      AnaTrackB*, Int_t syst_index, Int_t branch=0) const
   {
      (void)syst_index;
      (void)branch;

      return true;
   }

   bool IsRelevantTrueTrackForSystematicInToy(const AnaEventB&, const ToyBoxB&,
      AnaTrueTrackB*, Int_t syst_index, Int_t branch=0) const
   {
      (void)syst_index;
      (void)branch;

      return true;
   }

   bool IsRelevantSystematic(const AnaEventB&, const ToyBoxB&, Int_t syst_index,
      Int_t branch=0) const
   {
      (void)syst_index;
      (void)branch;

      return true;
   }

   bool CheckRedoSelection(const AnaEventB&, const ToyBoxB& PreviousToyBox,
      Int_t& redoFromStep)
   {
      (void)PreviousToyBox;
      redoFromStep=0;

      return true;
   }
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

//---- Define all steps -------
class FillSummaryAction_TPCECal: public StepBase
{
public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;  
   StepBase* MakeClone(){ return new FillSummaryAction_TPCECal(); }
};

/// Finds leading tracks with good quality in FGD
class FindLeadingTracksAction: public StepBase{
 public:
  using StepBase::Apply;
  bool Apply(AnaEventB& event, ToyBoxB& box) const;
  StepBase* MakeClone(){return new FindLeadingTracksAction();}
};

/// Selects leading tracks with good quality in FGD
class LeadingTracksCut: public StepBase
{
   public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new LeadingTracksCut(); }
};

/// Finds tracks starting in the FGD fiducial volume
class FindTracksFGDFVAction: public StepBase
{
   public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new FindTracksFGDFVAction(); }
};

/// Selects tracks starting in the FGD fiducial volume
class FGDFVTracksCut: public StepBase
{
   public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new FGDFVTracksCut(); }
};

/// Finds tracks with at least 18 nodes in the most downstream TPC
class FindTPCQualityAction: public StepBase
{
   public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new FindTPCQualityAction(); }
};

/// Selects tracks with at least 18 nodes in the most downstream TPC
class TPCQualityCut: public StepBase
{
   public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new TPCQualityCut(); }
};

/// Finds tracks that appear to enter the barrel ECal
class FindBarrelECalTracksAction: public StepBase
{
   public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new FindBarrelECalTracksAction(); }
};

/// Selects tracks that appear to enter the barrel ECal
class BarrelECalTracksCut: public StepBase
{
   public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new BarrelECalTracksCut(); }
};

/// Finds tracks that appear to enter the DS ECal
class FindDSECalTracksAction: public StepBase
{
   public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new FindDSECalTracksAction(); }
};

/// Selects tracks that appear to enter the DS ECal
class DSECalTracksCut: public StepBase
{
   public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new DSECalTracksCut(); }
};

#endif
