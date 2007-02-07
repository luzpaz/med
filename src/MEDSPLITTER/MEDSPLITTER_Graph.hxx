#ifndef GRAPH_HXX_
#define GRAPH_HXX_

#include "MEDSPLITTER.hxx"

#include "MEDMEM_SkyLineArray.hxx"

namespace MEDSPLITTER {
	
class MEDSPLITTER_EXPORT Graph
{
	
	public:
	typedef enum {METIS,SCOTCH} splitter_type;
	
	Graph(){}
	
	//creates a graph from a SKYLINEARRAY
	Graph(const MEDMEM::MEDSKYLINEARRAY* graph, int* edgeweight=0);
	
	virtual ~Graph(){}
	
  void setEdgesWeights(int* edgeweight){m_edgeweight=edgeweight;}
  void setVerticesWeights(int* cellweight){m_cellweight=cellweight;}

	//computes partitioning of the graph
  virtual void partGraph(int ndomain, const string&)=0;
	
	//! returns the partitioning
	const int* getPart() const {return m_partition->getValue();}
	
	//! returns the number of graph vertices (which can correspond to the cells in the mesh!)
	int nbVertices() const {return m_graph->getNumberOf();}
	
	const MEDMEM::MEDSKYLINEARRAY* getGraph() const {return m_graph;}
	
	protected:
	const 	MEDMEM::MEDSKYLINEARRAY* m_graph;
	
	MEDMEM::MEDSKYLINEARRAY* m_partition;

	int* m_edgeweight;	

  int* m_cellweight;
};

}
#endif /*GRAPH_HXX_*/
