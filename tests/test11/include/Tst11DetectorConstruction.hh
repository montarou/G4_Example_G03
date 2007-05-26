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
//
// $Id: Tst11DetectorConstruction.hh,v 1.6 2007-05-26 00:06:45 tkoi Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//

#ifndef Tst11DetectorConstruction_h
#define Tst11DetectorConstruction_h 1

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Material;
class Tst11DetectorMessenger;

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class Tst11DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    Tst11DetectorConstruction();
    ~Tst11DetectorConstruction();

  public:
     G4VPhysicalVolume* Construct();
     void SelectMaterial(G4String val);
     void SetMaterial( G4String materialChoice );

  private:
     void defineNISTMaterials();
     void SelectMaterialPointer();

     G4LogicalVolume*   simpleBoxLog;
     G4Material* selectedMaterial;
     G4Material* Air;
     G4Material* Al;
     G4Material* Pb;
     G4Material* U;
     G4String materialChoice;
     Tst11DetectorMessenger * detectorMessenger;
};

#endif

