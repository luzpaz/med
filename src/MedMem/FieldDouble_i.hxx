//=============================================================================
// File      : Fielddouble_i.hxx
// Project   : SALOME
// Author    : EDF 
// Copyright : EDF 2002
// $Header: /export/home/CVS/SALOME_ROOT/MED/src/MedMem/Fielddouble_i.hxx
//=============================================================================

#ifndef MED_FIELDDOUBLE_I_HXX_
#define MED_FIELDDOUBLE_I_HXX_

#include <SALOMEconfig.h> 

#include  CORBA_SERVER_HEADER(MED)

#include "Field_i.hxx"
#include "FieldOf_i.hxx"

#include "MEDMEM_Field.hxx"

class FIELDDOUBLE_i: public FIELDOF_i<double>
{
private:

public:
    FIELDDOUBLE_i();
    ~FIELDDOUBLE_i();
    FIELDDOUBLE_i(SALOME_MED::SUPPORT_ptr mySupportIOR,::FIELD<double> * const f);
    FIELDDOUBLE_i(FIELDDOUBLE_i & f);

    Engines::double_array * getValue (SALOME_MED::medModeSwitch mode ) 
				     throw (SALOME::SALOME_Exception);
};


#endif /* MED_FIELDDOUBLE_I_HXX_ */
