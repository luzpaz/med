#ifndef MEDMEM_SWIG_FIELDINT_HXX_
#define MEDMEM_SWIG_FIELDINT_HXX_

#include "MEDMEM_Field.hxx"

class FIELDINT : public MEDMEM::FIELD<int>
{
public:
  FIELDINT();
  FIELDINT(const MEDMEM::SUPPORT * Support, const int NumberOfComponents);
  FIELDINT(const MEDMEM::SUPPORT * Support, MEDMEM::driverTypes driverType,
	   const string & fileName="", const string & fieldName="",
	   const int iterationNumber=-1, const int orderNumber=-1);
  FIELDINT(const FIELDINT & m);
  FIELDINT(const MEDMEM::FIELD<int> & m);
  ~FIELDINT();
};

#endif /* MEDMEM_SWIG_FIELDINT_HXX_ */
