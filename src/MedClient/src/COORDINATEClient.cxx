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
#include "COORDINATEClient.hxx"
#include <string>
#include "UtilClient.hxx"
#include "Utils_CorbaException.hxx"

#include "ReceiverFactory.hxx"

using namespace MEDMEM;
using namespace MED_EN;

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
  SALOME_MED::MESH::coordinateInfos_var all;
  try
  {
        all = IOR_Mesh->getCoordGlobal();
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


  setCoordinatesSystem( all->coordSystem.in());

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

  SALOME::SenderDouble_var senderForCoords=IOR_Mesh->getSenderForCoordinates(MED_FULL_INTERLACE);
  tC=ReceiverFactory::getValue(senderForCoords,nC);

  ASSERT(nC == (getSpaceDimension() * nN));

  MEDARRAY<double> mC(tC, getSpaceDimension(), nN,MED_FULL_INTERLACE,true);
  setCoordinates(&mC,true);

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
const int*      COORDINATEClient::getNodesNumbers() const
{
  BEGIN_OF("void COORDINATEClient::getNodesNumbers()");

  if (!_complete) (const_cast < COORDINATEClient * >(this))->fillCopy();

  MESSAGE("NON IMPLEMENTE DANS L'API CORBA");
  END_OF("void COORDINATEClient::getNodesNumbers()");

  return NULL;
}
