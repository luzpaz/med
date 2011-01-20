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

#include "MEDCouplingFieldTemplate.hxx"
#include "MEDCouplingMesh.hxx"
#include "MEDCouplingFieldDouble.hxx"
#include "MEDCouplingFieldDiscretization.hxx"

using namespace ParaMEDMEM;

MEDCouplingFieldTemplate *MEDCouplingFieldTemplate::New(const MEDCouplingFieldDouble *f) throw(INTERP_KERNEL::Exception)
{
  return new MEDCouplingFieldTemplate(f);
}

/*!
 * The user should \b not use this method. Only useful for CORBA serialization/unserialization.
 */
MEDCouplingFieldTemplate *MEDCouplingFieldTemplate::New(TypeOfField type)
{
  return new MEDCouplingFieldTemplate(type);
}

MEDCouplingFieldTemplate::MEDCouplingFieldTemplate(const MEDCouplingFieldDouble *f) throw(INTERP_KERNEL::Exception):MEDCouplingField(*f) 
{
  checkCoherency();
}

MEDCouplingFieldTemplate::MEDCouplingFieldTemplate(TypeOfField type):MEDCouplingField(type)
{
}

void MEDCouplingFieldTemplate::checkCoherency() const throw(INTERP_KERNEL::Exception)
{
  if(_mesh==0)
    throw INTERP_KERNEL::Exception("MEDCouplingFieldTemplate::checkCoherency : Empty mesh !");
}

std::string MEDCouplingFieldTemplate::simpleRepr() const
{
  std::ostringstream ret;
  ret << "FieldTemplate with name : \"" << getName() << "\"\n";
  ret << "Description of field is : \"" << getDescription() << "\"\n";
  ret << "FieldTemplate space discretization is : " << _type->getStringRepr() << "\n";
  ret << "FieldTemplate nature of field is : " << MEDCouplingNatureOfField::getRepr(_nature) << "\n";
  if(_mesh)
    ret << "Mesh support information :\n__________________________\n" << _mesh->simpleRepr();
  else
    ret << "Mesh support information : No mesh set !\n";
  return ret.str();
}

std::string MEDCouplingFieldTemplate::advancedRepr() const
{
  return simpleRepr();
}

void MEDCouplingFieldTemplate::getTinySerializationIntInformation(std::vector<int>& tinyInfo) const
{
  tinyInfo.clear();
  tinyInfo.push_back((int)_type->getEnum());
  tinyInfo.push_back((int)_nature);
  std::vector<int> tinyInfo2;
  _type->getTinySerializationIntInformation(tinyInfo2);
  tinyInfo.insert(tinyInfo.end(),tinyInfo2.begin(),tinyInfo2.end());
  tinyInfo.push_back(tinyInfo2.size());
}

void MEDCouplingFieldTemplate::getTinySerializationDbleInformation(std::vector<double>& tinyInfo) const
{
  tinyInfo.clear();
  _type->getTinySerializationDbleInformation(tinyInfo);
}

void MEDCouplingFieldTemplate::getTinySerializationStrInformation(std::vector<std::string>& tinyInfo) const
{
  tinyInfo.clear();
  tinyInfo.push_back(_name);
  tinyInfo.push_back(_desc);
}

void MEDCouplingFieldTemplate::resizeForUnserialization(const std::vector<int>& tinyInfoI, DataArrayInt *&dataInt)
{
  dataInt=0;
  std::vector<int> tinyInfoITmp(tinyInfoI.begin()+2,tinyInfoI.end());
  _type->resizeForUnserialization(tinyInfoITmp,dataInt);
}

void MEDCouplingFieldTemplate::finishUnserialization(const std::vector<int>& tinyInfoI, const std::vector<double>& tinyInfoD, const std::vector<std::string>& tinyInfoS)
{
  _nature=(NatureOfField)tinyInfoI[1];
  _type->finishUnserialization(tinyInfoD);
  _name=tinyInfoS[0];
  _desc=tinyInfoS[1];
}

void MEDCouplingFieldTemplate::serialize(DataArrayInt *&dataInt) const
{
  _type->getSerializationIntArray(dataInt);
}
