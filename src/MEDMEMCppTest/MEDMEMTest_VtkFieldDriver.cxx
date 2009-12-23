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

#include <MEDMEM_VtkFieldDriver.hxx>
#include <MEDMEM_Med.hxx>
#include <MEDMEM_Field.hxx>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

/*!
 *  Check methods (14), defined in MEDMEM_VtkFieldDriver.hxx:
 *  template <class T> class VTK_FIELD_DRIVER : public GENDRIVER {
 *       //MUST BE PRIVATE, because it is impossible to set _ptrField after this constructor.
 *       //AND cannot compile, so maybe it is specially implemented to prevent usage of it.
 *   (!) template <class INTERLACING_TAG> VTK_FIELD_DRIVER();
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

  string filename_rd                = getResourceFile("pointe_import22.med");
  string emptyfilename              = "";
  string fileNotExistsName          = "/path_not_exists/file_not_exists.vtk";
  string filename_wr                = makeTmpFile( "myField_pointe.vtk" );

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
  MED_FIELD_RDONLY_DRIVER22<int> *aMedRdFieldDriver22_int = new MED_FIELD_RDONLY_DRIVER22<int>(filename_rd, aField);
  aMedRdFieldDriver22_int->open();
  aMedRdFieldDriver22_int->setFieldName(fieldname_rd_int);
  aMedRdFieldDriver22_int->read();
  aMedRdFieldDriver22_int->close();
  MESH mesh(MED_DRIVER,filename_rd,"maa1");
  aField->getSupport()->setMesh(&mesh);

  MED_FIELD_RDONLY_DRIVER22<double> *aMedRdFieldDriver22_double = new MED_FIELD_RDONLY_DRIVER22<double>(filename_rd, aField_1);
  aMedRdFieldDriver22_double->open();
  aMedRdFieldDriver22_double->setFieldName(fieldname_rd_double);
  aMedRdFieldDriver22_double->read();
  aMedRdFieldDriver22_double->close();
  aField_1->getSupport()->setMesh(&mesh);
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

  /////////////////////////
  //  TEST BINARY FORMAT //
  /////////////////////////

  DRIVERFACTORY::setVtkBinaryFormatForWriting( true );

  //BINARY: Test open() method
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

  //BINARY: Test write() method
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

  //BINARY: Test read() method for Vtk Field Driver
  CPPUNIT_ASSERT_THROW(aVtkFieldDriver_int->read(),MEDEXCEPTION);

  //BINARY: Test close() method
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

  //BINARY: Test openAppend() method
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
  //  BINARY: Test openAppend() and writeAppend() methods  //
  /////////////////////////////////////////////////////////

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

  //BINARY: Test writeAppend() method
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

  DRIVERFACTORY::setVtkBinaryFormatForWriting( false );

  VTK_FIELD_DRIVER<int> aVtkFieldDriver_intCpy_1;

  //Test copy constructor
  VTK_FIELD_DRIVER<int> aVtkFieldDriver_intCpy_2 (*aVtkFieldDriver_int);

  CPPUNIT_ASSERT_EQUAL(aVtkFieldDriver_intCpy_2, *aVtkFieldDriver_int);
  //Test (bool operator ==) defined in GENDRIVER class in MEDMEM_GenDriver.hxx
  CPPUNIT_ASSERT(aVtkFieldDriver_intCpy_2.GENDRIVER::operator== (*aVtkFieldDriver_int));

  //Test (friend ostream & operator <<) defined in GENDRIVER class in MEDMEM_GenDriver.hxx
  ostringstream ostr1, ostr2;
  ostr1<<*aVtkFieldDriver_int;
  ostr2<<aVtkFieldDriver_intCpy_2;

  CPPUNIT_ASSERT(ostr1.str() != "");
  CPPUNIT_ASSERT(ostr1.str() == ostr2.str());

  //Delete all objects
  delete aField;
  delete aField_1;
  delete aInvalidVtkFieldDriver_1;
  delete aInvalidVtkFieldDriver_2;
  delete aMedRdFieldDriver22_int;
  delete aMedRdFieldDriver22_double;
  delete aVtkFieldDriver_int;
  delete aVtkFieldDriver_double;
}
