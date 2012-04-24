//
// G4MuElecElasticModel.hh, 2011/08/29 A.Valentin, M. Raine
//
// Based on the following publications
//	    - Geant4 physics processes for microdosimetry simulation:
//	    very low energy electromagnetic models for electrons in Si,
//	    to be published in TNS
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...... 

#ifndef G4MuElecElasticModel_h
#define G4MuElecElasticModel_h 1

#include <map>
#include "G4MuElecCrossSectionDataSet.hh"
#include "G4VEmModel.hh"
#include "G4Electron.hh"
#include "G4ParticleChangeForGamma.hh"
#include "G4LogLogInterpolation.hh"
#include "G4ProductionCutsTable.hh"
#include "G4NistManager.hh"

class G4MuElecElasticModel : public G4VEmModel
{

public:

  G4MuElecElasticModel(const G4ParticleDefinition* p = 0, 
		          const G4String& nam = "MuElecElasticModel");

  virtual ~G4MuElecElasticModel();

  virtual void Initialise(const G4ParticleDefinition*, const G4DataVector&);

  virtual G4double CrossSectionPerVolume(const G4Material* material,
					   const G4ParticleDefinition* p,
					   G4double ekin,
					   G4double emin,
					   G4double emax);

  virtual void SampleSecondaries(std::vector<G4DynamicParticle*>*,
				 const G4MaterialCutsCouple*,
				 const G4DynamicParticle*,
				 G4double tmin,
				 G4double maxEnergy);

  inline void SetKillBelowThreshold (G4double threshold);		 
  G4double GetKillBelowThreshold () { return killBelowEnergy; }	

protected:

  G4ParticleChangeForGamma* fParticleChangeForGamma;

private:

  G4Material* nistSi;
  G4double killBelowEnergy;  
  G4double lowEnergyLimit;  
  G4double lowEnergyLimitOfModel;  
  G4double highEnergyLimit; 
  G4bool isInitialised;
  G4int verboseLevel;
  
  // Cross section
  
  typedef std::map<G4String,G4String,std::less<G4String> > MapFile;
  MapFile tableFile;

  typedef std::map<G4String,G4MuElecCrossSectionDataSet*,std::less<G4String> > MapData;
  MapData tableData;
  
  // Final state

  G4double Theta(G4ParticleDefinition * aParticleDefinition, G4double k, G4double integrDiff);

  G4double LogLogInterpolate(G4double e1, G4double e2, G4double e, G4double xs1, G4double xs2);
   
  G4double LinLogInterpolate(G4double e1, G4double e2, G4double e, G4double xs1, G4double xs2);
   
  G4double QuadInterpolator(G4double e11, 
 		            G4double e12, 
			    G4double e21, 
			    G4double e22, 
			    G4double x11,
			    G4double x12, 
			    G4double x21, 
			    G4double x22, 
			    G4double t1, 
			    G4double t2, 
			    G4double t, 
			    G4double e);

  typedef std::map<double, std::map<double, double> > TriDimensionMap;

  TriDimensionMap eDiffCrossSectionData;
  std::vector<double> eTdummyVec;

  typedef std::map<double, std::vector<double> > VecMap;
  VecMap eVecm;
   
  G4double RandomizeCosTheta(G4double k);
   
  //
   
  G4MuElecElasticModel & operator=(const  G4MuElecElasticModel &right);
  G4MuElecElasticModel(const  G4MuElecElasticModel&);

};

inline void G4MuElecElasticModel::SetKillBelowThreshold (G4double threshold) 
{ 
    killBelowEnergy = threshold; 
    
    if (threshold < 5*eV)
    {
       G4Exception ("*** WARNING : the G4MuElecElasticModel class is not validated below 5 eV !","",JustWarning,"") ;
       threshold = 0.025*eV;
    }
             
}		 


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif