// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#ifndef DRIVERTOOLS_HXX
#define DRIVERTOOLS_HXX

#include <MEDMEM.hxx>

#include "MEDMEM_define.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_FieldForward.hxx"
#include "MEDMEM_ArrayInterface.hxx"
#include <string>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <iostream>
#include <iomanip>

namespace MEDMEM {
class MESH;
class CONNECTIVITY;
class COORDINATE;
class GROUP;
class FAMILY;
class FIELD_;
struct MEDMEM_EXPORT _noeud
{
    mutable int number;
    std::vector<double> coord;
};

typedef pair<int,int> _link; // a pair of node numbers

struct MEDMEM_EXPORT _maille
{
    typedef std::map<int,_noeud>::iterator iter;
    MED_EN::medGeometryElement geometricType;
    std::vector< iter > sommets;
    mutable unsigned ordre; // l'ordre est fixé après insertion dans le set, et ne change ni l'état, ni l'ordre -> mutable
    mutable bool reverse; // to reverse sommets of a face
    mutable list<unsigned> groupes; // the GROUPs maille belongs to, used to create families

    _maille() : geometricType(MED_EN::MED_NONE),ordre(0),reverse(false)
    {
    };
    _maille(MED_EN::medGeometryElement _geometricType, size_t nelem) : geometricType(_geometricType),ordre(0),reverse(false)
    {
	sommets.reserve(nelem);
    };
    int dimension() const // retourne la dimension de la maille
    {
	return geometricType/100;
    };
    bool operator < (const _maille& ma) const;
    MED_EN::medEntityMesh getEntity(const int meshDimension) const throw (MEDEXCEPTION);
   _link link(int i) const;
};

struct MEDMEM_EXPORT _mailleIteratorCompare // pour ordonner le set d'iterateurs sur mailles
{
    bool operator () (std::set<_maille>::iterator i1, std::set<_maille>::iterator i2)
    {
	return *i1<*i2;
    }
};

struct MEDMEM_EXPORT _groupe
{
    typedef std::vector< std::set<_maille>::iterator>::const_iterator mailleIter;
    std::string nom;
    std::vector< std::set<_maille>::iterator > mailles; // iterateurs sur les mailles composant le groupe
    std::vector<int> groupes; // indices des sous-groupes composant le groupe
    std::map<const _maille*,int> relocMap; // map _maille* -> index in MEDMEM::GROUP.getNumber(MED_ALL_ELEMENTS). It is built by _intermediateMED::getGroups()
    bool empty() const { return mailles.empty() && groupes.empty(); }
};

struct MEDMEM_EXPORT _fieldBase {
  // a field contains several subcomponents each referring to its own support and
  // having several named components
  struct _sub_data {
    int                      _supp_id;    // group index within _intermediateMED::groupes
    std::vector<std::string> _comp_names; // componenet names
    void setData( int nb_comp, int supp_id )
    { _supp_id = supp_id - 1; _comp_names.resize(nb_comp); }
    int nbComponents() const { return _comp_names.size(); }
    std::string & compName( int i_comp ) { return _comp_names[ i_comp ]; }
  };
  std::vector< _sub_data > _sub;
  int                      _group_id; // group index within _intermediateMED::groupes
  // if _group_id == -1 then each subcomponent makes a separate MEDMEM::FIELD, else all subcomponents
  // are converted into one MEDMEM::FIELD. The latter is possible only if nb of components in all subs
  // is the same and supports of subcomponents do not overlap
  MED_EN::med_type_champ   _type;
  string                   _name;

  _fieldBase( MED_EN::med_type_champ theType, int nb_sub )
    : _group_id(-1),_type(theType) { _sub.resize( nb_sub ); }
  virtual std::list<std::pair< FIELD_*, int> > getField(std::vector<_groupe>& groupes) const = 0;
  void getGroupIds( std::set<int> & ids, bool all ) const; // return ids of main and/or sub-groups
  bool hasCommonSupport() const { return _group_id >= 0; } // true if there is one support for all subs
  bool hasSameComponentsBySupport() const;
  virtual void dump(std::ostream&) const;
  virtual ~_fieldBase() {}
};

template< class T > class _field: public _fieldBase
{
  std::vector< std::vector< T > > comp_values;
 public:
  _field< T > ( MED_EN::med_type_champ theType, int nb_sub, int total_nb_comp )
    : _fieldBase( theType, nb_sub ) { comp_values.reserve( total_nb_comp ); }
  std::vector< T >& addComponent( int nb_values ); // return a vector ready to fill in
  std::list<std::pair< FIELD_*, int> > getField(std::vector<_groupe>& groupes) const;
  virtual void dump(std::ostream&) const;
};

/*!
 * \if developper
 * Intermediate structure used by drivers to store data read from the other format file.
 * The structure provides functions that transform the stored data to the MED format : 
 * getCoordinate(), getConnectivity(), getGroups().
 * The elements inserted in maillage and points are automaticaly ordered.
 * Renumbering are performed by getConnectivity & getGroups before writing the MED structures.
 * Read the conception ducumentation for more details.
 * \endif
 */
struct MEDMEM_EXPORT _intermediateMED
{
    std::set<_maille>        maillage;
    std::vector<_groupe>     groupes;
    std::vector<GROUP *>     medGroupes;
    std::map< int, _noeud >  points;
    std::list< _fieldBase* > fields;
    bool hasMixedCells; // true if there are groups with mixed entity types

    CONNECTIVITY * getConnectivity(); // set MED connectivity from the intermediate structure
    COORDINATE * getCoordinate(const string & coordinateSystem="CARTESIAN"); // set MED coordinate from the 
                                                                             // intermediate structure
    void getFamilies(std::vector<FAMILY *> & _famCell, std::vector<FAMILY *> & _famFace, 
                     std::vector<FAMILY *> & _famEdge, std::vector<FAMILY *> & _famNode, MESH * _ptrMesh);
    void getGroups(std::vector<GROUP *> & _groupCell, std::vector<GROUP *> & _groupFace, 
	    std::vector<GROUP *> & _groupEdge, std::vector<GROUP *> & _groupNode, MESH * _ptrMesh);
    GROUP * getGroup( int i );

    void getFields(std::list< FIELD_* >& fields);

    // used by previous functions to renumber points & mesh.
    void treatGroupes(); // detect groupes of mixed dimention
    void numerotationMaillage(); 
    void numerotationPoints();

    ~_intermediateMED();
};

std::ostream& operator << (std::ostream& , const _maille& );
std::ostream& operator << (std::ostream& , const _groupe& );
std::ostream& operator << (std::ostream& , const _noeud& );
std::ostream& operator << (std::ostream& , const _intermediateMED& );
std::ostream& operator << (std::ostream& , const _fieldBase* );

// ===========================================================
//                 field template implementation           //
// ===========================================================

template <class T>
  std::vector< T >& _field< T >::addComponent( int nb_values )
{
  comp_values.push_back( std::vector< T >() );
  std::vector< T >& res = comp_values.back();
  res.resize( nb_values );
  return res;
}

//=======================================================================
//function : getField
//purpose  : return list of pairs< field, supporting_group_id >
//=======================================================================

template <class T>
std::list<std::pair< FIELD_*, int> > _field< T >::getField(std::vector<_groupe> & groupes) const
{
  std::list<std::pair< FIELD_*, int> > res;

  _groupe* grp = 0;
  if ( hasCommonSupport() ) { // several subs are combined into one field
    grp = & groupes[ _group_id ];
    if ( !grp || grp->empty() )
      return res;
  }
  FIELD< T, NoInterlace > * f = 0;
  int i_comp_tot = 0, nb_fields = 0;
  std::set<int> supp_id_set;
  // loop on subs of this field
  std::vector< _sub_data >::const_iterator sub_data = _sub.begin();
  for ( int i_sub = 1; sub_data != _sub.end(); ++sub_data, ++i_sub )
  {
    // nb values in a field
    if ( !hasCommonSupport() )
      grp = & groupes[ sub_data->_supp_id ];
    int nb_val = grp->relocMap.size();
    if ( !nb_val ) {
      INFOS("Skip field <" << _name << ">: invalid supporting group "
            << (hasCommonSupport() ? _group_id : sub_data->_supp_id )
            << " of " << i_sub << "-th subcomponent" );
      return res;
    }

    //create a field
    if ( !f || !hasCommonSupport() || !supp_id_set.insert( sub_data->_supp_id ).second )
    {
      supp_id_set.clear();
      ++nb_fields;
      f = new FIELD< T, NoInterlace >();
      f->setNumberOfComponents( sub_data->nbComponents() );
      f->setComponentsNames( & sub_data->_comp_names[ 0 ] );
      f->setNumberOfValues ( nb_val );
      f->setName( _name );
      //      f->setValueType( _type );
      vector<string> str( sub_data->nbComponents() );
      f->setComponentsDescriptions( &str[0] );
      f->setMEDComponentsUnits( &str[0] );
      res.push_back( make_pair( f , hasCommonSupport() ? _group_id : sub_data->_supp_id ));
      MESSAGE(" MAKE " << nb_fields << "-th field <" << _name << "> on group_id " << _group_id );
    }
    // set values
    MEDMEM_Array< T, NoInterlaceNoGaussPolicy > * medarray =
      new MEDMEM_Array < T, NoInterlaceNoGaussPolicy >( sub_data->nbComponents(),
						       nb_val );
    f->setArray( medarray );
    // loop on components of a sub
    for ( int i_comp = 0; i_comp < sub_data->nbComponents(); ++i_comp )
    {
      // get nb elements in a group
      _groupe & sub_grp = groupes[ sub_data->_supp_id ];
      int nb_supp_elems = sub_grp.mailles.size();
      MESSAGE("insert sub data, group_id: " << sub_data->_supp_id <<
              ", nb values: " << comp_values[ i_comp_tot ].size() <<
              ", relocMap.size: " << sub_grp.relocMap.size() <<
              ", nb mailles: " << nb_supp_elems);

      // store values
      const std::vector< T > & values = comp_values[ i_comp_tot++ ];
      bool oneValue = ( values.size() == 1 );
      ASSERT( oneValue || values.size() == nb_supp_elems );
      for ( int k = 0; k < nb_supp_elems; ++k )
      {
        const T& val = oneValue ? values[ 0 ] : values[ k ];
        const _maille* ma = &(*sub_grp.mailles[ k ]);
        std::map<const _maille*,int>::const_iterator ma_i = grp->relocMap.find( ma );
        if ( ma_i == grp->relocMap.end() )
          throw MEDEXCEPTION
            (LOCALIZED(STRING("_field< T >::getField(), cant find elem index. ")
                       << k << "-th elem: " << ma));
        if ( ma_i->second > nb_val )
          throw MEDEXCEPTION
            (LOCALIZED(STRING("_field< T >::getField(), wrong elem position. ")
                       << k << "-th elem: " << ma
                       << ", pos (" << ma_i->second << ") must be <= " << nb_val));
        medarray->setIJ( ma_i->second, i_comp + 1, val );
      }
    }
  }
  return res;
}


template <class T> void _field< T >::dump(std::ostream& os) const
{
  _fieldBase::dump(os);
  os << endl;
  for ( int i = 0 ; i < (int)comp_values.size(); ++i )
  {
    os << "    " << i+1 << "-th component, nb values: " << comp_values[ i ].size() << endl;
  }
}

}; // namespace MEDMEM

#endif /* DRIVERTOOLS_HXX */
