//=============================================================================
// File      : Family_i.hxx
// Project   : SALOME
// Copyright : EDF 2002
// Author    : EDF
// $Header: /export/home/CVS/SALOME_ROOT/MED/src/MedMem/Family_i.hxx
//=============================================================================
#ifndef MED_FAMILY_I_HXX_
#define MED_FAMILY_I_HXX_


#include "Support_i.hxx"

class FAMILY;
class FAMILY_i:
		public POA_SALOME_MED::FAMILY,
		public SUPPORT_i
{
protected :
    FAMILY_i();
    ~FAMILY_i();

    // C++ object containing values
     const ::FAMILY * const _family ;

public :
    // Constructors and associated internal methods
    FAMILY_i(const ::FAMILY * const f);
    FAMILY_i(const FAMILY_i & f);

    CORBA::Long 	   getIdentifier()      
				     throw (SALOME::SALOME_Exception);
    CORBA::Long            getNumberOfAttributes() 
				     throw (SALOME::SALOME_Exception);
    Engines::long_array*   getAttributesIdentifiers() 
			      	     throw (SALOME::SALOME_Exception);
    CORBA::Long            getAttributeIdentifier(CORBA::Long i) 
				     throw (SALOME::SALOME_Exception);
    Engines::long_array*   getAttributesValues() 
				     throw (SALOME::SALOME_Exception);
    CORBA::Long     	   getAttributeValue(CORBA::Long i) 
				     throw (SALOME::SALOME_Exception);
    Engines::string_array* getAttributesDescriptions() 
				     throw (SALOME::SALOME_Exception);
    char*  		   getAttributeDescription( CORBA::Long i) 
				     throw (SALOME::SALOME_Exception);

};
#endif /* MED_FAMILY_I_HXX_ */
