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
//=============================================================================
//
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

#include <vector>

using namespace MEDMEM;

//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
MESH_i::MESH_i():GMESH_i()
{
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
MESH_i::~MESH_i()
{
}
//=============================================================================
/*!
 * Constructor
 */
//=============================================================================
MESH_i::MESH_i(::MESH * const m ) :GMESH_i(m)
{
}
//=============================================================================
/*!
 * Copy Constructor 
 */
//=============================================================================
MESH_i::MESH_i( MESH_i & m) :GMESH_i(m._mesh)
{
}
//=============================================================================
/*!
 * CORBA: boolean indicating if connectivity exists
 */
//=============================================================================
CORBA::Boolean MESH_i::existConnectivity
(SALOME_MED::medConnectivity connectivityType,
 SALOME_MED::medEntityMesh entity)
  throw (SALOME::SALOME_Exception)
{
  if (_mesh==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", SALOME::INTERNAL_ERROR);
  try
    {
      return ((::MESH*)_mesh)->existConnectivity(connectivityType,
                                                 convertIdlEntToMedEnt(entity));
    }
  catch (MEDEXCEPTION &ex)
    {
      MESSAGE("Unable to acces mesh flag existConnectivity");
      THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
    }
}

//=============================================================================
/*!
 * CORBA: Accessor for a specific coordinate
 */
//=============================================================================
CORBA::Double MESH_i::getCoordinate(CORBA::Long Number, CORBA::Long Axis)
  throw (SALOME::SALOME_Exception)
{
  if (_mesh==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", SALOME::INTERNAL_ERROR);
  try
    {
      return ((::MESH*)_mesh)->getCoordinate(Number,Axis);
    }
  catch (MEDEXCEPTION &ex)
    {
      MESSAGE("Unable to acces this coordinate");
      THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
    }
}
//=============================================================================
/*!
 * CORBA: Accessor for Coordinates
 */
//=============================================================================
SALOME_TYPES::ListOfDouble * MESH_i::getCoordinates(SALOME_MED::medModeSwitch typeSwitch)
  throw (SALOME::SALOME_Exception)
{
  if (_mesh==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", SALOME::INTERNAL_ERROR);
  SALOME_TYPES::ListOfDouble_var myseq = new SALOME_TYPES::ListOfDouble;
  try
    {
      int spaceDimension=((::MESH*)_mesh)->getSpaceDimension();
      int nbNodes=((::MESH*)_mesh)->getNumberOfNodes();
      const double * coordinates =((::MESH*)_mesh)->getCoordinates(
                                                                   convertIdlModeToMedMode(typeSwitch));

      myseq->length(nbNodes*spaceDimension);
      for (int i=0; i<nbNodes*spaceDimension; i++)
        {
          myseq[i]=coordinates[i];
        };
    }
  catch (MEDEXCEPTION &ex)
    {
      MESSAGE("Unable to acces the coordinates");
      THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
    }
  return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: 2nd Accessor for Coordinates
 */
//=============================================================================
SALOME::SenderDouble_ptr MESH_i::getSenderForCoordinates(SALOME_MED::medModeSwitch typeSwitch)
  throw (SALOME::SALOME_Exception)
{
  if (_mesh==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh",SALOME::INTERNAL_ERROR);
  SALOME::SenderDouble_ptr ret;
  try
    {
      int spaceDimension=((::MESH*)_mesh)->getSpaceDimension();
      int nbNodes=((::MESH*)_mesh)->getNumberOfNodes();
      const double * coordinates =((::MESH*)_mesh)->getCoordinates(convertIdlModeToMedMode(typeSwitch));
      ret=SenderFactory::buildSender(*this,coordinates,nbNodes*spaceDimension);
    }
  catch (MEDEXCEPTION &ex)
    {
      MESSAGE("Unable to acces the coordinates");
      THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
    }
  catch(MultiCommException &ex2)
    THROW_SALOME_CORBA_EXCEPTION(ex2.what(),SALOME::INTERNAL_ERROR);
  return ret;
}
//=============================================================================
/*!
 * CORBA: Accessor for connectivities
 */
//=============================================================================
SALOME_TYPES::ListOfLong *  MESH_i::getConnectivity(SALOME_MED::medConnectivity mode, 
                                                    SALOME_MED::medEntityMesh entity, 
                                                    SALOME_MED::medGeometryElement geomElement)
  throw (SALOME::SALOME_Exception)
{
  if (_mesh==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", SALOME::INTERNAL_ERROR);
  if (verifieParam(entity,geomElement)==false)
    THROW_SALOME_CORBA_EXCEPTION("parameters don't match", SALOME::BAD_PARAM);
  SALOME_TYPES::ListOfLong_var myseq= new SALOME_TYPES::ListOfLong;
  try
    {
      int nbelements; 
      int elt1 = ((::MESH*)_mesh)->getNumberOfElements(
                                                       convertIdlEntToMedEnt(entity),
                                                       convertIdlEltToMedElt(geomElement));
      SCRUTE(elt1);
      if ( mode == SALOME_MED::MED_DESCENDING)
        {
          MESSAGE("MED_DESCENDING");
          int elt2 =(((::MESH*)_mesh)->getCellsTypes(MED_CELL))->getNumberOfConstituents(1);
          nbelements= elt2 * elt1;
          SCRUTE(elt2);
        }
      else
        {
          MESSAGE("MED_NODAL");
          nbelements = ((::MESH*)_mesh)->getConnectivityLength
            (convertIdlConnToMedConn(mode),
             convertIdlEntToMedEnt(entity),
             convertIdlEltToMedElt(geomElement));
        }
      SCRUTE(entity);
      SCRUTE(geomElement);
      SCRUTE(nbelements);
      myseq->length(nbelements);
      const int * numbers=((::MESH*)_mesh)->getConnectivity(convertIdlConnToMedConn(mode),
                                                            convertIdlEntToMedEnt(entity),
                                                            convertIdlEltToMedElt(geomElement));
      for (int i=0;i<nbelements;i++)
        {
          myseq[i]=numbers[i];
        }
    }
  catch (MEDEXCEPTION &ex)
    {
      MESSAGE("Unable to acces connectivities");
      THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
    }
  return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: 2nd Accessor for connectivities
 */
//=============================================================================
SALOME::SenderInt_ptr MESH_i::getSenderForConnectivity(SALOME_MED::medConnectivity mode, 
                                                       SALOME_MED::medEntityMesh entity, 
                                                       SALOME_MED::medGeometryElement geomElement)
  throw (SALOME::SALOME_Exception)
{
  if (_mesh==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", SALOME::INTERNAL_ERROR);
  if (verifieParam(entity,geomElement)==false)
    THROW_SALOME_CORBA_EXCEPTION("parameters don't match", SALOME::BAD_PARAM);
  SALOME::SenderInt_ptr ret;
  try
    {
      int nbelements=((::MESH*)_mesh)->getConnectivityLength(convertIdlConnToMedConn(mode),
                                                             convertIdlEntToMedEnt(entity),
                                                             convertIdlEltToMedElt(geomElement));
      const int * numbers=((::MESH*)_mesh)->getConnectivity(convertIdlConnToMedConn(mode),
                                                            convertIdlEntToMedEnt(entity),
                                                            convertIdlEltToMedElt(geomElement));
      ret=SenderFactory::buildSender(*this,numbers,nbelements);
    }
  catch (MEDEXCEPTION &ex)
    {
      MESSAGE("Unable to acces connectivities");
      THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
    }
  catch(MultiCommException &ex2)
    THROW_SALOME_CORBA_EXCEPTION(ex2.what(),SALOME::INTERNAL_ERROR);
  return ret;
}

//=============================================================================
/*!
 * CORBA: Accessor for connectivities
 */
//=============================================================================
SALOME_TYPES::ListOfLong* MESH_i::getConnectivityIndex(SALOME_MED::medConnectivity mode, 
                                                       SALOME_MED::medEntityMesh entity) 
  throw (SALOME::SALOME_Exception)
{
  if (_mesh==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", SALOME::INTERNAL_ERROR);
  SALOME_TYPES::ListOfLong_var myseq= new SALOME_TYPES::ListOfLong;
  try
    {
      int nbelements = ((::MESH*)_mesh)->getNumberOfElements(
                                                             convertIdlEntToMedEnt(entity),
                                                             MED_ALL_ELEMENTS) + 1;
      myseq->length(nbelements);
      const int * numbers=((::MESH*)_mesh)->getConnectivityIndex(convertIdlConnToMedConn(mode),
                                                                 convertIdlEntToMedEnt(entity));
      for (int i=0;i<nbelements;i++)
        {
          myseq[i]=numbers[i];
        }
    }
  catch (MEDEXCEPTION &ex)
    {
      MESSAGE("Unable to acces connectivities index");
      THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
    }
  return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Accessor for connectivity index
 */
//=============================================================================
SALOME::SenderInt_ptr MESH_i::getSenderForConnectivityIndex(SALOME_MED::medConnectivity mode,
                                                            SALOME_MED::medEntityMesh entity,
                                                            SALOME_MED::medGeometryElement geomElement)
  throw (SALOME::SALOME_Exception)
{
  if (_mesh==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", SALOME::INTERNAL_ERROR);
  SALOME::SenderInt_ptr ret;
  try
    {
      int nbelements=((::MESH*)_mesh)->getNumberOfElements( convertIdlEntToMedEnt(entity),
                                                            convertIdlEltToMedElt(geomElement)) + 1;
      int iType = 0, nbTypes = ((::MESH*)_mesh)->getNumberOfTypes(convertIdlEntToMedEnt(entity));
      const MED_EN::medGeometryElement * types = ((::MESH*)_mesh)->getTypes(convertIdlEntToMedEnt(entity));
      while ( iType < nbTypes && types[iType] != convertIdlEltToMedElt(geomElement))
        ++iType;
      int prev_nbelements = ((::MESH*)_mesh)->getGlobalNumberingIndex( convertIdlEntToMedEnt(entity))[iType]-1;

      const int * numbers=((::MESH*)_mesh)->getConnectivityIndex(convertIdlConnToMedConn(mode),
                                                                 convertIdlEntToMedEnt(entity)) + prev_nbelements;
      ret=SenderFactory::buildSender(*this,numbers,nbelements);
    }
  catch (MEDEXCEPTION &ex)
    {
      MESSAGE("Unable to acces connectivities");
      THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
    }
  catch(MultiCommException &ex2)
    THROW_SALOME_CORBA_EXCEPTION(ex2.what(),SALOME::INTERNAL_ERROR);
  return ret;
}

//=============================================================================
/*!
 * CORBA: Accessor for connectivities
 */
//=============================================================================
SALOME_TYPES::ListOfLong* MESH_i::getGlobalNumberingIndex( SALOME_MED::medEntityMesh entity)
  throw (SALOME::SALOME_Exception)
{
  if (_mesh==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", SALOME::INTERNAL_ERROR);
  SALOME_TYPES::ListOfLong_var myseq= new SALOME_TYPES::ListOfLong;
  try
    {
      int nbelements = ((::MESH*)_mesh)->getNumberOfTypes( convertIdlEntToMedEnt(entity)) + 1;
      myseq->length(nbelements);
      const int * numbers=((::MESH*)_mesh)->getGlobalNumberingIndex( convertIdlEntToMedEnt(entity));
      for (int i=0;i<nbelements;i++)
        {
          myseq[i]=numbers[i];
        }
    }
  catch (MEDEXCEPTION &ex)
    {
      MESSAGE("Unable to acces global index");
      THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
    }
  return myseq._retn();
}

//=============================================================================
/*!
 * CORBA: get global element number in connectivity array
 * not implemented for MED_NODE and MED_ALL_ENTITIES,
 * MED_NONE and MED_ALL_ELEMENTS.
 */
//=============================================================================
CORBA::Long MESH_i::getElementNumber(SALOME_MED::medConnectivity mode,
                                     SALOME_MED::medEntityMesh entity,
                                     SALOME_MED::medGeometryElement type,
                                     const SALOME_TYPES::ListOfLong& connectivity)
  throw (SALOME::SALOME_Exception)
{
  if (_mesh==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", SALOME::INTERNAL_ERROR);
  int numberOfValue = connectivity.length() ;
  int * myConnectivity = new int[numberOfValue] ;
  for (int i=0; i<numberOfValue; i++)
    myConnectivity[i]=connectivity[i] ;

  int result ;
  try
    {
      result = ((::MESH*)_mesh)->getElementNumber(convertIdlConnToMedConn(mode),
                                                  convertIdlEntToMedEnt(entity),
                                                  convertIdlEltToMedElt(type),
                                                  myConnectivity) ;
    }
  catch (MEDEXCEPTION &ex) 
    {
      THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
    }
  return result ;
}
//=============================================================================
/*!
 * CORBA: Accessor for Ascendant connectivities
 * not implemented for MED_ALL_ENTITIES and MED_MAILLE
 */
//=============================================================================
SALOME_TYPES::ListOfLong* MESH_i::getReverseConnectivity(SALOME_MED::medConnectivity mode)
  throw (SALOME::SALOME_Exception)
{
  if (_mesh==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", SALOME::INTERNAL_ERROR);
  SALOME_TYPES::ListOfLong_var myseq= new SALOME_TYPES::ListOfLong;
  try
    {
      int nbelements=((::MESH*)_mesh)->getReverseConnectivityLength(convertIdlConnToMedConn(mode));
      SCRUTE(nbelements);
      myseq->length(nbelements);
      const int * numbers=((::MESH*)_mesh)->getReverseConnectivity(convertIdlConnToMedConn(mode));
      for (int i=0;i<nbelements;i++)
        {
          myseq[i]=numbers[i];
        }
    }
  catch (MEDEXCEPTION &ex)
    {
      MESSAGE("Unable to acces reverse connectivities");
      THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
    }
  return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Accessor for connectivities
 */
//=============================================================================
SALOME_TYPES::ListOfLong* MESH_i::getReverseConnectivityIndex(SALOME_MED::medConnectivity mode)
  throw (SALOME::SALOME_Exception)
{
  if (_mesh==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", SALOME::INTERNAL_ERROR);
  SALOME_TYPES::ListOfLong_var myseq= new SALOME_TYPES::ListOfLong;
  try
    {
      int nbelements=((::MESH*)_mesh)->getReverseConnectivityIndexLength(convertIdlConnToMedConn(mode));
      myseq->length(nbelements);
      const int * numbers=((::MESH*)_mesh)->getReverseConnectivityIndex(convertIdlConnToMedConn(mode));
      for (int i=0;i<nbelements;i++)
        {
          myseq[i]=numbers[i];
        }
    }
  catch (MEDEXCEPTION &ex)
    {
      MESSAGE("Unable to acces reverse connectivities index");
      THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
    }
  return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Returns connectivity global informations
 */
//=============================================================================
SALOME_MED::MESH::connectivityInfos * MESH_i::getConnectGlobal (SALOME_MED::medEntityMesh entity)
  throw (SALOME::SALOME_Exception)
{
  if (_mesh==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", SALOME::INTERNAL_ERROR);
  SALOME_MED::MESH::connectivityInfos_var all=new SALOME_MED::MESH::connectivityInfos;
  try
    {
      MED_EN::medEntityMesh anEntity = convertIdlEntToMedEnt(entity);
      all->numberOfNodes  = ((::MESH*)_mesh)->getNumberOfNodes();

      int nbTypes=((::MESH*)_mesh)->getNumberOfTypes(anEntity);
      const medGeometryElement * types =((::MESH*)_mesh)->getTypes(anEntity);
      all->meshTypes.length(nbTypes);
      all->numberOfElements.length(nbTypes);
      all->nodalConnectivityLength.length(nbTypes);
      all->entityDimension=((::MESH*)_mesh)->getConnectivityptr()->getEntityDimension();
      for (int i=0; i<nbTypes; i++)
        {
          all->meshTypes[i]=convertMedEltToIdlElt(types[i]);
          all->numberOfElements[i]=((::MESH*)_mesh)->getNumberOfElements(anEntity,types[i]);
          all->nodalConnectivityLength[i]=
            ((::MESH*)_mesh)->getConnectivityLength(MED_EN::MED_NODAL,anEntity,types[i]);
        }
    }
  catch (MEDEXCEPTION &ex)
    {
      MESSAGE("Unable to acces connectivities informations");
      THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::INTERNAL_ERROR);
    }
  return all._retn();
}
