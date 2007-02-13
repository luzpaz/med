#include "Topology.hxx"
#include "ExplicitTopology.hxx"
#include "ParaMESH.hxx"
#include "UnstructuredParaSUPPORT.hxx"
#include "MEDMEM_Support.hxx"

namespace ParaMEDMEM 
{
	
/*! Constructor on all elements from a MESH */
UnstructuredParaSUPPORT::UnstructuredParaSUPPORT(const ParaMESH* const mesh, const MEDMEM::SUPPORT* support):
_entity(support->getEntity()),
_explicit_topology(new ExplicitTopology(*support))
{
  _mesh=mesh;
  _support=support;
}

UnstructuredParaSUPPORT::UnstructuredParaSUPPORT(const ParaMESH* const mesh, const MED_EN::medEntityMesh entity):
  ParaSUPPORT(mesh, new MEDMEM::SUPPORT(mesh->getMesh(), "support on all entities", entity)),
  _entity(entity),
  _explicit_topology(new ExplicitTopology(*this))
{
  //_mesh=mesh;
  //  _support=new SUPPORT(_mesh->getMesh(), "support on all entities", entity);
  //_explicit_topology(new ExplicitTopology(*support));
}

UnstructuredParaSUPPORT::~UnstructuredParaSUPPORT()
{
	delete _support;
}

}//end of namespace ParaMEDMEM
