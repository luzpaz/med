#ifndef __TU_MESH_TEST_TOOLKIT_HXX__
#define __TU_MESH_TEST_TOOLKIT_HXX__

#include "../Interpolation3D.hxx"

#define ERR_TOL 1.0e-8

class MEDMEM::Interpolation3D;
using MEDMEM::Interpolation3D;
class MEDMEM::MESH;

namespace INTERP_UTILS
{

class MeshTestToolkit
{

public:
  MeshTestToolkit() : _interpolator(new Interpolation3D()) {}
  
  ~MeshTestToolkit() { delete _interpolator; }

  // 1.0e-5 here is due to limited precision of "correct" volumes calculated in Salome
  void intersectMeshes(const char* mesh1path, const char* mesh1, const char* mesh2path, const char* mesh2, const double correctVol, const double prec = 1.0e-5, bool doubleTest = true) const;
  
  void dumpIntersectionMatrix(const IntersectionMatrix& m) const;

  double sumRow(const IntersectionMatrix& m, int i) const;

  double sumCol(const IntersectionMatrix& m, int i) const;

  void getVolumes( MEDMEM::MESH& mesh,const double*& tab) const;

  bool testVolumes(const IntersectionMatrix& m,  MEDMEM::MESH& sMesh,  MEDMEM::MESH& tMesh) const;

  double sumVolume(const IntersectionMatrix& m) const;

  bool areCompatitable( const IntersectionMatrix& m1,  const IntersectionMatrix& m2) const;

  bool testSymmetric(const IntersectionMatrix& m1, const IntersectionMatrix& m2) const;

  bool testDiagonal(const IntersectionMatrix& m) const;
  
  void calcIntersectionMatrix(const char* mesh1path, const char* mesh1, const char* mesh2path, const char* mesh2, IntersectionMatrix& m) const;

  std::pair<int,int> countNumberOfMatrixEntries(const IntersectionMatrix& m);

protected:

  Interpolation3D* _interpolator;

};
}
#endif
