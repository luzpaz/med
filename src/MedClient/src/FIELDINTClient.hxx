#ifndef _FIELDINTCLIENT_HXX
#define _FIELDINTCLIENT_HXX

#include "FIELDClient.hxx"

class FIELDINTClient :
  public FIELDClient<int,SALOME_MED::FIELDINT>
{
public:
  FIELDINTClient(SALOME_MED::FIELDINT_ptr ptrCorba,
		    SUPPORT * S = NULL);
  ~FIELDINTClient();
};

#endif
