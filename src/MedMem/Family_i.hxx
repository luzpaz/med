//  MED MedMem : MED idl descriptions implementation based on the classes of MEDMEM
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : Family_i.hxx
//  Author : EDF
//  Module : MED
//  $Header: /export/home/CVS/SALOME_ROOT/MED/src/MedMem/Family_i.hxx

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
