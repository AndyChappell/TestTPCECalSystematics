#include <boost/fusion/iterator/next.hpp>
#include "TPCECalSelection.hxx"
#include "baseSelection.hxx"
#include "CutUtils.hxx"
#include "SubDetId.hxx"
#include "EventBoxUtils.hxx"
#include "baseAnalysis.hxx"

bool NegativeMultiplicityCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void)event;

   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);
   tpcECalBox->negativeTracks.clear();
   if(tpcECalBox->HMtrack && (tpcECalBox->HMNtrack == tpcECalBox->HMtrack))
   {
      tpcECalBox->negativeTracks.push_back(tpcECalBox->HMNtrack);
   }

   return tpcECalBox->negativeTracks.size() > 0;
}

bool PositiveMultiplicityCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void)event;

   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);
   tpcECalBox->positiveTracks.clear();
   if(tpcECalBox->HMtrack && (tpcECalBox->HMPtrack == tpcECalBox->HMtrack))
   {
      tpcECalBox->positiveTracks.push_back(tpcECalBox->HMPtrack);
   }

   return tpcECalBox->positiveTracks.size() > 0;
}

bool FindNegativeLeadingTracksAction::Apply(AnaEventB& event, ToyBoxB& box) const
{
   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);
   
   // Find leading tracks with good quality and only in FGD FV
   cutUtils::FindLeadingTracks(event, *tpcECalBox);

   for(int i = 0; i < tpcECalBox->nNegativeTPCtracks; ++i)
   {
      tpcECalBox->negativeTracks.push_back(tpcECalBox->NegativeTPCtracks[i]);
   }

   return tpcECalBox->negativeTracks.size() > 0;
}

bool FindPositiveLeadingTracksAction::Apply(AnaEventB& event, ToyBoxB& box) const
{
   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);
   
   // Find leading tracks with good quality and only in FGD FV
   cutUtils::FindLeadingTracks(event, *tpcECalBox);

   for(int i = 0; i < tpcECalBox->nPositiveTPCtracks; ++i)
   {
      tpcECalBox->positiveTracks.push_back(tpcECalBox->PositiveTPCtracks[i]);
   }

   return true;
}

bool FindHighestMomentumNegativeTrackAction::Apply(AnaEventB& event, ToyBoxB& box) const
{
   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   Float_t highestMomentum = 0;
   AnaTrackB* hmTrack = nullptr;
   std::list<AnaTrackB*>::iterator i = tpcECalBox->negativeTracks.begin();
   while(i != tpcECalBox->negativeTracks.end())
   {
      if((*i)->Momentum > highestMomentum)
      {
         highestMomentum = (*i)->Momentum;
         hmTrack = *i;
      }
      i++;
   }
   tpcECalBox->negativeTracks.clear();
   if(hmTrack)
   {
      tpcECalBox->negativeTracks.push_back(hmTrack);
   }

   return true;
}

bool FindHighestMomentumPositiveTrackAction::Apply(AnaEventB& event, ToyBoxB& box) const
{
   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   Float_t highestMomentum = 0;
   AnaTrackB* hmTrack = nullptr;
   std::list<AnaTrackB*>::iterator i = tpcECalBox->positiveTracks.begin();
   while(i != tpcECalBox->positiveTracks.end())
   {
      if((*i)->Momentum > highestMomentum)
      {
         highestMomentum = (*i)->Momentum;
         hmTrack = *i;
      }
      i++;
   }
   tpcECalBox->positiveTracks.clear();
   if(hmTrack)
   {
      tpcECalBox->positiveTracks.push_back(hmTrack);
   }

   return true;
}

bool TotalMultiplicityCut::Apply(AnaEventB& event, ToyBoxB& box) const{

   (void)box;
   // Check we have at least one reconstructed track in the TPC

   EventBoxB* EventBox = event.EventBoxes[AnaEventB::kEventBoxTracker];
   bool passes = (EventBox->nTracksInGroup[EventBoxTracker::kTracksWithTPC]>0);

   return passes;
}

bool MultiplicityCut::Apply(AnaEventB& event, ToyBoxB& box) const{

   (void)event;
  
   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   return (tpcECalBox->fgd1Tracks.size() >= minimumTracks ||
      tpcECalBox->fgd2Tracks.size() >= minimumTracks);
}

bool TPCTrackQualityCut::Apply(AnaEventB& event, ToyBoxB& box) const{

   (void)event;
  
   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);
   std::list<AnaTrackB*>::iterator i = tpcECalBox->negativeTracks.begin();
   while(i != tpcECalBox->negativeTracks.end())
   {
      AnaTpcTrack* track = static_cast<AnaTpcTrack*>(
         anaUtils::GetTPCBackSegment(*i));

      if(track->NHits < TPC::MinimumNodes)
      {
         tpcECalBox->negativeTracks.erase(i++);
      }
      else
      {
         i++;
      }
   }

   i = tpcECalBox->positiveTracks.begin();
   while(i != tpcECalBox->positiveTracks.end())
   {
      std::list<AnaTrackB*>::iterator j = tpcECalBox->positiveTracks.end();
      AnaTpcTrack* track = static_cast<AnaTpcTrack*>(
         anaUtils::GetTPCBackSegment(*i));

      if(track->NHits < TPC::MinimumNodes)
      {
         tpcECalBox->positiveTracks.erase(i++);
      }
      else
      {
         i++;
      }
   }

   return (tpcECalBox->negativeTracks.size() > 0 ||
      tpcECalBox->positiveTracks.size() > 0);
}

bool ExternalVetoCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);
   std::list<AnaTrackB*>::iterator i = tpcECalBox->negativeTracks.begin();
   while(i != tpcECalBox->negativeTracks.end())
   {
      AnaTrackB* track = *i;

      if(!cutUtils::ExternalVetoCut(*track,
         cutUtils::FindVetoTrack(event, *track)))
      {
         tpcECalBox->negativeTracks.erase(i++);
      }
      else
      {
         i++;
      }
   }

   i = tpcECalBox->positiveTracks.begin();
   while(i != tpcECalBox->positiveTracks.end())
   {
      AnaTrackB* track = *i;

      if(!cutUtils::ExternalVetoCut(*track,
         cutUtils::FindVetoTrack(event, *track)))
      {
         tpcECalBox->positiveTracks.erase(i++);
      }
      else
      {
         i++;
      }
   }

   return (tpcECalBox->negativeTracks.size() > 0 ||
      tpcECalBox->positiveTracks.size() > 0);
}

bool ExternalFGD1lastlayersCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);
   std::list<AnaTrackB*>::iterator i = tpcECalBox->negativeTracks.begin();
   while(i != tpcECalBox->negativeTracks.end())
   {
      AnaTrackB* track = *i;

      if(cutUtils::FindFGDOOFVtrack(event, *track, tpcECalBox->DetectorFV) ||
         !(track->PositionStart[2] < 425))
      {
         tpcECalBox->negativeTracks.erase(i++);
      }
      else
      {
         i++;
      }
   }

   i = tpcECalBox->positiveTracks.begin();
   while(i != tpcECalBox->positiveTracks.end())
   {
      AnaTrackB* track = *i;

      if(cutUtils::FindFGDOOFVtrack(event, *track, tpcECalBox->DetectorFV) ||
         !(track->PositionStart[2] < 425))
      {
         tpcECalBox->positiveTracks.erase(i++);
      }
      else
      {
         i++;
      }
   }

   return (tpcECalBox->negativeTracks.size() > 0 ||
      tpcECalBox->positiveTracks.size() > 0);
}

bool FindDownstreamTracksAction::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void) event;

   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   int count = 0;
   Float_t highestMomentum = 0;
   std::list<AnaTrackB*>::iterator i = tpcECalBox->negativeTracks.begin();
   while(i != tpcECalBox->negativeTracks.end())
   {
      AnaTrackB* track = *i;

      AnaTpcTrack* backTpc = static_cast<AnaTpcTrack*>(
         anaUtils::GetTPCBackSegment(track));
      TVector3 pos = utils::ArrayToTVector3(backTpc->PositionEnd);

      if(std::isnan(pos.X()) || !(
         (pos.X() >= DS::TpcXMin && pos.X() <= DS::TpcXMax) &&
         (pos.Y() >= DS::TpcYMin && pos.Y() <= DS::TpcYMax) &&
         (pos.Z() >= DS::TpcZMin)))
      {
         i++;
         continue;
      }

      TVector3 dir = utils::ArrayToTVector3(backTpc->DirectionEnd);
      TVector3 zdir(0.0, 0.0, 1.0);
      double tpcBackAngleFromZ = TMath::RadToDeg() * dir.Unit().Angle(zdir);
      if(std::isnan(dir.X()) || !(tpcBackAngleFromZ <= DS::TpcAngleMax))
      {
         i++;
         continue;
      }

      if(track->Momentum > highestMomentum)
      {
         highestMomentum = track->Momentum;
         tpcECalBox->downstreamTrack = track;
         count++;
      }
      i++;
   }

   i = tpcECalBox->positiveTracks.begin();
   while(i != tpcECalBox->positiveTracks.end())
   {
      AnaTrackB* track = *i;

      AnaTpcTrack* backTpc = static_cast<AnaTpcTrack*>(
         anaUtils::GetTPCBackSegment(track));
      TVector3 pos = utils::ArrayToTVector3(backTpc->PositionEnd);

      if(std::isnan(pos.X()) || !(
         (pos.X() >= DS::TpcXMin && pos.X() <= DS::TpcXMax) &&
         (pos.Y() >= DS::TpcYMin && pos.Y() <= DS::TpcYMax) &&
         (pos.Z() >= DS::TpcZMin)))
      {
         i++;
         continue;
      }

      TVector3 dir = utils::ArrayToTVector3(backTpc->DirectionEnd);
      TVector3 zdir(0.0, 0.0, 1.0);
      double tpcBackAngleFromZ = TMath::RadToDeg() * dir.Unit().Angle(zdir);
      if(std::isnan(dir.X()) || !(tpcBackAngleFromZ <= DS::TpcAngleMax))
      {
         i++;
         continue;
      }

      if(track->Momentum > highestMomentum)
      {
         highestMomentum = track->Momentum;
         tpcECalBox->downstreamTrack = track;
      }
      i++;
   }

   return true;
}

bool DownstreamTracksCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void) event;

   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   return tpcECalBox->downstreamTrack;
}

bool FindBarrelTracksAction::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void) event;

   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   int count = 0;
   Float_t highestMomentum = 0;
   std::list<AnaTrackB*>::iterator i = tpcECalBox->negativeTracks.begin();
   while(i != tpcECalBox->negativeTracks.end())
   {
      AnaTrackB* track = *i;
      AnaTpcTrack* backTpc = static_cast<AnaTpcTrack*>(
         anaUtils::GetTPCBackSegment(track));
      TVector3 pos = utils::ArrayToTVector3(backTpc->PositionEnd);

      if(std::isnan(pos.X()) || !(
         ((pos.X() < Barrel::TpcXMin) || (pos.X() > Barrel::TpcXMax) ||
         (pos.Y() < Barrel::TpcYMin) || (pos.Y() > Barrel::TpcYMax)) &&
         (pos.Z() >= Barrel::TpcZMin && pos.Z() <= Barrel::TpcZMax)))
      {
         i++;
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
         i++;
         continue;
      }

      if(track->Momentum > highestMomentum)
      {
         highestMomentum = track->Momentum;
         tpcECalBox->barrelTrack = track;
         count++;
      }
      i++;
   }

   i = tpcECalBox->positiveTracks.begin();
   while(i != tpcECalBox->positiveTracks.end())
   {
      AnaTrackB* track = *i;
      AnaTpcTrack* backTpc = static_cast<AnaTpcTrack*>(
         anaUtils::GetTPCBackSegment(track));
      TVector3 pos = utils::ArrayToTVector3(backTpc->PositionEnd);

      if(std::isnan(pos.X()) || !(
         ((pos.X() < Barrel::TpcXMin) || (pos.X() > Barrel::TpcXMax) ||
         (pos.Y() < Barrel::TpcYMin) || (pos.Y() > Barrel::TpcYMax)) &&
         (pos.Z() >= Barrel::TpcZMin && pos.Z() <= Barrel::TpcZMax)))
      {
         i++;
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
         i++;
         continue;
      }

      if(track->Momentum > highestMomentum)
      {
         highestMomentum = track->Momentum;
         tpcECalBox->barrelTrack = track;
      }
      i++;
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

bool FGDTPCTracksCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void) event;

   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   SubDetId::SubDetEnum det = tpcECalBox->DetectorFV;
   EventBoxTracker::TrackGroupEnum groupID;
   EventBoxB* eventBox = event.EventBoxes[AnaEventB::kEventBoxTracker];
   AnaTrackB** fgd1Tracks = nullptr;
   AnaTrackB** fgd2Tracks = nullptr;
   int nFgd1Tracks = 0;
   int nFgd2Tracks = 0;
   switch(det)
   {
      case SubDetId::kFGD:
         groupID = EventBoxTracker::kTracksWithGoodQualityTPCInFGD1FV;
         fgd1Tracks = eventBox->TracksInGroup[groupID];
         nFgd1Tracks = eventBox->nTracksInGroup[groupID];
         groupID = EventBoxTracker::kTracksWithGoodQualityTPCInFGD2FV;
         fgd2Tracks = eventBox->TracksInGroup[groupID];
         nFgd2Tracks = eventBox->nTracksInGroup[groupID];
         break;
      case SubDetId::kFGD1:
         groupID = EventBoxTracker::kTracksWithGoodQualityTPCInFGD1FV;
         fgd1Tracks = eventBox->TracksInGroup[groupID];
         nFgd1Tracks = eventBox->nTracksInGroup[groupID];
         break;
      case SubDetId::kFGD2:
         groupID = EventBoxTracker::kTracksWithGoodQualityTPCInFGD2FV;
         fgd2Tracks = eventBox->TracksInGroup[groupID];
         nFgd2Tracks = eventBox->nTracksInGroup[groupID];
         break;
      default:
         return false;
   }

   for(Int_t i = 0; i < nFgd1Tracks; ++i)
   {
      AnaTrackB* track = fgd1Tracks[i];
      tpcECalBox->fgd1Tracks.push_back(track);
   }

   for(Int_t i = 0; i < nFgd2Tracks; ++i)
   {
      AnaTrackB* track = fgd2Tracks[i];
      tpcECalBox->fgd2Tracks.push_back(track);
   }

   return (tpcECalBox->fgd1Tracks.size() + tpcECalBox->fgd2Tracks.size()) > 0;
}

bool FGDFVTracksCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void)event;
   
   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   std::list<AnaTrackB*>::iterator i = tpcECalBox->fgd1Tracks.begin();
   while(i != tpcECalBox->fgd1Tracks.end())
   {
      cutUtils::FiducialCut(*(*i), SubDetId::kFGD1) ?
         i++ : tpcECalBox->fgd1Tracks.erase(i++);
   } 

   i = tpcECalBox->fgd2Tracks.begin();
   while(i != tpcECalBox->fgd2Tracks.end())
   {
      cutUtils::FiducialCut(*(*i), SubDetId::kFGD2) ?
         i++ : tpcECalBox->fgd2Tracks.erase(i++);
   }

   return (tpcECalBox->fgd1Tracks.size() + tpcECalBox->fgd2Tracks.size()) > 0;
}

bool SeparationTracksCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void)event;
   
   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   std::list<AnaTrackB*>::iterator i = tpcECalBox->fgd1Tracks.begin();
   while(i != tpcECalBox->fgd1Tracks.end())
   {
      std::list<AnaTrackB*>::iterator j = ++(std::list<AnaTrackB*>::iterator(i));
      while(j != tpcECalBox->fgd1Tracks.end())
      {
         float squaredSeparation =
            cutUtils::GetSeparationSquared((*i)->PositionStart, (*j)->PositionStart);
         if(squaredSeparation < 0.01)
         {
            tpcECalBox->fgdPairedTracks.push_back(
               new std::pair<AnaTrackB*, AnaTrackB*>(*i, *j));
         }
         j++;
      }
      i++;
   }

   i = tpcECalBox->fgd2Tracks.begin();
   while(i != tpcECalBox->fgd2Tracks.end())
   {
      std::list<AnaTrackB*>::iterator j = ++(std::list<AnaTrackB*>::iterator(i));
      while(j != tpcECalBox->fgd2Tracks.end())
      {
         float squaredSeparation =
            cutUtils::GetSeparationSquared((*i)->PositionStart, (*j)->PositionStart);
         if(squaredSeparation < 100)
         {
            tpcECalBox->fgdPairedTracks.push_back(
               new std::pair<AnaTrackB*, AnaTrackB*>(*i, *j));
         }
         j++;
      }
      i++;
   }

   tpcECalBox->fgd1Tracks.clear();
   tpcECalBox->fgd2Tracks.clear();

   return tpcECalBox->fgdPairedTracks.size() > 0;
}

bool OppositeChargeTracksCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void)event;
   
   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);

   std::list<std::pair<AnaTrackB*, AnaTrackB*>*>::iterator i = tpcECalBox->fgdPairedTracks.begin();
   while(i != tpcECalBox->fgdPairedTracks.end())
   {
      std::pair<AnaTrackB*, AnaTrackB*>* pair = *i;
      (pair->first->Charge == pair->second->Charge) ?
         tpcECalBox->fgdPairedTracks.erase(i++) : i++;
   }

   return tpcECalBox->fgdPairedTracks.size() > 0;
}

bool NegativePartnerTracksCut::Apply(AnaEventB& event, ToyBoxB& box) const
{
   (void)event;

   ToyBoxTPCECal *tpcECalBox = static_cast<ToyBoxTPCECal*>(&box);
   
   std::list<std::pair<AnaTrackB*, AnaTrackB*>*>::iterator i = tpcECalBox->fgdPairedTracks.begin();
   while(i != tpcECalBox->fgdPairedTracks.end())
   {
      std::pair<AnaTrackB*, AnaTrackB*>* pair = *i;
      if(pair->first->Charge < 0)
      {
         tpcECalBox->negativeTracks.push_back(pair->first);
      }
      else if(pair->second->Charge < 0)
      {
         tpcECalBox->negativeTracks.push_back(pair->second);
      }
      i++;
   }

   return tpcECalBox->negativeTracks.size() > 0;
}

