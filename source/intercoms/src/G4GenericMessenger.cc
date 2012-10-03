// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: G4UIaliasList.cc,v 1.6 2006-06-29 19:08:33 gunter Exp $
//

#include "G4GenericMessenger.hh"
#include "G4Types.hh"
#include "G4UImessenger.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIdirectory.hh"

#include <iostream>


G4GenericMessenger::G4GenericMessenger(void* obj, const G4String& dir, const G4String& doc): directory(dir), object(obj) {
  dircmd = new G4UIdirectory(dir);
  dircmd->SetGuidance(doc);
}

G4GenericMessenger::~G4GenericMessenger() {
  delete dircmd;
  for (std::map<G4String, Property>::iterator i = properties.begin(); i != properties.end(); i++) delete i->second.command;
  for (std::map<G4String, Method>::iterator i = methods.begin(); i != methods.end(); i++) delete i->second.command;
}


G4GenericMessenger::Command&
G4GenericMessenger::DeclareProperty(const G4String& name, const G4AnyType& var, const G4String& doc) {
  G4String fullpath = directory+name;
  G4UIcommand* cmd = new G4UIcommand(fullpath.c_str(), this);
  if(doc != "") cmd->SetGuidance(doc);  
  char ptype;
  if(var.TypeInfo() == typeid(int) || var.TypeInfo() == typeid(long) ||
     var.TypeInfo() == typeid(unsigned int) || var.TypeInfo() == typeid(unsigned long)) ptype = 'i';
  else if(var.TypeInfo() == typeid(float) || var.TypeInfo() == typeid(double)) ptype = 'd';
  else if(var.TypeInfo() == typeid(bool)) ptype = 'b';
  else if(var.TypeInfo() == typeid(G4String)) ptype = 's';
  else ptype = 's';
  cmd->SetParameter(new G4UIparameter("value", ptype, false));
  return properties[name] = Property(var, cmd);
}


G4GenericMessenger::Command&
G4GenericMessenger::DeclareMethod(const G4String& name, const G4AnyMethod& fun, const G4String& doc) {
  G4String fullpath = directory+name;
  G4UIcommand* cmd = new G4UIcommand(fullpath.c_str(), this);
  if(doc != "") cmd->SetGuidance(doc);
  for (size_t i = 0; i < fun.NArg(); i++) {
    cmd->SetParameter(new G4UIparameter("arg", 's', false));
  }
  return methods[name] = Method(fun, object, cmd);
}

G4String G4GenericMessenger::GetCurrentValue(G4UIcommand* command) {
  Property& p = properties[command->GetCommandName()];
  return p.variable.ToString();
}

void G4GenericMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
  if ( properties.find(command->GetCommandName()) != properties.end()) {
    Property& p = properties[command->GetCommandName()];
    p.variable.FromString(newValue);
  }
  else if (methods.find(command->GetCommandName()) != methods.end()) {
    Method& m = methods[command->GetCommandName()];
    if(m.method.NArg() == 0)
      m.method.operator()(m.object);
    else if (m.method.NArg() > 0) {
      m.method.operator()(m.object,newValue);
    }
    else {
      throw G4BadArgument();
    }
  }
}


void G4GenericMessenger::SetGuidance(const G4String& s) {
  dircmd->SetGuidance(s);
}


G4GenericMessenger::Command& G4GenericMessenger::Command::SetUnit(const G4String& unit) {
  // Change the type of command (unfortunatelly this is done a posteriory)
  // We need to delete the old command before creating the new one and therefore we need to recover the information
  // before the deletetion
  G4String cmdpath = command->GetCommandPath();
  G4UImessenger* messenger = command->GetMessenger();
  G4String range = command->GetRange();
  std::vector<G4String> guidance;
  for (G4int i = 0; i < command->GetGuidanceEntries(); i++) guidance.push_back(command->GetGuidanceLine(i));
  delete command;

  if (*type == typeid(float) || *type == typeid(double) ) {
    G4UIcmdWithADoubleAndUnit* cmd_t = new G4UIcmdWithADoubleAndUnit(cmdpath, messenger);
    cmd_t->SetDefaultUnit(unit);
    command = cmd_t;
  }
  else if (*type == typeid(G4ThreeVector)) {
    G4UIcmdWith3VectorAndUnit* cmd_t = new G4UIcmdWith3VectorAndUnit(cmdpath, messenger);
    cmd_t->SetDefaultUnit(unit);
    command = cmd_t;
  }
  else {
    G4cerr << "Only parameters of type <double> or <float> can be associated with units" << G4endl;
    return *this;
  }
  for (size_t i = 0; i < guidance.size(); i++) command->SetGuidance(guidance[i]);
  command->SetRange(range);
  return *this;
}

