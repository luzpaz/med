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
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_MedMeshDriver21.hxx"
#include "MEDMEM_DriverFactory.hxx" 
#include "MEDMEM_DriversDef.hxx"

#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_Grid.hxx"


using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

// Every memory allocation made in the MedDriver members function are desallocated in the Mesh destructor 

MED_MESH_DRIVER::MED_MESH_DRIVER():
  GENDRIVER(MED_DRIVER),
  _ptrMesh(( MESH *)MED_NULL),
  _meshName(""),
  _meshNum(MED_INVALID)
{
}

MED_MESH_DRIVER::MED_MESH_DRIVER(const string & fileName,
				 MESH * ptrMesh,
				 MED_EN::med_mode_acces accessMode): 
  GENDRIVER(fileName, accessMode, MED_DRIVER),
  _ptrMesh(ptrMesh), 
  _meshName(""),
  _meshNum(MED_INVALID)
{
}
  
MED_MESH_DRIVER::MED_MESH_DRIVER(const MED_MESH_DRIVER & driver): 
  GENDRIVER(driver),
  _ptrMesh(driver._ptrMesh),
  _meshName(driver._meshName),
  _meshNum(driver._meshNum)
{

}

MED_MESH_DRIVER::~MED_MESH_DRIVER()
{
  MESSAGE("MED_MESH_DRIVER::~MED_MESH_DRIVER()has been destroyed");

}

void MED_MESH_DRIVER::setMeshName(const string & meshName) 
{ 
  _meshName = meshName; 
}

string  MED_MESH_DRIVER::getMeshName() const 
{ 
  return _meshName; 
}

//---------------------------------- RDONLY PART -------------------------------------------------------------

IMED_MESH_RDONLY_DRIVER::IMED_MESH_RDONLY_DRIVER(): MED_MESH_DRIVER()
{
}
  
IMED_MESH_RDONLY_DRIVER::IMED_MESH_RDONLY_DRIVER(const string & fileName,
						 MESH * ptrMesh):
  MED_MESH_DRIVER(fileName, ptrMesh, MED_EN::RDONLY)
{ 
  MESSAGE("IMED_MESH_RDONLY_DRIVER::IMED_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}
  
IMED_MESH_RDONLY_DRIVER::IMED_MESH_RDONLY_DRIVER(const IMED_MESH_RDONLY_DRIVER & driver): 
  MED_MESH_DRIVER(driver)
{
}

int IMED_MESH_RDONLY_DRIVER::getDescendingConnectivity(CONNECTIVITY * Connectivity) 
{
  if (_status==MED_OPENED)
    {
      MESSAGE("MED_MESH_RDONLY_DRIVER::getDescendingConnectivity : "<<"call on the object " << Connectivity);
      MESSAGE("MED_MESH_RDONLY_DRIVER::getDescendingConnectivity : "<<"Not yet implemented !");
    }
  return MED_ERROR;
}

void IMED_MESH_RDONLY_DRIVER::buildAllGroups(vector<GROUP*> & Groups, vector<FAMILY*> & Families) 
{
  const char* LOC = "MED_MESH_RDONLY_DRIVER::buildAllGroups ";
  BEGIN_OF(LOC);

  int numberOfFamilies = Families.size() ;
  //SCRUTE(numberOfFamilies);
  map< string,list<FAMILY*> > groupsNames ;
  for(int i=0; i<numberOfFamilies; i++) {
    FAMILY * myFamily = Families[i] ;
    int numberOfGroups_ = myFamily->getNumberOfGroups();
    //SCRUTE(i);
    //SCRUTE(numberOfGroups_);
    for (int j=0; j<numberOfGroups_; j++) {
      //SCRUTE(j);
      //SCRUTE(myFamily->getGroupName(j+1));
      groupsNames[myFamily->getGroupName(j+1)].push_back(myFamily);
    }
  }
  int numberOfGroups = groupsNames.size() ;
  SCRUTE(numberOfGroups);
  Groups.resize(numberOfGroups);
  map< string,list<FAMILY*> >::const_iterator currentGroup ;
  int it = 0 ;
  for(currentGroup=groupsNames.begin();currentGroup!=groupsNames.end();currentGroup++) {
    GROUP * myGroup = new GROUP(healName((*currentGroup).first),(*currentGroup).second) ;
//     GROUP * myGroup = new GROUP() ;
//     myGroup->setName((*currentGroup).first);
//     SCRUTE(myGroup->getName());
//     //myGroup->setMesh(_ptrMesh);
//     myGroup->init((*currentGroup).second);
    Groups[it]=myGroup;
    //SCRUTE(it);
    it++;
  }

  END_OF(LOC);
}

void IMED_MESH_RDONLY_DRIVER::updateFamily()
{
  const char* LOC = "MED_MESH_RDONLY_DRIVER::updateFamily() ";
  BEGIN_OF(LOC);

  // we need to update family on constituent if we have constituent, but no 
  // descending connectivity, so, we must calculate all constituent and
  // numbering correctly family !
  _ptrMesh->_connectivity->updateFamily(_ptrMesh->_familyFace) ; // in 2d, do nothing
  _ptrMesh->_connectivity->updateFamily(_ptrMesh->_familyEdge) ; // in 3d, do nothing

  END_OF(LOC);
}


void IMED_MESH_RDONLY_DRIVER::write( void ) const
{
  throw MEDEXCEPTION("MED_MESH_RDONLY_DRIVER::write : Can't write with a RDONLY driver !");
}

/*--------------------- WRONLY PART -------------------------------*/

IMED_MESH_WRONLY_DRIVER::IMED_MESH_WRONLY_DRIVER():MED_MESH_DRIVER()
{
}
  
IMED_MESH_WRONLY_DRIVER::IMED_MESH_WRONLY_DRIVER(const string & fileName,
																								 MESH * ptrMesh,
																								 MED_EN::med_mode_acces access):
  MED_MESH_DRIVER(fileName,ptrMesh,access)
{
  MESSAGE("MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}

IMED_MESH_WRONLY_DRIVER::IMED_MESH_WRONLY_DRIVER(const IMED_MESH_WRONLY_DRIVER & driver): 
  MED_MESH_DRIVER(driver)
{
}

IMED_MESH_WRONLY_DRIVER::~IMED_MESH_WRONLY_DRIVER()
{
  //MESSAGE("MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been destroyed");
}

void IMED_MESH_WRONLY_DRIVER::read (void)
{
  throw MEDEXCEPTION("MED_MESH_WRONLY_DRIVER::read : Can't read with a WRONLY driver !");
}

/*--------------------- RDWR PART -------------------------------*/

IMED_MESH_RDWR_DRIVER::IMED_MESH_RDWR_DRIVER()
{
}

IMED_MESH_RDWR_DRIVER::IMED_MESH_RDWR_DRIVER(const string & fileName,
					   MESH * ptrMesh):
   IMED_MESH_RDONLY_DRIVER(fileName,ptrMesh),IMED_MESH_WRONLY_DRIVER(fileName,ptrMesh),MED_MESH_DRIVER(fileName,ptrMesh,RDWR)
{
  MESSAGE("MED_MESH_RDWR_DRIVER::MED_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}

IMED_MESH_RDWR_DRIVER::IMED_MESH_RDWR_DRIVER(const IMED_MESH_RDWR_DRIVER & driver): 
  IMED_MESH_RDONLY_DRIVER(driver),IMED_MESH_WRONLY_DRIVER(driver),MED_MESH_DRIVER(driver)
{
}

IMED_MESH_RDWR_DRIVER::~IMED_MESH_RDWR_DRIVER() {
  //MESSAGE("MED_MESH_RDWR_DRIVER::MED_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) has been destroyed");
}

/////////// Concrete MED_MESH_DRIVERS implementations

MED_MESH_RDONLY_DRIVER::MED_MESH_RDONLY_DRIVER()
{
  MESSAGE("You are using the default constructor of the Mesh read only Driver and it is 2.1 one");
  _concreteMeshDrv = new MED_MESH_RDONLY_DRIVER21();
}

MED_MESH_RDONLY_DRIVER::MED_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh):
  IMED_MESH_RDONLY_DRIVER(fileName,ptrMesh),MED_MESH_DRIVER(fileName,ptrMesh,RDONLY)
{
  _concreteMeshDrv = DRIVERFACTORY::buildMeshDriverFromFile(fileName,ptrMesh,RDONLY);
}

MED_MESH_RDONLY_DRIVER::MED_MESH_RDONLY_DRIVER(const MED_MESH_RDONLY_DRIVER & driver):MED_MESH_DRIVER(driver)
{
  _concreteMeshDrv = driver._concreteMeshDrv->copy();
}

MED_MESH_RDONLY_DRIVER::~MED_MESH_RDONLY_DRIVER()
{
  if (_concreteMeshDrv) delete _concreteMeshDrv;
}

void MED_MESH_RDONLY_DRIVER::write( void ) const
{
  _concreteMeshDrv->write();
}

void MED_MESH_RDONLY_DRIVER::activateFacesComputation()
{
  IMED_MESH_RDONLY_DRIVER *drvC=dynamic_cast<IMED_MESH_RDONLY_DRIVER *>(_concreteMeshDrv);
  if(drvC)
    drvC->activateFacesComputation();
}

void MED_MESH_RDONLY_DRIVER::desactivateFacesComputation()
{
  IMED_MESH_RDONLY_DRIVER *drvC=dynamic_cast<IMED_MESH_RDONLY_DRIVER *>(_concreteMeshDrv);
  if(drvC)
    drvC->desactivateFacesComputation();
}

void MED_MESH_RDONLY_DRIVER::read ( void )
{
  _concreteMeshDrv->read();
}

void MED_MESH_RDONLY_DRIVER::open()
{
  _concreteMeshDrv->open();
}

void MED_MESH_RDONLY_DRIVER::close()
{
  _concreteMeshDrv->close();
}

void MED_MESH_RDONLY_DRIVER::setMeshName(const string & meshName)
{
  _concreteMeshDrv->setMeshName(meshName);
}

string MED_MESH_RDONLY_DRIVER::getMeshName() const
{
  return _concreteMeshDrv->getMeshName();
}

GENDRIVER * MED_MESH_RDONLY_DRIVER::copy ( void ) const
{
  return new MED_MESH_RDONLY_DRIVER(*this);
}

void MED_MESH_RDONLY_DRIVER::merge ( const GENDRIVER& driver )
{
  const MED_MESH_RDONLY_DRIVER *other=dynamic_cast<const MED_MESH_RDONLY_DRIVER *>(&driver);
  if(other)
    _concreteMeshDrv->merge( *other->_concreteMeshDrv );
  else
    _concreteMeshDrv->merge( driver );
}
// int MED_MESH_RDONLY_DRIVER::getCOORDINATE()
// {
//   return _concreteMeshDrv->getCOORDINATE();
// }

// int MED_MESH_RDONLY_DRIVER::getCONNECTIVITY()
// {
//   return _concreteMeshDrv->getCONNECTIVITY();
// }

// int MED_MESH_RDONLY_DRIVER::getFAMILY()
// {
//   return _concreteMeshDrv->getFAMILY();
// }

// int MED_MESH_RDONLY_DRIVER::getNodalConnectivity(CONNECTIVITY * Connectivity)
// {
//   return _concreteMeshDrv->getNodalConnectivity(Connectivity);
// }

// int MED_MESH_RDONLY_DRIVER::getDescendingConnectivity(CONNECTIVITY * Connectivity)
// {
//   return _concreteMeshDrv->getDescendingConnectivity(Connectivity);
// }

// int MED_MESH_RDONLY_DRIVER::getNodesFamiliesNumber(int * MEDArrayNodeFamily)
// {
//   return _concreteMeshDrv->getNodesFamiliesNumber(MEDArrayNodeFamily);
// }

// int MED_MESH_RDONLY_DRIVER::getCellsFamiliesNumber(int** Arrays, CONNECTIVITY* Connectivity, MED_EN::medEntityMesh entity)
// {
//   return _concreteMeshDrv->getCellsFamiliesNumber(Arrays,Connectivity,entity);
// }

// void MED_MESH_RDONLY_DRIVER::getGRID()
// {
//   _concreteMeshDrv->getGRID();
// }

MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER()
{
  MESSAGE("You are using the default constructor of the Mesh write only Driver and it is 2.1 one");
  _concreteMeshDrv = new MED_MESH_WRONLY_DRIVER21();
}

MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh, MED_EN::med_mode_acces access):
  IMED_MESH_WRONLY_DRIVER(fileName,ptrMesh),MED_MESH_DRIVER(fileName,ptrMesh,access)
{
  _concreteMeshDrv = DRIVERFACTORY::buildMeshDriverFromFile(fileName,ptrMesh,access);
}

MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER(const MED_MESH_WRONLY_DRIVER & driver):MED_MESH_DRIVER(driver)
{
  _concreteMeshDrv = driver._concreteMeshDrv->copy();
}

MED_MESH_WRONLY_DRIVER::~MED_MESH_WRONLY_DRIVER()
{
  if (_concreteMeshDrv) delete _concreteMeshDrv;
}

void MED_MESH_WRONLY_DRIVER::open()
{
  _concreteMeshDrv->open();
}

void MED_MESH_WRONLY_DRIVER::close()
{
  _concreteMeshDrv->close();
}

void MED_MESH_WRONLY_DRIVER::setMeshName(const string & meshName)
{
  _concreteMeshDrv->setMeshName(meshName);
}

string MED_MESH_WRONLY_DRIVER::getMeshName() const
{
  return _concreteMeshDrv->getMeshName();
}

GENDRIVER * MED_MESH_WRONLY_DRIVER::copy ( void ) const
{
  return new MED_MESH_WRONLY_DRIVER(*this);
}

void MED_MESH_WRONLY_DRIVER::read ()
{
  _concreteMeshDrv->read();
}

void MED_MESH_WRONLY_DRIVER::write( void ) const
{
  _concreteMeshDrv->write();
}

void MED_MESH_WRONLY_DRIVER::merge ( const GENDRIVER& driver )
{
  const MED_MESH_WRONLY_DRIVER *other=dynamic_cast<const MED_MESH_WRONLY_DRIVER *>(&driver);
  if(other)
    _concreteMeshDrv->merge( *other->_concreteMeshDrv );
  else
    _concreteMeshDrv->merge( driver );
}

// int MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER::writeCoordinates    ()                           const
// {
//   return _concreteMeshDrv->writeCoordinates();
// }

// int MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER::writeConnectivities (MED_EN::medEntityMesh entity)       const
// {
//   return _concreteMeshDrv->writeConnectivities(entity);
// }

// int MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER::writeFamilyNumbers  ()                           const
// {
//   return _concreteMeshDrv->writeFamilyNumbers();
// }

// int MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER::writeFamilies       (vector<FAMILY*> & families) const
// {
//   return _concreteMeshDrv->writeFamilies(families);
// }

// int MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER::writeGRID() const
// {
//   return _concreteMeshDrv->writeGRID();
// }

MED_MESH_RDWR_DRIVER::MED_MESH_RDWR_DRIVER()
{
  _concreteMeshDrv=new MED_MESH_RDWR_DRIVER21();
}

MED_MESH_RDWR_DRIVER::MED_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh):
  IMED_MESH_RDWR_DRIVER(fileName,ptrMesh),MED_MESH_DRIVER(fileName,ptrMesh,RDWR)
{
  _concreteMeshDrv = DRIVERFACTORY::buildMeshDriverFromFile(fileName,ptrMesh,RDWR);
}

MED_MESH_RDWR_DRIVER::MED_MESH_RDWR_DRIVER(const MED_MESH_RDWR_DRIVER & driver): MED_MESH_DRIVER(driver)
{
  _concreteMeshDrv = driver._concreteMeshDrv->copy();
}

MED_MESH_RDWR_DRIVER::~MED_MESH_RDWR_DRIVER()
{
  if (_concreteMeshDrv) delete _concreteMeshDrv;
}

void MED_MESH_RDWR_DRIVER::read ( void )
{
  _concreteMeshDrv->read();
}

void MED_MESH_RDWR_DRIVER::write( void ) const
{
  _concreteMeshDrv->write();
}
 
void MED_MESH_RDWR_DRIVER::activateFacesComputation()
{
  IMED_MESH_RDONLY_DRIVER *drvC=dynamic_cast<IMED_MESH_RDONLY_DRIVER *>(_concreteMeshDrv);
  if(drvC)
    drvC->activateFacesComputation();
}

void MED_MESH_RDWR_DRIVER::desactivateFacesComputation()
{
  IMED_MESH_RDONLY_DRIVER *drvC=dynamic_cast<IMED_MESH_RDONLY_DRIVER *>(_concreteMeshDrv);
  if(drvC)
    drvC->desactivateFacesComputation();
}

void MED_MESH_RDWR_DRIVER::open()
{
  _concreteMeshDrv->open();
}

void MED_MESH_RDWR_DRIVER::close()
{
  _concreteMeshDrv->close();
}

// int MED_MESH_RDWR_DRIVER::getCOORDINATE()
// {
//   return _concreteMeshDrv->getCOORDINATE();
// }

// int MED_MESH_RDWR_DRIVER::getCONNECTIVITY()
// {
//   return _concreteMeshDrv->getCONNECTIVITY();
// }

// int MED_MESH_RDWR_DRIVER::getFAMILY()
// {
//   return _concreteMeshDrv->getFAMILY();
// }

// int MED_MESH_RDWR_DRIVER::getNodalConnectivity(CONNECTIVITY * Connectivity)
// {
//   return _concreteMeshDrv->getNodalConnectivity(Connectivity);
// }

// int MED_MESH_RDWR_DRIVER::getDescendingConnectivity(CONNECTIVITY * Connectivity)
// {
//   return _concreteMeshDrv->getDescendingConnectivity(Connectivity);
// }
 
// int MED_MESH_RDWR_DRIVER::getNodesFamiliesNumber(int * MEDArrayNodeFamily)
// {
//   return _concreteMeshDrv->getNodesFamiliesNumber(MEDArrayNodeFamily);
// }

// int MED_MESH_RDWR_DRIVER::getCellsFamiliesNumber(int** Arrays, CONNECTIVITY* Connectivity, MED_EN::medEntityMesh entity)
// {
//   return _concreteMeshDrv->getCellsFamiliesNumber(Arrays,Connectivity,entity);
// }

// void MED_MESH_RDWR_DRIVER::getGRID ()
// {
//   _concreteMeshDrv->getGRID();
// }

// int MED_MESH_RDWR_DRIVER::writeCoordinates    ()                           const
// {
//   _concreteMeshDrv->writeCoordinates();
// }

// int MED_MESH_RDWR_DRIVER::writeConnectivities (MED_EN::medEntityMesh entity)       const
// {
//   return _concreteMeshDrv->writeConnectivities (entity);
// }

// int MED_MESH_RDWR_DRIVER::writeFamilyNumbers  ()                           const
// {
//   return _concreteMeshDrv->writeFamilyNumbers();
// }

// int MED_MESH_RDWR_DRIVER::writeFamilies       (vector<FAMILY*> & families) const
// {
//   return _concreteMeshDrv->writeFamilies(families);
// }

// int MED_MESH_RDWR_DRIVER::writeGRID() const
// {
//   return _concreteMeshDrv->writeGRID();
// }

void MED_MESH_RDWR_DRIVER::setMeshName(const string & meshName)
{
  _concreteMeshDrv->setMeshName(meshName);
}

string MED_MESH_RDWR_DRIVER::getMeshName() const
{
  return _concreteMeshDrv->getMeshName();
}

GENDRIVER * MED_MESH_RDWR_DRIVER::copy ( void ) const
{
  return new MED_MESH_RDWR_DRIVER(*this);
}

void MED_MESH_RDWR_DRIVER::merge ( const GENDRIVER& driver )
{
  const MED_MESH_RDWR_DRIVER *other=dynamic_cast<const MED_MESH_RDWR_DRIVER *>(&driver);
  if(other)
    _concreteMeshDrv->merge( *other->_concreteMeshDrv );
  else
    _concreteMeshDrv->merge( driver );
}
