using namespace std;
#include "MEDMEM_SWIG_FieldDouble.hxx"

//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
FIELDDOUBLE::FIELDDOUBLE() : FIELD<double>()
{
  BEGIN_OF("Default Constructor (for Python API) FIELDDOUBLE");

  END_OF("Default Constructor (for Python API) FIELDDOUBLE");
}
//=============================================================================
/*!
 * Constructor with arguments
 */
//=============================================================================
FIELDDOUBLE::FIELDDOUBLE(const SUPPORT * Support, const int NumberOfComponents) : FIELD<double>(Support,NumberOfComponents)
{
  BEGIN_OF("Constructor with arguments (for Python API) FIELDDOUBLE");

  med_type_champ type = MED_REEL64;

  setValueType(type);

  END_OF("Constructor with arguments (for Python API) FIELDDOUBLE");
}
//=============================================================================
/*!
 * Another constructor with arguments
 */
//=============================================================================
FIELDDOUBLE::FIELDDOUBLE(const SUPPORT * Support, driverTypes driverType,
			 const string & fileName, const string & fieldName,
			 const int iterationNumber, const int orderNumber) :
  FIELD<double>(Support, driverType, fileName, fieldName,
		iterationNumber, orderNumber)
{
  BEGIN_OF("Another constructor with arguments (for Python API) FIELDDOUBLE");

  med_type_champ type = MED_REEL64;

  setValueType(type);

  END_OF("Another constructor with arguments (for Python API) FIELDDOUBLE");
}
//=============================================================================
/*!
 * Copy constructor
 */
//=============================================================================
FIELDDOUBLE::FIELDDOUBLE(const FIELDDOUBLE & m) :
  FIELD<double>( (FIELD<double> &) m)
{
  BEGIN_OF("Copy constructor (for Python API) FIELDDOUBLE");

  END_OF("Copy constructor (for Python API) FIELDDOUBLE");
}
//=============================================================================
/*!
 * Copy constructor from a FIELD<double>
 */
//=============================================================================
FIELDDOUBLE::FIELDDOUBLE(const FIELD<double> & m) :
  FIELD<double>( m)
{
  BEGIN_OF("Copy constructor (for Python API) FIELDDOUBLE from a FIELD<double>");

  END_OF("Copy constructor (for Python API) FIELDDOUBLE from a FIELD<double>");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
FIELDDOUBLE::~FIELDDOUBLE()
{
  BEGIN_OF("Default Destructor (for Python API) FIELDDOUBLE");

  END_OF("Default Destructor (for Python API) FIELDDOUBLE");
}
