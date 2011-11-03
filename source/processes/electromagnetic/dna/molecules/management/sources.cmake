#------------------------------------------------------------------------------
# sources.cmake
# Module : G4emlowenergy
# Package: Geant4.src.G4processes.G4electromagnetic.G4emlowenergy
#
# Sources description for a library.
# Lists the sources and headers of the code explicitely.
# Lists include paths needed.
# Lists the internal granular and global dependencies of the library.
# Source specific properties should be added at the end.
#
# Generated on : 24/9/2010
#
# $Id: sources.cmake,v 1.4 2010-11-15 08:24:43 gcosmo Exp $
#
#------------------------------------------------------------------------------

# List external includes needed.
include_directories(${CLHEP_INCLUDE_DIRS})

# List internal includes needed.
include_directories(${CMAKE_SOURCE_DIR}/source/geometry/management/include)
include_directories(${CMAKE_SOURCE_DIR}/source/global/HEPGeometry/include)
include_directories(${CMAKE_SOURCE_DIR}/source/global/HEPRandom/include)
include_directories(${CMAKE_SOURCE_DIR}/source/global/management/include)
include_directories(${CMAKE_SOURCE_DIR}/source/intercoms/include)
include_directories(${CMAKE_SOURCE_DIR}/source/materials/include)
include_directories(${CMAKE_SOURCE_DIR}/source/particles/management/include)
include_directories(${CMAKE_SOURCE_DIR}/source/track/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/electromagnetic/dna/management/include)

#
# Define the Geant4 Module.
#
include(Geant4MacroDefineModule)
GEANT4_DEFINE_MODULE(NAME G4emdna-molman
    HEADERS
        G4MolecularConfiguration.hh
        G4MolecularDecayChannel.hh
        G4MolecularDecayTable.hh
        G4MoleculeDefinition.hh
        G4MoleculeHandleManager.hh
        G4Molecule.hh
        G4MoleculeID.hh
        G4VMolecularDecayDisplacer.hh
    SOURCES
        G4MolecularConfiguration.cc
        G4MolecularDecayChannel.cc
        G4MolecularDecayTable.cc
        G4Molecule.cc
        G4MoleculeDefinition.cc
        G4MoleculeHandleManager.cc
        G4MoleculeID.cc
        G4VMolecularDecayDisplacer.cc
    GRANULAR_DEPENDENCIES
        G4geometrymng
        G4volumes
        G4globman
        G4heprandom
        G4intercoms
        G4materials
        G4partman
        G4procman
        G4track
    GLOBAL_DEPENDENCIES
        G4geometry
        G4global
        G4intercoms
        G4materials
        G4particles
        G4track
    LINK_LIBRARIES
)

# List any source specific properties here

