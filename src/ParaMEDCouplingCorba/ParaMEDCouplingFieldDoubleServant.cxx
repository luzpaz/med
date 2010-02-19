//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
#include "ParaMEDCouplingFieldDoubleServant.hxx"
#include "utilities.h"
using namespace ParaMEDMEM;

ParaMEDCouplingFieldDoubleServant::ParaMEDCouplingFieldDoubleServant(CORBA::ORB_ptr orb,MEDCouplingFieldDouble* field):MEDCouplingFieldDoubleServant(field),MPIObject_i()
{
  Engines::MPIObject_var pobj = POA_SALOME_MED::ParaMEDCouplingFieldDoubleCorbaInterface::_this();
  BCastIOR(orb,pobj,false);
}

void ParaMEDCouplingFieldDoubleServant::Register()
{
  if(_numproc == 0)
    for(int ip=1;ip<_nbproc;ip++)
      {
        SALOME_MED::ParaMEDCouplingFieldDoubleCorbaInterface_var fieldPtr=SALOME_MED::ParaMEDCouplingFieldDoubleCorbaInterface::_narrow((*_tior)[ip]);
        fieldPtr->Register();
      }
  MEDCouplingFieldDoubleServant::Register();
}

void ParaMEDCouplingFieldDoubleServant::Destroy()
{
  if(_numproc == 0)
    for(int ip=1;ip<_nbproc;ip++)
      {
        SALOME_MED::ParaMEDCouplingFieldDoubleCorbaInterface_var fieldPtr=SALOME_MED::ParaMEDCouplingFieldDoubleCorbaInterface::_narrow((*_tior)[ip]);
        fieldPtr->Destroy();
      }
  MEDCouplingFieldDoubleServant::Destroy();
}
