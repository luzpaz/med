#include "Interpolation2D.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Field.hxx"

int main(int argc, char** argv) 
{
  if (argc <2) { cout <<"erreur nb args "<<endl; exit(1);}

  IntersectionType intersectiontype;
  int type = atoi(argv[1]);

  switch(type)
    {
    case 0:
      intersectiontype=MEDMEM::Triangulation;
      break;
    case 1 :
      intersectiontype=MEDMEM::Convex; 
      break;
    case 2:
      intersectiontype=MEDMEM::Generic; 
      break;
    default :
      cout <<"error second argument should be 0, 1 or 2 (triangles or convex) "<<endl; exit(1);
    }

  MEDMEM::MESH target(MED_DRIVER,"/home/vb144235/resources/square128000.med","Mesh_1");
  MEDMEM::MESH source(MED_DRIVER,"/home/vb144235/resources/square30000.med","Mesh_1");

  MEDMEM::Interpolation2D interp; 
 
  interp.setOptions(1e-6,2,intersectiontype);
  vector<map<int,double> > surfaces= interp.interpolateMeshes(target,source); 

//   map<int,double>::iterator mi;
//    cerr<< "R�sultat interpolation: celulle " << 0 << endl;
//    for(mi=(surfaces[0]).begin(); mi!=(surfaces[0]).end(); mi++)
//      cerr<< (*mi).first << " ; " << (*mi).second << endl;

}
