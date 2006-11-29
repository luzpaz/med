// Copyright (C) 2006  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
#define ENABLE_FORCED_FAILURES

#include "MEDMEMTest.hxx"
#include <cppunit/TestAssert.h>

#include <MEDMEM_TopLevel.hxx>
#include <MEDMEM_MedFieldDriver22.hxx>
#include <MEDMEM_Field.hxx>
          
using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

/*!
 *  Check methods (6), defined in MEDMEM_TopLevel.hxx:
 *  (+) MED *readMedInFile(const std::string& fileName) throw(MEDEXCEPTION);
 *  (+) MESH *readMeshInFile(const std::string& fileName, const std::string& meshName);
 *  (+) template<class T> FIELD<T> *readFieldInFile
 *                            (const std::string& fileName, const std::string& fieldName);
 *  (+) void writeMedToFile(const MED *medObj, const std::string& fileName);
 *  (+) void writeMeshToFile(const MESH *meshObj, const std::string& fileName);
 *  (-) template<class T> void writeFieldToFile(const FIELD<T> *fieldObj, const std::string& fileName);
 */
void MEDMEMTest::testTopLevel()
{
  MED *aMed = NULL;
  MESH *aMesh = NULL;
  FIELD<double> *aField = NULL;
  FIELD<double> *aField_1 = new FIELD<double> ();
  string data_dir                   = getenv("DATA_DIR");
  string tmp_dir                    = getenv("TMP");
  if (tmp_dir == "")
    tmp_dir = "/tmp";
  string filename_rd                = data_dir + "/MedFiles/pointe.med";
  string filename22_rd              = data_dir + "/MedFiles/pointe_import22.med";
  string filenameMesh_wr            = tmp_dir + "/myMesh.med";
  string filenameMed_wr             = tmp_dir + "/myMed.med";
  string filenameField_wr           = tmp_dir + "/myField.med";
  string fileNotExist_rd            = "/notExist.med";
  string fileNotExist_wr            = "/path_not_exist/file_not_exist.med";
  string meshname                   = "maa1";
  string meshname_not_exist         = "anymesh";
  string fieldname_not_exist        = "anyfield";
  string fieldname                  = "fieldnodedouble";
// To remove tmp files from disk
  MEDMEMTest_TmpFilesRemover aRemover;
  aRemover.Register(filenameMesh_wr);
  aRemover.Register(filenameMed_wr);
  aRemover.Register(filenameField_wr);

  ///////////////////
  //Test Read Part //
  ///////////////////

  //Trying read from not existing file
#ifdef ENABLE_FORCED_FAILURES
  CPPUNIT_ASSERT_THROW(aMed = readMedInFile(fileNotExist_rd), MEDEXCEPTION);
  // (BUG) No exception in this case
#endif
  CPPUNIT_ASSERT_THROW(aMesh = readMeshInFile(fileNotExist_rd, meshname), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aField = readFieldInFile<double>(fileNotExist_rd, fieldname), MEDEXCEPTION);
  
  //Trying read not existing mesh from file
  CPPUNIT_ASSERT_THROW(aMesh = readMeshInFile(filename_rd, meshname_not_exist), MEDEXCEPTION);

  //Trying read not existing field from file
  CPPUNIT_ASSERT_THROW(aField = readFieldInFile<double>(filename22_rd, fieldname_not_exist), MEDEXCEPTION);

#ifdef ENABLE_FORCED_FAILURES
  //Test readMedInFile(),  readMeshInFile() and readFieldInFile() methods
  try
  {
    aMesh = readMeshInFile(filename_rd, meshname);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  //MEDEXCEPTION: can not open file, but file exist
#endif

  try
  {
    aMed = readMedInFile(filename_rd);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

#ifdef ENABLE_FORCED_FAILURES  
  try
  {
    aField = readFieldInFile<double>(filename22_rd, fieldname);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  //MEDEXCEPTION: can not open file, but file exist  
#endif
  //Check aMed
  CPPUNIT_ASSERT(aMed);


  ////////////////////
  //Test Write Part //
  ////////////////////
  //Create a MESH
  aMesh = createTestMesh();

  //Create a FIELD
  MED_FIELD_RDONLY_DRIVER22<double> *aMedRdFieldDriver22 =
     new MED_FIELD_RDONLY_DRIVER22<double>(filename22_rd, aField_1);
  aMedRdFieldDriver22->setFieldName(fieldname);
  aMedRdFieldDriver22->open();
  aMedRdFieldDriver22->read();
  aMedRdFieldDriver22->close();


//Trying write objects in the not existing file
#ifdef ENABLE_FORCED_FAILURES
  CPPUNIT_ASSERT_THROW(writeMedToFile(aMed, fileNotExist_wr),MEDEXCEPTION);
  // (BUG) No exception in this case
#endif
  CPPUNIT_ASSERT_THROW(writeMeshToFile(aMesh, fileNotExist_wr),MEDEXCEPTION);
#ifdef ENABLE_FORCED_FAILURES
  //CPPUNIT_ASSERT_THROW(writeFieldToFile<double>(aField_1, fileNotExist_wr),MEDEXCEPTION);
  //CPPUNIT_FAIL("writeFieldToFile(): Error during compilation");
  // (BUG) Error during compilation
#endif

  //Trying write mesh in the file with empty name
  aMesh->setName("");
  CPPUNIT_ASSERT_THROW(writeMeshToFile(aMesh, filenameField_wr),MEDEXCEPTION);

  //Test writeMedToFile() and writeMeshToFile() methods
  aMesh->setName(meshname);
  try
  {
    writeMeshToFile(aMesh, filenameMesh_wr);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  
  try
  {
    writeMedToFile(aMed, filenameMed_wr);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  delete aField_1;
}
