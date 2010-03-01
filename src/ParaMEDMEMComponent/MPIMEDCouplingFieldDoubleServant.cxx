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

MPIMEDCouplingFieldDoubleServant::MPIMEDCouplingFieldDoubleServant(CORBA::ORB_ptr orb,ParaMEDMEMComponent_i *pcompo,MEDCouplingFieldDouble* field):ParaMEDCouplingFieldDoubleServant(orb,field)
{
  _pcompo = pcompo;
  _field = field;
  Engines::MPIObject_var pobj = POA_SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface::_this();
  BCastIOR(orb,pobj,false);
}

void MPIMEDCouplingFieldDoubleServant::getDataByMPI(const char* coupling) throw(SALOME::SALOME_Exception)
{
  bool *exception;
  void *ret_th;
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

  try
    {
      _pcompo->_getOutputField(coupling,_field);
    }
  catch(const std::exception &ex)
    {
      MESSAGE(ex.what());
      THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::INTERNAL_ERROR);
    }
    
  if(_numproc == 0)
    {
      for(int ip=1;ip<_nbproc;ip++)
        {
          pthread_join(th[ip],&ret_th);
          exception = (bool*)ret_th;
          if(*exception)
            {
              // exception
              ostringstream msg;
              msg << "Error on get data by mpi on process " << ip;
              THROW_SALOME_CORBA_EXCEPTION(msg.str().c_str(),SALOME::INTERNAL_ERROR);
            }
          delete exception;
        }
      delete[] th;
    }
}

void *th_getdatabympi(void *s)
{
  thread_st *st = (thread_st*)s;
  bool *exception = new bool;
  *exception = false;
  try
    {
      SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface_var fieldPtr=SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface::_narrow((*(st->tior))[st->ip]);
      fieldPtr->getDataByMPI(st->coupling.c_str());
    }
  catch(...)
    {
      *exception = true;
    }
  delete st;
  return((void*)exception);
}

