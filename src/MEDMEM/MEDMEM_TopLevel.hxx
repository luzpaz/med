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
