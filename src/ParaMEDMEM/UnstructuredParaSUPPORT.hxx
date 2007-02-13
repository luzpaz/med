#ifndef UNSTRUCTUREDPARASUPPORT_HXX_
#define UNSTRUCTUREDPARASUPPORT_HXX_

#include "ParaSUPPORT.hxx"
#include "MEDMEM_define.hxx"
#include "ExplicitTopology.hxx"

using namespace MED_EN;
namespace MEDMEM
{
	class SUPPORT;
}

namespace ParaMEDMEM
{
  class Topology;
  class ExplicitTopology;
  class ParaMESH;
  

  class UnstructuredParaSUPPORT:public ParaSUPPORT
  {
  public:
    
    UnstructuredParaSUPPORT(const ParaMESH* const mesh, const MEDMEM::SUPPORT* support );
    UnstructuredParaSUPPORT(const ParaMESH* const mesh, const MED_EN::medEntityMesh entity);
    virtual ~UnstructuredParaSUPPORT();
    const Topology* getTopology() const {return _explicit_topology;}
    
  private:
    const ExplicitTopology* const  _explicit_topology;
    const MED_EN::medEntityMesh _entity;
	
  };
  
}
#endif /*STRUCTUREDPARASUPPORT_HXX_*/
