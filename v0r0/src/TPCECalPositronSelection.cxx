#include "TPCECalPositronSelection.hxx"
#include "baseSelection.hxx"
#include "CutUtils.hxx"
#include "SubDetId.hxx"
#include "EventBoxUtils.hxx"
#include "baseAnalysis.hxx"

TPCECalPositronSelection::TPCECalPositronSelection(bool forceBreak): SelectionBase(forceBreak)
{
}

void TPCECalPositronSelection::DefineDetectorFV()
{
   // Set the detector Fiducial Volume in which the selection is applied. This is now a mandatory method
   SetDetectorFV(SubDetId::kFGD);
}

bool TPCECalPositronSelection::FillEventSummary(AnaEventB& event, Int_t allCutsPassed[])
{
    return true;
}

void TPCECalPositronSelection::DefineSteps()
{
   // Cuts must be added in the right order
   // last "true" means the step sequence is broken if cut is not passed (default is "false")
   AddStep(StepBase::kCut, "Evt Qual", new EventQualityCut(), true);
   AddStep(StepBase::kCut, "FGD + TPC", new FGDTPCTracksCut(), true);
   AddStep(StepBase::kCut, "FGD FV", new FGDFVTracksCut(), true);
   AddStep(StepBase::kCut, ">1 track", new MultiplicityCut(2), true);
   AddStep(StepBase::kCut, "< 10 cm", new SeparationTracksCut(), true);
   AddStep(StepBase::kCut, "Opp Sign", new OppositeChargeTracksCut(), true);
   AddStep(StepBase::kCut, "Pos Partner", new PositivePartnerTracksCut(), true);
   AddStep(StepBase::kCut, "TPC Qual", new TPCTrackQualityCut(), true);
   AddStep(StepBase::kAction, "Find e+ PID", new FindPositronPIDAction());
   AddStep(StepBase::kCut, "e+ PID", new PositronPIDCut());
   AddStep(StepBase::kAction, "Find DS Track",
      new FindDownstreamTracksAction());
   AddStep(StepBase::kAction, "Find Barrel Track",
      new FindBarrelTracksAction());
   AddStep(StepBase::kAction, "Select Track", new SelectTrackAction());
   
   // Add a split to the trunk with 2 branches.
   AddSplit(2);

   AddStep(0, StepBase::kCut, "DS Track", new DownstreamTracksCut());
   AddStep(1, StepBase::kCut, "Barrel Track", new BarrelTracksCut());

   // Set the branch aliases to the two branches (this is mandatory)
   SetBranchAlias(0, "Downstream ECal", 0);
   SetBranchAlias(1, "Barrel ECal", 1);

   // By default the preselection correspond to cuts 0-2. 
   // It means that if any of the three first cuts (0,1,2) is not passed 
   // the loop over toys will be broken ===> A single toy will be run
   SetPreSelectionAccumLevel(2);
}

bool FindPositronPIDAction::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void)event;

   ToyBoxTPCECal* tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   std::list<AnaTrackB*>::iterator i = tpcECalBox->positiveTracks.begin();
   while(i != tpcECalBox->positiveTracks.end())
   {
      AnaTpcTrack* track = static_cast<AnaTpcTrack*>(
         anaUtils::GetTPCBackSegment(*i));

      Float_t pullElectron = track->Pullele;
      Float_t pullMuon = track->Pullmu;
      Float_t pullPion = track->Pullpi;
      Float_t pullProton = track->Pullp;
      
      if((track->Momentum > 0) &&
         (pullElectron > -1 && pullElectron < 2) &&
         (pullMuon < -2.5 || pullMuon > 2.5) &&
         (pullPion < -2 || pullPion > 2) &&
         (pullProton < -2.5 || pullProton > 2.5))
      {
         tpcECalBox->isPositronLike = true;
         i++;
      }
      else
      {
         tpcECalBox->positiveTracks.erase(i++);
      }
   }
   
   return true;
}

void TPCECalPositronSelection::InitializeEvent(AnaEventB& event)
{
  // Create the appropriate EventBox if it does not exist yet
  if (!event.EventBoxes[AnaEventB::kEventBoxTracker])
    event.EventBoxes[AnaEventB::kEventBoxTracker] = new EventBoxTracker();
  
  boxUtils::FillTracksWithTPC(event,             GetDetectorFV());
  boxUtils::FillTracksWithFGD(event,             GetDetectorFV());
  boxUtils::FillTrajsChargedInTPC(event);
  boxUtils::FillTrajsChargedInFGDAndNoTPC(event, GetDetectorFV());
}

bool PositronPIDCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void) event;

   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   return tpcECalBox->isPositronLike;
}
