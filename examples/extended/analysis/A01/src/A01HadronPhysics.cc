// $Id: A01HadronPhysics.cc,v 1.1 2002-11-13 07:23:31 duns Exp $
// --------------------------------------------------------------
// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//


#include "A01HadronPhysics.hh"

#include "globals.hh"
#include "G4ios.hh"
#include "g4std/iomanip"   


A01HadronPhysics::A01HadronPhysics(const G4String& name)
                   :  G4VPhysicsConstructor(name)
{
}

A01HadronPhysics::~A01HadronPhysics()
{
}

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4PionZero.hh"
#include "G4KaonPlus.hh"
#include "G4KaonMinus.hh"

void A01HadronPhysics::ConstructParticle()
{
  // pions
  G4PionPlus::PionPlusDefinition();
  G4PionMinus::PionMinusDefinition();
  G4PionZero::PionZeroDefinition();

  // K+/-
  G4KaonPlus::KaonPlusDefinition();
  G4KaonMinus::KaonMinusDefinition();
}


#include "G4ProcessManager.hh"

void A01HadronPhysics::ConstructProcess()
{
  G4ProcessManager * pManager = 0;

  // Pion Plus Physics
  pManager = G4PionPlus::PionPlus()->GetProcessManager();
   // add processes
  //  pManager->AddProcess(&fPiPlusIonisation, ordInActive,2, 2);

  pManager->AddProcess(&fPiPlusMultipleScattering);
  //  pManager->SetProcessOrdering(&fPiPlusMultipleScattering, idxAlongStep,  1);
  pManager->SetProcessOrdering(&fPiPlusMultipleScattering, idxPostStep,  1);

  // Pion Minus Physics
  pManager = G4PionMinus::PionMinus()->GetProcessManager();
   // add processes
  //  pManager->AddProcess(&fPiMinusIonisation, ordInActive,2, 2);

  pManager->AddProcess(&fPiMinusMultipleScattering);
  //  pManager->SetProcessOrdering(&fPiMinusMultipleScattering, idxAlongStep,  1);
  pManager->SetProcessOrdering(&fPiMinusMultipleScattering, idxPostStep,  1);

  // K+ Physics
  pManager = G4KaonPlus::KaonPlus()->GetProcessManager();
   // add processes
  //  pManager->AddProcess(&fKaonPlusIonisation, ordInActive,2, 2);

  pManager->AddProcess(&fKaonPlusMultipleScattering);
  //  pManager->SetProcessOrdering(&fKaonPlusMultipleScattering, idxAlongStep,  1);
  pManager->SetProcessOrdering(&fKaonPlusMultipleScattering, idxPostStep,  1);

  // K- Physics
  pManager = G4KaonMinus::KaonMinus()->GetProcessManager();
   // add processes
  //  pManager->AddProcess(&fKaonMinusIonisation, ordInActive,2, 2);

  pManager->AddProcess(&fKaonMinusMultipleScattering);
  //  pManager->SetProcessOrdering(&fKaonMinusMultipleScattering, idxAlongStep,  1);
  pManager->SetProcessOrdering(&fKaonMinusMultipleScattering, idxPostStep,  1);

}



