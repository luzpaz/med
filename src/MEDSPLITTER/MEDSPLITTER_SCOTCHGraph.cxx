#include <cstdio>
extern "C" {
#define restrict
#include "bin/scotch.h"
}
#include"MEDSPLITTER_Graph.hxx"
#include "MEDSPLITTER_SCOTCHGraph.hxx"

using namespace MEDSPLITTER;
	
SCOTCHGraph::SCOTCHGraph():Graph()
{
}

SCOTCHGraph::SCOTCHGraph(const MEDMEM::MEDSKYLINEARRAY* graph, int* edgeweight):Graph(graph,edgeweight)
{
}

SCOTCHGraph::~SCOTCHGraph()
{
	if (m_partition!=0) delete m_partition;
}

void SCOTCHGraph::partGraph(int ndomain, const string& options_string)
{
	// number of graph vertices
	int n = m_graph->getNumberOf();
	
	//graph
	int * xadj=const_cast<int*>(m_graph->getIndex());
	int * adjncy = const_cast<int*>(m_graph->getValue());
<<<<<<< MEDSPLITTER_SCOTCHGraph.cxx
	
	//base 0 or 1
	int base=1;
=======
>>>>>>> 1.1.4.1
	
	//ndomain
	int nparts = ndomain;
	
	// output parameters
	int* partition = new int[n+1];
	
	SCOTCH_Graph scotch_graph;
	
	SCOTCH_graphInit(&scotch_graph);
	
	
	SCOTCH_graphBuild(&scotch_graph,
<<<<<<< MEDSPLITTER_SCOTCHGraph.cxx
					  1, //premier indice 1
					  n, // nb of graph nodes
					  xadj,
					  0,
					  0,
					  0,
					  xadj[n], // number of edges
					  adjncy,
					  m_edgeweight);
=======
			  1, //premier indice 1
			  n, // nb of graph nodes
			  xadj,
			  0,
			  m_cellweight, //graph vertices loads
			  0,
			  xadj[n], // number of edges
			  adjncy,
			  m_edgeweight);
>>>>>>> 1.1.4.1

	SCOTCH_Strat scotch_strategy;					  
	SCOTCH_stratInit(&scotch_strategy);
<<<<<<< MEDSPLITTER_SCOTCHGraph.cxx
    SCOTCH_stratGraphMap(&scotch_strategy,
    						"b{strat=f{bal=0.0001}}");
=======

	//!user-defined options for the strategy
	if (options_string!="")
	  SCOTCH_stratGraphMap(&scotch_strategy,options_string.c_str());
>>>>>>> 1.1.4.1
    						  

	if (nparts>1)					  
		SCOTCH_graphPart(&scotch_graph,nparts,&scotch_strategy,partition);
	else
	  // partition for 1 subdomain
		for (int i=0; i<n+1; i++)
			partition[i]=1;
			
	SCOTCH_stratExit(&scotch_strategy);
	SCOTCH_graphExit(&scotch_graph);
										 
	int* index=new int [n+1];
	index[0]=1;
	for (int i=0; i<n; i++)
	{
		index[i+1]=index[i]+1;
	}
	
	//creating a skylinearray with no copy of the index and partition array
	// the fifth argument true specifies that only the pointers are passed 
	//to the object
	m_partition = new MEDMEM::MEDSKYLINEARRAY(n,n, index, partition, true);
}

