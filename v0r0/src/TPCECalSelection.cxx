#include "TPCECalSelection.hxx"
#include "baseSelection.hxx"
#include "CutUtils.hxx"
#include "SubDetId.hxx"
#include "EventBoxUtils.hxx"
#include "baseAnalysis.hxx"

//********************************************************************
TPCECalSelection::TPCECalSelection(bool forceBreak): SelectionBase(forceBreak) {
//********************************************************************

}

//********************************************************************
void TPCECalSelection::DefineDetectorFV(){
//********************************************************************

  // Set the detector Fiducial Volume in which the selection is applied. This is now a mandatory method
  SetDetectorFV(SubDetId::kFGD1);
}

//********************************************************************
void TPCECalSelection::DefineSteps(){
//********************************************************************

   // Cuts must be added in the right order
   // last "true" means the step sequence is broken if cut is not passed (default is "false")
   AddStep(StepBase::kCut, "event quality", new EventQualityCut(), true);
   AddStep(StepBase::kCut, "> 0 tracks ", new TotalMultiplicityCut(), true);  
   AddStep(StepBase::kAction, "find leading tracks",
      new FindLeadingTracksAction());
   AddStep(StepBase::kAction, "find FGD FV tracks",
      new FindFGDFVTracksAction());
   AddStep(StepBase::kCut, "FGD FV tracks cut", new FGDFVTracksCut());
   AddStep(StepBase::kAction, "Find downstream tracks",
      new FindDownstreamTracksAction());
   AddStep(StepBase::kAction, "Find barrel tracks",
      new FindBarrelTracksAction());
   AddStep(StepBase::kAction, "Select the track", new SelectTrackAction());
   AddStep(StepBase::kAction, "Find muon PID", new FindMuonPIDAction());
   AddStep(StepBase::kAction, "Find proton PID", new FindProtonPIDAction());

   // Add a split to the trunk with 2 branches.
   AddSplit(2);

   // First branch is for downstream ECal tracks
   AddStep(0, StepBase::kCut, "Downstream tracks cut", new DownstreamTracksCut());

   // Second branch is for barrel ECal tracks
   AddStep(1, StepBase::kCut, "Barrel tracks cut", new BarrelTracksCut());  

   // Set the branch aliases to the two branches (this is mandatory)
   SetBranchAlias(0, "Downstream ECal", 0);
   SetBranchAlias(1, "Barrel ECal", 1);

   // By default the preselection correspond to cuts 0-2. 
   // It means that if any of the three first cuts (0,1,2) is not passed 
   // the loop over toys will be broken ===> A single toy will be run
   SetPreSelectionAccumLevel(2);
}

//********************************************************************
bool TPCECalSelection::FillEventSummary(AnaEventB& event, Int_t allCutsPassed[]){
//********************************************************************

    if(allCutsPassed[0]){
        event.Summary->EventSample = nd280Samples::kFGD1NuMuCC;
    }
    return (event.Summary->EventSample != nd280Samples::kUnassigned);
}

//**************************************************
bool TotalMultiplicityCut::Apply(AnaEventB& event, ToyBoxB& box) const{
//**************************************************

  (void)box;
  // Check we have at least one reconstructed track in the TPC
  EventBoxB* EventBox = event.EventBoxes[AnaEventB::kEventBoxTracker];
  return (EventBox->nTracksInGroup[EventBoxTracker::kTracksWithTPC]>0);
}

//**************************************************
bool TrackQualityFiducialCut::Apply(AnaEventB& event, ToyBoxB& box) const{
//**************************************************

  (void)event;
  return (box.Vertex);
}

bool FindMuonPIDAction::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void)event;

   ToyBoxTPCECal* tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);
   if(!tpcECalBox->selectedTrack)
   {
      tpcECalBox->isMuonLike = false;
   }
   else if(tpcECalBox->selectedTrack->Momentum < 0)
   {
      tpcECalBox->isMuonLike = false;
   }
   else
   {
      tpcECalBox->isMuonLike = cutUtils::MuonPIDCut(
         *(tpcECalBox->selectedTrack), _prod5Cut);
   }

   return true;
}

bool FindProtonPIDAction::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void)event;

   ToyBoxTPCECal* tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);
   if(!tpcECalBox->selectedTrack)
   {
      tpcECalBox->isProtonLike = false;
   }
   else if(tpcECalBox->selectedTrack->Momentum < 0)
   {
      tpcECalBox->isProtonLike = false;
   }
   else
   {
      tpcECalBox->isProtonLike = cutUtils::ProtonPIDCut(
         *(tpcECalBox->selectedTrack));
   }

   return true;
}

//********************************************************************
bool ExternalVetoCut::Apply(AnaEventB& event, ToyBoxB& box) const{
//********************************************************************

    (void)event;
    if (!box.MainTrack) return false;
    return cutUtils::ExternalVetoCut(*(box.MainTrack),box.VetoTrack);
}

//********************************************************************
bool ExternalFGD1lastlayersCut::Apply(AnaEventB& event, ToyBoxB& box) const{
//********************************************************************

    (void)event;
    /// Reject external background from the last 2 layers of FGD1
    if (!box.MainTrack) return false;  
    if (!box.OOFVtrack) return true;
    return (box.MainTrack->PositionStart[2]<425.);
}

//**************************************************
bool FindVertexAction::Apply(AnaEventB& event, ToyBoxB& box) const{
//**************************************************

    (void)event;

    // reset the vertex 
    box.Vertex = NULL;

    if (!box.MainTrack) return false;

    box.Vertex = new AnaVertexB();
    utils::CreateArray(box.Vertex->Tracks, 1);

    box.Vertex->nTracks = 0;
    box.Vertex->Tracks[box.Vertex->nTracks++] = box.MainTrack;

    for(int i = 0; i < 4; ++i){
        box.Vertex->Position[i] = box.MainTrack->PositionStart[i];
    }
    if(  box.MainTrack->TrueTrack ) 
        box.Vertex->TrueVertex = box.MainTrack->TrueTrack->TrueVertex;
    return true;
}

//********************************************************************
bool FillSummaryAction_TPCECal::Apply(AnaEventB& event, ToyBoxB& box) const{
//********************************************************************
    if(!box.HMNtrack) return 1;

    event.Summary->LeptonCandidate[nd280Samples::kFGD1NuMuCC] = box.HMNtrack;
    for(int i = 0; i < 4; ++i){
        event.Summary->VertexPosition[nd280Samples::kFGD1NuMuCC][i] = box.HMNtrack->PositionStart[i];
    }
    if(box.HMNtrack->TrueTrack) event.Summary->TrueVertex[nd280Samples::kFGD1NuMuCC] = box.HMNtrack->TrueTrack->TrueVertex;

    return true;
}

//**************************************************
bool FindLeadingTracksAction::Apply(AnaEventB& event, ToyBoxB& box) const{
//**************************************************

    // Find leading tracks with good quality and only in FGD FV
    cutUtils::FindLeadingTracks(event,box);

    // For this selection the main track is the HMN track
    box.MainTrack = box.HMNtrack;
    return true;
}

//**************************************************
bool FindVetoTrackAction::Apply(AnaEventB& event, ToyBoxB& box) const{
//**************************************************

    box.VetoTrack = cutUtils::FindVetoTrack(event, *(box.MainTrack));  
    return true;
}

//**************************************************
bool FindOOFVTrackAction::Apply(AnaEventB& event, ToyBoxB& box) const{
//**************************************************

    box.OOFVtrack = cutUtils::FindFGDOOFVtrack(event, *(box.MainTrack), box.DetectorFV);
    return true;
}

//*********************************************************************
bool OneTPCTrackCut::Apply(AnaEventB& event, ToyBoxB& box) const{
//*********************************************************************

    (void)event;

    if (box.nNegativeTPCtracks+box.nPositiveTPCtracks==1)
      return true;
    else
      return false; 
}


//*********************************************************************
bool TwoTPCTracksCut::Apply(AnaEventB& event, ToyBoxB& box) const{
//*********************************************************************

    (void)event;

    if (box.nNegativeTPCtracks+box.nPositiveTPCtracks==2)
      return true;
    else
      return false; 
}

//**************************************************
void TPCECalSelection::InitializeEvent(AnaEventB& event){
//**************************************************

  // Create the appropriate EventBox if it does not exist yet
  if (!event.EventBoxes[AnaEventB::kEventBoxTracker])
    event.EventBoxes[AnaEventB::kEventBoxTracker] = new EventBoxTracker();
  
  boxUtils::FillTracksWithTPC(event,             GetDetectorFV());
  boxUtils::FillTracksWithFGD(event,             GetDetectorFV());
  boxUtils::FillTrajsChargedInTPC(event);
  boxUtils::FillTrajsChargedInFGDAndNoTPC(event, GetDetectorFV());
}

bool FindFGDFVTracksAction::Apply(AnaEventB& event, ToyBoxB& box) const
{
   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   for (int i = 0; i < tpcECalBox->nNegativeTPCtracks; i++)
   {
      AnaTrackB* track = tpcECalBox->NegativeTPCtracks[i];
      if (cutUtils::FiducialCut(*track, SubDetId::kFGD))
      {
         tpcECalBox->fgdFVTracks.push_back(track);
      }
   }
   for (int i = 0; i < tpcECalBox->nPositiveTPCtracks; i++)
   {
      AnaTrackB* track = tpcECalBox->PositiveTPCtracks[i];
      if (cutUtils::FiducialCut(*track, SubDetId::kFGD))
      {
         tpcECalBox->fgdFVTracks.push_back(track);
      }
   }

   return true;
}

bool FGDFVTracksCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   return tpcECalBox->fgdFVTracks.size() > 0;
}

bool FindDownstreamTracksAction::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void) event;

   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   std::vector<AnaTrackB*> tracks = tpcECalBox->fgdFVTracks;

   for (unsigned int i = 0; i < tracks.size(); i++)
   {
      AnaTrackB* track = tracks[i];
      AnaTpcTrack* backTpc = static_cast<AnaTpcTrack*>(
         anaUtils::GetTPCBackSegment(track));
      TVector3 pos = utils::ArrayToTVector3(backTpc->PositionEnd);

      if (!cutUtils::TPCTrackQualityCut(*backTpc))
      {
         continue;
      }
      if(std::isnan(pos.X()) || !(
         (pos.X() >= DS::TpcXMin && pos.X() <= DS::TpcXMax) &&
         (pos.Y() >= DS::TpcYMin && pos.Y() <= DS::TpcYMax) &&
         (pos.Z() >= DS::TpcZMin)))
      {
         continue;
      }

      TVector3 dir = utils::ArrayToTVector3(backTpc->DirectionEnd);
      TVector3 zdir(0.0, 0.0, 1.0);
      double tpcBackAngleFromZ = TMath::RadToDeg() * dir.Unit().Angle(zdir);
      if(std::isnan(dir.X()) || !(tpcBackAngleFromZ <= DS::TpcAngleMax))
      {
         continue;
      }

      tpcECalBox->downstreamTracks.push_back(track);
   }

   return true;
}

bool DownstreamTracksCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void) event;

   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   return tpcECalBox->downstreamTracks.size() > 0;
}

bool FindBarrelTracksAction::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void) event;

   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   std::vector<AnaTrackB*> tracks = tpcECalBox->fgdFVTracks;

   for (unsigned int i = 0; i < tracks.size(); i++)
   {
      AnaTrackB* track = tracks[i];
      AnaTpcTrack* backTpc = static_cast<AnaTpcTrack*>(
         anaUtils::GetTPCBackSegment(track));
      TVector3 pos = utils::ArrayToTVector3(backTpc->PositionEnd);

      if (!cutUtils::TPCTrackQualityCut(*backTpc))
      {
         continue;
      }
      if(std::isnan(pos.X()) || !(
         ((pos.X() < Barrel::TpcXMin) || (pos.X() > Barrel::TpcXMax) ||
         (pos.Y() < Barrel::TpcYMin) || (pos.Y() > Barrel::TpcYMax)) &&
         (pos.Z() >= Barrel::TpcZMin && pos.Z() <= Barrel::TpcZMax)))
      {
         continue;
      }

      TVector3 dir = utils::ArrayToTVector3(backTpc->DirectionEnd);
      TVector3 zdir(0.0, 0.0, 1.0);
      double tpcBackAzimuth = TMath::RadToDeg() * TMath::ATan(dir.X() / dir.Y());

      if(dir.X() > 0 && dir.Y() < 0)
      {
         tpcBackAzimuth += 180;
      }
      else if(dir.X() < 0 && dir.Y() < 0)
      {
         tpcBackAzimuth -= 180;
      }

      double tpcBackAngleFromZ = dir.Unit().Dot(zdir);
      tpcBackAngleFromZ = TMath::RadToDeg() * TMath::ACos(tpcBackAngleFromZ);

      if(std::isnan(dir.X()) || !(
         (tpcBackAngleFromZ >= Barrel::TpcAngleMin) &&
         (fabs(tpcBackAzimuth) <= Barrel::TpcAzimuthAbs)))
      {
         continue;
      }

      tpcECalBox->barrelTracks.push_back(track);
   }

   return true;
}

bool BarrelTracksCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void) event;

   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   return tpcECalBox->barrelTracks.size();
}

bool SelectTrackAction::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void) event;

   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   if(tpcECalBox->barrelTracks.size() > 0)
   {
      tpcECalBox->entersBarrel = true;

      Float_t highestMomentum = 0;
      for(unsigned int i = 0; i < tpcECalBox->barrelTracks.size(); i++)
      {
         AnaTrackB* track = tpcECalBox->barrelTracks[i];
         if(track->Momentum > highestMomentum)
         {
            highestMomentum = track->Momentum;
            tpcECalBox->selectedTrack = track;
         }
      }
   }
   else if(tpcECalBox->downstreamTracks.size() > 0)
   {
      tpcECalBox->entersDownstream = true;

      Float_t highestMomentum = 0;
      for(unsigned int i = 0; i < tpcECalBox->downstreamTracks.size(); i++)
      {
         AnaTrackB* track = tpcECalBox->downstreamTracks[i];
         if(track->Momentum > highestMomentum)
         {
            highestMomentum = track->Momentum;
            tpcECalBox->selectedTrack = track;
         }
      }
   }

   return true;
}
