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
#ifndef _TRANSLATIONROTATIONMATRIX_HXX_
#define _TRANSLATIONROTATIONMATRIX_HXX_

#include <cmath>

namespace INTERP_KERNEL
{
  class TranslationRotationMatrix
  {

  public:

    TranslationRotationMatrix()
    { 
      unsigned i;
      for(i=0;i<TRANSL_SIZE;i++)
        _translationCoeffs[i]=0.;
      for(i=0;i<ROT_SIZE;i++)
        _rotationCoeffs[i]=i%4?0.:1.;
    }

    void multiply(const TranslationRotationMatrix& A)
    {
      TranslationRotationMatrix result;
      //setting output matrix to zero
      for (int i=0; i<3; i++)
        result._rotationCoeffs[i*4]=0.0;
      //multiplying
      for (int i=0; i<3;i++)
        for (int j=0; j<3; j++)
          for (int k=0; k<3; k++)
            result._rotationCoeffs[j+i*3]+=A._rotationCoeffs[3*i+k]*_rotationCoeffs[j+k*3];
    
      for (int i=0;i<9; i++)
        _rotationCoeffs[i]=result._rotationCoeffs[i];
    }
  
    void rotate_vector(double* P)
    {
      double temp[3]={0.0, 0.0, 0.0};
    
      for (int i=0; i<3;i++)
        for (int j=0; j<3; j++)
          temp[i] +=_rotationCoeffs[3*i+j]*P[j];
       
      P[0]=temp[0];P[1]=temp[1];P[2]=temp[2];
    }
 
    void transform_vector(double*P)
    {
      P[0]+=_translationCoeffs[0];
      P[1]+=_translationCoeffs[1];
      P[2]+=_translationCoeffs[2];
      rotate_vector(P);
    }

    void translate(const double* P)
    {
      _translationCoeffs[0]=P[0];
      _translationCoeffs[1]=P[1];
      _translationCoeffs[2]=P[2];
    }
  
    void  rotate_x (double* P)
    {
      _rotationCoeffs[0]=1.0;
      double r_sqr = P[1]*P[1]+P[2]*P[2];
      if (r_sqr < EPS)
        {_rotationCoeffs[4]=1.0; _rotationCoeffs[8]=1.0; return;}
      double r = sqrt(r_sqr);
      double cos =P[1]/r;
      double sin =P[2]/r;

      _rotationCoeffs[4]=cos;
      _rotationCoeffs[5]=sin;
      _rotationCoeffs[7]=-sin;
      _rotationCoeffs[8]=cos;


      rotate_vector(P);
    }

    void  rotate_z (double* P)
    {
      _rotationCoeffs[8]=1.0;
      double r_sqr = P[0]*P[0]+P[1]*P[1];
      if (r_sqr < EPS)
        {_rotationCoeffs[4]=1.0; _rotationCoeffs[0]=1.0; return;}
      double r = sqrt(r_sqr);
      double cos =P[0]/r;
      double sin =P[1]/r;
    
      _rotationCoeffs[0]=cos;
      _rotationCoeffs[1]=sin; 
      _rotationCoeffs[3]=-sin;
      _rotationCoeffs[4]=cos;
    
      rotate_vector(P);
    }
                     
       
  private:
    static const double EPS;
    static const unsigned ROT_SIZE=9;
    static const unsigned TRANSL_SIZE=3;
    double _rotationCoeffs[ROT_SIZE];
    double _translationCoeffs[TRANSL_SIZE];
  };
  const double TranslationRotationMatrix::EPS=1e-12;
}

#endif
