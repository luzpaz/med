//=============================================================================
// File      : MEDMEM_Mesh_i.hxx
// Project   : SALOME
// Author    : EDF 
// Copyright : EDF 2002
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_Mesh_i.hxx
//=============================================================================

#ifndef _MED_MESH_I_HXX_
#define _MED_MESH_I_HXX_

#include <map>
#include <string>

#include <SALOMEconfig.h>
#include "SALOMEMultiComm.hxx"
#include CORBA_SERVER_HEADER(MED)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)
#include CORBA_SERVER_HEADER(SALOME_Comm)

namespace MEDMEM {
class MESH;
class MESH_i: public POA_SALOME_MED::MESH,
	      public PortableServer::RefCountServantBase,
	      public SALOMEMultiComm
{
public :
    static std::map < int,::MEDMEM::MESH *> meshMap;
private :
    static int meshIndex;

private :

protected:
    // C++ object containing values
    ::MEDMEM::MESH * const _mesh;
    const int      _corbaIndex;
    std::string         _meshId;


public:

  // Constructors and associated internal methods
  MESH_i();
  MESH_i(::MEDMEM::MESH * const m);
  MESH_i(MESH_i & m);
  ~MESH_i();

  // IDL Methods 
  char * 	  getName() 		throw (SALOME::SALOME_Exception);
  
  CORBA::Long     getSpaceDimension() 	throw (SALOME::SALOME_Exception);

  CORBA::Long     getMeshDimension() 	throw (SALOME::SALOME_Exception);
  
  CORBA::Boolean  getIsAGrid() 		throw (SALOME::SALOME_Exception);

  CORBA::Boolean  existConnectivity
		      ( SALOME_MED::medConnectivity connectivityType,
			SALOME_MED::medEntityMesh entity)
 			 throw (SALOME::SALOME_Exception);

  char * 	  getCoordinatesSystem() 
    		         throw (SALOME::SALOME_Exception);
  
  SALOME_MED::double_array* getCoordinates(SALOME_MED::medModeSwitch typeSwitch)
    		 	 throw (SALOME::SALOME_Exception);

  SALOME::Sender_ptr getSenderForCoordinates(SALOME_MED::medModeSwitch typeSwitch)
    throw (SALOME::SALOME_Exception);
  
  CORBA::Double  getCoordinate(CORBA::Long Number, CORBA::Long Axis)
                         throw (SALOME::SALOME_Exception);

  SALOME_MED::string_array* getCoordinatesNames() 
                         throw (SALOME::SALOME_Exception);
  
  SALOME_MED::string_array* getCoordinatesUnits()
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

  
  CORBA::Long 	  getNumberOfElements (SALOME_MED::medEntityMesh entity,
				       SALOME_MED::medGeometryElement geomElement) 
                         throw (SALOME::SALOME_Exception);
  
  SALOME_MED::long_array*   getConnectivity(SALOME_MED::medModeSwitch typeSwitch, 
					 SALOME_MED::medConnectivity mode, 
					 SALOME_MED::medEntityMesh entity, 
					 SALOME_MED::medGeometryElement geomElement)
                         throw (SALOME::SALOME_Exception);

  SALOME::Sender_ptr getSenderForConnectivity(SALOME_MED::medModeSwitch typeSwitch, 
					      SALOME_MED::medConnectivity mode, 
					      SALOME_MED::medEntityMesh entity, 
					      SALOME_MED::medGeometryElement geomElement)
                         throw (SALOME::SALOME_Exception);
  
  SALOME_MED::long_array*   getConnectivityIndex(SALOME_MED::medConnectivity mode,
					      SALOME_MED::medEntityMesh entity)
                         throw (SALOME::SALOME_Exception);
  
  SALOME_MED::long_array*   getGlobalNumberingIndex(SALOME_MED::medEntityMesh entity)
                         throw (SALOME::SALOME_Exception);

  CORBA::Long            getElementNumber(SALOME_MED::medConnectivity mode,
					  SALOME_MED::medEntityMesh entity,
					  SALOME_MED::medGeometryElement type,
					  const SALOME_MED::long_array& connectivity)
                         throw  (SALOME::SALOME_Exception);

  SALOME_MED::long_array*   getReverseConnectivity(SALOME_MED::medConnectivity mode)
                         throw  (SALOME::SALOME_Exception);

  SALOME_MED::long_array*   getReverseConnectivityIndex(SALOME_MED::medConnectivity mode)
                         throw        (SALOME::SALOME_Exception);

    // Family and Group
    CORBA::Long        	      getNumberOfFamilies(SALOME_MED::medEntityMesh entity)
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


    // Others
    void        addInStudy (SALOMEDS::Study_ptr myStudy, 
			    SALOME_MED::MESH_ptr myIor)   
			    throw (SALOME::SALOME_Exception, SALOMEDS::StudyBuilder::LockProtection);
    void        addInStudy (SALOMEDS::Study_ptr myStudy, 
			    SALOME_MED::MESH_ptr myIor,
			    const std::string & fileName)   
		            throw (SALOME::SALOME_Exception, SALOMEDS::StudyBuilder::LockProtection);
    CORBA::Long addDriver  (SALOME_MED::medDriverTypes driverType, 
			    const char* fileName, const char* meshName)
	    	     					  throw (SALOME::SALOME_Exception);
    void        rmDriver   (CORBA::Long i)		  throw (SALOME::SALOME_Exception);
    void        read       (CORBA::Long i)		  throw (SALOME::SALOME_Exception);
    void        write      (CORBA::Long i, const char* driverMeshName)
	    	     					  throw (SALOME::SALOME_Exception);

    //					Cuisine interne
    CORBA::Long getCorbaIndex() throw (SALOME::SALOME_Exception);
    ::MEDMEM::MESH * constructConstMesh() const;
    SALOME_MED::MESH::meshInfos * getMeshGlobal()
                                  throw (SALOME::SALOME_Exception);
    SALOME_MED::MESH::coordinateInfos * getCoordGlobal()
                                  throw (SALOME::SALOME_Exception);
    SALOME_MED::MESH::connectivityInfos * getConnectGlobal
				  (SALOME_MED::medEntityMesh entity)
                                  throw (SALOME::SALOME_Exception);

  void release();
};
}

#endif /* _MED_MESH_I_HXX_ */
