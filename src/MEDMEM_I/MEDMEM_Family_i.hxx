// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

//=============================================================================
// File      : MEDMEM_Family_i.hxx
// Project   : SALOME
// Author    : EDF
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_Family_i.hxx
//=============================================================================
//
#ifndef MED_FAMILY_I_HXX_
#define MED_FAMILY_I_HXX_

#include "MEDMEM_I.hxx"

#include "MEDMEM_Support_i.hxx"

namespace MEDMEM {
class FAMILY;
class MEDMEM_I_EXPORT FAMILY_i:
                virtual public POA_SALOME_MED::FAMILY,
                virtual public SUPPORT_i
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

    CORBA::Long               getIdentifier()      
                                     throw (SALOME::SALOME_Exception);
    CORBA::Long               getNumberOfAttributes() 
                                     throw (SALOME::SALOME_Exception);
    SALOME_TYPES::ListOfLong*   getAttributesIdentifiers() 
                                     throw (SALOME::SALOME_Exception);
    CORBA::Long               getAttributeIdentifier(CORBA::Long i) 
                                     throw (SALOME::SALOME_Exception);
    SALOME_TYPES::ListOfLong*   getAttributesValues() 
                                     throw (SALOME::SALOME_Exception);
    CORBA::Long               getAttributeValue(CORBA::Long i) 
                                     throw (SALOME::SALOME_Exception);
    SALOME_TYPES::ListOfString* getAttributesDescriptions() 
                                     throw (SALOME::SALOME_Exception);
    char*                     getAttributeDescription( CORBA::Long i) 
                                     throw (SALOME::SALOME_Exception);
    CORBA::Long               getNumberOfGroups()
                                     throw (SALOME::SALOME_Exception);
    char *                    getGroupName( CORBA::Long i)
                                     throw (SALOME::SALOME_Exception);
    SALOME_TYPES::ListOfString* getGroupsNames()
                                     throw (SALOME::SALOME_Exception);

};
}
#endif /* MED_FAMILY_I_HXX_ */
