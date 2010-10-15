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

#ifndef MEDDATAMANAGER_HXX
#define MEDDATAMANAGER_HXX

#include <string>

#include <MEDMEM.hxx>
#include "MEDMEM_Med.hxx"
#include "MEDMEM_Field.hxx"

namespace MEDMEM {

class MEDMEM_EXPORT MedDataManager
{
public:
  MedDataManager(string filename);
  MedDataManager(MED * med);

  FIELD_ * getField(const string & fieldName, const int dt,  const int it);
  FIELD<double> * getFieldDouble(const string & fieldName, const int dt,  const int it);
  FIELD<double> * getFieldDoubleFromId(int fieldIdx = 0, int iterationIdx = 0);
  void printFieldDouble(FIELD<double> * field);

private:
  MED * _med;
};

}
#endif // MEDDATAMANAGER_HXX
