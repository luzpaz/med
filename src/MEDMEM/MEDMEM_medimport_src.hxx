// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#ifndef IMPORT_HXX
#define IMPORT_HXX

#include <MEDMEM.hxx>

#include "MEDimport.hxx"
  
#include "MEDMEM_Exception.hxx"

#undef MED_H

namespace MEDMEM {

/*!
  This method is supposed to be the same as the med File V2.3 medimport
  utility. fileNameIn is .med V2.1 file supplied by the user including
  all the directory path. fileNameOut is .med V2.3.3 file produced by this
  command (same name + 2.3).
*/

MEDMEM_EXPORT char * med2_1_To_med2_3(char * fileNameIn) throw (MEDMEM::MEDEXCEPTION) ;
}

#endif /* IMPORT_HXX */

