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
  class ParaSUPPORT
  {
  public:
    ParaSUPPORT();
    ParaSUPPORT(const MEDMEM::SUPPORT&);
    virtual ~ParaSUPPORT();
    virtual const Topology* getTopology() const {};
    virtual const MEDMEM::SUPPORT* getSupport() const {return _support;}
    virtual const ParaMESH* getMesh() const {return _mesh;}
  protected :
    const MEDMEM::SUPPORT* _support;
    const ParaMESH* _mesh;
  };
  
}


#endif /*PARASUPPORT_HXX_*/
