#include "Interpolation3DSurf.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Field.hxx"

int main(int argc, char** argv) 
{
  if (argc <4) { cout <<"erreur nb args "<<endl; exit(1);}

  double medianPlane = atof(argv[1]);
  IntersectionType intersectiontype;
  int type = atoi(argv[2]);
  bool rotate = atoi(argv[3])==1;

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

  MEDMEM::MESH target(MED_DRIVER,"/data/tmpawa/ndjinga/MEDPARA/test/cylinderquad.med","cylinder");
  MEDMEM::MESH source(MED_DRIVER,"/data/tmpawa/ndjinga/MEDPARA/test/cylindertri.med","cylinder");

  MEDMEM::Interpolation3DSurf interp;

   interp.setOptions(1e-6,2,medianPlane,intersectiontype, rotate); 
   vector<map<int,double> > surfaces= interp.interpolateMeshes(source,target);

//   map<int,double>::iterator mi;
//    cerr<< "Résultat interpolation: celulle " << 0 << endl;
//    for(mi=(surfaces[0]).begin(); mi!=(surfaces[0]).end(); mi++)
//      cerr<< (*mi).first << " ; " << (*mi).second << endl;
}
