#ifndef MEDMEM_SWIG_FIELDDOUBLE_HXX_
#define MEDMEM_SWIG_FIELDDOUBLE_HXX_

#include "MEDMEM_Field.hxx"

class FIELDDOUBLE : public MEDMEM::FIELD<double>
{
public:
  FIELDDOUBLE();
  FIELDDOUBLE(const MEDMEM::SUPPORT * Support, const int NumberOfComponents);
  FIELDDOUBLE(const MEDMEM::SUPPORT * Support, MEDMEM::driverTypes driverType,
	      const string & fileName="", const string & fieldName="",
	      const int iterationNumber=-1, const int orderNumber=-1);
  FIELDDOUBLE(const FIELDDOUBLE & m);
  FIELDDOUBLE(const MEDMEM::FIELD<double> & m);
  ~FIELDDOUBLE();
};

#endif /* MEDMEM_SWIG_FIELDDOUBLE_HXX_ */
