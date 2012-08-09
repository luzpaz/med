// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
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
  IOR_Mesh(SALOME_MED::MESH::_duplicate(m)),
  _complete(false)//,_refCounter(1)
{
  if ( CORBA::is_nil( m ))
    throw MEDEXCEPTION("MESHClient(const SALOME_MED::MESH_ptr mesh): NULL mesh!");

  _coordinate = new COORDINATEClient(m, MED_FULL_INTERLACE);
  _connectivity = new CONNECTIVITYClient(m, MED_FULL_INTERLACE);

  blankCopy();
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
  SALOME_MED::GMESH::meshInfos_var all = IOR_Mesh->getMeshGlobal();

  //CORBA::String_var s;
  //s= IOR_Mesh->getName(); _name = s;
  //_spaceDimension = IOR_Mesh->getSpaceDimension();
  //_meshDimension  = IOR_Mesh->getMeshDimension();
  //_numberOfNodes  = IOR_Mesh->getNumberOfNodes();
  
  _name           = all->name;
  _spaceDimension = all->spaceDimension;
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
}
//=============================================================================
/*!
 * Remplit les informations sur les coordonnees et la connectivite 
 */
//=============================================================================

void MESHClient::fillCopy()
{
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
}

//=============================================================================
/*!
 *  Test equality between 2 MESHClients.
 */
//=============================================================================
bool MESHClient::operator==(const MESH& other) const
{
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
  IOR_Mesh->UnRegister();
}

//=============================================================================
/*!
 * For refCounter
 */
//=============================================================================

/*void MESHClient::addReference() const
{
  _refCounter++;
}*/

//=============================================================================
/*!
 * For refCounter
 */
//=============================================================================

/*void MESHClient::removeReference() const
{
  if (--_refCounter <= 0)
    {
      delete this;
    }
}*/

//=============================================================================
/*!
 * Write all the content of the MESH using driver referenced by the integer handler index
 */
//=============================================================================

void MESHClient::write(int index/*=0*/) const
{
  const_cast<MESHClient*>(this)->fillCopy();
  GMESH::write(index);
}

//=============================================================================
/*!
 * Write all the content of the GMESH using genDriver
 */
//=============================================================================

void MESHClient::write(const GENDRIVER &      genDriver,
                       MED_EN::med_mode_acces medMode) const
{
  const_cast<MESHClient*>(this)->fillCopy();
  GMESH::write(genDriver);
}

//=============================================================================
/*!
 * Write all the content of the GMESH
 */
//=============================================================================

void MESHClient::write(driverTypes        driverType,
                       const std::string& filename,
                       const std::string& meshname,
                       MED_EN::med_mode_acces medMode) const
{
  const_cast<MESHClient*>(this)->fillCopy();
  GMESH::write(driverType, filename);
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
