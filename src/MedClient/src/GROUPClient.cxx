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
#include "GROUPClient.hxx"
#include "MESHClient.hxx"
using namespace MEDMEM;

//=============================================================================
/*!
 * Constructeur
 */
//=============================================================================

GROUPClient::GROUPClient(const SALOME_MED::GROUP_ptr G,
			 MESH * M) 
  : SUPPORTClient(G, M),
    GROUP(),
    IOR_Group(SALOME_MED::GROUP::_duplicate(G))
{
  const char* LOC = "GROUPClient::GROUPClient()";
  BEGIN_OF(LOC);

  SCRUTE(G);
  SCRUTE(M);

  END_OF(LOC);
}

//=============================================================================
/*!
 * Remplit les informations générales
 */
//=============================================================================
void GROUPClient::blankCopy()
{
  const char* LOC = "GROUPClient::blankCopy()";
  BEGIN_OF(LOC);

  SUPPORTClient::blankCopy();
  _complete = false;

  END_OF(LOC);
}
//=============================================================================
/*!
 * Remplit les informations 
 */
//=============================================================================

void GROUPClient::fillCopy()
{
  const char* LOC = "GROUPClient::fillCopy()";
  BEGIN_OF(LOC);

  SUPPORTClient::fillCopy();
  _complete = true;

  END_OF(LOC);
}

//=============================================================================
/*!
 * Destructeur
 */
//=============================================================================

GROUPClient::~GROUPClient()
{
  const char* LOC = "GROUPClient::~GROUPClient()";
  BEGIN_OF(LOC);
  END_OF(LOC);
}
