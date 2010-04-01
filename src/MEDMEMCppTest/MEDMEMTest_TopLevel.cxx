//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "MEDMEMTest.hxx"
#include <cppunit/TestAssert.h>

#include <MEDMEM_TopLevel.hxx>
#include <MEDMEM_MedFieldDriver22.hxx>
#include <MEDMEM_Field.hxx>
#include <MEDMEM_Med.hxx>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

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
  MED * aMed = NULL;

  string filename_rd                = getResourceFile("pointe.med");
  string filename22_rd              = getResourceFile("pointe_import22.med");
  string filenameMesh_wr            = makeTmpFile("myMesh.med");
  string filenameMed_wr             = makeTmpFile("myMed.med");
  string filenameField_wr           = makeTmpFile("myField.med");
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
  {
    //Trying read from not existing file
    //#ifdef ENABLE_FORCED_FAILURES
    CPPUNIT_ASSERT_THROW(readMedInFile(fileNotExist_rd), MEDEXCEPTION);
    // (BUG) No exception in this case
    //#endif
    CPPUNIT_ASSERT_THROW(readMeshInFile(fileNotExist_rd, meshname), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(readFieldInFile<double>(fileNotExist_rd, fieldname), MEDEXCEPTION);

    //Trying read not existing mesh from file
    CPPUNIT_ASSERT_THROW(readMeshInFile(filename_rd, meshname_not_exist), MEDEXCEPTION);

    //Trying read not existing field from file
    //CPPUNIT_ASSERT_THROW(readFieldInFile<double>(filename22_rd, fieldname_not_exist), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(readFieldInFile<double>(filename_rd, fieldname_not_exist), MEDEXCEPTION);

    //Test readMeshInFile() method
    //#ifdef ENABLE_FORCED_FAILURES
    {
      MESH * aMesh = NULL;
      //MEDEXCEPTION: can not open file, but file exist
      CPPUNIT_ASSERT_NO_THROW(aMesh = readMeshInFile(filename22_rd, meshname));
      //CPPUNIT_ASSERT_NO_THROW(aMesh = readMeshInFile(filename_rd, meshname));
      CPPUNIT_ASSERT(aMesh);
      aMesh->removeReference();
    }
    //#endif

    //Test readMedInFile() method
    {
      CPPUNIT_ASSERT_NO_THROW(aMed = readMedInFile(filename_rd));
      CPPUNIT_ASSERT(aMed);
    }

    //Test readFieldInFile() method
    //#ifdef ENABLE_FORCED_FAILURES
    {
      FIELD<double> * aField = NULL;
      //MEDEXCEPTION: can not open file, but file exist
      CPPUNIT_ASSERT_NO_THROW(aField = readFieldInFile<double>(filename22_rd, fieldname));
      CPPUNIT_ASSERT(aField);
      aField->removeReference();
    }
    //#endif
  }

  ////////////////////
  //Test Write Part //
  ////////////////////
  {
    //Create a FIELD
    FIELD<double> *aField_1 = new FIELD<double>();

    MED_FIELD_RDONLY_DRIVER22<double> *aMedRdFieldDriver22 =
      new MED_FIELD_RDONLY_DRIVER22<double>(filename22_rd, aField_1);
    aMedRdFieldDriver22->setFieldName(fieldname);
    aMedRdFieldDriver22->open();
    aMedRdFieldDriver22->read();
    aMedRdFieldDriver22->close();

    //Trying write objects in the not existing file
    //#ifdef ENABLE_FORCED_FAILURES
    // (BUG) No exception in this case
    CPPUNIT_ASSERT_THROW(writeMedToFile(aMed, fileNotExist_wr), MEDEXCEPTION);
    //#endif

    //Create a MESH
    MESH * aMesh = MEDMEMTest_createTestMesh();
    CPPUNIT_ASSERT_THROW(writeMeshToFile(aMesh, fileNotExist_wr), MEDEXCEPTION);

    //#ifdef ENABLE_FORCED_FAILURES
    // (BUG) Error during compilation
    //MED_SRC/src/MEDMEM/MEDMEM_TopLevel.hxx:66: error: passing `const MEDMEM::FIELD<double, MEDMEM::FullInterlace>' as `this' argument of `int MEDMEM::FIELD<T, INTERLACING_TAG>::addDriver(MEDMEM::driverTypes, const std::string&, const std::string&, MED_EN::med_mode_acces) [with T = double, INTERLACING_TAG = MEDMEM::FullInterlace]' discards qualifiers  CPPUNIT_ASSERT_THROW(writeFieldToFile<double>(aField_1, fileNotExist_wr),MEDEXCEPTION);
    //CPPUNIT_FAIL("writeFieldToFile(): compilation error");
    //#endif

    //Trying write mesh in the file with empty name
    aMesh->setName("");
    CPPUNIT_ASSERT_THROW(writeMeshToFile(aMesh, filenameField_wr), MEDEXCEPTION);

    //Test writeMedToFile() and writeMeshToFile() methods
    aMesh->setName(meshname);
    CPPUNIT_ASSERT_NO_THROW(writeMeshToFile(aMesh, filenameMesh_wr));
    CPPUNIT_ASSERT_NO_THROW(writeMedToFile(aMed, filenameMed_wr));

    aField_1->removeReference();
    aMesh->removeReference();
    delete aMed;
  }
}
