// This code implementation is the intellectual property of
// the RD44 GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: G4LowEnergyPhotoElectric.cc,v 1.2 1999-03-27 19:22:36 aforti Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// --------------------------------------------------------------
//      GEANT 4 class implementation file
//      CERN Geneva Switzerland
//
//      For information related to this code contact:
//      CERN, IT Division, ASD group
//      History: first implementation, based on object model of
//      2nd December 1995, G.Cosmo
//      ------------ G4LowEnergyPhotoElectric physics process --------
//                   by Michel Maire, April 1996
// **************************************************************
// 12-06-96, Added SelectRandomAtom() method, by M.Maire
// 21-06-96, SetCuts implementation, M.Maire
// 17-09-96, PartialSumSigma(i)
//           split of ComputeBindingEnergy, M.Maire
// 08-01-97, crossection table + meanfreepath table, M.Maire
// 13-03-97, adapted for the new physics scheme, M.Maire
// 28-03-97, protection in BuildPhysicsTable, M.Maire
// 04-06-98, in DoIt, secondary production condition: range>min(threshold,safety)
// --------------------------------------------------------------

// This Class Header
#include "G4LowEnergyPhotoElectric.hh"

// Collaborating Class Headers
#include "G4EnergyLossTables.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Epdl97File.hh"
#include "G4Epdl89File.hh"
#include "G4EpdlTables.hh"
#include "G4PhysicsFreeVector.hh" 
//#include "G4FastVector.hh"
#include "G4Step.hh"
#include "Randomize.hh" 

#include "CLHEP/String/Strings.h"
//#include <rw/tvordvec.h>
//#include <rw/tvordvec.h>

// constructor
 
G4LowEnergyPhotoElectric::G4LowEnergyPhotoElectric(const G4String& processName)
  : G4VDiscreteProcess(processName),             // initialization
    LowestEnergyLimit (100*eV),
    HighestEnergyLimit(100*GeV),
    NumbBinTable(100)
{
   if (verboseLevel>0) {
     G4cout << GetProcessName() << " is created "<< endl;
     G4cout << "LowestEnergy: " << LowestEnergyLimit/keV << "keV ";
     G4cout << "HighestEnergy: " << HighestEnergyLimit/MeV << "MeV " << endl;
   }
   theCrossSectionTable = 0;
   theBindingEnergyTable = 0;
   theMeanFreePathTable = 0;
   theFluorTransitionTable = 0;
   theAugerTransitionTable = 0;
   PEManager = new HBookFile("lephotoel.hbook", 100);
}
 
// destructor
 
G4LowEnergyPhotoElectric::~G4LowEnergyPhotoElectric()
{
   if (theCrossSectionTable) {
      theCrossSectionTable->clearAndDestroy();
      delete theCrossSectionTable;
   }

   if (theBindingEnergyTable) {
      theBindingEnergyTable->clearAndDestroy();
      delete theBindingEnergyTable;
   }

   if (theMeanFreePathTable) {
      theMeanFreePathTable->clearAndDestroy();
      delete theMeanFreePathTable;
   }

   if (theFluorTransitionTable) {
      theFluorTransitionTable->clearAndDestroy();
      delete theFluorTransitionTable;
   }

   if (theAugerTransitionTable) {
      theAugerTransitionTable->clearAndDestroy();
      delete theAugerTransitionTable;
   }
   delete PEManager;
}
 
 
// methods.............................................................................
 
void G4LowEnergyPhotoElectric::BuildPhysicsTable(const G4ParticleDefinition& PhotonType)

// Build microscopic cross section table and mean free path table
{

   // Build microscopic cross section tables for the Photo Electric Effect
   BuildCrossSectionTable();

   // Build Binding Energy Table
   BuildBindingEnergyTable();

   // Build mean free path table for the Compton Scattering process
   BuildMeanFreePathTable();


}

G4VParticleChange* G4LowEnergyPhotoElectric::PostStepDoIt(const G4Track& aTrack, const G4Step&  aStep){

  cout<<"************** Starting PE DoIt ****************"<<endl;
  // incoming particle initialization
  //  aParticleChange.Initialize(aTrack);
  G4Material* aMaterial = aTrack.GetMaterial();

  const G4DynamicParticle* aDynamicPhoton = aTrack.GetDynamicParticle();
  const G4double PhotonEnergy = aDynamicPhoton->GetKineticEnergy();
  const G4ParticleMomentum PhotonDirection = aDynamicPhoton->GetMomentumDirection();
   
  // select randomly one element constituing the material.
  G4Element* anElement = SelectRandomAtom(aDynamicPhoton, aMaterial);

  // PAY ATTENTION TO THE MEANING OF THIS NUMBER
  G4int AtomNum = anElement->GetZ();

  // Select the subshell ionized the first one 
  // with binding energy less than the photon energy
  G4PhysicsVector* theBindEnVec = (*theBindingEnergyTable)(AtomNum-1);
  
  G4int g = 0;

  cout<<"PE the number of shells for "<<anElement->GetName()<<" is "<<theBindEnVec->GetVectorLength()<<endl;
  while(g < theBindEnVec->GetVectorLength() && PhotonEnergy < (*theBindEnVec)(g)){
    g++;
  }

  G4int thePrimaryShell = theBindEnVec->GetLowEdgeEnergy(g);
  cout<<"The first shell is the number "<<g<<" of the list: "<<thePrimaryShell<<endl;
  cout<<"It's energy: "<<(*theBindEnVec)(g)<<endl;

  // Create lists of pointers to DynamicParticles (photons and electrons)
  RWTPtrSlist<G4DynamicParticle> photvec;
  G4int photInd = 0; 
  RWTPtrSlist<G4DynamicParticle> elecvec;
  G4int elecInd = 0; 

  // primary outcoming electron
  G4double ElecKineEnergy = (PhotonEnergy - (*theBindEnVec)(g))*MeV;
  G4double theEnergyDeposit = (PhotonEnergy - ElecKineEnergy)*MeV;

  if (G4EnergyLossTables::GetRange(G4Electron::Electron(),ElecKineEnergy,aMaterial)
      >= min(G4Electron::GetCuts(), aStep.GetPostStepPoint()->GetSafety()) ){
    
    // the electron is created in the direction of the incident photon ...  
    G4DynamicParticle* aElectron = new G4DynamicParticle (G4Electron::Electron(), 
							  PhotonDirection, ElecKineEnergy) ;
    elecvec.append(aElectron);

    // load the transition probability table for the element
    // theTable[i][j][k] 
    // i = subshell, j = type of information (second shell, transition energy , 
    // transition probability), k = previous vectors.
    BuildFluorTransitionTable(AtomNum);
    
    G4bool ThereAreShells = TRUE;
    while(ThereAreShells == TRUE){
      
      // Select the second transition from another subshell
      // fluorPar[0] = SubShell 
      // fluorPar[1] = Sec SubShell (if there is), 
      // fluorPar[2] = Transition Probability
      // fluorPar[3] = Transition Energy
      // the same for augerPar
      cout<<"the Primary Shell before SelectRandomTransition: "<<thePrimaryShell<<endl;

      G4double fluorPar[4] = {0};
      ThereAreShells = SelectRandomTransition(thePrimaryShell, fluorPar, theFluorTransitionTable);
      cout<<"the Primary Shell after SelectRandomTransition: "<<thePrimaryShell<<endl;
      if(ThereAreShells != 0){

	// Daugther dynamic particle
	G4DynamicParticle* newPart;
	
	// Direction of the outcoming particle isotropic selection
	G4double newcosTh = 1-2*G4UniformRand();
	G4double newsinTh = sqrt(1-newcosTh*newcosTh);
	G4double newPhi = twopi*G4UniformRand();
	
	G4double dirx, diry, dirz;
	dirz = newcosTh;
	diry = newsinTh*cos(newPhi);
	dirx = newsinTh*sin(newPhi);
	G4ThreeVector newPartDirection(dirx, diry, dirz);
	
	newPart = new G4DynamicParticle (G4Gamma::Gamma(), newPartDirection, fluorPar[3]) ;
	photvec.append(newPart);
	thePrimaryShell = fluorPar[0];
	theEnergyDeposit -= fluorPar[3]*MeV;
	
	cout<<endl
	    <<"PE Transition Shell: "<<thePrimaryShell<<endl
	    <<"PE DeltaE: "<<fluorPar[3]<<endl
	    <<"PE Initial Egamma: "<<PhotonEnergy<<" First Electron T_e: "<<ElecKineEnergy<<endl
	    <<"PE Energy Deposit after the last Transition "<<theEnergyDeposit<<endl;
	cout<<"****************************************************"<<endl;
      }
    }

    //controllare se il setnumberofsecondaries  si puo' cambiare
    G4int numOfElec = elecvec.entries(), numOfPhot = photvec.entries();
    G4int numOfDau = numOfElec + numOfPhot;
    cout<<"PE Number of secondaries: "<<numOfDau
	<<", Number of elec: "<<numOfElec
	<<", Number of phot: "<<numOfPhot<<endl;

    aParticleChange.SetNumberOfSecondaries(numOfDau);
    
    for( G4int l = 0; l<numOfElec; l++ ){
      
      aParticleChange.AddSecondary(elecvec[l]);
    }
    
    for(l = 0; l < numOfPhot; l++) {

      aParticleChange.AddSecondary(photvec[l]); 
    }
    
    photvec.clear();
    elecvec.clear();
    cout<<"END OF FLUORESCENCE"<<endl;
  }
  
  else{
    
    ElecKineEnergy = 0. ;
    aParticleChange.SetNumberOfSecondaries(0) ;
  }
  // Kill the incident photon 
  aParticleChange.SetMomentumChange( 0., 0., 0. );
  aParticleChange.SetEnergyChange( 0. );
  aParticleChange.SetLocalEnergyDeposit(theEnergyDeposit) ;  
  aParticleChange.SetStatusChange( fStopAndKill ) ; 
  
  // Reset NbOfInteractionLengthLeft and return aParticleChange
  return G4VDiscreteProcess::PostStepDoIt( aTrack, aStep );
}

void G4LowEnergyPhotoElectric::BuildCrossSectionTable(){
 
  if (theCrossSectionTable) {
    
    theCrossSectionTable->clearAndDestroy(); delete theCrossSectionTable; 
  }
  cout<<"************** PE CS ****************"<<endl;
  G4int par[4] = {73, 0, 0, 0}; G4String name("epdl97");
  G4Epdl97File File(name,par);
  G4EpdlTables table(File);
  table.FillDataTable();
  theCrossSectionTable = new G4PhysicsTable(*(table.GetFstDataTable())) ;

}

void G4LowEnergyPhotoElectric::BuildBindingEnergyTable(){
 
  if (theBindingEnergyTable) {
    
    theBindingEnergyTable->clearAndDestroy(); delete theBindingEnergyTable; 
  }

  cout<<"************** PE BE ****************"<<endl;
  G4int par[4] = {91, 913, 0, 0}; G4String name("eadl.asc");
  G4Epdl89File File(name,par);
  G4EpdlTables table(File);
  table.FillDataTable();
  theBindingEnergyTable = new G4PhysicsTable(*(table.GetFstDataTable())) ;

}

void G4LowEnergyPhotoElectric::BuildFluorTransitionTable(G4int atomNum){
 
  if (theFluorTransitionTable) {
  
    theFluorTransitionTable->clearAndDestroy(); delete theFluorTransitionTable;
  }
  cout<<"************** PE FL ****************"<<endl;

  G4String name("eadl.asc");
  G4int par[4] = {92, 931, 91, 0}; 
  G4Epdl89File File(name,par);

  G4EpdlTables table(File);
  table.FillTheTable(atomNum);
  theFluorTransitionTable = table.GetGlobalList() ;
}

void G4LowEnergyPhotoElectric::BuildAugerTransitionTable(G4int  atomNum){
 
  if (theAugerTransitionTable) {
    
    theAugerTransitionTable->clearAndDestroy(); delete theAugerTransitionTable; 
  }

  G4int par[4] = {92, 932, 91, 0}; G4String name("eadl.asc");
  G4Epdl89File File(name,par);
  G4EpdlTables table(File);
  table.FillTheTable(atomNum);
  theAugerTransitionTable = table.GetGlobalList() ;
}

void G4LowEnergyPhotoElectric::BuildMeanFreePathTable(){

  if (theMeanFreePathTable) {
    theMeanFreePathTable->clearAndDestroy(); delete theMeanFreePathTable; }

  // material
  G4double NumbOfMaterials = G4Material::GetNumberOfMaterials();
  const G4MaterialTable* theMaterialTable = G4Material::GetMaterialTable() ;
  G4Material* material;

  // MeanFreePath 
  G4double LowEdgeEnergy, Value;
  theMeanFreePathTable = new G4PhysicsTable(NumbOfMaterials);
  G4PhysicsLogVector* ptrVector;

  for ( G4int J = 0 ; J < NumbOfMaterials; J++ ) { // For each material 
  
    //create physics vector then fill it ....
    ptrVector = new  G4PhysicsLogVector(LowestEnergyLimit, HighestEnergyLimit, NumbBinTable);
    
    material = (*theMaterialTable)(J);
    const G4ElementVector* theElementVector = material->GetElementVector();
    const G4double* theAtomNumDensityVector = material->GetAtomicNumDensityVector();   
    
    for ( G4int i = 0 ; i < NumbBinTable ; i++ ){ 
      //For each energy
      
      LowEdgeEnergy = ptrVector->GetLowEdgeEnergy(i);
      
      const G4double BigPath= DBL_MAX;
      G4double SIGMA = 0 ;
      
      for ( G4int k=0 ; k < material->GetNumberOfElements() ; k++ ){ 
	// For each element            

	G4double interCrsSec = DataLogInterpolation(LowEdgeEnergy, (*theElementVector)(k)->GetZ(), theCrossSectionTable)*barn;
	SIGMA += theAtomNumDensityVector[k]*interCrsSec;
      }       
      
      Value = SIGMA<=0.0 ? BigPath : 1./SIGMA ;
      
      ptrVector->PutValue( i , Value ) ;
    }
    
    theMeanFreePathTable->insertAt( J , ptrVector ) ;
  }
}

G4Element*
G4LowEnergyPhotoElectric::SelectRandomAtom(const G4DynamicParticle* aDynamicPhoton, G4Material* aMaterial){

  // select randomly 1 element within the material
  G4double GammaEnergy = aDynamicPhoton->GetKineticEnergy();
  const G4int NumberOfElements = aMaterial->GetNumberOfElements();
  const G4ElementVector* theElementVector = aMaterial->GetElementVector();
  if (NumberOfElements == 1) return (*theElementVector)(0);

  const G4double* theAtomNumDensityVector = aMaterial->GetAtomicNumDensityVector();

  G4double PartialSumSigma = 0. ;
  G4double rval = G4UniformRand()/MeanFreePath;
 
  for ( G4int i=0 ; i < NumberOfElements ; i++ ){ 

    G4double crossSection;
    if (GammaEnergy <  LowestEnergyLimit)
      crossSection = 0. ;
    else {
      if (GammaEnergy > HighestEnergyLimit) GammaEnergy = 0.99*HighestEnergyLimit ;
      crossSection = DataLogInterpolation(GammaEnergy, (*theElementVector)(i)->GetZ(), theCrossSectionTable)*barn;
    }

    PartialSumSigma += theAtomNumDensityVector[i] * crossSection;
    if (rval <= PartialSumSigma) return ((*theElementVector)(i));
  }
  G4cout << " WARNING !!! - The Material '"<< aMaterial->GetName()
       << "' has no elements, NULL pointer returned." << endl;
  return 0;
}


G4bool G4LowEnergyPhotoElectric::SelectRandomTransition(G4int thePrimShell, 
							G4double* TransParam,
							RWTPtrSlist< RWTPtrSlist<G4DataVector> >* TransitionTable){
  
  G4int SubShellCol, SecShellCol, ProbCol, EnergyCol;

  // too check when the subshell are finished
  G4bool ColIsFull = TRUE;
  
  if(TransParam[0] == 0){  
    
    SubShellCol = 0; ProbCol = 1; EnergyCol = 2;
  }
  
  if(TransParam[0] == 1){  
    
    G4int SubShellCol = 0; SecShellCol = 1; ProbCol = 2; EnergyCol = 3;
  }
  
  G4int ShellNum = 0;
  G4double TotalSum = 0; 
  while(thePrimShell != (*(*(*TransitionTable)[ShellNum])[0])[0]){
  
    ShellNum++;
    if(ShellNum == TransitionTable->entries()-1){
      break;
    }
  }

  cout<<"Shell index: "<<ShellNum<<" TransTableLength: "<<TransitionTable->entries()<<endl;

  if(ShellNum != TransitionTable->entries()-1) {
    
    //TransProb start from 1 because the first element of the list is the primary shall id number
    for(G4int TransProb = 1; TransProb < (*(*TransitionTable)[ShellNum])[ProbCol]->length(); TransProb++){ 
      
      TotalSum += (*(*(*TransitionTable)[ShellNum])[ProbCol])[TransProb];
    }
    
    G4double PartialProb = G4UniformRand()*TotalSum;
    G4double PartSum = 0;
    
    TransProb = 1; 
    while(TransProb < (*(*TransitionTable)[ShellNum])[ProbCol]->length()){
      
      PartSum += (*(*(*TransitionTable)[ShellNum])[ProbCol])[TransProb];
      
      if(PartialProb <= PartSum){
	
	TransParam[0] = (*(*(*TransitionTable)[ShellNum])[SubShellCol])[TransProb];

	// This if will be needed when Auger Effect will be added TransPar[i] initialized 
	// to 0 for fluorescence and to 1 for auger effect at the moment this distinction is 
	// tricky

	if(TransParam[1] == 0){
	  
	  TransParam[1] = 0;
	}
	else{
	  
	  TransParam[1] = (*(*(*TransitionTable)[ShellNum])[SecShellCol])[TransProb];
	}
	
	TransParam[2] = (*(*(*TransitionTable)[ShellNum])[ProbCol])[TransProb];
	TransParam[3] = (*(*(*TransitionTable)[ShellNum])[EnergyCol])[TransProb];
	break;
      }
      
      TransProb++;
    }

    cout<<"Transition Shell ID: "<<(*(*(*TransitionTable)[ShellNum])[SubShellCol])[TransProb]<<endl;
    cout<<"Transition Probability: "<<TransProb<<" Probability Column Length: "
	<<(*(*TransitionTable)[ShellNum])[ProbCol]->length()<<endl;
    if(TransProb == (*(*TransitionTable)[ShellNum])[ProbCol]->length()-1) {
      cout<<"It's the last shell"<<endl;
      ColIsFull = FALSE;
    }
  }
  else{
   
    cout<<"There are no more shells"<<endl;
    ColIsFull = FALSE;
  }
  
  return ColIsFull;
}






