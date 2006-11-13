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
#include <cppunit/Message.h>
#include <cppunit/TestAssert.h>

#include <cstdlib>
#include <exception>

#include <MEDMEM_MedMeshDriver22.hxx>
#include <MEDMEM_Mesh.hxx>

using namespace std;
using namespace MEDMEM;

/*!
 *  Check methods (13), defined in MEDMEM_MedMeshDriver22.hxx:
 *  class MED_MESH_DRIVER22 : public virtual MED_MESH_DRIVER {
 *   (yetno) MED_MESH_DRIVER22();
 *   (yetno) MED_MESH_DRIVER22(const string & fileName, MESH * ptrMesh, MED_EN::med_mode_acces accessMode);
 *   (yetno) MED_MESH_DRIVER22(const MED_MESH_DRIVER22 & driver);
 *   (yetno) virtual ~MED_MESH_DRIVER22();
 *   (+) void open();
 *   (yetno) void close();
 *  }
 *  class MED_MESH_RDONLY_DRIVER22 : public virtual IMED_MESH_RDONLY_DRIVER,
 *                                   public virtual MED_MESH_DRIVER22 {
 *   (yetno) MED_MESH_RDONLY_DRIVER22();
 *   (+) MED_MESH_RDONLY_DRIVER22(const string & fileName, MESH * ptrMesh);
 *   (yetno) MED_MESH_RDONLY_DRIVER22(const MED_MESH_RDONLY_DRIVER22 & driver);
 *   (yetno) virtual ~MED_MESH_RDONLY_DRIVER22();
 *   (yetno) void read (void);
 *  }
 *  class MED_MESH_WRONLY_DRIVER22 : public virtual IMED_MESH_WRONLY_DRIVER,
 *                                   public virtual MED_MESH_DRIVER22 {
 *   (yetno) MED_MESH_WRONLY_DRIVER22();
 *   (yetno) MED_MESH_WRONLY_DRIVER22(const string & fileName, MESH * ptrMesh);
 *   (yetno) MED_MESH_WRONLY_DRIVER22(const MED_MESH_WRONLY_DRIVER22 & driver);
 *   (yetno) virtual ~MED_MESH_WRONLY_DRIVER22();
 *   (yetno) void write(void) const;
 *  }
 *  class MED_MESH_RDWR_DRIVER22 : public IMED_MESH_RDWR_DRIVER,
 *                                 public MED_MESH_RDONLY_DRIVER22, public MED_MESH_WRONLY_DRIVER22 {
 *   (yetno) MED_MESH_RDWR_DRIVER22();
 *   (yetno) MED_MESH_RDWR_DRIVER22(const string & fileName, MESH * ptrMesh);
 *   (yetno) MED_MESH_RDWR_DRIVER22(const MED_MESH_RDWR_DRIVER22 & driver);
 *   (yetno) ~MED_MESH_RDWR_DRIVER22();
 *   (yetno) void write(void) const;
 *   (yetno) void read (void);
 *  }
 */


void MEDMEMTest::testMedMeshDriver22()
{
  MESH aMesh;
  string filename = getenv("DATA_DIR");
  filename += "/MedFiles/pointe_import22.med";
  string meshname = "maa1";
  string fileNotExistsName = "notExists.med";
  
  MED_MESH_RDONLY_DRIVER22 *aInvalidDriver22 = new MED_MESH_RDONLY_DRIVER22(fileNotExistsName, &aMesh);
  CPPUNIT_ASSERT_THROW(aInvalidDriver22->open(), MEDEXCEPTION);
  
  //Creation a Driver
  MED_MESH_RDONLY_DRIVER22 *aDriver22 = new MED_MESH_RDONLY_DRIVER22(filename, &aMesh);

  //Check Driver
  CPPUNIT_ASSERT(aDriver22);
  aDriver22->setMeshName(meshname);
    
  //Test open() method
  try
  {
    aDriver22->open();
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
    aDriver22->read();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
 
}
