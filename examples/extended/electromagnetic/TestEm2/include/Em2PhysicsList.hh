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
// $Id: Em2PhysicsList.hh,v 1.6 2002-10-14 15:56:11 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
//
// ------------------------------------------------------------
//	GEANT 4 class header file 
// Class Description:
//      This class is an derived class of G4VPhysicsConstructor
//
// --------------------------------------------------------------------------- 
//	History
//        Created:       14.10.02  V.Ivanchenko provide modular list on base 
//                                 of old Em2PhysicsList
//
//        Modified:      
// 
// ---------------------------------------------------------------------------
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef Em2PhysicsList_h
#define Em2PhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"

class Em2PhysicsListMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class Em2PhysicsList: public G4VModularPhysicsList
{
  public:

    Em2PhysicsList();
   ~Em2PhysicsList();

    void AddPhysicsList(const G4String& name);
    void SetCuts();
    void SetCutForGamma(G4double);
    void SetCutForElectron(G4double);
    void SetCutForPositron(G4double);
       
  private:
    G4double cutForGamma;
    G4double cutForElectron; 
    G4double cutForPositron;
    G4double currentDefaultCut;

    G4bool   emPhysicsListIsRegistered;
    
    Em2PhysicsListMessenger* pMessenger;         
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

