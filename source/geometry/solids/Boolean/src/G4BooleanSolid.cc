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
// $Id: G4BooleanSolid.cc,v 1.6 2002-01-10 15:34:27 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// Implementation for the abstract base class for solids created by boolean 
// operations between other solids
//
// History:
//
// 10.09.98 V.Grichine, creation according J. Apostolakis's recommendations

#include "G4BooleanSolid.hh"
#include "G4DisplacedSolid.hh"

#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4AffineTransform.hh"

//////////////////////////////////////////////////////////////////
//
//

G4BooleanSolid::G4BooleanSolid( const G4String& pName,
                                G4VSolid* pSolidA ,
                                G4VSolid* pSolidB   ) :
  G4VSolid(pName),
  createdDisplacedSolid(false)
{
  fPtrSolidA = pSolidA ;
  fPtrSolidB = pSolidB ;
}

//////////////////////////////////////////////////////////////////
//
//

G4BooleanSolid::G4BooleanSolid( const G4String& pName,
                                      G4VSolid* pSolidA ,
                                      G4VSolid* pSolidB ,
                                      G4RotationMatrix* rotMatrix,
                                const G4ThreeVector& transVector    ) :
  G4VSolid(pName),
  createdDisplacedSolid(true)
{
  fPtrSolidA = pSolidA ;
  fPtrSolidB = new G4DisplacedSolid("placedB",pSolidB,rotMatrix,transVector) ;
}

//////////////////////////////////////////////////////////////////
//
//

G4BooleanSolid::G4BooleanSolid( const G4String& pName,
                                      G4VSolid* pSolidA ,
                                      G4VSolid* pSolidB ,
                                const G4Transform3D& transform    ) :
  G4VSolid(pName),
  createdDisplacedSolid(true)
{
  fPtrSolidA = pSolidA ;
  fPtrSolidB = new G4DisplacedSolid("placedB",pSolidB,transform) ;
}

///////////////////////////////////////////////////////////////
//
// Destructor deletes second pointer created by 'new'

G4BooleanSolid::~G4BooleanSolid() 
{
  if(createdDisplacedSolid) delete fPtrSolidB ;
}

///////////////////////////////////////////////////////////////
//
// If Solid is made up from a Boolean operation of two solids,
//   return the corresponding solid (for no=0 and 1)
// If the solid is not a "Boolean", return 0
const G4VSolid* G4BooleanSolid::GetConstituentSolid(G4int no) const
{
  const G4VSolid*  subSolid=0;
  if( no == 0 )  
    subSolid = fPtrSolidA;
  else if( no == 1 ) 
    subSolid = fPtrSolidB;
  else
    G4Exception("G4BooleanSolid::GetConstituentSolid() - invalid subsolid index");

  return subSolid;
}

  G4VSolid* G4BooleanSolid::GetConstituentSolid(G4int no)
{
  G4VSolid*  subSolid=0;
  if( no == 0 )  
    subSolid = fPtrSolidA;
  else if( no == 1 ) 
    subSolid = fPtrSolidB;
  else
    G4Exception("G4BooleanSolid::GetConstituentSolid() - invalid subsolid index");

  return subSolid;
}




