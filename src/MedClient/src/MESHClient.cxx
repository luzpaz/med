#include "MESHClient.hxx"
#include <convert.hxx>
#include "UtilClient.hxx"
#include "COORDINATEClient.hxx"
#include "CONNECTIVITYClient.hxx"
#include "FAMILYClient.hxx"
#include "GROUPClient.hxx"

MESHClient::MESHClient(const SALOME_MED::MESH_ptr m) : 
  MESH(), 
  IOR_Mesh(SALOME_MED::MESH::_duplicate(m)),
  _complete(false)

{
  BEGIN_OF("MESHClient::MESHClient(SALOME_MED::MESH_ptr m)");

  ASSERT(m);

  _coordinate = new COORDINATEClient(m, MED_FULL_INTERLACE);
  _connectivity = new CONNECTIVITYClient(m, MED_FULL_INTERLACE);

  blankCopy();

  END_OF("MESHClient::MESHClient(SALOME_MED::MESH_ptr m)");
}

FAMILY * convertFamily(const SALOME_MED::FAMILY_ptr &F, MESH *M) {
  return new FAMILYClient(F, M);
}

GROUP * convertGroup(const SALOME_MED::GROUP_ptr &F, MESH *M) {
  return new GROUPClient(F, M);
}

void MESHClient::blankCopy()
{
  BEGIN_OF("MESHClient::blankCopy()");

  CORBA::String_var s;
  
  s= IOR_Mesh->getName(); _name = s;
  _spaceDimension = IOR_Mesh->getSpaceDimension();
  _meshDimension  = IOR_Mesh->getMeshDimension();
  _numberOfNodes  = IOR_Mesh->getNumberOfNodes();
  
   COORDINATEClient *_coord 
     = dynamic_cast<COORDINATEClient*>(_coordinate);
   ASSERT(_coord);
  CONNECTIVITYClient *_connect 
    = dynamic_cast<CONNECTIVITYClient*>(_connectivity);
  ASSERT(_connect);

   _coord->blankCopy();
  _connect->blankCopy();

  MESSAGE("");
  convertCorbaArray<SALOME_MED::FAMILY_ptr>
    (_familyNode, 
     _numberOfNodesFamilies, 
     IOR_Mesh->getFamilies(MED_NODE),
     (void *) (convertFamily), this);
 
  MESSAGE("");
  convertCorbaArray<SALOME_MED::FAMILY_ptr>
    (_familyEdge, 
     _numberOfEdgesFamilies, 
     IOR_Mesh->getFamilies(MED_EDGE),
     (void *) (convertFamily), this);
 
  MESSAGE("");
  convertCorbaArray<SALOME_MED::FAMILY_ptr>
    (_familyFace, 
     _numberOfFacesFamilies, 
     IOR_Mesh->getFamilies(MED_FACE),
     (void *) (convertFamily), this);
 
  MESSAGE("");
  convertCorbaArray<SALOME_MED::FAMILY_ptr>
    (_familyCell, 
     _numberOfCellsFamilies, 
     IOR_Mesh->getFamilies(MED_CELL),
     (void *) (convertFamily), this);
 
  MESSAGE("");
  convertCorbaArray<SALOME_MED::GROUP_ptr>
    (_groupNode, 
     _numberOfNodesGroups, 
     IOR_Mesh->getGroups(MED_NODE),
     (void *) (convertGroup), this);
 
  MESSAGE("");
  convertCorbaArray<SALOME_MED::GROUP_ptr>
    (_groupEdge, 
     _numberOfEdgesGroups, 
     IOR_Mesh->getGroups(MED_EDGE),
     (void *) (convertGroup), this);
 
  MESSAGE("");
  convertCorbaArray<SALOME_MED::GROUP_ptr>
    (_groupFace, 
     _numberOfFacesGroups, 
     IOR_Mesh->getGroups(MED_FACE),
     (void *) (convertGroup), this);
 
  MESSAGE("");
  convertCorbaArray<SALOME_MED::GROUP_ptr>
    (_groupCell, 
     _numberOfCellsGroups, 
     IOR_Mesh->getGroups(MED_CELL),
     (void *) (convertGroup), this);
 
  MESSAGE("");
  _complete = false;

  END_OF("MESHClient::blankCopy()");
}

void MESHClient::fillCopy()
{
  BEGIN_OF("MESHClient::fillCopy()");

  COORDINATEClient *_coord 
    = dynamic_cast<COORDINATEClient *> (_coordinate);
  ASSERT(_coord);
  CONNECTIVITYClient *_connect 
    = dynamic_cast<CONNECTIVITYClient *> (_connectivity);
  ASSERT(_connect);

  _coord->fillCopy();
  _connect->fillCopy();

  _complete = true;

  END_OF("MESHClient::fillCopy()");
}


MESHClient::~MESHClient()
{
  BEGIN_OF("MESHClient::~MESHClient()");
  END_OF("MESHClient::~MESHClient()");
}

