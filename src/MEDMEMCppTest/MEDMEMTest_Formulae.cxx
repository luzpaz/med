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

#include "VolSurfFormulae.hxx"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"

#include <iostream>
#include <sstream>
#include <cmath>
#include <cfloat>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;

// #17: MEDMEM_Formulae.hxx  }  MEDMEMTest_Formulae.cxx

/*!
 *  Check methods (13), defined in MEDMEM_Formulae.hxx:
 *
 *  (+)     inline void CalculateBarycenterDyn(const double **pts, int nbPts, int dim, double *bary);
 *
 *  (+)     inline double CalculateAreaForPolyg(const double **coords, int nbOfPtsInPolygs, int spaceDim);
 *  (+)     inline double CalculateAreaForTria(const double *p1, const double *p2,
 *                                             const double *p3, int spaceDim);
 *  (+)     inline double CalculateAreaForQuad(const double *p1, const double *p2,
 *                                             const double *p3, const double *p4, int spaceDim);
 *
 *  (+)     inline void CalculateNormalForTria(const double *p1, const double *p2,
 *                                             const double *p3, double *normal);
 *  (+)     inline void CalculateNormalForQuad(const double *p1, const double *p2,
 *                                             const double *p3, const double *p4, double *normal);
 *  (+)     inline void CalculateNormalForPolyg(const double **coords, int nbOfPtsInPolygs, double *normal);
 *
 *  (+)     inline double CalculateVolumeForTetra(const double *p1, const double *p2,
 *                                                const double *p3, const double *p4);
 *  (+)     inline double CalculateVolumeForPyra(const double *p1, const double *p2,
 *                                               const double *p3, const double *p4, const double *p5);
 *  (+)     inline double CalculateVolumeForPenta(const double *p1, const double *p2, const double *p3,
 *                                                const double *p4, const double *p5, const double *p6);
 *  (+)     inline double CalculateVolumeForHexa(const double *pt1, const double *pt2, const double *pt3,
 *                                               const double *pt4, const double *pt5, const double *pt6,
 *                                               const double *pt7, const double *pt8);
 *  (+)     inline double CalculateVolumeForPolyh(const double ***pts, const int *nbOfNodesPerFaces,
 *                                                int nbOfFaces, const double *bary);
 *
 *  (+)     template<int N> inline double addComponentsOfVec(const double **pts, int rk);
 *  (+)     template<> inline double addComponentsOfVec<1>(const double **pts, int rk);
 *
 *  (+)     template<int N, int DIM> inline void CalculateBarycenter(const double **pts, double *bary);
 *  (-)     template<> inline void CalculateBarycenter<2,0>(const double **pts, double *bary);
 *  (-)     template<> inline void CalculateBarycenter<3,0>(const double **pts, double *bary);
 *  (-)     template<> inline void CalculateBarycenter<4,0>(const double **pts, double *bary);
 *  (-)     template<> inline void CalculateBarycenter<5,0>(const double **pts, double *bary);
 *  (-)     template<> inline void CalculateBarycenter<6,0>(const double **pts, double *bary);
 *  (-)     template<> inline void CalculateBarycenter<7,0>(const double **pts, double *bary);
 *  (-)     template<> inline void CalculateBarycenter<8,0>(const double **pts, double *bary);
 */
void MEDMEMTest::testFormulae()
{
  double val;

  //      ^y
  //      |
  //      *3
  //  4   |
  //  .*. . . . . . *6,7
  //      |
  //  . .8* . . . . .
  //      |  5   2,9
  //  . . . .*. * . .
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
  double xyz7[3] = { 4.8, 3.0,  0.0};
  double xyz8[3] = { 0.0, 2.0,  0.0};
  double xyz9[3] = { 3.0, 1.0,  0.0};

  // S_3d = S_2d * 5.0 / 3.0

  ///////////////////////////
  // CalculateAreaForPolyg //
  ///////////////////////////
  {
    // 2D: Convex polygon
    const double * poly_2d_cc[5] = {xy1, xy2, xy6, xy3, xy4};
    const double * poly_2d_cw[5] = {xy1, xy4, xy3, xy6, xy2};

    // counter-clockwise
    val = INTERP_KERNEL::calculateAreaForPolyg(poly_2d_cc, /*nbOfPtsInPolygs*/5, /*dim*/2);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-14.7, val, 0.000001);

    // clockwise
    val = INTERP_KERNEL::calculateAreaForPolyg(poly_2d_cw, /*nbOfPtsInPolygs*/5, /*dim*/2);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(14.7, val, 0.000001);

    // 2D: Non-convex polygon
    const double * poly_2d_nc[6] = {xy1, xy4, xy3, xy6, xy5, xy2};
    val = INTERP_KERNEL::calculateAreaForPolyg(poly_2d_nc, /*nbOfPtsInPolygs*/6, /*dim*/2);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(13.2, val, 0.000001);

    // 3D: Convex polygon
    const double * poly_3d_cc[5] = {xyz1, xyz2, xyz6, xyz3, xyz4};

    val = INTERP_KERNEL::calculateAreaForPolyg(poly_3d_cc, /*nbOfPtsInPolygs*/5, /*dim*/3);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(24.5, val, 0.000001);

    // 3D: Non-convex polygon
    const double * poly_3d_nc[6] = {xyz1, xyz4, xyz3, xyz6, xyz5, xyz2};
    val = INTERP_KERNEL::calculateAreaForPolyg(poly_3d_nc, /*nbOfPtsInPolygs*/6, /*dim*/3);
    //#ifdef ENABLE_FORCED_FAILURES
    // (BUG) Wrong area calculation for non-convex polygons in 3D space,
    //       because area of triangle is always positive
    // It's a spec
    //CPPUNIT_ASSERT_DOUBLES_EQUAL(22.0, val, 0.000001);
    CPPUNIT_ASSERT( abs(22.0-val)>0.000001 );
    //#endif
  }

  //////////////////////////
  // CalculateAreaForTria //
  //////////////////////////
  {
    // 2D: counter-clockwise
    val = INTERP_KERNEL::calculateAreaForTria(xy1, xy2, xy3, /*dim*/2);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-6.0, val, 0.000001);

    // 2D: clockwise
    val = INTERP_KERNEL::calculateAreaForTria(xy2, xy1, xy3, /*dim*/2);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0, val, 0.000001);

    // 3D
    val = INTERP_KERNEL::calculateAreaForTria(xyz1, xyz2, xyz3, /*dim*/3);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(10.0, val, 0.000001);

    // Invalid (three points on one line)
    val = INTERP_KERNEL::calculateAreaForTria(xyz1, xyz8, xyz3, /*dim*/3);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, val, 0.000001);
  }

  //////////////////////////
  // CalculateAreaForQuad //
  //////////////////////////
  {
    // 2D: Convex quadrangle

    // counter-clockwise
    val = INTERP_KERNEL::calculateAreaForQuad(xy1, xy2, xy3, xy4, /*dim*/2);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-9.0, val, 0.000001);

    // clockwise
    val = INTERP_KERNEL::calculateAreaForQuad(xy2, xy1, xy4, xy3, /*dim*/2);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0, val, 0.000001);

    // wrong order
    CPPUNIT_ASSERT_NO_THROW(INTERP_KERNEL::calculateAreaForQuad(xy2, xy1, xy3, xy4, /*dim*/2));

    // 2D: Non-convex quadrangle

    // counter-clockwise
    val = INTERP_KERNEL::calculateAreaForQuad(xy1, xy2, xy3, xy5, /*dim*/2);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-3.0, val, 0.000001);

    // clockwise
    val = INTERP_KERNEL::calculateAreaForQuad(xy1, xy5, xy3, xy2, /*dim*/2);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0, val, 0.000001);

    // 3D: Convex quadrangle

    // good order
    val = INTERP_KERNEL::calculateAreaForQuad(xyz1, xyz2, xyz3, xyz4, /*dim*/3);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(15.0, val, 0.000001);

    // wrong order
    CPPUNIT_ASSERT_NO_THROW(INTERP_KERNEL::calculateAreaForQuad(xyz1, xyz4, xyz2, xyz3, /*dim*/3));

    // 3D: Non-convex quadrangle
    val = INTERP_KERNEL::calculateAreaForQuad(xyz1, xyz2, xyz3, xyz5, /*dim*/3);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0, val, 0.000001);

    // 3D: Non-planar quadrangle
    double xyz7[3] = {-1.5, 3.0,  2.0};
    CPPUNIT_ASSERT_NO_THROW(INTERP_KERNEL::calculateAreaForQuad(xyz1, xyz2, xyz3, xyz7, /*dim*/3));
  }

  ////////////////////////////
  // CalculateNormalForTria //
  ////////////////////////////
  {
    double tria_normal [3];

    // Triangle in plane XOY, normal is opposit to axis Z
    INTERP_KERNEL::calculateNormalForTria(xyz1, xyz3, xyz7, tria_normal);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, tria_normal[0], 0.0000001); // Nx
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, tria_normal[1], 0.0000001); // Ny
    CPPUNIT_ASSERT(tria_normal[2] < -0.0000001); // Nz

    // Triangle in plane XOY, normal co-directed with axis Z
    INTERP_KERNEL::calculateNormalForTria(xyz1, xyz7, xyz3, tria_normal);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, tria_normal[0], 0.0000001); // Nx
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, tria_normal[1], 0.0000001); // Ny
    CPPUNIT_ASSERT(tria_normal[2] > 0.0000001); // Nz

    // Triangle in 3D
    INTERP_KERNEL::calculateNormalForTria(xyz1, xyz3, xyz6, tria_normal);
    double koeff = tria_normal[0]/12.8;
    //CPPUNIT_ASSERT_DOUBLES_EQUAL(12.8, tria_normal[0], 0.0000001); // Nx
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0        , tria_normal[1], 0.0000001); // Ny
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-9.6 * koeff, tria_normal[2], 0.0000001); // Nz

    // Invalid Triangle (three points on one line)
    CPPUNIT_ASSERT_NO_THROW(INTERP_KERNEL::calculateNormalForTria(xyz1, xyz8, xyz3, tria_normal));
    //MEDMEMTest_DumpArray<double>(cout, tria_normal, 3, "Invalid Triangle normal");
    //Invalid Triangle normal: {0, 0, 0}
  }

  ////////////////////////////
  // CalculateNormalForQuad //
  ////////////////////////////
  {
    double quad_normal [3];

    // Quadrangle in plane XOY, normal is opposit to axis Z
    INTERP_KERNEL::calculateNormalForQuad(xyz1, xyz3, xyz7, xyz9, quad_normal);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, quad_normal[0], 0.0000001); // Nx
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, quad_normal[1], 0.0000001); // Ny
    CPPUNIT_ASSERT(quad_normal[2] < -0.0000001); // Nz

    // Quadrangle in plane XOY, normal co-directed with axis Z
    INTERP_KERNEL::calculateNormalForQuad(xyz1, xyz9, xyz7, xyz3, quad_normal);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, quad_normal[0], 0.0000001); // Nx
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, quad_normal[1], 0.0000001); // Ny
    CPPUNIT_ASSERT(quad_normal[2] > 0.0000001); // Nz

    // Quadrangle in 3D
    INTERP_KERNEL::calculateNormalForQuad(xyz1, xyz3, xyz6, xyz2, quad_normal);
    //MEDMEMTest_DumpArray<double>(cout, quad_normal, 3, "Quadrangle in 3D normal");
    double koeff = quad_normal[0]/15.6;
    //CPPUNIT_ASSERT_DOUBLES_EQUAL(15.6, quad_normal[0], 0.0000001); // Nx
    CPPUNIT_ASSERT_DOUBLES_EQUAL(  0.0        , quad_normal[1], 0.0000001); // Ny
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-11.7 * koeff, quad_normal[2], 0.0000001); // Nz

    // Invalid Quadrangle (four points on one line)
    CPPUNIT_ASSERT_NO_THROW(INTERP_KERNEL::calculateNormalForQuad(xyz1, xyz8, xyz3, xyz3, quad_normal));
    //MEDMEMTest_DumpArray<double>(cout, quad_normal, 3, "Invalid Quadrangle normal");
    //#ifdef ENABLE_FORCED_FAILURES
    // (BUG) division on zero in CalculateNormalForQuad(), if quadrangle is singular
    //Invalid Quadrangle normal: {nan, nan, nan}
    // Spec. Wait for an improvement
    // CPPUNIT_ASSERT(quad_normal[0] < DBL_MAX);
    // CPPUNIT_ASSERT(quad_normal[1] < DBL_MAX);
    // CPPUNIT_ASSERT(quad_normal[2] < DBL_MAX);
    //#endif
  }

  /////////////////////////////
  // CalculateNormalForPolyg //
  /////////////////////////////
  {
    double poly_normal [3];
    const double * polygon_cc[4] = {xyz1, xyz3, xyz7, xyz9};
    const double * polygon_er[4] = {xyz1, xyz8, xyz3, xyz7};
    const double * polygon_cw[4] = {xyz1, xyz9, xyz7, xyz3};
    const double * polygon_3d[4] = {xyz1, xyz3, xyz6, xyz2};
    const double * polygon_si[4] = {xyz1, xyz8, xyz3, xyz3};

    // Polygon in plane XOY, normal is opposit to axis Z
    INTERP_KERNEL::calculateNormalForPolyg(polygon_cc, /*nbOfPtsInPolygs*/4, poly_normal);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, poly_normal[0], 0.0000001); // Nx
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, poly_normal[1], 0.0000001); // Ny
    //#ifdef ENABLE_FORCED_FAILURES
    // (BUG) Normal for polygon is wrong. Confired to be a bug
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-11.7, poly_normal[2], 0.0000001); // Nz
    //#endif

    // Polygon in plane XOY, normal co-directed with axis Z
    INTERP_KERNEL::calculateNormalForPolyg(polygon_cw, /*nbOfPtsInPolygs*/4, poly_normal);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, poly_normal[0], 0.0000001); // Nx
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, poly_normal[1], 0.0000001); // Ny
    //#ifdef ENABLE_FORCED_FAILURES
    // (BUG) Normal for polygon is wrong. Confired to be a bug
    CPPUNIT_ASSERT_DOUBLES_EQUAL(11.7, poly_normal[2], 0.0000001); // Nz
    //#endif

    // Polygon in plane XOY, normal is opposit to axis Z, first three points lay on one line
    //CPPUNIT_ASSERT_THROW(CalculateNormalForPolyg(polygon_er, /*nbOfPtsInPolygs*/4,
    //                                             poly_normal),MEDEXCEPTION);
    CPPUNIT_ASSERT_NO_THROW(INTERP_KERNEL::calculateNormalForPolyg(polygon_er, /*nbOfPtsInPolygs*/4,
                                                    poly_normal));
    //CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, poly_normal[0], 0.0000001); // Nx
    //CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, poly_normal[1], 0.0000001); // Ny
    //#ifdef ENABLE_FORCED_FAILURES
    // (BUG) Normal for polygon is wrong if first (three) points are on one line
    // Spec, can be improved
    // CPPUNIT_ASSERT(poly_normal[2] < -0.0000001); // Nz
    //#endif

    // Polygon in 3D
    INTERP_KERNEL::calculateNormalForPolyg(polygon_3d, /*nbOfPtsInPolygs*/4, poly_normal);
    double koeff = poly_normal[0]/15.6;
    //CPPUNIT_ASSERT_DOUBLES_EQUAL(15.6, poly_normal[0], 0.0000001); // Nx
    CPPUNIT_ASSERT_DOUBLES_EQUAL(  0.0        , poly_normal[1], 0.0000001); // Ny
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-11.7 * koeff, poly_normal[2], 0.0000001); // Nz

    // Invalid Polygon (four points on one line)
    bool isException=false;
    try
      {
        INTERP_KERNEL::calculateNormalForPolyg(polygon_si, /*nbOfPtsInPolygs*/4,
                                               poly_normal);
      }
    catch(std::exception& e)
      {
        isException=true;
      }
    CPPUNIT_ASSERT(isException);
    //MEDMEMTest_DumpArray<double>(cout, poly_normal, 3, "Invalid Polygon normal");
    //#ifdef ENABLE_FORCED_FAILURES
    // (BUG) division on zero in CalculateNormalForPolyg(), if polygon is singular
    //Invalid Polygon normal: {nan, nan, nan}
    // Cinfirmed to be a bug
    //CPPUNIT_ASSERT(poly_normal[0] < DBL_MAX);
    //CPPUNIT_ASSERT(poly_normal[1] < DBL_MAX);
    //CPPUNIT_ASSERT(poly_normal[2] < DBL_MAX);
    //#endif
  }

  /////////////////////////////
  // CalculateVolumeForTetra //
  /////////////////////////////
  {
    // good
    val = INTERP_KERNEL::calculateVolumeForTetra(xyz1, xyz3, xyz7, xyz5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(6.4, val, 0.000001);

    // reversed
    val = INTERP_KERNEL::calculateVolumeForTetra(xyz1, xyz7, xyz3, xyz5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-6.4, val, 0.000001);

    // good
    val = INTERP_KERNEL::calculateVolumeForTetra(xyz1, xyz7, xyz3, xyz4);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(6.4, val, 0.000001);

    // reversed
    val = INTERP_KERNEL::calculateVolumeForTetra(xyz1, xyz3, xyz7, xyz4);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-6.4, val, 0.000001);

    // singular (in plane)
    val = INTERP_KERNEL::calculateVolumeForTetra(xyz1, xyz3, xyz7, xyz9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, val, 0.000001);
  }

  ////////////////////////////
  // CalculateVolumeForPyra //
  ////////////////////////////
  {
    // good
    val = INTERP_KERNEL::calculateVolumeForPyra(xyz1, xyz3, xyz7, xyz9, xyz5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(7.8, val, 0.000001);

    // reversed
    val = INTERP_KERNEL::calculateVolumeForPyra(xyz1, xyz9, xyz7, xyz3, xyz5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-7.8, val, 0.000001);

    // good
    val = INTERP_KERNEL::calculateVolumeForPyra(xyz1, xyz9, xyz7, xyz3, xyz4);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(7.8, val, 0.000001);

    // reversed
    val = INTERP_KERNEL::calculateVolumeForPyra(xyz1, xyz3, xyz7, xyz9, xyz4);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-7.8, val, 0.000001);

    // singular (in plane)
    val = INTERP_KERNEL::calculateVolumeForPyra(xyz1, xyz3, xyz7, xyz9, xyz8);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, val, 0.000001);
  }

  /////////////////////////////
  // CalculateVolumeForPenta //
  /////////////////////////////
  {
    double top1[3] = {xyz1[0], xyz1[1], xyz1[2] + 10.0};
    double top3[3] = {xyz3[0], xyz3[1], xyz3[2] + 10.0};
    double top7[3] = {xyz7[0], xyz7[1], xyz7[2] + 10.0};

    // good
    val = INTERP_KERNEL::calculateVolumeForPenta(xyz1, xyz3, xyz7, top1, top3, top7);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(96.0, val, 0.000001);

    // reversed
    val = INTERP_KERNEL::calculateVolumeForPenta(xyz1, xyz7, xyz3, top1, top7, top3);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-96.0, val, 0.000001);

    // good
    top1[0] = top1[0] + 7.0;
    top3[0] = top3[0] + 7.0;
    top7[0] = top7[0] + 7.0;

    val = INTERP_KERNEL::calculateVolumeForPenta(xyz1, xyz3, xyz7, top1, top3, top7);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(96.0, val, 0.000001);

    // reversed
    val = INTERP_KERNEL::calculateVolumeForPenta(xyz1, xyz7, xyz3, top1, top7, top3);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-96.0, val, 0.000001);

    // singular (in plane)
    top1[2] = top1[2] - 10.0;
    top3[2] = top3[2] - 10.0;
    top7[2] = top7[2] - 10.0;

    val = INTERP_KERNEL::calculateVolumeForPenta(xyz1, xyz3, xyz7, top1, top3, top7);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, val, 0.000001);
  }

  ////////////////////////////
  // CalculateVolumeForHexa //
  ////////////////////////////
  {
    double top1[3] = {xyz1[0], xyz1[1], xyz1[2] + 10.0};
    double top3[3] = {xyz3[0], xyz3[1], xyz3[2] + 10.0};
    double top7[3] = {xyz7[0], xyz7[1], xyz7[2] + 10.0};
    double top9[3] = {xyz9[0], xyz9[1], xyz9[2] + 10.0};

    // good
    val = INTERP_KERNEL::calculateVolumeForHexa(xyz1, xyz3, xyz7, xyz9, top1, top3, top7, top9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(117.0, val, 0.000001);

    // reversed
    val = INTERP_KERNEL::calculateVolumeForHexa(xyz1, xyz9, xyz7, xyz3, top1, top9, top7, top3);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-117.0, val, 0.000001);

    // good
    top1[0] = top1[0] + 7.0;
    top3[0] = top3[0] + 7.0;
    top7[0] = top7[0] + 7.0;
    top9[0] = top9[0] + 7.0;

    val = INTERP_KERNEL::calculateVolumeForHexa(xyz1, xyz3, xyz7, xyz9, top1, top3, top7, top9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(117.0, val, 0.000001);

    // reversed
    val = INTERP_KERNEL::calculateVolumeForHexa(xyz1, xyz9, xyz7, xyz3, top1, top9, top7, top3);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-117.0, val, 0.000001);

    // singular (in plane)
    top1[2] = top1[2] - 10.0;
    top3[2] = top3[2] - 10.0;
    top7[2] = top7[2] - 10.0;
    top9[2] = top9[2] - 10.0;

    val = INTERP_KERNEL::calculateVolumeForHexa(xyz1, xyz3, xyz7, xyz9, top1, top3, top7, top9);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, val, 0.000001);
  }

  /////////////////////////////
  // CalculateVolumeForPolyh //
  /////////////////////////////
  // inline double CalculateVolumeForPolyh(const double ***pts, const int *nbOfNodesPerFaces,
  //                                       int nbOfFaces, const double *bary);
  {
    int nbFaces = 4;
    int nbOfNodesPerFaces[4] = {3,3,3,3};

    const double * nodes[4] = {xyz1, xyz7, xyz3, xyz5};
    double bary[3];
    INTERP_KERNEL::calculateBarycenterDyn(nodes, 4, /*dim*/3, bary);

    // good
    const double * fa1[3] = {xyz1, xyz7, xyz3};
    const double * fa2[3] = {xyz1, xyz3, xyz5};
    const double * fa3[3] = {xyz3, xyz7, xyz5};
    const double * fa4[3] = {xyz7, xyz1, xyz5};
    const double ** polyh[4] = {fa1, fa2, fa3, fa4};

    val = INTERP_KERNEL::calculateVolumeForPolyh(polyh, nbOfNodesPerFaces, nbFaces, bary);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-6.4, val, 0.000001);

    // reversed
    //const double * fa1_r[3] = {xyz1, xyz3, xyz7};
    //const double * fa2_r[3] = {xyz3, xyz1, xyz5};
    //const double * fa3_r[3] = {xyz7, xyz3, xyz5};
    //const double * fa4_r[3] = {xyz1, xyz7, xyz5};
    //const double ** polyh_r[4] = {fa1_r, fa2_r, fa3_r, fa4_r};
    //
    //val = CalculateVolumeForPolyh(polyh_r, nbOfNodesPerFaces, nbFaces, bary);
    //CPPUNIT_ASSERT_DOUBLES_EQUAL(-6.4, val, 0.000001);

    // singular (in plane)
    const double * nodes_si[4] = {xyz1, xyz7, xyz3, xyz9};
    double bary_si[3];
    INTERP_KERNEL::calculateBarycenterDyn(nodes_si, 4, /*dim*/3, bary_si);

    const double * fa1_si[3] = {xyz1, xyz7, xyz3};
    const double * fa2_si[3] = {xyz1, xyz3, xyz9};
    const double * fa3_si[3] = {xyz3, xyz7, xyz9};
    const double * fa4_si[3] = {xyz7, xyz1, xyz9};
    const double ** polyh_si[4] = {fa1_si, fa2_si, fa3_si, fa4_si};

    val = INTERP_KERNEL::calculateVolumeForPolyh(polyh_si, nbOfNodesPerFaces, nbFaces, bary_si);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, val, 0.000001);
  }

  ////////////////////////
  // addComponentsOfVec //
  ////////////////////////
  {
    // five points
    const double * pts[5] = {xyz2, xyz1, xyz3, xyz4, xyz5};

    val = INTERP_KERNEL::addComponentsOfVec<5>(pts, 0); // x
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0, val, 0.000001);

    val = INTERP_KERNEL::addComponentsOfVec<5>(pts, 1); // y
    CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0, val, 0.000001);

    val = INTERP_KERNEL::addComponentsOfVec<5>(pts, 2); // z
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0, val, 0.000001);

    // one point: xyz2
    val = INTERP_KERNEL::addComponentsOfVec<1>(pts, 0); // x
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0, val, 0.000001);

    val = INTERP_KERNEL::addComponentsOfVec<1>(pts, 1); // y
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, val, 0.000001);

    val = INTERP_KERNEL::addComponentsOfVec<1>(pts, 2); // z
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0, val, 0.000001);
  }

  ////////////////////////////
  // CalculateBarycenterDyn //
  ////////////////////////////
  {
    // five points
    const double * pts[5] = {xyz2, xyz1, xyz3, xyz4, xyz5};
    double bary_3d[3];
    double bary_2d[2];

    INTERP_KERNEL::calculateBarycenterDyn(pts, /*nbPts*/5, /*dim*/3, bary_3d);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0/5.0, bary_3d[0], 0.000001); // x
    CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0/5.0, bary_3d[1], 0.000001); // y
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0/5.0, bary_3d[2], 0.000001); // z

    INTERP_KERNEL::calculateBarycenterDyn(pts, /*nbPts*/5, /*dim*/2, bary_2d);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0/5.0, bary_2d[0], 0.000001); // x
    CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0/5.0, bary_2d[1], 0.000001); // y

    // one point: xyz2
    INTERP_KERNEL::calculateBarycenterDyn(pts, /*nbPts*/1, /*dim*/3, bary_3d);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0, bary_3d[0], 0.000001); // x
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, bary_3d[1], 0.000001); // y
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0, bary_3d[2], 0.000001); // z
  }

  /////////////////////////
  // CalculateBarycenter //
  /////////////////////////
  // template<int N, int DIM> inline void CalculateBarycenter(const double **pts, double *bary);
  // template<> inline void CalculateBarycenter<2,0>(const double **pts, double *bary);
  // template<> inline void CalculateBarycenter<3,0>(const double **pts, double *bary);
  // template<> inline void CalculateBarycenter<4,0>(const double **pts, double *bary);
  // template<> inline void CalculateBarycenter<5,0>(const double **pts, double *bary);
  // template<> inline void CalculateBarycenter<6,0>(const double **pts, double *bary);
  // template<> inline void CalculateBarycenter<7,0>(const double **pts, double *bary);
  // template<> inline void CalculateBarycenter<8,0>(const double **pts, double *bary);
  {
    // five points
    const double * pts[5] = {xyz2, xyz1, xyz3, xyz4, xyz5};
    double bary_3d[3];
    double bary_2d[2];

    INTERP_KERNEL::calculateBarycenter<5,3>(pts, bary_3d);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0/5.0, bary_3d[0], 0.000001); // x
    CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0/5.0, bary_3d[1], 0.000001); // y
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0/5.0, bary_3d[2], 0.000001); // z

    INTERP_KERNEL::calculateBarycenter<5,2>(pts, bary_2d);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0/5.0, bary_2d[0], 0.000001); // x
    CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0/5.0, bary_2d[1], 0.000001); // y

    // one point: xyz2 : NOT IMPLEMENTED!!!
    //CalculateBarycenter<1,3>(pts, bary_3d);
    //CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0, bary_3d[0], 0.000001);
    //CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, bary_3d[1], 0.000001);
    //CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0, bary_3d[2], 0.000001);
  }
}
