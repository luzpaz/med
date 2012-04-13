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

#include <MEDMEM_PorflowMeshDriver.hxx>
#include <MEDMEM_Mesh.hxx>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;

/*!
 *  Check methods (18), defined in MEDMEM_PorflowMeshDriver.hxx:
 *  class PORFLOW_MESH_DRIVER : public GENDRIVER {
 *   (+) PORFLOW_MESH_DRIVER();
 *   (+) PORFLOW_MESH_DRIVER(const string & fileName, MESH * ptrMesh, MED_EN::med_mode_acces accessMode);
 *   (+) PORFLOW_MESH_DRIVER(const PORFLOW_MESH_DRIVER & driver);
 *   (+) virtual ~PORFLOW_MESH_DRIVER();
 *   (+) void open() throw (MEDEXCEPTION);
 *   (+) void close() throw (MEDEXCEPTION);
 *   (-) virtual void write(void) const = 0;
 *   (+) virtual void read (void) = 0;
 *   (+) void   setMeshName(const string & meshName);
 *   (+) string getMeshName() const;
 *  }
 *  class PORFLOW_MESH_RDONLY_DRIVER : public virtual PORFLOW_MESH_DRIVER {
 *   (+) PORFLOW_MESH_RDONLY_DRIVER();
 *   (+) PORFLOW_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh);
 *   (+) PORFLOW_MESH_RDONLY_DRIVER(const PORFLOW_MESH_RDONLY_DRIVER & driver);
 *   (+) virtual ~PORFLOW_MESH_RDONLY_DRIVER();
 *   (+) void write(void) const throw (MEDEXCEPTION);
 *   (+) void read (void) throw (MEDEXCEPTION);
 *  }
 *  class PORFLOW_MESH_WRONLY_DRIVER : public virtual PORFLOW_MESH_DRIVER {
 *   (+) PORFLOW_MESH_WRONLY_DRIVER();
 *   (+) PORFLOW_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh);
 *   (+) PORFLOW_MESH_WRONLY_DRIVER(const PORFLOW_MESH_WRONLY_DRIVER & driver);
 *   (+) virtual ~PORFLOW_MESH_WRONLY_DRIVER();
 *   (NOT IMPLEMENTED!!!) void write(void) const throw (MEDEXCEPTION);
 *   (+) void read (void) throw (MEDEXCEPTION);
 *  }
 *  class PORFLOW_MESH_RDWR_DRIVER : public PORFLOW_MESH_RDONLY_DRIVER, public PORFLOW_MESH_WRONLY_DRIVER {
 *   (+) PORFLOW_MESH_RDWR_DRIVER();
 *   (+) PORFLOW_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh);
 *   (+) PORFLOW_MESH_RDWR_DRIVER(const PORFLOW_MESH_RDWR_DRIVER & driver);
 *   (+) ~PORFLOW_MESH_RDWR_DRIVER();
 *   (CALLS PORFLOW_MESH_WRONLY_DRIVER::write()) void write(void) const throw (MEDEXCEPTION);
 *   (+) void read (void) throw (MEDEXCEPTION);
 *  }
 */
void MEDMEMTest::testPorflowMeshDriver()
{
  MESH *aMesh                      = new MESH;
  MESH *aMesh_1                    = new MESH;

  string filename_rd               = getResourceFile("Case1.inp");
  string filename_wr               = makeTmpFile("myWr_Case1.inp");
  string meshname                  = "Case1";
  string newmeshname               = "new" + meshname;
  string fileNotExistsName_rd      = "notExists.inp";
  string fileNotExistsName_wr      = "/path_not_exists/file_not_exists.inp";
  string filename_rdwr             = makeTmpFile("myRdWr_Case1.inp", filename_rd);
  string res_file1                 = makeTmpFile("Case1.xyz", getResourceFile("Case1.xyz"));
  string res_file2                 = makeTmpFile("Case1.cnc", getResourceFile("Case1.cnc"));

  MEDMEMTest_TmpFilesRemover aRemover;
  aRemover.Register(filename_wr);
  aRemover.Register(filename_rdwr);
  aRemover.Register(res_file1);
  aRemover.Register(res_file2);

  //-----------------------------Test READ ONLY part---------------------------------------//
  {
    {
      //Creation a incorrect Porflow read only driver
      PORFLOW_MESH_RDONLY_DRIVER *aInvalidPorflowRdDriver =
        new PORFLOW_MESH_RDONLY_DRIVER(fileNotExistsName_rd, aMesh);

      //Trying open not existing file
      CPPUNIT_ASSERT_THROW(aInvalidPorflowRdDriver->open(), MEDEXCEPTION);

      delete aInvalidPorflowRdDriver;
    }

    //Creation a correct Porflow read only driver (normal constructor)
    PORFLOW_MESH_RDONLY_DRIVER *aPorflowRdDriver = new PORFLOW_MESH_RDONLY_DRIVER(filename_rd, aMesh);

    //Check driver
    CPPUNIT_ASSERT(aPorflowRdDriver);

    //Trying read mesh from file, if file is not open
    CPPUNIT_ASSERT_THROW(aPorflowRdDriver->read(), MEDEXCEPTION);

    //Test open() method 
    // commented by skl - must be exception since thid file is
    // opened yet and such case is tested below
    //CPPUNIT_ASSERT_NO_THROW(aPorflowRdDriver->open());

    //#ifdef ENABLE_FORCED_FAILURES
    //Trying open file secondary
    //CPPUNIT_ASSERT_THROW(aPorflowRdDriver->open(), MEDEXCEPTION);
    CPPUNIT_ASSERT_NO_THROW(aPorflowRdDriver->open());
    //This case  work, but it corrypt driver
    //#endif

    //Test read() method
    CPPUNIT_ASSERT_NO_THROW(aPorflowRdDriver->read());

    //Check Mesh
    CPPUNIT_ASSERT(aMesh);

    //Trying fill not empty mesh
    CPPUNIT_ASSERT_THROW(aPorflowRdDriver->read(), MEDEXCEPTION);

    //Test write() method for Porflow RDONLY Driver
    CPPUNIT_ASSERT_THROW(aPorflowRdDriver->write(), MEDEXCEPTION);

    //Test setMeshName() and getMeshName()
    CPPUNIT_ASSERT_NO_THROW(aPorflowRdDriver->setMeshName(meshname));
    CPPUNIT_ASSERT_EQUAL(meshname, aPorflowRdDriver->getMeshName());

    //Test close() method
    CPPUNIT_ASSERT_NO_THROW(aPorflowRdDriver->close());

    //Default constructor
    PORFLOW_MESH_RDONLY_DRIVER aPorflowRdDriverCpy_1;
    // TO DO

    //Test copy constructor
    PORFLOW_MESH_RDONLY_DRIVER aPorflowRdDriverCpy_2 (*aPorflowRdDriver);

    //Test (bool operator ==) defined GENDRIVER class in MEDMEM_GenDriver.hxx
    CPPUNIT_ASSERT(aPorflowRdDriverCpy_2.GENDRIVER::operator==(*aPorflowRdDriver));

    //Test (friend ostream & operator <<) defined GENDRIVER class in MEDMEM_GenDriver.hxx
    ostringstream rostr1, rostr2;
    rostr1 << *aPorflowRdDriver;
    rostr2 << aPorflowRdDriverCpy_2;
    CPPUNIT_ASSERT(rostr1.str() != "");
    CPPUNIT_ASSERT_EQUAL(rostr1.str() , rostr2.str());

    delete aPorflowRdDriver;
  }

  //---------------------------Test WRITE ONLY part-------------------------------------//
  {
    {
      //Creation a incorrect Porflow write only driver
      PORFLOW_MESH_WRONLY_DRIVER *aInvalidPorflowWrDriver =
        new PORFLOW_MESH_WRONLY_DRIVER(fileNotExistsName_wr, aMesh);

      //Test case: trying open non existing file
      CPPUNIT_ASSERT_THROW(aInvalidPorflowWrDriver->open(), MEDEXCEPTION);

      delete aInvalidPorflowWrDriver;
    }

    //Creation a correct write only driver
    PORFLOW_MESH_WRONLY_DRIVER *aPorflowWrDriver = new PORFLOW_MESH_WRONLY_DRIVER(filename_wr, aMesh);

    //Check driver
    CPPUNIT_ASSERT(aPorflowWrDriver);

    //Test case: trying write mesh to file, if file is not open
    CPPUNIT_ASSERT_THROW(aPorflowWrDriver->write(), MEDEXCEPTION);

    //Test open() method
    //#ifndef ENABLE_FORCED_FAILURES
    FILE *tmpFile = fopen(filename_wr.data(), "w");
    if ( tmpFile )
      fclose(tmpFile);
    //#endif
    CPPUNIT_ASSERT_NO_THROW(aPorflowWrDriver->open());

    //Test case: trying open file secondary.
    CPPUNIT_ASSERT_THROW(aPorflowWrDriver->open(), MEDEXCEPTION);

    //Test write() method
    //#ifdef ENABLE_FORCED_FAILURES
    //Write method is not implemented
    //CPPUNIT_ASSERT_NO_THROW(aPorflowWrDriver->write());
    CPPUNIT_ASSERT_THROW(aPorflowWrDriver->write(), MEDEXCEPTION);
    //#endif

    //Test read() method for WRITE ONLY driver
    CPPUNIT_ASSERT_THROW(aPorflowWrDriver->read(), MEDEXCEPTION);

    //Test setMeshName() and getMeshName()
    CPPUNIT_ASSERT_NO_THROW(aPorflowWrDriver->setMeshName(newmeshname));
    CPPUNIT_ASSERT_EQUAL(newmeshname, aPorflowWrDriver->getMeshName());

    CPPUNIT_ASSERT_NO_THROW(aPorflowWrDriver->close());

    //Default constructor
    PORFLOW_MESH_WRONLY_DRIVER aPorflowWrDriverCpy_1;

    //Test copy constructor
    PORFLOW_MESH_WRONLY_DRIVER aPorflowWrDriverCpy_2 (*aPorflowWrDriver);

    //Test (bool operator ==) defined in GENDRIVER class
    CPPUNIT_ASSERT(aPorflowWrDriverCpy_2.GENDRIVER::operator==(*aPorflowWrDriver));

    //Test (friend ostream & operator <<) defined in GENDRIVER class
    ostringstream wostr1, wostr2;
    wostr1 << *aPorflowWrDriver;
    wostr2 << aPorflowWrDriverCpy_2;
    CPPUNIT_ASSERT(wostr1.str() != "");
    CPPUNIT_ASSERT(wostr1.str() == wostr2.str());

    delete aPorflowWrDriver;
  }

  //------------------------Test READ / WRITE part------------------------------------//
  {
    {
      //Creation a incorrect read/write driver
      PORFLOW_MESH_RDWR_DRIVER *aInvalidPorflowRdWrDriver =
        new PORFLOW_MESH_RDWR_DRIVER(fileNotExistsName_wr, aMesh_1);

      //Test case: trying open non existing file
      CPPUNIT_ASSERT_THROW(aInvalidPorflowRdWrDriver->open(), MEDEXCEPTION);

      delete aInvalidPorflowRdWrDriver;
    }

    //Creation a correct read/write driver
    PORFLOW_MESH_RDWR_DRIVER *aPorflowRdWrDriver = new PORFLOW_MESH_RDWR_DRIVER(filename_rdwr, aMesh_1);

    //Check driver
    CPPUNIT_ASSERT(aPorflowRdWrDriver);

    //Test case: trying write mesh to file, if file is not open
    CPPUNIT_ASSERT_THROW(aPorflowRdWrDriver->write(), MEDEXCEPTION);

    //Test case: trying read mesh from file, if file is not open
    CPPUNIT_ASSERT_THROW(aPorflowRdWrDriver->read(), MEDEXCEPTION);

    //Test open() method
    CPPUNIT_ASSERT_NO_THROW(aPorflowRdWrDriver->open());

    //Test case: trying open file secondary.
    //#ifdef ENABLE_FORCED_FAILURES
    // (BUG) This case work, but corrupt driver
    CPPUNIT_ASSERT_THROW(aPorflowRdWrDriver->open(), MEDEXCEPTION);
    //#endif

    //Test read() method
    CPPUNIT_ASSERT_NO_THROW();
    CPPUNIT_ASSERT_NO_THROW(aPorflowRdWrDriver->read());

    //Test write() method
    //aPorflowRdWrDriver->setMeshName(newmeshname);
    //aMesh->setName(newmeshname);
    //CPPUNIT_ASSERT_NO_THROW(aPorflowRdWrDriver->write());
    //Write method is not implemented
    //#ifdef ENABLE_FORCED_FAILURES
    //CPPUNIT_FAIL("PORFLOW_MESH_RDWR_DRIVER::write() method not implemented");
    //#endif

    //Check Mesh
    CPPUNIT_ASSERT(aMesh);

    CPPUNIT_ASSERT_NO_THROW(aPorflowRdWrDriver->close());

    //Default constructor
    PORFLOW_MESH_RDWR_DRIVER aPorflowRdWrDriverCpy_1;
    // TO DO

    //Test copy constructor
    PORFLOW_MESH_RDWR_DRIVER aPorflowRdWrDriverCpy_2 (*aPorflowRdWrDriver);

    //Test (bool operator ==) defined in GENDRIVER class
    CPPUNIT_ASSERT(aPorflowRdWrDriverCpy_2.GENDRIVER::operator==(*aPorflowRdWrDriver));

    //Test (friend ostream & operator <<) defined in GENDRIVER class
    ostringstream rwostr1, rwostr2;
    rwostr1 << *aPorflowRdWrDriver;
    rwostr2 << aPorflowRdWrDriverCpy_2;
    CPPUNIT_ASSERT(rwostr1.str() != "");
    CPPUNIT_ASSERT(rwostr1.str() == rwostr2.str());

    delete aPorflowRdWrDriver;
  }

  //Delete all objects
  aMesh->removeReference();
  aMesh_1->removeReference();
}
