/*
 File MEDMEM_CellModel.cxx
 $Header$
*/

#include "utilities.h"
#include "MEDMEM_CellModel.hxx"

CELLMODEL::CELLMODEL(medGeometryElement t)
{
  MESSAGE("CELLMODEL : constructeur pour le type " << t);
  switch (t)
    {
    case MED_POINT1  : {
      _name="MED_POINT1";
      _type=t;
      _dimension=0;
      _numberOfVertexes=1;
      _numberOfNodes=1;
      // nothing else : POINT are none constituent
      break;
    }
    case MED_SEG2    : {
      _name="MED_SEG2" ;
      _type=t;
      _dimension=1;
      _numberOfVertexes=2;
      _numberOfNodes=2;
      // constituent are POINT1 and we have no need to define _constituents vector
      vector<medGeometryElement> vector_type(2,MED_POINT1) ;
      vector<medGeometryElement> __constituents_type[]={vector_type};
      vector< vector<medGeometryElement> > _constituents_type(__constituents_type,__constituents_type+1) ;
      _constituentsType = _constituents_type ;
      break;
    }
    case MED_SEG3    : {
      _name="MED_SEG3" ;
      _type=t;
      _dimension=1;
      _numberOfVertexes=2;
      _numberOfNodes=3;
      // constituent are POINT1 and we have no need to define _constituents vector
      vector<medGeometryElement> vector_type(3,MED_POINT1) ;
      vector<medGeometryElement> __constituents_type[]={vector_type};
      vector< vector<medGeometryElement> > _constituents_type(__constituents_type,__constituents_type+1) ;
      _constituentsType = _constituents_type ;
      break;
    }
    case MED_TRIA3   : {
      _name="MED_TRIA3" ;
      _type=t;
      _dimension=2;
      _numberOfVertexes=3;
      _numberOfNodes=3;
      int _edge1[]={1,2} ;
      int _edge2[]={2,3} ;
      int _edge3[]={3,1} ;
      vector<int> edge1(_edge1,_edge1+2) ;
      vector<int> edge2(_edge2,_edge2+2) ;
      vector<int> edge3(_edge3,_edge3+2) ;
      vector<int> _vector_edge[]={edge1,edge2,edge3};
      vector< vector<int> > vector_edge(_vector_edge,_vector_edge+3) ;
      vector< vector<int> > __constituents__[]={vector_edge};
      vector< vector< vector<int> > > _constituents_(__constituents__,__constituents__+1) ;
      _constituents = _constituents_ ;
      vector<medGeometryElement> vector_edge_type(3,MED_SEG2) ;
      vector<medGeometryElement> vector_type(3,MED_POINT1) ;
      vector<medGeometryElement> __constituents_type[]={vector_edge_type,vector_type};
      vector< vector<medGeometryElement> > _constituents_type(__constituents_type,__constituents_type+2) ;
      _constituentsType = _constituents_type ;
      break;
    }
    case MED_TRIA6   : {
      _name="MED_TRIA6" ;
      _type=t;
      _dimension=2;
      _numberOfVertexes=3;
      _numberOfNodes=6;
      int _edge1[]={1,2,4} ;
      int _edge2[]={2,3,5} ;
      int _edge3[]={3,1,6} ;
      vector<int> edge1(_edge1,_edge1+3) ;
      vector<int> edge2(_edge2,_edge2+3) ;
      vector<int> edge3(_edge3,_edge3+3) ;
      vector<int> _vector_edge[]={edge1,edge2,edge3};
      vector< vector<int> > vector_edge(_vector_edge,_vector_edge+3) ;
      vector< vector<int> > __constituents__[]={vector_edge};
      vector< vector< vector<int> > > _constituents_(__constituents__,__constituents__+1) ;
      _constituents = _constituents_ ;
      vector<medGeometryElement> vector_edge_type(3,MED_SEG3) ;
      vector<medGeometryElement> vector_type(6,MED_POINT1) ;
      vector<medGeometryElement> __constituents_type[]={vector_edge_type,vector_type};
      vector< vector<medGeometryElement> > _constituents_type(__constituents_type,__constituents_type+2) ;
      _constituentsType = _constituents_type ;
      break;
    }
    case MED_QUAD4   : {
      _name="MED_QUAD4" ;
      _type=t;
      _dimension=2;
      _numberOfVertexes=4;
      _numberOfNodes=4;
      int _edge1[]={1,2} ;
      int _edge2[]={2,3} ;
      int _edge3[]={3,4} ;
      int _edge4[]={4,1} ;
      vector<int> edge1(_edge1,_edge1+2) ;
      vector<int> edge2(_edge2,_edge2+2) ;
      vector<int> edge3(_edge3,_edge3+2) ;
      vector<int> edge4(_edge4,_edge4+2) ;
      vector<int> _vector_edge[]={edge1,edge2,edge3,edge4};
      vector< vector<int> > vector_edge(_vector_edge,_vector_edge+4) ;
      vector< vector<int> > __constituents__[]={vector_edge};
      vector< vector< vector<int> > > _constituents_(__constituents__,__constituents__+1) ;
      _constituents = _constituents_ ;
      vector<medGeometryElement> vector_edge_type(4,MED_SEG2) ;
      vector<medGeometryElement> vector_type(4,MED_POINT1) ;
      vector<medGeometryElement> __constituents_type[]={vector_edge_type,vector_type};
      vector< vector<medGeometryElement> > _constituents_type(__constituents_type,__constituents_type+2) ;
      _constituentsType = _constituents_type ;
      break;
    }
    case MED_QUAD8   : {
      _name="MED_QUAD8" ;
      _type=t;
      _dimension=2;
      _numberOfVertexes=4;
      _numberOfNodes=8;
      int _edge1[]={1,2,5} ;
      int _edge2[]={2,3,6} ;
      int _edge3[]={3,4,7} ;
      int _edge4[]={4,1,8} ;
      vector<int> edge1(_edge1,_edge1+3) ;
      vector<int> edge2(_edge2,_edge2+3) ;
      vector<int> edge3(_edge3,_edge3+3) ;
      vector<int> edge4(_edge4,_edge4+3) ;
      vector<int> _vector_edge[]={edge1,edge2,edge3,edge4};
      vector< vector<int> > vector_edge(_vector_edge,_vector_edge+4) ;
      vector< vector<int> > __constituents__[]={vector_edge};
      vector< vector< vector<int> > > _constituents_(__constituents__,__constituents__+1) ;
      _constituents = _constituents_ ;
      vector<medGeometryElement> vector_edge_type(4,MED_SEG3) ;
      vector<medGeometryElement> vector_type(8,MED_POINT1) ;
      vector<medGeometryElement> __constituents_type[]={vector_edge_type,vector_type};
      vector< vector<medGeometryElement> > _constituents_type(__constituents_type,__constituents_type+2) ;
      _constituentsType = _constituents_type ;
      break;
    }
    case MED_TETRA4  : {
      _name="MED_TETRA4" ;
      _type=t;
      _dimension=3;
      _numberOfVertexes=4;
      _numberOfNodes=4;
      int _edge1[]={1,2} ;
      int _edge2[]={2,3} ;
      int _edge3[]={3,1} ;
      int _edge4[]={1,4} ;
      int _edge5[]={2,4} ;
      int _edge6[]={3,4} ;
      vector<int> edge1(_edge1,_edge1+2) ;
      vector<int> edge2(_edge2,_edge2+2) ;
      vector<int> edge3(_edge3,_edge3+2) ;
      vector<int> edge4(_edge4,_edge4+2) ;
      vector<int> edge5(_edge5,_edge5+2) ;
      vector<int> edge6(_edge6,_edge6+2) ;
      vector<int> _vector_edge[]={edge1,edge2,edge3,edge4,edge5,edge6};
      vector< vector<int> > vector_edge(_vector_edge,_vector_edge+6) ;
      int _face1[]={1,2,3} ;
      int _face2[]={1,4,2} ;
      int _face3[]={2,4,3} ;
      int _face4[]={3,4,1} ;
      vector<int> face1(_face1,_face1+3) ;
      vector<int> face2(_face2,_face2+3) ;
      vector<int> face3(_face3,_face3+3) ;
      vector<int> face4(_face4,_face4+3) ;
      vector<int> _vector_face[] = {face1,face2,face3,face4} ;
      vector< vector<int> > vector_face(_vector_face,_vector_face+4) ;
      vector< vector<int> > __constituents__[]={vector_face,vector_edge};
      vector< vector< vector<int> > > _constituents_(__constituents__,__constituents__+2) ;
      _constituents = _constituents_ ;
      vector<medGeometryElement> vector_edge_type(6,MED_SEG2) ;
      vector<medGeometryElement> vector_face_type(4,MED_TRIA3) ;
      vector<medGeometryElement> __constituents_type[]={vector_face_type,vector_edge_type};
      vector< vector<medGeometryElement> > _constituents_type(__constituents_type,__constituents_type+2) ;
      _constituentsType = _constituents_type ;
      break;
    }
    case MED_TETRA10 : {
      _name="MED_TETRA10" ;
      _type=t;
      _dimension=3;
      _numberOfVertexes=4;
      _numberOfNodes=10;
      int _edge1[]={1,2,5} ;
      int _edge2[]={2,3,6} ;
      int _edge3[]={3,1,7} ;
      int _edge4[]={1,4,8} ;
      int _edge5[]={2,4,9} ;
      int _edge6[]={3,4,10} ;
      vector<int> edge1(_edge1,_edge1+3) ;
      vector<int> edge2(_edge2,_edge2+3) ;
      vector<int> edge3(_edge3,_edge3+3) ;
      vector<int> edge4(_edge4,_edge4+3) ;
      vector<int> edge5(_edge5,_edge5+3) ;
      vector<int> edge6(_edge6,_edge6+3) ;
      vector<int> _vector_edge[]={edge1,edge2,edge3,edge4,edge5,edge6};
      vector< vector<int> > vector_edge(_vector_edge,_vector_edge+6) ;
      int _face1[]={1,2,3,5,6,7} ;
      int _face2[]={1,4,2,8,9,5} ;
      int _face3[]={2,4,3,9,10,6} ;
      int _face4[]={3,4,1,10,8,7} ;
      vector<int> face1(_face1,_face1+6) ;
      vector<int> face2(_face2,_face2+6) ;
      vector<int> face3(_face3,_face3+6) ;
      vector<int> face4(_face4,_face4+6) ;
      vector<int> _vector_face[] = {face1,face2,face3,face4} ;
      vector< vector<int> > vector_face(_vector_face,_vector_face+4) ;
      vector< vector<int> > __constituents__[]={vector_face,vector_edge};
      vector< vector< vector<int> > > _constituents_(__constituents__,__constituents__+2) ;
      _constituents = _constituents_ ;
      vector<medGeometryElement> vector_edge_type(6,MED_SEG3) ;
      vector<medGeometryElement> vector_face_type(4,MED_TRIA6) ;
      vector<medGeometryElement> __constituents_type[]={vector_face_type,vector_edge_type};
      vector< vector<medGeometryElement> > _constituents_type(__constituents_type,__constituents_type+2) ;
      _constituentsType = _constituents_type ;
    break;
    }
    case MED_HEXA8   : {
      _name="MED_HEXA8" ;
      _type=t;
      _dimension=3;
      _numberOfVertexes=8;
      _numberOfNodes=8;
      int _edge1[]={1,2} ;
      int _edge2[]={2,3} ;
      int _edge3[]={3,4} ;
      int _edge4[]={4,1} ;
      int _edge5[]={5,6} ;
      int _edge6[]={6,7} ;
      int _edge7[]={7,8} ;
      int _edge8[]={8,5} ;
      int _edge9[]={1,5} ;
      int _edge10[]={2,6} ;
      int _edge11[]={3,7} ;
      int _edge12[]={4,8} ;
      vector<int> edge1(_edge1,_edge1+2) ;
      vector<int> edge2(_edge2,_edge2+2) ;
      vector<int> edge3(_edge3,_edge3+2) ;
      vector<int> edge4(_edge4,_edge4+2) ;
      vector<int> edge5(_edge5,_edge5+2) ;
      vector<int> edge6(_edge6,_edge6+2) ;
      vector<int> edge7(_edge7,_edge7+2) ;
      vector<int> edge8(_edge8,_edge8+2) ;
      vector<int> edge9(_edge9,_edge9+2) ;
      vector<int> edge10(_edge10,_edge10+2) ;
      vector<int> edge11(_edge11,_edge11+2) ;
      vector<int> edge12(_edge12,_edge12+2) ;
      vector<int> _vector_edge[]={edge1,edge2,edge3,edge4,edge5,edge6,edge7,edge8,edge9,edge10,edge11,edge12};
      vector< vector<int> > vector_edge(_vector_edge,_vector_edge+12) ;
      int _face1[]={1,2,3,4} ;
      int _face2[]={5,8,7,6} ;
      int _face3[]={1,5,6,2} ;
      int _face4[]={2,6,7,3} ;
      int _face5[]={3,7,8,4} ;
      int _face6[]={4,8,5,1} ;
      vector<int> face1(_face1,_face1+4) ;
      vector<int> face2(_face2,_face2+4) ;
      vector<int> face3(_face3,_face3+4) ;
      vector<int> face4(_face4,_face4+4) ;
      vector<int> face5(_face5,_face5+4) ;
      vector<int> face6(_face6,_face6+4) ;
      vector<int> _vector_face[] = {face1,face2,face3,face4,face5,face6} ;
      vector< vector<int> > vector_face(_vector_face,_vector_face+6) ;
      vector< vector<int> > __constituents__[]={vector_face,vector_edge};
      vector< vector< vector<int> > > _constituents_(__constituents__,__constituents__+2) ;
      _constituents = _constituents_ ;
      vector<medGeometryElement> vector_edge_type(12,MED_SEG2) ;
      vector<medGeometryElement> vector_face_type(6,MED_QUAD4) ;
      vector<medGeometryElement> __constituents_type[]={vector_face_type,vector_edge_type};
      vector< vector<medGeometryElement> > _constituents_type(__constituents_type,__constituents_type+2);
      _constituentsType = _constituents_type ;
      break;
    }
    case MED_HEXA20  : {
      _name="MED_HEXA20" ;
      _type=t;
      _dimension=3;
      _numberOfVertexes=8;
      _numberOfNodes=20;

      int _edge1[]={1,2,9} ;
      int _edge2[]={2,3,10} ;
      int _edge3[]={3,4,11} ;
      int _edge4[]={4,1,12} ;
      int _edge5[]={5,6,13} ;
      int _edge6[]={6,7,14};
      int _edge7[]={7,8,15} ; 
      int _edge8[]={8,5,16} ;
      int _edge9[]={1,5,17} ;
      int _edge10[]={2,6,18} ;
      int _edge11[]={3,7,19} ;
      int _edge12[]={4,8,20} ;

      vector<int> edge1(_edge1,_edge1+3) ;
      vector<int> edge2(_edge2,_edge2+3) ;
      vector<int> edge3(_edge3,_edge3+3) ;
      vector<int> edge4(_edge4,_edge4+3) ;
      vector<int> edge5(_edge5,_edge5+3) ;
      vector<int> edge6(_edge6,_edge6+3) ;
      vector<int> edge7(_edge7,_edge7+3) ;
      vector<int> edge8(_edge8,_edge8+3) ;
      vector<int> edge9(_edge9,_edge9+3) ;
      vector<int> edge10(_edge10,_edge10+3) ;
      vector<int> edge11(_edge11,_edge11+3) ;
      vector<int> edge12(_edge12,_edge12+3) ;       
      vector<int> _vector_edge[]={edge1,edge2,edge3,edge4,edge5,edge6,edge7,edge8,edge9,edge10, edge11,edge12};
      vector< vector<int> > vector_edge(_vector_edge,_vector_edge+12) ;

      int _face1[]={1,2,3,4,9,10,11,12} ;
      int _face2[]={5,8,7,6,16,15,14,13} ;
      int _face3[]={1,5,6,2,17,13,18,9} ;
      int _face4[]={2,6,7,3,18,14,19,10} ;
      int _face5[]={3,7,8,4,19,15,20,11} ;
      int _face6[]={4,8,5,1,20,16,17,12} ;
      vector<int> face1(_face1,_face1+8);
      vector<int> face2(_face2,_face2+8);
      vector<int> face3(_face3,_face3+8);
      vector<int> face4(_face4,_face4+8);
      vector<int> face5(_face5,_face5+8);
      vector<int> face6(_face6,_face6+8);
      vector<int> _vector_face[]= {face1,face2,face3,face4,face5,face6} ;
      vector< vector<int> > vector_face(_vector_face,_vector_face+6);

      vector< vector<int> > __constituents__[]={vector_face,vector_edge};
      vector< vector< vector<int> > > _constituents_(__constituents__,__constituents__+2) ; 
      _constituents = _constituents_ ;
      vector<medGeometryElement> vector_edge_type(12,MED_SEG3) ;
      vector<medGeometryElement> vector_face_type(6,MED_QUAD8) ;
      vector<medGeometryElement> __constituents_type[]={vector_face_type,vector_edge_type};
      vector< vector<medGeometryElement> > _constituents_type(__constituents_type,__constituents_type+2) ;
      _constituentsType = _constituents_type ;
      break;
    }
    case MED_PENTA6  : {
      _name="MED_PENTA6" ;
      _type=t;
      _dimension=3;
      _numberOfVertexes=6;
      _numberOfNodes=6;

      int _edge1[]= {1,2} ;
      int _edge2[]= {2,3} ;
      int _edge3[]= {3,1} ;
      int _edge4[]= {4,5} ;
      int _edge5[]= {5,6} ;
      int _edge6[]= {6,4} ;
      int _edge7[]= {1,4} ;
      int _edge8[]= {2,5} ;
      int _edge9[]= {3,6} ;

      vector<int> edge1(_edge1,_edge1+2) ;
      vector<int> edge2(_edge2,_edge2+2) ;
      vector<int> edge3(_edge3,_edge3+2) ;
      vector<int> edge4(_edge4,_edge4+2) ;
      vector<int> edge5(_edge5,_edge5+2) ;
      vector<int> edge6(_edge6,_edge6+2) ;
      vector<int> edge7(_edge7,_edge7+2) ;
      vector<int> edge8(_edge8,_edge8+2) ;
      vector<int> edge9(_edge9,_edge9+2) ;    
      vector<int> _vector_edge[]={edge1,edge2,edge3,edge4,edge5,edge6,edge7,edge8,edge9};
      vector< vector<int> > vector_edge(_vector_edge,_vector_edge+9) ;

      int _face1[]={1,2,3};
      int _face2[]={4,6,5};
      int _face3[]={1,4,5,2};
      int _face4[]={2,5,6,3};
      int _face5[]={3,6,4,1};
      vector<int> face1(_face1,_face1+3);
      vector<int> face2(_face2,_face2+3);
      vector<int> face3(_face3,_face3+4);
      vector<int> face4(_face4,_face4+4);
      vector<int> face5(_face5,_face5+4);
      vector<int> _vector_face[]= {face1,face2,face3,face4,face5} ;
      vector< vector<int> > vector_face(_vector_face,_vector_face+5);
 
      vector< vector<int> > __constituents__[]={vector_face,vector_edge};
      vector< vector< vector<int> > > _constituents_(__constituents__,__constituents__+2) ;
      _constituents = _constituents_ ;
      vector<medGeometryElement> vector_edge_type(9,MED_SEG2) ;
      vector<medGeometryElement> vector_face_type(5,MED_QUAD4) ;
      vector_face_type[0]=MED_TRIA3 ;
      vector_face_type[1]=MED_TRIA3 ;
      vector<medGeometryElement> __constituents_type[]={vector_face_type,vector_edge_type};
      vector< vector<medGeometryElement> > _constituents_type(__constituents_type,__constituents_type+2) ;
      _constituentsType = _constituents_type ;
      break;
    }
    case MED_PENTA15 : {
      _name="MED_PENTA15" ;
      _type=t;
      _dimension=3;
      _numberOfVertexes=6;
      _numberOfNodes=15;
      int _edge1[]={1,2,7} ;
      int _edge2[]={2,3,8} ;
      int _edge3[]={3,1,9} ;
      int _edge4[]={4,5,10} ;
      int _edge5[]={5,6,11} ;
      int _edge6[]={6,4,12} ;
      int _edge7[]={1,4,13} ;
      int _edge8[]={2,5,14} ;
      int _edge9[]={3,6,15} ;

      vector<int> edge1(_edge1,_edge1+3) ;
      vector<int> edge2(_edge2,_edge2+3) ;
      vector<int> edge3(_edge3,_edge3+3) ;
      vector<int> edge4(_edge4,_edge4+3) ;
      vector<int> edge5(_edge5,_edge5+3) ;
      vector<int> edge6(_edge6,_edge6+3) ;
      vector<int> edge7(_edge7,_edge7+3) ;
      vector<int> edge8(_edge8,_edge8+3) ;
      vector<int> edge9(_edge9,_edge9+3) ;    
     vector<int> _vector_edge[]={edge1,edge2,edge3,edge4,edge5,edge6,edge7,edge8,edge9};
      vector< vector<int> > vector_edge(_vector_edge,_vector_edge+9) ;

      int _face1[]={1,2,3,7,8,9};
      int _face2[]={4,6,5,12,11,10};
      int _face3[]={1,4,5,2,13,10,14,7};
      int _face4[]={2,5,6,3,14,11,15,8};
      int _face5[]={3,6,4,1,15,12,13,9};
      vector<int> face1(_face1,_face1+6);
      vector<int> face2(_face2,_face2+6);
      vector<int> face3(_face3,_face3+8);
      vector<int> face4(_face4,_face4+8);
      vector<int> face5(_face5,_face5+8);
      vector<int> _vector_face[]= {face1,face2,face3,face4,face5} ;
      vector< vector<int> > vector_face(_vector_face,_vector_face+5);

      vector< vector<int> > __constituents__[]={vector_face,vector_edge};
      vector< vector< vector<int> > > _constituents_(__constituents__,__constituents__+2) ;
      _constituents = _constituents_ ;
      vector<medGeometryElement> vector_edge_type(9,MED_SEG3) ;
      vector<medGeometryElement> vector_face_type(5,MED_QUAD8) ;
      vector_face_type[0]=MED_TRIA6 ;
      vector_face_type[1]=MED_TRIA6 ;
      vector<medGeometryElement> __constituents_type[]={vector_face_type,vector_edge_type};
      vector< vector<medGeometryElement> > _constituents_type(__constituents_type,__constituents_type+2) ;
      _constituentsType = _constituents_type ;
      break;
    }
    case MED_PYRA5   : {
      _name="MED_PYRA5" ;
      _type=t;
      _dimension=3;
      _numberOfVertexes=5;
      _numberOfNodes=5;
      int _edge1[]={1,2} ;
      int _edge2[]={2,3} ;
      int _edge3[]={3,4} ;
      int _edge4[]={4,1} ;
      int _edge5[]={1,5} ;
      int _edge6[]={2,5} ;
      int _edge7[]={3,5} ;
      int _edge8[]={4,5} ;

      vector<int> edge1(_edge1,_edge1+2) ;
      vector<int> edge2(_edge2,_edge2+2) ;
      vector<int> edge3(_edge3,_edge3+2) ;
      vector<int> edge4(_edge4,_edge4+2) ;
      vector<int> edge5(_edge5,_edge5+2) ;
      vector<int> edge6(_edge6,_edge6+2) ;
      vector<int> edge7(_edge7,_edge7+2) ;
      vector<int> edge8(_edge8,_edge8+2) ;
     vector<int> _vector_edge[]={edge1,edge2,edge3,edge4,edge5,edge6,edge7,edge8};
      vector< vector<int> > vector_edge(_vector_edge,_vector_edge+8) ;

      int _face1[]={1,2,3,4} ;
      int _face2[]={1,5,2} ;
      int _face3[]={2,5,3} ;
      int _face4[]={3,5,4} ;
      int _face5[]={4,5,1} ;
      vector<int> face1(_face1,_face1+4);
      vector<int> face2(_face2,_face2+3);
      vector<int> face3(_face3,_face3+3);
      vector<int> face4(_face4,_face4+3);
      vector<int> face5(_face5,_face5+3);
      vector<int> _vector_face[]= {face1,face2,face3,face4,face5} ;
      vector< vector<int> > vector_face(_vector_face,_vector_face+5);

      vector< vector<int> > __constituents__[]={vector_face,vector_edge};
      vector< vector< vector<int> > > _constituents_(__constituents__,__constituents__+2) ;
      _constituents = _constituents_ ;
      vector<medGeometryElement> vector_edge_type(8,MED_SEG2) ;
      vector<medGeometryElement> vector_face_type(5,MED_TRIA3) ;
      vector_face_type[0]=MED_QUAD4 ;
    vector<medGeometryElement> __constituents_type[]={vector_face_type,vector_edge_type};
      vector< vector<medGeometryElement> > _constituents_type(__constituents_type,__constituents_type+2) ;
      _constituentsType = _constituents_type ;
      break;
    }
    case MED_PYRA13  : {
      _name="MED_PYRA13" ;
      _type=t;
      _dimension=3;
      _numberOfVertexes=5;
      _numberOfNodes=13;
      int _edge1[]={1,2,6} ;
      int _edge2[]={2,3,7} ;
      int _edge3[]={3,4,8} ;
      int _edge4[]={4,1,9} ;
      int _edge5[]={1,5,10} ;
      int _edge6[]={2,5,11} ;
      int _edge7[]={3,5,12} ;
      int _edge8[]={4,5,13} ;

      vector<int> edge1(_edge1,_edge1+3) ;
      vector<int> edge2(_edge2,_edge2+3) ;
      vector<int> edge3(_edge3,_edge3+3) ;
      vector<int> edge4(_edge4,_edge4+3) ;
      vector<int> edge5(_edge5,_edge5+3) ;
      vector<int> edge6(_edge6,_edge6+3) ;
      vector<int> edge7(_edge7,_edge7+3) ;
      vector<int> edge8(_edge8,_edge8+3) ;


      vector<int> _vector_edge[]={edge1,edge2,edge3,edge4,edge5,edge6,edge7,edge8};
      vector< vector<int> > vector_edge(_vector_edge,_vector_edge+8) ;

      int _face1[]={1,2,3,4,6,7,8,9} ;
      int _face2[]={1,5,2,10,11,6} ;
      int _face3[]={2,5,3,11,12,7} ;
      int _face4[]={3,5,4,12,13,8} ;
      int _face5[]={4,5,1,13,10,9} ;
      vector<int> face1(_face1,_face1+8);
      vector<int> face2(_face2,_face2+6);
      vector<int> face3(_face3,_face3+6);
      vector<int> face4(_face4,_face4+6);
      vector<int> face5(_face5,_face5+6);
      vector<int> _vector_face[]= {face1,face2,face3,face4,face5} ;
      vector< vector<int> > vector_face(_vector_face,_vector_face+5);


      vector< vector<int> > __constituents__[]={vector_face,vector_edge};
      vector< vector< vector<int> > > _constituents_(__constituents__,__constituents__+2) ;
      _constituents = _constituents_ ;
      vector<medGeometryElement> vector_edge_type(8,MED_SEG3) ;
      vector<medGeometryElement> vector_face_type(5,MED_TRIA6) ;
      vector_face_type[0]=MED_QUAD8 ;
     vector<medGeometryElement> __constituents_type[]={vector_face_type,vector_edge_type};
      vector< vector<medGeometryElement> > _constituents_type(__constituents_type,__constituents_type+2) ;

      _constituentsType = _constituents_type ;
      break;
    }
//      default : 
//        _type=0;
//        break;
    }
}

ostream & operator<<(ostream &os,const CELLMODEL &my)
{
  os << "Cell Model :" << endl ;
  os << "  - name               : " << my._name << endl;
  os << "  - type               : " << (int)my._type << endl;
  os << "  - dimension          : " << my._dimension << endl;
  os << "  - number of nodes    : " << my._numberOfNodes << endl ;
  os << "  - number of vertexes : " << my._numberOfVertexes << endl ;
  for(int i=0;i<my._constituents.size();i++)
    {
      os << "  - for dimension (-" << i+1 << ") : " << endl ;
      for(int j=0;j<my._constituents[i].size();j++)
	{
	  os << "    - constituents " << j+1 << " of type "<< my._constituentsType[i][j] <<" : " ;
	  for(int k=0;k<my._constituents[i][j].size();k++)
	    os << my._constituents[i][j][k] << " " ;
	  os << endl ;
	}
    }
  return os;
}



  // Return number of constituents type (which dimension is _dimension-1).
int CELLMODEL::getNumberOfConstituentsType() const
{
  set<medGeometryElement> constituentsTypes ;
  if (_constituentsType.size() > 0 ) {
    vector<medGeometryElement> constituentsType = _constituentsType[0] ;
    vector<medGeometryElement>::iterator itvec ;
    for(itvec=constituentsType.begin();itvec!=constituentsType.end();itvec++)
      constituentsTypes.insert(*itvec);
  }
  return constituentsTypes.size() ;
}

  // Return all types of constituents which dimension is (_dimension-1).
set <medGeometryElement>  CELLMODEL::getAllConstituentsType() const
{
  set<medGeometryElement> constituentsTypes ;
  if (_constituentsType.size() > 0 ) {
    vector<medGeometryElement> constituentsType = _constituentsType[0] ;
    vector<medGeometryElement>::iterator itvec ;
    for(itvec=constituentsType.begin();itvec!=constituentsType.end();itvec++)
      constituentsTypes.insert(*itvec);
  }
  return constituentsTypes ;
}

  // Return number of constituents foreach type (which dimension is _dimension-1).
map <medGeometryElement,int>  CELLMODEL::getNumberOfConstituentsForeachType() const
{
  map<medGeometryElement,int> numberOfConstituentsForeachType ;
  map<medGeometryElement,int>::iterator itmap ;
  if (_constituentsType.size() > 0 ) {
    vector<medGeometryElement> constituentsType = _constituentsType[0] ;
    vector<medGeometryElement>::iterator itvec ;
    for(itvec=constituentsType.begin();itvec!=constituentsType.end();itvec++) {
      itmap = numberOfConstituentsForeachType.find(*itvec);
      if (itmap==numberOfConstituentsForeachType.end()) // new element
	numberOfConstituentsForeachType[*itvec]=1 ;
      else
	numberOfConstituentsForeachType[*itvec]++ ;
    }
  }
  return numberOfConstituentsForeachType ;
}
