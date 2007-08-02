#ifndef __TETRA_AFFINE_TRANSFORM_HXX__
#define __TETRA_AFFINE_TRANSFORM_HXX__

#include <math.h>
#include <iostream>

#include "VectorUtils.hxx"

namespace INTERP_UTILS
{

  class TetraAffineTransform
  {


  public:
    TetraAffineTransform(const double** pts)
    {

      std::cout << "Creating transform from tetraeder : " << std::endl;
      std::cout << vToStr(pts[0]) << ", " << vToStr(pts[1]) << ", " << vToStr(pts[2]) << ", " << vToStr(pts[3]) << ", " << std::endl;

#if 0
      do {
#endif
	// three last points -> linear transform
	for(int i = 0; i < 3 ; ++i)
	  {
	    for(int j = 0 ; j < 3 ; ++j)
	      {
		// NB we insert columns, not rows
		_linearTransform[3*j + i] = (pts[i+1])[j] - (pts[0])[j];
	      }
	  }
#if 0
	calculateDeterminant();
	//assert(_determinant != 0.0);
	
	if(_determinant < 0.0) 
	  {
	    // swap two columns
	    const double* tmp = pts[1];
	    pts[1] = pts[2];
	    pts[2] = tmp;
	  }

      } while(_determinant < 0.0); // should do at most one more loop
#endif

      // we need the inverse transform
      invertLinearTransform();

      // first point -> translation
      // calculate here because translation takes place in "transformed space",
      // or in other words b = -A*O where A is the linear transform
      // and O is the position vector of the point that is mapped onto the origin
      for(int i = 0 ; i < 3 ; ++i)
	{
	  _translation[i] = -_linearTransform[3*i]*(pts[0])[0] - _linearTransform[3*i+1]*(pts[0])[1] - _linearTransform[3*i+2]*(pts[0])[2] ;
	}

      // precalculate determinant (again after inversion of transform)
      calculateDeterminant();
      
      std::cout << "*Self-check : Applying transformation to original points ... ";
      for(int i = 0; i < 4 ; ++i)
	{
	  double v[3];
	  apply(v, pts[i]);
	  //	  std::cout << vToStr(v) << std::endl;
	  for(int j = 0; j < 3; ++j)
	    {
	      assert(epsilonEqual(v[j], (3*i+j == 3 || 3*i+j == 7 || 3*i+j == 11 ) ? 1.0 : 0.0));
	    }
	}
      
      std::cout << " ok" << std::endl;
    }

    void apply(double* destPt, const double* srcPt) const
    {
      double* dest = destPt;

      const bool selfAllocation = (destPt == srcPt);

      
      
      if(selfAllocation)
	{
	  // alloc temporary memory
	  dest = new double[3];

	  //std::cout << "Oops! self-affectation" << std::endl;
	}

      for(int i = 0 ; i < 3 ; ++i)
	{
	  // matrix - vector multiplication
	  dest[i] = _linearTransform[3*i] * srcPt[0] + _linearTransform[3*i + 1] * srcPt[1] + _linearTransform[3*i + 2] * srcPt[2];

	  // translation
	  dest[i] += _translation[i];
	}

      if(selfAllocation)
	{
	  // copy result back to destPt
	  for(int i = 0 ; i < 3 ; ++i)
	    {
	      destPt[i] = dest[i];
	    }
	  delete[] dest;
	}
    }

    double determinant() const
    {
      return _determinant;
    }

    void dump()
    {
      using namespace std;
      
      std::cout << "A = " << std::endl << "[";
      for(int i = 0; i < 3; ++i)
	{
	  cout << _linearTransform[3*i] << ", " << _linearTransform[3*i + 1] << ", " << _linearTransform[3*i + 2];
	  if(i != 2 ) cout << endl;
	}
      cout << "]" << endl;
      
      cout << "b = " << "[" << _translation[0] << ", " << _translation[1] << ", " << _translation[2] << "]" << endl;
    }

  private:

    void invertLinearTransform()
    {
      //{ we copy the matrix for the lu-factorization
      // maybe inefficient
      double lu[9];
      for(int i = 0 ; i < 9; ++i)
	{
	  lu[i] = _linearTransform[i];
	}
      
      // calculate LU factorization
      int idx[3];
      factorizeLU(lu, idx);

      // calculate inverse by forward and backward substitution
      // store in _linearTransform
      // NB _linearTransform cannot be overwritten with lu in the loop
      for(int i = 0 ; i < 3 ; ++i)
	{
	  // form standard base vector i
	  const double b[3] = 
	    {
	      int(i == 0),
	      int(i == 1),
	      int(i == 2)
	    };

	  //std::cout << "b = [" << b[0] << ", " << b[1] << ", " << b[2] << "]" << std::endl; 
	  
	  double y[3];
	  forwardSubstitution(y, lu, b, idx);

	  double x[3];
	  backwardSubstitution(x, lu, y, idx);

	  // copy to _linearTransform matrix
	  // NB : this is a column operation, so we cannot 
	  // do this directly when we calculate x
	  for(int j = 0 ; j < 3 ; j++)
	    {
	      _linearTransform[3*j + i] = x[idx[j]];
	    }

	}
      
    }

    void calculateDeterminant()
    {
      const double subDet[3] = 
	{
	  _linearTransform[4] * _linearTransform[8] - _linearTransform[5] * _linearTransform[7],
	  _linearTransform[3] * _linearTransform[8] - _linearTransform[5] * _linearTransform[6],
	  _linearTransform[3] * _linearTransform[7] - _linearTransform[4] * _linearTransform[6]
	};

      _determinant = _linearTransform[0] * subDet[0] - _linearTransform[1] * subDet[1] + _linearTransform[2] * subDet[2]; 
    }

    /////////////////////////////////////////////////
    /// Auxiliary methods for inverse calculation ///
    /////////////////////////////////////////////////
    void factorizeLU(double* lu, int* idx) const
    {
      // 3 x 3 LU factorization
      // initialise idx
      for(int i = 0 ; i < 3 ; ++i)
	{
	  idx[i] = i;
	}
            
      for(int k = 0; k < 2 ; ++k)
	{
	  
	  // find pivot
	  int i = k;
	  double max = fabs(lu[3*idx[k] + k]);
	  int row = i;
	  while(i < 3)
	    {
	      if(fabs(lu[3*idx[i] + k]) > max)
		{
		  max = fabs(lu[3*idx[i] + k]);
		  row = i;
		}
	      ++i;
	    }
	      
	  // swap rows
	  int tmp = idx[k];
	  idx[k] = idx[row];
	  idx[row] = tmp;
      
	  // calculate row
	  for(int j = k + 1 ; j < 3 ; ++j)
	    {
	      // l_jk = u_jk / u_kk
	      lu[3*idx[j] + k] /= lu[3*idx[k] + k];
	      for(int s = k + 1; s < 3 ; ++s)
		{
		  // case s = k will always become zero, and then replaced by
		  // the l-value
		  // there's no need to calculate this explicitly

		  // u_js = u_js - l_jk * u_ks
		  lu[3*idx[j] + s] -= lu[3*idx[j] + k] * lu[3*idx[k] + s] ;
		}
	    }
	}

      
    }
      
    void forwardSubstitution(double* x, const double* lu, const double* b, const int* idx) const
    {
      x[idx[0]] = ( b[idx[0]] );// / lu[3*idx[0]];
      x[idx[1]] = ( b[idx[1]] - lu[3*idx[1]] * x[idx[0]] ); // / lu[3*idx[1] + 1];
      x[idx[2]] = ( b[idx[2]] - lu[3*idx[2]] * x[idx[0]] - lu[3*idx[2] + 1] * x[idx[1]] ); // / lu[3*idx[2] + 2];
    }

    void backwardSubstitution(double* x, const double* lu, const double* b, const int* idx) const
    {
      x[idx[2]] = ( b[idx[2]] ) / lu[3*idx[2] + 2];
      x[idx[1]] = ( b[idx[1]] - lu[3*idx[1] + 2] * x[idx[2]] ) / lu[3*idx[1] + 1];
      x[idx[0]] = ( b[idx[0]] - lu[3*idx[0] + 1] * x[idx[1]] - lu[3*idx[0] + 2] * x[idx[2]] ) / lu[3*idx[0]];
    }



    double _translation[3];
    double _linearTransform[9];

    double _determinant;

  };


};


#endif
