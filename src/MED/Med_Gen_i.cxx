//  MED MED : implemetation of MED idl descriptions
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
//  File   : Med_Gen_i.cxx
//  Author : Paul RASCLE, EDF
//  Module : MED
//  $Header$

using namespace std;
#include "Med_Gen_i.hxx"

#include "MEDMEM_Mesh_i.hxx"
#include "MEDMEM_Med_i.hxx"
#include "MEDMEM_FieldInt_i.hxx"
#include "MEDMEM_FieldDouble_i.hxx"
#include "MEDMEM_Support_i.hxx"

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
#include <HDFascii.hxx>
#include "SALOMEDS_Tool.hxx"

using namespace MEDMEM;

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
      
      //	    Utilisation de this  deconseillee par Paul ??
      //	    myBuilder->DefineComponentInstance(medfather,POA_Engines::MED_Gen::_this());
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
//         if (!_duringLoad) addInStudy(myStudy) ;

	SALOME_MED::MED_ptr myMedIOR ;
	try 
	{
	  // we create a new MED_i and add in study
	  MED_i * myMedI = new MED_i();
	  myMedIOR = myMedI->_this() ;
// 	  if (!_duringLoad) myMedI->addInStudy(myStudy,myMedIOR) ;
// 	  if (!_duringLoad) myMedI->addInStudy(myStudy,myMedIOR,fileName) ; 	  
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

	BEGIN_OF("Med_Gen_i::readStructFileWithFieldType (const char* fileName,const char* studyName)");

	SCRUTE(fileName);
  	SALOMEDS::Study_var myStudy = studyName2Study(studyName) ;

	if (!_duringLoad) addInStudy(myStudy) ;

	try 
	{
	  // we create a new MED_i and add in study
	  MED_i * myMedI = new MED_i();
	  SALOME_MED::MED_ptr myMedIOR = myMedI->_this() ;
	  if (!_duringLoad) myMedI->addInStudy(myStudy,myMedIOR,fileName) ;	  
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

        endService("Med_Gen_i::readStructFileWithFieldType");
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
        beginService("Med_Gen_i::readMeshInFile");
	SCRUTE(fileName);
  	SALOMEDS::Study_var myStudy = studyName2Study(studyName) ;

// 	if (!_duringLoad) addInStudy(myStudy) ;

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
	try
        {
	  // add the mesh object in study
// 	  if (!_duringLoad) meshi->addInStudy(myStudy,mesh);
	}
        catch (const SALOMEDS::StudyBuilder::LockProtection & lp) {}

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
					       CORBA::Long ordre,
					       CORBA::Long iter)
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
    	THROW_SALOME_CORBA_EXCEPTION("Wrong Study Name", \
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
		//NRI		aName->SetValue("Med");

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
        try
	{
		fieldSupport=(SUPPORT *)myField->getSupport() ;
		ASSERT(fieldSupport != NULL);
		MESH * myMesh=(MESH *)fieldSupport->getMesh();
		ASSERT(myMesh != NULL);
		myMesh->read();
		SCRUTE(myMesh->getName());
		fieldSupport->update();
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
			FIELDINT_i * myFieldIntI = new FIELDINT_i((FIELD<int>*)myField);
        		SALOME_MED::FIELD_ptr myFieldIOR = myFieldIntI->_this();
//         		if (!_duringLoad) myFieldIntI->addInStudy(myStudy,myFieldIOR) ;
			endService("Med_Gen_i::readFieldInFile");
        		return myFieldIOR;
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
			FIELDDOUBLE_i * myFieldDoubleI = new FIELDDOUBLE_i((FIELD<double>*)myField);
        		SALOME_MED::FIELD_ptr myFieldIOR = myFieldDoubleI->_this() ;
//         		if (!_duringLoad) myFieldDoubleI->addInStudy(myStudy,myFieldIOR) ;
			endService("Med_Gen_i::readFieldInFile");
        		return myFieldIOR;
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
SALOMEDS::TMPFile* Med_Gen_i::Save(SALOMEDS::SComponent_ptr theComponent,
				   const char* theURL,
				   bool isMultiFile) {
  const char* LOC = "Med_Gen_i::Save";
  BEGIN_OF(LOC);

  SALOMEDS::TMPFile_var aStreamFile;
  // Get a temporary directory to store a file
  TCollection_AsciiString aTmpDir = (isMultiFile)?TCollection_AsciiString((char*)theURL):SALOMEDS_Tool::GetTmpDir();
  // Create a list to store names of created files
  SALOMEDS::ListOfFileNames_var aSeq = new SALOMEDS::ListOfFileNames;
  TColStd_SequenceOfAsciiString aFileNames;

  CORBA::String_var aSaveStudyName("");
  if (isMultiFile) aSaveStudyName = strdup(SALOMEDS_Tool::GetNameFromPath(theComponent->GetStudy()->URL()));

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
	  TCollection_AsciiString aName(strdup(aSaveStudyName));
	  aName += "_MEDMESH_";
	  aName += myMesh->getName();
	  aName += ".med";
	  MESSAGE("Save mesh with name "<<aName.ToCString());
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
	  ostringstream a,b;
	  a<< myField->getOrderNumber();
	  b<< myField->getIterationNumber();

	  TCollection_AsciiString aName(strdup(aSaveStudyName));
	  aName += "_MEDFIELD_";
	  aName += myField->getName();
	  aName += "_ORDRE_";
	  aName += strdup(a.str().c_str());
	  aName += "_ITER_";
	  aName += strdup(b.str().c_str());
	  aName += ".med";
	  MESSAGE("Save mesh with name "<<aName.ToCString());
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
  aStreamFile = SALOMEDS_Tool::PutFilesToStream(aTmpDir.ToCString(), aSeq.in(), isMultiFile);
  // Remove the created file and tmp directory
  if (!isMultiFile) SALOMEDS_Tool::RemoveTemporaryFiles(aTmpDir.ToCString(), aSeq.in(), true);
  // Return the created byte stream
  return aStreamFile._retn();
  
  END_OF(LOC);
}

SALOMEDS::TMPFile* Med_Gen_i::SaveASCII(SALOMEDS::SComponent_ptr theComponent,
					const char* theURL,
					bool isMultiFile) {
  const char* LOC = "Med_Gen_i::SaveASCII";
  BEGIN_OF(LOC);
  
  SALOMEDS::TMPFile_var aStreamFile;
  // Get a temporary directory to store a file
  TCollection_AsciiString aTmpDir = (isMultiFile)?TCollection_AsciiString((char*)theURL):SALOMEDS_Tool::GetTmpDir();
  // Create a list to store names of created files
  SALOMEDS::ListOfFileNames_var aSeq = new SALOMEDS::ListOfFileNames;
  TColStd_SequenceOfAsciiString aFileNames;
  
  CORBA::String_var aSaveStudyName("");
  if (isMultiFile) aSaveStudyName = strdup(SALOMEDS_Tool::GetNameFromPath(theComponent->GetStudy()->URL()));
  
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
	  TCollection_AsciiString aName(strdup(aSaveStudyName));
	  aName += "_MEDMESH_";
	  aName += myMesh->getName();
	  aName += ".med";
	  MESSAGE("Save mesh with name "<<aName.ToCString());
	  long driverId = myMesh->addDriver(SALOME_MED::MED_DRIVER,(aTmpDir+aName).ToCString(),myMesh->getName());
	  myMesh->write(driverId,"");
	  HDFascii::ConvertFromHDFToASCII(strdup((aTmpDir+aName).ToCString()), true);
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
	  ostringstream a,b;
	  a<< myField->getOrderNumber();
	  b<< myField->getIterationNumber();

	  TCollection_AsciiString aName(strdup(aSaveStudyName));
	  aName += "_MEDFIELD_";
	  aName += myField->getName();
	  aName += "_ORDRE_";
	  aName += strdup(a.str().c_str());
	  aName += "_ITER_";
	  aName += strdup(b.str().c_str());
	  aName += ".med";
	  MESSAGE("Save mesh with name "<<aName.ToCString());
	  long driverId = myField->addDriver(SALOME_MED::MED_DRIVER,(aTmpDir+aName).ToCString(),myField->getName());
	  myField->write(driverId,"");
	  HDFascii::ConvertFromHDFToASCII(strdup((aTmpDir+aName).ToCString()), true);
	  aFileNames.Append(aName);
	}
      }
    }
  }

  int i;
  aSeq->length(aFileNames.Length());
  for(i = aFileNames.Length(); i > 0; i--) aSeq[i-1] = CORBA::string_dup(aFileNames.Value(i).ToCString());
  // Conver a file to the byte stream
  aStreamFile = SALOMEDS_Tool::PutFilesToStream(aTmpDir.ToCString(), aSeq.in(), isMultiFile);
  // Remove the created file and tmp directory
  if (!isMultiFile) SALOMEDS_Tool::RemoveTemporaryFiles(aTmpDir.ToCString(), aSeq.in(), true);
  // Return the created byte stream
  return aStreamFile._retn();
  
  END_OF(LOC);
}

//=============================================================================
/*!
 *  CORBA: Load Mesh objects (called when an existing study is opened)
 */
//=============================================================================

CORBA::Boolean Med_Gen_i::Load(SALOMEDS::SComponent_ptr theComponent,
			       const SALOMEDS::TMPFile& theStream,
			       const char* theURL,
			       bool isMultiFile) {
  const char* LOC = "Med_Gen_i::Load";
  BEGIN_OF(LOC);

  // Get a temporary directory for a file
  TCollection_AsciiString aTmpDir =
    (isMultiFile)?TCollection_AsciiString((char*)theURL):SALOMEDS_Tool::GetTmpDir();
  _saveFileName = CORBA::string_dup(aTmpDir.ToCString());
  SALOMEDS::ListOfFileNames_var aSeq =
    SALOMEDS_Tool::PutStreamToFiles(theStream, aTmpDir.ToCString(), isMultiFile);
  return true;

  END_OF(LOC);
}

CORBA::Boolean Med_Gen_i::LoadASCII(SALOMEDS::SComponent_ptr theComponent,
				    const SALOMEDS::TMPFile& theStream,
				    const char* theURL,
				    bool isMultiFile) {
  return Load(theComponent, theStream, theURL, isMultiFile);
}

//=============================================================================
/*!
 *  CORBA: 
 */
//=============================================================================

void Med_Gen_i::Close(SALOMEDS::SComponent_ptr theComponent)
{
  MESSAGE("Med_Gen_i::Close");
  SALOMEDS::SObject_var aMedMeshFather = theComponent->GetStudy()->FindObject("MEDMESH");
  if (!CORBA::is_nil(aMedMeshFather)) {
    SALOMEDS::ChildIterator_var anIter = theComponent->GetStudy()->NewChildIterator(aMedMeshFather);
    for(; anIter->More(); anIter->Next()) {
      SALOMEDS::SObject_var aSO = anIter->Value();
      SALOMEDS::GenericAttribute_var anAttr;
      if (aSO->FindAttribute(anAttr,"AttributeIOR")) {
	CORBA::Object_var myIOR = _orb->string_to_object(SALOMEDS::AttributeIOR::_narrow(anAttr)->Value());
	SALOME_MED::MESH_var myMesh = SALOME_MED::MESH::_narrow(myIOR);
	// here must call method destroy of myMesh, but it not implented yet
      }
    }
  }
}

//=============================================================================
/*!
 *  CORBA:
 */
//=============================================================================

char* Med_Gen_i::ComponentDataType()
{
  MESSAGE("Med_Gen_i::ComponentDataType");
  return strdup("MED") ; /* What is this type ? */
}
    
//=============================================================================
/*!
 *  CORBA: give a persistent reference of a transient object (for study save) 
 */
//=============================================================================

char* Med_Gen_i::IORToLocalPersistentID(SALOMEDS::SObject_ptr theSObject,
					const char* IORString,
					CORBA::Boolean isMultiFile,
					CORBA::Boolean isASCII) {
  const char * LOC = "Med_Gen_i::IORToLocalPersistentID" ;
  BEGIN_OF(LOC) ;
  SCRUTE(IORString);


  if (string(IORString).size()==0) return "_MED";
  // Well, we know where put object (_saveFilename) and we know object (IORString)
  // cast object :
  CORBA::Object_var myIOR = _orb->string_to_object(IORString);

  // MED
  SALOME_MED::MED_var myMed = SALOME_MED::MED::_narrow(myIOR);
  if (! CORBA::is_nil(myMed)) 
  {
        // nothing to save : Support will be saved inside the mesh
	string str_MedName="_MED Objet Med + /OBJ_MED/";
        return strdup(str_MedName.c_str()) ; 
  }
 
  // MESH
  SALOME_MED::MESH_var myMesh = SALOME_MED::MESH::_narrow(myIOR);
  if (! CORBA::is_nil(myMesh)) 
  {
    CORBA::String_var aName((string("_MEDMESH_")+ myMesh->getName() + ".med").c_str());
    return strdup(aName._retn()) ;
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
    return strdup(("_MED"+str_SupportName).c_str());
  }
    
  SALOME_MED::FIELD_var myField = SALOME_MED::FIELD::_narrow(myIOR);
  if (! CORBA::is_nil(myField)) 
  {
    string str_FieldName;
    ostringstream a,b;
    a<< myField->getOrderNumber();
    b<< myField->getIterationNumber();
    CORBA::String_var aName((string("_MEDFIELD_")+ myField->getName() +
			     string("_ORDRE_")+a.str()+
			     string("_ITER_")+b.str() +
			     ".med").c_str());
    return strdup(aName._retn());
  }

  //THROW_SALOME_CORBA_EXCEPTION("Unable to save IOR",SALOME::BAD_PARAM);
  return "_MED";

  END_OF(LOC) ;
}

//=============================================================================
/*!
 *  CORBA: give a transient reference (when loading an object, opening study)
 */
//=============================================================================

char* Med_Gen_i::LocalPersistentIDToIOR(SALOMEDS::SObject_ptr theSObject,
					const char* aLocalPersistentID,
					CORBA::Boolean isMultiFile,
					CORBA::Boolean isASCII)
     throw(SALOME::SALOME_Exception)
{
  const char * LOC = "Med_Gen_i::LocalPersistentIDToIOR" ;
  BEGIN_OF(LOC) ;
  TCollection_AsciiString aTmpDir(CORBA::string_dup(_saveFileName.c_str()));

  TCollection_AsciiString aSaveStudyName("");
  if (isMultiFile) aSaveStudyName = strdup(SALOMEDS_Tool::GetNameFromPath(theSObject->GetStudy()->URL()));

  if (strcmp(aLocalPersistentID, "Objet Med + /OBJ_MED/") == 0) return strdup(""); // MED

  if (strncmp(aLocalPersistentID, "_MEDMESH_",9) == 0) {// MESH
    MESH * myMesh= new MESH() ;
    int aMeshNameLen = strlen(aLocalPersistentID) - 12;
    char* aMeshName = new char[aMeshNameLen];
    strncpy(aMeshName, &(aLocalPersistentID[9]), aMeshNameLen-1);
    aMeshName[aMeshNameLen-1] = 0;
    myMesh->setName(aMeshName);

    char* aFileName;
    if (isASCII) {
      char* aResultPath = HDFascii::ConvertFromASCIIToHDF((aTmpDir + strdup(aLocalPersistentID)).ToCString());
      aFileName = new char[strlen(aResultPath) + 19];
      sprintf(aFileName, "%shdf_from_ascii.hdf", aResultPath);
      delete(aResultPath);
    } else aFileName = strdup((aTmpDir + aSaveStudyName + strdup(aLocalPersistentID)).ToCString());
    MED_MESH_RDONLY_DRIVER myMeshDriver(aFileName,myMesh);
    try
      {
	myMeshDriver.setMeshName(aMeshName);
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
      }
    MESH_i * meshi = new MESH_i(myMesh);
    //SALOME_MED::MESH_var mesh = SALOME_MED::MESH::_narrow(meshi->_this());
    SALOME_MED::MESH_ptr mesh = meshi->_this();
    SALOMEDS::ListOfFileNames_var aSeq = new SALOMEDS::ListOfFileNames;
    aSeq->length(1);
    aSeq[0]=CORBA::string_dup(aLocalPersistentID);
    if (!isMultiFile) SALOMEDS_Tool::RemoveTemporaryFiles(aTmpDir.ToCString(), aSeq.in(), true);
    if (isASCII) {
      SALOMEDS::ListOfFileNames_var aFilesToRemove = new SALOMEDS::ListOfFileNames;
      aFilesToRemove->length(1);
      aFilesToRemove[0] = strdup(&(aFileName[strlen(SALOMEDS_Tool::GetDirFromPath(aFileName))]));
      SALOMEDS_Tool::RemoveTemporaryFiles(SALOMEDS_Tool::GetDirFromPath(aFileName), aFilesToRemove, true);
    }
    delete(aFileName);
    return(CORBA::string_dup(_orb->object_to_string(mesh)));
  } else if (strncmp(aLocalPersistentID, "_MEDFIELD_",14) == 0) { // FIELD
    return(strdup("")); // not implemented yet
  }

  return strdup("");

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
  SALOMEDS::SComponent_var aFather = theStudy->FindComponent("MED");

  if (aFather->_is_nil()) {
    aFather = aBuilder->NewComponent("MED");
    if (aFather->_is_nil()) return aResultSO;
    anAttr = aBuilder->FindOrCreateAttribute(aFather, "AttributeName");
    aName = SALOMEDS::AttributeName::_narrow(anAttr);
    //NRI    aName->SetValue("MED");

    CORBA::Object_var objVarN = _NS->Resolve("/Kernel/ModulCatalog");
    SALOME_ModuleCatalog::ModuleCatalog_var Catalogue  = SALOME_ModuleCatalog::ModuleCatalog::_narrow(objVarN);
    SALOME_ModuleCatalog::Acomponent_var Comp = Catalogue->GetComponent( "MED" );
    if ( !Comp->_is_nil() ) {
      aName->SetValue( Comp->componentusername() );
    }    
    aBuilder->DefineComponentInstance(aFather, MED_Gen::_this());
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
 *  returns true, if can copy the object
 */
//=============================================================================
CORBA::Boolean Med_Gen_i::CanCopy(SALOMEDS::SObject_ptr theObject) {
  // Try to retrieve known by MED component mesh by given IOR
  SALOMEDS::GenericAttribute_var anAttr;
  if (!theObject->FindAttribute(anAttr, "AttributeIOR")) return false;
  try {
    CORBA::Object_var anObj = _orb->string_to_object(SALOMEDS::AttributeIOR::_narrow(anAttr)->Value());
    SALOME_MED::MESH_var aMesh = SALOME_MED::MESH::_narrow(anObj);
    // If the object is null one it can't be copied: return false
    if (aMesh->_is_nil()) return false;
  } catch(...) {
    return false;
  }
  return true;
}

//=============================================================================
/*!
 *  create copy of the object and put it to the stream
 */
//=============================================================================
SALOMEDS::TMPFile* Med_Gen_i::CopyFrom(SALOMEDS::SObject_ptr theObject, CORBA::Long& theObjectID) {
    // Declare a sequence of the byte to store the copied object
  SALOMEDS::TMPFile_var aStreamFile;

  // Try to get GEOM_Shape object by given SObject
  SALOMEDS::GenericAttribute_var anAttr;
  if (!theObject->FindAttribute(anAttr, "AttributeIOR")) return new SALOMEDS::TMPFile(0);
  CORBA::String_var anIOR = strdup(SALOMEDS::AttributeIOR::_narrow(anAttr)->Value());
  CORBA::Object_var anObj = _orb->string_to_object(anIOR);
  SALOME_MED::MESH_var aMesh = SALOME_MED::MESH::_narrow(anObj);
  if (aMesh->_is_nil()) return new SALOMEDS::TMPFile(0);

  // Get a temporary directory to store a temporary file
  CORBA::String_var aTmpDir = SALOMEDS_Tool::GetTmpDir();
  // Create a list to store names of created files
  SALOMEDS::ListOfFileNames_var aSeq = new SALOMEDS::ListOfFileNames;
  aSeq->length(1);
  aSeq[0] = strdup(aMesh->getName());
  char* aFullName = new char[strlen(aTmpDir)+strlen(aSeq[0])+1];
  strcpy(aFullName, aTmpDir);
  strcpy(aFullName+strlen(aTmpDir), aSeq[0]);
  long driverId = aMesh->addDriver(SALOME_MED::MED_DRIVER,strdup(aFullName) , aMesh->getName());
  aMesh->write(driverId,"");
  delete(aFullName);
  
  aStreamFile = SALOMEDS_Tool::PutFilesToStream(aTmpDir, aSeq.in(), false);
  SALOMEDS_Tool::RemoveTemporaryFiles(aTmpDir, aSeq.in(), true);
  
  // Assign an ID = 1 the the type SALOME_MED::MESH
  theObjectID = 1;
 
  return aStreamFile._retn();
}

//=============================================================================
/*!
 *  returns true, if can copy the object
 */
//=============================================================================
CORBA::Boolean Med_Gen_i::CanPaste(const char* theComponentName, CORBA::Long theObjectID) {
  // The MED component can paste only objects copied by MED component
  // and with the object type = 1 (mesh)
  if (strcmp(theComponentName, ComponentDataType()) != 0 || theObjectID != 1) return false;
  return true;
}

//=============================================================================
/*!
 *  returns true, if can copy the object
 */
//=============================================================================
SALOMEDS::SObject_ptr Med_Gen_i::PasteInto(const SALOMEDS::TMPFile& theStream,
					   CORBA::Long theObjectID,
					   SALOMEDS::SObject_ptr theObject) {
  SALOMEDS::SObject_var aResultSO = SALOMEDS::SObject::_duplicate(theObject);
  if (theStream.length() == 0) return aResultSO._retn();
  
  SALOMEDS::Study_var aStudy = theObject->GetStudy();

  CORBA::String_var aTmpDir = strdup(SALOMEDS_Tool::GetTmpDir());
  SALOMEDS::ListOfFileNames_var aSeq = SALOMEDS_Tool::PutStreamToFiles(theStream, aTmpDir, false);
  CORBA::String_var aMeshName = strdup(aSeq[0]);
  char* aFullName = new char[strlen(aTmpDir)+strlen(aMeshName)+1];
  strcpy(aFullName, aTmpDir);
  strcpy(aFullName+strlen(aTmpDir), aMeshName);

  MESH * myMesh= new MESH() ;
  myMesh->setName((char*)aMeshName);
  MED_MESH_RDONLY_DRIVER myMeshDriver(aFullName, myMesh);
  try {
    myMeshDriver.setMeshName((char*)aMeshName);
    myMeshDriver.open();
  } catch (const exception & ex) {
    MESSAGE("Exception Interceptee : ");
    SCRUTE(ex.what());
    delete(aFullName);
    return aResultSO._retn();
  };
  try {
    myMeshDriver.read();
    ("apres read");
    myMeshDriver.close();
  } catch (const exception & ex) {
    MESSAGE("Exception Interceptee : ");
    SCRUTE(ex.what());
    delete(aFullName);
    return aResultSO._retn();
  };
  // set new mesh name, becouse now there are no possibility to operate meshes with the same names
//    srand((unsigned int)time(NULL));
  int aRND = rand(); //Get a random number to present a name of a copied mesh
  char aCopiedMeshName[20];
  sprintf(aCopiedMeshName,"MESH_COPY_%d",aRND);
  myMesh->setName(aCopiedMeshName);
  MESH_i * meshi = new MESH_i(myMesh);
  SALOME_MED::MESH_ptr mesh = meshi->_this();
  // add the mesh object in study
  meshi->addInStudy(aStudy,mesh);
  // get the IOR attribute of just added mesh
  CORBA::String_var anIORString = _orb->object_to_string(mesh);
  aResultSO = aStudy->FindObjectIOR(anIORString);

  SALOMEDS_Tool::RemoveTemporaryFiles(aTmpDir, aSeq.in(), true);
  delete(aFullName);
  return aResultSO._retn();
}






//=============================================================================
/*! 
 * C factory, accessible with dlsym, after dlopen  
 */
//=============================================================================

extern "C"
{
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
}
