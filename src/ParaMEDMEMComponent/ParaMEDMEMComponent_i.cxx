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
#include "ParaMEDMEMComponent_i.hxx"
#include "utilities.h"
using namespace std;

MPIMEDCouplingFieldDoubleServant::MPIMEDCouplingFieldDoubleServant(CORBA::ORB_ptr orb,ParaMEDMEMComponent_i *pcompo,ParaMEDMEM::MEDCouplingFieldDouble* field):ParaMEDMEM::MEDCouplingFieldDoubleServant(field),MPIObject_i()
{
  _pcompo = pcompo;
  _field = field;
  Engines::MPIObject_var pobj = POA_SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface::_this();
  BCastIOR(orb,pobj,false);
}

void MPIMEDCouplingFieldDoubleServant::getDataByMPI(const char* coupling)
{
  pthread_t *th;
  if(_numproc == 0){
    th = new pthread_t[_nbproc];
    for(int ip=1;ip<_nbproc;ip++){
      thread_st *st = new thread_st;
      st->ip = ip;
      st->tior = _tior;
      st->coupling = coupling;
      pthread_create(&(th[ip]),NULL,th_getdatabympi,(void*)st);
    }
  }

  _pcompo->_getOutputField(coupling,_field);
    
  if(_numproc == 0){
    for(int ip=1;ip<_nbproc;ip++)
      pthread_join(th[ip],NULL);
    delete[] th;
  }
}

void MPIMEDCouplingFieldDoubleServant::Register()
{
  if(_numproc == 0)
    for(int ip=1;ip<_nbproc;ip++)
      (SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface::_narrow((*_tior)[ip]))->Register();
  MEDCouplingFieldDoubleServant::Register();
}

void MPIMEDCouplingFieldDoubleServant::Destroy()
{
  if(_numproc == 0)
    for(int ip=1;ip<_nbproc;ip++)
      (SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface::_narrow((*_tior)[ip]))->Destroy();
  MEDCouplingFieldDoubleServant::Destroy();
}

ParaMEDMEMComponent_i::ParaMEDMEMComponent_i() : Engines_Component_i(), MPIObject_i()
{
  _interface = new ParaMEDMEM::CommInterface();
}

ParaMEDMEMComponent_i::ParaMEDMEMComponent_i(int nbproc, int numproc,
                                             CORBA::ORB_ptr orb,
                                             PortableServer::POA_ptr poa, 
                                             PortableServer::ObjectId * contId, 
                                             const char *instanceName,
                                             const char *interfaceName,
                                             bool regist)
  : Engines_Component_i(orb,poa,contId,instanceName,interfaceName,false,regist), MPIObject_i(nbproc,numproc)
{
  _interface = new ParaMEDMEM::CommInterface();
}

ParaMEDMEMComponent_i::~ParaMEDMEMComponent_i()
{
  MESSAGE("* [" << _numproc << "] ParaMEDMEMComponent destructor");
  delete _interface;
}

void ParaMEDMEMComponent_i::_initializeCoupling(const char * coupling)
{
  int gsize, grank;
  set<int> procs;

  string service = coupling;
  if( service.size() == 0 )
    {
      MESSAGE("[" << _numproc << "] You have to give a service name !");
      throw POException(_numproc,"You have to give a service name !");
    }

  if( _gcom.find(service) != _gcom.end() )
    {
      MESSAGE("[" << _numproc << "] service " << service << " already exist !");
      throw POException(_numproc,"service " + service + " already exist !");
    }

  // Connection to distributed parallel component
#ifdef HAVE_MPI2
  remoteMPI2Connect(coupling);
#else
  MESSAGE("[" << _numproc << "] You have to use a MPI2 compliant mpi implementation !");
  throw POException(_numproc,"You have to use a MPI2 compliant mpi implementation !");
#endif

  MPI_Comm_size( _gcom[coupling], &gsize );
  MPI_Comm_rank( _gcom[coupling], &grank );
  MESSAGE("[" << grank << "] new communicator of " << gsize << " processes");

  // Creation of processors group for ParaMEDMEM
  if(_numproc==grank)
    {
      _source[coupling] = new ParaMEDMEM::MPIProcessorGroup(*_interface,0,_nbproc-1,_gcom[coupling]);
      _target[coupling] = new ParaMEDMEM::MPIProcessorGroup(*_interface,_nbproc,gsize-1,_gcom[coupling]);
      _commgroup[coupling] = _source[coupling];
    }
  else
    {
      _source[coupling] = new ParaMEDMEM::MPIProcessorGroup(*_interface,0,gsize-_nbproc-1,_gcom[coupling]);
      _target[coupling] = new ParaMEDMEM::MPIProcessorGroup(*_interface,gsize-_nbproc,gsize-1,_gcom[coupling]);
      _commgroup[coupling] = _target[coupling];
    }
  
  _dec[coupling] = NULL;
}

void ParaMEDMEMComponent_i::_setInputField(const char * coupling, ParaMEDMEM::MEDCouplingFieldDouble *field)
{
  string service = coupling;
  if( service.size() == 0 )
    {
      MESSAGE("[" << _numproc << "] You have to give a service name !");
      throw POException(_numproc,"You have to give a service name !");
    }

  if( _gcom.find(service) == _gcom.end() )
    {
      MESSAGE("[" << _numproc << "] service " << service << " don't exist !");
      throw POException(_numproc,"service " + service + " don't exist !");
    }

  if(!_dec[coupling])
    {

      //Creating the intersection Data Exchange Channel
      _dec[coupling] = new ParaMEDMEM::InterpKernelDEC(*_source[coupling], *_target[coupling]);
      
      //Attaching the field to the DEC
      _dec[coupling]->attachLocalField(field);

      // computing the interpolation matrix
      _dec[coupling]->synchronize();

    }

  //Attaching the field to the DEC
  _dec[coupling]->attachLocalField(field);
  
  //Receiving data
  _dec[coupling]->recvData();
}

void ParaMEDMEMComponent_i::_getOutputField(const char * coupling, ParaMEDMEM::MEDCouplingFieldDouble *field)
{
  string service = coupling;
  if( service.size() == 0 )
    {
      MESSAGE("[" << _numproc << "] You have to give a service name !");
      throw POException(_numproc,"You have to give a service name !");
    }

  if( _gcom.find(service) == _gcom.end() )
    {
      MESSAGE("[" << _numproc << "] service " << service << " don't exist !");
      throw POException(_numproc,"service " + service + " don't exist !");
    }

  if(!_dec[coupling])
    {

      //Creating the intersection Data Exchange Channel
      _dec[coupling] = new ParaMEDMEM::InterpKernelDEC(*_source[coupling], *_target[coupling]);
  
      //Attaching the field to the DEC
      _dec[coupling]->attachLocalField(field);
    
      // computing the interpolation matrix
      _dec[coupling]->synchronize();

    }

  //Attaching the field to the DEC
  _dec[coupling]->attachLocalField(field);

  //Sending data
  _dec[coupling]->sendData();
}

void ParaMEDMEMComponent_i::_terminateCoupling(const char * coupling)
{
  string service = coupling;
  if( service.size() == 0 )
    {
      MESSAGE("[" << _numproc << "] You have to give a service name !");
      throw POException(_numproc,"You have to give a service name !");
    }

  if( _gcom.find(service) == _gcom.end() )
    {
      MESSAGE("[" << _numproc << "] service " << service << " don't exist !");
      throw POException(_numproc,"service " + service + " don't exist !");
    }

  // Disconnection to distributed parallel component
#ifdef HAVE_MPI2
  remoteMPI2Disconnect(coupling);
#else
  MESSAGE("[" << _numproc << "] You have to use a MPI2 compliant mpi implementation !");
  throw POException(_numproc,"You have to use a MPI2 compliant mpi implementation !");
#endif

  /* Processors groups and DEC destruction */
  delete _source[coupling];
  _source.erase(coupling);
  delete _target[coupling];
  _target.erase(coupling);
  delete _dec[coupling];
  _dec.erase(coupling);
  _commgroup.erase(coupling);
}

void *th_getdatabympi(void *s)
{
  thread_st *st = (thread_st*)s;
  (SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface::_narrow((*(st->tior))[st->ip]))->getDataByMPI(st->coupling.c_str());
  delete st;
  return NULL;
}

