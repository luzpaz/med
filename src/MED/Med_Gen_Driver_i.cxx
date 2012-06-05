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
//  File   : Med_Gen_Driver_i.cxx
//  Author : Paul RASCLE, EDF
//  Module : MED
//
#include "Med_Gen_Driver_i.hxx"

#include "MEDMEM_Mesh_i.hxx"
#include "MEDMEM_Med_i.hxx"
#include "MEDMEM_FieldTemplate_i.hxx"
#include "MEDMEM_Support_i.hxx"

#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_MedFieldDriver.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_DriversDef.hxx"


#include "Utils_SINGLETON.hxx"
#include "OpUtil.hxx"
#include "Utils_CorbaException.hxx"
#include "utilities.h"

#include "SALOMEDS_Tool.hxx"

#include <string>
#include <map>

#include <HDFascii.hxx>

using namespace MEDMEM;

// Initialisation des variables statiques
map<string, MEDMEM::MED_i*> Med_Gen_Driver_i::_MedCorbaObj;

//=============================================================================
/*!
 *  default constructor: not for use
 */
//=============================================================================
Med_Gen_Driver_i::Med_Gen_Driver_i()
{
  MESSAGE("Med_Gen_Driver_i::Med_Gen_Driver_i");
}

//=============================================================================
/*!
 *  standard constructor
 */
//=============================================================================
Med_Gen_Driver_i::Med_Gen_Driver_i(CORBA::ORB_ptr orb)
{
  MESSAGE("activate object");
  _driver_orb = CORBA::ORB::_duplicate(orb);

  // get a NamingService interface
  _NS = SINGLETON_<SALOME_NamingService>::Instance();
  ASSERT(SINGLETON_<SALOME_NamingService>::IsAlreadyExisting());
  _NS->init_orb( _driver_orb );
}

//=============================================================================
/*!
 *  Destructor
 */
//=============================================================================
Med_Gen_Driver_i::~Med_Gen_Driver_i()
{
  MESSAGE("Med_Gen_Driver_i::~Med_Gen_Driver_i");
}

//=============================================================================
/*!
 *  GetMED [ static ]
 *
 *  Get Med of the study (for persistance)
 */
//=============================================================================
MEDMEM::MED_i* Med_Gen_Driver_i::GetMED(SALOMEDS::SComponent_ptr theComponent)
{
  // we have a separate MED_i for each component in a study
  SALOMEDS::Study_var study = theComponent->GetStudy();
  ostringstream os;
  os << study->StudyId() << "_" << theComponent->Tag();
  string mapKey = os.str();

  MED_i* med_i;
  map <string, MEDMEM::MED_i*>::iterator id_med;
  id_med = _MedCorbaObj.find( mapKey );
  if ( id_med == _MedCorbaObj.end() )
    _MedCorbaObj[ mapKey ] = med_i = new MED_i();
  else
    med_i = id_med->second;
  return med_i;
}

//=============================================================================
/*!
 *  CORBA: Save Mesh objects (called when a study is saved)
 */
//=============================================================================
namespace {

  // names to use instead of true and false
  enum { SAVE = 1, RESTORE = 0, ASCII = 1, NON_ASCII = 0 };

  //================================================================================
  /*!
   * \brief Return path and base name of a file to store med data in
    * \param theStudy - study being stored or restored
    * \param theURL - path to file storing the study
    * \param isMultiFile - storage mode
    * \param isSave - action kind: SAVE or RESTORE
    * \retval pair<string,string> - path and file base name
   */
  //================================================================================
  pair<string,string> getPersistanceDirAndFileName(SALOMEDS::SComponent_ptr theComponent,
                                                   const char*              theURL,
                                                   const bool               isMultiFile,
                                                   const bool               isSave)
  {
    string path, file;
    CORBA::String_var compName = theComponent->ComponentDataType();
    if (isMultiFile) {
      // file constantly holding data
      path = theURL;
      SALOMEDS::Study_var study = theComponent->GetStudy();
      file = SALOMEDS_Tool::GetNameFromPath( study->URL() );
      file += string( "_" ) + string( compName ) + ".med";
    }
    else {
      // temporary file
      path = SALOMEDS_Tool::GetTmpDir();
      if ( strcmp( "MED", compName ) != 0 )
        file = string( compName ) + "_"; // not MED
      file += "tmp.med";
    }
    return make_pair ( path, file );
  }

  //================================================================================
  /*!
   * \brief Save med objects published in a study
    * \param theStudy - study to store
    * \param theURL - path to store
    * \param isMultiFile - store mode
    * \param isAscii - store mode: ASCII or NON_ASCII
    * \retval SALOMEDS::TMPFile* - result file
   */
  //================================================================================
  SALOMEDS::TMPFile* saveStudy (SALOMEDS::SComponent_ptr theComponent,
                                const char*              theURL,
                                bool                     isMultiFile,
                                bool                     isAscii)
  {
    // Write all MEDMEM objects in one med file because of problems with
    // reference to external mesh when writting field in a separate file.
    // Actually, writting is OK, but when reading a field, its support
    // is updated using mesh data missing in the file being read

    // If arises a problem of meshes or other objects having equal names,
    // the solution can be in renaming them using study entry before writting
    // and renaming back during restoration, real names will be stored in
    // LocalPersistentID's for example

    if (CORBA::is_nil(theComponent)) {
      SALOMEDS::TMPFile_var aStreamFile;
      return aStreamFile._retn();
    }

    SALOMEDS::Study_var study = theComponent->GetStudy();

    pair<string,string> aDir_aFileName =
      getPersistanceDirAndFileName(theComponent, theURL, isMultiFile, SAVE );
    string& aPath     = aDir_aFileName.first;
    string& aBaseName = aDir_aFileName.second;
    string aFile      = aPath + aBaseName;

    SALOMEDS::ListOfFileNames_var aSeq = new SALOMEDS::ListOfFileNames;
    aSeq->length(1);
    aSeq[0] = CORBA::string_dup(aBaseName.c_str());

    if (isMultiFile) { // remove existing file
      //cout << "-----------------Remove " << aPath<< ", "<<aBaseName << endl;
      SALOMEDS_Tool::RemoveTemporaryFiles(aPath.c_str(), aSeq.in(), true);
    }

    // First save fields and their meshes and then not saved meshes

    set< ::MEDMEM::GMESH* > savedMeshes;
    {
      SALOMEDS::ChildIterator_var anIter = study->NewChildIterator(theComponent);
      anIter->InitEx(1);
      for (; anIter->More(); anIter->Next()) {
        SALOMEDS::SObject_var aSO = anIter->Value();
        SALOME_MED::FIELD_var myField = SALOME_MED::FIELD::_narrow( aSO->GetObject() );
        if (! CORBA::is_nil(myField)) {
          long driverId = myField->addDriver(SALOME_MED::MED_DRIVER,
                                             aFile.c_str(),
                                             myField->getName());
          myField->write(driverId,"");
          // save mesh
          SALOME_MED::SUPPORT_var sup = myField->getSupport();
          if ( !sup->_is_nil() ) {
            SALOME_MED::GMESH_var mesh = sup->getMesh();
            if ( !mesh->_is_nil() ) {
              CORBA::Long corbaID = mesh->getCorbaIndex();
              ::MEDMEM::GMESH* gmesh = GMESH_i::meshMap[ int(corbaID) ];
              if ( savedMeshes.insert( gmesh ).second ) {
                long driverId = mesh->addDriver(SALOME_MED::MED_DRIVER,
                                                aFile.c_str(),
                                                mesh->getName());
                mesh->write(driverId,"");
              }
            }
          }
        }
      }
    }

    {
      SALOMEDS::ChildIterator_var anIter = study->NewChildIterator(theComponent);
      anIter->InitEx(1);
      for (; anIter->More(); anIter->Next()) {
        SALOMEDS::SObject_var aSO = anIter->Value();
        SALOME_MED::MESH_var myMesh = SALOME_MED::MESH::_narrow( aSO->GetObject() );
        if (! CORBA::is_nil(myMesh)) {
          CORBA::Long corbaID = myMesh->getCorbaIndex();
          ::MEDMEM::GMESH* gmesh = GMESH_i::meshMap[ int(corbaID) ];
          if ( savedMeshes.insert( gmesh ).second ) {
            long driverId = myMesh->addDriver(SALOME_MED::MED_DRIVER,
                                              aFile.c_str(),
                                              myMesh->getName());
            myMesh->write(driverId,"");
          }
        }
      }
    }

    if ( isAscii )
      HDFascii::ConvertFromHDFToASCII( aFile.c_str(), true);

    // Convert a file to the byte stream
    SALOMEDS::TMPFile_var aStreamFile;
    aStreamFile = SALOMEDS_Tool::PutFilesToStream(aPath.c_str(), aSeq.in(), isMultiFile);

    // Remove a tmp file and directory
    if (!isMultiFile) {
      //cout << "-----------------Remove " << aPath<< ", "<<aBaseName << endl;
      SALOMEDS_Tool::RemoveTemporaryFiles(aPath.c_str(), aSeq.in(), true);
    }
    // Return the created byte stream
    return aStreamFile._retn();
  } // end of saveStudy()

  //================================================================================
  /*!
   * \brief Load study contents
    * \param theComponent - component holding med data
    * \param theURL -
    * \param isMultiFile -
    * \param isASCII -
    * \retval char* -
   */
  //================================================================================
  void loadStudy (SALOMEDS::SComponent_ptr theComponent,
                  const SALOMEDS::TMPFile& theStream,
                  const char*              theURL,
                  CORBA::Boolean           isMultiFile,
                  CORBA::Boolean           isASCII)
  {
    SALOMEDS::Study_var study = theComponent->GetStudy();

    // Get file name
    pair<string,string> aDir_aFileName = getPersistanceDirAndFileName
      (theComponent, theURL, isMultiFile, RESTORE);
    string& aPath     = aDir_aFileName.first;
    string& aBaseName = aDir_aFileName.second;
    string aFile      = aPath + aBaseName;

    SALOMEDS::ListOfFileNames_var aSeq =
      SALOMEDS_Tool::PutStreamToFiles(theStream, aPath.c_str(), isMultiFile);

    string aASCIIPath, aASCIIFile;
    if (isASCII)
    {
      aASCIIPath = HDFascii::ConvertFromASCIIToHDF(aFile.c_str());
      aASCIIFile = "hdf_from_ascii.hdf";
      aFile = aASCIIPath + aASCIIFile;
    }

    MED_i* med_i = Med_Gen_Driver_i::GetMED(theComponent);
    SALOME_MED::MED_var med = med_i->_this();

    // Read all meshes with supports and all fields
    try
    {
      //cout << "-----------------Filename " << aFile << endl;
      med_i->initWithFieldType(study, MED_DRIVER, aFile, true);

      // publishing must be done by initWithFieldType according to <persistence> flag
      med_i->addInStudy(study, med, theComponent, 0);
    }
    catch (const std::exception & ex)
    {
      MESSAGE("Exception Interceptee : ");
      SCRUTE(ex.what());
      THROW_SALOME_CORBA_EXCEPTION("Unable to read a hdf file",SALOME::BAD_PARAM);
    }

    // Remove tmp files
    bool keepTmpFiles = getenv( "MEDPERSIST_KEEP_TMP_FILES" ); // DEBUG
    if ( keepTmpFiles )
      cout << "TMP FILE: " << aFile << endl;
    if ( !isMultiFile && !keepTmpFiles ) {
      aSeq->length(1);
      aSeq[0]=CORBA::string_dup(aBaseName.c_str());
      SALOMEDS_Tool::RemoveTemporaryFiles(aPath.c_str(), aSeq.in(), true);
    }
    if (isASCII)
    {
      aSeq->length(1);
      aSeq[0] = CORBA::string_dup(aASCIIFile.c_str());
      //cout << "-----------------Remove " << aASCIIPath<< ", "<<aASCIIFile << endl;
      SALOMEDS_Tool::RemoveTemporaryFiles(aASCIIPath.c_str(), aSeq, true);
    }
  } // end loadStudy()

  //================================================================================
  /*!
   * \brief retrieve filed features from LocalPersistentID
    * \param aLocalPersistentID - ID
    * \param aFieldName - out filed name
    * \param aNumOrdre - out NumOrdre
    * \param anIterNumber - out IterNumber
   */
  //================================================================================
  void getFieldNameAndDtIt (const char*   aLocalPersistentID,
                            string &      aFieldName,
                            CORBA::Long & aNumOrdre,
                            CORBA::Long & anIterNumber)
  {
    //     aLocalPersistentID(("_MEDFIELD_"+ myField->getName() +
    //                       "_ORDRE_"+a.str()+
    //                       "_ITER_"+b.str()+".med"
    int aLPIdLen = strlen(aLocalPersistentID);
    const int _MEDFIELD_Len = strlen("_MEDFIELD_");
    const int _ORDRE_Len    = strlen("_ORDRE_");
    const int _ITER_Len     = strlen("_ITER_");

    // Get field name: look for _ORDRE_ in aLocalPersistentID
    int aFieldNameLen = 0, aFieldNameBeg = _MEDFIELD_Len, _ORDRE_Beg;
    for ( _ORDRE_Beg = aFieldNameBeg; _ORDRE_Beg < aLPIdLen; ++aFieldNameLen,++_ORDRE_Beg )
      if ( strncmp( &aLocalPersistentID[ _ORDRE_Beg ], "_ORDRE_", _ORDRE_Len ) == 0 )
        break;
    aFieldName = string( &(aLocalPersistentID[aFieldNameBeg]), aFieldNameLen);

    // Get orderNumber
    int anOrderNumberBeg = _ORDRE_Beg + _ORDRE_Len;
    aNumOrdre = atoi( & aLocalPersistentID[ anOrderNumberBeg ]);

    // Get iterationNumber: look for _ITER_ in aLocalPersistentID
    int _ITER_Beg = anOrderNumberBeg;
    for ( ; _ITER_Beg < aLPIdLen; ++_ITER_Beg )
      if ( strncmp( &aLocalPersistentID[ _ITER_Beg ], "_ITER_", _ITER_Len ) == 0 )
        break;
    anIterNumber = atoi( & aLocalPersistentID[ _ITER_Beg + _ITER_Len ]);
  }

  //================================================================================
  /*!
   * \brief Retrieve from aLocalPersistentID data to get support from med
    * \param aLocalPersistentID - persistent ID
    * \param type - string "FAMILY", "GROUP" or "SUPPORT
    * \param name - support name
    * \param mesh - mesh name
    * \param entity - support entity
    * \retval bool - true if all data found in aLocalPersistentID
   */
  //================================================================================
  bool getSupportData (string aLocalPersistentID,
                       string & type,
                       string & name,
                       string & mesh,
                       string & entity)
  {
    // aLocalPersistentID contains:
    // _MED_[FAMILY|GROUP|SUPPORT]/support_name/ENS_MAA/mesh_name/ENTITY/entity
    string::size_type slash1Pos = aLocalPersistentID.find("/");
    if ( slash1Pos == aLocalPersistentID.npos ) return false;
    string::size_type ens_maaPos = aLocalPersistentID.find("/ENS_MAA/", slash1Pos);
    if ( ens_maaPos == aLocalPersistentID.npos ) return false;
    string::size_type entityPos = aLocalPersistentID.find("/ENTITY/", ens_maaPos);
    if ( entityPos == aLocalPersistentID.npos ) return false;

    string::size_type medSize = strlen("_MED_");
    string::size_type ens_maaSize = strlen("/ENS_MAA/");
    string::size_type entitySize = strlen("/ENTITY/");

    type = aLocalPersistentID.substr( medSize, slash1Pos - medSize );
    name = aLocalPersistentID.substr( slash1Pos + 1, ens_maaPos - slash1Pos - 1);
    mesh = aLocalPersistentID.substr( ens_maaPos + ens_maaSize,
                                      entityPos - ens_maaPos - ens_maaSize);
    entity = aLocalPersistentID.substr( entityPos + entitySize );
//     cout << aLocalPersistentID << endl
//          << " type: " << type
//          << " name: " << name
//          << " mesh: " << mesh
//          << " entity: " << entity << endl;
    return true;
  }
} // no name namespace

//================================================================================
/*!
 * \brief Save data published under MED component
  * \param theComponent - MED component
  * \param theURL - path to store
  * \param isMultiFile - store mode
  * \retval SALOMEDS::TMPFile* - result file
 */
//================================================================================
SALOMEDS::TMPFile* Med_Gen_Driver_i::Save (SALOMEDS::SComponent_ptr theComponent,
                                           const char* theURL,
                                           bool isMultiFile)
{
  return saveStudy ( theComponent, theURL, isMultiFile, NON_ASCII );
}

//================================================================================
/*!
 * \brief Save data published under MED component in ASCII file
  * \param theComponent - MED component
  * \param theURL - path to store
  * \param isMultiFile - store mode
  * \retval SALOMEDS::TMPFile* - result file
 */
//================================================================================
SALOMEDS::TMPFile* Med_Gen_Driver_i::SaveASCII (SALOMEDS::SComponent_ptr theComponent,
                                                const char* theURL,
                                                bool isMultiFile)
{
  return saveStudy ( theComponent, theURL, isMultiFile, ASCII );
}

//=============================================================================
/*!
 *  CORBA: Load Mesh objects (called when an existing study is opened)
 */
//=============================================================================

CORBA::Boolean Med_Gen_Driver_i::Load (SALOMEDS::SComponent_ptr theComponent,
                                       const SALOMEDS::TMPFile& theStream,
                                       const char* theURL,
                                       bool isMultiFile)
{
  loadStudy ( theComponent, theStream, theURL, isMultiFile, NON_ASCII );

  return true;
}

CORBA::Boolean Med_Gen_Driver_i::LoadASCII (SALOMEDS::SComponent_ptr theComponent,
                                            const SALOMEDS::TMPFile& theStream,
                                            const char* theURL,
                                            bool isMultiFile)
{
  loadStudy ( theComponent, theStream, theURL, isMultiFile, ASCII );
  return true;
}

//=============================================================================
/*!
 *  CORBA:
 */
//=============================================================================
void Med_Gen_Driver_i::Close (SALOMEDS::SComponent_ptr theComponent)
{
  MESSAGE("Med_Gen_Driver_i::Close");
  SALOMEDS::SObject_var aMedMeshFather = theComponent->GetStudy()->FindObject("MEDMESH");
  if (!CORBA::is_nil(aMedMeshFather)) {
    SALOMEDS::ChildIterator_var anIter = theComponent->GetStudy()->NewChildIterator(aMedMeshFather);
    for(; anIter->More(); anIter->Next()) {
      SALOMEDS::SObject_var aSO = anIter->Value();
      SALOMEDS::GenericAttribute_var anAttr;
      if (aSO->FindAttribute(anAttr,"AttributeIOR")) {
        CORBA::Object_var myIOR =
          _driver_orb->string_to_object(SALOMEDS::AttributeIOR::_narrow(anAttr)->Value());
        SALOME_MED::MESH_var myMesh = SALOME_MED::MESH::_narrow(myIOR);
        // here must call method destroy of myMesh, but it not implemented yet
      }
    }
  }
}

//=============================================================================
/*!
 *  CORBA: give a persistent reference of a transient object (for study save)
 */
//=============================================================================
char* Med_Gen_Driver_i::IORToLocalPersistentID (SALOMEDS::SObject_ptr theSObject,
                                                const char* IORString,
                                                CORBA::Boolean isMultiFile,
                                                CORBA::Boolean isASCII)
{
  SCRUTE(IORString);

  if (string(IORString).size()==0) return CORBA::string_dup("_MED");
  // Well, we know where put object (_saveFilename) and we know object (IORString)
  // cast object :
  CORBA::Object_var myIOR = _driver_orb->string_to_object(IORString);

  // MED
  SALOME_MED::MED_var myMed = SALOME_MED::MED::_narrow(myIOR);
  if (! CORBA::is_nil(myMed))
  {
    string str_MedName="_MED Objet Med + /OBJ_MED/";
    return CORBA::string_dup(str_MedName.c_str());
  }

  // MESH
  SALOME_MED::MESH_var myMesh = SALOME_MED::MESH::_narrow(myIOR);
  if (! CORBA::is_nil(myMesh))
  {
    CORBA::String_var aName((string("_MEDMESH_")+ myMesh->getName() + ".med").c_str());
    return aName._retn();
  }

  // SUPPORT
  SALOME_MED::SUPPORT_var mySupport = SALOME_MED::SUPPORT::_narrow(myIOR);
  if (! CORBA::is_nil(mySupport))
  {
    string type, str_SupportName;
    SALOME_MED::FAMILY_var family = SALOME_MED::FAMILY::_narrow(myIOR);
    if ( !family->_is_nil() )
      type = "_MED_FAMILY";
    else {
      SALOME_MED::GROUP_var grp = SALOME_MED::GROUP::_narrow(myIOR);
      if ( !grp->_is_nil() )
        type = "_MED_GROUP";
      else
        type = "_MED_SUPPORT";
    }
    try  {
      ostringstream os;
      os << type << "/" << mySupport->getName();
      os << "/ENS_MAA/" << mySupport->getMesh()->getName();
      os << "/ENTITY/" << mySupport->getEntity();
      str_SupportName = os.str();
    }
    catch(...) {
      MESSAGE("Unable to save the support");
      THROW_SALOME_CORBA_EXCEPTION("Unable to save Field in Med"\
                                   ,SALOME::INTERNAL_ERROR);
    }
    return CORBA::string_dup(str_SupportName.c_str());
  }

  SALOME_MED::FIELD_var myField = SALOME_MED::FIELD::_narrow(myIOR);
  if (! CORBA::is_nil(myField))
  {
    string str_FieldName;
    ostringstream a,b;
    a<< myField->getOrderNumber();
    b<< myField->getIterationNumber();
    CORBA::String_var aName((string("_MEDFIELD_")+ myField->getName() +
                             string("_ORDRE_")+a.str()+
                             string("_ITER_")+b.str() +
                             ".med").c_str());
    return aName._retn();
  }

  //THROW_SALOME_CORBA_EXCEPTION("Unable to save IOR",SALOME::BAD_PARAM);
  return CORBA::string_dup("_MED");
}

//=============================================================================
/*!
 *  CORBA: give a transient reference (when loading an object, opening study)
 */
//=============================================================================
char* Med_Gen_Driver_i::LocalPersistentIDToIOR (SALOMEDS::SObject_ptr theSObject,
                                                const char* aLocalPersistentID,
                                                CORBA::Boolean isMultiFile,
                                                CORBA::Boolean isASCII)
  throw(SALOME::SALOME_Exception)
{
  // all object are restored in Load() if their name in study coincides
  // with a default one generated by object.addInStudy(...)
  CORBA::String_var ior = theSObject->GetIOR();
  bool restoredByLoad = ( ior.in() && strlen( ior ) > 0 );

  if ( !restoredByLoad )
  {
    CORBA::Object_var object;
    SALOMEDS::SComponent_var component = theSObject->GetFatherComponent();
    MED_i* med_i = Med_Gen_Driver_i::GetMED(component);
    SALOME_MED::MED_var med = med_i->_this();

    // MED
    if (strcmp(aLocalPersistentID, "_MED Objet Med + /OBJ_MED/") == 0)
    {
      //object = med;
      object = SALOME_MED::MED::_duplicate(med);
    }
    // MESH
    else if (strncmp(aLocalPersistentID, "_MEDMESH_",9) == 0)
    {
      int aMeshNameLen = strlen(aLocalPersistentID) - 12;
      string aMeshName( &(aLocalPersistentID[9]), aMeshNameLen);
      aMeshName[aMeshNameLen-1] = 0;
      try {
        object = med->getMeshByName( aMeshName.c_str() );
        if ( CORBA::is_nil( object )) {
          aMeshName = healName( aMeshName );
          object = med->getMeshByName( aMeshName.c_str() );
        }
      }
      catch (const std::exception & ex) {
        SCRUTE(ex.what());
        THROW_SALOME_CORBA_EXCEPTION("Unable to find a mesh by name in this file",
                                     SALOME::INTERNAL_ERROR);
      }
    }
    // FIELD
    else if (strncmp(aLocalPersistentID, "_MEDFIELD_",10) == 0)
    {
      // Field Name
      string aFieldName;
      CORBA::Long aNumOrdre, anIterNumber;
      getFieldNameAndDtIt( aLocalPersistentID, aFieldName, aNumOrdre, anIterNumber );
      // Get a field that is already read
      try {
        object = med->getField( aFieldName.c_str(), anIterNumber, aNumOrdre );
        if ( CORBA::is_nil( object )) {
          aFieldName = healName( aFieldName );
          object = med->getField( aFieldName.c_str(), anIterNumber, aNumOrdre );
        }
      }
      catch (const std::exception & ex) {
        SCRUTE(ex.what());
        THROW_SALOME_CORBA_EXCEPTION("Unable to find a field by name in this file",
                                     SALOME::INTERNAL_ERROR);
      }
    }
    // SUPPORT?
    else {
      string type, name, meshName, entity;
      if ( getSupportData( aLocalPersistentID, type, name, meshName, entity ))
      {
        MED_EN::medEntityMesh medEntity( atoi( entity.c_str() ));

        if ( type == "SUPPORT" ) {
          try {
            object = med_i->getSupport( meshName, medEntity );
            if ( CORBA::is_nil( object )) {
              meshName = healName( meshName );
              object = med_i->getSupport( meshName, medEntity );
            }
          }
          catch (const std::exception & ex) {
            SCRUTE(ex.what());
            THROW_SALOME_CORBA_EXCEPTION("Unable to find support in this file",
                                         SALOME::INTERNAL_ERROR);
          }
        }
        else {
          SALOME_MED::GMESH_var mesh;
          try {
            mesh = med->getMeshByName( meshName.c_str() );
            if ( mesh->_is_nil() ) {
              meshName = healName( meshName );
              mesh = med->getMeshByName( meshName.c_str() );
            }
          }
          catch (const std::exception & ex) {
            SCRUTE(ex.what());
            THROW_SALOME_CORBA_EXCEPTION("Unable to find mesh in this file",
                                         SALOME::INTERNAL_ERROR);
          }
          if ( !mesh->_is_nil() ) {
            string healedName = healName( name );
            try {
              if ( type == "FAMILY" ) {
                SALOME_MED::Family_array_var families = mesh->getFamilies( medEntity );
                for ( int i = 0; CORBA::is_nil(object) && i <= (int)families->length(); ++i )
                  if ( families[ i ]->getName() == name ||
                       families[ i ]->getName() == healedName )
                    object = SALOME_MED::FAMILY::_duplicate( families[ i ]);
              }
              else {
                SALOME_MED::Group_array_var groups = mesh->getGroups( medEntity );
                for ( int i = 0; CORBA::is_nil(object) && i <= (int)groups->length(); ++i )
                  if ( groups[ i ]->getName() == name ||
                       groups[ i ]->getName() == healedName )
                    object = SALOME_MED::GROUP::_duplicate( groups[ i ]);
              }
            }
            catch (const std::exception & ex) {
              SCRUTE(ex.what());
              THROW_SALOME_CORBA_EXCEPTION("Unable to find support in this file",
                                           SALOME::INTERNAL_ERROR);
            }
          }
        }
      }
    }
    if ( !CORBA::is_nil(object) )
      ior = _driver_orb->object_to_string( object );
    else
      THROW_SALOME_CORBA_EXCEPTION("Unable to find the object in this file",
                                   SALOME::INTERNAL_ERROR);

  } // !restoredByLoad

  return ior._retn();
}

//=============================================================================
/*!
 *  returns true, if can publish object
 */
//=============================================================================
bool Med_Gen_Driver_i::CanPublishInStudy (CORBA::Object_ptr theIOR)
{
  SALOME_MED::MESH_var aMesh = SALOME_MED::MESH::_narrow(theIOR);
  if ( !aMesh->_is_nil())
    return true;
  SALOME_MED::FIELD_var aField = SALOME_MED::FIELD::_narrow(theIOR);
  if ( !aField->_is_nil())
    return true;
  //SALOME_MED::SUPPORT_var aSupport = SALOME_MED::SUPPORT::_narrow(theIOR);
  //if ( !aSupport->_is_nil())
  //  return true;
  return false;
}

//=============================================================================
/*!
 *  publish the given object
 */
//=============================================================================
SALOMEDS::SObject_ptr Med_Gen_Driver_i::PublishInStudy (SALOMEDS::Study_ptr theStudy,
                                                        SALOMEDS::SObject_ptr theSObject,
                                                        CORBA::Object_ptr theObject,
                                                        const char* theName)
  throw (SALOME::SALOME_Exception)
{
  SALOMEDS::SObject_var aResultSO;

  if (CORBA::is_nil(theObject)) return aResultSO;
  if (theStudy->_is_nil()) return aResultSO;

  SALOMEDS::StudyBuilder_var aBuilder = theStudy->NewBuilder();
  SALOMEDS::SComponent_var aFather = theStudy->FindComponent(ComponentDataType());

  if (aFather->_is_nil()) {
    aFather = aBuilder->NewComponent(ComponentDataType());
    if (aFather->_is_nil()) return aResultSO;

    SALOMEDS::GenericAttribute_var anAttr = aBuilder->FindOrCreateAttribute(aFather, "AttributeName");
    SALOMEDS::AttributeName_var    aName = SALOMEDS::AttributeName::_narrow(anAttr);
    //NRI    aName->SetValue("MED");

    CORBA::Object_var objVarN = _NS->Resolve("/Kernel/ModulCatalog");
    SALOME_ModuleCatalog::ModuleCatalog_var Catalogue =
      SALOME_ModuleCatalog::ModuleCatalog::_narrow(objVarN);
    SALOME_ModuleCatalog::Acomponent_var Comp = Catalogue->GetComponent(ComponentDataType());
    if (!Comp->_is_nil()) {
      aName->SetValue(Comp->componentusername());
    }

    aBuilder->DefineComponentInstance(aFather, GetComponentInstance());
  }

  if (CORBA::is_nil(theSObject)) {
    SALOME_MED::MESH_var aMesh = SALOME_MED::MESH::_narrow(theObject);
    if (!aMesh->_is_nil()) {
      aMesh->addInStudy(theStudy, aMesh);
    }
    else {
      SALOME_MED::FIELD_var aField = SALOME_MED::FIELD::_narrow(theObject);
      if (!aField->_is_nil()) {
        aField->addInStudyToComponent(aFather, aField);
      }
      //else {
      //  SALOME_MED::SUPPORT_var aSupport = SALOME_MED::SUPPORT::_narrow(theObject);
      //  if (!aSupport->_is_nil())
      //    aSupport->addInStudy(theStudy, aSupport);
      //}
    }
    aResultSO = theStudy->FindObjectIOR(_driver_orb->object_to_string(theObject));
  } else {
    //if (!theSObject->ReferencedObject(aResultSO))
    //  THROW_SALOME_CORBA_EXCEPTION("Publish in study MED object error",SALOME::BAD_PARAM);
  }
  //aBuilder->Addreference(theObject, aResultSO);
  return aResultSO._retn();
}

//=============================================================================
/*!
 *  returns true, if can copy the object
 */
//=============================================================================
CORBA::Boolean Med_Gen_Driver_i::CanCopy (SALOMEDS::SObject_ptr theObject)
{
  // Try to retrieve known by MED component mesh by given IOR
  SALOMEDS::GenericAttribute_var anAttr;
  if (!theObject->FindAttribute(anAttr, "AttributeIOR")) return false;
  try {
    CORBA::Object_var anObj =
      _driver_orb->string_to_object(SALOMEDS::AttributeIOR::_narrow(anAttr)->Value());
    SALOME_MED::MESH_var aMesh = SALOME_MED::MESH::_narrow(anObj);
    // If the object is null one it can't be copied: return false
    if (aMesh->_is_nil()) return false;
  } catch(...) {
    return false;
  }
  return true;
}

//=============================================================================
/*!
 *  create copy of the object and put it to the stream
 */
//=============================================================================
SALOMEDS::TMPFile* Med_Gen_Driver_i::CopyFrom (SALOMEDS::SObject_ptr theObject,
                                               CORBA::Long& theObjectID)
{
  // Declare a sequence of the byte to store the copied object
  SALOMEDS::TMPFile_var aStreamFile;

  // Try to get GEOM_Shape object by given SObject
  SALOMEDS::GenericAttribute_var anAttr;
  if (!theObject->FindAttribute(anAttr, "AttributeIOR")) return new SALOMEDS::TMPFile(0);
  CORBA::String_var anIOR = CORBA::string_dup(SALOMEDS::AttributeIOR::_narrow(anAttr)->Value());
  CORBA::Object_var anObj = _driver_orb->string_to_object(anIOR);
  SALOME_MED::MESH_var aMesh = SALOME_MED::MESH::_narrow(anObj);
  if (aMesh->_is_nil()) return new SALOMEDS::TMPFile(0);

  // Get a temporary directory to store a temporary file
  CORBA::String_var aTmpDir = SALOMEDS_Tool::GetTmpDir().c_str();
  // Create a list to store names of created files
  SALOMEDS::ListOfFileNames_var aSeq = new SALOMEDS::ListOfFileNames;
  aSeq->length(1);
  aSeq[0] = CORBA::string_dup(aMesh->getName());
  char* aFullName = new char[strlen(aTmpDir)+strlen(aSeq[0])+1];
  strcpy(aFullName, aTmpDir);
  strcpy(aFullName+strlen(aTmpDir), aSeq[0]);

  long driverId = aMesh->addDriver(SALOME_MED::MED_DRIVER,aFullName , aMesh->getName());
  aMesh->write(driverId,"");

  //  aStreamFile = SALOMEDS_Tool::PutFilesToStream(aTmpDir.c_str(), aSeq.in(), false);
  char* aFullName1 = new char[strlen(aTmpDir)+1];
  strcpy(aFullName1, aTmpDir);
  aStreamFile = SALOMEDS_Tool::PutFilesToStream(aFullName1, aSeq.in(), false);
  //  SALOMEDS_Tool::RemoveTemporaryFiles(aTmpDir.c_str(), aSeq.in(), true);
  SALOMEDS_Tool::RemoveTemporaryFiles(aFullName1, aSeq.in(), true);

  // Assign an ID = 1 the the type SALOME_MED::MESH
  theObjectID = 1;

  return aStreamFile._retn();
}

//=============================================================================
/*!
 *  returns true, if can copy the object
 */
//=============================================================================
CORBA::Boolean Med_Gen_Driver_i::CanPaste (const char* theComponentName, CORBA::Long theObjectID)
{
  // The MED component can paste only objects copied by MED component
  // and with the object type = 1 (mesh)
  if (strcmp(theComponentName, ComponentDataType()) != 0 || theObjectID != 1) return false;
  return true;
}

//=============================================================================
/*!
 *  returns true, if can copy the object
 */
//=============================================================================
SALOMEDS::SObject_ptr Med_Gen_Driver_i::PasteInto (const SALOMEDS::TMPFile& theStream,
                                                   CORBA::Long theObjectID,
                                                   SALOMEDS::SObject_ptr theObject)
{
  SALOMEDS::SObject_var aResultSO = SALOMEDS::SObject::_duplicate(theObject);
  if (theStream.length() == 0) return aResultSO._retn();

  SALOMEDS::Study_var aStudy = theObject->GetStudy();

  CORBA::String_var aTmpDir = CORBA::string_dup(SALOMEDS_Tool::GetTmpDir().c_str());
  char* aFullName2 = new char[strlen(aTmpDir)+1];
  strcpy(aFullName2,aTmpDir);
  //  SALOMEDS::ListOfFileNames_var aSeq = SALOMEDS_Tool::PutStreamToFiles(theStream, aTmpDir, false);
  SALOMEDS::ListOfFileNames_var aSeq = SALOMEDS_Tool::PutStreamToFiles(theStream, aFullName2, false);
  CORBA::String_var aMeshName = CORBA::string_dup(aSeq[0]);
  char* aFullName = new char[strlen(aTmpDir)+strlen(aMeshName)+1];
  strcpy(aFullName, aTmpDir);
  strcpy(aFullName+strlen(aTmpDir), aMeshName);

  MESH * myMesh= new MESH();
  //  myMesh->setName(aMeshName.c_str());
  char* aFullMeshName = new char[strlen(aMeshName)+1];
  strcpy(aFullMeshName,aMeshName);
  myMesh->setName(aFullMeshName);
  MED_MESH_RDONLY_DRIVER myMeshDriver(aFullName, myMesh);
  try {
    myMeshDriver.setMeshName(aFullMeshName);
    myMeshDriver.open();
  } catch (const std::exception & ex) {
    MESSAGE("Exception Interceptee : ");
    SCRUTE(ex.what());
    return aResultSO._retn();
  };
  try {
    myMeshDriver.read();
    MESSAGE("apres read");
    myMeshDriver.close();
  } catch (const std::exception & ex) {
    MESSAGE("Exception Interceptee : ");
    SCRUTE(ex.what());
    return aResultSO._retn();
  };
  // set new mesh name, becouse now there are no possibility to operate meshes with the same names
//    srand((unsigned int)time(NULL));
  int aRND = rand(); //Get a random number to present a name of a copied mesh
  char aCopiedMeshName[127];
  sprintf(aCopiedMeshName,"MESH_COPY_%d",aRND);
  myMesh->setName(aCopiedMeshName);
  MESH_i * meshi = new MESH_i(myMesh);
  SALOME_MED::MESH_ptr mesh = meshi->_this();
  // add the mesh object in study
  meshi->addInStudy(aStudy,mesh);
  // get the IOR attribute of just added mesh
  CORBA::String_var anIORString = _driver_orb->object_to_string(mesh);
  aResultSO = aStudy->FindObjectIOR(anIORString);

  char * aFullName1 = new char[strlen(aTmpDir)+1];
  strcpy(aFullName1,aTmpDir);
  //  SALOMEDS_Tool::RemoveTemporaryFiles(aTmpDir.c_str(), aSeq.in(), true);
  SALOMEDS_Tool::RemoveTemporaryFiles(aFullName1, aSeq.in(), true);

  return aResultSO._retn();
}
