//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

//=============================================================================
// File      : MEDMEM_MedOp_i.hxx
// Project   : SALOME
// Author    : EDF
//=============================================================================
//
#ifndef _MED_MEDOP_I_HXX_
#define _MED_MEDOP_I_HXX_

#include <MEDMEM_I.hxx>
#include "MEDMEM_Med.hxx"
#include "MEDMEM_MedDataManager.hxx"
#include "MEDMEM_Field.hxx"

#include <string>
#include <ext/hash_map>
using namespace __gnu_cxx;

#include <SALOMEconfig.h>

#include "SALOME_GenericObj_i.hh"
#include CORBA_SERVER_HEADER(MEDOP)
#include CORBA_SERVER_HEADER(MED)

/*! This map is used for lifecycle management of fields used in this operator */
typedef hash_map<const char *,FIELD<double>*> FIELDDOUBLE_HashMap;

namespace MEDMEM {

class MEDMEM_I_EXPORT MEDOP_i: public POA_SALOME_MED::MEDOP,
             public SALOME::GenericObj_i
{

private :
  MEDMEM::MED * _med;
  MedDataManager * _medDataManager;
  FIELDDOUBLE_HashMap _fielddouble_map;

  MEDMEM::FIELD<double> * _getFieldDouble(SALOME_MED::FIELD_ptr field_ptr);
  const string _getKeyIdentifier(MEDMEM::FIELD_ * field);

  

protected:
  
public:
  // Constructors and associated internal methods
  MEDOP_i(MEDMEM::MED * med);
  ~MEDOP_i();

  CORBA::Long test();

  SALOME_MED::FIELD_ptr addition(SALOME_MED::FIELD_ptr f1_ptr, SALOME_MED::FIELD_ptr f2_ptr);
  SALOME_MED::FIELD_ptr substraction(SALOME_MED::FIELD_ptr f1_ptr, SALOME_MED::FIELD_ptr f2_ptr);
  SALOME_MED::FIELD_ptr multiplication(SALOME_MED::FIELD_ptr f1_ptr, SALOME_MED::FIELD_ptr f2_ptr);
  SALOME_MED::FIELD_ptr division(SALOME_MED::FIELD_ptr f1_ptr, SALOME_MED::FIELD_ptr f2_ptr);
  SALOME_MED::FIELD_ptr pow(SALOME_MED::FIELD_ptr f1_ptr, long power);
  SALOME_MED::FIELD_ptr lin(SALOME_MED::FIELD_ptr f1_ptr, double factor, double offset);
};
}

#endif /* _MED_MED_I_HXX_ */
