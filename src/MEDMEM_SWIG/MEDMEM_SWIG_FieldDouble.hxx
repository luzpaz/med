#ifndef MEDMEM_SWIG_FIELDDOUBLE_HXX_
#define MEDMEM_SWIG_FIELDDOUBLE_HXX_

#include "MEDMEM_Field.hxx"

class FIELDDOUBLE : public FIELD<double>
{
public:
  FIELDDOUBLE();
  FIELDDOUBLE(const SUPPORT * Support, const int NumberOfComponents);
  FIELDDOUBLE(const SUPPORT * Support, driverTypes driverType,
	      const string & fileName="", const string & fieldName="",
	      const int iterationNumber=-1, const int orderNumber=-1);
  FIELDDOUBLE(const FIELDDOUBLE & m);
  FIELDDOUBLE(const FIELD<double> & m);
  ~FIELDDOUBLE();
};

#endif /* MEDMEM_SWIG_FIELDDOUBLE_HXX_ */
