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
			 const string & fileName,
			 const string & fieldName) :
  FIELD<double>(Support, driverType, fileName, fieldName)
{
  BEGIN_OF("Another constructor with arguments (for Python API) FIELDDOUBLE");

  END_OF("Another constructor with arguments (for Python API) FIELDDOUBLE");
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
