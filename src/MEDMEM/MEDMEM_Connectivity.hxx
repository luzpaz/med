#ifndef CONNECTIVITY_HXX
#define CONNECTIVITY_HXX

#include <vector>

#include "utilities.h"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_PolyhedronArray.hxx"

using namespace MED_EN;

namespace MEDMEM {
class MEDSKYLINEARRAY;
class CELLMODEL;
class FAMILY;
class GROUP;

/*!
	This class deals with all type of connectivity .\n
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
                                        /*! pointer to an array which stores the nodal connectivity only for polygons */
  MEDSKYLINEARRAY* _polygonsNodal;
                                        /*! pointer to an array which stores the nodal connectivity only for polyhedron */
  POLYHEDRONARRAY* _polyhedronNodal;
					/*! pointer to an array which stores 
					    the descending connectivity */
  MEDSKYLINEARRAY* _descending;
                                        /*!  pointer to an array which stores the descending connectivity only for polygons */
  MEDSKYLINEARRAY* _polygonsDescending;
                                        /*!  pointer to an array which stores the descending connectivity only for polyhedron */
  MEDSKYLINEARRAY* _polyhedronDescending;
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
					/*! private method :\n
					    does nothing if already exists, else 
					    evaluates _nodal from _descending */
  void 	calculateNodalConnectivity(); 	
					/*! private method :\n
					    does nothing if already exists, else
					    evaluates from _nodal */  
  void calculateReverseNodalConnectivity(); 
					/*! private method :\n
					    does nothing if already exists, else
					    evaluates _descending from _nodal */ 
  void calculateDescendingConnectivity(); 
					/*! private method :\n
					    does nothing if already exists, else
					    evaluates from _descending */
  //  void calculateReverseDescendingConnectivity(CONNECTIVITY *myConnectivity);

  const med_int*      getReverseNodalConnectivity           ();
  const med_int*      getReverseNodalConnectivityIndex      ();
  const med_int*      getReverseDescendingConnectivity      ();
  const med_int*      getReverseDescendingConnectivityIndex ();

					/*! private method :\n
					    does nothing if already exists, else
					    evaluates _neighbourhood from _descending */
  void calculateNeighbourhood(CONNECTIVITY &myConnectivity);

public:

  friend class MED_MESH_RDONLY_DRIVER;
  friend class MED_MESH_WRONLY_DRIVER;
  friend ostream & operator<<(ostream &os, CONNECTIVITY &connectivity);

  // in order to fill CONNECTIVITY of MESH
  friend class GRID;

  CONNECTIVITY  (medEntityMesh Entity=MED_CELL);
  CONNECTIVITY  (int numberOfTypes, medEntityMesh Entity=MED_CELL);
  CONNECTIVITY  (const CONNECTIVITY & m);
  virtual ~CONNECTIVITY ();

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

  inline void setNumberOfNodes(med_int NumberOfNodes);
  
  inline med_int getEntityDimension() const;

  inline void setEntityDimension(med_int EntityDimension);

  void setPolygonsConnectivity(medConnectivity ConnectivityType, medEntityMesh Entity, med_int* PolygonsConnectivity, med_int* PolygonsConnectivityIndex, med_int ConnectivitySize, med_int NumberOfPolygons);

  void setPolyhedronConnectivity(medConnectivity ConnectivityType, med_int* PolyhedronConnectivity, med_int* PolyhedronIndex, med_int ConnectivitySize, med_int NumberOfPolyhedron, med_int* PolyhedronFacesIndex=(med_int*)NULL, med_int NumberOfFaces=0);

  inline bool existConnectivity(medConnectivity connectivityType, medEntityMesh Entity) const;

  inline bool existPolygonsConnectivity(medConnectivity connectivityType, medEntityMesh Entity) const;

  inline bool existPolyhedronConnectivity(medConnectivity connectivityType, medEntityMesh Entity) const;

  virtual void          calculateConnectivity (medConnectivity connectivityType, medEntityMesh Entity);

  virtual void          updateFamily (vector<FAMILY*> myFamilies);
  virtual void          updateGroup (vector<GROUP*> myFamilies);

  inline medEntityMesh              getEntity               ()                     const;
  inline med_int                    getNumberOfTypes        (medEntityMesh Entity) const;
  inline const medGeometryElement * getGeometricTypes       (medEntityMesh Entity) const
                                    			     throw (MEDEXCEPTION);
  medGeometryElement                getElementType          (medEntityMesh Entity,
							     int Number)           const;
  virtual inline const int *                getGlobalNumberingIndex (medEntityMesh Entity) const
                                    			     throw (MEDEXCEPTION);

  virtual const med_int *     getConnectivity      (medConnectivity ConnectivityType, 
					    medEntityMesh Entity,
                                            medGeometryElement Type);
  virtual const med_int *     getConnectivityIndex (medConnectivity ConnectivityType,
					    medEntityMesh Entity);
  const med_int* getPolygonsConnectivity(medConnectivity ConnectivityType, medEntityMesh Entity);
  const med_int* getPolygonsConnectivityIndex(medConnectivity ConnectivityType, medEntityMesh Entity);
  med_int getNumberOfPolygons() const;
  const med_int* getPolyhedronConnectivity(medConnectivity ConnectivityType);
  const med_int* getPolyhedronFacesIndex();
  const med_int* getPolyhedronIndex(medConnectivity ConnectivityType);
  med_int getNumberOfPolyhedronFaces() const;
  med_int getNumberOfPolyhedron() const;
  const CELLMODEL &   getType              (medGeometryElement Type) const; 
  const CELLMODEL *   getCellsTypes        (medEntityMesh Entity)    const 
                                    	    throw (MEDEXCEPTION);
 
  med_int       getNumberOfNodesInType     (medGeometryElement Type) const; 
  med_int       getNumberOfSubCellInType   (medGeometryElement Type) const; 
  virtual med_int 	getNumberOf                (medEntityMesh Entity, 
					    medGeometryElement Type) const;
  virtual const med_int*      getValue             (medConnectivity TypeConnectivity, 
                                            medGeometryElement Type); 
  virtual const med_int*      getValueIndex        (medConnectivity TypeConnectivity);

  virtual inline const med_int* getReverseConnectivity (medConnectivity ConnectivityType, 
                                                medEntityMesh Entity=MED_CELL)
                                    	        throw (MEDEXCEPTION);
  virtual inline const med_int* getReverseConnectivityIndex (medConnectivity ConnectivityType, 
						     medEntityMesh Entity=MED_CELL)
                                    	             throw (MEDEXCEPTION);

  const med_int*      getNeighbourhood() const;

};
};
/*----------------------*/
/* Methodes Inline	*/
/*----------------------*/
using namespace MEDMEM;

/*! Returns the  medEntityMesh */
//--------------------------------------------------//
inline medEntityMesh CONNECTIVITY::getEntity() const 
//--------------------------------------------------//
{ 
 	return _entity; 
}

/*!  Returns the number of different %medGeometryElement types 
     existing in the specified entity. \n
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
//   else if (_constituent == NULL)
//     {
//       MESSAGE("CONNECTIVITY::getNumberOfTypes : _constituent == NULL");
//       try
// 	{
// 	  (const_cast <CONNECTIVITY *> (this))->calculateDescendingConnectivity();
// 	}
//       catch (MEDEXCEPTION & ex)
// 	{
// 	  return 0 ;
// 	}

//       SCRUTE(_entityDimension);

//       if (_entityDimension != 2 && _entityDimension != 3) return 0;

//       try
// 	{
// 	  _constituent->calculateConnectivity(MED_NODAL,Entity);
// 	}
//       catch (MEDEXCEPTION & ex)
// 	{
// 	  return 0 ;
// 	}

//       return _constituent->getNumberOfTypes(Entity);
//     }
  else
	return 0; // because it is the right information (no exception needed)!
}

/*!  Returns an array of all %medGeometryElement types existing in the mesh 
     for the given %medEntityMesh.
     Note : Not implemented for MED_ALL_ENTITIES.
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
	throw MEDEXCEPTION("CONNECTIVITY::getGeometricTypes : Entity not defined !");
}

/*!   Returns an array containing the accumulated number of entities sorted by the geometric type.\n

      Exemple :\n

      In case of a CONNECTIVITY containing 3*MED_TRIA3 et 2*MED_QUAD4 : \n
            int * count = getGlobalNumberingIndex(MED_CELL)\n
      count[0] is always set to 1\n
      count[1] is set to 1+3=4\n
      count[2] is set to 4+2=6 = total number of cells + 1\n

      Note : Not implemented for MED_ALL_ENTITIES. \n
      Note : The geometric type order is given by the typedef enum medGeometryElement.
                      
*/
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
	throw MEDEXCEPTION("CONNECTIVITY::getGlobalNumberingIndex : Entity not defined !");
}

/*! Returns true if a connectivity exists on elements of type "Entity" */
//-----------------------------------------------------------------------------//
inline bool CONNECTIVITY::existConnectivity(medConnectivity ConnectivityType, 
					    medEntityMesh Entity) const
//-----------------------------------------------------------------------------//
{
  if (_entity == Entity) { 
    MESSAGE("existConnectivity : _entity==Entity="<<Entity);
    if ((ConnectivityType==MED_NODAL)&(_nodal!=(MEDSKYLINEARRAY*)NULL))
      return true;
    if ((ConnectivityType==MED_DESCENDING)&(_descending!=(MEDSKYLINEARRAY*)NULL))
      return true;
  } else if (_constituent!=NULL)
    return _constituent->existConnectivity(ConnectivityType,Entity);
  return false;
}

/*! Returns true if a polygons connectivity exists on elements of type "Entity" */
//-----------------------------------------------------------------------------//
inline bool CONNECTIVITY::existPolygonsConnectivity(medConnectivity ConnectivityType,
						    medEntityMesh Entity) const
//-----------------------------------------------------------------------------//
{
  if (_entity == Entity)
    {
      MESSAGE("existPolygonsConnectivity : _entity == Entity = "<<Entity);
      if (ConnectivityType == MED_NODAL && _polygonsNodal != (MEDSKYLINEARRAY*) NULL)
	return true;
      if (ConnectivityType == MED_DESCENDING && _polygonsDescending != (MEDSKYLINEARRAY*) NULL)
	return true;
    }
  else if (_constituent != (CONNECTIVITY*) NULL)
    return _constituent->existPolygonsConnectivity(ConnectivityType,Entity);
  return false;
}

/*! Returns true if a polyhedron connectivity exists on elements of type "Entity" */
//-----------------------------------------------------------------------------//
inline bool CONNECTIVITY::existPolyhedronConnectivity(medConnectivity ConnectivityType,
						      medEntityMesh Entity) const
//-----------------------------------------------------------------------------//
{
  if (_entity == Entity)
    {
      MESSAGE("existPolyhedronConnectivity : _entity == Entity = "<<Entity);
      if (ConnectivityType == MED_NODAL && _polyhedronNodal != (POLYHEDRONARRAY*) NULL)
	return true;
      if (ConnectivityType == MED_DESCENDING && _polyhedronDescending != (MEDSKYLINEARRAY*) NULL)
	return true;
    }
  else if (_constituent != (CONNECTIVITY*) NULL)
    return _constituent->existPolyhedronConnectivity(ConnectivityType,Entity);
  return false;
}

/*! 
Returns an array containing CELLMODEL foreach element type present 
in connectivity for given medEntityMesh (similar as getGeometricTypes).\n
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
      throw MEDEXCEPTION("CONNECTIVITY::getCellsTypes(medEntityMesh) :"
			 " CELLMODEL array is not defined !");
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


inline void CONNECTIVITY::setNumberOfNodes(med_int NumberOfNodes)
{
    _numberOfNodes=NumberOfNodes;
}
  
inline void CONNECTIVITY::setEntityDimension(med_int EntityDimension)
{
    _entityDimension=EntityDimension;
}

med_int CONNECTIVITY::getEntityDimension() const
{
  return _entityDimension;
}

#endif /* CONNECTIVITY_HXX */

