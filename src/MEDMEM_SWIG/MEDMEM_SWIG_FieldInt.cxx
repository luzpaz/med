#include "MEDMEM_SWIG_FieldInt.hxx"

using namespace std;
using namespace MED_EN;
using namespace MEDMEM;

//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
FIELDINT::FIELDINT() : FIELD<int>()
{
        BEGIN_OF("Default Constructor (for Python API) FIELDINT");

        END_OF("Default Constructor (for Python API) FIELDINT");
}
//=============================================================================
/*!
 * Constructor with arguments
 */
//=============================================================================
FIELDINT::FIELDINT(const SUPPORT * Support, const int NumberOfComponents) : FIELD<int>(Support,NumberOfComponents)
{
  BEGIN_OF("Constructor with arguments (for Python API) FIELDINT");

  med_type_champ type = MED_INT32;

  setValueType(type);

  END_OF("Constructor with arguments (for Python API) FIELDINT");
}
//=============================================================================
/*!
 * Another constructor with arguments
 */
//=============================================================================
FIELDINT::FIELDINT(const SUPPORT * Support, driverTypes driverType,
		   const string & fileName, const string & fieldName,
		   const int iterationNumber, const int orderNumber) :
  FIELD<int>(Support, driverType, fileName, fieldName,
	     iterationNumber, orderNumber)
{
  BEGIN_OF("Another constructor with arguments (for Python API) FIELDINT");

  med_type_champ type = MED_INT32;

  setValueType(type);

  END_OF("Another constructor with arguments (for Python API) FIELDINT");
}
//=============================================================================
/*!
 * Copy constructor
 */
//=============================================================================
FIELDINT::FIELDINT(const FIELDINT & m) : FIELD<int>( (FIELD<int> &) m)
{
  BEGIN_OF("Copy constructor (for Python API) FIELDINT");

  END_OF("Copy constructor (for Python API) FIELDINT");
}
//=============================================================================
/*!
 * Copy constructor from a FIELD<int>
 */
//=============================================================================
FIELDINT::FIELDINT(const FIELD<int> & m) :
  FIELD<int>( m)
{
  BEGIN_OF("Copy constructor (for Python API) FIELDINT from a FIELD<int>");

  END_OF("Copy constructor (for Python API) FIELDINT from a FIELD<int>");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
FIELDINT::~FIELDINT()
{
  BEGIN_OF("Default Destructor (for Python API) FIELDINT");

  END_OF("Default Destructor (for Python API) FIELDINT");
}
