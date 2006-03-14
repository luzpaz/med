//=============================================================================
// File      : MEDMEM_Med_i.cxx
// Project   : SALOME
// Author    : EDF
// Copyright : EDF 2002
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_i.cxx
//=============================================================================

#include <deque>

//#include "MEDMEM_Field.hxx"

#include "MEDMEM_Med_i.hxx"
#include "MEDMEM_Mesh_i.hxx"
#include "MEDMEM_FieldTemplate_i.hxx"
#include "MEDMEM_Support_i.hxx"
#include "MEDMEM_Family_i.hxx"
#include "MEDMEM_Group_i.hxx"
#include "MEDMEM_convert.hxx"

#include "MEDMEM_DriversDef.hxx"
#include "utilities.h"
#include "Utils_CorbaException.hxx"

using namespace std;
using namespace MEDMEM;

//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
MED_i::MED_i():_med((::MED*)NULL)
{
        BEGIN_OF("Default Constructor MED_i");
        END_OF("Default Constructor MED_i");
}
//=============================================================================
/*!
 * methods
 */
//=============================================================================
void MED_i::init(SALOMEDS::Study_ptr myStudy,driverTypes driverType, const string & fileName)
{
	const char * LOC = "MED_i::init(driverTypes, const string &)";
	BEGIN_OF(LOC);

  // we create all IOR from _med
	_med = new ::MED(driverType,fileName);

  // MESHES :
	deque<string> meshesNames = _med->getMeshNames();
	int numberOfMeshes = meshesNames.size();
	for (int i=0; i<numberOfMeshes; i++) 
	{
		::MESH * myMesh = _med->getMesh(meshesNames[i]);
		myMesh->read();
		MESH_i * myMeshI = new MESH_i(myMesh);
		SALOME_MED::MESH_ptr myMeshIOR = myMeshI->_this();
		_meshes[meshesNames[i]] = myMeshIOR;
// 		myMeshI->addInStudy(myStudy,myMeshIOR,fileName);
        }

  // SUPPORTS :
	_med->updateSupport();

  // FAMILIES :
  // we add all families 
	vector<FAMILY*> familyVector;
	vector<FAMILY*>::iterator familyVectorIt;
  // GROUPS :
  // we add all groups
	vector<GROUP*> groupVector;
	vector<GROUP*>::iterator groupVectorIt;
  
	MED_EN::MESH_ENTITIES::const_iterator currentEntity; 
	for (int i=0; i<numberOfMeshes; i++) 
	{
		::MESH * ptrMesh = _med->getMesh(meshesNames[i]);
		for (currentEntity = MED_EN::meshEntities.begin();
		     currentEntity != MED_EN::meshEntities.end(); 
		     currentEntity++) 
		{
// 		     MESSAGE(LOC << ": for entity " << (*currentEntity).first);
		// family :
  		     familyVector = ptrMesh->getFamilies((MED_EN::medEntityMesh)(*currentEntity).first);
		     int nb = familyVector.size();
// 		     MESSAGE(LOC << ": there is(are) " << nb << " family(ies)");
		     for (familyVectorIt = familyVector.begin();
			  familyVectorIt != familyVector.end();
			  familyVectorIt++) 
		     {
			  FAMILY_i * myFamilyI = new FAMILY_i(*familyVectorIt);
			  SALOME_MED::FAMILY_ptr myFamilyIOR = myFamilyI->POA_SALOME_MED::FAMILY::_this();
// 			   myFamilyI->addInStudy(myStudy,myFamilyIOR);
      		     }

	       // group :
      		     groupVector = ptrMesh->getGroups((MED_EN::medEntityMesh)(*currentEntity).first);
		     nb = groupVector.size();
// 		     MESSAGE(LOC << ": there is(are) " << nb << " group(s)");
      		    for (groupVectorIt = groupVector.begin();
			 groupVectorIt != groupVector.end();
			 groupVectorIt++) 
		    {
			 GROUP_i * myGroupI = new GROUP_i(*groupVectorIt);
			 SALOME_MED::GROUP_ptr myGroupIOR = myGroupI->POA_SALOME_MED::GROUP::_this();
// 			 myGroupI->addInStudy(myStudy,myGroupIOR);
      		    }
                }      
	}

	for (int i=0; i<numberOfMeshes; i++) 
	{
	      map<MED_EN::medEntityMesh,::SUPPORT*> mySupports = _med->getSupports(meshesNames[i]);
	      map<MED_EN::medEntityMesh,::SUPPORT*>::const_iterator itSupport;
   	      map<MED_EN::medEntityMesh, SALOME_MED::SUPPORT_ptr> & mySupportsIOR 
				= _supports[meshesNames[i]];
	      for ( itSupport = mySupports.begin(); itSupport != mySupports.end(); itSupport++ ) 
	      {
		    SUPPORT_i * mySupportI = new SUPPORT_i((*itSupport).second);
     		    SALOME_MED::SUPPORT_ptr mySupportIOR = mySupportI->_this();
		    mySupportsIOR[(*itSupport).first] = mySupportIOR;
// 		    mySupportI->addInStudy(myStudy,mySupportIOR);
	      }
        }

  // FIELDS :
	deque<string> fieldsNames = _med->getFieldNames();
	int numberOfFields = fieldsNames.size();
	for (int i=0; i<numberOfFields; i++) 
	{
	     deque<DT_IT_> myIteration = _med->getFieldIteration (fieldsNames[i]);
	     int numberOfIteration = myIteration.size();
	     for (int j=0; j<numberOfIteration; j++) 
             {
	         ::FIELD_ * myField = _med->getField(fieldsNames[i], myIteration[j].dt, myIteration[j].it);
	         string meshName = myField->getSupport()->getMesh()->getName();
	         medEntityMesh myEntity = myField->getSupport()->getEntity();
	         map<string, map<MED_EN::medEntityMesh, SALOME_MED::SUPPORT_ptr> >::const_iterator 
							     itSupportOnMesh = _supports.find(meshName);
	         if ( itSupportOnMesh == _supports.end() )
		      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
					 << "There is no support on mesh named |" 
					 << meshName << "|" ));
      		 const map<MED_EN::medEntityMesh, SALOME_MED::SUPPORT_ptr> & SupportOnMesh 
					= (*itSupportOnMesh).second;
                 map<MED_EN::medEntityMesh,SALOME_MED::SUPPORT_ptr>::const_iterator itSupport 
					= SupportOnMesh.find(myEntity);
                 if (itSupport == SupportOnMesh.end())
		      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
					 << "There is no support on entity "
					 << entity << " in mesh named |" 
					 << meshName << "|"));
                 SALOME_MED::SUPPORT_ptr mySupportIOR = (*itSupport).second;
      		 med_type_champ type = myField->getValueType();
      		 SALOME_MED::FIELD_ptr myFieldIOR;
      		 switch (type) 
		 {
      		         case MED_EN::MED_INT32 : 
			 {
			     ((FIELD<int>*)myField)->read();
			     FIELDTEMPLATE_I<int> *myFieldIntI = new FIELDTEMPLATE_I<int>((FIELD<int>*)myField);
			     myFieldIOR = myFieldIntI->_this();
// 	                     myFieldIntI->addInStudy(myStudy,myFieldIOR);
		             break;
      			 }

      			case MED_EN::MED_REEL64: 
                        {
			     ((FIELD<double>*)myField)->read();
			     FIELDTEMPLATE_I<double> *myFieldDoubleI = new FIELDTEMPLATE_I<double>((FIELD<double>*)myField);
			     myFieldIOR = myFieldDoubleI->_this();
// 			     myFieldDoubleI->addInStudy(myStudy,myFieldIOR);
			     break;
      		        }
      			default: 
			{
			     throw MED_EXCEPTION ( LOCALIZED( STRING(LOC)
					 <<"   * Iteration "<<FieldIteration[j].dt
					 <<", order number "<<FieldIteration[j].it
					 <<" has wrong type : "<<type));
			     break;
      			}
      		}

      		DT_IT_ dtIt;
      		dtIt.dt  = myIteration[j].dt;
      		dtIt.it  = myIteration[j].it;
      		_fields[fieldsNames[i]][dtIt] = myFieldIOR;
             }
  }
  
  END_OF(LOC);
}

//=============================================================================
/*!
 * methods
 */
//=============================================================================
void MED_i::initWithFieldType(SALOMEDS::Study_ptr myStudy,driverTypes driverType, const string & fileName)
{
	const char * LOC = "MED_i::initWithFieldType(driverTypes, const string &)";
	BEGIN_OF(LOC);

  // we create all IOR from _med

	SCRUTE(driverType);
	SCRUTE(fileName)

	_med = new ::MED(driverType,fileName);

	int numberOfMeshes = _med->getNumberOfMeshes();
	SCRUTE(numberOfMeshes);
  // MESHES :
	deque<string> meshesNames = _med->getMeshNames();
	string meshName0 = meshesNames[0];

	SCRUTE(meshName0);

	for (int i=0; i<numberOfMeshes; i++) 
	{
	    string meshNamei = meshesNames[i];
	    SCRUTE(meshNamei);

	    ::MESH * myMesh = _med->getMesh(meshNamei);
    	    myMesh->read();
	    MESH_i * myMeshI = new MESH_i(myMesh);
	    SALOME_MED::MESH_ptr myMeshIOR = myMeshI->_this();
	    _meshes[meshesNames[i]]=myMeshIOR;
	    myMeshI->addInStudy(myStudy,myMeshIOR);
	}

  // SUPPORTS :
	_med->updateSupport();

  // FAMILIES :
  // we add all families 
	vector<FAMILY*> familyVector;
	vector<FAMILY*>::iterator familyVectorIt;
  // GROUPS :
  // we add all groups
	vector<GROUP*> groupVector;
	vector<GROUP*>::iterator groupVectorIt;
  
	SCRUTE(numberOfMeshes);

	MED_EN::MESH_ENTITIES::const_iterator currentEntity; 
	for (int i=0; i<numberOfMeshes; i++) 
	{
		::MESH * ptrMesh = _med->getMesh(meshesNames[i]);

		SCRUTE(ptrMesh);

		for (currentEntity = MED_EN::meshEntities.begin();
		     currentEntity != MED_EN::meshEntities.end(); 
		     currentEntity++) 
		{
		     MESSAGE(LOC << ": for entity " << (*currentEntity).first);
		// family :
  		     familyVector = ptrMesh->getFamilies((MED_EN::medEntityMesh)(*currentEntity).first);
		     int nb = familyVector.size();
		     MESSAGE(LOC << ": there is(are) " << nb << " family(ies)");
		     for (familyVectorIt = familyVector.begin();
			  familyVectorIt != familyVector.end();
			  familyVectorIt++) 
		     {
			  FAMILY_i * myFamilyI = new FAMILY_i(*familyVectorIt);
			  SALOME_MED::FAMILY_ptr myFamilyIOR = myFamilyI->POA_SALOME_MED::FAMILY::_this();
			   myFamilyI->addInStudy(myStudy,myFamilyIOR);
      		     }

	       // group :
      		     groupVector = ptrMesh->getGroups((MED_EN::medEntityMesh)(*currentEntity).first);
		     nb = groupVector.size();
		     MESSAGE(LOC << ": there is(are) " << nb << " group(s)");
      		    for (groupVectorIt = groupVector.begin();
			 groupVectorIt != groupVector.end();
			 groupVectorIt++) 
		    {
			 GROUP_i * myGroupI = new GROUP_i(*groupVectorIt);
			 SALOME_MED::GROUP_ptr myGroupIOR = myGroupI->POA_SALOME_MED::GROUP::_this();
			 myGroupI->addInStudy(myStudy,myGroupIOR);
      		    }
                }      
	}

	for (int i=0; i<numberOfMeshes; i++) 
	{
	    map<MED_EN::medEntityMesh,::SUPPORT*> mySupports = _med->getSupports(meshesNames[i]);
	    map<MED_EN::medEntityMesh,::SUPPORT*>::const_iterator itSupport;
	    map<MED_EN::medEntityMesh,SALOME_MED::SUPPORT_ptr> & 
			mySupportsIOR = _supports[meshesNames[i]];
	    for (itSupport=mySupports.begin(); itSupport!=mySupports.end(); itSupport++ ) 
	    {
		 SUPPORT_i * mySupportI = new SUPPORT_i((*itSupport).second);
	         SALOME_MED::SUPPORT_ptr mySupportIOR = mySupportI->_this();
	         mySupportsIOR[(*itSupport).first]= mySupportIOR;
	         mySupportI->addInStudy(myStudy,mySupportIOR);
	    }
	}




// 	for (int i=0; i<numberOfMeshes; i++) 
// 	  {
// 	    string meshName = meshesNames[i];
// 	    char * supportEntryPath;
// 	    int lenName;
// 	    string supportName;
// 	    SALOMEDS::SObject_var supportEntry;

// 	    supportName = "SupportOnAll_MED_MAILLE";
// 	    lenName = 13 + 15 + strlen(meshName.c_str()) + 1 + strlen(supportName.c_str());
// 	    supportEntryPath = new char[lenName];
// 	    supportEntryPath = strcpy(supportEntryPath,"/Med/MEDMESH/");
// 	    supportEntryPath = strcat(supportEntryPath,"MEDSUPPORTS_OF_");
// 	    supportEntryPath = strcat(supportEntryPath,meshName.c_str());
// 	    supportEntryPath = strcat(supportEntryPath,"/");
// 	    supportEntryPath = strcat(supportEntryPath,supportName.c_str());

// 	    SCRUTE(supportEntryPath);

// 	    cout << "supportEntryPath in Med " << supportEntryPath << " length " << lenName << endl;

// 	    supportEntry = myStudy->FindObjectByPath(supportEntryPath);

// 	    if ( CORBA::is_nil(supportEntry) ) 
// 	      cout << "The reuse in Med is OK " << endl;
// 	    else 
// 	      cout << "the reuse in Med is not OK and there was a problem in the storage in the study" << endl;
// 	    delete [] supportEntryPath;



// 	    supportName = "SupportOnAll_MED_FACE";
// 	    lenName = 13 + 15 + strlen(meshName.c_str()) + 1 + strlen(supportName.c_str());
// 	    supportEntryPath = new char[lenName];
// 	    supportEntryPath = strcpy(supportEntryPath,"/Med/MEDMESH/");
// 	    supportEntryPath = strcat(supportEntryPath,"MEDSUPPORTS_OF_");
// 	    supportEntryPath = strcat(supportEntryPath,meshName.c_str());
// 	    supportEntryPath = strcat(supportEntryPath,"/");
// 	    supportEntryPath = strcat(supportEntryPath,supportName.c_str());

// 	    SCRUTE(supportEntryPath);

// 	    cout << "supportEntryPath in Med " << supportEntryPath << " length " << lenName << endl;

// 	    supportEntry = myStudy->FindObjectByPath(supportEntryPath);

// 	    if ( CORBA::is_nil(supportEntry) ) 
// 	      cout << "The reuse in Med is OK " << endl;
// 	    else 
// 	      cout << "the reuse in Med is not OK and there was a problem in the storage in the study" << endl;
// 	    delete [] supportEntryPath;



// 	    supportName = "SupportOnAll_MED_ARETE";
// 	    lenName = 13 + 15 + strlen(meshName.c_str()) + 1 + strlen(supportName.c_str());
// 	    supportEntryPath = new char[lenName];
// 	    supportEntryPath = strcpy(supportEntryPath,"/Med/MEDMESH/");
// 	    supportEntryPath = strcat(supportEntryPath,"MEDSUPPORTS_OF_");
// 	    supportEntryPath = strcat(supportEntryPath,meshName.c_str());
// 	    supportEntryPath = strcat(supportEntryPath,"/");
// 	    supportEntryPath = strcat(supportEntryPath,supportName.c_str());

// 	    SCRUTE(supportEntryPath);

// 	    cout << "supportEntryPath in Med " << supportEntryPath << " length " << lenName << endl;

// 	    supportEntry = myStudy->FindObjectByPath(supportEntryPath);

// 	    if ( CORBA::is_nil(supportEntry) ) 
// 	      cout << "The reuse in Med is OK " << endl;
// 	    else 
// 	      cout << "the reuse in Med is not OK and there was a problem in the storage in the study" << endl;
// 	    delete [] supportEntryPath;




// 	    supportName = "SupportOnAll_MED_NOEUD";
// 	    lenName = 13 + 15 + strlen(meshName.c_str()) + 1 + strlen(supportName.c_str());
// 	    supportEntryPath = new char[lenName];
// 	    supportEntryPath = strcpy(supportEntryPath,"/Med/MEDMESH/");
// 	    supportEntryPath = strcat(supportEntryPath,"MEDSUPPORTS_OF_");
// 	    supportEntryPath = strcat(supportEntryPath,meshName.c_str());
// 	    supportEntryPath = strcat(supportEntryPath,"/");
// 	    supportEntryPath = strcat(supportEntryPath,supportName.c_str());

// 	    SCRUTE(supportEntryPath);

// 	    cout << "supportEntryPath in Med " << supportEntryPath << " length " << lenName << endl;

// 	    supportEntry = myStudy->FindObjectByPath(supportEntryPath);

// 	    if ( CORBA::is_nil(supportEntry) ) 
// 	      cout << "The reuse in Med is OK " << endl;
// 	    else 
// 	      cout << "the reuse in Med is not OK and there was a problem in the storage in the study" << endl;
// 	    delete [] supportEntryPath;




// 	  }







  // FIELDS :
	deque<string> fieldsNames = _med->getFieldNames();
	int numberOfFields = fieldsNames.size();

	SCRUTE(numberOfFields);

	for (int i=0; i<numberOfFields; i++) 
	  {
	    MESSAGE("The name of the " << i << "^th fields is "<< fieldsNames[i]);

	    deque<DT_IT_> myIteration = _med->getFieldIteration (fieldsNames[i]);
	    int numberOfIteration = myIteration.size();

	    SCRUTE(numberOfIteration);

	    for (int j=0; j<numberOfIteration; j++) 
	      {
		MESSAGE("its " << i << "^th iteration is dt = " << myIteration[j].dt << " it = " << myIteration[j].it);
	      }
	  }

	for (int i=0; i<numberOfFields; i++) 
	  {

	    MESSAGE("dealing with the fields collection " << fieldsNames[i]);

             deque<DT_IT_> myIteration = _med->getFieldIteration (fieldsNames[i]);
             int numberOfIteration = myIteration.size();

	     SCRUTE(numberOfIteration);

             for (int j=0; j<numberOfIteration; j++) 
             {
	       int dt = myIteration[j].dt;
	       int it = myIteration[j].it;

	       MESSAGE("Treatement of " << fieldsNames[i] << " dt = " << dt << " it = " << it);

		  ::FIELD_ * myField = _med->getField(fieldsNames[i],dt,it);

		  string meshName = myField->getSupport()->getMesh()->getName();
		  medEntityMesh myEntity = myField->getSupport()->getEntity();
		  map<string, map<MED_EN::medEntityMesh,SALOME_MED::SUPPORT_ptr> >::const_iterator 
			itSupportOnMesh = _supports.find(meshName);
		  if (itSupportOnMesh == _supports.end() )
		  	throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
					 << "There is no support on mesh named |" 
					 << meshName << "|"));
		  const map<MED_EN::medEntityMesh,SALOME_MED::SUPPORT_ptr> & SupportOnMesh 
				= (*itSupportOnMesh).second;
		  map<MED_EN::medEntityMesh,SALOME_MED::SUPPORT_ptr>::const_iterator itSupport
				= SupportOnMesh.find(myEntity);
		  if (itSupport == SupportOnMesh.end() )
			throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
					 << "There is no support on entity "
					 << entity << " in mesh named |" 
					 << meshName << "|"));
		  SALOME_MED::SUPPORT_ptr mySupportIOR = (*itSupport).second;
		  med_type_champ type = myField->getValueType();

		  DT_IT_ dtIt;
		  dtIt.dt  = dt;
		  dtIt.it  = it;

		  switch (type) 
		  {
		     case MED_EN::MED_INT32: 
		     {
			((FIELD<int>*)myField)->read();
			FIELDTEMPLATE_I<int> *myFieldIntI = new FIELDTEMPLATE_I<int>((FIELD<int>*)myField);
			SALOME_MED::FIELDINT_ptr myFieldIntIOR;
			myFieldIntIOR = myFieldIntI->_this();

			MESSAGE(LOC << " add in study of the field " << fieldsNames[i].c_str() << " dt = " << dtIt.dt << " it = " << dtIt.it);

			myFieldIntI->addInStudy(myStudy,myFieldIntIOR);
			_fields[fieldsNames[i]][dtIt] = myFieldIntIOR;
			break;
  		     }

      		     case MED_EN::MED_REEL64: 
		     {
			((FIELD<double>*)myField)->read();
			FIELDTEMPLATE_I<double> *myFieldDoubleI = new FIELDTEMPLATE_I<double>((FIELD<double>*)myField);
			SALOME_MED::FIELDDOUBLE_ptr myFieldDoubleIOR;
			myFieldDoubleIOR = myFieldDoubleI->_this();

			MESSAGE(LOC << " add in study of the field " << fieldsNames[i].c_str() << " dt = " << dtIt.dt << " it = " << dtIt.it);

			myFieldDoubleI->addInStudy(myStudy,myFieldDoubleIOR);
			_fields[fieldsNames[i]][dtIt] = myFieldDoubleIOR;
			break;
  		     }

      		    default: 
		    {
			throw MED_EXCEPTION ( LOCALIZED( STRING(LOC)
					 <<"   * Iteration "<<FieldIteration[j].dt
					 <<", order number "<<FieldIteration[j].it
					 <<" has wrong type : "<<type));
			break;
  		     }
     		  }
	     }

	     MESSAGE("Here we are i="<< i);
	}
  














// 	for (int i=0; i<numberOfMeshes; i++) 
// 	  {
// 	    string meshName = meshesNames[i];
// 	    char * supportEntryPath;
// 	    int lenName;
// 	    string supportName;
// 	    SALOMEDS::SObject_var supportEntry;

// 	    supportName = "SupportOnAll_MED_MAILLE";
// 	    lenName = 13 + 15 + strlen(meshName.c_str()) + 1 + strlen(supportName.c_str());
// 	    supportEntryPath = new char[lenName];
// 	    supportEntryPath = strcpy(supportEntryPath,"/Med/MEDMESH/");
// 	    supportEntryPath = strcat(supportEntryPath,"MEDSUPPORTS_OF_");
// 	    supportEntryPath = strcat(supportEntryPath,meshName.c_str());
// 	    supportEntryPath = strcat(supportEntryPath,"/");
// 	    supportEntryPath = strcat(supportEntryPath,supportName.c_str());

// 	    SCRUTE(supportEntryPath);

// 	    cout << "supportEntryPath in Med " << supportEntryPath << " length " << lenName << endl;

// 	    supportEntry = myStudy->FindObjectByPath(supportEntryPath);

// 	    if ( CORBA::is_nil(supportEntry) ) 
// 	      cout << "The reuse in Med is OK " << endl;
// 	    else 
// 	      cout << "the reuse in Med is not OK and there was a problem in the storage in the study" << endl;
// 	    delete [] supportEntryPath;



// 	    supportName = "SupportOnAll_MED_FACE";
// 	    lenName = 13 + 15 + strlen(meshName.c_str()) + 1 + strlen(supportName.c_str());
// 	    supportEntryPath = new char[lenName];
// 	    supportEntryPath = strcpy(supportEntryPath,"/Med/MEDMESH/");
// 	    supportEntryPath = strcat(supportEntryPath,"MEDSUPPORTS_OF_");
// 	    supportEntryPath = strcat(supportEntryPath,meshName.c_str());
// 	    supportEntryPath = strcat(supportEntryPath,"/");
// 	    supportEntryPath = strcat(supportEntryPath,supportName.c_str());

// 	    SCRUTE(supportEntryPath);

// 	    cout << "supportEntryPath in Med " << supportEntryPath << " length " << lenName << endl;

// 	    supportEntry = myStudy->FindObjectByPath(supportEntryPath);

// 	    if ( CORBA::is_nil(supportEntry) ) 
// 	      cout << "The reuse in Med is OK " << endl;
// 	    else 
// 	      cout << "the reuse in Med is not OK and there was a problem in the storage in the study" << endl;
// 	    delete [] supportEntryPath;



// 	    supportName = "SupportOnAll_MED_ARETE";
// 	    lenName = 13 + 15 + strlen(meshName.c_str()) + 1 + strlen(supportName.c_str());
// 	    supportEntryPath = new char[lenName];
// 	    supportEntryPath = strcpy(supportEntryPath,"/Med/MEDMESH/");
// 	    supportEntryPath = strcat(supportEntryPath,"MEDSUPPORTS_OF_");
// 	    supportEntryPath = strcat(supportEntryPath,meshName.c_str());
// 	    supportEntryPath = strcat(supportEntryPath,"/");
// 	    supportEntryPath = strcat(supportEntryPath,supportName.c_str());

// 	    SCRUTE(supportEntryPath);

// 	    cout << "supportEntryPath in Med " << supportEntryPath << " length " << lenName << endl;

// 	    supportEntry = myStudy->FindObjectByPath(supportEntryPath);

// 	    if ( CORBA::is_nil(supportEntry) ) 
// 	      cout << "The reuse in Med is OK " << endl;
// 	    else 
// 	      cout << "the reuse in Med is not OK and there was a problem in the storage in the study" << endl;
// 	    delete [] supportEntryPath;




// 	    supportName = "SupportOnAll_MED_NOEUD";
// 	    lenName = 13 + 15 + strlen(meshName.c_str()) + 1 + strlen(supportName.c_str());
// 	    supportEntryPath = new char[lenName];
// 	    supportEntryPath = strcpy(supportEntryPath,"/Med/MEDMESH/");
// 	    supportEntryPath = strcat(supportEntryPath,"MEDSUPPORTS_OF_");
// 	    supportEntryPath = strcat(supportEntryPath,meshName.c_str());
// 	    supportEntryPath = strcat(supportEntryPath,"/");
// 	    supportEntryPath = strcat(supportEntryPath,supportName.c_str());

// 	    SCRUTE(supportEntryPath);

// 	    cout << "supportEntryPath in Med " << supportEntryPath << " length " << lenName << endl;

// 	    supportEntry = myStudy->FindObjectByPath(supportEntryPath);

// 	    if ( CORBA::is_nil(supportEntry) ) 
// 	      cout << "The reuse in Med is OK " << endl;
// 	    else 
// 	      cout << "the reuse in Med is not OK and there was a problem in the storage in the study" << endl;
// 	    delete [] supportEntryPath;




// 	  }















	END_OF(LOC);
}

//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
MED_i::~MED_i()
{
}
//=============================================================================
/*!
 * CORBA: Accessor for Number of meshes
 */
//=============================================================================
CORBA::Long MED_i::getNumberOfMeshes() 
throw (SALOME::SALOME_Exception)
{
        if (_med==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Med object",\
                                             SALOME::INTERNAL_ERROR);
	try
        {
                return _med->getNumberOfMeshes();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get the number of meshes in Med Object");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for Number of fields
 */
//=============================================================================
CORBA::Long MED_i::getNumberOfFields() 
throw (SALOME::SALOME_Exception)
{
        if (_med==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Med object",\
                                             SALOME::INTERNAL_ERROR);
	try
        {
                return _med->getNumberOfFields();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get the number of fields in Med Object");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for meshes names
 */
//=============================================================================
SALOME_MED::string_array * MED_i::getMeshNames()      
throw (SALOME::SALOME_Exception)
{
        if (_med==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Med object",\
                                             SALOME::INTERNAL_ERROR);
        SALOME_MED::string_array_var myseq = new SALOME_MED::string_array;
        try
        {
                int nbMeshes=_med->getNumberOfMeshes();
                myseq->length(nbMeshes);
                string * nameMeshes = new string[nbMeshes];
                _med->getMeshNames(nameMeshes);
                for (int i=0;i<nbMeshes;i++)
                {
                        myseq[i]=CORBA::string_dup(nameMeshes[i].c_str());
                }
		delete [] nameMeshes;
		nameMeshes=NULL;
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get the names of the meshes in Med Object");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
	return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Accessor for fields names
 */
//=============================================================================
SALOME_MED::string_array * MED_i::getFieldNames()     
  throw (SALOME::SALOME_Exception)
{
  if (_med==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Med object",\
                                             SALOME::INTERNAL_ERROR);
  SALOME_MED::string_array_var myseq = new SALOME_MED::string_array;
  try
    {
      deque<string> nameFields = _med->getFieldNames();
      int nbFields = nameFields.size();
      myseq->length(nbFields);

      for (int i=0;i<nbFields;i++)
	{
	  myseq[i]=CORBA::string_dup(nameFields[i].c_str());
	}
    }
  catch (MEDEXCEPTION &ex)
    {
      MESSAGE("Unable to get the names of the fields in Med Object");
      THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
    }
  return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Accessor for a specific mesh
 */
//=============================================================================
SALOME_MED::MESH_ptr MED_i::getMeshByName(const char* meshName) 
throw (SALOME::SALOME_Exception)
{
        if (_med==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Med object",\
                                             SALOME::INTERNAL_ERROR);
        try
        {
                MESH * mesh=_med->getMesh(meshName);
                MESH_i * m1 = new MESH_i(mesh);
		return m1->POA_SALOME_MED::MESH::_this();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get the specified mesh");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }

}
//=============================================================================
/*!
 * CORBA: Accessor for a specific mesh
 */
//=============================================================================
SALOME_MED::MESH_ptr MED_i::getMesh(SALOME_MED::FIELD_ptr fieldPtr) 
throw (SALOME::SALOME_Exception)
{
        if (_med==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Med object",\
                                             SALOME::INTERNAL_ERROR);
        int ind=fieldPtr->getCorbaIndex();
        SCRUTE(ind);

	MESH * mesh;
	SALOME_MED::FIELDINT_var fieldint =SALOME_MED::FIELDINT::_narrow(fieldPtr);
        if (CORBA::is_nil(fieldint))
        {
                MESSAGE("Double");
		SALOME_MED::FIELDDOUBLE_var fielddouble = 
			    SALOME_MED::FIELDDOUBLE::_narrow(fieldPtr);
                ASSERT(!CORBA::is_nil(fielddouble));

                ASSERT(FIELD_i::fieldMap.find(ind)!=FIELD_i::fieldMap.end());

                ::FIELD<double> * fdouble = (::FIELD<double> *)FIELD_i::fieldMap[ind];
                MESH * mesh=_med->getMesh(fdouble);
        }
        else
        {
                MESSAGE("Integer");
                ASSERT(FIELD_i::fieldMap.find(ind)!=FIELD_i::fieldMap.end());

                ::FIELD<int> * fint = (::FIELD<int> *)FIELD_i::fieldMap[ind];
                MESH * mesh=_med->getMesh(fint);
        }
        MESH_i * meshi = new MESH_i(mesh);
	return meshi->POA_SALOME_MED::MESH::_this();

}
//=============================================================================
/*!
 * CORBA: Accessor for Number of iteration of a given field by its name.
 */
//=============================================================================
CORBA::Long MED_i::getFieldNumberOfIteration(const char* fieldName) 
  throw (SALOME::SALOME_Exception)
{
  if (_med==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Med object",\
				 SALOME::INTERNAL_ERROR);

  try
    {
      deque<DT_IT_> fieldIteration = _med->getFieldIteration(string(fieldName));
      return fieldIteration.size();
    }
  catch (MEDEXCEPTION &ex)
    {
      MESSAGE("Unable to get the number of iteration of the field " << fieldName << " in Med Object");
      THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
    }
}
//=============================================================================
/*!
 * CORBA: Accessor for the Corba equivalent DT_IT_ (see MEDMEM direcrtory)
 * of a given field by its name and its number (sequence of two long integers).
 */
//=============================================================================
SALOME_MED::long_array * MED_i::getFieldIteration(const char* fieldName,CORBA::Long i) 
  throw (SALOME::SALOME_Exception)
{
  if (_med==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Med object",\
				 SALOME::INTERNAL_ERROR);

  SALOME_MED::long_array_var myseq = new SALOME_MED::long_array;
  try
    {
      deque<DT_IT_> fieldIteration = _med->getFieldIteration(string(fieldName));
      int numberOfIteration = fieldIteration.size();

      if (i < 0 || i >=numberOfIteration)
	THROW_SALOME_CORBA_EXCEPTION("The integer i should be geater or equal to 0 and lesser then numberOfIteration", \
				     SALOME::INTERNAL_ERROR);

      myseq->length(2);
      myseq[0] = fieldIteration[i].dt;
      myseq[1] = fieldIteration[i].it;
      return myseq._retn();
    }
  catch (MEDEXCEPTION &ex)
    {
      MESSAGE("Unable to get the sequence of DT_IT of the given field");
      THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
    }
}
//=============================================================================
/*!
 * CORBA: Accessor for the list of Corba equivalent DT_IT_ (see MEDMEM direcrtory)
 * of a given field by its name (sequence of two by iteration number long integers).
 */
//=============================================================================
SALOME_MED::long_array * MED_i::getFieldIterations(const char* fieldName) 
  throw (SALOME::SALOME_Exception)
{
  if (_med==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Med object",\
				 SALOME::INTERNAL_ERROR);

  SALOME_MED::long_array_var myseq = new SALOME_MED::long_array;
  try
    {
      deque<DT_IT_> fieldIteration = _med->getFieldIteration(string(fieldName));
      int numberOfIteration = fieldIteration.size();
      int size = 2*numberOfIteration;

      myseq->length(size);

      for (int i=0; i<numberOfIteration; i++)
	{
	  int index = 2*i;
	  myseq[index] = fieldIteration[i].dt;
	  index = index+1;
	  myseq[index] = fieldIteration[i].it;
	}

      return myseq._retn();
    }
  catch (MEDEXCEPTION &ex)
    {
      MESSAGE("Unable to get the sequence of DT_IT of the given field");
      THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
    }
}
//=============================================================================
/*!
 * CORBA: Accessor for a specific field
 */
//=============================================================================
SALOME_MED::FIELD_ptr MED_i::getField(const char* fieldName, 
				      CORBA::Long pasTemps, 
				      CORBA::Long numOrdre ) 
throw (SALOME::SALOME_Exception)
{
	const char * LOC="MED_i::getField(const char*,CORBA::Long,CORBA::Long) ";
	BEGIN_OF(LOC);

	DT_IT_ dtIt;

	dtIt.dt= (int)pasTemps;
	dtIt.it= (int)numOrdre;

	map<string,MAP_IOR_DT_IT_>::const_iterator itFields = _fields.find(fieldName);

	if ( itFields == _fields.end() ) 
    		THROW_SALOME_CORBA_EXCEPTION("Field not found !", SALOME::INTERNAL_ERROR);
  
	const MAP_IOR_DT_IT_ & map_dtIt = (*itFields).second;
	MAP_IOR_DT_IT_::const_iterator itMap_dtIt =  map_dtIt.find(dtIt);
  
	if ( itMap_dtIt == map_dtIt.end() )
		THROW_SALOME_CORBA_EXCEPTION("Iteration not found !", SALOME::INTERNAL_ERROR);
  
	END_OF(LOC);
	return (*itMap_dtIt).second;

}
//=============================================================================
/*!
 * CORBA: Accessor for a specific field
 */
//=============================================================================
CORBA::Long MED_i::addDriver(SALOME_MED::medDriverTypes driverType, const char* fileName) 
throw (SALOME::SALOME_Exception)
{
        if (_med==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Med object",\
                                             SALOME::INTERNAL_ERROR);
        try
        {
		int driv=_med->addDriver(convertIdlDriverToMedDriver(driverType),
					fileName);
		return driv;
	}
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to add a driver to the med object");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
	
}
//=============================================================================
/*!
 * CORBA: relase the driver (mandatory)
 */
//=============================================================================
void MED_i::rmDriver(CORBA::Long i) 
throw (SALOME::SALOME_Exception)
{
        if (_med==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Med object",\
                                             SALOME::INTERNAL_ERROR);
        try
        {
		_med->rmDriver(i);
	}
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to unlink the driver from the med object");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: read med objects relations
 */
//=============================================================================
void MED_i::readFileStruct(CORBA::Long i) 
throw (SALOME::SALOME_Exception)
{
        if (_med==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Med object",\
                                             SALOME::INTERNAL_ERROR);
        try
        {
		_med->readFileStruct(i);
	}
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to read the structure of this file ");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: write specifics objects f
 */
//=============================================================================
void MED_i::writeFrom(CORBA::Long i) 
throw (SALOME::SALOME_Exception)
{
        if (_med==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Med object",\
                                             SALOME::INTERNAL_ERROR);
        try
        {
		_med->writeFrom(i);
	}
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to write this driver ");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: write objects
 */
//=============================================================================
void MED_i::write(CORBA::Long i) 
throw (SALOME::SALOME_Exception)
{
        if (_med==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Med object",\
                                             SALOME::INTERNAL_ERROR);
        try
        {
		_med->write(i);
	}
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to write this file ");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Add Mesh in Med objects
 */
//=============================================================================
void MED_i::addMesh(SALOME_MED::MESH_ptr ptrMesh) 
throw (SALOME::SALOME_Exception)
{
        if (_med==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Med object",\
                                             SALOME::INTERNAL_ERROR);
        try
        {
        	int ind=ptrMesh->getCorbaIndex();
        	SCRUTE(ind);
       		ASSERT(MESH_i::meshMap.find(ind) !=MESH_i::meshMap.end());
                MESH * mesh= MESH_i::meshMap[ind];
		_med->addMesh(mesh);
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to link this mesh to this med object");
		THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Add field in Med objects
 */
//=============================================================================
void MED_i::addField(SALOME_MED::FIELD_ptr ptrField) 
throw (SALOME::SALOME_Exception)
{
        if (_med==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Med object",\
                                             SALOME::INTERNAL_ERROR);
        int ind=ptrField->getCorbaIndex();
        SCRUTE(ind);

	SALOME_MED::FIELDINT_var fieldint =SALOME_MED::FIELDINT::_narrow(ptrField);
        if (CORBA::is_nil(fieldint))
        {
                MESSAGE("Double");
		SALOME_MED::FIELDDOUBLE_var fielddouble = 
			    SALOME_MED::FIELDDOUBLE::_narrow(ptrField);
                ASSERT(!CORBA::is_nil(fielddouble));

                ASSERT(FIELD_i::fieldMap.find(ind)!=FIELD_i::fieldMap.end());

                ::FIELD<double> * fdouble = (::FIELD<double> *)FIELD_i::fieldMap[ind];
		// A modifier
                //_med->addField(fdouble);
        }
        else
        {
                MESSAGE("Integer");
                ASSERT(FIELD_i::fieldMap.find(ind)!=FIELD_i::fieldMap.end());
                ::FIELD<int> * fint = (::FIELD<int> *)FIELD_i::fieldMap[ind];
                //_med->addField(fint);
        }
}
//=============================================================================
/*!
 * CORBA: Add Med object in Study 
 */
//=============================================================================
void MED_i::addInStudy(SALOMEDS::Study_ptr myStudy, SALOME_MED::MED_ptr myIor) 
throw (SALOME::SALOME_Exception,SALOMEDS::StudyBuilder::LockProtection)
{
	BEGIN_OF("MED_Mesh_i::addInStudy");
        if ( _medId != "" )
        {
                MESSAGE("Med already in Study");
                    THROW_SALOME_CORBA_EXCEPTION("Med already in Study", \
                                 SALOME::BAD_PARAM);
        };

        SALOMEDS::StudyBuilder_var     myBuilder = myStudy->NewBuilder();
        myBuilder->NewCommand();
        SALOMEDS::GenericAttribute_var anAttr;
        SALOMEDS::AttributeName_var    aName;
        SALOMEDS::AttributeIOR_var     aIOR;

        // Create SComponent labelled 'MED' if it doesn't already exit
        SALOMEDS::SComponent_var medfather = myStudy->FindComponent("MED");
        if ( CORBA::is_nil(medfather) )
        {
	  THROW_SALOME_CORBA_EXCEPTION("Component Med not found",
				       SALOME::BAD_PARAM);
        };

        MESSAGE("Add a MED Object under Med");
        SALOMEDS::SObject_var newObj = myBuilder->NewObject(medfather);

        ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance();
        ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting());
        CORBA::ORB_var &orb = init(0,0);
        string iorStr = orb->object_to_string(myIor);
        anAttr = myBuilder->FindOrCreateAttribute(newObj, "AttributeIOR");
        aIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
        aIOR->SetValue(iorStr.c_str());
        anAttr = myBuilder->FindOrCreateAttribute(newObj, "AttributeName");
        aName = SALOMEDS::AttributeName::_narrow(anAttr);
        aName->SetValue("Objet MED");
        _medId = newObj->GetID();
        myBuilder->CommitCommand();

	// register the Corba pointer: increase the referrence count
   	MESSAGE("Registering of the Corba Med pointer");
	Register();

        END_OF("Med_i::addInStudy(SALOMEDS::Study_ptr myStudy)");
}

//=============================================================================
/*!
 * CORBA: Add Med object in Study with a name medObjName
 */
//=============================================================================
void MED_i::addInStudy(SALOMEDS::Study_ptr myStudy, SALOME_MED::MED_ptr myIor,
		       const char * fileName) 
  throw (SALOME::SALOME_Exception,SALOMEDS::StudyBuilder::LockProtection)
{
	BEGIN_OF("MED_i::addInStudy(myStudy, myIor, fileName)");
        if ( _medId != "" )
        {
                MESSAGE("Med already in Study");
                    THROW_SALOME_CORBA_EXCEPTION("Med already in Study", \
                                 SALOME::BAD_PARAM);
        };

        SALOMEDS::StudyBuilder_var     myBuilder = myStudy->NewBuilder();
        myBuilder->NewCommand();
        SALOMEDS::GenericAttribute_var anAttr;
        SALOMEDS::AttributeName_var    aName;
        SALOMEDS::AttributeIOR_var     aIOR;

        // Create SComponent labelled 'MED' if it doesn't already exit
        SALOMEDS::SComponent_var medfather = myStudy->FindComponent("MED");
        if ( CORBA::is_nil(medfather) )
        {
	  THROW_SALOME_CORBA_EXCEPTION("Component Med not found",
				       SALOME::BAD_PARAM);
        };

        MESSAGE("Add a MED Object under Med");

	char * medObjName;
	string::size_type pos1=string(fileName).rfind('/');
	string::size_type lenFileName = strlen(fileName);
	string fileNameShort = string(fileName,pos1+1,lenFileName-pos1-1);

	SCRUTE(fileNameShort);

	int lenName = 21 + strlen(fileNameShort.c_str()) + 1;
	medObjName = new char[lenName];
	medObjName = strcpy(medObjName,"MED_OBJECT_FROM_FILE_");
	medObjName = strcat(medObjName,fileNameShort.c_str());

        SALOMEDS::SObject_var newObj = myBuilder->NewObject(medfather);

        ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance();
        ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting());
        CORBA::ORB_var &orb = init(0,0);
        string iorStr = orb->object_to_string(myIor);
        anAttr = myBuilder->FindOrCreateAttribute(newObj, "AttributeIOR");
        aIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
        aIOR->SetValue(iorStr.c_str());
        anAttr = myBuilder->FindOrCreateAttribute(newObj, "AttributeName");
        aName = SALOMEDS::AttributeName::_narrow(anAttr);
        aName->SetValue(medObjName);
        _medId = newObj->GetID();
        myBuilder->CommitCommand();

	delete [] medObjName;

// 	char * medObjName1;
// 	lenName = 26 + strlen(fileNameShort.c_str());
// 	medObjName1 = new char[lenName];
// 	medObjName1 = strcpy(medObjName1,"/MED/MED_OBJECT_FROM_FILE_");
// 	medObjName1 = strcat(medObjName1,fileNameShort.c_str());

// 	SALOMEDS::SObject_var medEntry = myStudy->FindObjectByPath(medObjName1);
// 	if (!CORBA::is_nil(medEntry))
// 	  {
// 	    MESSAGE("MED_Mesh_i::addInStudy(myStudy, myIor, fileName) : The reuse in Med of Object MED from Study is OK");
// 	  }
// 	else
// 	  {
// 	    MESSAGE("MED_Mesh_i::addInStudy(myStudy, myIor, fileName) : the reuse in Med of Object MED is not OK and there was a problem in the storage in the study");
// 	  }

// 	delete [] medObjName1;

	// register the Corba pointer: increase the referrence count
   	MESSAGE("Registering of the Corba Med pointer");
	Register();

        END_OF("Med_i::addInStudy(myStudy, myIor, medObjName)");
}
