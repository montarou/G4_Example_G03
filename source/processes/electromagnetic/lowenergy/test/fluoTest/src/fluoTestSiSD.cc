//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "fluoTestSiSD.hh"
#include "fluoTestSensorHit.hh"
#include "fluoTestDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

fluoTestSiSD::fluoTestSiSD(G4String name,
                                   fluoTestDetectorConstruction* det)
:G4VSensitiveDetector(name),Si(det)
{
  collectionName.insert("SiCollection");
  HitSiID = new G4int[1];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

fluoTestSiSD::~fluoTestSiSD()
{
  delete [] HitSiID;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void fluoTestSiSD::Initialize(G4HCofThisEvent*HCE)
  
//initializes HCE with the hits collection(s) created by this 
  //sensitive detector
{
  SiCollection = new fluoTestSensorHitsCollection
                      (SensitiveDetectorName,collectionName[0]); 

 {HitSiID[0] = -1;};
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4bool fluoTestSiSD::ProcessHits(G4Step* aStep,G4TouchableHistory* ROhist)
{
  G4double edep = aStep->GetTotalEnergyDeposit();
  
  G4double stepl = 0.;
  if (aStep->GetTrack()->GetDefinition()->GetPDGCharge() != 0.)
      stepl = aStep->GetStepLength();
      
  if ((edep==0.)&&(stepl==0.)) return false; 

  G4TouchableHistory* theTouchable
    = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());
    
  G4VPhysicalVolume* physVol = theTouchable->GetVolume(); 
  //theTouchable->MoveUpHistory();
 
  if (HitSiID[0]==-1)
    { 
      fluoTestSensorHit* senHit = new fluoTestSensorHit();
      if (physVol == Si->GetSi()) 
	//verify whether the physical volume is the detector's one
      senHit->AddSi(edep,stepl);
      HitSiID[0] = SiCollection->insert(senHit) - 1;
      if (verboseLevel>0)
        G4cout << " New Sensor Hit  " << G4endl;
    }
 
else
    { 
      if (physVol == Si->GetSi())
	{(*SiCollection)[HitSiID[0]]->AddSi(edep,stepl);}
      if (verboseLevel>0)
        {G4cout << " Energy added to the detector "  << G4endl;} 
    }
 
 return true;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void fluoTestSiSD::EndOfEvent(G4HCofThisEvent* HCE)
{
  static G4int HCID = -1;
  if(HCID<0)
  { HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); }
  HCE->AddHitsCollection(HCID,SiCollection);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void fluoTestSiSD::clear()
{} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void fluoTestSiSD::DrawAll()
{} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void fluoTestSiSD::PrintAll()
{} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....




