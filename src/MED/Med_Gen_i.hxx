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
//  File   : Med_Gen_i.hxx
//  Author : Paul RASCLE, EDF
//  Module : MED
//  $Header$

#ifndef _MED_GEN_I_HXX_
#define _MED_GEN_I_HXX_

#include <SALOMEconfig.h>
#include <map>
#include CORBA_SERVER_HEADER(MED_Gen)
#include CORBA_SERVER_HEADER(MED)
#include CORBA_SERVER_HEADER(SALOME_ModuleCatalog)

#include "SALOME_Component_i.hxx"
#include "SALOME_NamingService.hxx"

#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

class Med_Gen_i:
  public POA_SALOME_MED::MED_Gen,
  public Engines_Component_i 
{
private:
  SALOMEDS::Study_var studyName2Study(const char* studyName)
    throw(SALOME::SALOME_Exception);
  void addInStudy(SALOMEDS::Study_var myStudy)
    throw(SALOME::SALOME_Exception);

public:
  Med_Gen_i();
  Med_Gen_i(CORBA::ORB_ptr orb,
	    PortableServer::POA_ptr poa,
	    PortableServer::ObjectId * contId, 
	    const char *instanceName, 
	    const char *interfaceName);
  virtual ~Med_Gen_i();

  //
  SALOME_MED::MESH_ptr readMeshInFile(const char* fileName,
				      const char* studyName,
				      const char* meshName)
  throw(SALOME::SALOME_Exception);

  SALOME_MED::FIELD_ptr readFieldInFile(const char* fileName,
				      const char* studyName,
				      const char* fieldName,
				      CORBA::Long ordre,
				      CORBA::Long iter)
  throw(SALOME::SALOME_Exception);

  SALOME_MED::MED_ptr   readStructFile(const char* fileName,
				      const char* studyName)
  throw(SALOME::SALOME_Exception);

  void    readStructFileWithFieldType(const char* fileName,
				      const char* studyName)
  throw(SALOME::SALOME_Exception);

  // inherited methods from SALOMEDS::Driver

//    void Save(const char *IORSComponent, const char *aUrlOfFile);
//    void Load(const char *IORSComponent, const char *aUrlOfFile); 
  SALOMEDS::TMPFile* Save(SALOMEDS::SComponent_ptr theComponent,
			  const char* theURL,
			  bool isMultiFile);
  SALOMEDS::TMPFile* SaveASCII(SALOMEDS::SComponent_ptr theComponent,
			       const char* theURL,
			       bool isMultiFile);

  CORBA::Boolean Load(SALOMEDS::SComponent_ptr theComponent,
		      const SALOMEDS::TMPFile& theStream,
		      const char* theURL,
		      bool isMultiFile);
  CORBA::Boolean LoadASCII(SALOMEDS::SComponent_ptr theComponent,
			   const SALOMEDS::TMPFile& theStream,
			   const char* theURL,
			   bool isMultiFile);

  void Close(SALOMEDS::SComponent_ptr theComponent);
  char* ComponentDataType();
    
  char* IORToLocalPersistentID(SALOMEDS::SObject_ptr theSObject,
			       const char* IORString,
			       CORBA::Boolean isMultiFile,
			       CORBA::Boolean isASCII);
  char* LocalPersistentIDToIOR(SALOMEDS::SObject_ptr theSObject,
			       const char* aLocalPersistentID,
			       CORBA::Boolean isMultiFile,
			       CORBA::Boolean isASCII)
    throw(SALOME::SALOME_Exception);

  bool CanPublishInStudy(CORBA::Object_ptr theIOR);

  SALOMEDS::SObject_ptr PublishInStudy(SALOMEDS::Study_ptr theStudy,
				       SALOMEDS::SObject_ptr theSObject,
				       CORBA::Object_ptr theObject,
				       const char* theName) throw (SALOME::SALOME_Exception) ;

  CORBA::Boolean CanCopy(SALOMEDS::SObject_ptr theObject);
  SALOMEDS::TMPFile* CopyFrom(SALOMEDS::SObject_ptr theObject, CORBA::Long& theObjectID);
  CORBA::Boolean CanPaste(const char* theComponentName, CORBA::Long theObjectID);
  SALOMEDS::SObject_ptr PasteInto(const SALOMEDS::TMPFile& theStream,
				  CORBA::Long theObjectID,
				  SALOMEDS::SObject_ptr theObject);
  
  private :
  static map <string, string>_MedCorbaObj;
  static string _myFileName;
  static string _saveFileName;

  private:
  bool   _duringLoad;
  int    _myStudyID;
  string _myStudyName;
  
  int myCounter;
  SALOME_NamingService *_NS;


};

#endif
