#include "Interpolation3DTest.hxx"
#include "MEDMEM_Mesh.hxx"

#include <iostream>
#include <map>
#include <vector>
#include <cmath>

#include "VectorUtils.hxx"

using namespace MEDMEM;
using namespace std;
using namespace INTERP_UTILS;

double Interpolation3DTest::sumVolume(IntersectionMatrix m)
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

bool Interpolation3DTest::isReflexive(IntersectionMatrix m1, IntersectionMatrix m2)
{
  int i = 0;
  bool isReflexive = true;
  for(IntersectionMatrix::const_iterator iter = m1.begin() ; iter != m1.end() ; ++iter)
    {
      for(map<int, double>::const_iterator iter2 = iter->begin() ; iter2 != iter->end() ; ++iter2)
	{
	  int j = iter2->first;
	  const double v1 = iter2->second;
	  if(m2[j - 1].count(i+1) > 0)
	    {
	      const double v2 = m2[j - 1][i + 1];
	      if(!epsilonEqual(v1, v2))
		{
		  std::cout << "V1( " << i << ", " << j << ") = " << v1 << " which is different from V2( " << j - 1 << ", " << i + 1 << ") = " << v2 << std::endl;
		  isReflexive = false;
		}
	    }
	  else
	    {
	      if(!epsilonEqual(v1, 0.0))
		{
		  std::cout << "V2( " << iter2->first - 1 << ", " << i + 1 << ") " << " does not exist" << std::endl;
		  isReflexive = false;
		}
	    }
	}
      ++i;
    }
  return isReflexive;
}

  //? this is not a good test
bool Interpolation3DTest::isIntersectionConsistent(IntersectionMatrix m)
{
  bool res = true;
  int i = 0;
  for(IntersectionMatrix::const_iterator iter = m.begin() ; iter != m.end() ; ++iter)
    {
      for(map<int, double>::const_iterator iter2 = iter->begin() ; iter2 != iter->end() ; ++iter2)
	{
	  // volume should be between 0 and 1 / 6
	  if(iter2->second < 0.0 - ERR_TOL || iter2->second > 1.0 / 6.0 + ERR_TOL)
	    {
	      cout << "Inconsistent intersection volume : V(" << i << ", " << iter2->first << ") = " << iter2->second << endl;
	      res = false;
	    }
	}
      ++i;
    }
  return res;
}

void Interpolation3DTest::dumpIntersectionMatrix(IntersectionMatrix m)
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

void Interpolation3DTest::reflexiveTetra()
{
  std::cout << std::endl << std::endl << "=============================" << std::endl;
  std::cout << " Reflexive tetra " << endl;
  MESH unitMesh(MED_DRIVER, "meshes/UnitTetra.med", "UnitTetra");

  std::cout << std::endl << "*** unit tetra" << std::endl;
  IntersectionMatrix matrix1 = interpolator->interpol_maillages(unitMesh, unitMesh);

#if 0  
  std::cout << std::endl << "*** non-unit large tetra" << std::endl;
  MESH largeMesh(MED_DRIVER, "meshes/tetra2.med", "Mesh_1");
  IntersectionMatrix matrix2 = interpolator->interpol_maillages(largeMesh, largeMesh);

  std::cout << std::endl << "*** non-unit small tetra" << std::endl;
  MESH smallMesh(MED_DRIVER, "meshes/tetra2_scaled.med", "Mesh_2");
  IntersectionMatrix matrix3 = interpolator->interpol_maillages(smallMesh, smallMesh);
#endif
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0 / 6.0, sumVolume(matrix1), ERR_TOL);
#if 0
  CPPUNIT_ASSERT_DOUBLES_EQUAL(48.0, sumVolume(matrix2), ERR_TOL);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.75, sumVolume(matrix3), ERR_TOL);
#endif
}

void Interpolation3DTest::calcIntersectionMatrix(const char* mesh1path, const char* mesh1, const char* mesh2path, const char* mesh2, IntersectionMatrix& m)
{
  std::cout << std::endl << "=== -> intersecting src = " << mesh1 << ", target = " << mesh2 << std::endl;

  std::cout << "Loading " << mesh1 << " from " << mesh1path << endl;
  MESH sMesh(MED_DRIVER, mesh1path, mesh1);

  std::cout << "Loading " << mesh2 << " from " << mesh2path << endl;
  MESH tMesh(MED_DRIVER, mesh2path, mesh2);

  m = interpolator->interpol_maillages(sMesh, tMesh);

  std::cout << "Intersection calculation done. " << std::endl << std::endl;
}

void Interpolation3DTest::intersectMeshes(const char* mesh1path, const char* mesh1, const char* mesh2path, const char* mesh2, const double correctVol, const double prec)
{
  std::cout << std::endl << std::endl << "=============================" << std::endl;

  IntersectionMatrix matrix1;
  calcIntersectionMatrix(mesh1path, mesh1, mesh2path, mesh2, matrix1);

  dumpIntersectionMatrix(matrix1);

  IntersectionMatrix matrix2;
  calcIntersectionMatrix(mesh2path, mesh2, mesh1path, mesh1, matrix2);

  dumpIntersectionMatrix(matrix2);

  bool reflexive = isReflexive(matrix1, matrix2);

  const double vol1 = sumVolume(matrix1);
  const double vol2 = sumVolume(matrix2);

  std::cout.precision(8);
  std::cout << "vol1 =  " << vol1 << ", vol2 = " << vol2 << ", correctVol = " << correctVol <<   std::endl;

  //CPPUNIT_ASSERT_DOUBLES_EQUAL(vol1, vol2, 1.0e-6);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(correctVol, vol1, prec);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(correctVol, vol2, prec);
  CPPUNIT_ASSERT_EQUAL(true, reflexive);
}



void Interpolation3DTest::tetraTetraScale()
{
  std::cout << std::endl << std::endl << "=============================" << std::endl;
  std::cout << " Scaled included tetra  " << endl;
  
  MESH srcMesh(MED_DRIVER, "meshes/tetra2.med", "Mesh_1");
  MESH targetMesh(MED_DRIVER, "meshes/tetra2_scaled.med", "Mesh_2");

  std::cout << "*** src - target" << std::endl;
  IntersectionMatrix matrix1 = interpolator->interpol_maillages(srcMesh, targetMesh);
  std::cout << std::endl << "*** target - src" << std::endl;
  IntersectionMatrix matrix2 = interpolator->interpol_maillages(targetMesh, srcMesh);

  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.75, sumVolume(matrix1), 1.0e-6 );
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.75, sumVolume(matrix2), 1.0e-6 );
}


void Interpolation3DTest::cyl1()
{
  std::cout << std::endl << std::endl << "=============================" << std::endl;
  std::cout << " Cylinders  " << endl;


  MESH srcMesh(MED_DRIVER, "meshes/cyl1_fine.med", "Mesh_1");
  MESH targetMesh(MED_DRIVER, "meshes/cyl1_rot_moderate.med", "Mesh_2");

  IntersectionMatrix matrix1 = interpolator->interpol_maillages(srcMesh, targetMesh);
  IntersectionMatrix matrix2 = interpolator->interpol_maillages(targetMesh, srcMesh);

  CPPUNIT_ASSERT_EQUAL(true, isIntersectionConsistent(matrix1));
  CPPUNIT_ASSERT_EQUAL(true, isIntersectionConsistent(matrix2));

  const double vol1 = sumVolume(matrix1);
  const double vol2 = sumVolume(matrix2);

  CPPUNIT_ASSERT_DOUBLES_EQUAL(vol1, vol2, 0.1);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(3.09079e6, vol1, 1.0e2); 
  CPPUNIT_ASSERT_DOUBLES_EQUAL(3.09079e6, vol2, 1.0e2); 
}


void Interpolation3DTest::box1()
{
  std::cout << std::endl << std::endl << "=============================" << std::endl;
  std::cout << " Boxes  " << endl;


  MESH srcMesh(MED_DRIVER, "meshes/box1_moderate.med", "Mesh_1");
  MESH targetMesh(MED_DRIVER, "meshes/box1_rot_moderate.med", "Mesh_2");

  IntersectionMatrix matrix1 = interpolator->interpol_maillages(srcMesh, targetMesh);
  IntersectionMatrix matrix2 = interpolator->interpol_maillages(targetMesh, srcMesh);

  //  CPPUNIT_ASSERT_EQUAL(true, isIntersectionConsistent(matrix1));
  //  CPPUNIT_ASSERT_EQUAL(true, isIntersectionConsistent(matrix2));

  const double vol1 = sumVolume(matrix1);
  const double vol2 = sumVolume(matrix2);

  //  CPPUNIT_ASSERT_DOUBLES_EQUAL(vol1, vol2, 1.0);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(750684, vol1, 1.0); 
  CPPUNIT_ASSERT_DOUBLES_EQUAL(750684, vol2, 1.0); 

}

void Interpolation3DTest::tetra1()
{
  std::cout << std::endl << std::endl << "=============================" << std::endl;
  std::cout << " General tetrahedra - 1-element meshes " << endl;

  MESH srcMesh(MED_DRIVER, "meshes/tetra1_gen.med", "Mesh_4");
  MESH targetMesh(MED_DRIVER, "meshes/tetra1_gen_rot.med", "Mesh_5");
std::cout << "*** src - target" << std::endl;
  IntersectionMatrix matrix1 = interpolator->interpol_maillages(srcMesh, targetMesh);
  std::cout << "*** target - src" << std::endl;
  IntersectionMatrix matrix2 = interpolator->interpol_maillages(targetMesh, srcMesh);

  std::cout << std::endl << std::endl << "--------------------" << std::endl;
  std::cout << "src - target" << std::endl;
  dumpIntersectionMatrix(matrix1);
  std::cout << std::endl << std::endl << "--------------------" << std::endl;
  std::cout << "target - src" << std::endl;
  dumpIntersectionMatrix(matrix2);
  
  //  CPPUNIT_ASSERT_EQUAL(true, isIntersectionConsistent(matrix1));
  //  CPPUNIT_ASSERT_EQUAL(true, isIntersectionConsistent(matrix2));

  const double vol1 = sumVolume(matrix1);
  const double vol2 = sumVolume(matrix2);

  CPPUNIT_ASSERT_DOUBLES_EQUAL(vol1, vol2, 1.0);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0494584, vol1, 1.0e-7); 
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0494584, vol2, 1.0e-7); 
}

void Interpolation3DTest::tetra3()
{
  std::cout << std::endl << std::endl << "=============================" << std::endl;
  std::cout << " General tetrahedra 10-element meshes " << endl;

  MESH srcMesh(MED_DRIVER, "meshes/tetra3_moderate.med", "Mesh_1");
  MESH targetMesh(MED_DRIVER, "meshes/tetra3_rot_moderate.med", "Mesh_3");

  IntersectionMatrix matrix1 = interpolator->interpol_maillages(srcMesh, targetMesh);
  
  IntersectionMatrix matrix2 = interpolator->interpol_maillages(targetMesh, srcMesh);

  std::cout << std::endl << std::endl << "--------------------" << std::endl;
  std::cout << "src - target" << std::endl;
  dumpIntersectionMatrix(matrix1);
  std::cout << std::endl << std::endl << "--------------------" << std::endl;
  std::cout << "target - src" << std::endl;
  dumpIntersectionMatrix(matrix2);
  
  //  CPPUNIT_ASSERT_EQUAL(true, isIntersectionConsistent(matrix1));
  //  CPPUNIT_ASSERT_EQUAL(true, isIntersectionConsistent(matrix2));

  const double vol1 = sumVolume(matrix1);
  const double vol2 = sumVolume(matrix2);

  CPPUNIT_ASSERT_DOUBLES_EQUAL(vol1, vol2, 1.0);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(538.76, vol1, 1.0); 
  CPPUNIT_ASSERT_DOUBLES_EQUAL(538.76, vol2, 1.0); 
}
