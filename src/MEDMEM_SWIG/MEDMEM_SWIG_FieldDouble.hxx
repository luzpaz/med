#ifndef MEDMEM_SWIG_FIELDDOUBLE_HXX_
#define MEDMEM_SWIG_FIELDDOUBLE_HXX_

#include "MEDMEM_Field.hxx"

class FIELDDOUBLE : public FIELD<double>
{
public:
  FIELDDOUBLE();
  FIELDDOUBLE(const SUPPORT * Support, const int NumberOfComponents);
  ~FIELDDOUBLE();
};

#endif /* MEDMEM_SWIG_FIELDDOUBLE_HXX_ */
