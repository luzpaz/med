#ifndef DRIVERFACTORY_HXX
#define DRIVERFACTORY_HXX

#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_VtkFieldDriver.hxx"
#include "MEDMEM_MedFieldDriver.hxx"
#include <string>

#include "MEDMEM_FieldForward.hxx"

namespace MEDMEM {

  class MESH;
  class MED;

  namespace DRIVERFACTORY {
    GENDRIVER *buildDriverForMesh(driverTypes driverType,
				  const std::string & fileName,
				  MESH *mesh,const string &  driverName,
				  MED_EN::med_mode_acces access);
    template<class T>
    GENDRIVER *buildDriverForField(driverTypes driverType,
				   const std::string & fileName,
				   FIELD<T> *fielde,
				   MED_EN::med_mode_acces access);
    GENDRIVER *buildDriverForMed(driverTypes driverType,
				 const std::string & fileName,
				 MED *mede,
				 MED_EN::med_mode_acces access);
  }

template<class T>
GENDRIVER *DRIVERFACTORY::buildDriverForField(driverTypes driverType,
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
	  ret=new MED_FIELD_RDONLY_DRIVER<T>(fileName,field);
	  break;
	}
	case MED_EN::MED_ECRI : {
	  ret=new MED_FIELD_WRONLY_DRIVER<T>(fileName,field);
	  break;
	}
	case MED_EN::MED_REMP : {
	  ret=new MED_FIELD_RDWR_DRIVER<T>(fileName,field);
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

}

#endif
