MPI/Examples : exMPI03
======================

Description
-----------
An example of dosimetry in a water phantom.
The example has same geometry and physics as exMPI02, only the analysis part
is different.

### Configuration:
- Geometry     : water phantom
- Primary      : broad beam (200 MeV proton)
- Physics List : FTFP_BERT
- Analysis     : g4tools histogramming

The environment variables, *G4LEDATA*, *G4LEVELGAMMADATA* and *G4SAIDXSDATA*
are required for data files.

### Features:
- Score dose distribution in a water phantom.
- Learn how to paralleized your applications.
- Create a ROOT file containing histograms/trees in each node.
  Each slave node generate a ROOT file, whose file name is different 
  from each other.
- MT is enabled.
- One of each object supported by g4tools is instantiated: 1D, 2D and 3D
  histograms and 1D and 2D profiles.
- Histograms and profiles are merged first among threads of the same 
  MPI-rank and then across MPI-ranks. See RunActionMaster class for 
  an example on how to use the mergers.
- G4Run object and scorers (command line ones) are also merged via
  MPI.
- Output: several root files are created: dose-rank*.root thread-merged
  histograms for each rank; dose-merged.root, merged across threads and 
  across ranks.

How to build
------------
Use CMake on Geant4 library installed with CMake build.

This example requires G4mpi library to be installed
(see examples/extended/parallel/MPI/source/REDME.md)

Follow these commands,

    > mkdir build
    > cd build
    > cmake -DG4mpi_DIR=<where-G4mpi-wasintalled>/lib[64]/G4mpi -DCMAKE_CXX_COMPILER=mpicxx \
      -DGeant4_DIR=<your Geant4 install path>/lib[64]/Geant4-V.m.n <path-to-source>
      (V.m.n is the version of Geant4, eg. Geant4-9.6.0)
    > make
    > make install

Replace mpicxx with your MPI-compiler wrapper.

How to run
----------

    > mpiexec -n # ./exMPI03 [run.mac]
