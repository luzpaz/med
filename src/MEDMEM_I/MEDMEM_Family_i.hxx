//=============================================================================
// File      : MEDMEM_Family_i.hxx
// Project   : SALOME
// Copyright : EDF 2002
// Author    : EDF
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_Family_i.hxx
//=============================================================================
#ifndef MED_FAMILY_I_HXX_
#define MED_FAMILY_I_HXX_


#include "MEDMEM_Support_i.hxx"

namespace MEDMEM {
class FAMILY;
class FAMILY_i:
		public POA_SALOME_MED::FAMILY,
		public SUPPORT_i
{
protected :
    FAMILY_i();
    ~FAMILY_i();

    // C++ object containing values
     const ::MEDMEM::FAMILY * const _family ;

public :
    // Constructors and associated internal methods
    FAMILY_i(const ::MEDMEM::FAMILY * const f);
    FAMILY_i(const FAMILY_i & f);

    CORBA::Long 	      getIdentifier()      
				     throw (SALOME::SALOME_Exception);
    CORBA::Long               getNumberOfAttributes() 
				     throw (SALOME::SALOME_Exception);
    SALOME_MED::long_array*   getAttributesIdentifiers() 
			      	     throw (SALOME::SALOME_Exception);
    CORBA::Long               getAttributeIdentifier(CORBA::Long i) 
				     throw (SALOME::SALOME_Exception);
    SALOME_MED::long_array*   getAttributesValues() 
				     throw (SALOME::SALOME_Exception);
    CORBA::Long     	      getAttributeValue(CORBA::Long i) 
				     throw (SALOME::SALOME_Exception);
    SALOME_MED::string_array* getAttributesDescriptions() 
				     throw (SALOME::SALOME_Exception);
    char*  		      getAttributeDescription( CORBA::Long i) 
				     throw (SALOME::SALOME_Exception);
    CORBA::Long               getNumberOfGroups()
                                     throw (SALOME::SALOME_Exception);
    char *                    getGroupName( CORBA::Long i)
                                     throw (SALOME::SALOME_Exception);
    SALOME_MED::string_array* getGroupsNames()
                                     throw (SALOME::SALOME_Exception);

};
}
#endif /* MED_FAMILY_I_HXX_ */
