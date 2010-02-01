//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D
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
#include "MEDCouplingUMeshClient.hxx"
#include "MEDCouplingUMesh.hxx"

#include <vector>

using namespace ParaMEDMEM;

MEDCouplingUMesh *MEDCouplingUMeshClient::New(SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr meshPtr)
{
  meshPtr->Register();
  MEDCouplingUMesh *ret=MEDCouplingUMesh::New();
  //1st call to getTinyInfo to get tiny array of key integers value
  //to corectly resize local copy of distant instance adressed by 'meshPtr'
  //1st value of returned array is the type of instance. Thanks to
  //CORBA and its type-check no use of this value is necessary.
  SALOME_MED::long_array *tinyI;
  SALOME_MED::string_array *tinyS;
  meshPtr->getTinyInfo(tinyI,tinyS);
  int tinyLgth=tinyI->length();
  std::vector<int> tinyV(tinyLgth);
  for(int i=0;i<tinyLgth;i++)
    tinyV[i]=(*tinyI)[i];
  std::vector<std::string> sts(tinyS->length());
  for(int i=0;i<sts.size();i++)
    sts[i]=(*tinyS)[i];
  delete tinyS;
  delete tinyI;
  DataArrayInt* a1=DataArrayInt::New();
  DataArrayDouble* a2=DataArrayDouble::New();
  //thanks to the entry point tinyV get from the 1st CORBA invokation,
  //resizing a1,a2 and sts.
  std::vector<std::string> uselessVector;
  //vector 'uselessVector' is useless thanks to CORBA that , contrary to MPI, does not need to allocate right length of arrays before invokation
  ret->resizeForUnserialization(tinyV,a1,a2,uselessVector);
  SALOME_MED::long_array *a1Corba;
  SALOME_MED::double_array *a2Corba;
  meshPtr->getSerialisationData(a1Corba,a2Corba);
  int myLgth=a1Corba->length();
  int *ptToFill=a1->getPointer();
  for(int i=0;i<myLgth;i++)
    ptToFill[i]=(*a1Corba)[i];
  delete a1Corba;
  myLgth=a2Corba->length();
  double *ptToFill2=a2->getPointer();
  for(int i=0;i<myLgth;i++)
    ptToFill2[i]=(*a2Corba)[i];
  delete a2Corba;
  //
  ret->unserialization(tinyV,a1,a2,sts);
  a1->decrRef();
  a2->decrRef();
  //
  meshPtr->Destroy();
  CORBA::release(meshPtr);
  return ret;
}
