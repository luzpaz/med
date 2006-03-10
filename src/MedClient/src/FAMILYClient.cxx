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
// See http://www.salome-platform.org/
//
#include "FAMILYClient.hxx"
#include "MESHClient.hxx"
#include "UtilClient.hxx"
using namespace MEDMEM;
//=============================================================================
/*!
 * Constructeur
 */
//=============================================================================
FAMILYClient::FAMILYClient(const SALOME_MED::FAMILY_ptr S,
			     MESH * M) 
  : SUPPORTClient(S, M), 
    FAMILY(), 
    IOR_Family(SALOME_MED::FAMILY::_duplicate(S))
{
  BEGIN_OF("FAMILYClient::FAMILYClient(SALOME_MED::FAMILY_ptr m)");

  SCRUTE(S);
  SCRUTE(M);

  blankCopy(false);

  END_OF("FAMILYClient::FAMILYClient(SALOME_MED::FAMILY_ptr m)");
}
//=============================================================================
/*!
 * Remplit les informations générales
 */
//=============================================================================
void FAMILYClient::blankCopy(bool blankSupport)
{
  BEGIN_OF("FAMILYClient::blankCopy()");

  if (blankSupport)
    SUPPORTClient::blankCopy();

  setIdentifier(IOR_Family->getIdentifier());

  int nAttr, nGr, n;
  int *i;
  std::string *s;

  nAttr = IOR_Family->getNumberOfAttributes();
  setNumberOfAttributes(nAttr);

  convertCorbaArray(i, n, IOR_Family->getAttributesIdentifiers());
  ASSERT(n == nAttr);
  setAttributesIdentifiers(i);

  convertCorbaArray(i, n, IOR_Family->getAttributesValues());
  ASSERT(n == _numberOfAttribute);
  setAttributesValues(i);

  convertCorbaArray(s, n, IOR_Family->getAttributesDescriptions());
  ASSERT(n == _numberOfAttribute);
  setAttributesDescriptions(s);

  nGr = IOR_Family->getNumberOfGroups();
  setNumberOfGroups(nGr);

  convertCorbaArray(s, n, IOR_Family->getGroupsNames());
  ASSERT(n == _numberOfGroup);
  setGroupsNames(s);

  _complete = false;

  END_OF("FAMILYClient::blankCopy()");
}

//=============================================================================
/*!
 * Remplit les informations générales
 */
//=============================================================================
void FAMILYClient::fillCopy(bool fillSupport)
{
  BEGIN_OF("FAMILYClient::fillCopy()");

  if (!_complete) {

    if (fillSupport)
      SUPPORTClient::fillCopy();

    // Get other FAMILY attributes

    _complete = true;
  }

  END_OF("FAMILYClient::fillCopy()");
}

//=============================================================================
/*!
 * Destructeur
 */
//=============================================================================

FAMILYClient::~FAMILYClient()
{
  BEGIN_OF("FAMILYClient::~FAMILYClient()");
  END_OF("FAMILYClient::~FAMILYClient()");
}
