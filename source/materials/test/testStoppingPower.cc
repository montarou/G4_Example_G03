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
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
// History: Test for calculation Stopping Power of ions
//          G4Stopping classes are in $G4INSTALL/source/materials/
// 
//
// 11.08.08, Ivantchenko
//
//
// $TESTTARGET should be gived, please look GNUmakefile


#include "G4MaterialStoppingICRU73.hh"
#include "G4SimpleMaterialStoppingICRU73.hh"
#include "G4IronStoppingICRU73.hh"

int main(){  
G4double E[31] = {.025,.03,.04,.05,.06,.07,.08,.09,.1,.15,.2,.25,.3,.4,.5,.6,.7,.8,.9,1,1.5,2,2.5,3,4,5,6,7,8,9,10};
G4int Z_Ion[16] = {3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18};
G4double A_Ion[16] = {6.941,9.0122,10.811,12.011,14.007,15.999,18.998,20.180,22.990,24.305,26.982,28.086,30.974,32.065,35.453,39.948};
G4int AA_Ion[16] = {7,9,11,12,14,16,19,20,23,24,27,28,31,32,35,40};
G4String NameMaterial[31]={"G4_A-150_TISSUE","G4_ADIPOSE_TISSUE_ICRP","G4_AIR","G4_ALUMINUM_OXIDE","G4_BONE_COMPACT_ICRU","G4_BONE_CORTICAL_ICRP","G4_C-552","G4_CALCIUM_FLUORIDE","G4_CARBON_DIOXIDE","G4_Pyrex_Glass","G4_KAPTON","G4_LITHIUM_FLUORIDE","G4_LITHIUM_TETRABORATE","G4_METHANE","G4_MUSCLE_STRIATED_ICRU","G4_NYLON-6/6","G4_PHOTO_EMULSION","G4_PLASTIC_SC_VINYLTOLUENE","G4_POLYCARBONATE","G4_POLYETHYLENE","G4_MYLAR","G4_LUCITE","G4_POLYSTYRENE","G4_TEFLON","G4_PROPANE","G4_SILICON_DIOXIDE","G4_SODIUM_IODIDE","G4_TISSUE-METHANE","G4_TISSUE-PROPANE","G4_WATER","G4_WATER_VAPOR"};

//G4double G4MaterialStoppingICRU73::GetDEDX(G4int ionZ, G4int idxMaterial,G4double kinEnergy)
//G4double G4SimpleMaterialStoppingICRU73::GetDEDX(G4int ionZ, G4int idxMaterial, G4double kinEnergy)
//G4double G4IronStoppingICRU73::GetDEDX(G4int idxMaterial, G4double kinEnergy)

  G4int idxmat = 11;
  G4int idxion = 5;
  G4double Energy = .5;
  G4MaterialStoppingICRU73 mS;
  G4double dedx = mS.GetDEDX(Z_Ion[idxion],idxmat,Energy*MeV*AA_Ion[idxion]);
  G4String matN = mS.GetMaterialName(idxmat);
  G4double dens = mS.GetDensity(idxmat);
  G4cout << "Z=" << Z_Ion[idxion] << " in " << matN << " at E = " << Energy << "MeV, DEDX is "<< dedx*mm/MeV << " MeV/mm,  or " 
  << dedx*gram/(1000*dens*MeV*cm2) << " MeV/g/cm2, Density= " << dens*cm3/gram << " g/cm3 " << G4endl;

  idxmat = 20;
  idxion = 5;
  Energy = 1.;
  G4SimpleMaterialStoppingICRU73 mS1;
  G4double dedx1 = mS1.GetDEDX(Z_Ion[idxion],idxmat,Energy*MeV*AA_Ion[idxion]);
  G4String matN1 = mS1.GetMaterialName(idxmat);
  G4double dens1 = mS1.GetDensity(idxmat);
  G4cout << "Z=" << Z_Ion[idxion] << " in " << matN1 << "               at E = " << Energy << "MeV, DEDX is "
	 << dedx1*mm/MeV << " MeV/mm, or " 
	 << dedx1*gram/(1000*dens1*MeV*cm2) << " MeV/g/cm2, Density= " << dens1*cm3/gram << " g/cm3 " << G4endl;
     
  idxmat = 10;
  Energy = .5;
  G4IronStoppingICRU73 mS2;
  G4double dedx2 = mS2.GetDEDX(idxmat,Energy*MeV*56);
  G4String matN2 = mS2.GetMaterialName(idxmat);
  G4double dens2 = mS2.GetDensity(idxmat);
  G4cout << "Z=26 in " << matN2 << "               at E = " << Energy << "MeV, DEDX =" << dedx2*mm/MeV << " MeV/mm,  or " 
	  << dedx2*gram/(1000*dens2*MeV*cm2) << " MeV/g/cm2,  Density= " << dens2*cm3/gram << " g/cm3 " << G4endl;
   G4cout << "### End ###" << G4endl; 
   
   return 0;
} 

