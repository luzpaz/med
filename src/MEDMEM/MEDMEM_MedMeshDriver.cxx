#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_MedMeshDriver21.hxx"
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
  GENDRIVER(),
  _ptrMesh(( MESH *)MED_NULL),
  _meshName(""),
  _meshNum(MED_INVALID)
{
}

MED_MESH_DRIVER::MED_MESH_DRIVER(const string & fileName,
				 MESH * ptrMesh,
				 MED_EN::med_mode_acces accessMode): 
  GENDRIVER(fileName,accessMode),
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
					       MESH * ptrMesh):MED_MESH_DRIVER(fileName, ptrMesh, MED_EN::MED_RDONLY)
{ 
  MESSAGE("MED_MESH_RDONLY_DRIVER::MED_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}
  
IMED_MESH_RDONLY_DRIVER::IMED_MESH_RDONLY_DRIVER(const IMED_MESH_RDONLY_DRIVER & driver): 
  MED_MESH_DRIVER(driver)
{
}

int IMED_MESH_RDONLY_DRIVER::getDescendingConnectivity(CONNECTIVITY * Connectivity) 
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER::getDescendingConnectivity : " ;
  if (_status==MED_OPENED)
    {
      MESSAGE(LOC<<"call on the object " << Connectivity);
      MESSAGE(LOC<<"Not yet implemented !");
    }
  return MED_ERROR;
}

void IMED_MESH_RDONLY_DRIVER::buildAllGroups(vector<GROUP*> & Groups, vector<FAMILY*> & Families) 
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER::buildAllGroups " ;
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
    GROUP * myGroup = new GROUP((*currentGroup).first,(*currentGroup).second) ;
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
  const char * LOC = "MED_MESH_RDONLY_DRIVER::updateFamily() " ;
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
					       MESH * ptrMesh):
  MED_MESH_DRIVER(fileName,ptrMesh,MED_WRONLY)
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
   IMED_MESH_RDONLY_DRIVER(fileName,ptrMesh),IMED_MESH_WRONLY_DRIVER(fileName,ptrMesh),MED_MESH_DRIVER(fileName,ptrMesh,MED_RDWR)
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
  _concreteDriverRd=new MED_MESH_RDONLY_DRIVER21;
}

MED_MESH_RDONLY_DRIVER::MED_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh)
{
  _concreteDriverRd=new MED_MESH_RDONLY_DRIVER21(fileName,ptrMesh);
}

MED_MESH_RDONLY_DRIVER::MED_MESH_RDONLY_DRIVER(const MED_MESH_RDONLY_DRIVER & driver)
{
  _concreteDriverRd=new MED_MESH_RDONLY_DRIVER21;//to complete
}

MED_MESH_RDONLY_DRIVER::~MED_MESH_RDONLY_DRIVER()
{
  delete _concreteDriverRd;
}

void MED_MESH_RDONLY_DRIVER::write( void ) const
{
  _concreteDriverRd->write();
}

void MED_MESH_RDONLY_DRIVER::read ( void )
{
  _concreteDriverRd->read();
}

void MED_MESH_RDONLY_DRIVER::open()
{
  _concreteDriverRd->open();
}

void MED_MESH_RDONLY_DRIVER::close()
{
  _concreteDriverRd->close();
}

void MED_MESH_RDONLY_DRIVER::setMeshName(const string & meshName)
{
  _concreteDriverRd->setMeshName(meshName);
}

string MED_MESH_RDONLY_DRIVER::getMeshName() const
{
  return _concreteDriverRd->getMeshName();
}

GENDRIVER * MED_MESH_RDONLY_DRIVER::copy ( void ) const
{
  return _concreteDriverRd->copy();
}

int MED_MESH_RDONLY_DRIVER::getCOORDINATE()
{
  return _concreteDriverRd->getCOORDINATE();
}

int MED_MESH_RDONLY_DRIVER::getCONNECTIVITY()
{
  return _concreteDriverRd->getCONNECTIVITY();
}

int MED_MESH_RDONLY_DRIVER::getFAMILY()
{
  return _concreteDriverRd->getFAMILY();
}

int MED_MESH_RDONLY_DRIVER::getNodalConnectivity(CONNECTIVITY * Connectivity)
{
  return _concreteDriverRd->getNodalConnectivity(Connectivity);
}

int MED_MESH_RDONLY_DRIVER::getDescendingConnectivity(CONNECTIVITY * Connectivity)
{
  return _concreteDriverRd->getDescendingConnectivity(Connectivity);
}

int MED_MESH_RDONLY_DRIVER::getNodesFamiliesNumber(int * MEDArrayNodeFamily)
{
  return _concreteDriverRd->getNodesFamiliesNumber(MEDArrayNodeFamily);
}

int MED_MESH_RDONLY_DRIVER::getCellsFamiliesNumber(int** Arrays, CONNECTIVITY* Connectivity, MED_EN::medEntityMesh entity)
{
  return _concreteDriverRd->getCellsFamiliesNumber(Arrays,Connectivity,entity);
}

void MED_MESH_RDONLY_DRIVER::getGRID()
{
  _concreteDriverRd->getGRID();
}

MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER()
{
  _concreteDriverWr=new MED_MESH_WRONLY_DRIVER21;
}

MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh)
{
  _concreteDriverWr=new MED_MESH_WRONLY_DRIVER21(fileName,ptrMesh);
}

MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER(const MED_MESH_WRONLY_DRIVER & driver)
{
  _concreteDriverWr=new MED_MESH_WRONLY_DRIVER21;
}

MED_MESH_WRONLY_DRIVER::~MED_MESH_WRONLY_DRIVER()
{
  delete _concreteDriverWr;
}

void MED_MESH_WRONLY_DRIVER::open()
{
  _concreteDriverWr->open();
}

void MED_MESH_WRONLY_DRIVER::close()
{
  _concreteDriverWr->close();
}

void MED_MESH_WRONLY_DRIVER::setMeshName(const string & meshName)
{
  _concreteDriverWr->setMeshName(meshName);
}

string MED_MESH_WRONLY_DRIVER::getMeshName() const
{
  return _concreteDriverWr->getMeshName();
}

GENDRIVER * MED_MESH_WRONLY_DRIVER::copy ( void ) const
{
  return _concreteDriverWr->copy();
}

void MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER::read ( void )
{
  _concreteDriverWr->read();
}

void MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER::write( void ) const
{
  _concreteDriverWr->write();
}

int MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER::writeCoordinates    ()                           const
{
  return _concreteDriverWr->writeCoordinates();
}

int MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER::writeConnectivities (MED_EN::medEntityMesh entity)       const
{
  return _concreteDriverWr->writeConnectivities(entity);
}

int MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER::writeFamilyNumbers  ()                           const
{
  return _concreteDriverWr->writeFamilyNumbers();
}

int MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER::writeFamilies       (vector<FAMILY*> & families) const
{
  return _concreteDriverWr->writeFamilies(families);
}

int MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER::writeGRID() const
{
  return _concreteDriverWr->writeGRID();
}

MED_MESH_RDWR_DRIVER::MED_MESH_RDWR_DRIVER()
{
  _concreteDriverRdWr=new MED_MESH_RDWR_DRIVER21;
}

MED_MESH_RDWR_DRIVER::MED_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh)
{
  _concreteDriverRdWr=new MED_MESH_RDWR_DRIVER21(fileName,ptrMesh);
}

MED_MESH_RDWR_DRIVER::MED_MESH_RDWR_DRIVER(const MED_MESH_RDWR_DRIVER & driver)
{
  _concreteDriverRdWr=new MED_MESH_RDWR_DRIVER21;
}

MED_MESH_RDWR_DRIVER::~MED_MESH_RDWR_DRIVER()
{
  delete _concreteDriverRdWr;
}

void MED_MESH_RDWR_DRIVER::read ( void )
{
  _concreteDriverRdWr->read();
}

void MED_MESH_RDWR_DRIVER::write( void ) const
{
  _concreteDriverRdWr->write();
}
 
void MED_MESH_RDWR_DRIVER::open()
{
  _concreteDriverRdWr->open();
}

void MED_MESH_RDWR_DRIVER::close()
{
  _concreteDriverRdWr->close();
}

int MED_MESH_RDWR_DRIVER::getCOORDINATE()
{
  return _concreteDriverRdWr->getCOORDINATE();
}

int MED_MESH_RDWR_DRIVER::getCONNECTIVITY()
{
  return _concreteDriverRdWr->getCONNECTIVITY();
}

int MED_MESH_RDWR_DRIVER::getFAMILY()
{
  return _concreteDriverRdWr->getFAMILY();
}

int MED_MESH_RDWR_DRIVER::getNodalConnectivity(CONNECTIVITY * Connectivity)
{
  return _concreteDriverRdWr->getNodalConnectivity(Connectivity);
}

int MED_MESH_RDWR_DRIVER::getDescendingConnectivity(CONNECTIVITY * Connectivity)
{
  return _concreteDriverRdWr->getDescendingConnectivity(Connectivity);
}
 
int MED_MESH_RDWR_DRIVER::getNodesFamiliesNumber(int * MEDArrayNodeFamily)
{
  return _concreteDriverRdWr->getNodesFamiliesNumber(MEDArrayNodeFamily);
}

int MED_MESH_RDWR_DRIVER::getCellsFamiliesNumber(int** Arrays, CONNECTIVITY* Connectivity, MED_EN::medEntityMesh entity)
{
  return _concreteDriverRdWr->getCellsFamiliesNumber(Arrays,Connectivity,entity);
}

void MED_MESH_RDWR_DRIVER::getGRID ()
{
  _concreteDriverRdWr->getGRID();
}

int MED_MESH_RDWR_DRIVER::writeCoordinates    ()                           const
{
  _concreteDriverRdWr->writeCoordinates();
}

int MED_MESH_RDWR_DRIVER::writeConnectivities (MED_EN::medEntityMesh entity)       const
{
  return _concreteDriverRdWr->writeConnectivities (entity);
}

int MED_MESH_RDWR_DRIVER::writeFamilyNumbers  ()                           const
{
  return _concreteDriverRdWr->writeFamilyNumbers();
}

int MED_MESH_RDWR_DRIVER::writeFamilies       (vector<FAMILY*> & families) const
{
  return _concreteDriverRdWr->writeFamilies(families);
}

int MED_MESH_RDWR_DRIVER::writeGRID() const
{
  return _concreteDriverRdWr->writeGRID();
}

void MED_MESH_RDWR_DRIVER::setMeshName(const string & meshName)
{
  _concreteDriverRdWr->setMeshName(meshName);
}

string MED_MESH_RDWR_DRIVER::getMeshName() const
{
  return _concreteDriverRdWr->getMeshName();
}

GENDRIVER * MED_MESH_RDWR_DRIVER::copy ( void ) const
{
  return _concreteDriverRdWr->copy();
}
