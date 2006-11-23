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
//

#include "MEDMEMTest.hxx"
#include <cppunit/TestAssert.h>

////#include "MEDMEM_nArray.hxx"
////#include "MEDMEM_ArrayConvert.hxx"
////#include "MEDMEM_Array.hxx"
//#include "MEDMEM_ArrayInterface.hxx"
//#include "MEDMEM_AsciiFieldDriver.hxx"
//#include "MEDMEM_CellModel.hxx"
//#include "MEDMEM_Compatibility21_22.hxx"
//#include "MEDMEM_Connectivity.hxx"
////#include "MEDMEM_Coordinate.hxx"
//#include "MEDMEM_define.hxx"
//#include "MEDMEM_DriverFactory.hxx"
//#include "MEDMEM_DriversDef.hxx"
//#include "MEDMEM_DriverTools.hxx"
//#include "MEDMEM_Exception.hxx"
//#include "MEDMEM_Family.hxx"
//#include "MEDMEM_FieldConvert.hxx"
//#include "MEDMEM_FieldForward.hxx"
//#include "MEDMEM_Field.hxx"
//#include "MEDMEM_Formulae.hxx"
//#include "MEDMEM_GaussLocalization.hxx"
//#include "MEDMEM_GenDriver.hxx"
//#include "MEDMEM_GibiMeshDriver.hxx"
//#include "MEDMEM_Grid.hxx"
//#include "MEDMEM_Group.hxx"
//#include "MEDMEM_IndexCheckingPolicy.hxx"
//#include "MEDMEM_InterlacingPolicy.hxx"
//#include "MEDMEM_InterlacingTraits.hxx"
//#include "MEDMEM_MedFieldDriver21.hxx"
//#include "MEDMEM_MedFieldDriver22.hxx"
//#include "MEDMEM_MedFieldDriver.hxx"
//#include "MEDMEM_Med.hxx"
//#include "MEDMEM_medimport_src.hxx"
//#include "MEDMEM_MedMedDriver21.hxx"
//#include "MEDMEM_MedMedDriver22.hxx"
//#include "MEDMEM_MedMedDriver.hxx"
//#include "MEDMEM_MEDMEMchampLire.hxx"
//#include "MEDMEM_MEDMEMgaussEcr.hxx"
//#include "MEDMEM_MEDMEMprofilEcr.hxx"
//#include "MEDMEM_MedMeshDriver21.hxx"
//#include "MEDMEM_MedMeshDriver22.hxx"
//#include "MEDMEM_MedMeshDriver.hxx"
//#include "MEDMEM_MedVersion.hxx"
//#include "MEDMEM_Mesh.hxx"
//#include "MEDMEM_Meshing.hxx"
//#include "MEDMEM_ModulusArray.hxx"
//#include "MEDMEM_PointerOf.hxx"
//#include "MEDMEM_PolyhedronArray.hxx"
//#include "MEDMEM_PorflowMeshDriver.hxx"
//#include "MEDMEM_RCBase.hxx"
//#include "MEDMEM_SetInterlacingType.hxx"
//#include "MEDMEM_SkyLineArray.hxx"
#include "MEDMEM_STRING.hxx"
//#include "MEDMEM_Support.hxx"
//#include "MEDMEM_Tags.hxx"
//#include "MEDMEM_TopLevel.hxx"
//#include "MEDMEM_TypeMeshDriver.hxx"
//#include "MEDMEM_Unit.hxx"
//#include "MEDMEM_Utilities.hxx"
//#include "MEDMEM_VtkFieldDriver.hxx"
//#include "MEDMEM_VtkMedDriver.hxx"
//#include "MEDMEM_VtkMeshDriver.hxx"

#include <sstream>
#include <cmath>

using namespace std;
using namespace MEDMEM;

// #30: MEDMEM_Med.hxx  }  MEDMEMTest_Med.cxx

/*!
 *  Check methods (26), defined in MEDMEM_Med.hxx:
 *  class MED {
 *   (yetno) MED();
 *   (yetno) MED (driverTypes driverType, const string & fileName);
 *   (yetno) ~MED();
 *   (yetno) void addField (FIELD_ * const ptrField) throw (MED_EXCEPTION);
 *   (yetno) void addMesh  (MESH   * const ptrMesh) throw (MED_EXCEPTION);
 *   (yetno) int addDriver (driverTypes driverType, const string & fileName,
 *                          MED_EN::med_mode_acces access=MED_EN::MED_REMP);
 *   (yetno) int addDriver (GENDRIVER & driver);
 *   (yetno) void rmDriver (int index=0) throw (MEDEXCEPTION);
 *   (yetno) void readFileStruct(int index=0) throw (MEDEXCEPTION);
 *   (yetno) void read (int index=0) throw (MEDEXCEPTION);
 *   (yetno) void writeFrom (int index=0) throw (MEDEXCEPTION);
 *   (yetno) void write (int index=0) throw (MEDEXCEPTION);
 *   (yetno) int  getNumberOfMeshes (void) const;
 *   (yetno) int  getNumberOfFields (void) const;
 *   (yetno) void getMeshNames (string * meshNames) const throw (MEDEXCEPTION);
 *   (yetno) deque<string> getMeshNames   () const;
 *   (yetno) MESH * getMesh (const string & meshName) const throw (MEDEXCEPTION);
 *   (yetno) MESH * getMesh (const  FIELD_ * const field) const throw (MEDEXCEPTION);
 *   (yetno) void getFieldNames (string * fieldNames) const throw (MEDEXCEPTION);
 *   (yetno) deque<string> getFieldNames () const;
 *   (yetno) deque<DT_IT_> getFieldIteration (const string & fieldName) const throw (MEDEXCEPTION);
 *   (yetno) FIELD_ * getField (const string & fieldName, const int dt,
 *                              const int it) const throw (MEDEXCEPTION);
 *   (yetno) template<class T> FIELD<T> * getFieldT
 *           (const string & fieldName, const int dt,  const int it) const throw (MEDEXCEPTION);
 *   (yetno) FIELD_ * getField2 (const string & fieldName, double time, int it=0) const throw (MEDEXCEPTION);
 *   (yetno) const map<MED_EN::medEntityMesh,SUPPORT *> & getSupports
 *           (const string & meshName) const throw (MEDEXCEPTION);
 *   (yetno) SUPPORT *  getSupport (const string & meshName,
 *                                  MED_EN::medEntityMesh entity) const throw (MEDEXCEPTION);
 *   (yetno) void updateSupport ();
 *  }
 */
void MEDMEMTest::testMed()
{
  CPPUNIT_FAIL("Case Not Implemented");
}
