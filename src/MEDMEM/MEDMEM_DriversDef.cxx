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

#include "MEDMEM_DriversDef.hxx"

using namespace std;

namespace MED_EN {

// Returns the (string) name of the geometry of 
// an element given by a med_geometrie_element value
GEO_NAME::GEO_NAME() : map<int,string>()
{
  map<int,string> &table  = (map<int,string>&)*this ;
  table[(int)0 ] = "MEDMEM_NONE" ;
  table[(int)MEDMEM_POINT1 ] = "MEDMEM_POINT1" ;
  table[(int)MEDMEM_SEG2   ] = "MEDMEM_SEG2"   ;
  table[(int)MEDMEM_SEG3   ] = "MEDMEM_SEG3"   ;
  table[(int)MEDMEM_TRIA3  ] = "MEDMEM_TRIA3"  ;
  table[(int)MEDMEM_TRIA6  ] = "MEDMEM_TRIA6"  ;
  table[(int)MEDMEM_QUAD4  ] = "MEDMEM_QUAD4"  ;
  table[(int)MEDMEM_QUAD8  ] = "MEDMEM_QUAD8"  ;
  table[(int)MEDMEM_TETRA4 ] = "MEDMEM_TETRA4" ;
  table[(int)MEDMEM_TETRA10] = "MEDMEM_TETRA10";
  table[(int)MEDMEM_HEXA8  ] = "MEDMEM_HEXA8"  ;
  table[(int)MEDMEM_HEXA20 ] = "MEDMEM_HEXA20" ;
  table[(int)MEDMEM_PENTA6 ] = "MEDMEM_PENTA6" ;
  table[(int)MEDMEM_PENTA15] = "MEDMEM_PENTA15";
  table[(int)MEDMEM_PYRA5  ] = "MEDMEM_PYRA5"  ;
  table[(int)MEDMEM_PYRA13 ] = "MEDMEM_PYRA13" ;
  table[(int)MEDMEM_POLYGON] = "MEDMEM_POLYGON";
  table[(int)MEDMEM_POLYHEDRA] = "MEDMEM_POLYHEDRA";
  assert( table.find( (int)MEDMEM_POLYHEDRA ) != table.end() ) ;
}
 
GEO_NAME::~GEO_NAME()
{
//   MESSAGE_MED(" destructor GEO_NAME::~GEO_NAME() "<< size() );
//   clear();
//   MESSAGE_MED(" end of destructor GEO_NAME::~GEO_NAME() "<< size() );
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
//   MESSAGE_MED(" destructor ENT_NAME::~ENT_NAME() "<< size() );
//   clear();
//   MESSAGE_MED(" end of destructor ENT_NAME::~ENT_NAME() "<< size() );
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
      MEDMEM_POINT1,
      MEDMEM_SEG2,
      MEDMEM_SEG3,
      MEDMEM_TRIA3,
      MEDMEM_QUAD4,
      MEDMEM_TRIA6,
      MEDMEM_QUAD8,
      MEDMEM_TETRA4,
      MEDMEM_PYRA5,
      MEDMEM_PENTA6,
      MEDMEM_HEXA8,
      MEDMEM_TETRA10,
      MEDMEM_PYRA13,
      MEDMEM_PENTA15,
      MEDMEM_HEXA20,
      MEDMEM_POLYGON,
      MEDMEM_POLYHEDRA};

    list<MED_EN::medGeometryElement> geomList(T,T+sizeof(T)/sizeof(MED_EN::medGeometryElement));   
    (*this)[MED_CELL] = geomList;
  }
  
  {
    const MED_EN::medGeometryElement T[] =  {
      MEDMEM_TRIA3,
      MEDMEM_QUAD4,
      MEDMEM_TRIA6,
      MEDMEM_QUAD8,
      MEDMEM_POLYGON};

    list<MED_EN::medGeometryElement> geomList(T,T+sizeof(T)/sizeof(MED_EN::medGeometryElement));   
    (*this)[MED_FACE] = geomList; 
  }
  
  {
    const MED_EN::medGeometryElement T[] =  {
      MEDMEM_SEG2,
      MEDMEM_SEG3 };

    list<MED_EN::medGeometryElement> geomList(T,T+sizeof(T)/sizeof(MED_EN::medGeometryElement));   
    (*this)[MED_EDGE] = geomList; 
  }
  
  {
    //    const med_geometrie_element T[] =  { MEDMEM_NONE };
    const MED_EN::medGeometryElement T[] =  { (MED_EN::medGeometryElement)0 };
    list<MED_EN::medGeometryElement> geomList(T,T+sizeof(T)/sizeof(MED_EN::medGeometryElement));   
    (*this)[MED_NODE] = geomList; 
  }
  
}

MESH_ENTITIES::~MESH_ENTITIES()
{
//   MESSAGE_MED(" destructor MESH_ENTITIES::~MESH_ENTITIES() "<< size() );
//   clear();
//   MESSAGE_MED(" end of destructor MESH_ENTITIES::~MESH_ENTITIES() "<< size() );
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

