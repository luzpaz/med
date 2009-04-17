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
#include "MEDCouplingRefCountServant.hxx"
#include "InterpKernelException.hxx"
#include "MEDCouplingRefCountObject.hxx"

#include <iostream>

using namespace ParaMEDMEM;

MEDCouplingRefCountServant::MEDCouplingRefCountServant(const RefCountObject *pointer):_cpp_pointer(pointer),_ref_counter(1)
{
  if(_cpp_pointer)
    _cpp_pointer->incrRef();
  else
    throw INTERP_KERNEL::Exception("Null pointer not valid !");
}

MEDCouplingRefCountServant::~MEDCouplingRefCountServant()
{
}

void MEDCouplingRefCountServant::Register()
{
  if(_cpp_pointer)
    _cpp_pointer->incrRef();
  _ref_counter++;
}

void MEDCouplingRefCountServant::Destroy()
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
