#include "FIELDDOUBLEClient.hxx"
namespace MEDMEM{

//=============================================================================
/*!
 * Constructor with arguments
 */
//=============================================================================
FIELDDOUBLEClient::FIELDDOUBLEClient(SALOME_MED::FIELDDOUBLE_ptr ptrCorba,
				     MEDMEM::SUPPORT * S) :
  FIELDClient<double,SALOME_MED::FIELDDOUBLE>(ptrCorba,S)
{
  BEGIN_OF("Constructor with arguments (for Python API) FIELDDOUBLEClient");

  END_OF("Constructor with arguments (for Python API) FIELDDOUBLEClient");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
FIELDDOUBLEClient::~FIELDDOUBLEClient()
{
  BEGIN_OF("Default Destructor (for Python API) FIELDDOUBLEClient");

  END_OF("Default Destructor (for Python API) FIELDDOUBLEClient");
}

}
