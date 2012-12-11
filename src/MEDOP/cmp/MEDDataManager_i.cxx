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

// Authors : Guillaume Boulant (EDF) - 01/06/2011

#include "MEDDataManager_i.hxx"
#include "SALOME_KernelServices.hxx"
#include "Basics_DirUtils.hxx"
#include "Basics_Utils.hxx"

#include "MEDLoader.hxx"
using namespace ParaMEDMEM;

#include <string>
#include <vector>
using namespace std;

MEDDataManager_i * MEDDataManager_i::_instance = NULL;
long MEDDataManager_i::LONG_UNDEFINED = -1;

MEDDataManager_i * MEDDataManager_i::getInstance() {
  // _GBO_ we will certainly need to define one single DataManager per
  // SALOME study and not one singleton for the whole session
  if ( _instance == NULL ) {
    _instance = new MEDDataManager_i();
  }
  return _instance;
}

#define IOR_UNDEF "IOR UNDEFINED"
MEDDataManager_i::MEDDataManager_i()
{
  LOG("Creating a MEDDataManager_i instance");
  _fieldLastId = 0;
  _sourceLastId = 0;
  _meshLastId = 0;
  _fieldseriesLastId = 0;
}
MEDDataManager_i::~MEDDataManager_i()
{
  LOG("Deleting MEDDataManager_i instance");
}

const char * MEDDataManager_i::file_to_source(const char * filepath)
{
  string * source = new string("file://");
  source->append(filepath);
  return source->c_str();;
}
const char * MEDDataManager_i::source_to_file(const char * source)
{
  string * filepath = new string(source);
  filepath->replace(0,7,"");
  return filepath->c_str();
}

/*!
 * This function loads the meta-data from the specified med file and
 * returns the associated datasource handler. The data source handler
 * is a key to retrieve all informations concerning the data (meshes,
 * fields).
 */
MEDOP::DatasourceHandler * MEDDataManager_i::addDatasource(const char *filepath) {

  // We first check that this datasource is not already registered  
  long sourceid = getDatasourceId(filepath);
  if ( sourceid != LONG_UNDEFINED ) {
    // The file is already registered under the identifier sourceid
    LOG("WRN: The file "<<filepath<<" is already registered with id="<<ToString(sourceid));
    return new MEDOP::DatasourceHandler(*_datasourceHandlerMap[sourceid]);
  }

  // Then we check that the file is readable by MEDLoader
  MEDLoader::CheckFileForRead(filepath);

  // Initialise the datasource handler
  MEDOP::DatasourceHandler * datasourceHandler = new MEDOP::DatasourceHandler();
  datasourceHandler->id = _sourceLastId; _sourceLastId++;
  datasourceHandler->name = (Kernel_Utils::GetBaseName(filepath)).c_str();
  datasourceHandler->uri = file_to_source(filepath);
  _datasourceHandlerMap[datasourceHandler->id] = datasourceHandler;

  // We start by read the list of meshes (spatial supports of fields)
  vector<string> meshNames = MEDLoader::GetMeshNames(filepath);
  int nbOfMeshes = meshNames.size();
  for (int iMesh = 0; iMesh < nbOfMeshes; iMesh++) {
    const char * meshName = meshNames[iMesh].c_str();
    LOG("name of mesh " << iMesh << " = " << meshName);

    MEDOP::MeshHandler * meshHandler = new MEDOP::MeshHandler();
    meshHandler->id       = _meshLastId; _meshLastId++;
    meshHandler->name     = meshName;
    meshHandler->sourceid = datasourceHandler->id;

    _meshHandlerMap[meshHandler->id] = meshHandler;

    // For each mesh, we can read the list of the names of the
    // associated fields, i.e. fields whose spatial support is this
    // mesh.
    vector<string> fieldNames = MEDLoader::GetAllFieldNamesOnMesh(filepath,
								  meshName);
    int nbOfFields = fieldNames.size();
    for (int iField = 0; iField < nbOfFields; iField++) {
      const char * fieldName = fieldNames[iField].c_str();
      LOG("-- name of field " << iField << " = " << fieldName);
      
      // A field name could identify several MEDCoupling fields, that
      // differ by their spatial discretization on the mesh (values on
      // cells, values on nodes, ...). This spatial discretization is
      // specified by the TypeOfField that is an integer value in this
      // list:
      // 0 = ON_CELLS 	
      // 1 = ON_NODES 	
      // 2 = ON_GAUSS_PT 	
      // 3 = ON_GAUSS_NE

      // As a consequence, before loading values of a field, we have
      // to determine the types of spatial discretization defined for
      // this field and to chooose one.

      vector<TypeOfField> listOfTypes = MEDLoader::GetTypesOfField(filepath,
								   meshName,
								   fieldName);
      int nbOfTypes = listOfTypes.size();
      for (int iType = 0; iType < nbOfTypes; iType++) {
	LOG("---- type "<<iType<<" of field "<<iField<< " = " << listOfTypes[iType]);

	// Then, we can get the iterations associated to this field on
	// this type of spatial discretization:
	std::vector< std::pair<int,int> > fieldIterations = 
	  MEDLoader::GetFieldIterations(listOfTypes[iType],
					filepath,
					meshName,
					fieldName);
	
	int nbFieldIterations = fieldIterations.size();
	LOG("---- nb. iterations = " << nbFieldIterations);

	// We can define the timeseries of fields (fieldseries) for
	// this type. A fieldseries is a macro object that handle the whole
	// set of time iterations of a field.
	MEDOP::FieldseriesHandler * fieldseriesHandler = new MEDOP::FieldseriesHandler();
	fieldseriesHandler->id     = _fieldseriesLastId; _fieldseriesLastId++;
	fieldseriesHandler->name   = fieldName;
	fieldseriesHandler->type   = listOfTypes[iType];
	fieldseriesHandler->meshid = meshHandler->id;
	fieldseriesHandler->nbIter = nbFieldIterations;
	_fieldseriesHandlerMap[fieldseriesHandler->id] = fieldseriesHandler;

	// We can then load meta-data concerning all iterations
	for (int iterationIdx=0; iterationIdx<nbFieldIterations; iterationIdx++) {
	  
	  int iteration = fieldIterations[iterationIdx].first;
	  int order = fieldIterations[iterationIdx].second;
	  
	  const char * source = datasourceHandler->uri;
	  MEDOP::FieldHandler * fieldHandler = newFieldHandler(fieldName,
							       meshName,
							       listOfTypes[iType],
							       iteration,
							       order,
							       source);
	  
	  fieldHandler->meshid = meshHandler->id;
	  fieldHandler->fieldseriesId = fieldseriesHandler->id;
	  _fieldHandlerMap[fieldHandler->id] = fieldHandler;
	}
      }
    }
  }
  
  return new MEDOP::DatasourceHandler(*datasourceHandler);
}

long MEDDataManager_i::getDatasourceId(const char *filepath) {
  const char * uri = file_to_source(filepath);
  DatasourceHandlerMapIterator it = _datasourceHandlerMap.begin();  
  while ( it != _datasourceHandlerMap.end() ) {
    if ( strcmp(it->second->uri,uri) == 0 ) {
      return it->first;
    }
    ++it;
  }
  return LONG_UNDEFINED;
}


MEDOP::MeshHandler * MEDDataManager_i::getMesh(CORBA::Long meshId) {
  if ( _meshHandlerMap.count(meshId) == 0 ) {
    std::string message =
      std::string("The mesh of id=") + ToString(meshId) +
      std::string(" does not exist in the data manager");
    LOG(message);
    throw KERNEL::createSalomeException(message.c_str());
  }
  return new MEDOP::MeshHandler(*(_meshHandlerMap[meshId]));
}


/*!
 * This function returns the list of mesh handlers associated to the
 * specified datasource. It corresponds to the list ofmeshes defined
 * in the datasource.
 */
MEDOP::MeshHandlerList * MEDDataManager_i::getMeshList(CORBA::Long datasourceId) {

  // We initiate a list with the maximum lentgh
  MEDOP::MeshHandlerList_var meshHandlerList = new MEDOP::MeshHandlerList();
  meshHandlerList->length(_meshHandlerMap.size());

  // Scan the map looking for meshes associated to the specified datasource
  int itemIdx = 0;
  MeshHandlerMapIterator meshIt;
  for ( meshIt=_meshHandlerMap.begin(); meshIt != _meshHandlerMap.end(); meshIt++) {
    if ( meshIt->second->sourceid == datasourceId ) {
      meshHandlerList[itemIdx] = *(meshIt->second);
      itemIdx++;
    }
  }

  // Adjust the length to the real number of elements
  meshHandlerList->length(itemIdx);
  return meshHandlerList._retn();
}

/*!
 * This function returns the list of fieldseries defined on the
 * specified mesh.
 */
MEDOP::FieldseriesHandlerList * MEDDataManager_i::getFieldseriesListOnMesh(CORBA::Long meshId) {
  // We initiate a list with the maximum lentgh
  MEDOP::FieldseriesHandlerList_var
    fieldseriesHandlerList = new MEDOP::FieldseriesHandlerList();
  fieldseriesHandlerList->length(_fieldseriesHandlerMap.size());

  // Scan the map looking for fieldseries defined on the specified mesh
  int itemIdx = 0;
  FieldseriesHandlerMapIterator it;
  for ( it=_fieldseriesHandlerMap.begin(); it != _fieldseriesHandlerMap.end(); it++) {
    if ( it->second->meshid == meshId ) {
      fieldseriesHandlerList[itemIdx] = *(it->second);
      itemIdx++;
    }
  }

  // Adjust the length to the real number of elements
  fieldseriesHandlerList->length(itemIdx);
  return fieldseriesHandlerList._retn();
}

/*!
 * A fieldseries is a timeseries of fields. Then the list of fields is
 * the different time iterations defined for the specified field id.  
 */
MEDOP::FieldHandlerList * MEDDataManager_i::getFieldListInFieldseries(CORBA::Long fieldseriesId) {

  // We initiate a list with the maximum lentgh
  MEDOP::FieldHandlerList_var fieldHandlerList = new MEDOP::FieldHandlerList();
  fieldHandlerList->length(_fieldHandlerMap.size());

  // Scan the map looking for field defined on the specified mesh
  int itemIdx = 0;
  FieldHandlerMapIterator it;
  for ( it=_fieldHandlerMap.begin(); it != _fieldHandlerMap.end(); it++) {
    if ( it->second->fieldseriesId == fieldseriesId ) {
      fieldHandlerList[itemIdx] = *(it->second);
      itemIdx++;
    }
  }

  // Adjust the length to the real number of elements
  fieldHandlerList->length(itemIdx);
  return fieldHandlerList._retn();
}

/*!
 * This returns the whole set of fields handlers for all datasource
 * that have been loaded using addDatasource.
 */
MEDOP::FieldHandlerList * MEDDataManager_i::getFieldHandlerList() {
  MEDOP::FieldHandlerList_var fieldHandlerSeq = new MEDOP::FieldHandlerList();
  fieldHandlerSeq->length(_fieldHandlerMap.size());

  int sequenceId = 0;
  FieldHandlerMapIterator fieldIt;
  for ( fieldIt=_fieldHandlerMap.begin(); fieldIt != _fieldHandlerMap.end(); fieldIt++) {
    fieldHandlerSeq[sequenceId] = *(fieldIt->second);
    sequenceId++;
  }
  return fieldHandlerSeq._retn();
}

/*!
 * This returns a copy of the fieldHandler associated to the specified id.
 */
MEDOP::FieldHandler * MEDDataManager_i::getFieldHandler(CORBA::Long fieldHandlerId) {
  LOG("getFieldHandler: START")

  FieldHandlerMapIterator fieldIt = _fieldHandlerMap.find(fieldHandlerId);
  if ( fieldIt != _fieldHandlerMap.end() ) {
    // >>> WARNING: CORBA struct specification indicates that the
    // assignement acts as a desctructor for the structure that is
    // pointed to. The values of the fields are copy first in the new
    // structure that receives the assignement and finally the initial
    // structure is destroyed. In the present case, WE WANT to keep
    // the initial fieldHandler in the map. We must then make a deep
    // copy of the structure found in the map and return the copy. The
    // CORBA struct specification indicates that a deep copy can be
    // done using the copy constructor.  <<<
    return new MEDOP::FieldHandler(*(fieldIt->second));
  }
  return NULL;
}

/*!
 * This returns a string representation of the field associated to the specified id.
 */
char * MEDDataManager_i::getFieldRepresentation(CORBA::Long fieldHandlerId) {
  LOG("getFieldRepresentation: START")
  MEDOP::FieldHandler * fieldHandler = getFieldHandler(fieldHandlerId);
  MEDCouplingFieldDouble* fieldDouble = getFieldDouble(fieldHandler);
  return CORBA::string_dup(fieldDouble->getArray()->repr().c_str());
}

void MEDDataManager_i::saveFields(const char * filepath,
				  const MEDOP::FieldIdList & fieldIdList)
{
  LOG("saveFields to : " << filepath);

  // We first have to check if the target filepath is writable
  // (segmentation fault in med otherwise)
  if (!Kernel_Utils::IsWritable(Kernel_Utils::GetDirName(std::string(filepath)))) {
    std::string message =
      std::string("The target filepath ") +
      std::string(filepath) +
      std::string(" is not writable");
    LOG(message);
    throw KERNEL::createSalomeException(message.c_str());
  }

  if ( fieldIdList.length() == 0 ) {
    throw KERNEL::createSalomeException("No fields to save");
  }
  
  // Consider the first field to initiate the med file
  CORBA::Long fieldHandlerId = fieldIdList[0];
  MEDOP::FieldHandler * fieldHandler = getFieldHandler(fieldHandlerId);
  MEDCouplingFieldDouble* fieldDouble = getFieldDouble(fieldHandler);  

  try {
    bool writeFromScratch = true;
    MEDLoader::WriteField(filepath, fieldDouble, writeFromScratch);
    
    writeFromScratch = false;
    for(CORBA::ULong i=1; i<fieldIdList.length(); i++) {
      fieldHandlerId = fieldIdList[i];
      fieldHandler = getFieldHandler(fieldHandlerId);
      fieldDouble = getFieldDouble(fieldHandler);  
      MEDLoader::WriteField(filepath, fieldDouble, writeFromScratch);
    }
  }
  catch (INTERP_KERNEL::Exception &ex) {
    std::string message = 
      std::string("Error when saving file ") +
      std::string(filepath) + std::string(" : ") + ex.what();
    throw KERNEL::createSalomeException(message.c_str());
  }
  catch (const std::exception& ex) {
    std::string message = 
      std::string("Error when saving file ") +
      std::string(filepath) + std::string(" : ") + ex.what();
    throw KERNEL::createSalomeException(message.c_str());
  }

}

/*!
 * This function must be used to indicate that the field with the
 * specified id must be considered as persistent (if persistent is
 * true) or not persistent (if persistent is false). If a field is
 * marked as persistent, then it is automatically saved when the
 * function savePersistentFields is called.
 */
void MEDDataManager_i::markAsPersistent(CORBA::Long fieldHandlerId, bool persistent) {
  LOG("mark as persistant : id="<<fieldHandlerId);
  _fieldPersistencyMap[fieldHandlerId] = persistent;
}

void MEDDataManager_i::savePersistentFields(const char * filepath) {
  LOG("savePersistentFields to : " << filepath);
  std::vector<long> listId;

  FieldPersistencyMapIterator mapIt;
  for ( mapIt = _fieldPersistencyMap.begin(); mapIt != _fieldPersistencyMap.end(); mapIt++) {
    if ( mapIt->second == true ) {
      listId.push_back(mapIt->first);
    }
  }
  
  MEDOP::FieldIdList fieldIdList;
  fieldIdList.length(listId.size());
  for (int i=0; i<listId.size(); i++) {
    fieldIdList[i] = CORBA::Long(listId[i]);
  }
  
  try {
    this->saveFields(filepath, fieldIdList);
  } 
  catch (const SALOME::SALOME_Exception & ex) {
    throw ex;
  }
  catch (const std::exception& ex) {
    std::string message = 
      std::string("Error when saving file ") +
      std::string(filepath) + std::string(" : ") + ex.what();
    throw KERNEL::createSalomeException(message.c_str());
  }
}

/*!
 * This function is responsible for creating the FieldHandler
 * instances. You must use this function because it manages
 * automatically the identifier value (autoincrementation of a static
 * variable)
 */
MEDOP::FieldHandler * MEDDataManager_i::newFieldHandler(const char * fieldname,
							const char * meshname,
							TypeOfField  type,
							long         iteration,
							long         order,
							const char * source)
{
  MEDOP::FieldHandler * fieldHandler = new MEDOP::FieldHandler();
  fieldHandler->id = _fieldLastId; _fieldLastId++;
  fieldHandler->fieldname = fieldname;
  fieldHandler->meshname  = meshname;
  fieldHandler->type      = type;
  fieldHandler->iteration = iteration;
  fieldHandler->order     = order;
  fieldHandler->source    = source;
  return fieldHandler;
}

/*!
 * This updates the metadata of the field identified by its id with
 * the data of the given field handler. Returns a copy of the updated
 * handler (that should be identical to the given field handler for
 * all data but not for the id that is an invariant for all session
 * long).
 * WARN: you should be warned that this function could leave the data
 * model in a non-coherent state, by example if you change the mesh
 * name while the mesh has not been updated.
 */
MEDOP::FieldHandler * MEDDataManager_i::updateFieldHandler(CORBA::Long fieldHandlerId,
							   const char * fieldname,
							   long         iteration,
							   long         order,
							   const char * source) {
  FieldHandlerMapIterator fieldIt = _fieldHandlerMap.find(fieldHandlerId);
  if ( fieldIt != _fieldHandlerMap.end() ) {
    // Update the attributes
    // >>> WARN: note that the id of a handler registered in the map
    // SHOULD NEVER be modified because it is the identifier used in
    // the whole application for this field all the session long.
    // <<<
    fieldIt->second->fieldname = fieldname;
    fieldIt->second->iteration = iteration;
    fieldIt->second->order     = order;
    fieldIt->second->source    = source;
    // Return a copy
    return new MEDOP::FieldHandler(*fieldIt->second);
  }
  return NULL;
}

MEDCouplingUMesh * MEDDataManager_i::getUMesh(long meshHandlerId) {

  LOG("getUMesh: START")

  MEDCouplingUMesh * myMesh = NULL;
  if ( _meshMap.count(meshHandlerId) > 0 ) {
    // The mesh has been found in the map
    myMesh = _meshMap[meshHandlerId];
  } else {
    // The mesh is not loaded yet ==> load it and register it in the map
    LOG("getUMesh: the mesh must be loaded. meshid="<<meshHandlerId);    
    if ( _meshHandlerMap[meshHandlerId] == NULL ) {
      std::string message = 
	std::string("No mesh for id=") + ToString(meshHandlerId);
      LOG("getUMesh: "<<message);
      throw KERNEL::createSalomeException(message.c_str());
    }

    long sourceid = _meshHandlerMap[meshHandlerId]->sourceid;
    const char * filepath = source_to_file((_datasourceHandlerMap[sourceid])->uri);  
    const char * meshName = _meshHandlerMap[meshHandlerId]->name;
    int meshDimRelToMax = 0;
    myMesh = MEDLoader::ReadUMeshFromFile(filepath,meshName,meshDimRelToMax);
    _meshMap[meshHandlerId] = myMesh;
  }
  return myMesh;
}

/**
 * Try to retrieve the id of the specified mesh, i.e. the key it is
 * registered with in the internal meshes map.
 */
long MEDDataManager_i::getUMeshId(const MEDCouplingMesh * mesh) {
  bool found = false;
  MeshMapIterator it = _meshMap.begin();  
  while ( it != _meshMap.end() ) {
    found = (it->second == mesh);
    if (found) {
      return it->first;
    }
    ++it;
  }
  return LONG_UNDEFINED;
}

/*!
 * This method returns the physical data of the specified field,
 * i.e. the MEDCoupling field associated to the specified field
 * handler. If the field source is a file and the data ar not loaded
 * yet, the this function load the data from the file in a MEDCoupling
 * field instance. Otherwize, it just returns the MEDCoupling field
 * instance.
 */
MEDCouplingFieldDouble * MEDDataManager_i::getFieldDouble(const MEDOP::FieldHandler * fieldHandler)
{

  LOG("getFieldDouble: START with id="<<fieldHandler->id);

  if ( _fieldDoubleMap.count(fieldHandler->id) > 0 ) {
  // The MEDCoupling field data are already loaded. Just return the
  // reference of the MEDCouplingFieldDouble pointer
    return _fieldDoubleMap[fieldHandler->id];
  }

  // The MEDCoupling field data are not loaded yet. Load the data and
  // register the MEDCoupling field in our internal map an all the
  // associated data if needed (i.e. the underlying mesh).

  // At this step, the mesh handler needs a meshid correctly
  // set. Normally, we should arrive at this step only in the case
  // where the field is loaded from a file ==> the meshid is defined
  // (see the addDatasource function).
  //
  // >>>> __GBO__ TO BE CHECKED AND SERIOUSLY TESTED. There at least
  // one case where we can arrive here with no previous call to
  // addDataSource: for example the field handler list can be obtained
  // from a call to addFieldsFromFile instead of addDataSource (see
  // for exemple the getFieldRepresentation service of the
  // dataManager, that comes here and then calls getUMesh where we
  // need a map initialized only in addDataSource) <<<<
  long meshid = fieldHandler->meshid;

  // We first have to check if the associated mesh is already loaded
  // and to load it if needed. The loaded meshes are registered in a
  // map whose key is the mesh handler id. This checking is
  // automatically done by the function getUMesh. It's important to do
  // it before the loading of field data to prevent from the case
  // where the mesh would not have been loaded already (in the
  // previous field loading).
  MEDCouplingUMesh * myMesh =this->getUMesh(meshid);

  long sourceid = _meshHandlerMap[meshid]->sourceid;

  const char * filepath = source_to_file((_datasourceHandlerMap[sourceid])->uri);
  const char * meshName = myMesh->getName();
  LOG("getFieldDouble: field "<<fieldHandler->fieldname<<" loaded from file "<<filepath);
  TypeOfField type = (TypeOfField)fieldHandler->type;
  int meshDimRelToMax = 0;
  MEDCouplingFieldDouble * myField = MEDLoader::ReadField(type,
							  filepath,
							  meshName,
							  meshDimRelToMax,
							  fieldHandler->fieldname,
							  fieldHandler->iteration,
							  fieldHandler->order);
  myField->setMesh(myMesh);
  _fieldDoubleMap[fieldHandler->id] = myField;
  return myField;
}

/*!
 * This adds the specified MEDCoupling field in the collection managed
 * by this DataManager. The associated FieldHandler is returned. This
 * is typically used in a context where the MEDCoupling field is
 * created from scratch, for example by operations in the
 * MEDCalculator.
 * @param[in] fieldDouble   the MEDCouplingFieldDouble instance to add
 * @param[in] meshHandlerId the id of the meshHandler this filed is associated to.
 * @return a copy of the FieldHandler registered in the internal map for this field.
 */
MEDOP::FieldHandler * MEDDataManager_i::addField(MEDCouplingFieldDouble * fieldDouble,
						 long meshHandlerId)
{
  const char * fieldName = fieldDouble->getName();
  const char * meshName  = fieldDouble->getMesh()->getName();
  TypeOfField  type      = fieldDouble->getTypeOfField();

  int iteration, order;
  // WARN: note that the variables "iteration" and "order" are passed
  // by reference to the function getTime (see documentation of
  // MEDCouplingField). As a consequence, the values of these
  // variables are updated by this function call. This is the mean to
  // retrieve the iteration and order of the field.
  double timestamp = fieldDouble->getTime(iteration, order);
  
  // For the fields that are created in memory (by operations for
  // example), the convention for the source attribute is to specify
  // the fielddouble name, because this name describes the operation
  // the field has been created with.
  string * source = new string("mem://"); source->append(fieldName);
  MEDOP::FieldHandler * fieldHandler = newFieldHandler(fieldName,
						       meshName,
						       type,
						       iteration,
						       order,
						       source->c_str());

  if ( meshHandlerId == LONG_UNDEFINED ) {
    // We have to gess the id of the underlying mesh to preserve data
    // integrity (a fieldHandler must have an attribute that contains
    // the id of its underlying mesh):
    //
    // WARNING: it's better to let the client code (the one who calls the
    // function addField) to specify this meshid. This guess procedure is
    // not reliable, it's just to have a second chance.
    //
    LOG("addField: The mesh id is not defined. Trying to guess from the mesh name "<<meshName);
    long meshid = this->getUMeshId(fieldDouble->getMesh());
    fieldHandler->meshid = meshid;
    if ( meshid == LONG_UNDEFINED ) {
      // No mesh has been found in the internal map
      LOG("addField: The mesh id for the mesh "<<meshName<<" can't be retrieved from the field "<<fieldName);
      // _GBO_ : Maybe it could be better to raise an exception
    }
  }
  else {
    fieldHandler->meshid = meshHandlerId;
  }
  
  _fieldHandlerMap[fieldHandler->id] = fieldHandler;
  _fieldDoubleMap[fieldHandler->id] = fieldDouble;
  // >>> WARNING: CORBA structure assignement specification ==> return
  // >>> a deep copy to avoid the destruction of the fieldHandler
  // >>> registered in the map (assignement acts as a destructor for
  // >>> CORBA struct).   
  return new MEDOP::FieldHandler(*fieldHandler);
}

/*!
 * This function updates the meta-data "fieldname" associated to the
 * specified field.
 */
void MEDDataManager_i::updateFieldMetadata(CORBA::Long  fieldHandlerId,
					   const char * fieldname,
					   CORBA::Long  iteration,
					   CORBA::Long  order,
					   const char * source)
{
  // We have to update the field handler registered in the internal
  // map AND the associated fieldDouble loaded in memory.
  MEDOP::FieldHandler * fieldHandler = getFieldHandler(fieldHandlerId);
  updateFieldHandler(fieldHandlerId,fieldname,iteration,order,source);
		     
  MEDCouplingFieldDouble* fieldDouble = getFieldDouble(fieldHandler);
  fieldDouble->setName(fieldname);

  // _GBO_ TO BE IMPLEMENTED: iteration and order
}

/**
 * This can be used to associate to the specified field another mesh
 * support than its current one. This is typically needed to operate 2
 * fields defined on the same mesh but coming from different med
 * files. In this case, the underlying meshes are different mesh
 * objects (from the MEDCoupling point of view) and then no operation
 * can be allowed by MEDCoupling. The operation of course fails if the
 * new mesh is not identical to the old one.
 */
void MEDDataManager_i::changeUnderlyingMesh(CORBA::Long fieldHandlerId, CORBA::Long meshHandlerId) {
  
  MEDOP::FieldHandler * fieldHandler = getFieldHandler(fieldHandlerId);
  MEDCouplingFieldDouble* fieldDouble = getFieldDouble(fieldHandler);
  MEDCouplingMesh * newMesh = getUMesh(meshHandlerId);

  try {
    fieldDouble->changeUnderlyingMesh(newMesh,10,1e-12);
  }
  catch (INTERP_KERNEL::Exception &ex) {
    std::string * message = new std::string("Error when changing the underlying mesh : ");
    message->append(ex.what());
    throw KERNEL::createSalomeException(message->c_str());
  }

  // The change of mesh is OK, then we can update the meta-data
  _fieldHandlerMap[fieldHandlerId]->meshid = meshHandlerId;
  _fieldHandlerMap[fieldHandlerId]->meshname = _meshHandlerMap[meshHandlerId]->name;


  // WARN: if this field has already been request by the tui for
  // manipulation (in a fieldproxy), then the data should be
  // synchronized
}



/*!
 * This functions display the internal data of the MEDDataManager on
 * the server side (data in the SALOME container).
 */
void MEDDataManager_i::serverlog() {
  
  LOG("==== Field Handler Map ====================================================");
  LOG("Size = "<<_fieldHandlerMap.size());
  FieldHandlerMapIterator fhmIt;
  for ( fhmIt = _fieldHandlerMap.begin(); fhmIt != _fieldHandlerMap.end(); fhmIt++) {
    long id = fhmIt->first;
    LOG("------------------------------------- id = "<<ToString(id));
    LOG("- id        \t= "<<fhmIt->second->id);
    LOG("- fieldname \t= "<<fhmIt->second->fieldname);
    LOG("- meshname  \t= "<<fhmIt->second->meshname);
  }

  LOG("==== Field Double Map ====================================================");
  LOG("Size = "<<_fieldDoubleMap.size());
  FieldDoubleMapIterator fdmIt;
  for ( fdmIt = _fieldDoubleMap.begin(); fdmIt != _fieldDoubleMap.end(); fdmIt++) {
    long id = (*fdmIt).first;
    MEDCouplingFieldDouble * fieldDouble = (*fdmIt).second;
    LOG("------------------------------------- id = "<<ToString(id));
    LOG("- fieldname \t= "<<fieldDouble->getName());
    LOG("- meshname  \t= "<<fieldDouble->getMesh()->getName());
  }
}

/*!
 * The event listener is created inside the GUI by the
 * WorkspaceController. This function is called by the WorkspaceController to
 * store the event listener IOR for the time of the session. Then this
 * IOR can be available to any point of the application that can
 * request the data manager (the python console for example).
 */
void MEDDataManager_i::setEventListenerIOR(const char * ior) {
  _medEventListenerIOR = ior;
}
/*!
 * Return the IOR of the event listener that resides in the
 * GUI. Having the IOR, you can restore the CORBA object by using:
 *
 * In a python SALOME context:
 *
 * >>> import salome
 * >>> salome.salome_init()
 * >>> myobject = salome.orb.string_to_object(ior)
 *
 * In a C++ SALOME context: (to do if needed)
 */
char * MEDDataManager_i::getEventListenerIOR() {
  if ( _medEventListenerIOR == "" ) {
    throw KERNEL::createSalomeException("The event listener IOR is not defined");
  }
  // WARN: return a copy because the pointer memory will be released
  // (CORBA specification)
  return CORBA::string_dup( _medEventListenerIOR.c_str() );
}
