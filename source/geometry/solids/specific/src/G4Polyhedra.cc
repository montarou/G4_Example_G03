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
// $Id: G4Polyhedra.cc,v 1.25 2005-08-08 14:52:20 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// --------------------------------------------------------------------
// GEANT 4 class source file
//
//
// G4Polyhedra.cc
//
// Implementation of a CSG polyhedra, as an inherited class of G4VCSGfaceted.
//
// To be done:
//    * Cracks: there are probably small cracks in the seams between the
//      phi face (G4PolyPhiFace) and sides (G4PolyhedraSide) that are not
//      entirely leakproof. Also, I am not sure all vertices are leak proof.
//    * Many optimizations are possible, but not implemented.
//    * Visualization needs to be updated outside of this routine.
//
// Utility classes:
//    * G4EnclosingCylinder: I decided a quick check of geometry would be a
//      good idea (for CPU speed). If the quick check fails, the regular
//      full-blown G4VCSGfaceted version is invoked.
//    * G4ReduciblePolygon: Really meant as a check of input parameters,
//      this utility class also "converts" the GEANT3-like PGON/PCON
//      arguments into the newer ones.
// Both these classes are implemented outside this file because they are
// shared with G4Polycone.
//
// --------------------------------------------------------------------

#include "G4Polyhedra.hh"

#include "G4PolyhedraSide.hh"
#include "G4PolyPhiFace.hh"

#include "Randomize.hh"

#include "G4Polyhedron.hh"
#include "G4EnclosingCylinder.hh"
#include "G4ReduciblePolygon.hh"
#include "G4VPVParameterisation.hh"

//
// Constructor (GEANT3 style parameters)
//
// GEANT3 PGON radii are specified in the distance to the norm of each face.
//  
G4Polyhedra::G4Polyhedra( const G4String& name, 
                                G4double phiStart,
                                G4double thePhiTotal,
                                G4int theNumSide,  
                                G4int numZPlanes,
                          const G4double zPlane[],
                          const G4double rInner[],
                          const G4double rOuter[]  )
  : G4VCSGfaceted( name )
{
  if (theNumSide <= 0)
  {
    G4cerr << "ERROR - G4Polyhedra::G4Polyhedra(): " << GetName() << G4endl
           << "        No sides specified !"
           << G4endl;
    G4Exception("G4Polyhedra::G4Polyhedra()", "InvalidSetup",
                FatalException, "Solid must have at least one side.");
  }

  //
  // Calculate conversion factor from G3 radius to G4 radius
  //
  G4double phiTotal = thePhiTotal;
  if ( (phiTotal <=0) || (phiTotal >= twopi*(1-DBL_EPSILON)) )
    phiTotal = twopi;
  G4double convertRad = std::cos(0.5*phiTotal/theNumSide);

  //
  // Some historical stuff
  //
  original_parameters = new G4PolyhedraHistorical;
  
  original_parameters->numSide = theNumSide;
  original_parameters->Start_angle = phiStart;
  original_parameters->Opening_angle = phiTotal;
  original_parameters->Num_z_planes = numZPlanes;
  original_parameters->Z_values = new G4double[numZPlanes];
  original_parameters->Rmin = new G4double[numZPlanes];
  original_parameters->Rmax = new G4double[numZPlanes];

  G4int i;
  for (i=0; i<numZPlanes; i++)
  {
    if (( i < numZPlanes-1) && ( zPlane[i] == zPlane[i+1] ))
    {
      if( (rInner[i]   > rOuter[i+1])
        ||(rInner[i+1] > rOuter[i])   )
      {
        DumpInfo();
        G4cerr << "ERROR - G4Polyhedra::G4Polyhedra()"
               << G4endl
               << "        Segments are not contiguous !" << G4endl
               << "        rMin[" << i << "] = " << rInner[i]
               << " -- rMax[" << i+1 << "] = " << rOuter[i+1] << G4endl
               << "        rMin[" << i+1 << "] = " << rInner[i+1]
               << " -- rMax[" << i << "] = " << rOuter[i] << G4endl;
        G4Exception("G4Polyhedra::G4Polyhedra()","InvalidSetup",FatalException, 
                    "Cannot create a Polyhedra with no contiguous segments.");
      }
    }
    original_parameters->Z_values[i] = zPlane[i];
    original_parameters->Rmin[i] = rInner[i]/convertRad;
    original_parameters->Rmax[i] = rOuter[i]/convertRad;
  }
  
  
  //
  // Build RZ polygon using special PCON/PGON GEANT3 constructor
  //
  G4ReduciblePolygon *rz =
    new G4ReduciblePolygon( rInner, rOuter, zPlane, numZPlanes );
  rz->ScaleA( 1/convertRad );
  
  //
  // Do the real work
  //
  Create( phiStart, phiTotal, theNumSide, rz );
  
  delete rz;
}


//
// Constructor (generic parameters)
//
G4Polyhedra::G4Polyhedra( const G4String& name, 
                                G4double phiStart,
                                G4double phiTotal,
                                G4int    theNumSide,  
                                G4int    numRZ,
                          const G4double r[],
                          const G4double z[]   )
  : G4VCSGfaceted( name )
{ 
  G4ReduciblePolygon *rz = new G4ReduciblePolygon( r, z, numRZ );
  
  Create( phiStart, phiTotal, theNumSide, rz );
  
  // Set original_parameters struct for consistency
  //
  SetOriginalParameters();
   
  delete rz;
}


//
// Create
//
// Generic create routine, called by each constructor
// after conversion of arguments
//
void G4Polyhedra::Create( G4double phiStart,
                          G4double phiTotal,
                          G4int    theNumSide,  
                          G4ReduciblePolygon *rz  )
{
  //
  // Perform checks of rz values
  //
  if (rz->Amin() < 0.0)
  {
    G4cerr << "ERROR - G4Polyhedra::Create() " << GetName() << G4endl
           << "        All R values must be >= 0 !"
           << G4endl;
    G4Exception("G4Polyhedra::Create()", "InvalidSetup",
                FatalException, "Illegal input parameters.");
  }

  G4double rzArea = rz->Area();
  if (rzArea < -kCarTolerance)
    rz->ReverseOrder();

  else if (rzArea < -kCarTolerance)
  {
    G4cerr << "ERROR - G4Polyhedra::Create() " << GetName() << G4endl
           << "        R/Z cross section is zero or near zero: "
           << rzArea << G4endl;
    G4Exception("G4Polyhedra::Create()", "InvalidSetup",
                FatalException, "Illegal input parameters.");
  }
    
  if ( (!rz->RemoveDuplicateVertices( kCarTolerance ))
    || (!rz->RemoveRedundantVertices( kCarTolerance )) ) 
  {
    G4cerr << "ERROR - G4Polyhedra::Create() " << GetName() << G4endl
           << "        Too few unique R/Z values !"
           << G4endl;
    G4Exception("G4Polyhedra::Create()", "InvalidSetup",
                FatalException, "Illegal input parameters.");
  }

  if (rz->CrossesItself( 1/kInfinity )) 
  {
    G4cerr << "ERROR - G4Polyhedra::Create() " << GetName() << G4endl
           << "        R/Z segments cross !"
           << G4endl;
    G4Exception("G4Polyhedra::Create()", "InvalidSetup",
                FatalException, "Illegal input parameters.");
  }

  numCorner = rz->NumVertices();


  startPhi = phiStart;
  while( startPhi < 0 ) startPhi += twopi;
  //
  // Phi opening? Account for some possible roundoff, and interpret
  // nonsense value as representing no phi opening
  //
  if ( (phiTotal <= 0) || (phiTotal > twopi*(1-DBL_EPSILON)) )
  {
    phiIsOpen = false;
    endPhi = phiStart+twopi;
  }
  else
  {
    phiIsOpen = true;
    
    //
    // Convert phi into our convention
    //
    endPhi = phiStart+phiTotal;
    while( endPhi < startPhi ) endPhi += twopi;
  }
  
  //
  // Save number sides
  //
  numSide = theNumSide;
  
  //
  // Allocate corner array. 
  //
  corners = new G4PolyhedraSideRZ[numCorner];

  //
  // Copy corners
  //
  G4ReduciblePolygonIterator iterRZ(rz);
  
  G4PolyhedraSideRZ *next = corners;
  iterRZ.Begin();
  do
  {
    next->r = iterRZ.GetA();
    next->z = iterRZ.GetB();
  } while( ++next, iterRZ.Next() );
  
  //
  // Allocate face pointer array
  //
  numFace = phiIsOpen ? numCorner+2 : numCorner;
  faces = new G4VCSGface*[numFace];
  
  //
  // Construct side faces
  //
  // To do so properly, we need to keep track of four successive RZ
  // corners.
  //
  // But! Don't construct a face if both points are at zero radius!
  //
  G4PolyhedraSideRZ *corner = corners,
                    *prev = corners + numCorner-1,
                    *nextNext;
  G4VCSGface   **face = faces;
  do
  {
    next = corner+1;
    if (next >= corners+numCorner) next = corners;
    nextNext = next+1;
    if (nextNext >= corners+numCorner) nextNext = corners;
    
    if (corner->r < 1/kInfinity && next->r < 1/kInfinity) continue;

    //
    // We must decide here if we can dare declare one of our faces
    // as having a "valid" normal (i.e. allBehind = true). This
    // is never possible if the face faces "inward" in r *unless*
    // we have only one side
    //
    G4bool allBehind;
    if ((corner->z > next->z) && (numSide > 1))
    {
      allBehind = false;
    }
    else
    {
      //
      // Otherwise, it is only true if the line passing
      // through the two points of the segment do not
      // split the r/z cross section
      //
      allBehind = !rz->BisectedBy( corner->r, corner->z,
                                   next->r, next->z, kCarTolerance );
    }
    
    *face++ = new G4PolyhedraSide( prev, corner, next, nextNext,
                 numSide, startPhi, endPhi-startPhi, phiIsOpen );
  } while( prev=corner, corner=next, corner > corners );
  
  if (phiIsOpen)
  {
    //
    // Construct phi open edges
    //
    *face++ = new G4PolyPhiFace( rz, startPhi, phiTotal/numSide, endPhi );
    *face++ = new G4PolyPhiFace( rz, endPhi,   phiTotal/numSide, startPhi );
  }
  
  //
  // We might have dropped a face or two: recalculate numFace
  //
  numFace = face-faces;
  
  //
  // Make enclosingCylinder
  //
  enclosingCylinder =
    new G4EnclosingCylinder( rz, phiIsOpen, phiStart, phiTotal );
}


//
// Destructor
//
G4Polyhedra::~G4Polyhedra()
{
  delete [] corners;
  if (original_parameters) delete original_parameters;
  
  delete enclosingCylinder;
}


//
// Copy constructor
//
G4Polyhedra::G4Polyhedra( const G4Polyhedra &source )
  : G4VCSGfaceted( source )
{
  CopyStuff( source );
}


//
// Assignment operator
//
const G4Polyhedra &G4Polyhedra::operator=( const G4Polyhedra &source )
{
  if (this == &source) return *this;

  G4VCSGfaceted::operator=( source );
  
  delete [] corners;
  if (original_parameters) delete original_parameters;
  
  delete enclosingCylinder;
  
  CopyStuff( source );
  
  return *this;
}


//
// CopyStuff
//
void G4Polyhedra::CopyStuff( const G4Polyhedra &source )
{
  //
  // Simple stuff
  //
  numSide    = source.numSide;
  startPhi  = source.startPhi;
  endPhi    = source.endPhi;
  phiIsOpen  = source.phiIsOpen;
  numCorner  = source.numCorner;

  //
  // The corner array
  //
  corners = new G4PolyhedraSideRZ[numCorner];
  
  G4PolyhedraSideRZ  *corn = corners,
        *sourceCorn = source.corners;
  do
  {
    *corn = *sourceCorn;
  } while( ++sourceCorn, ++corn < corners+numCorner );
  
  //
  // Original parameters
  //
  if (source.original_parameters)
  {
    original_parameters =
      new G4PolyhedraHistorical( *source.original_parameters );
  }
  
  //
  // Enclosing cylinder
  //
  enclosingCylinder = new G4EnclosingCylinder( *source.enclosingCylinder );
}


//
// Reset
//
// Recalculates and reshapes the solid, given pre-assigned
// original_parameters.
//
G4bool G4Polyhedra::Reset()
{
  if (!original_parameters)
  {
    G4Exception("G4Polyhedra::Reset()", "NotApplicableConstruct",
                JustWarning, "Parameters NOT resetted.");
    G4cerr << "Solid " << GetName() << " built using generic construct."
           << G4endl << "Specify original parameters first !" << G4endl;
    return 1;
  }

  //
  // Clear old setup
  //
  G4VCSGfaceted::DeleteStuff();
  delete [] corners;
  delete enclosingCylinder;

  //
  // Rebuild polyhedra
  //
  G4ReduciblePolygon *rz =
    new G4ReduciblePolygon( original_parameters->Rmin,
                            original_parameters->Rmax,
                            original_parameters->Z_values,
                            original_parameters->Num_z_planes );
  //
  // Calculate conversion factor
  //
  G4double phiTotal = original_parameters->Opening_angle;
  if ( (phiTotal <=0) || (phiTotal >= twopi*(1-DBL_EPSILON)) )
    phiTotal = twopi;
  G4double convertRad = std::cos(0.5*phiTotal/original_parameters->numSide);
  rz->ScaleA( 1/convertRad );

  Create( original_parameters->Start_angle, phiTotal,
          original_parameters->numSide, rz );
  delete rz;

  return 0;
}


//
// Inside
//
// This is an override of G4VCSGfaceted::Inside, created in order
// to speed things up by first checking with G4EnclosingCylinder.
//
EInside G4Polyhedra::Inside( const G4ThreeVector &p ) const
{
  //
  // Quick test
  //
  if (enclosingCylinder->MustBeOutside(p)) return kOutside;

  //
  // Long answer
  //
  return G4VCSGfaceted::Inside(p);
}


//
// DistanceToIn
//
// This is an override of G4VCSGfaceted::Inside, created in order
// to speed things up by first checking with G4EnclosingCylinder.
//
G4double G4Polyhedra::DistanceToIn( const G4ThreeVector &p,
                                    const G4ThreeVector &v ) const
{
  //
  // Quick test
  //
  if (enclosingCylinder->ShouldMiss(p,v))
    return kInfinity;
  
  //
  // Long answer
  //
  return G4VCSGfaceted::DistanceToIn( p, v );
}


//
// DistanceToIn
//
G4double G4Polyhedra::DistanceToIn( const G4ThreeVector &p ) const
{
  return G4VCSGfaceted::DistanceToIn(p);
}


//
// ComputeDimensions
//
void G4Polyhedra::ComputeDimensions(       G4VPVParameterisation* p,
                                     const G4int n,
                                     const G4VPhysicalVolume* pRep )
{
  p->ComputeDimensions(*this,n,pRep);
}


//
// GetEntityType
//
G4GeometryType G4Polyhedra::GetEntityType() const
{
  return G4String("G4Polyhedra");
}


//
// Stream object contents to an output stream
//
std::ostream& G4Polyhedra::StreamInfo( std::ostream& os ) const
{
  os << "-----------------------------------------------------------\n"
     << "    *** Dump for solid - " << GetName() << " ***\n"
     << "    ===================================================\n"
     << " Solid type: G4Polyhedra\n"
     << " Parameters: \n"
     << "    starting phi angle : " << startPhi/degree << " degrees \n"
     << "    ending phi angle   : " << endPhi/degree << " degrees \n";
  G4int i=0;
  if (original_parameters)
  {
    G4int numPlanes = original_parameters->Num_z_planes;
    os << "    number of Z planes: " << numPlanes << "\n"
       << "              Z values: \n";
    for (i=0; i<numPlanes; i++)
    {
      os << "              Z plane " << i << ": "
         << original_parameters->Z_values[i] << "\n";
    }
    os << "              Tangent distances to inner surface (Rmin): \n";
    for (i=0; i<numPlanes; i++)
    {
      os << "              Z plane " << i << ": "
         << original_parameters->Rmin[i] << "\n";
    }
    os << "              Tangent distances to outer surface (Rmax): \n";
    for (i=0; i<numPlanes; i++)
    {
      os << "              Z plane " << i << ": "
         << original_parameters->Rmax[i] << "\n";
    }
  }
  os << "    number of RZ points: " << numCorner << "\n"
     << "              RZ values (corners): \n";
     for (i=0; i<numCorner; i++)
     {
       os << "                         "
          << corners[i].r << ", " << corners[i].z << "\n";
     }
  os << "-----------------------------------------------------------\n";

  return os;
}


//
// GetPointOnPlane
//
// Auxiliary method for get point on surface
//
G4ThreeVector G4Polyhedra::GetPointOnPlane(G4ThreeVector p0, G4ThreeVector p1, 
                                           G4ThreeVector p2, G4ThreeVector p3) const
{
  G4double lambda1, lambda2, chose,aOne,aTwo;
  G4ThreeVector t, u, v, w, Area, normal;
  aOne = 1.;
  aTwo = 1.;

  t = p1 - p0;
  u = p2 - p1;
  v = p3 - p2;
  w = p0 - p3;

  chose = RandFlat::shoot(0.,aOne+aTwo);
  if( (chose>=0.) && (chose < aOne) )
  {
    lambda1 = RandFlat::shoot(0.,1.);
    lambda2 = RandFlat::shoot(0.,lambda1);
    return (p2+lambda1*v+lambda2*w);    
  }

  lambda1 = RandFlat::shoot(0.,1.);
  lambda2 = RandFlat::shoot(0.,lambda1);
  return (p0+lambda1*t+lambda2*u);
}


//
// GetPointOnTriangle
//
// Auxiliary method for get point on surface
//
G4ThreeVector G4Polyhedra::GetPointOnTriangle(G4ThreeVector p1,
                                              G4ThreeVector p2,
                                              G4ThreeVector p3) const
{
  G4double lambda1,lambda2;
  G4ThreeVector v=p3-p1, w=p1-p2;

  lambda1 = RandFlat::shoot(0.,1.);
  lambda2 = RandFlat::shoot(0.,lambda1);

  return (p2 + lambda1*w + lambda2*v);
}


//
// GetPointOnSurface
//
G4ThreeVector G4Polyhedra::GetPointOnSurface() const
{
  G4int j, numPlanes = original_parameters->Num_z_planes, Flag=0;
  G4double chose, totArea=0., Achose1, Achose2,
           rad1, rad2, sinphi1, sinphi2, cosphi1, cosphi2; 
  G4double a, b, l2, rang,
           ksi = (endPhi-startPhi)/(double)numSide,
           area, aTop=0., aBottom=0.,zVal=0.;
  G4ThreeVector p0, p1, p2, p3;
  std::vector<G4double> aVector1;
  std::vector<G4double> aVector2;
  std::vector<G4double> aVector3;

  G4double cosksi = std::cos(ksi/2.);

  // below we generate the areas relevant to our solid
  //
  for(j=0; j<numPlanes-1; j++)
  {
    a = original_parameters->Rmax[j+1];
    b = original_parameters->Rmax[j];
    l2 = sqr(original_parameters->Z_values[j]
            -original_parameters->Z_values[j+1]) + sqr(b-a);
    area = std::sqrt(l2-sqr((a-b)*cosksi))*(a+b)*cosksi;
    aVector1.push_back(area);
  }
  
  for(j=0; j<numPlanes-1; j++)
  {
    a = original_parameters->Rmin[j+1];//*cosksi;
    b = original_parameters->Rmin[j];//*cosksi;
    l2 = sqr(original_parameters->Z_values[j]
            -original_parameters->Z_values[j+1]) + sqr(b-a);
    area = std::sqrt(l2-sqr((a-b)*cosksi))*(a+b)*cosksi;
    aVector2.push_back(area);
  }
  
  for(j=0; j<numPlanes-1; j++)
  {
    if(phiIsOpen == true)
    {
      aVector3.push_back(0.5*(original_parameters->Rmax[j]
                             -original_parameters->Rmin[j]
                             +original_parameters->Rmax[j+1]
                             -original_parameters->Rmin[j+1])
      *std::fabs(original_parameters->Z_values[j+1]
                -original_parameters->Z_values[j]));
    }
    else { aVector3.push_back(0.); } 
  }
  
  for(j=0; j<numPlanes-1; j++)
  {
    totArea += numSide*(aVector1[j]+aVector2[j])+2.*aVector3[j];
  }
  
  // must include top and bottom areas
  if(original_parameters->Rmax[numPlanes-1] != 0.)
  {
    a = original_parameters->Rmax[numPlanes-1];
    b = original_parameters->Rmin[numPlanes-1];
    l2 = sqr(a-b);
    aTop = std::sqrt(l2-sqr((a-b)*cosksi))*(a+b)*cosksi; 
  }

  if(original_parameters->Rmax[0] != 0.)
  {
    a = original_parameters->Rmax[0];
    b = original_parameters->Rmin[0];
    l2 = sqr(a-b);
    aBottom = std::sqrt(l2-sqr((a-b)*cosksi))*(a+b)*cosksi; 
  }

  Achose1 = 0.;
  Achose2 = numSide*(aVector1[0]+aVector2[0])+2.*aVector3[0];

  chose = RandFlat::shoot(0.,totArea+aTop+aBottom);
  if( (chose >= 0.) && (chose < aTop + aBottom) )
  {  
    chose = RandFlat::shoot(startPhi,endPhi);
    rang = std::floor((chose-startPhi)/ksi-0.01);
    rang = std::fabs(rang);  
    sinphi1 = std::sin(startPhi+rang*ksi);
    sinphi2 = std::sin(startPhi+(rang+1)*ksi);
    cosphi1 = std::cos(startPhi+rang*ksi);
    cosphi2 = std::cos(startPhi+(rang+1)*ksi);
    
    chose = RandFlat::shoot(0., aTop + aBottom);
    if(chose>=0. && chose<aTop)
    {
      rad1 = original_parameters->Rmin[numPlanes-1];
      rad2 = original_parameters->Rmax[numPlanes-1];
      zVal = original_parameters->Z_values[numPlanes-1]; 
    }
    else 
    {
      rad1 = original_parameters->Rmin[0];
      rad2 = original_parameters->Rmax[0];
      zVal = original_parameters->Z_values[0]; 
    }
    p0 = G4ThreeVector(rad1*cosphi1,rad1*sinphi1,zVal);
    p1 = G4ThreeVector(rad2*cosphi1,rad2*sinphi1,zVal);
    p2 = G4ThreeVector(rad2*cosphi2,rad2*sinphi2,zVal);
    p3 = G4ThreeVector(rad1*cosphi2,rad1*sinphi2,zVal);
    return GetPointOnPlane(p0,p1,p2,p3); 
  }
  else
  {
    for (j=0; j< numPlanes-1; j++)
    {
      if(chose>=Achose1 && chose < Achose2){ Flag = j; }
      Achose1 += numSide*(aVector1[j]+aVector2[j])+2.*aVector3[j];
      Achose2 = Achose1 + numSide*(aVector1[j+1]+aVector2[j+1])
                        + 2.*aVector3[j+1];
    }
  }

  // at this point we have chosen a subsection
  // between to adjacent plane cuts...

  j = Flag; 
    
  totArea = numSide*(aVector1[j]+aVector2[j])+2.*aVector3[j];
  chose = RandFlat::shoot(0.,totArea);
  
  if( (chose>=0.) && (chose<numSide*aVector1[j]) )
  {
    chose = RandFlat::shoot(startPhi,endPhi);
    rang = std::floor((chose-startPhi)/ksi-0.01); 
    rang = std::fabs(rang);
    rad1 = original_parameters->Rmax[j];
    rad2 = original_parameters->Rmax[j+1];
    sinphi1 = std::sin(startPhi+rang*ksi);
    sinphi2 = std::sin(startPhi+(rang+1)*ksi);
    cosphi1 = std::cos(startPhi+rang*ksi);
    cosphi2 = std::cos(startPhi+(rang+1)*ksi);
    zVal = original_parameters->Z_values[j];
    
    p0 = G4ThreeVector(rad1*cosphi1,rad1*sinphi1,zVal);
    p1 = G4ThreeVector(rad1*cosphi2,rad1*sinphi2,zVal);

    zVal = original_parameters->Z_values[j+1];

    p2 = G4ThreeVector(rad2*cosphi2,rad2*sinphi2,zVal);
    p3 = G4ThreeVector(rad2*cosphi1,rad2*sinphi1,zVal);
    
    return GetPointOnPlane(p0,p1,p2,p3);
  }
  else if ( (chose >= numSide*aVector1[j])
         && (chose <= numSide*(aVector1[j]+aVector2[j])) )
  {
    chose = RandFlat::shoot(startPhi,endPhi);
    rang = std::floor((chose-startPhi)/ksi-0.01);
    rang = std::fabs(rang);
    rad1 = original_parameters->Rmin[j];
    rad2 = original_parameters->Rmin[j+1];
    sinphi1 = std::sin(startPhi+rang*ksi);
    sinphi2 = std::sin(startPhi+(rang+1)*ksi);
    cosphi1 = std::cos(startPhi+rang*ksi);
    cosphi2 = std::cos(startPhi+(rang+1)*ksi);
    zVal = original_parameters->Z_values[j];
    
    p0 = G4ThreeVector(rad1*cosphi1,rad1*sinphi1,zVal);
    p1 = G4ThreeVector(rad1*cosphi2,rad1*sinphi2,zVal);

    zVal = original_parameters->Z_values[j+1];
    
    p2 = G4ThreeVector(rad2*cosphi2,rad2*sinphi2,zVal);
    p3 = G4ThreeVector(rad2*cosphi1,rad2*sinphi1,zVal);
    
    return GetPointOnPlane(p0,p1,p2,p3);
  }

  chose = RandFlat::shoot(0.,2.2);
  if( (chose>=0.) && (chose < 1.) )
  {
    rang = startPhi;
  }
  else
  {
    rang = endPhi;
  } 

  cosphi1 = std::cos(rang); rad1 = original_parameters->Rmin[j];
  sinphi1 = std::sin(rang); rad2 = original_parameters->Rmax[j];
    
  p0 = G4ThreeVector(rad1*cosphi1,rad1*sinphi1,
                     original_parameters->Z_values[j]);
  p1 = G4ThreeVector(rad2*cosphi1,rad2*sinphi1,
                     original_parameters->Z_values[j]);
    
  rad1 = original_parameters->Rmax[j+1];
  rad2 = original_parameters->Rmin[j+1];
     
  p2 = G4ThreeVector(rad1*cosphi1,rad1*sinphi1,
                     original_parameters->Z_values[j+1]);
  p3 = G4ThreeVector(rad2*cosphi1,rad2*sinphi1,
                     original_parameters->Z_values[j+1]);
    
  return GetPointOnPlane(p0,p1,p2,p3);
}


//
// CreatePolyhedron
//
G4Polyhedron* G4Polyhedra::CreatePolyhedron() const
{ 
  return new G4PolyhedronPgon( original_parameters->Start_angle,
                               original_parameters->Opening_angle,
                               original_parameters->numSide,
                               original_parameters->Num_z_planes,
                               original_parameters->Z_values,
                               original_parameters->Rmin,
                               original_parameters->Rmax);
}  


//
// CreateNURBS
//
G4NURBS *G4Polyhedra::CreateNURBS() const
{
  return 0;
}


//
// G4PolyhedraHistorical stuff
//
G4PolyhedraHistorical::G4PolyhedraHistorical()
{
}

G4PolyhedraHistorical::~G4PolyhedraHistorical()
{
  delete [] Z_values;
  delete [] Rmin;
  delete [] Rmax;
}

G4PolyhedraHistorical::
G4PolyhedraHistorical( const G4PolyhedraHistorical& source )
{
  Start_angle   = source.Start_angle;
  Opening_angle = source.Opening_angle;
  numSide       = source.numSide;
  Num_z_planes  = source.Num_z_planes;
  
  Z_values = new G4double[Num_z_planes];
  Rmin     = new G4double[Num_z_planes];
  Rmax     = new G4double[Num_z_planes];
  
  for( G4int i = 0; i < Num_z_planes; i++)
  {
    Z_values[i] = source.Z_values[i];
    Rmin[i]     = source.Rmin[i];
    Rmax[i]     = source.Rmax[i];
  }
}

G4PolyhedraHistorical&
G4PolyhedraHistorical::operator=( const G4PolyhedraHistorical& right )
{
  if ( &right == this ) return *this;

  if (&right)
  {
    Start_angle   = right.Start_angle;
    Opening_angle = right.Opening_angle;
    numSide       = right.numSide;
    Num_z_planes  = right.Num_z_planes;
  
    delete [] Z_values;
    delete [] Rmin;
    delete [] Rmax;
    Z_values = new G4double[Num_z_planes];
    Rmin     = new G4double[Num_z_planes];
    Rmax     = new G4double[Num_z_planes];
  
    for( G4int i = 0; i < Num_z_planes; i++)
    {
      Z_values[i] = right.Z_values[i];
      Rmin[i]     = right.Rmin[i];
      Rmax[i]     = right.Rmax[i];
    }
  }
  return *this;
}
