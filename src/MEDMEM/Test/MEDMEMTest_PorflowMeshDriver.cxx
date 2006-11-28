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

#include <MEDMEM_PorflowMeshDriver.hxx>
#include <MEDMEM_Mesh.hxx>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;

/*!
 *  Check methods (18), defined in MEDMEM_PorflowMeshDriver.hxx:
 *  class PORFLOW_MESH_DRIVER : public GENDRIVER {
 *   (+) PORFLOW_MESH_DRIVER();
 *   (+) PORFLOW_MESH_DRIVER(const string & fileName, MESH * ptrMesh, MED_EN::med_mode_acces accessMode);
 *   (+) PORFLOW_MESH_DRIVER(const PORFLOW_MESH_DRIVER & driver);
 *   (+) virtual ~PORFLOW_MESH_DRIVER();
 *   (+) void open() throw (MEDEXCEPTION);
 *   (+) void close() throw (MEDEXCEPTION);
 *   (-) virtual void write(void) const = 0;
 *   (+) virtual void read (void) = 0;
 *   (+) void   setMeshName(const string & meshName);
 *   (+) string getMeshName() const;
 *  }
 *  class PORFLOW_MESH_RDONLY_DRIVER : public virtual PORFLOW_MESH_DRIVER {
 *   (+) PORFLOW_MESH_RDONLY_DRIVER();
 *   (+) PORFLOW_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh);
 *   (+) PORFLOW_MESH_RDONLY_DRIVER(const PORFLOW_MESH_RDONLY_DRIVER & driver);
 *   (+) virtual ~PORFLOW_MESH_RDONLY_DRIVER();
 *   (+) void write(void) const throw (MEDEXCEPTION);
 *   (+) void read (void) throw (MEDEXCEPTION);
 *  }
 *  class PORFLOW_MESH_WRONLY_DRIVER : public virtual PORFLOW_MESH_DRIVER {
 *   (+) PORFLOW_MESH_WRONLY_DRIVER();
 *   (+) PORFLOW_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh);
 *   (+) PORFLOW_MESH_WRONLY_DRIVER(const PORFLOW_MESH_WRONLY_DRIVER & driver);
 *   (+) virtual ~PORFLOW_MESH_WRONLY_DRIVER();
 *   (NOT IMPLEMENTED!!!) void write(void) const throw (MEDEXCEPTION);
 *   (+) void read (void) throw (MEDEXCEPTION);
 *  }
 *  class PORFLOW_MESH_RDWR_DRIVER : public PORFLOW_MESH_RDONLY_DRIVER, public PORFLOW_MESH_WRONLY_DRIVER {
 *   (+) PORFLOW_MESH_RDWR_DRIVER();
 *   (+) PORFLOW_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh);
 *   (+) PORFLOW_MESH_RDWR_DRIVER(const PORFLOW_MESH_RDWR_DRIVER & driver);
 *   (+) ~PORFLOW_MESH_RDWR_DRIVER();
 *   (CALLS PORFLOW_MESH_WRONLY_DRIVER::write()) void write(void) const throw (MEDEXCEPTION);
 *   (+) void read (void) throw (MEDEXCEPTION);
 *  }
 */
void MEDMEMTest::testPorflowMeshDriver()
{
  MESH *aMesh                      = new MESH();
  MESH *aMesh_1                    = new MESH();
  string data_dir                  = getenv("DATA_DIR");
  string tmp_dir                   = getenv("TMP");
  string filename_rd               = data_dir + "/inp_xyz/Case1.inp";
  string filename_wr               = tmp_dir  + "/myWr_Case1.inp";
  string meshname                  = "Case1";
  string newmeshname               = "new" + meshname;
  string fileNotExistsName_rd      = "notExists.inp";
  string fileNotExistsName_wr      = "/path_not_exists/file_not_exists.inp";
  string filename_rdwr             =  tmp_dir  + "/myRdWr_Case1.inp";
  string fcopy                     = "cp " + filename_rd  + " " + filename_rdwr;
  string fcopy1                    = "cp " + data_dir + "/inp_xyz/Case1.xyz" + " " + tmp_dir + "/Case1.xyz";
  string fcopy2                    = "cp " + data_dir + "/inp_xyz/Case1.cnc" + " " + tmp_dir + "/Case1.cnc";

  //Copy files in the TMP dir for testing READ/WRITE case
  system(fcopy.data());
  system(fcopy1.data());
  system(fcopy2.data());

  MEDMEMTest_TmpFilesRemover aRemover;
  aRemover.Register(filename_wr);
  aRemover.Register(filename_rdwr);
  aRemover.Register(tmp_dir + "/Case1.xyz");
  aRemover.Register(tmp_dir + "/Case1.cnc");

  //----------------------------------Test READ ONLY part---------------------------------------------------//
  //Creation a incorrectPorflow read only driver
  PORFLOW_MESH_RDONLY_DRIVER *aInvalidPorflowRdDriver = new PORFLOW_MESH_RDONLY_DRIVER(fileNotExistsName_rd, aMesh);

  //Trying open not existing file
  CPPUNIT_ASSERT_THROW(aInvalidPorflowRdDriver->open(), MEDEXCEPTION);

  //Creation a correct Porflow read only driver (normal constructor)
  PORFLOW_MESH_RDONLY_DRIVER *aPorflowRdDriver = new PORFLOW_MESH_RDONLY_DRIVER(filename_rd, aMesh);

  //Check driver
  CPPUNIT_ASSERT(aPorflowRdDriver);

  //Trying read mesh from file, if file is not open
  CPPUNIT_ASSERT_THROW(aPorflowRdDriver->read(), MEDEXCEPTION);

  //Test open() method
  CPPUNIT_ASSERT_NO_THROW(aPorflowRdDriver->open());

#ifdef ENABLE_FORCED_FAILURES
  //Trying open file secondary
  CPPUNIT_ASSERT_THROW(aPorflowRdDriver->open(), MEDEXCEPTION);
  //This case  work, but it corrypt driver
#endif

  //Test read() method
  try
  {
    aPorflowRdDriver->read();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  //Check Mesh
  CPPUNIT_ASSERT(aMesh);

  //Trying fill not empty mesh
  CPPUNIT_ASSERT_THROW(aPorflowRdDriver->read(), MEDEXCEPTION);

  //Test write() method for Porflow RDONLY Driver
  CPPUNIT_ASSERT_THROW(aPorflowRdDriver->write(), MEDEXCEPTION);

  //Test setMeshName() and getMeshName()
  try
  {
    aPorflowRdDriver->setMeshName(meshname);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_EQUAL(meshname, aPorflowRdDriver->getMeshName());

  //Test close() method
  try
  {
    aPorflowRdDriver->close();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Default constructor
  PORFLOW_MESH_RDONLY_DRIVER aPorflowRdDriverCpy_1;
  // TO DO

  //Test copy constructor
  PORFLOW_MESH_RDONLY_DRIVER aPorflowRdDriverCpy_2 = PORFLOW_MESH_RDONLY_DRIVER(*aPorflowRdDriver);
  CPPUNIT_ASSERT_EQUAL(aPorflowRdDriverCpy_2, *aPorflowRdDriver);

  //Test (bool operator ==) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  CPPUNIT_ASSERT(aPorflowRdDriverCpy_2.GENDRIVER::operator==(*aPorflowRdDriver));

  //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  ostringstream rostr1, rostr2;
  rostr1<<*aPorflowRdDriver;
  rostr2<<aPorflowRdDriverCpy_2;
  CPPUNIT_ASSERT(rostr1.str() != "");
  CPPUNIT_ASSERT_EQUAL(rostr1.str() , rostr2.str());


  //----------------------------------Test WRITE ONLY part------------------------------------------//
  //Creation a incorrect Porflow write only driver
  PORFLOW_MESH_WRONLY_DRIVER *aInvalidPorflowWrDriver = new PORFLOW_MESH_WRONLY_DRIVER(fileNotExistsName_wr, aMesh);

  //Test case: trying open non existing file
  CPPUNIT_ASSERT_THROW(aInvalidPorflowWrDriver->open(), MEDEXCEPTION);

  //Creation a correct write only driver
  PORFLOW_MESH_WRONLY_DRIVER *aPorflowWrDriver = new PORFLOW_MESH_WRONLY_DRIVER(filename_wr, aMesh);

  //Check driver
  CPPUNIT_ASSERT(aPorflowWrDriver);

  //Test case: trying write mesh to file, if file is not open
  CPPUNIT_ASSERT_THROW(aPorflowWrDriver->write(), MEDEXCEPTION);

  //Test open() method
#ifndef ENABLE_FORCED_FAILURES
  FILE *tmpFile = fopen(filename_wr.data(),"w");
  fclose(tmpFile);
#endif
  CPPUNIT_ASSERT_NO_THROW(aPorflowWrDriver->open());

  //Test case: trying open file secondary.
  CPPUNIT_ASSERT_THROW(aPorflowWrDriver->open(), MEDEXCEPTION);

  ////Test write() method
  //try
  //{
  //  aPorflowWrDriver->write();
  //}
  //catch(MEDEXCEPTION &e)
  //{
  //  CPPUNIT_FAIL(e.what());
  //}
  //catch( ... )
  //{
  //  CPPUNIT_FAIL("Unknown exception");
  //}
  //Write method is not implemented
#ifdef ENABLE_FORCED_FAILURES
  CPPUNIT_FAIL("PORFLOW_MESH_RDONLY_DRIVER::write() method not implemented");
#endif

  //Test read() method for WRITE ONLY driver
  CPPUNIT_ASSERT_THROW(aPorflowWrDriver->read(), MEDEXCEPTION);

  //Test setMeshName() and getMeshName()
  try
  {
    aPorflowWrDriver->setMeshName(newmeshname);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_EQUAL(newmeshname, aPorflowWrDriver->getMeshName());

  try
  {
    aPorflowWrDriver->close();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Default constructor
   PORFLOW_MESH_WRONLY_DRIVER aPorflowWrDriverCpy_1;

  //Test (void operator =) defined in GENDRIVER class in MEDMEM_GenDriver.hxx
  //aPorflowWrDriverCpy_1 = *aPorflowWrDriver;
  //Error dyring compilation
#ifdef ENABLE_FORCED_FAILURES
   CPPUNIT_FAIL("PORFLOW_MESH_WRONLY_DRIVER::operator= :error dyring compilation");
#endif

  //Test copy constructor
  PORFLOW_MESH_WRONLY_DRIVER aPorflowWrDriverCpy_2 = PORFLOW_MESH_WRONLY_DRIVER(*aPorflowWrDriver);
  CPPUNIT_ASSERT_EQUAL(aPorflowWrDriverCpy_2 , *aPorflowWrDriver);

  //Test (bool operator ==) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  CPPUNIT_ASSERT(aPorflowWrDriverCpy_2 == *aPorflowWrDriver);

  //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  ostringstream wostr1, wostr2;
  wostr1<<*aPorflowWrDriver;
  wostr2<<aPorflowWrDriverCpy_2;
  CPPUNIT_ASSERT(wostr1.str() != "");
  CPPUNIT_ASSERT(wostr1.str() == wostr2.str());


//----------------------------------Test READ / WRITE part------------------------------------------//
  //Creation a incorrect read/write driver
  PORFLOW_MESH_RDWR_DRIVER *aInvalidPorflowRdWrDriver = new  PORFLOW_MESH_RDWR_DRIVER(fileNotExistsName_wr, aMesh_1);

  //Test case: trying open non existing file
  CPPUNIT_ASSERT_THROW(aInvalidPorflowRdWrDriver->open(), MEDEXCEPTION);

  //Creation a correct read/write driver
  PORFLOW_MESH_RDWR_DRIVER *aPorflowRdWrDriver = new PORFLOW_MESH_RDWR_DRIVER(filename_rdwr, aMesh_1);

  //Check driver
  CPPUNIT_ASSERT(aPorflowRdWrDriver);

  //Test case: trying write mesh to file, if file is not open
  CPPUNIT_ASSERT_THROW(aPorflowRdWrDriver->write(), MEDEXCEPTION);

  //Test case: trying read mesh from file, if file is not open
  CPPUNIT_ASSERT_THROW(aPorflowRdWrDriver->read(), MEDEXCEPTION);


  //Test open() method
  try
  {
    aPorflowRdWrDriver->open();
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
  //Test case: trying open file secondary.
  CPPUNIT_ASSERT_THROW(aPorflowRdWrDriver->open(), MEDEXCEPTION);
  //This case work, but corrupt driver
#endif

  //Test read() method
  try
  {
    aPorflowRdWrDriver->read();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  ////Test write() method
  //aPorflowRdWrDriver->setMeshName(newmeshname);
  //aMesh->setName(newmeshname);
  //try
  //{
  // aPorflowRdWrDriver->write();
  //}
  //catch(MEDEXCEPTION &e)
  //{
  // CPPUNIT_FAIL(e.what());
  //}
  //catch( ... )
  //{
  //  CPPUNIT_FAIL("Unknown exception");
  //}
  //Write method is not implemented
#ifdef ENABLE_FORCED_FAILURES
  CPPUNIT_FAIL("PORFLOW_MESH_RDWR_DRIVER::write() method not implemented");
#endif

  //Check Mesh
  CPPUNIT_ASSERT(aMesh);

  try
  {
    aPorflowRdWrDriver->close();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }


  //Default constructor
  PORFLOW_MESH_RDWR_DRIVER aPorflowRdWrDriverCpy_1;
  // TO DO

  //Test copy constructor
  PORFLOW_MESH_RDWR_DRIVER aPorflowRdWrDriverCpy_2 = PORFLOW_MESH_RDWR_DRIVER(*aPorflowRdWrDriver);
  CPPUNIT_ASSERT_EQUAL(aPorflowRdWrDriverCpy_2, *aPorflowRdWrDriver);

  //Test (bool operator ==) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  CPPUNIT_ASSERT(aPorflowRdWrDriverCpy_2 ==  *aPorflowRdWrDriver);


  //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  ostringstream rwostr1, rwostr2;
  rwostr1<<*aPorflowRdWrDriver;
  rwostr2<<aPorflowRdWrDriverCpy_2;
  CPPUNIT_ASSERT(rwostr1.str() != "");
  CPPUNIT_ASSERT(rwostr1.str() == rwostr2.str());

  //Delete all objects
  delete aMesh;
  delete aMesh_1;
  delete aInvalidPorflowRdDriver;
  delete aPorflowRdDriver;
  delete aInvalidPorflowWrDriver;
  delete aPorflowWrDriver;
  delete aInvalidPorflowRdWrDriver;
  delete aPorflowRdWrDriver;

  //Delete files
  //remove(filename_wr.data());
  //remove(filename_rdwr.data());
  //remove((tmp_dir + "/Case1.xyz").data());
  //remove((tmp_dir + "/Case1.cnc").data());
}
