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

ParaMEDMEMComponent_i::ParaMEDMEMComponent_i() : Engines_Component_i(), MPIObject_i(), InterpolationOptions()
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
  : Engines_Component_i(orb,poa,contId,instanceName,interfaceName,false,regist), MPIObject_i(nbproc,numproc), InterpolationOptions()
{
  _interface = new ParaMEDMEM::CommInterface();
}

ParaMEDMEMComponent_i::~ParaMEDMEMComponent_i()
{
  MESSAGE("* [" << _numproc << "] ParaMEDMEMComponent destructor");
  delete _interface;
}

void ParaMEDMEMComponent_i::initializeCoupling(const char * coupling) throw(SALOME::SALOME_Exception)
{
  pthread_t *th;
  if(_numproc == 0){
    th = new pthread_t[_nbproc];
    for(int ip=1;ip<_nbproc;ip++){
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
	_source[coupling] = new ParaMEDMEM::MPIProcessorGroup(*_interface,0,_nbproc-1,_gcom[coupling]);
	_target[coupling] = new ParaMEDMEM::MPIProcessorGroup(*_interface,_nbproc,_gsize-1,_gcom[coupling]);
	_commgroup[coupling] = _source[coupling];
      }
    else
      {
	_source[coupling] = new ParaMEDMEM::MPIProcessorGroup(*_interface,0,_gsize-_nbproc-1,_gcom[coupling]);
	_target[coupling] = new ParaMEDMEM::MPIProcessorGroup(*_interface,_gsize-_nbproc,_gsize-1,_gcom[coupling]);
	_commgroup[coupling] = _target[coupling];
      }
    
    _dec[coupling] = NULL;
    
  }
  catch(const POException &ex){
    // exception
    ostringstream msg;
    msg << ex.msg << " on process number " << ex.numproc;
    MESSAGE(msg.str());
    THROW_SALOME_CORBA_EXCEPTION(msg.str().c_str(),SALOME::INTERNAL_ERROR);
  }
  catch(const INTERP_KERNEL::Exception &ex){
    MESSAGE(ex.what());
    THROW_SALOME_CORBA_EXCEPTION(ex.what(),SALOME::INTERNAL_ERROR);
  }
  catch(...){
    MESSAGE("Unknown exception");
    THROW_SALOME_CORBA_EXCEPTION("Unknown exception",SALOME::INTERNAL_ERROR);
  }

  if(_numproc == 0){
    for(int ip=1;ip<_nbproc;ip++)
      pthread_join(th[ip],NULL);
    delete[] th;
  }
}

void ParaMEDMEMComponent_i::terminateCoupling(const char * coupling) throw(SALOME::SALOME_Exception)
{
  pthread_t *th;
  if(_numproc == 0){
    th = new pthread_t[_nbproc];
    for(int ip=1;ip<_nbproc;ip++){
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
  }
  catch(const POException &ex){
    // exception
    ostringstream msg;
    msg << ex.msg << " on process number " << ex.numproc;
    MESSAGE(msg.str());
    THROW_SALOME_CORBA_EXCEPTION(msg.str().c_str(),SALOME::INTERNAL_ERROR);
  }

  if(_numproc == 0){
    for(int ip=1;ip<_nbproc;ip++)
      pthread_join(th[ip],NULL);
    delete[] th;
  }
}

void ParaMEDMEMComponent_i::setInterpolationOptions(long print_level,
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
						    bool P1P0_bary_method )
{
  pthread_t *th;
  if(_numproc == 0){
    th = new pthread_t[_nbproc];
    for(int ip=1;ip<_nbproc;ip++){
      thread_st *st = new thread_st;
      st->ip = ip;
      st->tior = _tior;
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

  INTERP_KERNEL::InterpolationOptions::setInterpolationOptions(print_level,
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

  if(_numproc == 0){
    for(int ip=1;ip<_nbproc;ip++)
      pthread_join(th[ip],NULL);
    delete[] th;
  }
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

      // Creating the intersection Data Exchange Channel
      // Processors which received the field are always the second argument of InterpKernelDEC object
      if(_numproc==_grank)
	_dec[coupling] = new ParaMEDMEM::InterpKernelDEC(*_target[coupling], *_source[coupling]);
      else
	_dec[coupling] = new ParaMEDMEM::InterpKernelDEC(*_source[coupling], *_target[coupling]);

      if(_setInterpolationOptions){
	_dec[coupling]->setPrintLevel(_print_level);
	_dec[coupling]->setIntersectionType(_intersection_type);
	_dec[coupling]->setPrecision(_precision);
	_dec[coupling]->setMedianPlane(_median_plane);
	_dec[coupling]->setDoRotate(_do_rotate);
	_dec[coupling]->setBoundingBoxAdjustment(_bounding_box_adjustment);
	_dec[coupling]->setBoundingBoxAdjustmentAbs(_bounding_box_adjustment_abs);
	_dec[coupling]->setMaxDistance3DSurfIntersect(_max_distance_for_3Dsurf_intersect);
	_dec[coupling]->setOrientation(_orientation);
	_dec[coupling]->setMeasureAbsStatus(_measure_abs);
	_dec[coupling]->setSplittingPolicy(_splitting_policy);
	_dec[coupling]->setP1P0BaryMethod(_P1P0_bary_method);
      }
      
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

      // Creating the intersection Data Exchange Channel
      // Processors which sent the field are always the first argument of InterpKernelDEC object
      if(_numproc==_grank)
	_dec[coupling] = new ParaMEDMEM::InterpKernelDEC(*_source[coupling], *_target[coupling]);
      else
	_dec[coupling] = new ParaMEDMEM::InterpKernelDEC(*_target[coupling], *_source[coupling]);
  
      if(_setInterpolationOptions){
	_dec[coupling]->setPrintLevel(_print_level);
	_dec[coupling]->setIntersectionType(_intersection_type);
	_dec[coupling]->setPrecision(_precision);
	_dec[coupling]->setMedianPlane(_median_plane);
	_dec[coupling]->setDoRotate(_do_rotate);
	_dec[coupling]->setBoundingBoxAdjustment(_bounding_box_adjustment);
	_dec[coupling]->setBoundingBoxAdjustmentAbs(_bounding_box_adjustment_abs);
	_dec[coupling]->setMaxDistance3DSurfIntersect(_max_distance_for_3Dsurf_intersect);
	_dec[coupling]->setOrientation(_orientation);
	_dec[coupling]->setMeasureAbsStatus(_measure_abs);
	_dec[coupling]->setSplittingPolicy(_splitting_policy);
	_dec[coupling]->setP1P0BaryMethod(_P1P0_bary_method);
      }
      
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

void *th_getdatabympi(void *s)
{
  thread_st *st = (thread_st*)s;
  (SALOME_MED::MPIMEDCouplingFieldDoubleCorbaInterface::_narrow((*(st->tior))[st->ip]))->getDataByMPI(st->coupling.c_str());
  delete st;
  return NULL;
}

void *th_setinterpolationoptions(void *s)
{
  thread_st *st = (thread_st*)s;
  try{
    (SALOME_MED::ParaMEDMEMComponent::_narrow((*(st->tior))[st->ip]))->setInterpolationOptions(st->print_level,
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
  catch(...){
    cerr << "Caught an exception in a thread on process: " << st->ip << endl;
  }
  delete st;
  return NULL;
}

void *th_initializecoupling(void *s)
{
  thread_st *st = (thread_st*)s;
  try{
    (SALOME_MED::ParaMEDMEMComponent::_narrow((*(st->tior))[st->ip]))->initializeCoupling(st->coupling.c_str());
  }
  catch(...){
    cerr << "Caught an exception in a thread on process: " << st->ip << endl;
  }
  delete st;
  return NULL;
}

void *th_terminatecoupling(void *s)
{
  thread_st *st = (thread_st*)s;
  (SALOME_MED::ParaMEDMEMComponent::_narrow((*(st->tior))[st->ip]))->terminateCoupling(st->coupling.c_str());
  delete st;
  return NULL;
}

