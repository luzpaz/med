#  ifdef MED_WITH_KERNEL
#include "LocalTraceCollector.hxx"
#  endif /* ifdef MED_WITH_KERNEL*/

namespace MEDMEM
{
  class INIT
  {
  public:
    INIT();
  };
}

MEDMEM::INIT init;

MEDMEM::INIT::INIT()
{
#ifdef MED_WITH_KERNEL
  LocalTraceCollector::instance();
#endif /* ifdef MED_WITH_KERNEL*/
}
