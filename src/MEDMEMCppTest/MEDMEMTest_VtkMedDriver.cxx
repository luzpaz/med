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
#include <MEDMEM_VtkMedDriver.hxx>
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
 *  Check methods (5), defined in MEDMEM_VtkMedDriver.hxx:
 *  class VTK_MED_DRIVER : public GENDRIVER {
 *   (+) VTK_MED_DRIVER(const string & fileName,  MED * const ptrMed);
 *   (+) VTK_MED_DRIVER(const VTK_MED_DRIVER & driver);
 *   (+) ~VTK_MED_DRIVER();
 *   (+) void write (void) const;
 *   (-) virtual void read (void)  {};
 *   (+) GENDRIVER * copy (void) const;
 *  }
 */
void MEDMEMTest::testVtkMedDriver()
{
  MED *aMed                = new MED();
  string filename_rd       = getResourceFile("pointe.med");
  string emptyfilename     = "";
  string fileNotExistsName = "/path_not_exists/file_not_exists.vtk";
  string filename_wr       = makeTmpFile( "myMED_pointe.vtk" );

  MEDMEMTest_TmpFilesRemover aRemover;
  aRemover.Register(filename_wr);

  //Read MED structure from file
  MED_MED_RDONLY_DRIVER21 *aMedMedRdDriver21 = new MED_MED_RDONLY_DRIVER21(filename_rd, aMed);
  aMedMedRdDriver21->open();
  aMedMedRdDriver21->read();
  aMedMedRdDriver21->close();
  //Check Med
  CPPUNIT_ASSERT(aMed);

  //Creation incorrect Vtk Med Driver (file is not exists)
  VTK_MED_DRIVER *aInvalidVtkMedDriver = new VTK_MED_DRIVER(fileNotExistsName,aMed);

  //Trying write data in the not existing file
  CPPUNIT_ASSERT_THROW(aInvalidVtkMedDriver->write(),MEDEXCEPTION);

  //Creation correct Vtk Med Driver
  VTK_MED_DRIVER *aVtkMedDriver = new VTK_MED_DRIVER(filename_wr ,aMed);

  //Check driver
  CPPUNIT_ASSERT(aVtkMedDriver);

  //Test write() method

  try
  {
    aVtkMedDriver->write();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Test BINARY write() method

  DRIVERFACTORY::setVtkBinaryFormatForWriting( true );
  try
  {
    aVtkMedDriver->write();
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
  

  //Test copy constructor
  VTK_MED_DRIVER *aVtkMedDriverCpy = new VTK_MED_DRIVER(*aVtkMedDriver);
  CPPUNIT_ASSERT_EQUAL(*aVtkMedDriverCpy, *aVtkMedDriver);

  //Test (operator ==) defined in GENDRIVER class in MEDMEM_GenDriver.hxx
  CPPUNIT_ASSERT(*aVtkMedDriverCpy == *aVtkMedDriver);

  //Test copy() function
  VTK_MED_DRIVER *aVtkMedDriverCpy_1 = (VTK_MED_DRIVER*)aVtkMedDriver->copy();
  CPPUNIT_ASSERT_EQUAL(*aVtkMedDriverCpy_1, *aVtkMedDriver);

  //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
  ostringstream ostr1, ostr2;
  ostr1<<*aVtkMedDriverCpy;
  ostr2<<*aVtkMedDriverCpy_1;
  CPPUNIT_ASSERT(ostr1.str() != "");
  CPPUNIT_ASSERT(ostr1.str() == ostr2.str());


  //Delete objects
  delete aMedMedRdDriver21;
  delete aInvalidVtkMedDriver;
  delete aVtkMedDriver;
  delete aVtkMedDriverCpy;
  delete aVtkMedDriverCpy_1;
}
