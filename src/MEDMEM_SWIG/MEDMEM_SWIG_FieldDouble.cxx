#include "MEDMEM_SWIG_FieldDouble.hxx"

//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
FIELDDOUBLE::FIELDDOUBLE() : FIELD<double>()
{
  BEGIN_OF("Default Constructor (for Python API) FIELDDOUBLE");

  END_OF("Default Constructor (for Python API) FIELDDOUBLE");
}
//=============================================================================
/*!
 * Constructor with arguments
 */
//=============================================================================
FIELDDOUBLE::FIELDDOUBLE(const SUPPORT * Support, const int NumberOfComponents) : FIELD<double>(Support,NumberOfComponents)
{
  BEGIN_OF("Constructor with arguments (for Python API) FIELDDOUBLE");

  med_type_champ type = MED_REEL64;

  setValueType(type);

  END_OF("Constructor with arguments (for Python API) FIELDDOUBLE");
}
/*
FIELDDOUBLE::FIELDDOUBLE(FIELD_ *field) 
  : FIELD<double> ( * (FIELD<double> *)(field))
{
  BEGIN_OF("Constructor (for Python API) FIELDDOUBLE avec parametre");

  SCRUTE(_value);

  END_OF("Constructor(for Python API) FIELDDOUBLE avec parametre");
}
*/
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
FIELDDOUBLE::~FIELDDOUBLE()
{
  BEGIN_OF("Default Destructor (for Python API) FIELDDOUBLE");

  END_OF("Default Destructor (for Python API) FIELDDOUBLE");
}
