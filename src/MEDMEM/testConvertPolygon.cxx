#include "MEDMEM_Mesh.hxx"

int main()
{
  //  MEDMEM::MESH mesh(MEDMEM::MED_DRIVER, "/export/home/vb144235/MED_INTEGRATION/MED_INSTALL/share/salome/resources/MedFiles/poly3D.med","poly3D");
  MEDMEM::MESH mesh(MEDMEM::MED_DRIVER, "/export/home/bergeaud/Extruded_hexagon_fluidmesh.med","FluidMesh_1");
  mesh.convertToPoly();
  int id=mesh.addDriver(MEDMEM::MED_DRIVER,"/export/home/bergeaud/Zxtruded_hexagon_fluidmesh_polyhedra.med","Fluidmesh_1");
  mesh.write(id);
}
