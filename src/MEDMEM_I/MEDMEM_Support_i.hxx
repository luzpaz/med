//=============================================================================
// File      : MEDMEM_Support_i.hxx
// Project   : SALOME
// Author    : EDF 
// Copyright : EDF 2002
// $Header: /export/home/PAL/MED/src/MEDMEM_I/MEDMEM_Support_i.hxx
//=============================================================================

#ifndef _MED_SUPPORT_I_HXX_
#define _MED_SUPPORT_I_HXX_

#include <map>
#include <string>

#include <SALOMEconfig.h>

#include "SALOMEMultiComm.hxx"
#include CORBA_SERVER_HEADER(MED)
#include CORBA_SERVER_HEADER(SALOME_Comm)

namespace MEDMEM {
class SUPPORT;

class SUPPORT_i: public POA_SALOME_MED::SUPPORT,
                public PortableServer::RefCountServantBase,
		public SALOMEMultiComm
{
public :
    static std::map < int,::MEDMEM::SUPPORT *> supportMap;
private :
    static int supportIndex;

protected :

     // C++ object containing values
     const ::MEDMEM::SUPPORT * const _support;
     const int   _corbaIndex ;
     std::string      _supportId ;

public:

    SUPPORT_i();
    ~SUPPORT_i();

    // Constructors and associated internal methods
    SUPPORT_i(const ::MEDMEM::SUPPORT * const s);
    SUPPORT_i(const SUPPORT_i & s);

    // IDL Methods
    char *               getName() 	   throw (SALOME::SALOME_Exception);
    char *               getDescription()  throw (SALOME::SALOME_Exception);
    SALOME_MED::MESH_ptr getMesh() 	   throw (SALOME::SALOME_Exception);
    CORBA::Boolean       isOnAllElements() throw (SALOME::SALOME_Exception);
    SALOME_MED::medEntityMesh getEntity()  throw (SALOME::SALOME_Exception);
    CORBA::Long    	 getNumberOfElements(SALOME_MED::medGeometryElement geomElement) 
					   throw (SALOME::SALOME_Exception);
    CORBA::Long          getNumberOfTypes()
                                           throw (SALOME::SALOME_Exception);
    SALOME_MED::long_array* getNumber(SALOME_MED::medGeometryElement geomElement) 
					   throw (SALOME::SALOME_Exception);
    SALOME::Sender_ptr getSenderForNumber(SALOME_MED::medGeometryElement geomElement) 
                                           throw (SALOME::SALOME_Exception);
    SALOME_MED::long_array* getNumberIndex() 
					   throw (SALOME::SALOME_Exception);
    SALOME::Sender_ptr getSenderForNumberIndex() 
                                           throw (SALOME::SALOME_Exception);
    CORBA::Long          getNumberOfGaussPoint(SALOME_MED::medGeometryElement geomElement) 
					   throw (SALOME::SALOME_Exception);
    SALOME_MED::long_array* getNumbersOfGaussPoint()
                                           throw (SALOME::SALOME_Exception);

    SALOME_MED::medGeometryElement_array* getTypes() 
					   throw (SALOME::SALOME_Exception);

    void getBoundaryElements()             throw (SALOME::SALOME_Exception);


  // Others
  void           addInStudy (SALOMEDS::Study_ptr myStudy, 
			  SALOME_MED::SUPPORT_ptr myIor)
    throw (SALOME::SALOME_Exception, SALOMEDS::StudyBuilder::LockProtection);

  void release();
  //					Cuisine interne
  CORBA::Long 	 getCorbaIndex()   throw (SALOME::SALOME_Exception);
  SALOME_MED::SUPPORT::supportInfos * getSupportGlobal()   throw (SALOME::SALOME_Exception);
};
}

#endif /* _MED_SUPPORT_I_HXX_ */
