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
///////////////////////////////////////////////////////////////////////////////
// File: CompositeCalorimeter.cc
// Description: Main function for Geant4 application HCAL Test-BEAM H2-96
///////////////////////////////////////////////////////////////////////////////

#include "CCalDetectorConstruction.hh"
#include "CCalEndOfEventAction.hh"
#include "CCalRunAction.hh"

#include "CCalPrimaryGeneratorAction.hh"
#include "LHEP.hh"
#include "QGSP.hh"
#include "QGSC.hh"

#include "G4RunManager.hh"
#include "G4UIterminal.hh"

#ifdef G4UI_USE_XM
#include "G4UIXm.hh"
#endif

#ifdef G4VIS_USE
  #include "CCalVisManager.hh"
#endif


int main(int argc,char** argv) {

#ifdef G4VIS_USE
  CCalVisManager *visManager = new CCalVisManager;
  visManager->Initialize();
#endif        

  G4RunManager * runManager = new G4RunManager;
  runManager->SetUserInitialization(new CCalDetectorConstruction);

  //***LOOKHERE*** CHOOSE THE PHYSICS LIST.
  // runManager->SetUserInitialization(new LHEP);     // LHEP     
  // runManager->SetUserInitialization(new QGSP);     // QGSP   
  runManager->SetUserInitialization(new QGSC);     // QGSC
  //***endLOOKHERE***

  ////////////////////////////
  //  User action classes.  //
  //  --------------------  //
  ////////////////////////////

  //////////////////////////////////
  // PRIMARY PARTICLEs GENERATION //
  //////////////////////////////////

  CCalPrimaryGeneratorAction* primaryGenerator = new CCalPrimaryGeneratorAction;
  runManager->SetUserAction(primaryGenerator);
  
  /////////
  // RUN //
  /////////

  runManager->SetUserAction(new CCalRunAction);
  
  ///////////
  // EVENT //
  ///////////

  runManager->SetUserAction(new CCalEndOfEventAction(primaryGenerator));
  
  G4UImanager * UI = G4UImanager::GetUIpointer();
  UI->ApplyCommand("/CCal/generator/verbose 2");
  UI->ApplyCommand("/gun/position -1380. 0. 0. mm");
  UI->ApplyCommand("/gun/direction 1. 0. 0.");
  UI->ApplyCommand("/gun/energy 100 GeV");

  G4UIsession * session = 0;
  // Define (G)UI terminal for interactive mode
  if (argc==1) {

#ifdef G4UI_USE_XM
      session = new G4UIXm(argc,argv);
#else // G4UIterminal is a (dumb) terminal.
      session = new G4UIterminal;
#endif

    G4cout <<" Run initializing ..."<<G4endl;
    UI->ApplyCommand("/process/verbose 0");
    UI->ApplyCommand("/run/verbose 2");
    UI->ApplyCommand("/run/initialize");
   
#ifdef G4VIS_USE
    // Create empty scene
    G4String visCommand = "/vis/scene/create";
    UI->ApplyCommand(visCommand);

    // Choose one default viewer 
    // (the user can always change it later on) 
    // visCommand = "/vis/open DAWNFILE";
    // visCommand = "/vis/open VRML2FILE";
    visCommand = "/vis/open OGLIX";
    UI->ApplyCommand(visCommand);

    visCommand = "/vis/viewer/flush";
    UI->ApplyCommand(visCommand);
    visCommand = "/tracking/storeTrajectory 1";
    UI->ApplyCommand(visCommand);
#endif

#ifdef G4UI_USE_XM
    // Customize the G4UIXm menubar with a macro file :
    UI->ApplyCommand("/control/execute gui.mac");
#else
    G4cout <<"Now, please, apply beamOn command..."<<G4endl;
#endif

    session->SessionStart();    
    delete session;

  } else {

    // Batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UI->ApplyCommand(command+fileName);

  }

  delete runManager;

#ifdef G4VIS_USE
  delete visManager;
#endif

  return 0;
}

