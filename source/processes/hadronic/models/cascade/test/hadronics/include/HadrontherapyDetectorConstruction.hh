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
// $Id: HadrontherapyDetectorConstruction.hh; Version 4.0 May 2005
// ----------------------------------------------------------------------------
//                 GEANT 4 - Hadrontherapy example
// ----------------------------------------------------------------------------
// Code developed by:
//
// G.A.P. Cirrone(a)*, F. Di Rosa(a), S. Guatelli(b), G. Russo(a)
// 
// (a) Laboratori Nazionali del Sud 
//     of the INFN, Catania, Italy
// (b) INFN Section of Genova, Genova, Italy
// 
// * cirrone@lns.infn.it
// ----------------------------------------------------------------------------

#ifndef HadrontherapyDetectorConstruction_H
#define HadrontherapyDetectorConstruction_H 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class HadrontherapyPhantomROGeometry;
class HadrontherapyBeamLine;
class HadrontherapyDetectorMessenger;
class HadrontherapyModulator;
class HadrontherapyPhantomSD;
class HadrontherapyMaterial;

class HadrontherapyDetectorConstruction : public G4VUserDetectorConstruction
{
public:

  HadrontherapyDetectorConstruction();

  ~HadrontherapyDetectorConstruction();

  G4VPhysicalVolume* Construct();  

private: 

  void ConstructBeamLine();
  // This method allows to define the beam line geometry in the
  // experimental set-up

 void ConstructPhantom(); 
 // This method allows to define the phantom geometry in the
 // experimental set-up
 
 void ConstructSensitiveDetector();
  // The sensitive detector is associated to the phantom volume

public: 

  void SetModulatorAngle(G4double angle);
  // This method allows moving the modulator through UI commands

  void SetRangeShifterXPosition(G4double translation);
  // This method allows to move the Range Shifter along
  // the X axis through UI commands

  void SetRangeShifterXSize(G4double halfSize);
  // This method allows to change the size of the range shifter along
  // the X axis through UI command.

  void SetFirstScatteringFoilSize(G4double halfSize);
  // This method allows to change the size of the first scattering foil
  // along the X axis through UI command.

  void SetSecondScatteringFoilSize (G4double halfSize); 
  // This method allows to change the size of the second scattering foil
  // along the X axis through UI command.

  void SetOuterRadiusStopper (G4double value); 
  // This method allows to change the size of the outer radius of the stopper
  // through UI command.

  void SetInnerRadiusFinalCollimator (G4double value);
  // This method allows to change the size of the inner radius of the 
  // final collimator through UI command.

  void SetRSMaterial(G4String material);
  // This method allows to change the material 
  // of the range shifter through UI command.

  G4double ComputeVoxelSize() {return phantomSizeX/numberOfVoxelsAlongX;};
  // Returns the size of the voxel along the X axis
 
private:
  
  HadrontherapyPhantomSD* phantomSD; // Pointer to sensitive detector

  HadrontherapyPhantomROGeometry* phantomROGeometry; // Pointer to ROGeometry 

  HadrontherapyBeamLine* beamLine; // Pointer to the beam line 
                                   // geometry component

  HadrontherapyModulator* modulator; // Pointer to the modulator 
                                     // geometry component

  G4VPhysicalVolume* physicalTreatmentRoom;
  G4VPhysicalVolume* patientPhysicalVolume;
  G4LogicalVolume* phantomLogicalVolume;
  G4VPhysicalVolume* phantomPhysicalVolume;
  
  HadrontherapyDetectorMessenger* detectorMessenger; 
  HadrontherapyMaterial* material;

  G4double phantomSizeX; 
  G4double phantomSizeY; 
  G4double phantomSizeZ;
   
  G4int numberOfVoxelsAlongX; 
  G4int numberOfVoxelsAlongY;
  G4int numberOfVoxelsAlongZ;  
};
#endif
