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

// #17: MEDMEM_Formulae.hxx       }  MEDMEMTest_Formulae.cxx

/*!
 *  Check methods (13), defined in MEDMEM_Formulae.hxx:
 *  (yetno) inline void CalculateBarycenterDyn(const double **pts, int nbPts, int dim, double *bary);
 *  (yetno) inline double CalculateAreaForPolyg(const double **coords, int nbOfPtsInPolygs, int spaceDim);
 *  (yetno) inline double CalculateAreaForTria(const double *p1, const double *p2,
 *                                             const double *p3, int spaceDim);
 *  (yetno) inline double CalculateAreaForQuad(const double *p1, const double *p2,
 *                                             const double *p3, const double *p4, int spaceDim);
 *  (yetno) inline void CalculateNormalForTria(const double *p1, const double *p2,
 *                                             const double *p3, double *normal);
 *  (yetno) inline void CalculateNormalForQuad(const double *p1, const double *p2,
 *                                             const double *p3, const double *p4, double *normal);
 *  (yetno) inline void CalculateNormalForPolyg(const double **coords, int nbOfPtsInPolygs, double *normal);
 *  (yetno) inline double CalculateVolumeForTetra(const double *p1, const double *p2,
 *                                                const double *p3, const double *p4);
 *  (yetno) inline double CalculateVolumeForPyra(const double *p1, const double *p2,
 *                                               const double *p3, const double *p4, const double *p5);
 *  (yetno) inline double CalculateVolumeForPenta(const double *p1, const double *p2, const double *p3,
 *                                                const double *p4, const double *p5, const double *p6);
 *  (yetno) inline double CalculateVolumeForHexa(const double *pt1, const double *pt2, const double *pt3,
 *                                               const double *pt4, const double *pt5, const double *pt6,
 *                                               const double *pt7, const double *pt8);
 *  (yetno) inline double CalculateVolumeForPolyh(const double ***pts, const int *nbOfNodesPerFaces,
 *                                                int nbOfFaces, const double *bary);
 *  (yetno) template<int N> inline double addComponentsOfVec(const double **pts, int rk);
 *  (yetno) template<> inline double addComponentsOfVec<1>(const double **pts, int rk);
 *  (yetno) template<int N, int DIM> inline void CalculateBarycenter(const double **pts, double *bary);
 *  (yetno) template<> inline void CalculateBarycenter<2,0>(const double **pts, double *bary);
 *  (yetno) template<> inline void CalculateBarycenter<3,0>(const double **pts, double *bary);
 *  (yetno) template<> inline void CalculateBarycenter<4,0>(const double **pts, double *bary);
 *  (yetno) template<> inline void CalculateBarycenter<5,0>(const double **pts, double *bary);
 *  (yetno) template<> inline void CalculateBarycenter<6,0>(const double **pts, double *bary);
 *  (yetno) template<> inline void CalculateBarycenter<7,0>(const double **pts, double *bary);
 *  (yetno) template<> inline void CalculateBarycenter<8,0>(const double **pts, double *bary);
 */
void MEDMEMTest::testFormulae()
{
  CPPUNIT_FAIL("Case Not Implemented");
}
