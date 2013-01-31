// Copyright (C) 2007-2012  CEA/DEN, EDF R&D
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
// Author : Anthony Geay (CEA/DEN)

#include "VolSurfUser.hxx"

#include <cmath>
#include <limits>

namespace INTERP_KERNEL
{
  double DistanceFromPtToSegInSpaceDim2(const double *pt, const double *pt0Seg2, const double *pt1Seg2) throw(INTERP_KERNEL::Exception)
  {
    double dx=pt1Seg2[0]-pt0Seg2[0],dy=pt1Seg2[1]-pt0Seg2[1];
    double norm=sqrt(dx*dx+dy*dy);
    if(norm==0.)
      return std::numeric_limits<double>::max();
    dx/=norm; dy/=norm;
    double dx2=pt[0]-pt0Seg2[0],dy2=pt[1]-pt0Seg2[1];
    double dotP=(dx2*dx+dy2*dy);
    if(dotP<0. || dotP>norm)
      return std::numeric_limits<double>::max();
    double x=pt0Seg2[0]+dotP*dx,y=pt0Seg2[1]+dotP*dy;
    return sqrt((x-pt[0])*(x-pt[0])+(y-pt[1])*(y-pt[1]));
  }

  double DistanceFromPtToTriInSpaceDim3(const double *pt, const double *pt0Tri3, const double *pt1Tri3, const double *pt2Tri3) throw(INTERP_KERNEL::Exception)
  {
    double matrix[12];
    if(!ComputeRotTranslationMatrixToPut3PointsOnOXY(pt0Tri3,pt1Tri3,pt2Tri3,matrix))
      return std::numeric_limits<double>::max();
    double xy0[2],xy1[2],xy2[2],xy[2]; xy0[0]=0.; xy0[1]=0.;
    xy1[0]=matrix[0]*pt1Tri3[0]+matrix[1]*pt1Tri3[1]+matrix[2]*pt1Tri3[2]+matrix[3]; xy1[1]=0.;
    xy2[0]=matrix[0]*pt2Tri3[0]+matrix[1]*pt2Tri3[1]+matrix[2]*pt2Tri3[2]+matrix[3];
    xy2[1]=matrix[4]*pt2Tri3[0]+matrix[5]*pt2Tri3[1]+matrix[6]*pt2Tri3[2]+matrix[7];
    xy[0]=matrix[0]*pt[0]+matrix[1]*pt[1]+matrix[2]*pt[2]+matrix[3];
    xy[1]=matrix[4]*pt[0]+matrix[5]*pt[1]+matrix[6]*pt[2]+matrix[7];
    double z=matrix[8]*pt[0]+matrix[9]*pt[1]+matrix[10]*pt[2]+matrix[11];
    double ret=std::numeric_limits<double>::max();
    int nbOfHint=0;
    if(xy[0]>0. && xy[0]<xy1[0])
      { ret=std::min(ret,sqrt(z*z+xy[1]*xy[1])); nbOfHint++; } //distance pt to edge [pt0Tri3,pt1Tri3]
    double tmp=DistanceFromPtToSegInSpaceDim2(xy,xy1,xy2); //distance pt to edge [pt1Tri3,pt2Tri3]
    if(tmp!=std::numeric_limits<double>::max())
      { ret=std::min(ret,tmp); nbOfHint++; }
    tmp=DistanceFromPtToSegInSpaceDim2(xy,xy2,xy0);//distance pt to edge [pt2Tri3,pt0Tri3]
    if(tmp!=std::numeric_limits<double>::max())
      { ret=std::min(ret,tmp); nbOfHint++; }
    if(nbOfHint==3)
      ret=std::min(ret,fabs(z));
    return ret;
  }

  double DistanceFromPtToPolygonInSpaceDim3(const double *pt, const int *connOfPolygonBg, const int *connOfPolygonEnd, const double *coords) throw(INTERP_KERNEL::Exception)
  {
    return 0.;
  }

  bool ComputeRotTranslationMatrixToPut3PointsOnOXY(const double *p0, const double *p1, const double *p2, double *matrix)
  {
    double norm=sqrt((p1[0]-p0[0])*(p1[0]-p0[0])+(p1[1]-p0[1])*(p1[1]-p0[1])+(p1[2]-p0[2])*(p1[2]-p0[2]));
    double c=(p1[0]-p0[0])/norm;
    double s=sqrt(1-c*c);
    double y=p1[2]-p0[2],z=p0[1]-p1[1];
    norm=sqrt(y*y+z*z); y/=norm; z/=norm;
    double r0[9]={c,-z*s,y*s,
                  z*s,y*y*(1-c)+c,y*z*(1-c),
                  -y*s,z*y*(1-c),z*z*(1-c)+c};
    // 2nd rotation matrix
    double x=p2[0]-p0[0];
    y=p2[1]-p0[1]; z=p2[2]-p0[2];
    double y1=x*r0[3]+y*r0[4]+z*r0[5],z1=x*r0[6]+y*r0[7]+z*r0[8];
    c=y1/sqrt(y1*y1+z1*z1);
    s=sqrt(1.-c*c);
    //
    std::copy(r0,r0+3,matrix);
    matrix[4]=c*r0[3]-s*r0[6]; matrix[5]=c*r0[4]-s*r0[7]; matrix[6]=c*r0[5]-s*r0[8];
    matrix[8]=s*r0[3]+c*r0[6]; matrix[9]=s*r0[4]+c*r0[7]; matrix[10]=s*r0[5]+c*r0[8];
    matrix[3]=-p0[0]*matrix[0]-p0[1]*matrix[1]-p0[2]*matrix[2];
    matrix[7]=-p0[0]*matrix[4]-p0[1]*matrix[5]-p0[2]*matrix[6];
    matrix[11]=-p0[0]*matrix[8]-p0[1]*matrix[9]-p0[2]*matrix[10];
    return true;
  }
}

