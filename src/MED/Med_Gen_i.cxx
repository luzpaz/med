//=============================================================================
// File      : Med_Gen_i.cxx
// Created   : mer fév 20 15:47:57 CET 2002
// Author    : Paul RASCLE, EDF
// Project   : SALOME
// Copyright : EDF 2001
// $Header$
//=============================================================================
using namespace std;

#include "Med_Gen_i.hxx"

#include "Mesh_i.hxx"
#include "Med_i.hxx"
#include "FieldInt_i.hxx"
#include "FieldDouble_i.hxx"
#include "Support_i.hxx"


#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_Med.hxx"
#include "MEDMEM_MedMedDriver.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_MedFieldDriver.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_DriversDef.hxx"


#include "Utils_SINGLETON.hxx"
#include "OpUtil.hxx"
#include "Utils_CorbaException.hxx"
#include "utilities.h"

#include <string>
#include <deque>
#include <map>

#include <TCollection_AsciiString.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include "SALOMEDS_Tool.hxx"

// Initialisation des variables statiques
 map <string, string> Med_Gen_i::_MedCorbaObj;
 string Med_Gen_i::_myFileName="";
 string Med_Gen_i::_saveFileName="";

//=============================================================================
/*!
 *  default constructor: not for use
 */
//=============================================================================

Med_Gen_i::Med_Gen_i()
{
  MESSAGE("Med_Gen_i::Med_Gen_i");
}

//=============================================================================
/*!
 *  standard constructor
 */
//=============================================================================

Med_Gen_i:: Med_Gen_i(CORBA::ORB_ptr orb,
				PortableServer::POA_ptr poa,
				PortableServer::ObjectId * contId, 
				const char *instanceName, 
				const char *interfaceName) :
  Engines_Component_i(orb, poa, contId, instanceName, interfaceName)
{
  MESSAGE("activate object");
  _thisObj = this ;
  _id = _poa->activate_object(_thisObj);

  _duringLoad=false;
  // get an NamingService interface
  _NS = SINGLETON_<SALOME_NamingService>::Instance() ;
  ASSERT(SINGLETON_<SALOME_NamingService>::IsAlreadyExisting()) ;
  _NS->init_orb( _orb ) ;
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
  
  myBuilder->NewCommand();
  
  // Create SComponent labelled 'Med' if it doesn't already exit
  SALOMEDS::SComponent_var medfather = myStudy->FindComponent("Med");
  if ( CORBA::is_nil(medfather) )
    {
      // mpv: component label must be created in spite of "Locked" study flag state
      bool aLocked = myStudy->GetProperties()->IsLocked();
      if (aLocked) myStudy->GetProperties()->SetLocked(false);
      
      MESSAGE("Add Component Med");
      medfather = myBuilder->NewComponent("Med");
      SALOMEDS::GenericAttribute_var anAttr = myBuilder->FindOrCreateAttribute(medfather, "AttributeName");
      SALOMEDS::AttributeName_var aName = SALOMEDS::AttributeName::_narrow(anAttr);
      aName->SetValue("Med");
      //	    Utilisation de this  deconseillee par Paul ??
      //	    myBuilder->DefineComponentInstance(medfather,POA_Engines::Med_Gen::_this());
      CORBA::Object_var myO = _poa->id_to_reference(*_id); // this ior...
      myBuilder->DefineComponentInstance(medfather,myO);
      
      if (aLocked) myStudy->GetProperties()->SetLocked(true);
    } 
  
  myBuilder->CommitCommand();
   
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
	SCRUTE(fileName);
  	SALOMEDS::Study_var myStudy = studyName2Study(studyName) ;
        if (!_duringLoad) addInStudy(myStudy) ;

	SALOME_MED::MED_ptr myMedIOR ;
	try 
	{
	  // we create a new MED_i and add in study
	  MED_i * myMedI = new MED_i();
	  myMedIOR = myMedI->_this() ;
	  if (!_duringLoad) myMedI->addInStudy(myStudy,myMedIOR) ; 	  
	  // create ::MED object, read all and add in study !
	  myMedI->init(myStudy,MED_DRIVER,fileName) ;
	}
        catch (const SALOMEDS::StudyBuilder::LockProtection & lp) {}
        catch(...)
        {
                MESSAGE("Erreur a la lecture du fichier");
                THROW_SALOME_CORBA_EXCEPTION("Unable to open File "\
                                                ,SALOME::BAD_PARAM);
        }
	return SALOME_MED::MED::_duplicate(myMedIOR) ;
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
  BEGIN_OF("Med_Gen_i::readStructFileWithFieldType (const char* fileName,const char* studyName)");

	SCRUTE(fileName);
  	SALOMEDS::Study_var myStudy = studyName2Study(studyName) ;

	if (!_duringLoad) addInStudy(myStudy) ;

	try 
	{
	  // we create a new MED_i and add in study
	  MED_i * myMedI = new MED_i();
	  SALOME_MED::MED_ptr myMedIOR = myMedI->_this() ;
	  if (!_duringLoad) myMedI->addInStudy(myStudy,myMedIOR) ;	  
	  // create ::MED object, read all and add in study !
	  myMedI->initWithFieldType(myStudy,MED_DRIVER,fileName) ;
	}
        catch (const SALOMEDS::StudyBuilder::LockProtection & lp) {}
        catch(...)
        {
                MESSAGE("Erreur a la lecture du fichier");
                THROW_SALOME_CORBA_EXCEPTION("Unable to open File "\
                                                ,SALOME::BAD_PARAM);
        }

	END_OF("Med_Gen_i::readStructFileWithFieldType (const char* fileName,const char* studyName)");


}

//=============================================================================
/*!
 *  Sert un Maillage
 */
//=============================================================================
SALOME_MED::MESH_ptr Med_Gen_i::readMeshInFile(const char* fileName,
                                               const char* studyName,
					       const char* meshName)
throw (SALOME::SALOME_Exception)
{
	SCRUTE(fileName);
  	SALOMEDS::Study_var myStudy = studyName2Study(studyName) ;

	if (!_duringLoad) addInStudy(myStudy) ;

// Creation du maillage

	MESH * myMesh= new MESH() ;
  	myMesh->setName(meshName);
  	MED_MESH_RDONLY_DRIVER myMeshDriver(fileName,myMesh);
	try
	{
  		myMeshDriver.setMeshName(meshName);
		myMeshDriver.open();
	}
        catch (const exception & ex)
        {
                MESSAGE("Exception Interceptee : ");
                SCRUTE(ex.what());
                THROW_SALOME_CORBA_EXCEPTION("Unable to find this mesh in this file",SALOME::BAD_PARAM);
        };
	try
	{
		myMeshDriver.read();
("apres read");
		myMeshDriver.close();
	}
        catch (const exception & ex)
        {
                MESSAGE("Exception Interceptee : ");
                SCRUTE(ex.what());
                THROW_SALOME_CORBA_EXCEPTION("Unable to read this mesh in this file",SALOME::BAD_PARAM);
        };

	MESH_i * meshi = new MESH_i(myMesh);
	//SALOME_MED::MESH_var mesh = SALOME_MED::MESH::_narrow(meshi->_this());
	SALOME_MED::MESH_ptr mesh = meshi->_this();
	try
        {
	// add the mesh object in study
	        if (!_duringLoad) meshi->addInStudy(myStudy,mesh);
	}
        catch (const SALOMEDS::StudyBuilder::LockProtection & lp) {}
        return SALOME_MED::MESH::_duplicate(mesh);
}
//=============================================================================
/*!
 *  Sert un Champ
 */
//=============================================================================
SALOME_MED::FIELD_ptr Med_Gen_i::readFieldInFile(const char* fileName,
                                               const char* studyName,
					       const char* fieldName,
					       CORBA::Long ordre,
					       CORBA::Long iter)
throw (SALOME::SALOME_Exception)
{
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
    	THROW_SALOME_CORBA_EXCEPTION("Wrong Study Name", \
                                 SALOME::BAD_PARAM);

        SALOMEDS::StudyBuilder_var myBuilder = myStudy->NewBuilder();
        SALOMEDS::SComponent_var medfather = myStudy->FindComponent("Med");
        if (CORBA::is_nil(medfather))
        {
		myBuilder->NewCommand();
                // mpv: component label must be created in spite of "Locked" study flag state
	        bool aLocked = myStudy->GetProperties()->IsLocked();
		if (aLocked) myStudy->GetProperties()->SetLocked(false);

                medfather = myBuilder->NewComponent("Med");
		SALOMEDS::AttributeName_var aName = SALOMEDS::AttributeName::_narrow(
                      myBuilder->FindOrCreateAttribute(medfather, "AttributeName"));
		aName->SetValue("Med");
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

	FIELD_ * myField= new FIELD_() ;
        MED * mymed = new MED(MED_DRIVER,fileName) ;
	try
	{
		deque<string> fieldsNames = mymed->getFieldNames() ;
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
/*
		deque<DT_IT_> myIteration = mymed->getFieldIteration (fieldName);
		if (myIteration.size() != 1)
		{
			MESSAGE("WARNING : My iteration size is ")
			SCRUTE(myIteration.size());
		}
*/
		myField = mymed->getField(fieldName,iter,ordre);
	}
        catch (const exception & ex)
        {
                MESSAGE("Exception Interceptee : ");
                SCRUTE(ex.what());
                THROW_SALOME_CORBA_EXCEPTION("Unable to find this field in this file",SALOME::BAD_PARAM);
        };
	                
	SUPPORT * fieldSupport;
	SALOME_MED::SUPPORT_ptr mySupportIOR;
        try
	{
		fieldSupport=(SUPPORT *)myField->getSupport() ;
		ASSERT(fieldSupport != NULL);
		MESH * myMesh=(MESH *)fieldSupport->getMesh();
		ASSERT(myMesh != NULL);
		myMesh->read();
		SCRUTE(myMesh->getName());
		fieldSupport->update();
        	SUPPORT_i * mySupportI = new SUPPORT_i(fieldSupport);
		mySupportIOR=mySupportI->_this();
	}
        catch (const exception & ex)
        {
                MESSAGE("Exception Interceptee : ");
                SCRUTE(ex.what());
                THROW_SALOME_CORBA_EXCEPTION("Unable to find associated support",SALOME::BAD_PARAM);
        };

	med_type_champ type = myField->getValueType() ;
	switch (type) 
	{
      	 case MED_FR::MED_INT32: 	
	 {
		try 
		{
			((FIELD<int>*)myField)->read() ;
			FIELDINT_i * myFieldIntI = new FIELDINT_i(mySupportIOR,(FIELD<int>*)myField);
        		POA_SALOME_MED::FIELD_tie<FIELD_i> * myFieldTie = new POA_SALOME_MED::FIELD_tie<FIELD_i>(myFieldIntI);
        		SALOME_MED::FIELD_ptr myFieldIOR = myFieldTie->_this() ;
        		if (!_duringLoad) myFieldIntI->addInStudy(myStudy,myFieldIOR) ;
        		return SALOME_MED::FIELD::_duplicate(myFieldIOR);
		}
		catch (const SALOMEDS::StudyBuilder::LockProtection & lp) {}
        	catch (const exception & ex)
        	{
               		MESSAGE("Exception Interceptee : ");
                	SCRUTE(ex.what());
                	THROW_SALOME_CORBA_EXCEPTION("Unable to read int field",SALOME::BAD_PARAM);
        	};
		break;
	 }
	 case MED_FR::MED_REEL64: 
	 {
		try 
		{
			((FIELD<double>*)myField)->read() ;
			FIELDDOUBLE_i * myFieldDoubleI = new FIELDDOUBLE_i(mySupportIOR,(FIELD<double>*)myField);
        		POA_SALOME_MED::FIELD_tie<FIELD_i> * myFieldTie = new POA_SALOME_MED::FIELD_tie<FIELD_i>(myFieldDoubleI);
        		SALOME_MED::FIELD_ptr myFieldIOR = myFieldTie->_this() ;
        		if (!_duringLoad) myFieldDoubleI->addInStudy(myStudy,myFieldIOR) ;
        		return SALOME_MED::FIELD::_duplicate(myFieldIOR);
		}
		catch (const SALOMEDS::StudyBuilder::LockProtection & lp) {}
        	catch (const exception & ex)
        	{
               		MESSAGE("Exception Interceptee : ");
                	SCRUTE(ex.what());
                	THROW_SALOME_CORBA_EXCEPTION("Unable to read double field",SALOME::BAD_PARAM);
        	};
		break;
	 }
	}

	
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
 *  CORBA: Save Mesh objects (called when a study is saved)
 */
//=============================================================================
SALOMEDS::TMPFile* Med_Gen_i::Save(SALOMEDS::SComponent_ptr theComponent) {
  const char* LOC = "Med_Gen_i::Save";
  BEGIN_OF(LOC);

  SALOMEDS::TMPFile_var aStreamFile;
  // Get a temporary directory to store a file
  TCollection_AsciiString aTmpDir = SALOMEDS_Tool::GetTmpDir();
  // Create a list to store names of created files
  SALOMEDS::ListOfFileNames_var aSeq = new SALOMEDS::ListOfFileNames;
  TColStd_SequenceOfAsciiString aFileNames;

  SALOMEDS::SObject_var aMedMeshFather = theComponent->GetStudy()->FindObject("MEDMESH");
  if (!CORBA::is_nil(aMedMeshFather)) {
    SALOMEDS::ChildIterator_var anIter = theComponent->GetStudy()->NewChildIterator(aMedMeshFather);
    for(; anIter->More(); anIter->Next()) {
      SALOMEDS::SObject_var aSO = anIter->Value();
      SALOMEDS::GenericAttribute_var anAttr;
      if (aSO->FindAttribute(anAttr,"AttributeIOR")) {
	CORBA::Object_var myIOR = _orb->string_to_object(SALOMEDS::AttributeIOR::_narrow(anAttr)->Value());
	SALOME_MED::MESH_var myMesh = SALOME_MED::MESH::_narrow(myIOR);
	if (! CORBA::is_nil(myMesh)) {
	  TCollection_AsciiString aName;
	  aName = "MED_tmp_";
	  aName += myMesh->getName();
	  aName += ".med";
//  	  MESSAGE("Save mesh with name "<<aName.ToCString());
	  long driverId = myMesh->addDriver(SALOME_MED::MED_DRIVER,(aTmpDir+aName).ToCString(),myMesh->getName());
	  myMesh->write(driverId,"");
	  aFileNames.Append(aName);
	}
      }
    }
  }

  SALOMEDS::SObject_var aMedFieldFather = theComponent->GetStudy()->FindObject("MEDFIELD");
  if (!CORBA::is_nil(aMedFieldFather)) {
    SALOMEDS::ChildIterator_var anIter = theComponent->GetStudy()->NewChildIterator(aMedFieldFather);
    for(; anIter->More(); anIter->Next()) {
      SALOMEDS::SObject_var aSO = anIter->Value();
      SALOMEDS::GenericAttribute_var anAttr;
      if (aSO->FindAttribute(anAttr,"AttributeIOR")) {
	CORBA::Object_var myIOR = _orb->string_to_object(SALOMEDS::AttributeIOR::_narrow(anAttr)->Value());
	SALOME_MED::FIELD_var myField = SALOME_MED::FIELD::_narrow(myIOR);
	if (! CORBA::is_nil(myField)) {
	  TCollection_AsciiString aName;
	  aName = "MED_tmp_";
	  aName += myField->getName();
	  aName += ".med";
//  	  MESSAGE("Save mesh with name "<<aName.ToCString());
	  long driverId = myField->addDriver(SALOME_MED::MED_DRIVER,(aTmpDir+aName).ToCString(),myField->getName());
	  myField->write(driverId,"");
	  aFileNames.Append(aName);
	}
      }
    }
  }

  int i;
  aSeq->length(aFileNames.Length());
  for(i = aFileNames.Length(); i > 0; i--) aSeq[i-1] = CORBA::string_dup(aFileNames.Value(i).ToCString());
  // Conver a file to the byte stream
  aStreamFile = SALOMEDS_Tool::PutFilesToStream(aTmpDir.ToCString(), aSeq);
  // Remove the created file and tmp directory
  SALOMEDS_Tool::RemoveTemporaryFiles(aTmpDir.ToCString(), aSeq, true);
  // Return the created byte stream
  return aStreamFile._retn();
  
  END_OF(LOC);
}

//=============================================================================
/*!
 *  CORBA: Load Mesh objects (called when an existing study is opened)
 */
//=============================================================================

CORBA::Boolean Med_Gen_i::Load(SALOMEDS::SComponent_ptr theComponent, const SALOMEDS::TMPFile& theStream) {
  const char* LOC = "Med_Gen_i::Load";
  BEGIN_OF(LOC);

  // Get a temporary directory for a file
  TCollection_AsciiString aTmpDir = SALOMEDS_Tool::GetTmpDir();
  _saveFileName = CORBA::string_dup(aTmpDir.ToCString());
  SALOMEDS::ListOfFileNames_var aSeq = SALOMEDS_Tool::PutStreamToFiles(theStream, aTmpDir.ToCString());
  return true;

  END_OF(LOC);
}

//=============================================================================
/*!
 *  CORBA: 
 */
//=============================================================================

void Med_Gen_i::Close(const char *IORSComponent)
{
  MESSAGE("Med_Gen_i::Close");
}

//=============================================================================
/*!
 *  CORBA:
 */
//=============================================================================

char* Med_Gen_i::ComponentDataType()
{
  MESSAGE("Med_Gen_i::ComponentDataType");
  return strdup("Med") ; /* What is this type ? */
}
    
//=============================================================================
/*!
 *  CORBA: give a persistent reference of a transient object (for study save) 
 */
//=============================================================================

char* Med_Gen_i::IORToLocalPersistentID(const char* IORString,
					CORBA::Boolean& IsAFile)
{
  const char * LOC = "Med_Gen_i::IORToLocalPersistentID" ;
  BEGIN_OF(LOC) ;

  SCRUTE(IORString);
  if (string(IORString).size()==0) return strdup("");
  // Well, we know where put object (_saveFilename) and we know object (IORString)
  // cast object :
  CORBA::Object_var myIOR = _orb->string_to_object(IORString);

  // MED
  SALOME_MED::MED_var myMed = SALOME_MED::MED::_narrow(myIOR);
  if (! CORBA::is_nil(myMed)) 
  {
        // nothing to save : Support will be saved inside the mesh
	string str_MedName="Objet Med + /OBJ_MED/";
        return strdup(str_MedName.c_str()) ; 
  }
 
  // MESH
  SALOME_MED::MESH_var myMesh = SALOME_MED::MESH::_narrow(myIOR);
  if (! CORBA::is_nil(myMesh)) 
  {
        string str_MeshName;
	try
	{
    		long driverId = myMesh->addDriver(SALOME_MED::MED_DRIVER,_saveFileName.c_str(),myMesh->getName()) ;
    		myMesh->write(driverId,"");
        	str_MeshName=string("/ENS_MAA/")+string(myMesh->getName());
	}
	catch(...)
	{
		MESSAGE("Unable to save the mesh");
                THROW_SALOME_CORBA_EXCEPTION("Unable to save Mesh in Med"\
                                              ,SALOME::INTERNAL_ERROR);
	}
    	return strdup(str_MeshName.c_str()) ;
  }
    
  // SUPPORT
  SALOME_MED::SUPPORT_var mySupport = SALOME_MED::SUPPORT::_narrow(myIOR);
  if (! CORBA::is_nil(mySupport)) 
  {
        // nothing to save : Support will be saved inside the mesh
	string str_SupportName;
	try
	{
    		str_SupportName=string("/FAS/")+string(mySupport->getName());
		str_SupportName+=string("/ENS_MAA/")+string(mySupport->getMesh()->getName());
   		SCRUTE(str_SupportName);
	}
	catch(...)
	{
		MESSAGE("Unable to save the support");
                THROW_SALOME_CORBA_EXCEPTION("Unable to save Field in Med"\
                                              ,SALOME::INTERNAL_ERROR);
	}
    return strdup(str_SupportName.c_str());
  }
    
  SALOME_MED::FIELD_var myField = SALOME_MED::FIELD::_narrow(myIOR);
  if (! CORBA::is_nil(myField)) 
  {
	string str_FieldName;
	try
	{
    		long driverId = myField->addDriver(SALOME_MED::MED_DRIVER,_saveFileName.c_str(),myField->getName()) ;
    		myField->write(driverId,"");
    		str_FieldName=string("/CHA/")+string(myField->getName());
		ostringstream a,b;
 		a<< myField->getOrderNumber();
 		b<< myField->getIterationNumber();
		str_FieldName+=string("/ORDRE/")+a.str()+string("/ITER/")+b.str();
	}
	catch(...)
	{
		MESSAGE("Unable to save the mesh");
                THROW_SALOME_CORBA_EXCEPTION("Unable to save Field in Med"\
                                              ,SALOME::INTERNAL_ERROR);
	}
	return strdup(str_FieldName.c_str());
  }

  //THROW_SALOME_CORBA_EXCEPTION("Unable to save IOR",SALOME::BAD_PARAM);
  return strdup("");

  END_OF(LOC) ;
}

//=============================================================================
/*!
 *  CORBA: give a transient reference (when loading an object, opening study)
 */
//=============================================================================

char* Med_Gen_i::LocalPersistentIDToIOR(const char* aLocalPersistentID)
{
  const char * LOC = "Med_Gen_i::LocalPersistentIDToIOR" ;
  BEGIN_OF(LOC) ;

  TCollection_AsciiString aTmpDir(CORBA::string_dup(_saveFileName.c_str()));
  TCollection_AsciiString aFileName("MED_tmp_");
  aFileName += CORBA::string_dup(aLocalPersistentID);
  aFileName += ".med";

  MESH * myMesh= new MESH() ;
  myMesh->setName(aLocalPersistentID);
  MED_MESH_RDONLY_DRIVER myMeshDriver((aTmpDir+aFileName).ToCString(),myMesh);
  try
    {
      myMeshDriver.setMeshName(aLocalPersistentID);
      myMeshDriver.open();
    }
  catch (const exception & ex)
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
  catch (const exception & ex)
    {
      MESSAGE("Exception Interceptee : ");
      SCRUTE(ex.what());
      THROW_SALOME_CORBA_EXCEPTION("Unable to read this mesh in this file",SALOME::BAD_PARAM);
    };
  
  MESH_i * meshi = new MESH_i(myMesh);
  //SALOME_MED::MESH_var mesh = SALOME_MED::MESH::_narrow(meshi->_this());
  SALOME_MED::MESH_ptr mesh = meshi->_this();

  SALOMEDS::ListOfFileNames_var aSeq = new SALOMEDS::ListOfFileNames;
  aSeq->length(1);
  aSeq[0]=CORBA::string_dup(aFileName.ToCString());
  SALOMEDS_Tool::RemoveTemporaryFiles(aTmpDir.ToCString(), aSeq, true);

  return(CORBA::string_dup(_orb->object_to_string(mesh)));

  END_OF(LOC) ;
}

//=============================================================================
/*!
 *  returns true, if can publish object
 */
//=============================================================================
bool Med_Gen_i::CanPublishInStudy(CORBA::Object_ptr theIOR) {
  SALOME_MED::MESH_var aMesh = SALOME_MED::MESH::_narrow(theIOR);
  return !(aMesh->_is_nil());
}

//=============================================================================
/*!
 *  publish the given object
 */
//=============================================================================
SALOMEDS::SObject_ptr Med_Gen_i::PublishInStudy(SALOMEDS::Study_ptr theStudy,
						SALOMEDS::SObject_ptr theSObject,
						CORBA::Object_ptr theObject,
						const char* theName) throw (SALOME::SALOME_Exception)
{
  SALOMEDS::SObject_var aResultSO;

  if(CORBA::is_nil(theObject)) return aResultSO;
  if(theStudy->_is_nil()) return aResultSO;

  SALOMEDS::StudyBuilder_var aBuilder = theStudy->NewBuilder();
  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeName_var    aName;
  SALOMEDS::AttributeIOR_var     anIOR;
  SALOMEDS::SComponent_var aFather = theStudy->FindComponent("Med");

  if (aFather->_is_nil()) {
    aFather = aBuilder->NewComponent("Med");
    if (aFather->_is_nil()) return aResultSO;
    anAttr = aBuilder->FindOrCreateAttribute(aFather, "AttributeName");
    aName = SALOMEDS::AttributeName::_narrow(anAttr);
    aName->SetValue("MED");
    aBuilder->DefineComponentInstance(aFather, Med_Gen::_this());
  }

  if (CORBA::is_nil(theSObject)) {
    SALOME_MED::MESH_var aMesh = SALOME_MED::MESH::_narrow(theObject);
    aMesh->addInStudy(theStudy, aMesh);
    SALOMEDS::SObject_var aResultSO = theStudy->FindObjectIOR(_orb->object_to_string(theObject));
  } else {
    if (!theSObject->ReferencedObject(aResultSO)) 
      THROW_SALOME_CORBA_EXCEPTION("Publish in study MED object error",SALOME::BAD_PARAM);
  }
//    aBuilder->Addreference(theObject, aResultSO);
  return aResultSO._retn();
}

//=============================================================================
/*! 
 * C factory, accessible with dlsym, after dlopen  
 */
//=============================================================================

extern "C"
{
  PortableServer::ObjectId * MedEngine_factory(
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
}
