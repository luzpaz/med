//=============================================================================
// File      : Med_i.cxx
// Project   : SALOME
// Author    : EDF
// Copyright : EDF 2002
// $Header: /export/home/CVS/SALOME_ROOT/MED/src/MedMem/Med_i.cxx
//=============================================================================

#include <deque>

//#include "MEDMEM_Field.hxx"

#include "Med_i.hxx"
#include "Mesh_i.hxx"
#include "FieldDouble_i.hxx"
#include "FieldInt_i.hxx"
#include "Support_i.hxx"
#include "Family_i.hxx"
#include "Group_i.hxx"

#include "MEDMEM_DriversDef.hxx"
#include "convert.hxx"
#include "utilities.h"
#include "Utils_CorbaException.hxx"

//=============================================================================
/*!
 * Default constructor
 */
//=================================POA_SALOME_MED::FAMILY::_============================================
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
  const char * LOC = "MED_i::init(driverTypes, const string &)" ;
  BEGIN_OF(LOC);

  _med = new ::MED(driverType,fileName) ;
  // we create all IOR from _med

  // MESH :
  deque<string> meshesNames = _med->getMeshNames() ;
  int numberOfMeshes = meshesNames.size() ;
  for (int i=0; i<numberOfMeshes; i++) {
    ::MESH * myMesh = _med->getMesh(meshesNames[i]) ;
    myMesh->read() ;
    MESH_i * myMeshI = new MESH_i(myMesh) ;
    SALOME_MED::MESH_ptr myMeshIOR = myMeshI->_this() ;
    _meshes[meshesNames[i]]=myMeshIOR ;
    myMeshI->addInStudy(myStudy,myMeshIOR,fileName) ;
  }

  // SUPPORT :
  _med->updateSupport() ;

  // we add all group and family in study :
  // we add all families 
  vector<FAMILY*> familyVector ;
  vector<FAMILY*>::iterator familyVectorIt ;
  // we add all groups
  vector<GROUP*> groupVector ;
  vector<GROUP*>::iterator groupVectorIt ;
  
  MED_FR::MESH_ENTITIES::const_iterator currentEntity; 
  for (int i=0; i<numberOfMeshes; i++) {
    ::MESH * ptrMesh = _med->getMesh(meshesNames[i]) ;
    for (currentEntity=MED_FR::meshEntities.begin();currentEntity != MED_FR::meshEntities.end(); currentEntity++) {
      // family :
      familyVector = ptrMesh->getFamilies((MED_EN::medEntityMesh)(*currentEntity).first) ;
      for(familyVectorIt=familyVector.begin();familyVectorIt!=familyVector.end();familyVectorIt++) {
	FAMILY_i * myFamilyI = new FAMILY_i(*familyVectorIt);
	SALOME_MED::FAMILY_ptr myFamilyIOR = myFamilyI->POA_SALOME_MED::FAMILY::_this() ;
	myFamilyI->addInStudy(myStudy,myFamilyIOR) ;
      }
      // group :
      groupVector = ptrMesh->getGroups((MED_EN::medEntityMesh)(*currentEntity).first) ;
      for(groupVectorIt=groupVector.begin();groupVectorIt!=groupVector.end();groupVectorIt++) {
	GROUP_i * myGroupI = new GROUP_i(*groupVectorIt);
	SALOME_MED::GROUP_ptr myGroupIOR = myGroupI->POA_SALOME_MED::GROUP::_this() ;
	myGroupI->addInStudy(myStudy,myGroupIOR) ;
      }
    }      
  }

  for (int i=0; i<numberOfMeshes; i++) {
    map<MED_FR::med_entite_maillage,::SUPPORT*> mySupports = _med->getSupports(meshesNames[i]) ;
    map<MED_FR::med_entite_maillage,::SUPPORT*>::const_iterator itSupport ;
    map<MED_FR::med_entite_maillage,SALOME_MED::SUPPORT_ptr> & mySupportsIOR = _supports[meshesNames[i]] ;
    for ( itSupport=mySupports.begin(); itSupport!=mySupports.end(); itSupport++ ) {
      SUPPORT_i * mySupportI = new SUPPORT_i((*itSupport).second);
      SALOME_MED::SUPPORT_ptr mySupportIOR = mySupportI->_this() ;
      mySupportsIOR[(*itSupport).first]= mySupportIOR ;
      mySupportI->addInStudy(myStudy,mySupportIOR) ;
    }
  }

  // FIELD :
  deque<string> fieldsNames = _med->getFieldNames() ;
  int numberOfFields = fieldsNames.size() ;
  for (int i=0; i<numberOfFields; i++) {
    deque<DT_IT_> myIteration = _med->getFieldIteration (fieldsNames[i]) ;
    int numberOfIteration = myIteration.size() ;
    for (int j=0; j<numberOfIteration; j++) {
      ::FIELD_ * myField = _med->getField(fieldsNames[i],myIteration[j].dt,myIteration[j].it) ;
      string meshName = myField->getSupport()->getMesh()->getName() ;
      medEntityMesh myEntity = myField->getSupport()->getEntity() ;
      map<string, map<MED_FR::med_entite_maillage,SALOME_MED::SUPPORT_ptr> >::const_iterator itSupportOnMesh = _supports.find(meshName) ;
      if ( itSupportOnMesh == _supports.end() )
	throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
					 << "There is no support on mesh named |" 
					 << meshName << "|"
					 )
			      );
      const map<MED_FR::med_entite_maillage,SALOME_MED::SUPPORT_ptr> & SupportOnMesh = (*itSupportOnMesh).second ;
      map<MED_FR::med_entite_maillage,SALOME_MED::SUPPORT_ptr>::const_iterator itSupport = SupportOnMesh.find((MED_FR::med_entite_maillage)myEntity) ;
      if (itSupport == SupportOnMesh.end() )
	throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
					 << "There is no support on entity "
					 << entity << " in mesh named |" 
					 << meshName << "|"
					 )
			      );
      SALOME_MED::SUPPORT_ptr mySupportIOR = (*itSupport).second ;
      med_type_champ type = myField->getValueType() ;
      SALOME_MED::FIELD_ptr myFieldIOR ;
      switch (type) {
      case MED_FR::MED_INT32: {
	((FIELD<int>*)myField)->read() ;
	FIELDINT_i * myFieldIntI = new FIELDINT_i(mySupportIOR,(FIELD<int>*)myField);
	//SALOME_MED::FIELDINT_ptr myFieldIntIOR = myFieldIntI->_this() ;
	POA_SALOME_MED::FIELD_tie<FIELD_i> * myFieldTie = new POA_SALOME_MED::FIELD_tie<FIELD_i>(myFieldIntI) ;
	myFieldIOR = myFieldTie->_this() ;
	myFieldIntI->addInStudy(myStudy,myFieldIOR) ;

	break ;
      }
      //	else
      case MED_FR::MED_REEL64: {
	((FIELD<double>*)myField)->read() ;
	FIELDDOUBLE_i * myFieldDoubleI = new FIELDDOUBLE_i(mySupportIOR,(FIELD<double>*)myField);
	//SALOME_MED::FIELDDOUBLE_ptr myFieldDoubleIOR = myFieldDoubleI->_this() ;
	POA_SALOME_MED::FIELD_tie<FIELD_i> * myFieldTie= new POA_SALOME_MED::FIELD_tie<FIELD_i>(myFieldDoubleI) ;
	myFieldIOR = myFieldTie->_this() ;
	myFieldDoubleI->addInStudy(myStudy,myFieldIOR) ;
	break ;
      }
      default: {
	throw MED_EXCEPTION ( LOCALIZED( STRING(LOC)
					 <<"   * Iteration "<<FieldIteration[j].dt
					 <<", order number "<<FieldIteration[j].it
					 <<" has wrond type : "<<type)
			      );
	break ;
      }
      }
      //      SALOME_MED::FIELD_ptr myFieldIOR = myFieldI->POA_SALOME_MED::FIELD::_this() ;
      DT_IT_ dtIt;
      dtIt.dt  = myIteration[j].dt ;
      dtIt.it  = myIteration[j].it ;
      _fields[fieldsNames[i]][dtIt] = myFieldIOR ;
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
  const char * LOC = "MED_i::initWithFieldType(driverTypes, const string &)" ;
  BEGIN_OF(LOC);

  _med = new ::MED(driverType,fileName) ;
  // we create all IOR from _med

  // MESH :
  deque<string> meshesNames = _med->getMeshNames() ;
  int numberOfMeshes = meshesNames.size() ;
  for (int i=0; i<numberOfMeshes; i++) {
    ::MESH * myMesh = _med->getMesh(meshesNames[i]) ;
    myMesh->read() ;
    MESH_i * myMeshI = new MESH_i(myMesh) ;
    SALOME_MED::MESH_ptr myMeshIOR = myMeshI->_this() ;
    //POA_SALOME_MED::MESH_tie<MESH_i> * myMeshTie = new POA_SALOME_MED::MESH_tie<MESH_i>(myMeshI) ;
    //SALOME_MED::MESH_ptr myMeshIOR = myMeshTie->_this() ;
    _meshes[meshesNames[i]]=myMeshIOR ;
    myMeshI->addInStudy(myStudy,myMeshIOR) ;
  }

  // SUPPORT :
  _med->updateSupport() ;
  for (int i=0; i<numberOfMeshes; i++) {
    map<MED_FR::med_entite_maillage,::SUPPORT*> mySupports = _med->getSupports(meshesNames[i]) ;
    map<MED_FR::med_entite_maillage,::SUPPORT*>::const_iterator itSupport ;
    map<MED_FR::med_entite_maillage,SALOME_MED::SUPPORT_ptr> & mySupportsIOR = _supports[meshesNames[i]] ;
    for ( itSupport=mySupports.begin(); itSupport!=mySupports.end(); itSupport++ ) {
      SUPPORT_i * mySupportI = new SUPPORT_i((*itSupport).second);
      SALOME_MED::SUPPORT_ptr mySupportIOR = mySupportI->_this() ;
      //POA_SALOME_MED::SUPPORT_tie<SUPPORT_i> * mySupportTie = new POA_SALOME_MED::SUPPORT_tie<SUPPORT_i>(mySupportI) ;
      //SALOME_MED::SUPPORT_ptr mySupportIOR = mySupportTie->_this() ;
      mySupportsIOR[(*itSupport).first]= mySupportIOR ;
      mySupportI->addInStudy(myStudy,mySupportIOR) ;
    }
  }

  // FIELD :
  deque<string> fieldsNames = _med->getFieldNames() ;
  int numberOfFields = fieldsNames.size() ;
  for (int i=0; i<numberOfFields; i++) {
    deque<DT_IT_> myIteration = _med->getFieldIteration (fieldsNames[i]) ;
    int numberOfIteration = myIteration.size() ;
    for (int j=0; j<numberOfIteration; j++) {
      ::FIELD_ * myField = _med->getField(fieldsNames[i],myIteration[j].dt,myIteration[j].it) ;
      string meshName = myField->getSupport()->getMesh()->getName() ;
      medEntityMesh myEntity = myField->getSupport()->getEntity() ;
      map<string, map<MED_FR::med_entite_maillage,SALOME_MED::SUPPORT_ptr> >::const_iterator itSupportOnMesh = _supports.find(meshName) ;
      if ( itSupportOnMesh == _supports.end() )
	throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
					 << "There is no support on mesh named |" 
					 << meshName << "|"
					 )
			      );
      const map<MED_FR::med_entite_maillage,SALOME_MED::SUPPORT_ptr> & SupportOnMesh = (*itSupportOnMesh).second ;
      map<MED_FR::med_entite_maillage,SALOME_MED::SUPPORT_ptr>::const_iterator itSupport = SupportOnMesh.find((MED_FR::med_entite_maillage)myEntity) ;
      if (itSupport == SupportOnMesh.end() )
	throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
					 << "There is no support on entity "
					 << entity << " in mesh named |" 
					 << meshName << "|"
					 )
			      );
      SALOME_MED::SUPPORT_ptr mySupportIOR = (*itSupport).second ;
      med_type_champ type = myField->getValueType() ;
      //      SALOME_MED::FIELD_ptr myFieldIOR ;

      DT_IT_ dtIt;
      dtIt.dt  = myIteration[j].dt ;
      dtIt.it  = myIteration[j].it ;

      switch (type) {
      case MED_FR::MED_INT32: {
	((FIELD<int>*)myField)->read() ;
	FIELDINT_i * myFieldIntI = new FIELDINT_i(mySupportIOR,(FIELD<int>*)myField);
	//SALOME_MED::FIELDINT_ptr myFieldIntIOR = myFieldIntI->_this() ;
	//	POA_SALOME_MED::FIELD_tie<FIELD_i> * myFieldTie = new POA_SALOME_MED::FIELD_tie<FIELD_i>(myFieldIntI) ;
	//	myFieldIOR = myFieldTie->_this() ;
	//	myFieldIntI->addInStudy(myStudy,myFieldIOR) ;

	SALOME_MED::FIELDINT_ptr myFieldIntIOR ;
	POA_SALOME_MED::FIELDINT_tie<FIELDINT_i> * myFieldIntTie = new POA_SALOME_MED::FIELDINT_tie<FIELDINT_i>(myFieldIntI) ;
	myFieldIntIOR = myFieldIntTie->_this() ;
	myFieldIntI->addInStudy(myStudy,myFieldIntIOR) ;

	_fields[fieldsNames[i]][dtIt] = myFieldIntIOR ;

	break ;
      }
      //	else
      case MED_FR::MED_REEL64: {
	((FIELD<double>*)myField)->read() ;
	FIELDDOUBLE_i * myFieldDoubleI = new FIELDDOUBLE_i(mySupportIOR,(FIELD<double>*)myField);
	//SALOME_MED::FIELDDOUBLE_ptr myFieldDoubleIOR = myFieldDoubleI->_this() ;
	//	POA_SALOME_MED::FIELD_tie<FIELD_i> * myFieldTie= new POA_SALOME_MED::FIELD_tie<FIELD_i>(myFieldDoubleI) ;
	//	myFieldIOR = myFieldTie->_this() ;
	//	myFieldDoubleI->addInStudy(myStudy,myFieldIOR) ;

	SALOME_MED::FIELDDOUBLE_ptr myFieldDoubleIOR ;
	POA_SALOME_MED::FIELDDOUBLE_tie<FIELDDOUBLE_i> * myFieldDoubleTie = new POA_SALOME_MED::FIELDDOUBLE_tie<FIELDDOUBLE_i>(myFieldDoubleI) ;
	myFieldDoubleIOR = myFieldDoubleTie->_this() ;
	myFieldDoubleI->addInStudy(myStudy,myFieldDoubleIOR) ;

	_fields[fieldsNames[i]][dtIt] = myFieldDoubleIOR ;

	break ;
      }
      default: {
	throw MED_EXCEPTION ( LOCALIZED( STRING(LOC)
					 <<"   * Iteration "<<FieldIteration[j].dt
					 <<", order number "<<FieldIteration[j].it
					 <<" has wrond type : "<<type)
			      );
	break ;
      }
      }
      //      SALOME_MED::FIELD_ptr myFieldIOR = myFieldI->POA_SALOME_MED::FIELD::_this() ;
      //      DT_IT_ dtIt;
      //      dtIt.dt  = myIteration[j].dt ;
      //      dtIt.it  = myIteration[j].it ;
      //      _fields[fieldsNames[i]][dtIt] = myFieldIOR ;
    }
  }
  
  END_OF(LOC);
}

//  MED_i::MED_i(::MED * const m):_med(m)
//  {
//          BEGIN_OF("Constructor MED_i");
//          END_OF("Constructor MED_i");
//  }
//=============================================================================
/*!
 * Constructor par recopie
 */
//=============================================================================
//  MED_i::MED_i(MED_i & m):_med(m._med)
//  {
//          BEGIN_OF("Constructor par recopie MED_i");
//          END_OF("Constructorpar recopie  MED_i");
//  }
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
        catch(...)
        {
                MESSAGE("Unable to get the number of meshes in Med Object");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces MED C++ Object"
                                                ,SALOME::INTERNAL_ERROR);
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
        catch(...)
        {
                MESSAGE("Unable to get the number of fields in Med Object");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces MED C++ Object"
                                                ,SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for meshes names
 */
//=============================================================================
Engines::string_array * MED_i::getMeshNames()      
throw (SALOME::SALOME_Exception)
{
        if (_med==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Med object",\
                                             SALOME::INTERNAL_ERROR);
        Engines::string_array_var myseq = new Engines::string_array;
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
        catch(...)
        {
                MESSAGE("Unable to get the names of the meshes in Med Object");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces MED C++ Object"
                                                ,SALOME::INTERNAL_ERROR);
        }
	return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Accessor for fields names
 */
//=============================================================================
Engines::string_array * MED_i::getFieldNames()     
throw (SALOME::SALOME_Exception)
{
        if (_med==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Med object",\
                                             SALOME::INTERNAL_ERROR);
        Engines::string_array_var myseq = new Engines::string_array;
        try
        {
                int nbFields=_med->getNumberOfFields();
                myseq->length(nbFields);
                string * nameFields = new string[nbFields];
                _med->getFieldNames(nameFields);
                for (int i=0;i<nbFields;i++)
                {
                        myseq[i]=CORBA::string_dup(nameFields[i].c_str());
                }
		delete nameFields;
		nameFields=NULL;
        }
        catch(...)
        {
                MESSAGE("Unable to get the names of the fields in Med Object");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces MED C++ Object"
                                                ,SALOME::INTERNAL_ERROR);
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
                SALOME_MED::MESH_ptr m2 = m1->POA_SALOME_MED::MESH::_this();
                m1->_remove_ref();
		return (SALOME_MED::MESH::_duplicate(m2));
        }
        catch(...)
        {
                MESSAGE("Unable to get the specified mesh");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces MED C++ Object"
                                                ,SALOME::INTERNAL_ERROR);
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
                ASSERT(FIELDOF_i<double>::fieldMap.find(ind)
			        !=FIELDOF_i<double>::fieldMap.end());
                ::FIELD<double> * fdouble = FIELDOF_i<double>::fieldMap[ind];
                MESH * mesh=_med->getMesh(fdouble);
        }
        else
        {
                MESSAGE("Integer");
                ASSERT(FIELDOF_i<int>::fieldMap.find(ind)!=FIELDOF_i<int>::fieldMap.end());
                ::FIELD<int> * fint = FIELDOF_i<int>::fieldMap[ind];
                MESH * mesh=_med->getMesh(fint);
        }
        MESH_i * meshi = new MESH_i(mesh);
        SALOME_MED::MESH_ptr meshptr = meshi->POA_SALOME_MED::MESH::_this();
        meshi->_remove_ref();
	return (SALOME_MED::MESH::_duplicate(meshptr));

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
  const char * LOC="MED_i::getField(const char*,CORBA::Long,CORBA::Long) " ;
  BEGIN_OF(LOC) ;

  DT_IT_ dtIt;

  dtIt.dt= (int)pasTemps;
  dtIt.it= (int)numOrdre;

  map<string,MAP_IOR_DT_IT_>::const_iterator itFields = _fields.find(fieldName);

  if ( itFields == _fields.end() ) 
    THROW_SALOME_CORBA_EXCEPTION("Field not found !", SALOME::INTERNAL_ERROR) ;
  
  const MAP_IOR_DT_IT_ & map_dtIt = (*itFields).second ;
  MAP_IOR_DT_IT_::const_iterator itMap_dtIt =  map_dtIt.find(dtIt);
  
  if ( itMap_dtIt == map_dtIt.end() )
    THROW_SALOME_CORBA_EXCEPTION("Iteration not found !", SALOME::INTERNAL_ERROR) ;
  
  END_OF(LOC) ;
  return (*itMap_dtIt).second;

//          if (_med==NULL)
//                  THROW_SALOME_CORBA_EXCEPTION("No associated Med object",\
//                                               SALOME::INTERNAL_ERROR);
//          try
//          {
//  	   if (dynamic_cast<FIELD<int> * >
//  	                (_med->getField(fieldName,pasTemps,numOrdre)) !=NULL)
//  	   {
//  		MESSAGE("Champ d entier");
//  		::FIELD<int>*f=dynamic_cast<FIELD<int> * >
//                          (_med->getField(fieldName,pasTemps,numOrdre));
//  		FIELDINT_i * medf = new FIELDINT_i(f);
//  		POA_SALOME_MED::FIELDINT_tie<FIELDINT_i> * f1 =
//                     new POA_SALOME_MED::FIELDINT_tie<FIELDINT_i>(medf,true);
//                  SALOME_MED::FIELDINT_ptr f2 = f1->_this();
//                  f1->_remove_ref();
//                  return f2;
		
//  	   }
//  	   else
//  	   {
//  	  	ASSERT (dynamic_cast<FIELD<double>* >(_med->getField(fieldName,pasTemps,numOrdre))!=NULL);
//  		MESSAGE("Champ de double");
//  		::FIELD<double>*f=dynamic_cast<FIELD<double>* >
//                          (_med->getField(fieldName,pasTemps,numOrdre));
//  		FIELDDOUBLE_i * medf = new FIELDDOUBLE_i(f);
//  		POA_SALOME_MED::FIELDDOUBLE_tie<FIELDDOUBLE_i> * f1 =
//                     new POA_SALOME_MED::FIELDDOUBLE_tie<FIELDDOUBLE_i>(medf,true);
//                  SALOME_MED::FIELDDOUBLE_ptr f2 = f1->_this();
//                  f1->_remove_ref();
//                  return f2;
//  	   }
//          }
//          catch(...)
//          {
//                  MESSAGE("Exception en accedant a l objet C++ Med");
//                  THROW_SALOME_CORBA_EXCEPTION("Unable to acces MED C++ Object"
//                                                  ,SALOME::INTERNAL_ERROR);
//          }
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
        catch(...)
        {
                MESSAGE("Unable to add a driver to the med object");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces MED C++ Object"
                                                ,SALOME::INTERNAL_ERROR);
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
        catch(...)
        {
                MESSAGE("Unable to unlink the driver from the med object");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces MED C++ Object"
                                                ,SALOME::INTERNAL_ERROR);
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
        catch(...)
        {
                MESSAGE("Unable to read the structure of this file ");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces MED C++ Object"
                                                ,SALOME::INTERNAL_ERROR);
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
        catch(...)
        {
                MESSAGE("Unable to write this driver ");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces MED C++ Object"
                                                ,SALOME::INTERNAL_ERROR);
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
        catch(...)
        {
                MESSAGE("Unable to write this file ");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces MED C++ Object"
                                                ,SALOME::INTERNAL_ERROR);
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
        catch(...)
        {
                MESSAGE("Unable to link this mesh to this med object");
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces MED C++ Object"
                                                ,SALOME::INTERNAL_ERROR);
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
                ASSERT(FIELDOF_i<double>::fieldMap.find(ind)
			        !=FIELDOF_i<double>::fieldMap.end());
                ::FIELD<double> * fdouble = FIELDOF_i<double>::fieldMap[ind];
		// A modifier
                //_med->addField(fdouble);
        }
        else
        {
                MESSAGE("Integer");
                ASSERT(FIELDOF_i<int>::fieldMap.find(ind)!=FIELDOF_i<int>::fieldMap.end());
                ::FIELD<int> * fint = FIELDOF_i<int>::fieldMap[ind];
                //_med->addField(fint);
        }
}
//=============================================================================
/*!
 * CORBA: Add Med in Study 
 */
//=============================================================================
void MED_i::addInStudy(SALOMEDS::Study_ptr myStudy, SALOME_MED::MED_ptr myIor) 
throw (SALOME::SALOME_Exception, SALOMEDS::StudyBuilder::LockProtection)
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
        SALOMEDS::SComponent_var medfather = myStudy->FindComponent("Med");
        if ( CORBA::is_nil(medfather) )
        {
	  THROW_SALOME_CORBA_EXCEPTION("Component Med not found",
				       SALOME::BAD_PARAM);
        } ;

        MESSAGE("Add a MED Object under Med");
        SALOMEDS::SObject_var newObj = myBuilder->NewObject(medfather);

        ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
        ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting()) ;
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

        END_OF("Med_i::addInStudy(SALOMEDS::Study_ptr myStudy)");
}




