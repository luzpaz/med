#include "LocalTraceCollector.hxx"

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
  LocalTraceCollector::instance();
}
