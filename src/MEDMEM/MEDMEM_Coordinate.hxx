/*
 File Coordinate.hxx
 $Header$
*/

#ifndef COORDINATE_HXX
#define COORDINATE_HXX

#include <string>

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"
//using namespace MED_EN;


#include "MEDMEM_Array.hxx"

class COORDINATE {

protected:
				    /*! contains "CARTESIAN",
					"CYLINDRICAL" or "SPHERICAL"*/
  string            _coordinateSystem;

  // all in _coordinate object !!!
  //  int               _spaceDimension;
  //  int               _numberOfNodes;

				     /*! array of size spaceDimension*NumberOfNodes */
  MEDARRAY<double>* _coordinate;     
				     /*! array of size spaceDimension */
  string *          _coordinateName; 
				     /*! array of size spaceDimension */
  string *          _coordinateUnit; 
				     /*! array of size NumberOfNodes : optionnal nodes numbers */
  int    *          _nodeNumber;     

public :

  friend class MED_MESH_RDONLY_DRIVER;
  friend class MED_MESH_WRONLY_DRIVER;


  COORDINATE();
  COORDINATE(medModeSwitch Mode,int SpaceDimension, int NumberOfNodes);
  ~COORDINATE();

  void setCoordinates(MEDARRAY<double> *Coordinate);
  void setCoordinatesNames(string * CoordinateName);
  void setCoordinatesUnits(string * CoordinateUnit);
  void setNodesNumbers(int * NodeNumber);

  int*            getNodesNumbers() const;
  string          getCoordinatesSystem() const;
  const double *  getCoordinates(medModeSwitch Mode);
  double          getCoordinate(int Number,int Axis); 
  // return coordinate of node number Number, on axis Axis (1: X, 2: Y, 3: Z)
  const double * getCoordinateAxis(int Axis); 
  // return all nodes coordinates from axis Axis
  string * getCoordinatesNames(); 
  string   getCoordinateName(int Axis);
  string * getCoordinatesUnits();
  string   getCoordinateUnit(int Axis);
};

#endif /* COORDINATE_HXX */
