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
// $Id: G4VCrossSectionSource.cc,v 1.3 2004-12-07 13:48:51 gunter Exp $ //
//

#include "globals.hh"
#include "G4HadronicException.hh"
#include "G4ios.hh"
#include "G4VCrossSectionSource.hh"
#include "G4ParticleDefinition.hh"
#include "G4KineticTrack.hh"
#include "G4CrossSectionVector.hh"
#include "G4CrossSectionSourcePtr.hh"
#include "G4Proton.hh"
#include "G4Neutron.hh"

G4VCrossSectionSource::G4VCrossSectionSource()
{ }


G4VCrossSectionSource::~G4VCrossSectionSource()
{ }


G4String G4VCrossSectionSource::
FindKeyParticle(const G4KineticTrack& trk1,const G4KineticTrack& trk2) const
{
  G4String result;
  
  G4ParticleDefinition * p1 = trk1.GetDefinition();
  G4ParticleDefinition * p2 = trk2.GetDefinition();
  
  if( (p1==G4Proton::Proton() && p2==G4Proton::Proton() ) ||
      (p1==G4Neutron::Neutron() && p2==G4Neutron::Neutron()) )
  {
    result = G4Proton::Proton()->GetParticleName();
  }
  else if( (p1==G4Neutron::Neutron() && p2==G4Proton::Proton()) ||
           (p2==G4Neutron::Neutron() && p1==G4Proton::Proton()) )
  {
    result = G4Neutron::Neutron()->GetParticleName();
  }
  else
  {
    throw G4HadronicException(__FILE__, __LINE__, "G4VCrossSectionSource: unklnown particles in FindKeyParticle");
  }
  return result;
}

G4bool G4VCrossSectionSource::operator==(const G4VCrossSectionSource &right) const
{
  return (this == (G4VCrossSectionSource *) &right);
}


G4bool G4VCrossSectionSource::operator!=(const G4VCrossSectionSource &right) const
{
  return (this != (G4VCrossSectionSource *) &right);
}


void G4VCrossSectionSource::Print() const
{
  G4int nComponents = 0;
  const G4CrossSectionVector* components = GetComponents();
  if (components)
    {
      nComponents = components->size();
    }
  G4cout << "---- " << this->Name() << " ---- has " << nComponents << " components" <<G4endl;
  G4int i;
  for (i=0; i<nComponents; i++)
    {
      G4cout << "-" <<  this->Name() << " - Component " << i << ": " <<G4endl;

      G4CrossSectionSourcePtr componentPtr = (*components)[i];
      G4VCrossSectionSource* component = componentPtr();
      component->Print();
    }
}


void G4VCrossSectionSource::PrintAll(const G4KineticTrack& trk1, const G4KineticTrack& trk2) const
{
  G4double sqrtS = (trk1.Get4Momentum() + trk2.Get4Momentum()).mag();
  G4double sigma = CrossSection(trk1,trk2) / millibarn;
  G4cout << "---- " << Name() << ": "
	 << "Ecm = " << sqrtS / GeV << " GeV -  " 
	 << " Cross section = " << sigma << " mb "
	 << G4endl;

  G4int nComponents = 0;
  const G4CrossSectionVector* components = GetComponents();
  if (components != 0)
    {
      nComponents = components->size();
    }
  G4int i;
  for (i=0; i<nComponents; i++)
    {
      G4cout << "* Component " << i << ": ";
      G4CrossSectionSourcePtr componentPtr = (*components)[i];
      G4VCrossSectionSource* component = componentPtr();
      component->PrintAll(trk1,trk2);
    }
}


G4bool G4VCrossSectionSource::InLimits(G4double e, G4double eLow, G4double eHigh) const
{
  G4bool answer = false;
  if (e >= eLow && e <= eHigh) answer = true;
  return answer;
}

G4double G4VCrossSectionSource::LowLimit() const
{
  return 0.; 
}


G4double G4VCrossSectionSource::HighLimit() const
{
  return DBL_MAX; 
}

G4bool G4VCrossSectionSource::IsValid(G4double e) const
{
  G4bool answer = false;
  if (e >= LowLimit() && e <= HighLimit()) answer = true;
  return answer;
}

const G4ParticleDefinition* G4VCrossSectionSource::FindLightParticle(const G4KineticTrack& trk1, 
								     const G4KineticTrack& trk2) const
{
  G4double mass1 = trk1.GetDefinition()->GetPDGMass();
  G4double mass2 = trk2.GetDefinition()->GetPDGMass();
  if (mass1 < mass2)
    {
      return trk1.GetDefinition();
    }
  else
    {
      return trk2.GetDefinition();
    }
}


const G4double G4VCrossSectionSource::FcrossX(G4double e, G4double e0, G4double sigma, 
					      G4double eParam, G4double power) const
{
  G4double result = 0.;

  G4double denom = eParam*eParam + (e-e0)*(e-e0);
  if (denom > 0.) 
  {
    G4double value = (2.* eParam * sigma * (e-e0) / denom) * std::pow(((e0 + eParam) / e), power);
    result = std::max(0., value);
  }
  return result;
}     
    
const G4double G4VCrossSectionSource::GetTransversePionMass() const
{
  // Parameter from UrQMD
  const G4double transversePionMass = 0.3 * GeV;
  return transversePionMass;
}


const G4double G4VCrossSectionSource::GetMinStringMass() const
{
  // Parameter from UrQMD
  const G4double minStringMass = 0.52 * GeV;
  return minStringMass;
}



