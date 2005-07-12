#ifndef _FIELDDOUBLECLIENT_HXX
#define _FIELDDOUBLECLIENT_HXX

#include "FIELDClient.hxx"

namespace MEDMEM{
    
class FIELDDOUBLEClient :
  public FIELDClient<double>
{
public:
  FIELDDOUBLEClient(SALOME_MED::FIELDDOUBLE_ptr ptrCorba,
		    MEDMEM::SUPPORT * S = NULL);
  ~FIELDDOUBLEClient();
};

}
#endif
