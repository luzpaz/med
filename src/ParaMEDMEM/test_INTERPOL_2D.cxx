#include "INTERPOLATION_2D.hxx"
#include "MEDMEM_Mesh.hxx"

int main()
{
  MEDMEM::MESH source(MED_DRIVER,"/home/vb144235/resources/square128000.med","Mesh_1");
  MEDMEM::MESH target(MED_DRIVER,"/home/vb144235/resources/square30000.med","Mesh_1");

   ParaMEDMEM::INTERPOLATION_2D interp;
	 //   interp.setOptions(1e-6,1,2);
  interp.interpol_maillages(source,target);
}
