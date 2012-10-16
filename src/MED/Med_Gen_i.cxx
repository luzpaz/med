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

//  MED MED : implemetation of MED idl descriptions
//  File   : Med_Gen_i.cxx
//  Author : Paul RASCLE, EDF
//  Module : MED

#include "Med_Gen_i.hxx"
#include "MED_version.h"

#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Med_i.hxx"
#include "MEDMEM_FieldTemplate_i.hxx"
#include "MEDMEM_Support_i.hxx"

#include "MEDMEM_Mesh_i.hxx"
#include "MEDMEM_Field.hxx"

#include "MEDMEM_MedFileBrowser.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_MedFieldDriver.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_Grid.hxx"


#include "Utils_SINGLETON.hxx"
#include "OpUtil.hxx"
#include "Utils_CorbaException.hxx"
#include "utilities.h"

#include "SALOMEDS_Tool.hxx"

#include <string>
#include <deque>
#include <map>

#include <HDFascii.hxx>

using namespace MEDMEM;

// Initialisation des variables statiques
//string Med_Gen_i::_myFileName="";
//string Med_Gen_i::_saveFileName="";
 Med_Gen_i* Med_Gen_i::_MEDGen = NULL;

//=============================================================================
/*!
 *  default constructor: not for use
 */
//=============================================================================
//Med_Gen_i::Med_Gen_i()
//{
//  MESSAGE("Med_Gen_i::Med_Gen_i");
//}

//=============================================================================
/*!
 *  standard constructor
 */
//=============================================================================
Med_Gen_i::Med_Gen_i(CORBA::ORB_ptr orb,
                     PortableServer::POA_ptr poa,
                     PortableServer::ObjectId * contId,
                     const char *instanceName,
                     const char *interfaceName)
  : Engines_Component_i(orb, poa, contId, instanceName, interfaceName),
    Med_Gen_Driver_i(orb)
{
  MESSAGE("activate object");
  _thisObj = this ;
  _id = _poa->activate_object(_thisObj);

  _duringLoad=false;
  // get an NamingService interface
  //_NS = SINGLETON_<SALOME_NamingService>::Instance() ;
  //ASSERT(SINGLETON_<SALOME_NamingService>::IsAlreadyExisting()) ;
  //_NS->init_orb( _orb ) ;

  //_myMedI = 0;
  _MEDGen = this;
}

//=============================================================================
/*!
 *  Destructor
 */
//=============================================================================
Med_Gen_i::~Med_Gen_i()
{
  MESSAGE("Med_Gen_i::~Med_Gen_i");
}

//=============================================================================
/*!
 *  GetServant [ static ]
 *
 *  Get servant of the CORBA object
 */
//=============================================================================
PortableServer::ServantBase_var Med_Gen_i::GetServant( CORBA::Object_ptr theObject )
{
  if( CORBA::is_nil( theObject ) || !_MEDGen || CORBA::is_nil( _MEDGen->_poa ) )
    return NULL;
  try {
    PortableServer::Servant aServant = _MEDGen->_poa->reference_to_servant( theObject );
    return aServant;
  }
  catch (...) {
    INFOS( "GetServant - Unknown exception was caught!!!" );
    return NULL;
  }
}

//=============================================================================
/*!
 *  private method : change a study name in SALOMEDS::Study_var
 */
//=============================================================================
SALOMEDS::Study_var Med_Gen_i::studyName2Study(const char* studyName)
  throw(SALOME::SALOME_Exception)
{
  string myStudyName(studyName);

  if (myStudyName.size() == 0)
    THROW_SALOME_CORBA_EXCEPTION("No Study Name given", \
                                 SALOME::BAD_PARAM);

  // Get StudyManager Reference, current study,

  CORBA::Object_var obj = _NS->Resolve("/myStudyManager");
  SALOMEDS::StudyManager_var myStudyManager =
    SALOMEDS::StudyManager::_narrow(obj);
  if(CORBA::is_nil(myStudyManager))
    THROW_SALOME_CORBA_EXCEPTION("No StudyManager Found in NameService", \
                                 SALOME::BAD_PARAM);

  SALOMEDS::Study_var myStudy =
    myStudyManager->GetStudyByName(myStudyName.c_str());
  if (CORBA::is_nil(myStudy))
    THROW_SALOME_CORBA_EXCEPTION("Wrong Study Name", \
                                 SALOME::BAD_PARAM);

  return SALOMEDS::Study::_duplicate(myStudy) ;
}

//=============================================================================
/*!
 *  private method : add Med component in Study (Not MedGen ???)
 */
//=============================================================================
void Med_Gen_i::addInStudy(SALOMEDS::Study_var myStudy)
  throw(SALOME::SALOME_Exception)
{
  SALOMEDS::StudyBuilder_var  myBuilder = myStudy->NewBuilder();
  // Create SComponent labelled 'Med' if it doesn't already exit
  SALOMEDS::SComponent_var medfather = myStudy->FindComponent("MED");
  if ( CORBA::is_nil(medfather) )
    {
      myBuilder->NewCommand();
      // mpv: component label must be created in spite of "Locked" study flag state
      bool aLocked = myStudy->GetProperties()->IsLocked();
      if (aLocked) myStudy->GetProperties()->SetLocked(false);

      MESSAGE("Add Component Med");
      medfather = myBuilder->NewComponent("MED");
      SALOMEDS::GenericAttribute_var anAttr = myBuilder->FindOrCreateAttribute(medfather, "AttributeName");
      SALOMEDS::AttributeName_var aName = SALOMEDS::AttributeName::_narrow(anAttr);
      //NRI      aName->SetValue("Med");

      CORBA::Object_var objVarN = _NS->Resolve("/Kernel/ModulCatalog");
      SALOME_ModuleCatalog::ModuleCatalog_var Catalogue  = SALOME_ModuleCatalog::ModuleCatalog::_narrow(objVarN);
      SALOME_ModuleCatalog::Acomponent_var Comp = Catalogue->GetComponent( "MED" );
      if ( !Comp->_is_nil() ) {
        aName->SetValue( Comp->componentusername() );
      }

      //            Utilisation de this  deconseillee par Paul ??
      //            myBuilder->DefineComponentInstance(medfather,POA_Engines::MED_Gen::_this());
      CORBA::Object_var myO = _poa->id_to_reference(*_id); // this ior...
      myBuilder->DefineComponentInstance(medfather,myO);

      if (aLocked) myStudy->GetProperties()->SetLocked(true);
      myBuilder->CommitCommand();
    }
}

//=============================================================================
/*!
 * Lit tous les objets contenus dans un fichier med et les lit en memoire
 */
//=============================================================================
SALOME_MED::MED_ptr Med_Gen_i::readStructFile (const char* fileName,
                                               const char* studyName)
  throw(SALOME::SALOME_Exception)
{
        beginService("Med_Gen_i::readStructFile");

        SCRUTE(fileName);
        SALOMEDS::Study_var myStudy = studyName2Study(studyName) ;
        //if (!_duringLoad) addInStudy(myStudy) ;

        SALOME_MED::MED_ptr myMedIOR ;
        try
        {
          // we create a new MED_i and add in study
          //SALOMEDS::SComponent_var theComponent = myStudy->FindComponent("MED");
          //MED_i * myMedI = 0;
          //if (CORBA::is_nil(theComponent))
          //  myMedI = new MED_i();
          //else
          //  myMedI = Med_Gen_Driver_i::GetMED(theComponent);
          MED_i * myMedI = new MED_i();
          myMedIOR = myMedI->_this() ;
//        if (!_duringLoad) myMedI->addInStudy(myStudy,myMedIOR) ;
//        if (!_duringLoad) myMedI->addInStudy(myStudy,myMedIOR,fileName) ;
          // create ::MED object, read all and add in study !
          myMedI->init(myStudy,MED_DRIVER,fileName) ;
        }
        catch (const SALOMEDS::StudyBuilder::LockProtection & ) {}
        catch(...)
        {
                MESSAGE("Erreur a la lecture du fichier");
                THROW_SALOME_CORBA_EXCEPTION("Unable to open File "\
                                                ,SALOME::BAD_PARAM);
        }

        endService("Med_Gen_i::readStructFile");
        return myMedIOR;
}

//=============================================================================
/*!
 *  Prepare un Maillage sans le lire en memoire avec stokage dans l'etude
 *  des champs avec leur type
 */
//=============================================================================
void Med_Gen_i::readStructFileWithFieldType (const char* fileName,
                                             const char* studyName)
  throw (SALOME::SALOME_Exception)
{
        beginService("Med_Gen_i::readStructFileWithFieldType");
        SCRUTE(fileName);
        SALOMEDS::Study_var myStudy = studyName2Study(studyName) ;
        if (!_duringLoad) addInStudy(myStudy) ;

        try
        {
          // we create a new MED_i and add in study
          SALOMEDS::SComponent_var theComponent = myStudy->FindComponent("MED");
          //MED_i * myMedI = 0;
          //if (CORBA::is_nil(theComponent))
          //  myMedI = new MED_i();
          //else
          //  myMedI = Med_Gen_Driver_i::GetMED(theComponent);
          MED_i * myMedI = new MED_i();
          SALOME_MED::MED_ptr myMedIOR = myMedI->_this();
          if (!_duringLoad) myMedI->addInStudy(myStudy,myMedIOR,theComponent,fileName);
          // create ::MED object, read all and add in study !
          myMedI->initWithFieldType(myStudy,MED_DRIVER,fileName);
        }
        catch (const SALOMEDS::StudyBuilder::LockProtection & ) {}
        catch(...)
        {
                MESSAGE("Erreur a la lecture du fichier");
                THROW_SALOME_CORBA_EXCEPTION("Unable to open File "\
                                                ,SALOME::BAD_PARAM);
        }

        endService("Med_Gen_i::readStructFileWithFieldType");
}

//=============================================================================
/*!
 *  Sert un Maillage
 */
//=============================================================================
SALOME_MED::GMESH_ptr Med_Gen_i::readMeshInFile(const char* fileName,
                                                const char* studyName,
                                                const char* meshName)
throw (SALOME::SALOME_Exception)
{
        beginService("Med_Gen_i::readMeshInFile");
        SCRUTE(fileName);
        SALOMEDS::Study_var myStudy = studyName2Study(studyName) ;

//      if (!_duringLoad) addInStudy(myStudy) ;

// Creation du maillage

        GMESH * myMesh;

        // skl for IPAL14240
        // check mesh or grid:
        try {
          if ( MEDFILEBROWSER( fileName ).isStructuredMesh( meshName ) )
            myMesh = new GRID();
          else
            myMesh = new MESH() ;
        }
        catch (const std::exception & ex) {
          MESSAGE("Exception Interceptee : ");
          SCRUTE(ex.what());
          myMesh = new MESH() ;
        };

        myMesh->setName(meshName);
        MED_MESH_RDONLY_DRIVER myMeshDriver(fileName,myMesh);
        try
        {
                myMeshDriver.setMeshName(meshName);
                myMeshDriver.open();
        }
#if defined(_DEBUG_) || defined(_DEBUG)
        catch (const std::exception & ex)
#else
        catch (const std::exception &)
#endif
        {
                MESSAGE("Exception Interceptee : ");
                SCRUTE(ex.what());
                THROW_SALOME_CORBA_EXCEPTION("Unable to find this mesh in this file",SALOME::BAD_PARAM);
        };
        try
        {
                myMeshDriver.read();
                MESSAGE("apres read");
                myMeshDriver.close();
        }
#if defined(_DEBUG_) || defined(_DEBUG)
        catch (const std::exception & ex)
#else
        catch (const std::exception &)
#endif
        {
                MESSAGE("Exception Interceptee : ");
                SCRUTE(ex.what());
                THROW_SALOME_CORBA_EXCEPTION("Unable to read this mesh in this file",SALOME::BAD_PARAM);
        };

        GMESH_i * meshi = new GMESH_i(myMesh);
        //SALOME_MED::MESH_var mesh = SALOME_MED::MESH::_narrow(meshi->_this());
        SALOME_MED::GMESH_ptr mesh = meshi->_this();
        try
        {
          // add the mesh object in study
//        if (!_duringLoad) meshi->addInStudy(myStudy,mesh);
        }
        catch (const SALOMEDS::StudyBuilder::LockProtection & ) {}

        endService("Med_Gen_i::readMeshInFile");
        return mesh;
}

//=============================================================================
/*!
 *  Sert un Champ
 */
//=============================================================================
SALOME_MED::FIELD_ptr Med_Gen_i::readFieldInFile(const char* fileName,
                                                 const char* studyName,
                                                 const char* fieldName,
                                                 CORBA::Long iter,
                                                 CORBA::Long ordre)
throw (SALOME::SALOME_Exception)
{
  beginService("Med_Gen_i::readFieldInFile");
  SCRUTE(fileName);
  string myStudyName(studyName);

  if (myStudyName.size() == 0)
    THROW_SALOME_CORBA_EXCEPTION("No Study Name given", \
                                 SALOME::BAD_PARAM);

  // Get StudyManager Reference, current study,

  CORBA::Object_var obj = _NS->Resolve("/myStudyManager");
  SALOMEDS::StudyManager_var myStudyManager =
    SALOMEDS::StudyManager::_narrow(obj);
  ASSERT(! CORBA::is_nil(myStudyManager));
  SALOMEDS::Study_var myStudy =
    myStudyManager->GetStudyByName(myStudyName.c_str());
  if (CORBA::is_nil(myStudy))
    THROW_SALOME_CORBA_EXCEPTION("Wrong Study Name",    \
                                 SALOME::BAD_PARAM);

  SALOMEDS::StudyBuilder_var myBuilder = myStudy->NewBuilder();
  SALOMEDS::SComponent_var medfather = myStudy->FindComponent("MED");
  if (CORBA::is_nil(medfather))
  {
    myBuilder->NewCommand();
    // mpv: component label must be created in spite of "Locked" study flag state
    bool aLocked = myStudy->GetProperties()->IsLocked();
    if (aLocked) myStudy->GetProperties()->SetLocked(false);

    medfather = myBuilder->NewComponent("MED");
    SALOMEDS::AttributeName_var aName = SALOMEDS::AttributeName::_narrow(
                                                                         myBuilder->FindOrCreateAttribute(medfather, "AttributeName"));
    //NRI           aName->SetValue("Med");

    CORBA::Object_var objVarN = _NS->Resolve("/Kernel/ModulCatalog");
    SALOME_ModuleCatalog::ModuleCatalog_var Catalogue  = SALOME_ModuleCatalog::ModuleCatalog::_narrow(objVarN);
    SALOME_ModuleCatalog::Acomponent_var Comp = Catalogue->GetComponent( "MED" );
    if ( !Comp->_is_nil() ) {
      aName->SetValue( Comp->componentusername() );
    }

    CORBA::Object_var myO = _poa->id_to_reference(*_id); // this ior...
    myBuilder->DefineComponentInstance(medfather,myO);

    if (aLocked) myStudy->GetProperties()->SetLocked(true);
    myBuilder->CommitCommand();

  }
  else
    MESSAGE("MED dejà dans l étude");

  MESSAGE("Lecture du fichier ")
    SCRUTE(fileName);

  // Creation du champ

  FIELD_ * myField;
  MEDFILEBROWSER mymed;
  try
  {
    mymed.readFileStruct(fileName) ;
  }
  catch (const std::exception & ex)
  {
    MESSAGE("Exception Interceptee : ");
    SCRUTE(ex.what());
    THROW_SALOME_CORBA_EXCEPTION("Unable to find this file ",SALOME::BAD_PARAM);
  }

  try
  {
    vector<string> fieldsNames = mymed.getFieldNames() ;
    int numberOfFields = fieldsNames.size();
    int i;
    for (i=0; i<numberOfFields; i++)
    {
      if (fieldsNames[i]== fieldName) break;
    }
    if (i == numberOfFields)
    {
      THROW_SALOME_CORBA_EXCEPTION("Unable to find this field ",SALOME::BAD_PARAM);
    }
    MESSAGE("trouve");

    switch( mymed.getFieldType( fieldName ))
    {
    case MED_REEL64: myField = new FIELD<double>; break;
    case MED_INT32:  
    case MED_INT64:  myField = new FIELD<int>; break;
    default:
      THROW_SALOME_CORBA_EXCEPTION("Invalid field type",SALOME::BAD_PARAM);
    }
    myField->setIterationNumber( iter );
    myField->setOrderNumber( ordre );
    myField->addDriver( MED_DRIVER, fileName, fieldName, MED_EN::RDONLY);

  }
#if defined(_DEBUG_) || defined(_DEBUG)
  catch (const std::exception & ex)
#else
  catch (const std::exception &)
#endif
  {
    MESSAGE("Exception Interceptee : ");
    SCRUTE(ex.what());
    THROW_SALOME_CORBA_EXCEPTION("Unable to find this field in this file",SALOME::BAD_PARAM);
  };

  GMESH* myMesh=0;
  try {
    string MeshName = mymed.getMeshName( fieldName );
    myMesh = mymed.isStructuredMesh( MeshName ) ? (GMESH*) new GRID : (GMESH*) new MESH;
    myMesh->addDriver(MED_DRIVER, fileName, MeshName, MED_EN::RDONLY );
    myMesh->read();
    SCRUTE(myMesh->getName());
  }
#if defined(_DEBUG_) || defined(_DEBUG)
  catch (const std::exception & ex)
#else
  catch (const std::exception &)
#endif
  {
    MESSAGE("Exception Interceptee : ");
    SCRUTE(ex.what());
    THROW_SALOME_CORBA_EXCEPTION("Unable to find associated mesh",SALOME::BAD_PARAM);
  };

  med_type_champ type = myField->getValueType() ;
  switch (type)
  {
  case MED_EN::MED_INT32:
    {
      try
      {
        ((FIELD<int>*)myField)->read() ;
        myField->getSupport()->setMesh( myMesh );
        FIELDTEMPLATE_I<int,FullInterlace> * myFieldIntI = new FIELDTEMPLATE_I<int,FullInterlace>((FIELD<int,FullInterlace>*)myField);
        SALOME_MED::FIELD_ptr myFieldIOR = myFieldIntI->_this();
        //                      if (!_duringLoad) myFieldIntI->addInStudy(myStudy,myFieldIOR) ;
        endService("Med_Gen_i::readFieldInFile");
        return myFieldIOR;
      }
      catch (const SALOMEDS::StudyBuilder::LockProtection &) {}
#if defined(_DEBUG_) || defined(_DEBUG)
      catch (const std::exception & ex)
#else
        catch (const std::exception &)
#endif
        {
          MESSAGE("Exception Interceptee : ");
          SCRUTE(ex.what());
          THROW_SALOME_CORBA_EXCEPTION("Unable to read int field",SALOME::BAD_PARAM);
        };
      break;
    }
  case MED_EN::MED_REEL64:
    {
      try
      {
        ((FIELD<double>*)myField)->read() ;
        myField->getSupport()->setMesh( myMesh );
        FIELDTEMPLATE_I<double,FullInterlace> * myFieldDoubleI = new FIELDTEMPLATE_I<double,FullInterlace>((FIELD<double,FullInterlace>*)myField);
        SALOME_MED::FIELD_ptr myFieldIOR = myFieldDoubleI->_this() ;
        //                      if (!_duringLoad) myFieldDoubleI->addInStudy(myStudy,myFieldIOR) ;
        endService("Med_Gen_i::readFieldInFile");
        return myFieldIOR;
      }
      catch (const SALOMEDS::StudyBuilder::LockProtection &) {}

#if defined(_DEBUG_) || defined(_DEBUG)
      catch (const std::exception & ex)
#else
        catch (const std::exception &)
#endif
        {
          MESSAGE("Exception Interceptee : ");
          SCRUTE(ex.what());
          THROW_SALOME_CORBA_EXCEPTION("Unable to read double field",SALOME::BAD_PARAM);
        };
      break;
    }
  default:
    THROW_SALOME_CORBA_EXCEPTION("Not recognized type of field !",SALOME::BAD_PARAM);
  }

  return SALOME_MED::FIELD::_nil();
}


//=============================================================================
/*!
 *  from Driver
 */
//=============================================================================
Engines::EngineComponent_ptr Med_Gen_i::GetComponentInstance()
{
  return MED_Gen::_this();
}

char* Med_Gen_i::ComponentDataType()
{
  MESSAGE("Med_Gen_i::ComponentDataType");
  return CORBA::string_dup("MED") ; /* What is this type ? */
}

// Version information
char* Med_Gen_i::getVersion()
{
#if SALOMEMED_DEVELOPMENT
  return CORBA::string_dup(SALOMEMED_VERSION_STR"dev");
#else
  return CORBA::string_dup(SALOMEMED_VERSION_STR);
#endif
}



//=============================================================================
/*!
 * C factory, accessible with dlsym, after dlopen
 */
//=============================================================================

extern "C" MED_EXPORT 
  PortableServer::ObjectId * MEDEngine_factory(
                               CORBA::ORB_ptr orb,
                               PortableServer::POA_ptr poa,
                               PortableServer::ObjectId * contId,
                               const char *instanceName,
                               const char *interfaceName)
{
  MESSAGE("PortableServer::ObjectId * MedEngine_factory()");
  SCRUTE(interfaceName);
  Med_Gen_i * myMed_Gen
    = new Med_Gen_i(orb, poa, contId, instanceName, interfaceName);
  return myMed_Gen->getId() ;
}
