// $Id: A01GeneralPhysics.hh,v 1.1 2002-11-13 07:18:14 duns Exp $
// --------------------------------------------------------------
// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//

#ifndef A01GeneralPhysics_h
#define A01GeneralPhysics_h 1

#include "globals.hh"
#include "G4ios.hh"

#include "G4VPhysicsConstructor.hh"


#include "G4Decay.hh"

class A01GeneralPhysics : public G4VPhysicsConstructor
{
  public:
    A01GeneralPhysics(const G4String& name = "general");
    virtual ~A01GeneralPhysics();

  public:
    // This method will be invoked in the Construct() method.
    // each particle type will be instantiated
    virtual void ConstructParticle();

    // This method will be invoked in the Construct() method.
    // each physics process will be instantiated and
    // registered to the process manager of each particle type
    virtual void ConstructProcess();

  protected:
    G4Decay fDecayProcess;
};


#endif








