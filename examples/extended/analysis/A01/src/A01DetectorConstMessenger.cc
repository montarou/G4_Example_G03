// $Id: A01DetectorConstMessenger.cc,v 1.1 2002-11-13 07:22:46 duns Exp $
// --------------------------------------------------------------
// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
#include "A01DetectorConstMessenger.hh"
#include "A01DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4ios.hh"

A01DetectorConstMessenger::A01DetectorConstMessenger(A01DetectorConstruction* mpga)
:target(mpga)
{
  mydetDirectory = new G4UIdirectory("/mydet/");
  mydetDirectory->SetGuidance("A01 detector setup control commands.");

  armCmd = new G4UIcmdWithADoubleAndUnit("/mydet/armAngle",this);
  armCmd->SetGuidance("Rotation angle of the second arm.");
  armCmd->SetParameterName("angle",true);
  armCmd->SetRange("angle>=0. && angle<180.");
  armCmd->SetDefaultValue(30.);
  armCmd->SetDefaultUnit("deg");
}

A01DetectorConstMessenger::~A01DetectorConstMessenger()
{
  delete armCmd;
  delete mydetDirectory;
}

void A01DetectorConstMessenger::SetNewValue(G4UIcommand * command,G4String newValue)
{
  if( command==armCmd )
  { target->SetArmAngle(armCmd->GetNewDoubleValue(newValue)); }
}

G4String A01DetectorConstMessenger::GetCurrentValue(G4UIcommand * command)
{
  G4String cv;
  if( command==armCmd )
  { cv = armCmd->ConvertToString(target->GetArmAngle(),"deg"); }

  return cv;
}

