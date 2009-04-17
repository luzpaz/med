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
#include "MEDCouplingFieldServant.hxx"
#include "MEDCouplingUMeshServant.hxx"
#include "MEDCouplingField.hxx"
#include "MEDCouplingUMesh.hxx"

using namespace ParaMEDMEM;

MEDCouplingFieldServant::MEDCouplingFieldServant(const MEDCouplingField *cppPointerOfMesh):MEDCouplingRefCountServant(cppPointerOfMesh)
{
}

SALOME_MED::MEDCouplingMeshCorbaInterface_ptr MEDCouplingFieldServant::getMesh()
{
  const MEDCouplingMesh *mesh=getPointer()->getMesh();
  const MEDCouplingUMesh *uMesh=dynamic_cast<const MEDCouplingUMesh *>(mesh);
  if(uMesh)
    {
      MEDCouplingUMeshServant *retServ=new MEDCouplingUMeshServant(uMesh);
      return retServ->_this();
    }
  throw INTERP_KERNEL::Exception("Not dealt mesh type !");
}
