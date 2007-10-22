#include "MEDMEM_Mesh.hxx"
#include "InterpolationUtils.hxx"
#include "PlanarIntersector.hxx"
#include "PolygonAlgorithms.hxx"
#include "ConvexIntersector.hxx"

using namespace MED_EN;
using namespace MEDMEM;

namespace MEDMEM
{
  template <int DIM>
  ConvexIntersector<DIM>::ConvexIntersector(const MEDMEM::MESH& mesh_A, const MEDMEM::MESH& mesh_B, 
					    double DimCaracteristic, double Precision,
					    double MedianPlane, bool do_rotate , int PrintLevel)
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
   _Precision = Precision;
   _Epsilon = Precision*DimCaracteristic;
   _MedianPlane = MedianPlane;
   _Do_rotate = do_rotate;
   _PrintLevel= PrintLevel;

//     vector< vector< const double *, int > _Ordered_nodes_A;
//     vector< vector< const double *, int > _Ordered_nodes_B;
//     vector< vector< double > >  _Coords_A;
//     vector< vector< double > >  _Coords_B;
//     vector< int > _Nb_dist_nodes_A;
//     vector< int > _Nb_dist_nodes_B;   

   if( _PrintLevel >= 1)
     {	 
       cout<< "  _intersection_type = convex " <<endl;
       if(DIM==3){
	 if(_Do_rotate) cout<< "  _do_rotate = true"<<endl;
	 else cout<< "  _do_rotate = false"<<endl;
       }
     }
 }

   template <int DIM>
   double ConvexIntersector<DIM>::intersectCells(int icell_A,   int icell_B, 
						 int nb_NodesA, int nb_NodesB)
  {
    double result=0;
    
    /*** Obtain the coordinates of A and B ***/
    vector< double> Coords_A (DIM*nb_NodesA);
    vector< double> Coords_B (DIM*nb_NodesB);
    int nb_dist_NodesA=nb_NodesA;
    int nb_dist_NodesB=nb_NodesB;
    int i_last = nb_NodesA - 1;
    const double * Pi_last= _Coords_A + DIM*(_Connect_A[_Conn_index_A[icell_A-1]-1+i_last]-1);

    for (int i_A=0; i_A<nb_NodesA; i_A++)
      {
	const double * Pi_A = _Coords_A + DIM*(_Connect_A[_Conn_index_A[icell_A-1]-1+i_A]-1);
	if(distance2<DIM>(Pi_last, Pi_A)> _Epsilon)
	  {
	    for (int idim=0; idim<DIM; idim++) Coords_A[DIM*i_A+idim] = *(Pi_A+idim);
	    i_last=i_A; Pi_last = Pi_A;
	  }
	  else nb_dist_NodesA--;
      }
    i_last = nb_NodesB - 1;
    Pi_last= _Coords_B + DIM*(_Connect_B[_Conn_index_B[icell_B-1]-1+i_last]-1);
    for (int i_B=0; i_B<nb_NodesB; i_B++)
      {
	const double * Pi_B = _Coords_B + DIM*(_Connect_B[_Conn_index_B[icell_B-1]-1+i_B]-1);
	if(distance2<DIM>(Pi_last, Pi_B)> _Epsilon)
	  {
	    for (int idim=0; idim<DIM; idim++) Coords_B[DIM*i_B+idim] = *(Pi_B + idim);
	    i_last=i_B; Pi_last = Pi_B;
	  }
	  else nb_dist_NodesB--;
      }
      
    /*** project cells A and B on the median plane ***/
    /***  and rotate the median plane ***/
    if(DIM==3) projection(Coords_A, Coords_B, nb_dist_NodesA, nb_dist_NodesB, _Epsilon,_MedianPlane,_Do_rotate); 
    
    /*** Compute the intersection area ***/
    INTERP_UTILS::PolygonAlgorithms<DIM> P( _Epsilon, _Precision);
    deque<double> inter =  P.PolygonAlgorithms<DIM>::intersect_convex_polygons(&Coords_A[0], &Coords_B[0],
									       nb_dist_NodesA, nb_dist_NodesB);
    double area[DIM];
    int nb_inter =((int)inter.size())/DIM;
    for(int i = 1; i<nb_inter-1; i++)
      {
	INTERP_UTILS::crossprod<DIM>(&inter[0],&inter[DIM*i],&inter[DIM*(i+1)],area);
	result +=0.5*norm<DIM>(area);
      }

    //DEBUG prints
    if( _PrintLevel >= 3)
      {	
	cout<<endl<< "Cell coordinates after projection"<<endl;
	cout<<endl<< "icell_A= " << icell_A << ", nb nodes A= " <<  nb_NodesA << endl;
	for(int i_A =0; i_A< nb_dist_NodesA; i_A++)
	  {for (int idim=0; idim<DIM; idim++) cout<< Coords_A[DIM*i_A+idim]<< " "; cout << endl;}
	cout<<endl<< "icell_B= " << icell_B << ", nb nodes B= " <<  nb_NodesB << endl;
	for(int i_B =0; i_B< nb_dist_NodesB; i_B++)
	  {for (int idim=0; idim<DIM; idim++) cout<< Coords_B[DIM*i_B+idim]<< " "; cout << endl;}
	cout<<endl<<"Number of nodes of the intersection = "<<  nb_inter <<endl;
       	for(int i=0; i<  nb_inter; i++)
	  {for (int idim=0; idim<DIM; idim++) cout<< inter[DIM*i+idim]<< " "; cout << endl;}
	cout<<endl<<"Intersection area = " << result << endl;
     }
    
    return result;
  }

}
