#include "MEDSPLITTER_Graph.hxx"

using namespace MEDSPLITTER;

<<<<<<< MEDSPLITTER_Graph.cxx
Graph::Graph(const MEDMEM::MEDSKYLINEARRAY* array, int* edgeweight):m_graph(array),m_partition(0),m_edgeweight(edgeweight)
=======
Graph::Graph(const MEDMEM::MEDSKYLINEARRAY* array, int* edgeweight):m_graph(array),m_partition(0),m_edgeweight(edgeweight),m_cellweight(0)
>>>>>>> 1.1.4.1
{
		
}

