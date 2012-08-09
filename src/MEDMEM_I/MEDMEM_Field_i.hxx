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
// File      : MEDMEM_Field_i.hxx
// Project   : SALOME
// Author    : EDF
// $Header   : $
//=============================================================================
//
#ifndef MED_FIELD_I_HXX_
#define MED_FIELD_I_HXX_

#include <MEDMEM_I.hxx>

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
class MEDMEM_I_EXPORT FIELD_i: public virtual POA_SALOME_MED::FIELD,
               public SALOME::GenericObj_i
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
    const int   _corbaIndex;
    string  _FieldId;
    FIELD_i();
    FIELD_i(MEDMEM::FIELD_ * const field, bool ownCppPtr);
    FIELD_i(FIELD_i & f);

public :
    // Associated internal methods
    ~FIELD_i();

    char *                  getName()        throw (SALOME::SALOME_Exception);
    void                    setName(const char* theName)
                                             throw (SALOME::SALOME_Exception);

    char *                  getDescription() throw (SALOME::SALOME_Exception);
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
    CORBA::Boolean getGaussPresence();
    CORBA::Double           getTime()        throw (SALOME::SALOME_Exception);
    CORBA::Long             getCorbaIndex()  throw (SALOME::SALOME_Exception);

    SALOME_TYPES::ListOfString * getComponentsNames()  throw (SALOME::SALOME_Exception);
    SALOME_TYPES::ListOfString * getComponentsUnits()  throw (SALOME::SALOME_Exception);
    SALOME_TYPES::ListOfString * getComponentsDescriptions()  throw (SALOME::SALOME_Exception);

    void addInStudy(SALOMEDS::Study_ptr myStudy,
                    SALOME_MED::FIELD_ptr myIor)
                    throw (SALOME::SALOME_Exception,
                           SALOMEDS::StudyBuilder::LockProtection);

    void addInStudyToComponent(SALOMEDS::SComponent_ptr myComponent,
                               SALOME_MED::FIELD_ptr    myIor)
                               throw (SALOME::SALOME_Exception,
                                      SALOMEDS::StudyBuilder::LockProtection);

    CORBA::Long addDriver (SALOME_MED::medDriverTypes driverType, 
                           const char* fileName, const char* fieldName)
                                          throw (SALOME::SALOME_Exception);
    void        rmDriver  (CORBA::Long i) throw (SALOME::SALOME_Exception);
    void        read      (CORBA::Long i) throw (SALOME::SALOME_Exception);
    void        write     (CORBA::Long i, const char* driverFieldName)
                                          throw (SALOME::SALOME_Exception);
    // Cuisine Interne
    MEDMEM::FIELD_ * constructConstField() const;

    // Publish MED Component
    SALOMEDS::SComponent_ptr PublishMedComponent(SALOMEDS::Study_ptr theStudy);

    // Return a default path to publish this field
    std::string getEntryPath ();

 };
}

//using namespace MEDMEM;

#endif /* MED_FIELD_I_HXX_ */
