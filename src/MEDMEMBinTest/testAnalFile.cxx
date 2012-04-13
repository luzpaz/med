// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
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

#include <string>
#include <deque>
#include <iostream>
#include "MEDMEM_MedFileBrowser.hxx"
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
           << " fileName.med where fileName.med should be a med file" << endl;
      cerr << "        It will then generate med file fileNameAnal.med" << endl << endl;
      exit(-1);
    }

  int i;
  // This test program is more or less designed to work with file TimeStamps.med
  // If you want to use it for other Med files, you have to alter the mounting in memory of the fields f1 and f2
  //string fileIn="/export/home/geay32/SALOME225/KERNEL_SRC/examples/TimeStamps.med";

  string fileIn = argv[1] ;
  const string ext=".med";
  string::size_type pos=fileIn.find(ext,0);
  string baseName (fileIn, 0, pos); // nom sans extension
  string fileOut=baseName + "Anal_V22.med";

  MEDFILEBROWSER *myMed=new MEDFILEBROWSER(fileIn);
  int nbOfMeshes=myMed->getNumberOfMeshes();
  int nbOfFields=myMed->getNumberOfFields();
  cout << nbOfMeshes << " --- " << nbOfFields << endl;
  vector<string> names=myMed->getMeshNames();
  for(i=0;i<nbOfMeshes;i++)
    cout << names[i] << endl;
  cout << "************* " << endl;
  vector<string> names2=myMed->getFieldNames();
  for(i=0;i<nbOfFields;i++)
    cout << names2[i] << endl;
  MESH *mesh=new MESH(MED_DRIVER,fileIn,names[0]);
  vector<DT_IT_> iterations=myMed->getFieldIteration(names2[2]);
  cout << iterations.size() << endl;
  FIELD<double> *f1s=new FIELD<double>(MED_DRIVER,fileIn,names2[2],
                                       iterations[0].dt,iterations[0].it,mesh);
  FIELD<double> *f2s=new FIELD<double>(MED_DRIVER,fileIn,names2[2],
                                       iterations[1].dt,iterations[1].it,mesh);
  int id=mesh->addDriver(MED_DRIVER,fileOut,mesh->getName());
  mesh->write(id);
  id=f1s->addDriver(MED_DRIVER, fileOut, f1s->getName());
  f1s->write(id);
  id=f2s->addDriver(MED_DRIVER, fileOut, f2s->getName());
  f2s->write(id);

  f1s->removeReference();
  f2s->removeReference();
  mesh->removeReference();
  delete myMed;
  return 0;
}

