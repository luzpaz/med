/*
  File MEDMEM_Meshing.hxx
  $Header$
*/

#ifndef MESHING_HXX
#define MESHING_HXX

#include "MEDMEM_Mesh.hxx"

using namespace MED_EN;

/*!
  This class MESHING is a special class to set a MESH object.
*/

class MESHING: public MESH
{
public :
  MESHING();
  ~MESHING();

  void setSpaceDimension   (const int SpaceDimension) ;
  //void setMeshDimension    (const int MeshDimension) ;
  void setNumberOfNodes    (const int NumberOfNodes) ;
  void setCoordinates      (const int SpaceDimension,
			    const int NumberOfNodes,
			    const double * Coordinates,
			    const string System,
			    const medModeSwitch Mode) ;
  void setCoordinatesSystem(const string System)
    throw (MEDEXCEPTION) ;
  void setCoordinatesNames (const string * names) ;
  void setCoordinateName (const string name, const int i) ;
  void setCoordinatesUnits (const string * units) ;
  void setCoordinateUnit (const string unit, const int i) ;

  void setNumberOfTypes    (const int NumberOfTypes,
			    const medEntityMesh Entity) 
    throw (MEDEXCEPTION) ;
  void setTypes            (const medGeometryElement * Types,
			    const medEntityMesh Entity)
    throw (MEDEXCEPTION) ;
  void setNumberOfElements (const int * NumberOfElements,
			    const medEntityMesh Entity)
    throw (MEDEXCEPTION) ;
  void setConnectivity     (const int * Connectivity,
			    const medEntityMesh Entity,
			    const medGeometryElement Type)
    throw (MEDEXCEPTION) ;

  void setConnectivities   (const int * ConnectivityIndex,
			    const int * ConnectivityValue,
			    const medConnectivity ConnectivityType,
			    const medEntityMesh Entity)
    throw (MEDEXCEPTION) ;

//   void setGroup            (const string name,
// 			    const string description,
// 			    const int NumberOfElements,
// 			    const int * ElementsNumbers,
// 			    const medEntityMesh Entity) ;
  void addGroup            (const GROUP & Group)
    throw (MEDEXCEPTION) ;
};

#endif /* MESHING_HXX */
