#include "MESHClient.hxx"
#include "MEDMEM_convert.hxx"
#include "UtilClient.hxx"
#include "COORDINATEClient.hxx"
#include "CONNECTIVITYClient.hxx"
#include "FAMILYClient.hxx"
#include "GROUPClient.hxx"
//=============================================================================
/*!
 * Constructeur
 */
//=============================================================================

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
//=============================================================================
/*!
 * Transforme un IOR Famille en Famille Client
 */
//=============================================================================

FAMILY * convertFamily(const SALOME_MED::FAMILY_ptr &F, MESH *M) 
{
  return new FAMILYClient(F, M);
}
//=============================================================================
/*!
 * Transforme un IOR groupe en groupe Client
 */
//=============================================================================
GROUP * convertGroup(const SALOME_MED::GROUP_ptr &F, MESH *M) 
{
  return new GROUPClient(F, M);
}
//=============================================================================
/*!
 * Remplit les informations g�n�rales 
 */
//=============================================================================
void MESHClient::blankCopy()
{
  BEGIN_OF("MESHClient::blankCopy()");
  SALOME_MED::MESH::meshInfos * all  = new SALOME_MED::MESH::meshInfos;
  all = IOR_Mesh->getMeshGlobal();

  //CORBA::String_var s;
  //s= IOR_Mesh->getName(); _name = s;
  //_spaceDimension = IOR_Mesh->getSpaceDimension();
  //_meshDimension  = IOR_Mesh->getMeshDimension();
  //_numberOfNodes  = IOR_Mesh->getNumberOfNodes();
  
  _name           = all->name;
  _spaceDimension = all->spaceDimension;
  _meshDimension  = all->meshDimension;
  _numberOfNodes  = all->numberOfNodes;

  COORDINATEClient *_coord 
    = dynamic_cast<COORDINATEClient*>(_coordinate);
  ASSERT(_coord);
  CONNECTIVITYClient *_connect 
    = dynamic_cast<CONNECTIVITYClient*>(_connectivity);
  ASSERT(_connect);

  _coord->blankCopy();
  _connect->blankCopy();

  convertCorbaArray<SALOME_MED::FAMILY_ptr>
    (_familyNode, 
     _numberOfNodesFamilies, 
     //IOR_Mesh->getFamilies(MED_NODE),
     &all->famNode,
     (void *) (convertFamily), this);
 
  convertCorbaArray<SALOME_MED::FAMILY_ptr>
    (_familyEdge, 
     _numberOfEdgesFamilies, 
     //IOR_Mesh->getFamilies(MED_EDGE),
     &all->famEdge,
     (void *) (convertFamily), this);
 
  convertCorbaArray<SALOME_MED::FAMILY_ptr>
    (_familyFace, 
     _numberOfFacesFamilies, 
     //IOR_Mesh->getFamilies(MED_FACE),
     &all->famFace,
     (void *) (convertFamily), this);
 
  convertCorbaArray<SALOME_MED::FAMILY_ptr>
    (_familyCell, 
     _numberOfCellsFamilies, 
     //IOR_Mesh->getFamilies(MED_CELL),
     &all->famCell,
     (void *) (convertFamily), this);
 
  convertCorbaArray<SALOME_MED::GROUP_ptr>
    (_groupNode, 
     _numberOfNodesGroups, 
     //IOR_Mesh->getGroups(MED_NODE),
     &all->groupNode,
     (void *) (convertGroup), this);
 
  convertCorbaArray<SALOME_MED::GROUP_ptr>
    (_groupEdge, 
     _numberOfEdgesGroups, 
     //IOR_Mesh->getGroups(MED_EDGE),
     &all->groupEdge,
     (void *) (convertGroup), this);
 
  convertCorbaArray<SALOME_MED::GROUP_ptr>
    (_groupFace, 
     _numberOfFacesGroups, 
     //IOR_Mesh->getGroups(MED_FACE),
     &all->groupFace,
     (void *) (convertGroup), this);
 
  convertCorbaArray<SALOME_MED::GROUP_ptr>
    (_groupCell, 
     _numberOfCellsGroups, 
     //IOR_Mesh->getGroups(MED_CELL),
     &all->groupCell,
     (void *) (convertGroup), this);
 
  _complete = false;

  END_OF("MESHClient::blankCopy()");
}
//=============================================================================
/*!
 * Remplit les informations sur les coordonnees et la connectivite 
 */
//=============================================================================

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

//=============================================================================
/*!
 * Destructeur
 */
//=============================================================================

MESHClient::~MESHClient()
{
  BEGIN_OF("MESHClient::~MESHClient()");
  END_OF("MESHClient::~MESHClient()");
}

