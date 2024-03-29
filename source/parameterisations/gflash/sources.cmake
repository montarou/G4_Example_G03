# - G4gflash module build definition

# Define the Geant4 Module.
geant4_add_module(G4gflash
  PUBLIC_HEADERS
    G4GFlashSpot.hh
    G4VGFlashSensitiveDetector.hh
    GFlashEnergySpot.hh
    GFlashHitMaker.hh
    GFlashHomoShowerParameterisation.hh
    GFlashParticleBounds.hh
    GFlashSamplingShowerParameterisation.hh
    GFlashSamplingShowerTuning.hh
    GFlashShowerModel.hh
    GFlashShowerModelMessenger.hh
    GVFlashHomoShowerTuning.hh
    GVFlashShowerParameterisation.hh
    Gamma.hh
  SOURCES
    GFlashEnergySpot.cc
    GFlashHitMaker.cc
    GFlashHomoShowerParameterisation.cc
    GFlashParticleBounds.cc
    GFlashSamplingShowerParameterisation.cc
    GFlashShowerModel.cc
    GFlashShowerModelMessenger.cc
    GVFlashShowerParameterisation.cc
    Gamma.cc)

geant4_module_link_libraries(G4gflash
  PUBLIC
    G4parameterisation
    G4track
    G4detector
    G4partman
    G4navigation
    G4geometrymng
    G4intercoms
    G4globman
  PRIVATE
    G4procman
    G4leptons
    G4mesons
    G4materials
    G4graphics_reps
    G4heprandom
    G4hepgeometry
  )
