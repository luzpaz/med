// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
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

#include "MEDMEM_Field.hxx"

namespace MEDMEM {
  class FIELDDOUBLE_i: public POA_SALOME_MED::FIELDDOUBLE,
		       public FIELD_i,
		       public SALOMEMultiComm
{
private:

public:
    FIELDDOUBLE_i();
    ~FIELDDOUBLE_i();
    FIELDDOUBLE_i(MEDMEM::FIELD<double> * const f, bool ownCppPtr=false);
    FIELDDOUBLE_i(FIELDDOUBLE_i & f);

    SALOME_MED::double_array * getValue (SALOME_MED::medModeSwitch mode ) 
				     throw (SALOME::SALOME_Exception);
    SALOME::SenderDouble_ptr getSenderForValue(SALOME_MED::medModeSwitch mode)
                                     throw (SALOME::SALOME_Exception);
};
}


#endif /* MED_FIELDDOUBLE_I_HXX_ */
