// Copyright (C) 2007-2014  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

#include "MEDCouplingFieldServant.hxx"
#include "MEDCouplingMeshServant.hxx"
#include "MEDCouplingUMeshServant.hxx"
#include "MEDCouplingCMeshServant.hxx"
#include "MEDCouplingExtrudedMeshServant.hxx"
#include "MEDCouplingField.hxx"
#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingCMesh.hxx"
#include "MEDCouplingExtrudedMesh.hxx"

using namespace ParaMEDMEM;

MEDCouplingFieldServant::MEDCouplingFieldServant(const MEDCouplingField *cppPointerOfMesh):MEDCouplingRefCountServant(cppPointerOfMesh,cppPointerOfMesh)
{
}

SALOME_MED::MEDCouplingMeshCorbaInterface_ptr MEDCouplingFieldServant::BuildCorbaRefFromCppPointer(const MEDCouplingMesh *mesh)
{
  const MEDCouplingUMesh *uMesh=dynamic_cast<const MEDCouplingUMesh *>(mesh);
  if(uMesh)
    {
      MEDCouplingUMeshServant *retServ=new MEDCouplingUMeshServant(uMesh);
      SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr ret=retServ->_this();//let this line even if it seems fool
      return ret;
    }
  const MEDCouplingCMesh *cMesh=dynamic_cast<const MEDCouplingCMesh *>(mesh);
  if(cMesh)
    {
      MEDCouplingCMeshServant *retServ=new MEDCouplingCMeshServant(cMesh);
      SALOME_MED::MEDCouplingCMeshCorbaInterface_ptr ret=retServ->_this();//let this line even if it seems fool
      return ret;
    }
  const MEDCouplingExtrudedMesh *eMesh=dynamic_cast<const MEDCouplingExtrudedMesh *>(mesh);
  if(eMesh)
    {
      MEDCouplingExtrudedMeshServant *retServ=new MEDCouplingExtrudedMeshServant(eMesh);
      return retServ->_this();
    }
  throw INTERP_KERNEL::Exception("Not dealt mesh type !");
}

SALOME_MED::MEDCouplingMeshCorbaInterface_ptr MEDCouplingFieldServant::getMesh()
{
  const MEDCouplingMesh *mesh=getPointer()->getMesh();
  return BuildCorbaRefFromCppPointer(mesh);
}
