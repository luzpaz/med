using namespace std;
/*
 File Group.cxx
 $Header$
*/
#include <list>

#include "MEDMEM_Group.hxx"
#include "MEDMEM_Family.hxx"
using namespace MEDMEM;

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

ostream & MEDMEM::operator<<(ostream &os, GROUP &myGroup)
{
  os << (SUPPORT) myGroup;

  int numberoffamilies = myGroup.getNumberOfFamilies();
  os << "  - Families ("<<numberoffamilies<<") :"<<endl;
  for (int j=1;j<numberoffamilies+1;j++)
    os << "    * "<<myGroup.getFamily(j)->getName().c_str()<<endl ;

  return os;
};

GROUP::GROUP(const string & name, const list<FAMILY*> & families) throw (MEDEXCEPTION)
{
  const char * LOC = "GROUP( const string & , const list<FAMILY*> & ) : " ;
  
  BEGIN_OF(LOC);

  MESSAGE(LOC<<name);

  int numberOfFamilies = families.size();
  _name = name ;
  _description = "GROUP";
  // description : none !
  // first FAMILY to set all !
  FAMILY * myFamily = families.front() ;
  _mesh =  myFamily->getMesh() ;
  _entity = myFamily->getEntity() ;
  bool isOnAllElts = myFamily->isOnAllElements() ;

  SCRUTE(isOnAllElts);
  SCRUTE(numberOfFamilies);


  if ((numberOfFamilies==1) && (isOnAllElts))
    {
      _numberOfFamilies = numberOfFamilies;
      _isOnAllElts = isOnAllElts ;
      _family.resize(_numberOfFamilies) ;
      _family[0] = myFamily;
      update();
      return;
    }
  else if ((!(numberOfFamilies==1)) && (isOnAllElts))
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "building of a GROUP object from several FAMILY, and one of them is on all entities"  )) ;

  _numberOfGeometricType = myFamily->getNumberOfTypes() ;
  _geometricType = new medGeometryElement[_numberOfGeometricType];
  //_geometricTypeNumber = new int[_numberOfGeometricType] ;
  _numberOfGaussPoint = new int[_numberOfGeometricType] ;
  _numberOfElements = new int[_numberOfGeometricType] ;
  const medGeometryElement * geometricType = myFamily->getTypes() ;
  //int * geometricTypeNumber = myFamily->getGeometricTypeNumber() ;
  const int * numberOfGaussPoint = myFamily->getNumberOfGaussPoint() ;

  SCRUTE(_numberOfGeometricType);

  for (int i=0 ; i<_numberOfGeometricType; i++) {
    _geometricType[i]= geometricType[i] ;
    // _geometricTypeNumber[i] = geometricTypeNumber[i] ;
    _numberOfGaussPoint[i] = numberOfGaussPoint[i] ;
    _numberOfElements[i]=myFamily->getNumberOfElements(geometricType[i]);
    MESSAGE(LOC << " Type : " << _geometricType[i] << " number of element(s) " << _numberOfElements[i]);
  }
  _isOnAllElts = false ;
  //_totalNumberOfEntities = myFamily->getNumberOfElements(MED_ALL_ELEMENTS) ;


  MEDSKYLINEARRAY * famNumber = myFamily->getnumber();

  const int * famNumberValue = myFamily->getNumber(MED_ALL_ELEMENTS);

  const int * famNumberIndex = myFamily->getNumberIndex();

  int famNumberCount = famNumber->getNumberOf();
  int famNumberLength = famNumber->getLength();

  SCRUTE(famNumber);
  SCRUTE(famNumberCount);
  SCRUTE(famNumberLength);
  SCRUTE(famNumberValue);
  SCRUTE(famNumberIndex);

//   _number = new MEDSKYLINEARRAY(*famNumber) ;
  _number = new MEDSKYLINEARRAY(famNumberCount,famNumberLength,
				famNumberIndex,famNumberValue) ;

  SCRUTE(_number);

  _numberOfFamilies = families.size();

  SCRUTE(numberOfFamilies);

  //SCRUTE(_numberOfFamilies);

  _family.resize(_numberOfFamilies) ;
  list<FAMILY*>::const_iterator li ;

  // MESSAGE(LOC<<"Printing of the object GROUP built right before the blending"<< (SUPPORT) *this);


  int it = 0 ;
  for (li=families.begin();li!=families.end();li++) {
    blending(*li);
    _family[it] = (*li) ;
    it++ ;
  }

  //MESSAGE(LOC<<"Printing of the object GROUP built "<< (GROUP)*this);

  END_OF(LOC);
};

GROUP::GROUP(const GROUP & m):SUPPORT(m)
{
  _numberOfFamilies = m._numberOfFamilies;
  _family = m._family;
};

// void GROUP::init(const list<FAMILY*> & families)
// {
//   const char * LOC = "GROUP::init( const list<FAMILY*> & ) : " ;
  
//   BEGIN_OF(LOC);
  
//   FAMILY * myFamily = families.front() ;
//   _mesh =  myFamily->getMesh() ;

//   _isOnAllElts = myFamily->isOnAllElements() ;

//   SCRUTE(_mesh);

//   SCRUTE(_isOnAllElts);

//   _entity = myFamily->getEntity() ;

//   SCRUTE(_mesh->getNumberOfTypes(_entity));

//   _numberOfGeometricType = myFamily->getNumberOfTypes() ;
//   _geometricType = new medGeometryElement[_numberOfGeometricType];
//   //_geometricTypeNumber = new int[_numberOfGeometricType] ;
//   _numberOfGaussPoint = new int[_numberOfGeometricType] ;
//   _numberOfElements = new int[_numberOfGeometricType] ;
//   medGeometryElement * geometricType = myFamily->getTypes() ;
//   //int * geometricTypeNumber = myFamily->getGeometricTypeNumber() ;
//   int * numberOfGaussPoint = myFamily->getNumberOfGaussPoint() ;
//   for (int i=0 ; i<_numberOfGeometricType; i++) {
//     _geometricType[i]= geometricType[i] ;
//     // _geometricTypeNumber[i] = geometricTypeNumber[i] ;
//     _numberOfGaussPoint[i] = numberOfGaussPoint[i] ;
//     _numberOfElements[i]=myFamily->getNumberOfElements(geometricType[i]);
//   }
//   _isOnAllElts = false ;
//   _totalNumberOfEntities = myFamily->getNumberOfElements(MED_ALL_ELEMENTS) ;
//   _number = new MEDSKYLINEARRAY(*myFamily->getnumber()) ;
  
//   _numberOfFamilies = families.size();
//   _family.resize(_numberOfFamilies) ;
//   list<FAMILY*>::const_iterator liIni = families.begin() ;
//   _family[0]=(*liIni);
//   liIni++;
//   list<FAMILY*>::const_iterator li ;
//   int it = 1 ;
//   for (li=liIni;li!=families.end();li++) {
//     blending(*li);
//     _family[it] = (*li) ;
//     it++ ;
//   }
  
//   END_OF(LOC);
// };

