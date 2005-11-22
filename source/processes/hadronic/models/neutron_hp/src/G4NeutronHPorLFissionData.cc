//
// 05-11-21 NeutronHP or Low Energy Parameterization Models 
//          Implemented by T. Koi (SLAC/SCCS)
//          If NeutronHP data do not available for an element, then Low Energy 
//          Parameterization models handle the interactions of the element.
//

#include "G4NeutronHPorLFissionData.hh"
#include "G4Neutron.hh"
#include "G4ElementTable.hh"
#include "G4NeutronHPData.hh"

#include "G4PhysicsVector.hh"



G4NeutronHPorLFissionData::G4NeutronHPorLFissionData( G4NeutronHPChannel* pChannel , std::set< G4String >* pSet )
{
   theFissionChannel = pChannel;
   unavailable_elements = pSet;   
}

 

G4bool G4NeutronHPorLFissionData::IsApplicable(const G4DynamicParticle*aP, const G4Element* anElement)
{
   G4bool result = true;
   G4double eKin = aP->GetKineticEnergy();
   if(eKin>20*MeV||aP->GetDefinition()!=G4Neutron::Neutron()) result = false;
   if ( unavailable_elements->find( anElement->GetName() ) != unavailable_elements->end() ) result = false;
   return result;
}

G4NeutronHPorLFissionData::G4NeutronHPorLFissionData()
{
//   BuildPhysicsTable(*G4Neutron::Neutron());
}

 

G4NeutronHPorLFissionData::~G4NeutronHPorLFissionData()
{
//  delete theCrossSections;
}


   
void G4NeutronHPorLFissionData::BuildPhysicsTable( const G4ParticleDefinition& aP )
{
   if( &aP!=G4Neutron::Neutron() ) 
      throw G4HadronicException(__FILE__, __LINE__, "Attempt to use NeutronHP data for particles other than neutrons!!!");  
}
 


void G4NeutronHPorLFissionData::DumpPhysicsTable(const G4ParticleDefinition& aP)
{
  if(&aP!=G4Neutron::Neutron()) 
     throw G4HadronicException(__FILE__, __LINE__, "Attempt to use NeutronHP data for particles other than neutrons!!!");  
//  G4cout << "G4NeutronHPorLFissionData::DumpPhysicsTable still to be implemented"<<G4endl;
}



#include "G4Nucleus.hh"
#include "G4NucleiPropertiesTable.hh"
#include "G4Neutron.hh"
#include "G4Electron.hh"

G4double G4NeutronHPorLFissionData::
GetCrossSection(const G4DynamicParticle* aP, const G4Element*anE, G4double aT)
{
  G4double result = 0;
  if ( anE->GetZ() < 90 ) return result;

//  G4bool outOfRange;
  G4int index = anE->GetIndex();

  // prepare neutron
  G4double eKinetic = aP->GetKineticEnergy();
  G4ReactionProduct theNeutron( aP->GetDefinition() );
  theNeutron.SetMomentum( aP->GetMomentum() );
  theNeutron.SetKineticEnergy( eKinetic );

  // prepare thermal nucleus
  G4Nucleus aNuc;
  G4double eps = 0.0001;
  G4double theA = anE->GetN();
  G4double theZ = anE->GetZ();
  G4double eleMass; 
  eleMass = ( G4NucleiPropertiesTable::GetNuclearMass(static_cast<G4int>(theZ+eps), static_cast<G4int>(theA+eps))
	     ) / G4Neutron::Neutron()->GetPDGMass();
  
  G4ReactionProduct boosted;
  G4double aXsection;
  
  // MC integration loop
  G4int counter = 0;
  G4double buffer = 0;
  G4int size = G4int(std::max(10., aT/60*kelvin));
  G4ThreeVector neutronVelocity = 1./G4Neutron::Neutron()->GetPDGMass()*theNeutron.GetMomentum();
  G4double neutronVMag = neutronVelocity.mag();
  while(counter == 0 || std::abs(buffer-result/counter) > 0.03*buffer)
  {
    if(counter) buffer = result/counter;
    while (counter<size)
    {
      counter ++;
      G4ReactionProduct aThermalNuc = aNuc.GetThermalNucleus(eleMass, aT);
      boosted.Lorentz(theNeutron, aThermalNuc);
      G4double theEkin = boosted.GetKineticEnergy();
      //aXsection = (*((*theCrossSections)(index))).GetValue(theEkin, outOfRange);
      aXsection = theFissionChannel[index].GetXsec( theEkin );
      // velocity correction.
      G4ThreeVector targetVelocity = 1./aThermalNuc.GetMass()*aThermalNuc.GetMomentum();
      aXsection *= (targetVelocity-neutronVelocity).mag()/neutronVMag;
      result += aXsection;
    }
    size += size;
  }
  result /= counter;
  return result;
}
