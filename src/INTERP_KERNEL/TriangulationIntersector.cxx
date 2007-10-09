#include "MEDMEM_Mesh.hxx"
#include "InterpolationUtils.hxx"
#include "PlanarIntersector.hxx"
#include "TriangulationIntersector.hxx"

//using namespace std;
using namespace MED_EN;
using namespace INTERP_UTILS;
//using namespace MEDMEM;

namespace MEDMEM
{
  template <int DIM> 
  TriangulationIntersector<DIM>::TriangulationIntersector(const MEDMEM::MESH& mesh_A, const MEDMEM::MESH& mesh_B, 
							  double DimCaracteristic, double Precision,
							  double MedianPlane, int PrintLevel)
    :PlanarIntersector(mesh_A,mesh_B, DimCaracteristic, Precision, MedianPlane)
 {
   _Connect_A= mesh_A.getConnectivity(MED_EN::MED_FULL_INTERLACE,MED_EN::MED_NODAL,
				     MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
   _Connect_B= mesh_B.getConnectivity(MED_EN::MED_FULL_INTERLACE,MED_EN::MED_NODAL,
				     MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
   _Conn_index_A= mesh_A.getConnectivityIndex(MED_EN::MED_NODAL, MED_EN::MED_CELL);
   _Conn_index_B= mesh_B.getConnectivityIndex(MED_EN::MED_NODAL, MED_EN::MED_CELL);
   _Coords_A = mesh_A.getCoordinates(MED_EN::MED_FULL_INTERLACE);
   _Coords_B = mesh_B.getCoordinates(MED_EN::MED_FULL_INTERLACE);
   _DimCaracteristic = DimCaracteristic;
   _Precision = Precision;
   _MedianPlane = MedianPlane;
   _do_rotate = true;
   _PrintLevel= PrintLevel;

   if( _PrintLevel >= 1)
     {	 
       cout<< "_intersection_type = triangles " <<endl;
       if(DIM==3)   cout<< "_do_rotate = true"<<endl;
     }
 }

   template <int DIM>
   double TriangulationIntersector<DIM>::intersectCells(int icell_A,   int icell_B, 
							int nb_NodesA, int nb_NodesB)
  {
    double result=0;
    
    //Obtain the coordinates of A and B
    vector< double> Coords_A (DIM*nb_NodesA);
    vector< double> Coords_B (DIM*nb_NodesB);
    for (int idim=0; idim<DIM; idim++)
      {
	for (int i_A=0; i_A<nb_NodesA; i_A++)
	  Coords_A[DIM*i_A+idim] = _Coords_A[DIM*(_Connect_A[_Conn_index_A[icell_A-1]-1+i_A]-1)+idim];
	for (int i_B=0; i_B<nb_NodesB; i_B++)
	  Coords_B[DIM*i_B+idim] = _Coords_B[DIM*(_Connect_B[_Conn_index_B[icell_B-1]-1+i_B]-1)+idim];
      }
    
    //project cells A and B on the median plane and rotate the median plane
    if(DIM==3) projection(Coords_A, Coords_B, nb_NodesA, nb_NodesB,
			  _DimCaracteristic * _Precision, _MedianPlane, _do_rotate);
    
    //Compute the intersection area
    double area[DIM];
    for(int i_A = 1; i_A<nb_NodesA-1; i_A++)
      {
	for(int i_B = 1; i_B<nb_NodesB-1; i_B++)
	  {
	    vector<double> inter;
	    INTERP_UTILS::intersec_de_triangle(&Coords_A[0],&Coords_A[DIM*i_A],&Coords_A[DIM*(i_A+1)],
					       &Coords_B[0],&Coords_B[DIM*i_B],&Coords_B[DIM*(i_B+1)],
					       inter, _DimCaracteristic, _Precision);
	    int nb_inter=((int)inter.size())/2;
	    if(nb_inter >3) inter=reconstruct_polygon(inter);
	    for(int i = 1; i<nb_inter-1; i++)
	      {
		INTERP_UTILS::crossprod<2>(&inter[0],&inter[2*i],&inter[2*(i+1)],area);
		result +=0.5*norm<2>(area);
	      }
	    //DEBUG prints
	    if(_PrintLevel >= 3)
	      {
		cout<<endl<<"Number of nodes of the intersection = "<< nb_inter <<endl;
		for(int i=0; i< nb_inter; i++)
		  {for (int idim=0; idim<2; idim++) cout<< inter[2*i+idim]<< " "; cout << endl;}
	      }
	  }
      }
    
    if(_PrintLevel >= 3) 
         {
	cout<<endl<< "Cell coordinates after projection"<<endl;
	cout<<endl<< "icell_A= " << icell_A << ", nb nodes A= " <<  nb_NodesA << endl;
	for(int i_A =0; i_A< nb_NodesA; i_A++)
	  {for (int idim=0; idim<2; idim++) cout<< Coords_A[DIM*i_A+idim]<< " "; cout << endl;}
	cout<<endl<< "icell_B= " << icell_B << ", nb nodes B= " <<  nb_NodesB << endl;
	for(int i_B =0; i_B< nb_NodesB; i_B++)
	  {for (int idim=0; idim<2; idim++) cout<< Coords_B[DIM*i_B+idim]<< " "; cout << endl;}
	cout<<endl<<"Intersection area= " << result << endl;
	 }
     
    return result;
  }
  
}
