//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D
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
#ifndef __VOLSURFFORMULAE_HXX__
#define __VOLSURFFORMULAE_HXX__

#include <math.h>

namespace INTERP_KERNEL
{
  inline void calculateBarycenterDyn(const double **pts, int nbPts,
                                     int dim, double *bary);

  inline double calculateAreaForPolyg(const double **coords, int nbOfPtsInPolygs,
                                      int spaceDim);


  // ===========================
  // Calculate Area for triangle
  // ===========================
  inline double calculateAreaForTria(const double *p1, const double *p2,
                                     const double *p3, int spaceDim)
  {
    double area ;

    if ( spaceDim == 2 )
      {
        area = -((p2[0]-p1[0])*(p3[1]-p1[1]) - (p3[0]-p1[0])*(p2[1]-p1[1]))/2.0;
      }
    else
      {
        area = sqrt(((p2[1]-p1[1])*(p3[2]-p1[2]) - (p3[1]-p1[1])*(p2[2]-p1[2]))*
                    ((p2[1]-p1[1])*(p3[2]-p1[2]) - (p3[1]-p1[1])*(p2[2]-p1[2]))
                    +
                    ((p3[0]-p1[0])*(p2[2]-p1[2]) - (p2[0]-p1[0])*(p3[2]-p1[2]))*
                    ((p3[0]-p1[0])*(p2[2]-p1[2]) - (p2[0]-p1[0])*(p3[2]-p1[2]))
                    +
                    ((p2[0]-p1[0])*(p3[1]-p1[1]) - (p3[0]-p1[0])*(p2[1]-p1[1]))*
                    ((p2[0]-p1[0])*(p3[1]-p1[1]) - (p3[0]-p1[0])*(p2[1]-p1[1])))/2.0;
      }

    return area ;
  }

  // =============================
  // Calculate Area for quadrangle
  // =============================
  inline double calculateAreaForQuad(const double *p1, const double *p2,
                                     const double *p3, const double *p4,
                                     int spaceDim)
  {
    double area ;

    if (spaceDim==2)
      {
        double a1 = (p2[0]-p1[0])/4.0, a2 = (p2[1]-p1[1])/4.0;
        double b1 = (p3[0]-p4[0])/4.0, b2 = (p3[1]-p4[1])/4.0;
        double c1 = (p3[0]-p2[0])/4.0, c2 = (p3[1]-p2[1])/4.0;
        double d1 = (p4[0]-p1[0])/4.0, d2 = (p4[1]-p1[1])/4.0;

        area = - 4.0*(  b1*c2 - c1*b2 + a1*c2 - c1*a2
                        + b1*d2 - d1*b2 + a1*d2 - d1*a2);
      }
    else
      {
        area = (sqrt(((p2[1]-p1[1])*(p4[2]-p1[2]) - (p4[1]-p1[1])*(p2[2]-p1[2]))*
                     ((p2[1]-p1[1])*(p4[2]-p1[2]) - (p4[1]-p1[1])*(p2[2]-p1[2]))
                     + ((p4[0]-p1[0])*(p2[2]-p1[2]) - (p2[0]-p1[0])*(p4[2]-p1[2]))*
                     ((p4[0]-p1[0])*(p2[2]-p1[2]) - (p2[0]-p1[0])*(p4[2]-p1[2]))
                     + ((p2[0]-p1[0])*(p4[1]-p1[1]) - (p4[0]-p1[0])*(p2[1]-p1[1]))*
                     ((p2[0]-p1[0])*(p4[1]-p1[1]) - (p4[0]-p1[0])*(p2[1]-p1[1])))
                +
                sqrt(((p4[1]-p3[1])*(p2[2]-p3[2]) - (p2[1]-p3[1])*(p4[2]-p3[2]))*
                     ((p4[1]-p3[1])*(p2[2]-p3[2]) - (p2[1]-p3[1])*(p4[2]-p3[2]))
                     + ((p2[0]-p3[0])*(p4[2]-p3[2]) - (p4[0]-p3[0])*(p2[2]-p3[2]))*
                     ((p2[0]-p3[0])*(p4[2]-p3[2]) - (p4[0]-p3[0])*(p2[2]-p3[2]))
                     + ((p4[0]-p3[0])*(p2[1]-p3[1]) - (p2[0]-p3[0])*(p4[1]-p3[1]))*
                     ((p4[0]-p3[0])*(p2[1]-p3[1]) - (p2[0]-p3[0])*(p4[1]-p3[1])))
                )/2.0;
      }

    return area ;
  }

  // ====================================
  // Calculate Normal Vector for Triangle
  // ====================================
  inline void calculateNormalForTria(const double *p1, const double *p2,
                                     const double *p3, double *normal)
  {
    normal[0] = ((p2[1]-p1[1])*(p3[2]-p1[2]) - (p3[1]-p1[1])*(p2[2]-p1[2]))/2.0;
    normal[1] = ((p3[0]-p1[0])*(p2[2]-p1[2]) - (p2[0]-p1[0])*(p3[2]-p1[2]))/2.0;
    normal[2] = ((p2[0]-p1[0])*(p3[1]-p1[1]) - (p3[0]-p1[0])*(p2[1]-p1[1]))/2.0;
  }

  // ======================================
  // Calculate Normal Vector for Quadrangle
  // ======================================
  inline void calculateNormalForQuad(const double *p1, const double *p2,
                                     const double *p3, const double *p4,
                                     double *normal)
  {
    double xnormal1 = (p2[1]-p1[1])*(p4[2]-p1[2]) - (p4[1]-p1[1])*(p2[2]-p1[2]);
    double xnormal2 = (p4[0]-p1[0])*(p2[2]-p1[2]) - (p2[0]-p1[0])*(p4[2]-p1[2]);
    double xnormal3 = (p2[0]-p1[0])*(p4[1]-p1[1]) - (p4[0]-p1[0])*(p2[1]-p1[1]);
    double xarea = sqrt(xnormal1*xnormal1 + xnormal2*xnormal2 + xnormal3*xnormal3);
    xnormal1 = xnormal1/xarea;
    xnormal2 = xnormal2/xarea;
    xnormal3 = xnormal3/xarea;
    xarea = calculateAreaForQuad(p1,p2,p3,p4,3);
    normal[0] = xnormal1*xarea ;
    normal[1] = xnormal2*xarea ;
    normal[2] = xnormal3*xarea ;
  }

  // ===================================
  // Calculate Normal Vector for Polygon
  // ===================================
  inline void calculateNormalForPolyg(const double **coords, int nbOfPtsInPolygs,
                                      double *normal)
  {
    double coordOfBary[3];

    calculateBarycenterDyn(coords,nbOfPtsInPolygs,3,coordOfBary);
    double xnormal1 = (coords[0][1]-coords[1][1]) * (coordOfBary[2]-coords[1][2])
      - (coords[0][2]-coords[1][2]) * (coordOfBary[1]-coords[1][1]);

    double xnormal2 = (coords[0][2]-coords[1][2]) * (coordOfBary[0]-coords[1][0])
      - (coords[0][0]-coords[1][0]) * (coordOfBary[2]-coords[1][2]);

    double xnormal3 = (coords[0][0]-coords[1][0]) * (coordOfBary[1]-coords[1][1])
      - (coords[0][1]-coords[1][1]) * (coordOfBary[0]-coords[1][0]);

    double xarea=sqrt(xnormal1*xnormal1 + xnormal2*xnormal2 + xnormal3*xnormal3);

    if ( xarea < 1.e-6 )
      {
        //std::string diagnosis"Can not calculate normal - polygon is singular";
        throw std::exception();
      }

    xnormal1 = -xnormal1/xarea;
    xnormal2 = -xnormal2/xarea;
    xnormal3 = -xnormal3/xarea;
    xarea = calculateAreaForPolyg(coords,nbOfPtsInPolygs,3);
    normal[0] = xnormal1*xarea ;
    normal[1] = xnormal2*xarea ;
    normal[2] = xnormal3*xarea ;
  }

  // ==========================
  // Calculate Area for Polygon
  // ==========================
  inline double calculateAreaForPolyg(const double **coords, int nbOfPtsInPolygs,
                                      int spaceDim)
  {
    double ret=0.;
    double coordOfBary[3];

    calculateBarycenterDyn(coords,nbOfPtsInPolygs,spaceDim,coordOfBary);
    for ( int i=0; i<nbOfPtsInPolygs; i++ )
      {
        double tmp = calculateAreaForTria(coords[i],coords[(i+1)%nbOfPtsInPolygs],
                                          coordOfBary,spaceDim);
        ret+=tmp;
      }
    return ret;
  }

  // ==========================
  // Calculate Volume for Tetra
  // ==========================
  inline double calculateVolumeForTetra(const double *p1, const double *p2,
                                        const double *p3, const double *p4)
  {
    return (  (p3[0]-p1[0])*(  (p2[1]-p1[1])*(p4[2]-p1[2])
                               - (p2[2]-p1[2])*(p4[1]-p1[1]) )
              - (p2[0]-p1[0])*(  (p3[1]-p1[1])*(p4[2]-p1[2])
                                 - (p3[2]-p1[2])*(p4[1]-p1[1]) )
              + (p4[0]-p1[0])*(  (p3[1]-p1[1])*(p2[2]-p1[2])
                                 - (p3[2]-p1[2])*(p2[1]-p1[1]) )
              ) / 6.0 ;
  }

  // =========================
  // Calculate Volume for Pyra
  // =========================
  inline double calculateVolumeForPyra(const double *p1, const double *p2,
                                       const double *p3, const double *p4,
                                       const double *p5)
  {
    return ( ((p3[0]-p1[0])*(  (p2[1]-p1[1])*(p5[2]-p1[2])
                               - (p2[2]-p1[2])*(p5[1]-p1[1]) )
              -(p2[0]-p1[0])*(  (p3[1]-p1[1])*(p5[2]-p1[2])
                                - (p3[2]-p1[2])*(p5[1]-p1[1]) )
              +(p5[0]-p1[0])*(  (p3[1]-p1[1])*(p2[2]-p1[2])
                                - (p3[2]-p1[2])*(p2[1]-p1[1]) ))
             +
             ((p4[0]-p1[0])*(  (p3[1]-p1[1])*(p5[2]-p1[2])
                               - (p3[2]-p1[2])*(p5[1]-p1[1]) )
              -(p3[0]-p1[0])*(  (p4[1]-p1[1])*(p5[2]-p1[2])
                                - (p4[2]-p1[2])*(p5[1]-p1[1]))
              +(p5[0]-p1[0])*(  (p4[1]-p1[1])*(p3[2]-p1[2])
                                - (p4[2]-p1[2])*(p3[1]-p1[1]) ))
             ) / 6.0 ;
  }

  // ==========================
  // Calculate Volume for Penta
  // ==========================
  inline double calculateVolumeForPenta(const double *p1, const double *p2,
                                        const double *p3, const double *p4,
                                        const double *p5, const double *p6)
  {
    double a1 = (p2[0]-p3[0])/2.0, a2 = (p2[1]-p3[1])/2.0, a3 = (p2[2]-p3[2])/2.0;
    double b1 = (p5[0]-p6[0])/2.0, b2 = (p5[1]-p6[1])/2.0, b3 = (p5[2]-p6[2])/2.0;
    double c1 = (p4[0]-p1[0])/2.0, c2 = (p4[1]-p1[1])/2.0, c3 = (p4[2]-p1[2])/2.0;
    double d1 = (p5[0]-p2[0])/2.0, d2 = (p5[1]-p2[1])/2.0, d3 = (p5[2]-p2[2])/2.0;
    double e1 = (p6[0]-p3[0])/2.0, e2 = (p6[1]-p3[1])/2.0, e3 = (p6[2]-p3[2])/2.0;
    double f1 = (p1[0]-p3[0])/2.0, f2 = (p1[1]-p3[1])/2.0, f3 = (p1[2]-p3[2])/2.0;
    double h1 = (p4[0]-p6[0])/2.0, h2 = (p4[1]-p6[1])/2.0, h3 = (p4[2]-p6[2])/2.0;

    double A = a1*c2*f3 - a1*c3*f2 - a2*c1*f3 + a2*c3*f1 + a3*c1*f2 - a3*c2*f1;
    double B = b1*c2*h3 - b1*c3*h2 - b2*c1*h3 + b2*c3*h1 + b3*c1*h2 - b3*c2*h1;
    double C = (a1*c2*h3 + b1*c2*f3) - (a1*c3*h2 + b1*c3*f2)
      - (a2*c1*h3 + b2*c1*f3) + (a2*c3*h1 + b2*c3*f1)
      + (a3*c1*h2 + b3*c1*f2) - (a3*c2*h1 + b3*c2*f1);
    double D = a1*d2*f3 - a1*d3*f2 - a2*d1*f3 + a2*d3*f1 + a3*d1*f2 - a3*d2*f1;
    double E = b1*d2*h3 - b1*d3*h2 - b2*d1*h3 + b2*d3*h1 + b3*d1*h2 - b3*d2*h1;
    double F = (a1*d2*h3 + b1*d2*f3) - (a1*d3*h2 + b1*d3*f2)
      - (a2*d1*h3 + b2*d1*f3) + (a2*d3*h1 + b2*d3*f1)
      + (a3*d1*h2 + b3*d1*f2) - (a3*d2*h1 + b3*d2*f1);
    double G = a1*e2*f3 - a1*e3*f2 - a2*e1*f3 + a2*e3*f1 + a3*e1*f2 - a3*e2*f1;
    double H = b1*e2*h3 - b1*e3*h2 - b2*e1*h3 + b2*e3*h1 + b3*e1*h2 - b3*e2*h1;
    double P = (a1*e2*h3 + b1*e2*f3) - (a1*e3*h2 + b1*e3*f2)
      - (a2*e1*h3 + b2*e1*f3) + (a2*e3*h1 + b2*e3*f1)
      + (a3*e1*h2 + b3*e1*f2) - (a3*e2*h1 + b3*e2*f1);

    return (-2.0*(2.0*(A + B + D + E + G + H) + C + F + P)/9.0);
  }

  // =========================
  // Calculate Volume for Hexa
  // =========================
  inline double calculateVolumeForHexa(const double *pt1, const double *pt2,
                                       const double *pt3, const double *pt4,
                                       const double *pt5, const double *pt6,
                                       const double *pt7, const double *pt8)
  {
    double a1=(pt3[0]-pt4[0])/8.0, a2=(pt3[1]-pt4[1])/8.0, a3=(pt3[2]-pt4[2])/8.0;
    double b1=(pt2[0]-pt1[0])/8.0, b2=(pt2[1]-pt1[1])/8.0, b3=(pt2[2]-pt1[2])/8.0;
    double c1=(pt7[0]-pt8[0])/8.0, c2=(pt7[1]-pt8[1])/8.0, c3=(pt7[2]-pt8[2])/8.0;
    double d1=(pt6[0]-pt5[0])/8.0, d2=(pt6[1]-pt5[1])/8.0, d3=(pt6[2]-pt5[2])/8.0;
    double e1=(pt3[0]-pt2[0])/8.0, e2=(pt3[1]-pt2[1])/8.0, e3=(pt3[2]-pt2[2])/8.0;
    double f1=(pt4[0]-pt1[0])/8.0, f2=(pt4[1]-pt1[1])/8.0, f3=(pt4[2]-pt1[2])/8.0;
    double h1=(pt7[0]-pt6[0])/8.0, h2=(pt7[1]-pt6[1])/8.0, h3=(pt7[2]-pt6[2])/8.0;
    double p1=(pt8[0]-pt5[0])/8.0, p2=(pt8[1]-pt5[1])/8.0, p3=(pt8[2]-pt5[2])/8.0;
    double q1=(pt3[0]-pt7[0])/8.0, q2=(pt3[1]-pt7[1])/8.0, q3=(pt3[2]-pt7[2])/8.0;
    double r1=(pt4[0]-pt8[0])/8.0, r2=(pt4[1]-pt8[1])/8.0, r3=(pt4[2]-pt8[2])/8.0;
    double s1=(pt2[0]-pt6[0])/8.0, s2=(pt2[1]-pt6[1])/8.0, s3=(pt2[2]-pt6[2])/8.0;
    double t1=(pt1[0]-pt5[0])/8.0, t2=(pt1[1]-pt5[1])/8.0, t3=(pt1[2]-pt5[2])/8.0;

    double A = a1*e2*q3 - a1*e3*q2 - a2*e1*q3 + a2*e3*q1 + a3*e1*q2 - a3*e2*q1;
    double B = c1*h2*q3 - c1*h3*q2 - c2*h1*q3 + c2*h3*q1 + c3*h1*q2 - c3*h2*q1;
    double C = (a1*h2 + c1*e2)*q3 - (a1*h3 + c1*e3)*q2
      - (a2*h1 + c2*e1)*q3 + (a2*h3 + c2*e3)*q1
      + (a3*h1 + c3*e1)*q2 - (a3*h2 + c3*e2)*q1;
    double D = b1*e2*s3 - b1*e3*s2 - b2*e1*s3 + b2*e3*s1 + b3*e1*s2 - b3*e2*s1;
    double E = d1*h2*s3 - d1*h3*s2 - d2*h1*s3 + d2*h3*s1 + d3*h1*s2 - d3*h2*s1;
    double F = (b1*h2 + d1*e2)*s3 - (b1*h3 + d1*e3)*s2
      - (b2*h1 + d2*e1)*s3 + (b2*h3 + d2*e3)*s1
      + (b3*h1 + d3*e1)*s2 - (b3*h2 + d3*e2)*s1;
    double G = (a1*e2*s3 + b1*e2*q3) - (a1*e3*s2 + b1*e3*q2)
      - (a2*e1*s3 + b2*e1*q3) + (a2*e3*s1 + b2*e3*q1)
      + (a3*e1*s2 + b3*e1*q2) - (a3*e2*s1 + b3*e2*q1);
    double H = (c1*h2*s3 + d1*h2*q3) - (c1*h3*s2 + d1*h3*q2)
      - (c2*h1*s3 + d2*h1*q3) + (c2*h3*s1 + d2*h3*q1)
      + (c3*h1*s2 + d3*h1*q2) - (c3*h2*s1 + d3*h2*q1);
    double I = ((a1*h2 + c1*e2)*s3 + (b1*h2 + d1*e2)*q3)
      - ((a1*h3 + c1*e3)*s2 + (b1*h3 + d1*e3)*q2)
      - ((a2*h1 + c2*e1)*s3 + (b2*h1 + d2*e1)*q3)
      + ((a2*h3 + c2*e3)*s1 + (b2*h3 + d2*e3)*q1)
      + ((a3*h1 + c3*e1)*s2 + (b3*h1 + d3*e1)*q2)
      - ((a3*h2 + c3*e2)*s1 + (b3*h2 + d3*e2)*q1);
    double J = a1*f2*r3 - a1*f3*r2 - a2*f1*r3 + a2*f3*r1 + a3*f1*r2 - a3*f2*r1;
    double K = c1*p2*r3 - c1*p3*r2 - c2*p1*r3 + c2*p3*r1 + c3*p1*r2 - c3*p2*r1;
    double L = (a1*p2 + c1*f2)*r3 - (a1*p3 + c1*f3)*r2
      - (a2*p1 + c2*f1)*r3 + (a2*p3 + c2*f3)*r1
      + (a3*p1 + c3*f1)*r2 - (a3*p2 + c3*f2)*r1;
    double M = b1*f2*t3 - b1*f3*t2 - b2*f1*t3 + b2*f3*t1 + b3*f1*t2 - b3*f2*t1;
    double N = d1*p2*t3 - d1*p3*t2 - d2*p1*t3 + d2*p3*t1 + d3*p1*t2 - d3*p2*t1;
    double O = (b1*p2 + d1*f2)*t3 - (b1*p3 + d1*f3)*t2
      - (b2*p1 + d2*f1)*t3 + (b2*p3 + d2*f3)*t1
      + (b3*p1 + d3*f1)*t2 - (b3*p2 + d3*f2)*t1;
    double P = (a1*f2*t3 + b1*f2*r3) - (a1*f3*t2 + b1*f3*r2)
      - (a2*f1*t3 + b2*f1*r3) + (a2*f3*t1 + b2*f3*r1)
      + (a3*f1*t2 + b3*f1*r2) - (a3*f2*t1 + b3*f2*r1);
    double Q = (c1*p2*t3 + d1*p2*r3) - (c1*p3*t2 + d1*p3*r2)
      - (c2*p1*t3 + d2*p1*r3) + (c2*p3*t1 + d2*p3*r1)
      + (c3*p1*t2 + d3*p1*r2) - (c3*p2*t1 + d3*p2*r1);
    double R = ((a1*p2 + c1*f2)*t3 + (b1*p2 + d1*f2)*r3)
      - ((a1*p3 + c1*f3)*t2 + (b1*p3 + d1*f3)*r2)
      - ((a2*p1 + c2*f1)*t3 + (b2*p1 + d2*f1)*r3)
      + ((a2*p3 + c2*f3)*t1 + (b2*p3 + d2*f3)*r1)
      + ((a3*p1 + c3*f1)*t2 + (b3*p1 + d3*f1)*r2)
      - ((a3*p2 + c3*f2)*t1 + (b3*p2 + d3*f2)*r1);
    double S = (a1*e2*r3 + a1*f2*q3) - (a1*e3*r2 + a1*f3*q2)
      - (a2*e1*r3 + a2*f1*q3) + (a2*e3*r1 + a2*f3*q1)
      + (a3*e1*r2 + a3*f1*q2) - (a3*e2*r1 + a3*f2*q1);
    double T = (c1*h2*r3 + c1*p2*q3) - (c1*h3*r2 + c1*p3*q2)
      - (c2*h1*r3 + c2*p1*q3) + (c2*h3*r1 + c2*p3*q1)
      + (c3*h1*r2 + c3*p1*q2) - (c3*h2*r1 + c3*p2*q1);
    double U = ((a1*h2 + c1*e2)*r3 + (a1*p2 + c1*f2)*q3)
      - ((a1*h3 + c1*e3)*r2 + (a1*p3 + c1*f3)*q2)
      - ((a2*h1 + c2*e1)*r3 + (a2*p1 + c2*f1)*q3)
      + ((a2*h3 + c2*e3)*r1 + (a2*p3 + c2*f3)*q1)
      + ((a3*h1 + c3*e1)*r2 + (a3*p1 + c3*f1)*q2)
      - ((a3*h2 + c3*e2)*r1 + (a3*p2 + c3*f2)*q1);
    double V = (b1*e2*t3 + b1*f2*s3) - (b1*e3*t2 + b1*f3*s2)
      - (b2*e1*t3 + b2*f1*s3) + (b2*e3*t1 + b2*f3*s1)
      + (b3*e1*t2 + b3*f1*s2) - (b3*e2*t1 + b3*f2*s1);
    double W = (d1*h2*t3 + d1*p2*s3) - (d1*h3*t2 + d1*p3*s2)
      - (d2*h1*t3 + d2*p1*s3) + (d2*h3*t1 + d2*p3*s1)
      + (d3*h1*t2 + d3*p1*s2) - (d3*h2*t1 + d3*p2*s1);
    double X = ((b1*h2 + d1*e2)*t3 + (b1*p2 + d1*f2)*s3)
      - ((b1*h3 + d1*e3)*t2 + (b1*p3 + d1*f3)*s2)
      - ((b2*h1 + d2*e1)*t3 + (b2*p1 + d2*f1)*s3)
      + ((b2*h3 + d2*e3)*t1 + (b2*p3 + d2*f3)*s1)
      + ((b3*h1 + d3*e1)*t2 + (b3*p1 + d3*f1)*s2)
      - ((b3*h2 + d3*e2)*t1 + (b3*p2 + d3*f2)*s1);
    double Y = (a1*e2*t3 + a1*f2*s3 + b1*e2*r3 + b1*f2*q3)
      - (a1*e3*t2 + a1*f3*s2 + b1*e3*r2 + b1*f3*q2)
      - (a2*e1*t3 + a2*f1*s3 + b2*e1*r3 + b2*f1*q3)
      + (a2*e3*t1 + a2*f3*s1 + b2*e3*r1 + b2*f3*q1)
      + (a3*e1*t2 + a3*f1*s2 + b3*e1*r2 + b3*f1*q2)
      - (a3*e2*t1 + a3*f2*s1 + b3*e2*r1 + b3*f2*q1);
    double Z = (c1*h2*t3 + c1*p2*s3 + d1*h2*r3 + d1*p2*q3)
      - (c1*h3*t2 + c1*p3*s2 + d1*h3*r2 + d1*p3*q2)
      - (c2*h1*t3 + c2*p1*s3 + d2*h1*r3 + d2*p1*q3)
      + (c2*h3*t1 + c2*p3*s1 + d2*h3*r1 + d2*p3*q1)
      + (c3*h1*t2 + c3*p1*s2 + d3*h1*r2 + d3*p1*q2)
      - (c3*h2*t1 + c3*p2*s1 + d3*h2*r1 + d3*p2*q1);
    double AA = ((a1*h2 + c1*e2)*t3 + (a1*p2 + c1*f2)*s3
                 +(b1*h2 + d1*e2)*r3 + (b1*p2 + d1*f2)*q3)
      - ((a1*h3 + c1*e3)*t2 + (a1*p3 + c1*f3)*s2
         +(b1*h3 + d1*e3)*r2 + (b1*p3 + d1*f3)*q2)
      - ((a2*h1 + c2*e1)*t3 + (a2*p1 + c2*f1)*s3
         +(b2*h1 + d2*e1)*r3 + (b2*p1 + d2*f1)*q3)
      + ((a2*h3 + c2*e3)*t1 + (a2*p3 + c2*f3)*s1
         +(b2*h3 + d2*e3)*r1 + (b2*p3 + d2*f3)*q1)
      + ((a3*h1 + c3*e1)*t2 + (a3*p1 + c3*f1)*s2
         +(b3*h1 + d3*e1)*r2 + (b3*p1 + d3*f1)*q2)
      - ((a3*h2 + c3*e2)*t1 + (a3*p2 + c3*f2)*s1
         + (b3*h2 + d3*e2)*r1 + (b3*p2 + d3*f2)*q1);

    return  64.0*(  8.0*(A + B + D + E + J + K + M + N)
                    + 4.0*(C + F + G + H + L + O + P + Q + S + T + V + W)
                    + 2.0*(I + R + U + X + Y + Z) + AA ) / 27.0 ;
  }

  // =========================================================================================================================
  // Calculate Volume for Generic Polyedron, even not convex one, WARNING !!! The polyedron's faces must be correctly ordered
  // =========================================================================================================================
  inline double calculateVolumeForPolyh(const double ***pts,
                                        const int *nbOfNodesPerFaces,
                                        int nbOfFaces,
                                        const double *bary)
  {
    double volume=0.;

    for ( int i=0; i<nbOfFaces; i++ )
      {
        double normal[3];
        double vecForAlt[3];

        calculateNormalForPolyg(pts[i],nbOfNodesPerFaces[i],normal);
        vecForAlt[0]=bary[0]-pts[i][0][0];
        vecForAlt[1]=bary[1]-pts[i][0][1];
        vecForAlt[2]=bary[2]-pts[i][0][2];
        volume+=vecForAlt[0]*normal[0]+vecForAlt[1]*normal[1]+vecForAlt[2]*normal[2];
      }
    return -volume/3.;
  }

  // ============================================================================================================================================
  // Calculate Volume for NON Generic Polyedron. Only polydrons with bary included in pts is supported by this method. Result is always positive.
  // ============================================================================================================================================
  inline double calculateVolumeForPolyhAbs(const double ***pts,
                                           const int *nbOfNodesPerFaces,
                                           int nbOfFaces,
                                           const double *bary)
  {
    double volume=0.;
    
    for ( int i=0; i<nbOfFaces; i++ )
      {
        double normal[3];
        double vecForAlt[3];

        calculateNormalForPolyg(pts[i],nbOfNodesPerFaces[i],normal);
        vecForAlt[0]=bary[0]-pts[i][0][0];
        vecForAlt[1]=bary[1]-pts[i][0][1];
        vecForAlt[2]=bary[2]-pts[i][0][2];
        volume+=fabs(vecForAlt[0]*normal[0]+vecForAlt[1]*normal[1]+vecForAlt[2]*normal[2]);
      }
    return volume/3.;
  }

  template<int N>
  inline double addComponentsOfVec(const double **pts, int rk)
  {
    return pts[N-1][rk]+addComponentsOfVec<N-1>(pts,rk);
  }

  template<>
  inline double addComponentsOfVec<1>(const double **pts, int rk)
  {
    return pts[0][rk];
  }

  template<int N, int DIM>
  inline void calculateBarycenter(const double **pts, double *bary)
  {
    bary[DIM-1]=addComponentsOfVec<N>(pts,DIM-1)/N;
    calculateBarycenter<N,DIM-1>(pts,bary);
  }

  template<>
  inline void calculateBarycenter<2,0>(const double **pts, double *bary)
  {
  }
  
  template<>
  inline void calculateBarycenter<3,0>(const double **pts, double *bary)
  {
  }
  
  template<>
  inline void calculateBarycenter<4,0>(const double **pts, double *bary)
  {
  }
  
  template<>
  inline void calculateBarycenter<5,0>(const double **pts, double *bary)
  {
  }
  
  template<>
  inline void calculateBarycenter<6,0>(const double **pts, double *bary)
  {
  }
  
  template<>
  inline void calculateBarycenter<7,0>(const double **pts, double *bary)
  {
  }
  
  template<>
  inline void calculateBarycenter<8,0>(const double **pts, double *bary)
  {
  }

  inline void calculateBarycenterDyn(const double **pts, int nbPts,
                                     int dim, double *bary)
  {
    for(int i=0;i<dim;i++)
      {
        double temp=0.;
        for(int j=0;j<nbPts;j++)
          {
            temp+=pts[j][i];
          }
        bary[i]=temp/nbPts;
      }
  }
}

#endif
