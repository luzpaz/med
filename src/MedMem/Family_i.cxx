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
//  File   : Family_i.cxx
//  Author : EDF
//  Module : MED
//  $Header: /export/home/CVS/SALOME_ROOT/MED/src/MedMem/Family_i.cxx

using namespace std;
#include "Family_i.hxx"

#include "utilities.h"
#include "Utils_CorbaException.hxx"

#include "MEDMEM_Family.hxx"

//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
FAMILY_i::FAMILY_i(): _family((::FAMILY*)NULL),
				SUPPORT_i()
{
        BEGIN_OF("Default Constructor FAMILY_i");
        END_OF("Default Constructor FAMILY_i");
}
//=============================================================================
/*!
 * constructor par recopie
 */
//=============================================================================
FAMILY_i::FAMILY_i(const FAMILY_i & f): _family(f._family),
				        SUPPORT_i(f._family)
{
        BEGIN_OF("Default Constructor FAMILY_i");
        END_OF("Default Constructor FAMILY_i");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
FAMILY_i::~FAMILY_i()
{
}
//=============================================================================
/*!
 * Constructor
 */
//=============================================================================
FAMILY_i::FAMILY_i(const ::FAMILY * const f): _family(f),
				              SUPPORT_i(f)
{
        BEGIN_OF("Constructor FAMILY_i");
        END_OF("Constructor FAMILY_i");
}
//=============================================================================
/*!
 * CORBA: Accessor for Family's Identifier
 */
//=============================================================================

CORBA::Long FAMILY_i::getIdentifier()      
throw (SALOME::SALOME_Exception)
{
	if (_family==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Family",\
					     SALOME::INTERNAL_ERROR); 
	try
        {
                return _family->getIdentifier();
        }
        catch(...)
        {
                MESSAGE("Unable to acces Family Identifier");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Family C++ Object"
                                                ,SALOME::INTERNAL_ERROR);
        }

}
//=============================================================================
/*!
 * CORBA: Accessor for number of attributes
 */
//=============================================================================
CORBA::Long FAMILY_i::getNumberOfAttributes() 
throw (SALOME::SALOME_Exception)
{
        if (_family==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Family",\
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _family->getNumberOfAttributes();
        }
        catch(...)
        {
                MESSAGE("Unable to get number of attributes");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Family C++ Object"
                                              ,SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for attributes identifiers
 */
//=============================================================================
Engines::long_array*  FAMILY_i::getAttributesIdentifiers() 
throw (SALOME::SALOME_Exception)
{
        if (_family==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Family",\
                                             SALOME::INTERNAL_ERROR);
	Engines::long_array_var myseq= new Engines::long_array;
	try
	{
		int nbAttribute=_family->getNumberOfAttributes();
		myseq->length(nbAttribute);
		int * identifiers=_family->getAttributesIdentifiers();
		for (int i=0;i<nbAttribute;i++)
		{
			myseq[i]=identifiers[i];
		}
	}
	catch(...)
        {
                MESSAGE("Unable to acces Family Identifiers");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Family C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
	return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Accessor for attribute identifier I
 */
//=============================================================================
CORBA::Long FAMILY_i::getAttributeIdentifier(CORBA::Long i) 
throw (SALOME::SALOME_Exception)
{    
	if (_family==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Family",\
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _family->getAttributeIdentifier(i);
        }
        catch(...)
        {
                MESSAGE("Unable to acces Family Identifier");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Family C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
 
}
//=============================================================================
/*!
 * CORBA: Accessor for attributes values
 */
//=============================================================================
Engines::long_array*  FAMILY_i::getAttributesValues() 
throw (SALOME::SALOME_Exception)
{
        if (_family==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Family",\
                                             SALOME::INTERNAL_ERROR);
        Engines::long_array_var myseq= new Engines::long_array;
        try
        {
                int nbAttribute=_family->getNumberOfAttributes();
                myseq->length(nbAttribute);
                int * values=_family->getAttributesValues();
                for (int i=0;i<nbAttribute;i++)
                {
                        myseq[i]=values[i];
                }
        }
        catch(...)
        {
                MESSAGE("Unable to acces attributs values");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Family C++ Object"
                                                ,SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Accessor for attribute value I
 */
//=============================================================================
CORBA::Long  FAMILY_i::getAttributeValue(CORBA::Long i) 
throw (SALOME::SALOME_Exception)
{   
        if (_family==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Family",\
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _family->getAttributeValue(i);
        }
        catch(...)
        {
                MESSAGE("Unable to acces specified attribut value");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Family C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for attributes desriptions
 */
//=============================================================================
Engines::string_array * FAMILY_i::getAttributesDescriptions() 
throw (SALOME::SALOME_Exception)
{
        if (_family==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Family",\
                                             SALOME::INTERNAL_ERROR);
	
	Engines::string_array_var myseq = new Engines::string_array;
        try
        {
		int nbAttribute=_family->getNumberOfAttributes();
		myseq->length(nbAttribute);
		string * descattribute=_family->getAttributesDescriptions();
		for (int i=0;i<nbAttribute;i++)
		{
			myseq[i]=CORBA::string_dup(descattribute[i].c_str());
		}
	}
	catch(...)
        {
                MESSAGE("Unable to acces attributs descriptions");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Family C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
	return myseq._retn();

}
//=============================================================================
/*!
 * CORBA: Accessor for attribute description i
 */
//=============================================================================
char *  FAMILY_i::getAttributeDescription( CORBA::Long i) 
throw (SALOME::SALOME_Exception)
{   
        if (_family==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Family",\
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return CORBA::string_dup(_family->getAttributeDescription(i).c_str());
        }
        catch(...)
        {
                MESSAGE("Unable to acces specified attribut description");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Family C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
}

