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
// Authors: S. Guatelli and M. G. Pia, INFN Genova, Italy
// 
// Based on code developed by the undergraduate student G. Guerrieri 
// Note: this is a preliminary beta-version of the code; an improved 
// version will be distributed in the next Geant4 public release, compliant
// with the design in a forthcoming publication, and subject to a 
// design and code review.
//
#include "G4MIRDRightLeg.hh"

#include "globals.hh"

#include "G4SDManager.hh"
#include "G4Cons.hh"

#include "G4VisAttributes.hh"
#include "G4HumanPhantomMaterial.hh"
#include "G4EllipticalTube.hh"
#include "G4ThreeVector.hh"
#include "G4VPhysicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UnionSolid.hh"
#include "G4HumanPhantomColour.hh"

G4MIRDRightLeg::G4MIRDRightLeg()
{
}

G4MIRDRightLeg::~G4MIRDRightLeg()
{
}

G4VPhysicalVolume* G4MIRDRightLeg::ConstructOrgan(G4VPhysicalVolume* mother, G4bool sensitivity, 
					      G4String volumeName, G4String colourName, G4bool wireFrame)
{
 
  G4cout << "Construct"<< volumeName << G4endl;

  G4HumanPhantomMaterial* material = new G4HumanPhantomMaterial();
  G4Material* soft = material -> GetMaterial("soft_tissue");
 
  G4double rmin1 = 0.* cm;
  G4double rmin2 = 0.* cm;
  G4double dz= 80.0 * cm; 
  G4double rmax1= 2.0 * cm;
  G4double rmax2= 10. * cm;
  G4double startphi= 0.* degree;
  G4double deltaphi= 360. * degree;

  G4Cons* leg1 = new G4Cons("Leg1",  
			   rmin1, rmax1, 
			   rmin2, rmax2, dz/2., 
			   startphi, deltaphi);
  
  //  G4UnionSolid* RightLeg = new G4UnionSolid("RightLeg", leg1, leg1,0,
  //                                    G4ThreeVector(20. *cm, 0., 0.));

  G4LogicalVolume* logicRightLeg = new G4LogicalVolume(leg1,
						   soft,
						   "logical" + volumeName,
						    0, 0, 0);
						   
  G4RotationMatrix* rm = new G4RotationMatrix();
  rm -> rotateX(90.* degree);

  G4VPhysicalVolume* physRightLeg = new G4PVPlacement(rm,
                                
				G4ThreeVector(-10. * cm, -40. * cm, 0. *cm),
      			       "physicalRightLeg",
  			       logicRightLeg,
			       mother,
			       false,
			       0, true);

  // Sensitive Body Part
  if (sensitivity == true)
  { 
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    logicRightLeg->SetSensitiveDetector( SDman->FindSensitiveDetector("BodyPartSD") );
  }

  // Visualization Attributes
  //G4VisAttributes* RightLegVisAtt = new G4VisAttributes(G4Colour(0.94,0.5,0.5));
  G4HumanPhantomColour* colourPointer = new G4HumanPhantomColour();
  G4Colour colour = colourPointer -> GetColour(colourName);
  G4VisAttributes* RightLegVisAtt = new G4VisAttributes(colour);
  RightLegVisAtt->SetForceSolid(wireFrame);
  logicRightLeg->SetVisAttributes(RightLegVisAtt);

  G4cout << "RightLeg created !!!!!!" << G4endl;

  // Testing RightLeg Volume
  G4double RightLegVol = logicRightLeg->GetSolid()->GetCubicVolume();
  G4cout << "Volume of RightLeg = " << RightLegVol/cm3 << " cm^3" << G4endl;
  
  // Testing RightLeg Material
  G4String RightLegMat = logicRightLeg->GetMaterial()->GetName();
  G4cout << "Material of RightLeg = " << RightLegMat << G4endl;
  
  // Testing Density
  G4double RightLegDensity = logicRightLeg->GetMaterial()->GetDensity();
  G4cout << "Density of Material = " << RightLegDensity*cm3/g << " g/cm^3" << G4endl;

  // Testing Mass
  G4double RightLegMass = (RightLegVol)*RightLegDensity;
  G4cout << "Mass of RightLeg = " << RightLegMass/gram << " g" << G4endl;

  
  return physRightLeg;
}
