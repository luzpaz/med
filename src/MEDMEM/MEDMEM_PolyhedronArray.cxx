//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
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
#include "MEDMEM_PolyhedronArray.hxx"

using namespace std;
using namespace MEDMEM;
using MED_EN::med_int;

POLYHEDRONARRAY::POLYHEDRONARRAY():_numberOfPolyhedron(0), _numberOfFaces(0), _numberOfNodes(0), _polyhedronIndex((med_int*)NULL), _facesIndex((med_int*)NULL), _nodes((med_int*)NULL)
{}

POLYHEDRONARRAY::POLYHEDRONARRAY(med_int numberofpolyhedron,med_int numberoffaces,med_int numberofnodes):_numberOfPolyhedron(numberofpolyhedron), _numberOfFaces(numberoffaces), _numberOfNodes(numberofnodes), _polyhedronIndex(numberofpolyhedron+1), _facesIndex(numberoffaces+1), _nodes(numberofnodes)
{}

POLYHEDRONARRAY::POLYHEDRONARRAY(const POLYHEDRONARRAY& m):_numberOfPolyhedron(m._numberOfPolyhedron), _numberOfFaces(m._numberOfFaces), _numberOfNodes(m._numberOfNodes), _polyhedronIndex(m._numberOfPolyhedron+1), _facesIndex(m._numberOfFaces+1), _nodes(m._numberOfNodes)
{
  _polyhedronIndex.set(m._numberOfPolyhedron+1, m.getPolyhedronIndex());
  _facesIndex.set(m._numberOfFaces+1, m.getFacesIndex());
  _nodes.set(m._numberOfNodes, m.getNodes());
}
