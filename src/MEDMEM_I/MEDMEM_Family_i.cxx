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
// File      :  MEDMEM_Family_i.cxx
// Project   :  SALOME
// Author    :  EDF
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_Family_i.cxx
//=============================================================================
//
#include "utilities.h"
#include "Utils_CorbaException.hxx"
#include "MEDMEM_Family_i.hxx"
#include "MEDMEM_Family.hxx"
using namespace MEDMEM;

//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
FAMILY_i::FAMILY_i(): _family((::FAMILY*)NULL)
{
}
//=============================================================================
/*!
 * Copy Constructor 
 */
//=============================================================================
FAMILY_i::FAMILY_i(const FAMILY_i & f): SUPPORT_i(f._family),_family(f._family)
                                        
{
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
FAMILY_i::FAMILY_i(const ::FAMILY * const f): SUPPORT_i(f),_family(f)
{
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
        catch(MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces Family Identifier");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::INTERNAL_ERROR);
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
        catch(MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get number of attributes");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for attributes identifiers
 */
//=============================================================================
SALOME_TYPES::ListOfLong*  FAMILY_i::getAttributesIdentifiers() 
throw (SALOME::SALOME_Exception)
{
        if (_family==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Family",\
                                             SALOME::INTERNAL_ERROR);
        SALOME_TYPES::ListOfLong_var myseq= new SALOME_TYPES::ListOfLong;
        try
        {
                int nbAttribute=_family->getNumberOfAttributes();
                myseq->length(nbAttribute);
                const int * identifiers=_family->getAttributesIdentifiers();
                for (int i=0;i<nbAttribute;i++)
                {
                        myseq[i]=identifiers[i];
                }
        }
        catch(MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces Family Identifiers");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::INTERNAL_ERROR);
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
        catch(MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces Family Identifier");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::INTERNAL_ERROR);
        }
 
}
//=============================================================================
/*!
 * CORBA: Accessor for attributes values
 */
//=============================================================================
SALOME_TYPES::ListOfLong*  FAMILY_i::getAttributesValues() 
throw (SALOME::SALOME_Exception)
{
        if (_family==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Family",\
                                             SALOME::INTERNAL_ERROR);
        SALOME_TYPES::ListOfLong_var myseq= new SALOME_TYPES::ListOfLong;
        try
        {
                int nbAttribute=_family->getNumberOfAttributes();
                myseq->length(nbAttribute);
                const int * values=_family->getAttributesValues();
                for (int i=0;i<nbAttribute;i++)
                {
                        myseq[i]=values[i];
                }
        }
        catch(MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces attributs values");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::INTERNAL_ERROR);
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
        catch(MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces specified attribut value");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for attributes desriptions
 */
//=============================================================================
SALOME_TYPES::ListOfString * FAMILY_i::getAttributesDescriptions() 
throw (SALOME::SALOME_Exception)
{
        if (_family==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Family",\
                                             SALOME::INTERNAL_ERROR);
        
        SALOME_TYPES::ListOfString_var myseq = new SALOME_TYPES::ListOfString;
        try
        {
                int nbAttribute=_family->getNumberOfAttributes();
                myseq->length(nbAttribute);
                const string * descattribute=_family->getAttributesDescriptions();
                for (int i=0;i<nbAttribute;i++)
                {
                        myseq[i]=CORBA::string_dup(descattribute[i].c_str());
                }
        }
        catch(MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces attributs descriptions");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::INTERNAL_ERROR);
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
        catch(MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces specified attribut description");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::INTERNAL_ERROR);
        }
}

//=============================================================================
/*!
 * CORBA: Accessor for number of groups the family belongs to
 */
//=============================================================================
CORBA::Long  FAMILY_i::getNumberOfGroups()
throw (SALOME::SALOME_Exception)
{
        if (_family==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Family",\
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _family->getNumberOfGroups();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces specified attribut value");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for groups names
 */
//=============================================================================
SALOME_TYPES::ListOfString * FAMILY_i::getGroupsNames()
throw (SALOME::SALOME_Exception)
{
        if (_family==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Family",\
                                             SALOME::INTERNAL_ERROR);

        SALOME_TYPES::ListOfString_var myseq = new SALOME_TYPES::ListOfString;
        try
        {
                int nbGroups=_family->getNumberOfGroups();
                myseq->length(nbGroups);
                const string * descGroups=_family->getGroupsNames();
                for (int i=0;i<nbGroups;i++)
                {
                        myseq[i]=CORBA::string_dup(descGroups[i].c_str());
                }
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces attributs descriptions");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();

}
//=============================================================================
/*!
 * CORBA: Accessor for attribute description i
 */
//=============================================================================
char *  FAMILY_i::getGroupName( CORBA::Long i)
throw (SALOME::SALOME_Exception)
{
        if (_family==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Family",\
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return CORBA::string_dup(_family->getGroupName(i).c_str());
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces specified attribut description");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::INTERNAL_ERROR);
        }
}

