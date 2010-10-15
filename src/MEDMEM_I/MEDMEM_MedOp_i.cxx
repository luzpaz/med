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
// File      : MEDMEM_MedOp_i.cxx
// Project   : SALOME
// Author    : EDF
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_i.cxx
//=============================================================================
//
#include <deque>

#include "MEDMEM_MedOp_i.hxx"
#include "MEDMEM_Field_i.hxx"
#include "MEDMEM_FieldTemplate_i.hxx"

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
  _medDataManager = new MedDataManager(_med);
  cout << "MEDOP::MEDOP: END" << endl;
}

MEDOP_i::~MEDOP_i()
{
}

//   // We analyse the mathematical type of values in the field to
//   // create a "typed" field that can receive the physical data.
//   med_type_champ type = myFieldDescr->getValueType();
//     switch (type) {
//     case MED_EN::MED_INT32 : 
//       {
//         FIELD<double> *field = (FIELD<double> *)
//         break;
//       }

//     case MED_EN::MED_REEL64: 
//       {
//         break;
//       }
//     default:
//       {
//         break;
//       }
//     }

//   }
  
//   return myFieldDescr;
// }

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

  MEDMEM::FIELD<double> * field = _medDataManager->getFieldDouble("testfield1",-1,-1);
  _medDataManager->printFieldDouble(field);

  // Can we create a field_ptr from this field?
  

  return 1;
}

MEDMEM::FIELD<double> * MEDOP_i::_getFieldDouble(SALOME_MED::FIELD_ptr field_ptr) {
  string name = field_ptr->getName();
  long dt = field_ptr->getOrderNumber();
  long it = field_ptr->getIterationNumber();

  MEDMEM::FIELD<double> * field = _medDataManager->getFieldDouble(name,dt,it);
  return field;
}


SALOME_MED::FIELD_ptr MEDOP_i::addition(SALOME_MED::FIELD_ptr f1_ptr, SALOME_MED::FIELD_ptr f2_ptr) {
  MEDMEM::FIELD<double> * f1 = _getFieldDouble(f1_ptr);
  MEDMEM::FIELD<double> * f2 = _getFieldDouble(f2_ptr);

  FIELD<double> field_addition = *f1 + *f2;
  field_addition.setName("toto");

  FIELDTEMPLATE_I<double> *field_addition_i = new FIELDTEMPLATE_I<double>(&field_addition);
  return field_addition_i->_this();
}
