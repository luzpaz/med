//  MED MEDMEM : MED files in memory
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : MEDMEM_Support.hxx
//  Module : MED

/*
  File Support.hxx
  $Header$
*/

#ifndef SUPPORT_HXX
#define SUPPORT_HXX

#include <vector>
#include <string>

#include "utilities.h"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

#include "MEDMEM_SkyLineArray.hxx"
//#include "MEDMEM_Mesh.hxx"

using namespace MED_EN;

class MESH ;

/*!

  This class describe a support of elements on an entity of the mesh.

  It contains the list of meshes elements for an entity (MED_NODE, 
  MED_CELL, MED_FACE or MED_EDGE).

*/

class SUPPORT
{
protected:
  /*!
    \if developper
    Support name.
    \endif
  */
  string                   _name;

  /*!
    \if developper
    Description of the support (optional).
    \endif
  */
  string                   _description;

  /*!
    \if developper
    Reference to the mesh on which the support is defined.
    \endif
  */
  MESH *                   _mesh;

  /*!
    \if developper
    Type of entity on which the support is defined
    (only one for each support).
    \endif
  */
  medEntityMesh            _entity ;

  /*!
    \if developper
    Number of geometric type defined in the support.
    \endif
  */
  int                      _numberOfGeometricType;

  /*!
    \if developper
    Array of all geometric type defined in the support.
    \endif
  */
  medGeometryElement *     _geometricType;

  /*!
    \if developper
    Array of size _numberOfGeometricType which contains
    for each type the number of gauss point
    (not yet implemented).
    \endif
  */
  int *                    _numberOfGaussPoint ;

   /*
    \if developper
    Array of size _numberOfGeometricType
    which contains number of geometric
    entity type in Mesh
    (to get corresponding CellModel).
    \endif
  */
  //int *                    _geometricTypeNumber;

  /*!
    \if developper
    If true, we consider all entities of type _entity.
    \endif
  */
  bool                     _isOnAllElts; 

  /*!
    \if developper
    Index array of size _numberOfGeometricType wich contains
    for each geometric type, the number of elements of this type.
    \endif
  */
  int *                    _numberOfElements;

  /*!
    \if developper
    Sum of each _numberOfElements component.
    \endif
  */
  int                      _totalNumberOfElements;

  // the two following arrays are defined only if _isOnAllElts is false :

  /*
    \if developper
    array of size _numberOfType+1 wich contains for
    each geometric type, index in _number._value
    (if _all is true, we must ask _mesh to get
    information). _typeIndex[i+1]-_typeIndex[i]
    represents count of entities of ith geometric
    type. _typeIndex[_numberOfType] contains total
    entities count. If _numberOf[i]=0,
    _typeIndex[i+1]=_typeIndex[i]
    defined only if _isOnAllElts is false
    \endif
  */
  //  int *      _typeIndex;

  /*!
    \if developper
    Array of size _index[_numberOfType]-1 wich contain number of
    entities of each geometric type. We use global numbering.\n
    Defined only if _isOnAllElts is false.
    \endif
  */
  MEDSKYLINEARRAY * _number;

public:
  SUPPORT();
  SUPPORT(MESH* Mesh, string Name="", medEntityMesh Entity=MED_CELL);
  SUPPORT(const SUPPORT & m);
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
  inline void setGeometricType(const medGeometryElement *GeometricType);
  inline void setNumberOfGaussPoint(const int *NumberOfGaussPoint);
  //  inline void setGeometricTypeNumber(int *GeometricTypeNumber);
  inline void setNumberOfElements(const int *NumberOfElements);
  inline void setTotalNumberOfElements(int TotalNumberOfElements);
  inline void setNumber(MEDSKYLINEARRAY * Number);
  inline void setNumber(const int * index, const int* value);

  inline string getName() const;
  inline string getDescription() const;
  inline MESH * getMesh() const;
  inline medEntityMesh getEntity() const;

  inline bool   isOnAllElements() const;
  inline int    getNumberOfTypes() const;
  inline const medGeometryElement* getTypes() const ;
  inline const int *  getNumberOfGaussPoint() const throw (MEDEXCEPTION);
  inline int          getNumberOfGaussPoint(medGeometryElement geomElement) const throw (MEDEXCEPTION);
  //inline int *  getGeometricTypeNumber() const;
  //inline int    getTotalNumberOfElement() const;
  inline int    getNumberOfElements(medGeometryElement GeometricType) const throw (MEDEXCEPTION);
  inline MEDSKYLINEARRAY *  getnumber() const throw (MEDEXCEPTION);
  inline const int *  getNumber(medGeometryElement GeometricType) const throw (MEDEXCEPTION);
  inline const int *  getNumberIndex() const throw (MEDEXCEPTION);

  void blending(SUPPORT * mySupport);

  void setpartial(string Description, int NumberOfGeometricType,
		  int TotalNumberOfEntity, medGeometryElement *GeometricType,
		  int *NumberOfEntity, int *NumberValue);

  void getBoundaryElements() throw (MEDEXCEPTION);
};
// _____________________
// Methodes Inline
// _____________________

/*!
  If isOnAllElements is false, returns number of elements in the
  support else returns number of nodes.

  Example : number of MED_TRIA3 or MED_ALL_ELEMENTS elements
  in entity of support.

  Note : If SUPPORT is defined on MED_NODE, use MED_NONE
  medGeometryElement type.
*/
//-----------------------------------------------------------------------------
inline int SUPPORT::getNumberOfElements(medGeometryElement GeometricType) const
  throw (MEDEXCEPTION)
//-----------------------------------------------------------------------------
{
  if (GeometricType==MED_ALL_ELEMENTS)
    return _totalNumberOfElements;
  for (int i=0;i<_numberOfGeometricType;i++)
    if (_geometricType[i]==GeometricType)
      return _numberOfElements[i];
  throw MEDEXCEPTION("Support::getNumberOfElements : Geometric type not found !") ;
}

//---------------------------------------------------------------------
inline MEDSKYLINEARRAY * SUPPORT::getnumber() const
  throw (MEDEXCEPTION)
//---------------------------------------------------------------------
{
  if (_number==NULL)
    throw MEDEXCEPTION("Support::getnumber : Not defined !") ;
  return _number ;
}

/*!
  If isOnAllElements is false, returns an array which contains
  all number of given medGeometryElement.

  Numbering is global, ie numbers are bounded by 1 and
  MESH::getNumberOfElement(entity,MED_ALL_ELEMENTS) and not by 1 and
  MESH::getNumberOfElement(entity,geomElement).

  Note : If SUPPORT is defined on MED_NODE, use MED_NONE
  medGeometryElement type.
*/
//---------------------------------------------------------------------
inline const int * SUPPORT::getNumber(medGeometryElement GeometricType) const
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

/*!
  If isOnAllElements is false, returns index of element number.
  Use it with getNumber(MED_ALL_ELEMENTS).

  Note : See getConnectivityIndex for details.
*/
//-------------------------------------------
inline const int * SUPPORT::getNumberIndex() const
//-------------------------------------------
  throw (MEDEXCEPTION)
{
  if (_isOnAllElts)
    throw MEDEXCEPTION("Support::getNumberIndex : Not defined, support is on all entity !") ;
  return _number->getIndex() ;
}

/*! A DOCUMENTER */
//-------------------------------------------------
inline const int * SUPPORT::getNumberOfGaussPoint() const
  throw (MEDEXCEPTION)
//-------------------------------------------------
{
  if (_numberOfGaussPoint!=NULL)
    return _numberOfGaussPoint ;
  else
    throw MEDEXCEPTION("Support::getNumberOfGaussPoint : Not defined !") ;
}

/*!
  Returns number of Gauss points for this medGeometryElement.

  Note :
  - Not defined if SUPPORT is on MED_NODE.
  - Not defined for MED_ALL_ELEMENTS medGeometryElement type.
*/
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

/*! set the attribute _name to Name */
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

/*! set the reference _mesh to Mesh */
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
//   if ( Entity == MED_NODE) {
//     _numberOfGeometricType=1 ;
//     if (_geometricType == (medGeometryElement *) NULL)
//       _geometricType=new medGeometryElement[1] ;
//     else 
//       {
// 	// delete previous ???
// 	delete [] _geometricType;
// 	_geometricType=new medGeometryElement[1] ;
//       }
//     _geometricType[0]=MED_NONE ;
//   }
}

/*! set the attribute _numberOfGeometricType to NumberOfGeometricType */
//---------------------------------------------------------------------
inline void SUPPORT::setNumberOfGeometricType(int NumberOfGeometricType)
//---------------------------------------------------------------------
{
  _numberOfGeometricType=NumberOfGeometricType;
  if (_geometricType!=NULL) {
    delete[] _geometricType ;
    _geometricType = NULL ;
  }
  if (_numberOfElements!=NULL) {
    delete[] _numberOfElements ;
    _numberOfElements = NULL ;
  }
  if (_numberOfGaussPoint!=NULL) {
    delete[] _numberOfGaussPoint ;
    _numberOfGaussPoint = NULL ;
  }
}

/*! set the attribute _geometricType to geometricType */
//---------------------------------------------------------------------
inline void SUPPORT::setGeometricType(const medGeometryElement *GeometricType)
//---------------------------------------------------------------------
{
  if (NULL == _geometricType)
    _geometricType=new medGeometryElement[_numberOfGeometricType];
  for (int i=0;i<_numberOfGeometricType;i++)
    _geometricType[i] = GeometricType[i];
  //  _geometricType=GeometricType;
}

/*! set the attribute _numberOfGaussPoint to NumberOfGaussPoint */
//-----------------------------------------------------------------
inline void SUPPORT::setNumberOfGaussPoint(const int *NumberOfGaussPoint)
//-----------------------------------------------------------------
{
  if (NULL == _numberOfGaussPoint)
    _numberOfGaussPoint=new int[_numberOfGeometricType];
  for (int i=0;i<_numberOfGeometricType;i++)
    _numberOfGaussPoint[i] = NumberOfGaussPoint[i];
  //  _numberOfGaussPoint = NumberOfGaussPoint ;
}

/*! set the attribute _geometricTypeNumber to GeometricTypeNumber */
//-------------------------------------------------------------------
//inline void SUPPORT::setGeometricTypeNumber(int *GeometricTypeNumber)
//-------------------------------------------------------------------
//{
//  _geometricTypeNumber=GeometricTypeNumber;
//}

/*!
  Set the attribute _numberOfElements to NumberOfElements and 
  calculate the total number of elements.
*/
//----------------------------------------------------------
inline void SUPPORT::setNumberOfElements(const int *NumberOfElements)
//----------------------------------------------------------
{
  if (NULL == _numberOfElements)
    _numberOfElements=new int[_numberOfGeometricType];
  memcpy(_numberOfElements,NumberOfElements,sizeof(int)*_numberOfGeometricType);
  _totalNumberOfElements = 0 ;
  for (int i=0;i<_numberOfGeometricType;i++)
    _totalNumberOfElements+=_numberOfElements[i];
}

/*! set the attribute _totalNumberOfElements to TotalNumberOfElements */
//--------------------------------------------------------------------
inline void SUPPORT::setTotalNumberOfElements(int TotalNumberOfElements)
//--------------------------------------------------------------------
{
  _totalNumberOfElements=TotalNumberOfElements;
}

/*! set the attribute _number to Number */
//---------------------------------------------------
inline void SUPPORT::setNumber(MEDSKYLINEARRAY * Number)
//---------------------------------------------------
{
  if (_number != NULL) delete _number ;
  _number=Number;
}

/*! set the attribute _number with index and value arrays */
//---------------------------------------------------
inline void SUPPORT::setNumber(const int * index, const int* value)
//---------------------------------------------------
{
  if (_number != NULL) delete _number ;
  _number= new MEDSKYLINEARRAY(_numberOfGeometricType,_totalNumberOfElements,index,value);
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

/*! returns a reference to the mesh */
//------------------------------------
inline MESH * SUPPORT::getMesh() const
//------------------------------------
{
  return _mesh;
}

/*!
  Returns true if all elements of this entity are
  concerned, false otherwise.
  If true, you must use mesh reference (getMesh) to get more information.
*/
//------------------------------------------
inline bool SUPPORT::isOnAllElements() const
//------------------------------------------
{
  return _isOnAllElts;
}

/*!
  Returns number of geometric Types
  defines in the support
*/
//------------------------------------------
inline int SUPPORT::getNumberOfTypes() const
//------------------------------------------
{
  //    if ((_isOnAllElts)&(_entity != MED_NODE))
  //      return _mesh->getNumberOfTypes(_entity) ;
  //    else
  return _numberOfGeometricType ;
}

/*!
  Returns the medEntityMesh's type used by the support.
  Note : A support deals only with one entity's type
  (for example : MED_FACE or MED_NODE)
*/
//---------------------------------------------
inline medEntityMesh SUPPORT::getEntity() const
//---------------------------------------------
{
  return _entity;
}

/*!
  If isOnAllElements is false, returns an array of <medGeometryElement>
  types used by the support.

  <medEntityMesh> is given by getEntity.
*/
//---------------------------------------------------
inline const medGeometryElement * SUPPORT::getTypes() const
//---------------------------------------------------
{
  //    if ((_isOnAllElts)&(_entity != MED_NODE))
  //      return _mesh->getTypes(_entity) ;
  //    else
  return _geometricType;
}

//---------------------------------------------------
//inline int * SUPPORT::getGeometricTypeNumber() const
//---------------------------------------------------
//  {
//    const char * LOC = "SUPPORT::getGeometricTypeNumber() : ";
//    if (_isOnAllElts)
//      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined (is on all elements) !"));
//    if (_geometricTypeNumber==NULL)
//      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined !"));
//    return _geometricTypeNumber;
//  }


#endif /* SUPPORT_HXX */
