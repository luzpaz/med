# ifndef DRIVERS_DEF_HXX
# define DRIVERS_DEF_HXX

// STL
# include <string>
# include <list>
# include <map>

using namespace std ;

#include "MEDMEM_define.hxx"
namespace MED_FR {

// Returns the (string) name of the geometry of 
// an element given by a med_geometrie_element value
class GEO_NAME : public map<int,string>
{
  
public :
  
  GEO_NAME ();
  
  string & operator[]( const MED_FR::med_geometrie_element &c ) const;
} ;


static const GEO_NAME geoNames ;
  
// Returns the (string) name of the type of 
// an entity given by a med_entite_maillage value
class ENT_NAME : public map<int,string>
{
  
public :
  ENT_NAME ();
  string & operator[]( const MED_FR::med_entite_maillage &c ) const;
} ;

static const ENT_NAME entNames ;

class MESH_ENTITIES : public map<int, const list<MED_FR::med_geometrie_element> >
{

public :

  MESH_ENTITIES ();
  const list<MED_FR::med_geometrie_element> & operator[]( const  MED_FR::med_entite_maillage &c ) const;
  
private:
  // This operator is used to initialize class objects of type  const list<med_geometrie_element>
  list<MED_FR::med_geometrie_element> & operator[]( const  MED_FR::med_entite_maillage &c );
  
} ;

static const MESH_ENTITIES meshEntities ;

} // End Of NameSpace MED_FR

#endif

