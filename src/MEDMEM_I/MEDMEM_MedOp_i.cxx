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

// Template function for type conversions.
#include <sstream>
template < class T >
string ToString(const T &arg)
{
  stringstream out;
  out << arg;
  return(out.str());
}

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
  delete _medDataManager;
  // __GBO__: Is the map FIELDDOUBLE_HashMap automatically deleted?
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

  MEDMEM::FIELD<double> * field = _medDataManager->getFieldDouble("testfield1",-1,-1);
  _medDataManager->printFieldDouble(field);

  // Can we create a field_ptr from this field?
  

  return 1;
}

/*!
 * This function retrieves the original MEDMEM::FIELD object embedded
 * in the servant pointed by the specified field_ptr. This works only
 * for usage in the container that embed the MED data structure.
 */
MEDMEM::FIELD<double> * MEDOP_i::_getFieldDouble(SALOME_MED::FIELD_ptr field_ptr) {
  string name = field_ptr->getName();
  long dt = field_ptr->getOrderNumber();
  long it = field_ptr->getIterationNumber();

  MEDMEM::FIELD<double> * field = _medDataManager->getFieldDouble(name,dt,it);
  return field;
}

/*!
 * This function creates a unique string identifier for the specified
 * field. For internal usage and for management purpose.
 */
const string MEDOP_i::_getKeyIdentifier(MEDMEM::FIELD_ * field) {
  string key = field->getName() + "_" +
    ToString(field->getOrderNumber()) + "_" +
    ToString(field->getIterationNumber());
  return key;
}

/*!
 * This function realizes the physical addition of the MEDMEM::FIELD
 * objects embedded in the servants specified by there CORBA
 * pointers. It returns a pointer to the servant that embeds the
 * resulting MEDMEM::FIELD. Note that the servant of a FIELD
 * containing values is an instance of the class
 * FIELDTEMPLATE_I<double/int>.
 */
SALOME_MED::FIELD_ptr MEDOP_i::addition(SALOME_MED::FIELD_ptr f1_ptr, SALOME_MED::FIELD_ptr f2_ptr) {
  MEDMEM::FIELD<double> * f1 = _getFieldDouble(f1_ptr);
  MEDMEM::FIELD<double> * f2 = _getFieldDouble(f2_ptr);
  
  FIELD<double> * field_addition = FIELD<double>::add(*f1, *f2);
  // Note that a "new" is done behind the scene of this addition. Then
  // the life cycle of this object has to be managed by the
  // application.
  
  //
  // The field is automatically added to the med data structure so
  // that it could be reused in a future operation (in particular for
  // a manipulation that combine several operation as "r = f1+f2+f3",
  // the operator will try to make the addition f1+f2 and then
  // resultof(f1+f2) + f3. This last operation will fail if the field
  // resultof(f1+f2) is not in the med data structure)
  //
  _med->addField(field_addition);

  //
  // We keep a trace of this object in a map whose key is a global
  // name including the name of the field and the order and iteration
  // numbers, so that we can manage the life cycle.
  //string key = this->_getKeyIdentifier(&field_addition);
  //_fielddouble_map[key.c_str()] = field_addition;

  FIELDTEMPLATE_I<double> *field_addition_i = new FIELDTEMPLATE_I<double>(field_addition);
  return field_addition_i->_this();
}
