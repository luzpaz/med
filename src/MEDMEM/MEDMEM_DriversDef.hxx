# ifndef DRIVERS_DEF_HXX
# define DRIVERS_DEF_HXX

// STL
# include <string>
# include <list>
# include <map>

using namespace std ;

#include "MEDMEM_define.hxx"
#include "utilities.h"
namespace MED_FR {

/*!  This Class inherits from  map. \n
     It is a constant map association int (which is a med_geometrie_element) and string. \n
     Operator [] returns the (string) name of the geometry of an element 
     given by a med_geometrie_element value. \n 
     Such a static map is instancied and can be used in any code to have
     the name of the geometry of an element : MED_FR:GEO_NAME 
     A simple test/use case can be found in test/testUGeoNameMeshEntities.cxx  */
//----------------------------------------
class GEO_NAME : public map<int,string>
//----------------------------------------
{
  
public :
  
  GEO_NAME();
  ~GEO_NAME();
  string & operator[]( const MED_FR::med_geometrie_element &c ) const;
} ;

static const GEO_NAME geoNames ;

//				____________________________	
  
/*! This Class inherits from map. \n
    It is a constant map association int (which is a med_entite_maillage) and string. \n
    Operator[] returns the (string) name of the type of an entity given by 
    a med_entite_maillage value. \n
    Such a static map is instancied and can be used in any code to have
    the name of the geometry of an element : MED_FR:ENT_NAME 
    A simple test/use case can be found in test/testUGeoNameMeshEntities.cxx  */

//----------------------------------------
class ENT_NAME : public map<int,string>
//----------------------------------------
{
  
public :

  ENT_NAME ();
  ~ENT_NAME();
  string & operator[]( const MED_FR::med_entite_maillage &c ) const;
} ;

static const ENT_NAME entNames ;

//				____________________________	

/*! This Class inherits from map. \n
    It is a constant map association int (which is a med_entite_maillage) and  a list. \n
    Operator[] returns the list of all exisiting med_geometrie_element for 
    a med_entite_maillage value. \n
    Such a static map is instancied and can be used in any code to have
    the name of the geometry of an element : MED_FR:MESH_ENTITIES
    A simple test/use case can be found in test/testUGeoNameMeshEntities.cxx  */


//--------------------------------------------------------------------------------
class MESH_ENTITIES : public map<int, const list<MED_FR::med_geometrie_element> >
//--------------------------------------------------------------------------------
{

public :

  MESH_ENTITIES ();
  ~MESH_ENTITIES();
  const list<MED_FR::med_geometrie_element> & operator[]( const  MED_FR::med_entite_maillage &c ) const;
  
private:
  					/*! This operator is used to initialize class objects of 
					    type  const list<med_geometrie_element> */
  list<MED_FR::med_geometrie_element> & operator[]( const  MED_FR::med_entite_maillage &c );
  
} ;

static const MESH_ENTITIES meshEntities ;

} // End Of NameSpace MED_FR

#endif

