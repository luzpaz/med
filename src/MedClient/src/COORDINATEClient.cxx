#include "COORDINATEClient.hxx"
#include <string>
#include "UtilClient.hxx"

COORDINATEClient::COORDINATEClient(const SALOME_MED::MESH_ptr m, 
				   medModeSwitch Mode) :
  COORDINATE(m->getSpaceDimension(), 1, Mode),
  _complete(false), 
  IOR_Mesh(SALOME_MED::MESH::_duplicate(m))
{
  BEGIN_OF("COORDINATEClient::COORDINATEClient(...)");

  blankCopy();

  END_OF("COORDINATEClient::COORDINATEClient(...)");
}

void COORDINATEClient::blankCopy()
{
  BEGIN_OF("void COORDINATEClient::blankCopy()");

  std::string *tA;
  long nA;
  
  convertCorbaArray(tA, nA, IOR_Mesh->getCoordinatesNames());
  ASSERT(nA == getSpaceDimension());
  setCoordinatesNames(tA);
  delete [] tA;
  
  convertCorbaArray(tA, nA, IOR_Mesh->getCoordinatesUnits());
  ASSERT(nA == getSpaceDimension());
  setCoordinatesUnits(tA);
  delete [] tA;
  
  setCoordinatesSystem(IOR_Mesh->getCoordinatesSystem());
  
  _complete = false;

  END_OF("void COORDINATEClient::blankCopy()");
}

void COORDINATEClient::fillCopy()
{
  BEGIN_OF("void COORDINATEClient::fillCopy()");

  long nN = IOR_Mesh->getNumberOfNodes();
  double *tC;
  long nC;
  convertCorbaArray(tC, nC, IOR_Mesh->getCoordinates(MED_FULL_INTERLACE));
  ASSERT(nC == (getSpaceDimension() * nN));
  
  MEDARRAY<double> mC(tC, getSpaceDimension(), nN);
  setCoordinates(&mC);
  
  _complete = true;
  
  END_OF("void COORDINATEClient::fillCopy()");
}


const double *  COORDINATEClient::getCoordinates(medModeSwitch Mode)
{
  BEGIN_OF("void COORDINATEClient::getCoordinates()");

  if (!_complete) fillCopy();
  const double * c = COORDINATE::getCoordinates(Mode);

  END_OF("void COORDINATEClient::getCoordinates()");

  return c;
}

double          COORDINATEClient::getCoordinate(int Number,int Axis)
{
  BEGIN_OF("void COORDINATEClient::getCoordinate()");

  if (!_complete) fillCopy();
  double d = COORDINATE::getCoordinate(Number, Axis);

  END_OF("void COORDINATEClient::getCoordinate()");

  return d;
}

const double *  COORDINATEClient::getCoordinateAxis(int Axis)
{
  BEGIN_OF("void COORDINATEClient::getCoordinateAxis()");

  if (!_complete) fillCopy();
  const double *c = COORDINATE::getCoordinateAxis(Axis);

  END_OF("void COORDINATEClient::getCoordinateAxis()");

  return c;
}

const int*      COORDINATEClient::getNodesNumbers() const
{
  BEGIN_OF("void COORDINATEClient::getNodesNumbers()");

  if (!_complete) (const_cast < COORDINATEClient * >(this))->fillCopy();

  MESSAGE("NON IMPLEMENTE DANS L'API CORBA");
  END_OF("void COORDINATEClient::getNodesNumbers()");

  return NULL;
}
