#include "MEDMEM_SWIG_AsciiFieldIntDriver.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

ASCII_FIELDINT_DRIVER::ASCII_FIELDINT_DRIVER(const string & fileName, FIELDINT * ptrField, 
					       MED_EN::med_sort_direc direction, const char *priority):
  ASCII_FIELD_DRIVER<int>(fileName,(FIELD<int> *) ptrField,direction,priority)
{
  BEGIN_OF("Constructor with arguments (for Python API) ASCII_FIELDINT_DRIVER");

  END_OF("Constructor with arguments (for Python API) ASCII_FIELDINT_DRIVER");
}

ASCII_FIELDINT_DRIVER::~ASCII_FIELDINT_DRIVER()
{
  BEGIN_OF("Default Destructor (for Python API) ASCII_FIELDINT_DRIVER");

  END_OF("Default Destructor (for Python API) ASCII_FIELDINT_DRIVER");
}
