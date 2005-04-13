#ifndef _FIELDINTCLIENT_HXX
#define _FIELDINTCLIENT_HXX

#include "FIELDClient.hxx"

namespace MEDMEM{
class FIELDINTClient :
  public FIELDClient<int>
{
public:
  FIELDINTClient(SALOME_MED::FIELDINT_ptr ptrCorba,
		 MEDMEM::SUPPORT * S = NULL);
  ~FIELDINTClient();
};
}
#endif
