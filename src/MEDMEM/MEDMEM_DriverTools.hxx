#ifndef DRIVERTOOLS_HXX
#define DRIVERTOOLS_HXX


#include "MEDMEM_define.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Field.hxx"
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
struct _noeud
{
    mutable int number;
    std::vector<double> coord;
};

typedef pair<int,int> _link; // a pair of node numbers

struct _maille
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

struct _mailleIteratorCompare // pour ordonner le set d'iterateurs sur mailles
{
    bool operator () (std::set<_maille>::iterator i1, std::set<_maille>::iterator i2)
    {
	return *i1<*i2;
    }
};

struct _groupe
{
    typedef std::vector< std::set<_maille>::iterator>::const_iterator mailleIter;
    std::string nom;
    std::vector< std::set<_maille>::iterator > mailles; // iterateurs sur les mailles composant le groupe
//     std::set< std::set<_maille>::iterator, _mailleIteratorCompare > mailles; // iterateurs sur les mailles composant le groupe
    std::vector<int> groupes; // indices des sous-groupes composant le groupe
    std::map<const _maille*,int> relocMap; // map _maille* -> index in MEDMEM::GROUP.getNumber(MED_ALL_ELEMENTS). It is built by _intermediateMED::getGroups()
    bool empty() const { return mailles.empty() && groupes.empty(); }
};

class _fieldBase {
 public:
  int                      nb_subcomponents;
  int                      nb_components;
  int                      group_id;
  MED_EN::med_type_champ   type;
  string                   name;
  std::vector<std::string> comp_names;
  _fieldBase( MED_EN::med_type_champ theType )
    : nb_subcomponents(0), nb_components(0),group_id(-1),type(theType) {}
  virtual std::list<FIELD_*> getField(std::vector<_groupe>& groupes) const = 0;
  virtual void dump(std::ostream&) const;
  virtual ~_fieldBase() {}
};

template< class T > class _field: public _fieldBase
{
  std::vector< std::vector< T > > comp_values;
  std::vector< int >              comp_supp_ids;
 public:
  _field< T > ( MED_EN::med_type_champ theType ): _fieldBase( theType ) {}
  std::vector< T >& addComponent( int support_id, int nb_values );
  std::list<FIELD_*> getField(std::vector<_groupe>& groupes) const;
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
struct _intermediateMED
{
    std::set<_maille> maillage;
    std::vector<_groupe> groupes;
    std::vector<GROUP *> medGroupes;
    std::map< int, _noeud > points;
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

};

template <class T>
  std::vector< T >& MEDMEM::_field< T >::addComponent( int support_id, int nb_values )
{
  if ( comp_supp_ids.empty() ) {
    comp_supp_ids.reserve( nb_subcomponents * nb_components );
    comp_values.reserve( nb_subcomponents * nb_components );
  }
  comp_supp_ids.push_back( support_id );
  comp_values.push_back( std::vector< T >() );
  std::vector< T >& res = comp_values.back();
  res.resize( nb_values );
  return res;
}


template <class T> 
std::list<MEDMEM::FIELD_*> MEDMEM::_field< T >::getField(std::vector<_groupe> & groupes) const
{
  std::list<FIELD_*> res;
  int i, j, i_comp, last_comp = 0;
  while ( last_comp < comp_values.size() )
  {
    // find the first array of the next field - last_comp
    int first_comp = last_comp;
    std::set<int> id_set;
    for ( i = first_comp + nb_components - 1; i < comp_values.size(); i += nb_components )
      if ( id_set.insert( comp_supp_ids[ i ] ).second ) // unique support
        last_comp = i + 1;

    _groupe& grp = groupes[ group_id ];
    if ( grp.empty () )
      continue;
    int nb_val = grp.relocMap.size();
    ASSERT ( nb_val );

    //create a filed
    FIELD< T > * f = new FIELD< T >;
    f->setNumberOfComponents( nb_components );
    f->setComponentsNames( &comp_names[0] );
    f->setNumberOfValues ( nb_val );
    f->setName( name );
    f->setValueType( type );
    vector<string> str( nb_components );
    f->setComponentsDescriptions( &str[0] );
    f->setMEDComponentsUnits( &str[0] );
    res.push_back( f );

    MESSAGE(" make field <" << name << "> on group_id " << group_id <<
            " relocMap.size: " << nb_val << " nb mailles: " << grp.mailles.size());
    // set values
    MEDARRAY< T > * medarray = new MEDARRAY< T >( nb_components, nb_val, MED_NO_INTERLACE );
    f->setValue( medarray );
    for ( j = 1; j <= nb_components; ++j ) {
      // loop on arrays of j-th component
      for ( i_comp = first_comp++; i_comp < last_comp; i_comp += nb_components )
      {
        // get nb elements in a group
        int gr_id = comp_supp_ids[ i_comp ] - 1;
        if ( gr_id < 0 || gr_id >= groupes.size() )
          throw MEDEXCEPTION
            (LOCALIZED(STRING("_field< T >::getField(), invalid group id: ")
                              << gr_id << ", nb groups: " << groupes.size()));
        _groupe & sub_grp = groupes[ gr_id ];
        int nb_supp_elems = sub_grp.mailles.size();
        MESSAGE(" comp_group_id " << gr_id << " nb values " << comp_values[ i_comp ].size() <<
                " relocMap.size: " << sub_grp.relocMap.size() << " nb mailles: " << nb_supp_elems);

        // store values
        const std::vector< T > & values = comp_values[ i_comp ];
        bool oneValue = ( values.size() == 1 );
        ASSERT( oneValue || values.size() == nb_supp_elems );
        for ( int k = 0; k < nb_supp_elems; ++k )
        {
          const T& val = oneValue ? values[ 0 ] : values[ k ];
          const _maille* ma = &(*sub_grp.mailles[ k ]);
          std::map<const _maille*,int>::const_iterator ma_i = grp.relocMap.find( ma );
          if ( ma_i == grp.relocMap.end() )
            throw MEDEXCEPTION
              (LOCALIZED(STRING("_field< T >::getField(), cant find elem index. ")
                         << k << "-th elem: " << ma));
          if ( ma_i->second > nb_val )
            throw MEDEXCEPTION
              (LOCALIZED(STRING("_field< T >::getField(), wrong elem position. ")
                         << k << "-th elem: " << ma
                         << ", pos (" << ma_i->second << ") must be <= " << nb_val));
          medarray->setIJ( ma_i->second, j, val );
        }
      }
    }
  }
  return res;
}


template <class T> void MEDMEM::_field< T >::dump(std::ostream& os) const
{
  _fieldBase::dump(os);
  os << endl;
  for ( int i = 0 ; i < comp_values.size(); ++i )
  {
    os << "  group: " << setw(3) << comp_supp_ids[ i ] <<
      "  nb values: " << comp_values[ i ].size() << endl;
  }
}
#endif /* DRIVERTOOLS_HXX */
