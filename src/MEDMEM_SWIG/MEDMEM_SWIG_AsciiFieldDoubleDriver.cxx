#include "MEDMEM_SWIG_AsciiFieldDoubleDriver.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

ASCII_FIELDDOUBLE_DRIVER::ASCII_FIELDDOUBLE_DRIVER(const string & fileName, FIELDDOUBLE * ptrField, 
					       MED_EN::med_sort_direc direction, const char *priority):
  ASCII_FIELD_DRIVER<double>(fileName,(FIELD<double> *) ptrField,direction,priority)
{
  BEGIN_OF("Constructor with arguments (for Python API) ASCII_FIELDDOUBLE_DRIVER");

  END_OF("Constructor with arguments (for Python API) ASCII_FIELDDOUBLE_DRIVER");
}

ASCII_FIELDDOUBLE_DRIVER::~ASCII_FIELDDOUBLE_DRIVER()
{
  BEGIN_OF("Default Destructor (for Python API) ASCII_FIELDDOUBLE_DRIVER");

  END_OF("Default Destructor (for Python API) ASCII_FIELDDOUBLE_DRIVER");
}
