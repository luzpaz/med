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
// File      : MEDMEM_Field_i.cxx
// Created   : mer fév 20 15:47:57 CET 2002
// Author    : EDF
// Project   : SALOME
//=============================================================================

#include "MEDMEM_Field_i.hxx"

#include "SALOME_NamingService.hxx"
#include "SALOME_LifeCycleCORBA.hxx"

#include CORBA_SERVER_HEADER(SALOME_ModuleCatalog)
#include CORBA_CLIENT_HEADER(MED_Gen)

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
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
FIELD_i::~FIELD_i()
{
  if ( _fieldTptr )
    _fieldTptr->removeReference();
  //if (_ownCppPtr) delete _fieldTptr;
}
//=============================================================================
/*!
 * Constructor
 */
//=============================================================================
FIELD_i::FIELD_i(::FIELD_ * const field, bool ownCppPtr):
  _ownCppPtr(ownCppPtr),
  _fieldTptr(field),
  _corbaIndex(FIELD_i::fieldIndex++),
  _FieldId("")
{
  if ( _fieldTptr ) _fieldTptr->addReference();
  FIELD_i::fieldMap[_corbaIndex]=_fieldTptr;

  MESSAGE("FIELD_i::FIELD_i  Checking of pointeurs !!!");

  SCRUTE(_fieldTptr);
}

//=============================================================================
/*!
 * Constructor par recopie
 */
//=============================================================================
FIELD_i::FIELD_i( FIELD_i & f):_ownCppPtr(false), _fieldTptr(f._fieldTptr),
                               _corbaIndex(FIELD_i::fieldIndex++),
                               _FieldId("")
{
  if ( _fieldTptr ) _fieldTptr->addReference();
  FIELD_i::fieldMap[_corbaIndex]=_fieldTptr;
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

void FIELD_i::setName(const char* theName)
  throw (SALOME::SALOME_Exception)
{
  if (_fieldTptr == NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Field", SALOME::INTERNAL_ERROR);

  try {
    _fieldTptr->setName(theName);
  }
  catch (MEDEXCEPTION &ex) {
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
  if (_fieldTptr==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Field",
                                 SALOME::INTERNAL_ERROR);
  
  SUPPORT_i* servant = new SUPPORT_i(_fieldTptr->getSupport());

  SALOME_MED::SUPPORT_ptr support=servant->_this();

  SCRUTE(_fieldTptr);

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
 * CORBA: Accessor for gauss numbers presence.
 */
//=============================================================================
CORBA::Boolean FIELD_i::getGaussPresence()
{
       if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _fieldTptr->getGaussPresence();
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
SALOME_TYPES::ListOfString * FIELD_i::getComponentsNames()
throw (SALOME::SALOME_Exception)    
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        SALOME_TYPES::ListOfString_var myseq = new SALOME_TYPES::ListOfString;
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
SALOME_TYPES::ListOfString * FIELD_i::getComponentsUnits()
throw (SALOME::SALOME_Exception)
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        SALOME_TYPES::ListOfString_var myseq = new SALOME_TYPES::ListOfString;
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
SALOME_TYPES::ListOfString * FIELD_i::getComponentsDescriptions()
throw (SALOME::SALOME_Exception)    
{
        if (_fieldTptr==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Field", \
                                             SALOME::INTERNAL_ERROR);
        SALOME_TYPES::ListOfString_var myseq = new SALOME_TYPES::ListOfString;
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
void FIELD_i::addInStudy (SALOMEDS::Study_ptr   myStudy, 
                          SALOME_MED::FIELD_ptr myIor)
  throw (SALOME::SALOME_Exception, SALOMEDS::StudyBuilder::LockProtection)
{
  SALOMEDS::SComponent_var aComponent = PublishMedComponent(myStudy);
  if (CORBA::is_nil(aComponent))
    THROW_SALOME_CORBA_EXCEPTION("SComponent labelled 'Med' not Found", SALOME::INTERNAL_ERROR);
  addInStudyToComponent(aComponent, myIor);
}

static SALOMEDS::SObject_ptr FindChildByName (SALOMEDS::SObject_ptr theFather,
                                              const string          theName)
{
  SALOMEDS::SObject_var aChild;
  if (CORBA::is_nil(theFather))
    return aChild._retn();

  SALOMEDS::SObject_var aCurChild;
  SALOMEDS::ChildIterator_ptr anIter = theFather->GetStudy()->NewChildIterator(theFather);
  for (; anIter->More() && aChild->_is_nil(); anIter->Next()) {
    aCurChild = anIter->Value();
    string aCurName = aCurChild->GetName();
    if (aCurName == theName)
      aChild = aCurChild;
  }
  return aChild._retn();
}

void FIELD_i::addInStudyToComponent (SALOMEDS::SComponent_ptr myComponent,
                                     SALOME_MED::FIELD_ptr    myIor)
  throw (SALOME::SALOME_Exception, SALOMEDS::StudyBuilder::LockProtection)
{
        if (CORBA::is_nil(myComponent) || CORBA::is_nil(myIor))
          THROW_SALOME_CORBA_EXCEPTION("Null parameter", SALOME::BAD_PARAM);

        if (_fieldTptr == NULL)
          THROW_SALOME_CORBA_EXCEPTION("No associated Field", SALOME::INTERNAL_ERROR);

        if (_FieldId != "") {
          MESSAGE("Field already in Study");
          THROW_SALOME_CORBA_EXCEPTION("Field already in Study", SALOME::BAD_PARAM);
        }

        SALOMEDS::Study_var myStudy = myComponent->GetStudy();
        SALOMEDS::StudyBuilder_var aBuilder = myStudy->NewBuilder();

        SALOMEDS::GenericAttribute_var anAttr;
        SALOMEDS::AttributeName_var    aName;
        SALOMEDS::AttributeIOR_var     aIOR;

        // Create SObject labelled 'MEDFIELD' if it doesn't already exist
        SALOMEDS::Study::ListOfSObject_var aMEDFIELDs =
          myStudy->FindObjectByName("MEDFIELD", myComponent->ComponentDataType());
        int aLength = aMEDFIELDs->length();
        SALOMEDS::SObject_var medfieldfather;
        if (aLength > 0) {
          medfieldfather = aMEDFIELDs[0];
        }
        else {
          MESSAGE("Add Object 'MEDFIELD'");
          medfieldfather = aBuilder->NewObject(myComponent);
          anAttr = aBuilder->FindOrCreateAttribute(medfieldfather, "AttributeName");
          aName = SALOMEDS::AttributeName::_narrow(anAttr);
          aName->SetValue("MEDFIELD");
        }

        // Create SObject labelled 'FIELDNAME' if it doesn't already exit
        string fieldName = _fieldTptr->getName();
        SALOMEDS::SObject_var medfieldnamefather = FindChildByName(medfieldfather, fieldName);
        if ( CORBA::is_nil(medfieldnamefather) ) 
        {
          MESSAGE("Add Object "<<fieldName);
          medfieldnamefather = aBuilder->NewObject(medfieldfather);
          anAttr = aBuilder->FindOrCreateAttribute(medfieldnamefather, "AttributeName");
          aName = SALOMEDS::AttributeName::_narrow(anAttr);
          aName->SetValue(fieldName.c_str());
        }

        string fieldEntryPath = "/";
        //fieldEntryPath += "Med/";
        string componentName = myComponent->GetName();
        fieldEntryPath += componentName + "/MEDFIELD/" + fieldName + "/";

        int iterationNumber = _fieldTptr->getIterationNumber();
        SCRUTE(iterationNumber);

        int orderNumber = _fieldTptr->getOrderNumber();
        SCRUTE(orderNumber);

        ostringstream iterationName;
        iterationName<<"(" << iterationNumber << "," << orderNumber << ")";
        string supportName = (_fieldTptr->getSupport())->getName();
        string meshName = ((_fieldTptr->getSupport())->getMesh())->getName();

        SCRUTE(meshName);
        for (string::size_type pos=0; pos<meshName.size(); ++pos) {
          if (isspace(meshName[pos])) meshName[pos] = '_';
        }

        char * fieldEntryName;
        int lenName = strlen(iterationName.str().c_str()) + 4 +
          strlen(supportName.c_str()) + 4 + strlen(meshName.c_str()) + 1;

        fieldEntryName = new char[lenName];
        fieldEntryName = strcpy(fieldEntryName,iterationName.str().c_str());
        fieldEntryName = strcat(fieldEntryName,"_ON_");
        fieldEntryName = strcat(fieldEntryName,supportName.c_str());
        fieldEntryName = strcat(fieldEntryName,"_OF_");
        fieldEntryName = strcat(fieldEntryName,meshName.c_str());

        SCRUTE(fieldEntryName);
        fieldEntryPath += fieldEntryName;

        // Create object labelled according to Field's Name

        SALOMEDS::SObject_var fieldSO = myStudy->FindObjectByPath(fieldEntryPath.c_str());
        bool alreadyPublished = ! CORBA::is_nil(fieldSO);
        aBuilder->NewCommand();
        if ( !alreadyPublished )
        {
          MESSAGE("Add a Field Object under "<<fieldName);
          fieldSO = aBuilder->NewObject(medfieldnamefather);
          // check that this method and getEntryPath() build the same path,
          // though this is true only for MED component
          MESSAGE("fieldEntryPath: "<< fieldEntryPath);
          MESSAGE("getEntryPath(): "<< getEntryPath());
          if (componentName == "Med")
            ASSERT( getEntryPath() == fieldEntryPath );
        }
        anAttr = aBuilder->FindOrCreateAttribute(fieldSO, "AttributeName");
        aName = SALOMEDS::AttributeName::_narrow(anAttr);
        aName->SetValue(fieldEntryName);

        ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
        ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting()) ;
        CORBA::ORB_var &orb = init(0,0);
        string iorStr = orb->object_to_string(myIor);
        anAttr = aBuilder->FindOrCreateAttribute(fieldSO, "AttributeIOR");
        aIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
        aIOR->SetValue(iorStr.c_str());
        _FieldId = fieldSO->GetID();

        if ( !alreadyPublished )
        {
          MESSAGE("Computing path to Support");

          string supportEntryPath = SUPPORT_i::getEntryPath( _fieldTptr->getSupport() );
          SCRUTE(supportEntryPath);

          SALOMEDS::SObject_var supportObject =
            myStudy->FindObjectByPath(supportEntryPath.c_str());

          SCRUTE(supportObject);

          if ( CORBA::is_nil(supportObject) ) 
          {
            MESSAGE("supportObject is a nil corba object");
            MESSAGE("FIELD_i::addInStudy : SUPPORT not found") ;
          } 
          else 
          {
            MESSAGE("supportObject is OK and is now going to be referenced !");
            SALOMEDS::SObject_var newObjSupport = aBuilder->NewObject(fieldSO);
            aBuilder->Addreference(newObjSupport,supportObject);
            MESSAGE(" OUF !!!");
          }
        }

        aBuilder->CommitCommand();

        delete [] fieldEntryName;

        // register the Corba pointer: increase the referrence count
        MESSAGE("Registering of the Corba Field pointer");
        Register();

        MESSAGE("FIELD_i::addInStudy");

        //END_OF();
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
                _fieldTptr->write(i);
        }
        catch (MEDEXCEPTION &)
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

//=============================================================================
/*!
 * internal method: publish MED component
 */
//=============================================================================

SALOMEDS::SComponent_ptr FIELD_i::PublishMedComponent(SALOMEDS::Study_ptr theStudy)
{
  if ( CORBA::is_nil(theStudy) )
    return SALOMEDS::SComponent::_nil();

  SALOMEDS::SComponent_var medfather = theStudy->FindComponent("MED");
  if ( !CORBA::is_nil(medfather) )
    return medfather._retn();

  ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
  ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting()) ;
  CORBA::ORB_var &orb = init(0,0);

  SALOME_NamingService* ns = SINGLETON_<SALOME_NamingService>::Instance();
  ASSERT(SINGLETON_<SALOME_NamingService>::IsAlreadyExisting());
  ns->init_orb( orb );

  SALOME_LifeCycleCORBA* lcc = new SALOME_LifeCycleCORBA( ns );

  SALOME_ModuleCatalog::ModuleCatalog_var aCatalog  = 
    SALOME_ModuleCatalog::ModuleCatalog::_narrow(ns->Resolve("/Kernel/ModulCatalog"));
  if ( CORBA::is_nil( aCatalog ) )
    return medfather._retn();
  SALOME_ModuleCatalog::Acomponent_var aComp = aCatalog->GetComponent( "MED" );
  if ( CORBA::is_nil( aComp ) )
    return medfather._retn();
  
  SALOMEDS::StudyBuilder_var aBuilder = theStudy->NewBuilder();
  aBuilder->NewCommand();
  bool aLocked = theStudy->GetProperties()->IsLocked();
  if (aLocked) theStudy->GetProperties()->SetLocked(false);
  
  medfather = aBuilder->NewComponent("MED");
  SALOMEDS::GenericAttribute_var anAttr = aBuilder->FindOrCreateAttribute(medfather, "AttributeName");
  SALOMEDS::AttributeName_var aName = SALOMEDS::AttributeName::_narrow(anAttr);
  aName->SetValue( aComp->componentusername() );
  
  Engines::EngineComponent_var aMedComponent = lcc->FindOrLoad_Component("FactoryServer", "MED");
  SALOME_MED::MED_Gen_var aMedEngine = SALOME_MED::MED_Gen::_narrow( aMedComponent );
  aBuilder->DefineComponentInstance(medfather, aMedEngine);
  
  if (aLocked) theStudy->GetProperties()->SetLocked(true);
  aBuilder->CommitCommand();
  
  return medfather._retn();
}

//================================================================================
/*!
 * \brief Return a default path to publish this field
  * \retval string - the path
 */
//================================================================================

string FIELD_i::getEntryPath ()
{
  string path;
  if ( _fieldTptr &&
       _fieldTptr->getSupport() &&
       _fieldTptr->getSupport()->getMesh() )
  {
    string meshName = _fieldTptr->getSupport()->getMesh()->getName();
    for (string::size_type pos=0; pos<meshName.size(); ++pos)
    {
      if (isspace(meshName[pos])) meshName[pos] = '_';
    }
    ostringstream os ;

    os << "/Med/MEDFIELD/" << _fieldTptr->getName() << "/" 
       << "(" << _fieldTptr->getIterationNumber()
       << "," << _fieldTptr->getOrderNumber()
       << ")_ON_" << _fieldTptr->getSupport()->getName()
       << "_OF_" << meshName;

    path = os.str();
  }
  return path;
}
