#include "FIELDINTClient.hxx"
namespace MEDMEM{
    
//=============================================================================
/*!
 * Constructor with arguments
 */
//=============================================================================
FIELDINTClient::FIELDINTClient(SALOME_MED::FIELDINT_ptr ptrCorba,
				     MEDMEM::SUPPORT * S) :
  FIELDClient<int,SALOME_MED::FIELDINT>(ptrCorba,S)
{
  BEGIN_OF("Constructor with arguments (for Python API) FIELDINTClient");

  END_OF("Constructor with arguments (for Python API) FIELDINTClient");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
FIELDINTClient::~FIELDINTClient()
{
  BEGIN_OF("Default Destructor (for Python API) FIELDINTClient");

  END_OF("Default Destructor (for Python API) FIELDINTClient");
}

}
