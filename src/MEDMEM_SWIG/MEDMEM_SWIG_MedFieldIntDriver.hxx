#ifndef MEDMEM_SWIG_MEDFIELDINTDRIVER_HXX_
#define MEDMEM_SWIG_MEDFIELDINTDRIVER_HXX_

#include "MEDMEM_MedFieldDriver.hxx"
#include "MEDMEM_SWIG_FieldInt.hxx"

class MED_FIELDINT_DRIVER : public MED_FIELD_DRIVER<int>
{
public:
  MED_FIELDINT_DRIVER();

  MED_FIELDINT_DRIVER(const string & fileName, FIELDINT * ptrField, 
		      med_mode_acces accessMode);

  ~MED_FIELDINT_DRIVER();
};

class MED_FIELDINT_RDONLY_DRIVER : public MED_FIELD_RDONLY_DRIVER<int>
{
public:
  MED_FIELDINT_RDONLY_DRIVER();

  MED_FIELDINT_RDONLY_DRIVER(const string & fileName,
			     FIELDINT * ptrField);

  ~MED_FIELDINT_RDONLY_DRIVER();
};

class MED_FIELDINT_WRONLY_DRIVER : public MED_FIELD_WRONLY_DRIVER<int>
{
public:
  MED_FIELDINT_WRONLY_DRIVER();

  MED_FIELDINT_WRONLY_DRIVER(const string & fileName,
			     FIELDINT * ptrField);

  ~MED_FIELDINT_WRONLY_DRIVER();
};

class MED_FIELDINT_RDWR_DRIVER : public MED_FIELD_RDWR_DRIVER<int>
{
public:
  MED_FIELDINT_RDWR_DRIVER();

  MED_FIELDINT_RDWR_DRIVER(const string & fileName,
			   FIELDINT * ptrField);

  ~MED_FIELDINT_RDWR_DRIVER();
};

#endif /* MEDMEM_SWIG_MEDFIELDINTDRIVER_HXX_ */
