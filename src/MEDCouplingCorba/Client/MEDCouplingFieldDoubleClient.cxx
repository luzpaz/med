// Copyright (C) 2007-2011  CEA/DEN, EDF R&D
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

#include "MEDCouplingFieldDoubleClient.hxx"
#include "MEDCouplingMeshClient.hxx"
#include "MEDCouplingMesh.hxx"

#include <vector>

using namespace ParaMEDMEM;

MEDCouplingFieldDouble *MEDCouplingFieldDoubleClient::New(SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr)
{
  fieldPtr->Register();
  //
  SALOME_TYPES::ListOfLong *tinyL;
  SALOME_TYPES::ListOfDouble *tinyD;
  SALOME_TYPES::ListOfString *tinyS;
  //1st CORBA call : getting all tiny info of all types (int, double string).
  fieldPtr->getTinyInfo(tinyL,tinyD,tinyS);
  int tinyLgth=tinyL->length();
  std::vector<int> tinyLV(tinyLgth);
  for(int i=0;i<tinyLgth;i++)
    tinyLV[i]=(*tinyL)[i];
  delete tinyL;
  //
  tinyLgth=tinyD->length();
  std::vector<double> tinyLD(tinyLgth);
  for(int i=0;i<tinyLgth;i++)
    tinyLD[i]=(*tinyD)[i];
  delete tinyD;
  //
  tinyLgth=tinyS->length();
  std::vector<std::string> tinyLS(tinyLgth);
  for(int i=0;i<tinyLgth;i++)
    tinyLS[i]=(*tinyS)[i];
  delete tinyS;
  //
  TypeOfField type=(TypeOfField) tinyLV[0];
  TypeOfTimeDiscretization td=(TypeOfTimeDiscretization) tinyLV[1];
  MEDCouplingFieldDouble *ret=MEDCouplingFieldDouble::New(type,td);
  //2nd CORBA call to retrieves the mesh.
  SALOME_MED::MEDCouplingMeshCorbaInterface_ptr meshPtr=fieldPtr->getMesh();
  MEDCouplingMesh *mesh=MEDCouplingMeshClient::New(meshPtr);
  meshPtr->UnRegister();
  CORBA::release(meshPtr);
  ret->setMesh(mesh);
  mesh->decrRef();
  DataArrayInt *array0;
  std::vector<DataArrayDouble *> arrays;
  ret->resizeForUnserialization(tinyLV,array0,arrays);
  SALOME_TYPES::ListOfLong *bigArr0;
  SALOME_TYPES::ListOfDouble2 *bigArr;
  //3rd CORBA invokation to get big content
  fieldPtr->getSerialisationData(bigArr0,bigArr);
  if(bigArr0->length()!=0)
    {
      int *pt=array0->getPointer();
      int lgth=array0->getNbOfElems();
      for(int i=0;i<lgth;i++)
        pt[i]=(*bigArr0)[i];
    }
  delete bigArr0;
  tinyLgth=arrays.size();
  for(int i=0;i<tinyLgth;i++)
    {
      SALOME_TYPES::ListOfDouble& oneArr=(*bigArr)[i];
      DataArrayDouble *curArrToFill=arrays[i];
      double *pt=curArrToFill->getPointer();
      int lgth=curArrToFill->getNbOfElems();
      for(int j=0;j<lgth;j++)
        pt[j]=oneArr[j];
    }
  delete bigArr;
  //
  //notify server that the servant is no more used.
  fieldPtr->UnRegister();
  //
  ret->finishUnserialization(tinyLV,tinyLD,tinyLS);
  //
  return ret;
}
