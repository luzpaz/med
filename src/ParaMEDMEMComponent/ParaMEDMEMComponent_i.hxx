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
