//  MED MEDMEM_SWIG : binding of C++ implementation and Python
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
//  File   : MEDMEM_SWIG_FieldInt.cxx
//  Module : MED

using namespace std;
#include "MEDMEM_SWIG_FieldInt.hxx"

//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
FIELDINT::FIELDINT() : FIELD<int>()
{
        BEGIN_OF("Default Constructor (for Python API) FIELDINT");

        END_OF("Default Constructor (for Python API) FIELDINT");
}
//=============================================================================
/*!
 * Constructor with arguments
 */
//=============================================================================
FIELDINT::FIELDINT(const SUPPORT * Support, const int NumberOfComponents) : FIELD<int>(Support,NumberOfComponents)
{
  BEGIN_OF("Constructor with arguments (for Python API) FIELDINT");

  med_type_champ type = MED_INT32;

  setValueType(type);

  END_OF("Constructor with arguments (for Python API) FIELDINT");
}
//=============================================================================
/*!
 * Another constructor with arguments
 */
//=============================================================================
FIELDINT::FIELDINT(const SUPPORT * Support, driverTypes driverType,
		   const string & fileName,
		   const string & fieldName) :
  FIELD<int>(Support, driverType, fileName, fieldName)
{
  BEGIN_OF("Another constructor with arguments (for Python API) FIELDINT");

  END_OF("Another constructor with arguments (for Python API) FIELDINT");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
FIELDINT::~FIELDINT()
{
  BEGIN_OF("Default Destructor (for Python API) FIELDINT");

  END_OF("Default Destructor (for Python API) FIELDINT");
}
