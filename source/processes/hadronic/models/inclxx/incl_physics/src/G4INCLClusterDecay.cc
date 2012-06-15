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
// INCL++ intra-nuclear cascade model
// Pekka Kaitaniemi, CEA and Helsinki Institute of Physics
// Davide Mancusi, CEA
// Alain Boudard, CEA
// Sylvie Leray, CEA
// Joseph Cugnon, University of Liege
//
// INCL++ revision: v5.1
//
#define INCLXX_IN_GEANT4_MODE 1

#include "globals.hh"

/** \file G4INCLClusterDecay.cc
 * \brief Static class for carrying out cluster decays
 *
 * Created on: 6th July 2011
 *     Author: Davide Mancusi
 */

#include "G4INCLClusterDecay.hh"
#include "G4INCLParticleTable.hh"
#include "G4INCLKinematicsUtils.hh"
#include "G4INCLRandom.hh"
// #include <cassert>
#include <algorithm>

namespace G4INCL {

  ParticleList ClusterDecay::decay(Cluster * const c) {
    ParticleList decayProducts;
    recursiveDecay(c, &decayProducts);

    // Correctly update the particle type
    if(c->getA()==1) {
// assert(c->getZ()==1 || c->getZ()==0);
      if(c->getZ()==1)
        c->setType(Proton);
      else
        c->setType(Neutron);
      c->setTableMass();
    }

    return decayProducts;
  }

  void ClusterDecay::recursiveDecay(Cluster * const c, ParticleList *decayProducts) {
    const G4int Z = c->getZ();
    const G4int A = c->getA();
// assert(c->getExcitationEnergy()>-1.e-5);
    if(c->getExcitationEnergy()<0.)
      c->setExcitationEnergy(0.);

    if(Z<ParticleTable::clusterTableZSize && A<ParticleTable::clusterTableASize) {
      ParticleTable::ClusterDecayType theDecayMode = ParticleTable::clusterDecayMode[Z][A];

      switch(theDecayMode) {
        default:
          ERROR("Unrecognized cluster-decay mode: " << theDecayMode << std::endl
              << c->print());
        case ParticleTable::StableCluster:
          // For stable clusters, just return
          return;
          break;
        case ParticleTable::ProtonDecay:
        case ParticleTable::NeutronDecay:
        case ParticleTable::AlphaDecay:
          // Two-body decays
          twoBodyDecay(c, theDecayMode, decayProducts);
          break;
        case ParticleTable::TwoProtonDecay:
        case ParticleTable::TwoNeutronDecay:
          // Three-body decays
          threeBodyDecay(c, theDecayMode, decayProducts);
          break;
        case ParticleTable::ProtonUnbound:
        case ParticleTable::NeutronUnbound:
          // Phase-space decays
          phaseSpaceDecay(c, theDecayMode, decayProducts);
          break;
      }

      // Calls itself recursively in case the produced remnant is still unstable.
      // Sneaky, isn't it.
      recursiveDecay(c,decayProducts);

    } else {
      // The cluster is too large for our decay-mode table. Decompose it only
      // if Z==0 || Z==A.
      DEBUG("Cluster is outside the decay-mode table." << c->print() << std::endl);
      if(Z==A) {
        DEBUG("Z==A, will decompose it in free protons." << std::endl);
        phaseSpaceDecay(c, ParticleTable::ProtonUnbound, decayProducts);
      } else if(Z==0) {
        DEBUG("Z==0, will decompose it in free neutrons." << std::endl);
        phaseSpaceDecay(c, ParticleTable::NeutronUnbound, decayProducts);
      }
    }
  }

  void ClusterDecay::twoBodyDecay(Cluster * const c, ParticleTable::ClusterDecayType theDecayMode, ParticleList *decayProducts) {
    Particle *decayParticle = 0;
    const ThreeVector mom(0.0, 0.0, 0.0);
    const ThreeVector pos = c->getPosition();

    // Create the emitted particle
    switch(theDecayMode) {
      case ParticleTable::ProtonDecay:
        decayParticle = new Particle(Proton, mom, pos);
        break;
      case ParticleTable::NeutronDecay:
        decayParticle = new Particle(Neutron, mom, pos);
        break;
      case ParticleTable::AlphaDecay:
        decayParticle = new Cluster(2,4);
        break;
      default:
        ERROR("Unrecognized cluster-decay mode in two-body decay: " << theDecayMode << std::endl
            << c->print());
        return;
    }
    decayParticle->makeParticipant();
    decayParticle->setNumberOfDecays(1);
    decayParticle->setPosition(c->getPosition());
    decayParticle->setEmissionTime(c->getEmissionTime());
    decayParticle->setTableMass();

    // Save some variables of the mother cluster
    G4double motherMass = c->getMass();
    const ThreeVector velocity = -c->boostVector();

    // Characteristics of the daughter particle
    const G4int daughterZ = c->getZ() - decayParticle->getZ();
    const G4int daughterA = c->getA() - decayParticle->getA();
    const G4double daughterMass = ParticleTable::getTableMass(daughterA,daughterZ);

    // The mother cluster becomes the daughter
    c->setZ(daughterZ);
    c->setA(daughterA);
    c->setMass(daughterMass);
    c->setExcitationEnergy(0.);

    // Decay kinematics in the mother rest frame
    const G4double decayMass = decayParticle->getMass();
// assert(motherMass-daughterMass-decayMass>-1.e-5); // Q-value should be >0
    G4double pCM = 0.;
    if(motherMass-daughterMass-decayMass>0.)
      pCM = KinematicsUtils::momentumInCM(motherMass, daughterMass, decayMass);
    const ThreeVector momentum = Random::normVector(pCM);
    c->setMomentum(momentum);
    c->adjustEnergyFromMomentum();
    decayParticle->setMomentum(-momentum);
    decayParticle->adjustEnergyFromMomentum();

    // Boost to the lab frame
    decayParticle->boost(velocity);
    c->boost(velocity);

    // Add the decay particle to the list of decay products
    decayProducts->push_back(decayParticle);
  }

  void ClusterDecay::threeBodyDecay(Cluster * const c, ParticleTable::ClusterDecayType theDecayMode, ParticleList *decayProducts) {
    Particle *decayParticle1 = 0;
    Particle *decayParticle2 = 0;
    const ThreeVector mom(0.0, 0.0, 0.0);
    const ThreeVector pos = c->getPosition();

    // Create the emitted particles
    switch(theDecayMode) {
      case ParticleTable::TwoProtonDecay:
        decayParticle1 = new Particle(Proton, mom, pos);
        decayParticle2 = new Particle(Proton, mom, pos);
        break;
      case ParticleTable::TwoNeutronDecay:
        decayParticle1 = new Particle(Neutron, mom, pos);
        decayParticle2 = new Particle(Neutron, mom, pos);
        break;
      default:
        ERROR("Unrecognized cluster-decay mode in three-body decay: " << theDecayMode << std::endl
            << c->print());
        return;
    }
    decayParticle1->makeParticipant();
    decayParticle2->makeParticipant();
    decayParticle1->setNumberOfDecays(1);
    decayParticle2->setNumberOfDecays(1);
    decayParticle1->setTableMass();
    decayParticle2->setTableMass();

    // Save some variables of the mother cluster
    const G4double motherMass = c->getMass();
    const ThreeVector velocity = -c->boostVector();

    // Masses and charges of the daughter particle and of the decay products
    const G4int decayZ1 = decayParticle1->getZ();
    const G4int decayA1 = decayParticle1->getA();
    const G4int decayZ2 = decayParticle2->getZ();
    const G4int decayA2 = decayParticle2->getA();
    const G4int decayZ = decayZ1 + decayZ2;
    const G4int decayA = decayA1 + decayA2;
    const G4int daughterZ = c->getZ() - decayZ;
    const G4int daughterA = c->getA() - decayA;
    const G4double decayMass1 = decayParticle1->getMass();
    const G4double decayMass2 = decayParticle2->getMass();
    const G4double daughterMass = ParticleTable::getTableMass(daughterA,daughterZ);

    // Q-values
    G4double qValue = motherMass - daughterMass - decayMass1 - decayMass2;
// assert(qValue > -1e-5); // Q-value should be >0
    if(qValue<0.)
      qValue=0.;
    const G4double qValueB = qValue * Random::shoot();

    // The decay particles behave as if they had more mass until the second
    // decay
    const G4double decayMass = decayMass1 + decayMass2 + qValueB;

    /* Stage A: mother --> daughter + (decay1+decay2) */

    // The mother cluster becomes the daughter
    c->setZ(daughterZ);
    c->setA(daughterA);
    c->setMass(daughterMass);
    c->setExcitationEnergy(0.);

    // Decay kinematics in the mother rest frame
    const G4double pCMA = KinematicsUtils::momentumInCM(motherMass, daughterMass, decayMass);
    const ThreeVector momentumA = Random::normVector(pCMA);
    c->setMomentum(momentumA);
    c->adjustEnergyFromMomentum();
    const ThreeVector decayBoostVector = momentumA/std::sqrt(decayMass*decayMass + momentumA.mag2());

    /* Stage B: (decay1+decay2) --> decay1 + decay2 */

    // Decay kinematics in the (decay1+decay2) rest frame
    const G4double pCMB = KinematicsUtils::momentumInCM(decayMass, decayMass1, decayMass2);
    const ThreeVector momentumB = Random::normVector(pCMB);
    decayParticle1->setMomentum(momentumB);
    decayParticle2->setMomentum(-momentumB);
    decayParticle1->adjustEnergyFromMomentum();
    decayParticle2->adjustEnergyFromMomentum();

    // Boost decay1 and decay2 to the Stage-A decay frame
    decayParticle1->boost(decayBoostVector);
    decayParticle2->boost(decayBoostVector);

    // Boost all particles to the lab frame
    decayParticle1->boost(velocity);
    decayParticle2->boost(velocity);
    c->boost(velocity);

    // Add the decay particles to the list of decay products
    decayProducts->push_back(decayParticle1);
    decayProducts->push_back(decayParticle2);
  }

  void ClusterDecay::phaseSpaceDecay(Cluster * const c, ParticleTable::ClusterDecayType theDecayMode, ParticleList *decayProducts) {
    const G4int theA = c->getA();
    const G4int theZ = c->getZ();
    const ThreeVector mom(0.0, 0.0, 0.0);
    const ThreeVector pos = c->getPosition();

    G4int theZStep;
    ParticleType theEjectileType;
    switch(theDecayMode) {
      case ParticleTable::ProtonUnbound:
        theZStep = 1;
        theEjectileType = Proton;
        break;
      case ParticleTable::NeutronUnbound:
        theZStep = 0;
        theEjectileType = Neutron;
        break;
      default:
        ERROR("Unrecognized cluster-decay mode in phase-space decay: " << theDecayMode << std::endl
            << c->print());
        return;
    }

    // Find the daughter cluster (first cluster which is not
    // proton/neutron-unbound, in the sense of the table)
    G4int finalDaughterZ, finalDaughterA;
    if(theZ<ParticleTable::clusterTableZSize && theA<ParticleTable::clusterTableASize) {
      finalDaughterZ=theZ;
      finalDaughterA=theA;
      while(ParticleTable::clusterDecayMode[finalDaughterZ][finalDaughterA]==theDecayMode) {
        finalDaughterA--;
        finalDaughterZ -= theZStep;
      }
    } else {
      finalDaughterA = 1;
      if(theDecayMode==ParticleTable::ProtonUnbound)
        finalDaughterZ = 1;
      else
        finalDaughterZ = 0;
    }
// assert(finalDaughterZ<=theZ && finalDaughterA<theA && finalDaughterA>0 && finalDaughterZ>=0);
    const G4double finalDaughterMass = ParticleTable::getTableMass(finalDaughterA, finalDaughterZ);

    // Compute the available decay energy
    const G4int nSplits = theA-finalDaughterA;
    const G4double ejectileMass = ParticleTable::getTableMass(1, theZStep);
    // c->getMass() can possibly contain some excitation energy, too
    G4double availableEnergy = c->getMass() - finalDaughterMass - nSplits*ejectileMass;
// assert(availableEnergy>-1.e-5);
    if(availableEnergy<0.)
      availableEnergy=0.;

    // Compute an estimate of the maximum event weight
    G4double maximumWeight = 1.;
    G4double eMax = finalDaughterMass + availableEnergy;
    G4double eMin = finalDaughterMass - ejectileMass;
    for(G4int iSplit=0; iSplit<nSplits; ++iSplit) {
      eMax += ejectileMass;
      eMin += ejectileMass;
      maximumWeight *= KinematicsUtils::momentumInCM(eMax, eMin, ejectileMass);
    }

    // Sample decays until the weight cutoff is satisfied
    G4double weight;
    std::vector<G4double> theCMMomenta;
    std::vector<G4double> invariantMasses;
    G4int nTries=0;
    /* Maximum number of trials dependent on nSplits. 50 trials seems to be
     * sufficient for small nSplits. For nSplits>=5, maximumWeight is a gross
     * overestimate of the actual maximum weight, leading to unreasonably high
     * rejection rates. For these cases, we set nSplits=1000, although the sane
     * thing to do would be to improve the importance sampling (maybe by
     * parametrising maximumWeight?).
     */
    G4int maxTries;
    if(nSplits<5)
      maxTries=50;
    else
      maxTries=1000;
    do {
      if(nTries++>maxTries) {
        WARN("Phase-space decay exceeded the maximum number of rejections (" << maxTries
            << "). Z=" << theZ << ", A=" << theA << ", E*=" << c->getExcitationEnergy()
            << ", availableEnergy=" << availableEnergy
            << ", nSplits=" << nSplits
            << std::endl);
        break;
      }

      // Construct a sorted vector of random numbers
      std::vector<G4double> randomNumbers;
      for(G4int iSplit=0; iSplit<nSplits-1; ++iSplit)
        randomNumbers.push_back(Random::shoot0());
      std::sort(randomNumbers.begin(), randomNumbers.end());

      // Divide the available decay energy in the right number of steps
      invariantMasses.clear();
      invariantMasses.push_back(finalDaughterMass);
      for(G4int iSplit=0; iSplit<nSplits-1; ++iSplit)
        invariantMasses.push_back(finalDaughterMass + (iSplit+1)*ejectileMass + randomNumbers.at(iSplit)*availableEnergy);
      invariantMasses.push_back(c->getMass());

      weight = 1.;
      theCMMomenta.clear();
      for(G4int iSplit=0; iSplit<nSplits; ++iSplit) {
        G4double motherMass = invariantMasses.at(nSplits-iSplit);
        const G4double daughterMass = invariantMasses.at(nSplits-iSplit-1);
// assert(motherMass-daughterMass-ejectileMass>-1.e-5);
        G4double pCM = 0.;
        if(motherMass-daughterMass-ejectileMass>0.)
          pCM = KinematicsUtils::momentumInCM(motherMass, daughterMass, ejectileMass);
        theCMMomenta.push_back(pCM);
        weight *= pCM;
      }
    } while(maximumWeight*Random::shoot()>weight);

    for(G4int iSplit=0; iSplit<nSplits; ++iSplit) {
      ThreeVector const velocity = -c->boostVector();

#if !defined(NDEBUG) && !defined(INCLXX_IN_GEANT4_MODE)
      const G4double motherMass = c->getMass();
#endif
      c->setA(c->getA() - 1);
      c->setZ(c->getZ() - theZStep);
      c->setMass(invariantMasses.at(nSplits-iSplit-1));

      Particle *ejectile = new Particle(theEjectileType, mom, pos);
      ejectile->setTableMass();

// assert(motherMass-c->getMass()-ejectileMass>-1.e-5);
      ThreeVector momentum;
      momentum = Random::normVector(theCMMomenta.at(iSplit));
      c->setMomentum(momentum);
      c->adjustEnergyFromMomentum();
      ejectile->setMomentum(-momentum);
      ejectile->adjustEnergyFromMomentum();

      // Boost to the lab frame
      c->boost(velocity);
      ejectile->boost(velocity);

      // Add the decay particle to the list of decay products
      decayProducts->push_back(ejectile);
    }
// assert(std::abs(c->getTableMass()-c->getMass())<1.e-3);
    c->setExcitationEnergy(0.);
  }
}

