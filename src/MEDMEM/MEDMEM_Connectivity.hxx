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
	this class deals with all type of connectivity 
	this a recursive class in order to store 
	
*/
class CONNECTIVITY // recursive class

{
  /*
    Class Attributs
  */

protected:
					 /*! contains MED_CELL MED_FACE or MED_EDGE */
  medEntityMesh           _entity ; 	
					 /*! contains MED_NODAL or MED_DESCEND */
  medConnectivity         _typeConnectivity ;
					 /*! count of differents cells types 
					    used by the mesh */  
  med_int                 _numberOfTypes ; 
					/*! array of all med_geometric_type used by MESH. */
  medGeometryElement*     _geometricTypes ;

					/*! map indexed by med_geometric_type 
					    which contains the different 
					    'CellModel' used by MESH. */
  CELLMODEL *             _type ;       
					/*! contains the dimension of the entity */
  med_int                 _entityDimension ;

                    			/*! needed by calculateReverseNodalConnectivity */
  med_int                 _numberOfNodes ; 

  					 /*! array of size _numberOfTypes+1 which 
					 gives for each cell type the first 
					 cell number in _nodal or _descending
					 array (depends on _typeConnectivity) 
                                   	 To get cells count for one type, we 
					 must minus _count[i+1] and _count[i]
					 ( 0 <= i < _numberOfTypes ). 
                                  	 Note that _count[_numberOfTypes] returns
					 total cells count + 1 */
  med_int *               _count ;

					/*! pointer on an array which stores the nodal connectivity */
  MEDSKYLINEARRAY* _nodal ;
					/*! pointer on an array which stores the descending connectivity */
  MEDSKYLINEARRAY* _descending ;
					/*! pointer on an array which stores the resverse nodal connectivity */
  MEDSKYLINEARRAY* _reverseNodalConnectivity ;
					/*! pointer on an array which stores the reverse descending connectivity */
  MEDSKYLINEARRAY* _reverseDescendingConnectivity ;
					/*! if face or edge, list of 2 cells or 
					    2 faces it belongs to.
					    If 2nd number equals 0, we have a 
					    boundary entity. We could use 
					    MEDSKYLINEARRAY, but we suppose we have
					    always only 2 (or 1) entities. */
  MEDSKYLINEARRAY* _neighbourhood ;
					/*! connectivity of sub cell if 
					    descendant connectivity is calculated */
  CONNECTIVITY * _constituent ; 	

  /*
    Class Methods
  */

private:
					/*! does nothing if already exists, else 
					    evaluates _nodal from _descending */
  void 	calculateNodalConnectivity() ; 	
					/*! does nothing if already exists, else
					    evaluates from _nodal */  
  void calculateReverseNodalConnectivity() ; 
					/*! does nothing if already exists, else
					    evaluates _descending from _nodal */ 
  void calculateDescendingConnectivity() ; 
					/*! does nothing if already exists, else
					    evaluates from _descending */
  //  void calculateReverseDescendingConnectivity(CONNECTIVITY *myConnectivity) ;
					/*! does nothing if already exists, else
					    evaluates _neighbourhood from _descending */
  void calculateNeighbourhood(CONNECTIVITY &myConnectivity) ;

public:

  friend class MED_MESH_RDONLY_DRIVER;
  friend class MED_MESH_WRONLY_DRIVER;

  CONNECTIVITY(medEntityMesh Entity=MED_CELL);
  CONNECTIVITY(int numberOfTypes, medEntityMesh Entity=MED_CELL);
  ~CONNECTIVITY();

  inline bool   existConnectivity     (medConnectivity connectivityType, medEntityMesh Entity) const ;
  void          calculateConnectivity (medConnectivity connectivityType, medEntityMesh Entity) ;
  void          updateFamily(vector<FAMILY*> myFamilies) ;

  inline medEntityMesh        getEntity               ()                     const;
  inline med_int              getNumberOfTypes        (medEntityMesh Entity) const;
  inline medGeometryElement * getGeometricTypes       (medEntityMesh Entity) const;
  medGeometryElement          getElementType(medEntityMesh Entity,int Number) const ;
  inline int *                getGlobalNumberingIndex (medEntityMesh Entity) const ;

  med_int *     getConnectivity      (medConnectivity ConnectivityType, medEntityMesh Entity,
                                      medGeometryElement Type) ;
  med_int *     getConnectivityIndex (medConnectivity ConnectivityType,medEntityMesh Entity) ;
 
  CELLMODEL &   getType              (medGeometryElement Type) const; 
  CELLMODEL *   getCellsTypes        (medEntityMesh Entity) const; 
 
  med_int       getNumberOfNodesInType    (medGeometryElement Type)                                    const; 
  med_int       getNumberOfSubCellInType  (medGeometryElement Type)                                    const; 
  med_int 	getNumberOf               (medEntityMesh Entity, medGeometryElement Type) 	        const;
  med_int*      getValue                  (medConnectivity TypeConnectivity, medGeometryElement Type) ; 
  med_int*      getValueIndex             (medConnectivity TypeConnectivity) ;
  med_int*      getReverseNodalConnectivity           ()               ;
  med_int*      getReverseNodalConnectivityIndex      ()               ;
  med_int*      getReverseDescendingConnectivity      ()               ;
  med_int*      getReverseDescendingConnectivityIndex ()               ;
  med_int*      getNeighbourhood() 				   const;

} ;
/*----------------------*/
/* Methodes Inline	*/
/*----------------------*/

/*! Returns the  medEntityMesh */
inline medEntityMesh CONNECTIVITY::getEntity() const 
{ 
  return _entity; 
}

/*!  Returns the number of different <medGeometryElement> types existing in the specified entity.

     Note : Not implemented for MED_ALL_ENTITIES. */
inline med_int CONNECTIVITY::getNumberOfTypes(medEntityMesh Entity) const
{
  MESSAGE("CONNECTIVITY::getNumberOfTypes : Entity = "<<Entity<<", _entity = "<<_entity);
  if (_entity==Entity)
    return _numberOfTypes; 
  else if (_constituent!=NULL)
    return _constituent->getNumberOfTypes(Entity) ;
  else
    return 0 ; // because it is the right information (no need exception !
  //throw MEDEXCEPTION("Entity not defined !") ;
}

/*!  Returns an array of all <med geometry elements> types existing in the mesh for the given medEntityMesh.

     Note : Not implemented for MED_ALL_ENTITIES.  */
inline medGeometryElement* CONNECTIVITY::getGeometricTypes(medEntityMesh Entity) const 
{
  if (_entity==Entity)
    return _geometricTypes;
  else if (_constituent!=NULL)
    return _constituent->getGeometricTypes(Entity) ;
  else
    throw MEDEXCEPTION("Entity not defined !") ;
}

/*! A DOCUMENTER */
inline int * CONNECTIVITY::getGlobalNumberingIndex(medEntityMesh Entity) const 
{
  if (_entity==Entity)
    return _count;
  else if (_constituent!=NULL)
    return _constituent->getGlobalNumberingIndex(Entity) ;
  else
    throw MEDEXCEPTION("Entity not defined !") ;
}

/*!  Returns true if a connectivity exists on elements of type "Entity" */
inline bool CONNECTIVITY::existConnectivity(medConnectivity ConnectivityType, medEntityMesh Entity) const
{
  if (_entity==Entity) { 
    MESSAGE("existConnectivity : _entity==Entity="<<Entity);
    if ((ConnectivityType==MED_NODAL)&(_nodal!=(MEDSKYLINEARRAY*)NULL))
      return true ;
    if ((ConnectivityType==MED_DESCENDING)&(_descending!=(MEDSKYLINEARRAY*)NULL))
      return true ;
  } else if (_constituent!=NULL)
    return _constituent->existConnectivity(ConnectivityType,Entity) ;
  return false ;
}

/*! 
Return an array which contain CELLMODEL foreach element type present in connectivity for given medEntityMesh (similar as getGeometricTypes)

Throw an execption if the given entity is not defined or if the array is not defined.
*/
inline CELLMODEL * CONNECTIVITY::getCellsTypes(medEntityMesh Entity) const
{
  if (Entity == _entity)
    if (_type!=NULL)
      return _type ;
    else
      throw MEDEXCEPTION("CONNECTIVITY::getCellsTypes(medEntityMesh) : CELLMODEL array is not defined !");
  else
    if (_constituent != NULL)
      return _constituent->getCellsTypes(Entity) ;
    else
      throw MEDEXCEPTION("CONNECTIVITY::getCellsTypes(medEntityMesh) : Not found Entity !");
}

#endif /* CONNECTIVITY_HXX */

