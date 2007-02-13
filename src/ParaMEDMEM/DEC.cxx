
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
{_source_field=field;
//if (field!=0)
//{
//	BlockTopology* topo=dynamic_cast<BlockTopology*>(field->getTopology());
		_comm_interface=&(field->getTopology()->getProcGroup()->getCommInterface());
		//}
}
}
