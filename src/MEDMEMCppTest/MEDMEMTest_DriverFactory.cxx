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
#include <cppunit/Message.h>
#include <cppunit/TestAssert.h>

#include "MEDMEM_DriverFactory.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_GibiMeshDriver.hxx"
#include "MEDMEM_MedFieldDriver21.hxx"
#include "MEDMEM_MedFieldDriver22.hxx"
#include "MEDMEM_MedFieldDriver.hxx"
#include "MEDMEM_Med.hxx"
#include "MEDMEM_MedMedDriver21.hxx"
#include "MEDMEM_MedMedDriver22.hxx"
#include "MEDMEM_MedMedDriver.hxx"
#include "MEDMEM_MedMeshDriver21.hxx"
#include "MEDMEM_MedMeshDriver22.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_PorflowMeshDriver.hxx"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_VtkFieldDriver.hxx"
#include "MEDMEM_VtkMedDriver.hxx"
#include "MEDMEM_VtkMeshDriver.hxx"

#include <sstream>
#include <cmath>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;

// #9: MEDMEM_DriverFactory.hxx  }  MEDMEMTest_DriverFactory.cxx

/*!
 *  Check methods (11), defined in MEDMEM_DriverFactory.hxx:
 *  namespace DRIVERFACTORY {
 *   (+)     MED_EN::medFileVersion getMedFileVersionForWriting();
 *   (+)     void setMedFileVersionForWriting(MED_EN::medFileVersion version);
 *   (+)     driverTypes deduceDriverTypeFromFileName(const std::string & fileName);
 *
 *   (+)     GENDRIVER * buildDriverForMesh (driverTypes driverType, const std::string & fileName,
 *                                           MESH *mesh, const string & driverName,
 *                                           MED_EN::med_mode_acces access);
 *   (+)     template<class T, class INTERLACING_TAG>
 *           GENDRIVER * buildDriverForField(driverTypes driverType, const std::string & fileName,
 *                                           FIELD<T,INTERLACING_TAG> *fielde,
 *                                           MED_EN::med_mode_acces access);
 *   (+)     GENDRIVER * buildDriverForMed  (driverTypes driverType, const std::string & fileName,
 *                                           MED *mede,
 *                                           MED_EN::med_mode_acces access);
 *
 *   (+)     GENDRIVER * buildMedDriverFromFile  (const string & fileName, MED * const ptrMed,
 *                                                MED_EN::med_mode_acces access);
 *   (+)     GENDRIVER * buildMeshDriverFromFile (const string & fileName, MESH * ptrMesh,
 *                                                MED_EN::med_mode_acces access);
 *   (+)     template<class T, class INTERLACING_TAG>
 *           GENDRIVER * buildFieldDriverFromFile(const string & fileName,
 *                                                FIELD<T,INTERLACING_TAG> * ptrField,
 *                                                MED_EN::med_mode_acces access);
 *
 *   (+)     GENDRIVER * buildConcreteMedDriverForMesh(const std::string & fileName, MESH *mesh,
 *                                                     const string & driverName,
 *                                                     MED_EN::med_mode_acces access,
 *                                                     MED_EN::medFileVersion version);
 *   (+)     template<class T, class INTERLACING_TAG>
 *           GENDRIVER * buildConcreteMedDriverForField(const std::string & fileName,
 *                                                      FIELD<T,INTERLACING_TAG> *fielde,
 *                                                      MED_EN::med_mode_acces access,
 *                                                      MED_EN::medFileVersion version);
 *  }
 */
void MEDMEMTest::testDriverFactory()
{
  ///////////////////////////////////////////////////////////////////////
  // Test 1: getMedFileVersionForWriting & setMedFileVersionForWriting //
  ///////////////////////////////////////////////////////////////////////

  // save current version preference
  MED_EN::medFileVersion aVersionSaved = DRIVERFACTORY::getMedFileVersionForWriting();

  // check version preference changing
  MED_EN::medFileVersion aVersionToSet =
    (aVersionSaved == MED_EN::V22) ? MED_EN::V21 : MED_EN::V22;
  DRIVERFACTORY::setMedFileVersionForWriting(aVersionToSet);
  CPPUNIT_ASSERT(aVersionToSet == DRIVERFACTORY::getMedFileVersionForWriting());

  // restore default version preference
  DRIVERFACTORY::setMedFileVersionForWriting(aVersionSaved);

  //////////////////////////////////////////
  // Test 2: deduceDriverTypeFromFileName //
  //////////////////////////////////////////
  CPPUNIT_ASSERT(DRIVERFACTORY::deduceDriverTypeFromFileName("test.med")   == MED_DRIVER);
  CPPUNIT_ASSERT(DRIVERFACTORY::deduceDriverTypeFromFileName("t.st.sauve") == GIBI_DRIVER);
  CPPUNIT_ASSERT(DRIVERFACTORY::deduceDriverTypeFromFileName("t.st.sauv")  == GIBI_DRIVER);
  CPPUNIT_ASSERT(DRIVERFACTORY::deduceDriverTypeFromFileName("t.st.cnc")   == PORFLOW_DRIVER);
  CPPUNIT_ASSERT(DRIVERFACTORY::deduceDriverTypeFromFileName("t.st.inp")   == PORFLOW_DRIVER);
  CPPUNIT_ASSERT(DRIVERFACTORY::deduceDriverTypeFromFileName("t.st.xyz")   == PORFLOW_DRIVER);
  CPPUNIT_ASSERT(DRIVERFACTORY::deduceDriverTypeFromFileName("t.st.vtk")   == VTK_DRIVER);
  CPPUNIT_ASSERT(DRIVERFACTORY::deduceDriverTypeFromFileName("t_st_med")   == NO_DRIVER);
  CPPUNIT_ASSERT(DRIVERFACTORY::deduceDriverTypeFromFileName("t.st.med1")  == NO_DRIVER);

  ////////////////////////////////////////////////////////
  // Test building drivers for different types of files //
  ////////////////////////////////////////////////////////
  GENDRIVER * aDriver = NULL;

  ///////////////////////////////
  // Test 3: buildDriverForMed //
  ///////////////////////////////
  MED med;

  // 3.1: MED_DRIVER

  // rdonly
  aDriver = DRIVERFACTORY::buildDriverForMed(MED_DRIVER, "anyfile", &med, MED_EN::RDONLY);
//#ifdef ENABLE_FORCED_FAILURES
  // (BUG) See more details below for the same problem of MED_MESH_RDONLY_DRIVER
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT_EQUAL(MED_EN::RDONLY, aDriver->getAccessMode());
//#endif

  MED_MED_RDONLY_DRIVER * aMedRDriverForMed = dynamic_cast<MED_MED_RDONLY_DRIVER *> (aDriver);
  CPPUNIT_ASSERT(aMedRDriverForMed);

  delete aDriver;

  // wronly
  aDriver = DRIVERFACTORY::buildDriverForMed(MED_DRIVER, "anyfile", &med, MED_EN::WRONLY);
//#ifdef ENABLE_FORCED_FAILURES
  // (BUG) See more details below for the same problem of MED_MESH_RDONLY_DRIVER
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);
//#endif

  MED_MED_WRONLY_DRIVER * aMedWDriverForMed = dynamic_cast<MED_MED_WRONLY_DRIVER *> (aDriver);
  CPPUNIT_ASSERT(aMedWDriverForMed);

  delete aDriver;

  // rdwr
  aDriver = DRIVERFACTORY::buildDriverForMed(MED_DRIVER, "anyfile", &med, MED_EN::RDWR);
//#ifdef ENABLE_FORCED_FAILURES
  // (BUG) See more details below for the same problem of MED_MESH_RDONLY_DRIVER
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR);
//#endif

  MED_MED_RDWR_DRIVER * aMedRWDriverForMed = dynamic_cast<MED_MED_RDWR_DRIVER *> (aDriver);
  CPPUNIT_ASSERT(aMedRWDriverForMed);

  delete aDriver;

  // 3.2: GIBI_DRIVER

  // rdonly
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForMed(GIBI_DRIVER, "anyfile", &med, MED_EN::RDONLY),
                       MED_EXCEPTION);
  // wronly
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForMed(GIBI_DRIVER, "anyfile", &med, MED_EN::WRONLY),
                       MED_EXCEPTION);
  // rdwr
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForMed(GIBI_DRIVER, "anyfile", &med, MED_EN::RDWR),
                       MED_EXCEPTION);

  // 3.3: PORFLOW_DRIVER

  // rdonly
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForMed(PORFLOW_DRIVER, "anyfile", &med, MED_EN::RDONLY),
                       MED_EXCEPTION);
  // wronly
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForMed(PORFLOW_DRIVER, "anyfile", &med, MED_EN::WRONLY),
                       MED_EXCEPTION);
  // rdwr
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForMed(PORFLOW_DRIVER, "anyfile", &med, MED_EN::RDWR),
                       MED_EXCEPTION);

  // 3.4: VTK_DRIVER

  // rdonly
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForMed(VTK_DRIVER, "anyfile", &med, MED_EN::RDONLY),
                       MED_EXCEPTION);

  // wronly
  aDriver = DRIVERFACTORY::buildDriverForMed(VTK_DRIVER, "anyfile", &med, MED_EN::WRONLY);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  // next string is commented by skl since VTK driver is
  // created without mode
  //CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

  VTK_MED_DRIVER * aVtkDriverForMed = dynamic_cast<VTK_MED_DRIVER *> (aDriver);
  CPPUNIT_ASSERT(aVtkDriverForMed);

  delete aDriver;

  // rdwr
  aDriver = DRIVERFACTORY::buildDriverForMed(VTK_DRIVER, "anyfile", &med, MED_EN::RDWR);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR);

  aVtkDriverForMed = dynamic_cast<VTK_MED_DRIVER *> (aDriver);
  CPPUNIT_ASSERT(aVtkDriverForMed);

  delete aDriver;

  // 3.5: ASCII_DRIVER
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForMed(ASCII_DRIVER, "anyfile", &med, MED_EN::RDONLY),
                       MED_EXCEPTION);

  // 3.6: NO_DRIVER
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForMed(NO_DRIVER, "anyfile", &med, MED_EN::RDONLY),
                       MED_EXCEPTION);

  ////////////////////////////////
  // Test 4: buildDriverForMesh //
  ////////////////////////////////
  MESH mesh;

  // 4.1: MED_DRIVER

  // rdonly
  aDriver = DRIVERFACTORY::buildDriverForMesh
    (MED_DRIVER, "anyfile", &mesh, "my driver name", MED_EN::RDONLY);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);


  MED_MESH_RDONLY_DRIVER * aMedRDriverForMesh = dynamic_cast<MED_MESH_RDONLY_DRIVER *> (aDriver);
  CPPUNIT_ASSERT(aMedRDriverForMesh);
  CPPUNIT_ASSERT(aMedRDriverForMesh->getMeshName() == "my driver name");


  delete aDriver;

  // wronly
  aDriver = DRIVERFACTORY::buildDriverForMesh
    (MED_DRIVER, "anyfile", &mesh, "my driver name", MED_EN::WRONLY);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

  MED_MESH_WRONLY_DRIVER * aMedWDriverForMesh = dynamic_cast<MED_MESH_WRONLY_DRIVER *> (aDriver);
  CPPUNIT_ASSERT(aMedWDriverForMesh);
  CPPUNIT_ASSERT(aMedWDriverForMesh->getMeshName() == "my driver name");

  delete aDriver;

  // rdwr
  aDriver = DRIVERFACTORY::buildDriverForMesh
    (MED_DRIVER, "anyfile", &mesh, "my driver name", MED_EN::RDWR);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR);

  MED_MESH_RDWR_DRIVER * aMedRWDriverForMesh = dynamic_cast<MED_MESH_RDWR_DRIVER *> (aDriver);
  CPPUNIT_ASSERT(aMedRWDriverForMesh);
  CPPUNIT_ASSERT(aMedRWDriverForMesh->getMeshName() == "my driver name");

  delete aDriver;

  // 4.2: GIBI_DRIVER

  // rdonly
  aDriver = DRIVERFACTORY::buildDriverForMesh
    (GIBI_DRIVER, "anyfile", &mesh, "my driver name", MED_EN::RDONLY);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);

  GIBI_MESH_RDONLY_DRIVER * aGibiDriverForMesh = dynamic_cast<GIBI_MESH_RDONLY_DRIVER *> (aDriver);
  CPPUNIT_ASSERT(aGibiDriverForMesh);

  delete aDriver;

  // wronly
  // 0020412: [CEA 343] GIBI Driver is not usable using MESH::addDriver
//   CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForMesh(GIBI_DRIVER, "anyfile", &mesh,
//                                                          "my driver name", MED_EN::WRONLY),
//                        MED_EXCEPTION);
  aDriver = DRIVERFACTORY::buildDriverForMesh
    (GIBI_DRIVER, "anyfile", &mesh, "my driver name", MED_EN::WRONLY);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

  GIBI_MESH_WRONLY_DRIVER * aGibiWRDriverForMesh = dynamic_cast<GIBI_MESH_WRONLY_DRIVER*>(aDriver);
  CPPUNIT_ASSERT(aGibiWRDriverForMesh);
  delete aDriver;

  // rdwr
  // 0020412: [CEA 343] GIBI Driver is not usable using MESH::addDriver
//   CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForMesh(GIBI_DRIVER, "anyfile", &mesh,
//                                                          "my driver name", MED_EN::RDWR),
//                        MED_EXCEPTION);
  aDriver = DRIVERFACTORY::buildDriverForMesh
    (GIBI_DRIVER, "anyfile", &mesh, "my driver name", MED_EN::RDWR);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR);

  GIBI_MESH_RDWR_DRIVER * aGibiRWDriverForMesh = dynamic_cast<GIBI_MESH_RDWR_DRIVER*>(aDriver);
  CPPUNIT_ASSERT(aGibiRWDriverForMesh);
  delete aDriver;

  // 4.3: PORFLOW_DRIVER

  // rdonly
  aDriver = DRIVERFACTORY::buildDriverForMesh
    (PORFLOW_DRIVER, "anyfile", &mesh, "my driver name", MED_EN::RDONLY);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);

  PORFLOW_MESH_RDONLY_DRIVER * aPorflowDriverForMesh = dynamic_cast<PORFLOW_MESH_RDONLY_DRIVER *> (aDriver);
  CPPUNIT_ASSERT(aPorflowDriverForMesh);

  delete aDriver;

  // wronly
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForMesh(PORFLOW_DRIVER, "anyfile", &mesh,
                                                         "my driver name", MED_EN::WRONLY),
                       MED_EXCEPTION);
  // rdwr
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForMesh(PORFLOW_DRIVER, "anyfile", &mesh,
                                                         "my driver name", MED_EN::RDWR),
                       MED_EXCEPTION);

  // 4.4: VTK_DRIVER

  // rdonly
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForMesh(VTK_DRIVER, "anyfile", &mesh,
                                                         "my driver name", MED_EN::RDONLY),
                       MED_EXCEPTION);

  // wronly
  aDriver = DRIVERFACTORY::buildDriverForMesh
    (VTK_DRIVER, "anyfile", &mesh, "my driver name", MED_EN::WRONLY);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

  VTK_MESH_DRIVER * aVtkDriverForMesh = dynamic_cast<VTK_MESH_DRIVER *> (aDriver);
  CPPUNIT_ASSERT(aVtkDriverForMesh);

  delete aDriver;

  // rdwr
  aDriver = DRIVERFACTORY::buildDriverForMesh
    (VTK_DRIVER, "anyfile", &mesh, "my driver name", MED_EN::RDWR);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  // next string is commented by skl since VTK driver is 
  // created without mode
  //CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR);

  aVtkDriverForMesh = dynamic_cast<VTK_MESH_DRIVER *> (aDriver);
  CPPUNIT_ASSERT(aVtkDriverForMesh);

  delete aDriver;

  // 4.5: ASCII_DRIVER
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForMesh(ASCII_DRIVER, "anyfile", &mesh,
                                                         "my driver name", MED_EN::RDONLY),
                       MED_EXCEPTION);

  // 4.6: NO_DRIVER
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForMesh(NO_DRIVER, "anyfile", &mesh,
                                                         "my driver name", MED_EN::RDONLY),
                       MED_EXCEPTION);

  /////////////////////////////////
  // Test 5: buildDriverForField //
  /////////////////////////////////
  FIELD<double> field;

  // 5.1: MED_DRIVER

  // rdonly
  aDriver = DRIVERFACTORY::buildDriverForField(MED_DRIVER, "anyfile", &field, MED_EN::RDONLY);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);

  MED_FIELD_RDONLY_DRIVER<double> * aMedRDriverForField =
    dynamic_cast<MED_FIELD_RDONLY_DRIVER<double> *> (aDriver);
  CPPUNIT_ASSERT(aMedRDriverForField);

  delete aDriver;

  // wronly
  aDriver = DRIVERFACTORY::buildDriverForField(MED_DRIVER, "anyfile", &field, MED_EN::WRONLY);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

  MED_FIELD_WRONLY_DRIVER<double> * aMedWDriverForField =
    dynamic_cast<MED_FIELD_WRONLY_DRIVER<double> *> (aDriver);
  CPPUNIT_ASSERT(aMedWDriverForField);

  delete aDriver;

  // rdwr
  aDriver = DRIVERFACTORY::buildDriverForField(MED_DRIVER, "anyfile", &field, MED_EN::RDWR);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
//#ifdef ENABLE_FORCED_FAILURES
  // (BUG) Wrong access mode (WRONLY) passed to parent class!
        // confusion between MED_DRWR (defined as WRONLY in MEDMEM_define.hxx)
        // and RDWR causes confusion in MEDMEM_MedFieldDriver22.hxx
  CPPUNIT_ASSERT_EQUAL(MED_EN::RDWR,aDriver->getAccessMode());
//#endif

  MED_FIELD_RDWR_DRIVER<double> * aMedRWDriverForField =
    dynamic_cast<MED_FIELD_RDWR_DRIVER<double> *> (aDriver);
  CPPUNIT_ASSERT(aMedRWDriverForField);

  delete aDriver;

  // 5.2: GIBI_DRIVER

  // rdonly
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForField(GIBI_DRIVER, "anyfile", &field, MED_EN::RDONLY),
                       MED_EXCEPTION);
  // wronly
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForField(GIBI_DRIVER, "anyfile", &field, MED_EN::WRONLY),
                       MED_EXCEPTION);
  // rdwr
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForField(GIBI_DRIVER, "anyfile", &field, MED_EN::RDWR),
                       MED_EXCEPTION);

  // 5.3: PORFLOW_DRIVER

  // rdonly
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForField(PORFLOW_DRIVER, "anyfile",
                                                          &field, MED_EN::RDONLY),
                       MED_EXCEPTION);
  // wronly
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForField(PORFLOW_DRIVER, "anyfile",
                                                          &field, MED_EN::WRONLY),
                       MED_EXCEPTION);
  // rdwr
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForField(PORFLOW_DRIVER, "anyfile",
                                                          &field, MED_EN::RDWR),
                       MED_EXCEPTION);

  // 5.4: VTK_DRIVER

  // rdonly
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForField(VTK_DRIVER, "anyfile", &field, MED_EN::RDONLY),
                       MED_EXCEPTION);

  // wronly
  aDriver = DRIVERFACTORY::buildDriverForField(VTK_DRIVER, "anyfile", &field, MED_EN::WRONLY);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

  VTK_FIELD_DRIVER<double> * aVtkDriverForField = dynamic_cast<VTK_FIELD_DRIVER<double> *> (aDriver);
  CPPUNIT_ASSERT(aVtkDriverForField);

  delete aDriver;

  // rdwr
  aDriver = DRIVERFACTORY::buildDriverForField(VTK_DRIVER, "anyfile", &field, MED_EN::RDWR);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  // next string is commented by skl since VTK driver is 
  // created without mode
  //CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR);

  aVtkDriverForField = dynamic_cast<VTK_FIELD_DRIVER<double> *> (aDriver);
  CPPUNIT_ASSERT(aVtkDriverForField);

  delete aDriver;

  // 5.5: ASCII_DRIVER

  // rdonly
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForField(ASCII_DRIVER, "anyfile", &field, MED_EN::RDONLY),
                       MED_EXCEPTION);
  // rdwr
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForField(ASCII_DRIVER, "anyfile", &field, MED_EN::RDWR),
                       MED_EXCEPTION);

  // wronly
  // it throws on empty field
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForField(ASCII_DRIVER, "anyfile", &field, MED_EN::WRONLY),
                       MED_EXCEPTION);

  // now try with valid field
  string filename  = getResourceFile("pointe.med");
  string meshname  = "maa1";
  string fieldname = "fieldcelldoublescalar";
  FIELD<double> * aField1 = new FIELD<double> (MED_DRIVER, filename, fieldname);
  const SUPPORT * aSupport = aField1->getSupport();
  MESH * aMesh = new MESH (MED_DRIVER, filename, aSupport->getMeshName());
  aSupport->setMesh(aMesh);

  aDriver = DRIVERFACTORY::buildDriverForField(ASCII_DRIVER, "anyfile", aField1, MED_EN::WRONLY);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

  // it works!
  //aDriver->open();
  //aDriver->write();
  //aDriver->close();

  ASCII_FIELD_DRIVER<double> * anAsciiDriverForField =
    dynamic_cast<ASCII_FIELD_DRIVER<double> *> (aDriver);
  CPPUNIT_ASSERT(anAsciiDriverForField);

  delete aDriver;

  delete aField1;
  delete aMesh;

  // 5.6: NO_DRIVER
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForField(NO_DRIVER, "anyfile", &field, MED_EN::RDONLY),
                       MED_EXCEPTION);

  ////////////////////////////////////
  // Test 6: buildMedDriverFromFile //
  ////////////////////////////////////
  string aFileName1 = getResourceFile("pointe.med");
  string aFileName2 = getResourceFile("polyedres.med");
  medFileVersion version1, version2;

  try {
    version1 = getMedFileVersion(aFileName1);
  } catch (MEDEXCEPTION & ex) {
    version1 = DRIVERFACTORY::globalMedFileVersionForWriting;
  }

  try {
    version2 = getMedFileVersion(aFileName2);
  } catch (MEDEXCEPTION & ex) {
    version2 = DRIVERFACTORY::globalMedFileVersionForWriting;
  }

  // 6.1. Med file V2.1
  string aFileName21 = aFileName1;
  if (version1 == MED_EN::V21 || version2 == MED_EN::V21) {
    if (version1 == MED_EN::V22)
      aFileName21 = aFileName2;

    // rdonly
    aDriver = DRIVERFACTORY::buildMedDriverFromFile(aFileName21, &med, MED_EN::RDONLY);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName21);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);

    MED_MED_RDONLY_DRIVER21 * aMedRDriverForMed21 = dynamic_cast<MED_MED_RDONLY_DRIVER21 *> (aDriver);
    CPPUNIT_ASSERT(aMedRDriverForMed21);

    delete aDriver;

    // wronly
    aDriver = DRIVERFACTORY::buildMedDriverFromFile(aFileName21, &med, MED_EN::WRONLY);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName21);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

    MED_MED_WRONLY_DRIVER21 * aMedWDriverForMed21 = dynamic_cast<MED_MED_WRONLY_DRIVER21 *> (aDriver);
    CPPUNIT_ASSERT(aMedWDriverForMed21);

    delete aDriver;

    // rdwr
    aDriver = DRIVERFACTORY::buildMedDriverFromFile(aFileName21, &med, MED_EN::RDWR);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName21);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR);

    MED_MED_RDWR_DRIVER21 * aMedRWDriverForMed21 = dynamic_cast<MED_MED_RDWR_DRIVER21 *> (aDriver);
    CPPUNIT_ASSERT(aMedRWDriverForMed21);

    delete aDriver;
  }

  // 6.2. Med file V2.2
  string aFileName22 = aFileName2;
  if (version2 == MED_EN::V22 || version1 == MED_EN::V22) {
    if (version2 == MED_EN::V21)
      aFileName22 = aFileName1;

    // rdonly
    aDriver = DRIVERFACTORY::buildMedDriverFromFile(aFileName22, &med, MED_EN::RDONLY);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName22);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);

    MED_MED_RDONLY_DRIVER22 * aMedRDriverForMed22 = dynamic_cast<MED_MED_RDONLY_DRIVER22 *> (aDriver);
    CPPUNIT_ASSERT(aMedRDriverForMed22);

    delete aDriver;

    // wronly
    aDriver = DRIVERFACTORY::buildMedDriverFromFile(aFileName22, &med, MED_EN::WRONLY);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName22);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

    MED_MED_WRONLY_DRIVER22 * aMedWDriverForMed22 = dynamic_cast<MED_MED_WRONLY_DRIVER22 *> (aDriver);
    CPPUNIT_ASSERT(aMedWDriverForMed22);

    delete aDriver;

    // rdwr
    aDriver = DRIVERFACTORY::buildMedDriverFromFile(aFileName22, &med, MED_EN::RDWR);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName22);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR);

    MED_MED_RDWR_DRIVER22 * aMedRWDriverForMed22 = dynamic_cast<MED_MED_RDWR_DRIVER22 *> (aDriver);
    CPPUNIT_ASSERT(aMedRWDriverForMed22);

    delete aDriver;
  }
  else {
    CPPUNIT_FAIL("Cannot check building of drivers from file for V2.2");
  }
  if (version1 != MED_EN::V21 && version2 != MED_EN::V21) {
    CPPUNIT_FAIL("Cannot check building of drivers from file for V2.1");
  }

  /////////////////////////////////////
  // Test 7: buildMeshDriverFromFile //
  /////////////////////////////////////

  // 7.1. Med file V2.1
  if (version1 == MED_EN::V21 || version2 == MED_EN::V21) {
    // rdonly
    aDriver = DRIVERFACTORY::buildMeshDriverFromFile(aFileName21, &mesh, MED_EN::RDONLY);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName21);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);

    MED_MESH_RDONLY_DRIVER21 * aMeshRDriverForMed21 = dynamic_cast<MED_MESH_RDONLY_DRIVER21 *> (aDriver);
    CPPUNIT_ASSERT(aMeshRDriverForMed21);

    delete aDriver;

    // wronly
    aDriver = DRIVERFACTORY::buildMeshDriverFromFile(aFileName21, &mesh, MED_EN::WRONLY);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName21);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

    MED_MESH_WRONLY_DRIVER21 * aMeshWDriverForMed21 = dynamic_cast<MED_MESH_WRONLY_DRIVER21 *> (aDriver);
    CPPUNIT_ASSERT(aMeshWDriverForMed21);

    delete aDriver;

    // rdwr
    aDriver = DRIVERFACTORY::buildMeshDriverFromFile(aFileName21, &mesh, MED_EN::RDWR);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName21);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR);

    MED_MESH_RDWR_DRIVER21 * aMeshRWDriverForMed21 = dynamic_cast<MED_MESH_RDWR_DRIVER21 *> (aDriver);
    CPPUNIT_ASSERT(aMeshRWDriverForMed21);

    delete aDriver;
  }

  // 7.2. Med file V2.2
  if (version2 == MED_EN::V22 || version1 == MED_EN::V22) {
    // rdonly
    aDriver = DRIVERFACTORY::buildMeshDriverFromFile(aFileName22, &mesh, MED_EN::RDONLY);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName22);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);

    MED_MESH_RDONLY_DRIVER22 * aMeshRDriverForMed22 = dynamic_cast<MED_MESH_RDONLY_DRIVER22 *> (aDriver);
    CPPUNIT_ASSERT(aMeshRDriverForMed22);

    delete aDriver;

    // wronly
    aDriver = DRIVERFACTORY::buildMeshDriverFromFile(aFileName22, &mesh, MED_EN::WRONLY);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName22);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

    MED_MESH_WRONLY_DRIVER22 * aMeshWDriverForMed22 = dynamic_cast<MED_MESH_WRONLY_DRIVER22 *> (aDriver);
    CPPUNIT_ASSERT(aMeshWDriverForMed22);

    delete aDriver;

    // rdwr
    aDriver = DRIVERFACTORY::buildMeshDriverFromFile(aFileName22, &mesh, MED_EN::RDWR);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName22);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR);

    MED_MESH_RDWR_DRIVER22 * aMeshRWDriverForMed22 = dynamic_cast<MED_MESH_RDWR_DRIVER22 *> (aDriver);
    CPPUNIT_ASSERT(aMeshRWDriverForMed22);

    delete aDriver;
  }

  //////////////////////////////////////
  // Test 8: buildFieldDriverFromFile //
  //////////////////////////////////////

  // 8.1. Med file V2.1
  if (version1 == MED_EN::V21 || version2 == MED_EN::V21) {
    // rdonly
    aDriver = DRIVERFACTORY::buildFieldDriverFromFile(aFileName21, &field, MED_EN::RDONLY);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName21);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);

    MED_FIELD_RDONLY_DRIVER21<double> * aFieldRDriverForMed21 =
      dynamic_cast<MED_FIELD_RDONLY_DRIVER21<double> *> (aDriver);
    CPPUNIT_ASSERT(aFieldRDriverForMed21);

    delete aDriver;

    // wronly
    aDriver = DRIVERFACTORY::buildFieldDriverFromFile(aFileName21, &field, MED_EN::WRONLY);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName21);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

    MED_FIELD_WRONLY_DRIVER21<double> * aFieldWDriverForMed21 =
      dynamic_cast<MED_FIELD_WRONLY_DRIVER21<double> *> (aDriver);
    CPPUNIT_ASSERT(aFieldWDriverForMed21);

    delete aDriver;

    // rdwr
    aDriver = DRIVERFACTORY::buildFieldDriverFromFile(aFileName21, &field, MED_EN::RDWR);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName21);
    //CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR); // see MEDMEM_define.hxx

    MED_FIELD_RDWR_DRIVER21<double> * aFieldRWDriverForMed21 =
      dynamic_cast<MED_FIELD_RDWR_DRIVER21<double> *> (aDriver);
    CPPUNIT_ASSERT(aFieldRWDriverForMed21);

    delete aDriver;
  }

  // 8.2. Med file V2.2
  if (version2 == MED_EN::V22 || version1 == MED_EN::V22) {
    // rdonly
    aDriver = DRIVERFACTORY::buildFieldDriverFromFile(aFileName22, &field, MED_EN::RDONLY);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName22);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);

    MED_FIELD_RDONLY_DRIVER22<double> * aFieldRDriverForMed22 =
      dynamic_cast<MED_FIELD_RDONLY_DRIVER22<double> *> (aDriver);
    CPPUNIT_ASSERT(aFieldRDriverForMed22);

    delete aDriver;

    // wronly
    aDriver = DRIVERFACTORY::buildFieldDriverFromFile(aFileName22, &field, MED_EN::WRONLY);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName22);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

    MED_FIELD_WRONLY_DRIVER22<double> * aFieldWDriverForMed22 =
      dynamic_cast<MED_FIELD_WRONLY_DRIVER22<double> *> (aDriver);
    CPPUNIT_ASSERT(aFieldWDriverForMed22);

    delete aDriver;

    // rdwr
    aDriver = DRIVERFACTORY::buildFieldDriverFromFile(aFileName22, &field, MED_EN::RDWR);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName22);
    //CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR); // see MEDMEM_define.hxx

    MED_FIELD_RDWR_DRIVER22<double> * aFieldRWDriverForMed22 =
      dynamic_cast<MED_FIELD_RDWR_DRIVER22<double> *> (aDriver);
    CPPUNIT_ASSERT(aFieldRWDriverForMed22);

    delete aDriver;
  }

  ///////////////////////////////////////////
  // Test 9: buildConcreteMedDriverForMesh //
  ///////////////////////////////////////////

  // 9.1. V2.1

  // rdonly
  aDriver = DRIVERFACTORY::buildConcreteMedDriverForMesh("anyfile", &mesh, "my driver name",
                                                         MED_EN::RDONLY, MED_EN::V21);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);

  MED_MESH_RDONLY_DRIVER21 * aMeshRDriverForMed21 = dynamic_cast<MED_MESH_RDONLY_DRIVER21 *> (aDriver);
  CPPUNIT_ASSERT(aMeshRDriverForMed21);
  CPPUNIT_ASSERT(aMeshRDriverForMed21->getMeshName() == "my driver name");

  delete aDriver;

  // wronly
  aDriver = DRIVERFACTORY::buildConcreteMedDriverForMesh("anyfile", &mesh, "my driver name",
                                                         MED_EN::WRONLY, MED_EN::V21);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

  MED_MESH_WRONLY_DRIVER21 * aMeshWDriverForMed21 = dynamic_cast<MED_MESH_WRONLY_DRIVER21 *> (aDriver);
  CPPUNIT_ASSERT(aMeshWDriverForMed21);
  CPPUNIT_ASSERT(aMeshWDriverForMed21->getMeshName() == "my driver name");

  delete aDriver;

  // rdwr
  aDriver = DRIVERFACTORY::buildConcreteMedDriverForMesh("anyfile", &mesh, "my driver name",
                                                         MED_EN::RDWR, MED_EN::V21);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR);

  MED_MESH_RDWR_DRIVER21 * aMeshRWDriverForMed21 = dynamic_cast<MED_MESH_RDWR_DRIVER21 *> (aDriver);
  CPPUNIT_ASSERT(aMeshRWDriverForMed21);
  CPPUNIT_ASSERT(aMeshRWDriverForMed21->getMeshName() == "my driver name");

  delete aDriver;

  // 9.2. V2.2

  // rdonly
  aDriver = DRIVERFACTORY::buildConcreteMedDriverForMesh("anyfile", &mesh, "my driver name",
                                                         MED_EN::RDONLY, MED_EN::V22);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);

  MED_MESH_RDONLY_DRIVER22 * aMeshRDriverForMed22 = dynamic_cast<MED_MESH_RDONLY_DRIVER22 *> (aDriver);
  CPPUNIT_ASSERT(aMeshRDriverForMed22);
  CPPUNIT_ASSERT(aMeshRDriverForMed22->getMeshName() == "my driver name");

  delete aDriver;

  // wronly
  aDriver = DRIVERFACTORY::buildConcreteMedDriverForMesh("anyfile", &mesh, "my driver name",
                                                         MED_EN::WRONLY, MED_EN::V22);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

  MED_MESH_WRONLY_DRIVER22 * aMeshWDriverForMed22 = dynamic_cast<MED_MESH_WRONLY_DRIVER22 *> (aDriver);
  CPPUNIT_ASSERT(aMeshWDriverForMed22);
  CPPUNIT_ASSERT(aMeshWDriverForMed22->getMeshName() == "my driver name");

  delete aDriver;

  // rdwr
  aDriver = DRIVERFACTORY::buildConcreteMedDriverForMesh("anyfile", &mesh, "my driver name",
                                                         MED_EN::RDWR, MED_EN::V22);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR);

  MED_MESH_RDWR_DRIVER22 * aMeshRWDriverForMed22 = dynamic_cast<MED_MESH_RDWR_DRIVER22 *> (aDriver);
  CPPUNIT_ASSERT(aMeshRWDriverForMed22);
  CPPUNIT_ASSERT(aMeshRWDriverForMed22->getMeshName() == "my driver name");

  delete aDriver;

  /////////////////////////////////////////////
  // Test 10: buildConcreteMedDriverForField //
  /////////////////////////////////////////////

  // 10.1. V2.1

  // rdonly
  aDriver = DRIVERFACTORY::buildConcreteMedDriverForField("anyfile", &field, MED_EN::RDONLY, MED_EN::V21);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);

  MED_FIELD_RDONLY_DRIVER21<double> * aFieldRDriverForMed21 =
    dynamic_cast<MED_FIELD_RDONLY_DRIVER21<double> *> (aDriver);
  CPPUNIT_ASSERT(aFieldRDriverForMed21);

  delete aDriver;

  // wronly
  aDriver = DRIVERFACTORY::buildConcreteMedDriverForField("anyfile", &field, MED_EN::WRONLY, MED_EN::V21);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

  MED_FIELD_WRONLY_DRIVER21<double> * aFieldWDriverForMed21 =
    dynamic_cast<MED_FIELD_WRONLY_DRIVER21<double> *> (aDriver);
  CPPUNIT_ASSERT(aFieldWDriverForMed21);

  delete aDriver;

  // rdwr
  aDriver = DRIVERFACTORY::buildConcreteMedDriverForField("anyfile", &field, MED_EN::RDWR, MED_EN::V21);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  //CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR);
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR); // see MEDMEM_define.hxx

  MED_FIELD_RDWR_DRIVER21<double> * aFieldRWDriverForMed21 =
    dynamic_cast<MED_FIELD_RDWR_DRIVER21<double> *> (aDriver);
  CPPUNIT_ASSERT(aFieldRWDriverForMed21);

  delete aDriver;

  // 10.2. V2.2

  // rdonly
  aDriver = DRIVERFACTORY::buildConcreteMedDriverForField("anyfile", &field, MED_EN::RDONLY, MED_EN::V22);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);

  MED_FIELD_RDONLY_DRIVER22<double> * aFieldRDriverForMed22 =
    dynamic_cast<MED_FIELD_RDONLY_DRIVER22<double> *> (aDriver);
  CPPUNIT_ASSERT(aFieldRDriverForMed22);

  delete aDriver;

  // wronly
  aDriver = DRIVERFACTORY::buildConcreteMedDriverForField("anyfile", &field, MED_EN::WRONLY, MED_EN::V22);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

  MED_FIELD_WRONLY_DRIVER22<double> * aFieldWDriverForMed22 =
    dynamic_cast<MED_FIELD_WRONLY_DRIVER22<double> *> (aDriver);
  CPPUNIT_ASSERT(aFieldWDriverForMed22);

  delete aDriver;

  // rdwr
  aDriver = DRIVERFACTORY::buildConcreteMedDriverForField("anyfile", &field, MED_EN::RDWR, MED_EN::V22);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  //CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR);
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR); // see MEDMEM_define.hxx

  MED_FIELD_RDWR_DRIVER22<double> * aFieldRWDriverForMed22 =
    dynamic_cast<MED_FIELD_RDWR_DRIVER22<double> *> (aDriver);
  CPPUNIT_ASSERT(aFieldRWDriverForMed22);

  delete aDriver;
}
