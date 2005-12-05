// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
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
