#include "MEDMEM_SWIG_FieldInt.hxx"

//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
FIELDINT::FIELDINT() : FIELD<int>()
{
        BEGIN_OF("Default Constructor (for Python API) FIELDINT");

        END_OF("Default Constructor (for Python API) FIELDINT");
}
//=============================================================================
/*!
 * Constructor with arguments
 */
//=============================================================================
FIELDINT::FIELDINT(const SUPPORT * Support, const int NumberOfComponents) : FIELD<int>(Support,NumberOfComponents)
{
  BEGIN_OF("Constructor with arguments (for Python API) FIELDINT");

  med_type_champ type = MED_INT32;

  setValueType(type);

  END_OF("Constructor with arguments (for Python API) FIELDINT");
}
/*
FIELDINT::FIELDINT(FIELD_ * field)
  : FIELD<int> ( * (FIELD<int> *) (field))
{
  BEGIN_OF("Constructor (for Python API) FIELDINT avec parametre");

  END_OF("Constructor (for Python API) FIELDINT avec parametre");
}
*/
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
FIELDINT::~FIELDINT()
{
  BEGIN_OF("Default Destructor (for Python API) FIELDINT");

  END_OF("Default Destructor (for Python API) FIELDINT");
}
