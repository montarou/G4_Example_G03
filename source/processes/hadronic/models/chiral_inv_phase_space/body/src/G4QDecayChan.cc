// This code implementation is the intellectual property of
// the RD44 GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: G4QDecayChan.cc,v 1.12 2001-11-21 11:21:47 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// -------------------------------------------------------------------
//      GEANT 4 class implementation file
//
//      For information related to this code contact:
//      CERN, CN Division, ASD group
//      ---------------- G4QDecayChan ----------------
//             by Mikhail Kossov, Sept 1999.
//      class for Decay Channels of Hadrons in CHIPS Model
// -------------------------------------------------------------------
 
//#define debug
//#define pdebug

#include "G4QDecayChanVector.hh"
#include "g4std/algorithm"

G4QDecayChan::G4QDecayChan(){}

G4QDecayChan::G4QDecayChan(G4double pLev, G4int PDG1, G4int PDG2, G4int PDG3):
  aDecayChanLimit(pLev)
{
  G4QPDGCode* firstPDG = new G4QPDGCode(PDG1);
  theMinMass =firstPDG->GetMass();
  aVecOfSecHadrons.push_back(firstPDG);
  G4QPDGCode* secondPDG = new G4QPDGCode(PDG2);
  theMinMass+=secondPDG->GetMass();
  aVecOfSecHadrons.push_back(secondPDG);
  if(PDG3) 
  {
    G4QPDGCode* thirdPDG = new G4QPDGCode(PDG3);
    theMinMass+=thirdPDG->GetMass();
    aVecOfSecHadrons.push_back(thirdPDG);
  }
#ifdef debug
  cout<<"G4QDecayChan is defined with pL="<<pLev<<",1="<<PDG1<<",2="<<PDG2<<",3="<<PDG3
      <<",m1="<<firstPDG->GetMass()<<",m2="<<secondPDG->GetMass()<<",minM="<<theMinMass<<endl;
#endif
}

G4QDecayChan::G4QDecayChan(const G4QDecayChan& right)
{
  aDecayChanLimit     = right.aDecayChanLimit;
  theMinMass          = right.theMinMass;
  //aVecOfSecHadrons (Vector)
  G4int nSH           = right.aVecOfSecHadrons.size();
  if(nSH) for(G4int ih=0; ih<nSH; ih++)
  {
    G4QPDGCode* curPC = new G4QPDGCode(right.aVecOfSecHadrons[ih]);
    aVecOfSecHadrons.push_back(curPC);
  }
}

G4QDecayChan::G4QDecayChan(G4QDecayChan* right)
{
  aDecayChanLimit     = right->aDecayChanLimit;
  theMinMass          = right->theMinMass;
  //aVecOfSecHadrons (Vector)
  G4int nSH           = right->aVecOfSecHadrons.size();
  if(nSH) for(G4int ih=0; ih<nSH; ih++)
  {
    G4QPDGCode* curPC = new G4QPDGCode(right->aVecOfSecHadrons[ih]);
    aVecOfSecHadrons.push_back(curPC);
  }
}

G4QDecayChan::~G4QDecayChan() 
{
  G4std::for_each(aVecOfSecHadrons.begin(), aVecOfSecHadrons.end(), DeleteQPDGCode());
}

// Assignment operator
const G4QDecayChan& G4QDecayChan::operator=(const G4QDecayChan& right)
{
  aDecayChanLimit     = right.aDecayChanLimit;
  theMinMass          = right.theMinMass;
  //aVecOfSecHadrons (Vector)
  G4int nSH           = right.aVecOfSecHadrons.size();
  if(nSH) for(G4int ih=0; ih<nSH; ih++)
  {
    G4QPDGCode* curPC = new G4QPDGCode(right.aVecOfSecHadrons[ih]);
    aVecOfSecHadrons.push_back(curPC);
  }

  return *this;
}

// Standard output for QDecayChan
G4std::ostream& operator<<(G4std::ostream& lhs, G4QDecayChan& rhs)
//       =========================================
{
  lhs << "[L=" << rhs.GetDecayChanLimit(); 
  G4QPDGCodeVector VSH = rhs.GetVecOfSecHadrons();
  G4int n = VSH.size();
  lhs << ", N=" << n << ": ";
  for (int i=0; i<n; i++)
  {
    if(!i) lhs << ":";
    else   lhs << ",";
    lhs << VSH[i]->GetPDGCode();
  }
  lhs << "]";
  return lhs;
}





