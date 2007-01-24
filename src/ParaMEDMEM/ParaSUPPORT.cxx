#include "ParaSUPPORT.hxx"
#include "ParaMESH.hxx"
#include "MEDMEM_Support.hxx"

namespace ParaMEDMEM
{

  ParaSUPPORT::ParaSUPPORT()
  {
  }
  
  ParaSUPPORT::ParaSUPPORT(const MEDMEM::SUPPORT& support):_support(&support) {}
  
  ParaSUPPORT::~ParaSUPPORT()
  {
  }
  
}

