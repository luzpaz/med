using namespace std;
#include "MEDMEM_SWIG_MedFieldIntDriver.hxx"

//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
MED_FIELDINT_DRIVER::MED_FIELDINT_DRIVER() : MED_FIELD_DRIVER<int>()
{
  BEGIN_OF("Default Constructor (for Python API) MED_FIELDINT_DRIVER");

  END_OF("Default Constructor (for Python API) MED_FIELDINT_DRIVER");
}
//=============================================================================
/*!
 * Constructor with arguments
 */
//=============================================================================
MED_FIELDINT_DRIVER::MED_FIELDINT_DRIVER(const string & fileName,
					 FIELDINT * ptrField, 
					 med_mode_acces accessMode) :
  MED_FIELD_DRIVER<int>(fileName,(FIELD<int> *) ptrField,accessMode)
{
  BEGIN_OF("Constructor with arguments (for Python API) MED_FIELDINT_DRIVER");

  END_OF("Constructor with arguments (for Python API) MED_FIELDINT_DRIVER");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
MED_FIELDINT_DRIVER::~MED_FIELDINT_DRIVER()
{
  BEGIN_OF("Default Destructor (for Python API) MED_FIELDINT_DRIVER");

  END_OF("Default Destructor (for Python API) MED_FIELDINT_DRIVER");
}
//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
MED_FIELDINT_RDONLY_DRIVER::MED_FIELDINT_RDONLY_DRIVER() :
  MED_FIELD_RDONLY_DRIVER<int>()
{
  BEGIN_OF("Default Constructor (for Python API) MED_FIELDINT_RDONLY_DRIVER");

  END_OF("Default Constructor (for Python API) MED_FIELDINT_RDONLY_DRIVER");
}
//=============================================================================
/*!
 * Constructor with arguments
 */
//=============================================================================
MED_FIELDINT_RDONLY_DRIVER::MED_FIELDINT_RDONLY_DRIVER(const string & fileName,
							     FIELDINT * ptrField) : 
  MED_FIELD_RDONLY_DRIVER<int>(fileName,(FIELD<int> *) ptrField)
{
  BEGIN_OF("Constructor with arguments (for Python API) MED_FIELDINT_RDONLY_DRIVER");

  END_OF("Constructor with arguments (for Python API) MED_FIELDINT_RDONLY_DRIVER");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
MED_FIELDINT_RDONLY_DRIVER::~MED_FIELDINT_RDONLY_DRIVER()
{
  BEGIN_OF("Default Destructor (for Python API) MED_FIELDINT_RDONLY_DRIVER");

  END_OF("Default Destructor (for Python API) MED_FIELDINT_RDONLY_DRIVER");
}
//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
MED_FIELDINT_WRONLY_DRIVER::MED_FIELDINT_WRONLY_DRIVER() :
  MED_FIELD_WRONLY_DRIVER<int>()
{
  BEGIN_OF("Default Constructor (for Python API) MED_FIELDINT_WRONLY_DRIVER");

  END_OF("Default Constructor (for Python API) MED_FIELDINT_WRONLY_DRIVER");
}
//=============================================================================
/*!
 * Constructor with arguments
 */
//=============================================================================
MED_FIELDINT_WRONLY_DRIVER::MED_FIELDINT_WRONLY_DRIVER(const string & fileName,
							     FIELDINT * ptrField) : 
  MED_FIELD_WRONLY_DRIVER<int>(fileName,(FIELD<int> *) ptrField)
{
  BEGIN_OF("Constructor with arguments (for Python API) MED_FIELDINT_WRONLY_DRIVER");

  END_OF("Constructor with arguments (for Python API) MED_FIELDINT_WRONLY_DRIVER");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
MED_FIELDINT_WRONLY_DRIVER::~MED_FIELDINT_WRONLY_DRIVER()
{
  BEGIN_OF("Default Destructor (for Python API) MED_FIELDINT_WRONLY_DRIVER");

  END_OF("Default Destructor (for Python API) MED_FIELDINT_WRONLY_DRIVER");
}
//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
MED_FIELDINT_RDWR_DRIVER::MED_FIELDINT_RDWR_DRIVER() :
  MED_FIELD_RDWR_DRIVER<int>()
{
  BEGIN_OF("Default Constructor (for Python API) MED_FIELDINT_RDWR_DRIVER");

  END_OF("Default Constructor (for Python API) MED_FIELDINT_RDWR_DRIVER");
}
//=============================================================================
/*!
 * Constructor with arguments
 */
//=============================================================================
MED_FIELDINT_RDWR_DRIVER::MED_FIELDINT_RDWR_DRIVER(const string & fileName,
							 FIELDINT * ptrField) : 
  MED_FIELD_RDWR_DRIVER<int>(fileName,(FIELD<int> *) ptrField)
{
  BEGIN_OF("Constructor with arguments (for Python API) MED_FIELDINT_RDWR_DRIVER");

  END_OF("Constructor with arguments (for Python API) MED_FIELDINT_RDWR_DRIVER");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
MED_FIELDINT_RDWR_DRIVER::~MED_FIELDINT_RDWR_DRIVER()
{
  BEGIN_OF("Default Destructor (for Python API) MED_FIELDINT_RDWR_DRIVER");

  END_OF("Default Destructor (for Python API) MED_FIELDINT_RDWR_DRIVER");
}
