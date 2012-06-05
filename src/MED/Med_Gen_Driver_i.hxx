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

//  MED MED : implemetation of MED idl descriptions
//  File   : Med_Gen_Driver_i.hxx
//  Author : Paul RASCLE, EDF
//  Module : MED

#ifndef _MED_GEN_DRIVER_I_HXX_
#define _MED_GEN_DRIVER_I_HXX_

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MED_Gen)
#include CORBA_SERVER_HEADER(MED)
#include CORBA_SERVER_HEADER(SALOME_ModuleCatalog)

#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

#include "SALOME_Component_i.hxx"
#include "SALOME_NamingService.hxx"

#include <map>

namespace MEDMEM {
  class MED_i;
}

class Med_Gen_Driver_i : public virtual POA_SALOME_MED::MED_Gen_Driver
{
private:
  Med_Gen_Driver_i();

public:
  //  Constructor and Destructor
  Med_Gen_Driver_i(CORBA::ORB_ptr orb);
  virtual ~Med_Gen_Driver_i();

  // Get Med object of the study (for persistance)
  static MEDMEM::MED_i* GetMED(SALOMEDS::SComponent_ptr theStudy);

  // inherited methods from SALOMEDS::Driver

  SALOMEDS::TMPFile* Save(SALOMEDS::SComponent_ptr theComponent,
                          const char* theURL,
                          bool isMultiFile);
  SALOMEDS::TMPFile* SaveASCII(SALOMEDS::SComponent_ptr theComponent,
                               const char* theURL,
                               bool isMultiFile);

  CORBA::Boolean Load(SALOMEDS::SComponent_ptr theComponent,
                      const SALOMEDS::TMPFile& theStream,
                      const char* theURL,
                      bool isMultiFile);
  CORBA::Boolean LoadASCII(SALOMEDS::SComponent_ptr theComponent,
                           const SALOMEDS::TMPFile& theStream,
                           const char* theURL,
                           bool isMultiFile);

  void Close(SALOMEDS::SComponent_ptr theComponent);

  char* IORToLocalPersistentID(SALOMEDS::SObject_ptr theSObject,
                               const char* IORString,
                               CORBA::Boolean isMultiFile,
                               CORBA::Boolean isASCII);
  char* LocalPersistentIDToIOR(SALOMEDS::SObject_ptr theSObject,
                               const char* aLocalPersistentID,
                               CORBA::Boolean isMultiFile,
                               CORBA::Boolean isASCII)
    throw(SALOME::SALOME_Exception);

  bool CanPublishInStudy(CORBA::Object_ptr theIOR);

  SALOMEDS::SObject_ptr PublishInStudy(SALOMEDS::Study_ptr theStudy,
                                       SALOMEDS::SObject_ptr theSObject,
                                       CORBA::Object_ptr theObject,
                                       const char* theName) throw (SALOME::SALOME_Exception);

  CORBA::Boolean CanCopy(SALOMEDS::SObject_ptr theObject);
  SALOMEDS::TMPFile* CopyFrom(SALOMEDS::SObject_ptr theObject, CORBA::Long& theObjectID);

  CORBA::Boolean CanPaste(const char* theComponentName, CORBA::Long theObjectID);
  SALOMEDS::SObject_ptr PasteInto(const SALOMEDS::TMPFile& theStream,
                                  CORBA::Long theObjectID,
                                  SALOMEDS::SObject_ptr theObject);

  // this method of SALOMEDS::Driver must be implemented in Component
  //char* ComponentDataType();

  // to be defined in Component
  virtual Engines::EngineComponent_ptr GetComponentInstance() = 0;

private:
  static std::map<std::string, MEDMEM::MED_i*> _MedCorbaObj;

  CORBA::ORB_ptr _driver_orb;

protected:
  SALOME_NamingService *_NS;
};

#endif
