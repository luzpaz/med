using namespace std;
/*
 File Support.cxx
 $Header$
*/

#include <set>
#include <list>

#include "utilities.h"
#include "MEDMEM_define.hxx"
#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Mesh.hxx"

using namespace MED_EN;

/* This class is a generic class for family and group */
//--------------------------------------------------------------------------
SUPPORT::SUPPORT(): _name(""),	_description(""), _mesh((MESH*)NULL),
		    _entity(MED_CELL), _numberOfGeometricType(0),
	    	    _geometricType((medGeometryElement*)NULL),
		    _numberOfGaussPoint((int*)NULL),
		    _geometricTypeNumber((int*)NULL), _isOnAllElts(false),
		    _numberOfEntities((int*)NULL), _totalNumberOfEntities(0),
		    _number((MEDSKYLINEARRAY*)NULL)
//--------------------------------------------------------------------------
{
    MESSAGE("Constructeur SUPPORT sans parametre");
}; 

//--------------------------------------------------------------------------
SUPPORT::SUPPORT(MESH* Mesh, string Name/*=""*/, medEntityMesh Entity/*=MED_CELL*/):
		_name(Name), _description(""), _mesh(Mesh), _entity(Entity),
		_numberOfGeometricType(0),
		_geometricType((medGeometryElement*)NULL),
		_numberOfGaussPoint((int*)NULL),
		_geometricTypeNumber((int*)NULL),
		_isOnAllElts(true), _numberOfEntities((int*)NULL),
		_totalNumberOfEntities(0),
		_number((MEDSKYLINEARRAY*)NULL)
//--------------------------------------------------------------------------
{
  MESSAGE("Constructeur SUPPORT avec parametres MESH* Mesh,string name,medEntityMesh Entity");
  if ( Entity == MED_NODE) {
    _numberOfGeometricType=1 ;
    _geometricType=new medGeometryElement[1] ;
    _geometricType[0]=MED_NONE ;
  }
};

//--------------------------------------------------------------------------
SUPPORT::SUPPORT(SUPPORT & m)
//--------------------------------------------------------------------------
{
  const char * LOC = "SUPPORT::SUPPORT(SUPPORT & m) : " ;
  BEGIN_OF(LOC) ;

  _name = m._name;
  _description = m._description;
  _mesh = m._mesh; // on recopie uniquement l'adresse
  _entity = m._entity;
  _numberOfGeometricType = m._numberOfGeometricType;
  if (m._geometricType != NULL)
    {
      _geometricType = new medGeometryElement[m._numberOfGeometricType];
      memcpy(_geometricType,m._geometricType,m._numberOfGeometricType*sizeof(medGeometryElement));
    }
  else
    _geometricType = (medGeometryElement *) NULL;
  if (m._numberOfGaussPoint != NULL)
    {
      _numberOfGaussPoint = new int[m._numberOfGeometricType];
      memcpy(_numberOfGaussPoint,m._numberOfGaussPoint,m._numberOfGeometricType*sizeof(int));
    }
  else
    _numberOfGaussPoint = (int *) NULL;
  if (m._geometricTypeNumber != NULL)
    {
      _geometricTypeNumber = new int[m._numberOfGeometricType];
      memcpy(_geometricTypeNumber,m._geometricTypeNumber,m._numberOfGeometricType*sizeof(int));
    }
  else
    _geometricTypeNumber = (int *) NULL;
  _isOnAllElts = m._isOnAllElts;
  if (m._numberOfEntities != NULL)
    {
      _numberOfEntities = new int[m._numberOfGeometricType];
      memcpy(_numberOfEntities,m._numberOfEntities,m._numberOfGeometricType*sizeof(int));
    }
  else
    _numberOfEntities = (int *) NULL;
  _totalNumberOfEntities = m._totalNumberOfEntities;
  if (m._isOnAllElts == false)
    _number = new MEDSKYLINEARRAY(* m._number);
  else
    _number = (MEDSKYLINEARRAY *) NULL;

  END_OF(LOC) ;
};

//-----------------
SUPPORT::~SUPPORT() 
//-----------------
{
    MESSAGE("Destructeur ~SUPPORT()");
    if (_geometricType!=NULL) 
      		delete[] _geometricType ;
    if (_numberOfGaussPoint!=NULL) 
      		delete[] _numberOfGaussPoint ;
    if (_geometricTypeNumber!=NULL) 
      		delete[] _geometricTypeNumber ;
    if (_numberOfEntities!=NULL) 
      		delete[] _numberOfEntities ;
    if (_number!=NULL) 
      		delete _number ;
}

//--------------------------------------------------
ostream & operator<<(ostream &os, const SUPPORT &my)
//--------------------------------------------------
{
  os << "Name : "<<my._name<< endl ;
  os << "Description : "<< my._description << endl ;
  os << "Mesh name : ";
  if (my._mesh==NULL)
    os << "ERROR : Mesh not defined !" << endl ;
  else
    os << my._mesh->getName() << endl ;
  os << "Entity : "<< my._entity << endl;
  os << "Entity list : "<< endl;
  if (!(my._isOnAllElts)) {
    int NumberOfTypes = my._numberOfGeometricType ;
    os << "NumberOfTypes : "<<NumberOfTypes<<endl;
    medGeometryElement * Types = my._geometricType ;
    for (int j=0;j<NumberOfTypes;j++) {
      os << "    * Type "<<Types[j]<<" : ";
      int NumberOfElements = my._numberOfEntities[j] ;
      int * Number = my._number->getI(j+1) ;
      for (int k=0; k<NumberOfElements;k++)
	os << Number[k] << " ";
      os << endl ;
    }
  } else
    os << "Is on all entities !"<< endl;

  return os ;
}

/*! A DOCUMENTER */
//-------------------
void SUPPORT::update()
//-------------------
{
  const char * LOC = "SUPPORT::update() : " ;
  BEGIN_OF(LOC) ;

  if (_isOnAllElts)
    if (_numberOfGaussPoint==NULL) {
      MESSAGE( "ENTITY : "<< _entity );
      int NumberOfTypes = _mesh->getNumberOfTypes(_entity) ;
      MESSAGE( "NumberOfTypes : "<< NumberOfTypes );
      _numberOfGaussPoint = new int[NumberOfTypes] ;
      for (int i=0;i<NumberOfTypes;i++)
	_numberOfGaussPoint[i]=1 ;
    }
  if (_entity == MED_NODE)
    _totalNumberOfEntities = _mesh->getNumberOfNodes();

  END_OF(LOC);
};

/*! A DOCUMENTER */
//-------------------
void SUPPORT::blending(SUPPORT * mySupport) 
//-------------------
{
  const char * LOC = "SUPPORT::blending() : " ;
  BEGIN_OF(LOC) ;

  MESSAGE(LOC<< "SUPPORT entry : " << *mySupport) ;

  // on same entity :
  if ( _entity != mySupport->getEntity() )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Entities are different !"));

  int * tmp_NumberOfElementsInType = new int[MED_NBR_GEOMETRIE_MAILLE];
  medGeometryElement * myType = new medGeometryElement[MED_NBR_GEOMETRIE_MAILLE];
  int * whereIsType = new int[MED_NBR_GEOMETRIE_MAILLE];
  //MESH_ENTITIES myMeshEntities() ;
  list<MED_FR::med_geometrie_element>::const_iterator listIt ;
  int it=0 ;
  for(listIt=(MED_FR::meshEntities[(MED_FR::med_entite_maillage)_entity]).begin();listIt!=(MED_FR::meshEntities[(MED_FR::med_entite_maillage)_entity]).end();listIt++){
    tmp_NumberOfElementsInType[it]=0;
    whereIsType[it]=0 ;
    try {
      tmp_NumberOfElementsInType[it]+=getNumberOfElements((medGeometryElement)(*listIt)) ;
      whereIsType[it]+=1 ;
    }
    catch (const MEDEXCEPTION & ex) {};
    try {
      tmp_NumberOfElementsInType[it]+=mySupport->getNumberOfElements((medGeometryElement)(*listIt)) ;
      whereIsType[it]+=2 ;
    }
    catch (const MEDEXCEPTION & ex) {};
    if (whereIsType[it]!=0) {
      myType[it]=(medGeometryElement)(*listIt) ;
      it++;
    }
  }
  // set new value :
  int * numberOfEntities=_numberOfEntities ;
  _numberOfEntities = new int[it] ;
  _totalNumberOfEntities = 0 ;
  //  int totalSize = 0 ;
  int ** tmp_array = new (int*)[it];
  for (int i=0;i<it;i++) {
    int numberOfElementsInType = tmp_NumberOfElementsInType[i] ;
    _numberOfEntities[i] = numberOfElementsInType ;
    tmp_array[i] = new int[numberOfElementsInType] ;
    //totalSize+=numberOfElementsInType*(myType[i]%100) ;
    _totalNumberOfEntities+=numberOfElementsInType ;
    if (whereIsType[i] == 1) { // only first Support
      memcpy(tmp_array[i],getNumber(myType[i]),sizeof(int)*numberOfElementsInType);
    } else if (whereIsType[i] == 2) { // only second Support
      memcpy(tmp_array[i],mySupport->getNumber(myType[i]),sizeof(int)*numberOfElementsInType);
    } else if (whereIsType[i] == 3) { // more difficult :-)
      set<int> elementList ;
      int i1 = 0 ;
      int i2 = 0 ;
      int ii = 0 ;
      int * number1 = getNumber(myType[i]) ;
      int * number2 = mySupport->getNumber(myType[i]) ;

      SCRUTE(number1);
      SCRUTE(number2);

      int numberOfElements1 = numberOfEntities[i] ;
      int numberOfElements2 = mySupport->getNumberOfElements(myType[i]) ;

      SCRUTE(numberOfElements1);
      SCRUTE(numberOfElements2);

      for(int j=0;j<numberOfElements1;j++){
	elementList.insert(number1[j]) ;
      }

      for(int j=0;j<numberOfElements2;j++){
	SCRUTE(number2[j]);
	elementList.insert(number2[j]) ;
      }

      //create the array !
      int newNumberOfEntities = elementList.size() ;
      _numberOfEntities[i] = newNumberOfEntities ;
      int * tmp_arrayNew = new int[newNumberOfEntities];

      set<int>::iterator its ;
      for(its=elementList.begin();its!=elementList.end(); its++) {
	tmp_arrayNew[ii]=*its ;
	ii++;
      }

      delete[] tmp_array[i] ;
      tmp_array[i] = tmp_arrayNew ;
      _totalNumberOfEntities-=(numberOfElementsInType-newNumberOfEntities) ;

    } else
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"ERROR")) ;
  }
  delete[] whereIsType ;
  delete[] tmp_NumberOfElementsInType ;

  _numberOfGeometricType = it ;
  medGeometryElement * geometricType=_geometricType ;
  _geometricType = new medGeometryElement[it] ;
  int * numberOfGaussPoint=_numberOfGaussPoint ;
  _numberOfGaussPoint= new int[it] ;
  int * geometricTypeNumber=_geometricTypeNumber ;
  _geometricTypeNumber = new int[it] ;

  MEDSKYLINEARRAY* numberNew = new MEDSKYLINEARRAY(it,_totalNumberOfEntities);
  int * numberIndex = numberNew->getIndex() ;
  numberIndex[0]=1;
  for (int i=0;i<it;i++) {
    memcpy(numberNew->getI(i+1),tmp_array[i],sizeof(int)*_numberOfEntities[i]) ;
    delete[] tmp_array[i] ;
    numberIndex[i+1]=numberIndex[i]+_numberOfEntities[i] ;

    _geometricType[i]=myType[i] ;
    _numberOfGaussPoint[i]=1 ;
    _geometricTypeNumber[i]=0 ;

  }
  delete _number ;
  _number = numberNew ;

  delete[] myType ;
  delete[] tmp_array ;

  delete[] geometricType ;
  delete[] numberOfGaussPoint ;
  delete[] geometricTypeNumber ;
  delete[] numberOfEntities ;

  END_OF(LOC);
};

/*!
    This function allows the user to set a support not on all entities Entity,
    it should be used after an initialisation with the constructor
    SUPPORT(MESH* Mesh, string Name="", medEntityMesh Entity=MED_CELL) and
    after the call to the function setAll(false).
    It allocates and initialises all the attributs of the class SUPPORT.;
    useful for the Python API.
 */

//-------------------
void SUPPORT::setpartial(string Description, int NumberOfGeometricType,
			 int TotalNumberOfEntity,
			 medGeometryElement *GeometricType,
			 int *NumberOfEntity, int *NumberValue) 
//-------------------
{
  const char * LOC = "SUPPORT::setpartial(string , int , int , medGeometryElement * , int * , int *) : " ;
  BEGIN_OF(LOC) ;

  _description=Description;

  _numberOfGeometricType=NumberOfGeometricType;

  _totalNumberOfEntities=TotalNumberOfEntity;

  _geometricType = new medGeometryElement[NumberOfGeometricType];
  _geometricType = GeometricType;

  // attributes initialised at 0 because not really useful but it has to be
  // allocated !! WARNING !!

  _geometricTypeNumber = new int[NumberOfGeometricType] ;
  for (int i=0;i<NumberOfGeometricType;i++) _geometricTypeNumber[i] = 0;

  //_numberOfEntities = new int[NumberOfGeometricType] ;
  _numberOfEntities = NumberOfEntity;

  MEDSKYLINEARRAY * number = new MEDSKYLINEARRAY();

  int * index = new int[_numberOfGeometricType+1];
  index[0]=1;

  for (int i=0;i<_numberOfGeometricType;i++)
    index[i+1] = index[i]+_numberOfEntities[i] ;

  number->setMEDSKYLINEARRAY(_numberOfGeometricType,_totalNumberOfEntities,index,NumberValue);

  if (_number!=NULL) delete _number ;
  _number = number;

  update();

  END_OF(LOC);
};
