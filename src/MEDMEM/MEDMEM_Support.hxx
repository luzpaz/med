/*
 File Support.hxx
 $Header$
*/

#ifndef SUPPORT_HXX
#define SUPPORT_HXX

#include <vector>
#include <string>

#include "utilities.h"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

#include "MEDMEM_SkyLineArray.hxx"
#include "MEDMEM_Mesh.hxx"

using namespace MED_EN;

class MESH ;

class SUPPORT 
{
protected:
					         /*! name of the support */
  string                   _name;
					         /*! description of the support (optionnal) */
  string                   _description;
						 /*! reference to the mesh on which the support is defined */
  MESH *                   _mesh;
						 /*! type of entity on which the support is defined 
						     (only one for each support) */
  medEntityMesh            _entity ;
						 /*! number of geometric type defined in the support */
  int                      _numberOfGeometricType;
						 /*! array of all geometric type defined in the support */
  medGeometryElement *     _geometricType;
						 /*! array of size _numberOfGeometricType which contains 
						     for each type the number of gauss point 
						     (not yet implemented) */
  int *                    _numberOfGaussPoint ; 
						 /*! array of size _numberOfGeometricType 
						     which contains number of geometric
						     entity type in Mesh 
						     (to get corresponding CellModel)*/
  int *                    _geometricTypeNumber; 
						 /*! If true, we consider all entities of type _entity */
  bool       		   _isOnAllElts; 	
						 /*! array of size _numberOfGeometricType wich contains
			                 	     for each geometric type, count of entities. 
						     (if _isOnAllElts is true, we could get same 
						     information from _mesh) */
  int *      		   _numberOfEntities;   
						 /*! sum of each _numberOfEntities component (if 
						     _isOnAllElts is true, we could get same 
						     information from _mesh) */
  int        		   _totalNumberOfEntities;    

  // the two following arrays are defined only if _isOnAllElts is false :

			     /* array of size _numberOfType+1 wich contains for 
                                 each geometric type, index in _number._value 
                                 (if _all is true, we must ask _mesh to get 
                                 information). _typeIndex[i+1]-_typeIndex[i] 
                                 represents count of entities of ith geometric 
                                 type. _typeIndex[_numberOfType] contains total 
                                 entities count. If _numberOf[i]=0, 
                                 _typeIndex[i+1]=_typeIndex[i] 
				 defined only if _isOnAllElts is false*/
  //  int *      _typeIndex; 
			     /*! array of size _index[_numberOfType] wich contain number of 
                                 entities of each geometric type. We use global numbering.
				 defined only if _isOnAllElts is false*/
  MEDSKYLINEARRAY * _number;   

public:
  SUPPORT(); 
  SUPPORT(MESH* Mesh, string Name="", medEntityMesh Entity=MED_CELL); 
  ~SUPPORT();
  friend ostream & operator<<(ostream &os,const SUPPORT &my);
  
  // function to set all value when SUPPORT was created by MedMedDriver without all MESH information !!! Change with new API !
  void update();
  
  inline void setName(string Name);
  inline void setDescription(string Description); 
  inline void setMesh(MESH *Mesh); 		 
  inline void setAll(bool All); 			 
  inline void setEntity(medEntityMesh Entity); 
  inline void setNumberOfGeometricType(int NumberOfGeometricType); 
  inline void setGeometricType(medGeometryElement *GeometricType); 
  inline void setNumberOfGaussPoint(int *NumberOfGaussPoint);
  inline void setGeometricTypeNumber(int *GeometricTypeNumber); 
  inline void setNumberOfEntities(int *NumberOfEntities); 
  inline void setTotalNumberOfEntities(int TotalNumberOfEntities); 
  inline void setNumber(MEDSKYLINEARRAY * Number); 
  
  inline string getName() const; 
  inline string getDescription() const; 
  inline MESH * getMesh() const; 
  inline medEntityMesh getEntity() const;

  inline bool   isOnAllElements() const; 
  inline int    getNumberOfTypes() const;
  inline medGeometryElement* getTypes() const ;
  inline int *  getNumberOfGaussPoint() const throw (MEDEXCEPTION);
  inline int    getNumberOfGaussPoint(medGeometryElement geomElement) const throw (MEDEXCEPTION);
  inline int *  getGeometricTypeNumber() const; 
  //  inline int    getNumberOfTotalEntity() const;
  inline int    getNumberOfElements(medGeometryElement GeometricType) const throw (MEDEXCEPTION);
  inline MEDSKYLINEARRAY *  getnumber() const throw (MEDEXCEPTION);
  inline int *  getNumber(medGeometryElement GeometricType) const throw (MEDEXCEPTION);
  inline int *  getNumberIndex() const throw (MEDEXCEPTION);

  void blending(SUPPORT * mySupport);

  /*
    This function allows the user to set a support not on all entities Entity,
    it should be used after an initialisation with the constructor
    SUPPORT(MESH* Mesh, string Name="", medEntityMesh Entity=MED_CELL) and
    after the call to the function setAll(false).
    It allocates and initialises all the attributs of the class SUPPORT.
  */

  void setpartial(string Description, int NumberOfGeometricType,
		  int TotalNumberOfEntity, medGeometryElement *GeometricType,
		  int *NumberOfEntity, int *NumberValue);
};
// _____________________
// Methodes Inline
// _____________________

/*!  If isOnAllElements is false, returns number of elements in the
     support else returns number of nodes.

      Example : number of MED_TRIA3 or MED_ALL_ELEMENTS elements
      in entity of support.

      Note : If SUPPORT is defined on MED_NODE, use MED_NONE
      medGeometryElement type.  */
//-----------------------------------------------------------------------------
inline int SUPPORT::getNumberOfElements(medGeometryElement GeometricType) const
throw (MEDEXCEPTION) 
//-----------------------------------------------------------------------------
{
  if (_isOnAllElts){ 
    return _mesh->getNumberOfElements(_entity,GeometricType);
  } else {
    if (GeometricType==MED_ALL_ELEMENTS)
      return _totalNumberOfEntities;
    for (int i=0;i<_numberOfGeometricType;i++)
      if (_geometricType[i]==GeometricType)
	return _numberOfEntities[i] ;
    throw MEDEXCEPTION("Support::getNumberOfElements : Type not found !") ;
  }
}
//  inline int SUPPORT::getNumberOf(medGeometryElement GeometricType) const 
//  throw (MEDEXCEPTION) 
//  {
//      if (GeometricType==MED_ALL_ELEMENTS)
//          return _totalNumberOfEntities ; 
//      for (int i=0;i<_numberOfGeometricType;i++)
//          if (_geometricType[i]==GeometricType)
//      return _numberOfEntities[i] ;
//      throw MEDEXCEPTION("Support::getNumberOf: GeometricType not found !");
//  }

//  inline int SUPPORT::getNumberOfTotalEntity() const 
//  { 
//    return _totalNumberOfEntities ; 
//  }

//---------------------------------------------------------------------
inline MEDSKYLINEARRAY * SUPPORT::getnumber() const 
  throw (MEDEXCEPTION) 
//---------------------------------------------------------------------
{
  if (_number==NULL) 
    throw MEDEXCEPTION("Support::getnumber : Not defined !") ;
  return _number ;
}

/*!   If isOnAllElements is false, returns an array which contains
      all number of given medGeometryElement.

      Numbering is global, ie numbers are bounded by 1 and
      MESH::getNumberOfElement(entity,MED_ALL_ELEMENTS) and not by 1 and
      MESH::getNumberOfElement(entity,geomElement).

      Note : If SUPPORT is defined on MED_NODE, use MED_NONE
      medGeometryElement type. */
//---------------------------------------------------------------------
inline int * SUPPORT::getNumber(medGeometryElement GeometricType) const 
  throw (MEDEXCEPTION) 
//---------------------------------------------------------------------
{
  const char * LOC = "Support::getNumber : " ;
  if (_isOnAllElts) 
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined, support is on all entity !")) ;
  if (GeometricType==MED_ALL_ELEMENTS)
    return _number->getValue() ; 
  for (int i=0;i<_numberOfGeometricType;i++)
    if (_geometricType[i]==GeometricType)
      return _number->getI(i+1) ;
  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"GeometricType not found !")) ;
}

/*!  If isOnAllElements is false, returns index of element number.
      Use it with getNumber(MED_ALL_ELEMENTS).

      Note : See getConnectivityIndex for details.  */
//-------------------------------------------
inline int * SUPPORT::getNumberIndex() const 
//-------------------------------------------
throw (MEDEXCEPTION) 
{
    if (_isOnAllElts) 
      throw MEDEXCEPTION("Support::getNumberIndex : Not defined, support is on all entity !") ;
      return _number->getIndex() ;
}

/*! A DOCUMENTER */
//-------------------------------------------------
inline int * SUPPORT::getNumberOfGaussPoint() const 
throw (MEDEXCEPTION) 
//-------------------------------------------------
{
  if (_numberOfGaussPoint!=NULL)
    return _numberOfGaussPoint ;
  else
    throw MEDEXCEPTION("Support::getNumberOfGaussPoint : Not defined !") ;
}

/*!  Returns number of Gauss points for this medGeometryElement.

      Note :
      - Not defined if SUPPORT is on MED_NODE.
      - Not defined for MED_ALL_ELEMENTS medGeometryElement type.  */
//-----------------------------------------------------------------------------
inline int SUPPORT::getNumberOfGaussPoint(medGeometryElement geomElement) const 
throw (MEDEXCEPTION) 
//-----------------------------------------------------------------------------
{
  if (_numberOfGaussPoint!=NULL) {
    for (int i=0;i<_numberOfGeometricType;i++)
      if (_geometricType[i]==geomElement)
	return _numberOfGaussPoint[i] ;
    throw MEDEXCEPTION("Support::getGlobalNumber : GeometricType not found !") ;
  } else
    throw MEDEXCEPTION("Support::getNumberOfGaussPoint : Not defined !") ;
}
//  inline int SUPPORT::getNumberLength() const 
//  throw (MEDEXCEPTION) 
//  {
//      if (_isOnAllElts) 
//        throw MEDEXCEPTION("Support::getNumberLength : Not defined, support is on all entity !") ;
//        return _number->getLength() ;
//  }

/*! set the attribute _name to Name   */
//--------------------------------------
inline void SUPPORT::setName(string Name)
//--------------------------------------
{ 
  _name=Name; 
}

/*! set the attribute _description to Description */
//--------------------------------------------------
inline void SUPPORT::setDescription(string Description)
//--------------------------------------------------
{ 
  _description=Description; 
}

/*! set the reference _mesh to Mesh   */
//--------------------------------------
inline void SUPPORT::setMesh(MESH *Mesh)
//--------------------------------------
{ 
  _mesh=Mesh; 
}

/*! set the attribute _isOnAllElts to All */
//------------------------------------------
inline void SUPPORT::setAll(bool All)
//------------------------------------------
{ 
  _isOnAllElts=All; 
}

/*! set the attribute _entity to Entity */
//------------------------------------------
inline void SUPPORT::setEntity(medEntityMesh Entity)
{ 
  _entity=Entity; 
  if ( Entity == MED_NODE) {
    _numberOfGeometricType=1 ;
    _geometricType=new medGeometryElement[1] ; // delete previous ???
    _geometricType[0]=MED_NONE ;
  }
}

/*! set the attribute _numberOfGeometricType to NumberOfGeometricType */
//---------------------------------------------------------------------
inline void SUPPORT::setNumberOfGeometricType(int NumberOfGeometricType)
//---------------------------------------------------------------------
{ 
  _numberOfGeometricType=NumberOfGeometricType; 
}

/*! set the attribute _geometricType to geometricType                */
//---------------------------------------------------------------------
inline void SUPPORT::setGeometricType(medGeometryElement *GeometricType)
//---------------------------------------------------------------------
{ 
  _geometricType=GeometricType; 
}

/*! set the attribute _numberOfGaussPoint to NumberOfGaussPoint  */
//-----------------------------------------------------------------
inline void SUPPORT::setNumberOfGaussPoint(int *NumberOfGaussPoint)
//-----------------------------------------------------------------
{
  _numberOfGaussPoint = NumberOfGaussPoint ;
}

/*! set the attribute _geometricTypeNumber to GeometricTypeNumber  */
//-------------------------------------------------------------------
inline void SUPPORT::setGeometricTypeNumber(int *GeometricTypeNumber)
//-------------------------------------------------------------------
{ 
  _geometricTypeNumber=GeometricTypeNumber; 
}

/*! set the attribute _numberOfEntities to NumberOfEntities */
//----------------------------------------------------------
inline void SUPPORT::setNumberOfEntities(int *NumberOfEntities)
//----------------------------------------------------------
{ 
  _numberOfEntities=NumberOfEntities; 
}

/*! set the attribute _totalNumberOfEntities to TotalNumberOfEntities */
//--------------------------------------------------------------------
inline void SUPPORT::setTotalNumberOfEntities(int TotalNumberOfEntities)
//--------------------------------------------------------------------
{ 
  _totalNumberOfEntities=TotalNumberOfEntities; 
}

/*! set the attribute _number to Number           */
//---------------------------------------------------
inline void SUPPORT::setNumber(MEDSKYLINEARRAY * Number)
//---------------------------------------------------
{ 
  _number=Number; 
}

/*! returns the name of the support. */
//------------------------------------
inline string SUPPORT::getName() const 
//------------------------------------
{ 
  return _name; 
}


/*! returns the description of the support. */
//--------------------------------------------
inline string SUPPORT::getDescription() const 
//--------------------------------------------
{ 
  return _description; 
}

/*! returns a refernce to the mesh  */
//------------------------------------
inline MESH * SUPPORT::getMesh() const 
//------------------------------------
{ 
  return _mesh; 
}

/*!  Returns true if all elements of this entity are
     concerned, false otherwise.
     If true, you must use mesh reference (getMesh) to get more information.*/
//------------------------------------------
inline bool SUPPORT::isOnAllElements() const 
//------------------------------------------
{ 
  return _isOnAllElts; 
}

/*!  Returns number of geometric Types
     defines in the support              */ 
//------------------------------------------
inline int SUPPORT::getNumberOfTypes() const 
//------------------------------------------
{ 
  if ((_isOnAllElts)&(_entity != MED_NODE))
    return _mesh->getNumberOfTypes(_entity) ;
  else
    return _numberOfGeometricType ; 
}

/*!  Returns the medEntityMesh's type used by the support.
     Note : A support deals only with one entity's type
     (for example : MED_FACE or MED_NODE)*/
//---------------------------------------------
inline medEntityMesh SUPPORT::getEntity() const 
//---------------------------------------------
{ 
  return _entity; 
}

/*!  If isOnAllElements is false, returns an array of <medGeometryElement>
     types used by the support.

     <medEntityMesh> is given by getEntity.  */
//---------------------------------------------------
inline medGeometryElement * SUPPORT::getTypes() const 
//---------------------------------------------------
{
  if ((_isOnAllElts)&(_entity != MED_NODE))
    return _mesh->getTypes(_entity) ;
  else
    return _geometricType; 
}

//---------------------------------------------------
inline int * SUPPORT::getGeometricTypeNumber() const
//---------------------------------------------------
{
  const char * LOC = "SUPPORT::getGeometricTypeNumber() : ";
  if (_isOnAllElts)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined (is on all elements) !"));
  if (_geometricTypeNumber==NULL)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined !"));
  return _geometricTypeNumber; 
}

    
#endif /* SUPPORT_HXX */
