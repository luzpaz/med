#include "MEDMEM_SWIG_MedFieldDoubleDriver.hxx"
using namespace std;

//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
MED_FIELDDOUBLE_DRIVER::MED_FIELDDOUBLE_DRIVER() : MED_FIELD_DRIVER<double>()
{
  BEGIN_OF("Default Constructor (for Python API) MED_FIELDDOUBLE_DRIVER");

  END_OF("Default Constructor (for Python API) MED_FIELDDOUBLE_DRIVER");
}
//=============================================================================
/*!
 * Constructor with arguments
 */
//=============================================================================
MED_FIELDDOUBLE_DRIVER::MED_FIELDDOUBLE_DRIVER(const string & fileName,
					       FIELDDOUBLE * ptrField, 
					       med_mode_acces accessMode) :
  MED_FIELD_DRIVER<double>(fileName,(FIELD<double> *) ptrField,accessMode)
{
  BEGIN_OF("Constructor with arguments (for Python API) MED_FIELDDOUBLE_DRIVER");

  END_OF("Constructor with arguments (for Python API) MED_FIELDDOUBLE_DRIVER");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
MED_FIELDDOUBLE_DRIVER::~MED_FIELDDOUBLE_DRIVER()
{
  BEGIN_OF("Default Destructor (for Python API) MED_FIELDDOUBLE_DRIVER");

  END_OF("Default Destructor (for Python API) MED_FIELDDOUBLE_DRIVER");
}
//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
MED_FIELDDOUBLE_RDONLY_DRIVER::MED_FIELDDOUBLE_RDONLY_DRIVER() :
  MED_FIELD_RDONLY_DRIVER<double>()
{
  BEGIN_OF("Default Constructor (for Python API) MED_FIELDDOUBLE_RDONLY_DRIVER");

  END_OF("Default Constructor (for Python API) MED_FIELDDOUBLE_RDONLY_DRIVER");
}
//=============================================================================
/*!
 * Constructor with arguments
 */
//=============================================================================
MED_FIELDDOUBLE_RDONLY_DRIVER::MED_FIELDDOUBLE_RDONLY_DRIVER(const string & fileName,
							     FIELDDOUBLE * ptrField) : 
  MED_FIELD_RDONLY_DRIVER<double>(fileName,(FIELD<double> *) ptrField)
{
  BEGIN_OF("Constructor with arguments (for Python API) MED_FIELDDOUBLE_RDONLY_DRIVER");

  END_OF("Constructor with arguments (for Python API) MED_FIELDDOUBLE_RDONLY_DRIVER");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
MED_FIELDDOUBLE_RDONLY_DRIVER::~MED_FIELDDOUBLE_RDONLY_DRIVER()
{
  BEGIN_OF("Default Destructor (for Python API) MED_FIELDDOUBLE_RDONLY_DRIVER");

  END_OF("Default Destructor (for Python API) MED_FIELDDOUBLE_RDONLY_DRIVER");
}
//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
MED_FIELDDOUBLE_WRONLY_DRIVER::MED_FIELDDOUBLE_WRONLY_DRIVER() :
  MED_FIELD_WRONLY_DRIVER<double>()
{
  BEGIN_OF("Default Constructor (for Python API) MED_FIELDDOUBLE_WRONLY_DRIVER");

  END_OF("Default Constructor (for Python API) MED_FIELDDOUBLE_WRONLY_DRIVER");
}
//=============================================================================
/*!
 * Constructor with arguments
 */
//=============================================================================
MED_FIELDDOUBLE_WRONLY_DRIVER::MED_FIELDDOUBLE_WRONLY_DRIVER(const string & fileName,
							     FIELDDOUBLE * ptrField) : 
  MED_FIELD_WRONLY_DRIVER<double>(fileName,(FIELD<double> *) ptrField)
{
  BEGIN_OF("Constructor with arguments (for Python API) MED_FIELDDOUBLE_WRONLY_DRIVER");

  END_OF("Constructor with arguments (for Python API) MED_FIELDDOUBLE_WRONLY_DRIVER");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
MED_FIELDDOUBLE_WRONLY_DRIVER::~MED_FIELDDOUBLE_WRONLY_DRIVER()
{
  BEGIN_OF("Default Destructor (for Python API) MED_FIELDDOUBLE_WRONLY_DRIVER");

  END_OF("Default Destructor (for Python API) MED_FIELDDOUBLE_WRONLY_DRIVER");
}
//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
MED_FIELDDOUBLE_RDWR_DRIVER::MED_FIELDDOUBLE_RDWR_DRIVER() :
  MED_FIELD_RDWR_DRIVER<double>()
{
  BEGIN_OF("Default Constructor (for Python API) MED_FIELDDOUBLE_RDWR_DRIVER");

  END_OF("Default Constructor (for Python API) MED_FIELDDOUBLE_RDWR_DRIVER");
}
//=============================================================================
/*!
 * Constructor with arguments
 */
//=============================================================================
MED_FIELDDOUBLE_RDWR_DRIVER::MED_FIELDDOUBLE_RDWR_DRIVER(const string & fileName,
							 FIELDDOUBLE * ptrField) : 
  MED_FIELD_RDWR_DRIVER<double>(fileName,(FIELD<double> *) ptrField)
{
  BEGIN_OF("Constructor with arguments (for Python API) MED_FIELDDOUBLE_RDWR_DRIVER");

  END_OF("Constructor with arguments (for Python API) MED_FIELDDOUBLE_RDWR_DRIVER");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
MED_FIELDDOUBLE_RDWR_DRIVER::~MED_FIELDDOUBLE_RDWR_DRIVER()
{
  BEGIN_OF("Default Destructor (for Python API) MED_FIELDDOUBLE_RDWR_DRIVER");

  END_OF("Default Destructor (for Python API) MED_FIELDDOUBLE_RDWR_DRIVER");
}
