#include "MEDMEM_SWIG_FieldInt.hxx"
using namespace std;

//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
//CCRTFIELDINT::FIELDINT() : FIELD<int>()
FIELDINT::FIELDINT() : FIELD<med_int>()
{
        BEGIN_OF("Default Constructor (for Python API) FIELDINT");

        END_OF("Default Constructor (for Python API) FIELDINT");
}
//=============================================================================
/*!
 * Constructor with arguments
 */
//=============================================================================
//CCRTFIELDINT::FIELDINT(const SUPPORT * Support, const int NumberOfComponents) : FIELD<int>(Support,NumberOfComponents)
FIELDINT::FIELDINT(const SUPPORT * Support, const int NumberOfComponents) : FIELD<med_int>(Support,NumberOfComponents)
{
  BEGIN_OF("Constructor with arguments (for Python API) FIELDINT");

//CCRT In fact med_int is defined by Med as int on PCLINUX and as long (64 bits) on Alpha-OSF
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
//CCRT  FIELD<int>(Support, driverType, fileName, fieldName,
  FIELD<med_int>(Support, driverType, fileName, fieldName,
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
//CCRTFIELDINT::FIELDINT(const FIELDINT & m) : FIELD<int>( (FIELD<int> &) m)
FIELDINT::FIELDINT(const FIELDINT & m) : FIELD<med_int>( (FIELD<med_int> &) m)
{
  BEGIN_OF("Copy constructor (for Python API) FIELDINT");

  END_OF("Copy constructor (for Python API) FIELDINT");
}
//=============================================================================
/*!
 * Copy constructor from a FIELD<int>
 */
//=============================================================================
//CCRTFIELDINT::FIELDINT(const FIELD<int> & m) :
FIELDINT::FIELDINT(const FIELD<med_int> & m) :
//CCRT  FIELD<int>( m)
  FIELD<med_int>( m)
{
  BEGIN_OF("Copy constructor (for Python API) FIELDINT from a FIELD<med_int>");

  END_OF("Copy constructor (for Python API) FIELDINT from a FIELD<med_int>");
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
