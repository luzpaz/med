//=============================================================================
// File      : Support_i.hxx
// Project   : SALOME
// Author    : EDF 
// Copyright : EDF 2002
// $Header: /export/home/CVS/SALOME_ROOT/MED/src/MedMem/Support_i.hxx
//=============================================================================

#ifndef _MED_SUPPORT_I_HXX_
#define _MED_SUPPORT_I_HXX_

#include <map>
#include <string>

#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(MED)

class SUPPORT;
class SALOME_MED::MESH;

class SUPPORT_i: 
		public POA_SALOME_MED::SUPPORT,
                public PortableServer::RefCountServantBase 
{
public :
    static map < int,::SUPPORT *> supportMap;
private :
    static int supportIndex;

protected :

     // C++ object containing values
     const ::SUPPORT * const _support;
     const int   _corbaIndex ;
     string      _supportId ;

public:

    SUPPORT_i();
    ~SUPPORT_i();

    // Constructors and associated internal methods
    SUPPORT_i(const ::SUPPORT * const s);
    SUPPORT_i(const SUPPORT_i & s);

    // IDL Methods
    char *               getName() 	   throw (SALOME::SALOME_Exception);
    char *               getDescription()  throw (SALOME::SALOME_Exception);
    SALOME_MED::MESH_ptr getMesh() 	   throw (SALOME::SALOME_Exception);
    CORBA::Boolean       isOnAllElements() throw (SALOME::SALOME_Exception);
    SALOME_MED::medEntityMesh getEntity()  throw (SALOME::SALOME_Exception);
    CORBA::Long    	 getNumberOfElements(SALOME_MED::medGeometryElement geomElement) 
					   throw (SALOME::SALOME_Exception);
    Engines::long_array* getNumber(SALOME_MED::medGeometryElement geomElement) 
					   throw (SALOME::SALOME_Exception);
    Engines::long_array* getNumberIndex() 
					   throw (SALOME::SALOME_Exception);
    CORBA::Long          getNumberOfGaussPoints(SALOME_MED::medGeometryElement geomElement) 
					   throw (SALOME::SALOME_Exception);
    SALOME_MED::medGeometryElement_array* getTypes() 
					   throw (SALOME::SALOME_Exception);

  // Others
  void           addInStudy (SALOMEDS::Study_ptr myStudy, 
			  SALOME_MED::SUPPORT_ptr myIor)
    throw (SALOME::SALOME_Exception);
  
  //					Cuisine interne
  CORBA::Long 	 getCorbaIndex()   throw (SALOME::SALOME_Exception);
};

#endif /* _MED_SUPPORT_I_HXX_ */
