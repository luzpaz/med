#ifndef MEDMEM_SWIG_FIELDINT_HXX_
#define MEDMEM_SWIG_FIELDINT_HXX_

#include "MEDMEM_Field.hxx"

class FIELDINT : public FIELD<int>
{
public:
  FIELDINT();
  FIELDINT(const SUPPORT * Support, const int NumberOfComponents);
  FIELDINT(const SUPPORT * Support, driverTypes driverType,
	   const string & fileName="", const string & fieldName="");
  ~FIELDINT();
};

#endif /* MEDMEM_SWIG_FIELDINT_HXX_ */
