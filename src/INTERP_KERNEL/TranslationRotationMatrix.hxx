#ifndef _TRANSLATIONROTATIONMATRIX_HXX_
#define _TRANSLATIONROTATIONMATRIX_HXX_

#include "InterpolationUtils.hxx"
#include <cmath>
//#include <vector>

namespace MEDMEM
{

  const double EPS=1e-12;

  class TranslationRotationMatrix
  {

  public:

    TranslationRotationMatrix():_rotation_coeffs(9,0.0), _translation_coeffs(3,0.0)
    { 
      _rotation_coeffs[0]=1.0;
      _rotation_coeffs[4]=1.0;
      _rotation_coeffs[8]=1.0;
    }

    void multiply(const TranslationRotationMatrix& A)
    {
      TranslationRotationMatrix result;
      //setting output matrix to zero
      for (int i=0; i<3; i++)
	result._rotation_coeffs[i*4]=0.0;
      //multiplying
      for (int i=0; i<3;i++)
	for (int j=0; j<3; j++)
	  for (int k=0; k<3; k++)
	    result._rotation_coeffs[j+i*3]+=A._rotation_coeffs[3*i+k]*_rotation_coeffs[j+k*3];
    
      for (int i=0;i<9; i++)
	_rotation_coeffs[i]=result._rotation_coeffs[i];

      // 	cout << "Matrix "<<endl;
      // 		for (int i=0; i<3; i++)
      // 			{
      // 				cout<<"| ";
      // 				for (int j=0; j<3;j++)
      // 					cout <<_rotation_coeffs[i*3+j]<<" ";
      // 				cout<<" "<<endl;
      // 			}
    }
  
    void rotate_vector(double* P)
    {
      double temp[3]={0.0, 0.0, 0.0};
    
      for (int i=0; i<3;i++)
	for (int j=0; j<3; j++)
	  temp[i] +=_rotation_coeffs[3*i+j]*P[j];
	
      P[0]=temp[0];P[1]=temp[1];P[2]=temp[2];
    }
 
    void transform_vector(double*P)
    {
      P[0]+=_translation_coeffs[0];
      P[1]+=_translation_coeffs[1];
      P[2]+=_translation_coeffs[2];
      rotate_vector(P);
    }

    void translate(const double* P)
    {
      _translation_coeffs[0]=P[0];
      _translation_coeffs[1]=P[1];
      _translation_coeffs[2]=P[2];
    }
  
    void  rotate_x (double* P)
    {
      _rotation_coeffs[0]=1.0;
      double r_sqr = P[1]*P[1]+P[2]*P[2];
      if (r_sqr < EPS)
	{_rotation_coeffs[4]=1.0; _rotation_coeffs[8]=1.0; return;}
      double r = sqrt(r_sqr);
      double cos =P[1]/r;
      double sin =P[2]/r;

      _rotation_coeffs[4]=cos;
      _rotation_coeffs[5]=sin;
      _rotation_coeffs[7]=-sin;
      _rotation_coeffs[8]=cos;


      rotate_vector(P);
    }

    void  rotate_z (double* P)
    {
      _rotation_coeffs[8]=1.0;
      double r_sqr = P[0]*P[0]+P[1]*P[1];
      if (r_sqr < EPS)
	{_rotation_coeffs[4]=1.0; _rotation_coeffs[0]=1.0; return;}
      double r = sqrt(r_sqr);
      double cos =P[0]/r;
      double sin =P[1]/r;
    
      _rotation_coeffs[0]=cos;
      _rotation_coeffs[1]=sin; 
      _rotation_coeffs[3]=-sin;
      _rotation_coeffs[4]=cos;
    
      rotate_vector(P);
    }
			
	
  private:
    vector<double> _rotation_coeffs;
    vector<double> _translation_coeffs;
  
  };

};
#endif
