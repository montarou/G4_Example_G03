// $Id: A01DriftChamber.hh,v 1.1 2002-11-13 07:17:45 duns Exp $
// --------------------------------------------------------------
// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
#ifndef A01DriftChamber_h
#define A01DriftChamber_h 1

#include "G4VSensitiveDetector.hh"
#include "A01DriftChamberHit.hh"
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class A01DriftChamber : public G4VSensitiveDetector
{

  public:
      A01DriftChamber(G4String name);
      virtual ~A01DriftChamber();

      virtual void Initialize(G4HCofThisEvent*HCE);
      virtual G4bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist);
      virtual void EndOfEvent(G4HCofThisEvent*HCE);

  private:
      A01DriftChamberHitsCollection * hitsCollection;
      G4int HCID;
};




#endif

