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
#include "MEDMEM_DriverFactory.hxx"
#include "MEDMEM_MedMedDriver.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_GibiMeshDriver.hxx"
#include "MEDMEM_PorflowMeshDriver.hxx"
#include "MEDMEM_VtkMeshDriver.hxx"
#include "MEDMEM_VtkMedDriver.hxx"
#include "MEDMEM_EnsightMeshDriver.hxx"
#include "MEDMEM_EnsightMedDriver.hxx"

#include "MEDMEM_Exception.hxx"

#include "MEDMEM_MedVersion.hxx"
#include "MEDMEM_Compatibility21_22.hxx"
#include "MEDMEM_MedMedDriver21.hxx"
#include "MEDMEM_MedMedDriver22.hxx"
#include "MEDMEM_MedMeshDriver21.hxx"
#include "MEDMEM_MedMeshDriver22.hxx"

using namespace MEDMEM;
using namespace MED_EN;

template<>
void MEDMEM::fill<-1,0x3>(double *a, const double *b)
{
}

template<>
bool MEDMEM::compare<-1>(const double *a, const double *b)
{
  return false;
}

MED_EN::medFileVersion DRIVERFACTORY::globalMedFileVersionForWriting = MED_EN::V22;

medFileVersion DRIVERFACTORY::getMedFileVersionForWriting()
{
  return DRIVERFACTORY::globalMedFileVersionForWriting;
}

void DRIVERFACTORY::setMedFileVersionForWriting(medFileVersion version)
{
  DRIVERFACTORY::globalMedFileVersionForWriting = version;
}

driverTypes DRIVERFACTORY::deduceDriverTypeFromFileName(const std::string & fileName)
{
  string extension(fileName);
  unsigned int pos=extension.rfind('.');
  if(pos==string::npos)
    return NO_DRIVER;
  extension.erase(0,pos+1);
  if(extension=="med")
    return MED_DRIVER;
  if(extension=="sauve" || extension=="sauv")
    return GIBI_DRIVER;
  if(extension=="cnc" || extension=="inp" || extension=="xyz")
    return PORFLOW_DRIVER;
  if(extension=="vtk")
    return VTK_DRIVER;
  if(extension=="case")
    return ENSIGHT_DRIVER;
  return NO_DRIVER;
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
	  ret = new MED_MESH_RDONLY_DRIVER(fileName, mesh);
	  ret->setMeshName(driverName);
	  return ret;
	}
	case MED_ECRI : {
	  ret = new MED_MESH_WRONLY_DRIVER(fileName, mesh);
	  ret->setMeshName(driverName);
	  return ret;
	}
	case MED_REMP : {
	  ret = new MED_MESH_RDWR_DRIVER(fileName, mesh);
	  ret->setMeshName(driverName);
	  return ret;
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

    case ENSIGHT_DRIVER : {
      switch(access)
	{
	case MED_LECT : {
	  ret=new ENSIGHT_MESH_DRIVER(fileName,mesh);
	  return ret;
	}
	case MED_ECRI : {
	  ret=new ENSIGHT_MESH_DRIVER(fileName,mesh);
	  return ret;
	}
	case MED_REMP : {
	  ret=new ENSIGHT_MESH_DRIVER(fileName,mesh);
	  return ret;
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
      throw MED_EXCEPTION ("NO_DRIVER has been specified to the method 1 which is not allowed");
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
	  ret=new MED_MED_RDWR_DRIVER(fileName,med);
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

    case ENSIGHT_DRIVER : {
      switch(access)
	{
	case MED_LECT : {
	  ret=new ENSIGHT_MED_RDONLY_DRIVER(fileName,med);
	  break ;
	}
	case MED_ECRI : {
	  ret=new ENSIGHT_MED_WRONLY_DRIVER(fileName,med);
	  break ;
	}
	case MED_REMP : {
	  throw MED_EXCEPTION ("not yet implemented");
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
      throw MED_EXCEPTION ("NO_DRIVER has been specified to the method 2 which is not allowed");
      break;
    }
    default:
      throw MED_EXCEPTION ("NO_DRIVER has been specified to the method 3 which is not allowed");
    }
  return ret;
}

GENDRIVER * DRIVERFACTORY::buildMedDriverFromFile(const string & fileName,
						  MED * const ptrMed,
						  MED_EN::med_mode_acces access)
{
  medFileVersion version;

  try
    {
      version = getMedFileVersion(fileName);
    }
  catch (MEDEXCEPTION & ex)
    {
      version = DRIVERFACTORY::globalMedFileVersionForWriting;
    }

  MESSAGE("buildMedDriverFromFile version of the file " << version);

  GENDRIVER * driver;

  switch(access)
    {
    case MED_LECT : {
      if (version == V21)
	driver = new MED_MED_RDONLY_DRIVER21(fileName,ptrMed);
      else if (version == V22)
	driver = new MED_MED_RDONLY_DRIVER22(fileName,ptrMed);
      return driver;
    }
    case MED_ECRI : {
      if (version == V21)
	driver = new MED_MED_WRONLY_DRIVER21(fileName,ptrMed);
      else if (version == V22)
	driver = new MED_MED_WRONLY_DRIVER22(fileName,ptrMed);
      return driver;
    }
    case MED_REMP : {
      if (version == V21)
	driver = new MED_MED_RDWR_DRIVER21(fileName,ptrMed);
      else if (version == V22)
	driver = new MED_MED_RDWR_DRIVER22(fileName,ptrMed);
      return driver;
    }
    default:
      throw MED_EXCEPTION ("access type has not been properly specified to the method");
    }
}

GENDRIVER * DRIVERFACTORY::buildMeshDriverFromFile(const string & fileName,
						   MESH * ptrMesh,
						   MED_EN::med_mode_acces access)
{
  medFileVersion version;

  try
    {
      version = getMedFileVersion(fileName);
    }
  catch (MEDEXCEPTION & ex)
    {
      version = DRIVERFACTORY::globalMedFileVersionForWriting;
    }

  MESSAGE("buildMeshDriverFromFile version of the file " << version);

  GENDRIVER * driver;

  switch(access)
    {
    case MED_LECT : {
      if (version == V21)
	driver = new MED_MESH_RDONLY_DRIVER21(fileName,ptrMesh);
      else if (version == V22)
	driver = new MED_MESH_RDONLY_DRIVER22(fileName,ptrMesh);
      return driver;
    }
    case MED_ECRI : {
      if (version == V21)
	driver = new MED_MESH_WRONLY_DRIVER21(fileName,ptrMesh);
      else if (version == V22)
	driver = new MED_MESH_WRONLY_DRIVER22(fileName,ptrMesh);
      return driver;
    }
    case MED_REMP : {
      if (version == V21)
	driver = new MED_MESH_RDWR_DRIVER21(fileName,ptrMesh);
      else if (version == V22)
	driver = new MED_MESH_RDWR_DRIVER22(fileName,ptrMesh);
      return driver;
    }
    default:
      throw MED_EXCEPTION ("access type has not been properly specified to the method");
    }
}

GENDRIVER * DRIVERFACTORY::buildConcreteMedDriverForMesh(const std::string & fileName,
							 MESH *ptrMesh,const string &  driverName,
							 MED_EN::med_mode_acces access,
							 MED_EN::medFileVersion version)
{
  GENDRIVER * driver;

  MESSAGE("buildConcreteMedDriverForMesh version of the file " << version);

  switch(access)
    {
    case MED_LECT : {
      if (version == V21)
	driver = new MED_MESH_RDONLY_DRIVER21(fileName,ptrMesh);
      else if (version == V22)
	driver = new MED_MESH_RDONLY_DRIVER22(fileName,ptrMesh);
      driver->setMeshName(driverName);
      return driver;
    }
    case MED_ECRI : {
      if (version == V21)
	driver = new MED_MESH_WRONLY_DRIVER21(fileName,ptrMesh);
      else if (version == V22)
	driver = new MED_MESH_WRONLY_DRIVER22(fileName,ptrMesh);
      driver->setMeshName(driverName);
      return driver;
    }
    case MED_REMP : {
      if (version == V21)
	driver = new MED_MESH_RDWR_DRIVER21(fileName,ptrMesh);
      else if (version == V22)
	driver = new MED_MESH_RDWR_DRIVER22(fileName,ptrMesh);
      driver->setMeshName(driverName);
      return driver;
    }
    default:
      throw MED_EXCEPTION ("access type has not been properly specified to the method");
    }
}
