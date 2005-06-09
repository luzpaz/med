#ifndef DRIVERFACTORY_HXX
#define DRIVERFACTORY_HXX

#include "MEDMEM_MedVersion.hxx"
#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_define.hxx"

#include <string>

namespace MEDMEM {

  class MESH;
  template<class T> class FIELD;
  class MED;
  class GENDRIVER;

  namespace DRIVERFACTORY {

    /*
      definition of static variable across the Med Memory of a Med File version
      for the writing of Med File set to V21
    */

    static MED_EN::medFileVersion globalMedFileVersionForWriting = MED_EN::V21;

    MED_EN::medFileVersion getMedFileVersionForWriting();

    void setMedFileVersionForWriting(MED_EN::medFileVersion version);

    GENDRIVER * buildDriverForMesh(driverTypes driverType,
				   const std::string & fileName,
				   MESH *mesh,const string &  driverName,
				   MED_EN::med_mode_acces access);
    template<class T>
    GENDRIVER * buildDriverForField(driverTypes driverType,
				    const std::string & fileName,
				    FIELD<T> *fielde,
				    MED_EN::med_mode_acces access);
    GENDRIVER * buildDriverForMed(driverTypes driverType,
				  const std::string & fileName,
				  MED *mede,
				  MED_EN::med_mode_acces access);
    GENDRIVER * buildMedDriverFromFile(const string & fileName,
				       MED * const ptrMed,
				       MED_EN::med_mode_acces access);
    GENDRIVER * buildMeshDriverFromFile(const string & fileName,
					MESH * ptrMesh,
					MED_EN::med_mode_acces access);
    template<class T>
    GENDRIVER * buildFieldDriverFromFile(const string & fileName,
					 FIELD<T> * ptrField,
					 MED_EN::med_mode_acces access);
    GENDRIVER * buildConcreteMedDriverForMesh(const std::string & fileName,
					      MESH *mesh,
					      const string & driverName,
					      MED_EN::med_mode_acces access,
					      MED_EN::medFileVersion version);
    template<class T>
    GENDRIVER * buildConcreteMedDriverForField(const std::string & fileName,
					       FIELD<T> *fielde,
					       MED_EN::med_mode_acces access,
					       MED_EN::medFileVersion version);
  }
}

#include "MEDMEM_VtkFieldDriver.hxx"
#include "MEDMEM_MedFieldDriver.hxx"

namespace MEDMEM {
  template<class T>
  GENDRIVER * DRIVERFACTORY::buildDriverForField(driverTypes driverType,
						 const std::string & fileName,
						 FIELD<T> *field,
						 MED_EN::med_mode_acces access)
  {
    GENDRIVER *ret;
    switch(driverType)
      {
      case MED_DRIVER : {
	switch(access)
	  {
	  case MED_EN::MED_LECT : {
	    ret = new MED_FIELD_RDONLY_DRIVER<T>(fileName,field);
	    break;
	  }
	  case MED_EN::MED_ECRI : {
	    ret= new MED_FIELD_WRONLY_DRIVER<T>(fileName,field);
	    break;
	  }
	  case MED_EN::MED_REMP : {
	    ret = new MED_FIELD_RDWR_DRIVER<T>(fileName,field);
	    break;
	  }
	  default:
	    throw MED_EXCEPTION ("access type has not been properly specified to the method");
	  }
	break;
      }

      case VTK_DRIVER : {
	switch(access)
	  {
	  case MED_EN::MED_LECT : {
	    throw MED_EXCEPTION ("access mode other than MED_ECRI and MED_REMP has been specified with the VTK_DRIVER type which is not allowed because VTK_DRIVER is only a write access driver");
	    break;
	  }
	  case MED_EN::MED_ECRI : {
	    ret=new VTK_FIELD_DRIVER<T>(fileName,field);
	    break;
	  }
	  case MED_EN::MED_REMP : {
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

      case NO_DRIVER : {
	throw MED_EXCEPTION ("driverType other than MED_DRIVER and VTK_DRIVER has been specified to the method which is not allowed for the object FIELD");
	break;
      }
      default:
	MED_EXCEPTION ("driverType other than MED_DRIVER and VTK_DRIVER has been specified to the method which is not allowed for the object FIELD");
      }
    return ret;
  }

  template<class T>
  GENDRIVER * DRIVERFACTORY::buildFieldDriverFromFile(const string & fileName,
						      FIELD<T> * ptrField,
						      MED_EN::med_mode_acces access)
  {
    MED_EN::medFileVersion version;

    try
      {
	version = getMedFileVersion(fileName);
      }
    catch (MEDEXCEPTION & ex)
      {
	version = DRIVERFACTORY::globalMedFileVersionForWriting;
      }

    MESSAGE("buildFieldDriverFromFile version of the file " << version);

    GENDRIVER * driver;

    switch(access)
      {
      case MED_EN::MED_LECT : {
	if (version == MED_EN::V21)
	  driver = new MED_FIELD_RDONLY_DRIVER21<T>(fileName,ptrField);
	else if (version == MED_EN::V22)
	  driver = new MED_FIELD_RDONLY_DRIVER22<T>(fileName,ptrField);
	return driver;
      }
      case MED_EN::MED_ECRI : {
	if (version == MED_EN::V21)
	  driver = new MED_FIELD_WRONLY_DRIVER21<T>(fileName,ptrField);
	else if (version == MED_EN::V22)
	  driver = new MED_FIELD_WRONLY_DRIVER22<T>(fileName,ptrField);
	return driver;
      }
      case MED_EN::MED_REMP : {
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

  template<class T>
  GENDRIVER * DRIVERFACTORY::buildConcreteMedDriverForField(const std::string & fileName,
							    FIELD<T> *ptrField,
							    MED_EN::med_mode_acces access,
							    MED_EN::medFileVersion version)
  {

    MESSAGE("buildConcreteMedDriverForField version of the file " << version);

    GENDRIVER * driver;

    switch(access)
      {
      case MED_EN::MED_LECT : {
	if (version == MED_EN::V21)
	  driver = new MED_FIELD_RDONLY_DRIVER21<T>(fileName,ptrField);
	else if (version == MED_EN::V22)
	  driver = new MED_FIELD_RDONLY_DRIVER22<T>(fileName,ptrField);
	return driver;
      }
      case MED_EN::MED_ECRI : {
	if (version == MED_EN::V21)
	  driver = new MED_FIELD_WRONLY_DRIVER21<T>(fileName,ptrField);
	else if (version == MED_EN::V22)
	  driver = new MED_FIELD_WRONLY_DRIVER22<T>(fileName,ptrField);
	return driver;
      }
      case MED_EN::MED_REMP : {
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
