//  MED MEDMEM : MED files in memory
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : MEDMEM_DriversDef.cxx
//  Module : MED

using namespace std;
#include "MEDMEM_DriversDef.hxx"

namespace MED_FR 
{

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
  
string & GEO_NAME::operator[]( const med_geometrie_element &c ) const
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
  table[(int)MED_MAILLE ] = "MED_MAILLE";
  table[(int)MED_FACE   ] = "MED_FACE"; 
  table[(int)MED_ARETE  ] = "MED_ARETE";
  table[(int)MED_NOEUD  ] = "MED_NOEUD";
}
string & ENT_NAME::operator[]( const med_entite_maillage &c ) const
{
  map<int,string> &table = (map<int,string>&)*this ;
  assert( table.find( (int)c ) != table.end() ) ;
  return table[ (int)c ] ;
}

MESH_ENTITIES::MESH_ENTITIES () : map<int, const list<med_geometrie_element> >() 
{
  // Override the const declaration in order to used the find method below
  map<int, const list<med_geometrie_element> > &table = (map<int, const list<med_geometrie_element> >&)*this ;
  
  // Initialize the value associated with the ROUGE_ key ( use the private operator = )
  {
    const med_geometrie_element T[] =  {
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

    static const list<med_geometrie_element> geomList(T,T+sizeof(T)/sizeof(med_geometrie_element));   
    (*this)[MED_MAILLE] = geomList;
  }
  
  {
    const med_geometrie_element T[] =  {
      MED_TRIA3,
      MED_QUAD4,
      MED_TRIA6,
      MED_QUAD8};

    static const list<med_geometrie_element> geomList(T,T+sizeof(T)/sizeof(med_geometrie_element));   
    (*this)[MED_FACE] = geomList; 
  }
  
  {
    const med_geometrie_element T[] =  {
      MED_SEG2,
      MED_SEG3 };

    static const list<med_geometrie_element> geomList(T,T+sizeof(T)/sizeof(med_geometrie_element));   
    (*this)[MED_ARETE] = geomList; 
  }
  
  {
    //    const med_geometrie_element T[] =  { MED_NONE };
    const med_geometrie_element T[] =  { (med_geometrie_element)0 };
    static const list<med_geometrie_element> geomList(T,T+sizeof(T)/sizeof(med_geometrie_element));   
    (*this)[MED_NOEUD] = geomList; 
  }
  
}

const list<med_geometrie_element> & MESH_ENTITIES::operator[]( const  med_entite_maillage &c ) const
{
  map<int,const list<med_geometrie_element> > &table = (map<int,const list<med_geometrie_element> >&)*this ;
  // Verify the object already exists in the map
  assert( table.find( (int)c ) != table.end() ) ;
  return table[ (int)c ] ;
}

// This operator is used to initialize class objects of type  const list<med_geometrie_element>
list<med_geometrie_element> & MESH_ENTITIES::operator[]( const  med_entite_maillage &c )
{
  return (list<med_geometrie_element> &) this->map<int,const list<med_geometrie_element> >::operator[]( (int)c) ;
}

} // End Of NameSpace MED_FR
