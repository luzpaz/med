//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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

/*!
 * This function is created for test purposes. It is present in the
 * CORBA interface and can be used to test some features of this class
 * from the CORBA environment of SALOME.
 */
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
 * This function realizes the addition of the MEDMEM::FIELD objects
 * embedded in the given servants specified by there CORBA
 * pointers. It returns a pointer to the servant that embeds the
 * resulting MEDMEM::FIELD. Note that the servant of a FIELD
 * containing values is an instance of the class
 * FIELDTEMPLATE_I<double/int>.
 */
SALOME_MED::FIELD_ptr MEDOP_i::addition(SALOME_MED::FIELD_ptr f1_ptr, SALOME_MED::FIELD_ptr f2_ptr) {

  // __GBO__ in the final version, we should process here the
  // numerical type of the values (int or double). For this demo
  // version, only double field are taken into account
  MEDMEM::FIELD<double> * f1 = _getFieldDouble(f1_ptr);
  MEDMEM::FIELD<double> * f2 = _getFieldDouble(f2_ptr);
  
  FIELD<double> * field_result = FIELD<double>::add(*f1, *f2);
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
  _med->addField(field_result);

  //
  // We keep a trace of this object in a map whose key is a global
  // name including the name of the field and the order and iteration
  // numbers, so that we can manage the life cycle.
  //string key = this->_getKeyIdentifier(&field_result);
  //_fielddouble_map[key.c_str()] = field_result;

  FIELDTEMPLATE_I<double> *field_result_i = new FIELDTEMPLATE_I<double>(field_result);
  return field_result_i->_this();
}


/*!
 * This function realizes the substraction of the MEDMEM::FIELD
 * objects embedded in the given servants specified by there CORBA
 * pointers. It returns a pointer to the servant that embeds the
 * resulting MEDMEM::FIELD. Note that the servant of a FIELD
 * containing values is an instance of the class
 * FIELDTEMPLATE_I<double/int>.
 */
SALOME_MED::FIELD_ptr MEDOP_i::substraction(SALOME_MED::FIELD_ptr f1_ptr, SALOME_MED::FIELD_ptr f2_ptr) {
  MEDMEM::FIELD<double> * f1 = _getFieldDouble(f1_ptr);
  MEDMEM::FIELD<double> * f2 = _getFieldDouble(f2_ptr);
  
  FIELD<double> * field_result = FIELD<double>::sub(*f1, *f2);
  _med->addField(field_result);

  FIELDTEMPLATE_I<double> *field_result_i = new FIELDTEMPLATE_I<double>(field_result);
  return field_result_i->_this();
}

/*!
 * This function realizes the multiplication of the MEDMEM::FIELD
 * objects embedded in the given servants specified by there CORBA
 * pointers. It returns a pointer to the servant that embeds the
 * resulting MEDMEM::FIELD. Note that the servant of a FIELD
 * containing values is an instance of the class
 * FIELDTEMPLATE_I<double/int>.
 */
SALOME_MED::FIELD_ptr MEDOP_i::multiplication(SALOME_MED::FIELD_ptr f1_ptr, SALOME_MED::FIELD_ptr f2_ptr) {
  MEDMEM::FIELD<double> * f1 = _getFieldDouble(f1_ptr);
  MEDMEM::FIELD<double> * f2 = _getFieldDouble(f2_ptr);
  
  FIELD<double> * field_result = FIELD<double>::mul(*f1, *f2);
  _med->addField(field_result);

  FIELDTEMPLATE_I<double> *field_result_i = new FIELDTEMPLATE_I<double>(field_result);
  return field_result_i->_this();
}

/*!
 * This function realizes the division of the MEDMEM::FIELD objects
 * embedded in the given servants specified by there CORBA
 * pointers. It returns a pointer to the servant that embeds the
 * resulting MEDMEM::FIELD. Note that the servant of a FIELD
 * containing values is an instance of the class
 * FIELDTEMPLATE_I<double/int>.
 */
SALOME_MED::FIELD_ptr MEDOP_i::division(SALOME_MED::FIELD_ptr f1_ptr, SALOME_MED::FIELD_ptr f2_ptr) {
  MEDMEM::FIELD<double> * f1 = _getFieldDouble(f1_ptr);
  MEDMEM::FIELD<double> * f2 = _getFieldDouble(f2_ptr);
  
  FIELD<double> * field_result = FIELD<double>::div(*f1, *f2);
  _med->addField(field_result);

  FIELDTEMPLATE_I<double> *field_result_i = new FIELDTEMPLATE_I<double>(field_result);
  return field_result_i->_this();
}

/*!
 * This function realizes the power of the MEDMEM::FIELD object
 * embedded in the given servant specified by its CORBA pointer. It
 * returns a pointer to the servant that embeds the resulting
 * MEDMEM::FIELD. Note that the servant of a FIELD containing values
 * is an instance of the class FIELDTEMPLATE_I<double/int>.
 */
SALOME_MED::FIELD_ptr MEDOP_i::pow(SALOME_MED::FIELD_ptr f1_ptr, long power) {
  MEDMEM::FIELD<double> * f1 = _getFieldDouble(f1_ptr);
  
  // The MEDMEM pow operation modifies the original field (that is not
  // what we want). So we have first to make a copy.
  FIELD<double> * field_result = new FIELD<double>(*f1);
  field_result->applyPow(power);
  _med->addField(field_result);

  FIELDTEMPLATE_I<double> *field_result_i = new FIELDTEMPLATE_I<double>(field_result);
  return field_result_i->_this();
}

/*!
 * This function creates a new field as the linear transformation of
 * the MEDMEM::FIELD object embedded in the given servant specified by
 * its CORBA pointers. The transformation is y= factor*x + offset. 
 *
 * It returns a pointer to the servant that embeds the resulting
 * MEDMEM::FIELD. Note that the servant of a FIELD containing values
 * is an instance of the class FIELDTEMPLATE_I<double/int>.
 */
SALOME_MED::FIELD_ptr MEDOP_i::lin(SALOME_MED::FIELD_ptr f1_ptr, double factor, double offset) {
  MEDMEM::FIELD<double> * f1 = _getFieldDouble(f1_ptr);
  
  // The MEDMEM operation applyLin modifies the original field (that
  // is not what we want). So we have first to make a copy.
  FIELD<double> * field_result = new FIELD<double>(*f1);
  *field_result+=*f1;
  field_result->applyLin(factor, offset);
  _med->addField(field_result);

  FIELDTEMPLATE_I<double> *field_result_i = new FIELDTEMPLATE_I<double>(field_result);
  return field_result_i->_this();
}
