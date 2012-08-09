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
// File      : MEDMEM_Group_i.cxx
// Project   : SALOME
// Author    : EDF 
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_Group_i.cxx
//=============================================================================
//
#include <vector>

#include "utilities.h"
#include "Utils_CorbaException.hxx"

#include "MEDMEM_Group_i.hxx"
#include "MEDMEM_Family_i.hxx"

#include "MEDMEM_Group.hxx"
#include "MEDMEM_Family.hxx"
using namespace MEDMEM;

//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
GROUP_i::GROUP_i():_group((::GROUP*)NULL)
{
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
GROUP_i::~GROUP_i()
{
}
//=============================================================================
/*!
 * Constructor
 */
//=============================================================================
GROUP_i::GROUP_i(const ::GROUP * const g):SUPPORT_i(g),_group(g)
{
}
//=============================================================================
/*!
 * Constructor par recopie
 */
//=============================================================================
GROUP_i::GROUP_i(const GROUP_i & g):SUPPORT_i(g._group),_group(g._group)
{
}
//=============================================================================
/*!
 * CORBA: Number of Families existing in the group
 */
//=============================================================================

CORBA::Long  GROUP_i::getNumberOfFamilies() 
throw (SALOME::SALOME_Exception)
{
        if (_group==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Group",\
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _group->getNumberOfFamilies();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get number of families of the group");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Returns references for families within the group
 */
//=============================================================================

SALOME_MED::Family_array* GROUP_i::getFamilies()         
throw (SALOME::SALOME_Exception)
{
        if (_group==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Group",\
                                             SALOME::INTERNAL_ERROR);
        SALOME_MED::Family_array_var myseq = new SALOME_MED::Family_array;
        try
        {
                int nbfam= _group->getNumberOfFamilies();
                myseq->length(nbfam);
                vector<FAMILY*> fam(nbfam);
                fam = _group->getFamilies();
                for (int i=0;i<nbfam;i++)
                {
                        FAMILY_i * f1=new FAMILY_i(fam[i]);
                        myseq[i] = f1->_this();
                }
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to access families");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Returns reference for family I within the group
 */
//=============================================================================

SALOME_MED::FAMILY_ptr GROUP_i::getFamily(CORBA::Long i) 
throw (SALOME::SALOME_Exception)
{
        if (_group==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Group",\
                                             SALOME::INTERNAL_ERROR);
        try
        {
                FAMILY * fam=_group->getFamily(i);
                FAMILY_i * f1=new FAMILY_i(fam);
                return f1->_this();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces to the specified family");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
