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
   AddStep(StepBase::kCut, "Evt Qual", new EventQualityCut(), true);
   AddStep(StepBase::kCut, "> 0 Tracks", new TotalMultiplicityCut(), true);  
   AddStep(StepBase::kAction, "Leading Tracks",
      new FindLeadingTracksAction());

   AddStep(StepBase::kAction, "Find Vertex", new FindVertexAction());  
   AddStep(StepBase::kCut, "Qual + Fid", new TrackQualityFiducialCut(), true);
   AddStep(StepBase::kCut, "Neg Mul", new NegativeMultiplicityCut());
   AddStep(StepBase::kAction, "Find Veto Track", new FindVetoTrackAction());
   AddStep(StepBase::kCut, "Veto", new ExternalVetoCut());
   AddStep(StepBase::kAction, "Find OOFV Track", new FindOOFVTrackAction());
   AddStep(StepBase::kCut, "External FGD1", new ExternalFGD1lastlayersCut());
      
/*   AddStep(StepBase::kAction, "Find FGD FV Track",
      new FindFGDFVTracksAction());
   AddStep(StepBase::kCut, "FGD FV Track", new FGDFVTracksCut());*/
   AddStep(StepBase::kAction, "Find DS Track",
      new FindDownstreamTracksAction());
   AddStep(StepBase::kAction, "Find Barrel Track",
      new FindBarrelTracksAction());
   AddStep(StepBase::kAction, "Select Track", new SelectTrackAction());
   AddStep(StepBase::kAction, "Find Muon PID", new FindMuonPIDAction());
   AddStep(StepBase::kAction, "Find Proton PID", new FindProtonPIDAction());

   // Add a split to the trunk with 2 branches.
   AddSplit(2);

   // First branch is for downstream ECal tracks
   AddStep(0, StepBase::kCut, "Muon PID", new MuonPIDCut());
   AddStep(0, StepBase::kCut, "DS Track", new DownstreamTracksCut());
   
   // Second branch is for barrel ECal tracks
   AddStep(1, StepBase::kCut, "Muon PID", new MuonPIDCut());
   AddStep(1, StepBase::kCut, "Barrel Track", new BarrelTracksCut());

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
bool NegativeMultiplicityCut::Apply(AnaEventB& event, ToyBoxB& box) const{
//**************************************************

  (void)event;

  return (box.HMNtrack==box.HMtrack);
}

//**************************************************
bool PositiveMultiplicityCut::Apply(AnaEventB& event, ToyBoxB& box) const{
//**************************************************

  (void)event;

  return (box.HMPtrack==box.HMtrack);
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
      AnaTpcTrack* backTpc = static_cast<AnaTpcTrack*>(
         anaUtils::GetTPCBackSegment(tpcECalBox->selectedTrack));

      Float_t pullElectron = backTpc->Pullele;
      Float_t pullMuon = backTpc->Pullmu;
      
      if((pullMuon > -2 && pullMuon < 2) &&
         (pullElectron > 2 || pullElectron < -1) &&
         (cutUtils::MuonPIDCut(*(tpcECalBox->selectedTrack), _prod5Cut) ||
         cutUtils::AntiMuonPIDCut(*(tpcECalBox->selectedTrack))))
      {
         tpcECalBox->isMuonLike = true;
      }
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

/*
bool FindFGDFVTracksAction::Apply(AnaEventB& event, ToyBoxB& box) const
{
   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   if(tpcECalBox->MainTrack)
   {
      if (cutUtils::FiducialCut(*tpcECalBox->MainTrack, SubDetId::kFGD))
      {
         tpcECalBox->fgdFVTracks.push_back(tpcECalBox->MainTrack);
      }
   }

   return true;
}

bool FGDFVTracksCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   return tpcECalBox->fgdFVTracks.size() > 0;
}
* */

bool FindDownstreamTracksAction::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void) event;

   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   if(tpcECalBox->MainTrack)
   {
      AnaTrackB* track = tpcECalBox->MainTrack;
      AnaTpcTrack* backTpc = static_cast<AnaTpcTrack*>(
         anaUtils::GetTPCBackSegment(track));
      TVector3 pos = utils::ArrayToTVector3(backTpc->PositionEnd);

      if (!cutUtils::TPCTrackQualityCut(*backTpc))
      {
         return true;
      }
      if(std::isnan(pos.X()) || !(
         (pos.X() >= DS::TpcXMin && pos.X() <= DS::TpcXMax) &&
         (pos.Y() >= DS::TpcYMin && pos.Y() <= DS::TpcYMax) &&
         (pos.Z() >= DS::TpcZMin)))
      {
         return true;
      }

      TVector3 dir = utils::ArrayToTVector3(backTpc->DirectionEnd);
      TVector3 zdir(0.0, 0.0, 1.0);
      double tpcBackAngleFromZ = TMath::RadToDeg() * dir.Unit().Angle(zdir);
      if(std::isnan(dir.X()) || !(tpcBackAngleFromZ <= DS::TpcAngleMax))
      {
         return true;
      }

      tpcECalBox->downstreamTrack = track;
   }

   return true;
}

bool DownstreamTracksCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void) event;

   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   return (tpcECalBox->downstreamTrack);
}

bool FindBarrelTracksAction::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void) event;

   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   if(tpcECalBox->MainTrack)
   {
      AnaTrackB* track = tpcECalBox->MainTrack;
      AnaTpcTrack* backTpc = static_cast<AnaTpcTrack*>(
         anaUtils::GetTPCBackSegment(track));
      TVector3 pos = utils::ArrayToTVector3(backTpc->PositionEnd);

      if (!cutUtils::TPCTrackQualityCut(*backTpc))
      {
         return true;
      }
      if(std::isnan(pos.X()) || !(
         ((pos.X() < Barrel::TpcXMin) || (pos.X() > Barrel::TpcXMax) ||
         (pos.Y() < Barrel::TpcYMin) || (pos.Y() > Barrel::TpcYMax)) &&
         (pos.Z() >= Barrel::TpcZMin && pos.Z() <= Barrel::TpcZMax)))
      {
         return true;
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
         return true;
      }

      tpcECalBox->barrelTrack = track;
   }

   return true;
}

bool BarrelTracksCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void) event;

   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   return tpcECalBox->barrelTrack;
}

bool SelectTrackAction::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void) event;

   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   if(tpcECalBox->barrelTrack)
   {
      tpcECalBox->entersBarrel = true;
      tpcECalBox->selectedTrack = tpcECalBox->barrelTrack;
   }
   else if(tpcECalBox->downstreamTrack)
   {
      tpcECalBox->entersDownstream = true;
      tpcECalBox->selectedTrack = tpcECalBox->downstreamTrack;
   }

   return true;
}

bool MuonPIDCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void) event;

   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   return tpcECalBox->isMuonLike;
}

