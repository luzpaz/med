using namespace std;
#include "MEDMEM_PolyhedronArray.hxx"
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
