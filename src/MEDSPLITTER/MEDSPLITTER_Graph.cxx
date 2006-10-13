#include "MEDSPLITTER_Graph.hxx"

using namespace MEDSPLITTER;

Graph::Graph(const MEDMEM::MEDSKYLINEARRAY* array, int* edgeweight):m_graph(array),m_partition(0),m_edgeweight(edgeweight),m_cellweight(0)
{
		
}

