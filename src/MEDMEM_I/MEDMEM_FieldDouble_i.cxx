//=============================================================================
// File      : MEDMEM_Fielddouble_i.hxx
// Project   : SALOME
// Author    : EDF 
// Copyright : EDF 2002
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_Fielddouble_i.hxx
//=============================================================================

#include "MEDMEM_FieldDouble_i.hxx"
#include "utilities.h"
#include "MEDMEM_convert.hxx"
#include "SenderFactory.hxx"
#include "MultiCommException.hxx"
using namespace MEDMEM;

//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
FIELDDOUBLE_i::FIELDDOUBLE_i(): FIELD_i()
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
			       FIELD_i(fd)
{
        BEGIN_OF("Default Constructor FIELDDOUBLE_i");
        END_OF("Default Constructor FIELDDOUBLE_i");
}
//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
FIELDDOUBLE_i::FIELDDOUBLE_i(::FIELD<double> * const f, bool ownCppPtr):
  FIELD_i(f,ownCppPtr)
{
        BEGIN_OF("Constructor FIELDDOUBLE_i");
        END_OF(" Constructor FIELDDOUBLE_i");
}

//=============================================================================
/*!
 * CORBA: Accessor for Field's values
*/
//=============================================================================

SALOME_MED::double_array* FIELDDOUBLE_i::getValue( SALOME_MED::medModeSwitch mode ) 
throw (SALOME::SALOME_Exception)
{
	if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        SALOME_MED::double_array_var myseq = new SALOME_MED::double_array;
        try
        {
		medModeSwitch modemed=convertIdlModeToMedMode(mode);
// 		::FIELD<double> *ptrD=dynamic_cast< ::FIELD<double>* >(_fieldTptr);
// the alternative is not safe but the dynamic_cast fails using the python API
		MEDMEM::FIELD<double> *ptrD = static_cast<MEDMEM::FIELD<double>* >(_fieldTptr);
                const double * values =ptrD->getValue(modemed);
		int nbval=ptrD->getValueLength(modemed);
                myseq->length(nbval);
                for (int i=0; i<nbval; i++)
                {
                        myseq[i]=values[i];
                };
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces Field ");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Accessor for Field's values
*/
//=============================================================================

SALOME::SenderDouble_ptr FIELDDOUBLE_i::getSenderForValue( SALOME_MED::medModeSwitch mode ) 
throw (SALOME::SALOME_Exception)
{
	if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        SALOME::SenderDouble_ptr ret;
        try
        {
		medModeSwitch modemed=convertIdlModeToMedMode(mode);
                // ::FIELD<double> *ptrD=dynamic_cast< ::FIELD<double>* >(_fieldTptr);
		// the alternative is not safe but the dynamic_cast fails using the python API
                MEDMEM::FIELD<double> *ptrD=static_cast< MEDMEM::FIELD<double>* >(_fieldTptr);
                const double * values =ptrD->getValue(modemed);
		int nbval=ptrD->getValueLength(modemed);
		ret=SenderFactory::buildSender(*this,values,nbval);
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces Field ");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return ret;
}
