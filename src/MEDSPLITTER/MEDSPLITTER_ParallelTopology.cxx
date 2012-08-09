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
#include <set>
#include <map>
#include <vector>

#include "InterpKernelHashMap.hxx"

#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_ConnectZone.hxx"
#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_MeshFuse.hxx"
#include "MEDMEM_SkyLineArray.hxx"
#include "MEDMEM_Utilities.hxx"

#include "MEDSPLITTER_MESHCollection.hxx"
#include "MEDSPLITTER_Topology.hxx"
#include "MEDSPLITTER_Graph.hxx"
#include "MEDSPLITTER_ParallelTopology.hxx"

using namespace INTERP_KERNEL;

using namespace MEDSPLITTER;

//empty constructor
ParallelTopology::ParallelTopology():m_nb_domain(0),m_mesh_dimension(0)
{}

//!constructing topology according to mesh collection
ParallelTopology::ParallelTopology(const vector<MEDMEM::MESH*>& meshes, 
                                   const vector<MEDMEM::CONNECTZONE*>& cz,
                                   vector<int*>& cellglobal,
                                   vector<int*>& nodeglobal,
                                   vector<int*>& faceglobal):m_nb_domain(meshes.size())/*,m_mesh_dimension(meshes[0]->getMeshDimension())*/
{

  int index_global=0;
  int index_node_global=0;
  int index_face_global=0;

  m_nb_cells.resize(m_nb_domain);
  m_nb_nodes.resize(m_nb_domain);
  m_nb_faces.resize(m_nb_domain);

  m_loc_to_glob.resize(m_nb_domain);
  m_node_loc_to_glob.resize(m_nb_domain);
  m_face_loc_to_glob.resize(m_nb_domain);

  MED_EN::medEntityMesh constituent_entity;

  bool parallel_mode = false;
  for (int idomain=0; !parallel_mode && idomain<m_nb_domain; idomain++)
    parallel_mode = (!meshes[idomain]);

  for (int idomain=0; idomain<m_nb_domain; idomain++)
  {
    if ( !meshes[idomain] )
      continue;
    m_mesh_dimension = meshes[idomain]->getMeshDimension();
    constituent_entity = (m_mesh_dimension == 3 ? MED_EN::MED_FACE : MED_EN::MED_EDGE );

    //creating cell maps
    m_nb_cells[idomain]=meshes[idomain]->getNumberOfElements(MED_EN::MED_CELL, MED_EN::MED_ALL_ELEMENTS);
    //              cout << "Nb cells (domain "<<idomain<<") = "<<m_nb_cells[idomain];
    m_loc_to_glob[idomain].resize(m_nb_cells[idomain]);

    if (cellglobal[idomain]==0 || parallel_mode)
    {
      MESSAGE_MED("Creating global numbering"); 
      //creating global numbering from scratch
      for (int i=0; i<m_nb_cells[idomain]; i++)
      {
        index_global++;
        m_glob_to_loc[index_global]=make_pair(idomain,i+1);
        //m_loc_to_glob[make_pair(idomain,i+1)]=index_global;
        m_loc_to_glob[idomain][i]=index_global;
        //        cout<<"glob:"<<index_global<<" --> ("<<idomain<<","<<i+1<<")"<<endl;
      }
    }
    //using global numbering coming from a previous numbering
    else
    {
      MESSAGE_MED("Using former global numbering");
      for (int i=0; i<m_nb_cells[idomain]; i++)
      {
        int global=cellglobal[idomain][i];
        m_glob_to_loc[global]=make_pair(idomain,i+1);
        //m_loc_to_glob[make_pair(idomain,i+1)]=global;
        m_loc_to_glob[idomain][i]=global;
        index_global++;
        //        cout<<"glob:"<<global<<" --> ("<<idomain<<","<<i+1<<")"<<endl;
      }
    }

    //cas sequentiel
    if (m_nb_domain==1)
    {
      m_nb_total_cells=index_global;
      m_nb_cells[0]=index_global;
      m_node_loc_to_glob[idomain].resize(meshes[idomain]->getNumberOfNodes());
      for (int i=0; i<meshes[idomain]->getNumberOfNodes(); i++)
      {
        m_node_glob_to_loc.insert(make_pair(i+1,make_pair(0,i+1)));
        //m_node_loc_to_glob.insert(make_pair(make_pair(0,i+1), i+1));
        m_node_loc_to_glob[0][i]=i+1;
      }
      m_nb_total_nodes=meshes[idomain]->getNumberOfNodes();   
      m_nb_nodes[0]=m_nb_total_nodes; 

      //                      meshes[idomain]->getConnectivity( MED_EN::MED_DESCENDING, MED_EN::MED_CELL, MED_EN::MED_ALL_ELEMENTS); 
      int nbfaces=meshes[idomain]->getNumberOfElements(constituent_entity,MED_EN::MED_ALL_ELEMENTS);
      m_face_loc_to_glob[idomain].resize(nbfaces);
      for (int i=0; i<nbfaces; i++)
      {
        m_face_glob_to_loc.insert(make_pair(i+1,make_pair(0,i+1)));
        //m_face_loc_to_glob.insert(make_pair(make_pair(0,i+1), i+1));
        m_face_loc_to_glob[0][i]=i+1;
      }
      m_nb_total_faces=nbfaces;   
      m_nb_faces[0]=nbfaces;
      MESSAGE_MED ("nb total cells "<< m_nb_total_cells);
      MESSAGE_MED("nb total nodes "<< m_nb_total_nodes);  
      MESSAGE_MED("nb total faces "<< m_nb_total_faces);  
      return;
    }

    //creating node maps
    m_nb_nodes[idomain]=meshes[idomain]->getNumberOfNodes();
    INTERP_KERNEL::HashMap <int,pair<int,int> > local2distant;
    m_node_loc_to_glob[idomain].resize(m_nb_nodes[idomain]);
    for (unsigned icz=0; icz<cz.size(); icz++)
    {
      if (cz[icz]->getLocalDomainNumber() == idomain && 
          cz[icz]->getLocalDomainNumber()>cz[icz]->getDistantDomainNumber())
      {
        int nb_node= cz[icz]->getNodeNumber();
        const int* node_corresp=cz[icz]->getNodeCorrespValue();
        int distant_ip = cz[icz]->getDistantDomainNumber();
        for (int i=0; i< nb_node; i++)
        {
          int local= node_corresp[i*2];
          int distant = node_corresp[i*2+1];
          local2distant.insert(make_pair(local, make_pair(distant_ip,distant)));    
        }
      }
    }
    // setting mappings for all nodes
    if (nodeglobal[idomain]==0)
    {
      for (int inode=0; inode<m_nb_nodes[idomain]; inode++)
      {
        if (local2distant.find(inode+1)==local2distant.end())
        {
          index_node_global++;
          m_node_glob_to_loc.insert(make_pair(index_node_global,make_pair(idomain,inode+1)));
          //m_node_loc_to_glob[make_pair(idomain,inode+1)]=index_node_global;
          m_node_loc_to_glob[idomain][inode]=index_node_global;
        }   
        else
        {
          int ip = (local2distant.find(inode+1)->second).first;
          int distant = (local2distant.find(inode+1)->second).second;
          //int global_number=m_loc_to_glob[make_pair(ip,distant)];
          int global_number=m_loc_to_glob[ip][distant-1];
          m_node_glob_to_loc.insert(make_pair(global_number,make_pair(idomain,inode+1)));
          //m_node_loc_to_glob[make_pair(idomain,inode+1)]=global_number;
          m_node_loc_to_glob[idomain][inode]=global_number;
        } 
      }
    }
    //using former node numbering
    else
    {//       cout << "("<<idomain<<","<<i+1<<")->"<<i+1<<endl;
      for (int inode=0; inode<m_nb_nodes[idomain]; inode++)
      {
        int global_number=nodeglobal[idomain][inode];
        //        cout << "global_number "<<global_number<<endl;
        m_node_glob_to_loc.insert(make_pair(global_number,make_pair(idomain,inode+1)));
        //m_node_loc_to_glob[make_pair(idomain,inode+1)]=global_number;
        m_node_loc_to_glob[idomain][inode]=global_number;
      }
    }


    //creating  dimension d-1 component mappings

    //              meshes[idomain]->getConnectivity( MED_EN::MED_DESCENDING, MED_EN::MED_CELL, MED_EN::MED_ALL_ELEMENTS); 
    m_nb_faces[idomain]=meshes[idomain]->getNumberOfElements(constituent_entity,MED_EN::MED_ALL_ELEMENTS);
    MESSAGE_MED ("Nb faces domain " << idomain<<m_nb_faces[idomain]);
    m_face_loc_to_glob[idomain].resize(m_nb_faces[idomain]);
    local2distant.clear();
    for (unsigned icz=0; icz<cz.size(); icz++)
    {
      if (cz[icz]->getLocalDomainNumber() == idomain && 
          cz[icz]->getLocalDomainNumber()>cz[icz]->getDistantDomainNumber())
      {
        int nb_face= cz[icz]->getFaceNumber();
        const int* face_corresp=cz[icz]->getFaceCorrespValue();
        int distant_ip = cz[icz]->getDistantDomainNumber();
        for (int i=0; i< nb_face; i++)
        {
          int local= face_corresp[i*2];
          int distant = face_corresp[i*2+1];
          local2distant.insert(make_pair(local, make_pair(distant_ip,distant)));    
        }
      }
    }
    // setting mappings for all faces
    if (faceglobal[idomain]==0)
    {
      for (int iface=0; iface<m_nb_faces[idomain]; iface++)
      {
        if (local2distant.find(iface+1)==local2distant.end())
        {
          index_face_global++;
          m_face_glob_to_loc.insert(make_pair(index_face_global,make_pair(idomain,iface+1)));
          //m_face_loc_to_glob[make_pair(idomain,iface+1)]=index_face_global;
          m_face_loc_to_glob[idomain][iface]=index_face_global;
        }   
        else
        {
          int ip = (local2distant.find(iface+1)->second).first;
          int distant = (local2distant.find(iface+1)->second).second;
          //int global_number=m_loc_to_glob[make_pair(ip,distant)];
          int global_number=m_loc_to_glob[ip][distant-1];
          m_face_glob_to_loc.insert(make_pair(global_number,make_pair(idomain,iface+1)));
          //m_face_loc_to_glob[make_pair(idomain,iface+1)]=global_number;
          m_face_loc_to_glob[idomain][iface]=global_number;
        } 
      }
    }
    //using former face numbering
    else
    {
      for (int iface=0; iface<m_nb_faces[idomain]; iface++)
      {
        int global_number=faceglobal[idomain][iface];
        //SCRUTE_MED(global_number);
        m_face_glob_to_loc.insert(make_pair(global_number,make_pair(idomain,iface+1)));
        //m_face_loc_to_glob[make_pair(idomain,iface+1)]=global_number;
        m_face_loc_to_glob[idomain][iface]=global_number;
      }
    }
  }

  m_nb_total_cells=index_global;
  m_nb_total_nodes=index_node_global;   
  m_nb_total_faces=index_face_global;
  SCRUTE_MED(m_nb_total_cells);
  SCRUTE_MED(m_nb_total_faces);
  SCRUTE_MED(m_nb_total_nodes);

}


//!constructing ParallelTopology from an old topology and a graph
ParallelTopology::ParallelTopology(boost::shared_ptr<Graph> graph, int nb_domain, int mesh_dimension):
  m_nb_domain(nb_domain),
  m_mesh_dimension(mesh_dimension),
  m_nb_cells(graph->nbVertices()),
  m_graph(graph)
{
  m_nb_cells.resize(m_nb_domain);
  m_nb_nodes.resize(m_nb_domain);
  m_nb_faces.resize(m_nb_domain);

  m_loc_to_glob.resize(m_nb_domain);
  m_node_loc_to_glob.resize(m_nb_domain);
  m_face_loc_to_glob.resize(m_nb_domain);

  // used in parallel mode only
  m_cell_loc_to_glob_fuse.resize(m_nb_domain);
  m_face_loc_to_glob_fuse.resize(m_nb_domain);

  for (int i=0; i<m_nb_domain; i++)
    m_nb_cells[i]=0;  

  const int* part = graph-> getPart();
  m_nb_total_cells= graph->nbVertices();

  for (int icell=0; icell<m_nb_total_cells; icell++)
  {
    int idomain = part[icell];
    m_nb_cells[idomain]++;
    //m_loc_to_glob[make_pair(idomain,m_nb_cells[idomain])]=icell+1;
    m_loc_to_glob[idomain].push_back(icell+1);
    m_glob_to_loc[icell+1]=make_pair(idomain,m_nb_cells[idomain]);

  }
  for (int idomain=0; idomain<m_nb_domain; idomain++)
    MESSAGE_MED("Nombre de cellules dans le domaine "<< idomain <<" : "<<m_nb_cells[idomain]); 

  SCRUTE_MED(m_nb_total_cells);

}

ParallelTopology::~ParallelTopology()
{

} 

/*!Converts a list of global node numbers
 * to a distributed array with local cell numbers.
 *
 * If a node in the list is represented on several domains,
 * only the first value is returned
 * */
void ParallelTopology::convertGlobalNodeList(const int* node_list, int nbnode, int* local, int* ip)
{
  if (m_node_glob_to_loc.empty()) 
    throw MEDMEM::MEDEXCEPTION("convertGlobalNodeList - Node mapping has not yet been built");
  for (int i=0; i< nbnode; i++)
  {
    pair<int,int> local_node = m_node_glob_to_loc.find(node_list[i])->second;
    ip[i]=local_node.first;
    local[i]=local_node.second;
  }
}

/*!Converts a list of global node numbers on domain ip
 * to a distributed array with local cell numbers.
 *
 * If a node in the list is represented on several domains,
 * only the value with domain ip is returned
 *
 * */
void ParallelTopology::convertGlobalNodeList(const int* node_list, int nbnode, int* local, int ip)
{
  if (m_node_glob_to_loc.empty()) 
    throw MEDMEM::MEDEXCEPTION("convertGlobalNodeList - Node mapping has not yet been built");

  for (int i=0; i< nbnode; i++)
  {
    typedef INTERP_KERNEL::HashMultiMap<int,pair<int,int> >::iterator mmiter;
    pair<mmiter,mmiter> range=m_node_glob_to_loc.equal_range(node_list[i]);
    for (mmiter it=range.first; it !=range.second; it++)
    { 
      int ipfound=(it->second).first;
      if (ipfound==ip)
        local[i]=(it->second).second;
    }
  }
} 

/*!Converts a list of global node numbers
 * to a distributed array with local cell numbers.
 *
 * If a node in the list is represented on several domains,
 * all the values are put in the array
 * */
void ParallelTopology::convertGlobalNodeListWithTwins(const int* node_list, int nbnode, int*& local, int*& ip,int*& full_array, int& size)
{
  if (m_node_glob_to_loc.empty()) 
    throw MEDMEM::MEDEXCEPTION("convertGlobalNodeList - Node mapping has not yet been built");

  size=0;
  for (int i=0; i< nbnode; i++)
  {
    int count= m_node_glob_to_loc.count(node_list[i]);
    //      if (count > 1) 
    //        cout << "noeud " << node_list[i]<< " doublon d'ordre " << count<<endl;
    size+=count;
  }
  int index=0;
  ip=new int[size];
  local=new int[size];
  full_array=new int[size];
  for (int i=0; i< nbnode; i++)
  {
    typedef INTERP_KERNEL::HashMultiMap<int,pair<int,int> >::iterator mmiter;
    pair<mmiter,mmiter> range=m_node_glob_to_loc.equal_range(node_list[i]);
    for (mmiter it=range.first; it !=range.second; it++)
    { 
      ip[index]=(it->second).first;
      local[index]=(it->second).second;
      full_array [index]=node_list[i];
      index++;
    }

  }
}

/*!Converts a list of global face numbers
 * to a distributed array with local face numbers.
 *
 * If a face in the list is represented on several domains,
 * all the values are put in the array
 * */
void ParallelTopology::convertGlobalFaceListWithTwins(const int* face_list, int nbface, int*& local, int*& ip, int*& full_array,int& size)
{
  size=0;
  for (int i=0; i< nbface; i++)
  {
    //int count = m_face_glob_to_loc.count(face_list[i]);
    //if (count >1) MESSAGE_MED("face en doublon "<<face_list[i]);
    size+= m_face_glob_to_loc.count(face_list[i]);
  }
  int index=0;
  ip=new int[size];
  local=new int[size];
  full_array=new int[size];
  for (int i=0; i< nbface; i++)
  {
    typedef INTERP_KERNEL::HashMultiMap<int,pair<int,int> >::iterator mmiter;
    pair<mmiter,mmiter> range=m_face_glob_to_loc.equal_range(face_list[i]);
    for (mmiter it=range.first; it !=range.second; it++)
    { 
      ip[index]=(it->second).first;
      local[index]=(it->second).second;
      full_array[index]=face_list[i];
      index++;
    }

  }
}

//!converts a list of global cell numbers
//!to a distributed array with local cell numbers 
void ParallelTopology::convertGlobalCellList(const int* cell_list, int nbcell, int* local, int* ip)
{
  for (int i=0; i< nbcell; i++)
  {
    INTERP_KERNEL::HashMap<int, pair<int,int> >::const_iterator iter = m_glob_to_loc.find(cell_list[i]);
    ip[i]=(iter->second).first;
    local[i]=(iter->second).second;
  }
}

/*!Converts a list of global face numbers
 * to a distributed array with local face numbers
 */ 
void ParallelTopology::convertGlobalFaceList(const int* face_list, int nbface, int* local, int* ip)
{
  for (int i=0; i< nbface; i++)
  {
    INTERP_KERNEL::HashMap<int, pair<int,int> >::const_iterator iter = m_face_glob_to_loc.find(face_list[i]);
    if (iter == m_face_glob_to_loc.end())
    {
      throw MED_EXCEPTION("convertGlobalFaceList - Face  not found");
    }
    ip[i]=(iter->second).first;
    local[i]=(iter->second).second;
    //    cout << " in convertGlobalFAceList face global "<<face_list[i]<<" -> ("<<ip[i]<<","<<local[i]<<")"<<endl;
  }
}

/*!Converts a list of global node numbers on domain ip
 * to a distributed array with local cell numbers.
 *
 * If a node in the list is represented on several domains,
 * only the value with domain ip is returned
 *
 * */
void ParallelTopology::convertGlobalFaceList(const int* face_list, int nbface, int* local, int ip)
{
  for (int i=0; i< nbface; i++)
  {
    typedef INTERP_KERNEL::HashMultiMap<int,pair<int,int> >::iterator mmiter;
    pair<mmiter,mmiter> range=m_face_glob_to_loc.equal_range(face_list[i]);
    for (mmiter it=range.first; it !=range.second; it++)
    { 
      int ipfound=(it->second).first;
      if (ipfound==ip)
        local[i]=(it->second).second; 

    }
  }
} 


////creating node mapping 
void ParallelTopology::createNodeMapping(map<MED_EN::medGeometryElement,int*>& type_connectivity,
                                         map<MED_EN::medGeometryElement,int>& present_type_numbers,
                                         vector<int>& polygon_conn,
                                         vector<int>& polygon_conn_index,
                                         vector<int>& polyhedron_conn,
                                         vector<int>& polyhedron_conn_index,
                                         vector<int>& polyhedron_face_index,
                                         int idomain)
{
  set<int> local_numbers;
  int local_index=0;

  map<MED_EN::medGeometryElement,int>::const_iterator iter;
  for (iter = present_type_numbers.begin(); iter!=present_type_numbers.end();iter++)
  {
    int type=iter->first;
    int nodes_per_type= type%100;

    if (!((type/100==m_mesh_dimension)
          ||(type==MED_EN::MED_POLYGON && m_mesh_dimension==2)
          ||(type==MED_EN::MED_POLYHEDRA && m_mesh_dimension==3))) continue;

    if (type != MED_EN::MED_POLYGON && type != MED_EN::MED_POLYHEDRA)
    {
      for (int icell=0; icell<present_type_numbers[type]; icell++)
      {
        for (int inode=0; inode<nodes_per_type; inode++)
        {
          int global=type_connectivity[type][icell*nodes_per_type+inode];
          if(local_numbers.find(global)==local_numbers.end())
          {
            local_index++;
            local_numbers.insert(global);
            //m_node_loc_to_glob.insert(make_pair(make_pair(idomain,local_index),global));
            m_node_loc_to_glob[idomain].push_back(global);
            m_node_glob_to_loc.insert(make_pair(global,make_pair(idomain,local_index)));
            //          cout << "node : global ="<<global<<" local =("<<idomain<<","<<local_index<<")"<<endl;         
          }
        }

      }
    }
    else if (type== MED_EN::MED_POLYGON)
    {
      for ( unsigned i = 0; i < polygon_conn.size(); ++i )
      {
        int global=polygon_conn[i];
        if(local_numbers.find(global)==local_numbers.end())
        {
          local_index++;
          local_numbers.insert(global);
          m_node_loc_to_glob[idomain].push_back(global);
          m_node_glob_to_loc.insert(make_pair(global,make_pair(idomain,local_index)));
        }
      }
    }
    else if (type==MED_EN::MED_POLYHEDRA)
    {
      for ( unsigned i = 0; i < polyhedron_conn.size(); ++i )
      {
        int global=polyhedron_conn[i];
        if(local_numbers.find(global)==local_numbers.end())
        {
          local_index++;
          local_numbers.insert(global);
          m_node_loc_to_glob[idomain].push_back(global);
          m_node_glob_to_loc.insert(make_pair(global,make_pair(idomain,local_index)));
        }
      }
    }

  }
  m_nb_nodes[idomain]=local_index;
}

//================================================================================
/*!
 * \brief Return true if the domain mesh contains a cell based on given global nodes
 */
//================================================================================

bool ParallelTopology::hasCellWithNodes( const MESHCollection& new_collection,
                                         int                   domain,
                                         const set<int>&       globNodes)
{
  // convert global nodes to local in the given domain
  set<int> nodes;
  set<int>::const_iterator n = globNodes.begin();
  for ( ; n != globNodes.end(); ++n )
    nodes.insert( convertGlobalNode( *n, domain ));

  const MED_EN::medConnectivity connType = MED_EN::MED_NODAL;
  const MED_EN::medEntityMesh   entity   = MED_EN::MED_CELL;

  // loop on all types of cells
  const MEDMEM::MESH* mesh = new_collection.getMesh( domain );
  int nbTypes = mesh->getNumberOfTypes( entity );
  const MED_EN::medGeometryElement * types = mesh->getTypes( entity );
  for ( int t = 0; t < nbTypes; ++t )
  {
    // get connectivity
    if ( !mesh->existConnectivity( connType, entity ))
      continue;
    int nbCell = mesh->getNumberOfElements( entity, types[t] );
    const int *conn, *index;
    conn  = mesh->getConnectivity(connType, entity, types[t]);
    index = mesh->getConnectivityIndex(connType, entity);
    // find a cell containing the first of given nodes,
    // then check if the found cell contains all the given nodes
    const int firstNode = *nodes.begin();
    for ( int i = 0; i < nbCell; ++i )
    {
      for ( int j = index[i]-1; j < index[i+1]-1; ++j )
        if ( conn[j] == firstNode )
        {
          unsigned nbSame = 0;
          for ( j = index[i]-1; j < index[i+1]-1; ++j )
            nbSame += nodes.count( conn[j] );
          if ( nbSame == nodes.size() )
            return true;
          break;
        }
    }
  }
  return false;
}

////creating face mapping 
void ParallelTopology::createFaceMapping(const MESHCollection& initial_collection,
                                         const MESHCollection& new_collection)
  //                     map<MED_EN::medGeometryElement,int*>& type_list,
  //                     map<MED_EN::medGeometryElement,int>& present_type_numbers,
  //                     int idomain

{
  // containers for the new topology
  vector<int> new_counts(m_nb_domain,0);
  vector<int> domain_counts(m_nb_domain,0);
  const Topology* old_topology=initial_collection.getTopology();
  int nb_domain_old=old_topology->nbDomain();
  int global_index=old_topology->getFaceNumber();
  //cout << "nb faces = " << global_index << endl;
  set <pair<int, pair<int,int> > > global_treated;

  //definition of the d-1 constituent for the considered mesh dimension
  MED_EN::medEntityMesh constituent_entity;
  switch (m_mesh_dimension)
  {
  case 3:
    constituent_entity= MED_EN::MED_FACE;
    break;
  case 2:
    constituent_entity = MED_EN::MED_EDGE;
    break;
  }

  for (int iold=0; iold<nb_domain_old;iold++)
  {
    if ( !initial_collection.getMesh(iold) ) continue;
    int nbtotalface = initial_collection.getMesh(iold)->getNumberOfElements(constituent_entity,MED_EN::MED_ALL_ELEMENTS);
    SCRUTE_MED(nbtotalface);
    const int* face_conn = 0;
    const int* face_offset = 0;
    if (nbtotalface >0)
    {
      face_conn = initial_collection.getMesh(iold)->getConnectivity(
                                                                    MED_EN::MED_NODAL,constituent_entity,MED_EN::MED_ALL_ELEMENTS);
      face_offset = initial_collection.getMesh(iold)->getConnectivityIndex(MED_EN::MED_NODAL,constituent_entity);
    }
    for (int iface=0;iface<nbtotalface; iface++)
    {
      int global_face_number = old_topology->convertFaceToGlobal(iold,iface+1);

      //      int inode = face_offset[iface];
      for (int i=0; i<m_nb_domain; i++) domain_counts[i]=0;
      set <int> nodes;
      int nbnodes;
      {
        nbnodes=face_offset[iface+1]-face_offset[iface];
        for (int inode= face_offset[iface];inode < face_offset[iface+1]; inode++)
        {
          int node=face_conn[inode-1];

          int global = old_topology->convertNodeToGlobal(iold,node);
          //        cout << "global node "<<global<<"ip "<<iold<< "noeud"<<node<<endl;
          nodes.insert(global);
          typedef INTERP_KERNEL::HashMultiMap<int,pair<int,int> >::iterator mmiter;
          pair<mmiter,mmiter> range=m_node_glob_to_loc.equal_range(global);

          int ip;
          for (mmiter it=range.first; it !=range.second; it++)
          { 
            ip=(it->second).first;
            domain_counts[ip]++;
          }
        }
      }

      set<int>::const_iterator iter_node = nodes.begin();
      int numbers[3];
      numbers[2]=0; // for segments
      for (int i=0; i<nbnodes; i++)
      {
        numbers[i]=*iter_node;
        iter_node++;
      }
      set <pair<int, pair<int,int> > > ::iterator iter_triplets;
      pair<int, pair<int,int> > triplet = make_pair(numbers[0],make_pair(numbers[1],numbers[2]));
      iter_triplets=global_treated.find(triplet);
      if (iter_triplets==global_treated.end())
      {
        global_treated.insert(triplet);
        //  int nbnodes=face_offset[iface+1]-face_offset[iface];
        if (global_face_number == -1) 
        {
          global_index++;
          global_face_number=global_index;

        }
        //  SCRUTE_MED(nbnodes);

        for (int inew=0;inew<m_nb_domain;inew++)
        {
          //     SCRUTE_MED(domain_counts[inew]);
          if(domain_counts[inew]==nbnodes)
          {
            if ( !hasCellWithNodes( new_collection, inew, nodes ))
              continue; // 0020861: EDF 1387 MED: Result of medsplitter gives standalone triangles

            new_counts[inew]++;
            m_face_glob_to_loc.insert(make_pair(global_face_number,make_pair(inew,new_counts[inew])));
            //m_face_loc_to_glob.insert(make_pair(make_pair(inew,new_counts[inew]),global_face_number));
            m_face_loc_to_glob[inew].push_back(global_face_number);
          }
        }
        //cout << "global_face_number = " << global_face_number << endl;
      }
    }
  }

  for (int inew=0;inew<m_nb_domain;inew++)
  {
    m_nb_faces[inew]=new_counts[inew];
    MESSAGE_MED(" Nb faces ["<<inew<<"]="<<m_nb_faces[inew]);
  }
  MESSAGE_MED(" total number of faces"<<getFaceNumber());
}

////creating node mapping 
void ParallelTopology::createFaceMapping2ndversion(const MESHCollection& initial_collection)

{
  // containers for the new topology
  vector<int> new_counts(m_nb_domain,0);
  vector<int> domain_counts(m_nb_domain,0);
  const Topology* old_topology=initial_collection.getTopology();
  int nb_domain_old=old_topology->nbDomain();
  //int global_index=old_topology->getFaceNumber();
  //  set <pair<int, pair<int,int> > > global_treated;

  //definition of the d-1 constituent for the considered mesh dimension
  MED_EN::medEntityMesh constituent_entity;
  switch (m_mesh_dimension)
  {
  case 3:
    constituent_entity= MED_EN::MED_FACE;
    break;
  case 2:
    constituent_entity = MED_EN::MED_EDGE;
    break;
  }

  for (int iold=0; iold<nb_domain_old;iold++)
  {
    int nbcell=old_topology->getCellNumber(iold);

    const int* face_conn = initial_collection.getMesh(iold)->
      getConnectivity(MED_EN::MED_DESCENDING,MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
    const int* face_offset = initial_collection.getMesh(iold)->
      getConnectivityIndex(MED_EN::MED_DESCENDING,MED_EN::MED_CELL);
    MESSAGE_MED("end of connectivity calculation");
    set<int> global_treated;
    for (int icell=0; icell<nbcell; icell++)
    {
      int global_cell_number=old_topology->convertCellToGlobal(iold,icell+1);
      int inew = getCellDomainNumber(global_cell_number);

      for (int iface = face_offset[icell]; iface < face_offset[icell+1]; iface++)
      {
        int global_face_number=old_topology->convertFaceToGlobal(iold,abs(face_conn[iface-1]));
        if (global_treated.find (global_face_number)==global_treated.end())
        {
          new_counts[inew]++;
          m_face_glob_to_loc.insert(make_pair(global_face_number,make_pair(inew,new_counts[inew])));

          //m_face_loc_to_glob.insert(make_pair(make_pair(inew,new_counts[inew]),global_face_number));
          m_face_loc_to_glob[inew].push_back(global_face_number);
          global_treated.insert(global_face_number);

        }
      }
    }
  }


  for (int inew=0;inew<m_nb_domain;inew++)
  {
    m_nb_faces[inew]=new_counts[inew];
    //  cout << " Nb faces ["<<inew<<"]="<<m_nb_faces[inew]<<endl;
  }
  MESSAGE_MED(" total number of faces"<<getFaceNumber());
}


//replacing a table of global numbering with a table with local numberings
// type_connectivity contains global connectivity for each type in input
// type_connectivity contains local connectivity for each type in output
void ParallelTopology::convertToLocal(map<MED_EN::medGeometryElement,int*>& type_connectivity,
                                      map<MED_EN::medGeometryElement,int>& present_type_numbers,
                                      int idomain,
                                      MED_EN::medEntityMesh entity)
{
  int dimension;
  switch (entity)
  {
  case MED_EN::MED_CELL:
    dimension=m_mesh_dimension;
    break;
  case MED_EN::MED_FACE:
    dimension=2;
    break;
  case MED_EN::MED_EDGE:
    dimension=1;
    break;
  } 

  MED_EN::MESH_ENTITIES::const_iterator currentEntity;
  list<MED_EN::medGeometryElement>::const_iterator iter;
  currentEntity  = MED_EN::meshEntities.find(MED_EN::MED_CELL);

  for (iter = (*currentEntity).second.begin();iter != (*currentEntity).second.end(); iter++)
  {
    MED_EN::medGeometryElement type = (*iter);
    if (type/100 != dimension) continue;
    for (int inode=0; inode<present_type_numbers[type]*(type%100); inode++)
    {
      //      cout <<" inode :"<<inode<< " global = "<<type_connectivity[type][inode];
      int global = type_connectivity[type][inode];
      typedef INTERP_KERNEL::HashMultiMap<int,pair<int,int> >::iterator mmiter;
      pair<mmiter,mmiter> range=m_node_glob_to_loc.equal_range(global);
      for (mmiter it=range.first; it !=range.second; it++)
      {
        if ((it->second).first==idomain)
          type_connectivity[type][inode]=(it->second).second;
      }
      //      cout << " local = " <<type_connectivity[type][inode]<<endl;
    }

  }
}

//replacing a table of global numbering with a table with local numberings
// type_connectivity contains global connectivity for each type in input
// type_connectivity contains local connectivity for each type in output
void ParallelTopology::convertToLocal2ndVersion(int* nodes, int nbnodes, int idomain)
{
  for (int inode=0; inode<nbnodes; inode++)
  {
    //      cout <<" inode :"<<inode<< " global = "<<type_connectivity[type][inode];
    int global = nodes[inode];
    typedef INTERP_KERNEL::HashMultiMap<int,pair<int,int> >::iterator mmiter;
    pair<mmiter,mmiter> range=m_node_glob_to_loc.equal_range(global);
    for (mmiter it=range.first; it !=range.second; it++)
    {
      if ((it->second).first==idomain)
        nodes[inode]=(it->second).second;
    }        
  }
}


//! computing arrays with node/node correspondencies
void ParallelTopology::computeNodeNodeCorrespondencies(int idomain, vector<MEDMEM::MEDSKYLINEARRAY*>& corr) const
{
  vector <int> sizes (m_nb_domain,0);
  vector <int*> values (m_nb_domain);
  for (int i=0; i<m_nb_domain; i++)
  {
    values[i]=0;
  }

  for (int inode=0; inode<m_nb_nodes[idomain]; inode++)
  {
    //int global = (m_node_loc_to_glob.find(make_pair(idomain,inode+1)))->second;
    int global = m_node_loc_to_glob[idomain][inode];
    typedef INTERP_KERNEL::HashMultiMap<int,pair<int,int> >::const_iterator mm;
    pair<mm,mm> range = m_node_glob_to_loc.equal_range(global);
    for (mm it=range.first; it !=range.second; it++)
    {
      int id = (it->second).first;
      if (id !=idomain)
      {
        sizes[id]++;
      }
    }
  }

  for (int ip=0; ip<m_nb_domain; ip++)
  {
    if (sizes[ip]>0)
      values[ip]=new int[2*sizes[ip]];
    sizes[ip]=0;
  }


  for (int inode=0; inode<m_nb_nodes[idomain]; inode++)
  {
    //int global = (m_node_loc_to_glob.find(make_pair(idomain,inode+1)))->second;
    int global = m_node_loc_to_glob[idomain][inode];
    typedef INTERP_KERNEL::HashMultiMap<int,pair<int,int> >::const_iterator mm;
    pair<mm,mm> range = m_node_glob_to_loc.equal_range(global);
    for (mm it=range.first; it !=range.second; it++)
    {
      int id = (it->second).first;
      if (id !=idomain)
      {
        values[id][sizes[id]*2]=inode+1;
        values[id][sizes[id]*2+1]=(it->second).second;
        sizes[id]++;
      }
    }
  }

  for (int i=0; i< m_nb_domain; i++)
  {
    if (sizes[i]!=0)
    {
      int* index = new int[sizes[i]+1];
      for (int j=0; j<sizes[i]+1; j++)
        index[j]=j+1;
      corr[i]=new MEDMEM::MEDSKYLINEARRAY(sizes[i],2*sizes[i],index,values[i]);
      delete[] index;
      delete[] values[i];
    }
  }
}

//================================================================================
/*!
 * \brief computing arrays with cell/cell correspondencies
 *  \param idomain - domain for which to compute correspondencies
 *  \param corr - out correspondencies
 *  \param graph - graph containing new cell distribution among domains
 *  \param id_shift - shitf to get cell id global on this processor from id global
 *                    over all procs
 */
//================================================================================

void ParallelTopology::computeCellCellCorrespondencies(int idomain, vector<MEDMEM::MEDSKYLINEARRAY*>& corr, const Graph* graph) const
{
  vector <int> sizes (m_nb_domain,0);
  vector <int*> values (m_nb_domain);
  for (int i=0; i<m_nb_domain; i++)
  {
    values[i]=0;
  }

  vector <INTERP_KERNEL::HashMultiMap<int,int> > cell_corresp;
  //TODO : remplacer int* par une map <int,int>
  //  vector<int*  > number_of_connections(m_nb_domain);
  //  vector<map<int,int> > number_of_connections;
  vector<map<int,int> > number_of_connections;
  cell_corresp.resize(m_nb_domain);
  number_of_connections.resize(m_nb_domain);
  //  for (int i=0; i<m_nb_domain; i++)
  //    {
  //      //    cell_corresp[i]=new multimap<int,int>;
  //      if (m_nb_cells[i] >0)
  //        {
  //          number_of_connections[i]=new int[m_nb_cells[idomain]];
  //          for (int j=0; j<m_nb_cells[idomain]; j++)
  //            number_of_connections[i][j]=0;
  //        }
  //    }

  const MEDMEM::MEDSKYLINEARRAY* skylinegraph = graph->getGraph();

  const int* index=skylinegraph->getIndex();
  const int* value=skylinegraph->getValue();

  TGlob2DomainLoc::const_iterator gl_do_lo_end = m_glob_to_loc.end();

  for (int icell=0; icell<m_nb_cells[idomain]; icell++)
  {
    int global= m_loc_to_glob[idomain][icell];
    for (int ii=index[global-1]-1; ii<index[global]-1; ii++)
    {
      int distant_global=value[ii];

      const pair<int,int>& local = m_glob_to_loc.find(distant_global)->second;

      if (local.first != idomain)
      {
        cell_corresp[local.first].insert(make_pair(icell+1,local.second));
        //              number_of_connections[local.first][icell]++;
        if (number_of_connections[local.first].find(icell)==number_of_connections[local.first].end())
          number_of_connections[local.first].insert(make_pair(icell,1));
        else
          number_of_connections[local.first][icell]++;

      }
    }
  }

  for (int inew=0; inew<m_nb_domain; inew++)
  {
    if (inew==idomain || number_of_connections[inew].empty()) continue;

    int* new_index=new int[m_nb_cells[idomain]+1];
    new_index[0]=1;
    for (int i=0; i<m_nb_cells[idomain]; i++)
    {

      if (number_of_connections[inew].find(i)!=number_of_connections[inew].end())
        new_index[i+1]=new_index[i]+number_of_connections[inew][i];
      else
        new_index[i+1]=new_index[i];
    }
    int* new_value;
    if (new_index[m_nb_cells[idomain]]-1 > 0)
      new_value=new int[new_index[m_nb_cells[idomain]]-1];
    else 
      new_value=0;

    int value_i=0;

    //                      INTERP_KERNEL::HashMultiMap<int,int>::iterator iter=cell_corresp[inew].begin();

    for (int i=0; i<m_nb_cells[idomain]; i++)
    {
      //          for (int j=new_index[i];j<new_index[i+1];j++,value_i++,iter++)
      //            new_value[value_i]=iter->second;

      typedef INTERP_KERNEL::HashMultiMap<int,int>::iterator mmiter;
      pair<mmiter,mmiter> range=cell_corresp[inew].equal_range(i+1);
      for (mmiter it=range.first; it!=range.second; it++)
      {
        new_value[value_i]=it->second;
        value_i++;
      }
    }
    if (value_i>0)    
    {
      corr[inew] = new MEDMEM::MEDSKYLINEARRAY(m_nb_cells[idomain], new_index[m_nb_cells[idomain]]-1, new_index,new_value,true);
    }
    else 
    {
      corr[inew]=0;
      if (new_value !=0) delete[]new_value;
      delete[]new_index;
    }   

  }

  //    for (int inew=0; inew<m_nb_domain; inew++)
  //    if (m_nb_cells[inew]>0)
  //      delete[] number_of_connections[inew];

}

//================================================================================
/*!
 * \brief Return max global face number
 */
//================================================================================

int ParallelTopology::getMaxGlobalFace() const
{
  int max = 0;
  TGlob2LocsMap::const_iterator g_l_l = m_face_glob_to_loc.begin();
  for ( ; g_l_l != m_face_glob_to_loc.end(); ++g_l_l )
    if ( g_l_l->first > max )
      max = g_l_l->first;
  return max;
}

void ParallelTopology::recreateFaceMapping(const TGeom2FacesByDomian& face_map)
{
  m_face_glob_to_loc.clear();
  for (int i=0; i<m_nb_domain;i++)
    m_face_loc_to_glob[i].clear();

  for (int idomain=0; idomain<m_nb_domain; idomain++)
  {
    int ilocal=1;
    TGeom2Faces::const_iterator iter = face_map[idomain].begin();
    for (; iter != face_map[idomain].end(); iter++)
    {
      for (unsigned i=0; i< (iter->second).size(); i++)
      {
        MEDSPLITTER_FaceModel* face = (iter->second)[i];
        //cout << "global :"<<face->getGlobal()<<" - "<<ilocal<<endl;
        m_face_glob_to_loc.insert(make_pair(face->getGlobal(),make_pair(idomain,ilocal)));
        m_face_loc_to_glob[idomain].push_back(face->getGlobal());
        ilocal++;
      }
    }
    m_nb_faces[idomain] =ilocal-1;
  }
}

//================================================================================
/*!
 * \brief Recreating cell and node mapping after send-reveive and fusion of domain meshes
 */
//================================================================================

void ParallelTopology::recreateMappingAfterFusion(const vector<MEDMEM::MESH*>& meshes)
{
  const char* LOC = "ParallelTopology::recreateMappingAfterFusion(): ";

  m_glob_to_loc.clear();
  m_node_glob_to_loc.clear();
  m_face_glob_to_loc.clear();

  for (int idomain=0; idomain<m_nb_domain; idomain++)
  {
    m_nb_cells[idomain] = m_nb_nodes[idomain] = m_nb_faces[idomain] = 0;
    m_loc_to_glob[idomain].clear();
    m_node_loc_to_glob[idomain].clear();
    m_face_loc_to_glob[idomain].clear();
    
    if ( !meshes[idomain]->getCoordinateptr() ) continue; // empty domian

    //creating cell maps

    m_nb_cells[idomain]=meshes[idomain]->getNumberOfElements(MED_EN::MED_CELL,
                                                             MED_EN::MED_ALL_ELEMENTS);
    if ( m_cell_loc_to_glob_fuse[idomain].size() != m_nb_cells[idomain] )
      throw MED_EXCEPTION(MEDMEM::STRING(LOC)<<" invalid nb of fused cells");

    m_loc_to_glob[idomain].swap( m_cell_loc_to_glob_fuse[idomain] );

    for (int i=0; i<m_nb_cells[idomain]; i++)
    {
      int global=m_loc_to_glob[idomain][i];
      m_glob_to_loc[global]=make_pair(idomain,i+1);
    }

    //creating node maps

    m_nb_nodes[idomain]=meshes[idomain]->getNumberOfNodes();
    m_node_loc_to_glob[idomain] = ((MEDMEM::MeshFuse*)meshes[idomain])->getNodeNumbers();
    if ( m_node_loc_to_glob[idomain].size() != m_nb_nodes[idomain] )
      throw MED_EXCEPTION(MEDMEM::STRING(LOC)<<" invalid nb of fused nodes");

    // setting mappings for all nodes
    for (int inode=0; inode<m_nb_nodes[idomain]; inode++)
    {
      int global_number=m_node_loc_to_glob[idomain][inode];
      m_node_glob_to_loc.insert(make_pair(global_number,make_pair(idomain,inode+1)));
    }


    //creating dimension d-1 component mappings

    MED_EN::medEntityMesh constituent_entity =
      (m_mesh_dimension == 3 ? MED_EN::MED_FACE : MED_EN::MED_EDGE );
    m_nb_faces[idomain] = meshes[idomain]->getNumberOfElements(constituent_entity,
                                                               MED_EN::MED_ALL_ELEMENTS);
    if ( m_face_loc_to_glob_fuse[idomain].size() != m_nb_faces[idomain] )
      throw MED_EXCEPTION(MEDMEM::STRING(LOC)<<" invalid nb of fused faces of domain "<< idomain
                          << ": expect " << m_nb_faces[idomain]
                          << " but have " << m_face_loc_to_glob_fuse[idomain].size());

    m_face_loc_to_glob[idomain].swap( m_face_loc_to_glob_fuse[idomain] );

    for (int iface=0; iface<m_nb_faces[idomain]; iface++)
    {
      int global_number=m_face_loc_to_glob[idomain][iface];
      m_face_glob_to_loc.insert(make_pair(global_number,make_pair(idomain,iface+1)));
    }
  }

}
