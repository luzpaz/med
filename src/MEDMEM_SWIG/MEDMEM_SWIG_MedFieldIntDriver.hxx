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
//  File   : MEDMEM_SWIG_MedFieldIntDriver.hxx
//  Module : MED

#ifndef MEDMEM_SWIG_MEDFIELDINTDRIVER_HXX_
#define MEDMEM_SWIG_MEDFIELDINTDRIVER_HXX_

#include "MEDMEM_MedFieldDriver.hxx"
#include "MEDMEM_SWIG_FieldInt.hxx"

class MED_FIELDINT_DRIVER : public MED_FIELD_DRIVER<int>
{
public:
  MED_FIELDINT_DRIVER();

  MED_FIELDINT_DRIVER(const string & fileName, FIELDINT * ptrField, 
		      med_mode_acces accessMode);

  ~MED_FIELDINT_DRIVER();
};

class MED_FIELDINT_RDONLY_DRIVER : public MED_FIELD_RDONLY_DRIVER<int>
{
public:
  MED_FIELDINT_RDONLY_DRIVER();

  MED_FIELDINT_RDONLY_DRIVER(const string & fileName,
			     FIELDINT * ptrField);

  ~MED_FIELDINT_RDONLY_DRIVER();
};

class MED_FIELDINT_WRONLY_DRIVER : public MED_FIELD_WRONLY_DRIVER<int>
{
public:
  MED_FIELDINT_WRONLY_DRIVER();

  MED_FIELDINT_WRONLY_DRIVER(const string & fileName,
			     FIELDINT * ptrField);

  ~MED_FIELDINT_WRONLY_DRIVER();
};

class MED_FIELDINT_RDWR_DRIVER : public MED_FIELD_RDWR_DRIVER<int>
{
public:
  MED_FIELDINT_RDWR_DRIVER();

  MED_FIELDINT_RDWR_DRIVER(const string & fileName,
			   FIELDINT * ptrField);

  ~MED_FIELDINT_RDWR_DRIVER();
};

#endif /* MEDMEM_SWIG_MEDFIELDINTDRIVER_HXX_ */
