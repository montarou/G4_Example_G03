// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: F02CalorimeterSD.hh,v 1.1 2001-03-27 16:26:16 grichine Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef F02CalorimeterSD_h
#define F02CalorimeterSD_h 1

#include "G4VSensitiveDetector.hh"
#include "globals.hh"

class F02DetectorConstruction;
class G4HCofThisEvent;
class G4Step;
#include "F02CalorHit.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class F02CalorimeterSD : public G4VSensitiveDetector
{
  public:
  
      F02CalorimeterSD(G4String, F02DetectorConstruction* );
     ~F02CalorimeterSD();

      void Initialize(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*,G4TouchableHistory*);
      void EndOfEvent(G4HCofThisEvent*);
      void clear();
      void PrintAll();

  private:
  
      F02CalorHitsCollection*  CalCollection;      
      F02DetectorConstruction* Detector;
      G4int*                   HitID;
};

#endif

