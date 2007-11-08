// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
/*
  File Support.hxx
  $Header$
*/

#ifndef SUPPORT_HXX
#define SUPPORT_HXX

#include <list>
#include <vector>
#include <string>
#include <list>

#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_SkyLineArray.hxx"
#include "MEDMEM_RCBase.hxx"

/*!

  This class describe a support of elements on an entity of the mesh.

  It contains the list of meshes elements for an entity (MED_NODE,
  MED_CELL, MED_FACE or MED_EDGE).

*/

namespace MEDMEM {

  class MESH;

class SUPPORT : public RCBASE
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
    Name of the associated mesh if the _mesh pointer
    is NULL.
    \endif
  */
  mutable string                   _meshName;

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
  mutable MESH *                   _mesh;

  /*!
    \if developper
    Type of entity on which the support is defined
    (only one for each support).
    \endif
  */
  MED_EN::medEntityMesh            _entity ;

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
  PointerOf<MED_EN::medGeometryElement>     _geometricType;

  /*!
    \if developper
    If true, we consider all entities of type _entity
    defined in the associated mesh
    \endif
  */
  bool                     _isOnAllElts;

  /*!
    \if developper
    Index array of size _numberOfGeometricType wich contains
    for each geometric type, the number of elements of this type.
    \endif
  */
  PointerOf<int>                    _numberOfElements;

  /*!
    \if developper
    Sum of each _numberOfElements component.
    \endif
  */
  int                      _totalNumberOfElements;

  // the two following arrays are defined only if _isOnAllElts is false :

  /*!
    \if developper
    Array of size _index[_numberOfType]-1 wich contain number of
    entities of each geometric type. We use global numbering.\n
    Defined only if _isOnAllElts is false.
    \endif
  */
  mutable MEDSKYLINEARRAY * _number;

  /*!
    \if developper
    Array of size _index[_numberOfType]-1 wich contain number of
    entities of each geometric type. We use file numbering.\n
    Defined only if _isOnAllElts is false.
    \endif
  */
  mutable MEDSKYLINEARRAY * _number_fromfile;

  /*!
    \if developper
    Array of size <_numberOfGeometricType> wich contain the profil name of 
    entities of each geometric type.\n
    Defined only if _isOnAllElts is false.
    If it exist an entities list on a geometric type in _number but there is no profil name associated
    ( MED_NOPFL ) the MED driver will consider and verify this entities list as being all the
    entities available on the associated mesh for this geometric type.
    \endif
  */

  vector< string > _profilNames;

public:

  SUPPORT();
  SUPPORT(MESH* Mesh, string Name="", MED_EN::medEntityMesh Entity=MED_EN::MED_CELL);
  SUPPORT(const SUPPORT & m);
  virtual ~SUPPORT();
  friend ostream & operator<<(ostream &os,const SUPPORT &my);

  SUPPORT& operator=(const SUPPORT &support);
  bool operator == (const SUPPORT &support) const;
  bool deepCompare(const SUPPORT &support) const;
  // function to set all value when SUPPORT was created by MedMedDriver without all MESH information !!! Change with new API !
  void update();

  inline void setName(string Name);
  inline void setDescription(string Description);
  void setMesh(MESH *Mesh) const;
  void setMeshDirectly(MESH *Mesh) const { _mesh=Mesh; }
  inline void setMeshName(const string & meshName);
  inline void setAll(bool All);
  inline void setEntity(MED_EN::medEntityMesh Entity);
  inline void setNumberOfGeometricType(int NumberOfGeometricType);
  inline void setGeometricType(const MED_EN::medGeometryElement *GeometricType);
  inline void setNumberOfElements(const int *NumberOfElements);
  inline void setTotalNumberOfElements(int TotalNumberOfElements);
  inline void setNumber(MEDSKYLINEARRAY * Number);
  inline void setNumber(const int * index, const int* value, bool shallowCopy=false);

  inline string getName() const;
  inline string getDescription() const;
  virtual inline MESH * getMesh() const;
  string getMeshName() const;
  inline MED_EN::medEntityMesh getEntity() const;

  inline bool   isOnAllElements() const;
  inline int    getNumberOfTypes() const;
  inline const MED_EN::medGeometryElement* getTypes() const ;
  inline int    getNumberOfElements(MED_EN::medGeometryElement GeometricType) const throw (MEDEXCEPTION);
  inline  const int * getNumberOfElements() const throw (MEDEXCEPTION);
  virtual inline MEDSKYLINEARRAY *  getnumber() const throw (MEDEXCEPTION);
  virtual inline MEDSKYLINEARRAY *  getnumberFromFile() const throw (MEDEXCEPTION);
  virtual inline const int *  getNumber(MED_EN::medGeometryElement GeometricType) const throw (MEDEXCEPTION);
  virtual inline const int *  getNumberFromFile(MED_EN::medGeometryElement GeometricType) const throw (MEDEXCEPTION);
  virtual inline const int *  getNumberIndex() const throw (MEDEXCEPTION);
  virtual int getValIndFromGlobalNumber(const int number) const throw (MEDEXCEPTION);

  void blending(SUPPORT * mySupport) throw (MEDEXCEPTION) ;

  // Les numéros d'entités dans les profils doivent être croissant
  // pour respecter la norme MED
  void setpartial(string Description, int NumberOfGeometricType,
		  int TotalNumberOfEntity, MED_EN::medGeometryElement *GeometricType,
		  int *NumberOfEntity, int *NumberValue);

  void setpartial(MEDSKYLINEARRAY * number, bool shallowCopy=false) throw (MEDEXCEPTION);


  void setpartial_fromfile(MEDSKYLINEARRAY * number, bool shallowCopy=false) throw (MEDEXCEPTION);
  
  // Si les noms de profils ne sont pas positionnés, les profils ne seront
  // pas écrits par MEDFICHIER.
  void   setProfilNames(vector<string> profilNames) throw (MEDEXCEPTION);
  //string getProfilName(const MED_EN::medGeometryElement GeometricType) const throw (MEDEXCEPTION);
  vector<string> getProfilNames() const throw (MEDEXCEPTION);

  void getBoundaryElements() throw (MEDEXCEPTION);
  void changeElementsNbs(MED_EN::medEntityMesh entity, const int *renumberingFromOldToNew, int limitNbClassicPoly, const int *renumberingFromOldToNewPoly=0);
  void intersecting(SUPPORT * mySupport) throw (MEDEXCEPTION) ;
  bool belongsTo(const SUPPORT& other, bool deepCompare=false) const;
  SUPPORT *getComplement() const;
  SUPPORT *substract(const SUPPORT& other) const throw (MEDEXCEPTION) ;
  SUPPORT *getBoundaryElements(MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION);
  void fillFromNodeList(const list<int>& listOfNode) throw (MEDEXCEPTION);
  void fillFromElementList(const list<int>& listOfElt) throw (MEDEXCEPTION);
  void clearDataOnNumbers();
  //A.G. Addings for RC
  virtual void addReference() const;
  virtual void removeReference() const;
protected:
  static list<int> *sub(int start,int end,const int *idsToSuppress,int lgthIdsToSuppress);
  static list<int> *sub(const int *ids,int lgthIds,const int *idsToSuppress,int lgthIdsToSuppress);
};

// _____________________
// Methodes Inline
// _____________________

/*!
  This method returns the number of all elements of the type GeometricType.

  If isOnAllElements is false, it returns the number of elements in the
  support else it returns number of elements in the mesh.

  Example : number of MED_TRIA3 or MED_ALL_ELEMENTS elements
  in entity of support.

  Note : If SUPPORT is defined on MED_NODE, use MED_ALL_ELEMENTS as
         medGeometryElement GeometricType and it will returns the number
	 of nodes in the support (or in the mesh).
*/
//-----------------------------------------------------------------------------
inline int SUPPORT::getNumberOfElements(MED_EN::medGeometryElement GeometricType) const
  throw (MEDEXCEPTION)
//-----------------------------------------------------------------------------
{
  if (GeometricType==MED_EN::MED_ALL_ELEMENTS)
    return _totalNumberOfElements;
  for (int i=0;i<_numberOfGeometricType;i++)
    if (_geometricType[i]==GeometricType)
      return _numberOfElements[i];
  throw MEDEXCEPTION("Support::getNumberOfElements : Geometric type not found !") ;
}
//-----------------------------------------------------------------------------
inline  const int * SUPPORT::getNumberOfElements() const throw (MEDEXCEPTION) {
//-----------------------------------------------------------------------------
  return _numberOfElements;
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

//---------------------------------------------------------------------
inline MEDSKYLINEARRAY * SUPPORT::getnumberFromFile() const
  throw (MEDEXCEPTION)
//---------------------------------------------------------------------
{
  if (_number_fromfile==NULL)
    throw MEDEXCEPTION("Support::getnumberFromFile : Not defined !") ;
  return _number_fromfile ;
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
inline const int * SUPPORT::getNumber(MED_EN::medGeometryElement GeometricType) const
  throw (MEDEXCEPTION)
//---------------------------------------------------------------------
{
  const char * LOC = "Support::getNumber : " ;
  if (_isOnAllElts)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined, support is on all entity !")) ;
  if (GeometricType==MED_EN::MED_ALL_ELEMENTS)
    return _number->getValue() ;
  for (int i=0;i<_numberOfGeometricType;i++)
    if (_geometricType[i]==GeometricType)
      return _number->getI(i+1) ;
  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"GeometricType not found !")) ;
}

//---------------------------------------------------------------------
inline const int * SUPPORT::getNumberFromFile(MED_EN::medGeometryElement GeometricType) const
  throw (MEDEXCEPTION)
//---------------------------------------------------------------------
{
  const char * LOC = "Support::getNumberFromFile : " ;
//   if (_isOnAllElts)
//     throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined, support is on all entity !")) ;
  if (GeometricType==MED_EN::MED_ALL_ELEMENTS)
    return _number_fromfile->getValue() ;
  for (int i=0;i<_numberOfGeometricType;i++)
    if (_geometricType[i]==GeometricType)
      return _number_fromfile->getI(i+1) ;
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


/*! set the meshName if there is ni reference _mesh to Mesh */
//--------------------------------------
inline void SUPPORT::setMeshName(const string & meshName)
//--------------------------------------
{
  const char * LOC = "SUPPORT::setMeshName(const string & meshName) : ";

  if (_mesh)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Setting meshName is not possible when an associated mesh is set !")) ;

  _meshName=meshName;
}

/*! set the attribute _isOnAllElts to All
  Even if _isonAllElts is true, geometric types definning the FIELD's SUPPORT
  must be read from the SUPPORT not from the associated MESH (the geometric
  types definning the FIELD's SUPPORT may be a subset of the geometric types
  defined in the MESH even if for each SUPPORT geometric type all MESH entities
  are used).
*/
//------------------------------------------
inline void SUPPORT::setAll(bool All)
//------------------------------------------
{
  _isOnAllElts=All;
}

/*! set the attribute _entity to Entity */
//------------------------------------------
inline void SUPPORT::setEntity(MED_EN::medEntityMesh Entity)
{
  _entity=Entity;
}

/*! set the attribute _numberOfGeometricType to NumberOfGeometricType */
//---------------------------------------------------------------------
inline void SUPPORT::setNumberOfGeometricType(int NumberOfGeometricType)
//---------------------------------------------------------------------
{
  _numberOfGeometricType=NumberOfGeometricType;

  _geometricType.set(0);
  _numberOfElements.set(0);
}

/*! set the attribute _geometricType to geometricType */
//---------------------------------------------------------------------
inline void SUPPORT::setGeometricType(const MED_EN::medGeometryElement *GeometricType)
//---------------------------------------------------------------------
{
  if (NULL == _geometricType)
    _geometricType.set(_numberOfGeometricType);
  for (int i=0;i<_numberOfGeometricType;i++)
    _geometricType[i] = GeometricType[i];

  if (_profilNames.empty())
    {
      // giving a default value to profile names
      vector<string> prof_names( _numberOfGeometricType);
      for (int itype=0; itype < _numberOfGeometricType; itype++)
	{
	  ostringstream typestr;
	  typestr<<_name<<"_type"<<itype;
	  prof_names[itype]=typestr.str();
	}
      _profilNames=prof_names;
    }
}


/*!
  Set the attribute _numberOfElements to NumberOfElements and
  calculate the total number of elements.
*/
//----------------------------------------------------------
inline void SUPPORT::setNumberOfElements(const int *NumberOfElements)
//----------------------------------------------------------
{
  if (NULL == _numberOfElements)
    if (_numberOfGeometricType)
      _numberOfElements.set(_numberOfGeometricType,NumberOfElements);
    else
      _numberOfElements.set(0);
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
inline void SUPPORT::setNumber(const int * index, const int* value, bool shallowCopy)
//---------------------------------------------------
{
  if (_number != NULL) delete _number ;
  _number= new MEDSKYLINEARRAY(_numberOfGeometricType,_totalNumberOfElements,index,value,shallowCopy);
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
  Returns the %medEntityMesh's type used by the support.
  Note : A support deals only with one entity's type
  (for example : MED_FACE or MED_NODE)
*/
//---------------------------------------------
inline MED_EN::medEntityMesh SUPPORT::getEntity() const
//---------------------------------------------
{
  return _entity;
}

/*!
  If isOnAllElements is false, returns an array of %medGeometryElement
  types used by the support.
*/
//---------------------------------------------------
inline const MED_EN::medGeometryElement * SUPPORT::getTypes() const
//---------------------------------------------------
{
  return _geometricType;
}

}//End namespace MEDMEM


#endif /* SUPPORT_HXX */
