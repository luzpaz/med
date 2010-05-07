//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
// This program test the implementation of the POLYHEDRONARRAY class. //
//
#include "MEDMEM_PolyhedronArray.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

int main(void)
{
  POLYHEDRONARRAY myPArray(2,11,40);
  med_int polyhedronindex[2] = {1,7};
  med_int facesindex[11] = {1,5,9,13,17,21,25,28,31,34,37};
  med_int nodes[40] = {1,2,6,5,2,3,7,6,4,3,7,8,1,4,8,5,1,2,3,4,5,6,7,8,5,6,9,6,7,9,7,8,9,8,5,9,5,6,7,8};

  myPArray.setPolyhedronIndex(polyhedronindex);
  myPArray.setFacesIndex(facesindex);
  myPArray.setNodes(nodes);

  POLYHEDRONARRAY myPArray2(myPArray);

  const med_int* __polyhedronindex = myPArray2.getPolyhedronIndex();
  const med_int* __facesindex = myPArray2.getFacesIndex();
  const med_int* __nodes = myPArray2.getNodes();

  cout << "__polyhedronindex =" << endl;
  for (int i=0; i<2; i++)
    cout << __polyhedronindex[i] << endl;
  cout << "__facesindex =" << endl;
  for (int i=0; i<11; i++)
    cout << __facesindex[i] << endl;
  cout << "__nodes =" << endl;
  for (int i=0; i<40; i++)
    cout << __nodes[i] << endl;

  return 0;
}
