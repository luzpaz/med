#ifndef TOPOLOGY_HXX_
#define TOPOLOGY_HXX_

#include "boost/shared_ptr.hpp"

namespace MEDMEM
{
	class	CONNECTZONE;
}

namespace MEDSPLITTER {
	
	class Graph;
	class MESHCollection;

	class Topology
	{
	public:
		Topology(){}
		Topology(std::vector<MEDMEM::MESH*>, std::vector<MEDMEM::CONNECTZONE*>){}
	
		virtual ~Topology(){}
	
		//!converts a list of global cell numbers
		//!to a distributed array with local cell numbers
		virtual void convertGlobalNodeList(const int* list, int nb, int* local, int*ip)=0;
		virtual void convertGlobalNodeList(const int* list, int nb, int* local, int ip)=0;
		//!converts a list of global node numbers
		//!to a distributed array with local cell numbers
		virtual void convertGlobalCellList(const int*list , int nb, int* local, int*ip)=0;
	
		//!converts a list of global face numbers
		//!to a distributed array with local face numbers
		virtual void convertGlobalFaceList(const int*list , int nb, int* local, int*ip)=0;
		virtual void convertGlobalFaceList(const int*list , int nb, int* local, int ip)=0;
		virtual void convertGlobalFaceListWithTwins(const int* face_list, int nbface, int*& local, int*& ip, int*& full_array, int& size)=0;
		virtual void convertGlobalNodeListWithTwins(const int* face_list, int nbnode, int*& local, int*& ip, int*& full_array, int& size)=0;
	
	
		//number of doamins
		virtual int nbDomain() const =0;
	
		//number of cells
    virtual	int nbCells() const=0;

	
		//number of cells on a specific domain
		virtual	int nbCells(int idomain) const=0;
	
		////creating node mapping 
		virtual void createNodeMapping(std::map<MED_EN::medGeometryElement,int*>& type_connectivity,
																	 std::map<MED_EN::medGeometryElement,int>& present_type_numbers,int domain)=0;
		
		////creating face mapping 
		//	virtual void createFaceMapping(std::map<MED_EN::medGeometryElement,int*>& type_connectivity,
		//										std::map<MED_EN::medGeometryElement,int>& present_type_numbers,int domain)=0;
		//			
		virtual void createFaceMapping(const MESHCollection&)=0;

		//converting node global numberings to local numberings
		virtual void convertToLocal(std::map<MED_EN::medGeometryElement,int*>& type_connectivity,
																std::map<MED_EN::medGeometryElement,int>& present_type_numbers,
																int idomain,
																MED_EN::medEntityMesh entity)=0;
										
		virtual int convertNodeToGlobal(int ip,int icell)const=0;
		virtual int convertFaceToGlobal(int ip,int icell)const=0;
		virtual int convertCellToGlobal(int ip,int icell)const=0;

		virtual void convertNodeToGlobal(int ip,const int* local, int n, int* global)const=0 ;
		virtual void convertCellToGlobal(int ip,const int* local, int n, int* global)const=0 ;
		virtual void convertFaceToGlobal(int ip,const int* local, int n, int* global)const=0 ;

		//retrieving number of nodes
		virtual int getNodeNumber(int idomain) const =0;
	  virtual int getNodeNumber() const=0;
  	//retrieving list of nodes
		virtual void getNodeList(int idomain, int* list) const =0;
	
		//retrieving number of nodes
		virtual int getCellNumber(int idomain) const =0;
	
		//retrieving list of nodes
		virtual void getCellList(int idomain, int* list) const =0;
	
		//retrieving number of faces
		virtual int getFaceNumber(int idomain) const =0;
    virtual int getFaceNumber()const =0;

		//retrieving list of nodes
		virtual void getFaceList(int idomain, int* list) const =0;
	
		//! computing arrays with node/node correspondencies
		virtual void computeNodeNodeCorrespondencies(int nbdomain, vector<MEDMEM::MEDSKYLINEARRAY*>&) const =0;
	
		//! computing arrays with cell/cell correspondencies
		virtual void computeCellCellCorrespondencies(int nbdomain, vector<MEDMEM::MEDSKYLINEARRAY*>&, const Graph*) const =0;
	
		//! retrieving graph
		virtual boost::shared_ptr<Graph> getGraph() const=0;
	};
}
#endif /*TOPOLOGY_HXX_*/
