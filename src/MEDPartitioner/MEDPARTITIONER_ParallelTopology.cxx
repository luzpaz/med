//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include <set>
#include <map>
#include <vector>

#include "InterpKernelHashMap.hxx"
#include "MEDPARTITIONER_MESHCollection.hxx"
#include "MEDPARTITIONER_Topology.hxx"
#include "MEDPARTITIONER_Graph.hxx"
#include "MEDPARTITIONER_ParallelTopology.hxx"
#include "MEDPARTITIONER_ConnectZone.hxx"

#include "MEDCouplingUMesh.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Utilities.hxx"

#ifndef WNT
using namespace __gnu_cxx;
#else
using namespace std;
#endif

using namespace MEDPARTITIONER;

//empty constructor
ParallelTopology::ParallelTopology():m_nb_domain(0),m_mesh_dimension(0)
{}

//!constructing topology according to mesh collection
ParallelTopology::ParallelTopology(const vector<ParaMEDMEM::MEDCouplingUMesh*>& meshes, 
                                   const vector<MEDPARTITIONER::CONNECTZONE*>& cz,
                                   vector<int*>& cellglobal,
                                   vector<int*>& nodeglobal,
                                   vector<int*>& faceglobal):m_nb_domain(meshes.size())/*,m_mesh_dimension(meshes[0]->getMeshDimension())*/
{

  int index_global=0;
  int index_node_global=0;
  int index_face_global=0;

  m_nb_cells.resize(m_nb_domain);
  m_nb_nodes.resize(m_nb_domain);
  //  m_nb_faces.resize(m_nb_domain);

  m_loc_to_glob.resize(m_nb_domain);
  m_node_loc_to_glob.resize(m_nb_domain);
  //  m_face_loc_to_glob.resize(m_nb_domain);

  //MED_EN::medEntityMesh constituent_entity;

  bool parallel_mode = false;
  for (int idomain=0; !parallel_mode && idomain<m_nb_domain; idomain++)
    parallel_mode = (!meshes[idomain]);

  for (int idomain=0; idomain<m_nb_domain; idomain++)
  {
    if ( !meshes[idomain] )
      continue;
    m_mesh_dimension = meshes[idomain]->getMeshDimension();
    //constituent_entity = (m_mesh_dimension == 3 ? MED_EN::MED_FACE : MED_EN::MED_EDGE );

    //creating cell maps
    m_nb_cells[idomain]=meshes[idomain]->getNumberOfCells();
    //    cout << "Nb cells (domain "<<idomain<<") = "<<m_nb_cells[idomain];
    m_loc_to_glob[idomain].resize(m_nb_cells[idomain]);

    if (cellglobal[idomain]==0 || parallel_mode)
    {
      MESSAGE_MED("Creating global numbering"); 
      //creating global numbering from scratch
      for (int i=0; i<m_nb_cells[idomain]; i++)
      {
       
        m_glob_to_loc.insert(make_pair(index_global,make_pair(idomain,i+1)));
        m_loc_to_glob[idomain][i]=index_global;
        //        cout<<"glob:"<<index_global<<" --> ("<<idomain<<","<<i+1<<")"<<endl;
        index_global++;
      }
    }
    //using global numbering coming from a previous numbering
    else
    {
      MESSAGE_MED("Using former global numbering");
      for (int i=0; i<m_nb_cells[idomain]; i++)
      {
        int global=cellglobal[idomain][i];
        m_glob_to_loc.insert(make_pair(global,make_pair(idomain,i)));
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
        m_node_glob_to_loc.insert(make_pair(i,make_pair(0,i)));
        m_node_loc_to_glob[0][i]=i;
      }
      m_nb_total_nodes=meshes[idomain]->getNumberOfNodes();   
      m_nb_nodes[0]=m_nb_total_nodes; 

      MESSAGE_MED ("nb total cells "<< m_nb_total_cells);
      MESSAGE_MED("nb total nodes "<< m_nb_total_nodes);  
      return;
    }

    //creating node maps
    m_nb_nodes[idomain]=meshes[idomain]->getNumberOfNodes();
    INTERP_KERNEL::HashMap <int,pair<int,int> > local2distant;
    m_node_loc_to_glob[idomain].resize(m_nb_nodes[idomain]);
    for (int icz=0; icz<cz.size(); icz++)
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
        if (local2distant.find(inode)==local2distant.end())
        {
          index_node_global++;
          m_node_glob_to_loc.insert(make_pair(index_node_global,make_pair(idomain,inode)));
          //m_node_loc_to_glob[make_pair(idomain,inode+1)]=index_node_global;
          m_node_loc_to_glob[idomain][inode]=index_node_global;
        }   
        else
        {
          int ip = (local2distant.find(inode)->second).first;
          int distant = (local2distant.find(inode)->second).second;
          int global_number=m_loc_to_glob[ip][distant];
          m_node_glob_to_loc.insert(make_pair(global_number,make_pair(idomain,inode)));
          m_node_loc_to_glob[idomain][inode]=global_number;
        } 
      }
    }
    //using former node numbering
    else
    {
      for (int inode=0; inode<m_nb_nodes[idomain]; inode++)
      {
        int global_number=nodeglobal[idomain][inode];
        m_node_glob_to_loc.insert(make_pair(global_number,make_pair(idomain,inode)));
        m_node_loc_to_glob[idomain][inode]=global_number;
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
ParallelTopology::ParallelTopology(Graph* graph, int nb_domain, int mesh_dimension):
  m_nb_domain(nb_domain),
  m_nb_cells(graph->nbVertices()),
  m_mesh_dimension(mesh_dimension)
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
    m_loc_to_glob[idomain].push_back(icell);
    m_glob_to_loc.insert(make_pair(icell,make_pair(idomain,m_nb_cells[idomain])));

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

