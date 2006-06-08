// Copyright (C) 2005  OPEN CASCADE, CEA, EDF R&D, LEG
//           PRINCIPIA R&D, EADS CCR, Lip6, BV, CEDRAT
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
#ifndef __MEDMEM_TOPLEVEL_HXX__
#define __MEDMEM_TOPLEVEL_HXX__

#include "MEDMEM_FieldForward.hxx"
#include "MEDMEM_Exception.hxx"
#include <string>

namespace MEDMEM
{
  class MED;
  class MESH;
}

namespace MEDMEM
{
  //read part
  MED *readMedInFile(const std::string& fileName) throw(MEDEXCEPTION);
  MESH *readMeshInFile(const std::string& fileName, const std::string& meshName);
  template<class T>
  FIELD<T> *readFieldInFile(const std::string& fileName, const std::string& fieldName);
  //write part
  void writeMedToFile(const MED *medObj, const std::string& fileName);
  void writeMeshToFile(const MESH *meshObj, const std::string& fileName);
  template<class T>
  void writeFieldToFile(const FIELD<T> *fieldObj, const std::string& fileName);
}

#include "MEDMEM_Field.hxx"

namespace MEDMEM
{
  template<class T>
  FIELD<T> *readFieldInFile(const std::string& fileName, const std::string& fieldName)
  {
    FIELD<T> *ret=new FIELD<T>;
    ret->setName(fieldName);
    driverTypes type=DRIVERFACTORY::deduceDriverTypeFromFileName(fileName);
    int id=ret->addDriver(type,fileName,fieldName);
    ret->read(id);
    return ret;
  }
  
  template<class T>
  void writeFieldToFile(const FIELD<T> *fieldObj, const std::string& fileName)
  {
    FIELD<T> *fieldObjNoC=(FIELD<T> *)fieldObj;
    driverTypes type=DRIVERFACTORY::deduceDriverTypeFromFileName(fileName);
    int id=fieldObj->addDriver(type, fileName, fieldObj->getName());
    fieldObjNoC->write(id);
  }
}

#endif
