using namespace std;
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_STRING.hxx"

#include "utilities.h"
using namespace MEDMEM;

/*! Default Constructor : should not be used */
//----------------------------------------------------------//
COORDINATE::COORDINATE():_coordinateSystem(""),
			 _coordinate(MEDARRAY<double>()),
			 _coordinateName(),
			 _coordinateUnit(),
			 _nodeNumber()
//----------------------------------------------------------//
{
    BEGIN_OF("Default Constructor COORDINATE");
}

/*! This constructor allocate a MEDARRAY of SpaceDimension * NumberOfNodes.\n
    It will create empty array for optional data (nodeNumber..) */
//------------------------------------------------------------------------------//
COORDINATE::COORDINATE(int SpaceDimension, int NumberOfNodes, medModeSwitch Mode):
  _coordinateSystem(""),
  _coordinate(SpaceDimension,NumberOfNodes,Mode),
  _coordinateName(SpaceDimension),
  _coordinateUnit(SpaceDimension),
  _nodeNumber()

//------------------------------------------------------------------------------//
{
    BEGIN_OF("Constructor COORDINATE");
}

/*! This constructor will COPY all data (it is a deep copy) included in m.\n  
    But only the default storage mode of coordinates array
    will be copied (not both storage representation modes even if they both
    exist in original object) :  for example only full_interlace mode  */
//------------------------------------------------------------------------------//
COORDINATE::COORDINATE(const COORDINATE & m):
  _coordinateSystem(m._coordinateSystem),
  _coordinate(m._coordinate,false)
//------------------------------------------------------------------------------//
{
  BEGIN_OF("Copy Constructor COORDINATE");

  int spaceDimension = _coordinate.getLeadingValue();
  int numberOfNodes = _coordinate.getLengthValue();

  SCRUTE(spaceDimension);
  setCoordinatesNames((const string*)m._coordinateName) ;
  setCoordinatesUnits((const string*)m._coordinateUnit) ;

  if ( (const int * const) m._nodeNumber != NULL)
    _nodeNumber.set(numberOfNodes,(const med_int*)m._nodeNumber);
  // PG : it's default no ?
//    else
//      {
//        _nodeNumber.set(0);
//      }
}


/* does nothing :   all attributs are object (not pointers) */
//----------------------//
COORDINATE::~COORDINATE()
//----------------------//
{
  MESSAGE("~COORDINATE()");
}

/*! sets the attribute _coordinate with Coordinate           */
//----------------------------------------------------------//
void COORDINATE::setCoordinates(MEDARRAY<double> *Coordinate,bool shallowCopy) 
//----------------------------------------------------------//
{ 

  const medModeSwitch mode = Coordinate->getMode(); 
  //  const int  spaceDimension = (int) Coordinate->getLeadingValue();
  //  const int  numberOfNodes  = (int) Coordinate->getLengthValue();
  if ( Coordinate->get(mode) != NULL)
  {
    if(shallowCopy)
      {
	_coordinate.shallowCopy(*Coordinate);
      }
    else
      {
	MEDARRAY<double> pourAttribut(*Coordinate,false);
	_coordinate = pourAttribut;
	//_coordinate.set(mode,Coordinate->get(mode));
      }
  }
  else
  {
	throw MED_EXCEPTION ( LOCALIZED(STRING("setCoordinates(MEDARRAY<double> *Coordinate)") << "No Coordinate"));
  }
}

/*!
  Sets the value in attribute _coordinate with Coordinate.
  _coordinate must be allocated (with 
  COORDINATE::COORDINATE(int,int,medModeSwitch).
*/
//----------------------------------------------------------//
void COORDINATE::setCoordinates(const medModeSwitch Mode, 
				const double *Coordinate) 
//----------------------------------------------------------//
{ 
//    if (_coordinate == NULL)
//      throw MEDEXCEPTION("COORDINATE::setCoordinates(double*) : coordinate array not allocated !");

  _coordinate.set(Mode,Coordinate);
}

/*! sets the attribute _coordinateName with CoordinateName   */
//----------------------------------------------------------//
void COORDINATE::setCoordinatesNames(const string * CoordinateName) 
//----------------------------------------------------------//
{
  int SpaceDimension = getSpaceDimension() ;
  _coordinateName.set(SpaceDimension) ;
  for (int i=0; i<SpaceDimension; i++)
    _coordinateName[i]=CoordinateName[i];
}

/*!
  sets the (i+1)^th component of the attribute _coordinateName with
  CoordinateName
*/
//----------------------------------------------------------//
void COORDINATE::setCoordinateName(const string CoordinateName, const int i)
//----------------------------------------------------------//
{
  _coordinateName[i]=CoordinateName;
}

/*! sets the attribute _coordinateUnit with CoordinateUnit   */
//----------------------------------------------------------//
void COORDINATE::setCoordinatesUnits(const string * CoordinateUnit) 
//----------------------------------------------------------//
{ 
  int SpaceDimension = getSpaceDimension() ;
  _coordinateUnit.set(SpaceDimension) ; 
  for (int i=0; i<SpaceDimension; i++)
    _coordinateUnit[i]=CoordinateUnit[i];
}

/*!
  sets the (i+1)^th component of the attribute _coordinateUnit with
  CoordinateUnit
*/
//----------------------------------------------------------//
void COORDINATE::setCoordinateUnit(const string CoordinateUnit, const int i) 
//----------------------------------------------------------//
{ 
  _coordinateUnit[i]=CoordinateUnit;
}

/*! sets the attribute _coordinateSystem with CoordinateSystem   */
//----------------------------------------------------------//
void COORDINATE::setCoordinatesSystem(const string CoordinateSystem) 
//----------------------------------------------------------//
{ 
	_coordinateSystem=CoordinateSystem; 
}

/*! sets the attribute _nodeNumber with NodeNumber */
//------------------------------------------------//
void COORDINATE::setNodesNumbers(const int * NodeNumber) 
//------------------------------------------------//
{ 	
  int NumberOfNodes = getNumberOfNodes() ;
  _nodeNumber.set(NumberOfNodes,NodeNumber) ; 
}

int COORDINATE::getSpaceDimension() const
{ 	
  return _coordinate.getLeadingValue() ; 
}

int COORDINATE::getNumberOfNodes() const
{ 	
  return _coordinate.getLengthValue() ; 
}


/*! returns a pointer to the optional array storing 
    eventual nodes numbers */
//-------------------------------------------------//
const int * COORDINATE::getNodesNumbers() const
//-------------------------------------------------//
{
	return  (const int *)_nodeNumber;
}

/*! returns a Pointer to Coordinates Array in specified mode representation */
//--------------------------------------------------------------------------//
const double *  COORDINATE::getCoordinates (medModeSwitch Mode)
//--------------------------------------------------------------------------//
{
	return _coordinate.get(Mode) ;
}

/* returns the coordinate of node Number on axis Axis */
//----------------------------------------------------//
double COORDINATE::getCoordinate(int Number,int Axis) 
//----------------------------------------------------//
{ 	
      	return _coordinate.getIJ(Number,Axis) ;
}

/* returns all nodes coordinates from  axis Axis      */
//----------------------------------------------------//
const double *  COORDINATE::getCoordinateAxis(int Axis)
//----------------------------------------------------//
{ 			//< return all nodes coordinates from axis Axis
     	 return _coordinate.getColumn(Axis) ;
}

/*! returns an array with names of coordinates. \n
      Example : \n
      - x,y,z \n
      - r,teta,phi \n
      - ... */
//--------------------------------------//
const string * COORDINATE::getCoordinatesNames() const
{
      	return _coordinateName ;
}

/* returns the name of axis Axis             */
//-------------------------------------------//
string COORDINATE::getCoordinateName(int Axis) const
//-------------------------------------------//
{
      	return _coordinateName[Axis-1];
}

/*!  returns an array with units of coordinates (cm, m, mm, ...)
     It could be empty. We suppose we are IS (meter).  */
//-----------------------------------------------------//
const string * COORDINATE::getCoordinatesUnits() const
//-----------------------------------------------------//
{
      	return _coordinateUnit ;
}

/*! returns the unit of axis Axis           */
//------------------------------------------//
string COORDINATE::getCoordinateUnit(int Axis) const
//------------------------------------------//
{
      	return _coordinateUnit[Axis-1] ;
}
/*! returns "CARTESIAN", "CYLINDRICAL" or "SPHERICAL"*/
//---------------------------------------------------//
string COORDINATE::getCoordinatesSystem() const
//---------------------------------------------------//
{
  return _coordinateSystem ;
}
