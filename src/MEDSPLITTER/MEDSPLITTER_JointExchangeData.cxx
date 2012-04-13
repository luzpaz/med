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
// File      : MEDSPLITTER_JointExchangeData.cxx
// Created   : Thu Jul 23 13:01:35 2009
// Author    : Edward AGAPOV (eap)
//

#include "MEDSPLITTER_JointExchangeData.hxx"

#include "MEDSPLITTER_FaceModel.hxx"

#include <MEDMEM_Meshing.hxx>
#include <MEDMEM_ConnectZone.hxx>

using namespace MEDSPLITTER;
using namespace MED_EN;

//================================================================================
/*!
 * \brief Initialization
 */
//================================================================================

JointExchangeData::JointExchangeData():
  _dist_domain(-1), _loc_domain(-1), _conn_here_size(0), _dist_mesh(0), _loc_mesh(0)
{
}

//================================================================================
/*!
 * \brief Set meshes
 */
//================================================================================

void JointExchangeData::setMeshes( int domain_dist, const MEDMEM::MESH* mesh_dist,
                                   int domain_here, const MEDMEM::MESH* mesh_here)
{
  _dist_domain = domain_dist;
  _loc_domain  = domain_here;
  _dist_mesh   = mesh_dist;
  _loc_mesh    = mesh_here;
}

//================================================================================
/*!
 * \brief Stores cell/cell pair of joint between domains (residing different procs by default)
 *  \param mesh - mesh on this proc
 *  \param domain_dist - distant domain
 *  \param domain_here - local domain
 *  \param glob_dist - global id of a distant domain
 *  \param glob_here - global id of a local domain
 *  \param loc_here - local id of a local domain (counted from 1)
 *  \param loc_dist - local id of a another domain (counted from 1)
 */
//================================================================================

void JointExchangeData::addCellCorrespondence(const MEDMEM::MESH* mesh,
                                              int domain_dist, int domain_here,
                                              int glob_dist,   int glob_here,
                                              int loc_here,    int loc_dist)
{
  int glob_key = ( domain_dist < domain_here ) ? glob_dist : glob_here;
  _glob_to_locs_here_and_dist.insert ( make_pair( glob_key, make_pair( loc_here, loc_dist )));

  _conn_here_size += mesh->getElementType( MED_CELL, loc_here ) % 100;
}

//================================================================================
/*!
 * \brief Return data to send and size of data to receive
 */
//================================================================================

int JointExchangeData::serialize(vector<int> & out_data) const
{
  int data_size =
    _glob_to_locs_here_and_dist.size() + _local_conn_here.size() + _global_conn_here.size();
  out_data.resize( data_size );

  int i = 0;
  TGlob2LocsHD::const_iterator glob_hloc_dloc = _glob_to_locs_here_and_dist.begin();
  for ( ; glob_hloc_dloc != _glob_to_locs_here_and_dist.end(); ++ glob_hloc_dloc )
    out_data[i++] = glob_hloc_dloc->second.first;

  for ( int n = 0; n < _local_conn_here.size(); ++n )
    out_data[i++] = _local_conn_here[n];
  
  for ( int n = 0; n < _global_conn_here.size(); ++n )
    out_data[i++] = _global_conn_here[n];

  // evaluate size of data to receive

  medGeometryElement last_type =
    _dist_mesh->getTypes(MED_CELL)[ _dist_mesh->getNumberOfTypes(MED_CELL)-1 ];

  int max_nb_node_by_cell = last_type % 100;
  
  data_size =
    _glob_to_locs_here_and_dist.size() +
    2 * max_nb_node_by_cell * _dist_mesh->getNumberOfElements( MED_CELL, MED_ALL_ELEMENTS );

  return data_size;
}

//================================================================================
/*!
 * \brief Stores received data
 */
//================================================================================

void JointExchangeData::deserialize(const std::vector<int> & in_data)
{
  const int* dist_loc = &in_data[0];

  int conn_real_size = 0;
  TGlob2LocsHD::iterator glob_hloc_dloc = _glob_to_locs_here_and_dist.begin();
  for ( ; glob_hloc_dloc != _glob_to_locs_here_and_dist.end(); ++ glob_hloc_dloc )
  {
    conn_real_size += _dist_mesh->getElementType( MED_CELL, *dist_loc ) % 100;
    glob_hloc_dloc->second.second = *dist_loc++;
  }
  
  const int* dist_loc_conn = dist_loc;
  const int* dist_glob_conn = dist_loc + conn_real_size;

  _global_conn_dist.resize( conn_real_size );
  _local_conn_dist.resize ( conn_real_size );

  for ( int n = 0; n < conn_real_size; ++n )
  {
    _global_conn_dist[n] = *dist_glob_conn++;
    _local_conn_dist [n] = *dist_loc_conn++;
  }
}

//================================================================================
/*!
 * \brief Create cell/cell correspondency array of the joint
 */
//================================================================================

MEDMEM::MEDSKYLINEARRAY* JointExchangeData::makeCellCorrespArray() const
{
  int nb_cells_here    = _loc_mesh->getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS);
  int cells_value_size = _glob_to_locs_here_and_dist.size();
  int* cell_index = new int [nb_cells_here+1];
  int* cell_value = new int [cells_value_size];

  TGlob2LocsHD::const_iterator glob_hloc_dloc = _glob_to_locs_here_and_dist.begin();

  // sort local/local pairs by locals of _loc_domain
  multiset< pair< int, int > > here_and_dist_cells;
  for ( ; glob_hloc_dloc != _glob_to_locs_here_and_dist.end(); ++ glob_hloc_dloc )
    here_and_dist_cells.insert( glob_hloc_dloc->second );

  multiset< pair< int, int > >::const_iterator here_dist_ids = here_and_dist_cells.begin();
  if ( here_dist_ids->second < 1 )
    throw MED_EXCEPTION("MEDSPLITTER::JointExchangeData::makeConnectZone(): "
                        "ParaDomainSelector::exchangeJoint() must be called before!");
  cell_index[0]=1;
  for ( int loc_id_here = 1, i_dist = 0; loc_id_here <= nb_cells_here; ++loc_id_here)
  {
    cell_index[ loc_id_here ] = cell_index[ loc_id_here-1 ];
    while ( here_dist_ids != here_and_dist_cells.end() &&
            here_dist_ids->first == loc_id_here )
    {
      cell_value[ i_dist ] = here_dist_ids->second;
      cell_index[ loc_id_here ]++;
      here_dist_ids++;
      i_dist++;
    }
  }

  MEDMEM::MEDSKYLINEARRAY* cell_cell=
    new MEDMEM::MEDSKYLINEARRAY( nb_cells_here, cells_value_size,
                                 cell_index, cell_value,/*shallowcopy=*/true);
  return cell_cell;
}

namespace
{
  //================================================================================
  /*!
   * \brief Return CELLMODEL for the geom type
   */
  //================================================================================

  const MEDMEM::CELLMODEL& getCellModel( medGeometryElement geom_type )
  {
    static map< medGeometryElement, MEDMEM::CELLMODEL > cell_models;
    MEDMEM::CELLMODEL& model = cell_models[ geom_type ];
    if ( model.getNumberOfNodes() < 1 )
      model = MEDMEM::CELLMODEL( geom_type );
    return model;
  }
}

//================================================================================
/*!
 * \brief Creates CONNECTZONE with MED_CELL and MED_NODE data filled.
 *
 * It must be exchanged with the corresponding joint on other proc using
 * ParaDomainSelector::exchangeJoint()
 */
//================================================================================

MEDMEM::CONNECTZONE* JointExchangeData::makeConnectZone(TGeom2FacesByDomian& face_map)
{
  MEDMEM::CONNECTZONE* cz= new MEDMEM::CONNECTZONE();
  cz->setLocalMesh          (const_cast<MEDMEM::MESH*>(_loc_mesh));
  cz->setDistantMesh        (const_cast<MEDMEM::MESH*>(_dist_mesh));
  cz->setLocalDomainNumber  (_loc_domain);
  cz->setDistantDomainNumber(_dist_domain);
  cz->setName               ("Connect zone defined by SPLITTER");
  cz->setEntityCorresp( MED_CELL, MED_CELL, makeCellCorrespArray() );

  // node/node correspondency

  // map local node id on this proc to local node id on distant proc
  map< int, int > here_2_dist_loc_nodes;
  int iconn_here = 0, iconn_dist = 0;

  TGlob2LocsHD::const_iterator glob_hloc_dloc = _glob_to_locs_here_and_dist.begin();
  for ( ; glob_hloc_dloc != _glob_to_locs_here_and_dist.end(); ++ glob_hloc_dloc )
  {
    int cell_here = glob_hloc_dloc->second.first;
    int cell_dist = glob_hloc_dloc->second.second;
    int nb_cell_nodes_here =  _loc_mesh->getElementType( MED_CELL, cell_here ) % 100;
    int nb_cell_nodes_dist = _dist_mesh->getElementType( MED_CELL, cell_dist ) % 100;
    for ( int n_here = 0; n_here < nb_cell_nodes_here; ++n_here )
    {
      for ( int n_dist = 0; n_dist < nb_cell_nodes_dist; ++n_dist )
        if ( _global_conn_here[ n_here + iconn_here ] == _global_conn_dist[ n_dist + iconn_dist])
        {
          here_2_dist_loc_nodes.insert( make_pair( _local_conn_here[ n_here + iconn_here ],
                                                   _local_conn_dist[ n_dist + iconn_dist ]));
          break;
        }
    }
    iconn_here += nb_cell_nodes_here;
    iconn_dist += nb_cell_nodes_dist;
  }
  // create MEDSKYLINEARRAY
  int nb_nodes_here   = here_2_dist_loc_nodes.size();
  int node_value_size = nb_nodes_here * 2;
  int* node_index = new int [nb_nodes_here+1];
  int* node_value = new int [node_value_size];
  MEDMEM::MEDSKYLINEARRAY* node_node =
    new MEDMEM::MEDSKYLINEARRAY( nb_nodes_here, node_value_size,
                                 node_index, node_value,/*shallowcopy=*/true);

  map< int, int >::const_iterator here_dist_ids = here_2_dist_loc_nodes.begin();
  for ( int i = 1; here_dist_ids != here_2_dist_loc_nodes.end(); ++here_dist_ids)
  {
    *node_index++ = i++;
    *node_value++ = here_dist_ids->first;
    *node_value++ = here_dist_ids->second;
  }
  *node_index = nb_nodes_here+1;

  here_2_dist_loc_nodes.clear();

  cz->setNodeCorresp (node_node);

  // Create faces

  if ( face_map.size() > _loc_domain )
  {
    TGeom2Faces& faces_of_geom = face_map[ _loc_domain ];
    
    int iconn_here = 0, iconn_dist = 0;
    glob_hloc_dloc = _glob_to_locs_here_and_dist.begin();
    for ( int iface = 0; glob_hloc_dloc != _glob_to_locs_here_and_dist.end(); ++glob_hloc_dloc )
    {
      int cell_here = glob_hloc_dloc->second.first;
      int cell_dist = glob_hloc_dloc->second.second;
      medGeometryElement type_here = _loc_mesh ->getElementType( MED_CELL, cell_here );
      medGeometryElement type_dist = _dist_mesh->getElementType( MED_CELL, cell_dist );
      const MEDMEM::CELLMODEL& model_here = getCellModel( type_here );

      MEDSPLITTER_FaceModel* face =
        MEDSPLITTER_FaceModel::getCommonFace( &_global_conn_here[iconn_here],
                                              &_local_conn_here [iconn_here],
                                              model_here,
                                              &_global_conn_dist[iconn_dist],
                                              type_dist % 100,
                                              _first_glob_sub_id + iface);
      faces_of_geom[ face->getType() ].push_back( face );

      iconn_here += type_here % 100;
      iconn_dist += type_dist % 100;
      iface++;
    }
  }

  // clear
  _glob_to_locs_here_and_dist.clear();
  _global_conn_here.clear();
  _global_conn_dist.clear();
  _local_conn_here.clear();
  _local_conn_dist.clear();

  return cz;
}

//================================================================================
/*!
 * \brief Set local and global conncetivity of joint cells
 */
//================================================================================

void JointExchangeData::setConnectivity(const int* glob_fused_nodes)
{
  _global_conn_here.reserve( _conn_here_size );
  _local_conn_here.reserve ( _conn_here_size );
  const int* conn  = _loc_mesh->getConnectivity(MED_NODAL, MED_CELL, MED_ALL_ELEMENTS);
  const int* index = _loc_mesh->getConnectivityIndex(MED_NODAL, MED_CELL);

  TGlob2LocsHD::iterator glob_locs_here_dist     = _glob_to_locs_here_and_dist.begin();
  TGlob2LocsHD::iterator glob_locs_here_dist_end = _glob_to_locs_here_and_dist.end();
  for ( ; glob_locs_here_dist != glob_locs_here_dist_end; ++glob_locs_here_dist )
  {
    int loc_cell_h = glob_locs_here_dist->second.first;
    _local_conn_here.insert( _local_conn_here.end(),
                             conn + index[ loc_cell_h-1 ]- 1,
                             conn + index[ loc_cell_h   ]- 1);
  }
  for ( int i = 0; i < _conn_here_size; ++i )
    _global_conn_here.push_back( glob_fused_nodes[ _local_conn_here[i]-1 ]);
}
