#include "MEDMEM_DriversDef.hxx"

using namespace std;

namespace MED_EN {

// Returns the (string) name of the geometry of 
// an element given by a med_geometrie_element value
GEO_NAME::GEO_NAME() : map<int,string>()
{
  map<int,string> &table  = (map<int,string>&)*this ;
  table[(int)0 ] = "MED_NONE" ;
  table[(int)MED_POINT1 ] = "MED_POINT1" ;
  table[(int)MED_SEG2   ] = "MED_SEG2"   ;
  table[(int)MED_SEG3   ] = "MED_SEG3"   ;
  table[(int)MED_TRIA3  ] = "MED_TRIA3"  ;
  table[(int)MED_TRIA6  ] = "MED_TRIA6"  ;
  table[(int)MED_QUAD4  ] = "MED_QUAD4"  ;
  table[(int)MED_QUAD8  ] = "MED_QUAD8"  ;
  table[(int)MED_TETRA4 ] = "MED_TETRA4" ;
  table[(int)MED_TETRA10] = "MED_TETRA10";
  table[(int)MED_HEXA8  ] = "MED_HEXA8"  ;
  table[(int)MED_HEXA20 ] = "MED_HEXA20" ;
  table[(int)MED_PENTA6 ] = "MED_PENTA6" ;
  table[(int)MED_PENTA15] = "MED_PENTA15";
  table[(int)MED_PYRA5  ] = "MED_PYRA5"  ;
  table[(int)MED_PYRA13 ] = "MED_PYRA13" ;
  assert( table.find( (int)MED_PYRA13 ) != table.end() ) ;
}
 
GEO_NAME::~GEO_NAME()
{
//   MESSAGE(" destructor GEO_NAME::~GEO_NAME() "<< size() );
//   clear();
//   MESSAGE(" end of destructor GEO_NAME::~GEO_NAME() "<< size() );
}
 
string & GEO_NAME::operator[]( const MED_EN::medGeometryElement &c ) const
{
  map<int,string> &table = (map<int,string>&)*this ;
  assert( table.find( (int)c ) != table.end() ) ;
  return table[ (int)c ] ;
}


// Returns the (string) name of the type of 
// an entity given by a med_entite_maillage value

ENT_NAME::ENT_NAME() : map<int,string>()
{
  map<int,string> &table = (map<int,string>&)*this ;
  table[(int)MED_CELL ] = "MED_MAILLE";
  table[(int)MED_FACE   ] = "MED_FACE"; 
  table[(int)MED_EDGE  ] = "MED_ARETE";
  table[(int)MED_NODE  ] = "MED_NOEUD";
}

ENT_NAME::~ENT_NAME()
{
//   MESSAGE(" destructor ENT_NAME::~ENT_NAME() "<< size() );
//   clear();
//   MESSAGE(" end of destructor ENT_NAME::~ENT_NAME() "<< size() );
}

string & ENT_NAME::operator[]( const MED_EN::medEntityMesh &c ) const
{
  map<int,string> &table = (map<int,string>&)*this ;
  assert( table.find( (int)c ) != table.end() ) ;
  return table[ (int)c ] ;
}

MESH_ENTITIES::MESH_ENTITIES () : map<int, const list<MED_EN::medGeometryElement> >() 
{
  // Override the const declaration in order to used the find method below
  //  map<int, const list<med_geometrie_element> > &table = (map<int, const list<med_geometrie_element> >&)*this ; unused local variable
  
  // Initialize the value associated with the ROUGE_ key ( use the private operator = )
  {
    const MED_EN::medGeometryElement T[] =  {
      MED_POINT1,
      MED_SEG2,
      MED_SEG3,
      MED_TRIA3,
      MED_QUAD4,
      MED_TRIA6,
      MED_QUAD8,
      MED_TETRA4,
      MED_PYRA5,
      MED_PENTA6,
      MED_HEXA8,
      MED_TETRA10,
      MED_PYRA13,
      MED_PENTA15,
      MED_HEXA20};

    static const list<MED_EN::medGeometryElement> geomList(T,T+sizeof(T)/sizeof(MED_EN::medGeometryElement));   
    (*this)[MED_CELL] = geomList;
  }
  
  {
    const MED_EN::medGeometryElement T[] =  {
      MED_TRIA3,
      MED_QUAD4,
      MED_TRIA6,
      MED_QUAD8};

    static const list<MED_EN::medGeometryElement> geomList(T,T+sizeof(T)/sizeof(MED_EN::medGeometryElement));   
    (*this)[MED_FACE] = geomList; 
  }
  
  {
    const MED_EN::medGeometryElement T[] =  {
      MED_SEG2,
      MED_SEG3 };

    static const list<MED_EN::medGeometryElement> geomList(T,T+sizeof(T)/sizeof(MED_EN::medGeometryElement));   
    (*this)[MED_EDGE] = geomList; 
  }
  
  {
    //    const med_geometrie_element T[] =  { MED_NONE };
    const MED_EN::medGeometryElement T[] =  { (MED_EN::medGeometryElement)0 };
    static const list<MED_EN::medGeometryElement> geomList(T,T+sizeof(T)/sizeof(MED_EN::medGeometryElement));   
    (*this)[MED_NODE] = geomList; 
  }
  
}

MESH_ENTITIES::~MESH_ENTITIES()
{
//   MESSAGE(" destructor MESH_ENTITIES::~MESH_ENTITIES() "<< size() );
//   clear();
//   MESSAGE(" end of destructor MESH_ENTITIES::~MESH_ENTITIES() "<< size() );
}

const list<MED_EN::medGeometryElement> & MESH_ENTITIES::operator[]( const  MED_EN::medEntityMesh &c ) const
{
  map<int,const list<MED_EN::medGeometryElement> > &table = (map<int,const list<MED_EN::medGeometryElement> >&)*this ;
  // Verify the object already exists in the map
  assert( table.find( (int)c ) != table.end() ) ;
  return table[ (int)c ] ;
}

// This operator is used to initialize class objects of type  const list<med_geometrie_element>
list<MED_EN::medGeometryElement> & MESH_ENTITIES::operator[]( const  MED_EN::medEntityMesh &c )
{
  return (list<MED_EN::medGeometryElement> &) this->map<int,const list<MED_EN::medGeometryElement> >::operator[]( (int)c) ;
}

} // End Of NampeSpace MED_EN

