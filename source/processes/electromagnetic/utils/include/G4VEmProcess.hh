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
// $Id: G4VEmProcess.hh,v 1.8 2004-08-06 11:30:59 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// -------------------------------------------------------------------
//
// GEANT4 Class header file
//
//
// File name:     G4VEmProcess
//
// Author:        Vladimir Ivanchenko
//
// Creation date: 01.10.2003
//
// Modifications:
// 30-06-04 make destructor virtual (V.Ivanchenko)
//
//
// Class Description:
//
// It is the unified Discrete process

// -------------------------------------------------------------------
//

#ifndef G4VEmProcess_h
#define G4VEmProcess_h 1

#include "G4VDiscreteProcess.hh"
#include "globals.hh"
#include "G4Material.hh"
#include "G4MaterialCutsCouple.hh"
#include "G4Track.hh"
#include "G4EmModelManager.hh"
#include "G4UnitsTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleChangeForLoss.hh"

class G4Step;
class G4VEmModel;
class G4DataVector;
class G4VParticleChange;
class G4PhysicsTable;
class G4PhysicsVector;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class G4VEmProcess : public G4VDiscreteProcess
{
public:

  G4VEmProcess(const G4String& name,
                     G4ProcessType type = fElectromagnetic);

  virtual ~G4VEmProcess();

  G4VParticleChange* PostStepDoIt(const G4Track&, const G4Step&);

  virtual std::vector<G4DynamicParticle*>* SecondariesPostStep(
                                   G4VEmModel*,
                             const G4MaterialCutsCouple*,
                             const G4DynamicParticle*) = 0;

  virtual G4bool IsApplicable(const G4ParticleDefinition& p) = 0;
    // True for all charged particles

  virtual void BuildPhysicsTable(const G4ParticleDefinition&);
  // Build physics table during initialisation

  virtual void PrintInfoDefinition();
  // Print out of the class parameters

  G4PhysicsTable* BuildLambdaTable();

  void SetLambdaBinning(G4int nbins);
  G4int LambdaBinning() const;
  // Binning for lambda table

  void SetMinKinEnergy(G4double e);
  G4double MinKinEnergy() const;
  // Min kinetic energy for tables

  void SetMaxKinEnergy(G4double e);
  G4double MaxKinEnergy() const;
  // Max kinetic energy for tables

  G4bool StorePhysicsTable(G4ParticleDefinition*,
                     const G4String& directory,
                           G4bool ascii = false);
    // Store PhysicsTable in a file.
    // Return false in case of failure at I/O

  G4bool RetrievePhysicsTable(G4ParticleDefinition*,
                        const G4String& directory,
                              G4bool ascii);
    // Retrieve Physics from a file.
    // (return true if the Physics Table can be build by using file)
    // (return false if the process has no functionality or in case of failure)
    // File name should is constructed as processName+particleName and the
    // should be placed under the directory specifed by the argument.

  void AddEmModel(G4int, G4VEmModel*, const G4Region* region = 0);
  // Add EM model coupled for the region

  void UpdateEmModel(const G4String&, G4double, G4double);
  // Define new energy range for the model identified by the name

  G4double GetLambda(G4double& kinEnergy, const G4MaterialCutsCouple* couple);
  // It returns the Lambda of the process

  const G4PhysicsTable* LambdaTable() const;

  G4double MicroscopicCrossSection(G4double kineticEnergy,
                             const G4MaterialCutsCouple* couple);
  // It returns the cross section of the process for energy/ material

  G4double MeanFreePath(     const G4Track& track,
                                   G4double previousStepSize,
                                   G4ForceCondition* condition);

  G4double PostStepGetPhysicalInteractionLength(
                             const G4Track& track,
                                   G4double   previousStepSize,
                                   G4ForceCondition* condition);

  const G4ParticleDefinition* Particle() const;
  const G4ParticleDefinition* SecondaryParticle() const;
  
  void SetDEDXTable(G4PhysicsTable*);

  virtual void ActivateFluorescence(G4bool, const G4Region* r = 0);
  virtual void ActivateAugerElectronProduction(G4bool, const G4Region* r = 0);

  G4VEmModel* SelectModelForMaterial(G4double kinEnergy, size_t& idxRegion) const;

  void SetIntegral(G4bool val);
  G4bool IsIntegral() const;

protected:

  void SetParticle(const G4ParticleDefinition* p);
  void SetSecondaryParticle(const G4ParticleDefinition* p);

  virtual G4double GetMeanFreePath(const G4Track& track,
                                         G4double previousStepSize,
                                         G4ForceCondition* condition);

  virtual G4PhysicsVector* LambdaPhysicsVector(const G4MaterialCutsCouple*) = 0;

  virtual G4double MinPrimaryEnergy(const G4ParticleDefinition*,
                                    const G4Material*, G4double cut) = 0;

  G4VEmModel* SelectModel(G4double& kinEnergy);

  size_t CurrentMaterialCutsCoupleIndex() const {return currentMaterialIndex;};

  void ResetNumberOfInteractionLengthLeft();

private:

  void Initialise();

  void Clear();

  void DefineMaterial(const G4MaterialCutsCouple* couple);

  G4double GetLambda(G4double kinEnergy);

  void ComputeLambda(G4double kinEnergy);

  // hide  assignment operator

  G4VEmProcess(G4VEmProcess &);
  G4VEmProcess & operator=(const G4VEmProcess &right);

// =====================================================================

protected:

  G4ParticleChangeForLoss      fParticleChange;

private:

  G4EmModelManager*            modelManager;

  // tables and vectors
  G4PhysicsTable*              theDEDXTable;
  G4PhysicsTable*              theLambdaTable;
  G4double*                    theEnergyOfCrossSectionMax;
  G4double*                    theCrossSectionMax;

  const G4ParticleDefinition*  particle;
  const G4ParticleDefinition*  secondaryParticle;

  const std::vector<G4double>* theCutsGamma;
  const std::vector<G4double>* theCutsElectron;
  const std::vector<G4double>* theCutsPositron;

  G4int                        nLambdaBins;

  G4double                     minKinEnergy;
  G4double                     maxKinEnergy;
  G4double                     lambdaFactor;

  // cash
  const G4Material*            currentMaterial;
  const G4MaterialCutsCouple*  currentCouple;
  size_t                       currentMaterialIndex;

  G4double                     mfpKinEnergy;
  G4double                     preStepKinEnergy;
  G4double                     preStepLambda;
  G4double                     preStepMFP;

  G4bool                       integral;
  G4bool                       meanFreePath;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline void G4VEmProcess::DefineMaterial(const G4MaterialCutsCouple* couple)
{
  if(couple != currentCouple) {
    currentCouple   = couple;
    currentMaterial = couple->GetMaterial();
    currentMaterialIndex = couple->GetIndex();
    if(integral && (!meanFreePath || preStepKinEnergy < mfpKinEnergy))
      ResetNumberOfInteractionLengthLeft();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline G4double G4VEmProcess::GetLambda(G4double& kineticEnergy,
                                  const G4MaterialCutsCouple* couple)
{
  DefineMaterial(couple);
  G4double x = 0.0;
  if(theLambdaTable) x = GetLambda(kineticEnergy);
  return x;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline G4double G4VEmProcess::GetLambda(G4double e)
{
  G4bool b;
  return (((*theLambdaTable)[currentMaterialIndex])->GetValue(e, b));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline void G4VEmProcess::ComputeLambda(G4double e)
{
  meanFreePath  = false;
  G4double emax = theEnergyOfCrossSectionMax[currentMaterialIndex];
  if (e <= emax) {
    mfpKinEnergy  = 0.0;
    preStepLambda = GetLambda(e);
  } else {
    e *= lambdaFactor;
    if(e > emax) {
      mfpKinEnergy = e;
      preStepLambda = GetLambda(e);
    } else {
      mfpKinEnergy = emax;
      preStepLambda = theCrossSectionMax[currentMaterialIndex];
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline G4double G4VEmProcess::GetMeanFreePath(const G4Track& track,
                                                    G4double,
                                                    G4ForceCondition* condition)
{
  *condition = NotForced;
  preStepKinEnergy = track.GetKineticEnergy();
  DefineMaterial(track.GetMaterialCutsCouple());
  if (meanFreePath) {
    if (integral) ComputeLambda(preStepKinEnergy);
    else          preStepLambda = GetLambda(preStepKinEnergy);
    if(0.0 < preStepLambda) preStepMFP = 1.0/preStepLambda;
    else                    preStepMFP = DBL_MAX;
  }
  //  G4cout<<GetProcessName()<<": e= "<<preStepKinEnergy<<" mfp= "<<preStepMFP<<G4endl;
  return preStepMFP;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline G4double G4VEmProcess::PostStepGetPhysicalInteractionLength(
                             const G4Track& track,
                             G4double   previousStepSize,
                             G4ForceCondition* condition)
{
  double pil = G4VDiscreteProcess::PostStepGetPhysicalInteractionLength(track, 
                                                                        previousStepSize,
                                                                        condition);
  if(integral && theDEDXTable && preStepKinEnergy > mfpKinEnergy) {
    G4bool b;
    G4double dedx = ((*theDEDXTable)[currentMaterialIndex])->GetValue(preStepKinEnergy, b);
    if(dedx > 0.0) pil = std::min(pil,(preStepKinEnergy - mfpKinEnergy)/dedx);
  }
  return pil;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline G4VEmModel* G4VEmProcess::SelectModel(G4double& kinEnergy)
{
  return modelManager->SelectModel(kinEnergy, currentMaterialIndex);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline G4VEmModel* G4VEmProcess::SelectModelForMaterial(
                                   G4double kinEnergy, size_t& idxRegion) const
{
  return modelManager->SelectModel(kinEnergy, idxRegion);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline void G4VEmProcess::ResetNumberOfInteractionLengthLeft()
{
  meanFreePath = true;
  G4VProcess::ResetNumberOfInteractionLengthLeft();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline const G4ParticleDefinition* G4VEmProcess::Particle() const
{
  return particle;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline const G4ParticleDefinition* G4VEmProcess::SecondaryParticle() const
{
  return secondaryParticle;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif
