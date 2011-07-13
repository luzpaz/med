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

#include "MEDCouplingMultiFieldsServant.hxx"
#include "DataArrayDoubleServant.hxx"
#include "MEDCouplingFieldServant.hxx"
#include "MEDCouplingMultiFields.hxx"
#include "MEDCouplingFieldTemplate.hxx"
#include "MEDCouplingMemArray.hxx"

using namespace ParaMEDMEM;

MEDCouplingMultiFieldsServant::MEDCouplingMultiFieldsServant(const MEDCouplingMultiFields *cppPointer):MEDCouplingRefCountServant(cppPointer,cppPointer)
{
}

SALOME_MED::MEDCouplingMeshesCorbaInterface *MEDCouplingMultiFieldsServant::getMeshes()
{
  SALOME_MED::MEDCouplingMeshesCorbaInterface *ret=new SALOME_MED::MEDCouplingMeshesCorbaInterface;
  std::vector<int> tmp;
  std::vector<MEDCouplingMesh *> cppMeshes=getPointer()->getDifferentMeshes(tmp);
  int sz=cppMeshes.size();
  ret->length(sz);
  for(int i=0;i<sz;i++)
    (*ret)[i]=MEDCouplingFieldServant::BuildCorbaRefFromCppPointer(cppMeshes[i]);
  return ret;
}

SALOME_MED::MEDCouplingMeshCorbaInterface_ptr MEDCouplingMultiFieldsServant::getMeshWithId(CORBA::Long id)
{
  std::vector<int> tmp;
  std::vector<MEDCouplingMesh *> cppMeshes=getPointer()->getDifferentMeshes(tmp);
  return MEDCouplingFieldServant::BuildCorbaRefFromCppPointer(cppMeshes[id]);
}

CORBA::Long MEDCouplingMultiFieldsServant::getMainTinyInfo(SALOME_TYPES::ListOfLong_out la, SALOME_TYPES::ListOfDouble_out da, CORBA::Long& nbOfArrays, CORBA::Long& nbOfFields)
{
  std::vector<int> tinyInfo;
  std::vector<double> tinyInfo2;
  int nbOfDiffMeshes, nbOfDiffArr;
  nbOfFields=getPointer()->getNumberOfFields();
  getPointer()->getTinySerializationInformation(tinyInfo,tinyInfo2,nbOfDiffMeshes,nbOfDiffArr);
  nbOfArrays=nbOfDiffArr;
  la=new SALOME_TYPES::ListOfLong;
  la->length(tinyInfo.size());
  for(int i=0;i<(int)tinyInfo.size();i++)
    (*la)[i]=tinyInfo[i];
  //
  da=new SALOME_TYPES::ListOfDouble;
  da->length(tinyInfo2.size());
  for(int i=0;i<(int)tinyInfo2.size();i++)
    (*da)[i]=tinyInfo2[i];
  return nbOfDiffMeshes;
}

void MEDCouplingMultiFieldsServant::getTinyInfo(CORBA::Long id, SALOME_TYPES::ListOfLong_out la, SALOME_TYPES::ListOfDouble_out da, SALOME_TYPES::ListOfString_out sa)
{
  const MEDCouplingFieldDouble *f=getPointer()->getFieldWithId(id);
  MEDCouplingFieldTemplate *f2=MEDCouplingFieldTemplate::New(f);
  la=new SALOME_TYPES::ListOfLong;
  std::vector<int> tinyInfo;
  f2->getTinySerializationIntInformation(tinyInfo);
  la->length(tinyInfo.size());
  for(int i=0;i<(int)tinyInfo.size();i++)
    (*la)[i]=tinyInfo[i];
  //
  da=new SALOME_TYPES::ListOfDouble;
  std::vector<double> tinyInfo2;
  f2->getTinySerializationDbleInformation(tinyInfo2);
  da->length(tinyInfo2.size());
  for(int i=0;i<(int)tinyInfo2.size();i++)
    (*da)[i]=tinyInfo2[i];
  //
  sa=new SALOME_TYPES::ListOfString;
  std::vector<std::string> tinyInfo3;
  f2->getTinySerializationStrInformation(tinyInfo3);
  sa->length(tinyInfo3.size());
  for(int i=0;i<(int)tinyInfo3.size();i++)
    (*sa)[i]=CORBA::string_dup(tinyInfo3[i].c_str());
  f2->decrRef();
}

void MEDCouplingMultiFieldsServant::getSerialisationData(CORBA::Long id, SALOME_TYPES::ListOfLong_out la)
{
  const MEDCouplingFieldDouble *f=getPointer()->getFieldWithId(id);
  MEDCouplingFieldTemplate *f2=MEDCouplingFieldTemplate::New(f);
  DataArrayInt *dataInt;
  f2->serialize(dataInt);
  //
  la=new SALOME_TYPES::ListOfLong;
  if(dataInt)
    {
      int lgth=dataInt->getNbOfElems();
      const int *ptr=dataInt->getConstPointer();
      la->length(lgth);
      for(int i=0;i<lgth;i++)
        (*la)[i]=ptr[i];
    }
  else
    la->length(0);
  f2->decrRef();
}

SALOME_MED::DataArrayDoubleCorbaInterface_ptr MEDCouplingMultiFieldsServant::getArray(CORBA::Long id)
{
  std::vector< std::vector<int> > refs;
  std::vector<DataArrayDouble *> das=getPointer()->getDifferentArrays(refs);
  DataArrayDoubleServant *retServ=new DataArrayDoubleServant(das[id]);
  return retServ->_this();
}

CORBA::Boolean MEDCouplingMultiFieldsServant::ExportDataAs(const char *format, SALOME::GenericObj_out exporter)
{
  std::string frmCpp(format);
  if(frmCpp=="MEDCorba")
    {
      SALOME_MED::MEDCouplingMultiFieldsCorbaInterface_ptr cPtr=_this();
      Register();
      exporter=cPtr;
      return true;
    }
  exporter=SALOME::GenericObj::_nil();
  return false;
}
