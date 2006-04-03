#include <string>
#include <deque>
#include <iostream>
#include "MEDMEM_MedMedDriver.hxx"
#include "MEDMEM_Med.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_DriverFactory.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

int main(int argc, char *argv[])
{
  if (argc !=2)
    {
      cerr << "Usage : " << argv[0] 
	   << " filename  where filename should be a med file" << endl << endl;
      exit(-1);
    }

  int i;
  //string fileIn="/export/home/geay32/SALOME225/KERNEL_SRC/examples/TimeStamps.med";

  string fileIn = argv[1] ;
  const string ext=".med";
  string::size_type pos=fileIn.find(ext,0);
  string baseName (fileIn, 0, pos); // nom sans extension
  string fileOut21=baseName + "Anal_V21.med";
  string fileOut22=baseName + "Anal_V22.med";

  MED *myMed=new MED;
  MED_MED_RDONLY_DRIVER *driverIn=new MED_MED_RDONLY_DRIVER(fileIn,myMed);
  driverIn->open();
  driverIn->readFileStruct();
  driverIn->close();
  int nbOfMeshes=myMed->getNumberOfMeshes();
  int nbOfFields=myMed->getNumberOfFields();
  cout << nbOfMeshes << " --- " << nbOfFields << endl;
  deque<string> names=myMed->getMeshNames();
  for(i=0;i<nbOfMeshes;i++)
    cout << names[i] << endl;
  cout << "************* " << endl;
  deque<string> names2=myMed->getFieldNames();
  for(i=0;i<nbOfFields;i++)
    cout << names2[i] << endl;
  MESH *mesh=myMed->getMesh(names[0]);
  mesh->read();
  myMed->updateSupport();
  deque<DT_IT_> iterations=myMed->getFieldIteration(names2[2]);
  cout << iterations.size() << endl;
  FIELD_ *f1=myMed->getField(names2[2],iterations[0].dt,iterations[0].it);
  FIELD_ *f2=myMed->getField(names2[2],iterations[1].dt,iterations[1].it);
  FIELD<double> *f1s=(FIELD<double> *)f1;
  f1s->read();
  FIELD<double> *f2s=(FIELD<double> *)f2;
  f2s->read();
  DRIVERFACTORY::setMedFileVersionForWriting(V21);
  int id=mesh->addDriver(MED_DRIVER,fileOut21,mesh->getName());
  mesh->write(id);
  id=f1s->addDriver(MED_DRIVER, fileOut21, f1s->getName());
  f1s->write(id);
  id=f2s->addDriver(MED_DRIVER, fileOut21, f2s->getName());
  f2s->write(id);

  DRIVERFACTORY::setMedFileVersionForWriting(V22);
  id=mesh->addDriver(MED_DRIVER,fileOut22,mesh->getName());
  mesh->write(id);
  id=f1s->addDriver(MED_DRIVER, fileOut22, f1s->getName());
  f1s->write(id);
  id=f2s->addDriver(MED_DRIVER, fileOut22, f2s->getName());
  f2s->write(id);

  delete myMed;
  return 0;
}

