using namespace std;
/*
 File Support.cxx
 $Header$
*/

#include <set>
#include <algorithm>
#include <list>

//#include "utilities.h"
//#include "MEDMEM_define.hxx"
#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_Support.hxx"
//#include "MEDMEM_Family.hxx"
//#include "MEDMEM_Group.hxx"
#include "MEDMEM_Mesh.hxx"

using namespace MED_EN;
using namespace MEDMEM;

/* This class is a generic class for family and group */

/*!
  Constructor.
*/
//--------------------------------------------------------------------------
SUPPORT::SUPPORT(): _name(""),	_description("None"), _mesh((MESH*)NULL),
		    _entity(MED_CELL), _numberOfGeometricType(0),
	    	    _geometricType((medGeometryElement*)NULL),
		    _numberOfGaussPoint((int*)NULL),
		    //_geometricTypeNumber((int*)NULL),
		    _isOnAllElts(false),
		    _numberOfElements((int*)NULL),
		    _totalNumberOfElements(0),
		    _number((MEDSKYLINEARRAY*)NULL)
//--------------------------------------------------------------------------
{
    MESSAGE("SUPPORT::SUPPORT()");
}; 

/*!
  Constructor.
*/
//--------------------------------------------------------------------------
SUPPORT::SUPPORT(MESH* Mesh, string Name/*=""*/, medEntityMesh Entity/*=MED_CELL*/):
		_name(Name), _description("None"), _mesh(Mesh), _entity(Entity),
		_numberOfGeometricType(0),
		_geometricType((medGeometryElement*)NULL),
		_numberOfGaussPoint((int*)NULL),
		//_geometricTypeNumber((int*)NULL),
		_isOnAllElts(true), 
		_numberOfElements((int*)NULL),
		_totalNumberOfElements(0),
		_number((MEDSKYLINEARRAY*)NULL)
//--------------------------------------------------------------------------
{
  MESSAGE("SUPPORT::SUPPORT(MESH*Mesh,string Name,medEntityMesh Entity)");
  update() ;
};

/*!
  Copy constructor.
*/
//--------------------------------------------------------------------------
SUPPORT::SUPPORT(const SUPPORT & m)
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
//    if (m._geometricTypeNumber != NULL)
//      {
//        _geometricTypeNumber = new int[m._numberOfGeometricType];
//        memcpy(_geometricTypeNumber,m._geometricTypeNumber,m._numberOfGeometricType*sizeof(int));
//      }
//    else
//      _geometricTypeNumber = (int *) NULL;
  _isOnAllElts = m._isOnAllElts;
  if (m._numberOfElements != NULL)
    {
      _numberOfElements = new int[_numberOfGeometricType];
      memcpy(_numberOfElements,m._numberOfElements,_numberOfGeometricType*sizeof(int));
    }
  else
    _numberOfElements = (int *) NULL;
  _totalNumberOfElements = m._totalNumberOfElements;
  if (m._isOnAllElts == false)
    _number = new MEDSKYLINEARRAY(* m._number);
  else
    _number = (MEDSKYLINEARRAY *) NULL;

  END_OF(LOC) ;
};


/*!
  Destructor.
*/
//-----------------
SUPPORT::~SUPPORT() 
//-----------------
{
  MESSAGE("Destructeur ~SUPPORT()");
  if (_geometricType != (medGeometryElement *) NULL) 
    delete [] _geometricType ;
  if (_numberOfGaussPoint != (int *) NULL) 
    delete [] _numberOfGaussPoint ;
  //      if (_geometricTypeNumber!=NULL) 
  //        		delete[] _geometricTypeNumber ;
  if (_numberOfElements != (int *) NULL) 
    delete[] _numberOfElements ;
  if (_number != (MEDSKYLINEARRAY *) NULL) 
    delete _number ;
}

/*!
  operator <<.
*/
//--------------------------------------------------
ostream & MEDMEM::operator<<(ostream &os, const SUPPORT &my)
//--------------------------------------------------
{
  os << "Name : "<< my.getName() << endl ;
  os << "Description : "<< my.getDescription() << endl ;
  os << "Mesh name : ";
  if (my.getMesh() == NULL)
    os << "ERROR : Mesh not defined !" << endl ;
  else
    os << my._mesh->getName() << endl ;
  os << "Entity : "<< my._entity << endl;
  os << "Entity list : "<< endl;
  if (!(my._isOnAllElts)) {
    int numberoftypes = my._numberOfGeometricType ;
    os << "NumberOfTypes : "<<numberoftypes<<endl;
    medGeometryElement * types = my._geometricType;
    for (int j=0;j<numberoftypes;j++) {
      int numberOfElements = my._numberOfElements[j];
      os << "    * Type "<<types[j]<<" : there is(are) "<<numberOfElements<<" element(s) :" << endl;
//       const int * number = my.getNumber(types[j]);
//       SCRUTE(number);
//       os << " --> ";
//       for (int k=0; k<numberOfElements;k++)
// 	os << number[k] << " ";
//       os << endl ;
    }
  } else
    os << "Is on all entities !"<< endl;
  
  return os ;
}

/*!
  Updade the SUPPORT attributs with rigth MESH information.
  
  It has an effect only if SUPPORT is on all elements.

  No more need in future release.
*/
//-------------------
void SUPPORT::update()
//-------------------
{
  const char * LOC = "SUPPORT::update() : " ;
  BEGIN_OF(LOC) ;

  if (_isOnAllElts) {
    if (_entity == MED_NODE) {
      _numberOfGeometricType=1 ;
      _geometricType=new medGeometryElement[1] ;
      _geometricType[0]=MED_NONE ;
      _numberOfElements = new int[1] ;
      _numberOfElements[0]=_mesh->getNumberOfNodes();
      _totalNumberOfElements=_numberOfElements[0];
      _numberOfGaussPoint = new int[1] ;
      _numberOfGaussPoint[0]=1;
    } else { // we duplicate information from _mesh
      _numberOfGeometricType=_mesh->getNumberOfTypes(_entity);
      if (_geometricType == (medGeometryElement *) NULL)
	_geometricType = new medGeometryElement[_numberOfGeometricType] ;
      memcpy(_geometricType,_mesh->getTypes(_entity),_numberOfGeometricType*sizeof(medGeometryElement));
      if (_numberOfElements == (int *) NULL)
	_numberOfElements = new int[_numberOfGeometricType] ;
      if (_numberOfGaussPoint == (int *) NULL)
	_numberOfGaussPoint = new int[_numberOfGeometricType] ;
      _totalNumberOfElements=0;
      for (int i=0;i<_numberOfGeometricType;i++) {
	_numberOfElements[i]=_mesh->getNumberOfElements(_entity,_geometricType[i]) ;
	_totalNumberOfElements+=_numberOfElements[i];
	_numberOfGaussPoint[i]=1 ;
      }
    }
  }
  END_OF(LOC);
};

/*!
  Blend the given SUPPORT mySupport into the calling object SUPPORT.
*/
//-------------------
void SUPPORT::blending(SUPPORT * mySupport) throw (MEDEXCEPTION)
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
  for(listIt=(MED_FR::meshEntities[(MED_FR::med_entite_maillage)_entity]).begin();listIt!=(MED_FR::meshEntities[(MED_FR::med_entite_maillage)_entity]).end();listIt++) {
    tmp_NumberOfElementsInType[it]=0;
    whereIsType[it]=0 ;
    try {
      int tmp_int = 0;
      tmp_int = getNumberOfElements((medGeometryElement)(*listIt)) ;
      tmp_NumberOfElementsInType[it]+=tmp_int ;
      whereIsType[it]+=1 ;
    }
    catch (MEDEXCEPTION & ex) { SCRUTE(sizeof(ex)); };
    try {
      int tmp_int = 0;
      tmp_int = mySupport->getNumberOfElements((medGeometryElement)(*listIt)) ;
      tmp_NumberOfElementsInType[it]+=tmp_int ;
      whereIsType[it]+=2 ;
    }
    catch (const MEDEXCEPTION & ex) {};
    if (whereIsType[it]!=0) {
      myType[it]=(medGeometryElement)(*listIt) ;
      SCRUTE(myType[it]);SCRUTE(it);SCRUTE((*listIt));
      it++;
    }
  }
  // set new value :
//   int * numberOfElements=_numberOfElements ;
//   _numberOfElements = new int[it] ;
  int * numberOfElements= new int[it];
  _totalNumberOfElements = 0 ;
  //int totalSize = 0 ;
  int ** tmp_array = new (int*)[it];
  for (int i=0;i<it;i++) {
    int numberOfElementsInType = tmp_NumberOfElementsInType[i] ;
    numberOfElements[i] = numberOfElementsInType ;
    tmp_array[i] = new int[numberOfElementsInType] ;
    //totalSize+=numberOfElementsInType*(myType[i]%100) ;
    _totalNumberOfElements+=numberOfElementsInType ;
    if (whereIsType[i] == 1) { // only first Support
      memcpy(tmp_array[i],getNumber(myType[i]),sizeof(int)*numberOfElementsInType);
    } else if (whereIsType[i] == 2) { // only second Support
      memcpy(tmp_array[i],mySupport->getNumber(myType[i]),sizeof(int)*numberOfElementsInType);
    } else if (whereIsType[i] == 3) { // more difficult :-)
      set<int> elementList ;
      //int i1 = 0 ; !! UNUSED VARIABLE !!
      //int i2 = 0 ; !!UNUSED VARIABLE !!
      int ii = 0 ;
      const int * number1 = getNumber(myType[i]) ;
      const int * number2 = mySupport->getNumber(myType[i]) ;

      SCRUTE(number1);
      SCRUTE(number2);

      int numberOfElements1 = getNumberOfElements(myType[i]) ;
      int numberOfElements2 = mySupport->getNumberOfElements(myType[i]) ;

      SCRUTE(numberOfElements1);
      SCRUTE(numberOfElements2);

      MESSAGE(LOC << " Type : " << myType[i] << " " << i);

      for(int j=0;j<numberOfElements1;j++){
	elementList.insert(number1[j]) ;
      }

      for(int j=0;j<numberOfElements2;j++){
	elementList.insert(number2[j]) ;
      }

      //create the array !
      int newNumberOfElements = elementList.size() ;

      SCRUTE(newNumberOfElements);

      numberOfElements[i] = newNumberOfElements ;
      int * tmp_arrayNew = new int[newNumberOfElements];

      set<int>::iterator its ;
      for(its=elementList.begin();its!=elementList.end(); its++) {
	tmp_arrayNew[ii]=*its ;
	ii++;
      }

      delete[] tmp_array[i] ;
      tmp_array[i] = tmp_arrayNew ;
      _totalNumberOfElements-=(numberOfElementsInType-newNumberOfElements) ;

    } else
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"ERROR")) ;
  }
  delete[] whereIsType ;
  delete[] tmp_NumberOfElementsInType ;
  delete [] _numberOfElements;

  _numberOfElements = numberOfElements;

  _numberOfGeometricType = it ;
  medGeometryElement * geometricType=_geometricType ;
  _geometricType = new medGeometryElement[it] ;
  int * numberOfGaussPoint=_numberOfGaussPoint ;
  _numberOfGaussPoint= new int[it] ;
//    int * geometricTypeNumber=_geometricTypeNumber ;
//    _geometricTypeNumber = new int[it] ;

//    MEDSKYLINEARRAY* numberNew = new MEDSKYLINEARRAY(it,_totalNumberOfElements);
//    int * numberIndex = numberNew->getIndex() ;
  int size = _mesh->getNumberOfElements(_entity,MED_ALL_ELEMENTS);
  if (_totalNumberOfElements == size) _isOnAllElts = true;

  int * numberValue = new int[_totalNumberOfElements] ;
  int * numberIndex = new int[it+1] ;
  numberIndex[0]=1;
  for (int i=0;i<it;i++) {
    memcpy(numberValue+numberIndex[i]-1,tmp_array[i],sizeof(int)*_numberOfElements[i]) ;
    delete[] tmp_array[i] ;
    numberIndex[i+1]=numberIndex[i]+_numberOfElements[i] ;

    _geometricType[i]=myType[i] ;
    _numberOfGaussPoint[i]=1 ;
  }
  if ( _number != (MEDSKYLINEARRAY *) NULL) delete _number ;
  //_number = numberNew ;
  _number = new MEDSKYLINEARRAY(it,_totalNumberOfElements,numberIndex,numberValue);
  delete[] numberIndex;

  delete[] numberValue;

  delete[] myType ;
  delete[] tmp_array ;

  delete[] geometricType ;
  delete[] numberOfGaussPoint ;
//    delete[] geometricTypeNumber ;
//  delete[] numberOfElements ;




  MESSAGE(LOC<<"Printing of the object SUPPORT blended "<< *this);




  END_OF(LOC);
};

/*!
    This function allows the user to set a support not on all entities Entity,
    it should be used after an initialisation with the constructor
    SUPPORT(MESH* Mesh, string Name="", medEntityMesh Entity=MED_CELL) and
    after the call to the function setAll(false).
    It allocates and initialises all the attributs of the class SUPPORT.
 */

//-------------------
void SUPPORT::setpartial(string Description, int NumberOfGeometricType,
			 int TotalNumberOfElements,
			 medGeometryElement *GeometricType,
			 int *NumberOfElements, int *NumberValue) 
//-------------------
{
  const char * LOC = "SUPPORT::setpartial(string , int , int , medGeometryElement * , int * , int *) : " ;
  BEGIN_OF(LOC) ;

  _isOnAllElts = false ;

  _description=Description;

  _numberOfGeometricType=NumberOfGeometricType;

  if (_geometricType!=NULL) delete[] _geometricType ;
  _geometricType = new medGeometryElement[NumberOfGeometricType];
  if (_numberOfElements!=NULL) delete[] _numberOfElements ;
  _numberOfElements = new int[NumberOfGeometricType];
  _totalNumberOfElements = TotalNumberOfElements;
  if (_numberOfGaussPoint!=NULL) delete[] _numberOfGaussPoint ;
  _numberOfGaussPoint = new int[NumberOfGeometricType];
  int * index = new int[_numberOfGeometricType+1];
  index[0]=1;
  for (int i=0;i<_numberOfGeometricType;i++) {
    _geometricType[i] = GeometricType[i] ;
    _numberOfElements[i] = NumberOfElements[i] ;
    _numberOfGaussPoint[i] = 1 ;
    index[i+1] = index[i]+NumberOfElements[i] ;
  }
  
  if (_number!=NULL) delete _number ;
  _number = new MEDSKYLINEARRAY(_numberOfGeometricType,_totalNumberOfElements,index,NumberValue);

  delete[] index ;

  END_OF(LOC);
};


/*!
  This method gets the boundary elements of the mesh. The support has to be
  build using the constructor SUPPORT(MESH *,string, medEntityMesh) or
  SUPPORT() followed by setMesh(MESH*) setName(string) and
  setEntity(medEntityMesh) before using this method.
*/
//-------------------
void SUPPORT::getBoundaryElements() throw (MEDEXCEPTION)
//-------------------
{
  const char * LOC = "SUPPORT::getBoundaryElements() : " ;
  BEGIN_OF(LOC) ;

  if (_mesh == (MESH*)NULL) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"You shlould use the appropriate SUPPORT Constructor before calling this method"));

  int spaceDimension = _mesh->getSpaceDimension();

  if (spaceDimension == 3)
    if (_entity != MED_FACE)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined in 3D mesh for entity "<<_entity<<" !"));
  if (spaceDimension == 2) 
    if (_entity != MED_EDGE)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined in 2D mesh for entity "<<_entity<<" !"));

  setAll(false);

  const int * myConnectivityValue = _mesh->getReverseConnectivity(MED_DESCENDING) ;
  const int * myConnectivityIndex = _mesh->getReverseConnectivityIndex(MED_DESCENDING) ;
  int numberOf = _mesh->getNumberOfElements(_entity,MED_ALL_ELEMENTS) ;
  list<int> myElementsList ;
  int size = 0 ;
  SCRUTE(numberOf) ;
  for (int i=0 ; i<numberOf; i++)
    if (myConnectivityValue[myConnectivityIndex[i]] == 0) {
      SCRUTE(i+1) ;
      myElementsList.push_back(i+1) ;
      size++ ;
    }
  SCRUTE(size) ;
  // Well, we must know how many geometric type we have found
  int * myListArray = new int[size] ;
  int id = 0 ;
  list<int>::iterator myElementsListIt ;
  for (myElementsListIt=myElementsList.begin();myElementsListIt!=myElementsList.end();myElementsListIt++) {
    myListArray[id]=(*myElementsListIt) ;
    SCRUTE(id);
    SCRUTE(myListArray[id]);
    id ++ ;
  }

  int numberOfGeometricType ;
  medGeometryElement* geometricType ;
  int * numberOfGaussPoint ;
  int * geometricTypeNumber ;
  int * numberOfElements ;
  //MEDSKYLINEARRAY * mySkyLineArray = new MEDSKYLINEARRAY() ;
  int * mySkyLineArrayIndex ;

  int numberOfType = _mesh->getNumberOfTypes(_entity) ;
  if (numberOfType == 1) { // wonderfull : it's easy !
    numberOfGeometricType = 1 ;
    geometricType = new medGeometryElement[1] ;
    const medGeometryElement *  allType = _mesh->getTypes(_entity);
    geometricType[0] = allType[0] ;
    numberOfGaussPoint = new int[1] ;
    numberOfGaussPoint[0] = 1 ;
    geometricTypeNumber = new int[1] ; // not use, but initialized to nothing
    geometricTypeNumber[0] = 0 ;
    numberOfElements = new int[1] ;
    numberOfElements[0] = size ;
    mySkyLineArrayIndex = new int[2] ;
    mySkyLineArrayIndex[0]=1 ;
    mySkyLineArrayIndex[1]=1+size ;
  }
  else {// hemmm
    map<medGeometryElement,int> theType ;
    for (myElementsListIt=myElementsList.begin();myElementsListIt!=myElementsList.end();myElementsListIt++) {
      medGeometryElement myType = _mesh->getElementType(_entity,*myElementsListIt) ;
      if (theType.find(myType) != theType.end() )
	theType[myType]+=1 ;
      else
	theType[myType]=1 ;
    }
    numberOfGeometricType = theType.size() ;
    geometricType = new medGeometryElement[numberOfGeometricType] ;
    //const medGeometryElement *  allType = _mesh->getTypes(_entity); !! UNUSED VARIABLE !!
    numberOfGaussPoint = new int[numberOfGeometricType] ;
    geometricTypeNumber = new int[numberOfGeometricType] ; // not use, but initialized to nothing
    numberOfElements = new int[numberOfGeometricType] ;
    mySkyLineArrayIndex = new int[numberOfGeometricType+1] ;
    int index = 0 ;
    mySkyLineArrayIndex[0]=1 ;
    map<medGeometryElement,int>::iterator theTypeIt ;
    for (theTypeIt=theType.begin();theTypeIt!=theType.end();theTypeIt++) {
      geometricType[index] = (*theTypeIt).first ;
      numberOfGaussPoint[index] = 1 ;
      geometricTypeNumber[index] = 0 ;
      numberOfElements[index] = (*theTypeIt).second ;
      mySkyLineArrayIndex[index+1]=mySkyLineArrayIndex[index]+numberOfElements[index] ;
      index++ ;
    }
  }
  //mySkyLineArray->setMEDSKYLINEARRAY(numberOfGeometricType,size,mySkyLineArrayIndex,myListArray) ;
  MEDSKYLINEARRAY * mySkyLineArray = new MEDSKYLINEARRAY(numberOfGeometricType,size,mySkyLineArrayIndex,myListArray) ;

  setNumberOfGeometricType(numberOfGeometricType) ;
  //  setGeometricType(geometricType) ;
  //  setNumberOfGaussPoint(numberOfGaussPoint) ;
  for (int i=0;i<numberOfGeometricType;i++)
    {
      _numberOfGaussPoint[i] = numberOfGaussPoint[i];
      _geometricType[i] = geometricType[i];
    }

  setNumberOfElements(numberOfElements) ;
  setTotalNumberOfElements(size) ;
  //  setNumber(mySkyLineArray) ;

  _number = new MEDSKYLINEARRAY(numberOfGeometricType,size);

  _number->setIndex(mySkyLineArrayIndex);

  for (int i=0;i<size;i++)
    {
      _number->setIndexValue(i+1,myListArray[i]);
    }

  delete[] numberOfElements;
  delete[] geometricTypeNumber;
  delete[] numberOfGaussPoint;
  delete[] geometricType;
  delete[] mySkyLineArrayIndex;
  delete[] myListArray;
  delete mySkyLineArray;

  END_OF(LOC) ;
}

/*!
  intersect the given SUPPORT mySupport into the calling SUPPORT object.
*/
//-------------------
void SUPPORT::intersecting(SUPPORT * mySupport) throw (MEDEXCEPTION)
//-------------------
{
  const char * LOC = "SUPPORT::intersecting(SUPPORT *) : " ;
  BEGIN_OF(LOC) ;

  MESSAGE(LOC<< "SUPPORT entry : " << *mySupport) ;

  MESSAGE(LOC<< "SUPPORT (calling object) : " << *this) ;

  // on same entity :
  if ( _entity != mySupport->getEntity() )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Entities are different !"));

  int * tmp_NumberOfElementsInType = new int[MED_NBR_GEOMETRIE_MAILLE];
  medGeometryElement * myType = new medGeometryElement[MED_NBR_GEOMETRIE_MAILLE];
  int * whereIsType = new int[MED_NBR_GEOMETRIE_MAILLE];
  //MESH_ENTITIES myMeshEntities() ;
  list<MED_FR::med_geometrie_element>::const_iterator listIt ;
  int it=0 ;
  for(listIt=(MED_FR::meshEntities[(MED_FR::med_entite_maillage)_entity]).begin();listIt!=(MED_FR::meshEntities[(MED_FR::med_entite_maillage)_entity]).end();listIt++) {
    tmp_NumberOfElementsInType[it]=0;
    whereIsType[it]=0 ;
    myType[it]= MED_NONE;
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
    if (whereIsType[it]==3) {
      myType[it]=(medGeometryElement)(*listIt) ;
      it++;
    }
  }

  MESSAGE("it = "<< it);

  // set new value :
  int * numberOfElements=_numberOfElements ;
  _numberOfElements = new int[it] ;
  _totalNumberOfElements = 0 ;
  //int totalSize = 0 ;
  int ** tmp_array = new (int*)[it];
  for (int i=0;i<it;i++) {
    int numberOfElementsInType = tmp_NumberOfElementsInType[i] ;
    _numberOfElements[i] = numberOfElementsInType ;
    tmp_array[i] = new int[numberOfElementsInType] ;
    _totalNumberOfElements+=numberOfElementsInType ;
    if (whereIsType[i] == 3) {
      const int * number1 = getNumber(myType[i]) ;
      const int * number2 = mySupport->getNumber(myType[i]) ;

      SCRUTE(number1);
      SCRUTE(number2);

      int numberOfElements1 = numberOfElements[i] ;
      int numberOfElements2 = mySupport->getNumberOfElements(myType[i]) ;

      SCRUTE(numberOfElements1);
      SCRUTE(numberOfElements2);

      set<int> setList1(number1,number1+numberOfElements1);
      set<int> setList2(number2,number2+numberOfElements2);

      for(set<int>::iterator its=setList1.begin();its!=setList1.end(); its++)
	{
	  MESSAGE("Number1 " << *its);
	}

      for(set<int>::iterator its=setList2.begin();its!=setList2.end(); its++)
	{
	  MESSAGE("Number2 " << *its);
	}

      set<int> setListIntersect;

      set_intersection(setList1.begin(),setList1.end(),setList2.begin(),
		       setList2.end(),inserter(setListIntersect,
					       setListIntersect.begin()));

      for(set<int>::iterator its=setListIntersect.begin();
	  its!=setListIntersect.end(); its++)
	{
	  MESSAGE("Number1 intersect Number2 " << *its);
	}

      int newNumberOfElements = setListIntersect.size() ;

      SCRUTE(newNumberOfElements);

      _numberOfElements[i] = newNumberOfElements ;
      int * tmp_arrayNew = new int[newNumberOfElements];

      int ii = 0 ;

      for(set<int>::iterator its=setListIntersect.begin();
	  its!=setListIntersect.end(); its++) {
	tmp_arrayNew[ii]=*its ;
	SCRUTE(tmp_arrayNew[ii]);
	ii++;
      }

      delete[] tmp_array[i] ;
      tmp_array[i] = tmp_arrayNew ;
      _totalNumberOfElements-=(numberOfElementsInType-newNumberOfElements) ;

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

  int size = _mesh->getNumberOfElements(_entity,MED_ALL_ELEMENTS);
  if (_totalNumberOfElements == size) _isOnAllElts = true;

  int * numberValue = new int[_totalNumberOfElements] ;
  int * numberIndex = new int[it+1] ;
  numberIndex[0]=1;
  for (int i=0;i<it;i++) {
    memcpy(numberValue+numberIndex[i]-1,tmp_array[i],sizeof(int)*_numberOfElements[i]) ;
    delete[] tmp_array[i] ;
    numberIndex[i+1]=numberIndex[i]+_numberOfElements[i] ;

    _geometricType[i]=myType[i] ;
    _numberOfGaussPoint[i]=1 ;
  }
  if ( _number != (MEDSKYLINEARRAY *) NULL) delete _number ;

  _number = new MEDSKYLINEARRAY(it,_totalNumberOfElements,numberIndex,numberValue);
  delete[] numberIndex;

  delete[] numberValue;

  delete[] myType ;
  delete[] tmp_array ;

  delete[] geometricType ;
  delete[] numberOfGaussPoint ;
//    delete[] geometricTypeNumber ;
  delete[] numberOfElements ;

  END_OF(LOC);
};

/*!
  operator == This operator does not compare attributs _name and _description.
*/
//--------------------------------------------------
bool MEDMEM::SUPPORT::operator == (const SUPPORT &support) const
//--------------------------------------------------
{
  const char * LOC = "bool SUPPORT::operator ==(const SUPPORT &support) const : ";

  BEGIN_OF(LOC);

  bool operatorReturn = false;

  operatorReturn = (_mesh == support._mesh) && (_entity == support._entity) &&
    (_numberOfGeometricType == support._numberOfGeometricType) &&
    (_isOnAllElts == support._isOnAllElts) &&
    (_totalNumberOfElements == support._totalNumberOfElements);

  if (operatorReturn)
    {
      if (!_isOnAllElts)
	{
	  for (int i=0; i<_numberOfGeometricType; i++)
	    {
	      operatorReturn = operatorReturn &&
		(_geometricType[i] == support._geometricType[i]) &&
		(_numberOfElements[i] == support._numberOfElements[i]) &&
		(_numberOfGaussPoint[i] == support._numberOfGaussPoint[i]);

	      if (operatorReturn)
		{
		  for (int j=0; j<_numberOfElements[i]; j++)
		    {
		      operatorReturn = operatorReturn &&
			(getNumber(_geometricType[i])[j] ==
			 support.getNumber(_geometricType[i])[j]);
		    }
		}
	    }
	}
    }

  END_OF(LOC);

  return operatorReturn;
};
