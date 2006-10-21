//
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
// $Id: G4AttHolder.cc,v 1.1 2006-10-21 10:10:46 allison Exp $
// GEANT4 tag $Name: not supported by cvs2svn $

#include "G4AttHolder.hh"

#include "G4AttValue.hh"
#include "G4AttDef.hh"
#include "G4AttDefStore.hh"

G4AttHolder::~G4AttHolder()
{
  for (size_t i = 0; i < fValues.size(); ++i) delete fValues[i];
}

const G4String& G4AttHolder::GetAttDefsName(size_t i) const
{
  return G4AttDefStore::GetName(fDefs[i]);
}

void G4AttHolder::CopyAndAddAttValues(const std::vector<G4AttValue>* values)
{
  std::vector<G4AttValue>* copy = new std::vector<G4AttValue>(*values);
  fValues.push_back(copy);
}
