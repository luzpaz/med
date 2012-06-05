// Copyright (C) 2007-2012  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef GRAPH_HXX_
#define GRAPH_HXX_

#include "MEDSPLITTER.hxx"

#include "MEDMEM_SkyLineArray.hxx"

namespace MEDSPLITTER {
        
class ParaDomainSelector;

class MEDSPLITTER_EXPORT Graph
{
  public:
    typedef enum {METIS,SCOTCH} splitter_type;

    Graph(){}

    //creates a graph from a SKYLINEARRAY
    Graph(MEDMEM::MEDSKYLINEARRAY* graph, int* edgeweight=0);

    virtual ~Graph();

    void setEdgesWeights(int* edgeweight){m_edgeweight=edgeweight;}
    void setVerticesWeights(int* cellweight){m_cellweight=cellweight;}

    //computes partitioning of the graph
    virtual void partGraph(int ndomain, const string&, ParaDomainSelector* sel=0)=0;

    //! returns the partitioning
    const int* getPart() const {return m_partition->getValue();}

    //! returns the number of graph vertices (which can correspond to the cells in the mesh!)
    int nbVertices() const {return m_graph->getNumberOf();}

    const MEDMEM::MEDSKYLINEARRAY* getGraph() const {return m_graph;}

  protected:

    MEDMEM::MEDSKYLINEARRAY* m_graph;

    MEDMEM::MEDSKYLINEARRAY* m_partition;

    int* m_edgeweight;  

    int* m_cellweight;
  };

}
#endif /*GRAPH_HXX_*/
