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
// File      : MEDMEM_Med_i.hxx
// Project   : SALOME
// Author    : EDF
//=============================================================================
//
#ifndef _MED_MED_I_HXX_
#define _MED_MED_I_HXX_

#include <MEDMEM_I.hxx>

#include <string>

#include <SALOMEconfig.h>

#include "SALOME_GenericObj_i.hh"
#include CORBA_SERVER_HEADER(MED)
#include CORBA_SERVER_HEADER(SALOMEDS)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

#include "MEDMEM_MedFileBrowser.hxx"
#include "MEDMEM_GenDriver.hxx"

namespace MEDMEM {
typedef map<DT_IT_, SALOME_MED::FIELD_ptr, LT_DT_IT_ > MAP_IOR_DT_IT_;
  
class GMESH;
class FIELD_;

class MEDMEM_I_EXPORT MED_i: public POA_SALOME_MED::MED,
             public SALOME::GenericObj_i
{

private :

protected:
    // C++ object containing values
  MEDFILEBROWSER                 _med;
  map<string, ::MEDMEM::GMESH* > _medmem_meshes;
  vector< ::MEDMEM::FIELD_* >    _medmem_fields;
  string _medId;

  map<string,SALOME_MED::GMESH_ptr>    _meshes;   // We can't have two MESHes with the same name.  

  map<string,map<MED_EN::medEntityMesh,SALOME_MED::SUPPORT_ptr> > _supports; 

  map<string,MAP_IOR_DT_IT_>          _fields;   // We can't have two FIELDs with the same name. 
  
public:
    // Constructors and associated internal methods
    MED_i();
    ~MED_i();

  // Analyse the file <fileName> by calling readFileStruct
  void init(SALOMEDS::Study_ptr myStudy, driverTypes driverType, const string & fileName);

  // Same method as above but with storage of fields with their type
  void initWithFieldType(SALOMEDS::Study_ptr myStudy, driverTypes driverType,
                         const string & fileName, bool persistence=false);

  void addInStudy(SALOMEDS::Study_ptr myStudy, SALOME_MED::MED_ptr myIor,
                  SALOMEDS::SComponent_ptr mySComp, const char * medObjName)
    throw (SALOME::SALOME_Exception, SALOMEDS::StudyBuilder::LockProtection);

  SALOME_MED::SUPPORT_ptr getSupport(string                meshName,
                                     MED_EN::medEntityMesh entity,
                                     const string          name="")
    throw (SALOME::SALOME_Exception);


    // IDL Methods 
    CORBA::Long             getNumberOfMeshes() throw (SALOME::SALOME_Exception);
    CORBA::Long             getNumberOfFields() throw (SALOME::SALOME_Exception);
    SALOME_TYPES::ListOfString * getMeshNames()      throw (SALOME::SALOME_Exception);
    SALOME_TYPES::ListOfString * getFieldNames()     throw (SALOME::SALOME_Exception);
    SALOME_MED::GMESH_ptr    getMeshByName(const char* meshName) 
                                                throw (SALOME::SALOME_Exception);
    SALOME_MED::GMESH_ptr    getMesh(SALOME_MED::FIELD_ptr fieldPtr) 
                                                throw (SALOME::SALOME_Exception);
    CORBA::Long getFieldNumberOfIteration(const char* fieldName) 
      throw (SALOME::SALOME_Exception);
    SALOME_TYPES::ListOfLong * getFieldIteration(const char* fieldName, CORBA::Long i) 
      throw (SALOME::SALOME_Exception);
    SALOME_TYPES::ListOfLong * getFieldIterations(const char* fieldName) 
      throw (SALOME::SALOME_Exception);
    SALOME_MED::FIELD_ptr   getField(const char* fieldName,
                                     CORBA::Long pasTemps, 
                                     CORBA::Long numOrdre) 
                                                throw (SALOME::SALOME_Exception);
    CORBA::Long             addDriver(SALOME_MED::medDriverTypes driverType, 
                                      const char* fileName) 
                                                throw (SALOME::SALOME_Exception);
    void                    rmDriver(CORBA::Long i) 
                                                throw (SALOME::SALOME_Exception);
    void                    readFileStruct(CORBA::Long i) 
                                                throw (SALOME::SALOME_Exception);
    void                    writeFrom(CORBA::Long i) 
                                                throw (SALOME::SALOME_Exception);
    void                    write(CORBA::Long i) 
                                                throw (SALOME::SALOME_Exception);
    void                    addMesh(SALOME_MED::GMESH_ptr ptrMesh) 
                                                throw (SALOME::SALOME_Exception);
    void                    addField(SALOME_MED::FIELD_ptr ptrField) 
                                                throw (SALOME::SALOME_Exception);
    void                    addInStudy(SALOMEDS::Study_ptr myStudy,
                                       SALOME_MED::MED_ptr myIor) 
                                                throw (SALOME::SALOME_Exception,
                                                       SALOMEDS::StudyBuilder::LockProtection);
};
}

#endif /* _MED_MED_I_HXX_ */
