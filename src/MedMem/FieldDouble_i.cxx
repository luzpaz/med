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
//  File   : Fielddouble_i.hxx
//  Author : EDF 
//  Module : MED
//  $Header: /export/home/CVS/SALOME_ROOT/MED/src/MedMem/Fielddouble_i.hxx

using namespace std;
#include "FieldDouble_i.hxx"
#include "utilities.h"
#include "convert.hxx"

//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
FIELDDOUBLE_i::FIELDDOUBLE_i(): FIELDOF_i<double>()
{
        BEGIN_OF("Default Constructor FIELDDOUBLE_i");
        END_OF("Default Constructor FIELDDOUBLE_i");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
FIELDDOUBLE_i::~FIELDDOUBLE_i()
{
        BEGIN_OF("Default Destructor FIELDDOUBLE_i");
        END_OF("Default Destructor FIELDDOUBLE_i");
}
//=============================================================================
/*!
 * Constructor par recopie
 */
//=============================================================================
FIELDDOUBLE_i::FIELDDOUBLE_i(FIELDDOUBLE_i & fd):
			       FIELDOF_i<double>(fd._fieldTptr)
{
        BEGIN_OF("Default Constructor FIELDDOUBLE_i");
        END_OF("Default Constructor FIELDDOUBLE_i");
}
//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
FIELDDOUBLE_i::FIELDDOUBLE_i(SALOME_MED::SUPPORT_ptr mySupportIOR,::FIELD<double> * const f): FIELDOF_i<double>(mySupportIOR,f)
{
        BEGIN_OF("Constructor FIELDDOUBLE_i");
        END_OF(" Constructor FIELDDOUBLE_i");
}

//=============================================================================
/*!
 * CORBA: Accessor for Field's values
*/
//=============================================================================

Engines::double_array* FIELDDOUBLE_i::getValue( SALOME_MED::medModeSwitch mode ) 
throw (SALOME::SALOME_Exception)
{
	if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        Engines::double_array_var myseq = new Engines::double_array;
        try
        {
                int nbval=_fieldTptr->getNumberOfComponents();

		// Ajout NB pour avoir la valeur correct de nbval
		SALOME_MED::medEntityMesh entity = _support->getEntity();
		if (_support->isOnAllElements())
		  {
		    if (entity == SALOME_MED::MED_NODE)
		      nbval = (_support->getMesh()->getNumberOfNodes())*nbval;
		    else
		      nbval = (_support->getMesh()->getNumberOfElements(entity,SALOME_MED::MED_ALL_ELEMENTS))*nbval;
		  }
		else
		  {
		    nbval = (_support->getNumberOfElements(SALOME_MED::MED_ALL_ELEMENTS))*nbval;
		  }

		medModeSwitch modemed=convertIdlModeToMedMode(mode);
                const double * values =_fieldTptr->getValue(modemed);

                myseq->length(nbval);
                for (int i=0; i<nbval; i++)
                {
                        myseq[i]=values[i];
                };
        }
        catch(...)
        {
                MESSAGE("Unable to acces Field ");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Field C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();
}
