
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
  
  DEC::DEC(ProcessorGroup& source_group, ProcessorGroup& target_group):_local_field(0), 
  _source_group(&source_group), _target_group(&target_group)
  {
    _union_group = source_group.fuse(target_group);  
  }

  DEC::~DEC()
  {
    delete _union_group;
  }  

void DEC::attachLocalField(const ParaFIELD* field) 
{
  _local_field=field;
  //if (field!=0)
  //{
  //BlockTopology* topo=dynamic_cast<BlockTopology*>(field->getTopology());
  _comm_interface=&(field->getTopology()->getProcGroup()->getCommInterface());
  //}
}

}
