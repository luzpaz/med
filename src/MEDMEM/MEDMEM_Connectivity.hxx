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
//  File   : MEDMEM_Connectivity.hxx
//  Module : MED

#ifndef CONNECTIVITY_HXX
#define CONNECTIVITY_HXX

#include <vector>

#include "utilities.h"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

using namespace MED_EN;

class MEDSKYLINEARRAY;
class CELLMODEL;
class FAMILY;

/*!
	This class deals with all type of connectivity ./n
	it a recursive class. 
*/

/* ------------------------------------------- */
class CONNECTIVITY
/* ------------------------------------------- */
{
  /* ---------------------- */
  /*	Class Attributs     */
  /* ---------------------- */

protected:
					 /*! contains MED_CELL MED_FACE or MED_EDGE */
  medEntityMesh           _entity; 	
					 /*! contains MED_NODAL or MED_DESCEND */
  medConnectivity         _typeConnectivity;
					 /*! count of differents cells types 
					    used by the mesh */  
  med_int                 _numberOfTypes; 
					/*! array of all med_geometric_type used by MESH. */
  medGeometryElement*     _geometricTypes;

					/*! map indexed by med_geometric_type 
					    which contains the different 
					    'CellModel' used by MESH. */
  CELLMODEL *             _type;       
					/*! contains the dimension of the entity */
  med_int                 _entityDimension;

                    			/*! needed by calculateReverseNodalConnectivity */
  med_int                 _numberOfNodes;

  					 /*! array of size _numberOfTypes+1 which 
					 gives for each cell type the first 
					 cell number in _nodal or _descending
					 array (depends on _typeConnectivity) 
                                   	 To get cells count for one type, we 
					 must minus _count[i+1] and _count[i]
					 ( 0 <= i < _numberOfTypes ). 
                                  	 Note that _count[_numberOfTypes] returns
					 total cells count + 1 */
  med_int *               _count;

					/*! pointer to an array which stores the nodal connectivity */
  MEDSKYLINEARRAY* _nodal;
					/*! pointer to an array which stores 
					    the descending connectivity */
  MEDSKYLINEARRAY* _descending;
					/*! pointer to an array which stores 
					    the reverse nodal connectivity */
  MEDSKYLINEARRAY* _reverseNodalConnectivity;
					/*! pointer to an array which stores 
					    the reverse descending connectivity */
  MEDSKYLINEARRAY* _reverseDescendingConnectivity;
					/*! if face or edge, list of 2 cells or 
					    2 faces it belongs to.
					    If 2nd number equals 0, we have a 
					    boundary entity. We could use 
					    MEDSKYLINEARRAY, but we suppose we have
					    always only 2 (or 1) entities. */
  MEDSKYLINEARRAY* _neighbourhood;
					/*! connectivity of sub cell if 
					    descendant connectivity is calculated */
  CONNECTIVITY * _constituent; 	

  /* -------------------- */
  /*    Class Methods     */
  /* -------------------- */

private:
					/*! private method :/n
					    does nothing if already exists, else 
					    evaluates _nodal from _descending */
  void 	calculateNodalConnectivity(); 	
					/*! private method :/n
					    does nothing if already exists, else
					    evaluates from _nodal */  
  void calculateReverseNodalConnectivity(); 
					/*! private method :/n
					    does nothing if already exists, else
					    evaluates _descending from _nodal */ 
  void calculateDescendingConnectivity(); 
					/*! private method :/n
					    does nothing if already exists, else
					    evaluates from _descending */
  //  void calculateReverseDescendingConnectivity(CONNECTIVITY *myConnectivity);

  const med_int*      getReverseNodalConnectivity           ();
  const med_int*      getReverseNodalConnectivityIndex      ();
  const med_int*      getReverseDescendingConnectivity      ();
  const med_int*      getReverseDescendingConnectivityIndex ();

					/*! private method :/n
					    does nothing if already exists, else
					    evaluates _neighbourhood from _descending */
  void calculateNeighbourhood(CONNECTIVITY &myConnectivity);

public:

  friend class MED_MESH_RDONLY_DRIVER;
  friend class MED_MESH_WRONLY_DRIVER;

  // in order to fill CONNECTIVITY of MESH
  friend class GRID;

  CONNECTIVITY  (medEntityMesh Entity=MED_CELL);
  CONNECTIVITY  (int numberOfTypes, medEntityMesh Entity=MED_CELL);
  CONNECTIVITY  (const CONNECTIVITY & m);
  ~CONNECTIVITY ();

  void setConstituent (CONNECTIVITY * Constituent)
                       			throw (MEDEXCEPTION);

  void setGeometricTypes (const medGeometryElement * Types,
			   const medEntityMesh Entity)
                                        throw (MEDEXCEPTION);

  void setCount (const int * Count, const medEntityMesh Entity)
                                        throw (MEDEXCEPTION);

  void setNodal (const int * Connectivity,
	         const medEntityMesh Entity,
		 const medGeometryElement Type)
                                        throw (MEDEXCEPTION);

  inline bool   existConnectivity     (medConnectivity connectivityType, medEntityMesh Entity) const;

  void          calculateConnectivity (medConnectivity connectivityType, medEntityMesh Entity);

  void          updateFamily (vector<FAMILY*> myFamilies);

  inline medEntityMesh              getEntity               ()                     const;
  inline med_int                    getNumberOfTypes        (medEntityMesh Entity) const;
  inline const medGeometryElement * getGeometricTypes       (medEntityMesh Entity) const
                                    			     throw (MEDEXCEPTION);
  medGeometryElement                getElementType          (medEntityMesh Entity,
							     int Number)           const;
  inline const int *                getGlobalNumberingIndex (medEntityMesh Entity) const
                                    			     throw (MEDEXCEPTION);

  const med_int *     getConnectivity      (medConnectivity ConnectivityType, 
					    medEntityMesh Entity,
                                            medGeometryElement Type);
  const med_int *     getConnectivityIndex (medConnectivity ConnectivityType,
					    medEntityMesh Entity);
 
  const CELLMODEL &   getType              (medGeometryElement Type) const; 
  const CELLMODEL *   getCellsTypes        (medEntityMesh Entity)    const 
                                    	    throw (MEDEXCEPTION);
 
  med_int       getNumberOfNodesInType     (medGeometryElement Type) const; 
  med_int       getNumberOfSubCellInType   (medGeometryElement Type) const; 
  med_int 	getNumberOf                (medEntityMesh Entity, 
					    medGeometryElement Type) const;
  const med_int*      getValue             (medConnectivity TypeConnectivity, 
                                            medGeometryElement Type); 
  const med_int*      getValueIndex        (medConnectivity TypeConnectivity);

  inline const med_int* getReverseConnectivity (medConnectivity ConnectivityType, 
                                                medEntityMesh Entity=MED_CELL)
                                    	        throw (MEDEXCEPTION);
  inline const med_int* getReverseConnectivityIndex (medConnectivity ConnectivityType, 
						     medEntityMesh Entity=MED_CELL)
                                    	             throw (MEDEXCEPTION);

  const med_int*      getNeighbourhood() const;

};
/*----------------------*/
/* Methodes Inline	*/
/*----------------------*/

/*! Returns the  medEntityMesh */
//--------------------------------------------------//
inline medEntityMesh CONNECTIVITY::getEntity() const 
//--------------------------------------------------//
{ 
 	return _entity; 
}

/*!  Returns the number of different <medGeometryElement> types 
     existing in the specified entity. /n
     Note : Not implemented for MED_ALL_ENTITIES. */
//-----------------------------------------------------------------------//
inline med_int CONNECTIVITY::getNumberOfTypes(medEntityMesh Entity) const
//-----------------------------------------------------------------------//
{
  MESSAGE("CONNECTIVITY::getNumberOfTypes : Entity = "<<Entity<<", _entity = "<<_entity);
  if (_entity==Entity)
	return _numberOfTypes; 
  else if (_constituent!=NULL)
	return _constituent->getNumberOfTypes(Entity);
  else
	return 0; // because it is the right information (no exception needed)!
}

/*!  Returns an array of all <med geometry elements> types existing in the mesh 
     for the given medEntityMesh./n
     Note : Not implemented for MED_ALL_ENTITIES. /n
     Throws an exception if Entity is unknown */
//------------------------------------------------------------------------------------------//
inline const medGeometryElement* CONNECTIVITY::getGeometricTypes(medEntityMesh Entity) const 
						throw (MEDEXCEPTION)
//------------------------------------------------------------------------------------------//
{
  if (_entity==Entity)
	return _geometricTypes;
  else if (_constituent!=NULL)
	return _constituent->getGeometricTypes(Entity);
  else
	throw MEDEXCEPTION("Entity not defined !");
}

/*! A DOCUMENTER */
//----------------------------------------------------------------------------------//
inline const int * CONNECTIVITY::getGlobalNumberingIndex(medEntityMesh Entity) const 
						throw (MEDEXCEPTION)
//----------------------------------------------------------------------------------//
{
  if (_entity==Entity)
	return _count;
  else if (_constituent!=NULL)
	return _constituent->getGlobalNumberingIndex(Entity);
  else
	throw MEDEXCEPTION("Entity not defined !");
}

/*!  Returns true if a connectivity exists on elements of type "Entity" */
//-----------------------------------------------------------------------------//
inline bool CONNECTIVITY::existConnectivity( medConnectivity ConnectivityType, 
					     medEntityMesh Entity) const
//-----------------------------------------------------------------------------//
{
  if (_entity==Entity) { 
    MESSAGE("existConnectivity : _entity==Entity="<<Entity);
    if ((ConnectivityType==MED_NODAL)&(_nodal!=(MEDSKYLINEARRAY*)NULL))
      return true;
    if ((ConnectivityType==MED_DESCENDING)&(_descending!=(MEDSKYLINEARRAY*)NULL))
      return true;
  } else if (_constituent!=NULL)
    return _constituent->existConnectivity(ConnectivityType,Entity);
  return false;
}

/*! 
Returns an array containing CELLMODEL foreach element type present 
in connectivity for given medEntityMesh (similar as getGeometricTypes)./n
Throw an execption if the given entity is not defined or if the array is not defined.
*/
//-----------------------------------------------------------------------------//
inline const CELLMODEL * CONNECTIVITY::getCellsTypes(medEntityMesh Entity) const
						throw (MEDEXCEPTION)
//-----------------------------------------------------------------------------//
{
  if (Entity == _entity)
    if (_type!=NULL)
      return _type;
    else
      throw MEDEXCEPTION("CONNECTIVITY::getCellsTypes(medEntityMesh) :
			   CELLMODEL array is not defined !");
  else
    if (_constituent != NULL)
      return _constituent->getCellsTypes(Entity);
    else
      throw MEDEXCEPTION("CONNECTIVITY::getCellsTypes(medEntityMesh) : Not found Entity !");
}

/*! A DOCUMENTER */
//------------------------------------------------------------------------------------------//
inline const med_int* CONNECTIVITY::getReverseConnectivity( medConnectivity ConnectivityType, 
						            medEntityMesh Entity) 
						            throw (MEDEXCEPTION)
//------------------------------------------------------------------------------------------//
{
  if(_entity==Entity)
    if (ConnectivityType==MED_NODAL)
      return getReverseNodalConnectivity();
    else if (ConnectivityType==MED_DESCENDING)
      return getReverseDescendingConnectivity();
    else
      throw MEDEXCEPTION("MESH::getReverseConnectivity : connectivity mode not supported !");

  // other entity :
  if (NULL==_constituent)
    calculateDescendingConnectivity();
  return _constituent->getReverseConnectivity(ConnectivityType,Entity);
}

/*! A DOCUMENTER */
//-----------------------------------------------------------------------------------------------//
inline const med_int* CONNECTIVITY::getReverseConnectivityIndex(medConnectivity ConnectivityType, 
								medEntityMesh Entity) 
						                throw (MEDEXCEPTION)
//-----------------------------------------------------------------------------------------------//
{
  if(_entity==Entity)
    if (ConnectivityType==MED_NODAL)
      return getReverseNodalConnectivityIndex();
    else if (ConnectivityType==MED_DESCENDING)
      return getReverseDescendingConnectivityIndex();
    else
      throw MEDEXCEPTION("MESH::getReverseConnectivityIndex : connectivity mode not supported !");
  
  // other entity :
  if (NULL==_constituent)
    calculateDescendingConnectivity();
  return _constituent->getReverseConnectivityIndex(ConnectivityType,Entity);
}

#endif /* CONNECTIVITY_HXX */

