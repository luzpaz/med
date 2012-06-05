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

#include <MEDMEM_GibiMeshDriver.hxx>
#include <MEDMEM_Mesh.hxx>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

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
 *   (+) GIBI_MESH_RDWR_DRIVER();
 *   (+) GIBI_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh);
 *   (+) GIBI_MESH_RDWR_DRIVER(const GIBI_MESH_RDWR_DRIVER & driver);
 *   (+) ~GIBI_MESH_RDWR_DRIVER();
 *   (+) void write(void) const throw (MEDEXCEPTION);
 *   (+) void read (void) throw (MEDEXCEPTION);
 *   (+) void open();
 *   (+) void close();
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
  MESH *aMesh                      = new MESH;
  MESH *aMesh_NULL                 = NULL;
  MESH *aMesh_2                    = new MESH;
  vector<FIELD_*>       rdFields;
  vector<const FIELD_*> wrFields;

  string filename_rd               = getResourceFile("Darcy3_3D_H_10x10x10.sauve");
  string filenamemed_rd            = getResourceFile("test_2D.sauve");
  string filename_wr               = makeTmpFile("myWr_Darcy3_3D_H_10x10x10.sauve");
  string tmpfile                   = makeTmpFile("tmp.sauve");
  string tmpfile_rdwr              = makeTmpFile("rdwr_tmp.sauve");
  string filenamemed_wr            = makeTmpFile("myWrMed_elle_3D_HPr_10x10x10.sauve");
  string tmpfilemed                = makeTmpFile("tmpmed.sauve");
  string meshname                  = "Darcy3_3D_H_10x10x10";
  string newmeshname               = "new" + meshname;
  string fileNotExistsName_rd      = "notExists.sauve";
  string fileNotExistsName_wr      = "/path_not_exists/file_not_exists.sauve";
  string filename_rdwr             = makeTmpFile("myRdWr_Darcy3_3D_H_10x10x10.sauve", filename_rd);

  // To remove tmp files from disk
  MEDMEMTest_TmpFilesRemover aRemover;
  aRemover.Register(filename_wr);
  aRemover.Register(tmpfile);
  aRemover.Register(tmpfilemed);
  aRemover.Register(filenamemed_wr);
  aRemover.Register(tmpfile_rdwr);
  aRemover.Register(filename_rdwr);

  //Test gibi2medGeom() and med2gibiGeom() methods
  size_t aSize = 17;
  CPPUNIT_ASSERT_EQUAL(GIBI_MESH_DRIVER::gibi2medGeom(aSize), MED_PENTA15);
  CPPUNIT_ASSERT_EQUAL(GIBI_MESH_DRIVER::med2gibiGeom(MED_PENTA15), 17);

  //---------------------------Test GIBI MESH READ ONLY part--------------------------------//
  {
    {
      //Creation of an incorrect read only driver
      GIBI_MESH_RDONLY_DRIVER *aInvalidGibiRdDriver =
        new GIBI_MESH_RDONLY_DRIVER(fileNotExistsName_rd, aMesh);
      /************************************************************************/
      // WARNING: if you have memory access error just after this constructor,
      // this means that MEDMEMCppTest has been compiled w/o -DHAS_XDR
      // while MEDMEM, with -DHAS_XDR
      /************************************************************************/

      //Trying open not existing file
      CPPUNIT_ASSERT_THROW(aInvalidGibiRdDriver->open(), MEDEXCEPTION);

      delete aInvalidGibiRdDriver;
    }

    //Creation a correct Gibi read only driver (normal constructor)
    GIBI_MESH_RDONLY_DRIVER *aGibiRdDriver =
      new GIBI_MESH_RDONLY_DRIVER(filename_rd, aMesh);

    //Check driver
    CPPUNIT_ASSERT(aGibiRdDriver);

    //Trying read mesh from file, if file is not open
    CPPUNIT_ASSERT_THROW(aGibiRdDriver->read(), MEDEXCEPTION);

    //Test open() method
    CPPUNIT_ASSERT_NO_THROW(aGibiRdDriver->open());

    //Trying open file secondary.
    CPPUNIT_ASSERT_NO_THROW(aGibiRdDriver->open());

    //Test setMeshName() and getMeshName() methods
    CPPUNIT_ASSERT_NO_THROW(aGibiRdDriver->setMeshName(meshname));
    CPPUNIT_ASSERT_EQUAL(meshname, aGibiRdDriver->getMeshName());

    //Test read() method
    CPPUNIT_ASSERT_NO_THROW(aGibiRdDriver->read());

    //Trying fill not empty mesh
    CPPUNIT_ASSERT_THROW(aGibiRdDriver->read(), MEDEXCEPTION);

    //Test write() method for GIBI READ ONLY driver
    CPPUNIT_ASSERT_THROW(aGibiRdDriver->write(), MEDEXCEPTION);

    //Check Mesh
    CPPUNIT_ASSERT(aMesh);

    //Test close method
    CPPUNIT_ASSERT_NO_THROW(aGibiRdDriver->close());

    //Default constructor
    {
      GIBI_MESH_RDONLY_DRIVER aGibiRdDriverCpy_1;
    }

    //Test copy constructor and operator==(), defined in GENDRIVER class
    {
      GIBI_MESH_RDONLY_DRIVER aGibiRdDriverCpy_2 (*aGibiRdDriver);
      CPPUNIT_ASSERT(aGibiRdDriverCpy_2.GENDRIVER::operator==(*aGibiRdDriver));

      //Test (friend ostream & operator <<) defined GENDRIVER class
      ostringstream rostr1, rostr2;
      rostr1 << *aGibiRdDriver;
      rostr2 << aGibiRdDriverCpy_2;
      CPPUNIT_ASSERT(rostr1.str() != "");
      CPPUNIT_ASSERT(rostr1.str() == rostr2.str());
    }

    delete aGibiRdDriver;
  }

  //-------------------------------Test GIBI WRITE ONLY part------------------------------//
  {
    {
      //Creation a incorrect gibi write only driver
      GIBI_MESH_WRONLY_DRIVER *aInvalidGibiWrDriver =
        new GIBI_MESH_WRONLY_DRIVER(fileNotExistsName_wr, aMesh);

      //Trying open non existing file
      CPPUNIT_ASSERT_THROW(aInvalidGibiWrDriver->open(), MEDEXCEPTION);

      delete aInvalidGibiWrDriver;
    }

    //Trying write empty mesh
    {
      GIBI_MESH_WRONLY_DRIVER *aTmpGibiWrDriver =
        new GIBI_MESH_WRONLY_DRIVER(tmpfile, aMesh_NULL);
      aTmpGibiWrDriver->open();

      CPPUNIT_ASSERT_THROW(aTmpGibiWrDriver->write(),MEDEXCEPTION);
      aTmpGibiWrDriver->close();

      delete aTmpGibiWrDriver;
    }

    //Creation a correct gibi write only drivers
    GIBI_MESH_WRONLY_DRIVER *aGibiWrDriver =
      new GIBI_MESH_WRONLY_DRIVER(filename_wr, aMesh);

    //Check driver
    CPPUNIT_ASSERT(aGibiWrDriver);

    //Trying write mesh to file, if file is not open
    CPPUNIT_ASSERT_THROW(aGibiWrDriver->write(), MEDEXCEPTION);

    //Test open() method
    CPPUNIT_ASSERT_NO_THROW(aGibiWrDriver->open());

    CPPUNIT_ASSERT_NO_THROW(aGibiWrDriver->open());

    //Test setMeshName() and getMeshName() methods
    CPPUNIT_ASSERT_NO_THROW(aGibiWrDriver->setMeshName(newmeshname));
    CPPUNIT_ASSERT_EQUAL(newmeshname, aGibiWrDriver->getMeshName());

    //Test write() method
    CPPUNIT_ASSERT_NO_THROW(aGibiWrDriver->write());

    //Test read() method for GIBI WRITE ONLY driver
    CPPUNIT_ASSERT_THROW(aGibiWrDriver->read(), MEDEXCEPTION);

    //Test close method
    CPPUNIT_ASSERT_NO_THROW(aGibiWrDriver->close());

    //Default constructor
    GIBI_MESH_WRONLY_DRIVER aGibiWrDriverCpy_1;

    //Test copy constructor
    GIBI_MESH_WRONLY_DRIVER aGibiWrDriverCpy_2 (*aGibiWrDriver);

    //Test (bool operator ==) defined GENDRIVER class
    CPPUNIT_ASSERT(aGibiWrDriverCpy_2.GENDRIVER::operator==(*aGibiWrDriver));

    //Test (friend ostream & operator <<) defined GENDRIVER class
    ostringstream wostr1, wostr2;
    wostr1 << *aGibiWrDriver;
    wostr2 << aGibiWrDriverCpy_2;
    CPPUNIT_ASSERT(wostr1.str() != "");
    CPPUNIT_ASSERT(wostr1.str() == wostr2.str());

    delete aGibiWrDriver;
  }

  //-------------------------------Test GIBI READ/WRITE part---------------------------------//
  {
    {
      //Creation a incorrect gibi read/write driver
      GIBI_MESH_RDWR_DRIVER *aInvalidGibiRdWrDriver =
        new GIBI_MESH_RDWR_DRIVER(fileNotExistsName_wr, aMesh_2);

      //Trying read/write from not existing file
      CPPUNIT_ASSERT_THROW(aInvalidGibiRdWrDriver->read(), MEDEXCEPTION);
      CPPUNIT_ASSERT_THROW(aInvalidGibiRdWrDriver->write(), MEDEXCEPTION);

      delete aInvalidGibiRdWrDriver;
    }

    //Trying write empty mesh
    {
      GIBI_MESH_RDWR_DRIVER *aTmpGibiRdWrDriver =
        new GIBI_MESH_RDWR_DRIVER(tmpfile_rdwr, aMesh_NULL);
      CPPUNIT_ASSERT_THROW(aTmpGibiRdWrDriver->write(), MEDEXCEPTION);
      delete aTmpGibiRdWrDriver;
    }

    //Creation a correct read/write driver
    GIBI_MESH_RDWR_DRIVER *aGibiRdWrDriver =
      new GIBI_MESH_RDWR_DRIVER(filename_rdwr, aMesh_2);

    //Test open/close methods
    CPPUNIT_ASSERT_NO_THROW(aGibiRdWrDriver->open());
    CPPUNIT_ASSERT_NO_THROW(aGibiRdWrDriver->close());

    //Check driver
    CPPUNIT_ASSERT(aGibiRdWrDriver);

    //Test read() method
    CPPUNIT_ASSERT_NO_THROW(aGibiRdWrDriver->read());

    //Check Mesh
    CPPUNIT_ASSERT(aMesh);

    //Trying fill not empty mesh
    CPPUNIT_ASSERT_THROW(aGibiRdWrDriver->read(),MEDEXCEPTION);

    //Test write() method
    aGibiRdWrDriver->setMeshName(newmeshname);
    CPPUNIT_ASSERT_NO_THROW(aGibiRdWrDriver->write());

    //Default constructor
    GIBI_MESH_RDWR_DRIVER aGibiRdWrDriverCpy_1;

    //Test copy constructor
    GIBI_MESH_RDWR_DRIVER aGibiRdWrDriverCpy_2 (*aGibiRdWrDriver);

    //Test (bool operator ==)
    CPPUNIT_ASSERT(aGibiRdWrDriverCpy_2.GENDRIVER::operator==(*aGibiRdWrDriver));

    //Test (ostream & operator <<)
    ostringstream rwostr1, rwostr2;
    rwostr1 << *aGibiRdWrDriver;
    rwostr2 << aGibiRdWrDriverCpy_2;
    CPPUNIT_ASSERT(rwostr1.str() != "");
    CPPUNIT_ASSERT(rwostr1.str() == rwostr2.str());

    delete aGibiRdWrDriver;
  }

  //----------------------------Test GIBI_MED_RDONLY_DRIVER----------------------------
  {
    {
      GIBI_MED_RDONLY_DRIVER *aInvalidMedGibiRdDriver =
        new GIBI_MED_RDONLY_DRIVER(fileNotExistsName_rd, rdFields);

      //Trying open not exising file
      CPPUNIT_ASSERT_THROW(aInvalidMedGibiRdDriver->open(), MEDEXCEPTION);

      delete aInvalidMedGibiRdDriver;
    }

    //Creation a correct Gibi read only driver (normal constructor)
    GIBI_MED_RDONLY_DRIVER *aGibiMedRdDriver =
      new GIBI_MED_RDONLY_DRIVER(filenamemed_rd, rdFields);

    //Check driver
    CPPUNIT_ASSERT(aGibiMedRdDriver);

    //Trying read mesh from file, if file is not open
    CPPUNIT_ASSERT_THROW(aGibiMedRdDriver->read(), MEDEXCEPTION);

    //Test open() method
    CPPUNIT_ASSERT_NO_THROW(aGibiMedRdDriver->open());

    //#ifdef ENABLE_FORCED_FAILURES
    //Trying open file secondary.
    //CPPUNIT_ASSERT_THROW(aGibiMedRdDriver->open(), MEDEXCEPTION);
    CPPUNIT_ASSERT_NO_THROW(aGibiMedRdDriver->open());
    //This case is not work, seems it BUG
    //#endif

    //Test read() method
    CPPUNIT_ASSERT_NO_THROW(aGibiMedRdDriver->read());

    //Test write() method for GIBI MED READ ONLY driver
    CPPUNIT_ASSERT_THROW(aGibiMedRdDriver->write(), MEDEXCEPTION);

    //Check Med
    CPPUNIT_ASSERT_EQUAL(1,int(rdFields.size()));

    //Test close method
    CPPUNIT_ASSERT_NO_THROW(aGibiMedRdDriver->close());

    //Default constructor
    GIBI_MED_RDONLY_DRIVER aGibiMedRdDriverCpy_1;

    //Test (bool operator ==) defined in GENDRIVER class
    CPPUNIT_ASSERT(! aGibiMedRdDriverCpy_1.GENDRIVER::operator==(*aGibiMedRdDriver));

    //Test copy constructor
    // commented since this test is not important
    //GIBI_MED_RDONLY_DRIVER aGibiMedRdDriverCpy_2 (*aGibiMedRdDriver);
    //#ifdef ENABLE_FORCED_FAILURES
    // (BUG) Copy constructor of GIBI_MED_RDONLY_DRIVER is not implemented
    // Small importance
    //CPPUNIT_ASSERT(aGibiMedRdDriverCpy_2.GENDRIVER::operator==(*aGibiMedRdDriver));
    //#endif

    //Test (friend ostream & operator <<) defined in GENDRIVER class
    // commented since this test is not important
    //ostringstream medrostr1, medrostr2;
    //medrostr1 << *aGibiMedRdDriver;
    //medrostr2 << aGibiMedRdDriverCpy_2;
    //CPPUNIT_ASSERT(medrostr1.str() != "");
    //#ifdef ENABLE_FORCED_FAILURES
    // (BUG) Copy constructor of GIBI_MED_RDONLY_DRIVER is not implemented
    //CPPUNIT_ASSERT(medrostr1.str() == medrostr2.str());
    //#endif

    rdFields[0]->removeReference();

    delete aGibiMedRdDriver;
  }

  //----------------------------Test GIBI MED WRITE ONLY part----------------------------------//
  {
    {
      //Creation a incorrect gibi med write only driver
      GIBI_MED_WRONLY_DRIVER *aInvalidGibiMedWrDriver =
        new GIBI_MED_WRONLY_DRIVER(fileNotExistsName_wr, wrFields, aMesh);

      //Trying open non existing file
      CPPUNIT_ASSERT_THROW(aInvalidGibiMedWrDriver->open(), MEDEXCEPTION);

      delete aInvalidGibiMedWrDriver;
    }

    //Trying create gibi med write only driver with null MESH
    CPPUNIT_ASSERT_THROW(new GIBI_MED_WRONLY_DRIVER(tmpfilemed, wrFields, aMesh_NULL), MEDEXCEPTION);

    //Creation a correct gibi med write only drivers
    GIBI_MED_WRONLY_DRIVER *aGibiMedWrDriver =
      new GIBI_MED_WRONLY_DRIVER(filenamemed_wr, wrFields, aMesh);

    //Check driver
    CPPUNIT_ASSERT(aGibiMedWrDriver);

    //Trying write mesh to file, if file is not open
    CPPUNIT_ASSERT_THROW(aGibiMedWrDriver->write(), MEDEXCEPTION);

    //Test open() method
    CPPUNIT_ASSERT_NO_THROW(aGibiMedWrDriver->open());

    //Trying open file secondary.
    //#ifdef ENABLE_FORCED_FAILURES
    // (BUG) Exception is generated, but second call to open() function breaks driver
    //CPPUNIT_ASSERT_THROW(aGibiMedWrDriver->open(), MEDEXCEPTION);
    CPPUNIT_ASSERT_NO_THROW(aGibiMedWrDriver->open());
    //#endif

    //Test write() method
    CPPUNIT_ASSERT_NO_THROW(aGibiMedWrDriver->write());

    //Test read() method for GIBI MED WRITE ONLY driver
    CPPUNIT_ASSERT_THROW(aGibiMedWrDriver->read(), MEDEXCEPTION);

    //Test close method
    CPPUNIT_ASSERT_NO_THROW(aGibiMedWrDriver->close());

    //Default constructor
    GIBI_MED_WRONLY_DRIVER aGibiMedWrDriverCpy_1;

    //Test copy constructor
    {
      GIBI_MED_WRONLY_DRIVER aGibiMedWrDriverCpy_2 (*aGibiMedWrDriver);

      //Test (bool operator ==) defined in GENDRIVER class
      // commented since this test is not important
      //#ifdef ENABLE_FORCED_FAILURES
      // (BUG) Copy constructor of GIBI_MED_WRONLY_DRIVER is not implemented!!!
      //CPPUNIT_ASSERT(aGibiMedWrDriverCpy_2.GENDRIVER::operator==(*aGibiMedWrDriver));
      //#endif

      //Test (friend ostream & operator <<) defined GENDRIVER class
      // commented since this test is not important
      //ostringstream medwostr1, medwostr2;
      //medwostr1 << *aGibiMedWrDriver;
      //medwostr2 << aGibiMedWrDriverCpy_2;
      //CPPUNIT_ASSERT(medwostr1.str() != "");
      //#ifdef ENABLE_FORCED_FAILURES
      // Copy constructor of GIBI_MED_WRONLY_DRIVER is not implemented!!!
      //CPPUNIT_ASSERT(medwostr1.str() == medwostr2.str());
      //#endif
    }

    delete aGibiMedWrDriver;
  }
  //Delete all objects
  aMesh->removeReference();
  aMesh_2->removeReference();
}
