//  Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include <MEDMEM_Compatibility21_22.hxx>
#include <MEDMEM_MedMedDriver22.hxx>
#include <MEDMEM_Med.hxx>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

/*!
 *  Check methods (21), defined in MEDMEM_MedMedDriver22.hxx:
 *  class MED_MED_DRIVER22 : public virtual MED_MED_DRIVER {
 *   (+) MED_MED_DRIVER22();
 *   (+) MED_MED_DRIVER22(const string & fileName,  MED * const ptrMed);
 *   (+) MED_MED_DRIVER22(const string & fileName,  MED * const ptrMed,
 *                            MED_EN::med_mode_acces accessMode);
 *   (+) MED_MED_DRIVER22(const MED_MED_DRIVER22 & driver);
 *   (+) virtual ~MED_MED_DRIVER22();
 *   (+) void open() throw (MEDEXCEPTION);
 *   (+) void close();
 *   (+) virtual void write          (void) const;
 *   (+) virtual void writeFrom      (void) const;
 *   (+) virtual void read           (void);
 *   (+) virtual void readFileStruct (void);
 *  }
 *  class MED_MED_RDONLY_DRIVER22 : public virtual IMED_MED_RDONLY_DRIVER,
 *                                  public virtual MED_MED_DRIVER22 {
 *   (+) MED_MED_RDONLY_DRIVER22();
 *   (+) MED_MED_RDONLY_DRIVER22(const string & fileName,  MED * const ptrMed);
 *   (+) MED_MED_RDONLY_DRIVER22(const MED_MED_RDONLY_DRIVER22 & driver);
 *   (+) virtual ~MED_MED_RDONLY_DRIVER22();
 *   (+) void write          (void) const throw (MEDEXCEPTION);
 *   (+) void writeFrom      (void) const throw (MEDEXCEPTION);
 *   (+) void read           (void) throw (MEDEXCEPTION);
 *   (+) void readFileStruct (void) throw (MEDEXCEPTION);
 *  }
 *  class MED_MED_WRONLY_DRIVER22 : public virtual IMED_MED_WRONLY_DRIVER,
 *                                  public virtual MED_MED_DRIVER22 {
 *   (+) MED_MED_WRONLY_DRIVER22();
 *   (+) MED_MED_WRONLY_DRIVER22(const string & fileName,  MED * const ptrMed);
 *   (+) MED_MED_WRONLY_DRIVER22(const MED_MED_WRONLY_DRIVER22 & driver);
 *   (+) virtual ~MED_MED_WRONLY_DRIVER22();
 *   (+) void write          (void) const throw (MEDEXCEPTION);
 *   (+) void writeFrom      (void) const throw (MEDEXCEPTION);
 *   (+) void read           (void) throw (MEDEXCEPTION);
 *   (+) void readFileStruct (void) throw (MEDEXCEPTION);
 *  }
 *  class MED_MED_RDWR_DRIVER22 : public virtual MED_MED_RDONLY_DRIVER22,
 *                                public virtual MED_MED_WRONLY_DRIVER22,
 *                                public virtual IMED_MED_RDWR_DRIVER {
 *   (+) MED_MED_RDWR_DRIVER22();
 *   (+) MED_MED_RDWR_DRIVER22(const string & fileName,  MED * const ptrMed);
 *   (+) MED_MED_RDWR_DRIVER22(const MED_MED_RDWR_DRIVER22 & driver);
 *   (+) ~MED_MED_RDWR_DRIVER22();
 *   (+) void write          (void) const throw (MEDEXCEPTION);
 *   (+) void writeFrom      (void) const throw (MEDEXCEPTION);
 *   (+) void read           (void) throw (MEDEXCEPTION);
 *   (+) void readFileStruct (void) throw (MEDEXCEPTION);
 *  }
 */
void MEDMEMTest::testMedMedDriver22() {
  MED *aMed                        = new MED();
  string filename_rd               = getResourceFile("pointe_import22.med");
  string emptyfilename             = "";
  string filename_wr               = makeTmpFile("myMedWr_pointe22.med");
  string fileNotExistsName_rd      = "notExists.med";
  string fileNotExistsName_wr      = "/path_not_exists/file_not_exists.med";
  string filename_rdwr             = makeTmpFile("myMedRdWr_pointe22.med", filename_rd);

  // To remove tmp files from disk
  MEDMEMTest_TmpFilesRemover aRemover;
  aRemover.Register(filename_wr);
  aRemover.Register(filename_rdwr);

  //-------------------------------Test READ_ONLY part-------------------------------------------------------
  //Creation incorrect Med_Med read only driver (file is not exist)
  MED_MED_RDONLY_DRIVER22 *aInvalidMedMedRdDriver22 = new MED_MED_RDONLY_DRIVER22(fileNotExistsName_rd, aMed);

  //Trying open not existing file
  CPPUNIT_ASSERT_THROW(aInvalidMedMedRdDriver22->open(),MEDEXCEPTION);

  //Creation incorrect Med_Med read only driver (with empty file name)
  MED_MED_RDONLY_DRIVER22 *aInvalidMedMedRdDriver22_1 = new MED_MED_RDONLY_DRIVER22(emptyfilename, aMed);

  //Trying open file with empty name
  CPPUNIT_ASSERT_THROW(aInvalidMedMedRdDriver22_1->open(),MEDEXCEPTION);


  //Creation a correct Med_Med read only driver (normal constructor)
  MED_MED_RDONLY_DRIVER22 *aMedMedRdDriver22 = new MED_MED_RDONLY_DRIVER22(filename_rd, aMed);

  //Check driver
  CPPUNIT_ASSERT(aMedMedRdDriver22);

  //Trying read mesh from file, if file is not open
  // !!!!!!!! Med driver does NOT NEED open() before read() but only before readFileStruct() !!!!
  //#ifdef  ENABLE_FORCED_FAILURES
  //CPPUNIT_ASSERT_THROW(aMedMedRdDriver22->read(), MEDEXCEPTION);
  //CPPUNIT_ASSERT_NO_THROW(aMedMedRdDriver22->read());
  // (BUG) No exception in this case.
  //#endif

  CPPUNIT_ASSERT_THROW(aMedMedRdDriver22->readFileStruct(),MEDEXCEPTION);

  //Test open() method
  try
  {
    aMedMedRdDriver22->open();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Trying open() file twice
  //#ifdef  ENABLE_FORCED_FAILURES
  //CPPUNIT_ASSERT_THROW(aMedMedRdDriver22->open(),MEDEXCEPTION);
        //  CPPUNIT_ASSERT_NO_THROW(aMedMedRdDriver22->open());
  //(BUG) No exception in this case.
  //#endif

  //Test read() method
  try
  {
    aMedMedRdDriver22->read();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Test read() method
  try
  {
    aMedMedRdDriver22->readFileStruct();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Test write() and WriteFrom() methods for READ_ONLY part
  CPPUNIT_ASSERT_THROW(aMedMedRdDriver22->write(), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aMedMedRdDriver22->writeFrom(), MEDEXCEPTION);

  //Check MED
  CPPUNIT_ASSERT(aMed);

  //Test close() method
  try
  {
    aMedMedRdDriver22->close();
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
  MED_MED_RDONLY_DRIVER22 aMedMedRdDriver22Cpy_1;

  //Test (void operator =) defined in GENDRIVER class
  //aMedMedRdDriver22Cpy_1 = *aMedMedRdDriver22;

  //Test copy constructor
  //#ifdef ENABLE_FAULTS
  MED_MED_RDONLY_DRIVER22 aMedMedRdDriver22Cpy_2 (*aMedMedRdDriver22);
  // (BUG) => Segmentation fault, because _concreteMedDrv is NULL
  //#endif
  //CPPUNIT_ASSERT_EQUAL(aMedMedRdDriver22Cpy_2, *aMedMedRdDriver22);

  //Test (bool operator ==) defined in GENDRIVER class in MEDMEM_GenDriver.hxx
  //CPPUNIT_ASSERT(aMedMedRdDriver22Cpy_2.GENDRIVER::operator==(*aMedMedRdDriver22));

  //Test (friend ostream & operator <<) defined GENDRIVER class
  //ostringstream rwostr1, rwostr2;
  //rwostr1<<aMedMedRdDriver22Cpy_1;
  //rwostr2<<aMedMedRdDriver22Cpy_2;
  //CPPUNIT_ASSERT(rwostr1.str() != "");
  //CPPUNIT_ASSERT(rwostr1.str() == rwostr2.str());

  //-------------------------------Test WRITE_ONLY part-------------------------------------------------------
  //Creation incorrect Med_Med write only driver (file is not exist)
  MED_MED_WRONLY_DRIVER22 *aInvalidMedMedWrDriver22 = new MED_MED_WRONLY_DRIVER22(fileNotExistsName_wr, aMed);

  //Trying open not existing file
  CPPUNIT_ASSERT_THROW(aInvalidMedMedWrDriver22->open(),MEDEXCEPTION);

  //Creation incorrect Med_Med write only driver (with empty file name)
  MED_MED_WRONLY_DRIVER22 *aInvalidMedMedWrDriver22_1 = new MED_MED_WRONLY_DRIVER22(emptyfilename, aMed);

  //Trying open file with empty name
  CPPUNIT_ASSERT_THROW(aInvalidMedMedWrDriver22_1->open(),MEDEXCEPTION);


  //Creation a correct Med_Med write only driver (normal constructor)
  MED_MED_WRONLY_DRIVER22 *aMedMedWrDriver22 = new MED_MED_WRONLY_DRIVER22(filename_wr, aMed);

  //Check driver
  CPPUNIT_ASSERT(aMedMedWrDriver22);


  //Trying write mesh to file, if file is not open
  // !!!!!!!! Med driver does NOT NEED open() before writeFrom() and write() !!!!
  //#ifdef ENABLE_FORCED_FAILURES
  //CPPUNIT_ASSERT_THROW(aMedMedWrDriver22->writeFrom(),MEDEXCEPTION);
  // (BUG) No exception in this case
  //#endif

  //#ifdef  ENABLE_FAULTS
  // !!!!!!!! Med driver does NOT NEED open() before writeFrom() and write() !!!!
  // CPPUNIT_ASSERT_THROW(aMedMedWrDriver22->write(), MEDEXCEPTION);
  //(BUG) => Segmentation fault
  //#endif

  //Test open() method
  try
  {
    aMedMedWrDriver22->open();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Trying open() file twice
  //#ifdef  ENABLE_FORCED_FAILURES
  CPPUNIT_ASSERT_NO_THROW(aMedMedWrDriver22->open());
  //(BUG) No exception in this case.
  //#endif

  //Test writeFrom() method
  try
  {
    aMedMedWrDriver22->writeFrom();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //#ifdef ENABLE_FAULTS
  //Test write() method
  try
  {
    aMedMedWrDriver22->write();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
   CPPUNIT_FAIL("Unknown exception");
  }
  // (BUG) => terminate called after throwing an instance of 'std::length_error'
  // what():  basic_string::_S_create
  //#endif

  //Test close method
  try
  {
    aMedMedWrDriver22->close();
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
  MED_MED_WRONLY_DRIVER22 aMedMedWrDriver22Cpy_1;

  //Test (void operator =) defined in GENDRIVER class
  //aMedMedWrDriver22Cpy_1 = *aMedMedWrDriver22;

  //Test copy constructor
  //#ifdef ENABLE_FAULTS
  MED_MED_WRONLY_DRIVER22 aMedMedWrDriver22Cpy_2 (*aMedMedWrDriver22);
  // (BUG) => Segmentation fault
  //#endif
  //CPPUNIT_ASSERT_EQUAL(aMedMedWrDriver22Cpy_2, *aMedMedWrDriver22);

  //Test (bool operator ==) defined in GENDRIVER class
  //CPPUNIT_ASSERT(aMedMedWrDriver22Cpy_2.GENDRIVER::operator==(*aMedMedWrDriver22));

  //Test (friend ostream & operator <<) defined GENDRIVER class
  //ostringstream rwostr1, rwostr2;
  //rwostr1<<aMedMedWrDriver22Cpy_1;
  //rwostr2<<aMedMedWrDriver22Cpy_2;
  //CPPUNIT_ASSERT(rwostr1.str() != "");
  //CPPUNIT_ASSERT(rwostr1.str() == rwostr2.str());


//-------------------------------Test READ/WRITE part-------------------------------------------------------
  //Creation incorrect Med_Med read/write driver (file is not exist)
  MED_MED_RDWR_DRIVER22 *aInvalidMedMedRdWrDriver22 = new MED_MED_RDWR_DRIVER22(fileNotExistsName_wr, aMed);

  //Trying open not existing file
  CPPUNIT_ASSERT_THROW(aInvalidMedMedRdWrDriver22->open(),MEDEXCEPTION);

  //Creation incorrect Med_Med read/write driver (with empty file name)
  MED_MED_RDWR_DRIVER22 *aInvalidMedMedRdWrDriver22_1 = new MED_MED_RDWR_DRIVER22(emptyfilename, aMed);

  //Trying open file with empty name
  CPPUNIT_ASSERT_THROW(aInvalidMedMedRdWrDriver22_1->open(),MEDEXCEPTION);


  //Creation a correct Med_Med read/write driver (normal constructor)
  MED_MED_RDWR_DRIVER22 *aMedMedRdWrDriver22 = new MED_MED_RDWR_DRIVER22(filename_rdwr, aMed);

  //Check driver
  CPPUNIT_ASSERT(aMedMedRdWrDriver22);

  //Trying read MED from file if it is not open
  // !!!!!!!! Med driver does NOT NEED open() before read() but only before readFileStruct() !!!!
  //#ifdef  ENABLE_FORCED_FAILURES
  //CPPUNIT_ASSERT_THROW(aMedMedRdWrDriver22->read(), MEDEXCEPTION);
  // (BUG) No exception in this case.
  //#endif

  CPPUNIT_ASSERT_THROW(aMedMedRdWrDriver22->readFileStruct(),MEDEXCEPTION);

  //Trying write mesh to file, if file is not open
  // !!!!!!!! Med driver does NOT NEED open() before writeFrom() but only before write() !!!!
  //#ifdef ENABLE_FORCED_FAILURES
  //CPPUNIT_ASSERT_THROW(aMedMedRdWrDriver22->writeFrom(),MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW(aMedMedRdWrDriver22->writeFrom());
  // (BUG) => No exception in this case
  //#endif

  //#ifdef ENABLE_FAULTS
  // !!!!!!! It does not throught since nothing has been yet read, so no attempts to write
  //CPPUNIT_ASSERT_THROW(aMedMedRdWrDriver22->write(), MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW(aMedMedRdWrDriver22->write());
  //(BUG) => Crash
  //#endif

  //Test open() method
  try
  {
    aMedMedRdWrDriver22->open();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Trying open() file twice
  //!!!! do not make troubles for the user
  //#ifdef  ENABLE_FORCED_FAILURES
  //CPPUNIT_ASSERT_THROW(aMedMedRdWrDriver22->open(),MEDEXCEPTION);
  //(BUG) No exception in this case.
  //#endif

  //Test readFileStruct() method
    try
  {
    aMedMedRdWrDriver22->readFileStruct();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Test read() method
  try
  {
    aMedMedRdWrDriver22->read();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Test writeFrom() method
  try
  {
    aMedMedRdWrDriver22->writeFrom();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //#ifdef ENABLE_FAULTS
  //Test write() method
  try
  {
    aMedMedRdWrDriver22->write();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  //#endif

  //Test close() method
  try
  {
    aMedMedRdWrDriver22->close();
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
  MED_MED_RDWR_DRIVER22 aMedMedRdWrDriver22Cpy_1;

  //Test copy constructor
  //#ifdef ENABLE_FAULTS
  MED_MED_RDWR_DRIVER22 aMedMedRdWrDriver22Cpy_2 (*aMedMedRdWrDriver22);
  // (BUG) => Segmentation fault
  //#endif
  //CPPUNIT_ASSERT_EQUAL(aMedMedRdWrDriver22Cpy_2, *aMedMedRdWrDriver22);

  //Test (bool operator ==) defined in GENDRIVER class
  //CPPUNIT_ASSERT(aMedMedRdWrDriver22Cpy_2.GENDRIVER::operator==(*aMedMedRdWrDriver22));

  //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  //ostringstream rwostr1, rwostr2;
  //rwostr1<<aMedMedRdWrDriver22Cpy_1;
  //rwostr2<<aMedMedRdWrDriver22Cpy_2;
  //CPPUNIT_ASSERT(rwostr1.str() != "");
  //CPPUNIT_ASSERT(rwostr1.str() == rwostr2.str());

  //Delete all objects
  delete aMed;
  delete aInvalidMedMedRdDriver22;
  delete aInvalidMedMedRdDriver22_1;
  delete aMedMedRdDriver22;

  delete aInvalidMedMedWrDriver22;
  delete aInvalidMedMedWrDriver22_1;
  delete aMedMedWrDriver22;

  delete aInvalidMedMedRdWrDriver22;
  delete aInvalidMedMedRdWrDriver22_1;
  delete aMedMedRdWrDriver22;
}
