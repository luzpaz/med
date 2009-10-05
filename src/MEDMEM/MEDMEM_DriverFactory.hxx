//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
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
#ifndef DRIVERFACTORY_HXX
#define DRIVERFACTORY_HXX

#include <MEDMEM.hxx>

#include "MEDMEM_MedVersion.hxx"
#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_define.hxx"

#include "MEDMEM_FieldForward.hxx"

#include <string>

namespace MEDMEM {

  class MESH;
  class MED;
  class GENDRIVER;

  namespace DRIVERFACTORY {

    /*
      definition of static variable across the Med Memory of a Med File version
      for the writing of Med File set to V22
    */

    MEDMEM_EXPORT extern MED_EN::medFileVersion globalMedFileVersionForWriting;

    MEDMEM_EXPORT MED_EN::medFileVersion getMedFileVersionForWriting();

    MEDMEM_EXPORT void setMedFileVersionForWriting(MED_EN::medFileVersion version);

    MEDMEM_EXPORT driverTypes deduceDriverTypeFromFileName(const std::string & fileName);

    driverTypes deduceDriverTypeFromFileName(const std::string & fileName);

    MEDMEM_EXPORT GENDRIVER * buildDriverForMesh(driverTypes driverType,
						 const std::string & fileName,
						 MESH *mesh,const string &  driverName,
						 MED_EN::med_mode_acces access);

    template<class T, class INTERLACING_TAG>
    GENDRIVER * buildDriverForField(driverTypes driverType,
				    const std::string & fileName,
				    FIELD<T,INTERLACING_TAG> *fielde,
				    MED_EN::med_mode_acces access);
    MEDMEM_EXPORT GENDRIVER * buildDriverForMed(driverTypes driverType,
						const std::string & fileName,
						MED *mede,
						MED_EN::med_mode_acces access);
    MEDMEM_EXPORT GENDRIVER * buildMedDriverFromFile(const string & fileName,
						     MED * const ptrMed,
						     MED_EN::med_mode_acces access);
    MEDMEM_EXPORT GENDRIVER * buildMeshDriverFromFile(const string & fileName,
						      MESH * ptrMesh,
						      MED_EN::med_mode_acces access);
    template<class T, class INTERLACING_TAG>
    GENDRIVER * buildFieldDriverFromFile(const string & fileName,
					 FIELD<T,INTERLACING_TAG> * ptrField,
					 MED_EN::med_mode_acces access);
    MEDMEM_EXPORT GENDRIVER * buildConcreteMedDriverForMesh(const std::string & fileName,
							    MESH *mesh,
							    const string & driverName,
							    MED_EN::med_mode_acces access,
							    MED_EN::medFileVersion version);
    template<class T, class INTERLACING_TAG>
    GENDRIVER * buildConcreteMedDriverForField(const std::string & fileName,
					       FIELD<T,INTERLACING_TAG> *fielde,
					       MED_EN::med_mode_acces access,
					       MED_EN::medFileVersion version);
  }
}

#include "MEDMEM_VtkFieldDriver.hxx"
#include "MEDMEM_MedFieldDriver.hxx"
#include "MEDMEM_MedFieldDriver21.hxx"
#include "MEDMEM_MedFieldDriver22.hxx"
#include "MEDMEM_AsciiFieldDriver.hxx"
#include "MEDMEM_EnsightFieldDriver.hxx"

namespace MEDMEM {
  template<class T, class INTERLACING_TAG>
  GENDRIVER * DRIVERFACTORY::buildDriverForField(driverTypes driverType,
						 const std::string & fileName,
						 FIELD<T,INTERLACING_TAG> *field,
						 MED_EN::med_mode_acces access)
  {
    GENDRIVER *ret;
    switch(driverType)
      {
      case MED_DRIVER : {
	switch(access)
	  {
	  case MED_EN::RDONLY : {
	    ret = new MED_FIELD_RDONLY_DRIVER<T>(fileName,field);
	    break;
	  }
	  case MED_EN::WRONLY : {
	    ret= new MED_FIELD_WRONLY_DRIVER<T>(fileName,field);
	    break;
	  }
	  case MED_EN::RDWR : {
	    ret = new MED_FIELD_RDWR_DRIVER<T>(fileName,field);
	    break;
	  }
	  default:
	    throw MED_EXCEPTION ("access type has not been properly specified to the method");
	  }
	break;
      }

      case ENSIGHT_DRIVER : {
	switch(access)
	  {
	  case MED_EN::RDONLY : {
	    ret = new ENSIGHT_FIELD_RDONLY_DRIVER(fileName,field);
	    break;
	  }
	  case MED_EN::WRONLY : {
	    ret=new ENSIGHT_FIELD_WRONLY_DRIVER(fileName,field);
	    break;
	  }
	  case MED_EN::RDWR : {
	    throw MED_EXCEPTION ("not yet implemented");
	    break ;
	  }
	  default:
	    throw MED_EXCEPTION ("access type has not been properly specified to the method");
	  }
	break;
      }

      case VTK_DRIVER : {
	switch(access)
	  {
	  case MED_EN::RDONLY : {
	    throw MED_EXCEPTION ("access mode other than MED_ECRI and MED_REMP has been specified with the VTK_DRIVER type which is not allowed because VTK_DRIVER is only a write access driver");
	    break;
	  }
	  case MED_EN::WRONLY : {
	    ret=new VTK_FIELD_DRIVER<T>(fileName,field);
	    break;
	  }
	  case MED_EN::RDWR : {
	    ret=new VTK_FIELD_DRIVER<T>(fileName,field);
	    break ;
	  }
	  default:
	    throw MED_EXCEPTION ("access type has not been properly specified to the method");
	  }
	break;
      }

      case GIBI_DRIVER : {
	throw MED_EXCEPTION ("driverType other than MED_DRIVER and VTK_DRIVER has been specified to the method which is not allowed for the object FIELD");
	break;
      }

      case PORFLOW_DRIVER : {
	throw MED_EXCEPTION ("driverType other than MED_DRIVER and VTK_DRIVER has been specified to the method which is not allowed for the object FIELD");
	break;
      }

      case ASCII_DRIVER : {
	switch(access)
	  {
	  case MED_EN::WRONLY : {
	    ret=new ASCII_FIELD_DRIVER<T>(fileName,field);
	    break;
	  }
	  default:
	    throw MED_EXCEPTION ("driver ASCII_DRIVER on FIELD only in write mod");
	  }
	break;
      }

      case NO_DRIVER : {
	throw MED_EXCEPTION ("driverType other than MED_DRIVER and VTK_DRIVER has been specified to the method which is not allowed for the object FIELD");
	break;
      }
      default:
	MED_EXCEPTION ("driverType other than MED_DRIVER and VTK_DRIVER has been specified to the method which is not allowed for the object FIELD");
      }
    return ret;
  }

  template<class T, class INTERLACING_TAG>
  GENDRIVER * DRIVERFACTORY::buildFieldDriverFromFile(const string & fileName,
						      FIELD<T,INTERLACING_TAG> * ptrField,
						      MED_EN::med_mode_acces access)
  {
    MED_EN::medFileVersion version;

    try
      {
	version = getMedFileVersion(fileName);
      }
    catch (MEDEXCEPTION & )
      {
	version = DRIVERFACTORY::globalMedFileVersionForWriting;
      }

    MESSAGE_MED("buildFieldDriverFromFile version of the file " << version);

    GENDRIVER * driver;

    switch(access)
      {
      case MED_EN::RDONLY : {
	if (version == MED_EN::V21)
	  driver = new MED_FIELD_RDONLY_DRIVER21<T>(fileName,ptrField);
	else if (version == MED_EN::V22)
	  driver = new MED_FIELD_RDONLY_DRIVER22<T>(fileName,ptrField);
	return driver;
      }
      case MED_EN::WRONLY : {
	if (version == MED_EN::V21)
	  driver = new MED_FIELD_WRONLY_DRIVER21<T>(fileName,ptrField);
	else if (version == MED_EN::V22)
	  driver = new MED_FIELD_WRONLY_DRIVER22<T>(fileName,ptrField);
	return driver;
      }
      case MED_EN::RDWR : {
	if (version == MED_EN::V21)
	  driver = new MED_FIELD_RDWR_DRIVER21<T>(fileName,ptrField);
	else if (version == MED_EN::V22)
	  driver = new MED_FIELD_RDWR_DRIVER22<T>(fileName,ptrField);
	return driver;
      }
      default:
	throw MED_EXCEPTION ("access type has not been properly specified to the method");
      }
  }

  template<class T, class INTERLACING_TAG>
  GENDRIVER * DRIVERFACTORY::buildConcreteMedDriverForField(const std::string & fileName,
							    FIELD<T,INTERLACING_TAG> *ptrField,
							    MED_EN::med_mode_acces access,
							    MED_EN::medFileVersion version)
  {

    MESSAGE_MED("buildConcreteMedDriverForField version of the file " << version);

    GENDRIVER * driver;

    switch(access)
      {
      case MED_EN::RDONLY : {
	if (version == MED_EN::V21)
	  driver = new MED_FIELD_RDONLY_DRIVER21<T>(fileName,ptrField);
	else if (version == MED_EN::V22)
	  driver = new MED_FIELD_RDONLY_DRIVER22<T>(fileName,ptrField);
	return driver;
      }
      case MED_EN::WRONLY : {
	if (version == MED_EN::V21)
	  driver = new MED_FIELD_WRONLY_DRIVER21<T>(fileName,ptrField);
	else if (version == MED_EN::V22)
	  driver = new MED_FIELD_WRONLY_DRIVER22<T>(fileName,ptrField);
	return driver;
      }
      case MED_EN::RDWR : {
	if (version == MED_EN::V21)
	  driver = new MED_FIELD_RDWR_DRIVER21<T>(fileName,ptrField);
	else if (version == MED_EN::V22)
	  driver = new MED_FIELD_RDWR_DRIVER22<T>(fileName,ptrField);
	return driver;
      }
      default:
	throw MED_EXCEPTION ("access type has not been properly specified to the method");
      }
  }
}

#endif
