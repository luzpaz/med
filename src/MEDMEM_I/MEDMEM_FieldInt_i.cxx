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
#include "SenderFactory.hxx"
#include "MultiCommException.hxx"
using namespace MEDMEM;

//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
FIELDINT_i::FIELDINT_i(): FIELD_i()
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
FIELDINT_i::FIELDINT_i(::FIELD<int> * const f, bool ownCppPtr):
  FIELD_i(f,ownCppPtr)
{
        BEGIN_OF("Constructor FIELDINT_i");
        END_OF(" Constructor FIELDINT_i");
}
//=============================================================================
/*!
 * Constructor par recopie
 */
//=============================================================================
FIELDINT_i::FIELDINT_i(FIELDINT_i &fi):FIELD_i(fi)
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
	    medModeSwitch modemed=convertIdlModeToMedMode(mode);
// 	    ::FIELD<int> *ptrI=dynamic_cast< ::FIELD<int>* >(_fieldTptr);
//the alternative is not safe but the previous one fails using the python API
	    MEDMEM::FIELD<int> *ptrI = (MEDMEM::FIELD<int> *) _fieldTptr;

	    const int * values =ptrI->getValue(modemed);
	    int nbval=ptrI->getValueLength(modemed);
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
//=============================================================================
/*!
 * CORBA: Accessor for Field's values
*/
//=============================================================================

SALOME::Sender_ptr FIELDINT_i::getSenderForValue( SALOME_MED::medModeSwitch mode ) 
throw (SALOME::SALOME_Exception)
{
	if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        SALOME::Sender_ptr ret;
        try
        {
		medModeSwitch modemed=convertIdlModeToMedMode(mode);
// 		::FIELD<int> *ptrI=dynamic_cast< ::FIELD<int>* >(_fieldTptr);
//the alternative is not safe but the previous one fails using the python API
		MEDMEM::FIELD<int> *ptrI = (MEDMEM::FIELD<int> *) _fieldTptr;

                const int * values =ptrI->getValue(modemed);
		int nbval=ptrI->getValueLength(modemed);
		ret=SenderFactory::buildSender(*this,values,nbval);
        }
        catch(MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces Field");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return ret;
}
