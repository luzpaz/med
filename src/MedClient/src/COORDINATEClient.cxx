#include "COORDINATEClient.hxx"
#include <string>
#include "UtilClient.hxx"
#include "Utils_CorbaException.hxx"

using namespace MEDMEM;
//=============================================================================
/*!
 * Constructeur
 */
//=============================================================================

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
//=============================================================================
/*!
 * Remplit les informations générales
 */
//=============================================================================
void COORDINATEClient::blankCopy()
{
  BEGIN_OF("void COORDINATEClient::blankCopy()");

  std::string *tA;
  long nA;
  SALOME_MED::MESH::coordinateInfos *all = new SALOME_MED::MESH::coordinateInfos;
  try
  {
        all= IOR_Mesh->getCoordGlobal();
  }
  catch (const exception & ex)
  {
        MESSAGE("Unable to acces Global information");
        THROW_SALOME_CORBA_EXCEPTION(ex.what() ,SALOME::INTERNAL_ERROR);
  }

  //convertCorbaArray(tA, nA, IOR_Mesh->getCoordinatesNames());
  convertCorbaArray(tA, nA, &all->coordNames);
  ASSERT(nA == getSpaceDimension());
  setCoordinatesNames(tA);
  delete [] tA;
  
  //convertCorbaArray(tA, nA, IOR_Mesh->getCoordinatesUnits());
  convertCorbaArray(tA, nA, &all->coordUnits);
  ASSERT(nA == getSpaceDimension());
  setCoordinatesUnits(tA);
  delete [] tA;
  

  setCoordinatesSystem( CORBA::string_dup(all->coordSystem));
  
  _complete = false;

  END_OF("void COORDINATEClient::blankCopy()");
}
//=============================================================================
/*!
 * Remplit les coordonnées 
 */
//=============================================================================

void COORDINATEClient::fillCopy()
{
  BEGIN_OF("void COORDINATEClient::fillCopy()");

  //PN ?? Est-ce qu on peut pas mettre une variable dans COORDINATEClient
  // qu on remplirait dans blankCopy ??
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

//=============================================================================
/*!
 * Retourne les coordonnées 
 */
//=============================================================================

const double *  COORDINATEClient::getCoordinates(medModeSwitch Mode)
{
  BEGIN_OF("void COORDINATEClient::getCoordinates()");

  if (!_complete) fillCopy();
  const double * c = COORDINATE::getCoordinates(Mode);

  END_OF("void COORDINATEClient::getCoordinates()");

  return c;
}
//=============================================================================
/*!
 * Retourne une coordonnée 
 */
//=============================================================================

double COORDINATEClient::getCoordinate(int Number,int Axis)
{
  BEGIN_OF("void COORDINATEClient::getCoordinate()");

  if (!_complete) fillCopy();
  double d = COORDINATE::getCoordinate(Number, Axis);

  END_OF("void COORDINATEClient::getCoordinate()");

  return d;
}
//=============================================================================
/*!
 * Retourne un axe 
 */
//=============================================================================
const double *  COORDINATEClient::getCoordinateAxis(int Axis)
{
  BEGIN_OF("void COORDINATEClient::getCoordinateAxis()");

  if (!_complete) fillCopy();
  const double *c = COORDINATE::getCoordinateAxis(Axis);

  END_OF("void COORDINATEClient::getCoordinateAxis()");

  return c;
}
//=============================================================================
/*!
 * Retourne le nombre de noeuds
 */
//=============================================================================
//CCRTconst int*      COORDINATEClient::getNodesNumbers() const
const med_int*      COORDINATEClient::getNodesNumbers() const
{
  BEGIN_OF("void COORDINATEClient::getNodesNumbers()");

  if (!_complete) (const_cast < COORDINATEClient * >(this))->fillCopy();

  MESSAGE("NON IMPLEMENTE DANS L'API CORBA");
  END_OF("void COORDINATEClient::getNodesNumbers()");

  return NULL;
}
