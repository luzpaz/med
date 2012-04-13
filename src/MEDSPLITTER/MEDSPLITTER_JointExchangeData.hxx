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
// File      : MEDSPLITTER_JointExchangeData.hxx
// Created   : Wed Jul 22 15:29:44 2009
// Author    : Edward AGAPOV (eap)


#ifndef __MEDSPLITTER_JointExchangeData_HXX__
#define __MEDSPLITTER_JointExchangeData_HXX__

#include "MEDSPLITTER_Topology.hxx"

#include <map>
#include <vector>

namespace MEDMEM
{
  class MESH;
  class CONNECTZONE;
  class MEDSKYLINEARRAY;
}

namespace MEDSPLITTER
{
  /*!
   * \brief Class storing data of the joint between domains residing different procs.
   *  It also provides services to pack/unpack data to be exchanged between proc and
   *  it creates MEDMEM::CONNECTZONE after data exchange.
   */
  class JointExchangeData
  {
  public:

    JointExchangeData();

    // Store cell/cell pair of joint between domains (residing different procs by default)
    void addCellCorrespondence(const MEDMEM::MESH* mesh_here,
                               int domain_dist, int domain_here,
                               int glob_dist, int glob_here,
                               int loc_here, int loc_dist = -1 );

    // Return nb of cell pairs
    int nbCellPairs() const { return _glob_to_locs_here_and_dist.size(); }

    // Stores meshes and corresponding domain ids
    void setMeshes( int domain_dist, const MEDMEM::MESH* mesh_dist,
                    int domain_here, const MEDMEM::MESH* mesh_here); 

    // Stores local and global connectivity of joint cells on this proc
    void setConnectivity( const int* glob_fused_nodes);

    int distantDomain() const { return _dist_domain; }
    int   localDomain() const { return _loc_domain; }

    // Exchanging
    int serialize (std::vector<int> & out_data) const;
    void deserialize(const std::vector<int> & in_data);

    //!< Set starting global id for sub-entitis of joint to create
    void setFisrtGlobalIdOfSubentity(int id) { _first_glob_sub_id = id; }

    // Create MEDSKYLINEARRAY describing cell/cell correspondencies of the joint
    MEDMEM::MEDSKYLINEARRAY* makeCellCorrespArray() const;

    // Creates CONNECTZONE
    MEDMEM::CONNECTZONE* makeConnectZone(TGeom2FacesByDomian& face_map);

  private:

    // map global cell id on of procs to pair of local cell ids: here and distant
    typedef std::multimap<int, std::pair< int, int > > TGlob2LocsHD;
    TGlob2LocsHD _glob_to_locs_here_and_dist;

    int _dist_domain, _loc_domain, _conn_here_size, _nb_cell_pairs, _first_glob_sub_id;

    const MEDMEM::MESH* _dist_mesh;
    const MEDMEM::MESH* _loc_mesh;

    std::vector<int> _global_conn_here, _global_conn_dist;
    std::vector<int> _local_conn_here,  _local_conn_dist;

  };
}
#endif
