using namespace std;
#include "MEDMEM_PolyhedronArray.hxx"
using namespace MEDMEM;

POLYHEDRONARRAY::POLYHEDRONARRAY():_numberOfPolyhedron(0), _numberOfFaces(0), _numberOfNodes(0), _polyhedronIndex((med_int*)NULL), _facesIndex((med_int*)NULL), _nodes((med_int*)NULL)
{}

POLYHEDRONARRAY::POLYHEDRONARRAY(med_int numberofpolyhedron,med_int numberoffaces,med_int numberofnodes):_numberOfPolyhedron(numberofpolyhedron), _numberOfFaces(numberoffaces), _numberOfNodes(numberofnodes), _polyhedronIndex(numberofpolyhedron), _facesIndex(numberoffaces), _nodes(numberofnodes)
{}
