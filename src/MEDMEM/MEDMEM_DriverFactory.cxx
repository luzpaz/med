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
using namespace MED_EN;

template<>
void fill<-1,0x3>(double *a, const double *b)
{
}

template<>
bool compare<-1>(const double *a, const double *b)
{
  return false;
}

GENDRIVER *DRIVERFACTORY::buildDriverForMesh(driverTypes driverType,
					     const std::string & fileName,
					     MESH *mesh,
					     const string & driverName,
					     med_mode_acces access)
{
  GENDRIVER *ret;
  switch(driverType)
    {
    case MED_DRIVER : {
      switch(access)
	{
	case MED_LECT : {
	  MED_MESH_RDONLY_DRIVER *retmed=new MED_MESH_RDONLY_DRIVER(fileName,
								    mesh);
	  retmed->setMeshName(driverName);
	  return retmed;
	}
	case MED_ECRI : {
	  MED_MESH_WRONLY_DRIVER *retmed=new MED_MESH_WRONLY_DRIVER(fileName,
								    mesh);
	  retmed->setMeshName(driverName);
	  return retmed;
	}
	case MED_REMP : {
	  MED_MESH_RDWR_DRIVER *retmed=new MED_MESH_RDWR_DRIVER(fileName,mesh);
	  retmed->setMeshName(driverName);
	  return retmed;
	}
	default:
	  throw MED_EXCEPTION ("access type has not been properly specified to the method");
	}
      break;
    }

    case GIBI_DRIVER : {
      switch(access)
	{
	case MED_LECT : {
	  ret=new GIBI_MESH_RDONLY_DRIVER(fileName,mesh);
	  return ret;
	}
	case MED_ECRI : {
	  throw MED_EXCEPTION ("access mode other than MED_LECT has been specified with the GIBI_DRIVER type which is not allowed because GIBI_DRIVER is only a read access driver");
	}
	case MED_REMP : {
	  throw MED_EXCEPTION ("access mode other than MED_LECT has been specified with the GIBI_DRIVER type which is not allowed because GIBI_DRIVER is only a read access driver");
	}
	default:
	  throw MED_EXCEPTION ("access type has not been properly specified to the method");
	}
      break;
    }

    case PORFLOW_DRIVER : {
      switch(access)
	{
	case MED_LECT : {
	  ret=new PORFLOW_MESH_RDONLY_DRIVER(fileName,mesh);
	  return ret;
	}
	case MED_ECRI : {
	  throw MED_EXCEPTION ("access mode other than MED_LECT has been specified with the PORFLOW_DRIVER type which is not allowed because PORFLOW_DRIVER is only a read access driver");
	}
	case MED_REMP : {
	  throw MED_EXCEPTION ("access mode other than MED_LECT has been specified with the PORFLOW_DRIVER type which is not allowed because PORFLOW_DRIVER is only a read access driver");
	}
	default:
	  throw MED_EXCEPTION ("access type has not been properly specified to the method");
	}
      break;
    }

    case VTK_DRIVER : {
      switch(access)
	{
	case MED_LECT : {
	  throw MED_EXCEPTION ("access mode other than MED_ECRI or MED_REMPT has been specified with the VTK_DRIVER type which is not allowed because VTK_DRIVER is only a write access driver");
	}
	case MED_ECRI : {
	  ret=new VTK_MESH_DRIVER(fileName,mesh);
	  return ret;
	}
	case MED_REMP : {
	  ret=new VTK_MESH_DRIVER(fileName,mesh);
	  return ret;
	}
	default:
	  throw MED_EXCEPTION ("access type has not been properly specified to the method");
	}
      break;
    }

    case NO_DRIVER : {
      throw MED_EXCEPTION ("NO_DRIVER has been specified to the method which is not allowed");
    }
    default:
      throw MED_EXCEPTION ("other driver than MED_DRIVER GIBI_DRIVER PORFLOW_DRIVER and VT_DRIVER has been specified to the method which is not allowed");
    }
}

GENDRIVER *DRIVERFACTORY::buildDriverForMed(driverTypes driverType,
					    const std::string & fileName,
					    MED *med, med_mode_acces access)
{
  GENDRIVER *ret;

  switch(driverType)
    {
    case MED_DRIVER : {
      switch(access)
	{
	case MED_LECT : {
	  ret=new MED_MED_RDONLY_DRIVER(fileName,med);
	  break ;
	}
	case MED_ECRI : {
	  ret=new MED_MED_WRONLY_DRIVER(fileName,med);
	  break ;
	}
	case MED_REMP : {
	  ret=new MED_MED_RDONLY_DRIVER(fileName,med);
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
	case MED_LECT : {
	  throw MED_EXCEPTION ("access mode other than MED_ECRI or MED_REMPT has been specified with the VTK_DRIVER type which is not allowed because VTK_DRIVER is only a write access driver");
	}
	case MED_ECRI : {
	  ret=new VTK_MED_DRIVER(fileName,med);
	  break ;
	}
	case MED_REMP : {
	  ret=new VTK_MED_DRIVER(fileName,med);
	  break ;
	}
	default:
	  throw MED_EXCEPTION ("access type has not been properly specified to the method");
	}
      break;
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

