#ifndef MEDMEM_SWIG_ASCIIFIELDDOUBLEDRIVER_HXX_
#define MEDMEM_SWIG_ASCIIDFIELDDOUBLEDRIVER_HXX_

#include "MEDMEM_AsciiFieldDriver.hxx"
#include "MEDMEM_SWIG_FieldDouble.hxx"

class ASCII_FIELDDOUBLE_DRIVER : public MEDMEM::ASCII_FIELD_DRIVER<double>
{
public:

  ASCII_FIELDDOUBLE_DRIVER(const string & fileName, FIELDDOUBLE * ptrField, 
			 MED_EN::med_sort_direc direction, const char *priority);

  ~ASCII_FIELDDOUBLE_DRIVER();
};

#endif 
