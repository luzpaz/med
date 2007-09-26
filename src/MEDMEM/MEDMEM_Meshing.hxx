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
  File MEDMEM_Meshing.hxx
  $Header$
*/

#ifndef MESHING_HXX
#define MESHING_HXX

#include "MEDMEM_Mesh.hxx"


namespace MEDMEM {

/*!
  This class MESHING is a special class to set a MESH object.
*/
class MESHING: public MESH
{
public :
  MESHING();
  ~MESHING();

  void setSpaceDimension   (int SpaceDimension) ;
  void setMeshDimension    (int MeshDimension) ;
  void setNumberOfNodes    (int NumberOfNodes) ;
  void setCoordinates      (int SpaceDimension,
			    int NumberOfNodes,
			    const double * Coordinates,
			    const string& System,
			    MED_EN::medModeSwitch Mode) ;
  void setCoordinatesSystem(const string& System)
    throw (MEDEXCEPTION) ;
  void setCoordinatesNames (const string * names) ;
  void setCoordinateName (const string& name, int i) ;
  void setCoordinatesUnits (const string * units) ;
  void setCoordinateUnit (const string& unit, int i) ;

  void setNumberOfTypes    (int NumberOfTypes,
			     MED_EN::medEntityMesh Entity)     throw (MEDEXCEPTION) ;
  void setTypes            (const MED_EN::medGeometryElement * Types,
			     MED_EN::medEntityMesh Entity)    throw (MEDEXCEPTION) ;
  void setNumberOfElements (const int * NumberOfElements,
			     MED_EN::medEntityMesh Entity)    throw (MEDEXCEPTION) ;
  void setConnectivity     (const int * Connectivity,
			     MED_EN::medEntityMesh Entity,
			     MED_EN::medGeometryElement Type)    throw (MEDEXCEPTION) ;
  
  void setPolygonsConnectivity     (const int * ConnectivityIndex,
				    const int * ConnectivityValue,
				    int nbOfPolygons,
				     MED_EN::medEntityMesh Entity)
    throw (MEDEXCEPTION) ;

  void setPolyhedraConnectivity     (const int * PolyhedronIndex,
				     const int * FacesIndex,
				     const int * Nodes,
				     int nbOfPolyhedra,
				     MED_EN::medEntityMesh Entity)
    throw (MEDEXCEPTION) ;

  void setConnectivities   (const int * ConnectivityIndex,
			    const int * ConnectivityValue,
			    MED_EN::medConnectivity ConnectivityType,
			    MED_EN::medEntityMesh Entity)
    throw (MEDEXCEPTION) ;

//   void setGroup            (const string name,
// 			    const string description,
// 			    const int NumberOfElements,
// 			    const int * ElementsNumbers,
// 			    const medEntityMesh Entity) ;
  void addGroup            (const GROUP & Group)
    throw (MEDEXCEPTION) ;
};
};

#endif /* MESHING_HXX */
