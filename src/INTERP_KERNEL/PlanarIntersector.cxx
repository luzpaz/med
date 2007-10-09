#include "TranslationRotationMatrix.hxx"
#include "InterpolationUtils.hxx"
#include "PlanarIntersector.hxx"
#include "MEDMEM_Mesh.hxx"

using namespace MED_EN;
using namespace INTERP_UTILS;
namespace MEDMEM
{

  void PlanarIntersector::projection( vector< double>& Coords_A, vector< double>& Coords_B, 
					     int nb_NodesA, int nb_NodesB, double epsilon, 
					     double median_plane, bool do_rotate)
  {
    double normal_A[3]={0,0,0};
    double normal_B[3]={0,0,0};
    double linear_comb[3];
    double proj;
    const int DIM=3;

    //Find the normal to cells A and B
    int i_A1=1;
    while(i_A1<nb_NodesA && distance2<DIM>(&Coords_A[0],&Coords_A[DIM*i_A1])< epsilon) i_A1++;
    int i_A2=i_A1+1;
    crossprod<DIM>(&Coords_A[0], &Coords_A[DIM*i_A1], &Coords_A[DIM*i_A2],normal_A);
    while(i_A2<nb_NodesA && dotprod<DIM>(normal_A,normal_A)<epsilon)
      {
	crossprod<DIM>(&Coords_A[0], &Coords_A[DIM*i_A1], &Coords_A[DIM*i_A2],normal_A);
	i_A2++;
      }
    int i_B1=1;
    while(i_B1<nb_NodesB && distance2<DIM>(&Coords_B[0],&Coords_B[DIM*i_B1])< epsilon) i_B1++;
    int i_B2=i_B1+1;
    crossprod<DIM>(&Coords_B[0], &Coords_B[DIM*i_B1], &Coords_B[DIM*i_B2],normal_B);
    while(i_B2<nb_NodesB && dotprod<DIM>(normal_B,normal_B)< epsilon)
      {
	crossprod<DIM>(&Coords_B[0], &Coords_B[DIM*i_B1], &Coords_B[DIM*i_B2],normal_B);
	i_B2++;
      }

    if(i_A2<nb_NodesA && i_B2<nb_NodesB)
      {
	//Build the normal of the median plane
	if(dotprod<DIM>(normal_A,normal_B)<0)
	  for(int idim =0; idim< DIM; idim++) normal_A[idim] *=-1;
	for(int idim =0; idim< DIM; idim++)
	  linear_comb[idim] = median_plane*normal_A[idim] + (1-median_plane)*normal_B[idim];
	double norm= sqrt(dotprod<DIM>(linear_comb,linear_comb));

	if(norm>epsilon)
	  {
	    for(int idim =0; idim< DIM; idim++) linear_comb[idim]/=norm;
	    
	    //Project the nodes of A and B on the median plane
	    for(int i_A=0; i_A<nb_NodesA; i_A++)
	      {
		proj = dotprod<DIM>(&Coords_A[DIM*i_A],linear_comb);
		for(int idim =0; idim< DIM; idim++)
		  Coords_A[DIM*i_A+idim] -=  proj*linear_comb[idim];
	      }
	    for(int i_B=0; i_B<nb_NodesB; i_B++)
	      {
		proj = dotprod<DIM>(&Coords_B[DIM*i_B],linear_comb);
		for(int idim =0; idim< DIM; idim++)
		  Coords_B[DIM*i_B+idim] -=  proj*linear_comb[idim];
	      }
	
	    //Buid the matrix sending  A into the Oxy plane and apply it to A and B  
	    if(do_rotate)
	      {
		TranslationRotationMatrix rotation;
		//rotate3DTriangle(&Coords_A[0], &Coords_A[DIM*i_A1], &Coords_A[DIM*i_A2], rotation);
		rotate3DTriangle(&Coords_B[0], &Coords_B[DIM*i_B1], &Coords_B[DIM*i_B2], rotation);
		for (int i=0; i<nb_NodesA; i++)    rotation.transform_vector(&Coords_A[DIM*i]);
		for (int i=0; i<nb_NodesB; i++)    rotation.transform_vector(&Coords_B[DIM*i]);
	      }
	  }
      }
    else
      {
	cout<< " Maille dégénérée " << "epsilon = " << epsilon << endl;
	cout<< " i_A1= " << i_A1 << " i_A2= " << i_A2 << endl;
	cout<< " distance2<DIM>(&Coords_A[0],&Coords_A[i_A1])= " <<  distance2<DIM>(&Coords_A[0],&Coords_A[i_A1]) << endl;
	cout<< "abs(normal_A) = " << abs(normal_A[0]) << " ; " <<abs( normal_A[1]) << " ; " << abs(normal_A[2]) << endl;
	cout<< " i_B1= " << i_B1 << " i_B2= " << i_B2 << endl; 
     	cout<< " distance2<DIM>(&Coords_B[0],&Coords_B[i_B1])= " <<  distance2<DIM>(&Coords_B[0],&Coords_B[i_B1]) << endl;
	cout<< "normal_B = " << normal_B[0] << " ; " << normal_B[1] << " ; " << normal_B[2] << endl;
      }
  }

  void PlanarIntersector::rotate3DTriangle( double* PP1, double*PP2, double*PP3,
					    TranslationRotationMatrix& rotation_matrix)
  {
    //initializes
    rotation_matrix.translate(PP1);
    
    double P2w[3];
    double P3w[3];
    P2w[0]=PP2[0]; P2w[1]=PP2[1];P2w[2]=PP2[2];
    P3w[0]=PP3[0]; P3w[1]=PP3[1];P3w[2]=PP3[2];
    
    // translating to set P1 at the origin
    for (int i=0; i<2; i++)
      {
	P2w[i]-=PP1[i];
	P3w[i]-=PP1[i];
      }
   
    // rotating to set P2 on the Oxy plane
    TranslationRotationMatrix A;
    A.rotate_x(P2w);
    A.rotate_vector(P3w);
    rotation_matrix.multiply(A);

    //rotating to set P2 on the Ox axis
    TranslationRotationMatrix B;
    B.rotate_z(P2w);
    B.rotate_vector(P3w);
    rotation_matrix.multiply(B);
  
    //rotating to set P3 on the Oxy plane
    TranslationRotationMatrix C;
    C.rotate_x(P3w);
    rotation_matrix.multiply(C);
  } 

}
