#ifndef MEDMEM_SWIG_FIELDINT_HXX_
#define MEDMEM_SWIG_FIELDINT_HXX_

#include "MEDMEM_Field.hxx"

//CCRTclass FIELDINT : public FIELD<int>
class FIELDINT : public FIELD<med_int>
{
public:
  FIELDINT();
  FIELDINT(const SUPPORT * Support, const int NumberOfComponents);
  FIELDINT(const SUPPORT * Support, driverTypes driverType,
	   const string & fileName="", const string & fieldName="",
	   const int iterationNumber=-1, const int orderNumber=-1);
  FIELDINT(const FIELDINT & m);
//CCRT  FIELDINT(const FIELD<int> & m);
  FIELDINT(const FIELD<med_int> & m);
  ~FIELDINT();
};

#endif /* MEDMEM_SWIG_FIELDINT_HXX_ */
