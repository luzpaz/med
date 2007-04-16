
/*! Data Exchange Channel 
 * Interface class for creation of a link between two 
 * MPI groups for exhanging mesh or field data*/

#include "CommInterface.hxx"
#include "Topology.hxx"
#include "BlockTopology.hxx"
#include "ComponentTopology.hxx"
#include "ParaFIELD.hxx"
#include "DEC.hxx"

namespace ParaMEDMEM
{
  
  DEC::DEC(ProcessorGroup& local_group, ProcessorGroup& distant_group):_source_field(0),_target_field(0), 
  _source_group(&local_group), _target_group(&distant_group)
  {
    
    _union_group = local_group.fuse(distant_group);  
  }

  DEC::~DEC()
  {
    delete _union_group;
  }  

void DEC::attachTargetField(const ParaFIELD* field) 
{
  _target_field=field;
  //if (field!=0)
  //{
  //BlockTopology* topo=dynamic_cast<BlockTopology*>(field->getTopology());
  _comm_interface=&(field->getTopology()->getProcGroup()->getCommInterface());
  //}
}
void DEC::attachSourceField(const ParaFIELD* field) 
{
  _source_field=field;
  //if (field!=0)
  //{
  //	BlockTopology* topo=dynamic_cast<BlockTopology*>(field->getTopology());
  _comm_interface=&(field->getTopology()->getProcGroup()->getCommInterface());
  //}
}
}
