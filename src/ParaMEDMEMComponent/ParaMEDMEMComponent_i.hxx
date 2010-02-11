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
#ifndef __PARAMEDMEMCOMPOCOMPONENT_I__
#define __PARAMEMDEMCOMPOCOMPONENT_I__

#include "SALOMEconfig.h"
#include CORBA_SERVER_HEADER(ParaMEDMEMComponent)
#include "MPIObject_i.hxx"
#include "SALOME_Component_i.hxx"
#include "Topology.hxx"
#include "MEDCouplingFieldDouble.hxx"
#include "InterpKernelDEC.hxx"
#include "InterpolationOptions.hxx"
#include "MPIProcessorGroup.hxx"
#include "CommInterface.hxx"
#include "MEDCouplingFieldDoubleServant.hxx"
#include "Utils_CorbaException.hxx"
#include <map>

void * th_setinterpolationoptions(void *st);
void * th_initializecoupling(void *st);
void * th_terminatecoupling(void *st);
void * th_getdatabympi(void *st);

typedef struct {
  int ip;
  long print_level;
  const char * intersection_type;
  double precision;
  double median_plane;
  bool do_rotate;
  double bounding_box_adjustment;
  double bounding_box_adjustment_abs;
  double max_distance_for_3Dsurf_intersect;
  long orientation;
  bool measure_abs;
  const char * splitting_policy;
  bool P1P0_bary_method;
  std::string coupling;
  Engines::IORTab* tior;
} thread_st;

namespace ParaMEDMEM
{
  class ParaMEDMEMComponent_i : public POA_SALOME_MED::ParaMEDMEMComponent, public Engines_Component_i, public MPIObject_i, public INTERP_KERNEL::InterpolationOptions
  {

  public:
    // Constructor
    ParaMEDMEMComponent_i();
    ParaMEDMEMComponent_i(int nbproc, int numproc,
			  CORBA::ORB_ptr orb,
			  PortableServer::POA_ptr poa, 
			  PortableServer::ObjectId * contId, 
			  const char *instanceName,
			  const char *interfaceName,
			  bool regist);
    
    // Destructor
    ~ParaMEDMEMComponent_i();
    void setInterpolationOptions(long print_level,
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
				 bool P1P0_bary_method );
    void initializeCoupling(const char * coupling) throw(SALOME::SALOME_Exception);
    void terminateCoupling(const char * coupling) throw(SALOME::SALOME_Exception);
    void _getOutputField(const char * coupling, MEDCouplingFieldDouble* field);
    
  protected:
    std::map<std::string,ProcessorGroup*> _commgroup;
    void _setInputField(const char * coupling, MEDCouplingFieldDouble* field);
    
  private:
    int _gsize, _grank;
    CommInterface* _interface;
    std::map<std::string,MPIProcessorGroup*> _source, _target;
    std::map<std::string,InterpKernelDEC*> _dec;
  };
}
#endif
