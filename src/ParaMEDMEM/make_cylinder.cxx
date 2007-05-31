#include "MEDMEM_Meshing.hxx"
#include <cmath>

using namespace MEDMEM;
using namespace MED_EN;
const int N=10;
int main()
{
  MESHING mesh;
  mesh.setName("cylinder_1");
  double* coords=new double[N*N*3];
   
  for (int i=0; i<N; i++)
    for (int j=0; j<N; j++)
      {
				coords[3*(i*N+j)]=cos((double)(i)/double(N)*6.28);
				coords[3*(i*N+j)+1]=sin((double)(i)/double(N)*6.28);
				coords[3*(i*N+j)+2]=(double)(j)/double((N-1));
      }
  mesh.setCoordinates(3,N*N,coords,"CARTESIAN", MED_FULL_INTERLACE);

  string names[3]={"x","y","z"};
  mesh.setCoordinatesNames(names);
  MED_EN::medGeometryElement Type[1]={MED_TRIA3};
  const int numberOfElements=N*(N-1)*2;
  
  mesh.setNumberOfTypes(1,MED_CELL);
  mesh.setTypes(Type,MED_CELL);
  mesh.setNumberOfElements(&numberOfElements, MED_CELL);
  int* conn = new int [6*N*(N-1)];
  for (int i=0; i<N; i++)
    for (int j=0; j<N-1; j++)
      {
	int i1=i+1;
	if (i1==N)
	  i1=0;
	conn[(i*(N-1)+j)*6]=i*N+j+1;
	conn[(i*(N-1)+j)*6+1]=(i1)*N+j+1;
	conn[(i*(N-1)+j)*6+2]=(i1)*N+j+1+1;
	conn[(i*(N-1)+j)*6+3]=i*N+j+1;
	conn[(i*(N-1)+j)*6+4]=i*N+j+1+1;
	conn[(i*(N-1)+j)*6+5]=(i1)*N+j+1+1;
	
	    
	  
      }
  mesh.setConnectivity(conn, MED_CELL,MED_TRIA3);
  mesh.setMeshDimension(3);
  int id=mesh.addDriver(MED_DRIVER,"/home/vb144235/resources/cylinder_3D.med", "cylinder_1");
  mesh.write(id);
}
