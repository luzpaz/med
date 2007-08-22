#include "Interpolation3DTest.hxx"
#include "MEDMEM_Mesh.hxx"

#include <iostream>
#include <map>
#include <vector>
#include <cmath>

#include "VectorUtils.hxx"

#define VOL_PREC 1.0e-11

using namespace MEDMEM;
using namespace std;
using namespace INTERP_UTILS;

double Interpolation3DTest::sumVolume(const IntersectionMatrix& m) const
{
  double vol = 0.0;
  for(IntersectionMatrix::const_iterator iter = m.begin() ; iter != m.end() ; ++iter)
    {
      for(map<int, double>::const_iterator iter2 = iter->begin() ; iter2 != iter->end() ; ++iter2)
	{
	  vol += fabs(iter2->second);
	}
    }
  return vol;
}

bool Interpolation3DTest::areCompatitable(const IntersectionMatrix& m1, const IntersectionMatrix& m2) const
{
  bool compatitable = true;
  int i = 0;
  for(IntersectionMatrix::const_iterator iter = m1.begin() ; iter != m1.end() ; ++iter)
    {
      for(map<int, double>::const_iterator iter2 = iter->begin() ; iter2 != iter->end() ; ++iter2)
	{
	  int j = iter2->first;
	  if(m2.at(j-1).count(i+1) == 0)
	    {
	      if(!epsilonEqual(iter2->second, 0.0))
		{
		  std::cout << "V1( " << i << ", " << j << ") exists, but V2( " << j - 1 << ", " << i + 1 << ") " << " does not " << std::endl;
		  compatitable = false;
		}
	    }
	}
      ++i;
    }
  return compatitable;
}
	    
bool Interpolation3DTest::testSymmetric(const IntersectionMatrix& m1, const IntersectionMatrix& m2) const
{

  int i = 0;
  bool isSymmetric = true;

  std::cout << "Checking symmetry src - target" << std::endl;
  isSymmetric = isSymmetric & areCompatitable(m1, m2) ;
  std::cout << "Checking symmetry target - src" << std::endl;
  isSymmetric = isSymmetric & areCompatitable(m2, m1);

 for(IntersectionMatrix::const_iterator iter = m1.begin() ; iter != m1.end() ; ++iter)
    {
      for(map<int, double>::const_iterator iter2 = iter->begin() ; iter2 != iter->end() ; ++iter2)
	{
	  int j = iter2->first;
	  const double v1 = iter2->second;
	  //if(m2[j - 1].count(i+1) > 0)
	  //  {
	  map<int, double> theMap =  m2.at(j-1);
	  const double v2 = theMap[i + 1];
	  if(v1 != v2)
	    {
	      std::cout << "V1( " << i << ", " << j << ") = " << v1 << " which is different from V2( " << j - 1 << ", " << i + 1 << ") = " << v2 << " | diff = " << v1 - v2 << std::endl;
	      if(!epsilonEqual(v1, v2, VOL_PREC))
		{
		  isSymmetric = false;
		}
	    }
	}
      ++i;
    }
 return isSymmetric;
}

bool Interpolation3DTest::testDiagonal(const IntersectionMatrix& m) const
{
  std::cout << "Checking if matrix is diagonal" << std::endl;
  int i = 1;
  bool isDiagonal = true;
  for(IntersectionMatrix::const_iterator iter = m.begin() ; iter != m.end() ; ++iter)
    {
      for(map<int, double>::const_iterator iter2 = iter->begin() ; iter2 != iter->end() ; ++iter2)
	{
	  int j = iter2->first;
	  const double vol = iter2->second;
	  if(vol != 0.0 && (i != j))
	    {
	      std::cout << "V( " << i - 1 << ", " << j << ") = " << vol << " which is not zero" << std::endl;
	      if(!epsilonEqual(vol, 0.0, VOL_PREC))
		{
		  isDiagonal = false;
		}
	    }
	}
      ++i;
    }
  return isDiagonal;
}

void Interpolation3DTest::dumpIntersectionMatrix(const IntersectionMatrix& m) const
{
  int i = 0;
  cout << "Intersection matrix is " << endl;
  for(IntersectionMatrix::const_iterator iter = m.begin() ; iter != m.end() ; ++iter)
    {
      for(map<int, double>::const_iterator iter2 = iter->begin() ; iter2 != iter->end() ; ++iter2)
	{
	  
	  cout << "V(" << i << ", " << iter2->first << ") = " << iter2->second << endl;
	  
	}
      ++i;
    }
  std::cout << "Sum of volumes = " << sumVolume(m) << std::endl << std::endl;
}

void Interpolation3DTest::setUp()
{
  interpolator = new Interpolation3D();
}

void Interpolation3DTest::tearDown()
{
  delete interpolator;
} 

void Interpolation3DTest::calcIntersectionMatrix(const char* mesh1path, const char* mesh1, const char* mesh2path, const char* mesh2, IntersectionMatrix& m) const
{
  const string dataDir = getenv("DATA_DIR");

  std::cout << std::endl << "=== -> intersecting src = " << mesh1 << ", target = " << mesh2 << std::endl;

  std::cout << "Loading " << mesh1 << " from " << mesh1path << endl;
  const MESH sMesh(MED_DRIVER, dataDir+mesh1path, mesh1);

  std::cout << "Loading " << mesh2 << " from " << mesh2path << endl;
  const MESH tMesh(MED_DRIVER, dataDir+mesh2path, mesh2);

  m = interpolator->interpol_maillages(sMesh, tMesh);

  std::cout << "Intersection calculation done. " << std::endl << std::endl;
}

void Interpolation3DTest::intersectMeshes(const char* mesh1path, const char* mesh1, const char* mesh2path, const char* mesh2, const double correctVol, const double prec) const
{
  std::cout << std::endl << std::endl << "=============================" << std::endl;

  using std::string;
  const string path1 = string(mesh1path) + string(mesh1);
  const string path2 = string(mesh2path) + string(mesh2);

  const bool isTestReflexive = (path1.compare(path2) == 0);

  IntersectionMatrix matrix1;
  calcIntersectionMatrix(mesh1path, mesh1, mesh2path, mesh2, matrix1);
  dumpIntersectionMatrix(matrix1);
  std::cout.precision(16);

  const double vol1 = sumVolume(matrix1);

  if(isTestReflexive)
    {
      std::cout << "vol =  " << vol1 <<"  correctVol = " << correctVol <<  std::endl;
      CPPUNIT_ASSERT_DOUBLES_EQUAL(correctVol, vol1, prec);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Reflexive test failed", true, testDiagonal(matrix1));
    }
  else
    {
      IntersectionMatrix matrix2;
      calcIntersectionMatrix(mesh2path, mesh2, mesh1path, mesh1, matrix2);    
      dumpIntersectionMatrix(matrix2);
      
      const double vol2 = sumVolume(matrix2);

      std::cout << "vol1 =  " << vol1 << ", vol2 = " << vol2 << ", correctVol = " << correctVol <<   std::endl;

      CPPUNIT_ASSERT_DOUBLES_EQUAL(correctVol, vol1, prec);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(correctVol, vol2, prec);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(vol1, vol2, prec);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Symmetry test failed", true, testSymmetric(matrix1, matrix2));
    }

}



