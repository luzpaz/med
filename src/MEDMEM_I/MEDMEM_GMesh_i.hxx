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
// File      : MEDMEM_GMesh_i.hxx
// Project   : SALOME
//=============================================================================
//
#ifndef _MED_GMESH_I_HXX_
#define _MED_GMESH_I_HXX_

#include <MEDMEM_I.hxx>

#include <map>
#include <string>

#include <SALOMEconfig.h>
#include "SALOMEMultiComm.hxx"
#include "SALOME_GenericObj_i.hh"
#include CORBA_SERVER_HEADER(MED)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)
#include CORBA_SERVER_HEADER(SALOME_Comm)

namespace MEDMEM {
class GMESH;
class MEDMEM_I_EXPORT GMESH_i: virtual public POA_SALOME_MED::GMESH,
              virtual public SALOMEMultiComm,
              virtual public SALOME::GenericObj_i
{
public :
    static std::map < int,::MEDMEM::GMESH *> meshMap;
private :
    static int meshIndex;

private :

protected:
    // C++ object containing values
    ::MEDMEM::GMESH * const _mesh;
    const int               _corbaIndex;
    std::string             _meshId;


public:

  // Constructors and associated internal methods
  GMESH_i();
  GMESH_i(::MEDMEM::GMESH * const m);
  GMESH_i(GMESH_i & m);
  ~GMESH_i();

  // IDL Methods 
  char *          getName()             throw (SALOME::SALOME_Exception);
  
  CORBA::Long     getSpaceDimension()   throw (SALOME::SALOME_Exception);

  CORBA::Long     getMeshDimension()    throw (SALOME::SALOME_Exception);
  
  CORBA::Boolean  getIsAGrid()          throw (SALOME::SALOME_Exception);

  char *          getCoordinatesSystem() 
                         throw (SALOME::SALOME_Exception);
  
  SALOME_TYPES::ListOfString* getCoordinatesNames() 
                         throw (SALOME::SALOME_Exception);
  
  SALOME_TYPES::ListOfString* getCoordinatesUnits()
                         throw (SALOME::SALOME_Exception); 
  
  CORBA::Long     getNumberOfNodes() 
                         throw (SALOME::SALOME_Exception);
  
  CORBA::Long     getNumberOfTypes       (SALOME_MED::medEntityMesh entity) 
                         throw   (SALOME::SALOME_Exception);
  
  SALOME_MED::medGeometryElement_array* getTypes(SALOME_MED::medEntityMesh entity)
                         throw   (SALOME::SALOME_Exception);

  SALOME_MED::medGeometryElement   getElementType(SALOME_MED::medEntityMesh entity,
                                                  CORBA::Long number)
                         throw   (SALOME::SALOME_Exception);

  
  CORBA::Long     getNumberOfElements (SALOME_MED::medEntityMesh entity,
                                       SALOME_MED::medGeometryElement geomElement) 
                         throw (SALOME::SALOME_Exception);
  

    // Family and Group
    CORBA::Long               getNumberOfFamilies(SALOME_MED::medEntityMesh entity)
                                        throw (SALOME::SALOME_Exception);
    CORBA::Long               getNumberOfGroups(SALOME_MED::medEntityMesh entity) 
                                        throw (SALOME::SALOME_Exception);
    SALOME_MED::Family_array* getFamilies(SALOME_MED::medEntityMesh entity) 
                                        throw (SALOME::SALOME_Exception);
    SALOME_MED::FAMILY_ptr    getFamily    (SALOME_MED::medEntityMesh entity,
                                            CORBA::Long i) 
                                        throw (SALOME::SALOME_Exception);
    SALOME_MED::Group_array*  getGroups(SALOME_MED::medEntityMesh entity) 
                                        throw (SALOME::SALOME_Exception);
    SALOME_MED::GROUP_ptr     getGroup (SALOME_MED::medEntityMesh entity, 
                                            CORBA::Long i) 
                                        throw (SALOME::SALOME_Exception);
    // 
    SALOME_MED::SUPPORT_ptr getBoundaryElements(SALOME_MED::medEntityMesh entity)
                                     throw (SALOME::SALOME_Exception);
    SALOME_MED::SUPPORT_ptr getSupportOnAll(SALOME_MED::medEntityMesh entity)
                                     throw (SALOME::SALOME_Exception);
    SALOME_MED::SUPPORT_ptr getSkin        (SALOME_MED::SUPPORT_ptr mySupport3D)
                                     throw (SALOME::SALOME_Exception);
    // 
    SALOME_MED::FIELD_ptr getVolume        (SALOME_MED::SUPPORT_ptr mySupport)
                                     throw (SALOME::SALOME_Exception);
    SALOME_MED::FIELD_ptr getArea          (SALOME_MED::SUPPORT_ptr mySupport)          
                                     throw (SALOME::SALOME_Exception);
    SALOME_MED::FIELD_ptr getLength        (SALOME_MED::SUPPORT_ptr mySupport)        
                                     throw (SALOME::SALOME_Exception);
    SALOME_MED::FIELD_ptr getNormal        (SALOME_MED::SUPPORT_ptr mySupport)        
                                     throw (SALOME::SALOME_Exception);
    SALOME_MED::FIELD_ptr getBarycenter    (SALOME_MED::SUPPORT_ptr mySupport)    
                                     throw (SALOME::SALOME_Exception);
    SALOME_MED::FIELD_ptr getNeighbourhood (SALOME_MED::SUPPORT_ptr mySupport) 
                                     throw (SALOME::SALOME_Exception);

    SALOME_MED::MESH_ptr convertInMESH() throw (SALOME::SALOME_Exception);

    // Others
    void        addInStudy (SALOMEDS::Study_ptr myStudy, 
                            SALOME_MED::GMESH_ptr myIor)   
                            throw (SALOME::SALOME_Exception, SALOMEDS::StudyBuilder::LockProtection);
    void        addInStudy (SALOMEDS::Study_ptr myStudy, 
                            SALOME_MED::GMESH_ptr myIor,
                            const std::string & fileName)   
                            throw (SALOME::SALOME_Exception, SALOMEDS::StudyBuilder::LockProtection);
    CORBA::Long addDriver  (SALOME_MED::medDriverTypes driverType, 
                            const char* fileName, const char* meshName)
                                                          throw (SALOME::SALOME_Exception);
    void        rmDriver   (CORBA::Long i)                throw (SALOME::SALOME_Exception);
    void        read       (CORBA::Long i)                throw (SALOME::SALOME_Exception);
    void        write      (CORBA::Long i, const char* driverMeshName)
                                                          throw (SALOME::SALOME_Exception);

    //                                  Cuisine interne
    CORBA::Long getCorbaIndex() throw (SALOME::SALOME_Exception);
    ::MEDMEM::GMESH * constructConstMesh() const;
    SALOME_MED::GMESH::meshInfos * getMeshGlobal()
                                  throw (SALOME::SALOME_Exception);
    SALOME_MED::GMESH::coordinateInfos * getCoordGlobal()
                                  throw (SALOME::SALOME_Exception);
    CORBA::Boolean areEquals(SALOME_MED::GMESH_ptr other);

};
}

#endif /* _MED_MESH_I_HXX_ */
