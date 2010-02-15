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
#include "MPIMEDCouplingFieldDoubleServant.hxx"
#include "utilities.h"
using namespace std;
using namespace ParaMEDMEM;

MPIMEDCouplingFieldDoubleServant::MPIMEDCouplingFieldDoubleServant(CORBA::ORB_ptr orb,ParaMEDMEMComponent_i *pcompo,MEDCouplingFieldDouble* field):MEDCouplingFieldDoubleServant(field),MPIObject_i()
{
  _pcompo = pcompo;
  _field = field;
  Engines::MPIObject_var pobj = POA_SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface::_this();
  BCastIOR(orb,pobj,false);
}

void MPIMEDCouplingFieldDoubleServant::getDataByMPI(const char* coupling)
{
  pthread_t *th;
  if(_numproc == 0)
    {
      th = new pthread_t[_nbproc];
      for(int ip=1;ip<_nbproc;ip++)
        {
          thread_st *st = new thread_st;
          st->ip = ip;
          st->tior = _tior;
          st->coupling = coupling;
          pthread_create(&(th[ip]),NULL,th_getdatabympi,(void*)st);
        }
    }

  _pcompo->_getOutputField(coupling,_field);
    
  if(_numproc == 0)
    {
      for(int ip=1;ip<_nbproc;ip++)
        pthread_join(th[ip],NULL);
      delete[] th;
    }
}

void MPIMEDCouplingFieldDoubleServant::Register()
{
  if(_numproc == 0)
    for(int ip=1;ip<_nbproc;ip++)
      {
        SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface_var fieldPtr=SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface::_narrow((*_tior)[ip]);
        fieldPtr->Register();
      }
  MEDCouplingFieldDoubleServant::Register();
}

void MPIMEDCouplingFieldDoubleServant::Destroy()
{
  if(_numproc == 0)
    for(int ip=1;ip<_nbproc;ip++)
      {
        SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface_var fieldPtr=SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface::_narrow((*_tior)[ip]);
        fieldPtr->Destroy();
      }
  MEDCouplingFieldDoubleServant::Destroy();
}

