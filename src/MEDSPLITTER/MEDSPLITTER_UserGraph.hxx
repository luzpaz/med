#ifndef MEDSPLITTER_USERGRAPH_HXX_
#define MEDSPLITTER_USERGRAPH_HXX_

#include "MEDSPLITTER.hxx"

namespace MEDSPLITTER
{
  class MEDSPLITTER_EXPORT UserGraph : public Graph
  {
  public:
    UserGraph(MEDMEM::MEDSKYLINEARRAY*, const int*, int);
    virtual ~UserGraph();
    void partGraph(int, const string& options="");
  };
}

#endif /*MEDSPLITTER_USERGRAPH_HXX_*/
