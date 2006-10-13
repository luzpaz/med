#include "MEDMEM_SkyLineArray.hxx"
#include "MEDMEM_ConnectZone.hxx"
#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_Mesh.hxx"

#include "MEDSPLITTER_Topology.hxx"
#include "MEDSPLITTER_SequentialTopology.hxx"

using namespace MEDSPLITTER;
	
SequentialTopology::SequentialTopology(){}

SequentialTopology::SequentialTopology(std::vector<MEDMEM::MESH*>, std::vector<MEDMEM::CONNECTZONE*>){}

//!converts a list of global cell numbers
//!to a distributed array with local cell numbers
void SequentialTopology::convertGlobalNodeList(const int*, int,int*,int*){}

//!converts a list of global node numbers
//!to a distributed array with local cell numbers
void SequentialTopology::convertGlobalCellList(const int*, int , int*, int *){}
	
//number of doamins
int SequentialTopology::nbDomain() const{}

//number of cells
int SequentialTopology::nbCells() const{}
	
//number of cells on a specific domain
int SequentialTopology::nbCells(int idomain) const{}

//!creating node mapping 
void SequentialTopology::createNodeMapping(vector<int*> type_connectivity,int* present_type_numbers, int idomain){}

//!converting node global numberings to local numberings
void SequentialTopology::convertToLocal(vector<int*> type_connectivity,int* present_type_numbers){}

//!retrieving number of nodes
int SequentialTopology::getNodeNumber(int idomain) const {}

//!retrieving list of nodes
void SequentialTopology::getNodeList(int idomain, int* list) const{}

//!retrieving number of cells
int SequentialTopology::getCellNumber(int idomain) const {}

//!retrieving list of cells
void SequentialTopology::getCellList(int idomain, int* list) const{}



