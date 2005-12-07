/*
 File Support.cxx
 $Header$
*/

#include <set>
#include <algorithm>
#include <list>

#include "MEDMEM_Support.hxx"
#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_Mesh.hxx"

using namespace std;
using namespace MED_EN;
using namespace MEDMEM;

#define MED_NBR_GEOMETRIE_MAILLE 15

/* This class is a generic class for family and group */

/*!
  Constructor.
*/
//--------------------------------------------------------------------------
SUPPORT::SUPPORT(): _name(""),	_description("None"), _mesh((MESH*)NULL),
		    _entity(MED_CELL), _numberOfGeometricType(0),
		    _isOnAllElts(false),
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
		_numberOfGeometricType(0), _isOnAllElts(true),
		_totalNumberOfElements(0), _number((MEDSKYLINEARRAY*)NULL)
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

  _name = m._name ;
  _description = m._description ;
  _mesh = m._mesh ; // on recopie uniquement l'adresse
  _entity = m._entity;
  _numberOfGeometricType = m._numberOfGeometricType;

  if (m._geometricType)
    _geometricType.set(_numberOfGeometricType,m._geometricType);

  if (m._numberOfGaussPoint)
    _numberOfGaussPoint.set(_numberOfGeometricType,m._numberOfGaussPoint);

  _isOnAllElts = m._isOnAllElts;

  if (m._numberOfElements)
    _numberOfElements.set(_numberOfGeometricType,m._numberOfElements);

  _totalNumberOfElements = m._totalNumberOfElements;

  if (m._isOnAllElts == false)
    _number = new MEDSKYLINEARRAY(* m._number);
  else
    _number = (MEDSKYLINEARRAY *) NULL;

  END_OF(LOC) ;
};
/*!
  Affectation operator. operator = perform et deep copy except for attribute _mesh
*/
//--------------------------------------------------------------------------
SUPPORT & SUPPORT::operator=(const SUPPORT & m)
//--------------------------------------------------------------------------
{
  const char * LOC = "SUPPORT::operator=(const SUPPORT & m) : " ;
  BEGIN_OF(LOC) ;

  if ( this == &m ) return *this;

  _name = m._name;
  _description = m._description;
  _mesh = m._mesh ; // on recopie uniquement l'adresse
  _entity = m._entity;
  _numberOfGeometricType = m._numberOfGeometricType;
  if (m._geometricType)
    _geometricType.set(_numberOfGeometricType,m._geometricType);
  if (m._numberOfGaussPoint)
    _numberOfGaussPoint.set(_numberOfGeometricType,m._numberOfGaussPoint);
  _isOnAllElts = m._isOnAllElts;
  if (m._numberOfElements)
    _numberOfElements.set(_numberOfGeometricType,m._numberOfElements);
  _totalNumberOfElements = m._totalNumberOfElements;
  if (m._isOnAllElts == false) {
    if (_number) delete _number;
    _number = new MEDSKYLINEARRAY(* m._number);
  } else
    _number = (MEDSKYLINEARRAY *) NULL;

  END_OF(LOC) ;
  return *this;
}

/*!
  Destructor.
*/
//-----------------
SUPPORT::~SUPPORT()
//-----------------
{
  MESSAGE("Destructeur ~SUPPORT()");
  clearDataOnNumbers();
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
    PointerOf<medGeometryElement> types = my._geometricType;
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

  if (_isOnAllElts)
    {
      if (_entity == MED_NODE)
	{
	  _numberOfGeometricType=1 ;
	  _geometricType.set(1);
	  _geometricType[0]=MED_NONE;
	  _numberOfElements.set(1);
	  _numberOfElements[0]=_mesh->getNumberOfNodes();
	  _totalNumberOfElements=_numberOfElements[0];
	  _numberOfGaussPoint.set(1) ;
	  _numberOfGaussPoint[0]=1;
	}
      else
	{ // we duplicate information from _mesh
	  _numberOfGeometricType=_mesh->getNumberOfTypesWithPoly(_entity);
	  _geometricType.set(_numberOfGeometricType,_mesh->getTypesWithPoly(_entity) );
	  _numberOfElements.set(_numberOfGeometricType);
	  _numberOfGaussPoint.set(_numberOfGeometricType);
	  _totalNumberOfElements=0;
	  for (int i=0;i<_numberOfGeometricType;i++)
	    {
	      _numberOfElements[i]=_mesh->getNumberOfElementsWithPoly(_entity,_geometricType[i]) ;
	      _totalNumberOfElements+=_numberOfElements[i];
	      _numberOfGaussPoint[i]=1 ;
	    }
	}
    }
  END_OF(LOC);
};
/*!
  Get the field value index from the support global number.
  Becareful, it doesn't take care of the field number of components
*/
//-------------------
int SUPPORT::getValIndFromGlobalNumber(const int number) const throw (MEDEXCEPTION)
//-------------------
{
  const char * LOC="getValIndFromGlobalNumber(const int number) : ";
  BEGIN_OF(LOC);

  if (_isOnAllElts) return number;

  int nbOfEltsThis    = getNumberOfElements(MED_ALL_ELEMENTS);
  const int *eltsThis = _number->getValue();

  int iThis;
  bool found=false;

  for(iThis=0;iThis<nbOfEltsThis && !found;)
    if(eltsThis[iThis]==number)
      return iThis;
    else
      iThis++;

  if(!found)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Can't find the global number |"
				 << number << "| in Support |"
				 << getName() << "|" ));

  return 0;

  END_OF(LOC);
}

/*!
  Blend the given SUPPORT mySupport into the calling object SUPPORT.
*/
//-------------------
void SUPPORT::blending(SUPPORT * mySupport) throw (MEDEXCEPTION)
//-------------------
{
  const char * LOC="SUPPORT::blending(SUPPORT *) : ";
  BEGIN_OF(LOC);
  if (_entity!=mySupport->getEntity())
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Entities are different !"));
  if(!(*_mesh == *mySupport->getMesh()))
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Mesh are different !"));
  if(_isOnAllElts)
    return;
  if(mySupport->isOnAllElements())
    {
      *this=*mySupport;
      return;
    }
  if(mySupport->_totalNumberOfElements==0)
    return;
  const int *ids=getNumber(MED_ALL_ELEMENTS);
  set<int> idsSet(ids,ids+getNumberOfElements(MED_ALL_ELEMENTS));
  const int *idsMySupport=mySupport->getNumber(MED_ALL_ELEMENTS);
  int mySupportSize=mySupport->getNumberOfElements(MED_ALL_ELEMENTS);
  set<int>::iterator iter;
  for(int i=0;i<mySupportSize;i++)
    idsSet.insert(idsMySupport[i]);
  int size=idsSet.size();
  if(size!=0)
    {
      list<int> idsList;
      for(iter=idsSet.begin();iter!=idsSet.end();iter++)
	idsList.push_back(*iter);
      if(_entity==MED_NODE)
	fillFromNodeList(idsList);
      else
	fillFromElementList(idsList);
    }
  else
    clearDataOnNumbers();
  END_OF(LOC);
}

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
  _geometricType.set(NumberOfGeometricType);
  _numberOfElements.set(NumberOfGeometricType);
  _totalNumberOfElements = TotalNumberOfElements;
  _numberOfGaussPoint.set(NumberOfGeometricType);

  int * index = new int[_numberOfGeometricType+1];
  index[0]=1;
  int elemDim = -1;
  for (int i=0;i<_numberOfGeometricType;i++) {
    if(GeometricType[i]/100 != elemDim)
      if(i==0)
	elemDim=GeometricType[i]/100;
      else
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"unhomogeneous geometric types (dimension) !"));
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
{
  const char * LOC="SUPPORT::intersecting(SUPPORT *) : ";
  BEGIN_OF(LOC);
  if (_entity!=mySupport->getEntity())
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Entities are different !"));
  if(!(*_mesh == *mySupport->getMesh()))
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Mesh are different !"));
  if(mySupport->isOnAllElements())
    return;
  if(_isOnAllElts)
    {
      *this=*mySupport;
      return;
    }
  if(_totalNumberOfElements==0)
    return;
  const int *ids=getNumber(MED_ALL_ELEMENTS);
  set<int> idsSet(ids,ids+getNumberOfElements(MED_ALL_ELEMENTS));
  const int *idsMySupport=mySupport->getNumber(MED_ALL_ELEMENTS);
  int mySupportSize=mySupport->getNumberOfElements(MED_ALL_ELEMENTS);
  set<int> idsSetMySupport(idsMySupport,idsMySupport+mySupportSize);
  set<int>::iterator iter;
  list<int> idsList;
  for(iter=idsSet.begin();iter!=idsSet.end();iter++)
    if(idsSetMySupport.find(*iter)!=idsSetMySupport.end())
      idsList.push_back(*iter);
  int size=idsSet.size();
  if(size!=0)
    {
      if(_entity==MED_NODE)
	fillFromNodeList(idsList);
      else
	fillFromElementList(idsList);
    }
  else
    {
      clearDataOnNumbers();
    }
  END_OF(LOC);
};

/*!
  Method that cleans up all the fields related to _numbers. Defined for code factorization.
 */
//--------------------------------------------------
void MEDMEM::SUPPORT::clearDataOnNumbers()
//--------------------------------------------------
{
  _numberOfGeometricType=0;
  _totalNumberOfElements=0;

  if(_number)
    {
      delete _number;
      _number=(MEDSKYLINEARRAY *) NULL;
    }
}

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

  operatorReturn = (*_mesh == *support._mesh) && (_entity == support._entity) &&
    (_numberOfGeometricType == support._numberOfGeometricType) &&
    ((_isOnAllElts && support._isOnAllElts) || (!_isOnAllElts && !support._isOnAllElts)) &&
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

void SUPPORT::changeElementsNbs(MED_EN::medEntityMesh entity, const int *renumberingFromOldToNew, int limitNbClassicPoly, const int *renumberingFromOldToNewPoly)
{
  if(entity != _entity)
    throw MEDEXCEPTION("SUPPORT::changeElementsNbs : Renumbering on a mismatch entity");
  list<int> newNbs;
  if(!_isOnAllElts)
    {
      const int *oldNbs=_number->getValue();
      for(int i=0;i<_totalNumberOfElements;i++)
	{
	  int globNb=oldNbs[i];
	  if(globNb<=limitNbClassicPoly)
	    newNbs.push_back(renumberingFromOldToNew[globNb-1]);
	  else
	    newNbs.push_back(renumberingFromOldToNewPoly[globNb-limitNbClassicPoly-1]);
	}
      newNbs.sort();
      fillFromElementList(newNbs);
    }
  else
    update();
}

/*!
  operator == + in case false a test if coordinates and connectivity of _mesh and support->_mesh are the same
*/
bool MEDMEM::SUPPORT::deepCompare(const SUPPORT &support) const
{
  bool operatorReturn =(_entity == support._entity) &&
    (_numberOfGeometricType == support._numberOfGeometricType) &&
    ( (_isOnAllElts && support._isOnAllElts) || (!_isOnAllElts  && !support._isOnAllElts) ) &&
    (_totalNumberOfElements == support._totalNumberOfElements);
  if (operatorReturn)
    {
      if (!_isOnAllElts)
	{
	  for (int i=0; i<_numberOfGeometricType && operatorReturn; i++)
	    {
	      operatorReturn = (_geometricType[i] == support._geometricType[i]) &&
		(_numberOfElements[i] == support._numberOfElements[i]) &&
		(_numberOfGaussPoint[i] == support._numberOfGaussPoint[i]);
	      if (operatorReturn)
		{
		  for (int j=0; j<_numberOfElements[i]; j++)
		    {
		      operatorReturn = (getNumber(_geometricType[i])[j] ==
			 support.getNumber(_geometricType[i])[j]);
		    }
		}
	    }
	}
    }
  if(operatorReturn)
    {
      if(!(*_mesh == *support._mesh))
	{
	  return _mesh->deepCompare(*support._mesh);
	}
    }
  return operatorReturn;
}

/*!
 States if this is included in other.
 */
bool MEDMEM::SUPPORT::belongsTo(const SUPPORT& other, bool deepCompare) const
{
  if(!(*_mesh == *other._mesh))
    {
      if(!deepCompare)
	return false;
      if(!_mesh->deepCompare(*other._mesh))
	return false;
    }
  if(_entity!=other._entity)
    return false;
  if(other._isOnAllElts)
    return true;
  if(_isOnAllElts && !other._isOnAllElts)
    return false;
  if(_numberOfGeometricType>other._numberOfGeometricType)
    return false;
  for(int i=0; i<_numberOfGeometricType; i++)
    {
      MED_EN::medGeometryElement curGeomType=_geometricType[i];
      int iOther=-1;
      for(int j=0; j<other._numberOfGeometricType; j++)
	  if(other._geometricType[j]==curGeomType)
	      iOther=j;
      if(iOther==-1)
	return false;
      if(_numberOfElements[i]>other._numberOfElements[iOther])
	return false;
      const int *numbers1=_number->getI(i+1);
      const int *numbers2=other._number->getI(iOther+1);
      for (int k=0; k<_numberOfElements[i]; k++)
	{
	  bool found=false;
	  for(int l=0;l<other._numberOfElements[iOther] && !found;l++)
	    {
	      if(numbers1[k]==numbers2[l])
		found=true;
	    }
	  if(!found)
	    return false;
	}
    }
  return true;
}
/*!
  Method used to sort array of id.
 */
int compareId(const void *x, const void *y)
{
  const int *x1=(const int *)x;
  const int *y1=(const int *)y;
  if(*x1<*y1)
    return -1;
  else if(*x1>*y1)
    return 1;
  else
    return 0;
}

/*!
  performs a common operation : Sub builds a sorted int array that is obtained by supression of all ids contained
  in array defined by (idsToSuppress,lgthIdsToSuppress) from array [start ... end]
  Example sub(0,7,{1,2,5},3) => {0,3,4,6,7} - WARNING returned list should be deallocated !
 */
list<int> *MEDMEM::SUPPORT::sub(int start,int end,const int *idsToSuppress,int lgthIdsToSuppress)
{
  int size=end-start+1;
  int sizeRet=size-lgthIdsToSuppress;
  list<int> *ret;
  if(sizeRet<0)
    throw MEDEXCEPTION("MEDMEM::SUPPORT::sub");
  else if(sizeRet==0)
    {
      return 0;
    }
  if(idsToSuppress==0)
    {
      ret=new list<int>;
      for(int l=0;l<size;l++)
	ret->push_back(start+l);
      return ret;
    }
  ret=new list<int>;
  int *temp=new int[lgthIdsToSuppress];
  memcpy(temp,idsToSuppress,sizeof(int)*lgthIdsToSuppress);
  qsort(temp,lgthIdsToSuppress,sizeof(int),compareId);
  int k=0;
  for(int i=start;i<=end;i++)
    if(temp[k]!=i)
      ret->push_back(i);
    else
      k++;
  delete [] temp;
  return ret;
}

/*!
  performs a common operation : Sub builds a sorted int array that is obtained by supression of all ids contained
  in array defined by (idsToSuppress,lgthIdsToSuppress) from array [start ... end]
  Example sub({1,3,4,5,6,7,9},7,{1,2,5},3) => {3,4,6,7,9}  - WARNING returned list should be deallocated !
 */
list<int> *MEDMEM::SUPPORT::sub(const int *ids,int lgthIds,const int *idsToSuppress,int lgthIdsToSuppress)
{
  list<int> *ret;
  int i,j=0;
  if(lgthIds<0)
    throw MEDEXCEPTION("MEDMEM::SUPPORT::sub");
  else if(lgthIds==0)
    return 0;
  ret=new list<int>;
  int *temp1=new int[lgthIds];
  memcpy(temp1,ids,sizeof(int)*lgthIds);
  qsort(temp1,lgthIds,sizeof(int),compareId);
  int *temp2=new int[lgthIdsToSuppress];
  memcpy(temp2,idsToSuppress,sizeof(int)*lgthIdsToSuppress);
  qsort(temp2,lgthIdsToSuppress,sizeof(int),compareId);
  for(i=0;i<lgthIds;)
    {
      if(j>=lgthIdsToSuppress)
	  ret->push_back(temp1[i++]);
      else if(temp1[i]>temp2[j])
	j++;
      else if(temp1[i]<temp2[j])
	ret->push_back(temp1[i++]);
      else
	i++;
    }
  delete [] temp1;
  delete [] temp2;
  return ret;
}

/*!
  returns a new SUPPORT (responsability to caller to destroy it)
  that is the complement to "this", lying on the same entity than "this".
 */
SUPPORT *MEDMEM::SUPPORT::getComplement() const
{
  SUPPORT *ret;
  const int nbOfElt=_mesh->getNumberOfElements(_entity,MED_EN::MED_ALL_ELEMENTS);
  int nbOfEltInSupp=getNumberOfElements(MED_EN::MED_ALL_ELEMENTS);
  if(_isOnAllElts || nbOfElt==nbOfEltInSupp)
    {
      ret=new SUPPORT;
      ret->setMesh(_mesh);
      ret->setEntity(_entity);
      string name="Complement of ";
      name+=_name;
      ret->setName(name);
      return ret;
    }
  const int *nbs=_number->getValue();
  list<int> *ids=sub(1,nbOfElt,nbs,nbOfEltInSupp);
  if(_entity==MED_EN::MED_NODE)
    ret=_mesh->buildSupportOnNodeFromElementList(*ids,_entity);
  else
    ret=_mesh->buildSupportOnElementsFromElementList(*ids,_entity);
  delete ids;
  return ret;
}

/*!
  returns a new support the user should delete.
 */
SUPPORT *MEDMEM::SUPPORT::substract(const SUPPORT& other) const throw (MEDEXCEPTION)
{
  const char * LOC = "SUPPORT *MEDMEM::subtract(const SUPPORT * other) : ";
  BEGIN_OF(LOC);
  SUPPORT *ret;
  if (_entity!=other.getEntity())
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Entities are different !"));
  if(!(*_mesh == *other.getMesh()))
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Mesh are different !"));
  if(other._isOnAllElts)
    {
      ret=new SUPPORT;
      ret->setMesh(_mesh);
      ret->setEntity(_entity);
      return ret;
    }
  if(_isOnAllElts)
    return other.getComplement();
  int nbOfEltInThis=getNumberOfElements(MED_EN::MED_ALL_ELEMENTS);
  const int *nbsThis=_number->getValue();
  int nbOfEltInOther=other.getNumberOfElements(MED_EN::MED_ALL_ELEMENTS);
  const int *nbsOther=other._number->getValue();
  list<int> *ids=sub(nbsThis,nbOfEltInThis,nbsOther,nbOfEltInOther);
  if(_entity==MED_EN::MED_NODE)
    ret=_mesh->buildSupportOnNodeFromElementList(*ids,_entity);
  else
    ret=_mesh->buildSupportOnElementsFromElementList(*ids,_entity);
  delete ids;
  return ret;
  END_OF(LOC);
}

/*!
  returns a new support the user has to delete. Entity is either MED_NODE to obtain node elements lying on boundary of "this"
  or MED_FACE,MED_EDGE (depends on the this->_mesh dimension).
 */
SUPPORT *MEDMEM::SUPPORT::getBoundaryElements(MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION)
{
  const char * LOC = "SUPPORT *MEDMEM::SUPPORT::getBoundaryElements(MED_EN::medEntityMesh Entity) : ";
  BEGIN_OF(LOC);
  int spaceDimension=_mesh->getSpaceDimension();
  MED_EN::medEntityMesh baseEntity=Entity;
  if (spaceDimension == 3)
    if (Entity!=MED_FACE)
      if(Entity==MED_NODE)
	baseEntity=MED_FACE;
      else
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined in 3D mesh for entity "<<Entity<<" !"));
  if (spaceDimension == 2)
    if (Entity!=MED_EDGE)
      if(Entity==MED_NODE)
	baseEntity=MED_EDGE;
      else
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined in 2D mesh for entity "<<Entity<<" !"));
  if(_isOnAllElts)
    return _mesh->getBoundaryElements(Entity);

  const int * myConnectivityValue=_mesh->getReverseConnectivity(MED_DESCENDING);
  const int * myConnectivityIndex=_mesh->getReverseConnectivityIndex(MED_DESCENDING);
  int numberOf=_mesh->getNumberOfElements(baseEntity,MED_ALL_ELEMENTS);
  const int *ids=_number->getValue();
  set<int> idsSet(ids,ids+_totalNumberOfElements);
  list<int> myElementsList;
  for (int i=0;i<numberOf;i++)
    {
      int nbOfDP1EntitySharing=0;
      if(idsSet.find(myConnectivityValue[myConnectivityIndex[i]-1])!=idsSet.end())
	nbOfDP1EntitySharing++;
      if(idsSet.find(myConnectivityValue[myConnectivityIndex[i]])!=idsSet.end())
	nbOfDP1EntitySharing++;
      if(nbOfDP1EntitySharing==1)
	myElementsList.push_back(i+1);
    }
  if(Entity==MED_NODE)
    {
      return _mesh->buildSupportOnNodeFromElementList(myElementsList,baseEntity);
    }
  else
    {
      return _mesh->buildSupportOnElementsFromElementList(myElementsList,baseEntity);
    }
}

/*!
  Method that fills this and updates all its attributes in order to lye on the the listOfNode.
 */
void MEDMEM::SUPPORT::fillFromNodeList(const list<int>& listOfNode) throw (MEDEXCEPTION)
{
  setEntity(MED_EN::MED_NODE);
  clearDataOnNumbers();
  int size=listOfNode.size();
  int totalNbInMesh=_mesh->getNumberOfElements(_entity,MED_ALL_ELEMENTS);
  if(totalNbInMesh==size)
    {
    _isOnAllElts=true;
    update();
    return;
    }
  else
    _isOnAllElts=false;
  int numberOfGeometricType=1;
  medGeometryElement* geometricType=new medGeometryElement[1];
  geometricType[0]=MED_NONE;
  int *numberOfGaussPoint=new int[1];
  numberOfGaussPoint[0]=1;
  int *numberOfElements=new int[1];
  numberOfElements[0]=size;
  int *mySkyLineArrayIndex=new int[2];
  mySkyLineArrayIndex[0]=1;
  mySkyLineArrayIndex[1]=1+numberOfElements[0];
  int *tab=new int[numberOfElements[0]];
  int i=0;
  for(list<int>::const_iterator iter2=listOfNode.begin();iter2!=listOfNode.end();iter2++)
    tab[i++]=*iter2;
  MEDSKYLINEARRAY * mySkyLineArray = new MEDSKYLINEARRAY(1,numberOfElements[0],mySkyLineArrayIndex,tab,true);
  setNumberOfGeometricType(numberOfGeometricType);
  setGeometricType(geometricType);
  setNumberOfGaussPoint(numberOfGaussPoint);
  setNumberOfElements(numberOfElements);
  setTotalNumberOfElements(numberOfElements[0]);
  setNumber(mySkyLineArray);

  delete[] numberOfElements;
  delete[] numberOfGaussPoint;
  delete[] geometricType;
}

/*
  Method created to factorize code. This method fills the current SUPPORT on entity 'entity' containing all the entities contained in
  elements 'listOfElt' of entity 'entity'. Warning this method should be called after both the attributes this->_mesh and this->_entity are correctly set.
 */
void MEDMEM::SUPPORT::fillFromElementList(const list<int>& listOfElt) throw (MEDEXCEPTION)
{
  clearDataOnNumbers();
  int size=listOfElt.size();
  int totalNbInMesh=_mesh->getNumberOfElementsWithPoly(_entity,MED_ALL_ELEMENTS);
  if(totalNbInMesh==size){
    _isOnAllElts=true;
    update();
    return;
  }
  else
  _isOnAllElts=false;
  // Well, we must know how many geometric type we have found
  int * myListArray = new int[size] ;
  int id = 0 ;
  list<int>::const_iterator myElementsListIt ;
  for (myElementsListIt=listOfElt.begin();myElementsListIt!=listOfElt.end();myElementsListIt++)
    myListArray[id++]=(*myElementsListIt) ;
  int numberOfGeometricType ;
  medGeometryElement* geometricType ;
  int * numberOfGaussPoint ;
  int * numberOfElements ;
  int * mySkyLineArrayIndex ;

  int numberOfType = _mesh->getNumberOfTypesWithPoly(_entity) ;
  if (numberOfType == 1) {
    numberOfGeometricType = 1 ;
    geometricType = new medGeometryElement[1] ;
    medGeometryElement *  allType = _mesh->getTypesWithPoly(_entity);
    geometricType[0] = allType[0] ;
    numberOfGaussPoint = new int[1] ;
    numberOfGaussPoint[0] = 1 ;
    numberOfElements = new int[1] ;
    numberOfElements[0] = size ;
    mySkyLineArrayIndex = new int[2] ;
    mySkyLineArrayIndex[0]=1 ;
    mySkyLineArrayIndex[1]=1+size ;
    delete [] allType;
  }
  else {// hemmm
    map<medGeometryElement,int> theType ;
    for (myElementsListIt=listOfElt.begin();myElementsListIt!=listOfElt.end();myElementsListIt++) {
      medGeometryElement myType = _mesh->getElementTypeWithPoly(_entity,*myElementsListIt) ;
      if (theType.find(myType) != theType.end() )
	theType[myType]+=1 ;
      else
	theType[myType]=1 ;
    }
    numberOfGeometricType = theType.size() ;
    geometricType = new medGeometryElement[numberOfGeometricType] ;
    numberOfGaussPoint = new int[numberOfGeometricType] ;
    numberOfElements = new int[numberOfGeometricType] ;
    mySkyLineArrayIndex = new int[numberOfGeometricType+1] ;
    int index = 0 ;
    mySkyLineArrayIndex[0]=1 ;
    map<medGeometryElement,int>::iterator theTypeIt ;
    for (theTypeIt=theType.begin();theTypeIt!=theType.end();theTypeIt++) {
      geometricType[index] = (*theTypeIt).first ;
      numberOfGaussPoint[index] = 1 ;
      numberOfElements[index] = (*theTypeIt).second ;
      mySkyLineArrayIndex[index+1]=mySkyLineArrayIndex[index]+numberOfElements[index] ;
      index++ ;
    }
  }
  MEDSKYLINEARRAY * mySkyLineArray = new MEDSKYLINEARRAY(numberOfGeometricType,size,mySkyLineArrayIndex,myListArray,true) ;
  setNumberOfGeometricType(numberOfGeometricType) ;
  setGeometricType(geometricType) ;
  setNumberOfGaussPoint(numberOfGaussPoint) ;
  setNumberOfElements(numberOfElements) ;
  setTotalNumberOfElements(size) ;
  setNumber(mySkyLineArray) ;

  delete[] numberOfElements;
  delete[] numberOfGaussPoint;
  delete[] geometricType;
}

/*! set the reference _mesh to Mesh */
//--------------------------------------
void SUPPORT::setMesh(MESH *Mesh) const
//--------------------------------------
{
  if(_mesh)
    _mesh->removeReference();
  _mesh=Mesh;
  _meshName = "";
  if(_mesh)
    _mesh->addReference();
}

/*!
  addReference : reference counter presently disconnected in C++ -> just connected for client.
*/
void MEDMEM::SUPPORT::addReference() const
{
}

/*!
  removeReference : reference counter presently disconnected in C++ -> just connected for client.
*/
void MEDMEM::SUPPORT::removeReference() const
{
}
