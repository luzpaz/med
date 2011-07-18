//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifdef ENABLE_PARMETIS
#include <parmetis.h>
#endif
extern "C" {
#include <metis.h>
}
#include "MEDPARTITIONER_METISGraph.hxx"
#include "MEDPARTITIONER_ParaDomainSelector.hxx"
#include "MEDMEM_Exception.hxx"
using namespace MEDPARTITIONER;

METISGraph::METISGraph():Graph()
{
}

METISGraph::METISGraph(MEDPARTITIONER::MEDSKYLINEARRAY* graph, int* edgeweight)
  :Graph(graph,edgeweight)
{
}

METISGraph::~METISGraph()
{
}

void METISGraph::partGraph(int                 ndomain,
                           const std::string&       options_string,
                           ParaDomainSelector* parallelizer)
{
  // number of graph vertices
  int n = m_graph->getNumberOf();

  //graph
  int * xadj=const_cast<int*>(m_graph->getIndex());
  int * adjncy = const_cast<int*>(m_graph->getValue());
  //constraints
  int * vwgt=m_cellweight;
  int * adjwgt=m_edgeweight;
  int wgtflag=(m_edgeweight!=0)?1:0+(m_cellweight!=0)?2:0;

  //base 0 or 1
  int base=0;

  //ndomain
  int nparts = ndomain;

  //options
  int options[5]={0,0,0,0,0};

  // output parameters
  int edgecut;
  int* partition = new int[n];

  cout << "ParMETIS : n="<<n<<endl;
  if (nparts >1)
  {
    if ( parallelizer )
    {
#ifdef ENABLE_PARMETIS
      // distribution of vertices of the graph among the processors
      int * vtxdist = parallelizer ? parallelizer->getNbVertOfProcs() : 0;
      MPI_Comm comm = MPI_COMM_WORLD;
      cout<<"vtxdist[1]"<<" "<<vtxdist[2]<<endl;
      ParMETIS_PartKway( vtxdist, xadj, adjncy, vwgt, adjwgt, &wgtflag,
                         &base, &nparts, options, &edgecut, partition, &comm );
       if (n<8 && nparts==3)
         {
           for (int i=0; i<n; i++)
             partition[i]=i%3;
         }
        
      
      
#else
      throw MEDMEM::MEDEXCEPTION("ParMETIS is not available. Check your products, please.");
#endif
    }
    else
    {
    
      if (options_string != "k")
        METIS_PartGraphRecursive(&n, xadj, adjncy, vwgt, adjwgt, &wgtflag,
                                 &base, &nparts, options, &edgecut, partition);
      else
        METIS_PartGraphKway(&n, xadj, adjncy, vwgt, adjwgt, &wgtflag,
                            &base, &nparts, options, &edgecut, partition);
    }
  }
  else
  {
    for (int i=0; i<n; i++)
      partition[i]=0;
  }
  std::vector<int> index(n+1);
  std::vector<int> value(n);
  index[0]=0;
  for (int i=0; i<n; i++)
  {
    index[i+1]=index[i]+1;
    value[i]=partition[i];
  }

  delete[]partition;

  //creating a skylinearray with no copy of the index and partition array
  // the fifth argument true specifies that only the pointers are passed 
  //to the object
  
  m_partition = new MEDPARTITIONER::MEDSKYLINEARRAY(index,value);
}

