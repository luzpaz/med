//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

//=============================================================================
// File      : MEDMEM_Med_i.cxx
// Project   : SALOME
// Author    : EDF
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_i.cxx
//=============================================================================
//
#include <deque>

//#include "MEDMEM_Field.hxx"
#include "MEDMEM_MedOp_i.hxx"
#include "utilities.h"
#include "Utils_CorbaException.hxx"

using namespace std;
using namespace MEDMEM;

//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
MEDOP_i::MEDOP_i(MEDMEM::MED * med)
{
  cout << "MEDOP::MEDOP: START" << endl;
  // We keep a reference to the MED object
  _med = med;
  cout << "MEDOP::MEDOP: END" << endl;
}

MEDOP_i::~MEDOP_i()
{
}



//=============================================================================
/*!
 * methods
 */
//=============================================================================
CORBA::Long MEDOP_i::test()
{
  cout << "MEDOP::test: START" << endl;
  int nbMeshes = _med->getNumberOfMeshes();
  cout << "MEDOP::test: nbMeshes = " << nbMeshes << endl;
  cout << "MEDOP::test: END" << endl;
  return 1;
}
