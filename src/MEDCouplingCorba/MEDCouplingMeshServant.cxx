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

#include "MEDCouplingMeshServant.hxx"
#include "InterpKernelException.hxx"
#include "MEDCouplingMesh.hxx"
#include "MEDCouplingMemArray.hxx"

using namespace ParaMEDMEM;

MEDCouplingMeshServant::MEDCouplingMeshServant(const MEDCouplingMesh *cppPointerOfMesh):MEDCouplingRefCountServant(cppPointerOfMesh)
{
}

void MEDCouplingMeshServant::getTinyInfo(SALOME_TYPES::ListOfLong_out la, SALOME_TYPES::ListOfString_out sa)
{
  la=new SALOME_TYPES::ListOfLong;
  std::vector<int> tinyInfoI;
  std::vector<std::string> tinyInfoS;
  getPointer()->getTinySerializationInformation(tinyInfoI,tinyInfoS);
  la->length(tinyInfoI.size());
  for(int i=0;i<tinyInfoI.size();i++)
    (*la)[i]=tinyInfoI[i];
  //
  sa=new SALOME_TYPES::ListOfString;
  int nbOfSts=tinyInfoS.size();
  sa->length(nbOfSts);
  for(int i=0;i<nbOfSts;i++)
    (*sa)[i]=CORBA::string_dup(tinyInfoS[i].c_str());
}

void MEDCouplingMeshServant::getSerialisationData(SALOME_TYPES::ListOfLong_out la, SALOME_TYPES::ListOfDouble_out da)
{
  DataArrayInt *array1;
  DataArrayDouble *array2;
  getPointer()->serialize(array1,array2);
  la=new SALOME_TYPES::ListOfLong;
  if(array1)
    {
      int lgth=array1->getNbOfElems();
      const int *data=array1->getConstPointer();
      la->length(lgth);
      for(int i=0;i<lgth;i++)
        (*la)[i]=data[i];
      array1->decrRef();
    }
  //
  da=new SALOME_TYPES::ListOfDouble;
  if(array2)
    {
      int lgth=array2->getNbOfElems();
      const double *data2=array2->getConstPointer();
      da->length(lgth);
      for(int i=0;i<lgth;i++)
        (*da)[i]=data2[i];
      //
      array2->decrRef();
    }
}
