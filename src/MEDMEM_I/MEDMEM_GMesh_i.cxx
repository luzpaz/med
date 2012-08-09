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

//=============================================================================
// File      : MEDMEM_Mesh_i.cxx
// Project   : SALOME
// Author    : EDF 
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_Mesh_i.cxx
//=============================================================================
//
#include <vector>

#include "utilities.h"
#include "Utils_CorbaException.hxx"
#include "Utils_ORB_INIT.hxx"
#include "Utils_SINGLETON.hxx"

#include "MEDMEM_convert.hxx"
#include "MEDMEM_Mesh_i.hxx"
#include "MEDMEM_Support_i.hxx"
#include "MEDMEM_Family_i.hxx"
#include "MEDMEM_Group_i.hxx"
#include "MEDMEM_FieldTemplate_i.hxx"

#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_CellModel.hxx"

#include "SenderFactory.hxx"
#include "MultiCommException.hxx"
using namespace MEDMEM;

// Initialisation des variables statiques
map < int, ::GMESH *> GMESH_i::meshMap ;
int GMESH_i::meshIndex = 0 ;



//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
GMESH_i::GMESH_i():
  SALOMEMultiComm(),
  SALOME::GenericObj_i(),
  _mesh(constructConstMesh()),
  _corbaIndex(MESH_i::meshIndex++),
  _meshId("") 
{
  MESH_i::meshMap[_corbaIndex]=_mesh;
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
GMESH_i::~GMESH_i()
{
  if ( _mesh )
    _mesh->removeReference();
}
//=============================================================================
/*!
 * Constructor
 */
//=============================================================================
GMESH_i::GMESH_i(::GMESH * const m ) :
  SALOMEMultiComm(),
  SALOME::GenericObj_i(),
  _mesh(m),
  _corbaIndex(MESH_i::meshIndex++),
  _meshId("") 
{
  if ( _mesh ) _mesh->addReference();
  GMESH_i::meshMap[_corbaIndex]=_mesh;
}
//=============================================================================
/*!
 * Copy Constructor 
 */
//=============================================================================
//MESH_i::MESH_i(const MESH_i & m) :_mesh(m._mesh),
GMESH_i::GMESH_i( GMESH_i & m) :
  SALOMEMultiComm(),
  SALOME::GenericObj_i(),
  _mesh(m._mesh),
  _corbaIndex(MESH_i::meshIndex++),
  _meshId("") 
{
  if ( _mesh ) _mesh->addReference();
  GMESH_i::meshMap[_corbaIndex]=_mesh;
}
//=============================================================================
/*!
 * Internal Method in order to have a const ptr
 */
//=============================================================================
::GMESH * GMESH_i::constructConstMesh() const
{
        ::MESH * const ptrMesh =new ::MESH();
        return ptrMesh;
}
//=============================================================================
/*!
 * CORBA: Accessor for Name
 */
//=============================================================================
char * GMESH_i::getName()        
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);

        try
        {
                return CORBA::string_dup(_mesh->getName().c_str());
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces the mesh name");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for corbaindex cuisine interne 
 */
//=============================================================================
CORBA::Long GMESH_i::getCorbaIndex() 
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        return _corbaIndex;
}
//=============================================================================
/*!
 * CORBA: Accessor for Space Dimension
 */
//=============================================================================
CORBA::Long GMESH_i::getSpaceDimension() 
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _mesh->getSpaceDimension();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces the space dimension ");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for Mesh Dimension
 */
//=============================================================================
CORBA::Long GMESH_i::getMeshDimension()
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _mesh->getMeshDimension();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces the mesh dimension ");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: boolean indicating if mesh is a Grid
 */
//=============================================================================
CORBA::Boolean GMESH_i::getIsAGrid()
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _mesh->getIsAGrid();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces mesh flag isAGrid");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}

//=============================================================================
/*!
 * CORBA: Accessor for Coordinates System
 */
//=============================================================================
char * GMESH_i::getCoordinatesSystem()
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);

        try
        {
                return CORBA::string_dup(_mesh->getCoordinatesSystem().c_str());
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces the type of CoordinatesSystem");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}

//=============================================================================
/*!
 * CORBA: Accessor for Coordinates Names
 */
//=============================================================================
SALOME_TYPES::ListOfString  * GMESH_i::getCoordinatesNames() 
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        SALOME_TYPES::ListOfString_var myseq = new SALOME_TYPES::ListOfString;
        try
        {
                int spaceDimension=_mesh->getSpaceDimension();
                const string * coordinatesName =_mesh->getCoordinatesNames();
                myseq->length(spaceDimension);
                for (int i=0; i<spaceDimension; i++)
                {
                        myseq[i]=CORBA::string_dup(coordinatesName[i].c_str());
                }
        }
        catch (MEDEXCEPTION &ex)
        {       
                MESSAGE("Unable to acces the coordinates names");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();

}
//=============================================================================
/*!
 * CORBA: Accessor for Coordinates Units
 */
//=============================================================================
SALOME_TYPES::ListOfString *  GMESH_i::getCoordinatesUnits()
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        SALOME_TYPES::ListOfString_var myseq = new SALOME_TYPES::ListOfString;
        try
        {
                int spaceDimension=_mesh->getSpaceDimension();
                const string * coordinatesUnits =_mesh->getCoordinatesUnits();
                myseq->length(spaceDimension);
                for (int i=0; i<spaceDimension; i++)
                {
                         myseq[i]=CORBA::string_dup(coordinatesUnits[i].c_str());
                };
        }
        catch (MEDEXCEPTION &ex)
        {       
                MESSAGE("Unable to acces the coordinates units");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Accessor for Number of Nodes
 */
//=============================================================================
CORBA::Long  GMESH_i::getNumberOfNodes() 
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _mesh->getNumberOfNodes();
        }
        catch (MEDEXCEPTION &ex)
        {       
                MESSAGE("Unable to acces number of nodes");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for number of Types
 */
//=============================================================================
CORBA::Long GMESH_i::getNumberOfTypes(SALOME_MED::medEntityMesh entity) 
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _mesh->getNumberOfTypes(convertIdlEntToMedEnt(entity));
        }
        catch (MEDEXCEPTION &ex)
        {       
                MESSAGE("Unable to acces number of differents types");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Accessor for existing geometry element types
 */
//=============================================================================
SALOME_MED::medGeometryElement GMESH_i::getElementType (SALOME_MED::medEntityMesh entity,
                                                       CORBA::Long number)
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _mesh->getElementType(convertIdlEntToMedEnt(entity),number);
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces number of differents element types");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}

//=============================================================================
/*!
 * CORBA: Accessor for existing geometry element types 
 *        Not implemented for MED_ALL_ENTITIES
 */
//=============================================================================
SALOME_MED::medGeometryElement_array * GMESH_i::getTypes (SALOME_MED::medEntityMesh entity) 
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        if (entity ==SALOME_MED::MED_ALL_ENTITIES)
                THROW_SALOME_CORBA_EXCEPTION("Not implemented for MED_ALL_ENTITIES",\
                                             SALOME::BAD_PARAM);
        SALOME_MED::medGeometryElement_array_var myseq = 
                                        new SALOME_MED::medGeometryElement_array;
        try
        {
                int nbTypes=_mesh->getNumberOfTypes(convertIdlEntToMedEnt(entity));
                const medGeometryElement * elemts  =_mesh->getTypes(
                                       convertIdlEntToMedEnt(entity));
                myseq->length(nbTypes);
                for (int i=0; i<nbTypes; i++)
                {
                        myseq[i]=convertMedEltToIdlElt(elemts[i]);
                };
        }
        catch (MEDEXCEPTION &ex)
        {       
                MESSAGE("Unable to acces coordinates");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Returns number of elements of type medGeometryElement
 *        Not implemented for MED_ALL_ELEMENTS 
*         implemented for MED_ALL_ENTITIES
 */
//=============================================================================
CORBA::Long GMESH_i::getNumberOfElements(SALOME_MED::medEntityMesh entity, 
                                        SALOME_MED::medGeometryElement geomElement)
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        if (verifieParam(entity,geomElement)==false)
                THROW_SALOME_CORBA_EXCEPTION("parameters don't match",\
                                             SALOME::BAD_PARAM);
        
        try
        {
//           if ( geomElement == SALOME_MED::MED_POLYGON ||
//                geomElement == SALOME_MED::MED_POLYHEDRA )
//             return _mesh->getNumberOfElementsWithPoly(convertIdlEntToMedEnt(entity),
//                                                       convertIdlEltToMedElt(geomElement));
//           else
            return _mesh->getNumberOfElements(convertIdlEntToMedEnt(entity),
                                              convertIdlEltToMedElt(geomElement));
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces number of elements");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}

//=============================================================================
/*!
 * CORBA: Returns number of families within the mesh
 */
//=============================================================================
CORBA::Long GMESH_i::getNumberOfFamilies(SALOME_MED::medEntityMesh entity)
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _mesh->getNumberOfFamilies(convertIdlEntToMedEnt(entity));
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces number of families of the mesh");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Returns number of groups within the mesh
 */
//=============================================================================
CORBA::Long GMESH_i::getNumberOfGroups(SALOME_MED::medEntityMesh entity) 
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                return _mesh->getNumberOfGroups(convertIdlEntToMedEnt(entity));
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces number of groups of the mesh");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: Returns references for families within the mesh
 */
//=============================================================================
SALOME_MED::Family_array * GMESH_i::getFamilies(SALOME_MED::medEntityMesh entity) 
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        SALOME_MED::Family_array_var myseq = new SALOME_MED::Family_array;
        try
        {
                int nbfam= _mesh->getNumberOfFamilies(convertIdlEntToMedEnt(entity));
                myseq->length(nbfam);
                vector<FAMILY*> fam(nbfam);
                fam = _mesh->getFamilies(convertIdlEntToMedEnt(entity));
                for (int i=0;i<nbfam;i++)
                {
                        FAMILY_i * f1=new FAMILY_i(fam[i]);
                        myseq[i] = f1->_this();
                }
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces families of the mesh");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Returns Coordinates global informations
 */
//=============================================================================
SALOME_MED::GMESH::coordinateInfos *  GMESH_i::getCoordGlobal()
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        SALOME_MED::GMESH::coordinateInfos_var all = new SALOME_MED::GMESH::coordinateInfos;
        try
        {
                all->coordSystem = CORBA::string_dup(_mesh->getCoordinatesSystem().c_str());

                int spaceDimension=_mesh->getSpaceDimension();
                const string * coordinatesUnits =_mesh->getCoordinatesUnits();
                const string * coordinatesName =_mesh->getCoordinatesNames();

                all->coordUnits.length(spaceDimension);
                all->coordNames.length(spaceDimension);
                for (int i=0; i<spaceDimension; i++)
                {
                         all->coordUnits[i]=CORBA::string_dup(coordinatesUnits[i].c_str());
                         all->coordNames[i]=CORBA::string_dup(coordinatesName[i].c_str());
                }
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces coordinate information ");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return all._retn();
}

//=============================================================================
/*!
 * CORBA: Returns references for family i within the mesh
 */
//=============================================================================
SALOME_MED::FAMILY_ptr GMESH_i::getFamily(SALOME_MED::medEntityMesh entity,
                                         CORBA::Long i) 
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                const FAMILY * fam = _mesh->getFamily(convertIdlEntToMedEnt(entity),i);
                FAMILY_i * f1=new FAMILY_i(fam);
                return f1->_this();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces specified family of the mesh");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }

}
//=============================================================================
/*
 * CORBA: Returns Mesh global informations
 */
//=============================================================================
SALOME_MED::GMESH::meshInfos *  GMESH_i::getMeshGlobal()
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        SALOME_MED::GMESH::meshInfos_var all = new SALOME_MED::GMESH::meshInfos;
        try
        {
                all->name = CORBA::string_dup(_mesh->getName().c_str());
                all->spaceDimension = _mesh->getSpaceDimension();
                all->meshDimension  = _mesh->getMeshDimension();
                all->numberOfNodes  = _mesh->getNumberOfNodes();
                all->isAGrid        = _mesh->getIsAGrid();

                int nbFam= _mesh->getNumberOfFamilies(MED_NODE);
                all->famNode.length(nbFam);
                vector<FAMILY*> vNode (nbFam);
                vNode = _mesh->getFamilies(MED_NODE);
                for (int i=0;i<nbFam;i++)
                {
                        FAMILY_i * f1=new FAMILY_i(vNode[i]);
                        all->famNode[i] = f1->_this();
                }

                nbFam = _mesh->getNumberOfFamilies(MED_EDGE);
                all->famEdge.length(nbFam);
                vector<FAMILY*> vEdge (nbFam);
                vEdge = _mesh->getFamilies(MED_EDGE);
                for (int i=0;i<nbFam;i++)
                for (int i=0;i<nbFam;i++)
                {
                        FAMILY_i * f1=new FAMILY_i(vEdge[i]);
                        all->famEdge[i] = f1->_this();
                }

                nbFam = _mesh->getNumberOfFamilies(MED_FACE);
                all->famFace.length(nbFam);
                vector<FAMILY*> vFace (nbFam);
                vFace = _mesh->getFamilies(MED_FACE);
                for (int i=0;i<nbFam;i++)
                {
                        FAMILY_i * f1=new FAMILY_i(vFace[i]);
                        all->famFace[i] = f1->_this();
                }

                nbFam = _mesh->getNumberOfFamilies(MED_CELL);
                all->famCell.length(nbFam);
                vector<FAMILY*> vCell (nbFam);
                vCell = _mesh->getFamilies(MED_CELL);
                for (int i=0;i<nbFam;i++)
                {
                        FAMILY_i * f1=new FAMILY_i(vCell[i]);
                        all->famCell[i] = f1->_this();
                }

                int nbGroup = _mesh->getNumberOfGroups(MED_NODE);
                all->groupNode.length(nbGroup);
                vector<GROUP*> gNode (nbGroup);
                gNode = _mesh->getGroups(MED_NODE);
                for (int i=0;i<nbGroup;i++)
                {
                        GROUP_i * f1=new GROUP_i(gNode[i]);
                        all->groupNode[i] = f1->_this();
                }

                nbGroup = _mesh->getNumberOfGroups(MED_EDGE);
                all->groupEdge.length(nbGroup);
                vector<GROUP*> gEdge (nbGroup);
                gEdge = _mesh->getGroups(MED_EDGE);
                for (int i=0;i<nbGroup;i++)
                {
                        GROUP_i * f1=new GROUP_i(gEdge[i]);
                        all->groupEdge[i] = f1->_this();
                }
                nbGroup = _mesh->getNumberOfGroups(MED_FACE);
                all->groupFace.length(nbGroup);
                vector<GROUP*> gFace (nbGroup);
                gFace = _mesh->getGroups(MED_FACE);
                for (int i=0;i<nbGroup;i++)
                {
                        GROUP_i * f1=new GROUP_i(gFace[i]);
                        all->groupFace[i] = f1->_this();
                }

                nbGroup = _mesh->getNumberOfGroups(MED_CELL);
                all->groupCell.length(nbGroup);
                vector<GROUP*> gCell (nbGroup);
                gCell = _mesh->getGroups(MED_CELL);
                for (int i=0;i<nbGroup;i++)
                {
                        GROUP_i * f1=new GROUP_i(gCell[i]);
                        all->groupCell[i] = f1->_this();
                }

        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces mesh");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return all._retn();
}
//=============================================================================
 /*
 * CORBA: Returns references for groups within the mesh
 */
//=============================================================================
SALOME_MED::Group_array *  GMESH_i::getGroups(SALOME_MED::medEntityMesh entity) 
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        SALOME_MED::Group_array_var myseq = new SALOME_MED::Group_array;
        try
        {
                int nbgroups = _mesh->getNumberOfGroups(convertIdlEntToMedEnt(entity));
                myseq->length(nbgroups);
                vector<GROUP*> groups(nbgroups);
                groups = _mesh->getGroups(convertIdlEntToMedEnt(entity));
                for (int i=0;i<nbgroups;i++)
                {
                        GROUP_i * f1=new GROUP_i(groups[i]);
                        myseq[i] = f1->_this();
                }
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces number of groups of the mesh");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
        return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Returns references for group i within the mesh
 */
//=============================================================================
SALOME_MED::GROUP_ptr GMESH_i::getGroup(SALOME_MED::medEntityMesh entity,
                                       CORBA::Long i) 
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                              SALOME::INTERNAL_ERROR);
        try
        {
                const GROUP * grou = _mesh->getGroup(convertIdlEntToMedEnt(entity),i);
                GROUP_i * f1=new GROUP_i(grou);
                return f1->_this();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to acces specified group of the mesh");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA:
 */
//=============================================================================
SALOME_MED::SUPPORT_ptr GMESH_i::getBoundaryElements(SALOME_MED::medEntityMesh entity)
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                              SALOME::INTERNAL_ERROR);
        try
        {
                SUPPORT * myNewSupport = _mesh->getBoundaryElements(convertIdlEntToMedEnt(entity));
                SUPPORT_i * mySupportI = new SUPPORT_i(myNewSupport);
                return mySupportI->_this();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get the volume ");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}

//=============================================================================
/*!
 * CORBA:
 */
//=============================================================================
SALOME_MED::SUPPORT_ptr GMESH_i::getSupportOnAll(SALOME_MED::medEntityMesh entity)
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                              SALOME::INTERNAL_ERROR);
        try
        {
          const SUPPORT * myNewSupport = _mesh->getSupportOnAll(convertIdlEntToMedEnt(entity));
          SUPPORT_i * mySupportI = new SUPPORT_i(myNewSupport);
          return mySupportI->_this();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get the support ");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: 
 */
//=============================================================================
SALOME_MED::FIELD_ptr GMESH_i::getVolume(SALOME_MED::SUPPORT_ptr mySupport)
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                              SALOME::INTERNAL_ERROR); try
        {
                int sup = mySupport->getCorbaIndex();
                ASSERT(SUPPORT_i::supportMap.find(sup)!=SUPPORT_i::supportMap.end());
                const SUPPORT * myCppSupport=SUPPORT_i::supportMap[sup];
                ::FIELD<double>*f=_mesh->getVolume( myCppSupport);
                FIELDTEMPLATE_I<double> *medf = new FIELDTEMPLATE_I<double>(f);
                return medf->_this();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get the volume ");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA:
 */
//=============================================================================
SALOME_MED::SUPPORT_ptr GMESH_i::getSkin(SALOME_MED::SUPPORT_ptr mySupport3D)
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                              SALOME::INTERNAL_ERROR);
        try
        {
                int sup = mySupport3D->getCorbaIndex();
                ASSERT(SUPPORT_i::supportMap.find(sup)!=SUPPORT_i::supportMap.end());
                const SUPPORT * myCppSupport=SUPPORT_i::supportMap[sup];
                SUPPORT * myNewSupport = _mesh->getSkin(myCppSupport);
                SUPPORT_i * mySupportI = new SUPPORT_i(myNewSupport);
                return mySupportI->_this() ;
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get the volume ");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: 
 */
//=============================================================================
SALOME_MED::FIELD_ptr GMESH_i::getArea(SALOME_MED::SUPPORT_ptr mySupport)
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                              SALOME::INTERNAL_ERROR);
        try
        {
                int sup = mySupport->getCorbaIndex();
                ASSERT(SUPPORT_i::supportMap.find(sup)!=SUPPORT_i::supportMap.end());
                const SUPPORT * myCppSupport=SUPPORT_i::supportMap[sup];
                ::FIELD<double>*f=_mesh->getArea( myCppSupport);
                FIELDTEMPLATE_I<double> *medf = new FIELDTEMPLATE_I<double>(f);
                return medf->_this();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get the area ");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: 
 */
//=============================================================================
SALOME_MED::FIELD_ptr GMESH_i::getLength(SALOME_MED::SUPPORT_ptr mySupport)
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                              SALOME::INTERNAL_ERROR);
        try
        {
                int sup = mySupport->getCorbaIndex();
                ASSERT(SUPPORT_i::supportMap.find(sup)!=SUPPORT_i::supportMap.end());
                const SUPPORT * myCppSupport=SUPPORT_i::supportMap[sup];
                ::FIELD<double>*f=_mesh->getLength( myCppSupport);
                FIELDTEMPLATE_I<double>  *medf = new FIELDTEMPLATE_I<double>(f);
                return medf->_this();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get the length ");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: 
 */
//=============================================================================
SALOME_MED::FIELD_ptr GMESH_i::getNormal(SALOME_MED::SUPPORT_ptr mySupport)
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                              SALOME::INTERNAL_ERROR);
        try
        {
                int sup = mySupport->getCorbaIndex();
                ASSERT(SUPPORT_i::supportMap.find(sup)!=SUPPORT_i::supportMap.end());
                const SUPPORT * myCppSupport=SUPPORT_i::supportMap[sup];
                ::FIELD<double>*f=_mesh->getNormal( myCppSupport);
                FIELDTEMPLATE_I<double> *medf = new FIELDTEMPLATE_I<double>(f);
                return medf->_this();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get the normal ");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: 
 */
//=============================================================================
SALOME_MED::FIELD_ptr GMESH_i::getBarycenter(SALOME_MED::SUPPORT_ptr mySupport)
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                              SALOME::INTERNAL_ERROR);
        try
        {
                int sup = mySupport->getCorbaIndex();
                ASSERT(SUPPORT_i::supportMap.find(sup)!=SUPPORT_i::supportMap.end());
                const SUPPORT * myCppSupport=SUPPORT_i::supportMap[sup];
                ::FIELD<double>*f=_mesh->getBarycenter( myCppSupport);
                FIELDTEMPLATE_I<double> *medf = new FIELDTEMPLATE_I<double>(f);
                return medf->_this();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to get the barycenter ");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: 
 */
//=============================================================================
SALOME_MED::FIELD_ptr GMESH_i::getNeighbourhood(SALOME_MED::SUPPORT_ptr mySupport)
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                              SALOME::INTERNAL_ERROR);
        MESSAGE("Not Implemented");
        return SALOME_MED::FIELD::_nil();
}
//=============================================================================
/*!
 * CORBA: add the Mesh in the StudyManager 
 */
//=============================================================================
void GMESH_i::addInStudy(SALOMEDS::Study_ptr myStudy,SALOME_MED::GMESH_ptr myIor )
throw (SALOME::SALOME_Exception,SALOMEDS::StudyBuilder::LockProtection)
{
        if ( _meshId != "" )
        {
                MESSAGE("Mesh already in Study");
                    THROW_SALOME_CORBA_EXCEPTION("Mesh already in Study", \
                                 SALOME::BAD_PARAM);
        };

        SALOMEDS::StudyBuilder_var     myBuilder = myStudy->NewBuilder();
        SALOMEDS::GenericAttribute_var anAttr;
        SALOMEDS::AttributeName_var    aName;
        SALOMEDS::AttributeIOR_var     aIOR;

        // Find SComponent labelled 'MED'
        SALOMEDS::SComponent_var medfather = myStudy->FindComponent("MED");
        myBuilder->NewCommand();
        if ( CORBA::is_nil(medfather) ) 
          THROW_SALOME_CORBA_EXCEPTION("SComponent labelled 'Med' not Found",SALOME::INTERNAL_ERROR);

        // Create SObject labelled 'MESH' if it doesn't already exit
//      SALOMEDS::SObject_var medmeshfather = myStudy->FindObject("MEDMESH");
//      if ( CORBA::is_nil(medmeshfather) ) 
//      {
//              MESSAGE("Add Object MEDMESH");
//              medmeshfather = myBuilder->NewObject(medfather);
//              //medmeshfather.Name = "MEDMESH" ;
//                 anAttr = myBuilder->FindOrCreateAttribute(medmeshfather, "AttributeName");
//                 aName = SALOMEDS::AttributeName::_narrow(anAttr);
//                 aName->SetValue("MEDMESH");

//      } ;

        string aPath = "/Med/MEDMESH";
        SALOMEDS::SObject_var medmeshfather = myStudy->FindObjectByPath( aPath.c_str() );
        if ( CORBA::is_nil(medmeshfather) ) 
          {
            MESSAGE("Add Object MEDMESH");

            myBuilder->AddDirectory( aPath.c_str() );
            medmeshfather = myStudy->FindObjectByPath( aPath.c_str() );
          } ;

        aPath += "/" + _mesh->getName();
        SALOMEDS::SObject_var meshSO = myStudy->FindObjectByPath( aPath.c_str());
        bool alreadyPublished = ! CORBA::is_nil( meshSO );
        if ( !alreadyPublished ) {
          MESSAGE("Add a mesh Object under MED/MEDMESH");
          meshSO = myBuilder->NewObject(medmeshfather);

          anAttr = myBuilder->FindOrCreateAttribute(meshSO, "AttributeName");
          aName = SALOMEDS::AttributeName::_narrow(anAttr);
          aName->SetValue(_mesh->getName().c_str());
        }

        ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
        ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting()) ;
        CORBA::ORB_var &orb = init(0,0);
        string iorStr = orb->object_to_string(myIor);
        anAttr = myBuilder->FindOrCreateAttribute(meshSO, "AttributeIOR");
        aIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
        aIOR->SetValue(iorStr.c_str());
        _meshId = meshSO->GetID();
        myBuilder->CommitCommand();

        // register the Corba pointer: increase the referrence count
        MESSAGE("Registering of the Corba Mesh pointer");
        Register();

}
//=============================================================================
/*!
 * CORBA: add the Mesh in the StudyManager
 */
//=============================================================================
void GMESH_i::addInStudy(SALOMEDS::Study_ptr myStudy,SALOME_MED::GMESH_ptr myIor,const string & fileName )
throw (SALOME::SALOME_Exception,SALOMEDS::StudyBuilder::LockProtection)
{
        if ( _meshId != "" )
        {
                MESSAGE("Mesh already in Study");
                    THROW_SALOME_CORBA_EXCEPTION("Mesh already in Study", \
                                 SALOME::BAD_PARAM);
        };

        SALOMEDS::StudyBuilder_var     myBuilder = myStudy->NewBuilder();
        SALOMEDS::GenericAttribute_var anAttr;
        SALOMEDS::AttributeName_var    aName;
        SALOMEDS::AttributeIOR_var     aIOR;
        SALOMEDS::AttributeComment_var aComment;

        // Find SComponent labelled 'Med'
        SALOMEDS::SComponent_var medfather = myStudy->FindComponent("MED");
        if ( CORBA::is_nil(medfather) )
          THROW_SALOME_CORBA_EXCEPTION("SComponent labelled 'Med' not Found",SALOME::INTERNAL_ERROR);

        // Create SObject labelled 'MESH' if it doesn't already exit
        SALOMEDS::SObject_var medmeshfather = myStudy->FindObject("MEDMESH");
        if ( CORBA::is_nil(medmeshfather) )
        {
                MESSAGE("Add Object MEDMESH");
                medmeshfather = myBuilder->NewObject(medfather);
                //medmeshfather.Name = "MEDMESH" ;
                anAttr = myBuilder->FindOrCreateAttribute(medmeshfather, "AttributeName");
                aName = SALOMEDS::AttributeName::_narrow(anAttr);
                aName->SetValue("MEDMESH");

        } ;

        MESSAGE("Add a mesh Object under MED/MEDMESH");
        myBuilder->NewCommand();
        SALOMEDS::SObject_var newObj = myBuilder->NewObject(medmeshfather);

        ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
        ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting()) ;
        CORBA::ORB_var &orb = init(0,0);
        CORBA::String_var iorStr = orb->object_to_string(myIor);
        anAttr = myBuilder->FindOrCreateAttribute(newObj, "AttributeIOR");
        aIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
        aIOR->SetValue(iorStr.in());
        anAttr = myBuilder->FindOrCreateAttribute(newObj, "AttributeName");
        aName = SALOMEDS::AttributeName::_narrow(anAttr);
        aName->SetValue(_mesh->getName().c_str());
        anAttr = myBuilder->FindOrCreateAttribute(newObj, "AttributeComment");
        aComment = SALOMEDS::AttributeComment::_narrow(anAttr);
        aComment->SetValue(fileName.c_str());
        _meshId = newObj->GetID();
        myBuilder->CommitCommand();

        // register the Corba pointer: increase the referrence count
        MESSAGE("Registering of the Corba Mesh pointer");
        Register();

}
//=============================================================================

//=============================================================================
/*!
 * CORBA: write mesh in a med file
 */
//=============================================================================
void GMESH_i::write(CORBA::Long i, const char* driverMeshName)        
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                _mesh->write(i);
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to write the mesh ");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA: read mesh in a med file
 */
//=============================================================================
void GMESH_i::read(CORBA::Long i)             
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                _mesh->read(i);
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to read the mesh ");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA : release driver
 */
//=============================================================================
void GMESH_i::rmDriver(CORBA::Long i)         
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                _mesh->rmDriver();
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to unlink the mesh from the driver ");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
                THROW_SALOME_CORBA_EXCEPTION("Unable to acces Support C++ Object"\
                                                ,SALOME::INTERNAL_ERROR);
        }
}
//=============================================================================
/*!
 * CORBA : attach driver
 */
//=============================================================================
CORBA::Long GMESH_i::addDriver(SALOME_MED::medDriverTypes driverType, 
                              const char* fileName, const char* meshName)
throw (SALOME::SALOME_Exception)
{
        if (_mesh==NULL)
                THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
                                             SALOME::INTERNAL_ERROR);
        try
        {
                int drivernum=_mesh->addDriver(
                                        convertIdlDriverToMedDriver(driverType),
                                        fileName,
                                        meshName);
                return drivernum;
        }
        catch (MEDEXCEPTION &ex)
        {
                MESSAGE("Unable to link the mesh to the driver ");
                THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
        }
}

//=============================================================================
/*!
 * CORBA : Test if this and other aggregate the same MESH using the MESH::operator==
 */
//=============================================================================
CORBA::Boolean GMESH_i::areEquals(SALOME_MED::GMESH_ptr other)
{
  PortableServer::ServantBase *baseServ;
  try {
    baseServ=_default_POA()->reference_to_servant(other);
  }
  catch(...){
    baseServ=0;
  }
  if(baseServ)
    {
     baseServ->_remove_ref();
     GMESH_i *otherServ=dynamic_cast<GMESH_i *>(baseServ);
     return *_mesh==*otherServ->_mesh;
    }
  return false;
}

//================================================================================
/*!
 * \brief Converts this GMESH into MESH
 */
//================================================================================

SALOME_MED::MESH_ptr GMESH_i::convertInMESH() throw (SALOME::SALOME_Exception)
{
  if (_mesh==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh",SALOME::INTERNAL_ERROR);

  const MESH* mesh = _mesh->convertInMESH();
  MESH_i* mesh_i = new MESH_i( const_cast< MESH*> ( mesh ));
  return mesh_i->_this();
}

