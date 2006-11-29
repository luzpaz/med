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
//#define ENABLE_FORCED_FAILURES

#ifdef ENABLE_FAULTS
  // (BUG)
#endif

#ifdef ENABLE_FORCED_FAILURES
  //CPPUNIT_FAIL("");
#endif

#include "MEDMEMTest.hxx"
#include <cppunit/TestAssert.h>

#include <MEDMEM_VtkFieldDriver.hxx>
#include <MEDMEM_Med.hxx>
#include <MEDMEM_Field.hxx>


using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

/*!
 *  Check methods (14), defined in MEDMEM_VtkFieldDriver.hxx:
 *  template <class T> class VTK_FIELD_DRIVER : public GENDRIVER {
 *   (yetno) template <class INTERLACING_TAG> VTK_FIELD_DRIVER();
 *   (+) template <class INTERLACING_TAG> VTK_FIELD_DRIVER
 *               (const string & fileName, FIELD<T, INTERLACING_TAG> * ptrField)
 *   (+) VTK_FIELD_DRIVER(const VTK_FIELD_DRIVER & fieldDriver)
 *   (+) ~VTK_FIELD_DRIVER()
 *   (+) void openConst() const throw (MEDEXCEPTION)
 *   (+) void openConstAppend() const throw (MEDEXCEPTION)
 *   (+) void open() throw (MEDEXCEPTION)
 *   (+) void openAppend() throw (MEDEXCEPTION)
 *   (+) void closeConst() const throw (MEDEXCEPTION)
 *   (+) void close()
 *   (+) void setFieldName(const string & fieldName);
 *   (+) string getFieldName() const;
 *   (+) void read (void) throw (MEDEXCEPTION);
 *   (+) void write(void) const throw (MEDEXCEPTION);
 *   (+) void writeAppend(void) const throw (MEDEXCEPTION);
 *  }
 */
void MEDMEMTest::testVtkFieldDriver()
{
  FIELD<int> *aField                = new FIELD<int> ();
  FIELD<double> *aField_1           = new FIELD<double> ();
  string data_dir                   = getenv("DATA_DIR");
  string tmp_dir                    = getenv("TMP");
  if(tmp_dir == "")
    tmp_dir ="/tmp";
  string filename_rd                = data_dir + "/MedFiles/pointe.med";
  string emptyfilename              = "";
  string fileNotExistsName          = "/path_not_exists/file_not_exists.vtk";
  string filename_wr                =  tmp_dir  + "/myField_pointe.vtk";
  string fieldname_rd_int           = "fieldnodeint";
  string fieldname_wr_int           = "myintfield";
  string fieldname_rd_double        = "fieldnodedouble";
  string fieldname_wr_double        = "mydoublefield";

  // To remove tmp files from disk
  MEDMEMTest_TmpFilesRemover aRemover;
  aRemover.Register(filename_wr);
  
  /////////////////////////////////////
  //  TEST1: Open not existing file  //
  /////////////////////////////////////
  //Creation Invalid VtkFieldDriver (file not exist)
  VTK_FIELD_DRIVER<int> *aInvalidVtkFieldDriver_1 = new VTK_FIELD_DRIVER<int>(fileNotExistsName, aField);
  //Trying open not existing file
  CPPUNIT_ASSERT_THROW(aInvalidVtkFieldDriver_1->open(), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aInvalidVtkFieldDriver_1->openConst(), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aInvalidVtkFieldDriver_1->openConstAppend(), MEDEXCEPTION);

  /////////////////////////////////////////////
  //  TEST2: Open file with empty file name  //
  /////////////////////////////////////////////
  //Creation Invalid VtkFieldDriver (empty file name)
  VTK_FIELD_DRIVER<int> *aInvalidVtkFieldDriver_2 = new VTK_FIELD_DRIVER<int>(emptyfilename, aField);

  //Trying file with empty name
  CPPUNIT_ASSERT_THROW(aInvalidVtkFieldDriver_2->open(), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aInvalidVtkFieldDriver_2->openConst(), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aInvalidVtkFieldDriver_2->openConstAppend(), MEDEXCEPTION);

  ////////////////////////
  //  TEST2: Main test  //
  ////////////////////////
  //Read Fields from file
  MED_FIELD_RDONLY_DRIVER21<int> *aMedRdFieldDriver21_int = new MED_FIELD_RDONLY_DRIVER21<int>(filename_rd, aField);
  aMedRdFieldDriver21_int->open();
  aMedRdFieldDriver21_int->setFieldName(fieldname_rd_int);
  aMedRdFieldDriver21_int->read();
  aMedRdFieldDriver21_int->close();
  
  MED_FIELD_RDONLY_DRIVER21<double> *aMedRdFieldDriver21_double = new MED_FIELD_RDONLY_DRIVER21<double>(filename_rd, aField_1);
  aMedRdFieldDriver21_double->open();
  aMedRdFieldDriver21_double->setFieldName(fieldname_rd_double);
  aMedRdFieldDriver21_double->read();
  aMedRdFieldDriver21_double->close();
  //Check fields
  CPPUNIT_ASSERT(aField);
  
  //Creation correct VtkFieldDriver 
  VTK_FIELD_DRIVER<int> *aVtkFieldDriver_int = new VTK_FIELD_DRIVER<int>(filename_wr, aField);
  
  //Check driver
  CPPUNIT_ASSERT(aVtkFieldDriver_int);
  
  //Test setFieldName() and getFieldName() methods
  try
  {
    aVtkFieldDriver_int->setFieldName(fieldname_wr_int);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  CPPUNIT_ASSERT_EQUAL(fieldname_wr_int, aVtkFieldDriver_int->getFieldName());
  
  //Test open() method 
  try 
  { 
    aVtkFieldDriver_int->open();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

#ifdef ENABLE_FAULTS  
  //Test write() method 
  try
  {
    aVtkFieldDriver_int->write();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
    // => Segmentation fault 
#endif
  
  //Test read() method for Vtk Field Driver 
  CPPUNIT_ASSERT_THROW(aVtkFieldDriver_int->read(),MEDEXCEPTION);
  
  //Test close() method
  try
  {
    aVtkFieldDriver_int->close();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Test openAppend() method 
  try
  {
    aVtkFieldDriver_int->close();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  
  /////////////////////////////////////////////////////////
  //  TEST? Test openAppend() and writeAppend() methods  //
  /////////////////////////////////////////////////////////

  //Creation correct VtkFieldDriver 
  VTK_FIELD_DRIVER<double> *aVtkFieldDriver_double = new VTK_FIELD_DRIVER<double>(filename_wr, aField_1);
  
  //Check driver
  CPPUNIT_ASSERT(aVtkFieldDriver_double);
  
  //Test openAppend() method 
  try 
  { 
    aVtkFieldDriver_double->openAppend();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  
  aVtkFieldDriver_double->setFieldName(fieldname_wr_double);

#ifdef ENABLE_FAULTS
  //Test writeAppend() method 
  try 
  { 
    aVtkFieldDriver_double->writeAppend();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  // => Segmentation fault
#endif
  
  try 
  { 
    aVtkFieldDriver_double->close();
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
  //VTK_FIELD_DRIVER<int> aVtkFieldDriver_intCpy_1;
  //Test (void operator =) defined in GENDRIVER class in MEDMEM_GenDriver.hxx
  //aVtkFieldDriver_intCpy_1 = *aVtkFieldDriver_int;
  CPPUNIT_FAIL("Compilation error: no matching function for call to 'MEDMEM::VTK_FIELD_DRIVER<int>::VTK_FIELD_DRIVER()");
#endif


#ifdef ENABLE_FAULTS
  //Test copy constructor
  VTK_FIELD_DRIVER<int> aVtkFieldDriver_intCpy_2 = VTK_FIELD_DRIVER<int> (*aVtkFieldDriver_int);
  // => Segmentation fault after call Copy Constructor
  

  CPPUNIT_ASSERT_EQUAL(aVtkFieldDriver_intCpy_2, *aVtkFieldDriver_int);
  //Test (bool operator ==) defined in GENDRIVER class in MEDMEM_GenDriver.hxx
  CPPUNIT_ASSERT(aVtkFieldDriver_intCpy_2.GENDRIVER::operator== (*aVtkFieldDriver_int));
  
  //Test (friend ostream & operator <<) defined in GENDRIVER class in MEDMEM_GenDriver.hxx
  ostringstream ostr1, ostr2;
  ostr1<<*aVtkFieldDriver_int;
  ostr2<<aVtkFieldDriver_intCpy_2;

  CPPUNIT_ASSERT(ostr1.str() != "");
  CPPUNIT_ASSERT(ostr1.str() == ostr2.str());
#endif
  //Delete all objects
  delete aField;
  delete aField_1;
#ifdef ENABLE_FORCED_FAILURES  
  delete aInvalidVtkFieldDriver_1;
  // (Bug) Exception in the destructor after trying close not existing file.
#endif
  delete aInvalidVtkFieldDriver_2;
  delete aMedRdFieldDriver21_int;
  delete aMedRdFieldDriver21_double;
  delete aVtkFieldDriver_int;
  delete aVtkFieldDriver_double;
}
