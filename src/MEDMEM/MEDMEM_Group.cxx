/*
 File Group.cxx
 $Header$
*/
#include <list>

#include "MEDMEM_Group.hxx"
#include "MEDMEM_Family.hxx"

GROUP::GROUP():_numberOfFamilies(0),_family() 
{
  MESSAGE("GROUP()");
};

GROUP::~GROUP() 
{
  MESSAGE("~GROUP()");
};
  
GROUP & GROUP::operator=(const GROUP &group) 
{
  MESSAGE("GROUP::operator=");
  _numberOfFamilies = group._numberOfFamilies ;
  _family      	    = group._family ;
  return *this;
};

GROUP::GROUP(const string & name, const list<FAMILY*> & families)
{
  const char * LOC = "GROUP( const string & , const list<FAMILY*> & ) : " ;
  
  BEGIN_OF(LOC);

  MESSAGE(LOC<<name);
  _name = name ;
  // description : none !
  // first FAMILY to set all !
  FAMILY * myFamily = families.front() ;
  _mesh =  myFamily->getMesh() ;
  _entity = myFamily->getEntity() ;
  _numberOfGeometricType = myFamily->getNumberOfTypes() ;
  _geometricType = new medGeometryElement[_numberOfGeometricType];
  //_geometricTypeNumber = new int[_numberOfGeometricType] ;
  _numberOfGaussPoint = new int[_numberOfGeometricType] ;
  _numberOfEntities = new int[_numberOfGeometricType] ;
  medGeometryElement * geometricType = myFamily->getTypes() ;
  //int * geometricTypeNumber = myFamily->getGeometricTypeNumber() ;
  int * numberOfGaussPoint = myFamily->getNumberOfGaussPoint() ;
  for (int i=0 ; i<_numberOfGeometricType; i++) {
    _geometricType[i]= geometricType[i] ;
    // _geometricTypeNumber[i] = geometricTypeNumber[i] ;
    _numberOfGaussPoint[i] = numberOfGaussPoint[i] ;
    _numberOfEntities[i]=myFamily->getNumberOfElements(geometricType[i]);
  }
  _isOnAllElts = false ;
  _totalNumberOfEntities = myFamily->getNumberOfElements(MED_ALL_ELEMENTS) ;
  _number = new MEDSKYLINEARRAY(*myFamily->getnumber()) ;

  _numberOfFamilies = families.size();
  _family.resize(_numberOfFamilies) ;
  list<FAMILY*>::const_iterator li ;
  int it = 0 ;
  for (li=families.begin();li!=families.end();li++) {
    blending(*li);
    _family[it] = (*li) ;
    it++ ;
  }

  END_OF(LOC);
};

void GROUP::init(const list<FAMILY*> & families)
{
  const char * LOC = "GROUP::init( const list<FAMILY*> & ) : " ;
  
  BEGIN_OF(LOC);
  
  FAMILY * myFamily = families.front() ;
  _mesh =  myFamily->getMesh() ;
  _entity = myFamily->getEntity() ;
  _numberOfGeometricType = myFamily->getNumberOfTypes() ;
  _geometricType = new medGeometryElement[_numberOfGeometricType];
  //_geometricTypeNumber = new int[_numberOfGeometricType] ;
  _numberOfGaussPoint = new int[_numberOfGeometricType] ;
  _numberOfEntities = new int[_numberOfGeometricType] ;
  medGeometryElement * geometricType = myFamily->getTypes() ;
  //int * geometricTypeNumber = myFamily->getGeometricTypeNumber() ;
  int * numberOfGaussPoint = myFamily->getNumberOfGaussPoint() ;
  for (int i=0 ; i<_numberOfGeometricType; i++) {
    _geometricType[i]= geometricType[i] ;
    // _geometricTypeNumber[i] = geometricTypeNumber[i] ;
    _numberOfGaussPoint[i] = numberOfGaussPoint[i] ;
    _numberOfEntities[i]=myFamily->getNumberOfElements(geometricType[i]);
  }
  _isOnAllElts = false ;
  _totalNumberOfEntities = myFamily->getNumberOfElements(MED_ALL_ELEMENTS) ;
  _number = new MEDSKYLINEARRAY(*myFamily->getnumber()) ;
  
  _numberOfFamilies = families.size();
  _family.resize(_numberOfFamilies) ;
  list<FAMILY*>::const_iterator liIni = families.begin() ;
  _family[0]=(*liIni);
  liIni++;
  list<FAMILY*>::const_iterator li ;
  int it = 1 ;
  for (li=liIni;li!=families.end();li++) {
    blending(*li);
    _family[it] = (*li) ;
    it++ ;
  }
  
  END_OF(LOC);
};

