#include "TetraAffineTransform.hxx"
#include "VectorUtils.hxx"

#include <cmath>
#include <iostream>

#include "Log.hxx"

namespace INTERP_UTILS
{
  /////////////////////////////////////////////////////////////////////////////////////////
  /// PUBLIC INTERFACE METHODS                                               //////////////
  /////////////////////////////////////////////////////////////////////////////////////////

  /*
   * Constructor
   * Create the TetraAffineTransform object from the tetrahedron 
   * with corners specified in pts. If the tetrahedron is degenerate or almost degenerate, 
   * construction succeeds, but the determinant of the transform is set to 0.
   *
   * @param pts  a 4x3 matrix containing 4 points (pts[0], ..., pts[3]) of 3 coordinates each
   */
  TetraAffineTransform::TetraAffineTransform(const double** pts)
  {
    
    LOG(2,"Creating transform from tetraeder : ");
    LOG(2, vToStr(pts[0]) << ", " << vToStr(pts[1]) << ", " << vToStr(pts[2]) << ", " << vToStr(pts[3]));
    
    // three last points -> linear transform
    for(int i = 0; i < 3 ; ++i)
      {
	for(int j = 0 ; j < 3 ; ++j)
	  {
	    // NB we insert columns, not rows
	    _linearTransform[3*j + i] = (pts[i+1])[j] - (pts[0])[j];
	  }
      }
    
    calculateDeterminant();
    
    LOG(3, "determinant before inverse = " << _determinant);
    
    // check that tetra is non-planar -> determinant is not zero
    // otherwise set _determinant to zero to signal caller that transformation did not work
    if(epsilonEqual(_determinant, 0.0))
      {
	_determinant = 0.0;
	return;
      }

    // we need the inverse transform
    invertLinearTransform();
    
    // first point -> translation
    // calculate here because translation takes place in "transformed space",
    // or in other words b = -A*O where A is the linear transform
    // and O is the position vector of the point that is mapped onto the origin
    for(int i = 0 ; i < 3 ; ++i)
      {
	_translation[i] = -(_linearTransform[3*i]*(pts[0])[0] + _linearTransform[3*i+1]*(pts[0])[1] + _linearTransform[3*i+2]*(pts[0])[2]) ;
      }
    
    // precalculate determinant (again after inversion of transform)
    calculateDeterminant();

#ifdef INVERSION_SELF_CHECK
    // debugging : check that applying the inversed transform to the original points
    // gives us the unit tetrahedron
    LOG(4, "transform determinant is " << _determinant);
    LOG(4, "*Self-check : Applying transformation to original points ... ");
    for(int i = 0; i < 4 ; ++i)
      {
	double v[3];
	apply(v, pts[i]);
	LOG(4, vToStr(v))
	  for(int j = 0; j < 3; ++j)
	    {
	      assert(epsilonEqual(v[j], (3*i+j == 3 || 3*i+j == 7 || 3*i+j == 11 ) ? 1.0 : 0.0));
	    }
      }
    
    LOG(4, " ok");
#endif
  }

  /*
   * Calculates the transform of point srcPt and stores the result in destPt.
   * If destPt == srcPt, then srcPt is overwritten safely.
   *  
   *
   * @param destPt  double[3] in which to store the transformed point
   * @param srcPt   double[3] containing coordinates of points to be transformed
   *
   */
  void TetraAffineTransform::apply(double* destPt, const double* srcPt) const
  {
    double* dest = destPt;
    
    // are we self-allocating ?
    const bool selfAllocation = (destPt == srcPt);
    
    if(selfAllocation)
      {
	// alloc temporary memory
	dest = new double[3];
	
	LOG(6, "Info : Self-affectation in TetraAffineTransform::apply");
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

  /*
   * Gives the determinant of the linear part of the transform
   *
   * @return determinant of the transform
   *
   */
  double TetraAffineTransform::determinant() const
  {
    return _determinant;
  }

  /*
   * Outputs  to std::cout the matrix A and the vector b
   * of the transform Ax + b
   *
   */
  void TetraAffineTransform::dump() const
  {
    using namespace std;
    
    std::cout << "A = " << std::endl << "[";
    for(int i = 0; i < 3; ++i)
      {
	std::cout << _linearTransform[3*i] << ", " << _linearTransform[3*i + 1] << ", " << _linearTransform[3*i + 2];
	if(i != 2 ) std::cout << endl;
      }
    std::cout << "]" << endl;
    
    std::cout << "b = " << "[" << _translation[0] << ", " << _translation[1] << ", " << _translation[2] << "]" << endl;
  }

  /////////////////////////////////////////////////////////////////////////////////////////
  /// PRIVATE  METHODS                                                       //////////////
  /////////////////////////////////////////////////////////////////////////////////////////

  /*
   * Calculates the inverse of the matrix A, stored in _linearTransform
   * by LU-factorization and substitution
   *
   */
  void TetraAffineTransform::invertLinearTransform()
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

	LOG(6,  "b = [" << b[0] << ", " << b[1] << ", " << b[2] << "]");
	
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
  
  /*
   * Updates the member _determinant of the matrix A of the transformation.
   *
   */
  void TetraAffineTransform::calculateDeterminant()
  {
    const double subDet[3] = 
      {
	_linearTransform[4] * _linearTransform[8] - _linearTransform[5] * _linearTransform[7],
	_linearTransform[3] * _linearTransform[8] - _linearTransform[5] * _linearTransform[6],
	_linearTransform[3] * _linearTransform[7] - _linearTransform[4] * _linearTransform[6]
      };

    _determinant = _linearTransform[0] * subDet[0] - _linearTransform[1] * subDet[1] + _linearTransform[2] * subDet[2]; 
  }

  /* 
   * Calculates the LU-factorization of the matrix A (_linearTransform)
   * and stores it in lu. Since partial pivoting is used, there are 
   * row swaps. This is represented by the index permutation vector idx : to access element
   * (i,j) of lu, use lu[3*idx[i] + j]
   *
   * @param lu double[9] in which to store LU-factorization
   * @param idx int[3] in which to store row permutation vector
   */
  void TetraAffineTransform::factorizeLU(double* lu, int* idx) const
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
	double max = std::abs(lu[3*idx[k] + k]);
	int row = i;
	while(i < 3)
	  {
	    if(std::abs(lu[3*idx[i] + k]) > max)
	      {
		max = fabs(lu[3*idx[i] + k]);
		row = i;
	      }
	    ++i;
	  }
	      
	// swap rows in index vector
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
		// case s = k will always become zero, and then be replaced by
		// the l-value
		// there's no need to calculate this explicitly

		// u_js = u_js - l_jk * u_ks
		lu[3*idx[j] + s] -= lu[3*idx[j] + k] * lu[3*idx[k] + s] ;
	      }
	  }
      }
  }

  /*
   * Solves the system Lx = b, where L is lower unit-triangular (ones on the diagonal)
   * 
   * @param x   double[3] in which the solution is stored
   * @param lu  double[9] containing the LU-factorization
   * @param b   double[3] containing the right-hand side
   * @param idx int[3] containing the permutation vector associated with lu
   *
   */
  void TetraAffineTransform::forwardSubstitution(double* x, const double* lu, const double* b, const int* idx) const
  {
    // divisions are not carried out explicitly because lu does not store
    // the diagonal values of L, which are all 1
    x[idx[0]] = ( b[idx[0]] );// / lu[3*idx[0]]
    x[idx[1]] = ( b[idx[1]] - lu[3*idx[1]] * x[idx[0]] ); // / lu[3*idx[1] + 1];
    x[idx[2]] = ( b[idx[2]] - lu[3*idx[2]] * x[idx[0]] - lu[3*idx[2] + 1] * x[idx[1]] ); // / lu[3*idx[2] + 2];
  }

  /*
   * Solves the system Ux = b, where U is upper-triangular 
   * 
   * @param x   double[3] in which the solution is stored
   * @param lu  double[9] containing the LU-factorization
   * @param b   double[3] containing the right-hand side
   * @param idx int[3] containing the permutation vector associated with lu
   *
   */
  void TetraAffineTransform::backwardSubstitution(double* x, const double* lu, const double* b, const int* idx) const
  {
    x[idx[2]] = ( b[idx[2]] ) / lu[3*idx[2] + 2];
    x[idx[1]] = ( b[idx[1]] - lu[3*idx[1] + 2] * x[idx[2]] ) / lu[3*idx[1] + 1];
    x[idx[0]] = ( b[idx[0]] - lu[3*idx[0] + 1] * x[idx[1]] - lu[3*idx[0] + 2] * x[idx[2]] ) / lu[3*idx[0]];
  }
  

};
