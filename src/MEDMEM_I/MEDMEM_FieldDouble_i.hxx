//=============================================================================
// File      : MEDMEM_Fielddouble_i.hxx
// Project   : SALOME
// Author    : EDF 
// Copyright : EDF 2002
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_Fielddouble_i.hxx
//=============================================================================

#ifndef MED_FIELDDOUBLE_I_HXX_
#define MED_FIELDDOUBLE_I_HXX_

#include <SALOMEconfig.h> 

#include  CORBA_SERVER_HEADER(MED)

#include "MEDMEM_Field_i.hxx"
#include "MEDMEM_FieldOf_i.hxx"

#include "MEDMEM_Field.hxx"

namespace MEDMEM {
class FIELDDOUBLE_i: public FIELDOF_i<double>
{
private:

public:
    FIELDDOUBLE_i();
    ~FIELDDOUBLE_i();
    FIELDDOUBLE_i(SALOME_MED::SUPPORT_ptr mySupportIOR,::FIELD<double> * const f);
    FIELDDOUBLE_i(FIELDDOUBLE_i & f);

    SALOME_MED::double_array * getValue (SALOME_MED::medModeSwitch mode ) 
				     throw (SALOME::SALOME_Exception);
};
}


#endif /* MED_FIELDDOUBLE_I_HXX_ */
