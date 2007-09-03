#ifndef __VECTOR_UTILS_HXX__
#define __VECTOR_UTILS_HXX__

#include <string>
#include <sstream>
#include <cmath>
#include <numeric>
#include <map>

#define VOL_PREC 1.0e-6
#define DEFAULT_REL_TOL 1.0e-6
#define DEFAULT_ABS_TOL 5.0e-12

namespace INTERP_UTILS
{
  ///////////////////////////////////////////////////////////////////////
  // Math operations for vectors represented by double[3] - arrays   ////
  ///////////////////////////////////////////////////////////////////////
  
  /*
   * Copies a double[3] vector from src to dest
   *
   * @param src   source vector
   * @param dest  destination vector
   *
   */
  inline void copyVector3(const double* src, double* dest)
  {
    for(int i = 0 ; i < 3 ; ++i)
      {
	dest[i] = src[i];
      }
  }
  
  /*
   * Creates a string representation of a double[3] vector
   *
   * @param  pt  a 3-vector
   * @return a string of the form [x, y, z]
   */
  inline const std::string vToStr(const double* pt)
  {
    std::stringstream ss(std::ios::out);
    ss << "[" << pt[0] << ", " << pt[1] << ", " << pt[2] << "]";
    return ss.str();
  }

  /*
   * Calculates the cross product of two double[3] - vectors.
   *
   * @param v1    vector v1
   * @param v2    vector v2
   * @param res   vector in which to store the result v1 x v2. It should not be one of v1 and v2.
   */
  inline void cross(const double* v1, const double* v2,double* res)
  {
    res[0] = v1[1]*v2[2] - v1[2]*v2[1];
    res[1] = v1[2]*v2[0] - v1[0]*v2[2];
    res[2] = v1[0]*v2[1] - v1[1]*v2[0];
  }

  /*
   * Calculates the dot product of two double[3] - vectors
   *
   * @param v1   vector v1
   * @param v2   vector v2
   * @return   dot (scalar) product v1.v2
   */
  inline double dot(const double* v1, const double* v2)
  {
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
  }

  /*
   * Calculates norm of a double[3] vector
   *
   * @param v  a vector v
   * @return euclidean norm of v
   */
  inline double norm(const double* v)
  {
    return sqrt(dot(v,v));
  }

  /*
   * Compares doubles using an absolute tolerance
   * This is suitable mainly for comparisons with 0.0
   * 
   * @param x         first value
   * @param y         second value
   * @param errTol    maximum allowed absolute difference that is to be treated as equality
   * @return  true if |x - y| < errTol, false otherwise
   */
  inline bool epsilonEqual(const double x, const double y, const double errTol = DEFAULT_ABS_TOL)
  {
    return y < x ? x - y < errTol : y - x < errTol;
    //    return std::abs(x - y) < errTol;
  }

  /*
   * Compares doubles using a relative tolerance
   * This is suitable mainly for comparing larger values to each other. Before performing the relative test,
   * an absolute test is performed to guard from problems when comparing to 0.0
   * 
   * @param x         first value
   * @param y         second value
   * @param relTol    maximum allowed relative difference that is to be treated as equality
   * @param absTol    maximum allowed absolute difference that is to be treated as equality
   * @return  true if |x - y| <= absTol or |x - y|/max(|x|,|y|) <= relTol, false otherwise
   */
  inline bool epsilonEqualRelative(const double x, const double y, const double relTol = DEFAULT_REL_TOL, const double absTol = DEFAULT_ABS_TOL)
  {
    // necessary for comparing values close to zero
    // in order to avoid division by very small numbers
    if(std::abs(x - y) < absTol)
      {
	return true;
      }

    const double relError = std::abs((x - y) / std::max(std::abs(x), std::abs(y)));

    return relError < relTol;
  }

};




#endif
