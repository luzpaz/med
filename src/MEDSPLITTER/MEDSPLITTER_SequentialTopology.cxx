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
#include "MEDMEM_SkyLineArray.hxx"
#include "MEDMEM_ConnectZone.hxx"
#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Exception.hxx"

#include "MEDSPLITTER_Topology.hxx"
#include "MEDSPLITTER_SequentialTopology.hxx"

using namespace MEDSPLITTER;
  
SequentialTopology::SequentialTopology(){throw MEDEXCEPTION("SequentialTopology - not implemented yet");}

SequentialTopology::SequentialTopology(std::vector<MEDMEM::MESH*>, std::vector<MEDMEM::CONNECTZONE*>){throw MEDEXCEPTION("SequentialTopology - not implemented yet");}

//!converts a list of global cell numbers
//!to a distributed array with local cell numbers
void SequentialTopology::convertGlobalNodeList(const int*, int,int*,int*){}

//!converts a list of global node numbers
//!to a distributed array with local cell numbers
void SequentialTopology::convertGlobalCellList(const int*, int , int*, int *){}

//number of doamins
int SequentialTopology::nbDomain() const
{
  return 0;
}

//number of cells
int SequentialTopology::nbCells() const
{
  return 0;
}

//number of cells on a specific domain
int SequentialTopology::nbCells(int idomain) const
{
  return 0;
}

//!creating node mapping 
void SequentialTopology::createNodeMapping(vector<int*> type_connectivity,int* present_type_numbers, int idomain){}

//!converting node global numberings to local numberings
void SequentialTopology::convertToLocal(vector<int*> type_connectivity,int* present_type_numbers){}

//!retrieving number of nodes
int SequentialTopology::getNodeNumber(int idomain) const
{
  return 0;
}

//!retrieving list of nodes
void SequentialTopology::getNodeList(int idomain, int* list) const{}

//!retrieving number of cells
int SequentialTopology::getCellNumber(int idomain) const
{
  return 0;
}

//!retrieving list of cells
void SequentialTopology::getCellList(int idomain, int* list) const{}
