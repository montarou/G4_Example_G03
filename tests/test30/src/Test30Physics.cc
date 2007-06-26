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
// -------------------------------------------------------------
//      GEANT 4 class 
//
//      History: based on object model of
//      ---------- Test30Physics -------
//                by Vladimir Ivanchenko, 12 March 2002 
// 
//    Modified:
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "Test30Physics.hh"

#include "G4UnitsTable.hh"
#include "Test30VSecondaryGenerator.hh"
#include "G4PhysicsTable.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTypes.hh"

#include "G4ProcessManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4PionZero.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4KaonPlus.hh"
#include "G4KaonMinus.hh"
#include "G4Proton.hh"
#include "G4AntiProton.hh"
#include "G4Neutron.hh"
#include "G4AntiNeutron.hh"
#include "G4NeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4GenericIon.hh"
#include "G4Alpha.hh"
#include "G4Deuteron.hh"
#include "G4Triton.hh"
#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"
#include "G4LEPionPlusInelastic.hh"
#include "G4LEPionMinusInelastic.hh"
#include "G4LEProtonInelastic.hh"
#include "G4LENeutronInelastic.hh"
#include "G4LEPionPlusInelastic.hh"
#include "G4LEPionMinusInelastic.hh"
#include "G4LEProtonInelastic.hh"

#include "G4StringChipsInterface.hh"
#include "G4PreCompoundModel.hh"
#include "G4ExcitationHandler.hh"
#include "G4BinaryCascade.hh"
#include "G4BinaryLightIonReaction.hh"
#include "G4CascadeInterface.hh"
#include "G4WilsonAbrasionModel.hh"

#include "G4TheoFSGenerator.hh"
#include "G4FTFModel.hh"
#include "G4ExcitedStringDecay.hh"
#include "G4LundStringFragmentation.hh"

#include "G4ElasticHadrNucleusHE.hh"
#include "G4LElastic.hh"
#include "G4HadronElastic.hh"
#include "G4CascadeElasticInterface.hh"
#include "G4QuasiElasticChannel.hh"
#include "G4GeneratorPrecompoundInterface.hh"
#include "G4StringChipsParticleLevelInterface.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

Test30Physics::Test30Physics()
{
  Initialise();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

Test30Physics::~Test30Physics()
{
  //  if(theProcess) delete theProcess;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void Test30Physics::Initialise()
{
  G4Electron::ElectronDefinition();
  G4Positron::PositronDefinition();
  G4MuonPlus::MuonPlusDefinition();
  G4MuonMinus::MuonMinusDefinition();

  G4NeutrinoE::NeutrinoEDefinition();
  G4AntiNeutrinoE::AntiNeutrinoEDefinition();
  G4NeutrinoMu::NeutrinoMuDefinition();
  G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();

  G4PionPlus::PionPlusDefinition();
  G4PionMinus::PionMinusDefinition();
  G4PionZero::PionZeroDefinition();
  G4Eta::EtaDefinition();
  G4EtaPrime::EtaPrimeDefinition();
  G4KaonPlus::KaonPlusDefinition();
  G4KaonMinus::KaonMinusDefinition();
  G4KaonZero::KaonZeroDefinition();
  G4AntiKaonZero::AntiKaonZeroDefinition();
  G4KaonZeroLong::KaonZeroLongDefinition();
  G4KaonZeroShort::KaonZeroShortDefinition();

	  // Strange barions
  G4Lambda::LambdaDefinition();
  G4AntiLambda::AntiLambdaDefinition();
  G4SigmaZero::SigmaZeroDefinition();
  G4AntiSigmaZero::AntiSigmaZeroDefinition();
  G4SigmaPlus::SigmaPlusDefinition();
  G4AntiSigmaPlus::AntiSigmaPlusDefinition();
  G4SigmaMinus::SigmaMinusDefinition();
  G4AntiSigmaMinus::AntiSigmaMinusDefinition();
  G4XiZero::XiZeroDefinition();
  G4AntiXiZero::AntiXiZeroDefinition();
  G4XiMinus::XiMinusDefinition();
  G4AntiXiMinus::AntiXiMinusDefinition();
  G4OmegaMinus::OmegaMinusDefinition();
  G4AntiOmegaMinus::AntiOmegaMinusDefinition();


  G4Proton::ProtonDefinition();
  G4AntiProton::AntiProtonDefinition();
  G4Neutron::NeutronDefinition();
  G4AntiNeutron::AntiNeutronDefinition();

  G4GenericIon::GenericIonDefinition();
  G4Deuteron::DeuteronDefinition();
  G4Alpha::AlphaDefinition();
  G4Triton::TritonDefinition();
  theProcess = 0;
  theDeExcitation = 0;
  thePreCompound = 0;
  theQuasiElastic = 0;
  hkmod = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4VProcess* Test30Physics::GetProcess(const G4String& gen_name,
		                      const G4String& part_name,
		                            G4Material* mat)
{
  G4cout <<  "Test30Physics entry" << G4endl;
  if(theProcess) delete theProcess;
  theProcess = 0;

  G4ProcessManager* man = 0;

  if(part_name == "proton")   man = new G4ProcessManager(G4Proton::Proton());
  else if(part_name == "pi+") man = new G4ProcessManager(G4PionPlus::PionPlus());
  else if(part_name == "pi-") man = new G4ProcessManager(G4PionMinus::PionMinus());
  else if(part_name == "neutron") man = new G4ProcessManager(G4Neutron::Neutron());
  else if(part_name == "deuteron") man = new G4ProcessManager(G4Deuteron::Deuteron());
  else if(part_name == "alpha") man = new G4ProcessManager(G4Alpha::Alpha());
  else if(part_name == "GenericIon") man = new G4ProcessManager(G4GenericIon::GenericIon());

  if(!man) return 0;

  theProcess = new Test30HadronProduction();
  G4cout <<  "Process is created; gen= " << gen_name << G4endl;

  // Physics list for the given run
  Test30VSecondaryGenerator* sg = 0;

  // Choose generator

  if(gen_name == "lepar") {
    if(part_name == "proton")   
      sg = new Test30VSecondaryGenerator(new G4LEProtonInelastic(),mat);
    else if(part_name == "pi+") 
      sg = new Test30VSecondaryGenerator(new G4LEPionPlusInelastic(),mat);
    else if(part_name == "pi-") 
      sg = new Test30VSecondaryGenerator(new G4LEPionMinusInelastic(),mat);
    else if(part_name == "neutron") 
      sg = new Test30VSecondaryGenerator(new G4LENeutronInelastic(),mat);
    theProcess->SetSecondaryGenerator(sg);
    man->AddDiscreteProcess(theProcess);

  } else if(gen_name == "CHIPS") {
    sg = new Test30VSecondaryGenerator(new G4StringChipsInterface(),mat);
    theProcess->SetSecondaryGenerator(sg);
    man->AddDiscreteProcess(theProcess);

  } else if(gen_name == "preCompound") {
    theDeExcitation = new G4ExcitationHandler();
    G4PreCompoundModel* pcm = new G4PreCompoundModel(theDeExcitation);
    thePreCompound = pcm;
    sg = new Test30VSecondaryGenerator(pcm,mat);
    theProcess->SetSecondaryGenerator(sg);
    man->AddDiscreteProcess(theProcess);

  } else if(gen_name == "binary") {
    G4BinaryCascade* hkm = new G4BinaryCascade();
    sg = new Test30VSecondaryGenerator(hkm, mat);
    theProcess->SetSecondaryGenerator(sg);
    man->AddDiscreteProcess(theProcess);

  } else if(gen_name == "ftfp") {

    G4TheoFSGenerator* theModel = new G4TheoFSGenerator;
    G4FTFModel* theStringModel = new G4FTFModel();
    G4GeneratorPrecompoundInterface* theCascade = 
      new G4GeneratorPrecompoundInterface;
    G4ExcitedStringDecay* theStringDecay = 
      new G4ExcitedStringDecay(new G4LundStringFragmentation());
    theStringModel->SetFragmentationModel(theStringDecay);

    theModel->SetTransport(theCascade);
    theQuasiElastic = new G4QuasiElasticChannel;
    theModel->SetQuasiElasticChannel(theQuasiElastic);
    theModel->SetHighEnergyGenerator(theStringModel);
    theModel->SetMinEnergy(GeV);

    sg = new Test30VSecondaryGenerator(theModel, mat);
    theProcess->SetSecondaryGenerator(sg);
    man->AddDiscreteProcess(theProcess);

  } else if(gen_name == "ftfc") {

    G4TheoFSGenerator* theModel = new G4TheoFSGenerator;
    G4FTFModel* theStringModel = new G4FTFModel();
    G4StringChipsParticleLevelInterface* theCascade = 
      new G4StringChipsParticleLevelInterface;
    G4ExcitedStringDecay* theStringDecay = 
      new G4ExcitedStringDecay(new G4LundStringFragmentation());
    theStringModel->SetFragmentationModel(theStringDecay);

    theModel->SetTransport(theCascade);
    theQuasiElastic = new G4QuasiElasticChannel;
    theModel->SetQuasiElasticChannel(theQuasiElastic);
    theModel->SetHighEnergyGenerator(theStringModel);
    theModel->SetMinEnergy(GeV);

    sg = new Test30VSecondaryGenerator(theModel, mat);
    theProcess->SetSecondaryGenerator(sg);
    man->AddDiscreteProcess(theProcess);

  } else if(gen_name == "ftfb") {

    G4TheoFSGenerator * model = new G4TheoFSGenerator;
    G4FTFModel * stringmodel= new G4FTFModel;
    G4BinaryCascade* cascade = new G4BinaryCascade();
    G4ExcitedStringDecay * stringDecay = 
      new G4ExcitedStringDecay(new G4LundStringFragmentation());
    model->SetHighEnergyGenerator(stringmodel);
    stringmodel->SetFragmentationModel(stringDecay);
    model->SetTransport(cascade);
    sg = new Test30VSecondaryGenerator(model, mat);
    theProcess->SetSecondaryGenerator(sg);
    man->AddDiscreteProcess(theProcess);

  } else if(gen_name == "binary_ion") {
    G4BinaryLightIonReaction* hkm = new G4BinaryLightIonReaction();
    sg = new Test30VSecondaryGenerator(hkm, mat);
    theProcess->SetSecondaryGenerator(sg);
    man->AddDiscreteProcess(theProcess);

  } else if(gen_name == "bertini") {
    G4CascadeInterface* hkm = new G4CascadeInterface();
    sg = new Test30VSecondaryGenerator(hkm, mat);
    theProcess->SetSecondaryGenerator(sg);
    man->AddDiscreteProcess(theProcess);

  } else if(gen_name == "LElastic") {
    G4LElastic* els = new G4LElastic();
    sg = new Test30VSecondaryGenerator(els, mat);
    theProcess->SetSecondaryGenerator(sg);
    man->AddDiscreteProcess(theProcess);

  } else if(gen_name == "Elastic") {
    G4HadronElastic* els = new G4HadronElastic();
    els->SetHEModelLowLimit(0.0);
    sg = new Test30VSecondaryGenerator(els, mat);
    theProcess->SetSecondaryGenerator(sg);
    man->AddDiscreteProcess(theProcess);

  } else if(gen_name == "HElastic") {
    G4ElasticHadrNucleusHE* els = new G4ElasticHadrNucleusHE();
    sg = new Test30VSecondaryGenerator(els, mat);
    theProcess->SetSecondaryGenerator(sg);
    man->AddDiscreteProcess(theProcess);

  } else if(gen_name == "BertiniElastic") {
    G4CascadeElasticInterface* els = new G4CascadeElasticInterface();
    sg = new Test30VSecondaryGenerator(els, mat);
    theProcess->SetSecondaryGenerator(sg);
    man->AddDiscreteProcess(theProcess);

  } else if(gen_name == "abrasion") {
    G4WilsonAbrasionModel* wam = new G4WilsonAbrasionModel();
    size_t ne = mat->GetNumberOfElements();
    const G4ElementVector* ev = mat->GetElementVector();
    for(size_t i=0; i<ne; i++) {
      G4Element* elm = (*ev)[i];
      wam->ActivateFor(elm);
    }    
    sg = new Test30VSecondaryGenerator(wam, mat);
    theProcess->SetSecondaryGenerator(sg);
    man->AddDiscreteProcess(theProcess);

  } else {
    G4cout << gen_name
           << " generator is unkown - no hadron production" << G4endl;
  }

  G4cout <<  "Secondary generator <"
         << gen_name << "> is initialized"
         << G4endl;
  return theProcess;

}	

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
  






