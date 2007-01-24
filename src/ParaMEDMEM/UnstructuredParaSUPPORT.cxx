#include "Topology.hxx"
#include "BlockTopology.hxx"
#include "ParaGRID.hxx"
#include "UnstructuredParaSUPPORT.hxx"
#include "MEDMEM_Support.hxx"

namespace ParaMEDMEM 
{
	
/*! Constructor on all elements from a MESH */
UnstructuredParaSUPPORT::UnstructuredParaSUPPORT(const ParaMESH* const mesh, const SUPPORT* support):
_mesh(mesh), 
_entity(support->getEntity()),
_support(support),
_block_topology(mesh->getBlockTopology()->getProcGroup(), support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS))
{
}

StructuredParaSUPPORT::~StructuredParaSUPPORT()
{
	delete _support;
}

}//end of namespace ParaMEDMEM
