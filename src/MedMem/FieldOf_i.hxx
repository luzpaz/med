//=============================================================================
// File      : FieldOf_i.hxx
// Project   : SALOME
// Author    : EDF
// Copyright : EDF 2002
// $Header: /export/home/CVS/SALOME_ROOT/MED/src/MedMem/FieldOf_i.hxx
//=============================================================================


#ifndef MED_FIELDOF_I_HXX_
#define MED_FIELDOF_I_HXX_

#include <map>
#include <string>
# include <sstream>

#include <SALOMEconfig.h>

# include "Utils_ORB_INIT.hxx"
# include "Utils_SINGLETON.hxx"

#include CORBA_SERVER_HEADER(MED)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

#include "Field_i.hxx"
#include "Support_i.hxx"

#include "convert.hxx"

#include "MEDMEM_Support.hxx"
#include "MEDMEM_Field.hxx"

template <class T> class FIELDOF_i: public FIELD_i
{
public :
        static map < int, ::FIELD<T> * > fieldMap ;
protected :
        static int fieldIndex ;

protected :
    // C++ object containing values

    ::FIELD<T> * const  _fieldTptr;
    const int	_corbaIndex;
    string  _FieldId;

  // CORBA : SUPPORT IOR
  const SALOME_MED::SUPPORT_ptr _support ;


public :
    // Constructors and associated internal methods
    FIELDOF_i();
    FIELDOF_i(SALOME_MED::SUPPORT_ptr support,::FIELD<T> * const field);
    FIELDOF_i(::FIELD<T> * const f);
    FIELDOF_i(FIELDOF_i & f);
    ~FIELDOF_i();

    char *   		    getName() 	     throw (SALOME::SALOME_Exception);
    char *   		    getDescription() throw (SALOME::SALOME_Exception);
    SALOME_MED::SUPPORT_ptr getSupport()     throw (SALOME::SALOME_Exception);
    CORBA::Long             getNumberOfComponents()   	   
					     throw (SALOME::SALOME_Exception);
    char *                  getComponentName(CORBA::Long i)   
					     throw (SALOME::SALOME_Exception);
    char *                  getComponentUnit(CORBA::Long i)   
					     throw (SALOME::SALOME_Exception);
    CORBA::Long             getIterationNumber()   	       
					     throw (SALOME::SALOME_Exception);
    CORBA::Long             getOrderNumber() throw (SALOME::SALOME_Exception);
    CORBA::Double           getTime()        throw (SALOME::SALOME_Exception);
    CORBA::Long             getCorbaIndex()  throw (SALOME::SALOME_Exception);

    Engines::string_array * getComponentsNames()  throw (SALOME::SALOME_Exception);
    Engines::string_array * getComponentsUnits()  throw (SALOME::SALOME_Exception);
    void addInStudy(SALOMEDS::Study_ptr myStudy, 
		    SALOME_MED::FIELD_ptr myIor)  throw (SALOME::SALOME_Exception);

    CORBA::Long addDriver (SALOME_MED::medDriverTypes driverType, 
			   const char* fileName, const char* fieldName)
                    	                  throw (SALOME::SALOME_Exception);
    void        rmDriver  (CORBA::Long i) throw (SALOME::SALOME_Exception);
    void        read      (CORBA::Long i) throw (SALOME::SALOME_Exception);
    void        write     (CORBA::Long i, const char* driverFieldName)
                    		          throw (SALOME::SALOME_Exception);

    // Cuisine Interne
    ::FIELD<T> * constructConstField() const;

 };
template <class T> map < int, ::FIELD<T> * > FIELDOF_i<T>::fieldMap ;
template <class T> int  FIELDOF_i<T>::fieldIndex = 0;
//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================

//template <class T> FIELDOF_i<T>::FIELDOF_i():_fieldTptr(FIELDOF_i<T>::constructConstField())
template <class T> FIELDOF_i<T>::FIELDOF_i():_fieldTptr(constructConstField()),
					     _support((SALOME_MED::SUPPORT_ptr) NULL)
{
        BEGIN_OF("Default Constructor Field_i");
        END_OF(" Default Constructor Field_i");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
template <class T> FIELDOF_i<T>::~FIELDOF_i()
{
}
//=============================================================================
/*!
 * Constructor
 */
//=============================================================================
template <class T> FIELDOF_i<T>::FIELDOF_i(SALOME_MED::SUPPORT_ptr support,::FIELD<T> * const field):
  _fieldTptr(field),
  _corbaIndex(FIELDOF_i<T>::fieldIndex++),
  _FieldId(""),
  _support(SALOME_MED::SUPPORT::_duplicate(support))
{
  BEGIN_OF("Constructor FIELDOF_i(SALOME_MED::SUPPORT_ptr support,::FIELD<T> * const field)");
  FIELDOF_i<T>::fieldMap[_corbaIndex]=_fieldTptr;

  MESSAGE("FIELDOF_i<T>::FIELDOF_i  Checking of pointeurs !!!");

  SCRUTE(_fieldTptr);
  SCRUTE(_support);
  SCRUTE(_support->getMesh());

  END_OF("Constructor FIELDOF_i(SALOME_MED::SUPPORT_ptr support,::FIELD<T> * const field)");
}


template <class T> FIELDOF_i<T>::FIELDOF_i( FIELD<T> * const f):
  _fieldTptr(f),
  _corbaIndex(FIELDOF_i<T>::fieldIndex++),
  _FieldId(""),
  _support((SALOME_MED::SUPPORT_ptr) NULL)
{
        BEGIN_OF("Constructor Field_i");
        FIELDOF_i<T>::fieldMap[_corbaIndex]=_fieldTptr;
        END_OF("Constructor Field_i");
}
//=============================================================================
/*!
 * Constructor par recopie
 */
//=============================================================================
template <class T> FIELDOF_i<T>::FIELDOF_i( FIELDOF_i & f):_fieldTptr(f._fieldTptr),
					   _corbaIndex(FIELDOF_i<T>::fieldIndex++),
    					   _FieldId("")
{
        BEGIN_OF("Constructor Field_i");
        FIELDOF_i<T>::fieldMap[_corbaIndex]=_fieldTptr;
        END_OF("Constructor Field_i");
}
//=============================================================================
/*!
 * Constructor d un pointeur constant 
 */
//=============================================================================
template <class T> ::FIELD<T> * FIELDOF_i<T>::constructConstField() const
{
        ::FIELD<T> * const ptrField =new ::FIELD<T>();
        return ptrField;
}
//=============================================================================
/*!
 * CORBA: Accessor for Fields's Name
 */
//=============================================================================
template <class T> char * FIELDOF_i<T>::getName()
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return CORBA::string_dup(_fieldTptr->getName().c_str());
        }
        catch(...)
        {
		MESSAGE("Exception en accedant au nom");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Field C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for Fields's Description
 */
//=============================================================================
template <class T> char * FIELDOF_i<T>::getDescription()
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return CORBA::string_dup(_fieldTptr->getDescription().c_str());
        }
        catch(...)
        {
		MESSAGE("Exception en accedant a la description");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Field C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for Fields's Support
 */
//=============================================================================

template <class T> SALOME_MED::SUPPORT_ptr FIELDOF_i<T>::getSupport()
  throw (SALOME::SALOME_Exception)
{
  BEGIN_OF("SALOME_MED::SUPPORT_ptr FIELDOF_i<T>::getSupport()");

  if (_fieldTptr==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Field",
				 SALOME::INTERNAL_ERROR);
  if (_support==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No Support in Field",
				 SALOME::INTERNAL_ERROR);

  SCRUTE(_fieldTptr);

  SCRUTE(_support);

  SALOME_MED::SUPPORT_ptr support = SALOME_MED::SUPPORT::_duplicate(_support);

  END_OF("SALOME_MED::SUPPORT_ptr FIELDOF_i<T>::getSupport()");

  return support ;
}
//=============================================================================
/*!
 * CORBA: Accessor for Fields's Number of components
 */
//=============================================================================
template <class T> CORBA::Long FIELDOF_i<T>::getNumberOfComponents()
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _fieldTptr->getNumberOfComponents();
        }
        catch(...)
        {
		MESSAGE("Exception en accedant au support");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Field C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for names of component I
 */
//=============================================================================
template <class T> char * FIELDOF_i<T>::getComponentName(CORBA::Long i)
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return CORBA::string_dup(_fieldTptr->getComponentName(i).c_str());
        }
        catch(...)
        {
		MESSAGE("Exception en accedant au nom d un component");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Field C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for unit of component I
 */
//=============================================================================
template <class T> char * FIELDOF_i<T>::getComponentUnit(CORBA::Long i)
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return CORBA::string_dup(_fieldTptr->getMEDComponentUnit(i).c_str());
        }
        catch(...)
        {
		MESSAGE("Exception en accedant au nom d un component");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Field C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for iteration number 
 */
//=============================================================================
template <class T> CORBA::Long FIELDOF_i<T>::getIterationNumber()
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _fieldTptr->getIterationNumber();
        }
        catch(...)
        {
		MESSAGE("Exception en accedant au champ");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Field C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for Corba Number 
 */
//=============================================================================
template <class T> CORBA::Long FIELDOF_i<T>::getCorbaIndex()
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
	return _corbaIndex;
	
}
//=============================================================================
/*!
 * CORBA: Accessor for iteration number 
 */
//=============================================================================
template <class T> CORBA::Long FIELDOF_i<T>::getOrderNumber()
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _fieldTptr->getOrderNumber();
        }
        catch(...)
        {
		MESSAGE("Exception en accedant au champ");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Field C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor  
 */
//=============================================================================
template <class T> CORBA::Double FIELDOF_i<T>::getTime()
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _fieldTptr->getTime();
        }
        catch(...)
        {
		MESSAGE("Exception en accedant au champ");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Field C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for Fields's Components names
 */
//=============================================================================
template <class T> Engines::string_array * FIELDOF_i<T>::getComponentsNames()
throw (SALOME::SALOME_Exception)    
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
	Engines::string_array_var myseq = new Engines::string_array;
        try
        {
		int nbcom = _fieldTptr->getNumberOfComponents();
		myseq->length(nbcom);
		const string * namecom=_fieldTptr->getComponentsNames();
		for (int i=0;i<nbcom;i++)
                {
                        myseq[i]=CORBA::string_dup(namecom[i].c_str());
                }
        }
        catch(...)
        {
		MESSAGE("Exception en accedant au champ");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Field C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
	return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Accessor for Fields's Components units
 */
//=============================================================================
template <class T> Engines::string_array * FIELDOF_i<T>::getComponentsUnits()
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
	Engines::string_array_var myseq = new Engines::string_array;
        try
        {
		int nbcom = _fieldTptr->getNumberOfComponents();
		myseq->length(nbcom);
		const string * unitcom=_fieldTptr->getMEDComponentsUnits();
		for (int i=0;i<nbcom;i++)
                {
                        myseq[i]=CORBA::string_dup(unitcom[i].c_str());
                }
        }
        catch(...)
        {
		MESSAGE("Exception en accedant au champ");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Field C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
	return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Add in Study
 */
//=============================================================================
template <class T> void FIELDOF_i<T>::addInStudy(SALOMEDS::Study_ptr myStudy, 
						 SALOME_MED::FIELD_ptr myIor )
throw (SALOME::SALOME_Exception)
{
        BEGIN_OF(" FIELDOF_i::addInStudy");
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        if ( _FieldId != "" )
        {
                MESSAGE("Field already in Study");
                    THROW_SALOME_CORBA_EXCEPTION("Field already in Study", \
                                 SALOME::BAD_PARAM);
        };


        SALOMEDS::StudyBuilder_var myBuilder = myStudy->NewBuilder();
        SALOMEDS::GenericAttribute_var anAttr;
        SALOMEDS::AttributeName_var    aName;
        SALOMEDS::AttributeIOR_var     aIOR;

        // Create SComponent labelled 'Med'
        SALOMEDS::SComponent_var medfather = myStudy->FindComponent("Med");
        if ( CORBA::is_nil(medfather) )
	  THROW_SALOME_CORBA_EXCEPTION("SComponent labelled 'Med' not Found",SALOME::INTERNAL_ERROR);

 	// Create SObject labelled 'MEDFIELD' if it doesn't already exit
	SALOMEDS::SObject_var medfieldfather = myStudy->FindObject("MEDFIELD");
  	if ( CORBA::is_nil(medfieldfather) ) 
	{
	  MESSAGE("Add Object 'MEDFIELD'");
	  medfieldfather = myBuilder->NewObject(medfather);
	  anAttr = myBuilder->FindOrCreateAttribute(medfieldfather, "AttributeName");
	  aName = SALOMEDS::AttributeName::_narrow(anAttr);
	  aName->SetValue("MEDFIELD");
	  
  	} ;

 	// Create SObject labelled 'FIELDNAME' if it doesn't already exit
	SALOMEDS::SObject_var medfieldnamefather = myStudy->FindObject((_fieldTptr->getName()).c_str());
  	if ( CORBA::is_nil(medfieldnamefather) ) 
	{
	  MESSAGE("Add Object "<<_fieldTptr->getName());
	  medfieldnamefather = myBuilder->NewObject(medfieldfather);
	  anAttr = myBuilder->FindOrCreateAttribute(medfieldnamefather, "AttributeName");
	  aName = SALOMEDS::AttributeName::_narrow(anAttr);
	  aName->SetValue((_fieldTptr->getName()).c_str());

  	} ;

        // Create object labelled according to Field's Name

        MESSAGE("Add a Field Object under "<<_fieldTptr->getName());
        myBuilder->NewCommand();
        SALOMEDS::SObject_var newObj = myBuilder->NewObject(medfieldnamefather);

        ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
        ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting()) ;
        CORBA::ORB_var &orb = init(0,0);

	ostringstream iterationName ;
	iterationName<<"("<<_fieldTptr->getIterationNumber()<<","<<_fieldTptr->getOrderNumber()<<")";
        anAttr = myBuilder->FindOrCreateAttribute(newObj, "AttributeName");
        aName = SALOMEDS::AttributeName::_narrow(anAttr);
        aName->SetValue(iterationName.str().c_str());

	string iorStr = orb->object_to_string(myIor);
        anAttr = myBuilder->FindOrCreateAttribute(newObj, "AttributeIOR");
        aIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
        aIOR->SetValue(iorStr.c_str());

	SALOMEDS::SObject_var supportObject = myStudy->FindObject(_support->getName());
  	if ( CORBA::is_nil(supportObject) ) {
	  MESSAGE("FIELDOF_i::addInStudy : SUPPORT not found") ;
	} else {
	  SALOMEDS::SObject_var newObjSupport = myBuilder->NewObject(newObj);
	  myBuilder->Addreference(newObjSupport,supportObject);
	}
        _FieldId = newObj->GetID();
	MESSAGE("FIELDOF_i::addInStudy _FieldId="<< _FieldId);

        END_OF(" FIELDOF_i::addInStudy");

}
//=============================================================================
/*!
 * CORBA: write
 */
//=============================================================================
template <class T> void FIELDOF_i<T>::write (CORBA::Long i, const char* driverFieldName)
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
	try
	{
		_fieldTptr->write(i,driverFieldName);
        }
        catch(...)
        {
		MESSAGE("Exception en accedant au champ");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Field C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: read
 */
//=============================================================================
template <class T> void FIELDOF_i<T>::read (CORBA::Long i)
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
	try
	{
		_fieldTptr->read(i);
        }
        catch(...)
        {
		MESSAGE("Exception en accedant au champ");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Field C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: rmDriver
 */
//=============================================================================
template <class T> void FIELDOF_i<T>::rmDriver (CORBA::Long i)
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
	try
	{
		_fieldTptr->rmDriver(i);
        }
        catch(...)
        {
		MESSAGE("Exception en accedant au champ");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Field C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: addDriver
 */
//=============================================================================
template <class T> CORBA::Long FIELDOF_i<T>::addDriver (SALOME_MED::medDriverTypes driverType, 
			const char* fileName, const char* fieldName) throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
	try
	{
		int drivernum=_fieldTptr->addDriver(
					convertIdlDriverToMedDriver(driverType),
				        fileName,
				        fieldName);
		return drivernum;
        }
        catch(...)
        {
		MESSAGE("Exception en accedant au champ");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Field C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
}

#endif /* MED_FIELDOF_I_HXX_ */
