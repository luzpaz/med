#ifndef MEDMEM_SWIG_FIELDINT_HXX_
#define MEDMEM_SWIG_FIELDINT_HXX_

#include "MEDMEM_Field.hxx"

class FIELDINT : public FIELD<int>
{
public:
  FIELDINT();
  FIELDINT(const SUPPORT * Support, const int NumberOfComponents);
  FIELDINT(const SUPPORT * Support, driverTypes driverType,
	   const string & fileName="", const string & fieldName="",
	   const int iterationNumber=-1, const int orderNumber=-1);
  FIELDINT(const FIELDINT & m);
  FIELDINT(const FIELD<int> & m);
  ~FIELDINT();
};

#endif /* MEDMEM_SWIG_FIELDINT_HXX_ */
