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
using namespace ParaMEDMEM;

ParaMEDMEMComponent_i::ParaMEDMEMComponent_i() : Engines_Component_i(), MPIObject_i()
{
  _interface = new CommInterface;
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
  _interface = new CommInterface;
}

ParaMEDMEMComponent_i::~ParaMEDMEMComponent_i()
{
  MESSAGE("* [" << _numproc << "] ParaMEDMEMComponent destructor");
  delete _interface;
}

void ParaMEDMEMComponent_i::initializeCoupling(const char * coupling) throw(SALOME::SALOME_Exception)
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
          pthread_create(&(th[ip]),NULL,th_initializecoupling,(void*)st);
        }
    }

  try{
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

    MPI_Comm_size( _gcom[coupling], &_gsize );
    MPI_Comm_rank( _gcom[coupling], &_grank );
    MESSAGE("[" << _grank << "] new communicator of " << _gsize << " processes");

    // Creation of processors group for ParaMEDMEM
    // source is always the lower processor numbers
    // target is always the upper processor numbers
    if(_numproc==_grank)
      {
        _source[coupling] = new MPIProcessorGroup(*_interface,0,_nbproc-1,_gcom[coupling]);
        _target[coupling] = new MPIProcessorGroup(*_interface,_nbproc,_gsize-1,_gcom[coupling]);
        _commgroup[coupling] = _source[coupling];
      }
    else
      {
        _source[coupling] = new MPIProcessorGroup(*_interface,0,_gsize-_nbproc-1,_gcom[coupling]);
        _target[coupling] = new MPIProcessorGroup(*_interface,_gsize-_nbproc,_gsize-1,_gcom[coupling]);
        _commgroup[coupling] = _target[coupling];
      }
    
    _dec[coupling] = NULL;
    _dec_options[coupling] = NULL;
    
  }
  catch(const POException &ex)
    {
      // exception
      ostringstream msg;
      msg << ex.msg << " on process number " << ex.numproc;
      MESSAGE(msg.str());
      THROW_SALOME_CORBA_EXCEPTION(msg.str().c_str(),SALOME::INTERNAL_ERROR);
    }
  catch(const INTERP_KERNEL::Exception &ex)
    {
      MESSAGE(ex.what());
      THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::INTERNAL_ERROR);
    }
  catch(...)
    {
      MESSAGE("Unknown exception");
      THROW_SALOME_CORBA_EXCEPTION("Unknown exception",SALOME::INTERNAL_ERROR);
    }

  if(_numproc == 0)
    {
      for(int ip=1;ip<_nbproc;ip++){
        pthread_join(th[ip],&ret_th);
        exception = (bool*)ret_th;
        if(*exception){
          // exception
          ostringstream msg;
          msg << "Error on initialize coupling on process " << ip;
          THROW_SALOME_CORBA_EXCEPTION(msg.str().c_str(),SALOME::INTERNAL_ERROR);
        }
        delete exception;
      }
      delete[] th;
    }
}

void ParaMEDMEMComponent_i::terminateCoupling(const char * coupling) throw(SALOME::SALOME_Exception)
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
          pthread_create(&(th[ip]),NULL,th_terminatecoupling,(void*)st);
        }
    }

  try{
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
    if(_dec_options[coupling])
      {
        delete _dec_options[coupling];
        _dec_options.erase(coupling);
      }
  }
  catch(const POException &ex)
    {
      // exception
      ostringstream msg;
      msg << ex.msg << " on process number " << ex.numproc;
      MESSAGE(msg.str());
      THROW_SALOME_CORBA_EXCEPTION(msg.str().c_str(),SALOME::INTERNAL_ERROR);
    }

  if(_numproc == 0)
    {
      for(int ip=1;ip<_nbproc;ip++){
        pthread_join(th[ip],&ret_th);
        exception = (bool*)ret_th;
        if(*exception){
          // exception
          ostringstream msg;
          msg << "Error on terminate coupling on process " << ip;
          THROW_SALOME_CORBA_EXCEPTION(msg.str().c_str(),SALOME::INTERNAL_ERROR);
        }
        delete exception;
      }
      delete[] th;
    }
}

void ParaMEDMEMComponent_i::setInterpolationOptions(const char * coupling,
                                                    long print_level,
                                                    const char * intersection_type,
                                                    double precision,
                                                    double median_plane,
                                                    bool do_rotate,
                                                    double bounding_box_adjustment,
                                                    double bounding_box_adjustment_abs,
                                                    double max_distance_for_3Dsurf_intersect,
                                                    long orientation,
                                                    bool measure_abs,
                                                    const char * splitting_policy,
                                                    bool P1P0_bary_method ) throw(SALOME::SALOME_Exception)
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
          st->print_level = print_level;
          st->intersection_type = intersection_type;
          st->precision = precision;
          st->median_plane = median_plane;
          st->do_rotate = do_rotate;
          st->bounding_box_adjustment = bounding_box_adjustment;
          st->bounding_box_adjustment_abs = bounding_box_adjustment_abs;
          st->max_distance_for_3Dsurf_intersect = max_distance_for_3Dsurf_intersect;
          st->orientation = orientation;
          st->measure_abs = measure_abs;
          st->splitting_policy = splitting_policy;
          st->P1P0_bary_method = P1P0_bary_method;
          pthread_create(&(th[ip]),NULL,th_setinterpolationoptions,(void*)st);
        }
    }

  if(!_dec_options[coupling])
    _dec_options[coupling] = new INTERP_KERNEL::InterpolationOptions;

  bool ret = _dec_options[coupling]->setInterpolationOptions(print_level,
                                                             intersection_type,
                                                             precision,
                                                             median_plane,
                                                             do_rotate,
                                                             bounding_box_adjustment,
                                                             bounding_box_adjustment_abs,
                                                             max_distance_for_3Dsurf_intersect,
                                                             orientation,
                                                             measure_abs,
                                                             splitting_policy,
                                                             P1P0_bary_method );

  if(!ret)
    {
      // exception
      ostringstream msg;
      msg << "[" << _numproc << "] Error on setting options";
      MESSAGE(msg.str());
      THROW_SALOME_CORBA_EXCEPTION(msg.str().c_str(),SALOME::INTERNAL_ERROR);
    }
  
  if(_numproc == 0)
    {
      for(int ip=1;ip<_nbproc;ip++){
        pthread_join(th[ip],&ret_th);
        exception = (bool*)ret_th;
        if(*exception){
          // exception
          ostringstream msg;
          msg << "Error on setting options on process " << ip;
          THROW_SALOME_CORBA_EXCEPTION(msg.str().c_str(),SALOME::INTERNAL_ERROR);
        }
        delete exception;
      }
      delete[] th;
    }
}

void ParaMEDMEMComponent_i::_setInputField(const char * coupling, SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface_ptr fieldptr, MEDCouplingFieldDouble *field)
{
  bool *exception;
  void *ret_th;
  pthread_t th;
  if(_numproc == 0)
    {
      thread_st *st = new thread_st;
      st->fieldptr = fieldptr;
      st->coupling = coupling;
      pthread_create(&th,NULL,th_getdata,(void*)st);
    }

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

      // Creating the intersection Data Exchange Channel
      // Processors which received the field are always the second argument of InterpKernelDEC object
      if(_numproc==_grank)
        _dec[coupling] = new InterpKernelDEC(*_target[coupling], *_source[coupling]);
      else
        _dec[coupling] = new InterpKernelDEC(*_source[coupling], *_target[coupling]);

      if(_dec_options[coupling])
        _dec[coupling]->copyOptions(*(_dec_options[coupling]));
      
      //Attaching the field to the DEC
      _dec[coupling]->attachLocalField(field);

      // computing the interpolation matrix
      _dec[coupling]->synchronize();

    }
  else
    //Attaching the field to the DEC
    _dec[coupling]->attachLocalField(field);
  
  //Receiving data
  _dec[coupling]->recvData();

  if(_numproc == 0)
    {
      pthread_join(th,&ret_th);
      exception = (bool*)ret_th;
      if(*exception){
        // exception
        ostringstream msg;
        msg << "Error on get data by mpi";
        THROW_SALOME_CORBA_EXCEPTION(msg.str().c_str(),SALOME::INTERNAL_ERROR);
      }
      delete exception;
    }

}

void ParaMEDMEMComponent_i::_getOutputField(const char * coupling, MEDCouplingFieldDouble *field)
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
      // Creating the intersection Data Exchange Channel
      // Processors which sent the field are always the first argument of InterpKernelDEC object
      if(_numproc==_grank)
        _dec[coupling] = new InterpKernelDEC(*_source[coupling], *_target[coupling]);
      else
        _dec[coupling] = new InterpKernelDEC(*_target[coupling], *_source[coupling]);
  
      if(_dec_options[coupling])
        _dec[coupling]->copyOptions(*(_dec_options[coupling]));
      
      //Attaching the field to the DEC
      _dec[coupling]->attachLocalField(field);
    
      // computing the interpolation matrix
      _dec[coupling]->synchronize();
    }
  else
    //Attaching the field to the DEC
    _dec[coupling]->attachLocalField(field);

  //Sending data
  _dec[coupling]->sendData();
}

void *th_setinterpolationoptions(void *s)
{
  thread_st *st = (thread_st*)s;
  bool *exception = new bool;
  *exception = false;
  try
    {
      SALOME_MED::ParaMEDMEMComponent_var compo=SALOME_MED::ParaMEDMEMComponent::_narrow((*(st->tior))[st->ip]);
      compo->setInterpolationOptions(st->coupling.c_str(),
                                     st->print_level,
                                     st->intersection_type,
                                     st->precision,
                                     st->median_plane,
                                     st->do_rotate,
                                     st->bounding_box_adjustment,
                                     st->bounding_box_adjustment_abs,
                                     st->max_distance_for_3Dsurf_intersect,
                                     st->orientation,
                                     st->measure_abs,
                                     st->splitting_policy,
                                     st->P1P0_bary_method);
    }
  catch(...)
    {
      *exception = true;
    }
  delete st;
  return((void*)exception);
}

void *th_initializecoupling(void *s)
{
  thread_st *st = (thread_st*)s;
  bool *exception = new bool;
  *exception = false;
  try
    {
      SALOME_MED::ParaMEDMEMComponent_var compo=SALOME_MED::ParaMEDMEMComponent::_narrow((*(st->tior))[st->ip]);
      compo->initializeCoupling(st->coupling.c_str());
    }
  catch(...)
    {
      *exception = true;
    }
  delete st;
  return((void*)exception);
}

void *th_terminatecoupling(void *s)
{
  thread_st *st = (thread_st*)s;
  bool *exception = new bool;
  *exception = false;
  try
    {
      SALOME_MED::ParaMEDMEMComponent_var compo=SALOME_MED::ParaMEDMEMComponent::_narrow((*(st->tior))[st->ip]);
      compo->terminateCoupling(st->coupling.c_str());
    }
  catch(...)
    {
      *exception = true;
    }
  delete st;
  return((void*)exception);
}

void *th_getdata(void *s)
{
  thread_st *st = (thread_st*)s;
  bool *exception = new bool;
  *exception = false;
  try
    {
      st->fieldptr->getDataByMPI(st->coupling.c_str());
    }
  catch(...)
    {
      *exception = true;
    }
  delete st;
  return((void*)exception);
}

