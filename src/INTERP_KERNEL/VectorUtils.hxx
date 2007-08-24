#ifndef __VECTOR_UTILS_HXX__
#define __VECTOR_UTILS_HXX__

#include <string>
#include <sstream>
#include <cmath>
#include <numeric>
#include <map>

#define VOL_PREC 1.0e-6
#define DEFAULT_REL_TOL 1.0e-6
#define DEFAULT_ABS_TOL 1.0e-12

namespace INTERP_UTILS
{
 
  inline void copyVector3(const double* src, double* dest)
  {
    for(int i = 0 ; i < 3 ; ++i)
      {
	dest[i] = src[i];
      }
  }
  
  inline const std::string vToStr(const double* pt)
  {
    std::stringstream ss(std::ios::out);
    ss << "[" << pt[0] << ", " << pt[1] << ", " << pt[2] << "]";
    return ss.str();
  }

  inline void cross(const double* v1, const double* v2,double* res)
  {
    res[0] = v1[1]*v2[2] - v1[2]*v2[1];
    res[1] = v1[2]*v2[0] - v1[0]*v2[2];
    res[2] = v1[0]*v2[1] - v1[1]*v2[0];
  }

  inline double dot(const double* v1, const double* v2)
  {
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
  }

  inline double norm(const double* v)
  {
    return sqrt(dot(v,v));
  }

  inline double angleBetweenVectors(const double* v1, const double* v2, const double* n)
  {
    const double denominator = dot(v1, v2);
    double v3[3];
   
    cross(v1, v2, v3);
    const double numerator = dot(n, v3);
    return atan2(numerator, denominator);

  }

  /// Should be used for comparisons to zero
  inline bool epsilonEqual(const double x, const double y, const double errTol = DEFAULT_ABS_TOL)
  {
    return std::abs(x - y) <= errTol;
  }

  // Should be used for comparisons between numbers that could be large
  inline bool epsilonEqualRelative(const double x, const double y, const double relTol = DEFAULT_REL_TOL, const double absTol = DEFAULT_ABS_TOL)
  {
    // necessary for comparing values close to zero
    // in order to avoid division by very small numbers
    if(std::abs(x - y) < absTol)
      {
	return true;
      }

    const double relError = std::abs((x - y) / std::max(x, y));

    return relError <= relTol;
  }

};




#endif
