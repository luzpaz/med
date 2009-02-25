//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
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
#ifdef _DEBUG_
  const char* LOC = "MESHClient::MESHClient(SALOME_MED::MESH_ptr m)";
#endif
  BEGIN_OF(LOC);

  ASSERT(m);

  _coordinate = new COORDINATEClient(m, MED_FULL_INTERLACE);
  _connectivity = new CONNECTIVITYClient(m, MED_FULL_INTERLACE);

  blankCopy();

  END_OF(LOC);
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
#ifdef _DEBUG_
  const char* LOC = "MESHClient::blankCopy()";
#endif
  BEGIN_OF(LOC);
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

  END_OF(LOC);
}
//=============================================================================
/*!
 * Remplit les informations sur les coordonnees et la connectivite 
 */
//=============================================================================

void MESHClient::fillCopy()
{
#ifdef _DEBUG_
  const char* LOC = "MESHClient::fillCopy()";
#endif
  BEGIN_OF(LOC);

  COORDINATEClient *_coord 
    = dynamic_cast<COORDINATEClient *> (_coordinate);
  ASSERT(_coord);
  CONNECTIVITYClient *_connect 
    = dynamic_cast<CONNECTIVITYClient *> (_connectivity);
  ASSERT(_connect);

  _coord->fillCopy();
  _connect->fillCopy();

  int size = _familyNode.size();

  for (int i = 0; i < size; i++)
    {
      FAMILYClient * _fam = dynamic_cast<FAMILYClient *> (_familyNode[i]);
      ASSERT(_fam);

      _fam->fillCopy();
    }

  size = _familyCell.size();

  for (int i = 0; i < size; i++)
    {
      FAMILYClient * _fam = dynamic_cast<FAMILYClient *> (_familyCell[i]);
      ASSERT(_fam);

      _fam->fillCopy();
    }

  size = _familyFace.size();

  for (int i = 0; i < size; i++)
    {
      FAMILYClient * _fam = dynamic_cast<FAMILYClient *> (_familyFace[i]);
      ASSERT(_fam);

      _fam->fillCopy();
    }

  size = _familyEdge.size();

  for (int i = 0; i < size; i++)
    {
      FAMILYClient * _fam = dynamic_cast<FAMILYClient *> (_familyEdge[i]);
      ASSERT(_fam);

      _fam->fillCopy();
    }

  _complete = true;

  END_OF(LOC);
}

//=============================================================================
/*!
 *  Test equality between 2 MESHClients.
 */
//=============================================================================
bool MESHClient::operator==(const MESH& other) const
{
#ifdef _DEBUG_
  const char* LOC = "MESHClient::operator==";
#endif
  BEGIN_OF(LOC);
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
#ifdef _DEBUG_
  const char* LOC = "MESHClient::~MESHClient()";
#endif
  BEGIN_OF(LOC);
  IOR_Mesh->Destroy();
  END_OF(LOC);
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

//=============================================================================
/*!
 * Write all the content of the MESH using driver referenced by the integer handler index
 */
//=============================================================================

void MESHClient::write(int index/*=0*/, const string & driverName/* = ""*/)
{
  this->fillCopy();
  MESH::write(index,driverName);
}

//================================================================================
/*!
 * \brief fillCopy() and then print myself to a stream
 */
//================================================================================

void MESHClient::printMySelf(ostream &os) const
{
  MESHClient* that = (MESHClient*)this;
  that->fillCopy();
  MESH::printMySelf( os );  
}
