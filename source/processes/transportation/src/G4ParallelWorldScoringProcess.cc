//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: G4ParallelWorldScoringProcess.cc,v 1.1 2006-04-29 02:24:01 asaim Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//

#include "G4ios.hh"
#include "G4ParallelWorldScoringProcess.hh"
#include "G4Step.hh"
#include "G4Navigator.hh"
#include "G4VTouchable.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleChange.hh"
#include "G4PathFinder.hh"
#include "G4TransportationManager.hh"
#include "G4ParticleChange.hh"
#include "G4StepPoint.hh"
#include "G4FieldTrackUpdator.hh"

#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"

//--------------------------------
// Constructor with name and type:
//--------------------------------
G4ParallelWorldScoringProcess::
G4ParallelWorldScoringProcess(const G4String& processName,G4ProcessType theType)
:G4VProcess(processName,theType), fGhostNavigator(0), fNavigatorID(-1), fFieldTrack('0')
{
  pParticleChange = &aDummyParticleChange;

  fGhostStep = new G4Step();
  fGhostPreStepPoint = fGhostStep->GetPreStepPoint();
  fGhostPostStepPoint = fGhostStep->GetPostStepPoint();

  fTransportationManager = G4TransportationManager::GetTransportationManager();
  fPathFinder = G4PathFinder::GetInstance();

  if (verboseLevel>0)
  {
    G4cout << GetProcessName() << " is created " << G4endl;
  }
}

// -----------
// Destructor:
// -----------
G4ParallelWorldScoringProcess::~G4ParallelWorldScoringProcess()
{
  delete fGhostStep;
}

//------------------------------------------------------
//
// SetParallelWorld 
//
//------------------------------------------------------
void G4ParallelWorldScoringProcess::
SetParallelWorld(G4String parallelWorldName)
{
  fGhostWorldName = parallelWorldName;
  fGhostWorld = fTransportationManager->GetParallelWorld(fGhostWorldName);
  fGhostNavigator = fTransportationManager->GetNavigator(fGhostWorld);
}

void G4ParallelWorldScoringProcess::
SetParallelWorld(G4VPhysicalVolume* parallelWorld)
{
  fGhostWorldName = parallelWorld->GetName();
  fGhostWorld = parallelWorld;
  fGhostNavigator = fTransportationManager->GetNavigator(fGhostWorld);
}

//------------------------------------------------------
//
// StartTracking
//
//------------------------------------------------------
void G4ParallelWorldScoringProcess::StartTracking(G4Track*trk)
{
  // Activate navigator
  if(fGhostNavigator)
  { fNavigatorID = fTransportationManager->ActivateNavigator(fGhostNavigator); }
  else
  {
    G4Exception("G4ParallelWorldScoringProcess::StartTracking",
       "ProcParaWorld000",FatalException,
       "G4ParallelWorldScoringProcess is used for tracking without having a parallel world assigned");
  }

  // Setup touchables
  fPathFinder->Locate(trk->GetPosition(),trk->GetMomentumDirection());
  fOldGhostTouchable = fPathFinder->CreateTouchableHandle(fNavigatorID);
  fGhostPreStepPoint->SetTouchableHandle(fOldGhostTouchable);
  fNewGhostTouchable = fOldGhostTouchable;
  fGhostPostStepPoint->SetTouchableHandle(fNewGhostTouchable);

  // Initialize 
  fGhostSafety = -1.;
  fOnBoundary = false;
}

//----------------------------------------------------------
//
//  AtRestGetPhysicalInteractionLength()
//
//----------------------------------------------------------
G4double 
G4ParallelWorldScoringProcess::AtRestGetPhysicalInteractionLength(
         const G4Track& /*track*/, 
         G4ForceCondition* condition)
{
  *condition = Forced;
  return DBL_MAX;
}

//------------------------------------
//
//             AtRestDoIt()
//
//------------------------------------
G4VParticleChange* G4ParallelWorldScoringProcess::AtRestDoIt(
     const G4Track& track,
     const G4Step& step)
{ 
  fOldGhostTouchable = fGhostPostStepPoint->GetTouchableHandle();
  G4VSensitiveDetector* aSD = 0;
  if(fOldGhostTouchable->GetVolume())
  { aSD = fOldGhostTouchable->GetVolume()->GetLogicalVolume()->GetSensitiveDetector(); }
  CopyStep(step);
  fGhostPreStepPoint->SetSensitiveDetector(aSD);

  fNewGhostTouchable = fOldGhostTouchable;
  
  fGhostPreStepPoint->SetTouchableHandle(fOldGhostTouchable);
  fGhostPostStepPoint->SetTouchableHandle(fNewGhostTouchable);
  if(fNewGhostTouchable->GetVolume())
  {
    fGhostPostStepPoint->SetSensitiveDetector(
      fNewGhostTouchable->GetVolume()->GetLogicalVolume()->GetSensitiveDetector());
  }
  else
  { fGhostPostStepPoint->SetSensitiveDetector(0); }

  if (verboseLevel>1) Verbose(step);

  G4VSensitiveDetector* sd = fGhostPreStepPoint->GetSensitiveDetector();
  if(sd)
  {
    sd->Hit(fGhostStep);
  }

  pParticleChange->Initialize(track);
  return pParticleChange;
}

//----------------------------------------------------------
//
//  PostStepGetPhysicalInteractionLength()
//
//----------------------------------------------------------
G4double 
G4ParallelWorldScoringProcess::PostStepGetPhysicalInteractionLength(
         const G4Track& /*track*/, 
         G4double   /*previousStepSize*/, 
         G4ForceCondition* condition)
{
  // I must be invoked anyway to score the hit.
  *condition = StronglyForced;
  return DBL_MAX;
}

//------------------------------------
//
//             PostStepDoIt()
//
//------------------------------------
G4VParticleChange* G4ParallelWorldScoringProcess::PostStepDoIt(
     const G4Track& track,
     const G4Step& step)
{ 
  fOldGhostTouchable = fGhostPostStepPoint->GetTouchableHandle();
  G4VSensitiveDetector* aSD = 0;
  if(fOldGhostTouchable->GetVolume())
  { aSD = fOldGhostTouchable->GetVolume()->GetLogicalVolume()->GetSensitiveDetector(); }
  CopyStep(step);
  fGhostPreStepPoint->SetSensitiveDetector(aSD);

  if(fOnBoundary)
  {
    // Locate the point and get new touchable
    fPathFinder->Locate(step.GetPostStepPoint()->GetPosition(),
                        step.GetPostStepPoint()->GetMomentumDirection());
    fNewGhostTouchable = fPathFinder->CreateTouchableHandle(fNavigatorID);
  }
  else
  {
    // Do I need this ??????????????????????????????????????????????????????????
    // fGhostNavigator->LocateGlobalPointWithinVolume(track.GetPosition());
    // ?????????????????????????????????????????????????????????????????????????
    // reuse the touchable
    fNewGhostTouchable = fOldGhostTouchable;
  }
    
  fGhostPreStepPoint->SetTouchableHandle(fOldGhostTouchable);
  fGhostPostStepPoint->SetTouchableHandle(fNewGhostTouchable);

  if(fNewGhostTouchable->GetVolume())
  {
    fGhostPostStepPoint->SetSensitiveDetector(
      fNewGhostTouchable->GetVolume()->GetLogicalVolume()->GetSensitiveDetector());
  }
  else
  { fGhostPostStepPoint->SetSensitiveDetector(0); }

  if (verboseLevel>1) Verbose(step);

  G4VSensitiveDetector* sd = fGhostPreStepPoint->GetSensitiveDetector();
  if(sd)
  {
    sd->Hit(fGhostStep);
  }

  pParticleChange->Initialize(track); // Does not change the track properties
  return pParticleChange;
}


//---------------------------------------
//
//  AlongStepGetPhysicalInteractionLength
//
//---------------------------------------
G4double G4ParallelWorldScoringProcess::AlongStepGetPhysicalInteractionLength(
            const G4Track& track, G4double  previousStepSize, G4double  currentMinimumStep,
            G4double& proposedSafety, G4GPILSelection* selection)
{
  static G4FieldTrack endTrack('0');
  static ELimited eLimited;
  
  *selection = CandidateForSelection;
  G4double returnedStep = DBL_MAX;

  if (previousStepSize > 0.)
  { fGhostSafety -= previousStepSize; }
  else
  { fGhostSafety = -1.; }
  if (fGhostSafety < 0.) fGhostSafety = 0.0;
      
  // ------------------------------------------
  // Determination of the proposed STEP LENGTH:
  // ------------------------------------------
  if (currentMinimumStep <= fGhostSafety && currentMinimumStep > 0.)
  {
    // I have no chance to limit
    returnedStep = fGhostSafety;
    fOnBoundary = false;
  }
  else // (currentMinimumStep > fGhostSafety: I may limit the Step)
  {
    G4FieldTrackUpdator::Update(&fFieldTrack,&track);
    returnedStep = fPathFinder->ComputeStep(fFieldTrack,currentMinimumStep,fNavigatorID,
       track.GetCurrentStepNumber(),fGhostSafety,eLimited,endTrack);
    if(eLimited == kDoNot)
    {
      returnedStep = fGhostSafety;
      fOnBoundary = false;
    }
    else
    {
      // Track is on the boundary
      fOnBoundary = false;
    }
  }

  // ----------------------------------------------
  // Returns the fGhostSafety as the proposedSafety
  // The SteppingManager will take care of keeping
  // the smallest one.
  // ----------------------------------------------
  proposedSafety = fGhostSafety;
  return returnedStep;
}

G4VParticleChange* G4ParallelWorldScoringProcess::AlongStepDoIt(
    const G4Track& track, const G4Step& )
{
  // Dummy ParticleChange ie: does nothing
  // Expecting G4Transportation to move the track
  pParticleChange->Initialize(track);
  return pParticleChange; 
}


void G4ParallelWorldScoringProcess::CopyStep(const G4Step & step)
{
  fGhostStep->SetTrack(step.GetTrack());
  fGhostStep->SetStepLength(step.GetStepLength());
  fGhostStep->SetTotalEnergyDeposit(step.GetTotalEnergyDeposit());
  fGhostStep->SetControlFlag(step.GetControlFlag());

  *fGhostPreStepPoint = *(step.GetPreStepPoint());

  *fGhostPostStepPoint = *(step.GetPostStepPoint());
}

void G4ParallelWorldScoringProcess::Verbose(const G4Step& step) const
{
  G4cout << "In mass geometry ------------------------------------------------" << G4endl;
  G4cout << " StepLength : " << step.GetStepLength()/mm << "      TotalEnergyDeposit : "
         << step.GetTotalEnergyDeposit()/MeV << G4endl;
  G4cout << " PreStepPoint : "
         << step.GetPreStepPoint()->GetPhysicalVolume()->GetName() << " - ";
  if(step.GetPreStepPoint()->GetProcessDefinedStep())
  { G4cout << step.GetPreStepPoint()->GetProcessDefinedStep()->GetProcessName(); }
  else
  { G4cout << "NoProcessAssigned"; }
  G4cout << G4endl;
  G4cout << "                " << step.GetPreStepPoint()->GetPosition() << G4endl;
  G4cout << " PostStepPoint : ";
  if(step.GetPostStepPoint()->GetPhysicalVolume()) 
  { G4cout << step.GetPostStepPoint()->GetPhysicalVolume()->GetName(); }
  else
  { G4cout << "OutOfWorld"; }
  G4cout << " - ";
  if(step.GetPostStepPoint()->GetProcessDefinedStep())
  { G4cout << step.GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName(); }
  else
  { G4cout << "NoProcessAssigned"; }
  G4cout << G4endl;
  G4cout << "                 " << step.GetPostStepPoint()->GetPosition() << G4endl;

  G4cout << "In ghost geometry ------------------------------------------------" << G4endl;
  G4cout << " StepLength : " << fGhostStep->GetStepLength()/mm
         << "      TotalEnergyDeposit : "
         << fGhostStep->GetTotalEnergyDeposit()/MeV << G4endl;
  G4cout << " PreStepPoint : "
         << fGhostStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() << " ["
           << fGhostStep->GetPreStepPoint()->GetTouchable()->GetReplicaNumber()
           << " ]" << " - ";
  if(fGhostStep->GetPreStepPoint()->GetProcessDefinedStep())
  { G4cout << fGhostStep->GetPreStepPoint()->GetProcessDefinedStep()->GetProcessName(); }
  else
  { G4cout << "NoProcessAssigned"; }
  G4cout << G4endl;
  G4cout << "                " << fGhostStep->GetPreStepPoint()->GetPosition() << G4endl;
  G4cout << " PostStepPoint : ";
  if(fGhostStep->GetPostStepPoint()->GetPhysicalVolume()) 
  {
    G4cout << fGhostStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() << " ["
           << fGhostStep->GetPostStepPoint()->GetTouchable()->GetReplicaNumber()
           << " ]";
  }
  else
  { G4cout << "OutOfWorld"; }
  G4cout << " - ";
  if(fGhostStep->GetPostStepPoint()->GetProcessDefinedStep())
  { G4cout << fGhostStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName(); }
  else
  { G4cout << "NoProcessAssigned"; }
  G4cout << G4endl;
  G4cout << "                 " << fGhostStep->GetPostStepPoint()->GetPosition() << " == "
         << fGhostStep->GetTrack()->GetMomentumDirection() 
         << G4endl;

}

