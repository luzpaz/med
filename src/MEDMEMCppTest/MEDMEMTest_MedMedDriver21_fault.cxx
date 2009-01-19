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

#include <MEDMEM_Compatibility21_22.hxx>
#include <MEDMEM_MedMedDriver21.hxx>
#include <MEDMEM_Med.hxx>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

/*!
 *  Check methods (21), defined in MEDMEM_MedMedDriver21.hxx:
 *  class MED_MED_DRIVER21 : public virtual MED_MED_DRIVER {
 *   (+) MED_MED_DRIVER21();
 *   (+) MED_MED_DRIVER21(const string & fileName,  MED * const ptrMed);
 *   (+) MED_MED_DRIVER21(const string & fileName,  MED * const ptrMed,
 *                            MED_EN::med_mode_acces accessMode);
 *   (+) MED_MED_DRIVER21(const MED_MED_DRIVER21 & driver);
 *   (+) virtual ~MED_MED_DRIVER21();
 *   (+) void open() throw (MEDEXCEPTION);
 *   (+) void close();
 *   (+) virtual void write          (void) const;
 *   (+) virtual void writeFrom      (void) const;
 *   (+) virtual void read           (void);
 *   (+) virtual void readFileStruct (void);
 *  }
 *  class MED_MED_RDONLY_DRIVER21 : public virtual IMED_MED_RDONLY_DRIVER,
 *                                  public virtual MED_MED_DRIVER21 {
 *   (+) MED_MED_RDONLY_DRIVER21();
 *   (+) MED_MED_RDONLY_DRIVER21(const string & fileName,  MED * const ptrMed);
 *   (+) MED_MED_RDONLY_DRIVER21(const MED_MED_RDONLY_DRIVER21 & driver);
 *   (+) virtual ~MED_MED_RDONLY_DRIVER21();
 *   (+) void write          (void) const throw (MEDEXCEPTION);
 *   (+) void writeFrom      (void) const throw (MEDEXCEPTION);
 *   (+) void read           (void) throw (MEDEXCEPTION);
 *   (+) void readFileStruct (void) throw (MEDEXCEPTION);
 *  }
 *  class MED_MED_WRONLY_DRIVER21 : public virtual IMED_MED_WRONLY_DRIVER,
 *                                  public virtual MED_MED_DRIVER21 {
 *   (+) MED_MED_WRONLY_DRIVER21();
 *   (+) MED_MED_WRONLY_DRIVER21(const string & fileName,  MED * const ptrMed);
 *   (+) MED_MED_WRONLY_DRIVER21(const MED_MED_WRONLY_DRIVER21 & driver);
 *   (+) virtual ~MED_MED_WRONLY_DRIVER21();
 *   (+) void write          (void) const throw (MEDEXCEPTION);
 *   (+) void writeFrom      (void) const throw (MEDEXCEPTION);
 *   (+) void read           (void) throw (MEDEXCEPTION);
 *   (+) void readFileStruct (void) throw (MEDEXCEPTION);
 *  }
 *  class MED_MED_RDWR_DRIVER21 : public MED_MED_RDONLY_DRIVER21,
 *                              public MED_MED_WRONLY_DRIVER21,
 *                              public IMED_MED_RDWR_DRIVER {
 *   (+) MED_MED_RDWR_DRIVER21();
 *   (+) MED_MED_RDWR_DRIVER21(const string & fileName,  MED * const ptrMed);
 *   (+) MED_MED_RDWR_DRIVER21(const MED_MED_RDWR_DRIVER21 & driver);
 *   (+) ~MED_MED_RDWR_DRIVER21();
 *   (+) void write          (void) const throw (MEDEXCEPTION);
 *   (+) void writeFrom      (void) const throw (MEDEXCEPTION);
 *   (+) void read           (void) throw (MEDEXCEPTION);
 *   (+) void readFileStruct (void) throw (MEDEXCEPTION);
 *  }
 */
void MEDMEMTest_testMedMedDriver21() {
  MED *aMed                        = new MED();
  string data_dir                  = getenv("MED_ROOT_DIR");
  string tmp_dir                   = getenv("TMP") ? getenv("TMP") : "/tmp";
  if(tmp_dir == "")
    tmp_dir = "/tmp";

  string filename_rd               = data_dir + "/share/salome/resources/med/pointe.med";
  string emptyfilename             = "";
  string filename_wr               = tmp_dir  + "/myMedWr_pointe21.med";
  string fileNotExistsName_rd      = "notExists.med";
  string fileNotExistsName_wr      = "/path_not_exists/file_not_exists.med";
  string filename_rdwr             = tmp_dir + "/myMedRdWr_pointe21.med";
  string fcopy                     = "cp " + filename_rd + " " + filename_rdwr;

  // To remove tmp files from disk
  MEDMEMTest_TmpFilesRemover aRemover;
  aRemover.Register(filename_wr);
  aRemover.Register(filename_rdwr);

  //Copy file in the TMP dir for testing READ/WRITE case
  system(fcopy.data());

  //-------------------------------Test READ_ONLY part-------------------------------------------------------
  //Creation incorrect Med_Med read only driver (file is not exist)
  MED_MED_RDONLY_DRIVER21 *aInvalidMedMedRdDriver21 = new MED_MED_RDONLY_DRIVER21(fileNotExistsName_rd, aMed);

  //Trying open not existing file
  CPPUNIT_ASSERT_THROW(aInvalidMedMedRdDriver21->open(),MEDEXCEPTION);

  //Creation incorrect Med_Med read only driver (with empty file name)
  MED_MED_RDONLY_DRIVER21 *aInvalidMedMedRdDriver21_1 = new MED_MED_RDONLY_DRIVER21(emptyfilename, aMed);

  //Trying open file with empty name
  CPPUNIT_ASSERT_THROW(aInvalidMedMedRdDriver21_1->open(),MEDEXCEPTION);


  //Creation a correct Med_Med read only driver (normal constructor)
  MED_MED_RDONLY_DRIVER21 *aMedMedRdDriver21 = new MED_MED_RDONLY_DRIVER21(filename_rd, aMed);

  //Check driver
  CPPUNIT_ASSERT(aMedMedRdDriver21);

  //Trying read mesh from file, if file is not open
//#ifdef  ENABLE_FORCED_FAILURES
  CPPUNIT_ASSERT_THROW(aMedMedRdDriver21->read(), MEDEXCEPTION);
  // (BUG) No exception in this case.
//#endif

  CPPUNIT_ASSERT_THROW(aMedMedRdDriver21->readFileStruct(),MEDEXCEPTION);

  //Test open() method
  try
  {
    aMedMedRdDriver21->open();
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
  CPPUNIT_ASSERT_THROW(aMedMedRdDriver21->open(),MEDEXCEPTION);
  //(BUG) No exception in this case.
//#endif

  //Test read() method
  try
  {
    aMedMedRdDriver21->read();
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
    aMedMedRdDriver21->readFileStruct();
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
  CPPUNIT_ASSERT_THROW(aMedMedRdDriver21->write(), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aMedMedRdDriver21->writeFrom(), MEDEXCEPTION);

  //Check MED
  CPPUNIT_ASSERT(aMed);

  //Test close() method
  try
  {
    aMedMedRdDriver21->close();
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
  MED_MED_RDONLY_DRIVER21 aMedMedRdDriver21Cpy_1;

  //Test (void operator =) defined in GENDRIVER class
  //aMedMedRdDriver21Cpy_1 = *aMedMedRdDriver21;

  //Test copy constructor
//#ifdef ENABLE_FAULTS
  MED_MED_RDONLY_DRIVER21 aMedMedRdDriver21Cpy_2 (*aMedMedRdDriver21);
  // (BUG) => Segmentation fault, because _concreteMedDrv is NULL
  // Process terminating with default action of signal 11 (SIGSEGV)
  //  Access not within mapped region at address 0x0
  //    at 0x34881A7A: MEDMEM::MED_MED_DRIVER::MED_MED_DRIVER(MEDMEM::MED_MED_DRIVER const&) (MEDMEM_MedMedDriver.cxx:56)
  //    by 0x34A07786: MEDMEM::MED_MED_RDONLY_DRIVER21::MED_MED_RDONLY_DRIVER21(MEDMEM::MED_MED_RDONLY_DRIVER21 const&) (MEDMEM_MedMedDriver21.cxx:179)
  //    by 0x343A02AD: MEDMEMTest::testMedMedDriver21() (MEDMEMTest_MedMedDriver21.cxx:219)
//#endif

  //CPPUNIT_ASSERT_EQUAL(aMedMedRdDriver21Cpy_2, *aMedMedRdDriver21);

  //Test (bool operator ==) defined in GENDRIVER class in MEDMEM_GenDriver.hxx
  //CPPUNIT_ASSERT(aMedMedRdDriver21Cpy_2.GENDRIVER::operator==(*aMedMedRdDriver21));

  //Test (friend ostream & operator <<) defined GENDRIVER class
  //ostringstream rwostr1, rwostr2;
  //rwostr1<<aMedMedRdDriver21Cpy_1;
  //rwostr2<<aMedMedRdDriver21Cpy_2;
  //CPPUNIT_ASSERT(rwostr1.str() != "");
  //CPPUNIT_ASSERT(rwostr1.str() == rwostr2.str());

  //-------------------------------Test WRITE_ONLY part-------------------------------------------------------
  //Creation incorrect Med_Med write only driver (file is not exist)
  MED_MED_WRONLY_DRIVER21 *aInvalidMedMedWrDriver21 = new MED_MED_WRONLY_DRIVER21(fileNotExistsName_wr, aMed);

  //Trying open not existing file
  CPPUNIT_ASSERT_THROW(aInvalidMedMedWrDriver21->open(),MEDEXCEPTION);

  //Creation incorrect Med_Med write only driver (with empty file name)
  MED_MED_WRONLY_DRIVER21 *aInvalidMedMedWrDriver21_1 = new MED_MED_WRONLY_DRIVER21(emptyfilename, aMed);

  //Trying open file with empty name
  CPPUNIT_ASSERT_THROW(aInvalidMedMedWrDriver21_1->open(),MEDEXCEPTION);


  //Creation a correct Med_Med write only driver (normal constructor)
  MED_MED_WRONLY_DRIVER21 *aMedMedWrDriver21 = new MED_MED_WRONLY_DRIVER21(filename_wr, aMed);

  //Check driver
  CPPUNIT_ASSERT(aMedMedWrDriver21);


  //Trying write mesh to file, if file is not open
//#ifdef ENABLE_FORCED_FAILURES
  CPPUNIT_ASSERT_THROW(aMedMedWrDriver21->writeFrom(),MEDEXCEPTION);
  // (BUG) No exception in this case
//#endif

//#ifdef  ENABLE_FAULTS
  CPPUNIT_ASSERT_THROW(aMedMedWrDriver21->write(), MEDEXCEPTION);
  //(BUG) => Segmentation fault
//#endif

  //Test open() method
  try
  {
    aMedMedWrDriver21->open();
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
  CPPUNIT_ASSERT_THROW(aMedMedWrDriver21->open(),MEDEXCEPTION);
  //(BUG) No exception in this case.
//#endif

  //Test writeFrom() method
  try
  {
    aMedMedWrDriver21->writeFrom();
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
    aMedMedWrDriver21->write();
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
    aMedMedWrDriver21->close();
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
  MED_MED_WRONLY_DRIVER21 aMedMedWrDriver21Cpy_1;

  //Test (void operator =) defined in GENDRIVER class
  //aMedMedWrDriver21Cpy_1 = *aMedMedWrDriver21;

  //Test copy constructor
//#ifdef ENABLE_FAULTS
  MED_MED_WRONLY_DRIVER21 aMedMedWrDriver21Cpy_2 (*aMedMedWrDriver21);
  // (BUG) => Segmentation fault
//#endif
  //CPPUNIT_ASSERT_EQUAL(aMedMedWrDriver21Cpy_2, *aMedMedWrDriver21);

  //Test (bool operator ==) defined in GENDRIVER class
  //CPPUNIT_ASSERT(aMedMedWrDriver21Cpy_2.GENDRIVER::operator==(*aMedMedWrDriver21));

  //Test (friend ostream & operator <<) defined GENDRIVER class
  //ostringstream rwostr1, rwostr2;
  //rwostr1<<aMedMedWrDriver21Cpy_1;
  //rwostr2<<aMedMedWrDriver21Cpy_2;
  //CPPUNIT_ASSERT(rwostr1.str() != "");
  //CPPUNIT_ASSERT(rwostr1.str() == rwostr2.str());


//-------------------------------Test READ/WRITE part-------------------------------------------------------
  //Creation incorrect Med_Med read/write driver (file is not exist)
  MED_MED_RDWR_DRIVER21 *aInvalidMedMedRdWrDriver21 = new MED_MED_RDWR_DRIVER21(fileNotExistsName_wr, aMed);

  //Trying open not existing file
  CPPUNIT_ASSERT_THROW(aInvalidMedMedRdWrDriver21->open(),MEDEXCEPTION);

  //Creation incorrect Med_Med read/write driver (with empty file name)
  MED_MED_RDWR_DRIVER21 *aInvalidMedMedRdWrDriver21_1 = new MED_MED_RDWR_DRIVER21(emptyfilename, aMed);

  //Trying open file with empty name
  CPPUNIT_ASSERT_THROW(aInvalidMedMedRdWrDriver21_1->open(),MEDEXCEPTION);


  //Creation a correct Med_Med read/write driver (normal constructor)
  MED_MED_RDWR_DRIVER21 *aMedMedRdWrDriver21 = new MED_MED_RDWR_DRIVER21(filename_rdwr, aMed);

  //Check driver
  CPPUNIT_ASSERT(aMedMedRdWrDriver21);

  //Trying read MED from file if it is not open
//#ifdef  ENABLE_FORCED_FAILURES
  CPPUNIT_ASSERT_THROW(aMedMedRdDriver21->read(), MEDEXCEPTION);
  // (BUG) No exception in this case.
//#endif

  CPPUNIT_ASSERT_THROW(aMedMedRdDriver21->readFileStruct(),MEDEXCEPTION);

  //Trying write mesh to file, if file is not open
//#ifdef ENABLE_FORCED_FAILURES
  CPPUNIT_ASSERT_THROW(aMedMedRdWrDriver21->writeFrom(),MEDEXCEPTION);
  // (BUG) => No exception in this case
//#endif

//#ifdef ENABLE_FAULTS
  CPPUNIT_ASSERT_THROW(aMedMedRdWrDriver21->write(), MEDEXCEPTION);
  //(BUG) => Crash
//#endif

  //Test open() method
  try
  {
    aMedMedRdWrDriver21->open();
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
  CPPUNIT_ASSERT_THROW(aMedMedRdWrDriver21->open(),MEDEXCEPTION);
  //(BUG) No exception in this case.
//#endif

  //Test readFileStruct() method
    try
  {
    aMedMedRdWrDriver21->readFileStruct();
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
    aMedMedRdWrDriver21->read();
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
    aMedMedRdWrDriver21->writeFrom();
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
    aMedMedRdWrDriver21->write();
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
    aMedMedRdWrDriver21->close();
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
  MED_MED_RDWR_DRIVER21 aMedMedRdWrDriver21Cpy_1;

  //Test copy constructor
//#ifdef ENABLE_FAULTS
  MED_MED_RDWR_DRIVER21 aMedMedRdWrDriver21Cpy_2 (*aMedMedRdWrDriver21);
  // (BUG) => Segmentation fault
//#endif
  //CPPUNIT_ASSERT_EQUAL(aMedMedRdWrDriver21Cpy_2, *aMedMedRdWrDriver21);

  //Test (bool operator ==) defined in GENDRIVER class
  //CPPUNIT_ASSERT(aMedMedRdWrDriver21Cpy_2.GENDRIVER::operator==(*aMedMedRdWrDriver21));

  //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  //ostringstream rwostr1, rwostr2;
  //rwostr1<<aMedMedRdWrDriver21Cpy_1;
  //rwostr2<<aMedMedRdWrDriver21Cpy_2;
  //CPPUNIT_ASSERT(rwostr1.str() != "");
  //CPPUNIT_ASSERT(rwostr1.str() == rwostr2.str());

  //Delete all objects
  delete aMed;
  delete aInvalidMedMedRdDriver21;
  delete aInvalidMedMedRdDriver21_1;
  delete aMedMedRdDriver21;

  delete aInvalidMedMedWrDriver21;
  delete aInvalidMedMedWrDriver21_1;
  delete aMedMedWrDriver21;

  delete aInvalidMedMedRdWrDriver21;
  delete aInvalidMedMedRdWrDriver21_1;
  delete aMedMedRdWrDriver21;
}

int main (int argc, char** argv)
{
  MEDMEMTest_testMedMedDriver21();
}
