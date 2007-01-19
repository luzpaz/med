#ifndef PARALLELTOPOLOGY_HXX_
#define PARALLELTOPOLOGY_HXX_

#include <ext/hash_map>
#include "boost/shared_ptr.hpp"

#include "MEDSPLITTER_Topology.hxx"

namespace __gnu_cxx
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


	class ParallelTopology:public Topology
	{
	
	public:
	
		ParallelTopology();
	
		ParallelTopology(vector<MEDMEM::MESH*>, vector<MEDMEM::CONNECTZONE*>,vector<int*>&, vector<int*>&, vector<int*>&);
	
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
													 int idomain);
							
		//	void createFaceMapping(std::map<MED_EN::medGeometryElement,int*>& type_connectivity,
		//						   std::map<MED_EN::medGeometryElement,int>& present_type_numbers,
		//						   int idomain);					
							
		void createFaceMapping(const MESHCollection &);
		void createFaceMapping2ndversion(const MESHCollection &);
	
		//!converting node global numberings to local numberings
		void convertToLocal(std::map<MED_EN::medGeometryElement,int*>& type_connectivity,
												std::map<MED_EN::medGeometryElement,int>& present_type_numbers,
												int idomain,
												MED_EN::medEntityMesh entity);


	
		//! computing arrays with node/node correspondencies
		void computeNodeNodeCorrespondencies(int nbdomain,vector<MEDMEM::MEDSKYLINEARRAY*>& ) const;
	
		//! computing arrays with node/node correspondencies
		void computeCellCellCorrespondencies(int nbdomain,vector<MEDMEM::MEDSKYLINEARRAY*>&, const Graph* ) const;

		//! retrieving Graph
		//	boost::shared_ptr<Graph> getGraph() const;
	
	
		//!converting node local numbering to global
		inline  int convertNodeToGlobal(int ip,int icell) const
		{
			return m_node_loc_to_glob.find(make_pair(ip,icell))->second;
		}

		//!converting face local numbering to global
		inline  int convertFaceToGlobal(int ip,int icell) const
		{
			//     if (m_face_loc_to_glob.find(make_pair(ip,icell))==m_face_loc_to_glob.end())
			//       return -1;
			//     else
      return m_face_loc_to_glob.find(make_pair(ip,icell))->second;
		}

		//converting cell global numbering to local
		inline  int convertCellToGlobal(int ip,int icell) const
		{
			//     if (m_loc_to_glob.find(make_pair(ip,icell))==m_loc_to_glob.end())
			//       return -1;
			//     else
      return m_loc_to_glob.find(make_pair(ip,icell))->second;
		}

		inline  void convertNodeToGlobal(int ip, const int* local, int n, int* global)const
		{
			for (int i=0; i<n; i++)
				global[i]=m_node_loc_to_glob.find(make_pair(ip,local[i]))->second;
		}

		inline  void convertCellToGlobal(int ip, const int* local, int n, int* global)const
		{
			for (int i=0; i<n; i++)
				global[i]=m_loc_to_glob.find(make_pair(ip,local[i]))->second;
		}

		inline  void convertFaceToGlobal(int ip, const int* local, int n, int* global)const
		{
			for (int i=0; i<n; i++)
				{
					if (m_face_loc_to_glob.find(make_pair(ip,local[i]))==m_face_loc_to_glob.end())
						{
							cout << "problem : face # "<< local[i] << " of processor "<< ip<< " was not found in mapping loc2glob"<<endl;	
							global[i]=-1;
						}
					else
						global[i]=m_face_loc_to_glob.find(make_pair(ip,local[i]))->second;
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
			//		return m_node_glob_to_loc.rbegin()->first;
		}

		//!retrieving list of nodes in global numbers
		inline  void getNodeList(int idomain, int* list) const
		{
			for (int i=0; i<m_nb_nodes[idomain];i++)
				{
					list[i]=(m_node_loc_to_glob.find(make_pair(idomain,i+1)))->second;
				}
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
					list[i]=(m_loc_to_glob.find(make_pair(idomain,i+1)))->second;
				}
	
		}

		inline int getFaceNumber(int idomain) const
		{
			return m_nb_faces[idomain];
		}

		inline  int getFaceNumber() const
		{
			if (m_face_glob_to_loc.empty()) return 0;
			//		return m_face_glob_to_loc.rbegin()->first;
		}

		//!retrieving list of faces in global numbers
		inline  void getFaceList(int idomain, int* list) const
		{
			for (int i=0; i<m_nb_faces[idomain];i++)
				{
					list[i]=(m_face_loc_to_glob.find(make_pair(idomain,i+1)))->second;
				}
	
		}

		boost::shared_ptr<Graph> getGraph() const
		{
			return m_graph;
		}


	private:
		//!mapping global -> local
		__gnu_cxx::hash_map<int,pair<int,int> > m_glob_to_loc;

		//  bool is_equal_pair (pair<int,int> a, pair<int,int> b){
		//      return  (a.first==b.first && a.second==b.second);
		// }
		//!mapping local -> global
		//map<pair<int,int>,int> m_loc_to_glob;
   
    //
		__gnu_cxx::hash_map<pair<int,int>,int, __gnu_cxx::hash<pair<int,int> > > m_loc_to_glob;
    
		//!mapping global -> local
		__gnu_cxx::hash_multimap<int,pair<int,int> > m_node_glob_to_loc;
	
		//!mapping local -> global
		//	map<pair<int,int>,int> m_node_loc_to_glob;
    __gnu_cxx::hash_map<pair<int,int>,int, __gnu_cxx::hash<pair<int,int> > > m_node_loc_to_glob;
   
    
	
		//!mapping global -> local
		__gnu_cxx::hash_multimap<int,pair<int,int> > m_face_glob_to_loc;
	
		//!mapping local -> global
    __gnu_cxx::hash_map<pair<int,int>,int, __gnu_cxx::hash<pair<int,int> > > m_face_loc_to_glob;
   
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
