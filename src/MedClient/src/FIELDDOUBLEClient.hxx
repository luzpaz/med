#ifndef _FIELDDOUBLECLIENT_HXX
#define _FIELDDOUBLECLIENT_HXX

#include "FIELDClient.hxx"

class FIELDDOUBLEClient :
  public FIELDClient<double,SALOME_MED::FIELDDOUBLE>
{
public:
  FIELDDOUBLEClient(SALOME_MED::FIELDDOUBLE_ptr ptrCorba,
		    SUPPORT * S = NULL);
  ~FIELDDOUBLEClient();
};

#endif
