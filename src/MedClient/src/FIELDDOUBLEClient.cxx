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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "FIELDDOUBLEClient.hxx"
namespace MEDMEM{

//=============================================================================
/*!
 * Constructor with arguments
 */
//=============================================================================
FIELDDOUBLEClient::FIELDDOUBLEClient(SALOME_MED::FIELDDOUBLE_ptr ptrCorba,
				     MEDMEM::SUPPORT * S) :
  FIELDClient<double>(ptrCorba,S)
{
  BEGIN_OF("Constructor with arguments (for Python API) FIELDDOUBLEClient");

  END_OF("Constructor with arguments (for Python API) FIELDDOUBLEClient");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
FIELDDOUBLEClient::~FIELDDOUBLEClient()
{
  BEGIN_OF("Default Destructor (for Python API) FIELDDOUBLEClient");

  END_OF("Default Destructor (for Python API) FIELDDOUBLEClient");
}

}
