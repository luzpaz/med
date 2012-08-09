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

#include "GMESHClient.hxx"
#include "MEDMEM_convert.hxx"
#include "UtilClient.hxx"
#include "COORDINATEClient.hxx"
#include "CONNECTIVITYClient.hxx"
#include "FAMILYClient.hxx"
#include "GROUPClient.hxx"
#include "MESHClient.hxx"
#include "Utils_CorbaException.hxx"

using namespace MEDMEM;
using namespace MED_EN;

//=============================================================================
/*!
 * Constructeur
 */
//=============================================================================

GMESHClient::GMESHClient(const SALOME_MED::GMESH_ptr m) : 
  GMESH(), //_refCounter(1),
  IOR_Mesh(SALOME_MED::GMESH::_duplicate(m)),
  _complete(false), _uMesh(0)

{
  ASSERT(m);

  blankCopy();
}
//=============================================================================
/*!
 * Transforme un IOR Famille en Famille Client
 */
//=============================================================================

FAMILY * convertFamily(const SALOME_MED::FAMILY_ptr &F, GMESH *M) 
{
  return new FAMILYClient(F, M);
}
//=============================================================================
/*!
 * Transforme un IOR groupe en groupe Client
 */
//=============================================================================
GROUP * convertGroup(const SALOME_MED::GROUP_ptr &F, GMESH *M) 
{
  return new GROUPClient(F, M);
}

//=============================================================================
/*!
 * Remplit les informations générales 
 */
//=============================================================================
void GMESHClient::blankCopy()
{
  SALOME_MED::GMESH::meshInfos_var all = IOR_Mesh->getMeshGlobal();

  _name           = all->name;
  _spaceDimension = all->spaceDimension;

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

  if ( !IOR_Mesh->getIsAGrid() && !dynamic_cast<MESHClient*>(this))
  {
    SALOME_MED::MESH_var umeshIOR = IOR_Mesh->convertInMESH();
    _uMesh = new MESHClient( umeshIOR );
  }

  _complete = false;
}
//=============================================================================
/*!
 * Remplit les informations sur les coordonnees et la connectivite 
 */
//=============================================================================

void GMESHClient::fillCopy()
{
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

  if ( _uMesh )
    _uMesh->fillCopy();
  else
    THROW_SALOME_CORBA_EXCEPTION("GRID client does not exists",SALOME::INTERNAL_ERROR);

  _complete = true;
}

//=============================================================================
/*!
 *  Test equality between 2 GMESHClients.
 */
//=============================================================================
bool GMESHClient::operator==(const GMESH& other) const
{
  const GMESHClient* otherClt=dynamic_cast<const GMESHClient *>(&other);
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

GMESHClient::~GMESHClient()
{
  if ( _uMesh )
    _uMesh->removeReference();
  IOR_Mesh->UnRegister();
}

//=============================================================================
/*!
 * Write all the content of the GMESH using driver referenced by the integer handler index
 */
//=============================================================================

void GMESHClient::write(int index/*=0*/) const
{
  if ( index < 0 || index >= (int)_drivers.size() || !_drivers[index] )
    throw MED_EXCEPTION ( LOCALIZED( STRING("GMESHClient::write(int index): ")
                                     << "The index given is invalid, index must be between  0 and |"
                                     << _drivers.size() ));

  const_cast<GMESHClient*>(this)->fillCopy();
  _uMesh->write( *_drivers[index], _drivers[index]->getAccessMode() );
}

//=============================================================================
/*!
 * Write all the content of the GMESH using genDriver
 */
//=============================================================================

void GMESHClient::write(const GENDRIVER &      genDriver,
                        MED_EN::med_mode_acces medMode) const
{
  const_cast<GMESHClient*>(this)->fillCopy();
  _uMesh->write(genDriver,medMode);
}

//=============================================================================
/*!
 * Write all the content of the GMESH
 */
//=============================================================================

void GMESHClient::write(driverTypes        driverType,
                        const std::string& filename,
                        const std::string& meshname,
                        MED_EN::med_mode_acces medMode) const
{
  const_cast<GMESHClient*>(this)->fillCopy();
  _uMesh->write(driverType, filename,meshname,medMode);
}

//================================================================================
/*!
 * \brief fillCopy() and then print myself to a stream
 */
//================================================================================

void GMESHClient::printMySelf(ostream &os) const
{
  GMESHClient* that = (GMESHClient*)this;
  that->fillCopy();
  if ( _uMesh )
    _uMesh->printMySelf( os );
  else
    os << "GMESHClient " << (void*) this;
}

bool GMESHClient::isEmpty() const
{
  return _uMesh ? _uMesh->isEmpty() : (IOR_Mesh->getNumberOfNodes() < 1);
}

bool GMESHClient::deepCompare(const GMESH& other) const
{
  if ( getIsAGrid() != other.getIsAGrid() ||
       getIsAGrid()) // no client nor server for GRID -> can't compare
    return false;

  return _uMesh ? _uMesh->deepCompare( other ) : false;
}

int GMESHClient::getMeshDimension() const
{
  return IOR_Mesh->getMeshDimension();
}

bool GMESHClient::getIsAGrid() const
{
  return IOR_Mesh->getIsAGrid();
}

std::string GMESHClient::getCoordinatesSystem() const
{
  CORBA::String_var s = IOR_Mesh->getCoordinatesSystem();
  return s._retn();
}

const std::string* GMESHClient::getCoordinatesNames() const
{
  return _uMesh ? _uMesh->getCoordinatesNames() : (std::string*) 0;
}

const std::string* GMESHClient::getCoordinatesUnits() const
{
  return _uMesh ? _uMesh->getCoordinatesUnits() : (std::string*) 0;
}

int GMESHClient::getNumberOfNodes() const
{
  return IOR_Mesh->getNumberOfNodes();
}

int GMESHClient::getNumberOfTypes(MED_EN::medEntityMesh entity) const
{
  return IOR_Mesh->getNumberOfTypes(entity);
}

int GMESHClient::getNumberOfElements(MED_EN::medEntityMesh entity,
                                     MED_EN::medGeometryElement type) const
{
  return IOR_Mesh->getNumberOfElements(entity,type);
}

const MED_EN::medGeometryElement* GMESHClient::getTypes(MED_EN::medEntityMesh entity) const
{
  return _uMesh ? _uMesh->getTypes(entity) : (MED_EN::medGeometryElement*) 0;
}

MED_EN::medGeometryElement GMESHClient::getElementType(MED_EN::medEntityMesh entity,
                                                       int i) const
{
  return IOR_Mesh->getElementType(entity,i);
}

const MESH* GMESHClient::convertInMESH() const
{
  if ( !_uMesh )
  {
    SALOME_MED::MESH_var umeshIOR = IOR_Mesh->convertInMESH();
    const_cast<GMESHClient*>(this)->_uMesh = new MESHClient( umeshIOR );
  }
  _uMesh->addReference();
  return _uMesh;
}

SUPPORT* GMESHClient::getBoundaryElements(MED_EN::medEntityMesh entity) const throw (MEDEXCEPTION)
{
  SALOME_MED::SUPPORT_var s = IOR_Mesh->getBoundaryElements(entity);
  return new SUPPORTClient( s );
}

SUPPORT* GMESHClient::getSkin(const SUPPORT* sup) throw (MEDEXCEPTION)
{
  return _uMesh ? _uMesh->getSkin( sup ) : (SUPPORT*)0;
}

SUPPORT* GMESHClient::buildSupportOnNodeFromElementList(const std::list<int>& elems,
                                                        MED_EN::medEntityMesh entity) const
   throw (MEDEXCEPTION)
{
  return _uMesh ? _uMesh->buildSupportOnNodeFromElementList(elems,entity ) : (SUPPORT*)0;
}

void GMESHClient::fillSupportOnNodeFromElementList(const std::list<int>& elems,
                                                   SUPPORT* sup) const
   throw (MEDEXCEPTION)
{
  if ( _uMesh ) _uMesh->fillSupportOnNodeFromElementList(elems,sup);
}

FIELD<double>* GMESHClient::getVolume(const SUPPORT* sup , bool isAbs) const
  throw (MEDEXCEPTION)
{
  FIELD<double>* f = (FIELD<double>*) 0;
  if ( _uMesh && sup )
  {
    sup->setMesh( _uMesh );
    f = _uMesh->getVolume(sup,isAbs);
    sup->setMesh( this );
  }
  return f;
}

FIELD<double>* GMESHClient::getArea(const SUPPORT* sup) const throw (MEDEXCEPTION)
{
  FIELD<double>* f = (FIELD<double>*) 0;
  if ( _uMesh && sup )
  {
    sup->setMesh( _uMesh );
    f = _uMesh->getArea(sup);
    sup->setMesh( this );
  }
  return f;
}

FIELD<double>* GMESHClient::getLength(const SUPPORT* sup) const throw (MEDEXCEPTION)
{
  FIELD<double>* f = (FIELD<double>*) 0;
  if ( _uMesh && sup )
  {
    sup->setMesh( _uMesh );
    f = _uMesh->getLength(sup);
    sup->setMesh( this );
  }
  return f;
}

FIELD<double>* GMESHClient::getNormal(const SUPPORT* sup) const throw (MEDEXCEPTION)
{
  FIELD<double>* f = (FIELD<double>*) 0;
  if ( _uMesh && sup )
  {
    sup->setMesh( _uMesh );
    f = _uMesh->getNormal(sup);
    sup->setMesh( this );
  }
  return f;
}

FIELD<double>* GMESHClient::getBarycenter(const SUPPORT* sup) const throw (MEDEXCEPTION)
{
  FIELD<double>* f = (FIELD<double>*) 0;
  if ( _uMesh && sup )
  {
    sup->setMesh( _uMesh );
    f = _uMesh->getBarycenter(sup);
    sup->setMesh( this );
  }
  return f;
}

std::vector<std::vector<double> > GMESHClient::getBoundingBox() const
{
  std::vector<std::vector<double> > bb;
  if ( _uMesh )
    bb = _uMesh->getBoundingBox();
  return bb;
}

