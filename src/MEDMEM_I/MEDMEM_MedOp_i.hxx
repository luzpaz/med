//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

//=============================================================================
// File      : MEDMEM_MedOp_i.hxx
// Project   : SALOME
// Author    : EDF
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_Med_i.hxx
//=============================================================================
//
#ifndef _MED_MEDOP_I_HXX_
#define _MED_MEDOP_I_HXX_

#include <MEDMEM_I.hxx>
#include "MEDMEM_Med.hxx"

#include <string>

#include <SALOMEconfig.h>

#include "SALOME_GenericObj_i.hh"
#include CORBA_SERVER_HEADER(MEDOP)

namespace MEDMEM {

class MEDMEM_I_EXPORT MEDOP_i: public POA_SALOME_MED::MEDOP,
             public SALOME::GenericObj_i
{

private :
  MED * _med;

protected:
  
public:
  // Constructors and associated internal methods
  MEDOP_i(MEDMEM::MED * med);
  ~MEDOP_i();

  CORBA::Long test();

};
}

#endif /* _MED_MED_I_HXX_ */
