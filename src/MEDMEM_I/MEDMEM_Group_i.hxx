//=============================================================================
// File      : MEDMEM_Group_i.hxx
// Project   : SALOME
// Author    : EDF
// Copyright : EDF 2002
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_Group_i.hxx
//=============================================================================
#ifndef MED_GROUP_I_HXX_
#define MED_GROUP_I_HXX_

#include "MEDMEM_Support_i.hxx"

namespace MEDMEM {
class FAMILY_i;
class GROUP;

class GROUP_i:
                public POA_SALOME_MED::GROUP,
	        public SUPPORT_i
{
protected :
    GROUP_i();
    ~GROUP_i();

    // C++ object containing values
    const ::MEDMEM::GROUP * const  _group;

public :
    // Constructors and associated internal methods
    GROUP_i(const ::MEDMEM::GROUP * const g);
    GROUP_i(const GROUP_i & g);

    CORBA::Long    	      getNumberOfFamilies() 
					throw (SALOME::SALOME_Exception);
    SALOME_MED::Family_array* getFamilies() 	         
					throw (SALOME::SALOME_Exception);
    SALOME_MED::FAMILY_ptr    getFamily(CORBA::Long i) 
					throw (SALOME::SALOME_Exception);
};
}

#endif /* MED_GROUP_I_HXX_ */
