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
  _initInterpolationOptions = false;
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
  _initInterpolationOptions = false;
  _interface = new ParaMEDMEM::CommInterface();
}

ParaMEDMEMComponent_i::~ParaMEDMEMComponent_i()
{
  MESSAGE("* [" << _numproc << "] ParaMEDMEMComponent destructor");
  delete _interface;
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
						    bool P1P0_bary_method ) throw(SALOME::SALOME_Exception)
{
  _initInterpolationOptions=true;
  string interType = intersection_type;
  string splitPolicy = splitting_policy;

  _print_level=print_level;
  if( interType.find("Triangulation") != string::npos )
    _intersection_type=INTERP_KERNEL::Triangulation;
  else if( interType.find("Convex") != string::npos )
    _intersection_type=INTERP_KERNEL::Convex;
  else if( interType.find("Geometric2D") != string::npos )
    _intersection_type=INTERP_KERNEL::Geometric2D;
  else if( interType.find("PointLocator") != string::npos )
    _intersection_type=INTERP_KERNEL::PointLocator;
  else{
    // exception
    ostringstream msg;
    msg << "Intersection type no valid";
    MESSAGE(msg.str());
    THROW_SALOME_CORBA_EXCEPTION(msg.str().c_str(),SALOME::INTERNAL_ERROR);
  }
  _precision=precision;
  _median_plane=median_plane;
  _do_rotate=do_rotate;
  _bounding_box_adjustment=bounding_box_adjustment;
  _bounding_box_adjustment_abs=bounding_box_adjustment_abs;
  _max_distance_for_3Dsurf_intersect=max_distance_for_3Dsurf_intersect;
  _orientation=orientation;
  _measure_abs=measure_abs;
  if( splitPolicy.find("PLANAR_FACE_5") != string::npos )
    _splitting_policy=INTERP_KERNEL::PLANAR_FACE_5;
  else if( splitPolicy.find("PLANAR_FACE_6") != string::npos )
    _splitting_policy=INTERP_KERNEL::PLANAR_FACE_6;
  else if( splitPolicy.find("GENERAL_24") != string::npos )
    _splitting_policy=INTERP_KERNEL::GENERAL_24;
  else if( splitPolicy.find("GENERAL_48") != string::npos )
    _splitting_policy=INTERP_KERNEL::GENERAL_48;
  else{
    // exception
    ostringstream msg;
    msg << "Splitting policy no valid";
    MESSAGE(msg.str());
    THROW_SALOME_CORBA_EXCEPTION(msg.str().c_str(),SALOME::INTERNAL_ERROR);
  }
  _P1P0_bary_method=P1P0_bary_method;
}


void ParaMEDMEMComponent_i::_initializeCoupling(const char * coupling)
{
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

      if(_initInterpolationOptions){
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
  
      if(_initInterpolationOptions){
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

