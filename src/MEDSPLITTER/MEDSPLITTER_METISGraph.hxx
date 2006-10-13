#ifndef METISGRAPH_HXX_
#define METISGRAPH_HXX_

namespace MEDSPLITTER {
class METISGraph:public Graph
{
public:
	METISGraph();
	METISGraph(const MEDMEM::MEDSKYLINEARRAY*, int* edgeweight=0);
	virtual ~METISGraph();
  void partGraph(int ndomain, const string& options_string="");
//private:
//	const MEDMEM::MEDSKYLINEARRAY* m_graph;
};
}
#endif /*METISGRAPH_HXX_*/
