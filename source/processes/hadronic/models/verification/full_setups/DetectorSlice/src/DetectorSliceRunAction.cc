#include "DetectorSliceRunAction.hh"
#include "globals.hh"
#include "G4Run.hh"

#include "DetectorSliceAnalysis.hh"


DetectorSliceRunAction::~DetectorSliceRunAction() { 
  // Close the AIDA file at the end of the job (not at the end of the run!).
  DetectorSliceAnalysis::getInstance()->close();  
}


void DetectorSliceRunAction::BeginOfRunAction( const G4Run* aRun ) {
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
  // Reset the histograms/ntuples of the eventual previous run.
  DetectorSliceAnalysis::getInstance()->init();
}


void DetectorSliceRunAction::EndOfRunAction( const G4Run* ) {
  // Commit the histograms/ntuples.
  DetectorSliceAnalysis::getInstance()->finish();
}   

