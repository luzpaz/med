#include "Interpolation3D.hxx"
#include "MeshTestToolkit.hxx"
#include "Log.hxx"

#include "MEDMEM_Mesh.hxx"

#include <cassert>
#include <string>

using namespace MEDMEM;
using namespace MED_EN;

namespace INTERP_UTILS
{
  class PerfTestToolkit : public MeshTestToolkit
  {
    
  public:
    void calcIntersectionMatrix(const char* mesh1path, const char* mesh1, const char* mesh2path, const char* mesh2, IntersectionMatrix& m) 
    {
      const string dataBaseDir = getenv("MED_ROOT_DIR");
      const string dataDir = dataBaseDir + "share/salome/resources/med/";

      LOG(1, std::endl << "=== -> intersecting src = " << mesh1 << ", target = " << mesh2 );
      
      LOG(5, "Loading " << mesh1 << " from " << mesh1path);
      const MESH sMesh(MED_DRIVER, dataDir+mesh1path, mesh1);
      const int numSrcElems = sMesh.getNumberOfElements(MED_CELL, MED_ALL_ELEMENTS);
      LOG(1, "Source mesh has " << numSrcElems << " elements");
    
    
      LOG(5, "Loading " << mesh2 << " from " << mesh2path);
      const MESH tMesh(MED_DRIVER, dataDir+mesh2path, mesh2);
      const int numTargetElems = tMesh.getNumberOfElements(MED_CELL, MED_ALL_ELEMENTS);
    
      LOG(1, "Target mesh has " << numTargetElems << " elements");
    
      m = _interpolator->interpol_maillages(sMesh, tMesh);
    
      std::pair<int, int> eff = countNumberOfMatrixEntries(m);
      LOG(1, eff.first << " of " << numTargetElems * numSrcElems << " intersections calculated : ratio = " 
	  << double(eff.first) / double(numTargetElems * numSrcElems));
      LOG(1, eff.second << " non-zero elements of " << eff.first << " total : filter efficiency = " 
	  << double(eff.second) / double(eff.first));
    
      LOG(1, "Intersection calculation done. " << std::endl );
    
    }
  };
}

int main(int argc, char** argv)
{
  using INTERP_UTILS::PerfTestToolkit;

  assert(argc == 3);
  
  // load meshes
  const string mesh1 = argv[1];
  const string mesh2 = argv[2];

  const string mesh1path = mesh1 + ".med";
  const string mesh2path = mesh2 + ".med";

  IntersectionMatrix m;

  PerfTestToolkit testTools;

  testTools.calcIntersectionMatrix(mesh1path.c_str(), mesh1.c_str(), mesh2path.c_str(), mesh2.c_str(), m);

  testTools.dumpIntersectionMatrix(m);
    
  return 0;

}

