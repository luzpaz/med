#ifndef MESH_HXX
#define MESH_HXX

#include <string>
#include <vector>

#include "utilities.h"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

//#include "MEDMEM_Support.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Connectivity.hxx"

#include "MEDMEM_MedMeshDriver.hxx"

class CELLMODEL;
class FAMILY; 
class GROUP; 
class SUPPORT; 

template <class T> class FIELD;

//class GENDRIVER ;
//class MED_MESH_RDONLY_DRIVER ;
//class MED_MESH_WRONLY_DRIVER ;

using namespace MED_EN;

/*! This class contains all the informations related with a MESH : 
  - COORDINATES
  - CONNECTIVITIES
  - FAMILIES OF NODES
  - FAMILIES OF CELLS
  - FAMILIES OF FACES
  - FAMILIES OF EDGES
  
  NOTE: A Family is only on one type of entity (MED_CELL, MED_FACE, MED_EDGE, MED_NODE).
  You can't have a family on MED_CELL and MED_FACE
  
*/

class MESH
{


public :

  // ------- Drivers Management Part
protected:
  
  //-----------------------//
  class INSTANCE 
  //-----------------------//
  {
  public:
    virtual GENDRIVER * run(const string & fileName, MESH * ptrMesh) const = 0 ;
  } ;
  
  //-------------------------------------------------------//
  template <class T> class INSTANCE_DE : public INSTANCE 
  //-------------------------------------------------------//
  {
  public :
    GENDRIVER * run(const string & fileName, MESH * ptrMesh) const 
    { return new T(fileName,ptrMesh) ; }
  } ;
  
  static INSTANCE_DE<MED_MESH_RDWR_DRIVER> inst_med ;
  //static INSTANCE_DE<VTK_DRIVER>   inst_vtk   ;
  static const INSTANCE * const instances[] ;

  // ------ End of Drivers Management Part


  //-----------------------//
  //   Attributes
  //-----------------------//

protected :

  string        _name; // A POSITIONNER EN FCT DES IOS ?

  int           _numberOfMEDNodeFamily ; // INUTILE
  int *         _MEDArrayNodeFamily ;    // SOLUTION TEMPORAIRE
  int *         _numberOfMEDCellFamily ; // INUTILE
  int *         _numberOfMEDFaceFamily ; // INUTILE
  int *         _numberOfMEDEdgeFamily ; // INUTILE
  int **        _MEDArrayCellFamily ;    // SOLUTION TEMPORAIRE
  int **        _MEDArrayFaceFamily ;    // SOLUTION TEMPORAIRE
  int **        _MEDArrayEdgeFamily ;    // SOLUTION TEMPORAIRE
  
  COORDINATE *   _coordinate;
  CONNECTIVITY * _connectivity;

  int           _spaceDimension;
  int           _meshDimension ;
  int           _numberOfNodes    ;

  int             _numberOfNodesFamilies;  //INUTILE ? -> _familyNode.size()
  vector<FAMILY*> _familyNode ;  // array of size _numberOfNodesFamilies; 
  int             _numberOfCellsFamilies;
  vector<FAMILY*> _familyCell ;  // array of size _numberOfCellsFamilies; 
  int             _numberOfFacesFamilies;
  vector<FAMILY*> _familyFace ;  // array of size _numberOfFacesFamilies; 
  int             _numberOfEdgesFamilies;
  vector<FAMILY*> _familyEdge ;  // array of size _numberOfEdgesFamilies;

  int   	 _numberOfNodesGroups; //INUTILE ?
  vector<GROUP*> _groupNode;    // array of size _numberOfNodesGroups; 
  int   	 _numberOfCellsGroups; //INUTILE ?
  vector<GROUP*> _groupCell;    // array of size _numberOfCellsGroups; 
  int   	 _numberOfFacesGroups; //INUTILE ?
  vector<GROUP*> _groupFace;    // array of size _numberOfFacesGroups; 
  int   	 _numberOfEdgesGroups; //INUTILE ?
  vector<GROUP*> _groupEdge;    // array of size _numberOfEdgesGroups; 
  // list of all Group

  vector<GENDRIVER *> _drivers; // Storage of the drivers currently in use
 
  //-----------------------//
  //   Methods
  //-----------------------//

public :

  friend class MED_MESH_RDONLY_DRIVER;
  friend class MED_MESH_WRONLY_DRIVER;

  void init();
  MESH();
  MESH(const MESH &m);
  MESH & operator=(const MESH &m);
  MESH( driverTypes driverType, const string & fileName="", 
	      const string & meshName="");
  ~MESH();
  friend ostream & operator<<(ostream &os, MESH &my) ;

  int  addDriver(driverTypes driverType, 
		      const string & fileName ="Default File Name.med",
                 const string & driverName="Default Mesh Name");
  int  addDriver(GENDRIVER & driver);
  void rmDriver(int index=0);

  inline void read(int index=0);  
  inline void write(int index=0, const string & driverName = "");
  inline void write(const GENDRIVER &);

  inline int *  getMEDArrayNodeFamily() ;
  inline int ** getMEDArrayCellFamily() ;
  inline int ** getMEDArrayFaceFamily() ;
  inline int ** getMEDArrayEdgeFamily() ;

  //  void calculateReverseConnectivity();
  //  void createFaces(); 	//Faces creation => full constituent informations
  //  void buildConstituent() ; // calculate descendent connectivity + face-cell connectivity


  inline void 	      setName(string name);

  inline string       getName() const;
  inline int 	      getSpaceDimension();
  inline int 	      getMeshDimension();

  inline int 	      getNumberOfNodes();
  inline COORDINATE * getCoordinateptr();
  inline string       getCoordinatesSystem();
  inline const double *     getCoordinates(medModeSwitch Mode);
  inline const double       getCoordinate(int Number,int Axis);
  inline string *     getCoordinatesNames();
  inline string *     getCoordinatesUnits();
  //  inline int *        getNodesNumbers();

  inline int          getNumberOfTypes(medEntityMesh Entity);
  inline medGeometryElement *  getTypes(medEntityMesh Entity);
  inline CELLMODEL *  getCellsTypes(medEntityMesh Entity);
  inline int *        getGlobalNumberingIndex(medEntityMesh Entity);
  inline int          getNumberOfElements(medEntityMesh Entity,medGeometryElement Type);
  inline bool         existConnectivity(medConnectivity ConnectivityType,medEntityMesh Entity);
  inline medGeometryElement getElementType(medEntityMesh Entity,int Number) ;
  inline void         calculateConnectivity(medModeSwitch Mode,medConnectivity ConnectivityType,medEntityMesh Entity);
  inline int *        getConnectivity(medModeSwitch Mode,medConnectivity ConnectivityType,medEntityMesh Entity, medGeometryElement Type);
  inline int *        getConnectivityIndex(medConnectivity ConnectivityType,medEntityMesh Entity);
  int                 getElementNumber(medConnectivity ConnectivityType, medEntityMesh Entity, medGeometryElement Type, int * connectivity) ;
  inline int *        getReverseConnectivity(medConnectivity ConnectivityType);
  inline int *        getReverseConnectivityIndex(medConnectivity ConnectivityType);

  inline int          getNumberOfFamilies(medEntityMesh Entity);
  inline vector<FAMILY*> getFamilies(medEntityMesh Entity);
  inline FAMILY*      getFamily(medEntityMesh Entity,int i);
  inline int          getNumberOfGroups(medEntityMesh Entity);
  inline vector<GROUP*> getGroups(medEntityMesh Entity);
  inline GROUP*      getGroup(medEntityMesh Entity,int i);


  SUPPORT * getBoundaryElements(medEntityMesh Entity) throw (MEDEXCEPTION) ;

  //  Node DonneBarycentre(const Cell &m) const;
  FIELD<double>* getVolume(const SUPPORT * Support) throw (MEDEXCEPTION) ; // Support must be on 3D elements
  FIELD<double>* getArea(const SUPPORT * Support) throw (MEDEXCEPTION) ; // Support must be on 2D elements
  FIELD<double>* getLength(const SUPPORT * Support) throw (MEDEXCEPTION) ; // Support must be on 1D elements
  FIELD<double>* getNormal(const SUPPORT * Support) throw (MEDEXCEPTION) ; // Support must be on 2D elements
  FIELD<double>* getBarycenter(const SUPPORT * Support) throw (MEDEXCEPTION) ;
  //  FIELD<int>* getNeighbourhood(SUPPORT * Support) throw (MEDEXCEPTION) ; // Il faut preciser !
};

// ---------------------------------------
// 		Methodes Inline
// ---------------------------------------

inline void MESH::read(int index=0)  
{ 
  const char * LOC = "MESH::read(int index=0) : ";
  BEGIN_OF(LOC);

  if (_drivers[index]) {
    _drivers[index]->open();   
    _drivers[index]->read(); 
    _drivers[index]->close(); 
  }
  else
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "The index given is invalid, index must be between  0 and |" 
                                     << _drivers.size() 
                                     )
                          ); 
  END_OF(LOC);
} 

/*! Write all the content of the MESH using driver referenced by the handler <index>*/
inline void MESH::write(int index=0, const string & driverName = "") 
{ 
  const char * LOC = "MESH::write(int index=0, const string & driverName = \"\") : ";
  BEGIN_OF(LOC);

  if ( _drivers[index] ) {
    _drivers[index]->open();   
    if (driverName != "") _drivers[index]->setMeshName(driverName); 
    _drivers[index]->write(); 
    _drivers[index]->close(); 
  }
  else
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "The index given is invalid, index must be between  0 and |" 
                                     << _drivers.size() 
                                     )
                          ); 
  END_OF(LOC);
} 

// This method is MED specific : don't use it 
// must be private. 
inline void MESH::write(const GENDRIVER & genDriver) 
{ 
  const char * LOC = "MESH::write(const GENDRIVER & genDriver): ";
  BEGIN_OF(LOC);

  for (int index=0; index < _drivers.size(); index++ )
    if ( *_drivers[index] == genDriver ) { 
      _drivers[index]->open();   
      _drivers[index]->write(); 
      _drivers[index]->close();
      // ? FINALEMENT PAS BESOIN DE L'EXCEPTION ?
    }
  
  END_OF(LOC);
  
} 

/*! Set the MESH name */
inline void MESH::setName(string name) 
{
  _name=name ; //NOM interne à la classe
}

/*! Get the MESH name */
inline string MESH::getName() const 
{
  return _name ;
}

/*! Get the dimension of the space */
inline int MESH::getSpaceDimension() 
{
  return _spaceDimension;
}

/*! Get the dimension of the MESH */
inline int MESH::getMeshDimension() 
{
  return _meshDimension;
}

/*! Get the number of nodes used in the MESH */
inline int MESH::getNumberOfNodes() 
{
  return _numberOfNodes;
}

/*! Get the COORDINATES object. Use it only if you need COORDINATES informations not provided by the MESH class.*/
inline COORDINATE * MESH::getCoordinateptr() 
{
  return _coordinate;
}

/*! Get the system in which coordinates are given (CARTESIAN,CYLINDRICAL,SPHERICAL) __??MED_CART??__. */
inline string MESH::getCoordinatesSystem()
{
  return _coordinate->getCoordinatesSystem();
}

/*! Get the whole coordinates array in a given interlacing mode. The interlacing mode are :
  - MED_NO_INTERLACE   :  X1 X2 Y1 Y2 Z1 Z2
  - MED_FULL_INTERLACE :  X1 Y1 Z1 X2 Y2 Z2 
 */
inline const double * MESH::getCoordinates(medModeSwitch Mode)
{
  return _coordinate->getCoordinates(Mode);
}

/*! Get the coordinate n° number on axis n°axis*/
inline const double MESH::getCoordinate(int number, int axis)
{
  return _coordinate->getCoordinate(number,axis);
}

/*! Get the coordinate names array ("x       ","y       ","z       ")
  of size n*MED_TAILLE_PNOM
*/
inline string * MESH::getCoordinatesNames() 
{
  return _coordinate->getCoordinatesNames();
}

/*! Get the coordinate unit names array ("cm       ","cm       ","cm       ")
  of size n*MED_TAILLE_PNOM
*/
inline string * MESH::getCoordinatesUnits() 
{
  return _coordinate->getCoordinatesUnits();
}
//  int * MESH::getNodesNumbers() {
//    return nodesNumbers;
//  }

/*! Get the number of different geometric types for a given entity type.
  For exemple getNumberOfTypes(MED_CELL) would return 2 is the MESH have some MED_TETRA4 and MED_HEXA6 in it. 
  medEntityMesh entity : MED_CELL, MED_FACE, MED_EDGE, MED_NODE, MED_ALL_ENTITIES
*/
inline int MESH::getNumberOfTypes(medEntityMesh entity)
{
  MESSAGE("MESH::getNumberOfTypes(medEntityMesh entity) : "<<entity);
  if (entity == MED_NODE)
    return 1 ;
  if (_connectivity != NULL)
    return _connectivity->getNumberOfTypes(entity) ;
  throw MEDEXCEPTION(LOCALIZED("MESH::getNumberOfTypes( medEntityMesh ) : Connectivity not defined !"));
}

/*! 
  Get the list of geometric types used by a given entity.
  medEntityMesh entity : MED_CELL, MED_FACE, MED_EDGE, MED_ALL_ENTITIES
  REM : Don't use MED_NODE 
*/
inline medGeometryElement * MESH::getTypes(medEntityMesh entity)
{
  if (entity == MED_NODE)
    throw MEDEXCEPTION(LOCALIZED("MESH::getTypes( medEntityMesh ) : No medGeometryElement with MED_NODE entity !"));
  
  if (_connectivity != NULL)
    return _connectivity->getGeometricTypes(entity) ;
  throw MEDEXCEPTION(LOCALIZED("MESH::getTypes( medEntityMesh ) : Connectivity not defined !"));
}

/*! 
  Get the whole list of CELLMODEL used by cells of given type (medEntityMesh).
  REMARK : Don't use MED_NODE as medEntityMesh
*/
inline CELLMODEL * MESH::getCellsTypes(medEntityMesh Entity)
{
  if (_connectivity != NULL)
  return _connectivity->getCellsTypes(Entity) ;
  throw MEDEXCEPTION(LOCALIZED("MESH::getCellsTypes( medEntityMesh ) : Connectivity not defined !"));
}

/*! Since informations related with an entity type are stored in independent arrays, the only way to
    get a uniq index is to call this method.
    For exemple if you want the global numbers of your ... ????? size ????? 
*/
inline int * MESH::getGlobalNumberingIndex(medEntityMesh entity)
{
  if (_connectivity != NULL)
  return _connectivity->getGlobalNumberingIndex(entity);
  throw MEDEXCEPTION(LOCALIZED("MESH::getNumberOfTypes( medEntityMesh ) : Connectivity not defined !"));
}
inline int MESH::getNumberOfElements(medEntityMesh entity, medGeometryElement Type)
{
  const char * LOC = "MESH::getNumberOfElements(medEntityMesh,medGeometryElement) : " ;
  if (entity==MED_NODE)
    if ((Type==MED_NONE)|(Type==MED_ALL_ELEMENTS))
      return _numberOfNodes ;
    else
      return 0 ;
      //throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"wrong medGeometryElement with MED_NODE"));
  else 
    if (_connectivity != (CONNECTIVITY*)NULL)
      return _connectivity->getNumberOf(entity,Type) ;
    else
      return 0 ;
      //throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"connectivity not defined !"));
}
inline bool MESH::existConnectivity(medConnectivity connectivityType, medEntityMesh entity)
{
  if (_connectivity==(CONNECTIVITY*)NULL)
    throw MEDEXCEPTION("MESH::existConnectivity(medConnectivity,medEntityMesh) : no connectivity defined !");
  return _connectivity->existConnectivity(connectivityType,entity) ;
}
/*!
  Return the geometric type of element Number of entity Entity.

  Throw an exception if Entity is not defined or Number are wrong (too big).
 */
inline medGeometryElement MESH::getElementType(medEntityMesh Entity,int Number)
{
  if (_connectivity==(CONNECTIVITY*)NULL)
    throw MEDEXCEPTION("MESH::getElementType(medEntityMesh,int) : no connectivity defined !");
  return _connectivity->getElementType(Entity,Number) ;
}
inline void MESH::calculateConnectivity(medModeSwitch Mode,medConnectivity ConnectivityType,medEntityMesh entity)
{
  if (Mode==MED_FULL_INTERLACE)
    _connectivity->calculateConnectivity(ConnectivityType,entity) ;
  else
    throw MEDEXCEPTION(LOCALIZED("MESH::calculateConnectivity : only for MED_FULL_INTERLACE mode"));
}
inline int * MESH::getConnectivity(medModeSwitch Mode,medConnectivity ConnectivityType,medEntityMesh entity, medGeometryElement Type)
{
  if (Mode==MED_FULL_INTERLACE)
    return _connectivity->getConnectivity(ConnectivityType,entity,Type) ;
  throw MEDEXCEPTION(LOCALIZED("MESH::getConnectivity : only for MED_FULL_INTERLACE mode"));
}
inline int * MESH::getConnectivityIndex(medConnectivity ConnectivityType,medEntityMesh entity)
{
  return _connectivity->getConnectivityIndex(ConnectivityType, entity) ;
}
inline int * MESH::getReverseConnectivity(medConnectivity ConnectivityType)
{
  if (ConnectivityType==MED_NODAL)
    return _connectivity->getReverseNodalConnectivity() ;
  else if (ConnectivityType==MED_DESCENDING)
    return _connectivity->getReverseDescendingConnectivity() ;
  throw MEDEXCEPTION("MESH::getReverseConnectivity : connectivity mode not supported !");
}
inline int * MESH::getReverseConnectivityIndex(medConnectivity ConnectivityType)
{
  if (ConnectivityType==MED_NODAL)
    return _connectivity->getReverseNodalConnectivityIndex() ;
  else if (ConnectivityType==MED_DESCENDING)
    return _connectivity->getReverseDescendingConnectivityIndex() ;
  throw MEDEXCEPTION("MESH::getReverseConnectivityIndex : connectivity mode not supported !");
}


inline int MESH::getNumberOfFamilies (medEntityMesh entity) 
{
  switch (entity) {
  case MED_NODE : 
    return _numberOfNodesFamilies ;
  case MED_CELL : 
    return _numberOfCellsFamilies ;
  case MED_FACE :
    return _numberOfFacesFamilies ;
  case MED_EDGE :
    return _numberOfEdgesFamilies ;
  default :
    throw MEDEXCEPTION("MESH::getNumberOfFamilies : Unknown entity");
  }
}
inline int MESH::getNumberOfGroups (medEntityMesh entity) 
{
  switch (entity) {
  case MED_NODE : 
    return _numberOfNodesGroups ;
  case MED_CELL : 
    return _numberOfCellsGroups ;
  case MED_FACE :
    return _numberOfFacesGroups ;
  case MED_EDGE :
    return _numberOfEdgesGroups ;
  default :
    throw MEDEXCEPTION("MESH::getNumberOfGroups : Unknown entity");
  }
}
vector<FAMILY*> MESH::getFamilies(medEntityMesh entity) {
  switch (entity) {
  case MED_NODE : 
    return _familyNode ;
  case MED_CELL : 
    return _familyCell ;
  case MED_FACE :
    return _familyFace ;
  case MED_EDGE :
    return _familyEdge ;
  default :
    throw MEDEXCEPTION("MESH::getFamilies : Unknown entity");
  }
}

vector<GROUP*> MESH::getGroups(medEntityMesh entity) {
  switch (entity) {
  case MED_NODE : 
    return _groupNode ;
  case MED_CELL : 
    return _groupCell ;
  case MED_FACE :
    return _groupFace ;
  case MED_EDGE :
    return _groupEdge ;
  default :
    throw MEDEXCEPTION("MESH::getGroups : Unknown entity");
  }
}

FAMILY* MESH::getFamily(medEntityMesh entity, int i) {
  if (i<=0) 
    throw MEDEXCEPTION("MESH::getFamily(i) : argument i must be > 0");
  int NumberOfFamilies = 0 ;
  vector<FAMILY*> Family ;
  switch (entity) {
  case MED_NODE : {
    NumberOfFamilies = _numberOfNodesFamilies ;
    Family = _familyNode ;
    break ;
  }
  case MED_CELL : {
    NumberOfFamilies = _numberOfCellsFamilies ;
    Family = _familyCell ;
    break ;
  }
  case MED_FACE : {
    NumberOfFamilies = _numberOfFacesFamilies ;
    Family = _familyFace ;
    break ;
  }
  case MED_EDGE : {
    NumberOfFamilies = _numberOfEdgesFamilies ;
    Family = _familyEdge ;
    break ;
  }
  default :
    throw MEDEXCEPTION("MESH::getFamilies : Unknown entity");
  }
  if (i>NumberOfFamilies) 
    throw MEDEXCEPTION("MESH::getFamily(entity,i) : argument i must be <= _numberOfFamilies");
  return Family[i-1]; 
}

GROUP* MESH::getGroup(medEntityMesh entity, int i) {
  const char * LOC = "MESH::getGroup(medEntityMesh entity, int i) : " ;
  if (i<=0) 
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"argument i must be > 0"));
  int NumberOfGroups = 0 ;
  vector<GROUP*> Group ;
  switch (entity) {
  case MED_NODE : {
    NumberOfGroups = _numberOfNodesGroups ;
    Group = _groupNode ;
    break ;
  }
  case MED_CELL : {
    NumberOfGroups = _numberOfCellsGroups ;
    Group = _groupCell ;
    break ;
  }
  case MED_FACE : {
    NumberOfGroups = _numberOfFacesGroups ;
    Group = _groupFace ;
    break ;
  }
  case MED_EDGE : {
    NumberOfGroups = _numberOfEdgesGroups ;
    Group = _groupEdge ;
    break ;
  }
  default :
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Unknown entity"));
  }
  if (i>NumberOfGroups) 
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"argument i="<<i<<" must be <= _numberOfGroups="<<NumberOfGroups));
  return Group[i-1]; 
}


//    int * get_() {
//      return  ;
//    }

//inline void MESH::write(const string & driverName)  { 
//  write(0,driverName);
//}


// this method are temporary, perhaps we remove it if we put information in an other place
inline int * MESH::getMEDArrayNodeFamily() {
  return _MEDArrayNodeFamily ;
}
inline int ** MESH::getMEDArrayCellFamily() {
  return _MEDArrayCellFamily ;
}
inline int ** MESH::getMEDArrayFaceFamily() {
  return _MEDArrayFaceFamily ;
}
inline int ** MESH::getMEDArrayEdgeFamily() {
  return _MEDArrayEdgeFamily ;
}

#endif /* MESH_HXX */
