// Copyright (C) 2007-2013  CEA/DEN, EDF R&D
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
// Author : Anthony Geay (CEA/DEN)

#include "MEDCouplingMeshClient.hxx"
#include "MEDCouplingUMeshClient.hxx"
#include "MEDCouplingExtrudedMeshClient.hxx"
#include "MEDCouplingExtrudedMesh.hxx"
#include "MEDCouplingCMeshClient.hxx"
#include "MEDCouplingCMesh.hxx"
#include "MEDCouplingUMesh.hxx"

#include <vector>

using namespace ParaMEDMEM;

MEDCouplingMesh *MEDCouplingMeshClient::New(SALOME_MED::MEDCouplingMeshCorbaInterface_ptr meshPtr)
{
  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr umeshPtr=SALOME_MED::MEDCouplingUMeshCorbaInterface::_narrow(meshPtr);
  if(!CORBA::is_nil(umeshPtr))
    {
      MEDCouplingMesh *ret=MEDCouplingUMeshClient::New(umeshPtr);
      CORBA::release(umeshPtr);
      return ret;
    }
  SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface_ptr emeshPtr=SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface::_narrow(meshPtr);
  if(!CORBA::is_nil(emeshPtr))
    {
      MEDCouplingMesh *ret=MEDCouplingExtrudedMeshClient::New(emeshPtr);
      CORBA::release(emeshPtr);
      return ret;
    }
  SALOME_MED::MEDCouplingCMeshCorbaInterface_ptr cmeshPtr=SALOME_MED::MEDCouplingCMeshCorbaInterface::_narrow(meshPtr);
  if(!CORBA::is_nil(cmeshPtr))
    {
      MEDCouplingMesh *ret=MEDCouplingCMeshClient::New(cmeshPtr);
      CORBA::release(cmeshPtr);
      return ret;
    }
  return 0;
}

void MEDCouplingMeshClient::fillMeshFromCorbaData(MEDCouplingMesh *meshCpp, SALOME_MED::MEDCouplingMeshCorbaInterface_ptr meshPtr)
{
  meshPtr->Register();
  //1st call to getTinyInfo to get tiny array of key integers value
  //to corectly resize local copy of distant instance adressed by 'meshPtr'
  //1st value of returned array is the type of instance. Thanks to
  //CORBA and its type-check no use of this value is necessary.
  SALOME_TYPES::ListOfDouble *tinyD;
  SALOME_TYPES::ListOfLong *tinyI;
  SALOME_TYPES::ListOfString *tinyS;
  meshPtr->getTinyInfo(tinyD,tinyI,tinyS);
  int tinyLgth=tinyI->length();
  std::vector<int> tinyV(tinyLgth);
  for(int i=0;i<tinyLgth;i++)
    tinyV[i]=(*tinyI)[i];
  int tinyLgth2=tinyD->length();
  std::vector<double> tinyV2(tinyLgth2);
  for(int i=0;i<tinyLgth2;i++)
    tinyV2[i]=(*tinyD)[i];
  std::vector<std::string> sts(tinyS->length());
  for(int i=0;i<(int)sts.size();i++)
    sts[i]=(*tinyS)[i];
  delete tinyS;
  delete tinyI;
  delete tinyD;
  DataArrayInt* a1=DataArrayInt::New();
  DataArrayDouble* a2=DataArrayDouble::New();
  //thanks to the entry point tinyV get from the 1st CORBA invokation,
  //resizing a1,a2 and sts.
  std::vector<std::string> uselessVector;
  //vector 'uselessVector' is useless thanks to CORBA that , contrary to MPI, does not need to allocate right length of arrays before invokation
  meshCpp->resizeForUnserialization(tinyV,a1,a2,uselessVector);
  SALOME_TYPES::ListOfLong *a1Corba;
  SALOME_TYPES::ListOfDouble *a2Corba;
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
  meshCpp->unserialization(tinyV2,tinyV,a1,a2,sts);
  a1->decrRef();
  a2->decrRef();
  //
  meshPtr->UnRegister();
}
