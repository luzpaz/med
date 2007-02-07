#ifndef SCOTCHGRAPH_HXX_
#define SCOTCHGRAPH_HXX_

#include "MEDSPLITTER.hxx"

namespace MEDSPLITTER {
class MEDSPLITTER_EXPORT SCOTCHGraph:public Graph
{
public:
	SCOTCHGraph();
	SCOTCHGraph(const MEDMEM::MEDSKYLINEARRAY*, int* edgeweight=0);
	virtual ~SCOTCHGraph();
  void partGraph(int ndomain, const string& options_string="");
//private:
//	const MEDMEM::MEDSKYLINEARRAY* m_graph;
};
}
#endif /*SCOTCHGRAPH_HXX_*/
