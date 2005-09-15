//=============================================================================
// File      : MEDMEM_Field_i.cxx
// Created   : mer fév 20 15:47:57 CET 2002
// Author    : EDF
// Project   : SALOME
// Copyright : EDF 2002
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_Field_i.cxx
//=============================================================================
#include "MEDMEM_Field_i.hxx"

using namespace MEDMEM;
using namespace MED_EN;

map < int, ::FIELD_ * > FIELD_i::fieldMap ;
int  FIELD_i::fieldIndex = 0;
//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================

//FIELD_i::FIELD_i():_fieldTptr(FIELD_i::constructConstField())
FIELD_i::FIELD_i():_fieldTptr(constructConstField()),_corbaIndex(FIELD_i::fieldIndex++)
{
        BEGIN_OF("Default Constructor Field_i");
        END_OF(" Default Constructor Field_i");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
FIELD_i::~FIELD_i()
{
  if (_ownCppPtr) delete _fieldTptr;
}
//=============================================================================
/*!
 * Constructor
 */
//=============================================================================
FIELD_i::FIELD_i(::FIELD_ * const field, bool ownCppPtr):
  _fieldTptr(field),
  _corbaIndex(FIELD_i::fieldIndex++),
  _FieldId(""),
  _ownCppPtr(ownCppPtr)
{
  BEGIN_OF("Constructor FIELD_i(SALOME_MED::SUPPORT_ptr support,::FIELD<T> * const field)");
  FIELD_i::fieldMap[_corbaIndex]=_fieldTptr;

  MESSAGE("FIELD_i::FIELD_i  Checking of pointeurs !!!");

  SCRUTE(_fieldTptr);

  END_OF("Constructor FIELD_i(SALOME_MED::SUPPORT_ptr support,::FIELD<T> * const field)");
}

//=============================================================================
/*!
 * Constructor par recopie
 */
//=============================================================================
FIELD_i::FIELD_i( FIELD_i & f):_fieldTptr(f._fieldTptr),
			       _corbaIndex(FIELD_i::fieldIndex++),
			       _FieldId(""), _ownCppPtr(false)
{
        BEGIN_OF("Constructor Field_i");
        FIELD_i::fieldMap[_corbaIndex]=_fieldTptr;
        END_OF("Constructor Field_i");
}
//=============================================================================
/*!
 * Constructor d un pointeur constant 
 */
//=============================================================================
 ::FIELD_ * FIELD_i::constructConstField() const
{
        ::FIELD_ * const ptrField =new ::FIELD_();
        return ptrField;
}
//=============================================================================
/*!
 * CORBA: Accessor for Fields's Name
 */
//=============================================================================
char * FIELD_i::getName()
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return CORBA::string_dup(_fieldTptr->getName().c_str());
        }
        catch (MEDEXCEPTION &ex)
        {
		MESSAGE("Exception en accedant au nom");
	        THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for Fields's Description
 */
//=============================================================================
char * FIELD_i::getDescription()
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return CORBA::string_dup(_fieldTptr->getDescription().c_str());
        }
        catch (MEDEXCEPTION &ex)
        {
		MESSAGE("Exception en accedant a la description");
	        THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for Fields's Support
 */
//=============================================================================

SALOME_MED::SUPPORT_ptr FIELD_i::getSupport()
  throw (SALOME::SALOME_Exception)
{
  BEGIN_OF("SALOME_MED::SUPPORT_ptr FIELD_i::getSupport()");

  if (_fieldTptr==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Field",
				 SALOME::INTERNAL_ERROR);
  
  SUPPORT_i* servant = new SUPPORT_i(_fieldTptr->getSupport());

  SALOME_MED::SUPPORT_ptr support=servant->_this();

  SCRUTE(_fieldTptr);

  END_OF("SALOME_MED::SUPPORT_ptr FIELD_i::getSupport()");

  return support ;
}
//=============================================================================
/*!
 * CORBA: Accessor for Fields's Number of components
 */
//=============================================================================
CORBA::Long FIELD_i::getNumberOfComponents()
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _fieldTptr->getNumberOfComponents();
        }
        catch (MEDEXCEPTION &ex)
        {
		MESSAGE("Exception en accedant au support");
	        THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for names of component I
 */
//=============================================================================
char * FIELD_i::getComponentName(CORBA::Long i)
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return CORBA::string_dup(_fieldTptr->getComponentName(i).c_str());
        }
        catch (MEDEXCEPTION &ex)
        {
		MESSAGE("Exception en accedant au nom d un component");
	        THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for unit of component I
 */
//=============================================================================
char * FIELD_i::getComponentUnit(CORBA::Long i)
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return CORBA::string_dup(_fieldTptr->getMEDComponentUnit(i).c_str());
        }
        catch (MEDEXCEPTION &ex)
        {
		MESSAGE("Exception en accedant au nom d un component");
	        THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for description of component I
 */
//=============================================================================
char * FIELD_i::getComponentDescription(CORBA::Long i)
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return CORBA::string_dup(_fieldTptr->getComponentDescription(i).c_str());
        }
        catch (MEDEXCEPTION &ex)
        {
		MESSAGE("Exception en accedant a la description d un component");
	        THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for iteration number 
 */
//=============================================================================
CORBA::Long FIELD_i::getIterationNumber()
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _fieldTptr->getIterationNumber();
        }
        catch (MEDEXCEPTION &ex)
        {
		MESSAGE("Exception en accedant au champ");
	        THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for Corba Number 
 */
//=============================================================================
CORBA::Long FIELD_i::getCorbaIndex()
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
CORBA::Long FIELD_i::getOrderNumber()
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _fieldTptr->getOrderNumber();
        }
        catch (MEDEXCEPTION &ex)
        {
		MESSAGE("Exception en accedant au champ");
	        THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor  
 */
//=============================================================================
CORBA::Double FIELD_i::getTime()
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _fieldTptr->getTime();
        }
        catch (MEDEXCEPTION &ex)
        {
		MESSAGE("Exception en accedant au champ");
	        THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for Fields's Components names
 */
//=============================================================================
SALOME_MED::string_array * FIELD_i::getComponentsNames()
throw (SALOME::SALOME_Exception)    
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
	SALOME_MED::string_array_var myseq = new SALOME_MED::string_array;
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
        catch (MEDEXCEPTION &ex)
        {
		MESSAGE("Exception en accedant au champ");
	        THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
	return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Accessor for Fields's Components units
 */
//=============================================================================
SALOME_MED::string_array * FIELD_i::getComponentsUnits()
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
	SALOME_MED::string_array_var myseq = new SALOME_MED::string_array;
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
        catch (MEDEXCEPTION &ex)
        {
		MESSAGE("Exception en accedant au champ");
	        THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
	return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Accessor for Fields's Components descriptions
 */
//=============================================================================
SALOME_MED::string_array * FIELD_i::getComponentsDescriptions()
throw (SALOME::SALOME_Exception)    
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
	SALOME_MED::string_array_var myseq = new SALOME_MED::string_array;
        try
        {
		int nbcom = _fieldTptr->getNumberOfComponents();
		myseq->length(nbcom);
		const string * namecom=_fieldTptr->getComponentsDescriptions();
		for (int i=0;i<nbcom;i++)
                {
                        myseq[i]=CORBA::string_dup(namecom[i].c_str());
                }
        }
        catch (MEDEXCEPTION &ex)
        {
		MESSAGE("Exception en accedant au champ");
	        THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
	return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Add in Study
 */
//=============================================================================
void FIELD_i::addInStudy(SALOMEDS::Study_ptr myStudy, 
						 SALOME_MED::FIELD_ptr myIor )
		    throw (SALOME::SALOME_Exception, SALOMEDS::StudyBuilder::LockProtection)
{
        BEGIN_OF(" FIELD_i::addInStudy");
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
        SALOMEDS::SComponent_var medfather = myStudy->FindComponent("MED");
        if ( CORBA::is_nil(medfather) )
	  THROW_SALOME_CORBA_EXCEPTION("SComponent labelled 'MED' not Found",SALOME::INTERNAL_ERROR);

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

	string fieldName = _fieldTptr->getName();

 	// Create SObject labelled 'FIELDNAME' if it doesn't already exit
	SALOMEDS::SObject_var medfieldnamefather = myStudy->FindObject(fieldName.c_str());
  	if ( CORBA::is_nil(medfieldnamefather) ) 
	{
	  MESSAGE("Add Object "<<fieldName);
	  medfieldnamefather = myBuilder->NewObject(medfieldfather);
	  anAttr = myBuilder->FindOrCreateAttribute(medfieldnamefather, "AttributeName");
	  aName = SALOMEDS::AttributeName::_narrow(anAttr);
	  aName->SetValue(fieldName.c_str());

  	} ;

        // Create object labelled according to Field's Name

        MESSAGE("Add a Field Object under "<<fieldName);
        myBuilder->NewCommand();
        SALOMEDS::SObject_var newObj = myBuilder->NewObject(medfieldnamefather);

        ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
        ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting()) ;
        CORBA::ORB_var &orb = init(0,0);

	int iterationNumber = _fieldTptr->getIterationNumber();
	SCRUTE(iterationNumber);

	int orderNumber = _fieldTptr->getOrderNumber();
	SCRUTE(orderNumber);

	ostringstream iterationName ;
	iterationName<<"(" << iterationNumber << "," << orderNumber << ")";
	//	string supportName = _support->getName();
	string supportName = (_fieldTptr->getSupport())->getName();
	//	string meshName = (_support->getMesh())->getName();
	string meshName = ((_fieldTptr->getSupport())->getMesh())->getName();
	string meshNameStudy = meshName;

	char * fieldEntryName;
	int lenName = strlen(iterationName.str().c_str()) + 4 +
	  strlen(supportName.c_str()) + 4 + strlen(meshName.c_str()) + 1;

	fieldEntryName = new char[lenName];
	fieldEntryName = strcpy(fieldEntryName,iterationName.str().c_str());
	fieldEntryName = strcat(fieldEntryName,"_ON_");
	fieldEntryName = strcat(fieldEntryName,supportName.c_str());
	fieldEntryName = strcat(fieldEntryName,"_OF_");

	for (string::size_type pos=0; pos<meshNameStudy.size();++pos)
	  {
	    if (isspace(meshNameStudy[pos])) meshNameStudy[pos] = '_';
	  }

	SCRUTE(meshNameStudy);

	fieldEntryName = strcat(fieldEntryName,meshNameStudy.c_str());

	SCRUTE(fieldEntryName);

        anAttr = myBuilder->FindOrCreateAttribute(newObj, "AttributeName");
        aName = SALOMEDS::AttributeName::_narrow(anAttr);
//         aName->SetValue(iterationName.str().c_str());
        aName->SetValue(fieldEntryName);

	string iorStr = orb->object_to_string(myIor);
        anAttr = myBuilder->FindOrCreateAttribute(newObj, "AttributeIOR");
        aIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
        aIOR->SetValue(iorStr.c_str());
        myBuilder->CommitCommand();
        _FieldId = newObj->GetID();

	MESSAGE("Computing path to Support");

	char * supportEntryPath;
	lenName = 28 + 15 + strlen(meshName.c_str()) + 1 +
	  strlen(supportName.c_str()) + 1;
	supportEntryPath = new char[lenName];
	supportEntryPath = strcpy(supportEntryPath,"/Med/MEDMESH/MEDSUPPORTS_OF_");
	supportEntryPath = strcat(supportEntryPath,meshNameStudy.c_str());
	supportEntryPath = strcat(supportEntryPath,"/");
	supportEntryPath = strcat(supportEntryPath,supportName.c_str());

	SCRUTE(supportEntryPath);

	MESSAGE("supportEntryPath in field " << supportEntryPath << " length " << lenName);

// 	SALOMEDS::SObject_var supportObject = myStudy->FindObject(supportName.c_str());
	SALOMEDS::SObject_var supportObject = myStudy->FindObjectByPath(supportEntryPath);

	SCRUTE(supportObject);

  	if ( CORBA::is_nil(supportObject) ) 
        {
	  MESSAGE("supportObject is a nil corba object");
	  MESSAGE("FIELD_i::addInStudy : SUPPORT not found") ;
	} 
        else 
        {
	  MESSAGE("supportObject is OK and is now going to be referenced !");
	  SALOMEDS::SObject_var newObjSupport = myBuilder->NewObject(newObj);
	  myBuilder->Addreference(newObjSupport,supportObject);
	  MESSAGE(" OUF !!!");
	}

        myBuilder->CommitCommand();

	delete [] supportEntryPath;
	delete [] fieldEntryName;

	MESSAGE("FIELD_i::addInStudy");

        //END_OF("FIELD_i::addInStudy");
}
//=============================================================================
/*!
 * CORBA: write
 */
//=============================================================================
void FIELD_i::write (CORBA::Long i, const char* driverFieldName)
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
	try
	{
		_fieldTptr->write(i,driverFieldName);
        }
        catch (MEDEXCEPTION &ex)
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
void FIELD_i::read (CORBA::Long i)
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
	try
	{
		_fieldTptr->read(i);
        }
        catch (MEDEXCEPTION &ex)
        {
		MESSAGE("Exception en accedant au champ");
	        THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: rmDriver
 */
//=============================================================================
void FIELD_i::rmDriver (CORBA::Long i)
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
	try
	{
		_fieldTptr->rmDriver(i);
        }
        catch (MEDEXCEPTION &ex)
        {
		MESSAGE("Exception en accedant au champ");
	        THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: addDriver
 */
//=============================================================================
CORBA::Long FIELD_i::addDriver (SALOME_MED::medDriverTypes driverType, 
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
        catch (MEDEXCEPTION &ex)
        {
		MESSAGE("Exception en accedant au champ");
	        THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}

