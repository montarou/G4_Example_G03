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
// neutron_hp -- source file
// J.P. Wellisch, Nov-1996
// A prototype of the low energy neutron transport model.
//
#include "G4NeutronHPGamma.hh"
#include "G4SystemOfUnits.hh"

G4ThreadLocal int G4NeutronHPGamma::instancecount = 0;

  G4NeutronHPGamma::G4NeutronHPGamma() 
  {
    next = 0;
    instancecount ++;
  }

  G4NeutronHPGamma::~G4NeutronHPGamma() {instancecount--;}

G4bool G4NeutronHPGamma::Init(std::ifstream & aDataFile)
{
  G4bool theResult = true;
  if(aDataFile >> levelEnergy)
  {
    aDataFile >> gammaEnergy >> probability;
    levelEnergy *= keV;
    gammaEnergy *= keV;
  }
  else
  {
    theResult=false;
  }
  return theResult;
}
