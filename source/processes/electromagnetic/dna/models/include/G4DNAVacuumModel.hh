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

#ifndef G4DNAVacuumModel_h
#define G4DNAVacuumModel_h 1

#include "G4DNACrossSectionDataSet.hh"
#include "G4Electron.hh"
#include "G4LogLogInterpolation.hh"
#include "G4NistManager.hh"
#include "G4ParticleChangeForGamma.hh"
#include "G4ProductionCutsTable.hh"
#include "G4Proton.hh"
#include "G4VDNAModel.hh"

/*!
 * \brief The G4DNAVacuumModel class
 * Implementation of the vacuum model allowing the user to use G4_Galactic as void in a
 * Geant4-DNA simulation.
 */
class G4DNAVacuumModel : public G4VDNAModel
{
 public:
  /*!
   * \brief G4DNAVacuumModel
   * Constructor
   * \param applyToMaterial
   * \param p
   * \param nam
   */
  G4DNAVacuumModel(const G4String& applyToMaterial = "all", const G4ParticleDefinition* p = nullptr,
    const G4String& nam = "DNAPTBVacuumModel");

  /*!
   * \brief ~G4DNAVacuumModel
   * Destructor
   */
  ~G4DNAVacuumModel() override;

  G4DNAVacuumModel(const G4DNAVacuumModel&) = delete;  // prevent copy-construction
  G4DNAVacuumModel& operator=(const G4DNAVacuumModel& right) = delete;  // prevent assignement

  /*!
   * \brief Initialise
   * Registers the G4_Galactic material as "void material" for every particle
   */
  void Initialise(const G4ParticleDefinition*, const G4DataVector&) override;

  /*!
   * \brief CrossSectionPerVolume
   * \param material
   * \param materialName
   * \param p
   * \param ekin
   * \param emin
   * \param emax
   * \return cross section value
   */
  G4double CrossSectionPerVolume(const G4Material* material,
    const G4ParticleDefinition* p, G4double ekin, G4double emin, G4double emax) override;

  /*!
   * \brief SampleSecondaries
   * \param materialName
   * \param particleChangeForGamma
   * \param tmin
   * \param tmax
   */
  void SampleSecondaries(std::vector<G4DynamicParticle*>*, const G4MaterialCutsCouple*, const G4DynamicParticle*, G4double tmin, G4double tmax) override;

 private:
  G4int verboseLevel = 0;  ///< verbose level

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif
