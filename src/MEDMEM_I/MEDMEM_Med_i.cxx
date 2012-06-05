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
// File      : MEDMEM_Med_i.cxx
// Project   : SALOME
// Author    : EDF
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_i.cxx
//=============================================================================
//
#include <deque>

//#include "MEDMEM_Field.hxx"
#include "MEDMEM_Med_i.hxx"
#include "MEDMEM_Mesh_i.hxx"
#include "MEDMEM_FieldTemplate_i.hxx"
#include "MEDMEM_Support_i.hxx"
#include "MEDMEM_Family_i.hxx"
#include "MEDMEM_Group_i.hxx"
#include "MEDMEM_convert.hxx"

#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Grid.hxx"

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
MED_i::MED_i()
{
}

//=======================================================================
//function : init
//purpose  : * methods
//=======================================================================

void MED_i::init(SALOMEDS::Study_ptr myStudy,driverTypes /*driverType*/, const string & fileName)
{

  // we create all IOR from _med
  _med.readFileStruct(fileName);

  // MESHES :
  vector<string> meshesNames = _med.getMeshNames();
  for (int i=0; i<(int)meshesNames.size(); i++) 
  {
    const MESH * myMesh = 0;
    if ( _med.isStructuredMesh( meshesNames[i] ))
      myMesh = GRID( MED_DRIVER, fileName, meshesNames[i] ).convertInMESH();
    else
      myMesh = new MESH( MED_DRIVER, fileName, meshesNames[i] );
    MESH_i * myMeshI = new MESH_i(const_cast<MESH*>( myMesh ));
    SALOME_MED::MESH_ptr myMeshIOR = myMeshI->_this();
    _meshes[meshesNames[i]] = myMeshIOR;
    _medmem_meshes[meshesNames[i]] = const_cast<MESH*>( myMesh );
  }

  // FAMILIES :
  // we add all families 
  vector<FAMILY*> familyVector;
  vector<FAMILY*>::iterator familyVectorIt;
  // GROUPS :
  // we add all groups
  vector<GROUP*> groupVector;
  vector<GROUP*>::iterator groupVectorIt;
  
  MED_EN::MESH_ENTITIES::const_iterator currentEntity; 

  // FIELDS :
  vector<string> fieldsNames = _med.getFieldNames();
  for (int i=0; i<(int)fieldsNames.size(); i++) 
  {
    string meshName = _med.getMeshName( fieldsNames[i] );
    map<string, GMESH* >::const_iterator name_mesh = _medmem_meshes.find( meshName );
    GMESH* mesh = name_mesh == _medmem_meshes.end() ? (MESH*) 0 : name_mesh->second;
    med_type_champ type = _med.getFieldType(fieldsNames[i]);
    VEC_DT_IT_ myIteration = _med.getFieldIteration (fieldsNames[i]);
    for (int j=0; j<(int)myIteration.size(); j++) 
    {
      SALOME_MED::FIELD_ptr myFieldIOR;
      switch (type) 
      {
      case MED_EN::MED_INT32 : 
        {
          FIELD<int>* myField = new FIELD<int>( MED_DRIVER, fileName, fieldsNames[i],
                                                myIteration[j].dt, myIteration[j].it,
                                                mesh);
          FIELDTEMPLATE_I<int> *myFieldIntI = new FIELDTEMPLATE_I<int>(myField);
          myFieldIOR = myFieldIntI->_this();
          break;
        }

      case MED_EN::MED_REEL64: 
        {
          FIELD<double>* myField = new FIELD<double>( MED_DRIVER, fileName, fieldsNames[i],
                                                      myIteration[j].dt, myIteration[j].it,
                                                      mesh);
          FIELDTEMPLATE_I<double> *myFieldDoubleI = new FIELDTEMPLATE_I<double>(myField);
          myFieldIOR = myFieldDoubleI->_this();
          break;
        }
      default:
        {
          ostringstream stream;
          stream << "MED_i::init(driverTypes, const string &)"
                 <<"   * Iteration "<<myIteration[j].dt
                 <<", order number "<<myIteration[j].it
                 <<" has wrong type : "<<type;
          throw MED_EXCEPTION(stream.str().c_str());
          break;
        }
      }

      DT_IT_ dtIt;
      dtIt.dt  = myIteration[j].dt;
      dtIt.it  = myIteration[j].it;
      _fields[fieldsNames[i]][dtIt] = myFieldIOR;
    }
  }
}

//=============================================================================
/*!
 * methods
 */
//=============================================================================
void MED_i::initWithFieldType(SALOMEDS::Study_ptr myStudy,driverTypes /*driverType*/, const string & fileName, bool persistence)
{
  // if (persistence):
  //    some objects can be not published

  // we create all IOR from _med
  _med.readFileStruct(fileName);

  // MESHES :
  vector<string> meshesNames = _med.getMeshNames();
  for (int i=0; i<(int)meshesNames.size(); i++) 
  {
    const MESH * myMesh = 0;
    if ( _med.isStructuredMesh( meshesNames[i] ))
      myMesh = GRID( MED_DRIVER, fileName, meshesNames[i] ).convertInMESH();
    else
      myMesh = new MESH( MED_DRIVER, fileName, meshesNames[i] );
    MESH_i * myMeshI = new MESH_i(const_cast<MESH*>( myMesh ));
    SALOME_MED::MESH_ptr myMeshIOR = myMeshI->_this();
    _meshes[meshesNames[i]] = myMeshIOR;
    _medmem_meshes[meshesNames[i]] = const_cast<MESH*>( myMesh );
    if ( !persistence )
      myMeshI->addInStudy(myStudy,myMeshIOR);
  }

  if ( !persistence )
  {
    // FAMILIES :
    // we add all families in the study
    vector<FAMILY*> familyVector;
    vector<FAMILY*>::iterator familyVectorIt;
    // GROUPS :
    // we add all groups in the study
    vector<GROUP*> groupVector;
    vector<GROUP*>::iterator groupVectorIt;

    MED_EN::MESH_ENTITIES::const_iterator currentEntity; 
    for (int i=0; i<(int)meshesNames.size(); i++) 
    {
      ::GMESH * ptrMesh = _medmem_meshes[meshesNames[i]];

      SCRUTE(ptrMesh);

      for (currentEntity = MED_EN::meshEntities.begin();
           currentEntity != MED_EN::meshEntities.end(); 
           currentEntity++)
      {
        MESSAGE("initWithFieldType: for entity " << (*currentEntity).first);
        // family :
        familyVector = ptrMesh->getFamilies((MED_EN::medEntityMesh)(*currentEntity).first);
        int nb = familyVector.size();
        MESSAGE("initWithFieldType: there is(are) " << nb << " family(ies)");
        for (familyVectorIt = familyVector.begin();
             familyVectorIt != familyVector.end();
             familyVectorIt++)
        {
          FAMILY_i * myFamilyI = new FAMILY_i(*familyVectorIt);
          SALOME_MED::FAMILY_ptr myFamilyIOR = myFamilyI->_this();
          myFamilyI->addInStudy(myStudy,myFamilyIOR);
        }

        // group :
        groupVector = ptrMesh->getGroups((MED_EN::medEntityMesh)(*currentEntity).first);
        nb = groupVector.size();
        MESSAGE("initWithFieldType: there is(are) " << nb << " group(s)");
        for (groupVectorIt = groupVector.begin();
             groupVectorIt != groupVector.end();
             groupVectorIt++)
        {
          GROUP_i * myGroupI = new GROUP_i(*groupVectorIt);
          SALOME_MED::GROUP_ptr myGroupIOR = myGroupI->_this();
          myGroupI->addInStudy(myStudy,myGroupIOR);
        }
      }
    }
  }

  // SUPPORTS on All:

  map<string, set<string> > publishedSupportsByMesh;

  map<string, ::MEDMEM::GMESH* >::const_iterator name2mesh = _medmem_meshes.begin();
  for ( ; name2mesh != _medmem_meshes.end(); ++name2mesh )
    {
      ::MEDMEM::GMESH* mesh = name2mesh->second;
      for ( medEntityMesh entity = 0; entity < MED_ALL_ENTITIES; ++entity )
        {
          if ( mesh->getNumberOfElements( entity, MED_ALL_ELEMENTS ) > 0 )
            {
              const ::MEDMEM::SUPPORT* sup = mesh->getSupportOnAll( entity );
              SUPPORT_i * mySupportI = new SUPPORT_i( sup );
              SALOME_MED::SUPPORT_ptr mySupportIOR = mySupportI->_this();
              try // mySupportI->addInStudy() may throw
              {
                mySupportI->addInStudy(myStudy,mySupportIOR);
                publishedSupportsByMesh[ mesh->getName() ].insert(sup->getName());
              }
              catch (...)
              {
                mySupportIOR->UnRegister();
              }
            }
        }
    }

  // FIELDS :

  vector<string> fieldsNames = _med.getFieldNames();
  for (int i=0; i<(int)fieldsNames.size(); i++) 
  {
    string meshName = _med.getMeshName( fieldsNames[i] );
    map<string, GMESH* >::const_iterator name_mesh = _medmem_meshes.find( meshName );
    GMESH* mesh = name_mesh == _medmem_meshes.end() ? (GMESH*) 0 : name_mesh->second;

    med_type_champ type = _med.getFieldType(fieldsNames[i]);
    VEC_DT_IT_ myIteration = _med.getFieldIteration (fieldsNames[i]);

    for (int j=0; j<(int)myIteration.size(); j++) 
    {
      SALOME_MED::FIELD_ptr myFieldIOR;
      ::FIELD_ * myField = 0;
      switch (type) 
      {
      case MED_EN::MED_INT32 : 
        {
          FIELD<int>* myIntField = new FIELD<int>( MED_DRIVER, fileName, fieldsNames[i],
                                                   myIteration[j].dt, myIteration[j].it,
                                                   mesh);
          FIELDTEMPLATE_I<int> *myFieldIntI = new FIELDTEMPLATE_I<int>(myIntField);
          myFieldIOR = myFieldIntI->_this();
          myField = myIntField;
          break;
        }

      case MED_EN::MED_REEL64: 
        {
          FIELD<double>* myDblField = new FIELD<double>(MED_DRIVER, fileName, fieldsNames[i],
                                                        myIteration[j].dt, myIteration[j].it,
                                                        mesh);
          FIELDTEMPLATE_I<double> *myFieldDoubleI = new FIELDTEMPLATE_I<double>(myDblField);
          myFieldIOR = myFieldDoubleI->_this();
          myField = myDblField;
          break;
        }
      default:
        {
          ostringstream stream;
          stream << "MED_i::init(driverTypes, const string &)"
                 <<"   * Iteration "<<myIteration[j].dt
                 <<", order number "<<myIteration[j].it
                 <<" has wrong type : "<<type;
          throw MED_EXCEPTION(stream.str().c_str());
          break;
        }
      }

      // check if corresponding support exists in the study
      if ( !persistence )
      {
        string supportName = myField->getSupport()->getName();
        bool supportExists = !publishedSupportsByMesh[ meshName ].insert(supportName).second;
        if ( !supportExists )
        {
          SUPPORT_i * mySupportI = new SUPPORT_i( myField->getSupport() );
          SALOME_MED::SUPPORT_ptr mySupportIOR = mySupportI->_this();
          mySupportI->addInStudy(myStudy,mySupportIOR);
        }
      }
      DT_IT_ dtIt;
      dtIt.dt  = myIteration[j].dt;
      dtIt.it  = myIteration[j].it;
      _fields[fieldsNames[i]][dtIt] = myFieldIOR;
      if ( !persistence )
        myFieldIOR->addInStudy(myStudy,myFieldIOR);
    }
  }
}

//=======================================================================
//function : getSupport
//purpose  : * \brief Return support
//           * \param meshName - name of the mesh the suppotr belongs to
//           * \param entity - support entity
//           * \retval SALOME_MED::SUPPORT_ptr - found support
//           * Raises if support not found
//=======================================================================

SALOME_MED::SUPPORT_ptr MED_i::getSupport(string                meshName,
                                          MED_EN::medEntityMesh entity,
                                          const string          name)
  throw (SALOME::SALOME_Exception)
{
  // try to find a support of field by a name of support

  const ::MEDMEM::SUPPORT* support = 0;
  if ( !name.empty() )
  {
    for ( int i = 0; !support && i < (int)_medmem_fields.size(); ++i )
      if ( name     == _medmem_fields[i]->getSupport()->getName() &&
           meshName == _medmem_fields[i]->getSupport()->getMeshName())
        support = _medmem_fields[i]->getSupport();
    if ( !support )
      throw MED_EXCEPTION(STRING("MED_i::getSupport(meshName, entity, supportName) : ")
                          << "There is no support named |"<< name<<"|"
                          << " on mesh named |" << meshName << "|");
  }

  if ( !support )
  {
    // get support on all elements of given entity

    map<string, ::MEDMEM::GMESH* >::iterator name_mesh = _medmem_meshes.find( meshName );
    if ( name_mesh == _medmem_meshes.end() )
      throw MED_EXCEPTION(STRING("MED_i::getSupport(meshName, entity, supportName) : ")
                          << "There is no mesh named |" << meshName << "|");
    support = name_mesh->second->getSupportOnAll(entity);
  }
  if ( ! support )
    throw MED_EXCEPTION(STRING("MED_i::getSupport(meshName, entity, supportName) : ")
                        << "There is no support on entity " << entity
                        << " in mesh named |"<< meshName << "|");

  SUPPORT_i * mySupportI = new SUPPORT_i( support );
  SALOME_MED::SUPPORT_ptr mySupportIOR = mySupportI->_this();
  return mySupportIOR;
}

//=======================================================================
//function : ~MED_i
//purpose  : * Destructor
//=======================================================================

MED_i::~MED_i()
{
  map<string, ::MEDMEM::GMESH* >::iterator n_m =  _medmem_meshes.begin();
  for ( ; n_m != _medmem_meshes.end(); ++ n_m )
    n_m->second->removeReference();
  _medmem_meshes.clear();

  for ( int i = 0; i < (int)_medmem_fields.size(); ++i )
    _medmem_fields[i]->removeReference();
  _medmem_fields.clear();
}

//=======================================================================
//function : getNumberOfMeshes
//purpose  : * CORBA: Accessor for Number of meshes
//=======================================================================

CORBA::Long MED_i::getNumberOfMeshes() 
throw (SALOME::SALOME_Exception)
{
        try
        {
                return _med.getNumberOfMeshes();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get the number of meshes in Med Object");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}

//=======================================================================
//function : getNumberOfFields
//purpose  : * CORBA: Accessor for Number of fields
//=======================================================================

CORBA::Long MED_i::getNumberOfFields() 
throw (SALOME::SALOME_Exception)
{
        try
        {
                return _med.getNumberOfFields();
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
SALOME_TYPES::ListOfString * MED_i::getMeshNames()      
throw (SALOME::SALOME_Exception)
{
        SALOME_TYPES::ListOfString_var myseq = new SALOME_TYPES::ListOfString;
        try
        {
                int nbMeshes=_med.getNumberOfMeshes();
                myseq->length(nbMeshes);
                string * nameMeshes = new string[nbMeshes];
                _med.getMeshNames(nameMeshes);
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
SALOME_TYPES::ListOfString * MED_i::getFieldNames()     
  throw (SALOME::SALOME_Exception)
{
  SALOME_TYPES::ListOfString_var myseq = new SALOME_TYPES::ListOfString;
  try
    {
      vector<string> nameFields = _med.getFieldNames();
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

//=======================================================================
//function : getMeshByName
//purpose  : * CORBA: Accessor for a specific mesh
//=======================================================================

SALOME_MED::GMESH_ptr MED_i::getMeshByName(const char* meshName) 
throw (SALOME::SALOME_Exception)
{
        try
        {
          if (_meshes.count( meshName ))
            return SALOME_MED::GMESH::_duplicate(_meshes[meshName] );

          GMESH * myMesh = 0;
          if ( _med.isStructuredMesh( meshName ))
            myMesh = new GRID( MED_DRIVER, _med.getFileName(), meshName );
          else
            myMesh = new MESH( MED_DRIVER, _med.getFileName(), meshName );
          GMESH_i * m1 = new GMESH_i(myMesh);
          return m1->_this();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get the specified mesh");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }

}

//=======================================================================
//function : getMesh
//purpose  : * CORBA: Accessor for a specific mesh
//=======================================================================

SALOME_MED::GMESH_ptr MED_i::getMesh(SALOME_MED::FIELD_ptr fieldPtr) 
throw (SALOME::SALOME_Exception)
{
        int ind=fieldPtr->getCorbaIndex();
        SCRUTE(ind);

        SALOME_MED::FIELDINT_var fieldint =SALOME_MED::FIELDINT::_narrow(fieldPtr);
        if (CORBA::is_nil(fieldint))
        {
                MESSAGE("Double");
                SALOME_MED::FIELDDOUBLE_var fielddouble = 
                            SALOME_MED::FIELDDOUBLE::_narrow(fieldPtr);
                ASSERT(!CORBA::is_nil(fielddouble));

                ASSERT(FIELD_i::fieldMap.find(ind)!=FIELD_i::fieldMap.end());

        }
        else
        {
                MESSAGE("Integer");
                ASSERT(FIELD_i::fieldMap.find(ind)!=FIELD_i::fieldMap.end());
        }
        FIELD_* field = FIELD_i::fieldMap[ind];
        string meshName;
        if ( field->getSupport() )
        {
          if ( field->getSupport()->getMesh() )
            meshName = field->getSupport()->getMesh()->getName();
          else
            field->getSupport()->getMeshName();
        }
        SALOME_MED::GMESH_var meshIOR;
        if ( _meshes.count( meshName ))
          meshIOR = SALOME_MED::GMESH::_duplicate(_meshes[meshName] );
        return meshIOR._retn();
}

//=======================================================================
//function : getFieldNumberOfIteration
//purpose  : * CORBA: Accessor for Number of iteration of a given field by its name.
//=======================================================================

CORBA::Long MED_i::getFieldNumberOfIteration(const char* fieldName) 
  throw (SALOME::SALOME_Exception)
{
  try
    {
      vector<DT_IT_> fieldIteration = _med.getFieldIteration(string(fieldName));
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
SALOME_TYPES::ListOfLong * MED_i::getFieldIteration(const char* fieldName,CORBA::Long i) 
  throw (SALOME::SALOME_Exception)
{
  SALOME_TYPES::ListOfLong_var myseq = new SALOME_TYPES::ListOfLong;
  try
    {
      vector<DT_IT_> fieldIteration = _med.getFieldIteration(string(fieldName));
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
SALOME_TYPES::ListOfLong * MED_i::getFieldIterations(const char* fieldName) 
  throw (SALOME::SALOME_Exception)
{
  SALOME_TYPES::ListOfLong_var myseq = new SALOME_TYPES::ListOfLong;
  try
    {
      vector<DT_IT_> fieldIteration = _med.getFieldIteration(string(fieldName));
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

//=======================================================================
//function : getField
//purpose  : * CORBA: Accessor for a specific field
//=======================================================================

SALOME_MED::FIELD_ptr MED_i::getField(const char* fieldName, 
                                      CORBA::Long pasTemps, 
                                      CORBA::Long numOrdre ) 
throw (SALOME::SALOME_Exception)
{

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
  
        return (*itMap_dtIt).second;

}

//=======================================================================
//function : addDriver
//purpose  : * CORBA: Accessor for a specific field
//=======================================================================

CORBA::Long MED_i::addDriver(SALOME_MED::medDriverTypes driverType, const char* fileName) 
throw (SALOME::SALOME_Exception)
{
  return 0;
}

//=======================================================================
//function : rmDriver
//purpose  : * CORBA: relase the driver (mandatory)
//=======================================================================

void MED_i::rmDriver(CORBA::Long i) 
throw (SALOME::SALOME_Exception)
{
}

//=======================================================================
//function : readFileStruct
//purpose  : * CORBA: read med objects relations
//=======================================================================

void MED_i::readFileStruct(CORBA::Long i) 
throw (SALOME::SALOME_Exception)
{
}

//=======================================================================
//function : writeFrom
//purpose  : * CORBA: write specifics objects f
//=======================================================================

void MED_i::writeFrom(CORBA::Long i) 
throw (SALOME::SALOME_Exception)
{
}

//=======================================================================
//function : write
//purpose  : * CORBA: write objects
//=======================================================================

void MED_i::write(CORBA::Long i) 
throw (SALOME::SALOME_Exception)
{
}

//=======================================================================
//function : addMesh
//purpose  : * CORBA: Add Mesh in Med objects
//=======================================================================

void MED_i::addMesh(SALOME_MED::GMESH_ptr ptrMesh) 
throw (SALOME::SALOME_Exception)
{
  THROW_SALOME_CORBA_EXCEPTION("MED_i::addMesh() is obsolete method",SALOME::BAD_PARAM);
}

//=======================================================================
//function : addField
//purpose  : * CORBA: Add field in Med objects
//=======================================================================

void MED_i::addField(SALOME_MED::FIELD_ptr ptrField) 
throw (SALOME::SALOME_Exception)
{
  THROW_SALOME_CORBA_EXCEPTION("MED_i::addField() is obsolete method",SALOME::BAD_PARAM);
}

//=======================================================================
//function : addInStudy
//purpose  : * CORBA: Add Med object in Study 
//=======================================================================

void MED_i::addInStudy(SALOMEDS::Study_ptr myStudy, SALOME_MED::MED_ptr myIor) 
throw (SALOME::SALOME_Exception,SALOMEDS::StudyBuilder::LockProtection)
{
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
        CORBA::String_var iorStr = orb->object_to_string(myIor);
        anAttr = myBuilder->FindOrCreateAttribute(newObj, "AttributeIOR");
        aIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
        aIOR->SetValue(iorStr.in());
        anAttr = myBuilder->FindOrCreateAttribute(newObj, "AttributeName");
        aName = SALOMEDS::AttributeName::_narrow(anAttr);
        aName->SetValue("Objet MED");
        _medId = newObj->GetID();
        myBuilder->CommitCommand();

        // register the Corba pointer: increase the referrence count
        MESSAGE("Registering of the Corba Med pointer");
        Register();

}

//=============================================================================
/*!
 * CORBA: Add Med object in Study with a name medObjName
 *
 * FOR PERSISTANCE: if fileName is NULL string, looks for a SObject with a
 * suitable name and bad IOR and update the IOR
 */
//=============================================================================
void MED_i::addInStudy (SALOMEDS::Study_ptr myStudy,
                        SALOME_MED::MED_ptr myIor,
                        SALOMEDS::SComponent_ptr medfather,
                        const char * fileName)
  throw (SALOME::SALOME_Exception,SALOMEDS::StudyBuilder::LockProtection)
{
        if ( _medId != "" ) {
          MESSAGE("Med already in Study");
          THROW_SALOME_CORBA_EXCEPTION("Med already in Study", SALOME::BAD_PARAM);
        };

        SALOMEDS::StudyBuilder_var     myBuilder = myStudy->NewBuilder();
        myBuilder->NewCommand();
        SALOMEDS::GenericAttribute_var anAttr;
        SALOMEDS::AttributeName_var    aName;
        SALOMEDS::AttributeIOR_var     aIOR;

        // Create SComponent labelled 'MED' if it doesn't already exit
        //SALOMEDS::SComponent_var medfather = myStudy->FindComponent("MED");
        if ( CORBA::is_nil(medfather) )
        {
          THROW_SALOME_CORBA_EXCEPTION("Component Med not found",
                                       SALOME::BAD_PARAM);
        };

        SALOMEDS::SObject_var medObj;
        if ( ! fileName )
        {
          //  FOR PERSISTANCE: if fileName is a NULL string,
          //  looks for a SObject with a suitable name and
          //  bad IOR and update the IOR
          SALOMEDS::ChildIterator_var anIter = myStudy->NewChildIterator( medfather );
          for ( ; anIter->More(); anIter->Next()) {
            SALOMEDS::SObject_var obj = anIter->Value();
            // check name
            CORBA::String_var name = obj->GetName();
            const char* suitableName1 = "MED_OBJECT_FROM_FILE_";
            const char* suitableName2 = "Objet MED";
            if ( strncmp( name, suitableName1, strlen(suitableName1)) == 0 || 
                 strcmp( name, suitableName2 ) == 0 ) {
              // check IOR
              CORBA::String_var ior = obj->GetIOR();
              if ( strlen( ior ) == 0 ) {
                medObj = obj;
                break;
              }
            }
          }
          if ( medObj->_is_nil() )
            return; // nothing looking like MED found in the study
        }
        else
        {
          MESSAGE("Add a MED Object under the medfather");

          char * medObjName;
          string::size_type pos1=string(fileName).rfind('/');
          string::size_type lenFileName = strlen(fileName);
          string fileNameShort = string(fileName,pos1+1,lenFileName-pos1-1);

          SCRUTE(fileNameShort);

          int lenName = 21 + strlen(fileNameShort.c_str()) + 1;
          medObjName = new char[lenName];
          medObjName = strcpy(medObjName,"MED_OBJECT_FROM_FILE_");
          medObjName = strcat(medObjName,fileNameShort.c_str());

          string path ("/");
          path += string(medfather->GetName());
          path += string("/");
          path += medObjName;
          //string path = string("/Med/") + medObjName;
          medObj = myStudy->FindObjectByPath(path.c_str());
          if ( medObj->_is_nil() )
            medObj = myBuilder->NewObject(medfather);

          anAttr = myBuilder->FindOrCreateAttribute(medObj, "AttributeName");
          aName = SALOMEDS::AttributeName::_narrow(anAttr);
          aName->SetValue(medObjName);

          delete [] medObjName;
        }
        ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance();
        ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting());
        CORBA::ORB_var &orb = init(0,0);
        string iorStr = orb->object_to_string(myIor);
        anAttr = myBuilder->FindOrCreateAttribute(medObj, "AttributeIOR");
        aIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
        aIOR->SetValue(iorStr.c_str());

        _medId = medObj->GetID();
        myBuilder->CommitCommand();

        // register the Corba pointer: increase the referrence count
        MESSAGE("Registering of the Corba Med pointer");
        Register();
}
