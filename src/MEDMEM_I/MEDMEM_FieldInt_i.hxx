//=============================================================================
// File      : MEDMEM_FieldInt_i.hxx
// Project   : SALOME
// Author    : EDF 
// Copyright : EDF 2002
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_FieldInt_i.hxx
//=============================================================================

#ifndef MED_FIELDINT_I_HXX_
#define MED_FIELDINT_I_HXX_

#include <SALOMEconfig.h> 

#include CORBA_SERVER_HEADER(MED)

#include "MEDMEM_Field_i.hxx"
#include "MEDMEM_FieldOf_i.hxx"

#include "MEDMEM_Field.hxx"

namespace MEDMEM {
class FIELDINT_i: public FIELDOF_i<int>
{
private:
    FIELDINT_i();

public:
    ~FIELDINT_i();
    FIELDINT_i(SALOME_MED::SUPPORT_ptr mySupportIOR, ::FIELD<int> * const f);
    FIELDINT_i(FIELDINT_i & f);
    SALOME_MED::long_array * getValue (SALOME_MED::medModeSwitch mode ) 
				   throw (SALOME::SALOME_Exception);
};
}
#endif /* MED_FIELDINT_I_HXX_ */
