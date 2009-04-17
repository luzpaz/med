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
#include "MEDCouplingMeshClient.hxx"
#include "MEDCouplingUMeshClient.hxx"
#include "MEDCouplingUMesh.hxx"

#include <vector>

using namespace ParaMEDMEM;

MEDCouplingMesh *MEDCouplingMeshClient::New(SALOME_MED::MEDCouplingMeshCorbaInterface_ptr meshPtr)
{
  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr umeshPtr=SALOME_MED::MEDCouplingUMeshCorbaInterface::_narrow(meshPtr);
  if(!CORBA::is_nil(umeshPtr))
    {
      MEDCouplingMesh *ret=MEDCouplingUMeshClient::New(umeshPtr);
      CORBA::release(meshPtr);
      return ret;
    }
  return 0;
}
