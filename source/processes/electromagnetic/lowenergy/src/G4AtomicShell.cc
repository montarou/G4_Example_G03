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
// $Id: G4AtomicShell.cc,v 1.2 ????
// GEANT4 tag $Name: not supported by cvs2svn $
//
// Authors: Elena Guardincerri (Elena.Guardincerri@ge.infn.it)
//          Alfonso Mantero (Alfonso.Mantero@ge.infn.it)
//
// History:
// -----------
//  16 Sept 2001 Modofied according to a design iteration in the 
//              LowEnergy category
//
// -------------------------------------------------------------------

#include "G4AtomicShell.hh"

// this is the constructor: U have to give some information the shell 
// wich the subshell belongs to
// the bindingenergy of the subshell u r creating, and the table of the 
// transition datas

G4AtomicShell::G4AtomicShell(G4int id, G4double energy)
{
  identifier = id;
  bindingEnergy = energy;
 
} 

G4AtomicShell::~G4AtomicShell()
{ }

G4double G4AtomicShell::BindingEnergy() const {

  return bindingEnergy;
}


G4int G4AtomicShell::ShellId() const{

  return identifier;
}





