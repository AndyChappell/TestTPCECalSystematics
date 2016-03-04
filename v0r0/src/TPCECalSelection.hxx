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
   }
  
   virtual ~ToyBoxTPCECal()
   {
   }

   /// Track with TPC that start in the FGD FV.
   std::vector<AnaTrackB*> fgdFVTracks;
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

//---- Define all steps -------
class FillSummaryAction_TPCECal: public StepBase
{
public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;  
   StepBase* MakeClone(){ return new FillSummaryAction_TPCECal(); }
};

/// Leading tracks with good quality in FGD
class FindLeadingTracksAction: public StepBase{
 public:
  using StepBase::Apply;
  bool Apply(AnaEventB& event, ToyBoxB& box) const;
  StepBase* MakeClone(){return new FindLeadingTracksAction();}
};

/// Tracks starting in the FGD fiducial volume
class FindTracksFGDFVAction: public StepBase
{
   public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new FindTracksFGDFVAction(); }
};

class LeadingTracksCut: public StepBase
{
   public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new LeadingTracksCut(); }
};

class FGDFVTracksCut: public StepBase
{
   public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new FGDFVTracksCut(); }
};

#endif
