//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D
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
extern "C" {
#include "metis.h"
}
#include"MEDSPLITTER_Graph.hxx"
#include "MEDSPLITTER_METISGraph.hxx"

using namespace MEDSPLITTER;
	
METISGraph::METISGraph():Graph()
{
}

METISGraph::METISGraph(const MEDMEM::MEDSKYLINEARRAY* graph, int* edgeweight)
	:Graph(graph,edgeweight)
{
}

METISGraph::~METISGraph()
{
	if (m_partition!=0) {delete m_partition; m_partition=0;}
	if (m_graph!=0)  {delete m_graph; m_graph=0;}
}

void METISGraph::partGraph(int ndomain, const string& options_string)
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
	int base=1;
	
	//ndomain
	int nparts = ndomain;
	
	//options
	int options[5]={0,0,0,0,0};
	
	// output parameters
	int edgecut;
	int* partition = new int[n+1];
	
	if (nparts >1)
	  {
	    if (options_string != "k")
	      METIS_PartGraphRecursive(&n, xadj, adjncy, vwgt, adjwgt, &wgtflag,
				       &base, &nparts, options, &edgecut, partition);
	    else
	      METIS_PartGraphKway(&n, xadj, adjncy, vwgt, adjwgt, &wgtflag,
				  &base, &nparts, options, &edgecut, partition);
	  }
	else
	  {
	    for (int i=0; i<n+1; i++)
	      partition[i]=1;
	  }
									 
	int* index=new int [n+1];
	index[0]=1;
	for (int i=0; i<n; i++)
	{
		index[i+1]=index[i]+1;
		partition[i]--;
	}
	
	//creating a skylinearray with no copy of the index and partition array
	// the fifth argument true specifies that only the pointers are passed 
	//to the object
	m_partition = new MEDMEM::MEDSKYLINEARRAY(n,n, index, partition, true);
}

