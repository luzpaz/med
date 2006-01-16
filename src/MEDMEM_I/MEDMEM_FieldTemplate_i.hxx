#ifndef __MED_FIELDTEMPLATE_I_HXX__
#define __MED_FIELDTEMPLATE_I_HXX__

#include <SALOMEconfig.h> 
#include  CORBA_SERVER_HEADER(MED)
#include "MEDMEM_Field_i.hxx"
#include "MEDMEM_Field.hxx"
#include "SenderFactory.hxx"
#include "MultiCommException.hxx"
#include "MEDMEM_ArrayConvert.hxx"
#include "MEDMEM_TraitsForFields.hxx"

namespace MEDMEM
{
  template < class T, class INTERLACING_TAG >
  class FIELDTEMPLATE_I : public FIELDI_TRAITS<T,INTERLACING_TAG>::InterfaceForServant,
                          public FIELD_i,
                          public SALOMEMultiComm
  {
  public:
    FIELDTEMPLATE_I();
    ~FIELDTEMPLATE_I();
    FIELDTEMPLATE_I(MEDMEM::FIELD<T, INTERLACING_TAG> *f, bool ownCppPtr=false);
    FIELDTEMPLATE_I(FIELDTEMPLATE_I & f);
    typename FIELDI_TRAITS<T,INTERLACING_TAG>::SeqType* getValue (SALOME_MED::medModeSwitch mode ) 
      throw (SALOME::SALOME_Exception);
    typename FIELDI_TRAITS<T,INTERLACING_TAG>::SenderPtrType getSenderForValue(SALOME_MED::medModeSwitch mode)
      throw (SALOME::SALOME_Exception);
  };

  //=============================================================================
  /*!
   * Default constructor
   */
  //=============================================================================
  template < class T, class INTERLACING_TAG >
  FIELDTEMPLATE_I<T,INTERLACING_TAG>::FIELDTEMPLATE_I(): FIELD_i()
  {
    BEGIN_OF("Default Constructor FIELDTEMPLATE_I");
    END_OF("Default Constructor FIELDTEMPLATE_I");
  }
  //=============================================================================
  /*!
   * Destructor
   */
  //=============================================================================
  template < class T, class INTERLACING_TAG >
  FIELDTEMPLATE_I<T,INTERLACING_TAG>::~FIELDTEMPLATE_I()
  {
    BEGIN_OF("Default Destructor FIELDTEMPLATE_I");
    END_OF("Default Destructor FIELDTEMPLATE_I");
  }
  //=============================================================================
  /*!
   * Constructor par recopie
   */
  //=============================================================================
  template < class T, class INTERLACING_TAG >
  FIELDTEMPLATE_I<T,INTERLACING_TAG>::FIELDTEMPLATE_I(FIELDTEMPLATE_I<T, INTERLACING_TAG>& fd):
    FIELD_i(fd)
  {
    BEGIN_OF("Default Constructor FIELDTEMPLATE_I");
    END_OF("Default Constructor FIELDTEMPLATE_I");
  }
  //=============================================================================
  /*!
   * Default constructor
   */
  //=============================================================================
  template < class T, class INTERLACING_TAG >
  FIELDTEMPLATE_I<T,INTERLACING_TAG>::FIELDTEMPLATE_I(MEDMEM::FIELD<T, INTERLACING_TAG> *f, bool ownCppPtr):FIELD_i(f,ownCppPtr)
  {
    BEGIN_OF("Constructor FIELDTEMPLATE_I");
    END_OF(" Constructor FIELDTEMPLATE_I");
  }

  //=============================================================================
  /*!
   * CORBA: Accessor for Field's values
   */
  //=============================================================================
  template < class T, class INTERLACING_TAG >
  typename FIELDI_TRAITS<T,INTERLACING_TAG>::SeqType *FIELDTEMPLATE_I<T,INTERLACING_TAG>::getValue( SALOME_MED::medModeSwitch mode ) 
    throw (SALOME::SALOME_Exception)
  {
    if (_fieldTptr==NULL)
      THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
				   SALOME::INTERNAL_ERROR);
    typename FIELDI_TRAITS<T,INTERLACING_TAG>::SeqVarType myseq = new typename FIELDI_TRAITS<T,INTERLACING_TAG>::SeqType;
    try
      {
	medModeSwitch modemed=convertIdlModeToMedMode(mode);
	// 		::FIELD<T> *ptrD=dynamic_cast< ::FIELD<T>* >(_fieldTptr);
	// the alternative is not safe but the dynamic_cast fails using the python API
	MEDMEM::FIELD<T, INTERLACING_TAG> *ptrD = static_cast<MEDMEM::FIELD<T, INTERLACING_TAG>* >(_fieldTptr);
	int nbval=ptrD->getValueLength();
	if(ptrD->getInterlacingType()==modemed)
	  {
	    const T* values =ptrD->getValue();
	    myseq->length(nbval);
	    for (int i=0; i<nbval; i++)
	      {
		myseq[i]=values[i];
	      }
	  }
	else
	  {
	    T *values;
	    if  ( ptrD->getGaussPresence() ) 
	      {
		typename MEDMEM_ArrayInterface<T,INTERLACING_TAG,Gauss>::Array * myArray = ptrD->getArrayGauss();
		int size=myArray->getArraySize();
		values=new T[size];
		delete ArrayConvert(*myArray,values);
	      }
	    else 
	      {
		typename MEDMEM_ArrayInterface<T,INTERLACING_TAG,NoGauss>::Array * myArray = ptrD->getArrayNoGauss();
		int size=myArray->getArraySize();
		values=new T[size];
		delete ArrayConvert(*myArray,values);
	      }
	    for (int i=0; i<nbval; i++)
	      {
		myseq[i]=values[i];
	      }
	  }
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
  template < class T, class INTERLACING_TAG >
  typename FIELDI_TRAITS<T,INTERLACING_TAG>::SenderPtrType FIELDTEMPLATE_I<T,INTERLACING_TAG>::getSenderForValue( SALOME_MED::medModeSwitch mode ) 
    throw (SALOME::SALOME_Exception)
  {
    if (_fieldTptr==NULL)
      THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
				   SALOME::INTERNAL_ERROR);
    typename FIELDI_TRAITS<T,INTERLACING_TAG>::SenderPtrType ret;
    try
      {
	medModeSwitch modemed=convertIdlModeToMedMode(mode);
	// ::FIELD<T> *ptrD=dynamic_cast< ::FIELD<T>* >(_fieldTptr);
	// the alternative is not safe but the dynamic_cast fails using the python API
	MEDMEM::FIELD<T, INTERLACING_TAG> *ptrD=static_cast< MEDMEM::FIELD<T, INTERLACING_TAG>* >(_fieldTptr);
	int nbval=ptrD->getValueLength();
	if(ptrD->getInterlacingType()==modemed)
	  {
	    const T* values =ptrD->getValue();
	    ret=SenderFactory::buildSender(*this,values,nbval);
	  }
	else
	  {
	    T *values;
	    if  ( ptrD->getGaussPresence() ) 
	      {
		typename MEDMEM_ArrayInterface<T,INTERLACING_TAG,Gauss>::Array * myArray = ptrD->getArrayGauss();
		int size=myArray->getArraySize();
		values=new T[size];
		delete ArrayConvert(*myArray,values);
		
	      }
	    else 
	      {
		typename MEDMEM_ArrayInterface<T,INTERLACING_TAG,NoGauss>::Array * myArray = ptrD->getArrayNoGauss();
		int size=myArray->getArraySize();
		values=new T[size];
		delete ArrayConvert(*myArray,values);
	      }
	    ret=SenderFactory::buildSender(*this,values,nbval);
	  }
      }
    catch (MEDEXCEPTION &ex)
      {
	MESSAGE("Unable to acces Field ");
	THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
      }
    return ret;
  }

}

#endif
