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

#include "MEDCouplingRefCountServant.hxx"
#include "InterpKernelException.hxx"
#include "MEDCouplingRefCountObject.hxx"
#include "MEDCouplingTimeLabel.hxx"

#include <iostream>

using namespace ParaMEDMEM;

MEDCouplingRefCountServant::MEDCouplingRefCountServant(const RefCountObject *pointer, const TimeLabel *pointer2):_ref_counter(1),
                                                                                                                 _cpp_pointer(pointer),
                                                                                                                 _tl_pointer(pointer2)
{
  if(_cpp_pointer)
    _cpp_pointer->incrRef();
  else
    throw INTERP_KERNEL::Exception("Null pointer not valid !");
}

MEDCouplingRefCountServant::~MEDCouplingRefCountServant()
{
}


CORBA::Long MEDCouplingRefCountServant::getTimeLabel()
{
  if(_tl_pointer)
    {
      _tl_pointer->updateTime();
      return _tl_pointer->getTimeOfThis();
    }
  return -1;
}

SALOME::StringSeq *MEDCouplingRefCountServant::GetExportableFormats()
{
  SALOME::StringSeq *ret=new SALOME::StringSeq;
  ret->length(1);
  (*ret)[0]=CORBA::string_dup("MEDCorba");
  return ret;
}

void MEDCouplingRefCountServant::Register()
{
  if(_cpp_pointer)
    _cpp_pointer->incrRef();
  _ref_counter++;
}

void MEDCouplingRefCountServant::UnRegister()
{
  if(_cpp_pointer)
    ((RefCountObject *)_cpp_pointer)->decrRef();
  _ref_counter--;
  if(_ref_counter<=0)
    {
      if(_ref_counter<0)
        std::cerr << "Counter < 0 detected .... ???? " << std::endl;
      PortableServer::ObjectId_var oid = _default_POA()->servant_to_id(this);
      _default_POA()->deactivate_object(oid);
      _remove_ref();
    }
}

void MEDCouplingRefCountServant::Destroy()
{
  std::cerr << "WARNING SALOME::GenericObj::Destroy() function is obsolete! Use UnRegister() instead." << std::endl;
  UnRegister();
}
    