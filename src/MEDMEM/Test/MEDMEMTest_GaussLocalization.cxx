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

// #18: MEDMEM_GaussLocalization.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (13), defined in MEDMEM_GaussLocalization.hxx:
 *  class GAUSS_LOCALIZATION_ {
 *   (yetno) virtual MED_EN::medModeSwitch getInterlacingType() const;
 *   (yetno) virtual ~GAUSS_LOCALIZATION_();
 *  }
 *  (yetno) template <class INTERLACING_TAG=FullInterlace> class GAUSS_LOCALIZATION;
 *  (yetno) template <class INTERLACING_TAG> ostream & operator<<
 *                (ostream &os, const GAUSS_LOCALIZATION<INTERLACING_TAG> &loc);
 *  template <class INTERLACING_TAG> class GAUSS_LOCALIZATION : public GAUSS_LOCALIZATION_ {
 *   (yetno) friend ostream & operator<< <INTERLACING_TAG>
 *                     (ostream &os, const GAUSS_LOCALIZATION<INTERLACING_TAG> &loc);
 *   (yetno) GAUSS_LOCALIZATION() throw (MEDEXCEPTION);
 *   (yetno) GAUSS_LOCALIZATION(const string & locName,
 *                       const MED_EN::medGeometryElement typeGeo,
 *                       const int  nGauss,
 *                       const ArrayNoGauss & cooRef,
 *                       const ArrayNoGauss & cooGauss,
 *                       const vector<double>  & wg) throw (MEDEXCEPTION);
 *   (yetno) GAUSS_LOCALIZATION(const string & locName,
 *                       const MED_EN::medGeometryElement  typeGeo,
 *                       const int  nGauss,
 *                       const double  * const cooRef,
 *                       const double  * const cooGauss,
 *                       const double  * const wg) throw (MEDEXCEPTION);
 *   (yetno) virtual ~GAUSS_LOCALIZATION();
 *   (yetno) GAUSS_LOCALIZATION & operator=(const GAUSS_LOCALIZATION & gaussLoc);
 *   (yetno) bool operator == (const GAUSS_LOCALIZATION &loc) const;
 *   (yetno) string          getName()    const;
 *   (yetno) MED_EN::medGeometryElement getType() const;
 *   (yetno) int             getNbGauss() const;
 *   (yetno) ArrayNoGauss    getRefCoo () const;
 *   (yetno) ArrayNoGauss    getGsCoo  () const;
 *   (yetno) vector <double> getWeight () const;
 *   (yetno) inline MED_EN::medModeSwitch  getInterlacingType() const;
 *  }
 */
void MEDMEMTest::testGaussLocalization()
{
  CPPUNIT_FAIL("Case Not Implemented");
}
