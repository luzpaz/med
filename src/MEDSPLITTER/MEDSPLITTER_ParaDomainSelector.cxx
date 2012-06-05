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
// File      : MEDSPLITTER_ParaDomainSelector.cxx
// Created   : Wed Jun 24 12:39:59 2009
// Author    : Edward AGAPOV (eap)

#include "MEDSPLITTER_ParaDomainSelector.hxx"

#include "MEDSPLITTER_UserGraph.hxx"
#include "MEDSPLITTER_JointExchangeData.hxx"

#include <MEDMEM_Meshing.hxx>
#include <MEDMEM_DriversDef.hxx>

#include <iostream>
#include <numeric>

#ifdef HAVE_MPI2
#include <mpi.h>
#endif

#ifndef WIN32
#include <sys/sysinfo.h>
#endif

using namespace MEDSPLITTER;
using namespace MED_EN;
using namespace std;

//================================================================================
/*!
 * \brief Constructor. Find out my rank and world size
 */
//================================================================================

ParaDomainSelector::ParaDomainSelector(bool mesure_memory)
  :_rank(0),_world_size(1), _nb_result_domains(-1), _mesure_memory(mesure_memory),
   _init_time(0.0), _init_memory(0), _max_memory(0)
{
#ifdef HAVE_MPI2
  MPI_Comm_size(MPI_COMM_WORLD,&_world_size) ;
  MPI_Comm_rank(MPI_COMM_WORLD,&_rank) ;
  _init_time = MPI_Wtime();
#endif
  evaluateMemory();
}

//================================================================================
/*!
 * \brief Destructor
 */
//================================================================================

ParaDomainSelector::~ParaDomainSelector()
{
}

//================================================================================
/*!
 * \brief Return true if is running on different hosts
 */
//================================================================================

bool ParaDomainSelector::isOnDifferentHosts() const
{
  evaluateMemory();
  if ( _world_size < 2 )
    return false;

#ifdef HAVE_MPI2
  char name_here[ MPI_MAX_PROCESSOR_NAME+1 ], name_there[ MPI_MAX_PROCESSOR_NAME+1 ];
  int size;
  MPI_Get_processor_name( name_here, &size);

  int next_proc = (rank() + 1) % nbProcs();
  int prev_proc = (rank() - 1 + nbProcs()) % nbProcs();
  int tag  = 1111111;

  MPI_Status status;
  MPI_Sendrecv((void*)&name_here[0],  MPI_MAX_PROCESSOR_NAME, MPI_CHAR, next_proc, tag,
               (void*)&name_there[0], MPI_MAX_PROCESSOR_NAME, MPI_CHAR, prev_proc, tag,
               MPI_COMM_WORLD, &status);
  return string(name_here) != string(name_there);
#else
  return false;
#endif
}

//================================================================================
/*!
 * \brief Return true if the domain with domainIndex is to be loaded on this proc
 *  \param domainIndex - index of mesh domain
 *  \retval bool - to load or not
 */
//================================================================================

bool ParaDomainSelector::isMyDomain(int domainIndex) const
{
  evaluateMemory();
  return (_rank == getProccessorID( domainIndex ));
}

//================================================================================
/*!
 * \brief Return processor id where the domain with domainIndex resides
 *  \param domainIndex - index of mesh domain
 *  \retval int - processor id
 */
//================================================================================

int ParaDomainSelector::getProccessorID(int domainIndex) const
{
  evaluateMemory();
  return ( domainIndex % _world_size );
}

//================================================================================
/*!
 * \brief Gather info on nb of entities on each processor and return total nb.
 *
 * Is called
 * 1) for MED_CELL to know global id shift for domains at graph construction;
 * 2) for sub-entity to know total nb of sub-entities before creating those of joints
 */
//================================================================================

int ParaDomainSelector::gatherNbOf(MED_EN::medEntityMesh        entity,
                                   const vector<MEDMEM::MESH*>& domain_meshes)
{
  evaluateMemory();

  // get nb of elems of each domain mesh
  int nb_domains = domain_meshes.size();
  vector<int> nb_elems( nb_domains, 0 );
  for ( int i = 0; i < nb_domains; ++i )
    if ( domain_meshes[i] )
      nb_elems[i] = domain_meshes[i]->getNumberOfElements(entity, MED_ALL_ELEMENTS);

  // receive nb of elems from other procs
  vector<int> all_nb_elems( nb_domains );
#ifdef HAVE_MPI2
  MPI_Allreduce((void*)&nb_elems[0], (void*)&all_nb_elems[0], nb_domains,
                MPI_INT, MPI_SUM, MPI_COMM_WORLD);
#endif
  int total_nb = std::accumulate( all_nb_elems.begin(), all_nb_elems.end(), 0 );

  vector<int>& elem_shift_by_domain
    = (entity == MED_CELL) ? _cell_shift_by_domain : _face_shift_by_domain;

  // fill elem_shift_by_domain

  vector< int > ordered_nbs, domain_order( nb_domains );
  ordered_nbs.push_back(0);
  for ( int iproc = 0; iproc < nbProcs(); ++iproc )
    for ( int idomain = 0; idomain < nb_domains; ++idomain )
      if ( getProccessorID( idomain ) == iproc )
      {
        domain_order[ idomain ] = ordered_nbs.size() - 1;
        ordered_nbs.push_back( ordered_nbs.back() + all_nb_elems[idomain] );
      }
  elem_shift_by_domain.resize( nb_domains+1, 0 );
  for ( int idomain = 0; idomain < nb_domains; ++idomain )
    elem_shift_by_domain[ idomain ] = ordered_nbs[ domain_order[ idomain ]];

  elem_shift_by_domain.back() = ordered_nbs.back(); // to know total nb of elements

  if ( entity == MED_CELL )
  {
    // fill _nb_vert_of_procs
    _nb_vert_of_procs.resize( _world_size+1, 0 );
    for ( int i = 0; i < nb_domains; ++i )
    {
      int rank = getProccessorID( i );
      _nb_vert_of_procs[ rank+1 ] += all_nb_elems[ i ];
    }
    _nb_vert_of_procs[0] = 1; // base = 1
    for ( int i = 1; i < _nb_vert_of_procs.size(); ++i )
      _nb_vert_of_procs[ i ] += _nb_vert_of_procs[ i-1 ]; // to CSR format
  }
  else
  {
    // to compute global ids of faces in joints
    //_total_nb_faces = total_nb;
  }

//   if ( !_rank) {
//     MEDMEM::STRING out("_nb_vert_of_procs :");
//     for ( int i = 0; i < _nb_vert_of_procs.size(); ++i )
//       out << _nb_vert_of_procs[i] << " ";
//     std::cout << out << std::endl;
//   }
//   if ( !_rank) {
//     MEDMEM::STRING out("elem_shift_by_domain :");
//     for ( int i = 0; i < elem_shift_by_domain.size(); ++i )
//       out << elem_shift_by_domain[i] << " ";
//     std::cout << out << std::endl;
//   }

  evaluateMemory();

  return total_nb;
}

//================================================================================
/*!
 * \brief Return distribution of the graph vertices among the processors
 *  \retval int* - array conatining nb of vertices on all processors
 *
 * gatherNbOf( MED_CELL ) must be called before.
 * The result array is to be used as the first arg of ParMETIS_V3_PartKway() and
 * is freed by ParaDomainSelector.
 */
//================================================================================

#define gatherNbOf_NOT_CALLED(meth) throw MED_EXCEPTION \
("ParaDomainSelector::" #meth "(): gatherNbOf( MED_CELL ) must be called before")

int* ParaDomainSelector::getNbVertOfProcs() const
{
  evaluateMemory();
  if ( _nb_vert_of_procs.empty() )
    gatherNbOf_NOT_CALLED(getNbVertOfProcs);

  return (int*) & _nb_vert_of_procs[0];
}
//================================================================================
/*!
 * \brief Return nb of cells in domains with lower index.
 *
 * gatherNbOf( MED_CELL ) must be called before.
 * Result added to local id on given domain gives id in the whole distributed mesh
 */
//================================================================================

int ParaDomainSelector::getDomainShift(int domainIndex) const
{
  evaluateMemory();
  if ( _cell_shift_by_domain.empty() )
    gatherNbOf_NOT_CALLED(getDomainShift);

  return _cell_shift_by_domain[ domainIndex ];
}

//================================================================================
/*!
 * \brief Return nb of cells on processors with lower rank.
 *
 * gatherNbOf( MED_CELL ) must be called before.
 * Result added to global id on this processor gives id in the whole distributed mesh
 */
//================================================================================

int ParaDomainSelector::getProcShift() const
{
  evaluateMemory();
  if ( _nb_vert_of_procs.empty() )
    gatherNbOf_NOT_CALLED(getProcShift);

  return _nb_vert_of_procs[_rank]-1;
}

//================================================================================
/*!
 * \brief Gather graphs from all processors into one
 */
//================================================================================

auto_ptr<Graph> ParaDomainSelector::gatherGraph(const Graph* graph) const
{
  Graph* glob_graph = 0;

  evaluateMemory();
#ifdef HAVE_MPI2

  // ---------------
  // Gather indices
  // ---------------

  vector<int> index_size_of_proc( nbProcs() ); // index sizes - 1
  for ( int i = 1; i < _nb_vert_of_procs.size(); ++i )
    index_size_of_proc[i-1] = _nb_vert_of_procs[ i ] - _nb_vert_of_procs[ i-1 ];

  int index_size = 1 + _cell_shift_by_domain.back();
  int* graph_index = new int[ index_size ];
  const int* index = graph->getGraph()->getIndex();
  int* proc_index_displacement = (int*) & _nb_vert_of_procs[0];

  MPI_Allgatherv((void*) (index+1),         // send local index except first 0 (or 1)
                 index_size_of_proc[_rank], // index size on this proc
                 MPI_INT,
                 (void*) graph_index,       // receive indices
                 & index_size_of_proc[0],   // index size on each proc
                 proc_index_displacement,   // displacement of each proc index
                 MPI_INT,
                 MPI_COMM_WORLD);
  graph_index[0] = index[0]; // it is not overwritten thanks to proc_index_displacement[0]==1

  // get sizes of graph values on each proc by the got indices of graphs
  vector< int > value_size_of_proc( nbProcs() ), proc_value_displacement(1,0);
  for ( int i = 0; i < nbProcs(); ++i )
  {
    if ( index_size_of_proc[i] > 0 )
      value_size_of_proc[i] = graph_index[ proc_index_displacement[ i+1 ]-1 ] - graph_index[0];
    else
      value_size_of_proc[i] = 0;
    proc_value_displacement.push_back( proc_value_displacement.back() + value_size_of_proc[i] );
  }
  
  // update graph_index
  for ( int i = 1; i < nbProcs(); ++i )
  {
    int shift = graph_index[ proc_index_displacement[i]-1 ]-graph_index[0];
    for ( int j = proc_index_displacement[i]; j < proc_index_displacement[i+1]; ++j )
      graph_index[ j ] += shift;
  }
  
  // --------------
  // Gather values
  // --------------

  int value_size = graph_index[ index_size-1 ] - graph_index[ 0 ];
  int* graph_value = new int[ value_size ];
  const int* value = graph->getGraph()->getValue();

  MPI_Allgatherv((void*) value,                // send local value
                 value_size_of_proc[_rank],    // value size on this proc
                 MPI_INT,
                 (void*) graph_value,          // receive values
                 & value_size_of_proc[0],      // value size on each proc
                 & proc_value_displacement[0], // displacement of each proc value
                 MPI_INT,
                 MPI_COMM_WORLD);

  // -----------------
  // Gather partition
  // -----------------

  int * partition = new int[ _cell_shift_by_domain.back() ];
  const int* part = graph->getPart();
  
  MPI_Allgatherv((void*) part,              // send local partition
                 index_size_of_proc[_rank], // index size on this proc
                 MPI_INT,
                 (void*)(partition-1),      // -1 compensates proc_index_displacement[0]==1
                 & index_size_of_proc[0],   // index size on each proc
                 proc_index_displacement,   // displacement of each proc index
                 MPI_INT,
                 MPI_COMM_WORLD);

  // -----------
  // Make graph
  // -----------

  MEDMEM::MEDSKYLINEARRAY* array =
    new MEDMEM::MEDSKYLINEARRAY( index_size-1, value_size, graph_index, graph_value, true );

  glob_graph = new UserGraph( array, partition, index_size-1 );

  evaluateMemory();

  delete [] partition;

#endif // HAVE_MPI2

  return auto_ptr<Graph>( glob_graph );
}

//================================================================================
/*!
 * \brief Sets global numbering for the entity.
 *
 * This method must be once called for MED_CELL before exchangeJoint() calls
 */
//================================================================================

void ParaDomainSelector::gatherEntityTypesInfo(vector<MEDMEM::MESH*>& domain_meshes,
                                               MED_EN::medEntityMesh  entity)
{
  const list<medGeometryElement> & all_types = meshEntities[ entity ];

  evaluateMemory();

  // Put nb of elements of the entity of all domains in one vector
  // and by the way find out mesh dimension

  vector<int> nb_of_type( domain_meshes.size() * all_types.size(), 0 );
  int mesh_dim = -1, space_dim = -1;

  for ( int idomain = 0; idomain < domain_meshes.size(); ++idomain )
  {
    if ( !isMyDomain(idomain)) continue;

    int* domain_nbs = & nb_of_type[ idomain * all_types.size()];

    list<medGeometryElement>::const_iterator type = all_types.begin();
    for ( int t = 0; type != all_types.end(); ++t,++type )
      domain_nbs[t] = domain_meshes[idomain]->getNumberOfElements(entity,*type);

    int i_mesh_dim = domain_meshes[idomain]->getMeshDimension();
    int i_space_dim = domain_meshes[idomain]->getSpaceDimension();
    if ( mesh_dim < i_mesh_dim && i_mesh_dim <= 3 )
      mesh_dim = i_mesh_dim;
    if ( space_dim < i_space_dim && i_space_dim <= 3 )
      space_dim = i_space_dim;
  }

  // Receive nbs from other procs

  vector< int > nb_recv( nb_of_type.size() );
#ifdef HAVE_MPI2
  MPI_Allreduce((void*)&nb_of_type[0], (void*)&nb_recv[0], nb_of_type.size(),
                MPI_INT, MPI_SUM, MPI_COMM_WORLD);
#endif

  // Set info to meshes of distant domains

  for ( int idomain = 0; idomain < domain_meshes.size(); ++idomain )
  {
    if ( isMyDomain(idomain)) continue;

    MEDMEM::MESHING* meshing = (MEDMEM::MESHING*) domain_meshes[idomain];
    if ( meshing->getMeshDimension() < mesh_dim )
    {
      //meshing->setSpaceDimension( space_dim );
      meshing->setCoordinates( space_dim, /*NumberOfNodes=*/0, 0, "", 0);
    }

    vector< medGeometryElement > types;
    vector< int >                nb_elems;

    int* domain_nbs = & nb_recv[ idomain * all_types.size()];

    list<medGeometryElement>::const_iterator type = all_types.begin();
    for ( int t = 0; type != all_types.end(); ++t,++type )
    {
      if ( domain_nbs[t]==0 ) continue;
      types.push_back( *type );
      nb_elems.push_back( domain_nbs[t] );
    }
    meshing->setNumberOfTypes( types.size(), entity );
    if ( !types.empty() )
    {
      meshing->setTypes           ( & types[0], entity );
      meshing->setNumberOfElements( & nb_elems[0], entity );
    }
  }
  evaluateMemory();
}

//================================================================================
/*!
 * \brief Set nb of cell/cell pairs in a joint between domains
 */
//================================================================================

void ParaDomainSelector::setNbCellPairs( int nb_cell_pairs, int dist_domain, int loc_domain )
{
  // This method is needed for further computing global numbers of faces in joint.
  // Store if both domains are on this proc else on one of procs only
  if ( isMyDomain( dist_domain ) || dist_domain < loc_domain )
  {
    if ( _nb_cell_pairs_by_joint.empty() )
      _nb_cell_pairs_by_joint.resize( _nb_result_domains*(_nb_result_domains+1), 0);

    int joint_id = jointId( loc_domain, dist_domain );
    _nb_cell_pairs_by_joint[ joint_id ] = nb_cell_pairs;
  }
  evaluateMemory();
}

//================================================================================
/*!
 * \brief Return nb of cell/cell pairs in a joint between domains on different procs
 */
//================================================================================

int ParaDomainSelector::getNbCellPairs( int dist_domain, int loc_domain ) const
{
  evaluateMemory();

  int joint_id = jointId( loc_domain, dist_domain );
  return _nb_cell_pairs_by_joint[ joint_id ];
}

//================================================================================
/*!
 * \brief Gather size of each joint
 */
//================================================================================

void ParaDomainSelector::gatherNbCellPairs()
{
  const char* LOC = "MEDSPLITTER::ParaDomainSelector::gatherNbCellPairs(): ";
  if ( _nb_cell_pairs_by_joint.empty() )
    _nb_cell_pairs_by_joint.resize( _nb_result_domains*(_nb_result_domains+1), 0);
  evaluateMemory();

  vector< int > send_buf = _nb_cell_pairs_by_joint;
#ifdef HAVE_MPI2
  MPI_Allreduce((void*)&send_buf[0],
                (void*)&_nb_cell_pairs_by_joint[0],
                _nb_cell_pairs_by_joint.size(),
                MPI_INT, MPI_SUM, MPI_COMM_WORLD);
#endif
  // check that the set nbs of cell pairs are correct,
  // namely that each joint is treated on one proc only
  for ( int j = 0; j < _nb_cell_pairs_by_joint.size(); ++j )
    if ( _nb_cell_pairs_by_joint[j] != send_buf[j] && send_buf[j]>0 )
      throw MED_EXCEPTION(MEDMEM::STRING(LOC) << "invalid nb of cell pairs");
}

//================================================================================
/*!
 * \brief Send-receive joint data
 */
//================================================================================

void ParaDomainSelector::exchangeJoint( JointExchangeData* joint ) const
{
#ifdef HAVE_MPI2
  vector<int> send_data, recv_data( joint->serialize( send_data ));

  int dest = getProccessorID( joint->distantDomain() );
  int tag  = 1001 + jointId( joint->localDomain(), joint->distantDomain() );
  
  MPI_Status status;
  MPI_Sendrecv((void*)&send_data[0], send_data.size(), MPI_INT, dest, tag,
               (void*)&recv_data[0], recv_data.size(), MPI_INT, dest, tag,
               MPI_COMM_WORLD, &status);  

  joint->deserialize( recv_data );
#endif
}

//================================================================================
/*!
 * \brief Return the first global id of sub-entity for the joint
 */
//================================================================================

int ParaDomainSelector::getFisrtGlobalIdOfSubentity( int loc_domain, int dist_domain ) const
{
  // total_nb_faces includes faces existing before creation of joint faces
  // (got in gatherNbOf( MED_FACE )).
  evaluateMemory();

  int total_nb_faces = _face_shift_by_domain.empty() ? 0 : _face_shift_by_domain.back();
  int id = total_nb_faces + 1;

  if ( _nb_cell_pairs_by_joint.empty() )
    throw MED_EXCEPTION("MEDSPLITTER::ParaDomainSelector::getFisrtGlobalIdOfSubentity(), "
                        "gatherNbCellPairs() must be called before");
  int joint_id = jointId( loc_domain, dist_domain );
  for ( int j = 0; j < joint_id; ++j )
    id += _nb_cell_pairs_by_joint[ j ];

  return id;
}

//================================================================================
/*!
 * \brief Send-receive local ids of joint faces
 */
//================================================================================

int* ParaDomainSelector::exchangeSubentityIds( int loc_domain, int dist_domain,
                                               const vector<int>& loc_ids_here ) const
{
  int* loc_ids_dist = new int[ loc_ids_here.size()];
#ifdef HAVE_MPI2
  int dest = getProccessorID( dist_domain );
  int tag  = 2002 + jointId( loc_domain, dist_domain );
  MPI_Status status;
  MPI_Sendrecv((void*)&loc_ids_here[0], loc_ids_here.size(), MPI_INT, dest, tag,
               (void*) loc_ids_dist,    loc_ids_here.size(), MPI_INT, dest, tag,
               MPI_COMM_WORLD, &status);  
  evaluateMemory();
#endif

  return loc_ids_dist;
}

//================================================================================
/*!
 * \brief Return identifier for a joint
 */
//================================================================================

int ParaDomainSelector::jointId( int local_domain, int distant_domain ) const
{
  evaluateMemory();
  if (_nb_result_domains < 0)
    throw MED_EXCEPTION("ParaDomainSelector::jointId(): setNbDomains() must be called before()");

  if ( local_domain < distant_domain )
    swap( local_domain, distant_domain );
  return local_domain * _nb_result_domains + distant_domain;
}

//================================================================================
/*!
 * \brief Return domain order so that first go domains on proc 0 and so n
 */
//================================================================================

// int ParaDomainSelector::getDomianOrder(int idomain, int nb_domains) const
// {
//   return nb_domains / nbProcs() * getProccessorID( idomain ) + idomain / nbProcs();
// }

//================================================================================
/*!
 * \brief Return time passed from construction in seconds
 */
//================================================================================

double ParaDomainSelector::getPassedTime() const
{
#ifdef HAVE_MPI2
  return MPI_Wtime() - _init_time;
#else
  return 0.0;
#endif
}

//================================================================================
/*!
 * \brief Evaluate current memory usage and return the maximal one in KB
 */
//================================================================================

int ParaDomainSelector::evaluateMemory() const
{
  if ( _mesure_memory )
  {
    int used_memory = 0;
#ifndef WIN32
    struct sysinfo si;
    int err = sysinfo( &si );
    if ( !err )
      used_memory =
        (( si.totalram - si.freeram + si.totalswap - si.freeswap ) * si.mem_unit ) / 1024;
#endif
    if ( used_memory > _max_memory )
      ((ParaDomainSelector*) this)->_max_memory = used_memory;

    if ( !_init_memory )
      ((ParaDomainSelector*) this)->_init_memory = used_memory;
  }
  return _max_memory - _init_memory;
}
