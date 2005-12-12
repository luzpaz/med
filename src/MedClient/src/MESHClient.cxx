#include "MESHClient.hxx"
#include "MEDMEM_convert.hxx"
#include "UtilClient.hxx"
#include "COORDINATEClient.hxx"
#include "CONNECTIVITYClient.hxx"
#include "FAMILYClient.hxx"
#include "GROUPClient.hxx"

using namespace MEDMEM;
using namespace MED_EN;

//=============================================================================
/*!
 * Constructeur
 */
//=============================================================================

MESHClient::MESHClient(const SALOME_MED::MESH_ptr m) : 
  MESH(), _refCounter(1),
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
 * Remplit les informations générales 
 */
//=============================================================================
void MESHClient::blankCopy()
{
  BEGIN_OF("MESHClient::blankCopy()");
  SALOME_MED::MESH::meshInfos_var all = IOR_Mesh->getMeshGlobal();

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
     //IOR_Mesh->getFamilies(MED_NODE),
     &all->famNode,
     (void *) (convertFamily), this);
 
  convertCorbaArray<SALOME_MED::FAMILY_ptr>
    (_familyEdge, 
     //IOR_Mesh->getFamilies(MED_EDGE),
     &all->famEdge,
     (void *) (convertFamily), this);
 
  convertCorbaArray<SALOME_MED::FAMILY_ptr>
    (_familyFace, 
     //IOR_Mesh->getFamilies(MED_FACE),
     &all->famFace,
     (void *) (convertFamily), this);
 
  convertCorbaArray<SALOME_MED::FAMILY_ptr>
    (_familyCell, 
     //IOR_Mesh->getFamilies(MED_CELL),
     &all->famCell,
     (void *) (convertFamily), this);
 
  convertCorbaArray<SALOME_MED::GROUP_ptr>
    (_groupNode, 
     //IOR_Mesh->getGroups(MED_NODE),
     &all->groupNode,
     (void *) (convertGroup), this);
 
  convertCorbaArray<SALOME_MED::GROUP_ptr>
    (_groupEdge, 
     //IOR_Mesh->getGroups(MED_EDGE),
     &all->groupEdge,
     (void *) (convertGroup), this);
 
  convertCorbaArray<SALOME_MED::GROUP_ptr>
    (_groupFace, 
     //IOR_Mesh->getGroups(MED_FACE),
     &all->groupFace,
     (void *) (convertGroup), this);
 
  convertCorbaArray<SALOME_MED::GROUP_ptr>
    (_groupCell, 
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
 *  Test equality between 2 MESHClients.
 */
//=============================================================================
bool MESHClient::operator==(const MESH& other) const
{
  BEGIN_OF("MESHClient::operator==");
  const MESHClient* otherClt=dynamic_cast<const MESHClient *>(&other);
  if(otherClt)
    {
      if(this==otherClt)
	{
	  return true;
	}
      return IOR_Mesh->areEquals(otherClt->IOR_Mesh);
    }
  return false;
}

//=============================================================================
/*!
 * Destructeur
 */
//=============================================================================

MESHClient::~MESHClient()
{
  BEGIN_OF("MESHClient::~MESHClient()");
  IOR_Mesh->Destroy();
  END_OF("MESHClient::~MESHClient()");
}

//=============================================================================
/*!
 * For refCounter
 */
//=============================================================================

void MESHClient::addReference() const
{
  _refCounter++;
}

//=============================================================================
/*!
 * For refCounter
 */
//=============================================================================

void MESHClient::removeReference() const
{
  if (--_refCounter <= 0)
    {
      delete this;
    }
}
