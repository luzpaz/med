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

#include <MEDMEM_GibiMeshDriver.hxx>
#include <MEDMEM_Mesh.hxx>
//#include <MEDMEM_Compatibility21_22.hxx>
//#include <MEDMEM_MedMedDriver22.hxx>
#include <MEDMEM_Med.hxx>

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

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

/*!
 *  Check methods (31), defined in MEDMEM_GibiMeshDriver.hxx:
 *  class GIBI_MESH_DRIVER : public GENDRIVER {
 *   (+) GIBI_MESH_DRIVER();
 *   (+) GIBI_MESH_DRIVER(const string & fileName, MESH * ptrMesh, MED_EN::med_mode_acces accessMode);
 *   (+) GIBI_MESH_DRIVER(const GIBI_MESH_DRIVER & driver);
 *   (+) virtual ~GIBI_MESH_DRIVER();
 *   (+) virtual void write(void) const = 0;
 *   (+) virtual void read (void) = 0;
 *   (+) void   setMeshName(const string & meshName);
 *   (+) string getMeshName() const;
 *   (+) static MED_EN::medGeometryElement gibi2medGeom(size_t gibiTypeNb);
 *   (+) static int med2gibiGeom(MED_EN::medGeometryElement medGeomType);
 *  }
 *  class GIBI_MESH_RDONLY_DRIVER : public virtual GIBI_MESH_DRIVER {
 *   (+) GIBI_MESH_RDONLY_DRIVER();
 *   (+) GIBI_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh);
 *   (+) GIBI_MESH_RDONLY_DRIVER(const GIBI_MESH_RDONLY_DRIVER & driver);
 *   (+) virtual ~GIBI_MESH_RDONLY_DRIVER();
 *   (+) void write(void) const throw (MEDEXCEPTION);
 *   (+) void read (void) throw (MEDEXCEPTION);
 *   (+) void open();
 *   (+) void close();
 *  }
 *  class GIBI_MESH_WRONLY_DRIVER : public virtual GIBI_MESH_DRIVER {
 *   (+) GIBI_MESH_WRONLY_DRIVER();
 *   (+) GIBI_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh);
 *   (+) GIBI_MESH_WRONLY_DRIVER(const GIBI_MESH_WRONLY_DRIVER & driver);
 *   (+) virtual ~GIBI_MESH_WRONLY_DRIVER();
 *   (+) void write(void) const throw (MEDEXCEPTION);
 *   (+) void read (void) throw (MEDEXCEPTION);
 *   (+) void open();
 *   (+) void close();
 *  }
 *  class GIBI_MESH_RDWR_DRIVER : public GIBI_MESH_RDONLY_DRIVER, public GIBI_MESH_WRONLY_DRIVER {
 *   (yetno) GIBI_MESH_RDWR_DRIVER();
 *   (yetno) GIBI_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh);
 *   (yetno) GIBI_MESH_RDWR_DRIVER(const GIBI_MESH_RDWR_DRIVER & driver);
 *   (yetno) ~GIBI_MESH_RDWR_DRIVER();
 *   (yetno) void write(void) const throw (MEDEXCEPTION);
 *   (yetno) void read (void) throw (MEDEXCEPTION);
 *   (yetno) void open();
 *   (yetno) void close();
 *  }
 *  class GIBI_MED_RDONLY_DRIVER : public GIBI_MESH_RDONLY_DRIVER {
 *   (+) GIBI_MED_RDONLY_DRIVER();
 *   (+) GIBI_MED_RDONLY_DRIVER(const string & fileName, MED * ptrMed);
 *   (+) GIBI_MED_RDONLY_DRIVER(const GIBI_MED_RDONLY_DRIVER & driver);
 *   (+) virtual ~GIBI_MED_RDONLY_DRIVER();
 *   (+) void read (void) throw (MEDEXCEPTION);
 *  }
 *  class GIBI_MED_WRONLY_DRIVER : public GIBI_MESH_WRONLY_DRIVER {
 *   (+) GIBI_MED_WRONLY_DRIVER();
 *   (+) GIBI_MED_WRONLY_DRIVER(const string & fileName, MED * ptrMed, MESH * ptrMesh);
 *   (+) GIBI_MED_WRONLY_DRIVER(const GIBI_MED_WRONLY_DRIVER & driver);
 *   (+) virtual ~GIBI_MED_WRONLY_DRIVER();
 *   (+) void write(void) const throw (MEDEXCEPTION);
 *  }
 */

void MEDMEMTest::testGibiMeshDriver()
{
  MESH *aMesh                      = new MESH();
  MESH *aMesh_1                    = NULL;
  MED  *aMed                       = new MED();
  MED  *aMed_1                     = NULL;
  string data_dir                  = getenv("DATA_DIR");
  string tmp_dir                   = getenv("TMP");
  string filename_rd               = data_dir + "/Sauv/Darcy3_3D_H_10x10x10.sauve";
  string filenamemed_rd            = data_dir + "/Sauv/elle_3D_HPr_10x10x10.sauve";
  string filename_wr               = tmp_dir  + "/myWr_Darcy3_3D_H_10x10x10.sauve";
  string tmpfile                   = tmp_dir  + "/tmp.sauve";
  string filenamemed_wr            = tmp_dir  + "/myWrMed_elle_3D_HPr_10x10x10.sauve";
  string tmpfilemed                = tmp_dir  + "/tmpmed.sauve";
  string meshname                  = "Darcy3_3D_H_10x10x10";
  string newmeshname               = "new" + meshname;
  string fileNotExistsName_rd      = "notExists.sauve";
  string fileNotExistsName_wr      = "/path_not_exists/file_not_exists.sauve";
  string filename_rdwr             =  tmp_dir  + "/myRdWr_Darcy3_3D_H_10x10x10.sauve";
  string fcopy                     = "cp " + filename_rd + " " + filename_rdwr;


  //Test gibi2medGeom() and med2gibiGeom() methods
  size_t aSize = 17;
  CPPUNIT_ASSERT_EQUAL(GIBI_MESH_DRIVER::gibi2medGeom(aSize), MED_PENTA15);
  CPPUNIT_ASSERT_EQUAL(GIBI_MESH_DRIVER::med2gibiGeom(MED_PENTA15), 17);

  //----------------------------------Test GIBI MESH READ ONLY part------------------------------------------//

  //Creation a incorrect read only driver
  GIBI_MESH_RDONLY_DRIVER *aInvalidGibiRdDriver = new GIBI_MESH_RDONLY_DRIVER(fileNotExistsName_rd, aMesh);

  //Trying open not existing file
  CPPUNIT_ASSERT_THROW(aInvalidGibiRdDriver->open(), MEDEXCEPTION);

  //Creation a correct Gibi read only driver (normal constructor)
  GIBI_MESH_RDONLY_DRIVER *aGibiRdDriver = new GIBI_MESH_RDONLY_DRIVER(filename_rd, aMesh);

  //Check driver
  CPPUNIT_ASSERT(aGibiRdDriver);

  //Trying read mesh from file, if file is not open
  CPPUNIT_ASSERT_THROW(aGibiRdDriver->read(), MEDEXCEPTION);

  //Test open() method
  try
  {
    aGibiRdDriver->open();
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
  //Trying open file secondary.
  CPPUNIT_ASSERT_THROW(aGibiRdDriver->open(), MEDEXCEPTION);
  //This case is not work, seems it BUG
#endif

  //Test setMeshName() and getMeshName() methods
  try
  {
    aGibiRdDriver->setMeshName(meshname);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_EQUAL(meshname, aGibiRdDriver->getMeshName());

  //Test read() method
  try
  {
    aGibiRdDriver->read();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Trying fill not empty mesh
  CPPUNIT_ASSERT_THROW(aGibiRdDriver->read(), MEDEXCEPTION);

  //Test write() method for GIBI READ ONLY driver
  CPPUNIT_ASSERT_THROW(aGibiRdDriver->write(), MEDEXCEPTION);

  //Check Mesh
  CPPUNIT_ASSERT(aMesh);

  //Test close method
  try
  {
    aGibiRdDriver->close();
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
  GIBI_MESH_RDONLY_DRIVER aGibiRdDriverCpy_1;

  //Test (void operator =) defined in GENDRIVER class in MEDMEM_GenDriver.hxx
  aGibiRdDriverCpy_1 = *aGibiRdDriver;
  CPPUNIT_ASSERT_EQUAL(aGibiRdDriverCpy_1, *aGibiRdDriver);

  //Test (bool operator ==) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  CPPUNIT_ASSERT(aGibiRdDriverCpy_1 == *aGibiRdDriver);

  //Test copy constructor
  GIBI_MESH_RDONLY_DRIVER aGibiRdDriverCpy_2 = GIBI_MESH_RDONLY_DRIVER(aGibiRdDriverCpy_1);
  CPPUNIT_ASSERT_EQUAL(aGibiRdDriverCpy_2, *aGibiRdDriver);

  //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  ostringstream rostr1, rostr2;
  rostr1<<*aGibiRdDriver;
  rostr2<<aGibiRdDriverCpy_1;
  CPPUNIT_ASSERT(rostr1.str() != "");
  CPPUNIT_ASSERT(rostr1.str() == rostr2.str());


  //----------------------------------Test GIBI WRITE ONLY part------------------------------------------//
  //Creation a incorrect gibi write only driver
  GIBI_MESH_WRONLY_DRIVER *aInvalidGibiWrDriver = new GIBI_MESH_WRONLY_DRIVER(fileNotExistsName_wr, aMesh);

  //Trying open non existing file
  CPPUNIT_ASSERT_THROW(aInvalidGibiWrDriver->open(), MEDEXCEPTION);

  //Creation a correct gibi write only drivers
  GIBI_MESH_WRONLY_DRIVER *aGibiWrDriver = new GIBI_MESH_WRONLY_DRIVER(filename_wr, aMesh);

  //Trying write empty mesh
  GIBI_MESH_WRONLY_DRIVER *aTmpGibiWrDriver = new GIBI_MESH_WRONLY_DRIVER(tmpfile, aMesh_1);
  aTmpGibiWrDriver->open();

  CPPUNIT_ASSERT_THROW(aTmpGibiWrDriver->write(),MEDEXCEPTION);
  aTmpGibiWrDriver->close();

  //Check driver
  CPPUNIT_ASSERT(aGibiWrDriver);

  //Trying write mesh to file, if file is not open
  CPPUNIT_ASSERT_THROW(aGibiWrDriver->write(), MEDEXCEPTION);

  //Test open() method
  try
  {
    aGibiWrDriver->open();
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
  //Trying open file secondary.
  CPPUNIT_ASSERT_THROW(aGibiWrDriver->open(), MEDEXCEPTION);
  //his case is not work, seems it BUG
#endif

  //Test setMeshName() and getMeshName() methods
  try
  {
    aGibiWrDriver->setMeshName(newmeshname);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_EQUAL(newmeshname, aGibiWrDriver->getMeshName());


  //Test write() method
  try
  {
    aGibiWrDriver->write();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Test read() method for GIBI WRITE ONLY driver
  CPPUNIT_ASSERT_THROW(aGibiWrDriver->read(), MEDEXCEPTION);

  //Test close method
  try
  {
    aGibiWrDriver->close();
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
  GIBI_MESH_WRONLY_DRIVER aGibiWrDriverCpy_1;
  //Test (void operator =) defined in GENDRIVER class in MEDMEM_GenDriver.hxx
  //Compilation error after using this operator
  //aGibiWrDriverCpy_1 = *aGibiWrDriver;
  //CPPUNIT_ASSERT_EQUAL(aGibiWrDriverCpy_1, *aGibiWrDriver);
#ifdef ENABLE_FORCED_FAILURES
  CPPUNIT_FAIL("GIBI_MESH_WRONLY_DRIVER::operator=: Error during compilation");
#endif

  //Test copy constructor
  GIBI_MESH_WRONLY_DRIVER aGibiWrDriverCpy_2 = GIBI_MESH_WRONLY_DRIVER(*aGibiWrDriver);
  CPPUNIT_ASSERT_EQUAL(aGibiWrDriverCpy_2, *aGibiWrDriver);

  //Test (bool operator ==) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  CPPUNIT_ASSERT(aGibiWrDriverCpy_2 == *aGibiWrDriver);

  //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  ostringstream wostr1, wostr2;
  wostr1<<*aGibiWrDriver;
  wostr2<<aGibiWrDriverCpy_2;
  CPPUNIT_ASSERT(wostr1.str() != "");
  CPPUNIT_ASSERT(wostr1.str() == wostr2.str());




#ifdef ENABLE_FORCED_FAILURES
  CPPUNIT_FAIL("GIBI_MESH_RDWR_DRIVER not implemented");
#endif
  /* Read/write part not implemented
  //----------------------------------Test GIBI READ / WRITE part------------------------------------------//

  //Creation a incorrect gibi read/write driver
  GIBI_MESH_RDWR_DRIVER *aInvalidGibiRdWrDriver = new  GIBI_MESH_RDWR_DRIVER(fileNotExistsName_wr, aMesh_1);

  //Test case: trying open non existing file
  CPPUNIT_ASSERT_THROW(aInvalidGibiRdWrDriver->open(), MEDEXCEPTION);

  //Creation a correct read/write driver
  GIBI_MESH_RDWR_DRIVER *aGibiRdWrDriver = new GIBI_MESH_RDWR_DRIVER(filename_rdwr, aMesh);

  //Check driver
  CPPUNIT_ASSERT(aGibiRdWrDriver);

  //Test case: trying write mesh to file, if file is not open
  CPPUNIT_ASSERT_THROW(aGibiRdWrDriver->write(), MEDEXCEPTION);

  //Test case: trying read mesh from file, if file is not open
  CPPUNIT_ASSERT_THROW(aGibiRdWrDriver->read(), MEDEXCEPTION);

  //Test open() method
  try
  {
    aGibiRdWrDriver->open();
  }
  catch(MEDEXCEPTION &e)
    {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  //Test case: trying open file secondary.
  //CPPUNIT_ASSERT_THROW(aGibiRdWrDriver->open(), MEDEXCEPTION);
  //This case is not work, seems it BUG


  //Test read() method
  try
  {
    aGibiRdWrDriver->read();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Test write() method
  aGibiRdWrDriver->setMeshName(newmeshname);
  try
  {
    aGibiRdWrDriver->write();
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

  //Default constructor
  //GIBI_MESH_RDWR_DRIVER aGibiRdWrDriverCpy_1;

  //Test (void operator =) defined in GENDRIVER class in MEDMEM_GenDriver.hxx
  //aGibiRdWrDriverCpy_1 = *aGibiRdWrDriver;

  //Test copy constructor
  GIBI_MESH_RDWR_DRIVER aGibiRdWrDriverCpy_1 = GIBI_MESH_RDWR_DRIVER(*aGibiRdWrDriver);
  CPPUNIT_ASSERT_EQUAL(aGibiRdWrDriverCpy_1, *aGibiRdWrDriver);

  //Test (bool operator ==) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  CPPUNIT_ASSERT(aGibiRdWrDriverCpy_1 == *aGibiRdWrDriver);

  //Test close method
  try
  {
    aGibiRdWrDriver->close();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }


  //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  ostringstream rwostr1, rwostr2;
  rwostr1<<*aGibiRdWrDriver;
  rwostr2<<aGibiRdWrDriverCpy_1;
  CPPUNIT_ASSERT(rwostr1.str() != "");
  CPPUNIT_ASSERT(rwostr1.str() == rwostr2.str());
  */
  
  //--------------------------------Test GIBI_MED_RDONLY_DRIVER-----------------------------------------
  GIBI_MED_RDONLY_DRIVER *aInvalidMedGibiRdDriver = new GIBI_MED_RDONLY_DRIVER(fileNotExistsName_rd, aMed);
    
  //Trying open not exising file
  CPPUNIT_ASSERT_THROW(aInvalidMedGibiRdDriver->open(), MEDEXCEPTION);
  
  //Creation a correct Gibi read only driver (normal constructor)
  GIBI_MED_RDONLY_DRIVER *aGibiMedRdDriver = new GIBI_MED_RDONLY_DRIVER(filenamemed_rd, aMed);

  //Check driver
  CPPUNIT_ASSERT(aGibiMedRdDriver);

  //Trying read mesh from file, if file is not open
  CPPUNIT_ASSERT_THROW(aGibiMedRdDriver->read(), MEDEXCEPTION);

  //Test open() method
  try
  {
    aGibiMedRdDriver->open();
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
  //Trying open file secondary.
  CPPUNIT_ASSERT_THROW(aGibiMedRdDriver->open(), MEDEXCEPTION);
  //This case is not work, seems it BUG
#endif
  
  //Test read() method
  try
  {
    aGibiMedRdDriver->read();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Test write() method for GIBI MED READ ONLY driver
  CPPUNIT_ASSERT_THROW(aGibiMedRdDriver->write(), MEDEXCEPTION);

  //Check Med
  CPPUNIT_ASSERT(aMed);
  
  //Test close method
  try
  {
    aGibiMedRdDriver->close();
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
  GIBI_MED_RDONLY_DRIVER aGibiMedRdDriverCpy_1;

  //Test (void operator =) defined in GENDRIVER class in MEDMEM_GenDriver.hxx
  aGibiMedRdDriverCpy_1 = *aGibiMedRdDriver;
  CPPUNIT_ASSERT_EQUAL(aGibiMedRdDriverCpy_1, *aGibiMedRdDriver);

  //Test (bool operator ==) defined in GENDRIVER class in MEDMEM_GenDriver.hxx
  CPPUNIT_ASSERT(aGibiMedRdDriverCpy_1 == *aGibiMedRdDriver);

  //Test copy constructor
  GIBI_MED_RDONLY_DRIVER aGibiMedRdDriverCpy_2 = GIBI_MED_RDONLY_DRIVER(*aGibiMedRdDriver);
#ifdef ENABLE_FORCED_FAILURES  
  CPPUNIT_ASSERT_EQUAL_MESSAGE("Copy constructor work incorrect...",aGibiMedRdDriverCpy_2, *aGibiMedRdDriver);
#endif

  //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  ostringstream medrostr1, medrostr2;
  medrostr1<<*aGibiMedRdDriver;
  medrostr2<<aGibiMedRdDriverCpy_1;
  CPPUNIT_ASSERT(medrostr1.str() != "");
  CPPUNIT_ASSERT(medrostr1.str() == medrostr2.str());


  //----------------------------------Test GIBI MED WRITE ONLY part------------------------------------------//
  //Creation a incorrect gibi med write only driver
  GIBI_MED_WRONLY_DRIVER *aInvalidGibiMedWrDriver = new GIBI_MED_WRONLY_DRIVER(fileNotExistsName_wr, aMed ,aMesh);
  
  //Trying open non existing file
  CPPUNIT_ASSERT_THROW(aInvalidGibiMedWrDriver->open(), MEDEXCEPTION);

   //Trying create gibi med write only driver with null MED and MESH
  CPPUNIT_ASSERT_THROW(GIBI_MED_WRONLY_DRIVER *aTmpGibiMedWrDriver = new GIBI_MED_WRONLY_DRIVER(tmpfilemed, aMed_1 ,aMesh_1), MEDEXCEPTION);

 //Creation a correct gibi med write only drivers
  GIBI_MED_WRONLY_DRIVER *aGibiMedWrDriver = new GIBI_MED_WRONLY_DRIVER(filenamemed_wr, aMed, aMesh);

  //Check driver
  CPPUNIT_ASSERT(aGibiMedWrDriver);

  //Trying write mesh to file, if file is not open
  CPPUNIT_ASSERT_THROW(aGibiMedWrDriver->write(), MEDEXCEPTION);

  //Test open() method
  try
  {
    aGibiMedWrDriver->open();
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
  //Trying open file secondary.
  CPPUNIT_ASSERT_THROW(aGibiMedWrDriver->open(), MEDEXCEPTION);
  //Exception is generated, but second call open() function break driver
#endif

  //Test write() method
  try
  {
    aGibiMedWrDriver->write();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Test read() method for GIBI MED WRITE ONLY driver
  CPPUNIT_ASSERT_THROW(aGibiMedWrDriver->read(), MEDEXCEPTION);

  //Test close method
  try
  {
    aGibiMedWrDriver->close();
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
  GIBI_MED_WRONLY_DRIVER aGibiMedWrDriverCpy_1;
  //Test (void operator =) defined in GENDRIVER class in MEDMEM_GenDriver.hxx
  //Compilation error after using this operator
  //aGibiMedWrDriverCpy_1 = *aGibiMedWrDriver;
  //CPPUNIT_ASSERT_EQUAL(aGibiMedWrDriverCpy_1, *aGibiMedWrDriver);
#ifdef ENABLE_FORCED_FAILURES
  CPPUNIT_FAIL("GIBI_MED_WRONLY_DRIVER::operator=: Error during compilation");
#endif


#ifdef ENABLE_FAULTS
  //Test copy constructor
  GIBI_MED_WRONLY_DRIVER aGibiMedWrDriverCpy_2 = GIBI_MED_WRONLY_DRIVER(*aGibiMedWrDriver);
  //Segmentation fault after delete copied object
#ifdef ENABLE_FORCED_FAILURES  
  CPPUNIT_ASSERT_EQUAL_MESSAGE("Copy constructor work incorrect...",*aGibiMedWrDriverCpy_2, *aGibiMedWrDriver);
#endif

  //Test (bool operator ==) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  CPPUNIT_ASSERT(aGibiMedWrDriverCpy_2 == *aGibiMedWrDriver);
  //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  ostringstream medwostr1, medwostr2;
  medwostr1<<*aGibiMedWrDriver;
  medwostr2<<aGibiMedWrDriverCpy_2;
  CPPUNIT_ASSERT(medwostr1.str() != "");
  CPPUNIT_ASSERT(medwostr1.str() == medwostr2.str());
#endif  

  //Delete all objects
  delete aMesh;
  delete aInvalidGibiRdDriver;
  delete aGibiRdDriver;

  delete aInvalidGibiWrDriver;
  delete aTmpGibiWrDriver;
  delete aGibiWrDriver;

  delete aInvalidMedGibiRdDriver;
  delete aGibiMedRdDriver;

  delete aInvalidGibiMedWrDriver;
  delete aGibiMedWrDriver;

  //Remove tmp files
  remove(filename_wr.data());
  remove(tmpfile.data());
  remove(tmpfilemed.data());
  remove(filenamemed_wr.data());
}
