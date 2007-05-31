#include "MEDMEM_Meshing.hxx"

int main(int argc, char** argv)
{
  if (argc!=3) {cout <<"error in arguments"<<endl; exit(1);}
  int N;
  sscanf(argv[1], "%d", &N);
  cout << "number of nodes "<<N<<endl;
  string meshname (argv[2]);
  MEDMEM::MESHING mesh;
  mesh.setName("cylinder");
  double* coords=new double[3*N*N];
  
  int* conn=new int [6*(N-1)*(N-1)];
  for (int i=0; i<N; i++)
    for (int j=0; j<N; j++)
      {
	coords[3*(j+i*N)]=double(i)/(N-1);
	coords[3*(j+i*N)+1]=double(i)/(N-1);
	coords[3*(j+i*N)+2]=double(j)/(N-1);
      }
  for (int i=0; i<N-1; i++)
    for (int j=0; j<N-1; j++)
      {
	conn[6*(j+i*(N-1))]=j+i*(N)+1;
	conn[6*(j+i*(N-1))+1]=j+1+i*(N)+1;
	conn[6*(j+i*(N-1))+2]=j+1+(i+1)*(N)+1;
	conn[6*(j+i*(N-1))+3]=j+i*(N)+1;
	conn[6*(j+i*(N-1))+4]=j+(i+1)*(N)+1;
	conn[6*(j+i*(N-1))+5]=j+1+(i+1)*(N)+1;
      }
  
  mesh.setNumberOfNodes(N*N);
  mesh.setCoordinates(3, N*N,coords, "CARTESIAN",MED_EN::MED_FULL_INTERLACE);
  mesh.setNumberOfTypes(1,MED_EN::MED_CELL);
  MED_EN::medGeometryElement type[1] = {MED_EN::MED_TRIA3};
  mesh.setTypes(type,MED_EN::MED_CELL);
  int nb=(N-1)*(N-1)*2;
  mesh.setNumberOfElements(&nb,MED_EN::MED_CELL);
  mesh.setMeshDimension(2);

  mesh.setConnectivity(conn,MED_EN::MED_CELL, MED_EN::MED_TRIA3);
  mesh.setMeshDimension(2);
  int id=mesh.addDriver(MEDMEM::MED_DRIVER, meshname,"cylinder");
  mesh.write(id);
}
