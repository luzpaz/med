#include "MEDMEM_Mesh.hxx"
#include "InterpolationUtils.hxx"
#include "PlanarIntersector.hxx"
#include "PolygonAlgorithms.hxx"
#include "GenericIntersector.hxx"

using namespace MED_EN;
using namespace MEDMEM;

namespace MEDMEM
{
  template <int DIM>
  GenericIntersector<DIM>::GenericIntersector(const MEDMEM::MESH& mesh_A, const MEDMEM::MESH& mesh_B, 
					      double DimCaracteristic, double Precision,
					      double MedianPlane, bool do_rotate, int PrintLevel )
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
   
   if( _PrintLevel >= 1)
     {	 
       cout<< "  _intersection_type = Generic " <<endl;
       if(_Do_rotate) cout<< "  _do_rotate = true"<<endl;
       else cout<< "  _do_rotate = false"<<endl;
       cout<< "warning: Genric case not yet tested" << endl;
     }
 }

   template <int DIM>
   double GenericIntersector<DIM>::intersectCells(int icell_A,   int icell_B, 
						 int nb_NodesA, int nb_NodesB)
  {
    double result=0;
    
    /******* Obtain the coordinates of A and B ********/
    vector< double> Coords_A (DIM*nb_NodesA);
    vector< double> Coords_B (DIM*nb_NodesB);
    for (int idim=0; idim<DIM; idim++)
      {
	for (int i_A=0; i_A<nb_NodesA; i_A++)
	  Coords_A[DIM*i_A+idim] = _Coords_A[DIM*(_Connect_A[_Conn_index_A[icell_A-1]-1+i_A]-1)+idim];
	for (int i_B=0; i_B<nb_NodesB; i_B++)
	  Coords_B[DIM*i_B+idim] = _Coords_B[DIM*(_Connect_B[_Conn_index_B[icell_B-1]-1+i_B]-1)+idim];
      }
    
    /*** project cells A and B on the median plane ***/
    /*** and rotate the median plane ***/
    if(DIM==3) projection(Coords_A, Coords_B, nb_NodesA, nb_NodesB, _Epsilon,_MedianPlane,_Do_rotate);
    
    /**********Decompose each polygon into convex component***********/
    INTERP_UTILS::PolygonAlgorithms<DIM> P(_Epsilon, _Precision);
    vector< map< int,int > > components_A (0), components_B(0);
    vector< int > components_index_A(0), components_index_B(0); 
    int nb_comp_A =P.convex_decomposition( &Coords_A[0], nb_NodesA, 
				       components_A, components_index_A, _Epsilon);
    int nb_comp_B =P.convex_decomposition( &Coords_B[0], nb_NodesB, 
				       components_B, components_index_B, _Epsilon);

    if( _PrintLevel >= 3)
      {
	cout<<endl<< "Cell coordinates after projection"<<endl;
	cout<< "icell_A= " << icell_A << ", nb nodes A= " <<  nb_NodesA << endl;
	for(int i_A =0; i_A< nb_NodesA; i_A++)
	  for (int idim=0; idim<DIM; idim++) cout<< Coords_A[DIM*i_A+idim]<< " "; cout << endl;
	cout<< "icell_B= " << icell_B << ", nb nodes B= " <<  nb_NodesB << endl;
	for(int i_B =0; i_B< nb_NodesB; i_B++)
	  for (int idim=0; idim<DIM; idim++) cout<< Coords_B[DIM*i_B+idim]<< " "; cout << endl;
      }
    
    /*** Compute the intersection area ***/
    for(int i_comp_A = 0; i_comp_A<nb_comp_A; i_comp_A++)
      {
	int sign_A = components_index_A[i_comp_A] <0 ? -1:1;
	int nb_nodes_A = sign_A * components_index_A[i_comp_A];
        double * Coords_subP_A;	
	map< int,int >::iterator mi_A = (components_A[i_comp_A]).begin();

	for(int i_comp_B = 0; i_comp_B<nb_comp_B; i_comp_B++)
	  {
	    int sign_B = components_index_B[i_comp_B] <0 ? -1:1;
	    int nb_nodes_B = sign_B * components_index_B[i_comp_B];
	    double * Coords_subP_B;
	    map< int,int >::iterator mi_B = (components_B[i_comp_B]).begin();
	    
	    for (int i_A=0; i_A< nb_nodes_A; i_A++)
	      {
		for (int idim=0; idim<DIM; idim++) 
		  Coords_subP_A[DIM*i_A+idim] = Coords_A[DIM*(* mi_A).second+idim];
		mi_A++;
	      }
	    for (int i_B=0; i_B< nb_nodes_B; i_B++)
	      {
		for (int idim=0; idim<DIM; idim++) 
		  Coords_subP_B[DIM*i_B+idim] = Coords_B[DIM*(* mi_B).second+idim];
		mi_B++;
	      }
	    
	    deque<double> inter =  
	      P.PolygonAlgorithms<DIM>::intersect_convex_polygons(&Coords_subP_A[0], &Coords_subP_B[0],
								  nb_nodes_A, nb_nodes_B);  
	    double area[DIM];
	    int nb_inter =((int)inter.size())/DIM;
	    for(int i = 1; i<nb_inter-1; i++)
	      {
		INTERP_UTILS::crossprod<DIM>(&inter[0],&inter[DIM*i],&inter[DIM*(i+1)],area);
		result += 0.5 * norm<DIM>(area)*sign_A*sign_B;
	      }	
	    //DEBUG prints
	    if(_PrintLevel >= 3)
	      {
		cout<<endl<<"Number of nodes of the intersection = "<<  nb_inter <<endl<<endl;
		for(int i=0; i< nb_inter; i++)
		  for (int idim=0; idim<2; idim++) 	cout<< inter[2*i+idim]<< " "; cout << endl; 
	      }
	  }
      }
    
    return result;
  }
  
}
