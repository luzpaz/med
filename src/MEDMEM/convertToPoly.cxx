#include "MEDMEM_Mesh.hxx"
#include <iostream>

using namespace MED_EN;
using namespace MEDMEM;
using namespace std;

int main(int argc, char** argv)
{
    if( argc != 4 )
    {
      cout << "Usage: convertToPoly input_med_file output_med_file mesh_name" << endl;
      return -1;
    }
  else
    {
      string filename(argv[1]);
      string meshname(argv[3]);
      string outputfilename(argv[2]);
      
      MESH mesh(MED_DRIVER, filename, meshname);

      mesh.convertToPoly();

      int id = mesh.addDriver(MED_DRIVER,outputfilename,meshname);
      mesh.write(id);
      return 0;
    }
}
