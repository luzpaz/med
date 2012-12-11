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

#ifndef _MED_DATAMANAGER_I_HXX_
#define _MED_DATAMANAGER_I_HXX_

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MEDDataManager)
#include "SALOME_GenericObj_i.hh"

#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingFieldDouble.hxx"
using namespace ParaMEDMEM;

/*! This map is used for lifecycle management of fields used in this operator */
#include <map>
#include <string>
using namespace std;

typedef map<long,MEDOP::DatasourceHandler*> DatasourceHandlerMap;
typedef map<long,MEDOP::DatasourceHandler*>::iterator DatasourceHandlerMapIterator;

typedef map<long,MEDOP::MeshHandler*> MeshHandlerMap;
typedef map<long,MEDOP::MeshHandler*>::iterator MeshHandlerMapIterator;

typedef map<long,MEDOP::FieldseriesHandler*> FieldseriesHandlerMap;
typedef map<long,MEDOP::FieldseriesHandler*>::iterator FieldseriesHandlerMapIterator;

typedef map<long,MEDOP::FieldHandler*> FieldHandlerMap;
typedef map<long,MEDOP::FieldHandler*>::iterator FieldHandlerMapIterator;
typedef map<long,bool> FieldPersistencyMap;
typedef map<long,bool>::iterator FieldPersistencyMapIterator;

typedef map<long,MEDCouplingFieldDouble*> FieldDoubleMap;
typedef map<long,MEDCouplingFieldDouble*>::iterator FieldDoubleMapIterator;

typedef map<long,MEDCouplingUMesh*> MeshMap;
typedef map<long,MEDCouplingUMesh*>::iterator MeshMapIterator;

#include "MEDOP.hxx"
class MEDOP_EXPORT MEDDataManager_i: public POA_MEDOP::MEDDataManager,
				     public SALOME::GenericObj_i
{
public:

  // 
  // ===========================================================
  // Functions specified in the IDL interface
  // ===========================================================
  //
  // -----------------------------------------------------------
  // Datasource management
  MEDOP::DatasourceHandler * addDatasource(const char *filepath);

  // -----------------------------------------------------------
  // Mesh management
  MEDOP::MeshHandlerList * getMeshList(CORBA::Long datasourceId);
  MEDOP::MeshHandler * getMesh(CORBA::Long meshId); 

  // -----------------------------------------------------------
  // Field management
  MEDOP::FieldseriesHandlerList * getFieldseriesListOnMesh(CORBA::Long meshId);
  MEDOP::FieldHandlerList * getFieldListInFieldseries(CORBA::Long fieldseriesId);

  MEDOP::FieldHandler *     getFieldHandler(CORBA::Long fieldHandlerId);
  char *                    getFieldRepresentation(CORBA::Long fieldHandlerId);
  MEDOP::FieldHandlerList * getFieldHandlerList();

  void                      saveFields(const char * filepath,
				       const MEDOP::FieldIdList & fieldIdList);
  void                      markAsPersistent(CORBA::Long fieldHandlerId, bool persistent);
  void                      savePersistentFields(const char * filepath);


  void updateFieldMetadata(CORBA::Long  fieldHandlerId,
			   const char * fieldname,
			   CORBA::Long  iteration,
			   CORBA::Long  order,
			   const char * source);

  void changeUnderlyingMesh(CORBA::Long fieldHandlerId, CORBA::Long meshHandlerId);

  void   setEventListenerIOR(const char * ior);
  char * getEventListenerIOR();

  void serverlog();

  // 
  // ===========================================================
  // Other public functions (non available via CORBA)
  // ===========================================================
  //
  static long LONG_UNDEFINED;
  static MEDDataManager_i * getInstance();
  MEDCouplingFieldDouble *  getFieldDouble(const MEDOP::FieldHandler * fieldHandler);
  MEDOP::FieldHandler *     addField(MEDCouplingFieldDouble * fieldDouble,
				     long meshHandlerId=LONG_UNDEFINED);

private:
  MEDDataManager_i();
  ~MEDDataManager_i();

  // The MEDDataManager is a singleton, whose instance can be obtained
  // using the getInstance static method.
  static MEDDataManager_i * _instance;

  long _sourceLastId;
  long _meshLastId;
  long _fieldseriesLastId;
  long _fieldLastId;
  DatasourceHandlerMap _datasourceHandlerMap;
  MeshHandlerMap _meshHandlerMap;
  FieldseriesHandlerMap _fieldseriesHandlerMap;
  FieldHandlerMap _fieldHandlerMap;
  FieldDoubleMap _fieldDoubleMap;
  MeshMap _meshMap;
  FieldPersistencyMap _fieldPersistencyMap;

  string _medEventListenerIOR;

  MEDOP::FieldHandler * newFieldHandler(const char * fieldname,
					const char * meshname,
					TypeOfField  type,
					long         iteration,
					long         order,
					const char * source);

  MEDOP::FieldHandler * updateFieldHandler(CORBA::Long fieldHandlerId,
					   const char * fieldname,
					   long         iteration,
					   long         order,
					   const char * source);

  const char * file_to_source(const char * filepath);
  const char * source_to_file(const char * source);
  long getDatasourceId(const char *filepath);

  MEDCouplingUMesh * getUMesh(long meshHandlerId);
  long getUMeshId(const MEDCouplingMesh * mesh);
};

#endif // _MED_DATAMANAGER_I_HXX_
