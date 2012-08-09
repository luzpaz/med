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

#include <MEDMEM_MedFileBrowser.hxx>
#include <MEDMEM_VtkMedDriver.hxx>

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
  MEDFILEBROWSER *aMed     = new MEDFILEBROWSER();
  string filename_rd       = getResourceFile("pointe.med");
  string emptyfilename     = "";
  string fileNotExistsName = "/path_not_exists/file_not_exists.vtk";
  string filename_wr       = makeTmpFile( "myMED_pointe.vtk" );

  MEDMEMTest_TmpFilesRemover aRemover;
  aRemover.Register(filename_wr);

  //Read MED structure from file (not generic code)
  vector<const FIELD_*> fields;
  aMed->readFileStruct( filename_rd );
  MESH mesh(MED_DRIVER, aMed->getFileName(), aMed->getMeshNames()[0] );
  vector<string> fieldsNames = aMed->getFieldNames();
  for ( unsigned i = 0; i < fieldsNames.size(); ++i )
  {
    VEC_DT_IT_ vecDtIt = aMed->getFieldIteration( fieldsNames[i] );
    for ( unsigned j = 0; j < vecDtIt.size(); ++j )
      if ( aMed->getFieldType( fieldsNames[i] ) == MED_REEL64 )
        fields.push_back( new FIELD<double>(MED_DRIVER, filename_rd, fieldsNames[i],
                                            vecDtIt[j].dt, vecDtIt[j].it, &mesh));
      else
        fields.push_back( new FIELD<int>(MED_DRIVER, filename_rd, fieldsNames[i],
                                         vecDtIt[j].dt, vecDtIt[j].it, &mesh));
  }      
  
  //Check Med
  CPPUNIT_ASSERT(fields.size());

  //Creation incorrect Vtk Med Driver (file is not exists)
  VTK_MED_DRIVER *aInvalidVtkMedDriver = new VTK_MED_DRIVER(fileNotExistsName,fields);

  //Trying write data in the not existing file
  CPPUNIT_ASSERT_THROW(aInvalidVtkMedDriver->write(),MEDEXCEPTION);

  //Creation correct Vtk Med Driver
  VTK_MED_DRIVER *aVtkMedDriver = new VTK_MED_DRIVER(filename_wr ,fields);

  //Check driver
  CPPUNIT_ASSERT(aVtkMedDriver);

  //Test write() method

  CPPUNIT_ASSERT_NO_THROW(aVtkMedDriver->write());


  //Test BINARY write() method

  DRIVERFACTORY::setVtkBinaryFormatForWriting( true );
  CPPUNIT_ASSERT_NO_THROW( aVtkMedDriver->write() );
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
  delete aInvalidVtkMedDriver;
  delete aVtkMedDriver;
  delete aVtkMedDriverCpy;
  delete aVtkMedDriverCpy_1;
  delete aMed;

  for ( unsigned i = 0; i < fields.size(); ++i )
    fields[i]->removeReference();
}
