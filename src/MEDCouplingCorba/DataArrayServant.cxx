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

#include "DataArrayServant.hxx"
#include "MEDCouplingMemArray.hxx"

using namespace ParaMEDMEM;

DataArrayServant::DataArrayServant(const DataArray *cppPointer):MEDCouplingRefCountServant(cppPointer,cppPointer)
{
}

char *DataArrayServant::getName()
{
  return CORBA::string_dup(getPointer()->getName().c_str());
}

SALOME_TYPES::ListOfString *DataArrayServant::getInfoOnComponents()
{
  SALOME_TYPES::ListOfString *ret=new SALOME_TYPES::ListOfString;
  const std::vector<std::string> &comps=getPointer()->getInfoOnComponents();
  ret->length(comps.size());
  for(std::size_t i=0;i<comps.size();i++)
    (*ret)[i]=CORBA::string_dup(comps[i].c_str());
  return ret;
}
