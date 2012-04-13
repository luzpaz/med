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
#include <cppunit/Message.h>
#include <cppunit/TestAssert.h>

#include "MEDMEM_DriverFactory.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_GibiMeshDriver.hxx"
#include "MEDMEM_MedFieldDriver.hxx"
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
 *  Check methods (7), defined in MEDMEM_DriverFactory.hxx:
 *  namespace DRIVERFACTORY {
 *   (+)     driverTypes deduceDriverTypeFromFileName(const std::string & fileName);
 *
 *   (+)     GENDRIVER * buildDriverForMesh (driverTypes driverType, const std::string & fileName,
 *                                           MESH *mesh, const string & driverName,
 *                                           MED_EN::med_mode_acces access);
 *   (+)     template<class T, class INTERLACING_TAG>
 *           GENDRIVER * buildDriverForField(driverTypes driverType, const std::string & fileName,
 *                                           FIELD<T,INTERLACING_TAG> *fielde,
 *                                           MED_EN::med_mode_acces access);
 *
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

  // Obsolete, removed

  //////////////////////////////////////////////////////////////////////////
  // Test 1b: getVtkBinaryFormatForWriting & setVtkBinaryFormatForWriting //
  //////////////////////////////////////////////////////////////////////////

  // save current format
  bool wasBinary = DRIVERFACTORY::getVtkBinaryFormatForWriting();

  // check changing format
  DRIVERFACTORY::setVtkBinaryFormatForWriting( !wasBinary );
  CPPUNIT_ASSERT(DRIVERFACTORY::getVtkBinaryFormatForWriting() == !wasBinary);

  // restore default version preference
  DRIVERFACTORY::setVtkBinaryFormatForWriting(wasBinary);

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

  // Obsolete, removed

  ////////////////////////////////
  // Test 4: buildDriverForMesh //
  ////////////////////////////////
  MESH *mesh=new MESH;

  // 4.1: MED_DRIVER

  // rdonly
  aDriver = DRIVERFACTORY::buildDriverForMesh
    (MED_DRIVER, "anyfile", mesh, "my driver name", MED_EN::RDONLY);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);


  MED_MESH_RDONLY_DRIVER * aMedRDriverForMesh = dynamic_cast<MED_MESH_RDONLY_DRIVER *> (aDriver);
  CPPUNIT_ASSERT(aMedRDriverForMesh);
  CPPUNIT_ASSERT(aMedRDriverForMesh->getMeshName() == "my driver name");


  delete aDriver; aDriver=0;

  // wronly
  aDriver = DRIVERFACTORY::buildDriverForMesh
    (MED_DRIVER, "anyfile", mesh, "my driver name", MED_EN::WRONLY);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

  MED_MESH_WRONLY_DRIVER * aMedWDriverForMesh = dynamic_cast<MED_MESH_WRONLY_DRIVER *> (aDriver);
  CPPUNIT_ASSERT(aMedWDriverForMesh);
  CPPUNIT_ASSERT(aMedWDriverForMesh->getMeshName() == "my driver name");

  delete aDriver; aDriver=0;

  // rdwr
  aDriver = DRIVERFACTORY::buildDriverForMesh
    (MED_DRIVER, "anyfile", mesh, "my driver name", MED_EN::RDWR);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR);

  MED_MESH_RDWR_DRIVER * aMedRWDriverForMesh = dynamic_cast<MED_MESH_RDWR_DRIVER *> (aDriver);
  CPPUNIT_ASSERT(aMedRWDriverForMesh);
  CPPUNIT_ASSERT(aMedRWDriverForMesh->getMeshName() == "my driver name");

  delete aDriver; aDriver=0;

  // 4.2: GIBI_DRIVER

  // rdonly
  aDriver = DRIVERFACTORY::buildDriverForMesh
    (GIBI_DRIVER, "anyfile", mesh, "my driver name", MED_EN::RDONLY);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);

  GIBI_MESH_RDONLY_DRIVER * aGibiDriverForMesh = dynamic_cast<GIBI_MESH_RDONLY_DRIVER *> (aDriver);
  CPPUNIT_ASSERT(aGibiDriverForMesh);

  delete aDriver; aDriver=0;

  // wronly
  aDriver = DRIVERFACTORY::buildDriverForMesh
    (GIBI_DRIVER, "anyfile", mesh, "my driver name", MED_EN::WRONLY);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

  GIBI_MESH_WRONLY_DRIVER * aGibiWRDriverForMesh = dynamic_cast<GIBI_MESH_WRONLY_DRIVER*>(aDriver);
  CPPUNIT_ASSERT(aGibiWRDriverForMesh);
  delete aDriver; aDriver=0;

  // rdwr
  aDriver = DRIVERFACTORY::buildDriverForMesh
    (GIBI_DRIVER, "anyfile", mesh, "my driver name", MED_EN::RDWR);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR);

  GIBI_MESH_RDWR_DRIVER * aGibiRWDriverForMesh = dynamic_cast<GIBI_MESH_RDWR_DRIVER*>(aDriver);
  CPPUNIT_ASSERT(aGibiRWDriverForMesh);
  delete aDriver; aDriver=0;

  // 4.3: PORFLOW_DRIVER

  // rdonly
  aDriver = DRIVERFACTORY::buildDriverForMesh
    (PORFLOW_DRIVER, "anyfile", mesh, "my driver name", MED_EN::RDONLY);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);

  PORFLOW_MESH_RDONLY_DRIVER * aPorflowDriverForMesh = dynamic_cast<PORFLOW_MESH_RDONLY_DRIVER *> (aDriver);
  CPPUNIT_ASSERT(aPorflowDriverForMesh);

  delete aDriver; aDriver=0;

  // wronly
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForMesh(PORFLOW_DRIVER, "anyfile", mesh,
                                                         "my driver name", MED_EN::WRONLY),
                       MED_EXCEPTION);
  // rdwr
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForMesh(PORFLOW_DRIVER, "anyfile", mesh,
                                                         "my driver name", MED_EN::RDWR),
                       MED_EXCEPTION);

  // 4.4: VTK_DRIVER

  // rdonly
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForMesh(VTK_DRIVER, "anyfile", mesh,
                                                         "my driver name", MED_EN::RDONLY),
                       MED_EXCEPTION);

  // wronly
  aDriver = DRIVERFACTORY::buildDriverForMesh
    (VTK_DRIVER, "anyfile", mesh, "my driver name", MED_EN::WRONLY);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

  VTK_MESH_DRIVER * aVtkDriverForMesh = dynamic_cast<VTK_MESH_DRIVER *> (aDriver);
  CPPUNIT_ASSERT(aVtkDriverForMesh);

  delete aDriver; aDriver=0;

  // rdwr
  aDriver = DRIVERFACTORY::buildDriverForMesh
    (VTK_DRIVER, "anyfile", mesh, "my driver name", MED_EN::RDWR);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  // next string is commented by skl since VTK driver is 
  // created without mode
  //CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR);

  aVtkDriverForMesh = dynamic_cast<VTK_MESH_DRIVER *> (aDriver);
  CPPUNIT_ASSERT(aVtkDriverForMesh);

  delete aDriver; aDriver=0;

  // 4.5: ASCII_DRIVER
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForMesh(ASCII_DRIVER, "anyfile", mesh,
                                                         "my driver name", MED_EN::RDONLY),
                       MED_EXCEPTION);

  // 4.6: NO_DRIVER
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForMesh(NO_DRIVER, "anyfile", mesh,
                                                         "my driver name", MED_EN::RDONLY),
                       MED_EXCEPTION);

  /////////////////////////////////
  // Test 5: buildDriverForField //
  /////////////////////////////////
  FIELD<double> *field=new FIELD<double>();

  // 5.1: MED_DRIVER

  // rdonly
  aDriver = DRIVERFACTORY::buildDriverForField(MED_DRIVER, "anyfile", field, MED_EN::RDONLY);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);

  MED_FIELD_RDONLY_DRIVER<double> * aMedRDriverForField =
    dynamic_cast<MED_FIELD_RDONLY_DRIVER<double> *> (aDriver);
  CPPUNIT_ASSERT(aMedRDriverForField);

  delete aDriver; aDriver=0;

  // wronly
  aDriver = DRIVERFACTORY::buildDriverForField(MED_DRIVER, "anyfile", field, MED_EN::WRONLY);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

  MED_FIELD_WRONLY_DRIVER<double> * aMedWDriverForField =
    dynamic_cast<MED_FIELD_WRONLY_DRIVER<double> *> (aDriver);
  CPPUNIT_ASSERT(aMedWDriverForField);

  delete aDriver; aDriver=0;

  // rdwr
  aDriver = DRIVERFACTORY::buildDriverForField(MED_DRIVER, "anyfile", field, MED_EN::RDWR);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT_EQUAL(MED_EN::RDWR,aDriver->getAccessMode());

  MED_FIELD_RDWR_DRIVER<double> * aMedRWDriverForField =
    dynamic_cast<MED_FIELD_RDWR_DRIVER<double> *> (aDriver);
  CPPUNIT_ASSERT(aMedRWDriverForField);

  delete aDriver; aDriver=0;

  // 5.2: GIBI_DRIVER

  // rdonly
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForField(GIBI_DRIVER, "anyfile", field, MED_EN::RDONLY),
                       MED_EXCEPTION);
  // wronly
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForField(GIBI_DRIVER, "anyfile", field, MED_EN::WRONLY),
                       MED_EXCEPTION);
  // rdwr
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForField(GIBI_DRIVER, "anyfile", field, MED_EN::RDWR),
                       MED_EXCEPTION);

  // 5.3: PORFLOW_DRIVER

  // rdonly
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForField(PORFLOW_DRIVER, "anyfile",
                                                          field, MED_EN::RDONLY),
                       MED_EXCEPTION);
  // wronly
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForField(PORFLOW_DRIVER, "anyfile",
                                                          field, MED_EN::WRONLY),
                       MED_EXCEPTION);
  // rdwr
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForField(PORFLOW_DRIVER, "anyfile",
                                                          field, MED_EN::RDWR),
                       MED_EXCEPTION);

  // 5.4: VTK_DRIVER

  // rdonly
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForField(VTK_DRIVER, "anyfile", field, MED_EN::RDONLY),
                       MED_EXCEPTION);

  // wronly
  aDriver = DRIVERFACTORY::buildDriverForField(VTK_DRIVER, "anyfile", field, MED_EN::WRONLY);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

  VTK_FIELD_DRIVER<double> * aVtkDriverForField = dynamic_cast<VTK_FIELD_DRIVER<double> *> (aDriver);
  CPPUNIT_ASSERT(aVtkDriverForField);

  delete aDriver; aDriver=0;

  // rdwr
  aDriver = DRIVERFACTORY::buildDriverForField(VTK_DRIVER, "anyfile", field, MED_EN::RDWR);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  // next string is commented by skl since VTK driver is 
  // created without mode
  //CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR);

  aVtkDriverForField = dynamic_cast<VTK_FIELD_DRIVER<double> *> (aDriver);
  CPPUNIT_ASSERT(aVtkDriverForField);

  delete aDriver; aDriver=0;

  // 5.5: ASCII_DRIVER

  // rdonly
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForField(ASCII_DRIVER, "anyfile", field, MED_EN::RDONLY),
                       MED_EXCEPTION);
  // rdwr
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForField(ASCII_DRIVER, "anyfile", field, MED_EN::RDWR),
                       MED_EXCEPTION);

  // wronly
  // it throws on empty field
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForField(ASCII_DRIVER, "anyfile", field, MED_EN::WRONLY),
                       MED_EXCEPTION);

  // now try with valid field
  string filename  = getResourceFile("pointe.med");
  string meshname  = "maa1";
  string fieldname = "fieldcelldoublescalar";
  FIELD<double> * aField1 = new FIELD<double>(MED_DRIVER, filename, fieldname);
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

  delete aDriver; aDriver=0;

  aField1->removeReference();
  aMesh->removeReference();

  // 5.6: NO_DRIVER
  CPPUNIT_ASSERT_THROW(DRIVERFACTORY::buildDriverForField(NO_DRIVER, "anyfile", field, MED_EN::RDONLY),
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
    version1 = MED_EN::V22;
  }

  try {
    version2 = getMedFileVersion(aFileName2);
  } catch (MEDEXCEPTION & ex) {
    version2 = MED_EN::V22;
  }

  // 6.1. Med file V2.1
  string aFileName21 = aFileName1;
  if (version1 == MED_EN::V21 || version2 == MED_EN::V21) {
    if (version1 == MED_EN::V22)
      aFileName21 = aFileName2;

  }

//   // 6.2. Med file V2.2
  string aFileName22 = aFileName2;
  if (version2 == MED_EN::V22 || version1 == MED_EN::V22) {
    if (version2 == MED_EN::V21)
      aFileName22 = aFileName1;

  }

  /////////////////////////////////////
  // Test 7: buildMeshDriverFromFile //
  /////////////////////////////////////

  // 7.1. Med file V2.1
  if (version1 == MED_EN::V21 || version2 == MED_EN::V21) {
    // Obsolete, removed
  }

  // 7.2. Med file V2.2
  if (version2 == MED_EN::V22 || version1 == MED_EN::V22) {
    // rdonly
    aDriver = DRIVERFACTORY::buildMeshDriverFromFile(aFileName22, mesh, MED_EN::RDONLY);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName22);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);

    MED_MESH_RDONLY_DRIVER * aMeshRDriverForMed22 = dynamic_cast<MED_MESH_RDONLY_DRIVER *> (aDriver);
    CPPUNIT_ASSERT(aMeshRDriverForMed22);

    delete aDriver; aDriver=0;

    // wronly
    aDriver = DRIVERFACTORY::buildMeshDriverFromFile(aFileName22, mesh, MED_EN::WRONLY);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName22);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

    MED_MESH_WRONLY_DRIVER * aMeshWDriverForMed22 = dynamic_cast<MED_MESH_WRONLY_DRIVER *> (aDriver);
    CPPUNIT_ASSERT(aMeshWDriverForMed22);

    delete aDriver; aDriver=0;

    // rdwr
    aDriver = DRIVERFACTORY::buildMeshDriverFromFile(aFileName22, mesh, MED_EN::RDWR);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName22);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR);

    MED_MESH_RDWR_DRIVER * aMeshRWDriverForMed22 = dynamic_cast<MED_MESH_RDWR_DRIVER *> (aDriver);
    CPPUNIT_ASSERT(aMeshRWDriverForMed22);

    delete aDriver; aDriver=0;
  }

  //////////////////////////////////////
  // Test 8: buildFieldDriverFromFile //
  //////////////////////////////////////

  // 8.1. Med file V2.1
  if (version1 == MED_EN::V21 || version2 == MED_EN::V21) {
    // rdonly
    aDriver = DRIVERFACTORY::buildFieldDriverFromFile(aFileName21, field, MED_EN::RDONLY);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName21);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);

    delete aDriver; aDriver=0;

    // wronly
    aDriver = DRIVERFACTORY::buildFieldDriverFromFile(aFileName21, field, MED_EN::WRONLY);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName21);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

    delete aDriver; aDriver=0;

    // rdwr
    aDriver = DRIVERFACTORY::buildFieldDriverFromFile(aFileName21, field, MED_EN::RDWR);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName21);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR); // see MEDMEM_define.hxx

    delete aDriver; aDriver=0;
  }

  // 8.2. Med file V2.2
  if (version2 == MED_EN::V22 || version1 == MED_EN::V22) {
    // rdonly
    aDriver = DRIVERFACTORY::buildFieldDriverFromFile(aFileName22, field, MED_EN::RDONLY);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName22);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);

    MED_FIELD_RDONLY_DRIVER<double> * aFieldRDriverForMed22 =
      dynamic_cast<MED_FIELD_RDONLY_DRIVER<double> *> (aDriver);
    CPPUNIT_ASSERT(aFieldRDriverForMed22);

    delete aDriver; aDriver=0;

    // wronly
    aDriver = DRIVERFACTORY::buildFieldDriverFromFile(aFileName22, field, MED_EN::WRONLY);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName22);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

    MED_FIELD_WRONLY_DRIVER<double> * aFieldWDriverForMed22 =
      dynamic_cast<MED_FIELD_WRONLY_DRIVER<double> *> (aDriver);
    CPPUNIT_ASSERT(aFieldWDriverForMed22);

    delete aDriver; aDriver=0;

    // rdwr
    aDriver = DRIVERFACTORY::buildFieldDriverFromFile(aFileName22, field, MED_EN::RDWR);
    CPPUNIT_ASSERT(aDriver->getFileName() == aFileName22);
    CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR); // see MEDMEM_define.hxx

    MED_FIELD_RDWR_DRIVER<double> * aFieldRWDriverForMed22 =
      dynamic_cast<MED_FIELD_RDWR_DRIVER<double> *> (aDriver);
    CPPUNIT_ASSERT(aFieldRWDriverForMed22);

    delete aDriver; aDriver=0;
  }

  ///////////////////////////////////////////
  // Test 9: buildConcreteMedDriverForMesh //
  ///////////////////////////////////////////

  // 9.1. V2.1

  // rdonly
  CPPUNIT_ASSERT_THROW(aDriver = DRIVERFACTORY::buildConcreteMedDriverForMesh("anyfile", mesh, "my driver name",
                                                                              MED_EN::RDONLY, MED_EN::V21),
                       MED_EXCEPTION);
  delete aDriver; aDriver=0;

  // wronly
  CPPUNIT_ASSERT_THROW(aDriver = DRIVERFACTORY::buildConcreteMedDriverForMesh("anyfile", mesh, "my driver name",
                                                                              MED_EN::WRONLY, MED_EN::V21),
                       MED_EXCEPTION);

  delete aDriver; aDriver=0;

  // rdwr
  CPPUNIT_ASSERT_THROW(aDriver = DRIVERFACTORY::buildConcreteMedDriverForMesh("anyfile", mesh, "my driver name",
                                                                              MED_EN::RDWR, MED_EN::V21),
                       MED_EXCEPTION);

  delete aDriver; aDriver=0;

  // 9.2. V2.2

  // rdonly
  aDriver = DRIVERFACTORY::buildConcreteMedDriverForMesh("anyfile", mesh, "my driver name",
                                                         MED_EN::RDONLY, MED_EN::V22);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);

  MED_MESH_RDONLY_DRIVER * aMeshRDriverForMed22 = dynamic_cast<MED_MESH_RDONLY_DRIVER *> (aDriver);
  CPPUNIT_ASSERT(aMeshRDriverForMed22);
  CPPUNIT_ASSERT(aMeshRDriverForMed22->getMeshName() == "my driver name");

  delete aDriver; aDriver=0;

  // wronly
  aDriver = DRIVERFACTORY::buildConcreteMedDriverForMesh("anyfile", mesh, "my driver name",
                                                         MED_EN::WRONLY, MED_EN::V22);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

  MED_MESH_WRONLY_DRIVER * aMeshWDriverForMed22 = dynamic_cast<MED_MESH_WRONLY_DRIVER *> (aDriver);
  CPPUNIT_ASSERT(aMeshWDriverForMed22);
  CPPUNIT_ASSERT(aMeshWDriverForMed22->getMeshName() == "my driver name");

  delete aDriver; aDriver=0;

  // rdwr
  aDriver = DRIVERFACTORY::buildConcreteMedDriverForMesh("anyfile", mesh, "my driver name",
                                                         MED_EN::RDWR, MED_EN::V22);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR);

  MED_MESH_RDWR_DRIVER * aMeshRWDriverForMed22 = dynamic_cast<MED_MESH_RDWR_DRIVER *> (aDriver);
  CPPUNIT_ASSERT(aMeshRWDriverForMed22);
  CPPUNIT_ASSERT(aMeshRWDriverForMed22->getMeshName() == "my driver name");
  mesh->removeReference();
  delete aDriver; aDriver=0;

  /////////////////////////////////////////////
  // Test 10: buildConcreteMedDriverForField //
  /////////////////////////////////////////////

  // 10.1. V2.1

  // rdonly
  CPPUNIT_ASSERT_THROW(aDriver = DRIVERFACTORY::buildConcreteMedDriverForField("anyfile", field, MED_EN::RDONLY, MED_EN::V21),
                       MED_EXCEPTION);
  delete aDriver; aDriver=0;

  // wronly
  CPPUNIT_ASSERT_THROW(aDriver = DRIVERFACTORY::buildConcreteMedDriverForField("anyfile", field, MED_EN::WRONLY, MED_EN::V21),
                       MED_EXCEPTION);
  delete aDriver; aDriver=0;

  // rdwr
  CPPUNIT_ASSERT_THROW(aDriver = DRIVERFACTORY::buildConcreteMedDriverForField("anyfile", field, MED_EN::RDWR, MED_EN::V21), MED_EXCEPTION);
  delete aDriver; aDriver=0;

  // 10.2. V2.2

  // rdonly
  aDriver = DRIVERFACTORY::buildConcreteMedDriverForField("anyfile", field, MED_EN::RDONLY, MED_EN::V22);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDONLY);

  MED_FIELD_RDONLY_DRIVER<double> * aFieldRDriverForMed22 =
    dynamic_cast<MED_FIELD_RDONLY_DRIVER<double> *> (aDriver);
  CPPUNIT_ASSERT(aFieldRDriverForMed22);

  delete aDriver; aDriver=0;

  // wronly
  aDriver = DRIVERFACTORY::buildConcreteMedDriverForField("anyfile", field, MED_EN::WRONLY, MED_EN::V22);
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::WRONLY);

  MED_FIELD_WRONLY_DRIVER<double> * aFieldWDriverForMed22 =
    dynamic_cast<MED_FIELD_WRONLY_DRIVER<double> *> (aDriver);
  CPPUNIT_ASSERT(aFieldWDriverForMed22);

  delete aDriver; aDriver=0;

  // rdwr
  aDriver = DRIVERFACTORY::buildConcreteMedDriverForField("anyfile", field, MED_EN::RDWR, MED_EN::V22);
  field->removeReference();
  CPPUNIT_ASSERT(aDriver->getFileName() == "anyfile");
  CPPUNIT_ASSERT(aDriver->getAccessMode() == MED_EN::RDWR); // see MEDMEM_define.hxx

  MED_FIELD_RDWR_DRIVER<double> * aFieldRWDriverForMed22 =
    dynamic_cast<MED_FIELD_RDWR_DRIVER<double> *> (aDriver);
  CPPUNIT_ASSERT(aFieldRWDriverForMed22);

  delete aDriver; aDriver=0;
}
