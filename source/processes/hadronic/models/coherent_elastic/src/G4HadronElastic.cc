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
// $Id: G4HadronElastic.cc,v 1.16 2006-06-19 08:08:39 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
// Physics model class G4HadronElastic (derived from G4LElastic)
//
//
// G4 Model: Low-energy Elastic scattering with 4-momentum balance
// F.W. Jones, TRIUMF, 04-JUN-96
// Uses  G4ElasticHadrNucleusHE and G4VQCrossSection
//
//
// 25-JUN-98 FWJ: replaced missing Initialize for ParticleChange.
// 09-Set-05 V.Ivanchenko HARP version of the model: fix scattering
//           on hydrogen, use relativistic Lorentz transformation
// 24-Nov-05 V.Ivanchenko sample cost in center of mass reference system
// 03-Dec-05 V.Ivanchenko add protection to initial momentum 20 MeV/c in
//           center of mass system (before it was in lab system)
//           below model is not valid
// 14-Dec-05 V.Ivanchenko change protection to cos(theta) < -1 and
//           rename the class
// 13-Apr-06 V.Ivanchenko move to coherent_elastic subdirectory; remove
//           charge exchange; remove limitation on incident momentum;
//           add s-wave regim below some momentum        
// 24-Apr-06 V.Ivanchenko add neutron scattering on hydrogen from CHIPS
//

#include "G4HadronElastic.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4IonTable.hh"
#include "G4QElasticCrossSection.hh"
#include "G4VQCrossSection.hh"
#include "G4ElasticHadrNucleusHE.hh"
#include "Randomize.hh"
#include "G4Proton.hh"
#include "G4Neutron.hh"
#include "G4Deuteron.hh"
#include "G4Alpha.hh"

G4HadronElastic::G4HadronElastic(G4double elim, G4double plow, G4double ehigh) 
  : G4HadronicInteraction()
{
  SetMinEnergy( 0.0*GeV );
  SetMaxEnergy( DBL_MAX );
  verboseLevel= 0;
  plablow     = plow;
  ekinhigh    = ehigh;
  ekinlim     = elim;
  qCManager   = G4QElasticCrossSection::GetPointer();
  hElastic    = new G4ElasticHadrNucleusHE();

  theProton   = G4Proton::Proton();
  theNeutron  = G4Neutron::Neutron();
  theDeuteron = G4Deuteron::Deuteron();
  theAlpha    = G4Alpha::Alpha();
}

G4HadronElastic::~G4HadronElastic()
{
  delete hElastic;
}

G4VQCrossSection* G4HadronElastic::GetCS()
{
  return qCManager;
}

G4ElasticHadrNucleusHE* G4HadronElastic::GetHElastic()
{
  return hElastic;
}

G4HadFinalState* G4HadronElastic::ApplyYourself(
		 const G4HadProjectile& aTrack, G4Nucleus& targetNucleus)
{
  theParticleChange.Clear();
  const G4HadProjectile* aParticle = &aTrack;
  G4double aTarget = targetNucleus.GetN();
  G4double zTarget = targetNucleus.GetZ();

  // Elastic scattering off Hydrogen

  G4double plab = aParticle->GetTotalMomentum();
  G4double ekin = aParticle->GetKineticEnergy();
  if (verboseLevel > 1) 
    G4cout << "G4HadronElastic::DoIt: Incident particle plab=" 
	   << plab/GeV << " GeV/c " 
	   << " ekin(MeV) = " << ekin/MeV << "  " 
	   << aParticle->GetDefinition()->GetParticleName() << G4endl;

  // Scattered particle referred to axis of incident particle
  const G4ParticleDefinition* theParticle = aParticle->GetDefinition();
  G4double m1 = theParticle->GetPDGMass();

  G4int Z = static_cast<G4int>(zTarget);
  G4int A = static_cast<G4int>(aTarget);
  G4int N = A - Z;
  G4int projPDG = theParticle->GetPDGEncoding();
  if (verboseLevel > 1) 
    G4cout << "G4HadronElastic for " << theParticle->GetParticleName()
	   << " PDGcode= " << projPDG << " on nucleus Z= " << Z 
	   << " A= " << A << " N= " << N 
	   << G4endl;

  G4ParticleDefinition * theDef = 0;

  if(Z == 1 && A == 1) theDef = G4Proton::Proton();
  else if (Z == 1 && A == 2) theDef = G4Deuteron::Deuteron();
  else if (Z == 1 && A == 3) theDef = G4Triton::Triton();
  else if (Z == 2 && A == 3) theDef = G4He3::He3();
  else if (Z == 2 && A == 4) theDef = G4Alpha::Alpha();
  else theDef = G4ParticleTable::GetParticleTable()->FindIon(Z,A,0,Z);
 
  G4double m2 = theDef->GetPDGMass();
  G4LorentzVector lv1 = aParticle->Get4Momentum();
  G4LorentzVector lv0(0.0,0.0,0.0,m2);
   
  G4LorentzVector lv  = lv0 + lv1;
  G4ThreeVector bst = lv.boostVector();
  lv1.boost(-bst);
  lv0.boost(-bst);
  G4ThreeVector p1 = lv1.vect();
  G4double ptot = p1.mag();
  G4double tmax = 4.0*ptot*ptot;
  G4double t = 0.0;

  // Choose generator
  G4ElasticGenerator gtype = fLElastic;
  if ((theParticle == theProton || theParticle == theNeutron) && Z == 1) {
    gtype = fQElastic;
  } else if(ekin >= ekinhigh) {
    gtype = fHElastic;
  } else if(plab <= plablow) {
    gtype = fSWave;
  }

  // Sample t
  if(gtype == fQElastic) {
    if (verboseLevel > 1) 
      G4cout << "G4HadronElastic: Z= " << Z << " N= " 
	     << N << " pdg= " <<  projPDG
	     << " mom(GeV)= " << plab/GeV << "  " << qCManager << G4endl; 
    G4double cs = qCManager->GetCrossSection(false,plab,Z,N,projPDG);
    if(cs > 0.0) t = qCManager->GetExchangeT(Z,N,projPDG);
    else gtype = fSWave;
  }

  if(gtype == fLElastic) {
    t = GeV*GeV*SampleT(ptot,m1,m2,aTarget);
    if(t > tmax) gtype = fSWave;
  }

  if(gtype == fHElastic) {
    t = hElastic->SampleT(theParticle,plab,Z,A);
    if(t > tmax) gtype = fSWave;
  }

  if(gtype == fSWave) t = G4UniformRand()*tmax;

  if(verboseLevel>1)
    G4cout <<"type= " << gtype <<" t= " << t << " tmax= " << tmax 
	   << " ptot= " << ptot << G4endl;

  // Sampling in CM system
  G4double phi  = G4UniformRand()*twopi;
  G4double cost = 1. - 2.0*t/tmax;
  if(std::abs(cost) > 1.0) cost = -1.0 + 2.0*G4UniformRand();
  G4double sint = std::sqrt((1.0-cost)*(1.0+cost));
  
  if (verboseLevel>1) 
    G4cout << "cos(t)=" << cost << " sin(t)=" << sint << G4endl;

  G4ThreeVector v1(sint*std::cos(phi),sint*std::sin(phi),cost);
  p1 = p1.unit();
  v1.rotateUz(p1);
  v1 *= ptot;
  G4LorentzVector nlv1(v1.x(),v1.y(),v1.z(),std::sqrt(ptot*ptot + m1*m1));
  G4LorentzVector nlv0 = lv0 + lv1 - nlv1;

  nlv0.boost(bst);
  nlv1.boost(bst); 

  G4double eFinal = nlv1.e() - m1;
  if (verboseLevel > 1) 
    G4cout << " P0= "<< nlv0 << "   P1= "
	   << nlv1<<" m= " << m1 << " ekin0= " << eFinal 
	   << " ekin1= " << nlv0.e() - m2 
	   <<G4endl;
  if(eFinal < 0.0) {
    G4cout << "G4HadronElastic WARNING ekin= " << eFinal
	   << " after scattering of " 
	   << aParticle->GetDefinition()->GetParticleName()
	   << " p(GeV/c)= " << plab
	   << " on " << theDef->GetParticleName()
	   << G4endl;
    eFinal = 0.0;
  }

  theParticleChange.SetMomentumChange(nlv1.vect().unit());
  theParticleChange.SetEnergyChange(eFinal);
  
  G4double erec =  nlv0.e() - m2;
  if(erec > ekinlim) {
    G4DynamicParticle * aSec = new G4DynamicParticle(theDef, nlv0);
    theParticleChange.AddSecondary(aSec);
  } else {
    theParticleChange.SetLocalEnergyDeposit(erec);
  }

  return &theParticleChange;
}

G4double 
G4HadronElastic::SampleT(G4double, G4double, G4double, G4double atno2)
{
  // G4cout << "Entering elastic scattering 2"<<G4endl;
  // Compute the direction of elastic scattering.
  // It is planned to replace this code with a method based on
  // parameterized functions and a Monte Carlo method to invert the CDF.

  G4double ran = G4UniformRand();
  G4double aa, bb, cc, dd, rr;
  if (atno2 <= 62.) {
    aa = std::pow(atno2, 1.63);
    bb = 14.5*std::pow(atno2, 0.66);
    cc = 1.4*std::pow(atno2, 0.33);
    dd = 10.;
  } else {
    aa = std::pow(atno2, 1.33);
    bb = 60.*std::pow(atno2, 0.33);
    cc = 0.4*std::pow(atno2, 0.40);
    dd = 10.;
  }
  aa = aa/bb;
  cc = cc/dd;
  rr = (aa + cc)*ran;
  if (verboseLevel > 1) {
    G4cout << "DoIt: aa,bb,cc,dd,rr" << G4endl;
    G4cout << aa << " " << bb << " " << cc << " " << dd << " " << rr << G4endl;
  }
  G4double t1 = -std::log(ran)/bb;
  G4double t2 = -std::log(ran)/dd;
  if (verboseLevel > 1) {
    G4cout << "t1,Fctcos " << t1 << " " << Fctcos(t1, aa, bb, cc, dd, rr) << G4endl;
    G4cout << "t2,Fctcos " << t2 << " " << Fctcos(t2, aa, bb, cc, dd, rr) << G4endl;
  }
  G4double eps = 0.001;
  G4int ind1 = 10;
  G4double t = 0.0;
  G4int ier1;
  ier1 = Rtmi(&t, t1, t2, eps, ind1,
	      aa, bb, cc, dd, rr);
  if (verboseLevel > 1) {
    G4cout << "From Rtmi, ier1=" << ier1 << G4endl;
    G4cout << "t, Fctcos " << t << " " << Fctcos(t, aa, bb, cc, dd, rr) << G4endl;
  }
  if (ier1 != 0) t = 0.25*(3.*t1 + t2);
  if (verboseLevel > 1) {
      G4cout << "t, Fctcos " << t << " " << Fctcos(t, aa, bb, cc, dd, rr) << 
              G4endl;
  }
  return t;
}

// The following is a "translation" of a root-finding routine
// from GEANT3.21/GHEISHA.  Some of the labelled block structure has
// been retained for clarity.  This routine will not be needed after
// the planned revisions to DoIt().

G4int
G4HadronElastic::Rtmi(G4double* x, G4double xli, G4double xri, G4double eps, 
		      G4int iend, 
		      G4double aa, G4double bb, G4double cc, G4double dd, 
		      G4double rr)
{
   G4int ier = 0;
   G4double xl = xli;
   G4double xr = xri;
   *x = xl;
   G4double tol = *x;
   G4double f = Fctcos(tol, aa, bb, cc, dd, rr);
   if (f == 0.) return ier;
   G4double fl, fr;
   fl = f;
   *x = xr;
   tol = *x;
   f = Fctcos(tol, aa, bb, cc, dd, rr);
   if (f == 0.) return ier;
   fr = f;

// Error return in case of wrong input data
   if (fl*fr >= 0.) {
      ier = 2;
      return ier;
   }

// Basic assumption fl*fr less than 0 is satisfied.
// Generate tolerance for function values.
   G4int i = 0;
   G4double tolf = 100.*eps;

// Start iteration loop
label4:
   i++;

// Start bisection loop
   for (G4int k = 1; k <= iend; k++) {
      *x = 0.5*(xl + xr);
      tol = *x;
      f = Fctcos(tol, aa, bb, cc, dd, rr);
      if (f == 0.) return 0;
      if (f*fr < 0.) {      // Interchange xl and xr in order to get the
         tol = xl;          // same Sign in f and fr
         xl = xr;
         xr = tol;
         tol = fl;
         fl = fr;
         fr = tol;
      }
      tol = f - fl;
      G4double a = f*tol;
      a = a + a;
      if (a < fr*(fr - fl) && i <= iend) goto label17;
      xr = *x;
      fr = f;

// Test on satisfactory accuracy in bisection loop
      tol = eps;
      a = std::abs(xr);
      if (a > 1.) tol = tol*a;
      if (std::abs(xr - xl) <= tol && std::abs(fr - fl) <= tolf) goto label14;
   }
// End of bisection loop

// No convergence after iend iteration steps followed by iend
// successive steps of bisection or steadily increasing function
// values at right bounds.  Error return.
   ier = 1;

label14:
   if (std::abs(fr) > std::abs(fl)) {
      *x = xl;
      f = fl;
   }
   return ier;

// Computation of iterated x-value by inverse parabolic interp
label17:
   G4double a = fr - f;
   G4double dx = (*x - xl)*fl*(1. + f*(a - tol)/(a*(fr - fl)))/tol;
   G4double xm = *x;
   G4double fm = f;
   *x = xl - dx;
   tol = *x;
   f = Fctcos(tol, aa, bb, cc, dd, rr);
   if (f == 0.) return ier;

// Test on satisfactory accuracy in iteration loop
   tol = eps;
   a = std::abs(*x);
   if (a > 1) tol = tol*a;
   if (std::abs(dx) <= tol && std::abs(f) <= tolf) return ier;

// Preparation of next bisection loop
   if (f*fl < 0.) {
      xr = *x;
      fr = f;
   }
   else {
      xl = *x;
      fl = f;
      xr = xm;
      fr = fm;
   }
   goto label4;
}


// Test function for root-finder

G4double
G4HadronElastic::Fctcos(G4double t, 
			G4double aa, G4double bb, G4double cc, G4double dd, 
			G4double rr)
{
   const G4double expxl = -82.;
   const G4double expxu = 82.;

   G4double test1 = -bb*t;
   if (test1 > expxu) test1 = expxu;
   if (test1 < expxl) test1 = expxl;

   G4double test2 = -dd*t;
   if (test2 > expxu) test2 = expxu;
   if (test2 < expxl) test2 = expxl;

   return aa*std::exp(test1) + cc*std::exp(test2) - rr;
}


void
G4HadronElastic::Defs1(G4double p, G4double px, G4double py, G4double pz, 
		       G4double pxinc, G4double pyinc, G4double pzinc, 
		       G4double* pxnew, G4double* pynew, G4double* pznew)
{
// Transform scattered particle to reflect direction of incident particle
   G4double pt2 = pxinc*pxinc + pyinc*pyinc;
   if (pt2 > 0.) {
      G4double cost = pzinc/p;
      G4double sint1 = std::sqrt(std::abs((1. - cost )*(1.+cost)));
      G4double sint2 = std::sqrt(pt2)/p;
      G4double sint = 0.5*(sint1 + sint2);
      G4double ph = pi*0.5;
      if (pyinc < 0.) ph = pi*1.5;
      if (std::abs(pxinc) > 1.e-6) ph = std::atan2(pyinc, pxinc);
      G4double cosp = std::cos(ph);
      G4double sinp = std::sin(ph);
      if (verboseLevel > 1) {
         G4cout << "cost sint " << cost << " " << sint << G4endl;
         G4cout << "cosp sinp " << cosp << " " << sinp << G4endl;
      }
      *pxnew = cost*cosp*px - sinp*py + sint*cosp*pz;
      *pynew = cost*sinp*px + cosp*py + sint*sinp*pz;
      *pznew =     -sint*px                 +cost*pz;
   }
   else {
       G4double cost=pzinc/p;
       *pxnew = cost*px;
       *pynew = py;
       *pznew = cost*pz;
   }
}
