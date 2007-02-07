#ifndef SequentialTOPOLOGY_HXX_
#define SequentialTOPOLOGY_HXX_

#include "MEDSPLITTER.hxx"

namespace MEDSPLITTER {
class MEDSPLITTER_EXPORT SequentialTopology:public Topology
{
	
public:
	
	SequentialTopology();
	SequentialTopology(std::vector<MEDMEM::MESH*>, std::vector<MEDMEM::CONNECTZONE*>);
	
	//!converts a list of global cell numbers
	//!to a distributed array with local cell numbers
	void convertGlobalNodeList(const int*, int,int*,int*);
	
	//!converts a list of global node numbers
	//!to a distributed array with local cell numbers
	void convertGlobalCellList(const int*, int , int*, int *);
		
	//number of doamins
	int nbDomain() const;
	
	//number of cells
    int nbCells() const;
		
	//number of cells on a specific domain
	int nbCells(int idomain) const;
	
	//!creating node mapping 
	void createNodeMapping(vector<int*> type_connectivity,int* present_type_numbers, int idomain);
	
	//!converting node global numberings to local numberings
	void convertToLocal(vector<int*> type_connectivity,int* present_type_numbers);
	
	//!retrieving number of nodes
	int getNodeNumber(int idomain) const ;
	
	//!retrieving list of nodes
	void getNodeList(int idomain, int* list) const;
	
	//!retrieving number of cells
	int getCellNumber(int idomain) const ;
	
	//!retrieving list of cells
	void getCellList(int idomain, int* list) const;
	
private:
	//!mapping global -> local
	map<int,pair<int,int> > m_glob_to_loc;
	
	//!mapping local -> global
	map<pair<int,int>,int> m_loc_to_glob;
	
	//!mapping global -> local
	multimap<int,pair<int,int> > m_node_glob_to_loc;
	
	//!mapping local -> global
	map<pair<int,int>,int> m_node_loc_to_glob;
	
	vector<int> m_nb_cells;
	
	vector<int> m_nb_nodes;
	
	int m_nb_total_cells;
	
	int m_nb_total_nodes;
};
}
#endif /*SequentialTOPOLOGY_HXX_*/
