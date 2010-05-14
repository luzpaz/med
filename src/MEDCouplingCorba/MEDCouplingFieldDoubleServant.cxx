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

#include "MEDCouplingFieldDoubleServant.hxx"
#include "MEDCouplingFieldDouble.hxx"

using namespace ParaMEDMEM;

MEDCouplingFieldDoubleServant::MEDCouplingFieldDoubleServant(const MEDCouplingFieldDouble *cppPointerOfMesh):MEDCouplingFieldServant(cppPointerOfMesh)
{
}

void MEDCouplingFieldDoubleServant::getTinyInfo(SALOME_MED::long_array_out la, SALOME_MED::double_array_out da, SALOME_MED::string_array_out sa)
{
  la=new SALOME_MED::long_array;
  std::vector<int> tinyInfo;
  getPointer()->getTinySerializationIntInformation(tinyInfo);
  la->length(tinyInfo.size());
  for(int i=0;i<tinyInfo.size();i++)
    (*la)[i]=tinyInfo[i];
  //
  da=new SALOME_MED::double_array;
  std::vector<double> tinyInfo2;
  getPointer()->getTinySerializationDbleInformation(tinyInfo2);
  da->length(tinyInfo2.size());
  for(int i=0;i<tinyInfo2.size();i++)
    (*da)[i]=tinyInfo2[i];
  //
  sa=new SALOME_MED::string_array;
  std::vector<std::string> tinyInfo3;
  getPointer()->getTinySerializationStrInformation(tinyInfo3);
  sa->length(tinyInfo3.size());
  for(int i=0;i<tinyInfo3.size();i++)
    (*sa)[i]=CORBA::string_dup(tinyInfo3[i].c_str());
}

void MEDCouplingFieldDoubleServant::getSerialisationData(SALOME_MED::double_array2_out da2)
{
  std::vector<DataArrayDouble *> arrays;
  getPointer()->serialize(arrays);
  da2=new SALOME_MED::double_array2;
  int lgth=arrays.size();
  da2->length(lgth);
  for(int i=0;i<lgth;i++)
    {
      SALOME_MED::double_array& locCorbaArr=(*da2)[i];
      DataArrayDouble *locArr=arrays[i];
      int lgth2=locArr->getNbOfElems();
      locCorbaArr.length(lgth2);
      const double *pt=locArr->getConstPointer();
      for(int j=0;j<lgth2;j++)
        locCorbaArr[j]=pt[j];
    }
}
