#ifndef PARASUPPORT_HXX_
#define PARASUPPORT_HXX_

namespace MEDMEM
{
  class SUPPORT;
}
namespace ParaMEDMEM
{
  class Topology;
  class ParaMESH;
  class ProcessorGroup;
  class ParaSUPPORT
  {
  public:
    ParaSUPPORT();
    ParaSUPPORT(const ParaMESH* mesh, const MEDMEM::SUPPORT* support):
      _support(support), _mesh(mesh){};
    ParaSUPPORT(const MEDMEM::SUPPORT&, const ProcessorGroup& proc_group);
    virtual ~ParaSUPPORT();
    virtual const Topology* getTopology() const{};
    virtual const MEDMEM::SUPPORT* getSupport() const {return _support;}
    virtual const ParaMESH* getMesh() const {return _mesh;}
  protected :
    const MEDMEM::SUPPORT* _support;
    const ParaMESH* _mesh;
  };
  
}


#endif /*PARASUPPORT_HXX_*/
