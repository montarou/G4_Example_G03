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
// 
// --------------------------------------------------------------------
// GEANT 4 class header file
//
//
// G4PolynomRoot
//
#ifndef G4POLYNOMROOT_HH
#define G4POLYNOMROOT_HH

#include <cmath>
#include <cfloat>

#include "globals.hh"


class G4PolynomRoot 
{

  public:

  G4PolynomRoot(){}
  ~G4PolynomRoot(){}
  
  public:

  G4int FindRoots(G4double *op, G4int degree, G4double *zeror, G4double *zeroi) ;

  private:

  G4double *p;
  G4double *qp;
  G4double *k;
  G4double *qk;
  G4double *svk;
  G4double sr;
  G4double si;
  G4double u,v; 
  G4double a,b,c,d ;
  G4double a1,a2,a3,a6,a7;
  G4double e,f,g,h ;
  G4double szr,szi ;
  G4double lzr,lzi;
  G4int n,nn,nmi,zerok;
  
  /*  The following statements set machine constants. */
  static const G4double base ;
  static const G4double eta ;
  static const G4double infin ;
  static const G4double smalno ;
  static const G4double are ;
  static const G4double mre ;
  static const G4double lo ;

  void Quadratic(G4double a,G4double b1,G4double c,G4double *sr,G4double *si,
	    G4double *lr,G4double *li);
  void ComputeFixedShiftPolynomial(G4int l2, G4int *nz);
  void QuadraticPolynomialIteration(G4double *uu,G4double *vv,G4int *nz);
  void RealPolynomialIteration(G4double *sss, G4int *nz, G4int *iflag);
  void ComputeScalarFactors(G4int *type);
  void ComputeNextPolynomial(G4int *type);
  void ComputeNewEstimate(G4int type,G4double *uu,G4double *vv);
  void QuadraticSyntheticDivision(G4int n,G4double *u,G4double *v,G4double *p,G4double *q,
	      G4double *a,G4double *b);
  

};


#endif
