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

#ifdef ENABLE_FAULTS
  // (BUG)
#endif

#ifdef ENABLE_FORCED_FAILURES
  //CPPUNIT_FAIL("");
#endif

#include "MEDMEMTest.hxx"
#include <cppunit/TestAssert.h>

#include <MEDMEM_VtkMeshDriver.hxx>
#include <MEDMEM_MedMeshDriver21.hxx>
#include <MEDMEM_Mesh.hxx>

using namespace std;
using namespace MEDMEM;

/*!
 *  Check methods (11), defined in MEDMEM_VtkMeshDriver.hxx:
 *  class VTK_MESH_DRIVER : public GENDRIVER {
 *   (yetno) VTK_MESH_DRIVER();
 *   (+) VTK_MESH_DRIVER(const string & fileName,  MESH * ptrMesh);
 *   (yetno) VTK_MESH_DRIVER(const VTK_MESH_DRIVER & driver);
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
  MESH *aMesh                      = new MESH();
  string data_dir                  = getenv("DATA_DIR");
  string tmp_dir                   = getenv("TMP");
  string filename_rd               = data_dir + "/MedFiles/pointe.med";
  string emptyfilename             = "";
  string meshname_rd               = "maa1";
  string meshname                  = "MyMesh";
  string fileNotExistsName         = "/path_not_exists/file_not_exists.vtk";
  string filename                  =  tmp_dir  + "/my_pointe.vtk";
  
  //Creation a invalid Vtk driver
  VTK_MESH_DRIVER *aInvalidVtkDriver = new VTK_MESH_DRIVER(fileNotExistsName,aMesh);
  
  //Trying open not existing file
  CPPUNIT_ASSERT_THROW(aInvalidVtkDriver->open(), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aInvalidVtkDriver->openConst(), MEDEXCEPTION);
  
  //Create Vtk driver with empty file name
  VTK_MESH_DRIVER *aEmptyVtkDriver = new VTK_MESH_DRIVER(emptyfilename, aMesh);
  
  //Test open() and openConst() methods with empty file name
  CPPUNIT_ASSERT_THROW(aEmptyVtkDriver->open(), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aEmptyVtkDriver->openConst(), MEDEXCEPTION);

  //Creation corect driver
  VTK_MESH_DRIVER *aVtkDriver = new VTK_MESH_DRIVER(filename, aMesh);

  //Create a Mesh 
  MED_MESH_RDONLY_DRIVER21 *aMedMeshRdDriver21 = new MED_MESH_RDONLY_DRIVER21(filename_rd, aMesh);
  aMedMeshRdDriver21->open();
  aMedMeshRdDriver21->setMeshName(meshname_rd);
    
  //Check Driver
  CPPUNIT_ASSERT(aVtkDriver);
  
  //Test openConst() and closeConst() methods
  try
  {
    aVtkDriver->openConst();
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
    aVtkDriver->closeConst();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  
  //Test open() method
  try
  {
    aVtkDriver->open();
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
  //Trying open file secondary
  CPPUNIT_ASSERT_THROW(aVtkDriver->open(), MEDEXCEPTION);
  //No exception in this case
#endif
  
  //Test read method
  CPPUNIT_ASSERT_THROW(aVtkDriver->read(), MEDEXCEPTION);

#ifdef ENABLE_FAULTS
  //Trying write empty mesh
  CPPUNIT_ASSERT_THROW(aVtkDriver->write(), MEDEXCEPTION);
  //=>Segmentation fault, seems it a BUG.
#endif

  //Read mesh from Med file
  aMedMeshRdDriver21->read();
  
  //Check mesh
  CPPUNIT_ASSERT(aMesh);

  //Test SetMeshName and getMeshName methods
  try
  {
    aVtkDriver->setMeshName(meshname);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
                        
  CPPUNIT_ASSERT_EQUAL(meshname, aVtkDriver->getMeshName());
  
  //Test write method
   try
  {
    aVtkDriver->write();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
   
  //Test close method
  try
  {
    aVtkDriver->close();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  
   
   //Test Default constructor 
   VTK_MESH_DRIVER aVtkDriverCpy_1;
#ifdef ENABLE_FAULTS   
   //Test (void operator =) defined in GENDRIVER class in MEDMEM_GenDriver.hxx
   aVtkDriverCpy_1 = *aVtkDriver;
   CPPUNIT_ASSERT_EQUAL(aVtkDriverCpy_1, *aVtkDriver);
   //=>Segmentation fault
#endif
   
#ifdef ENABLE_FAULTS   
   //Test copy constructor
   VTK_MESH_DRIVER aVtkDriverCpy_2 = VTK_MESH_DRIVER(*aVtkDriver);
   CPPUNIT_ASSERT_EQUAL(aVtkDriverCpy_2, *aVtkDriver);
   //=>Segmentation fault
#endif
   
   //Test (bool operator ==) defined GENDRIVER class in MEDMEM_GenDriver.hxx
   //CPPUNIT_ASSERT(aVtkDriverCpy_2 == aVtkDriverCpy_1);

   //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
   //ostringstream ostr1, ostr2;
   //ostr1<<aVtkDriverCpy_2;
   //ostr2<<aVtkDriverCpy_1;
   //CPPUNIT_ASSERT(ostr1.str() != "");
   //CPPUNIT_ASSERT_EQUAL(ostr1.str(), ostr2.str());

  //Delete objects
#ifdef ENABLE_FORCED_FAILURES
  delete aInvalidVtkDriver;
  //Exception after trying close not existing file
#endif
  delete aEmptyVtkDriver;
  delete aVtkDriver;
  delete aMedMeshRdDriver21;
  
  //Remove tmp files
  remove(filename.data());
}
