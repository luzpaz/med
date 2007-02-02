#ifndef UNSTRUCTUREDPARASUPPORT_HXX_
#define UNSTRUCTUREDPARASUPPORT_HXX_

#include "ParaSUPPORT.hxx"
#include "MEDMEM_define.hxx"

using namespace MED_EN;
namespace MEDMEM
{
	class SUPPORT;
}

namespace ParaMEDMEM
{
class BlockTopology;
class ParaMESH;

class UnstructuredParaSUPPORT:public ParaSUPPORT
{
public:
	
	UnstructuredParaSUPPORT(const ParaMESH* const mesh, SUPPORT* support );
	virtual ~UnstructuredParaSUPPORT();
	const Topology* getTopology() const {return _block_topology;}
   
private:
	const BlockTopology* const  _block_topology;
	const MED_EN::medEntityMesh _entity;
	
	
};

}
#endif /*STRUCTUREDPARASUPPORT_HXX_*/
