#include "MEDMEM_DriverFactory.hxx"

#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_GibiMeshDriver.hxx"
#include "MEDMEM_PorflowMeshDriver.hxx"
#include "MEDMEM_VtkMeshDriver.hxx"

#include "MEDMEM_Med.hxx"
#include "MEDMEM_MedMedDriver.hxx"
#include "MEDMEM_VtkMedDriver.hxx"

//#include "MEDMEM_Field.hxx"

#include "MEDMEM_Exception.hxx"
//#include "MEDMEM_STRING.hxx"
//#include "utilities.h"

using namespace MEDMEM;

GENDRIVER *DRIVERFACTORY::buildDriverForMesh(driverTypes driverType, const std::string & fileName, MESH *mesh,const string &  driverName)
{
  GENDRIVER *ret;
  switch(driverType)
    {
    case MED_DRIVER : {
      MED_MESH_RDWR_DRIVER *retmed=new MED_MESH_RDWR_DRIVER(fileName,mesh);
      retmed->setMeshName(driverName);
      return retmed;
    }

    case GIBI_DRIVER : {
      ret=new GIBI_MESH_RDWR_DRIVER(fileName,mesh);
      return ret;
    }

    case PORFLOW_DRIVER : {
      ret=new PORFLOW_MESH_RDWR_DRIVER(fileName,mesh);
      return ret;
    }

    case VTK_DRIVER : {
      ret=new VTK_MESH_DRIVER(fileName,mesh);
      return ret;
    }

    case NO_DRIVER : {
      throw MED_EXCEPTION ("NO_DRIVER has been specified to the method which is not allowed");
    }
    default:
      throw MED_EXCEPTION ("other driver than MED_DRIVER GIBI_DRIVER PORFLOW_DRIVER and VT_DRIVER has been specified to the method which is not allowed");
    }
}

GENDRIVER *DRIVERFACTORY::buildDriverForMed(driverTypes driverType, const std::string & fileName, MED *med)
{
  GENDRIVER *ret;
  switch(driverType)
    {
    case MED_DRIVER : {
      ret=new MED_MED_RDWR_DRIVER(fileName,med);
      break ;
    }

    case VTK_DRIVER : {
      ret=new VTK_MED_DRIVER(fileName,med);
      break ;
    }

    case GIBI_DRIVER : {
      throw MED_EXCEPTION ("GIBI_DRIVER has been specified to the method which is not allowed because there is no GIBI driver for the MED object");
      break;
    }

    case PORFLOW_DRIVER : {
      throw MED_EXCEPTION ("PORFLOW_DRIVER has been specified to the method which is not allowed because there is no PORFLOW driver for the MED object");
      break;
    }

    case NO_DRIVER : {
      throw MED_EXCEPTION ("NO_DRIVER has been specified to the method which is not allowed");
      break;
    }
    default:
      throw MED_EXCEPTION ("NO_DRIVER has been specified to the method which is not allowed");
    }
  return ret;
}

