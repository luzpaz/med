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

#include "MEDMEM_MedDataManager.hxx"
#include "MEDMEM_MedMedDriver.hxx"
#include "MEDMEM_Mesh.hxx"
using namespace MEDMEM;

#include <iostream>
using namespace std;


MedDataManager::MedDataManager(string filename) {
  _med = new MED;
  MED_MED_RDONLY_DRIVER *driverIn = new MED_MED_RDONLY_DRIVER(filename, _med);
  driverIn->open();
  driverIn->readFileStruct();
  driverIn->close();
}

MedDataManager::MedDataManager(MED * med) {
  _med = med;
}


/**
 * This function returns the specified field and manage the loading of
 * physical data if needed. This required the isRead() improvment on
 * classe FIELD_ of MEDMEM (CVS branch BR_medop).
 */
FIELD_ * MedDataManager::getField(const string & fieldName, const int dt,  const int it) {
  FIELD_ * myFieldDescr = _med->getField(fieldName, dt, it);
  if ( ! myFieldDescr->isRead() ) {
    cout << "### getField: the data are not read ==> loading ..." << endl;
    // We first have to load the mesh data
    MESH *mesh = _med->getMesh(myFieldDescr);
    if ( ! mesh->isRead() ) {
      mesh->read();
      _med->updateSupport();
    }

    // Then we can load the physical values
    myFieldDescr->read();
  }
  cout << "### getField: the data are loaded ==> ok" << endl;
  return myFieldDescr;
}

FIELD<double> * MedDataManager::getFieldDouble(const string & fieldName, const int dt,  const int it) {
  return dynamic_cast< FIELD<double> * >(getField(fieldName, dt, it));
}

FIELD<double> * MedDataManager::getFieldDoubleFromId(int fieldIdx, int iterationIdx) {
  string fieldname = _med->getFieldNames()[fieldIdx];
  deque<DT_IT_> iterations = _med->getFieldIteration(fieldname);
  return getFieldDouble(fieldname,
                        iterations[iterationIdx].dt,
                        iterations[iterationIdx].it);
}

void MedDataManager::printFieldDouble(FIELD<double> * field) {
  int NumberOfElements = field->getSupport()->getNumberOfElements(MED_ALL_ELEMENTS);
  int NumberOfComponents = field->getNumberOfComponents();

  cout << "========================== field: " << field->getName() << endl;
  cout << "NumberOfElements   = " << NumberOfElements << endl;
  cout << "NumberOfComponents = " << NumberOfComponents << endl;

  for (int i = 1; i < NumberOfElements + 1; i++) {
    const double * value = field->getRow(i);
    for (int j = 0; j < NumberOfComponents; j++)
      cout << value[j] << " ";
    cout << " | ";
  }
  cout << endl;
}
