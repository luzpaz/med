// Copyright (C) 2007-2012  CEA/DEN, EDF R&D
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
 
#include "MEDMEM_ConnectZone.hxx"
#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_GaussLocalization.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_MeshFuse.hxx"

#include "MEDMEM_Exception.hxx"

#include "MEDSPLITTER_utils.hxx" 

#include "MEDSPLITTER_Graph.hxx"

#include "MEDSPLITTER_Topology.hxx"
#include "MEDSPLITTER_ParallelTopology.hxx"
#include "MEDSPLITTER_SequentialTopology.hxx"
#include "MEDSPLITTER_ParaDomainSelector.hxx"
#include "MEDSPLITTER_MeshSendReceive.hxx"
#include "MEDSPLITTER_JointExchangeData.hxx"

#include "MEDSPLITTER_MESHCollection.hxx"
#include "MEDSPLITTER_MESHCollectionDriver.hxx"
#include "MEDSPLITTER_MESHCollectionMedXMLDriver.hxx"
#include "MEDSPLITTER_MESHCollectionMedAsciiDriver.hxx"

#include "MEDSPLITTER_UserGraph.hxx"

#if defined(MED_ENABLE_METIS) || defined(MED_ENABLE_PARMETIS)
#include "MEDSPLITTER_METISGraph.hxx"
#endif
#ifdef MED_ENABLE_SCOTCH
#include "MEDSPLITTER_SCOTCHGraph.hxx"
#endif

#include "InterpKernelHashMap.hxx"

#include <vector>
#include <string>
#include <set>

#include <iostream>
#include <fstream>

using namespace MEDSPLITTER;

using namespace INTERP_KERNEL;

//template inclusion
#include "MEDSPLITTER_MESHCollection.H"

MESHCollection::MESHCollection()
  : _topology(0),
    _owns_topology(false),
    _driver(0),
    _domain_selector( 0 ),
    _i_non_empty_mesh(-1),
    _driver_type(MEDSPLITTER::MedXML),
    _subdomain_boundary_creates(false),
    _family_splitting(false),
    _create_empty_groups(false)
{
}

namespace
{
  //================================================================================
  /*!
   * \brief Creates a new domain mesh 
   */
  //================================================================================

  MEDMEM::MESH* newMesh(const std::string& name, int dim, int space, MEDMEM::MESH* meshToDelete=0)
  {
    delete meshToDelete;
    MEDMEM::MESHING* mesh = new MEDMEM::MeshFuse;
    mesh->setName( name );
    //mesh->setMeshDimension ( dim );
    //mesh->setSpaceDimension( space );
    return mesh;
  }
}

/*!constructor creating a new mesh collection (mesh series + topology)
 *from an old collection and a new topology
 *
 * On output, the constructor has built the meshes corresponding to the new mesh collection.
 * The new topology has been updated so that face and node mappings are included.
 * The families have been cast to their projections in the new topology.
 *
 * \param initial_collection collection from which the data (coordinates, connectivity) are taken
 * \param topology topology containing the cell mappings
 */

MESHCollection::MESHCollection(const MESHCollection& initial_collection, Topology* topology, bool family_splitting, bool create_empty_groups)
  : _topology(topology),
    _owns_topology(false),
    _cell_graph(topology->getGraph()),
    _driver(0),
    _domain_selector( initial_collection._domain_selector ),
    _i_non_empty_mesh(-1),
    _name(initial_collection._name),
    _driver_type(MEDSPLITTER::MedXML),
    _subdomain_boundary_creates(false),
    _family_splitting(family_splitting),
    _create_empty_groups(create_empty_groups)
{
  string mesh_name = initial_collection.getName();
  _mesh.resize(_topology->nbDomain());

  int space_dim = initial_collection.getSpaceDimension();
  int mesh_dim  = initial_collection.getMeshDimension();
  if ( mesh_dim < 1 )
    space_dim = mesh_dim = initial_collection._driver->readMeshDimension();

  for (int idomain=0; idomain < _topology->nbDomain(); idomain++)
  {
    //creating the new mesh
    _mesh[idomain]= newMesh( MEDMEM::STRING(mesh_name)<<"_"<<idomain+1, mesh_dim, space_dim );

    createNodalConnectivity(initial_collection,idomain, MED_EN::MED_CELL);

    if ( _mesh[idomain]->getNumberOfNodes() > 0 )
      _i_non_empty_mesh = idomain;
  }

  _topology->createFaceMapping(initial_collection, *this ); 
  for (int idomain=0; idomain<_topology->nbDomain(); idomain++)
  {
    switch (mesh_dim)
    {
    case 3:
      createNodalConnectivity(initial_collection,idomain, MED_EN::MED_FACE);
      break;
    case 2:
      createNodalConnectivity(initial_collection,idomain, MED_EN::MED_EDGE);
      break;
    default :
      if ( !isParallelMode() || _domain_selector->isMyDomain( idomain ))
        cerr<<"MEDSPLITTER : Mesh dimension must be 2 or 3"<<endl;
    }
  }

  castFamilies(initial_collection);

  // Exchange domain parts

  if ( isParallelMode() )
  {
    _domain_selector->setNbDomains( _topology->nbDomain() );

    vector< MeshSendReceive > mesh_sender( _topology->nbDomain() );
    list<int> domainsToClear; // sent domains
    bool isSent;
    // first, send domains
    for (int idomain=0; idomain<_topology->nbDomain(); idomain++)
    {
      // get node numbers global over all procs
      vector<int> glob_nodes_all_proc( _topology->getNodeNumber( idomain )); // to fill in
      vector<int> glob_cells_all_proc( _topology->getCellNumber( idomain ));
      vector<int> glob_faces_all_proc( _topology->getFaceNumber( idomain ));
      if ( !glob_cells_all_proc.empty() )
      {
        // get ids global on this proc
        _topology->getNodeList( idomain, & glob_nodes_all_proc[0] );
        _topology->getCellList( idomain, & glob_cells_all_proc[0] );
        _topology->getFaceList( idomain, & glob_faces_all_proc[0] );
        // convert cell ids to ids global over all procs
        int cell_shift = _domain_selector->getProcShift();
        for ( int i = 0; i < glob_cells_all_proc.size(); ++i )
          glob_cells_all_proc[i] += cell_shift;
      }
      if ( _domain_selector->isMyDomain( idomain ))
      {
        // prepare to receiving other parts of the domain
        ((MEDMEM::MeshFuse*) _mesh[idomain])->setNodeNumbers( glob_nodes_all_proc );
        _topology->getFusedCellNumbers( idomain ) = glob_cells_all_proc;
        _topology->getFusedFaceNumbers( idomain ) = glob_faces_all_proc;
      }
      else
      {
        // sending
        int target_proc = _domain_selector->getProccessorID( idomain );
        mesh_sender[ idomain ].send( target_proc, idomain, _mesh[idomain],
                                     glob_cells_all_proc,
                                     glob_faces_all_proc,
                                     glob_nodes_all_proc );
        if ( !glob_nodes_all_proc.empty() )
          domainsToClear.push_back( idomain );
      }
      // clear just sent domain meshes
      for ( list<int>::iterator dom = domainsToClear.begin(); dom != domainsToClear.end(); )
      {
        if (( isSent = mesh_sender[ *dom ].isSent() ))
          _mesh[*dom] = newMesh( _mesh[*dom]->getName(), mesh_dim, space_dim, _mesh[*dom]);
        dom = isSent ? domainsToClear.erase( dom ) : ++dom;
      }
    }

    // then, receive domains
    MeshSendReceive mesh_receiver;
    int this_proc = _domain_selector->rank();
    for (int idomain=0; idomain<_topology->nbDomain(); idomain++)
    {
      if ( _domain_selector->isMyDomain( idomain ))
      {
        for (int iproc = 0; iproc < _domain_selector->nbProcs(); ++iproc)
        {
          if ( iproc == this_proc ) continue;
          vector<int>  nodes_other_proc, cells_other_proc, faces_other_proc;
          mesh_receiver.recv( iproc, idomain, cells_other_proc, faces_other_proc,nodes_other_proc);
          if ( MEDMEM::MESH* received_mesh = mesh_receiver.getMesh() )
          {
            // unite meshes and global node numbers stored in MeshFuse
            MEDMEM::MeshFuse* fuse = (MEDMEM::MeshFuse*) _mesh[idomain];
            fuse->concatenate( received_mesh, nodes_other_proc );
            delete received_mesh;

            // unite global element numbers
            fuse->append( MED_EN::MED_CELL,
                          _topology->getFusedCellNumbers( idomain ), cells_other_proc );

            fuse->append( mesh_dim==3 ? MED_EN::MED_FACE : MED_EN::MED_EDGE,
                          _topology->getFusedFaceNumbers( idomain ), faces_other_proc );

            if ( _mesh[idomain]->getNumberOfNodes() > 0 )
              _i_non_empty_mesh = idomain;
          }
        }
      }
      // clear just sent domain meshes
      for ( list<int>::iterator dom = domainsToClear.begin(); dom != domainsToClear.end(); )
      {
        if (( isSent = mesh_sender[ *dom ].isSent() ))
          _mesh[*dom] = newMesh( _mesh[*dom]->getName(), mesh_dim, space_dim,_mesh[*dom]);
        dom = isSent ? domainsToClear.erase( dom ) : ++dom;
      }
    }
    // clear sent domain meshes
    mesh_sender.clear();
    for ( list<int>::iterator dom = domainsToClear.begin(); dom != domainsToClear.end(); ++dom)
      _mesh[*dom] = newMesh( _mesh[*dom]->getName(), mesh_dim, space_dim,_mesh[*dom]);

    _topology->recreateMappingAfterFusion( getMesh() );
  }
  if ( _i_non_empty_mesh < 0 ) // non of domains resides on this proc,
    _i_non_empty_mesh = 0; // in this case we need only dimension that is set to all meshes

}

/*! constructing the MESH collection from a distributed file
 *
 * \param filename name of the master file containing the list of all the MED files
 */
MESHCollection::MESHCollection(const string& filename)
  : _topology(0),
    _owns_topology(true),
    _driver(0),
    _domain_selector( 0 ),
    _i_non_empty_mesh(-1),
    _driver_type(MEDSPLITTER::Undefined),
    _subdomain_boundary_creates(false),
    _family_splitting(false),
    _create_empty_groups(false)
{
  char filenamechar[256];
  strcpy(filenamechar,filename.c_str());
  try
  {
    _driver=new MESHCollectionMedXMLDriver(this);
    _driver->read (filenamechar);
    _driver_type = MedXML;

  }
  catch(MEDEXCEPTION&){
    delete _driver;
    try
    {
      _driver=new MESHCollectionMedAsciiDriver(this);
      _driver->read (filenamechar);
      _driver_type=MedAscii;
    }
    catch(MEDEXCEPTION&)
    {
      delete _driver;
      throw MEDEXCEPTION("file does not comply with any recognized format");
    }
  }
  for ( int idomain = 0; idomain < _mesh.size(); ++idomain )
    if ( _mesh[idomain] && _mesh[idomain]->getNumberOfNodes() > 0 )
      _i_non_empty_mesh = idomain;
}

/*! Constructing the MESH collection from selected domains of a distributed file
 * 
 * \param filename  - name of the master file containing the list of all the MED files
 * \param domainSelector - selector of domains to load
 */
MESHCollection::MESHCollection(const string& filename, ParaDomainSelector& domainSelector)
  : _topology(0),
    _owns_topology(true),
    _driver(0),
    _domain_selector( domainSelector.nbProcs() > 1 ? & domainSelector : 0 ),
    _i_non_empty_mesh(-1),
    _driver_type(MEDSPLITTER::Undefined),
    _subdomain_boundary_creates(false),
    _family_splitting(false),
    _create_empty_groups(false)
{
  try
  {
    _driver=new MESHCollectionMedXMLDriver(this);
    _driver->read ( (char*)filename.c_str(), _domain_selector );
    _driver_type = MedXML;
  }
  catch(MEDEXCEPTION&)
  {
    delete _driver;
    try
    {
      _driver=new MESHCollectionMedAsciiDriver(this);
      _driver->read ( (char*)filename.c_str(), _domain_selector );
      _driver_type=MedAscii;
    }
    catch(MEDEXCEPTION&)
    {
      delete _driver;
      throw MEDEXCEPTION("file does not comply with any recognized format");
    }
  }
  if ( isParallelMode() )
    // to know nb of cells on each proc to compute global cell ids from locally global
    _domain_selector->gatherNbOf( MED_EN::MED_CELL, getMesh() );

  // find non-empty domain mesh
  for ( int idomain = 0; idomain < _mesh.size(); ++idomain )
    if ( _mesh[idomain] && _mesh[idomain]->getNumberOfNodes() > 0 )
      _i_non_empty_mesh = idomain;
}

/*! constructing the MESH collection from a sequential MED-file
 *
 * \param filename MED file
 * \param meshname name of the mesh that is to be read
 */
MESHCollection::MESHCollection(const string& filename, const string& meshname)
  : _topology(0),
    _owns_topology(true),
    _driver(0),
    _domain_selector( 0 ),
    _i_non_empty_mesh(-1),
    _name(meshname),
    _driver_type(MEDSPLITTER::MedXML),
    _subdomain_boundary_creates(false),
    _family_splitting(false),
    _create_empty_groups(false)
{
  char filenamechar[256];
  char meshnamechar[256];
  strcpy(filenamechar,filename.c_str());
  strcpy(meshnamechar,meshname.c_str());
  try // avoid memory leak in case of inexistent filename
  {
    retrieveDriver()->readSeq (filenamechar,meshnamechar);
  }
  catch ( MED_EXCEPTION& e )
  {
    if ( _driver ) delete _driver; _driver=0;
    throw e;
  }
  if ( _mesh[0] && _mesh[0]->getNumberOfNodes() > 0 )
    _i_non_empty_mesh = 0;
}

MESHCollection::~MESHCollection()
{
  for (unsigned i=0; i<_mesh.size();i++)
    if (_mesh[i]!=0) {/*delete*/ _mesh[i]->removeReference(); }
  for (unsigned i=0; i<_connect_zones.size();i++)
    if (_connect_zones[i]!=0) {delete _connect_zones[i];}
  if (_driver !=0) {delete _driver; _driver=0;}
  if (_topology!=0 && _owns_topology) {delete _topology; _topology=0;}
}

/*!gets the connectivity for a certain type
 *
 * The output array type_connectivity should have been allocated
 * at dimension nbnode_per_type* nb_cells before the call
 *
 * \param cell_list list of elements (global cell numbers) for which the connectivity is required
 * \param nb_cells number of elements
 * \param entity type of entity for which the nodal connectivity is required
 * \param type type of the elements for which the connectivity is required
 * \param type_connectivity on output contains the connectivity of all the elements of the list
 * */ 

void MESHCollection::getNodeConnectivity(const int* cell_list,int nb_cells,MED_EN::medEntityMesh entity,
                                         MED_EN::medGeometryElement type, int* type_connectivity) const
{
  int *local=new int[nb_cells];
  int *ip=new int[nb_cells];
  switch (entity)
  {
  case MED_EN::MED_CELL:
    _topology->convertGlobalCellList(cell_list,nb_cells,local,ip);
    break;
  case MED_EN::MED_FACE:
  case MED_EN::MED_EDGE:
    _topology->convertGlobalFaceList(cell_list,nb_cells,local,ip);
    break;
  }


  //  int nbnode_per_type=(int)type%100;
  //  vector<int> number_of_types_array(_topology->nbDomain(),0);
  //  for (int i=0; i<_topology->nbDomain(); i++)
  //    number_of_types_array[i]=_mesh[i]->getNumberOfTypes(entity);

  //defining a connectivity table for different domains 
  vector  <const int*> conn_ip(_topology->nbDomain());
  vector  <const int*> conn_index_ip(_topology->nbDomain());


  vector< map <MED_EN::medGeometryElement, int> > offset;
  //  offset.resize(_topology->nbDomain());

  for (int i=0; i<_topology->nbDomain();i++)
  {
    if ( !_mesh[i] ) continue;
    int nb_elem = _mesh[i]->getNumberOfElements(entity,type);
    if (nb_elem>0)
    {
      conn_ip[i]=_mesh[i]->getConnectivity(MED_EN::MED_NODAL,entity,MED_EN::MED_ALL_ELEMENTS);
      conn_index_ip[i] = _mesh[i]->getConnectivityIndex(MED_EN::MED_NODAL,entity);
      //       global_index= _mesh[i]->getGlobalNumberingIndex(entity);
    }                                             
    else
    {
      conn_ip[i]=0;
      conn_index_ip[i]=0;
    }     
    //      int number_of_types = number_of_types_array[i];
    //      const MEDMEM::CELLMODEL* types =  _mesh[ip[icell]]->getCellsTypes(entity);
    //      for (int itype=0; itype<number_of_types; itype++) 
    //        offset[i][types[itype].getType()]=global_index[itype]-1;
  }

  int* type_connectivity_ptr=type_connectivity;
  for (int icell=0; icell<nb_cells; icell++)
  {
    //      int type_offset = offset[ip[icell]][type];
    const int* conn=conn_ip[ip[icell]]; 
    const int* conn_index=conn_index_ip[ip[icell]];
    for (int inode=conn_index[local[icell]-1]; inode<conn_index[local[icell]]; inode++)
    {
      *type_connectivity_ptr=
        _topology->convertNodeToGlobal(ip[icell],conn[inode-1]);
      type_connectivity_ptr++;                       
    }
  }

  delete[]local;
  delete[]ip;
}

/*!gets the connectivity for MED_POLYGON type
 *
 * \param cell_list list of elements (global cell numbers) for which the connectivity is required
 * \param nb_cells number of elements
 * \param entity type of entity for which the nodal connectivity is required
 * \param type_connectivity on output contains the connectivity of all the elements of the list
 * \param connectivity_index on output contains the connectivity index for all polygons 
 * */ 

void MESHCollection::getPolygonNodeConnectivity(const int* cell_list,int nb_cells,MED_EN::medEntityMesh entity,
                                                vector<int>& type_connectivity, vector<int>& connectivity_index) const
{

  int *local=new int[nb_cells];
  int *ip=new int[nb_cells];
  switch (entity)
  {
  case MED_EN::MED_CELL:
    _topology->convertGlobalCellList(cell_list,nb_cells,local,ip);
    break;
  case MED_EN::MED_FACE:
  case MED_EN::MED_EDGE:
    _topology->convertGlobalFaceList(cell_list,nb_cells,local,ip);
    break;
  }


  //defining a connectivity table for different domains 
  vector  <const int*> conn_ip(_topology->nbDomain());
  vector  <const int*> conn_index_ip(_topology->nbDomain());
  vector <const int* > conn_face_index(_topology->nbDomain());
  vector<int> nb_plain_elems(_topology->nbDomain());

  vector< map <MED_EN::medGeometryElement, int> > offset;

  for (int i=0; i<_topology->nbDomain();i++)
  {
    int nb_elem = _mesh[i]->getNumberOfElements(entity,MED_EN::MED_POLYGON);
    if (nb_elem>0)
    {
      conn_ip[i]=_mesh[i]->getConnectivity(MED_EN::MED_NODAL,entity,MED_EN::MED_ALL_ELEMENTS);
      conn_index_ip[i] = _mesh[i]->getConnectivityIndex(MED_EN::MED_NODAL,entity);
    }
    else
    {
      conn_ip[i]=0;
      conn_index_ip[i]=0;
    }     
  }

  connectivity_index.resize(nb_cells+1);
  connectivity_index[0]=1;
  for (int icell=0; icell<nb_cells; icell++)
  {
    //int nb_plain= nb_plain_elems[ip[icell]];
    const int* conn=conn_ip[ip[icell]]; 
    const int* conn_index=conn_index_ip[ip[icell]];
    for (int inode=conn_index[local[icell]-1/*-nb_plain*/]; inode<conn_index[local[icell]/*-nb_plain*/]; inode++)
    {
      type_connectivity.push_back(
                                  _topology->convertNodeToGlobal(ip[icell],conn[inode-1]));
    }
    connectivity_index[icell+1]=connectivity_index[icell]
      -conn_index[local[icell]-1/*-nb_plain*/]+conn_index[local[icell]/*-nb_plain*/];
  }

  delete[]local;
  delete[]ip;
}


/*!gets the connectivity for MED_POLYHEDRA type
 *
 * \param cell_list list of elements (global cell numbers) for which the connectivity is required
 * \param nb_cells number of elements
 * \param entity type of entity for which the nodal connectivity is required
 * \param type_connectivity on output contains the connectivity of all the elements of the list
 * \param connectivity_index on output contains the connectivity index for all polygons 
 * */ 

void MESHCollection::getPolyhedraNodeConnectivity(const int* cell_list,int nb_cells,MED_EN::medEntityMesh entity,
                                                  vector<int>& type_connectivity, vector<int>& connectivity_index/*, vector<int>& face_connectivity_index*/) const
{

  int *local=new int[nb_cells];
  int *ip=new int[nb_cells];
  switch (entity)
  {
  case MED_EN::MED_CELL:
    _topology->convertGlobalCellList(cell_list,nb_cells,local,ip);
    break;
  case MED_EN::MED_FACE:
  case MED_EN::MED_EDGE:
    _topology->convertGlobalFaceList(cell_list,nb_cells,local,ip);
    break;
  }


  //defining a connectivity table for different domains 
  vector  <const int*> conn_ip(_topology->nbDomain());
  vector  <const int*> conn_index_ip(_topology->nbDomain());
  vector<int> nb_plain_elems(_topology->nbDomain());

  vector< map <MED_EN::medGeometryElement, int> > offset;

  for (int i=0; i<_topology->nbDomain();i++)
  {
    nb_plain_elems[i] = _mesh[i]->getNumberOfElements(entity, MED_EN::MED_ALL_ELEMENTS);
    int nb_elem = _mesh[i]->getNumberOfElements(entity,MED_EN::MED_POLYHEDRA);
    if (nb_elem>0)
    {
      conn_ip[i]=_mesh[i]->getConnectivity(MED_EN::MED_NODAL,MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
      conn_index_ip[i] = _mesh[i]->getConnectivityIndex(MED_EN::MED_NODAL,MED_EN::MED_CELL);
    }
    else
    {
      conn_ip[i]=0;
      conn_index_ip[i]=0;
    }
  }

  connectivity_index.resize(nb_cells+1);
  connectivity_index[0]=1;
  for (int icell=0; icell<nb_cells; icell++)
  {
    const int* conn=conn_ip[ip[icell]]; 
    const int* conn_index=conn_index_ip[ip[icell]];
    connectivity_index[icell+1]=connectivity_index[icell]+
      conn_index[local[icell]]-conn_index[local[icell]-1];

    for (int inode=conn_index[local[icell]-1]; inode<conn_index[local[icell]]; inode++)
    {
      if ( conn[inode-1] == -1 )
        type_connectivity.push_back( -1 );
      else
        type_connectivity.push_back(_topology->convertNodeToGlobal(ip[icell],conn[inode-1]));
    }

  }

  delete[]local;
  delete[]ip;
}

/*! constructing the MESH collection from a file
 *
 * The method creates as many MED-files as there are domains in the 
 * collection. It also creates a master file that lists all the MED files.
 * The MED files created in ths manner contain joints that describe the 
 * connectivity between subdomains.
 *
 * \param filename name of the master file that will contain the list of the MED files
 *
 */
void MESHCollection::write(const string& filename)
{
  //building the connect zones necessary for writing joints
  cout<<"Building Connect Zones"<<endl;
  if (_topology->nbDomain()>1)
    buildConnectZones();
  cout <<"End of connect zones building"<<endl;
  //suppresses link with driver so that it can be changed for writing
  if (_driver!=0)delete _driver;
  _driver=0;

  char filenamechar[256];
  strcpy(filenamechar,filename.c_str());
  retrieveDriver()->write (filenamechar, _domain_selector);
}

/*! creates or gets the link to the collection driver
 */
MESHCollectionDriver* MESHCollection::retrieveDriver()
{
  if (_driver==0)
  {
    switch(_driver_type)
    {
    case MedXML:
      _driver=new MESHCollectionMedXMLDriver(this);
      break;
    case MedAscii:
      _driver=new MESHCollectionMedAsciiDriver(this);
      break;
    default:
      throw MEDEXCEPTION("Unrecognized driver");
    }
  }

  return _driver;
}


/*! gets an existing driver
 *
 */
MESHCollectionDriver* MESHCollection::getDriver() const
{
  return _driver;
}


/*! gets the list of types for global numbers cell_list
 *
 * \param cell_list list of global numbers
 * \param entity entity type
 * \param type_list on output, list of types for the cells given in cell_list
 */
void MESHCollection::getTypeList(int* cell_list,int nb_cells,
                                 MED_EN::medEntityMesh entity,
                                 MED_EN::medGeometryElement* type_list) const 
{
  MESSAGE_MED (" Beginning of getTypeList with entity "<<entity);
  int *local=new int[nb_cells];
  int *ip=new int[nb_cells];
  switch (entity)
  {
  case MED_EN::MED_CELL:
    _topology->convertGlobalCellList(cell_list,nb_cells,local,ip);
    break;
  case MED_EN::MED_FACE:
  case MED_EN::MED_EDGE:
    _topology->convertGlobalFaceList(cell_list,nb_cells,local,ip);
    break;
  }

  for (int icell=0; icell<nb_cells; icell++)
  {
    type_list[icell]=_mesh[ip[icell]]->getElementType(entity,local[icell]);
  }
  delete[]local;
  delete[]ip;
  MESSAGE_MED("end of getTypeList");
}



/*!gets the descending connectivity for a certain type
 *
 * The output array type_connectivity should have been allocated
 * at dimension nbnode_per_type* nb_cells before the call
 *
 * \param cell_list list of elements (global cell numbers) for which the connectivity is required
 * \param nb_cells number of elements
 * \param entity type of entity for which the nodal connectivity is required
 * \param type type of the elements for which the connectivity is required
 * \param type_connectivity on output contains the connectivity of all the elements of the list
 * */ 

void MESHCollection::getFaceConnectivity(const int* cell_list,int nb_cells,MED_EN::medEntityMesh entity,
                                         MED_EN::medGeometryElement type, int* type_connectivity) const
{
  int *local=new int[nb_cells];
  int *ip=new int[nb_cells];
  switch (entity)
  {
  case MED_EN::MED_CELL:
    _topology->convertGlobalCellList(cell_list,nb_cells,local,ip);
    break;
  case MED_EN::MED_FACE:
  case MED_EN::MED_EDGE:
    _topology->convertGlobalFaceList(cell_list,nb_cells,local,ip);
    break;
  }


  int nbface_per_type;
  switch (type){
  case 308:
    nbface_per_type=6;
    break;
  case 304:
    nbface_per_type=4;
    break;
  case 306:
    nbface_per_type=5;
    break;
  }

  vector<int> number_of_types_array(_topology->nbDomain(),0);
  for (int i=0; i<_topology->nbDomain(); i++)
    number_of_types_array[i]=_mesh[i]->getNumberOfTypes(entity);

  //defining a connectivity table for different domains 
  vector  <const int*> conn_ip(_topology->nbDomain());
  for (int i=0; i<_topology->nbDomain();i++)
  {
    int nb_elem = _mesh[i]->getNumberOfElements(entity,type);
    if (nb_elem>0)
      conn_ip[i]=_mesh[i]->getConnectivity(MED_EN::MED_DESCENDING,entity,type);
    else
      conn_ip[i]=0;                     
  }

  for (int icell=0; icell<nb_cells; icell++)
  {
    int number_of_types = number_of_types_array[ip[icell]];
    const MEDMEM::CELLMODEL* types =  _mesh[ip[icell]]->getCellsTypes(entity);
    int type_offset=0;
    for (int itype=0; itype< number_of_types; itype++)
    {
      if (types[itype].getType() < type)
        type_offset += _mesh[ip[icell]]->getNumberOfElements(entity,types[itype].getType());
    }
    const int* conn=conn_ip[ip[icell]];           
    for (int iface=0; iface<nbface_per_type; iface++)
    {
      type_connectivity[icell*nbface_per_type+iface] = _topology->convertFaceToGlobal
        (ip[icell], abs(conn[(local[icell] - type_offset - 1) * nbface_per_type + iface]));           
    }
  }

  delete[]local;
  delete[]ip;
}

/*! gets the list of coordinates for a given list of global node numbers
 * 
 * The vector containing the coordinates on output should
 * have been allocated at a dimension _space_dimension * nb_nodes
 * before the call
 * 
 * \param node_list list of global node numbers
 * \param nb_nodes number of nodes in the list
 * \param coordinates on output, contains the coordinates
 */

void MESHCollection::getCoordinates(int* node_list,int nb_nodes, double* coordinates) const
{
  int* local=new int[nb_nodes];
  int* ip=new int[nb_nodes];
  int space_dimension= getSpaceDimension();
  _topology->convertGlobalNodeList(node_list,nb_nodes,local,ip);
  for (int i=0; i< nb_nodes; i++)
  {
    const double* coord=_mesh[ip[i]]->getCoordinates(MED_EN::MED_FULL_INTERLACE);
    for (int icoord=0; icoord<space_dimension; icoord++)
      coordinates[i*space_dimension+icoord]=coord[(local[i]-1)*space_dimension+icoord];
  }
  delete[]local;
  delete[] ip;
}
/*! returns constituent entity*/
MED_EN::medEntityMesh MESHCollection::getSubEntity() const
{
  return getMeshDimension()==3 ? MED_EN::MED_FACE : MED_EN::MED_EDGE;
}

/*! retrieves the space dimension*/
int MESHCollection::getSpaceDimension() const
{
  return _i_non_empty_mesh < 0 ? -1 : _mesh[_i_non_empty_mesh]->getSpaceDimension();
}
/*! retrieves the mesh dimension*/
int MESHCollection::getMeshDimension() const
{
  return _i_non_empty_mesh < 0 ? -1 : _mesh[_i_non_empty_mesh]->getMeshDimension();
}

/*! retrieves the type of coordinates system*/
string MESHCollection::getSystem() const
{
  return _i_non_empty_mesh < 0 ? "" : _mesh[_i_non_empty_mesh]->getCoordinatesSystem();
}

/*!retrieves the name of the mesh*/
string MESHCollection::getMeshName() const
{
  return _i_non_empty_mesh < 0 ? (_mesh[0] ? _mesh[0]->getName() : "") : _mesh[_i_non_empty_mesh]->getName();
}

vector<MEDMEM::MESH*>& MESHCollection::getMesh() 
{
  return _mesh;
}

MEDMEM::MESH* MESHCollection::getMesh(int idomain) const
{
  return _mesh[idomain];
}

vector<MEDMEM::CONNECTZONE*>& MESHCollection::getCZ()
{
  return _connect_zones;
}

Topology* MESHCollection::getTopology() const 
{
  return _topology;
}

void MESHCollection::setTopology(Topology* topo)
{
  if (_topology!=0)
  {
    throw MED_EXCEPTION(STRING("Erreur : topology is already set"));
  }
  else
    _topology = topo;
}

void MESHCollection::setIndivisibleGroup(const string& name)
{
  _indivisible_regions.push_back(name);

}

/*! Browses the domains and the regions that have 
 * been marked as indivisible in order to create a vector 
 * the dimlension of which is the total number of cells, and
 * that contains 0 if the cell belongs to no indivisible group
 * and that contains an integer corresponding to the group otherwise.
 *
 * \param   indivisible_tag on input is an int* allocated as int[nbcells]
 *        on output contains the tags 
 */


void MESHCollection::treatIndivisibleRegions(int* indivisible_tag)
{
  //tag 0 is positioned on all the cells that are not affected by these tags
  for (int i=0; i<_topology->nbCells(); i++)
    indivisible_tag[i]=0;

  //treating cell groups    
  for (int idomain=0; idomain<_topology->nbDomain();idomain++)
    for (int igroup=0; igroup<_mesh[idomain]->getNumberOfGroups(MED_EN::MED_CELL); igroup++)
      for (unsigned i=0; i<_indivisible_regions.size(); i++)
      {
        const MEDMEM::GROUP* group = _mesh[idomain]->getGroup(MED_EN::MED_CELL,igroup+1);
        string groupname = group->getName();
        if (trim(groupname)==trim(_indivisible_regions[i]))
        {
          int nbcells=group->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS);
          const int* numbers=group->getNumber(MED_EN::MED_ALL_ELEMENTS);
          int* global=new int[nbcells];
          _topology->convertCellToGlobal(idomain,numbers,nbcells,global);
          for (int icell=0; icell<nbcells; icell++)
            indivisible_tag[global[icell]-1]=i+1;
          delete[] global;
        } 
      }
}

//================================================================================
/*!
 * \brief Create cell->node and node->cell connectivities for domains
 */
//================================================================================

template<class TID2CONN>
void MESHCollection::fillGlobalConnectivity(TID2CONN & node2cell, TID2CONN & cell2node )
{
  for (int idomain=0; idomain<_topology->nbDomain(); idomain++)
  {
    if ( !_mesh[idomain] ) continue;
    //  MED_EN::medGeometryElement* type_array;
    int nb_cells= _topology->nbCells(idomain);
    int* cell_list = new int[nb_cells];

    //retrieving global id list
    _topology->getCellList(idomain, cell_list);

    int nb_plain_cells = _mesh[idomain]->getNumberOfElements(MED_EN::MED_CELL,
                                                             MED_EN::MED_ALL_ELEMENTS);
    if (nb_plain_cells >0)
    {          
      const int* conn_index = _mesh[idomain]->getConnectivityIndex(MED_EN::MED_NODAL,
                                                                   MED_EN::MED_CELL);

      const int* conn =   _mesh[idomain]->getConnectivity(MED_EN::MED_NODAL,
                                                          MED_EN::MED_CELL,
                                                          MED_EN::MED_ALL_ELEMENTS);                                                                    
      int nbnodes = conn_index[nb_plain_cells]-1;
      int* global_nodes =new int [nbnodes];
      _topology->convertNodeToGlobal(idomain, conn, nbnodes, global_nodes);
      for (int icell=0; icell< nb_plain_cells; icell++)
      {
        for (int inode=conn_index[icell]; inode < conn_index[icell+1]; inode++)
        {
          int node_global_id = global_nodes[inode-1];
          if ( node_global_id > 0 )
          {
            int cell_global_id = cell_list[icell];
            cell2node [cell_global_id].push_back(node_global_id);
            node2cell [node_global_id].push_back(cell_global_id);
          }
        }
      }
      delete[] global_nodes;
    }

    delete[] cell_list;
  }
}

/*! Method creating the cell graph
 *
 * \param array returns the pointer to the structure that contains the graph 
 * \param edgeweight returns the pointer to the table that contains the edgeweights
 *        (only used if indivisible regions are required)
 */

void MESHCollection::buildCellGraph(MEDMEM::MEDSKYLINEARRAY* & array,int *& edgeweights )
{

  int cell_number=1;
  int node_number=1;
  for (int i=0; i<_topology->nbDomain(); i++)
  {
    cell_number+=_topology->getCellNumber(i);
    node_number+=_topology->getNodeNumber(i);
  }
  //list of cells for a given node
  //vector< vector<int> > node2cell(node_number);
  map< int, vector<int> > node2cell;

  //list of nodes for a given cell
  //vector< vector <int> > cell2node(cell_number);
  map< int, vector <int> > cell2node;

  //  map<MED_EN::medGeometryElement,int*> type_cell_list;

  //tagging for the indivisible regions
  int* indivisible_tag=0;
  bool has_indivisible_regions=false;
  if (!_indivisible_regions.empty())
  {
    has_indivisible_regions=true;
    indivisible_tag=new int[_topology->nbCells()];
    treatIndivisibleRegions(indivisible_tag);
  }

  fillGlobalConnectivity(node2cell, cell2node );

  cout << "beginning of skyline creation"<<endl;
  //creating the MEDMEMSKYLINEARRAY containing the graph

  int* size = new int[_topology->nbCells()];
  int** temp=new int*[_topology->nbCells()];
  int** temp_edgeweight=0;
  if (has_indivisible_regions)
    temp_edgeweight=new int*[_topology->nbCells()];

  int cell_glob_shift = 0;

  // Get connection to cells on other procs
  multimap< int, int > loc2dist; // global cell ids on this proc -> other proc cells
  if ( isParallelMode() )
  {
    cell_glob_shift = _domain_selector->getProcShift();

    set<int> loc_domains; // domains on this proc
    for ( int idom = 0; idom < _mesh.size(); ++idom )
      if ( _mesh[ idom ] )
        loc_domains.insert( idom );

    for ( int idom = 0; idom < _mesh.size(); ++idom )
    {
      if ( !_mesh[idom] ) continue;
      vector<int> loc2glob_corr; // pairs of corresponding cells (loc_loc & glob_dist)
      retrieveDriver()->readLoc2GlobCellConnect(idom, loc_domains, _domain_selector, loc2glob_corr);
      //MEDMEM::STRING out;
      for ( int i = 0; i < loc2glob_corr.size(); i += 2 )
      {
        int glob_here  = _topology->convertCellToGlobal(idom,loc2glob_corr[i]); 
        int glob_there = loc2glob_corr[i+1]; 
        loc2dist.insert ( make_pair( glob_here, glob_there));
        //out << glob_here << "-" << glob_there << " ";
      }
      //cout << "\nRank " << _domain_selector->rank() << ": BndCZ: " << out << endl;
    }
  }

  //going across all cells

  map<int,int> cells_neighbours;
  for (int i=0; i< _topology->nbCells(); i++)
  {


    vector<int> cells(50);

    for (unsigned inode=0; inode< cell2node[i+1].size(); inode++)
    {
      int nodeid=cell2node[i+1][inode];
      for (unsigned icell=0; icell<node2cell[nodeid].size();icell++)
        cells_neighbours[node2cell[nodeid][icell]]++;
    }
    size[i]=0;
    int dimension = getMeshDimension();
    cells.clear();

    for (map<int,int>::const_iterator iter=cells_neighbours.begin(); iter != cells_neighbours.end(); iter++)  
    {
      if (iter->second >= dimension && iter->first != i+1) 
      {
        cells.push_back(iter->first + cell_glob_shift);
        //       cells[isize++]=iter->first;
      }
    }
    // add neighbour cells from distant domains
    multimap< int, int >::iterator loc_dist = loc2dist.find( i+1 );
    for (; loc_dist!=loc2dist.end() && loc_dist->first==( i+1 ); ++loc_dist )
      cells.push_back( loc_dist->second );

    size[i]=cells.size();

    temp[i]=new int[size[i]];
    if (has_indivisible_regions)
      temp_edgeweight[i]=new int[size[i]];
    //
    int itemp=0;

    for (vector<int>::const_iterator iter=cells.begin(); iter!=cells.end();iter++)
    {
      temp[i][itemp]=*iter;
      if (has_indivisible_regions)
      {
        int tag1 = indivisible_tag[(i+1)-1];
        int tag2 = indivisible_tag[*iter-1];
        if (tag1==tag2 && tag1!=0)
          temp_edgeweight[i][itemp]=_topology->nbCells()*100000;
        else
          temp_edgeweight[i][itemp]=1;
      } 
      itemp++;
    }
    cells_neighbours.clear();
  }
  cout <<"end of graph definition"<<endl;
  int* index=new int[_topology->nbCells()+1];
  index[0]=1;
  for (int i=0; i<_topology->nbCells(); i++)
    index[i+1]=index[i]+size[i];

  node2cell.clear();
  cell2node.clear();
  if (indivisible_tag!=0) delete [] indivisible_tag;

  //SKYLINEARRAY structure holding the cell graph
  array= new MEDMEM::MEDSKYLINEARRAY(_topology->nbCells(),index[_topology->nbCells()]-index[0]);
  array->setIndex(index);

  for (int i=0; i<_topology->nbCells(); i++)
  {
    array->setI(i+1,temp[i]);
    delete[]temp[i];
  }

  if (has_indivisible_regions)
  {
    edgeweights=new int[array->getLength()];
    for (int i=0; i<_topology->nbCells(); i++)
    {
      for (int j=index[i]; j<index[i+1];j++)
        edgeweights[j-1]=temp_edgeweight[i][j-index[i]];
      delete[] temp_edgeweight[i];  
    }
    delete[]temp_edgeweight;
  }
  delete[] index;
  delete[] temp;
  delete[] size;

  cout<< "end of graph creation"<<endl;
}

/*! Creates the partition corresponding to the cell graph and the partition number
 *
 * \param nbdomain number of subdomains for the newly created graph
 *
 * returns a topology based on the new graph
 */
Topology* MESHCollection::createPartition(int nbdomain, 
                                          Graph::splitter_type split, 
                                          const string& options_string,
                                          int* user_edge_weights,
                                          int* user_vertices_weights)
{
  if (nbdomain <1) throw MEDEXCEPTION("Number of subdomains must be >0");
  MEDMEM::MEDSKYLINEARRAY* array=0;
  int* edgeweights=0;

  MESSAGE_MED("Building cell graph");
  buildCellGraph(array,edgeweights);

  switch (split)
  {
  case Graph::METIS:
#if defined(MED_ENABLE_METIS) || defined(MED_ENABLE_PARMETIS)
    _cell_graph=boost::shared_ptr<Graph>(new METISGraph(array,edgeweights));
#else
    throw MEDEXCEPTION("METIS Graph is not available. Check your products, please.");
#endif
    break;
  case Graph::SCOTCH:
#ifdef MED_ENABLE_SCOTCH
    _cell_graph=boost::shared_ptr<Graph>(new SCOTCHGraph(array,edgeweights));
#else
    throw MEDEXCEPTION("SCOTCH Graph is not available. Check your products, please.");
#endif
    break;
  }

  //!user-defined weights
  if (user_edge_weights!=0) 
    _cell_graph->setEdgesWeights(user_edge_weights);
  if (user_vertices_weights!=0)
    _cell_graph->setVerticesWeights(user_vertices_weights);

  MESSAGE_MED("Partitioning graph");
  _cell_graph->partGraph(nbdomain,options_string,_domain_selector);

  // DEBUG
//   MEDMEM::STRING out("RESULT GRAPH #");
//   out << (_domain_selector?_domain_selector->rank():0) << ": ";
//   const int* part = _cell_graph->getPart();
//   int n = _cell_graph->nbVertices();
//   for ( int e=0; e < n; ++e )
//     out << part[e] <<" ";
//   cout << out << endl;
  

  MESSAGE_MED("Building new topology");
  //_cell_graph is a shared pointer 
  Topology* topology = new ParallelTopology (_cell_graph, nbdomain, getMeshDimension());

  //cleaning
  if (edgeweights!=0) delete[] edgeweights;
  //if (array!=0) delete array;
  MESSAGE_MED("End of partition creation");
  return topology;
}

/*! Creates a topology for a partition specified by the user
 *
 * \param table user-specified partition (for each cell contains the domain number from 0 to n-1)
 *
 * returns a topology based on the new partition
 */
Topology* MESHCollection::createPartition(const int* partition)
{
  MEDMEM::MEDSKYLINEARRAY* array=0;
  int* edgeweights=0;

  buildCellGraph(array,edgeweights);

  set<int> domains;
  for (int i=0; i<_topology->nbCells(); i++)
  {
    domains.insert(partition[i]);
  }
  int nbdomain=domains.size();

  _cell_graph=boost::shared_ptr<Graph>(new UserGraph(array, partition, _topology->nbCells()));

  //_cell_graph is a shared pointer 
  Topology* topology = new ParallelTopology (_cell_graph, nbdomain, getMeshDimension());

  //if (array!=0) delete array;
  return topology;
}


/*! building Connect Zones for storing the informations
 * of the connectivity 
 *
 * The connect zones are created for every domain that has common nodes with 
 * domain \a idomain
 *
 * \param idomain domain number for which the connect zones are created
 * */

// void MESHCollection::buildConnectZones(int idomain)
// {
//   // constructing node/node correspondencies
//   vector<MEDMEM::MEDSKYLINEARRAY*> node_node_correspondency;
//   node_node_correspondency.resize(_topology->nbDomain());

//   cout << "Computing node/node corresp"<<endl;

//   _topology->computeNodeNodeCorrespondencies(idomain, node_node_correspondency );

//   for (int idistant=0; idistant< _topology->nbDomain(); idistant++)
//   {
//     // on regarde si une correspondance noeud/noeud a été trouvée 
//     // entre idomain et idistant
//     // si oui, on crée une connectzone
//     if (node_node_correspondency[idistant]!=0)
//     {
//       MEDMEM::CONNECTZONE* cz= new MEDMEM::CONNECTZONE();
//       cz->setLocalMesh(_mesh[idomain]);
//       cz->setDistantMesh(_mesh[idistant]);
//       cz->setLocalDomainNumber(idomain);
//       cz->setDistantDomainNumber(idistant);
//       cz-> setName ("Connect zone defined by SPLITTER");
//       cz->setNodeCorresp(node_node_correspondency[idistant]);
//       _connect_zones.push_back(cz);  
//     }
//   }
//   cout << "Computing node/node corresp"<<endl;

//   vector<MEDMEM::MEDSKYLINEARRAY*> cell_cell_correspondency;
//   cell_cell_correspondency.resize(_topology->nbDomain());
//   _topology->computeCellCellCorrespondencies(idomain, cell_cell_correspondency, _cell_graph.get());

//   for (int idistant=0; idistant< _topology->nbDomain(); idistant++)
//   {
//     //the connect zone has been created by the node/node computation
//     if (cell_cell_correspondency[idistant]!=0)
//     {
//       MEDMEM::CONNECTZONE* cz=0;
//       for (int icz=0; icz<_connect_zones.size();icz++)
//         if (_connect_zones[icz]->getLocalDomainNumber()==idomain &&
//             _connect_zones[icz]->getDistantDomainNumber()==idistant)
//           cz = _connect_zones[icz];
//       if (cz!=0) 
//         cz->setEntityCorresp(MED_EN::MED_CELL,MED_EN::MED_CELL, cell_cell_correspondency[idistant]);
//       else 
//         throw MEDEXCEPTION("MESHCollection::buildConnectZones() -A connect zone should exist");   
//       //delete cell_cell_correspondency[idistant];
//     }

//   }
// }

//================================================================================
/*!
 * \brief Adds a group of joint faces
 *  \param loc_face_ids - local numbers of faces
 *  \param idomian - domain index where faces are local
 *  \param idistant - the other domain index
 */
//================================================================================

void MESHCollection::addJointGroup(const std::vector<int>& loc_face_ids, int idomain, int idistant)
{
  MEDMEM::MESHING* meshing = dynamic_cast<MEDMEM::MESHING*> (_mesh[idomain]);
  MED_EN::medEntityMesh constituent_entity = getSubEntity();

  MEDMEM::STRING jointname("joint_");
  jointname<<idistant+1;

  MEDMEM::GROUP * tmp_grp = new GROUP, * joint_group = tmp_grp;
  // try to find already present group with such a name
  //  vector<MEDMEM::GROUP*> groups = meshing->getGroups( constituent_entity );
  //  for ( int g = 0; g < groups.size(); ++g )
  //    if ( groups[g]->getName() == jointname.str() )
  //    {
  //      joint_group = groups[g];
  //      break;
  //    }
  // assure uniqueness of group name
  bool unique = false;
  vector<MEDMEM::GROUP*> groups = meshing->getGroups( constituent_entity );
  do
  {
    unique = true;
    for ( int g = 0; unique && g < groups.size(); ++g )
      unique = ( groups[g]->getName() != jointname );
    if ( !unique )
      jointname << "_" << idomain+1;
  }
  while ( !unique );
  joint_group->setName(jointname);
  joint_group->setMesh(meshing);
  joint_group->setEntity(constituent_entity);
  map<MED_EN::medGeometryElement, vector<int> > joint_types;

  int nbfaces = loc_face_ids.size();
  for (int i=0; i<nbfaces; i++)
  {    
    MED_EN::medGeometryElement type = meshing->getElementType(constituent_entity,loc_face_ids[i]);
    joint_types[type].push_back(loc_face_ids[i]);
  }
  joint_group->setNumberOfGeometricType(joint_types.size());
  MED_EN::medGeometryElement* types=new MED_EN::medGeometryElement[joint_types.size()];
  int* nb_in_types=new int[joint_types.size()];
  int* group_index=new int[joint_types.size()+1];

  group_index[0]=1;
  int itype=0;
  int iface =0;
  int* group_value=new int[nbfaces];
  for (map<MED_EN::medGeometryElement, vector<int> >::const_iterator iterj=joint_types.begin();
       iterj != joint_types.end();
       iterj++)
  {
    nb_in_types[itype]=(iterj->second).size();
    types[itype]=iterj->first;
    itype++;
    group_index[itype]=group_index[itype-1]+(iterj->second).size();
    for (int i=0; i<  (iterj->second).size(); i++)
      group_value[iface++]=(iterj->second)[i];
  }
  joint_group->setGeometricType(types);
  joint_group->setNumberOfElements(nb_in_types);
  joint_group->setNumber(group_index, group_value, /*shallowCopy=*/true);
  delete[] types;
  delete[] nb_in_types;

  if ( joint_group == tmp_grp )
    meshing->addGroup(*tmp_grp);
  tmp_grp->removeReference();
}

/*! building Connect Zones for storing the informations
 * of the connectivity 
 * */

void MESHCollection::buildConnectZones()
{
  vector <map <MED_EN::medGeometryElement, vector<MEDSPLITTER_FaceModel*> > > face_map(_topology->nbDomain());
  map< pair<int,int>, MEDMEM::MEDSKYLINEARRAY*> cell_corresp_here;

  MED_EN::medEntityMesh constituent_entity = getSubEntity();

  if ( isParallelMode() )
  {
    buildConnectZonesBetweenProcs(face_map, cell_corresp_here);
  }

  cout << "Computing node/node corresp"<<endl;

  //Creating nodes
  for (int idomain=0; idomain<_topology->nbDomain(); idomain++)
  {

    // constructing node/node correspondencies
    vector<MEDMEM::MEDSKYLINEARRAY*> node_node_correspondency(_topology->nbDomain());
    _topology->computeNodeNodeCorrespondencies(idomain, node_node_correspondency );

    for (int idistant=0; idistant< _topology->nbDomain(); idistant++)
    {
      // on regarde si une correspondance noeud/noeud a été trouvée 
      // entre idomain et idistant
      // si oui, on crée une connectzone
      if (node_node_correspondency[idistant]!=0)
      {
        MEDMEM::CONNECTZONE* cz= new MEDMEM::CONNECTZONE();
        cz->setLocalMesh(_mesh[idomain]);
        cz->setDistantMesh(_mesh[idistant]);
        cz->setLocalDomainNumber(idomain);
        cz->setDistantDomainNumber(idistant);
        cz-> setName ("Connect zone defined by SPLITTER");
        cz->setNodeCorresp(node_node_correspondency[idistant]);
        _connect_zones.push_back(cz);  
      }
    }
  }
  cout << "Computing face corresp"<<endl;

  //creating faces if required 
  if (_subdomain_boundary_creates)
  {
    int global_face_id = _topology->getFaceNumber()+1;
    //int global_face_id = _topology->getMaxGlobalFace()+1;

    map <pair<int,int>, vector<int> > faces_in_joint;

    if ( !isParallelMode() )
      // taking faces that are already present in the mesh into account
      for (int idomain=0; idomain<_topology->nbDomain(); idomain++)
      {
        getFaces(idomain,face_map[idomain]); 
      }

    // creating faces that are located at the interface between
    // subdomains 

    vector <int> nb_added_groups( _topology->nbDomain(), 0 );

    for (int idomain=0; idomain<_topology->nbDomain(); idomain++)
    {
      vector<MEDMEM::MEDSKYLINEARRAY*> cell_cell_correspondency( _topology->nbDomain() );
      if ( !isParallelMode() )
        _topology->computeCellCellCorrespondencies(idomain, cell_cell_correspondency, _cell_graph.get());

      for (int idistant=0; idistant< _topology->nbDomain(); idistant++)
      {
        if (idistant <= idomain) continue;

        MEDMEM::MEDSKYLINEARRAY* cell_correspondency = 0;
        if ( isParallelMode() )
          cell_correspondency = cell_corresp_here[ make_pair (idomain,idistant)];
        else
          cell_correspondency = cell_cell_correspondency[idistant];

        //the connect zone has been created by the node/node computation

        if ( cell_correspondency )
        {
          int nbcells      = cell_correspondency->getNumberOf();
          const int* index = cell_correspondency->getIndex();
          const int* value = cell_correspondency->getValue();
          if ( isParallelMode() )
            global_face_id = _domain_selector->getFisrtGlobalIdOfSubentity( idomain, idistant );

          for (int ilocal=0; ilocal<nbcells; ilocal++)
          { 
            for (int icelldistant = index[ilocal]; icelldistant < index[ilocal+1]; icelldistant++)
            {
              int distant_id = value[icelldistant-1];
              MEDSPLITTER_FaceModel* face = getCommonFace(idomain,ilocal+1,idistant,distant_id,global_face_id);
              face_map[idomain][face->getType()].push_back(face);
              MEDSPLITTER_FaceModel* face2 = getCommonFace(idistant,distant_id,idomain, ilocal+1,global_face_id);
              face_map[idistant][face->getType()].push_back(face2);
              faces_in_joint[make_pair(idomain,idistant)].push_back(global_face_id);
              global_face_id++;
            } 
          }
        }

      }
      //cleaning up
      for (int idistant=0; idistant< _topology->nbDomain(); idistant++)
        delete cell_cell_correspondency[idistant];
    }


    _topology->recreateFaceMapping(face_map);

    //transforming the face_map into a constituent entity connectivity
    for (int idomain=0; idomain< _topology->nbDomain();idomain++) 
    {
      int nbtypes = face_map[idomain].size();
      vector<medGeometryElement> types;
      vector <int> nb_elems;
      vector <int*> conn;

      MEDMEM::MESHING* meshing = dynamic_cast<MEDMEM::MESHING*> (_mesh[idomain]);
      if ( !meshing->getConnectivityptr() )
        continue; // no cells in idomain

      for (map <medGeometryElement, vector<MEDSPLITTER_FaceModel*> >::const_iterator iter= face_map[idomain].begin();
           iter != face_map[idomain].end(); iter ++)
      {
        types.push_back(iter->first);
        int nb_elem_in_type = (iter->second).size();
        nb_elems.push_back(nb_elem_in_type);
        int nb_node_per_type=(iter->first)%100;
        int* connectivity= new int [nb_node_per_type*nb_elem_in_type];
        for (int ielem=0; ielem<nb_elem_in_type; ielem++)
        {
          for (int inode=0;  inode<nb_node_per_type; inode++)
            connectivity[ielem*nb_node_per_type+inode]=(*(iter->second)[ielem])[inode];
        }
        conn.push_back(connectivity);

      }
      //setting the faces in the mesh
      meshing->setNumberOfTypes(nbtypes,constituent_entity);
      meshing->setTypes(&types[0],constituent_entity);
      meshing->setNumberOfElements(&nb_elems[0],constituent_entity);

      for (int itype=0; itype<nbtypes; itype++)
      {
        meshing->setConnectivity( constituent_entity, types[itype], conn[itype] );
        delete[]conn[itype];
      }
      for (int idistant =0; idistant<_topology->nbDomain(); idistant++)
      {
        map <pair<int,int>, vector<int> >::iterator iter;
        iter = faces_in_joint.find(make_pair(idomain,idistant));
        if (iter == faces_in_joint.end())
        {
          iter = faces_in_joint.find (make_pair(idistant,idomain));
          if (iter == faces_in_joint.end()) 
            continue;
        }

        int nbfaces = (iter->second).size();   
        vector<int> face_joint(nbfaces*2);
        MEDMEM::CONNECTZONE* cz=0;
        for (unsigned icz=0; icz<_connect_zones.size();icz++)
          if (_connect_zones[icz]->getLocalDomainNumber()==idomain &&
              _connect_zones[icz]->getDistantDomainNumber()==idistant)
            cz = _connect_zones[icz];

        int nbtotalfaces= _topology->getFaceNumber(idomain);

        //creating arrays for the MEDSKYLINEARRAY structure containing the joint
        int* index =new int[nbtotalfaces+1];
        for (int i=0; i<nbtotalfaces+1;i++)
          index[i]=0;
        int*value=new int[nbfaces];

        map<int,int> faces;
        vector<int> local_faces( nbfaces );
        for (int iface=0; iface<nbfaces; iface++)
        {
          int iglobal = (iter->second)[iface];
          int localid=_topology->convertGlobalFace(iglobal,idomain);
          int distantid=_topology->convertGlobalFace(iglobal,idistant);
          faces.insert(make_pair(localid,distantid));
          local_faces[iface]=localid;
        }

        int iloc=0;
        index[0]=1;
        for (map<int,int>::const_iterator iter=faces.begin(); 
             iter != faces.end();
             iter++)
        {
          index[iter->first]=1;
          value[iloc++]=iter->second;            
        }

        for (int i=0; i<nbtotalfaces;i++)
          index[i+1]+=index[i];
        bool shallowcopy=true;  
        MEDMEM::MEDSKYLINEARRAY* skarray=new MEDMEM::MEDSKYLINEARRAY(nbtotalfaces,nbfaces,index,value,shallowcopy);  

        if (cz!=0)  
          cz->setEntityCorresp(constituent_entity,constituent_entity,skarray);              
        else 
          throw MEDEXCEPTION("MESHCollection::buildConnectZones() -A connect zone should exist");            
        // Creating a group of the faces constituting the joint
        addJointGroup( local_faces, idomain, idistant );
        nb_added_groups[ idomain ]++;
      }
    }

    if ( isParallelMode() )
    {
      // Now all faces have got local ids and we can receive local ids from other procs.
      // Set face/face data to zones with other procs and create a group
      for (int icz=0; icz<_connect_zones.size();icz++)
      {
        MEDMEM::CONNECTZONE* cz=_connect_zones[icz];
        if ( _domain_selector->isMyDomain( cz->getDistantDomainNumber()) ) continue;
        
        int glob_id = _domain_selector->getFisrtGlobalIdOfSubentity( cz->getLocalDomainNumber(),
                                                                     cz->getDistantDomainNumber());
        int nb_cz_faces = _domain_selector->getNbCellPairs( cz->getDistantDomainNumber(),
                                                            cz->getLocalDomainNumber());
        vector< int > loc_ids_here( nb_cz_faces );
        for ( int i = 0; i < nb_cz_faces; ++i )
          loc_ids_here[i] = _topology->convertGlobalFace(glob_id++,cz->getLocalDomainNumber());

        int* loc_ids_dist = _domain_selector->exchangeSubentityIds( cz->getLocalDomainNumber(),
                                                                    cz->getDistantDomainNumber(),
                                                                    loc_ids_here );
        int nb_faces_here= _topology->getFaceNumber(cz->getLocalDomainNumber());
        int* face_index = new int[ nb_faces_here+1 ];
        face_index[0]=1;
        for ( int loc_id = 0, i = 0; loc_id < nb_faces_here; ++loc_id)
        {
          face_index[ loc_id+1 ] = face_index[ loc_id ];
          if ( i < loc_ids_here.size() && loc_ids_here[i] == loc_id+1 )
          {
            face_index[ loc_id+1 ]++;
            i++;
          }
        }
        MEDMEM::MEDSKYLINEARRAY* skarray=
          new MEDMEM::MEDSKYLINEARRAY(nb_faces_here, nb_cz_faces, face_index, loc_ids_dist, true);
        cz->setEntityCorresp(constituent_entity,constituent_entity,skarray);

        addJointGroup( loc_ids_here, cz->getLocalDomainNumber(), cz->getDistantDomainNumber());
        nb_added_groups[ cz->getLocalDomainNumber() ]++;
      }
    }

    for (int idomain=0; idomain<_topology->nbDomain(); idomain++)
    {
      // delete face_map
      for (map <medGeometryElement, vector<MEDSPLITTER_FaceModel*> >::const_iterator iter= face_map[idomain].begin();
           iter != face_map[idomain].end(); iter ++)
        for (unsigned i=0; i<(iter->second).size();i++)
          delete (iter->second)[i];

      if ( nb_added_groups[ idomain ] > 0 &&
           _mesh[idomain]->getNumberOfFamilies( constituent_entity ) > 0 )
        // needed because if there were face families before, driver won't
        // create families from just added groups (see MEDMEM_MedMeshDriver.cxx:3330),
        // actually it is a bug of driver - it must check presence of groups in families
        _mesh[idomain]->createFamilies(); 
    }
  }

  if ( isParallelMode() )
    // Excange info on types of constituent_entity needed while writing joints
    // to get ids local in geom type for distant procs
    _domain_selector->gatherEntityTypesInfo( _mesh, constituent_entity );

  cout << "Computing cell/cell corresp"<<endl;

  //Creating cell/cell correspondencies
  for (int idomain=0;idomain<_topology->nbDomain();idomain++)
  {
    vector<MEDMEM::MEDSKYLINEARRAY*> cell_cell_correspondency( _topology->nbDomain() );
    if ( !isParallelMode() )
      _topology->computeCellCellCorrespondencies(idomain,cell_cell_correspondency,_cell_graph.get());

    for (int idistant=0; idistant< _topology->nbDomain(); idistant++)
    {
      MEDMEM::MEDSKYLINEARRAY* cell_correspondency = 0;
      if ( isParallelMode() )
        cell_correspondency = cell_corresp_here[ make_pair (idomain,idistant)];
      else
        cell_correspondency = cell_cell_correspondency[idistant];

      //the connect zone has been created by the node/node computation
      if ( cell_correspondency )
      {
        MEDMEM::CONNECTZONE* cz=0;
        for (unsigned icz=0; icz<_connect_zones.size();icz++)
          if (_connect_zones[icz]->getLocalDomainNumber()==idomain &&
              _connect_zones[icz]->getDistantDomainNumber()==idistant)
            cz = _connect_zones[icz];
        if (cz!=0)  
          cz->setEntityCorresp(MED_EN::MED_CELL,MED_EN::MED_CELL, cell_correspondency);
        else 
          throw MEDEXCEPTION("MESHCollection::buildConnectZones() -A connect zone should exist");   
      }
    }
  }
}

/*! building Connect Zones for storing the informations
 * of the connectivity in the parallel mode
 * */

void MESHCollection::buildConnectZonesBetweenProcs(TGeom2FacesByDomian & face_map,
                                                   map< pair<int,int>, MEDMEM::MEDSKYLINEARRAY*> & cell_cell_correspondency_here)
{
  using namespace MED_EN;

  // graph over all procs
  auto_ptr<Graph> global_graph( _domain_selector->gatherGraph( _cell_graph.get() ));

  vector< vector< JointExchangeData > > joints_of_domain( _topology->nbDomain() );

  for (int idomain=0; idomain<_topology->nbDomain(); idomain++)
  {
    if ( !_domain_selector->isMyDomain( idomain )) continue;

    vector< JointExchangeData > & joints = joints_of_domain[ idomain ];
    joints.resize( _topology->nbDomain() );

    // Find corresponding cells on other procs

    const int* gra_index = global_graph->getGraph()->getIndex();
    const int* gra_value = global_graph->getGraph()->getValue();
    const int* partition = global_graph->getPart();
    const int dj = gra_index[0];

    vector< int > glob_cells_here( _topology->getCellNumber( idomain ));
    _topology->getCellList( idomain, & glob_cells_here[0]);
    for ( int loc_here = 0; loc_here < glob_cells_here.size(); ++loc_here )
    {
      int glob_here = glob_cells_here[ loc_here ];
      for ( int j = gra_index[ glob_here-1 ]; j < gra_index[ glob_here ]; ++j )
      {
        int glob_neighbor = gra_value[ j-dj ];
        int neighbor_dom = partition[ glob_neighbor-1 ];
        if ( neighbor_dom == idomain ) continue;

        if ( _domain_selector->isMyDomain( neighbor_dom ))
        {
          joints[ neighbor_dom ].addCellCorrespondence
            (_mesh[idomain], neighbor_dom, idomain, glob_neighbor, glob_here, loc_here + 1,
             _topology->convertGlobalCell(glob_neighbor).second );
        }
        else
        {
          joints[ neighbor_dom ].addCellCorrespondence
            (_mesh[idomain], neighbor_dom, idomain, glob_neighbor, glob_here, loc_here + 1 );
        }
      }
    }
  }
  global_graph.reset(); // free memory

  // set joints in a queue to exchange
  typedef map< int, JointExchangeData* > TOrderedJoints;
  TOrderedJoints queue;
  for (int idomain=0; idomain<_topology->nbDomain(); idomain++)
  {
    if ( !_domain_selector->isMyDomain( idomain )) continue;

    vector< JointExchangeData > & joints = joints_of_domain[ idomain ];
    for (int idist=0; idist<_topology->nbDomain(); ++idist )
    {
      JointExchangeData& joint = joints[idist];

      int nb_cell_pairs = joint.nbCellPairs();
      if ( nb_cell_pairs == 0 )
        continue;
      else
        _domain_selector->setNbCellPairs( nb_cell_pairs, idist, idomain );

      joint.setMeshes( idist, _mesh[idist], idomain, _mesh[idomain] );

      if ( _domain_selector->isMyDomain( idist ))
      {
        // a joint on this proc
        cell_cell_correspondency_here[ make_pair( idomain, idist )] = joint.makeCellCorrespArray();
      }
      else
      {
        // a joint with distant proc
        joint.setConnectivity( & ((MEDMEM::MeshFuse*)_mesh[idomain])->getNodeNumbers()[0] );
        int order = _domain_selector->jointId( idomain, idist );
        queue[ order ] = & joint;
      }
    }
  }
  // gather info on cell geom types needed to exchange joints
  _domain_selector->gatherEntityTypesInfo( _mesh, MED_EN::MED_CELL );

  // gather info on nb of sub-entities to compute their global numbers for joints
  _domain_selector->gatherNbOf( getSubEntity(), _mesh );
  _domain_selector->gatherNbCellPairs();
  if ( _subdomain_boundary_creates )
  {
    // taking faces that are already present in the mesh into account
    for (int idomain=0; idomain<_topology->nbDomain(); idomain++)
      if ( _domain_selector->isMyDomain( idomain ))
        getFaces(idomain,face_map[idomain]);
  }
  else
  {
    face_map.clear(); // mark for the joint not to create faces
  }

  // exchange joint data with other procs and make CONNECTZONEs
  TOrderedJoints::iterator ord_joint = queue.begin();
  for ( ; ord_joint != queue.end(); ++ord_joint )
  {
    JointExchangeData* joint = ord_joint->second;

    _domain_selector->exchangeJoint( joint );
    if ( _subdomain_boundary_creates )
    {
      int first_sub_id = _domain_selector->getFisrtGlobalIdOfSubentity( joint->localDomain(),
                                                                        joint->distantDomain() );
      joint->setFisrtGlobalIdOfSubentity( first_sub_id );
    }
    _connect_zones.push_back ( joint->makeConnectZone( face_map ));
  }
}

/*! projects old collection families on new collection families
 */
void MESHCollection::castFamilies(const MESHCollection& old_collection)
{
  vector <list<int> > element_array  (_topology->nbDomain());

  //loop on old domains to create groups out of the existing families
  if (_family_splitting)
    for (int idomain=0; idomain < old_collection._topology->nbDomain(); idomain++)
      old_collection.getMesh(idomain)->createGroups();

  //definition of the entities array which 
  //defines the entities over which the information is cast
  MED_EN::medEntityMesh entities[3];
  entities[0]=MED_EN::MED_NODE;
  entities[1]=getSubEntity();
  entities[2]=MED_EN::MED_CELL;

  for (int ientity=0; ientity<=2;ientity++)
  {

    //int nbgroups = old_collection.getMesh(0)->getNumberOfGroups(entities[ientity]);

    map <string, set<int> > group_map;
    for (int idomain=0; idomain < old_collection._topology->nbDomain(); idomain++)
    {
      if ( !old_collection.getMesh(idomain) ) continue;
      for (int igroup=0; igroup<old_collection.getMesh(idomain)->getNumberOfGroups(entities[ientity]); igroup++)
      {
        //retrieves a group
        MEDMEM::GROUP* group = (old_collection.getMesh(idomain)->getGroups(entities[ientity]))[igroup];
        //increments the number of groups if it is a new group
        //if (group_map.find(group->getName())==group_map.end())

        group_map[group->getName()].insert(idomain);
        //   group_map.insert(make_pair(group->getName(), idomain);

      }   
    }
    int nbgroups=group_map.size();
    vector <int> igroupold(old_collection._topology->nbDomain(),0);
    map<string,set<int> >::const_iterator iter=group_map.begin();

    for (int igroup=0; igroup<nbgroups; igroup++)
    {
      vector <const MEDMEM::SUPPORT*> old_supports(old_collection._topology->nbDomain());
      string group_name = iter->first;
      iter++; 

      //parameters stored for passing group description
      // from the old meshes to the new ones

      for (int idomain=0; idomain < old_collection._topology->nbDomain(); idomain++)
      {
        //                for (set<int>::iterator iter=group_map[group_name].begin(); iter!=group_map[group_name].end(); iter++)
        //                cout << *iter<<" ";
        //                cout <<endl;
        if (group_map[group_name].find(idomain)==group_map[group_name].end()) continue;

        //retrieves the group igroup on domain idomain
        MEDMEM::GROUP* group = (old_collection.getMesh(idomain)->getGroups(entities[ientity]))[igroupold[idomain]];
        old_supports[idomain] = static_cast<const MEDMEM::SUPPORT*> (group);
        igroupold[idomain]++;
      }

      vector <MEDMEM::GROUP*>new_groups(_topology->nbDomain());
      vector <MEDMEM::SUPPORT*> new_supports(_topology->nbDomain());
      for (int i=0; i<_topology->nbDomain(); i++)
      {
        new_groups[i]=new MEDMEM::GROUP();
        new_supports[i]=static_cast<MEDMEM::SUPPORT*>(new_groups[i]);
      }
      castSupport(old_collection,old_supports,new_supports);      

      //creating new groups from the previous list of elements
      for (int idomain=0; idomain <_topology->nbDomain(); idomain++)
      {
        MEDMEM::MESHING* mesh_builder=static_cast<MEDMEM::MESHING*> (_mesh[idomain]);
        if ( new_supports[idomain] )
          mesh_builder->addGroup(*new_groups[idomain]);
      }
      //groups are copied by the addGroup method,
      //so they can be safely deleted here
      for (int i=0; i<_topology->nbDomain(); i++)
      {
        if ( new_supports[i] ) new_groups[i]->removeReference();
      }

    }// on groups
  }//on entities
}


void MESHCollection::castSupport(const MESHCollection& old_collection, vector<const MEDMEM::SUPPORT*>& old_support, vector<MEDMEM::SUPPORT*>& new_support)
{

  if (old_collection._topology->nbDomain() != (int)old_support.size())
  {
    throw MED_EXCEPTION(STRING("Error : wrong call to MESHCollection::castSupport"));
  }
  vector <list<int> > element_array  (_topology->nbDomain());

  //parameters stored for passing description
  // from the old meshes to the new ones
  string name;
  string description;
  MED_EN::medEntityMesh entity;
  vector <string> support_name(1);
  support_name[0]="support";
  for (int inew=0; inew< _topology->nbDomain(); inew++)
    element_array[inew].clear();

  for (int idomain=0; idomain < old_collection._topology->nbDomain(); idomain++)
  {
    //retrieves the group igroup on domain idomain
    const MEDMEM::SUPPORT* support = old_support[idomain];
    if (old_support[idomain]==0) continue;
    name = support->getName();
    description=support->getDescription();
    int nbelem = support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS);
    if (nbelem==0 && !_create_empty_groups) continue;

    int* list_of_elems;
    if (support->isOnAllElements())
    {
      list_of_elems = new int[nbelem];
      for (int i=0; i<nbelem;i++)
        list_of_elems[i]=i+1;
    }
    else
      list_of_elems = const_cast<int*> (support->getNumber(MED_EN::MED_ALL_ELEMENTS));

    int* array=new int[nbelem];
    int* ip=0;
    int* local=0;
    int* full_array=0;
    entity = support->getEntity();
    int size;

    switch (entity)
    {
    case MED_EN::MED_CELL :
      ip=new int[nbelem];
      local= new int[nbelem];
      size=nbelem;
      old_collection.getTopology()->convertCellToGlobal(idomain,list_of_elems,nbelem,array);
      _topology->convertGlobalCellList(array,nbelem,local,ip);
      for (int i=0; i<nbelem; i++)
        //              cell_arrays[ip[i]][local[i]]=id;
      {
        //          cout <<"(glob,ip,iloc)/nbelem"<<array[i]<<" "<<ip[i]<<" "<<local[i]<<"/"<<nbelem<<endl;
        element_array[ip[i]].push_back(local[i]);
      }
      break;
    case MED_EN::MED_FACE :
    case MED_EN::MED_EDGE :
      old_collection.getTopology()->convertFaceToGlobal(idomain,list_of_elems,nbelem,array);
      _topology->convertGlobalFaceListWithTwins(array,nbelem,local,ip,full_array,size);
      for (int i=0; i<size; i++)
        element_array[ip[i]].push_back(local[i]);
      delete[] full_array;  
      break;
    case MED_EN::MED_NODE :
      old_collection.getTopology()->convertNodeToGlobal(idomain,list_of_elems,nbelem,array);
      _topology->convertGlobalNodeListWithTwins(array,nbelem,local,ip,full_array,size);
      for (int i=0; i<size; i++)
        element_array[ip[i]].push_back(local[i]);
      delete[] full_array;
      break;

    }
    delete[] ip;
    delete[] local;
    delete[] array;

    if (support->isOnAllElements()) delete[] list_of_elems;
  }

  //creating new groups from the previous list of elements
  for (int idomain=0; idomain <_topology->nbDomain(); idomain++)
  {
    if ( _mesh[idomain]->getNumberOfNodes() < 1 || 
         (element_array[idomain].empty() && !_create_empty_groups))
    {
      new_support[idomain]->removeReference();
      new_support[idomain]=0;
      continue;
    }
    MEDMEM::SUPPORT* support= new_support[idomain];
    support->setName(name);
    support->setMesh(_mesh[idomain]);
    support->setDescription(description);
    support->setEntity(entity);

    if ( !element_array[idomain].empty() ) /* if() was added for issue 0021576
                                              to prevent creation of faces */
      {
        element_array[idomain].sort();
        element_array[idomain].unique();

        if (entity != MED_EN::MED_NODE)
          support->fillFromElementList(element_array[idomain]);
        else
          support->fillFromNodeList(element_array[idomain]);
      }
  }
}

void MESHCollection::castField(const MESHCollection& old_collection, const string& fieldname, int itnumber, int ordernumber)
{
  int type=old_collection.getDriver()->getFieldType(fieldname);
  char field_char[80];
  strcpy(field_char,fieldname.c_str());

  if (type ==0)
    castFields<int>(old_collection, field_char, itnumber, ordernumber);
  else
    castFields<double>(old_collection, field_char, itnumber, ordernumber);
}

void MESHCollection::castAllFields(const MESHCollection& initial_collection)
{
  vector <string> field_names;
  vector <int> iternumber;
  vector <int> ordernumber;
  vector <int> types;
  initial_collection.getDriver()->readFileStruct(field_names,iternumber,ordernumber,types);

  for (unsigned i=0; i<field_names.size(); i++)
  {
    char field_char[80];
    strcpy(field_char,field_names[i].c_str());

    // choosing whether the field is of int or double type
    if (types[i] ==0)
      castFields<int>(initial_collection, field_char, iternumber[i], ordernumber[i]);
    else
      castFields<double>(initial_collection, field_char, iternumber[i], ordernumber[i]);
  }
}

void MESHCollection::createNodalConnectivity(const MESHCollection& initial_collection,int idomain, MED_EN::medEntityMesh entity)
{
  MESSAGE_MED ("beginning of createNodalConnectivity for entity "<<entity);
  int dimension=0;
  int nb_elems=0;
  MEDMEM::MESHING* mesh_builder = static_cast<MEDMEM::MESHING*>(_mesh[idomain]);


  //number of elements per type
  std::map<MED_EN::medGeometryElement,int> type_numbers;

  //creating arrays for storing global numbers and cell types
  switch (entity)
  {
  case MED_EN::MED_CELL:
    dimension=initial_collection.getMeshDimension();
    nb_elems=_topology->getCellNumber(idomain);
    break;
  case MED_EN::MED_EDGE:
  case MED_EN::MED_FACE:
    dimension=initial_collection.getMeshDimension()-1;
    nb_elems=_topology->getFaceNumber(idomain); 
    break;
  default:
    nb_elems=0;
    break;
  }

  if (nb_elems == 0) return;
  SCRUTE_MED(nb_elems);


  int *list= new int[nb_elems];
  MED_EN::medGeometryElement *cell_type_list= new MED_EN::medGeometryElement[nb_elems];


  //  cout << "Beginning of retrieval "<<endl;
  //retrieving global id list
  switch (entity)
  {
  case MED_EN::MED_CELL:
    _topology->getCellList(idomain,list);
    break;
  case MED_EN::MED_EDGE:
  case MED_EN::MED_FACE:
    _topology->getFaceList(idomain,list);
    break;
  default:

    break;
  }

  //retrieving cell_types
  initial_collection.getTypeList(list,nb_elems,entity,cell_type_list);
  //  cout <<"end of type retrieval"<<endl;
  //vector containing the number of cells per type
  type_numbers.clear();
  for (int icell=0; icell<nb_elems; icell++)
  {
    map<MED_EN::medGeometryElement,int>::iterator iter= type_numbers.find(cell_type_list[icell]);
    if (iter!=type_numbers.end())
      (iter->second)++;
    else  
      type_numbers[cell_type_list[icell]]=1;

  }
  //cout << "Nombre de tetras"<<type_numbers[304]<<endl;
  int nb_present_types=type_numbers.size();

  //setting the list of cells for each type
  map<MED_EN::medGeometryElement,int> index;

  map<MED_EN::medGeometryElement,int*> type_cell_list;

  MED_EN::MESH_ENTITIES::const_iterator currentEntity;
  std::map<MED_EN::medGeometryElement,int>::const_iterator iter;
  //currentEntity  = MED_EN::meshEntities.find(entity);
  for (iter = type_numbers.begin();iter != type_numbers.end(); iter++)  
  {
    MED_EN::medGeometryElement type = iter->first;
    if (!isDimensionOK(type,dimension)) continue;
    //if (iter->second==0) continue;
    index[type]=0;
    type_cell_list[type]=new int[type_numbers[type]];
    // cout << "type :"<<type<<" nb:"<<type_numbers[type]<<endl;
  }

  for (int icell=0; icell<nb_elems; icell++)
  {
    type_cell_list[cell_type_list[icell]][index[cell_type_list[icell]]++]=list[icell];
  }

  delete[]list;
  delete[]cell_type_list;

  //setting the list of present types
  int* present_type_numbers=new int[nb_present_types];
  MED_EN::medGeometryElement* type_array = new MED_EN::medGeometryElement[nb_present_types];
  MESSAGE_MED("Nb de types presents "<<nb_present_types);
  int itype=0;
  for (iter = type_numbers.begin();iter != type_numbers.end(); iter++)  
  {
    MED_EN::medGeometryElement type = iter->first;
    if (!isDimensionOK(type,dimension)) continue;

    type_array[itype]=type;

    present_type_numbers[itype]=type_numbers[type];

    MESSAGE_MED("Nombre d'elements de type "<<type<<" : "<<type_numbers[type]);
    itype++;
  }

  //retrieving connectivity in global numbering for each type
  map<MED_EN::medGeometryElement,int*> type_connectivity;
  vector<int> polygon_conn;
  vector<int> polygon_conn_index;
  vector<int> polyhedron_conn;
  vector<int> polyhedron_conn_index;
  vector<int> polyhedron_face_index;

  //Treating nodes


  for (iter = type_numbers.begin();iter != type_numbers.end(); iter++)  
  {
    MED_EN::medGeometryElement type = iter->first;


    if (!isDimensionOK(type,dimension)) continue;
    if (type != MED_EN::MED_POLYGON && type != MED_EN::MED_POLYHEDRA)
    { 
      int nbnode_per_type = (int)type%100;
      type_connectivity[type]=new int[type_numbers[type]*nbnode_per_type];
      initial_collection.getNodeConnectivity(type_cell_list[type],type_numbers[type],entity,type,type_connectivity[type]);
    }
    else if (type == MED_EN::MED_POLYGON && dimension==2)
    {
      initial_collection.getPolygonNodeConnectivity(type_cell_list[type],type_numbers[type],entity,polygon_conn,polygon_conn_index);
    }
    else if (type == MED_EN::MED_POLYHEDRA && dimension==3)
    {
      initial_collection.getPolyhedraNodeConnectivity(type_cell_list[type],type_numbers[type],entity,polyhedron_conn,polyhedron_conn_index);
    }
    delete[] type_cell_list[type];
  } 

  //creating node mapping 
  //!TODO : compute the total number of nodes 
  if (entity==MED_EN::MED_CELL)
  {
    _topology->createNodeMapping(type_connectivity,type_numbers,polygon_conn,polygon_conn_index,
                                 polyhedron_conn,polyhedron_conn_index,polyhedron_face_index,idomain);
  } 

  //converting node global numberings to local numberings
  //for (iter = (*currentEntity).second.begin();iter != (*currentEntity).second.end(); iter++)
  for (iter = type_numbers.begin();iter != type_numbers.end(); iter++)  
  {
    MED_EN::medGeometryElement type = iter->first;

    if (!isDimensionOK(type, dimension)) continue;
    if (type_numbers[type]==0) continue;
    if (type != MED_EN::MED_POLYGON && type != MED_EN::MED_POLYHEDRA)
    { 
      int nbnode_per_type = (int)type%100;
      _topology->convertToLocal2ndVersion(type_connectivity[type],type_numbers[type]*nbnode_per_type,idomain);
    }
    else if (type == MED_EN::MED_POLYGON && dimension==2)
    {
      int nbpoly = type_numbers[type]; 
      _topology->convertToLocal2ndVersion(&polygon_conn[0], polygon_conn_index[nbpoly]-1, idomain);  
    }
    else if (type == MED_EN::MED_POLYHEDRA && dimension==3)
    {
      int nbpoly = type_numbers[type]; 
      _topology->convertToLocal2ndVersion(&polyhedron_conn[0], polyhedron_face_index[polyhedron_conn_index[nbpoly]-1]-1, idomain);  
    }

  } 


  //writing coordinates
  if (entity==MED_EN::MED_CELL) 
  {
    //setting coordinates from initial_collection coordinates
    int nbnode=_topology->getNodeNumber(idomain);
    MESSAGE_MED("Number of nodes on domain "<< idomain <<" : "<<nbnode);

    double* coordinates=new double[initial_collection.getSpaceDimension()*nbnode];
    int* node_list=new int[nbnode];
    _topology->getNodeList(idomain,node_list);
    initial_collection.getCoordinates(node_list,nbnode,coordinates);
    delete[] node_list;

    // redundant specification of number of nodes is required!! MED imperfection, sorry...  

    //TODO : change MEDMEM so that it accepts a direct setting of coordinates
    // (in the present version, it is deep-copied)
    mesh_builder->setCoordinates(initial_collection.getSpaceDimension(),
                                 nbnode, coordinates, initial_collection.getSystem(),
                                 MED_EN::MED_FULL_INTERLACE);
    delete [] coordinates;
  }

  int nb_plain_types=0;
  for (iter = type_numbers.begin();iter != type_numbers.end(); iter++) 
  { 
    MED_EN::medGeometryElement type = iter->first;

    if (!isDimensionOK(type, dimension)) continue;
    if (type_numbers[type]==0) continue;
    nb_plain_types++;
  }
  mesh_builder->setNumberOfTypes(nb_plain_types,entity);
  mesh_builder->setTypes(type_array,entity);
  mesh_builder->setNumberOfElements(present_type_numbers,entity);

  delete[]present_type_numbers;
  delete[]type_array;
  //setting node connectivities
  for (iter = type_numbers.begin();iter != type_numbers.end(); iter++)  
  {
    MED_EN::medGeometryElement type = iter->first;

    if (!isDimensionOK(type,dimension)) continue;
    if (type_numbers[type]==0) continue;

    if (type != MED_EN::MED_POLYHEDRA && type != MED_EN::MED_POLYGON)
    {
      mesh_builder->setConnectivity(entity,type,type_connectivity[type]);
      delete[] type_connectivity[type];
    }
    else if (type == MED_EN::MED_POLYGON && dimension ==2)
    {
      mesh_builder->setConnectivity(entity,type,&polygon_conn[0],&polygon_conn_index[0]);
    }
    else if (type == MED_EN::MED_POLYHEDRA && dimension ==3)
    {
      mesh_builder->setConnectivity(entity,type,&polyhedron_conn[0],&polyhedron_conn_index[0]);
    }
  }
  MESSAGE_MED("end of createNodalConnectivity");
}


/*! retrieves the faces that are present in a mesh and stores them in a 
 * dynamic structure made of a map of MEDSPLITTER_FaceModel
 *
 * \param idomain domain id on which the faces are collected
 * \param face_map container storing the faces 
 */
void MESHCollection::getFaces(int idomain, 
                              map<MED_EN::medGeometryElement, vector<MEDSPLITTER_FaceModel*> >& face_map)                     
{
  MED_EN::medEntityMesh constituent_entity = getSubEntity();
  const medGeometryElement* types;
  try
  {
    types = _mesh[idomain]->getTypes(constituent_entity);
    if ( !types ) return;
  }
  catch(MEDEXCEPTION&){ return;}

  int nbtypes  = _mesh[idomain]->getNumberOfTypes(constituent_entity);
  const int* global_numbering= _mesh[idomain]->getGlobalNumberingIndex(constituent_entity);
  int* conn = const_cast<int*> (_mesh[idomain]->getConnectivity(MED_EN::MED_NODAL,constituent_entity, MED_EN::MED_ALL_ELEMENTS));
  for (int itype=0; itype<nbtypes; itype++)
  {
    for (int iface=global_numbering[itype]; iface<global_numbering[itype+1]; iface++)
    {
      MEDSPLITTER_FaceModel* face_model = new MEDSPLITTER_FaceModel();
      MED_EN::medGeometryElement type =  types[itype];
      face_model->setType(type);
      int nbnodes = type%100;
      face_model->setNbNodes(nbnodes);
      face_model->setGlobal(_topology->convertFaceToGlobal(idomain,iface));
      for (int i=0; i<nbnodes; i++)
      {
        (*face_model)[i]=*conn++;
      }
      face_map[type].push_back(face_model);
    }
  }
}

/*! retrieves the face that is common to two cells located on two different processors
 *
 * \param ip1 domain id for cell 1
 * \param ilocal1 cell id for cell 1
 * \param ip2 domain id for cell 2
 * \param ilocal2 cell id for cell 2
 * \param face_index global index for the newly created face 
 */
MEDSPLITTER_FaceModel* MESHCollection::getCommonFace(int ip1,int ilocal1,int ip2,int ilocal2,int face_index)
{
  MED_EN::medGeometryElement type1 = _mesh[ip1]->getElementType(MED_EN::MED_CELL,ilocal1);
  MEDMEM::CELLMODEL celltype1 (type1);

  const int* conn_index1 =  _mesh[ip1]->getConnectivityIndex(MED_EN::MED_NODAL,MED_EN::MED_CELL);
  const int* conn1 = _mesh[ip1]->getConnectivity(MED_EN::MED_NODAL,MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);

  // MED_EN::medGeometryElement type2 = _mesh[ip2]->getElementType(MED_EN::MED_CELL,ilocal2);
  //MEDMEM::CELLTYPE celltype2 (type2);
  const int* conn_index2 =  _mesh[ip2]->getConnectivityIndex(MED_EN::MED_NODAL,MED_EN::MED_CELL);
  const int* conn2 = _mesh[ip2]->getConnectivity(MED_EN::MED_NODAL,MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);

  vector<int> nodes1, nodes1_local;
  vector<int> nodes2;
  for (int i=  conn_index1[ilocal1-1]; i<conn_index1[ilocal1]; i++)
  {
    nodes1.push_back(_topology->convertNodeToGlobal(ip1,*(conn1+i-1)));
    nodes1_local.push_back( conn1[i-1] );
  }
  for (int i=  conn_index2[ilocal2-1]; i<conn_index2[ilocal2]; i++)
    nodes2.push_back(_topology->convertNodeToGlobal(ip2,*(conn2+i-1)));

  return MEDSPLITTER_FaceModel::getCommonFace( &nodes1[0], &nodes1_local[0], celltype1,
                                               &nodes2[0], nodes2.size(),  face_index);
}

//================================================================================
/*!
 * \brief Makes a face common for two given cells
 *  \param nodes1 - globl nodes of the first cell
 *  \param nodes1_local - local nodes of the first cell
 *  \param celltype1 - cell model of the first cell
 *  \param nodes2 - globl nodes of the second cell
 *  \param nb_nodes2 - nb of nodes of the second cell
 *  \param global_id - id of the new common face
 */
//================================================================================

MEDSPLITTER_FaceModel*
MEDSPLITTER_FaceModel::getCommonFace(const int*               nodes1,
                                     const int*               nodes1_local,
                                     const MEDMEM::CELLMODEL& celltype1,
                                     const int*               nodes2,
                                     int                      nb_nodes2,
                                     int                      global_id)
{
  int nbfaces= celltype1.getNumberOfConstituents(1);
  int ** faces = celltype1.getConstituents(1);
  MED_EN::medGeometryElement* types = celltype1.getConstituentsType(1);
  int iface=0;
  int dimension=celltype1.getDimension();

  while (iface<nbfaces)
  {
    //SCRUTE_MED (iface);
    int nbnodes= types[iface]%100;
    const int* nodes = celltype1.getNodesConstituent(1,iface+1);
    int common_nodes=0;
    for (int i=0; i<nbnodes;i++)
    {
      for (int i2=0; i2<nb_nodes2; i2++)
      {
        if (nodes1[nodes[i]-1]==nodes2[i2]) common_nodes++;
      }
    }
    if (common_nodes>=dimension) break;
    iface++;
  }

  if (iface==nbfaces)
    throw MEDEXCEPTION("MEDSPLITTER::getCommonFace - No common face found !");

  MEDSPLITTER_FaceModel* face_model = new MEDSPLITTER_FaceModel;
  face_model->setType(types[iface]);
  int nbnodes = types[iface]%100;
  face_model->setNbNodes(nbnodes);
  face_model->setGlobal(global_id); 
  for (int i=0; i<nbnodes; i++)
    (*face_model)[i]=nodes1_local[faces[iface][i]-1];

  return face_model;
}
