//=============================================================================
// File      : MEDMEM_Fieldint_i.hxx
// Project   : SALOME
// Author    : EDF 
// Copyright : EDF 2002
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_Fieldint_i.hxx
//=============================================================================

#include "utilities.h"
#include "MEDMEM_FieldInt_i.hxx"
#include "MEDMEM_convert.hxx"
using namespace MEDMEM;

//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
//CCRTFIELDINT_i::FIELDINT_i(): FIELDOF_i<int>()
FIELDINT_i::FIELDINT_i(): FIELDOF_i<med_int>()
{
        BEGIN_OF("Default Constructor FIELDINT_i");
        END_OF("Default Constructor FIELDINT_i");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
FIELDINT_i::~FIELDINT_i()
{
        BEGIN_OF("Default Destructor FIELDINT_i");
        END_OF("Default Destructor FIELDINT_i");
}
//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
//CCRTFIELDINT_i::FIELDINT_i(SALOME_MED::SUPPORT_ptr mySupportIOR, ::FIELD<int> * const f): FIELDOF_i<int>(mySupportIOR,f)
FIELDINT_i::FIELDINT_i(SALOME_MED::SUPPORT_ptr mySupportIOR, ::FIELD<med_int> * const f): FIELDOF_i<med_int>(mySupportIOR,f)
{
        BEGIN_OF("Constructor FIELDINT_i");
        END_OF(" Constructor FIELDINT_i");
}
//=============================================================================
/*!
 * Constructor par recopie
 */
//=============================================================================
FIELDINT_i::FIELDINT_i(FIELDINT_i &fi):
//CCRT					FIELDOF_i<int>(fi._fieldTptr)
					FIELDOF_i<med_int>(fi._fieldTptr)
{
        BEGIN_OF("Constructor FIELDINT_i");
        END_OF(" Constructor FIELDINT_i");
}

//=============================================================================
/*!
 * CORBA: Accessor for Field's values
*/
//=============================================================================

SALOME_MED::long_array* FIELDINT_i::getValue( SALOME_MED::medModeSwitch mode ) 
throw (SALOME::SALOME_Exception)
{
	if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        SALOME_MED::long_array_var myseq = new SALOME_MED::long_array;
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
//CCRT                const int * values =_fieldTptr->getValue(modemed);
                const med_int * values =_fieldTptr->getValue(modemed);

                myseq->length(nbval);
                for (int i=0; i<nbval; i++)
                {
                        myseq[i]=values[i];
                };
        }
        catch(MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces Field");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();
}
