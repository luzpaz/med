//  MED MedMem : MED idl descriptions implementation based on the classes of MEDMEM
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : FieldInt_i.hxx
//  Author : EDF 
//  Module : MED
//  $Header: /export/home/CVS/SALOME_ROOT/MED/src/MedMem/FieldInt_i.hxx

#ifndef MED_FIELDINT_I_HXX_
#define MED_FIELDINT_I_HXX_

#include <SALOMEconfig.h> 

#include CORBA_SERVER_HEADER(MED)

#include "Field_i.hxx"
#include "FieldOf_i.hxx"

#include "MEDMEM_Field.hxx"

class FIELDINT_i: public FIELDOF_i<int>
{
private:
    FIELDINT_i();

public:
    ~FIELDINT_i();
    FIELDINT_i(SALOME_MED::SUPPORT_ptr mySupportIOR, ::FIELD<int> * const f);
    FIELDINT_i(FIELDINT_i & f);
    Engines::long_array * getValue (SALOME_MED::medModeSwitch mode ) 
				   throw (SALOME::SALOME_Exception);
};
#endif /* MED_FIELDINT_I_HXX_ */
