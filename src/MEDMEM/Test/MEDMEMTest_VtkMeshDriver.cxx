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

#include "MEDMEMTest.hxx"
#include <cppunit/TestAssert.h>

#include <MEDMEM_VtkMeshDriver.hxx>
#include <MEDMEM_MedMeshDriver21.hxx>
#include <MEDMEM_Mesh.hxx>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;

/*!
 *  Check methods (11), defined in MEDMEM_VtkMeshDriver.hxx:
 *  class VTK_MESH_DRIVER : public GENDRIVER {
 *   (+) VTK_MESH_DRIVER();
 *   (+) VTK_MESH_DRIVER(const string & fileName,  MESH * ptrMesh);
 *   (+) VTK_MESH_DRIVER(const VTK_MESH_DRIVER & driver);
 *   (+) ~VTK_MESH_DRIVER();
 *   (+) void open();
 *   (+) void close();
 *   (+) void openConst() const throw (MEDEXCEPTION);
 *   (+) void closeConst() const throw (MEDEXCEPTION);
 *   (+) void write(void) const throw (MEDEXCEPTION);
 *   (+) void read (void) throw (MEDEXCEPTION);
 *   (+) void   setMeshName(const string & meshName);
 *   (+) string getMeshName() const;
 *  }
 */
void MEDMEMTest::testVtkMeshDriver()
{
  MESH * aMesh = new MESH();

  string data_dir                  = getenv("DATA_DIR");
  string tmp_dir                   = getenv("TMP");
  string filename_rd               = data_dir + "/MedFiles/pointe.med";
  string emptyfilename             = "";
  string meshname_rd               = "maa1";
  string meshname                  = "MyMesh";
  string fileNotExistsName         = "/path_not_exists/file_not_exists.vtk";
  string filename                  =  tmp_dir  + "/my_pointe.vtk";

  MEDMEMTest_TmpFilesRemover aRemover;
  aRemover.Register(filename);

  {
    //Creation a invalid Vtk driver
    VTK_MESH_DRIVER *aInvalidVtkDriver = new VTK_MESH_DRIVER(fileNotExistsName, aMesh);

    //Trying open not existing file
    CPPUNIT_ASSERT_THROW(aInvalidVtkDriver->open(), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(aInvalidVtkDriver->openConst(), MEDEXCEPTION);

#ifdef ENABLE_FORCED_FAILURES
    // (BUG) In destructor of VTK_MESH_DRIVER: Exception after trying close not existing file
    CPPUNIT_ASSERT_NO_THROW(delete aInvalidVtkDriver);
#endif
  }

  {
    //Create Vtk driver with empty file name
    VTK_MESH_DRIVER *aEmptyVtkDriver = new VTK_MESH_DRIVER(emptyfilename, aMesh);

    //Test open() and openConst() methods with empty file name
    CPPUNIT_ASSERT_THROW(aEmptyVtkDriver->open(), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(aEmptyVtkDriver->openConst(), MEDEXCEPTION);

    delete aEmptyVtkDriver;
  }

  //Creation corect driver
  VTK_MESH_DRIVER *aVtkDriver = new VTK_MESH_DRIVER(filename, aMesh);
  CPPUNIT_ASSERT(aVtkDriver);

  //Create a Mesh
  MED_MESH_RDONLY_DRIVER21 *aMedMeshRdDriver21 = new MED_MESH_RDONLY_DRIVER21(filename_rd, aMesh);
  aMedMeshRdDriver21->open();
  aMedMeshRdDriver21->setMeshName(meshname_rd);

  //Test openConst() and closeConst() methods
  CPPUNIT_ASSERT_NO_THROW(aVtkDriver->openConst());
  CPPUNIT_ASSERT_NO_THROW(aVtkDriver->closeConst());

  //Test open() method
  CPPUNIT_ASSERT_NO_THROW(aVtkDriver->open());

  //Trying open file secondary
#ifdef ENABLE_FORCED_FAILURES
  // (BUG) No exception on attempt to open a file for the second time
  CPPUNIT_ASSERT_THROW(aVtkDriver->open(), MEDEXCEPTION);
#endif

  //Test read method
  CPPUNIT_ASSERT_THROW(aVtkDriver->read(), MEDEXCEPTION);

  //Trying write empty mesh
#ifdef ENABLE_FAULTS
  // ? (BUG) ? In VTK_MESH_DRIVER::write() => Segmentation fault on attempt to write an empty mesh
  CPPUNIT_ASSERT_THROW(aVtkDriver->write(), MEDEXCEPTION);
#endif

  //Read mesh from Med file
  aMedMeshRdDriver21->read();

  //Check mesh
  CPPUNIT_ASSERT(aMesh);

  //Test SetMeshName and getMeshName methods
  CPPUNIT_ASSERT_NO_THROW(aVtkDriver->setMeshName(meshname));
  CPPUNIT_ASSERT_EQUAL(meshname, aVtkDriver->getMeshName());

  //Test write method
  CPPUNIT_ASSERT_NO_THROW(aVtkDriver->write());

  //Test close method
  CPPUNIT_ASSERT_NO_THROW(aVtkDriver->close());

  //Test Default constructor
  VTK_MESH_DRIVER aVtkDriverCpy_1;

  //Test copy constructor
#ifdef ENABLE_FAULTS
  // (BUG) In copy constructor of VTK_MESH_DRIVER: Segmentation fault
  VTK_MESH_DRIVER aVtkDriverCpy_2 (*aVtkDriver);
#endif

  //Test (bool operator ==) defined in GENDRIVER class
  //CPPUNIT_ASSERT(aVtkDriverCpy_2.GENDRIVER::operator==(aVtkDriver));

  //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  //ostringstream ostr1, ostr2;
  //ostr1 << aVtkDriverCpy_2;
  //ostr2 << aVtkDriver;
  //CPPUNIT_ASSERT(ostr1.str() != "");
  //CPPUNIT_ASSERT_EQUAL(ostr1.str(), ostr2.str());

  //Delete objects
  delete aVtkDriver;
  delete aMedMeshRdDriver21;
  delete aMesh;
}
