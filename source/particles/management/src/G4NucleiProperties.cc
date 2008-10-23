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
// $Id: G4NucleiProperties.cc,v 1.16 2008-10-23 13:34:59 kurasige Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// ------------------------------------------------------------
//	GEANT 4 class header file 
//
// ------------------------------------------------------------
//
// Hadronic Process: Nuclear De-excitations
// by V. Lara (Oct 1998)
// Migrate into particles category by H.Kurashige (17 Nov. 98)
// Added Shell-Pairing corrections to the Cameron mass 
// excess formula by V.Lara (9 May 99)


#include "G4NucleiProperties.hh"

G4bool   G4NucleiProperties::isIntialized = false;

G4double G4NucleiProperties::mass_proton = -1.;
G4double G4NucleiProperties::mass_neutron = -1.;
G4double G4NucleiProperties::mass_deuteron = -1.;
G4double G4NucleiProperties::mass_triton = -1.;
G4double G4NucleiProperties::mass_alpha = -1.;
G4double G4NucleiProperties::mass_He3 = -1.;
G4double G4NucleiProperties::electronMass[MaxZ];

G4double  G4NucleiProperties::AtomicMass(G4double A, G4double Z)
{
  const G4double hydrogen_mass_excess = G4NucleiPropertiesTable::GetMassExcess(1,1);
  const G4double neutron_mass_excess =  G4NucleiPropertiesTable::GetMassExcess(0,1);

  G4double mass =
      (A-Z)*neutron_mass_excess + Z*hydrogen_mass_excess - BindingEnergy(A,Z) + A*amu_c2;

  return mass;
}

G4double  G4NucleiProperties::BindingEnergy(G4double A, G4double Z)
{ 
  //
  // Weitzsaecker's Mass formula
  //
  G4int Npairing = G4int(A-Z)%2;                  // pairing
  G4int Zpairing = G4int(Z)%2;
  G4double binding =
      - 15.67*A                           // nuclear volume
      + 17.23*std::pow(A,2./3.)                // surface energy
      + 93.15*((A/2.-Z)*(A/2.-Z))/A       // asymmetry
      + 0.6984523*Z*Z*std::pow(A,-1./3.);      // coulomb
  if( Npairing == Zpairing ) binding += (Npairing+Zpairing-1) * 12.0 / std::sqrt(A);  // pairing

  return -binding*MeV;
}


G4double G4NucleiProperties::GetNuclearMass(const G4double A, const G4double Z)
{
  if (!isIntialized) {
    isIntialized = true;
    G4ParticleDefinition * nucleus = 0;
    nucleus = G4ParticleTable::GetParticleTable()->FindParticle("proton"); // proton 
    if (nucleus!=0) mass_proton = nucleus->GetPDGMass();
    nucleus = G4ParticleTable::GetParticleTable()->FindParticle("neutron"); // neutron 
    if (nucleus!=0) mass_neutron = nucleus->GetPDGMass();
    nucleus = G4ParticleTable::GetParticleTable()->FindParticle("deuteron"); // deuteron 
    if (nucleus!=0) mass_deuteron = nucleus->GetPDGMass();
    nucleus = G4ParticleTable::GetParticleTable()->FindParticle("triton"); // triton 
    if (nucleus!=0) mass_triton = nucleus->GetPDGMass();
    nucleus = G4ParticleTable::GetParticleTable()->FindParticle("alpha"); // alpha 
    if (nucleus!=0) mass_alpha = nucleus->GetPDGMass();
    nucleus = G4ParticleTable::GetParticleTable()->FindParticle("He3"); // He3 
    if (nucleus!=0) mass_He3 = nucleus->GetPDGMass();

    for (int iz=1; iz<MaxZ; iz+=1){
      electronMass[iz] =  iz*electron_mass_c2 
	                         - 1.433e-5*MeV*std::pow(G4double(iz),2.39); ;
    }
  }

  if (A < 1 || Z < 0 || Z > A) {
#ifdef G4VERBOSE
    if (G4ParticleTable::GetParticleTable()->GetVerboseLevel()>0) {
      G4cout << "G4NucleiProperties::GetNuclearMass: Wrong values for A = " << A 
	     << " and Z = " << Z << G4endl;
    }
#endif    
    return 0.0;
  } else {
    G4double mass= -1.;
    if ( (Z<=2) ) {
      if ( (Z==1)&&(A==1) ) {
	mass = mass_proton;
      } else if ( (Z==0)&&(A==1) ) {
	mass = mass_neutron;
      } else if ( (Z==1)&&(A==2) ) {
	mass = mass_deuteron;
      } else if ( (Z==1)&&(A==3) ) {
	mass = mass_triton;
      } else if ( (Z==2)&&(A==4) ) {
	mass = mass_alpha;
      } else if ( (Z==2)&&(A==3) ) {
	mass = mass_He3;
      }
    }
    if (mass < 0.) {
      if (Z >= MaxZ) {
	mass = GetAtomicMass(A,Z) - Z*electron_mass_c2 + 1.433e-5*MeV*std::pow(Z,2.39);      
      } else {
	mass = GetAtomicMass(A,Z) - electronMass[G4int(Z)];
      }
    }
    return mass;
  }
}

G4bool G4NucleiProperties::IsInStableTable(const G4double A, const G4double Z)
{
  if (Z < 0 || Z > A) {
#ifdef G4VERBOSE
    if (G4ParticleTable::GetParticleTable()->GetVerboseLevel()>0) {
      G4cout << "G4NucleiProperties::IsInStableTable: Wrong values for A = " 
	     << A << " and Z = " << Z << G4endl;	
    }
#endif 
    return false;

  } else {
    G4int iA = G4int(A);
    G4int iZ = G4int(Z);
    return G4NucleiPropertiesTable::IsInTable(iZ,iA);
  }
}

G4double G4NucleiProperties::GetMassExcess(const G4int A, const G4int Z)
{
  if (A < 1 || Z < 0 || Z > A) {
#ifdef G4VERBOSE
    if (G4ParticleTable::GetParticleTable()->GetVerboseLevel()>0) {
      G4cout << "G4NucleiProperties::GetMassExccess: Wrong values for A = " 
	     << A << " and Z = " << Z << G4endl;
    }
#endif    
    return 0.0;
    
  } else {

    if (G4NucleiPropertiesTable::IsInTable(Z,A)){
      return G4NucleiPropertiesTable::GetMassExcess(Z,A);
    } else if (G4NucleiPropertiesTheoreticalTable::IsInTable(Z,A)){
      return G4NucleiPropertiesTheoreticalTable::GetMassExcess(Z,A);
    } else {
      return MassExcess(A,Z);
    }
  }

}


G4double G4NucleiProperties::GetAtomicMass(const G4double A, const G4double Z)
{
  if (Z < 0 || Z > A) {
#ifdef G4VERBOSE
    if (G4ParticleTable::GetParticleTable()->GetVerboseLevel()>0) {
      G4cout << "G4NucleiProperties::GetAtomicMass: Wrong values for A = " 
	     << A << " and Z = " << Z << G4endl;	
    }
#endif 
    return 0.0;

  } else if (std::abs(A - G4int(A)) > 1.e-10) {
    return AtomicMass(A,Z);

  } else {
    G4int iA = G4int(A);
    G4int iZ = G4int(Z);
    if (G4NucleiPropertiesTable::IsInTable(iZ,iA)) {
      return G4NucleiPropertiesTable::GetAtomicMass(iZ,iA);
    } else if (G4NucleiPropertiesTheoreticalTable::IsInTable(iZ,iA)){
      return G4NucleiPropertiesTheoreticalTable::GetAtomicMass(iZ,iA);
    } else {
      return AtomicMass(A,Z);
    }
  }
}

G4double G4NucleiProperties::GetBindingEnergy(const G4int A, const G4int Z)
{
  if (A < 1 || Z < 0 || Z > A) {
#ifdef G4VERBOSE
    if (G4ParticleTable::GetParticleTable()->GetVerboseLevel()>0) {
      G4cout << "G4NucleiProperties::GetMassExccess: Wrong values for A = " 
	     << A << " and Z = " << Z << G4endl;
    }
#endif
    return 0.0;

  } else {
    if (G4NucleiPropertiesTable::IsInTable(Z,A)) {
      return G4NucleiPropertiesTable::GetBindingEnergy(Z,A);
    } else if (G4NucleiPropertiesTheoreticalTable::IsInTable(Z,A)) {
      return G4NucleiPropertiesTheoreticalTable::GetBindingEnergy(Z,A);
    }else {
      return BindingEnergy(A,Z);
    }

  }
}


G4double G4NucleiProperties::MassExcess(G4double A, G4double Z) 
{
  return GetAtomicMass(A,Z) - A*amu_c2;
}
	
