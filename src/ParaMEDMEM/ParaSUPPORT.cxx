#include "ParaSUPPORT.hxx"
#include "ParaMESH.hxx"
#include "MEDMEM_Support.hxx"

namespace ParaMEDMEM
{

  ParaSUPPORT::ParaSUPPORT()
  {
  }
  
  ParaSUPPORT::ParaSUPPORT(const MEDMEM::SUPPORT& support, const ProcessorGroup& proc_group):_support(&support) {
  _mesh = new ParaMESH(*(support.getMesh()),  proc_group, "mesh from support");
  }
  
  ParaSUPPORT::~ParaSUPPORT()
  {
  }
  
}

