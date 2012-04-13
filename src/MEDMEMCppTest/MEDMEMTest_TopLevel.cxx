// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "MEDMEMTest.hxx"
#include <cppunit/TestAssert.h>

#include <MEDMEM_TopLevel.hxx>
#include <MEDMEM_MedFieldDriver.hxx>
#include <MEDMEM_Field.hxx>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

/*!
 *  Check methods (6), defined in MEDMEM_TopLevel.hxx:
 *  (+) MESH *readMeshInFile(const std::string& fileName, const std::string& meshName);
 *  (+) template<class T> FIELD<T> *readFieldInFile
 *                            (const std::string& fileName, const std::string& fieldName);
 *  (+) void writeMeshToFile(const MESH *meshObj, const std::string& fileName);
 *  (-) template<class T> void writeFieldToFile(const FIELD<T> *fieldObj, const std::string& fileName);
 */
void MEDMEMTest::testTopLevel()
{
  string filename_rd                = getResourceFile("pointe.med");
  string filename22_rd              = getResourceFile("pointe.med");
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
    CPPUNIT_ASSERT_THROW(readMeshInFile(fileNotExist_rd, meshname), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(readFieldInFile<double>(fileNotExist_rd, fieldname), MEDEXCEPTION);

    //Trying read not existing mesh from file
    CPPUNIT_ASSERT_THROW(readMeshInFile(filename_rd, meshname_not_exist), MEDEXCEPTION);

    //Trying read not existing field from file
    CPPUNIT_ASSERT_THROW(readFieldInFile<double>(filename_rd, fieldname_not_exist), MEDEXCEPTION);

    //Test readMeshInFile() method
    {
      MESH * aMesh = NULL;
      CPPUNIT_ASSERT_NO_THROW(aMesh = readMeshInFile(filename22_rd, meshname));
      CPPUNIT_ASSERT(aMesh);
      aMesh->removeReference();
    }

    //Test readFieldInFile() method
    {
      FIELD<double> * aField = NULL;
      CPPUNIT_ASSERT_NO_THROW(aField = readFieldInFile<double>(filename22_rd, fieldname));
      CPPUNIT_ASSERT(aField);
      aField->removeReference();
    }
  }

  ////////////////////
  //Test Write Part //
  ////////////////////
  {
    //Create a FIELD
    FIELD<double> *aField_1 = new FIELD<double>();

    MED_FIELD_RDONLY_DRIVER<double> *aMedRdFieldDriver22 =
      new MED_FIELD_RDONLY_DRIVER<double>(filename22_rd, aField_1);
    aMedRdFieldDriver22->setFieldName(fieldname);
    aMedRdFieldDriver22->open();
    aMedRdFieldDriver22->read();
    aMedRdFieldDriver22->close();

    //Create a MESH
    MESH * aMesh = MEDMEMTest_createTestMesh();
    CPPUNIT_ASSERT_THROW(writeMeshToFile(aMesh, fileNotExist_wr), MEDEXCEPTION);

    //Trying write mesh in the file with empty name
    aMesh->setName("");
    CPPUNIT_ASSERT_THROW(writeMeshToFile(aMesh, filenameField_wr), MEDEXCEPTION);

    //Test writeMeshToFile() method
    aMesh->setName(meshname);
    CPPUNIT_ASSERT_NO_THROW(writeMeshToFile(aMesh, filenameMesh_wr));

    aField_1->removeReference();
    aMesh->removeReference();
  }
}
