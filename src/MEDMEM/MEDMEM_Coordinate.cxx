using namespace std;
#include "MEDMEM_Coordinate.hxx"
#include "utilities.h"

/*! This class contains coordinates of the nodes */
//----------------------------------------------------------//
COORDINATE::COORDINATE():_coordinate(),
			 _coordinateName(),
			 _coordinateUnit(),
			 _coordinateSystem(""),
			 _nodeNumber()
//----------------------------------------------------------//
{
    BEGIN_OF("Default Constructor COORDINATE");
}

//------------------------------------------------------------------------------//
COORDINATE::COORDINATE(medModeSwitch Mode, int SpaceDimension, int NumberOfNodes):
                        _nodeNumber(),_coordinateUnit(SpaceDimension),
			_coordinateSystem(""),
			_coordinateName(SpaceDimension)
//------------------------------------------------------------------------------//
{
    BEGIN_OF("Constructor COORDINATE");
    _coordinate = new MEDARRAY<double>(SpaceDimension,NumberOfNodes,Mode);
}

//------------------------------------------------------------------------------//
COORDINATE::COORDINATE(const COORDINATE & m):
    			_coordinateSystem(m._coordinateSystem)
//------------------------------------------------------------------------------//
{
  BEGIN_OF("Copy Constructor COORDINATE");
  int spaceDimension;
  int numberOfNodes;
  if (m._coordinate != NULL)
    {
      spaceDimension = (int) m._coordinate->getLeadingValue();
      numberOfNodes = (int) m._coordinate->getLengthValue();
      _coordinate = new MEDARRAY<double>(*m._coordinate);
    }
  else 
    {
      _coordinate = (MEDARRAY<double>*) NULL;
      spaceDimension = 0;
      numberOfNodes = 0;
    } 
  
  _coordinateName.set(spaceDimension);
  for (int i=0; i<spaceDimension; i++)
    {
      _coordinateName[i]=m._coordinateName[i];
    }
  
  _coordinateUnit.set(spaceDimension);
  for (int i=0; i<spaceDimension; i++)
    {
      _coordinateUnit[i]=m._coordinateUnit[i];
    }
  
  // PN A VERIFIER
  _nodeNumber.set(numberOfNodes);
  if (m._nodeNumber != NULL)
    {
      memcpy(_nodeNumber,m._nodeNumber,numberOfNodes*sizeof(int));
    }
}


//----------------------//
COORDINATE::~COORDINATE()
//----------------------//
{
  MESSAGE("~COORDINATE()");
  if (_coordinate!=NULL)
    {
      MESSAGE("deleting _coordinate" ) ;
      delete _coordinate ;
    }
  // all other attribut are object (not pointer)
}

/*! set the attribute _coordinate with Coordinate           */
//----------------------------------------------------------//
void COORDINATE::setCoordinates(MEDARRAY<double> *Coordinate) 
//----------------------------------------------------------//
{ 
//PN a voir ...
    if ((_coordinate!=NULL) )
    {
      MESSAGE("deleting  old _coordinate" ) ;
      delete _coordinate ;
    }
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
	_coordinateUnit.set( CoordinateUnit ) ; 
}

/*! set the attribute _coordinateSystem with CoordinateSystem   */
//----------------------------------------------------------//
void COORDINATE::setCoordinatesSystem(string CoordinateSystem) 
//----------------------------------------------------------//
{ 
	_coordinateSystem=CoordinateSystem; 
}

/*! set the attribute _nodeNumber with NodeNumber */
//------------------------------------------------//
void COORDINATE::setNodesNumbers(int * NodeNumber) 
//------------------------------------------------//
{ 	
	_nodeNumber.set(NodeNumber) ; 
}

/*! returns the number of nodes defined in the mesh*/
//-------------------------------------------------//
int * COORDINATE::getNodesNumbers() 
//-------------------------------------------------//
{
	return  _nodeNumber;
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
