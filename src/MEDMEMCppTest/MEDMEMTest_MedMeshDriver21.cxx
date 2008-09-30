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

#include <MEDMEM_MedMeshDriver21.hxx>
#include <MEDMEM_Mesh.hxx>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;

/*!
 *  Check methods (13), defined in MEDMEM_MedMeshDriver21.hxx:
 *  class MED_MESH_DRIVER21 : public virtual MED_MESH_DRIVER {
 *   (+) MED_MESH_DRIVER21();
 *   (+) MED_MESH_DRIVER21(const string & fileName, MESH * ptrMesh, MED_EN::med_mode_acces accessMode);
 *   (+) MED_MESH_DRIVER21(const MED_MESH_DRIVER21 & driver);
 *   (+) virtual ~MED_MESH_DRIVER21();
 *   (+) void open();
 *   (+) void close();
 *  }
 *  class MED_MESH_RDONLY_DRIVER21 : public virtual IMED_MESH_RDONLY_DRIVER,
 *                                   public virtual MED_MESH_DRIVER21 {
 *   (+) MED_MESH_RDONLY_DRIVER21();
 *   (+) MED_MESH_RDONLY_DRIVER21(const string & fileName, MESH * ptrMesh);
 *   (+) MED_MESH_RDONLY_DRIVER21(const MED_MESH_RDONLY_DRIVER21 & driver);
 *   (+) virtual ~MED_MESH_RDONLY_DRIVER21();
 *   (+) void read (void);
 *  }
 *  class MED_MESH_WRONLY_DRIVER21 : public virtual IMED_MESH_WRONLY_DRIVER,
 *                                   public virtual MED_MESH_DRIVER21 {
 *   (+) MED_MESH_WRONLY_DRIVER21();
 *   (+) MED_MESH_WRONLY_DRIVER21(const string & fileName, MESH * ptrMesh);
 *   (+) MED_MESH_WRONLY_DRIVER21(const MED_MESH_WRONLY_DRIVER21 & driver);
 *   (+) virtual ~MED_MESH_WRONLY_DRIVER21();
 *   (+) void write(void) const;
 *  }
 *  class MED_MESH_RDWR_DRIVER21 : public IMED_MESH_RDWR_DRIVER,
 *                                 public MED_MESH_RDONLY_DRIVER21, public MED_MESH_WRONLY_DRIVER21 {
 *   (+) MED_MESH_RDWR_DRIVER21();
 *   (+) MED_MESH_RDWR_DRIVER21(const string & fileName, MESH * ptrMesh);
 *   (+) MED_MESH_RDWR_DRIVER21(const MED_MESH_RDWR_DRIVER21 & driver);
 *   (+) ~MED_MESH_RDWR_DRIVER21();
 *   (+) void write(void) const;
 *   (+) void read (void);
 *  }
 */
void MEDMEMTest::testMedMeshDriver21()
{
  MESH *aMesh                      = new MESH();
  MESH *aMesh_1                    = new MESH();

  string data_dir                  = getenv("DATA_DIR");
  string tmp_dir                   = getenv("TMP");
  if (tmp_dir == "") tmp_dir = "/tmp";

  string filename_rd               = data_dir + "/MedFiles/pointe.med";
  string filename_wr               = tmp_dir  + "/myWr_pointe21.med";
  string tmpfile                   = tmp_dir  + "/tmp.med";
  string meshname                  = "maa1";
  string newmeshname               = "new" + meshname;
  string fileNotExistsName_rd      = "notExists.med";
  string fileNotExistsName_wr      = "/path_not_exists/file_not_exists.med";
  string filename_rdwr             =  tmp_dir  + "/myRdWr_pointe22.med";
  char* longmeshname               = new char[MED_TAILLE_NOM+2];
  string fcopy                     = "cp " + filename_rd + " " + filename_rdwr;
  for (int i = 0; i<MED_TAILLE_NOM+2; ++i)
    longmeshname[i] = 'a';

  //Copy file in the TMP dir for testing READ/WRITE case
  system(fcopy.data());

  // To remove tmp files from disk
  MEDMEMTest_TmpFilesRemover aRemover;
  aRemover.Register(filename_wr);
  aRemover.Register(filename_rdwr);
  aRemover.Register(tmpfile);

  //----------------------------------Test READ ONLY part---------------------------------------------------//

  //Creation a incorrect read only driver
  MED_MESH_RDONLY_DRIVER21 *aInvalidRdDriver21 = new MED_MESH_RDONLY_DRIVER21(fileNotExistsName_rd, aMesh);

  //Trying open not existing file
  CPPUNIT_ASSERT_THROW(aInvalidRdDriver21->open(), MEDEXCEPTION);

  //Creation a correct read only driver (normal constructor)
  MED_MESH_RDONLY_DRIVER21 *aRdDriver21 = new MED_MESH_RDONLY_DRIVER21(filename_rd, aMesh);

  //Check driver
  CPPUNIT_ASSERT(aRdDriver21);

  //Trying read mesh from file, if file is not open
  CPPUNIT_ASSERT_THROW(aRdDriver21->read(), MEDEXCEPTION);

  //Test open() method
  try
  {
    aRdDriver21->open();
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
  //CPPUNIT_ASSERT_THROW(aRdDriver21->open(), MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW(aRdDriver21->open());
  //This case is not work, seems it BUG

  //Trying read mesh from file, if mesh name is not set, i.e. empty
  CPPUNIT_ASSERT_THROW(aRdDriver21->read(), MEDEXCEPTION);
  //No correct exception in this case

  //Trying read mesh from file with very long name
  //aRdDriver21->setMeshName(longmeshname);
  CPPUNIT_ASSERT_THROW(aRdDriver21->read(), MEDEXCEPTION);
  //No correct exception in this case
  //#endif

  //Set correct  Mesh name
  //Test setMeshName() and getMeshName() methods
  try
  {
    aRdDriver21->setMeshName(meshname);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_EQUAL(meshname, aRdDriver21->getMeshName());


  //Test read() method
  try
  {
    aRdDriver21->read();
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
  CPPUNIT_ASSERT_THROW(aRdDriver21->write(), MEDEXCEPTION);

  //Check Mesh
  CPPUNIT_ASSERT(aMesh);

  //Default constructor
  MED_MESH_RDONLY_DRIVER21 aRdDriver21Cpy_1;

  //Test (void operator =) defined in GENDRIVER class in MEDMEM_GenDriver.hxx
  //aRdDriver21Cpy_1 = *aRdDriver21;

  //Test (bool operator ==) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  CPPUNIT_ASSERT(aRdDriver21Cpy_1.GENDRIVER::operator==(*aRdDriver21));

  //Test copy constructor
  MED_MESH_RDONLY_DRIVER21 aRdDriver21Cpy_2 (aRdDriver21Cpy_1);
  CPPUNIT_ASSERT_EQUAL(aRdDriver21Cpy_2, *aRdDriver21);

  //Test close() method
  try
  {
    aRdDriver21->close();
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
  CPPUNIT_ASSERT_THROW(aRdDriver21Cpy_1.read(), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aRdDriver21Cpy_2.read(), MEDEXCEPTION);

  //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  ostringstream rostr1, rostr2;
  rostr1<<aRdDriver21Cpy_1;
  rostr2<<aRdDriver21Cpy_2;
  CPPUNIT_ASSERT(rostr1.str() != "");
  CPPUNIT_ASSERT(rostr1.str() == rostr2.str());


  //----------------------------------Test WRITE ONLY part------------------------------------------//

  //Creation a incorrect write only driver
  MED_MESH_WRONLY_DRIVER21 *aInvalidWrDriver21 = new MED_MESH_WRONLY_DRIVER21(fileNotExistsName_wr, aMesh);

  //Trying open non existing file
  CPPUNIT_ASSERT_THROW(aInvalidWrDriver21->open(), MEDEXCEPTION);

  //Trying write empty mesh
  MED_MESH_WRONLY_DRIVER21 *aTmpWrDriver21 = new MED_MESH_WRONLY_DRIVER21(tmpfile, aMesh_1);

  //#ifdef ENABLE_FORCED_FAILURES
  aTmpWrDriver21->open();
  aTmpWrDriver21->setMeshName("EmptyMesh");
  CPPUNIT_ASSERT_THROW(aTmpWrDriver21->write(),MEDEXCEPTION);
  aTmpWrDriver21->close();
  //Unknown exception in this case
  //#endif

  //Creation a correct write only driver
  MED_MESH_WRONLY_DRIVER21 *aWrDriver21 = new MED_MESH_WRONLY_DRIVER21(filename_wr, aMesh);

  //Check driver
  CPPUNIT_ASSERT(aWrDriver21);

  //Trying write mesh to file, if file is not open
  CPPUNIT_ASSERT_THROW(aWrDriver21->write(), MEDEXCEPTION);

  //Test open() method
  try
  {
    aWrDriver21->open();
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
  //CPPUNIT_ASSERT_THROW(aWrDriver21->open(), MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW(aWrDriver21->open());
  //This case is not work, seems it BUG
  //#endif

  //#ifdef ENABLE_FORCED_FAILURES
  //Test case: trying write mesh to file, if mesh name is not set, i.e empty
  aMesh->setName("");
  aWrDriver21->setMeshName("");
  CPPUNIT_ASSERT_THROW(aWrDriver21->write(), MEDEXCEPTION);
  //No exception in this case

  //Trying write mesh to file with very long name
  aWrDriver21->setMeshName(longmeshname);
  CPPUNIT_ASSERT_THROW(aWrDriver21->write(), MEDEXCEPTION);
  //No exception in this case
  //#endif

  //Set initialy mesh name
  aMesh->setName(meshname);
  //Test setMeshName() and getMeshName() methods
  try
  {
    aWrDriver21->setMeshName(meshname);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_EQUAL(meshname, aWrDriver21->getMeshName());

  //Test write() method
  try
  {
    aWrDriver21->write();
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
  CPPUNIT_ASSERT_THROW(aWrDriver21->read(), MEDEXCEPTION);

  //Default constructor
   MED_MESH_WRONLY_DRIVER21 aWrDriver21Cpy_1;

  //Test (void operator =) defined in GENDRIVER class
  //aWrDriver21Cpy_1 = *aWrDriver21;

  //Test (bool operator ==) defined GENDRIVER class
  CPPUNIT_ASSERT(aWrDriver21Cpy_1.GENDRIVER::operator==(*aWrDriver21));

  //Test copy constructor
  MED_MESH_WRONLY_DRIVER21 aWrDriver21Cpy_2 (aWrDriver21Cpy_1);
  CPPUNIT_ASSERT_EQUAL(aWrDriver21Cpy_2 , *aWrDriver21);

  try
  {
    aWrDriver21->close();
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
  CPPUNIT_ASSERT_THROW(aWrDriver21Cpy_1.write(), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aWrDriver21Cpy_2.write(), MEDEXCEPTION);

  //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  ostringstream wostr1, wostr2;
  wostr1<<aWrDriver21Cpy_1;
  wostr2<<aWrDriver21Cpy_2;
  CPPUNIT_ASSERT(wostr1.str() != "");
  CPPUNIT_ASSERT(wostr1.str() == wostr2.str());


  //----------------------------------Test READ / WRITE part------------------------------------------//

  //Creation a incorrect read/write driver
  MED_MESH_RDWR_DRIVER21 *aInvalidRdWrDriver21 = new  MED_MESH_RDWR_DRIVER21(fileNotExistsName_wr, aMesh_1);

  //Test case: trying open non existing file
  CPPUNIT_ASSERT_THROW(aInvalidRdWrDriver21->open(), MEDEXCEPTION);

  //Creation a correct read/write driver
  MED_MESH_RDWR_DRIVER21 *aRdWrDriver21 = new MED_MESH_RDWR_DRIVER21(filename_rdwr, aMesh_1);

  //Check driver
  CPPUNIT_ASSERT(aRdWrDriver21);

  //Test case: trying write mesh to file, if file is not open
  CPPUNIT_ASSERT_THROW(aRdWrDriver21->write(), MEDEXCEPTION);

  //Test case: trying read mesh from file, if file is not open
  CPPUNIT_ASSERT_THROW(aRdWrDriver21->read(), MEDEXCEPTION);

  //Test open() method
  try
  {
    aRdWrDriver21->open();
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
  CPPUNIT_ASSERT_NO_THROW(aRdWrDriver21->open());
  //This case is not work, seems it BUG
  //#endif

  //#ifdef ENABLE_FORCED_FAILURES
  //Set mesh name
  aMesh_1->setName("EmptyMesh");
  aRdWrDriver21->setMeshName("EmptyMesh");

  //Trying write in file empty mesh
  CPPUNIT_ASSERT_THROW(aRdWrDriver21->write(),MEDEXCEPTION);
  //No exception in this case, seems it BUG
  //#endif

  //Set initialy mesh name
  aMesh_1->setName(meshname);
  //Test setMeshName() and getMeshName() methods
  try
  {
    aRdWrDriver21->setMeshName(meshname);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_EQUAL(meshname, aRdWrDriver21->getMeshName());

  // Test read() method
  try
  {
    aRdWrDriver21->read();
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
  //#ifdef ENABLE_FORCED_FAILURES
  aRdWrDriver21->setMeshName("");
  aMesh_1->setName("");
  CPPUNIT_ASSERT_THROW(aRdWrDriver21->read(), MEDEXCEPTION);
  //No correct exception in this case

  //Trying write mesh to file, if mesh name is not set, i.e empty
  CPPUNIT_ASSERT_THROW(aRdWrDriver21->write(), MEDEXCEPTION);
  //No correct exception in this case

  //Trying read mesh from file with very long name
  aRdWrDriver21->setMeshName(longmeshname);
  aMesh_1->setName(longmeshname);
  CPPUNIT_ASSERT_THROW(aRdWrDriver21->read(), MEDEXCEPTION);
  //No correct exception in this case

  //Trying write mesh to file with very long name
  CPPUNIT_ASSERT_THROW(aRdWrDriver21->write(), MEDEXCEPTION);
  //No correct exception in this case
  //#endif

  //Test write() method
  aRdWrDriver21->setMeshName(newmeshname);
  aMesh_1->setName(newmeshname);
  try
  {
    aRdWrDriver21->write();
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
  MED_MESH_RDWR_DRIVER21 aRdWrDriver21Cpy_1;

  //Test (void operator =) defined in GENDRIVER class
  aRdWrDriver21Cpy_1 = *aRdWrDriver21;

  //Test (bool operator ==) defined GENDRIVER class
  CPPUNIT_ASSERT(aRdWrDriver21Cpy_1.GENDRIVER::operator==(*aRdWrDriver21));

  //Test copy constructor
  MED_MESH_RDWR_DRIVER21 aRdWrDriver21Cpy_2 (aRdWrDriver21Cpy_1);
  CPPUNIT_ASSERT_EQUAL(aRdWrDriver21Cpy_2, *aRdWrDriver21);

  try
  {
    aRdWrDriver21->close();
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
  CPPUNIT_ASSERT_THROW(aRdWrDriver21Cpy_1.read(), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aRdWrDriver21Cpy_2.read(), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aRdWrDriver21Cpy_1.write(), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aRdWrDriver21Cpy_2.write(), MEDEXCEPTION);

  //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  ostringstream rwostr1, rwostr2;
  rwostr1<<aRdWrDriver21Cpy_1;
  rwostr2<<aRdWrDriver21Cpy_2;
  CPPUNIT_ASSERT(rwostr1.str() != "");
  CPPUNIT_ASSERT(rwostr1.str() == rwostr2.str());

  //Delete all objects
  delete longmeshname;

  delete aInvalidRdDriver21;
  delete aRdDriver21;

  delete aInvalidWrDriver21;
  delete aWrDriver21;
  delete aTmpWrDriver21;

  delete aInvalidRdWrDriver21;
  delete aRdWrDriver21;

  delete aMesh;
  delete aMesh_1;
}
