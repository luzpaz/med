#ifndef DRIVERFACTORY_HXX
#define DRIVERFACTORY_HXX

#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_VtkFieldDriver.hxx"
#include "MEDMEM_MedFieldDriver.hxx"
#include <string>

namespace MEDMEM {

  class MESH;
  template<class T> class FIELD;
  class MED;

  namespace DRIVERFACTORY {
    GENDRIVER *buildDriverForMesh(driverTypes driverType, const std::string & fileName, MESH *mesh,const string &  driverName);
    template<class T>
    GENDRIVER *buildDriverForField(driverTypes driverType, const std::string & fileName, FIELD<T> *field);
    GENDRIVER *buildDriverForMed(driverTypes driverType, const std::string & fileName, MED *med);
  }

template<class T>
GENDRIVER *DRIVERFACTORY::buildDriverForField(driverTypes driverType, const std::string & fileName, FIELD<T> *field)
{
  GENDRIVER *ret;
  switch(driverType)
    {
    case MED_DRIVER : {
      ret=new MED_FIELD_RDWR_DRIVER<T>(fileName,field);
      break;
    }

    case VTK_DRIVER : {
      ret=new VTK_FIELD_DRIVER<T>(fileName,field);
      break ;
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
