#ifndef MEDMEM_SWIG_ASCIIFIELDINTDRIVER_HXX_
#define MEDMEM_SWIG_ASCIIFIELDINTDRIVER_HXX_

#include "MEDMEM_AsciiFieldDriver.hxx"
#include "MEDMEM_SWIG_FieldInt.hxx"

class ASCII_FIELDINT_DRIVER : public MEDMEM::ASCII_FIELD_DRIVER<int>
{
public:

  ASCII_FIELDINT_DRIVER(const string & fileName, FIELDINT * ptrField, 
			 MED_EN::med_sort_direc direction, const char *priority);

  ~ASCII_FIELDINT_DRIVER();
};

#endif 
