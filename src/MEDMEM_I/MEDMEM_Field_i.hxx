//=============================================================================
// File      : MEDMEM_Field_i.hxx
// Project   : SALOME
// Author    : EDF
// Copyright : EDF 2002
// $Header: /export/home/PAL/MED_SRC/src/MEDMEM_I/MEDMEM_Field_i.hxx
//=============================================================================


#ifndef MED_FIELD_I_HXX_
#define MED_FIELD_I_HXX_

#include <map>
#include <string>
#include <sstream>

#include "Utils_CorbaException.hxx"
#include <SALOMEconfig.h>

# include "Utils_ORB_INIT.hxx"
# include "Utils_SINGLETON.hxx"

#include CORBA_SERVER_HEADER(MED)
#include CORBA_SERVER_HEADER(SALOMEDS)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

#include "MEDMEM_Support_i.hxx"

#include "MEDMEM_convert.hxx"

#include "MEDMEM_Support.hxx"
#include "MEDMEM_Field.hxx"

namespace MEDMEM {
class FIELD_i: public virtual POA_SALOME_MED::FIELD,
	       public PortableServer::RefCountServantBase
{
public :
        static map < int, ::MEDMEM::FIELD_ * > fieldMap ;
protected :
        static int fieldIndex ;
        bool _ownCppPtr;
protected :
    // C++ object containing values and Constructor
    // are protected because it is not supposed to be instancied

    ::MEDMEM::FIELD_ * const  _fieldTptr;
    const int	_corbaIndex;
    string  _FieldId;
    FIELD_i();
    FIELD_i(MEDMEM::FIELD_ * const field, bool ownCppPtr);
    FIELD_i(FIELD_i & f);

public :
    // Associated internal methods
    ~FIELD_i();

    char *   		    getName() 	     throw (SALOME::SALOME_Exception);
    char *   		    getDescription() throw (SALOME::SALOME_Exception);
    SALOME_MED::SUPPORT_ptr getSupport()     throw (SALOME::SALOME_Exception);
    CORBA::Long             getNumberOfComponents()   	   
					     throw (SALOME::SALOME_Exception);
    char *                  getComponentName(CORBA::Long i)   
					     throw (SALOME::SALOME_Exception);
    char *                  getComponentUnit(CORBA::Long i)   
					     throw (SALOME::SALOME_Exception);
    char *                  getComponentDescription(CORBA::Long i)   
					     throw (SALOME::SALOME_Exception);
    CORBA::Long             getIterationNumber()   	       
					     throw (SALOME::SALOME_Exception);
    CORBA::Long             getOrderNumber() throw (SALOME::SALOME_Exception);
    CORBA::Double           getTime()        throw (SALOME::SALOME_Exception);
    CORBA::Long             getCorbaIndex()  throw (SALOME::SALOME_Exception);

    SALOME_MED::string_array * getComponentsNames()  throw (SALOME::SALOME_Exception);
    SALOME_MED::string_array * getComponentsUnits()  throw (SALOME::SALOME_Exception);
    SALOME_MED::string_array * getComponentsDescriptions()  throw (SALOME::SALOME_Exception);
    void addInStudy(SALOMEDS::Study_ptr myStudy, 
		    SALOME_MED::FIELD_ptr myIor)  
		    throw (SALOME::SALOME_Exception, 
                           SALOMEDS::StudyBuilder::LockProtection);

    CORBA::Long addDriver (SALOME_MED::medDriverTypes driverType, 
			   const char* fileName, const char* fieldName)
                    	                  throw (SALOME::SALOME_Exception);
    void        rmDriver  (CORBA::Long i) throw (SALOME::SALOME_Exception);
    void        read      (CORBA::Long i) throw (SALOME::SALOME_Exception);
    void        write     (CORBA::Long i, const char* driverFieldName)
                    		          throw (SALOME::SALOME_Exception);
  void release();
    // Cuisine Interne
    MEDMEM::FIELD_ * constructConstField() const;

 };
}

//using namespace MEDMEM;

#endif /* MED_FIELD_I_HXX_ */
