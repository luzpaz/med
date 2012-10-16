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
//  File   : Med_Gen_i.hxx
//  Author : Paul RASCLE, EDF
//  Module : MED

#ifndef _MED_GEN_I_HXX_
#define _MED_GEN_I_HXX_

#include <SALOMEconfig.h>
#include <map>
#include CORBA_SERVER_HEADER(MED_Gen)
#include CORBA_SERVER_HEADER(MED)
#include CORBA_SERVER_HEADER(SALOME_ModuleCatalog)

#include "SALOME_Component_i.hxx"
#include "SALOME_NamingService.hxx"

#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

#include "Med_Gen_Driver_i.hxx"

namespace MEDMEM {
  class MED_i;
}

#ifdef WIN32
 #if defined MED_EXPORTS || defined MEDEngine_EXPORTS
  #if defined WIN32
   #define MED_EXPORT __declspec( dllexport )
  #else
   #define MED_EXPORT
  #endif
 #else
  #if defined WIN32
   #define MED_EXPORT __declspec( dllimport )
  #else
   #define MED_EXPORT
  #endif
 #endif
#else
 #define MED_EXPORT
#endif

class MED_EXPORT Med_Gen_i:
  public POA_SALOME_MED::MED_Gen,
  public Engines_Component_i,
  public Med_Gen_Driver_i
{
private:
  SALOMEDS::Study_var studyName2Study(const char* studyName)
    throw(SALOME::SALOME_Exception);
  void addInStudy(SALOMEDS::Study_var myStudy)
    throw(SALOME::SALOME_Exception);

public:
  //Med_Gen_i();
  Med_Gen_i(CORBA::ORB_ptr orb,
            PortableServer::POA_ptr poa,
            PortableServer::ObjectId * contId,
            const char *instanceName,
            const char *interfaceName);
  virtual ~Med_Gen_i();

  //
  SALOME_MED::GMESH_ptr readMeshInFile(const char* fileName,
                                      const char* studyName,
                                      const char* meshName)
    throw(SALOME::SALOME_Exception);

  SALOME_MED::FIELD_ptr readFieldInFile(const char* fileName,
                                        const char* studyName,
                                        const char* fieldName,
                                        CORBA::Long iter,
                                        CORBA::Long ordre)
    throw(SALOME::SALOME_Exception);

  SALOME_MED::MED_ptr   readStructFile(const char* fileName,
                                       const char* studyName)
    throw(SALOME::SALOME_Exception);

  void    readStructFileWithFieldType(const char* fileName,
                                      const char* studyName)
    throw(SALOME::SALOME_Exception);

  // Get last created instance of the class
  static Med_Gen_i* GetMEDGen() { return _MEDGen; }

  // Get Med of the study (for persistance)
  static SALOME_MED::MED_ptr GetMED(SALOMEDS::SComponent_ptr theStudy);

  // Get object of the CORBA reference
  static PortableServer::ServantBase_var GetServant( CORBA::Object_ptr theObject );

  template<class T>
  static T DownCast(CORBA::Object_ptr theArg)
  {
    return dynamic_cast<T>(GetServant(theArg).in());
  }

  // (re)defined methods of Driver
  char* ComponentDataType();
  virtual Engines::EngineComponent_ptr GetComponentInstance();

  // Version information
  virtual char* getVersion();
  
private:
  //static std::string _myFileName;
  //static std::string _saveFileName;
  static Med_Gen_i*  _MEDGen;    // Point to last created instance of the class

private:
  bool   _duringLoad;
  //int    _myStudyID;
  //std::string _myStudyName;

  //int myCounter;
};

#endif
