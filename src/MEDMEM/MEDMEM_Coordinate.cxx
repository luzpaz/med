#include "MEDMEM_Coordinate.hxx"
#include "utilities.h"

/*! This class contains coordinates of the nodes */
//----------------------------------------------------------//
COORDINATE::COORDINATE():_coordinate((MEDARRAY<double>*)NULL),
			 _coordinateName((string*)NULL),
			 _coordinateUnit((string*)NULL),
			 _nodeNumber((int*)NULL)
//----------------------------------------------------------//
{
    BEGIN_OF("Default Constructor COORDINATE");
}

//------------------------------------------------------------------------------//
COORDINATE::COORDINATE(medModeSwitch Mode,int SpaceDimension, int NumberOfNodes):
			_nodeNumber((int*)NULL) 
//------------------------------------------------------------------------------//
{
    BEGIN_OF("Constructor COORDINATE");
    _coordinateName = new string[SpaceDimension] ;
    _coordinateUnit = new string[SpaceDimension] ;
    _coordinate = new MEDARRAY<double>(SpaceDimension,NumberOfNodes,Mode);
}
//----------------------//
COORDINATE::~COORDINATE()
//----------------------//
{
    MESSAGE("Debut Destructeur COORDINATE");
    if (_coordinate!=NULL)
      delete _coordinate ;
    if (_coordinateName!=NULL)
      delete[] _coordinateName ;
    if (_coordinateUnit!=NULL)
      delete[] _coordinateUnit ;
    if (_nodeNumber!=NULL)
      delete[] _nodeNumber ;
  }

/*! set the attribute _coordinate with Coordinate           */
//----------------------------------------------------------//
void COORDINATE::setCoordinates(MEDARRAY<double> *Coordinate) 
//----------------------------------------------------------//
{ 
	_coordinate=Coordinate ; 
}

/*! set the attribute _coordinateName with CoordinateName   */
//----------------------------------------------------------//
void COORDINATE::setCoordinatesNames(string * CoordinateName) 
//----------------------------------------------------------//
{ 	
	_coordinateName=CoordinateName ; 
}

/*! set the attribute _coordinateUnit with CoordinateUnit   */
//----------------------------------------------------------//
void COORDINATE::setCoordinatesUnits(string * CoordinateUnit) 
//----------------------------------------------------------//
{ 
	_coordinateUnit=CoordinateUnit ; 
}

/*! set the attribute _nodeNumber with NodeNumber */
//------------------------------------------------//
void COORDINATE::setNodesNumbers(int * NodeNumber) 
//------------------------------------------------//
{ 	
	_nodeNumber=NodeNumber ; 
}

/*! returns the number of nodes defined in the mesh*/
//-------------------------------------------------//
int * COORDINATE::getNodesNumbers() const
//-------------------------------------------------//
{
	return _nodeNumber;
}

/*! returns the mode of coordinates (FULL_INTERLACE or NO_INTERLACE) */
//-------------------------------------------------------------------//
const double *  COORDINATE::getCoordinates (medModeSwitch Mode) 
//-------------------------------------------------------------------//
{
	return _coordinate->get(Mode) ;
}

/* returns the coordinate of node Number on axis Axis */
//----------------------------------------------------//
double COORDINATE::getCoordinate(int Number,int Axis) 
//----------------------------------------------------//
{ 	
      	return _coordinate->getIJ(Number,Axis) ;
}

/* returns all nodes coordinates from  axis Axis      */
//----------------------------------------------------//
const double *  COORDINATE::getCoordinateAxis(int Axis) 
//----------------------------------------------------//
{ 			//< return all nodes coordinates from axis Axis
     	 return _coordinate->getI(MED_NO_INTERLACE,Axis) ;
}

/*! Returns an array with names of coordinates.
      Example :
      - x,y,z
      - r,teta,phi
      - ... */
//--------------------------------------//
string * COORDINATE::getCoordinatesNames() 
{
      	return _coordinateName ;
}

/* returns the name of axis Axis             */
//-------------------------------------------//
string COORDINATE::getCoordinateName(int Axis) 
//-------------------------------------------//
{
      	return _coordinateName[Axis-1];
}

/*!  Returns an array with units of coordinates (cm, m, mm, ...)
     It could be empty. We suppose we are IS (meter).  */
//-----------------------------------------------------//
string * COORDINATE::getCoordinatesUnits() 
//-----------------------------------------------------//
{
      	return _coordinateUnit ;
}

/*! Returns the unit of axis Axis           */
//------------------------------------------//
string COORDINATE::getCoordinateUnit(int Axis) 
//------------------------------------------//
{
      	return _coordinateUnit[Axis-1] ;
}
/*! Returns "CARTESIAN", "CYLINDRICAL" or "SPHERICAL"*/
//---------------------------------------------------//
string COORDINATE::getCoordinatesSystem() const
//---------------------------------------------------//
{
  return _coordinateSystem ;
}
