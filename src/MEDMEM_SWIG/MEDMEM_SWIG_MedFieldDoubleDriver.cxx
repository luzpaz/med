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
//  File   : MEDMEM_SWIG_MedFieldDoubleDriver.cxx
//  Module : MED

using namespace std;
#include "MEDMEM_SWIG_MedFieldDoubleDriver.hxx"

//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
MED_FIELDDOUBLE_DRIVER::MED_FIELDDOUBLE_DRIVER() : MED_FIELD_DRIVER<double>()
{
  BEGIN_OF("Default Constructor (for Python API) MED_FIELDDOUBLE_DRIVER");

  END_OF("Default Constructor (for Python API) MED_FIELDDOUBLE_DRIVER");
}
//=============================================================================
/*!
 * Constructor with arguments
 */
//=============================================================================
MED_FIELDDOUBLE_DRIVER::MED_FIELDDOUBLE_DRIVER(const string & fileName,
					       FIELDDOUBLE * ptrField, 
					       med_mode_acces accessMode) :
  MED_FIELD_DRIVER<double>(fileName,(FIELD<double> *) ptrField,accessMode)
{
  BEGIN_OF("Constructor with arguments (for Python API) MED_FIELDDOUBLE_DRIVER");

  END_OF("Constructor with arguments (for Python API) MED_FIELDDOUBLE_DRIVER");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
MED_FIELDDOUBLE_DRIVER::~MED_FIELDDOUBLE_DRIVER()
{
  BEGIN_OF("Default Destructor (for Python API) MED_FIELDDOUBLE_DRIVER");

  END_OF("Default Destructor (for Python API) MED_FIELDDOUBLE_DRIVER");
}
//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
MED_FIELDDOUBLE_RDONLY_DRIVER::MED_FIELDDOUBLE_RDONLY_DRIVER() :
  MED_FIELD_RDONLY_DRIVER<double>()
{
  BEGIN_OF("Default Constructor (for Python API) MED_FIELDDOUBLE_RDONLY_DRIVER");

  END_OF("Default Constructor (for Python API) MED_FIELDDOUBLE_RDONLY_DRIVER");
}
//=============================================================================
/*!
 * Constructor with arguments
 */
//=============================================================================
MED_FIELDDOUBLE_RDONLY_DRIVER::MED_FIELDDOUBLE_RDONLY_DRIVER(const string & fileName,
							     FIELDDOUBLE * ptrField) : 
  MED_FIELD_RDONLY_DRIVER<double>(fileName,(FIELD<double> *) ptrField)
{
  BEGIN_OF("Constructor with arguments (for Python API) MED_FIELDDOUBLE_RDONLY_DRIVER");

  END_OF("Constructor with arguments (for Python API) MED_FIELDDOUBLE_RDONLY_DRIVER");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
MED_FIELDDOUBLE_RDONLY_DRIVER::~MED_FIELDDOUBLE_RDONLY_DRIVER()
{
  BEGIN_OF("Default Destructor (for Python API) MED_FIELDDOUBLE_RDONLY_DRIVER");

  END_OF("Default Destructor (for Python API) MED_FIELDDOUBLE_RDONLY_DRIVER");
}
//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
MED_FIELDDOUBLE_WRONLY_DRIVER::MED_FIELDDOUBLE_WRONLY_DRIVER() :
  MED_FIELD_WRONLY_DRIVER<double>()
{
  BEGIN_OF("Default Constructor (for Python API) MED_FIELDDOUBLE_WRONLY_DRIVER");

  END_OF("Default Constructor (for Python API) MED_FIELDDOUBLE_WRONLY_DRIVER");
}
//=============================================================================
/*!
 * Constructor with arguments
 */
//=============================================================================
MED_FIELDDOUBLE_WRONLY_DRIVER::MED_FIELDDOUBLE_WRONLY_DRIVER(const string & fileName,
							     FIELDDOUBLE * ptrField) : 
  MED_FIELD_WRONLY_DRIVER<double>(fileName,(FIELD<double> *) ptrField)
{
  BEGIN_OF("Constructor with arguments (for Python API) MED_FIELDDOUBLE_WRONLY_DRIVER");

  END_OF("Constructor with arguments (for Python API) MED_FIELDDOUBLE_WRONLY_DRIVER");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
MED_FIELDDOUBLE_WRONLY_DRIVER::~MED_FIELDDOUBLE_WRONLY_DRIVER()
{
  BEGIN_OF("Default Destructor (for Python API) MED_FIELDDOUBLE_WRONLY_DRIVER");

  END_OF("Default Destructor (for Python API) MED_FIELDDOUBLE_WRONLY_DRIVER");
}
//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
MED_FIELDDOUBLE_RDWR_DRIVER::MED_FIELDDOUBLE_RDWR_DRIVER() :
  MED_FIELD_RDWR_DRIVER<double>()
{
  BEGIN_OF("Default Constructor (for Python API) MED_FIELDDOUBLE_RDWR_DRIVER");

  END_OF("Default Constructor (for Python API) MED_FIELDDOUBLE_RDWR_DRIVER");
}
//=============================================================================
/*!
 * Constructor with arguments
 */
//=============================================================================
MED_FIELDDOUBLE_RDWR_DRIVER::MED_FIELDDOUBLE_RDWR_DRIVER(const string & fileName,
							 FIELDDOUBLE * ptrField) : 
  MED_FIELD_RDWR_DRIVER<double>(fileName,(FIELD<double> *) ptrField)
{
  BEGIN_OF("Constructor with arguments (for Python API) MED_FIELDDOUBLE_RDWR_DRIVER");

  END_OF("Constructor with arguments (for Python API) MED_FIELDDOUBLE_RDWR_DRIVER");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
MED_FIELDDOUBLE_RDWR_DRIVER::~MED_FIELDDOUBLE_RDWR_DRIVER()
{
  BEGIN_OF("Default Destructor (for Python API) MED_FIELDDOUBLE_RDWR_DRIVER");

  END_OF("Default Destructor (for Python API) MED_FIELDDOUBLE_RDWR_DRIVER");
}
