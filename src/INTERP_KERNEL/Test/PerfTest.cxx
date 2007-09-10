#include "Interpolation3D.hxx"
#include "TestingUtils.hxx"
#include <cassert>
#include <string>

int main(int argc, char** argv)
{
  assert(argc == 3);
  
  // load meshes
  const string mesh1 = argv[1];
  const string mesh2 = argv[2];

  const string mesh1path = "meshes/" + mesh1 + ".med";
  const string mesh2path = "meshes/" + mesh2 + ".med";

  IntersectionMatrix m;

  calcIntersectionMatrix(mesh1path.c_str(), mesh1.c_str(), mesh2path.c_str(), mesh2.c_str(), m);

  // dumpIntersectionMatrix(m);
  
  return 0;

}
