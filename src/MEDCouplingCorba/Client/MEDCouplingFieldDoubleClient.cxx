//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
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

#include "MEDCouplingFieldDoubleClient.hxx"
#include "MEDCouplingMeshClient.hxx"
#include "MEDCouplingMesh.hxx"

#include <vector>

using namespace ParaMEDMEM;

MEDCouplingFieldDouble *MEDCouplingFieldDoubleClient::New(SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr)
{
  fieldPtr->Register();
  //
  SALOME_MED::long_array *tinyL;
  SALOME_MED::double_array *tinyD;
  SALOME_MED::string_array *tinyS;
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
  SALOME_MED::MEDCouplingMeshCorbaInterface::_duplicate(meshPtr);
  MEDCouplingMesh *mesh=MEDCouplingMeshClient::New(meshPtr);
  meshPtr->Destroy();
  ret->setMesh(mesh);
  mesh->decrRef();
  std::vector<DataArrayDouble *> arrays;
  ret->resizeForUnserialization(tinyLV,arrays);
  SALOME_MED::double_array2 *bigArr;
  //3rd CORBA invokation to get big content
  fieldPtr->getSerialisationData(bigArr);
  tinyLgth=arrays.size();
  for(int i=0;i<tinyLgth;i++)
    {
      SALOME_MED::double_array& oneArr=(*bigArr)[i];
      DataArrayDouble *curArrToFill=arrays[i];
      double *pt=curArrToFill->getPointer();
      int lgth=curArrToFill->getNbOfElems();
      for(int j=0;j<lgth;j++)
        pt[j]=oneArr[j];
    }
  delete bigArr;
  //
  //notify server that the servant is no more used.
  fieldPtr->Destroy();
  //decrements the local CORBA pointer.
  CORBA::release(fieldPtr);
  //
  ret->finishUnserialization(tinyLV,tinyLD,tinyLS);
  //
  return ret;
}
