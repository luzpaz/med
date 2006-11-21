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

#include "MEDMEM_Formulae.hxx"
#include "MEDMEM_STRING.hxx"

#include <sstream>
#include <cmath>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;

// #17: MEDMEM_Formulae.hxx  }  MEDMEMTest_Formulae.cxx

/*!
 *  Check methods (13), defined in MEDMEM_Formulae.hxx:
 *
 *  (yetno) inline void CalculateBarycenterDyn(const double **pts, int nbPts, int dim, double *bary);
 *
 *  (+)     inline double CalculateAreaForPolyg(const double **coords, int nbOfPtsInPolygs, int spaceDim);
 *  (+)     inline double CalculateAreaForTria(const double *p1, const double *p2,
 *                                             const double *p3, int spaceDim);
 *  (+)     inline double CalculateAreaForQuad(const double *p1, const double *p2,
 *                                             const double *p3, const double *p4, int spaceDim);
 *
 *  (yetno) inline void CalculateNormalForTria(const double *p1, const double *p2,
 *                                             const double *p3, double *normal);
 *  (yetno) inline void CalculateNormalForQuad(const double *p1, const double *p2,
 *                                             const double *p3, const double *p4, double *normal);
 *  (yetno) inline void CalculateNormalForPolyg(const double **coords, int nbOfPtsInPolygs, double *normal);
 *
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
 *
 *  (yetno) template<int N> inline double addComponentsOfVec(const double **pts, int rk);
 *  (yetno) template<> inline double addComponentsOfVec<1>(const double **pts, int rk);
 *
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
  double val;

  //      ^y
  //      |
  //      *3
  //  4   |
  //  .*. . . . . . *6
  //      |
  //  . . . . . . . .
  //      |  5
  //  . . . .*. *2. .
  //     1|
  // -.-.-*-.-.-.-.-.-->x
  //      |

  // S_12634 = (3 + (3+4.8)*2 + 4.8 + 1.5*4)/2 = 1.5 + 7.8 + 2.4 + 3 = 14.7
  // S_143652 = S_14362 - S_625 = 14.7 - 2 * 1.5 / 2 = 13.2

  double xy1[2]  = { 0.0, 0.0};
  double xy2[2]  = { 3.0, 1.0};
  double xy3[2]  = { 0.0, 4.0};
  double xy4[2]  = {-1.5, 3.0};
  double xy5[2]  = { 1.5, 1.0};
  double xy6[2]  = { 4.8, 3.0};

  double xyz1[3] = { 0.0, 0.0,  0.0};
  double xyz2[3] = { 3.0, 1.0,  4.0}; // cos(alpha) = 3/5
  double xyz3[3] = { 0.0, 4.0,  0.0};
  double xyz4[3] = {-1.5, 3.0, -2.0}; // z4 = z2 * x4 / x2 = - 4 * 1.5 / 3
  double xyz5[3] = { 1.5, 1.0,  2.0}; // z5 = z2 * x5 / x2 = 4 * 1.5 / 3
  double xyz6[3] = { 4.8, 3.0,  6.4}; // z6 = z2 * x6 / x2 = 4 * 4.8 / 3

  // S_3d = S_2d * 5.0 / 3.0

  ///////////////////////////
  // CalculateAreaForPolyg //
  ///////////////////////////

  // 2D: Convex polygon
  const double * poly_2d_cc[5] = {xy1, xy2, xy6, xy3, xy4};
  const double * poly_2d_cw[5] = {xy1, xy4, xy3, xy6, xy2};

  // counter-clockwise
  val = CalculateAreaForPolyg(poly_2d_cc, /*nbOfPtsInPolygs*/5, /*dim*/2);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(-14.7, val, 0.000001);

  // clockwise
  val = CalculateAreaForPolyg(poly_2d_cw, /*nbOfPtsInPolygs*/5, /*dim*/2);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(14.7, val, 0.000001);

  // 2D: Non-convex polygon
  const double * poly_2d_nc[6] = {xy1, xy4, xy3, xy6, xy5, xy2};
  val = CalculateAreaForPolyg(poly_2d_nc, /*nbOfPtsInPolygs*/6, /*dim*/2);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(13.2, val, 0.000001);

  // 3D: Convex polygon
  const double * poly_3d_cc[5] = {xyz1, xyz2, xyz6, xyz3, xyz4};

  val = CalculateAreaForPolyg(poly_3d_cc, /*nbOfPtsInPolygs*/5, /*dim*/3);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(24.5, val, 0.000001);

  // 3D: Non-convex polygon
  const double * poly_3d_nc[6] = {xyz1, xyz4, xyz3, xyz6, xyz5, xyz2};
  val = CalculateAreaForPolyg(poly_3d_nc, /*nbOfPtsInPolygs*/6, /*dim*/3);
#ifdef ENABLE_FORCED_FAILURES
  // (BUG) Wrong area calculation for non-convex polygons in 3D space,
  //       because area of triangle is always positive
  CPPUNIT_ASSERT_DOUBLES_EQUAL(22.0, val, 0.000001);
#endif

  //////////////////////////
  // CalculateAreaForTria //
  //////////////////////////

  // 2D: counter-clockwise
  val = CalculateAreaForTria(xy1, xy2, xy3, /*dim*/2);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(-6.0, val, 0.000001);

  // 2D: clockwise
  val = CalculateAreaForTria(xy2, xy1, xy3, /*dim*/2);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0, val, 0.000001);

  // 3D
  val = CalculateAreaForTria(xyz1, xyz2, xyz3, /*dim*/3);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(10.0, val, 0.000001);

  //////////////////////////
  // CalculateAreaForQuad //
  //////////////////////////

  // 2D: Convex quadrangle

  // counter-clockwise
  val = CalculateAreaForQuad(xy1, xy2, xy3, xy4, /*dim*/2);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(-9.0, val, 0.000001);

  // clockwise
  val = CalculateAreaForQuad(xy2, xy1, xy4, xy3, /*dim*/2);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0, val, 0.000001);

  // wrong order
  CPPUNIT_ASSERT_NO_THROW(CalculateAreaForQuad(xy2, xy1, xy3, xy4, /*dim*/2));

  // 2D: Non-convex quadrangle

  // counter-clockwise
  val = CalculateAreaForQuad(xy1, xy2, xy3, xy5, /*dim*/2);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(-3.0, val, 0.000001);

  // clockwise
  val = CalculateAreaForQuad(xy1, xy5, xy3, xy2, /*dim*/2);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0, val, 0.000001);

  // 3D: Convex quadrangle

  // good order
  val = CalculateAreaForQuad(xyz1, xyz2, xyz3, xyz4, /*dim*/3);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(15.0, val, 0.000001);

  // wrong order
  CPPUNIT_ASSERT_NO_THROW(CalculateAreaForQuad(xyz1, xyz4, xyz2, xyz3, /*dim*/3));

  // 3D: Non-convex quadrangle
  val = CalculateAreaForQuad(xyz1, xyz2, xyz3, xyz5, /*dim*/3);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0, val, 0.000001);

  // 3D: Non-planar quadrangle
  double xyz7[3] = {-1.5, 3.0,  2.0};
  CPPUNIT_ASSERT_NO_THROW(CalculateAreaForQuad(xyz1, xyz2, xyz3, xyz7, /*dim*/3));

  CPPUNIT_FAIL("Case Not Complete: only area calculation tested.");
}
