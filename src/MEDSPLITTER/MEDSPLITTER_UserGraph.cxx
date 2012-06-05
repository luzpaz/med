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
#include "MEDSPLITTER_Graph.hxx"
#include "MEDSPLITTER_UserGraph.hxx"


using namespace MEDSPLITTER;

/*! constructor that allows to specify the desired partition
 * \param partition as table giving the domain number for each cell 
 *        (domain numbers range from 0 to ndomain-1
 * \param n number of cells in the mesh
 */
UserGraph::UserGraph(MEDMEM::MEDSKYLINEARRAY* array, const int* partition, int n):Graph(array,0)
{

  int* index=new int [n+1];
  index[0]=1;
  for (int i=0; i<n; i++)
  {
    index[i+1]=index[i]+1;
  }

  //creating a skylinearray with copy of the index and partition array
  // the fifth argument false specifies that the values are passed  
  //to the object
  //m_partition = new MEDMEM::MEDSKYLINEARRAY(n,n, index, partition, true);
  m_partition = new MEDMEM::MEDSKYLINEARRAY(n,n, index, partition, false);
  delete[] index;
}

UserGraph::~UserGraph()
{
}

void UserGraph::partGraph(int ndomain, const string& options, ParaDomainSelector* sel)
{
  cerr<<"MEDSPLITTER::UserGraph::partGraph() should not have to be used"<<endl;
}

