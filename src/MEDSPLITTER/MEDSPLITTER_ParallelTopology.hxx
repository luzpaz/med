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
#ifndef PARALLELTOPOLOGY_HXX_
#define PARALLELTOPOLOGY_HXX_

#include <set>
#include "InterpKernelHashMap.hxx"
#include "boost/shared_ptr.hpp"

#include "MEDSPLITTER_Topology.hxx"

namespace INTERP_KERNEL
{
  template<> struct hash< pair<int,int> >
  {
    size_t operator()( const pair<int,int>& x ) const
    {
      return hash< int >()( x.first*1000000+x.second );
    }
  };
}

namespace MEDSPLITTER {

  class Graph;
  class MESHCollection;
  class MEDSPLITTER_FaceModel;

  class ParallelTopology:public Topology
  {

  public:

    ParallelTopology();

    ParallelTopology(const std::vector<MEDMEM::MESH*>&,
                     const std::vector<MEDMEM::CONNECTZONE*>&,
                     std::vector<int*>&,
                     std::vector<int*>&,
                     std::vector<int*>&);

    ParallelTopology(boost::shared_ptr<Graph> graph, int nbdomain, int mesh_dimension);

    ~ParallelTopology();
    //!converts a list of global cell numbers
    //!to a distributed array with local cell numbers
    void convertGlobalNodeList(const int*, int,int*,int*);
    void convertGlobalNodeList(const int*, int,int*,int);
    void convertGlobalNodeListWithTwins(const int* face_list, int nbnode, int*& local, int*& ip, int*& full_array, int& size);

    //!converts a list of global node numbers
    //!to a distributed array with local cell numbers
    void convertGlobalCellList(const int*, int , int*, int *);

    //!converts a list of global face numbers
    //!to a distributed array with local face numbers
    void convertGlobalFaceList(const int*, int , int*, int *);  
    void convertGlobalFaceList(const int*, int , int*, int);  
    void convertGlobalFaceListWithTwins(const int* face_list, int nbface, int*& local, int*& ip, int*& full_array,int& size);

    //!creating node mapping 

    void createNodeMapping(std::map<MED_EN::medGeometryElement,int*>& type_connectivity,
                           std::map<MED_EN::medGeometryElement,int>& present_type_numbers,
                           std::vector<int>& conn,
                           std::vector<int>& conn_index,
                           std::vector<int>& polyhedron_conn,
                           std::vector<int>& polyhedron_conn_index,
                           std::vector<int>& polyhedron_face_index,
                           int idomain);

    //  void createFaceMapping(std::map<MED_EN::medGeometryElement,int*>& type_connectivity,
    //               std::map<MED_EN::medGeometryElement,int>& present_type_numbers,
    //               int idomain);          

    void createFaceMapping(const MESHCollection &, const MESHCollection&);
    void createFaceMapping2ndversion(const MESHCollection &);

    //!converting node global numberings to local numberings
    void convertToLocal(std::map<MED_EN::medGeometryElement,int*>& type_connectivity,
                        std::map<MED_EN::medGeometryElement,int>& present_type_numbers,
                        int idomain,
                        MED_EN::medEntityMesh entity);
    void convertToLocal2ndVersion(int* nodes, int nbnodes, int idomain);



    //! computing arrays with node/node correspondencies
    void computeNodeNodeCorrespondencies(int nbdomain,vector<MEDMEM::MEDSKYLINEARRAY*>& ) const;

    //! computing arrays with node/node correspondencies
    void computeCellCellCorrespondencies(int nbdomain,vector<MEDMEM::MEDSKYLINEARRAY*>&, const Graph*) const;

    //! retrieving Graph
    //  boost::shared_ptr<Graph> getGraph() const;


    //!converting node local numbering to global
    inline  int convertNodeToGlobal(int ip,int icell) const
    {
      //return m_node_loc_to_glob.find(make_pair(ip,icell))->second;
      return icell < 1 ? icell : m_node_loc_to_glob[ip][icell-1];
    }

    //!converting face local numbering to global
    inline  int convertFaceToGlobal(int ip,int iface) const
    {
      //     if (m_face_loc_to_glob.find(make_pair(ip,icell))==m_face_loc_to_glob.end())
      //       return -1;
      //     else
      //return m_face_loc_to_glob.find(make_pair(ip,icell))->second;
      return m_face_loc_to_glob[ip][iface-1];
    }

    //converting cell global numbering to local
    inline  int convertCellToGlobal(int ip,int icell) const
    {
      //     if (m_loc_to_glob.find(make_pair(ip,icell))==m_loc_to_glob.end())
      //       return -1;
      //     else
      //return m_loc_to_glob.find(make_pair(ip,icell))->second;
      return m_loc_to_glob[ip][icell-1];
    }

    inline  void convertNodeToGlobal(int ip, const int* local, int n, int* global)const
    {
      for (int i=0; i<n; i++)
        //global[i]=m_node_loc_to_glob[ip][local[i]-1];
        global[i]=convertNodeToGlobal(ip,local[i]);
      //global[i]=m_node_loc_to_glob.find(make_pair(ip,local[i]))->second;
    }

    inline  void convertCellToGlobal(int ip, const int* local, int n, int* global)const
    {
      for (int i=0; i<n; i++)
        global[i]=m_loc_to_glob[ip][local[i]-1];  
      //global[i]=m_loc_to_glob.find(make_pair(ip,local[i]))->second;
    }

    inline  void convertFaceToGlobal(int ip, const int* local, int n, int* global)const
    {
      for (int i=0; i<n; i++)
      {
        global[i]=m_face_loc_to_glob[ip][local[i]-1];
        //          if (m_face_loc_to_glob.find(make_pair(ip,local[i]))==m_face_loc_to_glob.end())
        //            {
        //              cout << "problem : face # "<< local[i] << " of processor "<< ip<< " was not found in mapping loc2glob"<<endl; 
        //              global[i]=-1;
        //            }
        //          else
        //            global[i]=m_face_loc_to_glob.find(make_pair(ip,local[i]))->second;
      }
    }

    inline  int nbDomain() const
    {
      return m_nb_domain;
    }

    int nbCells() const
    {
      return m_nb_total_cells;
    }


    inline  int nbCells( int idomain) const
    {
      return m_nb_cells[idomain];
    }




    //!retrieving number of nodes
    inline  int getNodeNumber(int idomain) const
    {
      return m_nb_nodes[idomain];
    }

    inline  int getNodeNumber() const
    {
      if (m_node_glob_to_loc.empty()) return 0;
      set <int> keys;
      for (INTERP_KERNEL::HashMultiMap<int, pair<int,int> >::const_iterator iter= m_node_glob_to_loc.begin();
           iter!=m_node_glob_to_loc.end();
           iter++) {
        keys.insert(iter->first);
      }
      return keys.size();
    }

    //!retrieving list of nodes in global numbers
    inline  void getNodeList(int idomain, int* list) const
    {
      for (int i=0; i<m_nb_nodes[idomain];i++)
      {
        list[i]=m_node_loc_to_glob[idomain][i];
        //list[i]=(m_node_loc_to_glob.find(make_pair(idomain,i+1)))->second;
      }
    }

    //!< retrieving cell numbers after fusing in parallel mode
    std::vector<int> & getFusedCellNumbers(int idomain)
    {
      return m_cell_loc_to_glob_fuse[idomain];
    }
    const std::vector<int> & getFusedCellNumbers(int idomain) const
    {
      return m_cell_loc_to_glob_fuse[idomain];
    }

    //!< retrieving face numbers after fusing in parallel mode
    std::vector<int> & getFusedFaceNumbers(int idomain)
    {
      return m_face_loc_to_glob_fuse[idomain];
    }
    const std::vector<int> & getFusedFaceNumbers(int idomain) const
    {
      return m_face_loc_to_glob_fuse[idomain];
    }


    //!retrieving number of nodes
    inline  int getCellNumber(int idomain) const
    {
      return m_nb_cells[idomain];
    }

    inline  int getCellDomainNumber(int global) const
    {
      return (m_glob_to_loc.find(global)->second).first;
    }

    //!retrieving list of nodes in global numbers
    inline  void getCellList(int idomain, int* list) const
    {
      for (int i=0; i<m_nb_cells[idomain];i++)
      {
        list[i]=m_loc_to_glob[idomain][i];
        //list[i]=(m_loc_to_glob.find(make_pair(idomain,i+1)))->second;
      }

    }

    inline int getFaceNumber(int idomain) const
    {
      return m_nb_faces[idomain];
    }

    inline  int getFaceNumber() const
    {
      if (m_face_glob_to_loc.empty()) return 0;
      set <int> keys;
      for (INTERP_KERNEL::HashMultiMap<int, pair<int,int> >::const_iterator iter= m_face_glob_to_loc.begin();
           iter!=m_face_glob_to_loc.end();
           iter++) {
        keys.insert(iter->first);
      }
      return keys.size();
    }


    //!retrieving list of faces in global numbers
    inline  void getFaceList(int idomain, int* list) const
    {
      for (int i=0; i<m_nb_faces[idomain];i++)
      {
        list[i]=m_face_loc_to_glob[idomain][i];
        //list[i]=(m_face_loc_to_glob.find(make_pair(idomain,i+1)))->second;
      }

    }

    //! converting a global cell number to a local representation (domain + local number)
    inline std::pair<int,int> convertGlobalCell(int iglobal) const
    {
      return m_glob_to_loc.find(iglobal)->second;
    }

    inline int convertGlobalFace(int iglobal, int idomain)
    {
      typedef INTERP_KERNEL::HashMultiMap<int, pair<int,int> >::const_iterator MMiter;
      pair<MMiter,MMiter> eq = m_face_glob_to_loc.equal_range(iglobal);
      for (MMiter it=eq.first; it != eq.second; it++)
      {
        SCRUTE_MED (it->second.first);
        SCRUTE_MED (idomain);
        if (it->second.first == idomain) return it->second.second;

      }
      return -1;
    }

    inline int convertGlobalNode(int iglobal, int idomain)
    {
      typedef INTERP_KERNEL::HashMultiMap<int, pair<int,int> >::const_iterator MMiter;
      pair<MMiter,MMiter> eq = m_node_glob_to_loc.equal_range(iglobal);
      for (MMiter it=eq.first; it != eq.second; it++)
      {
        if (it->second.first == idomain) return it->second.second;
      }
      return -1;
    }
    //!adding a face to the topology
    inline void appendFace(int idomain, int ilocal, int iglobal)
    {
      m_face_loc_to_glob[idomain].push_back(iglobal);
      m_face_glob_to_loc.insert(make_pair(iglobal,make_pair(idomain,ilocal)));
    }

    //return max global face number
    int getMaxGlobalFace() const;

    boost::shared_ptr<Graph> getGraph() const
    {
      return m_graph;
    }

    //!recreating a face mapping from scratch
    void recreateFaceMapping(const TGeom2FacesByDomian& face_map);

    // recreating cell and node mapping after send-reveive and fusion of domain meshes
    virtual void recreateMappingAfterFusion(const std::vector<MEDMEM::MESH*>& );



  private:

    bool hasCellWithNodes( const MESHCollection&, int dom, const std::set<int>& nodes );


  private:
    //!mapping global -> local
    typedef INTERP_KERNEL::HashMap<int,pair<int,int> > TGlob2DomainLoc;
    TGlob2DomainLoc m_glob_to_loc;

    //  bool is_equal_pair (pair<int,int> a, pair<int,int> b){
    //      return  (a.first==b.first && a.second==b.second);
    // }
    //!mapping local -> global
    //map<pair<int,int>,int> m_loc_to_glob;

    //
    //INTERP_KERNEL::HashMap<pair<int,int>,int, INTERP_KERNEL::hash<pair<int,int> > > m_loc_to_glob;
    vector<vector<int> >  m_loc_to_glob;
    //!mapping global -> local
    INTERP_KERNEL::HashMultiMap<int,pair<int,int> > m_node_glob_to_loc;

    //!mapping local -> global
    //  map<pair<int,int>,int> m_node_loc_to_glob;
    //INTERP_KERNEL::HashMap<pair<int,int>,int, INTERP_KERNEL::hash<pair<int,int> > > m_node_loc_to_glob;
    vector<vector <int> > m_node_loc_to_glob;

    // global numbers in parallel mode
    vector<vector <int> > m_cell_loc_to_glob_fuse; // glob nums after fusing
    vector<vector <int> > m_face_loc_to_glob_fuse; // glob nums after fusing


    //!mapping global -> local
    typedef INTERP_KERNEL::HashMultiMap<int,pair<int,int> > TGlob2LocsMap;
    TGlob2LocsMap m_face_glob_to_loc;

    //!mapping local -> global
    //INTERP_KERNEL::HashMap<pair<int,int>,int, INTERP_KERNEL::hash<pair<int,int> > > m_face_loc_to_glob;
    vector<vector <int> > m_face_loc_to_glob;

    //map<pair<int,int>,int> m_face_loc_to_glob;

    vector<int> m_nb_cells;

    vector<int> m_nb_nodes;

    vector<int> m_nb_faces;

    int m_nb_total_cells;

    int m_nb_total_nodes;

    int m_nb_total_faces;

    int m_nb_domain;

    int m_mesh_dimension;

    boost::shared_ptr<Graph> m_graph;
  };


}
#endif /*PARALLELTOPOLOGY_HXX_*/
