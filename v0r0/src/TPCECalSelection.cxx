#include "TPCECalSelection.hxx"
#include "baseSelection.hxx"
#include "CutUtils.hxx"
#include "Parameters.hxx"
#include "SubDetId.hxx"
#include "EventBoxUtils.hxx"


#include "nd280Samples.hxx"
#include "BasicUtils.hxx"
#include "baseAnalysis.hxx"
#include "VersioningUtils.hxx"
#include "SystematicUtils.hxx"

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
   AddStep(StepBase::kCut,  "TPC Tracks Cut", new LeadingTracksCut());

   AddStep(StepBase::kAction, "Find TPC Quality Tracks",
      new FindTPCQualityAction());
   AddStep(StepBase::kCut,  "TPC Quality Cut", new TPCQualityCut());
   
   AddStep(StepBase::kAction, "Find FGD FV", new FindTracksFGDFVAction());
   AddStep(StepBase::kCut,  "FGD FV Cut", new FGDFVTracksCut());

   // This will need a branch assignment when DS tracks are added
   AddStep(StepBase::kAction, "Find Barrel tracks",
      new FindBarrelECalTracksAction());
   AddStep(StepBase::kCut,  "Barrel Cut", new BarrelECalTracksCut());

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

bool LeadingTracksCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void) box;

   // Check we have at least one reconstructed track in the TPC
   EventBoxB* EventBox = event.EventBoxes[AnaEventB::kEventBoxTracker];

   return (EventBox->nTracksInGroup[EventBoxTracker::kTracksWithTPC] > 0);
}

bool FindTracksFGDFVAction::Apply(AnaEventB& event, ToyBoxB& box) const
{
   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   for (unsigned int i = 0; i < tpcECalBox->tpcQualityTracks.size(); i++)
   {
      AnaTrackB* track = tpcECalBox->tpcQualityTracks[i];
      if (cutUtils::FiducialCut(*track, SubDetId::kFGD))
      {
         tpcECalBox->fgdFVTracks.push_back(track);
      }
   }

   return true;
}

bool FGDFVTracksCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void) event;

   ToyBoxTPCECal *tpcECalbox = static_cast<ToyBoxTPCECal*>(&box);

   return (tpcECalbox->fgdFVTracks.size() > 0);
}

bool FindTPCQualityAction::Apply(AnaEventB& event, ToyBoxB& box) const
{
   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   for (int i = 0; i < tpcECalBox->nPositiveTPCtracks; i++)
   {
      AnaTrackB* track = tpcECalBox->PositiveTPCtracks[i];
      AnaTpcTrackB* backTPCTrack = anaUtils::GetTPCBackSegment(track);
      if (cutUtils::TPCTrackQualityCut(*backTPCTrack))
      {
         tpcECalBox->tpcQualityTracks.push_back(track);
      }
   }
   for (int i = 0; i < tpcECalBox->nNegativeTPCtracks; i++)
   {
      AnaTrackB* track = tpcECalBox->NegativeTPCtracks[i];
      AnaTpcTrackB* backTPCTrack = anaUtils::GetTPCBackSegment(track);
      if (cutUtils::TPCTrackQualityCut(*backTPCTrack))
      {
         tpcECalBox->tpcQualityTracks.push_back(track);
      }
   }

   return true;
}

bool TPCQualityCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void) event;

   ToyBoxTPCECal *tpcECalbox = static_cast<ToyBoxTPCECal*>(&box);

   return (tpcECalbox->tpcQualityTracks.size() > 0);
}

bool FindBarrelECalTracksAction::Apply(AnaEventB& event, ToyBoxB& box) const
{
   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   for (unsigned int i = 0; i < tpcECalBox->fgdFVTracks.size(); i++)
   {
      AnaTrackB* track = tpcECalBox->fgdFVTracks[i];
      TVector3 posEnd = utils::ArrayToTVector3(track->PositionEnd);
      TVector3 dirEnd = utils::ArrayToTVector3(track->DirectionEnd);
      
      if (std::isnan(posEnd.X()) || std::isnan(dirEnd.X()) ||
         !(((posEnd.X() >= Barrel::TpcXMin) && (posEnd.X() <= Barrel::TpcXMax)) &&
         ((posEnd.Y() >= Barrel::TpcYMin) && (posEnd.Y() <= Barrel::TpcYMax))) &&
         ((posEnd.Z() >= Barrel::TpcZMin) && (posEnd.Z() <= Barrel::TpcZMax)))
      {
         continue;
      }

      TVector3 zdir(0.0, 0.0, 1.0);
      double trackAzimuth = TMath::RadToDeg() *
         TMath::ATan(dirEnd.X() / dirEnd.Y());

      if (dirEnd.X() > 0 && dirEnd.Y() < 0)
      {
         trackAzimuth += 180;
      }
      else if (dirEnd.X() < 0 && dirEnd.Y() < 0)
      {
         trackAzimuth -= 180;
      }

      double trackAngleFromZ = TMath::RadToDeg() * TMath::ACos(
         dirEnd.Unit().Dot(zdir));

      if(!((trackAngleFromZ >= Barrel::TpcAngleMin) &&
         (fabs(trackAzimuth) <= Barrel::TpcAzimuthAbs)))
      {
         continue;
      }

      tpcECalBox->barrelTracks.push_back(track);
   }

   return true;
}

bool BarrelECalTracksCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void) event;

   ToyBoxTPCECal *tpcECalbox = static_cast<ToyBoxTPCECal*>(&box);

   return (tpcECalbox->barrelTracks.size() > 0);
}
