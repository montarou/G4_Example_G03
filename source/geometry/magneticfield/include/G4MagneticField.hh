// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: G4MagneticField.hh,v 1.3 2000-04-27 09:14:06 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
// class G4MagneticField
//
// Class description:
//
// Magnetic Field abstract class, implements inquiry function interface.

// History:
// - Created. JA, January 13th, 1996.
// - Added default & copy constructors, virtual destructor and
//   assignment operator. G.Cosmo, November 5th, 1997.

#ifndef G4MAGNETIC_FIELD_DEF
#define G4MAGNETIC_FIELD_DEF

#include "G4Field.hh"

class G4MagneticField : public G4Field
{
  public:  // with description

     G4MagneticField();
     virtual ~G4MagneticField();
       // Constructor and destructor. No actions.

     G4MagneticField(const G4MagneticField &p);
     G4MagneticField& operator = (const G4MagneticField &p);
       // Copy constructor & assignment operator.

     virtual void  GetFieldValue( const  double Point[3],
					 double *Bfield ) const = 0;
};

// Implementation 

inline  G4MagneticField::G4MagneticField() {}
inline  G4MagneticField::~G4MagneticField() {}
inline  G4MagneticField::G4MagneticField(const G4MagneticField &p) {}
inline  G4MagneticField& G4MagneticField::operator = (const G4MagneticField &p)
 { *this = p; return *this; }

#endif /* G4MAGNETIC_FIELD_DEF */
