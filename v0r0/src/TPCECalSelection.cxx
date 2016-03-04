#include "TPCECalSelection.hxx"
#include "baseSelection.hxx"
#include "CutUtils.hxx"
#include "Parameters.hxx"
#include "SubDetId.hxx"
#include "EventBoxUtils.hxx"

TPCECalSelection::TPCECalSelection(bool forceBreak):
   SelectionBase(forceBreak)
{
}

void TPCECalSelection::DefineSteps()
{
   // Add an action to the trunk
   AddStep(StepBase::kCut, "Event Quality", new EventQualityCut(), true);
   AddStep(StepBase::kAction, "Find Leading Tracks",
      new FindLeadingTracksAction());
   AddStep(StepBase::kAction, "Find FGD FV", new FindTracksFGDFVAction());
   AddStep(StepBase::kAction, "Fill Summary",
      new FillSummaryAction_TPCECal());
   AddStep(StepBase::kCut,  "TPC Tracks Cut", new LeadingTracksCut());
   AddStep(StepBase::kCut,  "FGD FV Cut", new FGDFVTracksCut());

   // Add a split to the trunk with 2 branches.
   //AddSplit(2);

   // First branch is for Barrel ECal events.
   //AddStep(0, StepBase::kCut, "1-track", new  OneTPCTrackCut());

   // Second branch is for Downstream ECal events.
   //AddStep(1, StepBase::kCut, "2-tracks", new TwoTPCTracksCut());

   // Set the branch aliases to the two branches (this is mandatory)
   // -Branch number: one split => one number
   // -Name
   // -Alias number: to be used to refer to a branch in DrawingTools
   SetBranchAlias(0, "Barrel", 0);
   //SetBranchAlias(1, "Downstream", 1);

   // By default the preselection correspond to cuts 0-2. 
   // It means that if any of the three first cuts (0,1,2) is not passed 
   // the loop over toys will be broken ===> A single toy will be run
   SetPreSelectionAccumLevel(2);
}

void TPCECalSelection::DefineDetectorFV()
{
   // Set the detector Fiducial Volume in which the selection is applied.
   // To specify different fiduical volumes for different branches use
   // SetDetectorFV(SubDetId::<Detector>, branch).
   SetDetectorFV(SubDetId::kFGD);
}

void TPCECalSelection::InitializeEvent(AnaEventB& event)
{
   // Create the appropriate EventBox if it does not exist yet
   if (!event.EventBoxes[AnaEventB::kEventBoxTracker])
   {
      event.EventBoxes[AnaEventB::kEventBoxTracker] = new EventBoxTracker();
   }

   boxUtils::FillTracksWithTPC(event, GetDetectorFV());
   boxUtils::FillTracksWithFGD(event, GetDetectorFV());
   boxUtils::FillTrajsChargedInTPC(event);
   boxUtils::FillTrajsChargedInFGDAndNoTPC(event, GetDetectorFV());
}

bool TPCECalSelection::FillEventSummary(AnaEventB& event, Int_t allCutsPassed[])
{
   (void)event;
   (void)allCutsPassed;

   return true;
}


bool FillSummaryAction_TPCECal::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void)event;
   (void)box;

   return true;
}

bool FindLeadingTracksAction::Apply(AnaEventB& event, ToyBoxB& box) const
{
   // Find leading tracks with good quality and only in FGD FV
   cutUtils::FindLeadingTracks(event, box);

   return true;
}

bool FindTracksFGDFVAction::Apply(AnaEventB& event, ToyBoxB& box) const
{
   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   for (int i = 0; i < tpcECalBox->nPositiveTPCtracks; i++)
   {
      if (cutUtils::FiducialCut(*(tpcECalBox->PositiveTPCtracks[i]), SubDetId::kFGD))
      {
         tpcECalBox->fgdFVTracks.push_back(tpcECalBox->PositiveTPCtracks[i]);
      }
   }
   for (int i = 0; i < tpcECalBox->nNegativeTPCtracks; i++)
   {
      if (cutUtils::FiducialCut(*(tpcECalBox->NegativeTPCtracks[i]), SubDetId::kFGD))
      {
         tpcECalBox->fgdFVTracks.push_back(tpcECalBox->NegativeTPCtracks[i]);
      }
   }

   return true;
}

bool LeadingTracksCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void) box;

   // Check we have at least one reconstructed track in the TPC
   EventBoxB* EventBox = event.EventBoxes[AnaEventB::kEventBoxTracker];

   return (EventBox->nTracksInGroup[EventBoxTracker::kTracksWithTPC] > 0);
}

bool FGDFVTracksCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void) event;

   ToyBoxTPCECal *tpcECalbox = static_cast<ToyBoxTPCECal*>(&box);

   return (tpcECalbox->fgdFVTracks.size() > 0);
}
