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
#include "ParaFIELD.hxx"
#include "IntersectionDEC.hxx"
#include "MPIProcessorGroup.hxx"
#include "CommInterface.hxx"
#include <map>

class ParaMEDMEMComponent_i : public POA_SALOME_MED::ParaMEDMEMComponent, public Engines_Component_i, public MPIObject_i
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

protected:
  ParaMEDMEM::ProcessorGroup* _commgroup;
  void initializeCoupling(const char * coupling, const bool source);
  void setInputFieldCoupling(const char * coupling, ParaMEDMEM::ParaFIELD* field);
  void getOutputFieldCoupling(const char * coupling, ParaMEDMEM::ParaFIELD* field);
  void terminateCoupling(const char * coupling);

private:
  ParaMEDMEM::CommInterface* _interface;
  std::map<std::string,ParaMEDMEM::MPIProcessorGroup*> _source, _target;
  std::map<std::string,ParaMEDMEM::IntersectionDEC*> _dec;
};
#endif
