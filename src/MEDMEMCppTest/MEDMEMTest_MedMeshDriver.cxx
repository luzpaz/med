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
#include <MEDMEM_MedMeshDriver.hxx>
#include <MEDMEM_Mesh.hxx>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;

/*!
 *  Check methods (13), defined in MEDMEM_MedMeshDriver.hxx:
 *  class MED_MESH_DRIVER {
 *   (+) MED_MESH_DRIVER();
 *   (+) MED_MESH_DRIVER(const string & fileName, MESH * ptrMesh, MED_EN::med_mode_acces accessMode);
 *   (+) MED_MESH_DRIVER(const MED_MESH_DRIVER22 & driver);
 *   (+) virtual ~MED_MESH_DRIVER();
 *   (+) void open();
 *   (+) void close();
 *  }
 *  class MED_MESH_RDONLY_DRIVER : public virtual MED_MESH_DRIVER,
 *  {
 *   (+) MED_MESH_RDONLY_DRIVER();
 *   (+) MED_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh);
 *   (+) MED_MESH_RDONLY_DRIVER(const MED_MESH_RDONLY_DRIVER & driver);
 *   (+) virtual ~MED_MESH_RDONLY_DRIVER();
 *   (+) void read (void);
 *  }
 *  class MED_MESH_WRONLY_DRIVER : public virtual MED_MESH_DRIVER
 *  {
 *   (+) MED_MESH_WRONLY_DRIVER();
 *   (+) MED_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh);
 *   (+) MED_MESH_WRONLY_DRIVER(const MED_MESH_WRONLY_DRIVER & driver);
 *   (+) virtual ~MED_MESH_WRONLY_DRIVER();
 *   (+) void write(void) const;
 *  }
 *  class MED_MESH_RDWR_DRIVER : public MED_MESH_RDONLY_DRIVER, public MED_MESH_WRONLY_DRIVER
 *  {
 *   (+) MED_MESH_RDWR_DRIVER();
 *   (+) MED_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh);
 *   (+) MED_MESH_RDWR_DRIVER(const MED_MESH_RDWR_DRIVER & driver);
 *   (+) ~MED_MESH_RDWR_DRIVER();
 *   (+) void write(void) const;
 *   (+) void read (void);
 *  }
 */
void MEDMEMTest::testMedMeshDriver()
{
  MESH *aMesh                      = new MESH;
  MESH *aMesh_1                    = new MESH;

  string filename_rd               = getResourceFile("pointe.med");
  string filename_wr               = makeTmpFile("myWr_pointe22.med");
  string tmpfile                   = makeTmpFile("tmp.med");
  string meshname                  = "maa1";
  string newmeshname               = "new" + meshname;
  string fileNotExistsName_rd      = "notExists.med";
  string fileNotExistsName_wr      = "/path_not_exists/file_not_exists.med";
  string filename_rdwr             = makeTmpFile("myRdWr_pointe22.med", filename_rd);
  char* longmeshname               = new char[MED_NAME_SIZE+2];
  for (int i = 0; i<MED_NAME_SIZE+2; ++i)
    longmeshname[i] = 'a';
  longmeshname[MED_NAME_SIZE+1] = '\0';

  // To remove tmp files from disk
  MEDMEMTest_TmpFilesRemover aRemover;
  aRemover.Register(filename_wr);
  aRemover.Register(filename_rdwr);
  aRemover.Register(tmpfile);

  //----------------------------------Test READ ONLY part---------------------------------------------------//

  //Creation a incorrect read only driver
  MED_MESH_RDONLY_DRIVER *aInvalidRdDriver22 = new MED_MESH_RDONLY_DRIVER(fileNotExistsName_rd, aMesh);

  //Trying open not existing file
  CPPUNIT_ASSERT_THROW(aInvalidRdDriver22->open(), MEDEXCEPTION);

  //Creation a correct read only driver (normal constructor)
  MED_MESH_RDONLY_DRIVER *aRdDriver22 = new MED_MESH_RDONLY_DRIVER(filename_rd, aMesh);

  //Check driver
  CPPUNIT_ASSERT(aRdDriver22);

  //Trying read mesh from file, if file is not open
  CPPUNIT_ASSERT_THROW(aRdDriver22->read(), MEDEXCEPTION);

  //Test open() method
  try
  {
    aRdDriver22->open();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //#ifdef  ENABLE_FORCED_FAILURES
  //Trying open file secondary.
  //CPPUNIT_ASSERT_THROW(aRdDriver22->open(), MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW(aRdDriver22->open());
  //This case is not work, seems it BUG
  //#endif

  //Trying read mesh from file, if mesh name is not set, i.e. empty
  CPPUNIT_ASSERT_THROW(aRdDriver22->read(), MEDEXCEPTION);

  //Trying read mesh from file with very long name
  aRdDriver22->setMeshName(longmeshname);
  CPPUNIT_ASSERT_THROW(aRdDriver22->read(), MEDEXCEPTION);

  //Set correct  Mesh name
  //Test setMeshName() and getMeshName() methods
  try
  {
    aRdDriver22->setMeshName(meshname);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_EQUAL(meshname, aRdDriver22->getMeshName());


  //Test read() method
  try
  {
    aRdDriver22->read();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Test write() method for READ ONLY driver
  CPPUNIT_ASSERT_THROW(aRdDriver22->write(), MEDEXCEPTION);

  //Check Mesh
  CPPUNIT_ASSERT(aMesh);

  //Default constructor
  MED_MESH_RDONLY_DRIVER aRdDriver22Cpy_1;

  //Test (void operator =) defined in GENDRIVER class in MEDMEM_GenDriver.hxx
  //aRdDriver22Cpy_1 = *aRdDriver22;

  //Test (bool operator ==) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  CPPUNIT_ASSERT(aRdDriver22Cpy_1.GENDRIVER::operator==(*aRdDriver22));

  //Test copy constructor
  MED_MESH_RDONLY_DRIVER aRdDriver22Cpy_2 (aRdDriver22Cpy_1);
  CPPUNIT_ASSERT_EQUAL(aRdDriver22Cpy_2, *aRdDriver22);

  //Test close() method
  try
  {
    aRdDriver22->close();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Trying read mesh from copy closed driver
  CPPUNIT_ASSERT_THROW(aRdDriver22Cpy_1.read(), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aRdDriver22Cpy_2.read(), MEDEXCEPTION);

  //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  ostringstream rostr1, rostr2;
  rostr1<<aRdDriver22Cpy_1;
  rostr2<<aRdDriver22Cpy_2;
  CPPUNIT_ASSERT(rostr1.str() != "");
  CPPUNIT_ASSERT(rostr1.str() == rostr2.str());


  //----------------------------------Test WRITE ONLY part------------------------------------------//

  //Creation a incorrect write only driver
  MED_MESH_WRONLY_DRIVER *aInvalidWrDriver22 = new MED_MESH_WRONLY_DRIVER(fileNotExistsName_wr, aMesh);

  //Trying open non existing file
  CPPUNIT_ASSERT_THROW(aInvalidWrDriver22->open(), MEDEXCEPTION);

  //Trying write empty mesh
  MED_MESH_WRONLY_DRIVER *aTmpWrDriver22 = new MED_MESH_WRONLY_DRIVER(tmpfile, aMesh_1);

  //#ifdef ENABLE_FORCED_FAILURES
  aTmpWrDriver22->open();
  aTmpWrDriver22->setMeshName("EmptyMesh");
  CPPUNIT_ASSERT_THROW(aTmpWrDriver22->write(),MEDEXCEPTION);
  aTmpWrDriver22->close();
  //Unknown exception in this case
  //#endif

  //Creation a correct write only driver
  MED_MESH_WRONLY_DRIVER *aWrDriver22 = new MED_MESH_WRONLY_DRIVER(filename_wr, aMesh);

  //Check driver
  CPPUNIT_ASSERT(aWrDriver22);

  //Trying write mesh to file, if file is not open
  CPPUNIT_ASSERT_THROW(aWrDriver22->write(), MEDEXCEPTION);

  //Test open() method
  try
  {
    aWrDriver22->open();
  }
  catch(MEDEXCEPTION &e)
    {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //#ifdef ENABLE_FORCED_FAILURES
  //Trying open file secondary.
  //CPPUNIT_ASSERT_THROW(aWrDriver22->open(), MEDEXCEPTION);
  //This case is not work, seems it BUG
  //#endif

  //Test case: trying write mesh to file, if mesh name is not set, i.e empty
  aMesh->setName("");
  aWrDriver22->setMeshName("");
  CPPUNIT_ASSERT_THROW(aWrDriver22->write(), MEDEXCEPTION);

  //Trying write mesh to file with very long name
  aWrDriver22->setMeshName(longmeshname);
  CPPUNIT_ASSERT_THROW(aWrDriver22->write(), MEDEXCEPTION);

  //Set initialy mesh name
  aMesh->setName(meshname);
  //Test setMeshName() and getMeshName() methods
  try
  {
    aWrDriver22->setMeshName(meshname);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_EQUAL(meshname, aWrDriver22->getMeshName());

  //  aWrDriver22->open();
  //Test write() method
  try
  {
    aWrDriver22->write();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Test read() method for WRITE ONLY driver
  CPPUNIT_ASSERT_THROW(aWrDriver22->read(), MEDEXCEPTION);

  //Default constructor
   MED_MESH_WRONLY_DRIVER aWrDriver22Cpy_1;

  //Test (void operator =) defined in GENDRIVER class
  //aWrDriver22Cpy_1 = *aWrDriver22;

  //Test (bool operator ==) defined GENDRIVER class
  CPPUNIT_ASSERT(aWrDriver22Cpy_1.GENDRIVER::operator==(*aWrDriver22));

  //Test copy constructor
  MED_MESH_WRONLY_DRIVER aWrDriver22Cpy_2 (aWrDriver22Cpy_1);
  CPPUNIT_ASSERT_EQUAL(aWrDriver22Cpy_2 , *aWrDriver22);

  try
  {
    aWrDriver22->close();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Test case: Trying write mesh using copy closed driver
  CPPUNIT_ASSERT_THROW(aWrDriver22Cpy_1.write(), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aWrDriver22Cpy_2.write(), MEDEXCEPTION);

  //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  ostringstream wostr1, wostr2;
  wostr1<<aWrDriver22Cpy_1;
  wostr2<<aWrDriver22Cpy_2;
  CPPUNIT_ASSERT(wostr1.str() != "");
  CPPUNIT_ASSERT(wostr1.str() == wostr2.str());


  //----------------------------------Test READ / WRITE part------------------------------------------//

  //Creation a incorrect read/write driver
  MED_MESH_RDWR_DRIVER *aInvalidRdWrDriver22 = new  MED_MESH_RDWR_DRIVER(fileNotExistsName_wr, aMesh_1);

  //Test case: trying open non existing file
  CPPUNIT_ASSERT_THROW(aInvalidRdWrDriver22->open(), MEDEXCEPTION);

  //Creation a correct read/write driver
  MED_MESH_RDWR_DRIVER *aRdWrDriver22 = new MED_MESH_RDWR_DRIVER(filename_rdwr, aMesh_1);

  //Check driver
  CPPUNIT_ASSERT(aRdWrDriver22);

  //Test case: trying write mesh to file, if file is not open
  CPPUNIT_ASSERT_THROW(aRdWrDriver22->write(), MEDEXCEPTION);

  //Test case: trying read mesh from file, if file is not open
  CPPUNIT_ASSERT_THROW(aRdWrDriver22->read(), MEDEXCEPTION);

  //Test open() method
  try
  {
    aRdWrDriver22->open();
  }
  catch(MEDEXCEPTION &e)
    {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //#ifdef ENABLE_FORCED_FAILURES
  //Test case: trying open file secondary.
  //CPPUNIT_ASSERT_THROW(aRdWrDriver22->open(), MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW(aRdWrDriver22->open());
  //This case is not work, seems it BUG
  //#endif

  //#ifdef ENABLE_FORCED_FAILURES
  //Set mesh name
  aMesh_1->setName("EmptyMesh");
  aRdWrDriver22->setMeshName("EmptyMesh");
  //Trying write in file empty mesh
  CPPUNIT_ASSERT_THROW(aRdWrDriver22->write(),MEDEXCEPTION);
  //No exception in this case, seems it BUG
  //#endif

  //Set initialy mesh name
  aMesh_1->setName(meshname);
  //Test setMeshName() and getMeshName() methods
  try
  {
    aRdWrDriver22->setMeshName(meshname);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_EQUAL(meshname, aRdWrDriver22->getMeshName());

  //Test read() method
  try
  {
    aRdWrDriver22->read();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Trying read mesh from file, if mesh name is not set, i.e. empty
  aRdWrDriver22->setMeshName("");
  aMesh_1->setName("");
  CPPUNIT_ASSERT_THROW(aRdWrDriver22->read(), MEDEXCEPTION);

  //Trying write mesh to file, if mesh name is not set, i.e empty
  CPPUNIT_ASSERT_THROW(aRdWrDriver22->write(), MEDEXCEPTION);

  //Trying read mesh from file with very long name
  aRdWrDriver22->setMeshName(longmeshname);
  aMesh_1->setName(longmeshname);
  CPPUNIT_ASSERT_THROW(aRdWrDriver22->read(), MEDEXCEPTION);

  //Trying write mesh to file with very long name
  CPPUNIT_ASSERT_THROW(aRdWrDriver22->write(), MEDEXCEPTION);

  //Test write() method
  aRdWrDriver22->setMeshName(newmeshname);
  aMesh_1->setName(newmeshname);
  try
  {
    aRdWrDriver22->write();
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
  CPPUNIT_ASSERT(aMesh_1);

  //Default constructor
  MED_MESH_RDWR_DRIVER aRdWrDriver22Cpy_1;

  //Test (void operator =) defined in GENDRIVER class
  aRdWrDriver22Cpy_1= *aRdWrDriver22;

  //Test (bool operator ==) defined GENDRIVER class
  CPPUNIT_ASSERT(aRdWrDriver22Cpy_1.GENDRIVER::operator==(*aRdWrDriver22));

  //Test copy constructor
  MED_MESH_RDWR_DRIVER aRdWrDriver22Cpy_2 (aRdWrDriver22Cpy_1);
  // CPPUNIT_ASSERT(aRdWrDriver22Cpy_2.GENDRIVER::operator==( *aRdWrDriver22));

  try
  {
    aRdWrDriver22->close();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Test case: trying read/write mesh using copy closed driver
  CPPUNIT_ASSERT_THROW(aRdWrDriver22Cpy_1.read(), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aRdWrDriver22Cpy_2.read(), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aRdWrDriver22Cpy_1.write(), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aRdWrDriver22Cpy_2.write(), MEDEXCEPTION);

  //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  ostringstream rwostr1, rwostr2;
  rwostr1<<aRdWrDriver22Cpy_1;
  rwostr2<<aRdWrDriver22Cpy_2;
  cout << aRdWrDriver22Cpy_1;
  cout << aRdWrDriver22Cpy_2;
  CPPUNIT_ASSERT(rwostr1.str() != "");
  CPPUNIT_ASSERT(rwostr1.str() == rwostr2.str());

  //Delete all objects
  delete []longmeshname;

  delete aInvalidRdDriver22;
  delete aRdDriver22;

  delete aInvalidWrDriver22;
  delete aWrDriver22;

  delete aInvalidRdWrDriver22;
  delete aRdWrDriver22;
  delete aTmpWrDriver22;

  aMesh->removeReference();
  aMesh_1->removeReference();
}
