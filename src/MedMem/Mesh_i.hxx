//  MED MedMem : MED idl descriptions implementation based on the classes of MEDMEM
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : Mesh_i.hxx
//  Author : EDF 
//  Module : MED
//  $Header: /export/home/CVS/SALOME_ROOT/MED/src/MedMem/Mesh_i.hxx

#ifndef _MED_MESH_I_HXX_
#define _MED_MESH_I_HXX_

#include <map>
#include <string>

#include <SALOMEconfig.h>
//#include "SALOME_Component_i.hxx"
#include CORBA_SERVER_HEADER(MED)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)
//#include "MED.hh"

class MESH;
class MESH_i:
		public POA_SALOME_MED::MESH,
		public PortableServer::RefCountServantBase
//		public Engines_Component_i
{
public :
    static map < int,::MESH *> meshMap;
private :
    static int meshIndex;

private :

protected:
    // C++ object containing values
    ::MESH * const _mesh;
    const int      _corbaIndex;
    string         _meshId;


public:

  // Constructors and associated internal methods
  MESH_i();
  MESH_i(::MESH * const m);
  MESH_i(MESH_i & m);
  ~MESH_i();

  // IDL Methods 
  char * 	  getName() 		throw (SALOME::SALOME_Exception);
  
  CORBA::Long   getSpaceDimension() 	throw (SALOME::SALOME_Exception);

  CORBA::Long   getMeshDimension() 	throw (SALOME::SALOME_Exception);
  
  char * 	  	   getCoordinateSystem() 
    throw (SALOME::SALOME_Exception);
  
  Engines::double_array* getCoordinates(SALOME_MED::medModeSwitch typeSwitch)
    throw (SALOME::SALOME_Exception);
  
  Engines::string_array* getCoordinatesNames() 
    throw (SALOME::SALOME_Exception);
  
  Engines::string_array* getCoordinatesUnits()
    throw (SALOME::SALOME_Exception); 
  
  CORBA::Long            getNumberOfNodes() 
    throw (SALOME::SALOME_Exception);
  
  CORBA::Long            getNumberOfTypes       (SALOME_MED::medEntityMesh entity) 
    throw   (SALOME::SALOME_Exception);
  
  SALOME_MED::medGeometryElement_array* getTypes(SALOME_MED::medEntityMesh entity)
    throw   (SALOME::SALOME_Exception);
  
  CORBA::Long 	   getNumberOfElements (SALOME_MED::medEntityMesh entity,
					SALOME_MED::medGeometryElement geomElement) 
    throw (SALOME::SALOME_Exception);
  
  Engines::long_array*   getConnectivity(SALOME_MED::medModeSwitch typeSwitch, 
					 SALOME_MED::medConnectivity mode, 
					 SALOME_MED::medEntityMesh entity, 
					 SALOME_MED::medGeometryElement geomElement)
    throw (SALOME::SALOME_Exception);
  
  Engines::long_array*   getConnectivityIndex(SALOME_MED::medConnectivity mode,
					      SALOME_MED::medEntityMesh entity)
    throw (SALOME::SALOME_Exception);
  
  CORBA::Long            getElementNumber(SALOME_MED::medConnectivity mode,
					  SALOME_MED::medEntityMesh entity,
					  SALOME_MED::medGeometryElement type,
					  const Engines::long_array& connectivity)
    throw  (SALOME::SALOME_Exception);

  Engines::long_array*   getReverseConnectivity(SALOME_MED::medConnectivity mode)
    throw  (SALOME::SALOME_Exception);

  Engines::long_array*   getReverseConnectivityIndex(SALOME_MED::medConnectivity mode)
    throw        (SALOME::SALOME_Exception);

    // Family and Group
    CORBA::Long        getNumberOfFamilies (SALOME_MED::medEntityMesh entity)
		    		     throw (SALOME::SALOME_Exception);
    CORBA::Long        getNumberOfGroups   (SALOME_MED::medEntityMesh entity) 
		    		     throw (SALOME::SALOME_Exception);
    SALOME_MED::Family_array* getFamilies  (SALOME_MED::medEntityMesh entity) 
		    		     throw (SALOME::SALOME_Exception);
    SALOME_MED::FAMILY_ptr    getFamily    (SALOME_MED::medEntityMesh entity,
				            CORBA::Long i) 
		    		     throw (SALOME::SALOME_Exception);
    SALOME_MED::Group_array*  getGroups    (SALOME_MED::medEntityMesh entity) 
		    		     throw (SALOME::SALOME_Exception);
    SALOME_MED::GROUP_ptr     getGroup     (SALOME_MED::medEntityMesh entity, 
				            CORBA::Long i) 
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
			    SALOME_MED::MESH_ptr myIor)   throw (SALOME::SALOME_Exception, SALOMEDS::StudyBuilder::LockProtection);
    void        addInStudy (SALOMEDS::Study_ptr myStudy, 
			    SALOME_MED::MESH_ptr myIor,
			    const string & fileName)   throw (SALOME::SALOME_Exception, SALOMEDS::StudyBuilder::LockProtection);
    CORBA::Long addDriver  (SALOME_MED::medDriverTypes driverType, 
			    const char* fileName, const char* meshName)
	    	     					  throw (SALOME::SALOME_Exception);
    void        rmDriver   (CORBA::Long i)		  throw (SALOME::SALOME_Exception);
    void        read       (CORBA::Long i)		  throw (SALOME::SALOME_Exception);
    void        write      (CORBA::Long i, const char* driverMeshName)
	    	     					  throw (SALOME::SALOME_Exception);

    //					Cuisine interne
    CORBA::Long getCorbaIndex() throw (SALOME::SALOME_Exception);
    ::MESH * constructConstMesh() const;
};

#endif /* _MED_MESH_I_HXX_ */
