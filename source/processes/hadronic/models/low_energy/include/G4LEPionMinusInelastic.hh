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
//
// $Id: G4LEPionMinusInelastic.hh,v 1.8 2003-07-01 15:49:03 hpw Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
 // Hadronic Process: Low Energy PionMinus Inelastic Process
 // original by J.L. Chuma, TRIUMF, 03-Feb-1997
 // Last modified: 27-Mar-1997
 
#ifndef G4LEPionMinusInelastic_h
#define G4LEPionMinusInelastic_h 1
 
// Class Description
// Final state production model for PionMinus inelastic scattering below 20 GeV; 
// To be used in your physics list in case you need this physics.
// In this case you want to register an object of this class with 
// the corresponding process.
// Class Description - End

#include "G4InelasticInteraction.hh"
 
 class G4LEPionMinusInelastic : public G4InelasticInteraction
 {
 public:
    
    G4LEPionMinusInelastic() : G4InelasticInteraction()
    {
      SetMinEnergy( 0.0 );
      SetMaxEnergy( 55.*GeV );
    }
    
    ~G4LEPionMinusInelastic() { }
    
    G4HadFinalState * ApplyYourself(const G4HadProjectile &aTrack,
                                      G4Nucleus &targetNucleus );
    
 private:
    
    void Cascade(                               // derived from CASPIM
      G4FastVector<G4ReactionProduct,128> &vec,
      G4int &vecLen,
      const G4HadProjectile *originalIncident,
      G4ReactionProduct &currentParticle,
      G4ReactionProduct &targetParticle,
      G4bool &incidentHasChanged, 
      G4bool &targetHasChanged,
      G4bool &quasiElastic );
    
 };
 
#endif
 
