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

#include <MEDMEM_Field.hxx>
#include <MEDMEM_MedFieldDriver22.hxx>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

/*!
 *  Check methods (17), defined in MEDMEM_MedFieldDriver22.hxx:
 *  template <class T> class MED_FIELD_DRIVER22 : public virtual MED_FIELD_DRIVER<T> {
 *   (+) MED_FIELD_DRIVER22();
 *   (+) template <class INTERLACING_TAG> MED_FIELD_DRIVER22(const string & fileName,
 *          FIELD<T, INTERLACING_TAG> * ptrField, MED_EN::med_mode_acces accessMode);
 *   (+) MED_FIELD_DRIVER22(const MED_FIELD_DRIVER22 & fieldDriver);
 *   (+) virtual ~MED_FIELD_DRIVER22();
 *   (+) void open() throw (MEDEXCEPTION);
 *   (+) void close();
 *  }
 *  template <class T> class MED_FIELD_RDONLY_DRIVER22 : public virtual MED_FIELD_DRIVER22<T>,
 *                                                       public virtual IMED_FIELD_RDONLY_DRIVER<T> {
 *   (+) MED_FIELD_RDONLY_DRIVER22();
 *   (+) template <class INTERLACING_TAG> MED_FIELD_RDONLY_DRIVER22
 *                 (const string & fileName, FIELD<T, INTERLACING_TAG> * ptrField);
 *   (+) MED_FIELD_RDONLY_DRIVER22(const MED_FIELD_RDONLY_DRIVER22 & fieldDriver);
 *   (+) virtual ~MED_FIELD_RDONLY_DRIVER22();
 *   (+) void write(void) const throw (MEDEXCEPTION);
 *   (+) void read (void) throw (MEDEXCEPTION);
 *  }
 *  template <class T> class MED_FIELD_WRONLY_DRIVER22 : public virtual MED_FIELD_DRIVER22<T>,
 *                                                       public virtual IMED_FIELD_WRONLY_DRIVER<T> {
 *   (+) MED_FIELD_WRONLY_DRIVER22();
 *   (+) template <class INTERLACING_TAG> MED_FIELD_WRONLY_DRIVER22
 *              (const string & fileName, FIELD<T, INTERLACING_TAG> * ptrField);
 *   (+) MED_FIELD_WRONLY_DRIVER22(const MED_FIELD_WRONLY_DRIVER22 & fieldDriver);
 *   (+) virtual ~MED_FIELD_WRONLY_DRIVER22();
 *   (+) void write(void) const throw (MEDEXCEPTION);
 *   (+) void read (void) throw (MEDEXCEPTION);
 *  }
 *  template <class T> class MED_FIELD_RDWR_DRIVER22 : public MED_FIELD_RDONLY_DRIVER22<T>,
 *                           public MED_FIELD_WRONLY_DRIVER22<T>, public IMED_FIELD_RDWR_DRIVER<T> {
 *   (+) MED_FIELD_RDWR_DRIVER22();
 *   (+) template <class INTERLACING_TAG> MED_FIELD_RDWR_DRIVER22
 *              (const string & fileName, FIELD<T, INTERLACING_TAG> * ptrField);
 *   (+) MED_FIELD_RDWR_DRIVER22(const MED_FIELD_RDWR_DRIVER22 & fieldDriver);
 *   (+) ~MED_FIELD_RDWR_DRIVER22();
 *   (+) void write(void) const throw (MEDEXCEPTION);
 *   (+) void read (void) throw (MEDEXCEPTION);
 *  }
 */
void MEDMEMTest::testMedFieldDriver22()
{
  FIELD<int> *aField                = new FIELD<int> ();
  FIELD<int> *aField_1              = new FIELD<int> ();
  FIELD<double> *aField_2           = new FIELD<double> ();
  string data_dir                   = getenv("DATA_DIR");
  string tmp_dir                    = getenv("TMP");
  if (tmp_dir == "")
    tmp_dir = "/tmp";
  string filename_rd                = data_dir + "/MedFiles/pointe_import22.med";
  string filenameWithOutFileds      = data_dir + "/MedFiles/mesh_import22.med";
  string fileldnotexist             = "anyfield";
  string fieldname                  = "fieldnodeint";
  string fieldname_cpy              = fieldname + "_cpy";
  string fileNotExist_rd            = "notExist22.med";
  string emptyfilename              = "";

  string filename_wr                = tmp_dir + "/myWrField_pointe22.med";
  string cp_file_wr                 = "cp " + filename_rd + " " + filename_wr;
  string fileNotExist_wr            = "/path_not_exist/file_not_exist.med";
  string emptyfile_wr               = tmp_dir + "/myWrField_pointe_empty22.med";
  string other_file                 = data_dir + "/MedFiles/cube_hexa8_import22.med";
  string other_file_wr              = tmp_dir + "/myWRcube_hexa8_import22.med";
  string cp_other_file              = "cp " + other_file + " " + other_file_wr;

  string filename_rdwr              = tmp_dir + "/myRdWrField_pointe22.med";
  string cp_file_rdwr               = "cp " + filename_rd + " " + filename_rdwr;
  string emptyfile_rdwr             = tmp_dir + "/myRdWrField_pointe_empty22.med";
  string fieldnameDouble            = "fieldnodedouble";
  string fieldnameDouble_cpy        = fieldnameDouble + "_cpy";

  // To remove tmp files from disk
  MEDMEMTest_TmpFilesRemover aRemover;
  aRemover.Register(filename_wr);
  aRemover.Register(emptyfile_wr);
  aRemover.Register(other_file_wr);
  aRemover.Register(filename_rdwr);
  aRemover.Register(emptyfile_rdwr);

  //--------------------------Test READ ONLY part------------------------------//

  /////////////////////////////////////
  //  TEST1: Open not existing file  //
  /////////////////////////////////////

  //Creation Invalid Read Only MedFieldDriver22 (file not exist)
  MED_FIELD_RDONLY_DRIVER22<int> *aInvalidMedRdFieldDriver22_1 =
    new MED_FIELD_RDONLY_DRIVER22<int>(fileNotExist_rd, aField);
  //Trying open not existing file
  CPPUNIT_ASSERT_THROW(aInvalidMedRdFieldDriver22_1->open(), MEDEXCEPTION);

  //////////////////////////////////////////////
  //  TEST2: Open file with empty file name  ///
  //////////////////////////////////////////////

  //Creation Invalid Read Only MedFieldDriver22 (empty file name)
  MED_FIELD_RDONLY_DRIVER22<int> *aInvalidMedRdFieldDriver22_2 =
    new MED_FIELD_RDONLY_DRIVER22<int>(emptyfilename, aField);
  //Trying file with empty name
  CPPUNIT_ASSERT_THROW(aInvalidMedRdFieldDriver22_2->open(), MEDEXCEPTION);

  /////////////////////////////////////////////////////
  //  TEST3: Reading field from file without fields  //
  /////////////////////////////////////////////////////
  MED_FIELD_RDONLY_DRIVER22<int> *aInvalidMedRdFieldDriver22_3 =
    new MED_FIELD_RDONLY_DRIVER22<int>(filenameWithOutFileds, aField);
  aInvalidMedRdFieldDriver22_3->open();
  aInvalidMedRdFieldDriver22_3->setFieldName(fieldname);
  //Trying read field from file
  CPPUNIT_ASSERT_THROW(aInvalidMedRdFieldDriver22_3->read(), MEDEXCEPTION);
  aInvalidMedRdFieldDriver22_3->close();

  ////////////////////////
  //  TEST4: Main test  //
  ////////////////////////
  //Creation correct Read Only MedFieldDriver22
  MED_FIELD_RDONLY_DRIVER22<int> *aMedRdFieldDriver22 =
    new MED_FIELD_RDONLY_DRIVER22<int>(filename_rd, aField);

  //Check driver
  CPPUNIT_ASSERT(aMedRdFieldDriver22);

  //Trying read field before open file
  CPPUNIT_ASSERT_THROW(aMedRdFieldDriver22->read(),MEDEXCEPTION);

  //Test open() method
  try
  {
    aMedRdFieldDriver22->open();
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
  //CPPUNIT_ASSERT_THROW(aMedRdFieldDriver22->open(), MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW(aMedRdFieldDriver22->open());
  // (BUG) No exception in this case
  //#endif

  //Trying read field form file if it name is empty
  CPPUNIT_ASSERT_THROW(aMedRdFieldDriver22->read(), MEDEXCEPTION);

  //Test setFieldName() and getFieldName()
  try
  {
    aMedRdFieldDriver22->setFieldName(fileldnotexist);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  CPPUNIT_ASSERT_EQUAL(fileldnotexist, aMedRdFieldDriver22->getFieldName());

  //Trying read not existing field from file
  CPPUNIT_ASSERT_THROW(aMedRdFieldDriver22->read(), MEDEXCEPTION);

  //Test read() method
  aMedRdFieldDriver22->setFieldName(fieldname);
  try
  {
    aMedRdFieldDriver22->read();
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
  CPPUNIT_ASSERT_THROW(aMedRdFieldDriver22->write(), MEDEXCEPTION);

  //Test close() method
  try
  {
    aMedRdFieldDriver22->close();
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
  MED_FIELD_RDONLY_DRIVER22<int> aMedRdFieldDriver22Cpy_1;

  //Test (void operator =) defined in GENDRIVER class in MEDMEM_GenDriver.hxx
  aMedRdFieldDriver22Cpy_1 = *aMedRdFieldDriver22;

  //Test (bool operator ==) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  CPPUNIT_ASSERT(aMedRdFieldDriver22Cpy_1 ==  *aMedRdFieldDriver22);

  //Test copy constructorg
  MED_FIELD_RDONLY_DRIVER22<int> aMedRdFieldDriver22Cpy_2 (*aMedRdFieldDriver22);
  CPPUNIT_ASSERT_EQUAL(aMedRdFieldDriver22Cpy_2, *aMedRdFieldDriver22);

  //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  ostringstream rostr1, rostr2;
  rostr1<<aMedRdFieldDriver22Cpy_1;
  rostr2<<aMedRdFieldDriver22Cpy_2;
  CPPUNIT_ASSERT(rostr1.str() != "");
  CPPUNIT_ASSERT(rostr1.str() == rostr2.str());


  //--------------------------Test WRITE ONLY part------------------------------//

  //Copy file
  system(cp_other_file.c_str());
  system(cp_file_wr.c_str());

  /////////////////////////////////////
  //  TEST1: Open not existing file  //
  /////////////////////////////////////

  //Creation Invalid Write Only MedFieldDriver22 (file not exist)
  MED_FIELD_WRONLY_DRIVER22<int> *aInvalidMedWrFieldDriver22_1 =
    new MED_FIELD_WRONLY_DRIVER22<int>(fileNotExist_wr, aField_1);
  //Trying open not existing file
  CPPUNIT_ASSERT_THROW(aInvalidMedWrFieldDriver22_1->open(), MEDEXCEPTION);

  /////////////////////////////////////////////
  //  TEST2: Open file with empty file name  //
  /////////////////////////////////////////////
  //Creation Invalid Write Only MedFieldDriver22 (empty file name)
  MED_FIELD_WRONLY_DRIVER22<int> *aInvalidMedWrFieldDriver22_2 =
    new MED_FIELD_WRONLY_DRIVER22<int>(emptyfilename, aField_1);
  //Trying open not existing file and file with empty name
  CPPUNIT_ASSERT_THROW(aInvalidMedWrFieldDriver22_2->open(), MEDEXCEPTION);

  ///////////////////////////////////////////////////////
  //  TEST3: Writing field in empty file without mesh  //
  ///////////////////////////////////////////////////////
  //Creation Invalid Write Only MedFieldDriver22
   MED_FIELD_WRONLY_DRIVER22<int> *aInvalidMedWrFieldDriver22_3 =
     new MED_FIELD_WRONLY_DRIVER22<int>(emptyfile_wr, aField);

  aInvalidMedWrFieldDriver22_3->open();
  //#ifdef ENABLE_FORCED_FAILURES
  //CPPUNIT_ASSERT_THROW(aInvalidMedWrFieldDriver22_3->write(), MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW(aInvalidMedWrFieldDriver22_3->write());
  //(Bug) No Exception in this case
  aInvalidMedWrFieldDriver22_3->close();
  //#endif

  //////////////////////////////////////////////
  //  TEST4: Writing field in the other file  //
  //////////////////////////////////////////////

  //Creation Invalid Write Only MedFieldDriver22
  MED_FIELD_WRONLY_DRIVER22<int> *aInvalidMedWrFieldDriver22_4 =
    new MED_FIELD_WRONLY_DRIVER22<int>(other_file_wr, aField);
  aInvalidMedWrFieldDriver22_4->open();
  //#ifdef ENABLE_FORCED_FAILURES
  //CPPUNIT_ASSERT_THROW(aInvalidMedWrFieldDriver22_4->write(), MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW(aInvalidMedWrFieldDriver22_4->write());
  //(Bug) No Exception in this case
  //#endif

  ////////////////////////
  //  TEST5: Main test  //
  ////////////////////////

  //Creation correct Write Only MedFieldDriver22
  MED_FIELD_WRONLY_DRIVER22<int> *aMedWrFieldDriver22 =
    new MED_FIELD_WRONLY_DRIVER22<int>(filename_wr, aField);

  //Check driver
  CPPUNIT_ASSERT(aMedWrFieldDriver22);

  //Trying write field before open file
  //#ifdef ENABLE_FORCED_FAILURES
  CPPUNIT_ASSERT_THROW(aMedWrFieldDriver22->write(),MEDEXCEPTION);
  // (BUG) No exception in this case
  //#endif

  //Test open() method
  try
  {
    aMedWrFieldDriver22->open();
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
  //CPPUNIT_ASSERT_THROW(aMedWrFieldDriver22->open(), MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW(aMedWrFieldDriver22->open());
  // (BUG) No exception in this case
  //#endif

  //Test setFieldName() and getFieldName
  aField->setName(fieldname_cpy);
  try
  {
    aMedWrFieldDriver22->setFieldName(fieldname_cpy);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  CPPUNIT_ASSERT_EQUAL(fieldname_cpy, aMedWrFieldDriver22->getFieldName());

  //Test write() method
  try
  {
    aMedWrFieldDriver22->write();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Test read() method for Write only part
  CPPUNIT_ASSERT_THROW(aMedWrFieldDriver22->read(),MEDEXCEPTION);

  //Test close() method
  try
  {
    aMedWrFieldDriver22->close();
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
  MED_FIELD_WRONLY_DRIVER22<int> aMedWrFieldDriver22Cpy_1;

  //Test (void operator =) defined in GENDRIVER class
  aMedWrFieldDriver22Cpy_1 = *aMedWrFieldDriver22;

  //Test (bool operator ==) defined in GENDRIVER class
  CPPUNIT_ASSERT(aMedWrFieldDriver22Cpy_1.GENDRIVER::operator==(*aMedWrFieldDriver22));

  //Test copy constructor
  MED_FIELD_WRONLY_DRIVER22<int> aMedWrFieldDriver22Cpy_2 (*aMedWrFieldDriver22);
  CPPUNIT_ASSERT_EQUAL(aMedWrFieldDriver22Cpy_2, *aMedWrFieldDriver22);

  //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  ostringstream wostr1, wostr2;
  wostr1 << aMedWrFieldDriver22Cpy_1;
  wostr2 << aMedWrFieldDriver22Cpy_2;
  CPPUNIT_ASSERT(wostr1.str() != "");
  CPPUNIT_ASSERT(wostr1.str() == wostr2.str());


  //--------------------------Test READ/WRITE part------------------------------//

  //Copy files
  system(cp_file_rdwr.c_str());
  /////////////////////////////////////
  //  TEST1: Open not existing file  //
  /////////////////////////////////////

  //Creation Invalid Read/Write MedFieldDriver22 (file not exist)
  MED_FIELD_RDWR_DRIVER22<int> *aInvalidMedRdWrFieldDriver22_1 =
    new MED_FIELD_RDWR_DRIVER22<int>(fileNotExist_wr, aField_1);
  //Trying open not existing file
  CPPUNIT_ASSERT_THROW(aInvalidMedRdWrFieldDriver22_1->open(), MEDEXCEPTION);

  /////////////////////////////////////////////
  //  TEST2: Open file with empty file name  //
  /////////////////////////////////////////////
  //Creation Invalid Read/Write MedFieldDriver22 (empty file name)
  MED_FIELD_RDWR_DRIVER22<int> *aInvalidMedRdWrFieldDriver22_2 =
    new MED_FIELD_RDWR_DRIVER22<int>(emptyfilename, aField_1);
  //Trying open not existing file and file with empty name
  CPPUNIT_ASSERT_THROW(aInvalidMedRdWrFieldDriver22_2->open(), MEDEXCEPTION);

  /////////////////////////////////////////////////////
  //  TEST3: Reading field from file without fields  //
  /////////////////////////////////////////////////////
  MED_FIELD_RDWR_DRIVER22<int> *aInvalidMedRdWrFieldDriver22_3 =
    new MED_FIELD_RDWR_DRIVER22<int>(filenameWithOutFileds, aField_1);
  aInvalidMedRdWrFieldDriver22_3->open();
  aInvalidMedRdWrFieldDriver22_3->setFieldName(fieldname);
  //Trying read field from file
  CPPUNIT_ASSERT_THROW(aInvalidMedRdWrFieldDriver22_3->read(), MEDEXCEPTION);
  aInvalidMedRdWrFieldDriver22_3->close();

  ///////////////////////////////////////////////////////
  //  TEST4: Writing field in empty file without mesh  //
  ///////////////////////////////////////////////////////

  //Creation Invalid Read/Write MedFieldDriver22
   MED_FIELD_RDWR_DRIVER22<int> *aInvalidMedRdWrFieldDriver22_4 =
     new MED_FIELD_RDWR_DRIVER22<int>(emptyfile_rdwr, aField);

  aInvalidMedRdWrFieldDriver22_4->open();
  //#ifdef ENABLE_FORCED_FAILURES
  //  CPPUNIT_ASSERT_THROW(aInvalidMedRdWrFieldDriver22_4->write(), MEDEXCEPTION);
  // (BUG) No exception in this case
  //#endif

  //////////////////////////////////////////////
  //  TEST6: Writing field in the other file  //
  //////////////////////////////////////////////

  //Creation Invalid Read/Write MedFieldDriver22
  MED_FIELD_RDWR_DRIVER22<int> *aInvalidMedRdWrFieldDriver22_5 =
    new MED_FIELD_RDWR_DRIVER22<int>(other_file_wr, aField);
  aInvalidMedRdWrFieldDriver22_5->open();
  //#ifdef ENABLE_FORCED_FAILURES
  //CPPUNIT_ASSERT_THROW(aInvalidMedRdWrFieldDriver22_5->write(), MEDEXCEPTION);
  // (BUG) No exception in this case
  //#endif

  ////////////////////////
  //  TEST7: Main test  //
  ////////////////////////

  //Creation correct Read/Write MedFieldDriver22
  MED_FIELD_RDWR_DRIVER22<double> *aMedRdWrFieldDriver22 =
    new MED_FIELD_RDWR_DRIVER22<double>(filename_rdwr, aField_2);

  //Check driver
  CPPUNIT_ASSERT(aMedRdWrFieldDriver22);

  //Trying read/write field before open file
  CPPUNIT_ASSERT_THROW(aMedRdWrFieldDriver22->read(),MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aMedRdWrFieldDriver22->write(),MEDEXCEPTION);

  //Test open() method
  try
  {
    aMedRdWrFieldDriver22->open();
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
  //CPPUNIT_ASSERT_THROW(aMedRdWrFieldDriver22->open(), MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW(aMedRdWrFieldDriver22->open());
  // (BUG) No exception in this case
  //#endif

  //Trying read field from file if field name is empty
  CPPUNIT_ASSERT_THROW(aMedRdFieldDriver22->read(), MEDEXCEPTION);

  //Test setFieldName() and getFieldName
  try
  {
    aMedRdWrFieldDriver22->setFieldName(fileldnotexist);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  CPPUNIT_ASSERT_EQUAL(fileldnotexist, aMedRdWrFieldDriver22->getFieldName());

  //Trying read not existing field from file
  CPPUNIT_ASSERT_THROW(aMedRdWrFieldDriver22->read(), MEDEXCEPTION);

  //Test read() method
  aMedRdWrFieldDriver22->setFieldName(fieldnameDouble);
  try
  {
    aMedRdWrFieldDriver22->read();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Trying write field in the file with empty name
  aField_2->setName(emptyfilename);
  aMedRdWrFieldDriver22->setFieldName(emptyfilename);
  CPPUNIT_ASSERT_THROW(aMedRdWrFieldDriver22->write(), MEDEXCEPTION);

  //Test write() method
  aField_2->setName(fieldnameDouble_cpy);
  aMedRdWrFieldDriver22->setFieldName(fieldnameDouble_cpy);
  try
  {
    aMedRdWrFieldDriver22->write();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Test close() method
  try
  {
    aMedRdWrFieldDriver22->close();
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
  MED_FIELD_RDWR_DRIVER22<double> aMedRdWrFieldDriver22Cpy_1;

  //Test (void operator =) defined in GENDRIVER class
  aMedRdWrFieldDriver22Cpy_1 = *aMedRdWrFieldDriver22;

  //Test (bool operator ==) defined in GENDRIVER class
  CPPUNIT_ASSERT(aMedRdWrFieldDriver22Cpy_1.GENDRIVER::operator==(*aMedRdWrFieldDriver22));

  //Test copy constructor
  MED_FIELD_RDWR_DRIVER22<double> aMedRdWrFieldDriver22Cpy_2 (*aMedRdWrFieldDriver22);
  CPPUNIT_ASSERT_EQUAL(aMedRdWrFieldDriver22Cpy_2, *aMedRdWrFieldDriver22);

  //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  ostringstream rwostr1, rwostr2;
  rwostr1 << aMedRdWrFieldDriver22Cpy_1;
  rwostr2 << aMedRdWrFieldDriver22Cpy_2;
  CPPUNIT_ASSERT(rwostr1.str() != "");
  CPPUNIT_ASSERT(rwostr1.str() == rwostr2.str());

  //Delete all objects
  delete aField;
  delete aField_1;
  delete aField_2;

  delete aInvalidMedRdFieldDriver22_1;
  delete aInvalidMedRdFieldDriver22_2;
  delete aInvalidMedRdFieldDriver22_3;
  delete aMedRdFieldDriver22;

  delete aInvalidMedWrFieldDriver22_1;
  delete aInvalidMedWrFieldDriver22_2;
  delete aInvalidMedWrFieldDriver22_3;
  delete aInvalidMedWrFieldDriver22_4;
  delete aMedWrFieldDriver22;

  delete aInvalidMedRdWrFieldDriver22_1;
  delete aInvalidMedRdWrFieldDriver22_2;
  delete aInvalidMedRdWrFieldDriver22_3;
  delete aInvalidMedRdWrFieldDriver22_4;
  delete aInvalidMedRdWrFieldDriver22_5;
  delete aMedRdWrFieldDriver22;
}
