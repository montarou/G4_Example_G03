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
// G4CascadeCoalescence:  Factory model for final-state interactions to
//   produce light ions from cascade nucleons.  The algorithm implemented
//   here is descirbed in Section 2.3 of the LAQGSM documentation (p. 11-12)
//   [http://lib-www.lanl.gov/la-pubs/00818645.pdf].
//
// The relative-momentum cut offs for each cluster type may be set with
// environment variables:
//	DPMAX_2CLUSTER		0.090 GeV/c for deuterons
//	DPMAX_3CLUSTER		0.108 GeV/c for tritons, He-3
//	DPMAX_4CLUSTER		0.115 GeV/c for alphas
//
// 20110917  Michael Kelsey
// 20110920  M. Kelsey -- Use environment variables to set momentum cuts for
//	     tuning, replace polymorphic argument lists with use of
//	     "ClusterCandidate"
// 20110922  M. Kelsey -- Follow G4InuclParticle::print(ostream&) migration
// 20110927  M. Kelsey -- Bug fix; missing <iterator> header, strtof -> strtod

#include "G4CascadeCoalescence.hh"
#include "G4CollisionOutput.hh"
#include "G4InuclElementaryParticle.hh"
#include "G4InuclNuclei.hh"
#include "G4InuclParticle.hh"
#include "G4ParticleLargerBeta.hh"
#include "G4ParticleLargerEkin.hh"
#include "G4ThreeVector.hh"
#include <vector>
#include <numeric>
#include <algorithm>
#include <iterator>
#include <stdlib.h>


// Short names for lists and iterators for convenience

typedef std::vector<G4InuclElementaryParticle> hadronList;
typedef hadronList::const_iterator hadronIter;

// Maximum relative momentum (in Bertini GeV) for nucleons in each cluster type

const G4double G4CascadeCoalescence::dpMaxDoublet = 
  getenv("DPMAX_2CLUSTER") ? strtod(getenv("DPMAX_2CLUSTER"),NULL) : 0.090;

const G4double G4CascadeCoalescence::dpMaxTriplet = 
  getenv("DPMAX_3CLUSTER") ? strtod(getenv("DPMAX_3CLUSTER"),NULL) : 0.108;

const G4double G4CascadeCoalescence::dpMaxAlpha = 
  getenv("DPMAX_4CLUSTER") ? strtod(getenv("DPMAX_4CLUSTER"),NULL) : 0.115;


// Constructor and Destructor

G4CascadeCoalescence::G4CascadeCoalescence(G4int verbose)
  : verboseLevel(verbose), thisFinalState(0), thisHadrons(0) {}

G4CascadeCoalescence::~G4CascadeCoalescence() {}


// Final state particle list is modified directly

void G4CascadeCoalescence::FindClusters(G4CollisionOutput& finalState) {
  if (verboseLevel)
    G4cout << " >>> G4CascadeCoalescence::FindClusters()" << G4endl;

  thisFinalState = &finalState;		// Save pointers for use in processing
  thisHadrons = &finalState.getOutgoingParticles();

  selectCandidates();
  createNuclei();
  removeNucleons();

  if (verboseLevel>1) thisFinalState->printCollisionOutput();
}


// Scan list for possible nucleon clusters

void G4CascadeCoalescence::selectCandidates() {
  if (verboseLevel)
    G4cout << " >>> G4CascadeCoalescence::selectCandidates()" << G4endl;

  allClusters.clear();
  usedNucleons.clear();

  size_t nHad = thisHadrons->size();
  for (size_t idx1=0; idx1<nHad; idx1++) {
    if (!getHadron(idx1).nucleon()) continue;
    for (size_t idx2=idx1+1; idx2<nHad; idx2++) {
      if (!getHadron(idx2).nucleon()) continue;
      for (size_t idx3=idx2+1; idx3<nHad; idx3++) {
	if (!getHadron(idx3).nucleon()) continue;
	for (size_t idx4=idx3+1; idx4<nHad; idx4++) {
	  if (!getHadron(idx4).nucleon()) continue;
	  tryClusters(idx1, idx2, idx3, idx4);
	}
      }
    }
  }

  // All potential candidates built; report statistics
  if (verboseLevel>1) {
    G4cout << " Found " << allClusters.size() << " candidates, using "
	   << usedNucleons.size() << " nucleons" << G4endl;
  }
}


// Do combinatorics of current set of four, skip nucleons already used

void 
G4CascadeCoalescence::tryClusters(size_t idx1, size_t idx2, size_t idx3, size_t idx4) {
  fillCluster(idx1,idx2,idx3,idx4);
  if (clusterTried(thisCluster)) return;

  if (verboseLevel>1)
    G4cout << " >>> G4CascadeCoalescence::tryClusters " << idx1 << " " << idx2
	   << " " << idx3 << " " << idx4 << G4endl;

  triedClusters.insert(clusterHash(thisCluster));  // Remember current attempt

  if (!nucleonUsed(idx1) && !nucleonUsed(idx2) &&
      !nucleonUsed(idx3) && !nucleonUsed(idx4)) {
    if (goodCluster(thisCluster)) {
      allClusters.push_back(thisCluster);
      usedNucleons.insert(idx1);
      usedNucleons.insert(idx2);
      usedNucleons.insert(idx3);
      usedNucleons.insert(idx4);
      return;
    } else {			// No good alpha, look for triplets
      tryClusters(idx1,idx2,idx3);
      tryClusters(idx1,idx2,idx4);
      tryClusters(idx1,idx3,idx4);
      tryClusters(idx2,idx3,idx4);
    }
  }
  if (nucleonUsed(idx1)) tryClusters(idx2,idx3,idx4);
  if (nucleonUsed(idx2)) tryClusters(idx1,idx3,idx4);
  if (nucleonUsed(idx3)) tryClusters(idx1,idx2,idx4);
  if (nucleonUsed(idx4)) tryClusters(idx1,idx2,idx3);
}

void 
G4CascadeCoalescence::tryClusters(size_t idx1, size_t idx2, size_t idx3) {
  fillCluster(idx1,idx2,idx3);
  if (clusterTried(thisCluster)) return;

  if (verboseLevel>1)
    G4cout << " >>> G4CascadeCoalescence::tryClusters " << idx1 << " " << idx2
	   << " " << idx3 << G4endl;

  triedClusters.insert(clusterHash(thisCluster));  // Remember current attempt

  if (!nucleonUsed(idx1) && !nucleonUsed(idx2) && !nucleonUsed(idx3)) {
    fillCluster(idx1,idx2,idx3);
    if (goodCluster(thisCluster)) {
      allClusters.push_back(thisCluster);
      usedNucleons.insert(idx1);
      usedNucleons.insert(idx2);
      usedNucleons.insert(idx3);
      return;
    } else {			// No good triplet, look for deuterons
      tryClusters(idx1,idx2);
      tryClusters(idx1,idx3);
      tryClusters(idx2,idx3);
    }
  }
  if (nucleonUsed(idx1)) tryClusters(idx2,idx3);
  if (nucleonUsed(idx2)) tryClusters(idx1,idx3);
  if (nucleonUsed(idx3)) tryClusters(idx1,idx2);
}

void 
G4CascadeCoalescence::tryClusters(size_t idx1, size_t idx2) {
  fillCluster(idx1,idx2);
  if (clusterTried(thisCluster)) return;

  if (verboseLevel>1)
    G4cout << " >>> G4CascadeCoalescence::tryClusters " << idx1 << " " << idx2
	   << G4endl;

  triedClusters.insert(clusterHash(thisCluster));  // Remember current attempt

  if (nucleonUsed(idx1) || nucleonUsed(idx2)) return;

  fillCluster(idx1,idx2);
  if (goodCluster(thisCluster)) {
    allClusters.push_back(thisCluster);
    usedNucleons.insert(idx1);
    usedNucleons.insert(idx2);
  }
}


// Process list of candidate clusters into light ions

void G4CascadeCoalescence::createNuclei() {
  if (verboseLevel)
    G4cout << " >>> G4CascadeCoalescence::createNuclei()" << G4endl;

  usedNucleons.clear();

  for (size_t i=0; i<allClusters.size(); i++) {
    if (verboseLevel>1) G4cout << " attempting candidate #" << i << G4endl;

    const ClusterCandidate& cand = allClusters[i];
    if (makeLightIon(cand)) {			// Success, put ion in output
      thisFinalState->addOutgoingNucleus(thisLightIon);
      usedNucleons.insert(cand.begin(), cand.end());
    }
  }
}


// Remove nucleons indexed in "usedNucleons" from output

void G4CascadeCoalescence::removeNucleons() {
  if (verboseLevel>1)
    G4cout << " >>> G4CascadeCoalescence::removeNucleons()" << G4endl;

  // Remove nucleons from output from last to first (to preserve indexing)
  std::set<size_t>::reverse_iterator usedIter;
  for (usedIter = usedNucleons.rbegin(); usedIter != usedNucleons.rend(); ++usedIter)
    thisFinalState->removeOutgoingParticle(*usedIter);

  usedNucleons.clear();
}


// Compute momentum of whole cluster

G4LorentzVector 
G4CascadeCoalescence::getClusterMomentum(const ClusterCandidate& aCluster) const {
  static G4LorentzVector ptot;
  ptot.set(0.,0.,0.,0.);
  for (size_t i=0; i<aCluster.size(); i++)
    ptot += getHadron(aCluster[i]).getMomentum();

  return ptot;
}


// Determine magnitude of largest momentum in CM frame

G4double G4CascadeCoalescence::maxDeltaP(const ClusterCandidate& aCluster) const {
  if (verboseLevel>2) reportArgs("maxDeltaP", aCluster);

  G4LorentzVector pcms = getClusterMomentum(aCluster);
  G4ThreeVector boost = pcms.boostVector();

  G4double dp, maxDP = -1.;
  for (size_t i=0; i<aCluster.size(); i++) {
    const G4InuclElementaryParticle& nucl = getHadron(aCluster[i]);
    if ((dp = nucl.getMomentum().boost(-boost).rho()) > maxDP) maxDP = dp;
  }

  if (verboseLevel>2) G4cout << " maxDP = " << maxDP << G4endl;

  return maxDP;
}


// Compute "cluster type code" as sum of nucleon codes

G4int G4CascadeCoalescence::
clusterType(const ClusterCandidate& aCluster) const {
  G4int type = 0;
  for (size_t i=0; i<aCluster.size(); i++) {
    const G4InuclElementaryParticle& had = getHadron(aCluster[i]);
    type += had.nucleon() ? had.type() : 0;
  }

  return type;
}

// Compute "cluster hash" as chain of indices

size_t G4CascadeCoalescence::
clusterHash(const ClusterCandidate& aCluster) const {
  G4int hash = 0;
  for (size_t i=0; i<aCluster.size(); i++) {
    hash = hash*1000 + aCluster[i];	// FIXME:  Use hadron list size instead?
  }

  return hash;
}


// Create cluster candidate with listed indices

void G4CascadeCoalescence::fillCluster(size_t idx1, size_t idx2) {
  thisCluster.clear();
  thisCluster.push_back(idx1);
  thisCluster.push_back(idx2);
}

void G4CascadeCoalescence::fillCluster(size_t idx1, size_t idx2, size_t idx3) {
  thisCluster.clear();
  thisCluster.push_back(idx1);
  thisCluster.push_back(idx2);
  thisCluster.push_back(idx3);
}

void G4CascadeCoalescence::fillCluster(size_t idx1, size_t idx2, size_t idx3,
				      size_t idx4) {
  thisCluster.clear();
  thisCluster.push_back(idx1);
  thisCluster.push_back(idx2);
  thisCluster.push_back(idx3);
  thisCluster.push_back(idx4);
}



// Make sure all candidates in cluster are nucleons

bool G4CascadeCoalescence::allNucleons(const ClusterCandidate& clus) const {
  bool result = true;
  for (size_t i=0; i<clus.size(); i++)
    result &= getHadron(clus[0]).nucleon();
  return result;
}


// Determine if collection of nucleons can form a light ion

bool G4CascadeCoalescence::goodCluster(const ClusterCandidate& clus) const {
  if (verboseLevel>2) reportArgs("goodCluster",clus);

  if (!allNucleons(clus)) return false;

  if (clus.size() == 2)					// Deuterons (pn)
    return (clusterType(clus) == 3 && maxDeltaP(clus) < dpMaxDoublet);

  if (clus.size() == 3)					// Tritons or He-3
    return ((clusterType(clus) == 4 || clusterType(clus) == 5)	// ppn OR pnn
	    && maxDeltaP(clus) < dpMaxTriplet);

  if (clus.size() == 4)					// Alphas (ppnn)
    return (clusterType(clus) == 6 && maxDeltaP(clus) < dpMaxAlpha);

  return false;
}



// Convert candidate nucleon set into output nucleus

bool G4CascadeCoalescence::makeLightIon(const ClusterCandidate& aCluster) {
  if (verboseLevel>1) reportArgs("makeLightIon",aCluster);

  thisLightIon.clear();		// Initialize nucleus buffer before filling

  if (aCluster.size()<2) return false;		// Sanity check

  G4int A = aCluster.size();
  G4int Z = -1;

  G4int type = clusterType(aCluster);
  if (A==2 && type==3) Z = 1;		// Deuteron (pn)
  if (A==3 && type==5) Z = 1;		// Triton (pnn)
  if (A==3 && type==4) Z = 2;		// He-3 (ppn)
  if (A==4 && type==6) Z = 2;		// He-4/alpha (ppnn)

  if (Z < 0) return false;		// Invalid cluster content

  // NOTE:  Four-momentum will not be conserved due to binding energy
  thisLightIon.fill(getClusterMomentum(aCluster), A, Z, 0.,
		    G4InuclParticle::Coalescence);

  if (verboseLevel>1) reportResult("makeLightIon output",thisLightIon);
  return true;
}


// Report cluster arguments for validation

void G4CascadeCoalescence::reportArgs(const G4String& name,
				      const ClusterCandidate& aCluster) const {
  G4cout << " >>> G4CascadeCoalescence::" << name << " ";
  std::copy(aCluster.begin(), aCluster.end(),
	    std::ostream_iterator<int>(G4cout, " "));
  G4cout << G4endl;

  for (size_t i=0; i<aCluster.size(); i++)
    G4cout << getHadron(aCluster[i]) << G4endl;
}

void G4CascadeCoalescence::reportResult(const G4String& name,
					const G4InuclNuclei& nucl) const {
  G4cout << " >>> G4CascadeCoalescence::" << name << G4endl << nucl << G4endl;
}
