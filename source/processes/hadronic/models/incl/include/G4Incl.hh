//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: G4Incl.hh,v 1.1 2007-05-25 05:39:11 miheikki Exp $ 
// Translation of INCL4.2/ABLA V3 
// Pekka Kaitaniemi, HIP (translation)
// Christelle Schmidt, IPNL (fission code)
// Alain Boudard, CEA (contact person INCL/ABLA)
// Aatos Heikkinen, HIP (project coordination)

#ifndef G4Incl_hh
#define G4Incl_hh 1

#include "globals.hh"

#include "G4InclDataDefs.hh"
#include "G4Abla.hh"

/**
 *  Class containing INCL4 hadronic cascade algorithm.
 */

class G4Incl {

public:
  /**
   *   
   * We support Doxygen with JavaDoc style.
   *
   * \author{pekka.kaitaniemi@helsinki.fi}
   */


  /**
   * The standard constructor used to initialize the INCL4 algorithm.
   */
  G4Incl();

  /**
   * Constructor in which the user can define initialization
   * data. This is useful for unit testing purposes.
   * @param hazard a pointer to G4Hazard structure
   */
  G4Incl(G4Hazard *hazard, G4Dton *dton, G4Saxw *saxw, G4Ws *ws);

  /**
   * Constructor to be used with Geant4.
   */
  G4Incl(G4Hazard *hazard, G4Calincl *calincl, G4Ws *ws, G4Mat *mat, G4VarNtp *varntp);
  
  /**
   * A destructor.
   * A more elaborate description of the destructor.
   */
  ~G4Incl();

  /**
   *
   */
  void setVerboseLevel(G4int level);

  /**
   *
   */
  G4int getVerboseLevel();
  
  /**
   * Set G4Dton data.
   */
  void setDtonData(G4Dton *newDton);

  /**
   * Set G4Ws data.
   */
  void setWsData(G4Ws *newWs);
  
  /**
   * Set G4Hazard data.
   */
  void setHazardData(G4Hazard *newHazard);

  /**
   * Set G4Saxw data.
   */
  void setSaxwData(G4Saxw *newSaxw);

  /**
   * Set G4Spl2 data.
   */
  void setSpl2Data(G4Spl2 *newSpl2);

  /**
   * Set G4Mat data.
   */
  void setMatData(G4Mat *newMat);

  /**
   * Set G4Calincl data.
   */
  void setCalinclData(G4Calincl *newCalincl);

  /**
   * Set G4LightNuc data.
   */
  void setLightNucData(G4LightNuc *newLightNuc);

  /**
   * Set G4LightGausNuc data.
   */
  void setLightGausNucData(G4LightGausNuc *newLightGausNuc);

  /**
   * Set Bl1 data.
   */
  void setBl1Data(G4Bl1 *newBl1);

  /**
   * Set Bl2 data.
   */
  void setBl2Data(G4Bl2 *newBl2);

  /**
   * Set Bl3 data.
   */
  void setBl3Data(G4Bl3 *newBl3);

  /**
   * Set Bl4 data.
   */
  void setBl4Data(G4Bl4 *newBl4);

  /**
   * Set Bl5 data.
   */
  void setBl5Data(G4Bl5 *newBl5);

  /**
   * Set Bl6 data.
   */
  void setBl6Data(G4Bl6 *newBl6);

  /**
   * Set Bl8 data.
   */
  void setBl8Data(G4Bl8 *newBl8);

  /**
   * Set Bl9 data.
   */
  void setBl9Data(G4Bl9 *newBl9);

  /**
   * Set Bl10 data.
   */
  void setBl10Data(G4Bl10 *newBl10);

  /**
   * Set Kind data.
   */
  void setKindData(G4Kind *newKind);

public: // For Geant4
  /**
   * Process one event with INCL4. No built-in de-excitation.
   */ 
  void processEventIncl();

  /**
   * Process one event with INCL4 and built-in ABLA de-excitation.
   * Connection between INCL and ABLA not implemented yet.
   */ 
  void processEventInclAbla();

public: // Methods used to initialize INCL
  /**
   * Initialize INCL
   * @param initRandomSeed choose whether INCL should initialize random seeds.
   */
  void initIncl(G4bool initRandomSeed);

  /**
   * Initialize target materials
   * @param izmat charge number
   * @param iamat mass number
   * @param imat material number (array index)
   */
  void initMaterial(G4int izmat, G4int iamat, G4int imat);

  /**
   * a normal member taking two arguments and returning an integer value.
   * @param l an integer argument.
   * @param q a constant character pointer.
   * @return The test results
   */
  G4double deutv(G4int l, G4double q);
  
  /**
   * Returns the values of the function:
   * \f[
   * (0.23162461 + (j - 1))^2
   * \f]
   * @param j an integer parameter
   * @return a double value
   */
  G4double fm2(G4int j);

  /**
   * Interpolates function described by struct G4Saxw around a point.
   *
   * @param xv interpolation point (a double parameter)
   * @return a double value
   */
  G4double interpolateFunction(G4double xv);

  /**
   * Calculates the first derivative of the function stored in
   * structure G4Saxw.
   *
   * @param xv an integer parameter
   */
  void firstDerivative(G4int k);

  /**
   * Returns the values of the function:
   *  \f[
   *  \frac{r^2}{1 + e^{\frac{r - r_{0}}{A_{dif}}}}
   *  \f]
   *
   * @param r a G4double argument
   * @return a double value
   */
  G4double wsax(G4double r);

  /**
   * Returns the values of the function:
   * \f[
   *    \frac{1}{A_{dif}^{2}} \frac{r^3 e^{r - r_0}}{1 + e^{\frac{r - r_{0}}{A_{dif}}}}
   * \f]
   *
   * @param r a G4double argument
   * @return a double value
   */
  G4double derivWsax(G4double r);

  /**
   * Returns the values of the function:
   * \f[
   *    r^2 (1.0 + r_0 \frac{r^2}{A_{dif}}) e^{-\frac{r^2}{A_{dif}^2}}
   * \f]
   * @param r a G4double argument
   * @return a double value
   */
  G4double dmho(G4double r);

  /**
   * Returns the values of the function:
   * \f[
   *    -\frac{2r^4}{A_{dif}^2} (r_0(1.0 - \frac{r^2}{A_{dif}^2}) - 1.0) e^{-\frac{r^2}{A_{dif}^2}}
   * \f]
   * @param r a G4double argument
   * @return a double value
   */
  G4double derivMho(G4double r);

  /**
   * Returns the values of the function:
   * \f[
   *    \frac{r^4}{A_{dif}^2} e^{-\frac{1}{2} \frac{r^2}{A_{dif}^2}}
   * \f]
   * @param r a G4double argument
   * @return a double value
   */
  G4double derivGaus(G4double r);

  /** ce subroutine appele sur le premier tir va calculer la densite du deuton
   * dans l'espace des impulsions et preparer l'interpolation permettant ensuite
   * le tir au hasard d'un module de l'impulsion (q).
   * ce subroutine remplit le common /spl2/:
   * xsp(0:1), ysp integrale normalisee de la densite de 0 a q.
   * a(),b(),c() coefs des nsp points pour une interpolation du second degre.
   * q est en fm-1. 
   */
  void densDeut();

  /**
   * Integrate using Alkazhov's method.
   * @param ami a double parameter
   * @param ama a double parameter
   * @param dr a double parameter
   * @param functionChoice an integer parameter
   * @return a double value
   */
  G4double integrate(G4double ami, G4double ama, G4double step, G4int functionChoice);
    
  /**
   * Deuteron density
   * @param q a double parameter
   * @return a double value
   */
  G4double dens(G4double q);

  /**
   * Spl2Ab
   */
  void spl2ab();

  /**
   * Splineab
   * @param xv a double parameter
   * @return a double value
   */
  G4double splineab(G4double xv);

public: // Main INCL routines
  /**
   * INCL model as a function.
   */
  void pnu(G4int *ibert_p, G4int *nopart_p, G4int *izrem_p, G4int *iarem_p, G4double *esrem_p,
	   G4double *erecrem_p, G4double *alrem_p, G4double *berem_p, G4double *garem_p,
	   G4double *bimpact_p, G4int *l_p);

// void pnu(G4int *ibert_p, float *f_p[], G4int *nopart_p, G4int
// 		*kind_p[], G4double *ep_p[], G4double *alpha_p[], G4double *beta_p[], G4double *gam_p[],
// 		G4int *izrem_p, G4int *iarem_p, G4double *esrem_p, G4double *erecrem_p, G4double
// 		 *alrem_p, G4double *berem_p, G4double *garem_p, G4double *bimpact_p, G4int *l_p);

  /**
   * Collision routine
   */
  void collis(G4double *p1_p, G4double *p2_p, G4double *p3_p, G4double *e1_p, G4double *pout11_p, G4double *pout12_p, 
	      G4double *pout13_p, G4double *eout1_p, G4double *q1_p, G4double *q2_p, G4double *q3_p,
	      G4double *q4_p, G4int *np_p, G4int *ip_p, G4int *k2_p, G4int *k3_p, G4int *k4_p, 
	      G4int *k5_p, G4int *m1_p, G4int *m2_p, G4int *is1_p, G4int *is2_p);

  /**
   * This routine describes the anisotropic decay of a particle of
   * mass xi into 2 particles of masses x1,x2 the anisotropy is
   * supposed to follow a 1+3*hel*(cos(theta))**2 law with respect to
   * the direction of the incoming particle in the input, p1,p2,p3 is
   * the momentum of particle xi in the output, p1,p2,p3 is the
   * momentum of particle x1, while q1,q2,q3 is the momentum of
   * particle x2
   * @param p1_p pointer to momentum component 1
   * @param p2_p pointer to momentum component 2
   * @param p3_p pointer to momentum component 3
   * @param wp_p pointer to a double parameter
   * @param q1_p pointer to momentum component 1
   * @param q2_p pointer to momentum component 2
   * @param q3_p pointer to momentum component 3
   * @param wq_p pointer to a double parameter
   * @param xi_p pointer to a double parameter
   * @param x1_p pointer to momentum component 1
   * @param x2_p pointer to momentum component 2
   * @param x3_p pointer to momentum compone n3
   * @param hel_p pointer to a double parameter
   */
  void decay2(G4double *p1_p, G4double *p2_p, G4double *p3_p, G4double *wp_p, G4double *q1_p, 
	      G4double *q2_p, G4double *q3_p, G4double *wq_p, G4double *xi_p, G4double *x1_p,
	      G4double *x2_p, G4double *hel_p);

  /**
   * Time calculation
   * @param i an index of particle 1
   * @param j an index of particle 2
   */
  void time(G4int i, G4int j);

  /**
   * New time
   * @param l1 an integer parameter
   * @param l2 an integer parameter
   */
  void newt(G4int l1, G4int l2);

  /**
   * New1
   * @param l1 an integer parameter
   */
  void new1(G4int l1);

  /**
   * New2
   * @param y1 a double parameter
   * @param y2 a double parameter
   * @param y3 a double parameter
   * @param q1 a double parameter
   * @param q2 a double parameter
   * @param q3 a double parameter
   * @param q4 a double parameter
   * @param npion number of pions
   * @param l1 an integer parameter
   */
  void new2(G4double y1, G4double y2, G4double y3, G4double q1, G4double q2, G4double q3, 
	    G4double q4, G4int npion, G4int l1);

  /**
   * New3
   * @param y1 a double parameter
   * @param y2 a double parameter
   * @param y3 a double parameter
   * @param q1 a double parameter
   * @param q2 a double parameter
   * @param q3 a double parameter
   * @param q4 a double parameter
   * @param npion number of pions
   * @param l1 an integer parameter
   */
  void new3(G4double y1, G4double y2, G4double y3, G4double q1, G4double q2, G4double q3, 
	    G4double q4, G4int npion, G4int l1);

  /**
   * Lorentz transformation
   * @param q1 a double parameter
   * @param q2 a double parameter
   * @param q3 a double parameter
   * @param b1 a double parameter
   * @param b2 a double parameter
   * @param b3 a double parameter
   * @param E energy
   */
  void loren(G4double *q1, G4double *q2, G4double *q3, G4double *b1, G4double *b2, G4double *b3, G4double *E);

  /**
   * Pauli blocking.
   * @param l an integer parameter
   * @param xr a double parameter
   * @param pr a double parameter
   * @return a double value
   */
  G4double pauliBlocking(G4int l, G4double xr, G4double pr);

  /**
   * fit by j.vandermeulen
   * low enrgy fit of j.c., d. l'hote, j. vdm, nim b111(1996)215
   *
   * i = 2,0,-2  for pp,pn,nn
   * m = 0,1,2 for nucleon-nucleon,nucleon-delta,delta,delta
   *
   * @param E energy
   * @param m a double parameter
   * @param i a double parameter
   * @return a double value
   */
  G4double lowEnergy(G4double E, G4double m, G4double i);

  /**
   * Total cross-sections
   * i=2,0,-2  for pp,pn,nn
   * m=0,1,2 for nucleon-nucleon,nucleon-delta,delta,delta
   *
   * @param E energy
   * @param m an integer parameter
   * @param i an integer parameter
   * @return a double value
   */
  G4double totalCrossSection(G4double E, G4int m, G4int i);

  /**
   * Srec
   * @param Ein energy
   * @param d a double parameter
   * @param i an integer parameter
   * @param isa an integer parameter
   * @return a double value
   */
  G4double srec(G4double Ein, G4double d, G4int i, G4int isa);

  /**
   * Delta production cross section
   * @param E energy
   * @param i an integer parameter
   * @return a double value
   */
  G4double deltaProductionCrossSection(G4double E, G4int i);

  /**
   * sigma(pi+ + p) in the (3,3) region
   * new fit by j.vandermeulen + constant value above the (3,3)
   * resonance
   * @param x a double parameter
   * @return a double value
   */
  G4double pionNucleonCrossSection(G4double x);

  /**
   * Transmission probability for a nucleon of kinetic energy
   * E on the edge of the well of depth v0 (nr approximation)
   * iz is the isospin of the nucleon,izn the instanteneous charge
   * of the nucleus and r is the target radius
   * @param E a double parameter
   * @param iz a double parameter
   * @param izn a double parameter
   * @param r a double parameter
   * @param v0 a double parameter
   * @return a double value
   */
  G4double transmissionProb(G4double E, G4double iz, G4double izn, G4double r, G4double v0);

  /**
   * Ref
   * @param x1 a double parameter
   * @param x2 a double parameter
   * @param x3 a double parameter
   * @param p1 a double parameter
   * @param p2 a double parameter
   * @param p3 a double parameter
   * @param E a double parameter
   * @param r2 a double parameter
   * @return a double value
   */
  G4double ref(G4double x1, G4double x2, G4double x3, G4double p1,
		 G4double p2, G4double p3, G4double E, G4double r2);

  /**
   * ForceAbsor
   */
  void forceAbsor(G4int nopart, G4double iarem, G4double izrem, G4double esrem, G4double erecrem, 
		  G4double alrem, G4double berem, G4double garem, G4double jrem);

  /**
   * ForceAbs
   * @param iprojo projectile
   * @param at target mass number
   * @param zt target charge number 
   * @param ep projectile energy
   * @param bmax a double parameter
   * @param pt a double parameter
   * @return absorption probability
   */
  G4double forceAbs(G4double iprojo, G4double at, G4double zt, G4double ep, G4double bmax, G4double pt);

  /**
   * absoprption xsec revised version rkt-97/5                             
   * neutron data from barashenkov                                         
   * this gives absorption xsec for given zp,ap,zt,at,e (mev/nucleon)      
   * arguement changed to mev; then e=ep/ap mev/nucleon                    
   * can be used for neutrons also.                                        
   * this has coulomb as ours                                          
   * @param zp projectile charge number
   * @param zp projectile mass number
   * @param zt a double parameter
   * @param zt target charge number
   * @param at target mass number
   * @param ep projectile energy
   */
  G4double xabs2(G4double zp, G4double ap, G4double zt, G4double at, G4double ep);

  /**
   * Standard random number generator.
   * @param *rndm pointer to the variable reserved for random number
   * @param *seed pointer to the random seed
   */
  void standardRandom(G4double *rndm, G4int *seed);

  /**
   * First derivative of a gaussian potential.
   * @param *rndm pointer to the variable reserved for random number
   */
  void gaussianRandom(G4double *rndm);

  /**
   * Safe exponential function which eliminates the CPU under and overflows.
   * @param x a double parameter
   * @return a double value
   */
  G4double safeExp(G4double x);

  /**
   * Nuclear radius
   * @param A mass number (double parameter)
   */
  G4double radius(G4double A);
    
  /** Parametrisation de la section efficace de réaction calculée par incl4.1
   * iprojo=1 proton incident, iprojo=2, neutron incident).
   * entre al et u, entre 10 et 100 mev protons, 20 et 100 mev neutrons.
   * bon ordre de grandeur pour les noyaux légers (c, o ...), trés faux
   * a energie sup a 100 mev.
   * (Comment needs to be translated)
   * @param projectile an integer parameter (1 = proton, 2 = neutron)
   * @param E energy of the projectile (double parameter)
   * @param A target mass number (double parameter)
   * @return cross section (double value)
   */
  G4double crossSection(G4int projectile, G4double E, G4double A); 

  /**
   * coulombTransm
   * subroutine coulomb_transm(e,fm1,z1,fm2,z2,proba)
   * calcul du coulombien dans lahet (proba de transmission ou
   * d'entree dans le potentiel nucleaire).
   * @param E energy (a double parameter)
   * @param fm1 a double parameter
   * @param z1 a double parameter
   * @param fm2 a double parameter
   * @param z2 a double parameter
   * @return a double value
   */
  G4double coulombTransm(G4double E, G4double fm1, G4double z1, G4double fm2, G4double z2);

  /**
   * Clmb1
   * @param eta a double parameter \f$\eta = c_2*z_1*z_2*\sqrt{m/E}\f$                                          
   * @param rho a double parameter \f$\rho = c_3*(r_1+r_2)*\sqrt{mE}\f$                                        
   * @return a double value
   */
  G4double clmb1(G4double rho, G4double eta, G4double *ml);

  /**
   * First derivative of a gaussian potential.
   * @param eta a double parameter \f$\eta = c_2*z_1*z_2*\sqrt{m/E}\f$                                          
   * @param rho a double parameter \f$\rho = c_3*(r_1+r_2)*\sqrt{mE}\f$                                        
   * @return a double value
   */
  G4double clmb2(G4double rho, G4double eta, G4double *t1);

public: // Utilities
  /**
   * Returns the smaller of two numbers.
   * @param a a double value
   * @param b a double value
   * @return a double value
   */
  G4double min(G4double a, G4double b);

  /**
   * Returns the smaller of two numbers.
   * @param a an integer value
   * @param b an integer value
   * @return an integer value
   */
  G4int min(G4int a, G4int b);

  /**
   * Returns the greater of two numbers.
   * @param a a double value
   * @param b a double value
   * @return a double value
   */
  G4double max(G4double a, G4double b);

  /**
   * Returns the greater of two numbers.
   * @param a an integer value
   * @param b an integer value
   * @return an integer value
   */
  G4int max(G4int a, G4int b);

  /**
   * Rounds a double to the nearest int
   * @param a double parameter
   * @return an integer value
   */
  G4int nint(G4double number);

  /**
   * Calls a function
   * @param functionChoice an integer value representing the choice of
   * function (0 = wsax, 1 = derivWsax, 2 = dmho, 3 = derivMho, 4 = derivGaus)
   * @param r a double parameter
   * @return a double value
   */
  G4double callFunction(G4int functionChoice, G4double r);

  G4double am(G4double a, G4double b, G4double c, G4double d);
  G4double pcm(G4double e, G4double a, G4double c);
  G4double sign(G4double a, G4double b);
  G4double utilabs(G4double a);
  G4double amax1(G4double a, G4double b);
  G4double w(G4double a, G4double b, G4double c, G4double d);
  G4int idnint(G4double a);
   private:

  /**
   * Random seeds for INCL4 internal random number generators.
   */
  G4Hazard *hazard;

  /** 
   * Data structure for INCL4.
   */
  G4Dton *dton;

  /** 
   * Data structure for INCL4. Contains the Woods-Saxon potential
   * functions for target nuclei.
   */
  G4Saxw *saxw;

  /** 
   * Data structure for INCL4.
   */
  G4Ws *ws;

  /**
   * G4Spl2
   */
  G4Spl2 *spl2;

  /**
   * G4LightGausNuc
   */
  G4LightGausNuc *light_gaus_nuc;

  /**
   * G4LightNuc
   */
  G4LightNuc *light_nuc;

  /**
   * G4Calincl
   */
  G4Calincl *calincl;

  /**
   * G4Mat
   */
  G4Mat *mat;

  /**
   *
   */
  G4Bl1 *bl1;

    /**
   *
   */
  G4Bl2 *bl2;

    /**
   *
   */
  G4Bl3 *bl3;

    /**
   *
   */
  G4Bl4 *bl4;

    /**
   *
   */
  G4Bl5 *bl5;

    /**
   *
   */
  G4Bl6 *bl6;

  /**
   *
   */
  G4Bl8 *bl8;

    /**
   *
   */
  G4Bl9 *bl9;

    /**
   *
   */
  G4Bl10 *bl10;

  /**
   *
   */
  G4Kind *kindstruct;

  /**
   *
   */
  G4Paul *paul;
  
  /**
   * Detailed information of the cascade
   */
  G4VarAvat *varavat;

  /**
   * Cascade output.
   */
  G4VarNtp *varntp;

  /** 
   * Defines the verbosity of console output. Values can be between 0
   * and 4 where 0 means silent and 4 the most verbose possible
   * output.
   */
  G4int verboseLevel;

  /**
   * Function ID for wsax.
   * @see wsax
   * @see integrate
   * @see callFunction
   */
  G4int wsaxFunction;

  /**
   * Function ID for derivWsax.
   * @see derivWsax
   * @see integrate
   * @see callFunction
   */
  G4int derivWsaxFunction;

  /**
   * Function ID for dmho.
   * @see derivWsax
   * @see integrate
   * @see callFunction
   */
  G4int dmhoFunction;

  /**
   * Function ID for derivMho.
   * @see derivMho
   * @see integrate
   * @see callFunction
   */
  G4int derivMhoFunction;

  /**
   * Function ID for derivGaus.
   * @see derivGaus
   * @see integrate
   * @see callFunction
   */
  G4int derivGausFunction;

  /**
   * Function ID for dens.
   * @see dens
   * @see integrate
   * @see callFunction
   */
  G4int densFunction;

  G4int kind[300]; //= (*kind_p);
  G4double ep[300]; // = (*ep_p);
  G4double alpha[300]; // = (*alpha_p); 
  G4double beta[300]; // = (*beta_p);
  G4double gam[300]; // = (*gam_p);

  G4Volant *volant;
  G4Abla *abla;
};

#endif
