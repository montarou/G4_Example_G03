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
// GEANT4 physics class: G4PhotoNuclearCrossSection -- header file
// Created: M.V. Kossov, CERN/ITEP(Moscow), 10-OCT-01
// The last update: M.V. Kossov, CERN/ITEP (Moscow) 17-May-02

#ifndef G4PhotoNuclearCrossSection_h
#define G4PhotoNuclearCrossSection_h 1

#include "G4VCrossSectionDataSet.hh"
#include "G4DynamicParticle.hh"
#include "G4Element.hh"
#include "G4ParticleTable.hh"
#include "G4NucleiProperties.hh"
#include <vector>

class G4PhotoNuclearCrossSection : public G4VCrossSectionDataSet
{
  public:

    G4PhotoNuclearCrossSection(const G4String& name = "PhotoNuclearXS");
    virtual ~G4PhotoNuclearCrossSection();

    virtual void CrossSectionDescription(std::ostream&) const;

    virtual G4bool
    IsIsoApplicable(const G4DynamicParticle* particle, G4int /*Z*/, G4int /*A*/,
                    const G4Element*, const G4Material*);

    virtual G4double
    GetIsoCrossSection(const G4DynamicParticle*, G4int /*Z*/, G4int /*A*/,
                       const G4Isotope*, const G4Element*, const G4Material*);

  private:

    G4int GetFunctions(G4double a, G4double* y, G4double* z);
    G4double EquLinearFit(G4double X, G4int N, const G4double X0,
                          const G4double XD, const G4double* Y);
    G4double ThresholdEnergy(G4int Z, G4int N);

  // Body
  private:

    static G4ThreadLocal G4int     lastN;   // The last N of calculated nucleus
    static G4ThreadLocal G4int     lastZ;   // The last Z of calculated nucleus
    static G4ThreadLocal G4double  lastSig; // Last value of the Cross Section
    static G4ThreadLocal G4double* lastGDR; // Pointer to the last array of GDR cross sections
    static G4ThreadLocal G4double* lastHEN; // Pointer to the last array of HEn cross sections
    static G4ThreadLocal G4double  lastE;   // Last used in the cross section Energy
    static G4ThreadLocal G4double  lastTH;  // Last value of the Energy Threshold (A-dependent)
    static G4ThreadLocal G4double  lastSP;  // Last value of the ShadowingPomeron (A-dependent)

    // Vector of pointers to the GDRPhotonuclearCrossSection
    static G4ThreadLocal std::vector <G4double*> *GDR_G4MT_TLS_;

    // Vector of pointers to the HighEnPhotonuclearCrossSect
    static G4ThreadLocal std::vector <G4double*> *HEN_G4MT_TLS_;
};

#endif
