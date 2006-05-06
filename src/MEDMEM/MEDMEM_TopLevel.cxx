#include "MEDMEM_TopLevel.hxx"
#include "MEDMEM_Med.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_DriverFactory.hxx"
#include "MEDMEM_MedMedDriver.hxx"

using namespace MED_EN;
using namespace MEDMEM;

MED *::MEDMEM::readMedInFile(const std::string& fileName) throw(MEDEXCEPTION)
{
  MED *myMedObj=new MED;
  driverTypes type=DRIVERFACTORY::deduceDriverTypeFromFileName(fileName);
  int id=myMedObj->addDriver(type,fileName,MED_REMP);
  myMedObj->read(id);
  return myMedObj;
}

MESH *::MEDMEM::readMeshInFile(const std::string& fileName, const std::string& meshName)
{
  MESH *mesh=new MESH;
  driverTypes type=DRIVERFACTORY::deduceDriverTypeFromFileName(fileName);
  int id=mesh->addDriver(type,fileName,mesh->getName(),MED_REMP);
  mesh->read(id);
  return mesh;
}

void ::MEDMEM::writeMedToFile(const MED *medObj, const std::string& fileName)
{ 
  MED *medObjNoC=(MED *)medObj;//MED::write should be a const method ... As it's not the case
  driverTypes type=DRIVERFACTORY::deduceDriverTypeFromFileName(fileName);
  int id=medObjNoC->addDriver(type,fileName,MED_REMP);
  medObjNoC->write(id);
}

void ::MEDMEM::writeMeshToFile(const MESH *meshObj, const std::string& fileName)
{
  MESH *meshObjNoC=(MESH *)meshObj;//MED::write should be a const method ... As it's not the case
  driverTypes type=DRIVERFACTORY::deduceDriverTypeFromFileName(fileName);
  int id=meshObjNoC->addDriver(type,fileName,meshObjNoC->getName(),MED_REMP);
  meshObjNoC->write(id);
}
