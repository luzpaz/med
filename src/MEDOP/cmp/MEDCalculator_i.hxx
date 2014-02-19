// Copyright (C) 2007-2014  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// Authors : Guillaume Boulant (EDF) - 01/06/2011

#ifndef _MED_CALCULATOR_I_HXX_
#define _MED_CALCULATOR_I_HXX_

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MEDDataManager)
#include CORBA_SERVER_HEADER(MEDCalculator)
#include "SALOME_GenericObj_i.hh"

#include "MEDDataManager_i.hxx"

#include "MEDOP.hxx"
class MEDOP_EXPORT MEDCalculator_i: public POA_MEDOP::MEDCalculator,
            public SALOME::GenericObj_i
{
public:

  // 
  // ===========================================================
  // Functions specified in the IDL interface
  // ===========================================================
  //
  MEDOP::FieldHandler * add(const MEDOP::FieldHandler & f1_hdl, const MEDOP::FieldHandler & f2_hdl);
  MEDOP::FieldHandler * sub(const MEDOP::FieldHandler & f1_hdl, const MEDOP::FieldHandler & f2_hdl);
  MEDOP::FieldHandler * mul(const MEDOP::FieldHandler & f1_hdl, const MEDOP::FieldHandler & f2_hdl);
  MEDOP::FieldHandler * div(const MEDOP::FieldHandler & f1_hdl, const MEDOP::FieldHandler & f2_hdl);
  MEDOP::FieldHandler * pow(const MEDOP::FieldHandler & f_hdl, CORBA::Long power);
  MEDOP::FieldHandler * lin(const MEDOP::FieldHandler & f_hdl, double factor, double offset);
  MEDOP::FieldHandler * dup(const MEDOP::FieldHandler & f_hdl);

  MEDOP::FieldHandler * fct(const MEDOP::FieldHandler & f_hdl,
          const char * function, CORBA::Long nbResComponents);

  // 
  // ===========================================================
  // Other public functions (non available via CORBA)
  // ===========================================================
  //
  static MEDCalculator_i * getInstance();

private:
  MEDCalculator_i();
  ~MEDCalculator_i();

  // The MEDCalculator is a singleton, whose instance can be obtained
  // using the getInstance static method.
  static MEDCalculator_i * _instance;

  MEDDataManager_i * _medDataManager;
};

#endif // _MED_CALCULATOR_I_HXX_
