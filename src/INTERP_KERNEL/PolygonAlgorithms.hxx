#ifndef _PolygonAlgorithms_HXX_
#define _PolygonAlgorithms_HXX_

#include <deque>
#include <map>

namespace INTERP_UTILS
{
  template< int DIM>
  class VertexLess
  {
  public:
    bool operator()(const double * P_1, const double * P_2) 
    {
      for(int idim=0; idim<DIM; idim++)
	{	 
	  if(P_1[idim] < P_2[idim] )  return true;
	  else if( P_1[idim] > P_2[idim]) return false;
	}
      return false;
    }
  };
  
  template< int DIM>
  class PolygonAlgorithms
  {
  public:
    PolygonAlgorithms(double epsilon, double precision);
    deque<double> intersect_convex_polygons(const double* P_1,const double* P_2, int N1, int N2);

    //Not yet tested
    int convex_decomposition(const double * P, int N, vector< map< int,int > >& components,
			     vector< int >& components_index, const double epsilon);
    
  private:
    deque< double > _Inter;/* vertices of the intersection  P1^P2 */
    vector< pair< int,int > > _End_segments; /* segments containing inter final edges */   
    /* status list of segments (ending point, starting point) intersected by the sweeping line */
    /* and a boolean true if the ending point is in the intersection */
    multimap< int, pair< int,bool> > _Status;
    bool _Is_in_intersection;
    bool _Terminus;
    double _Vdouble[DIM];
    double _Epsilon;
    double _Precision;

    void define_indices(int& i_loc, int& i_next, int& i_prev, 
			const double *& Poly1, const double *& Poly2,
			int& j1, int& j1_glob, int& j2, int& j2_glob,
			int& j3, int& j3_glob, int& j4, int& j4_glob, 
			int& i_glob, int& i_next_glob, int& i_prev_glob, 
			const double * P_1, const double * P_2, 
			int N1, int N2, int sign);
    void add_crossings( const double * A, const double * B, int i , int i_next,
			const double * C, const double * D, int j1, int j2,
			const double * E, const double * F, int j3, int j4,
			const double * G);
    void add_crossing0(const double * A, const double * B, int i, int i_next,
		       const double * C, const double * D, int j, int j_next);
    void add_crossing( double * ABCD, pair< int,int > i_i_next, pair< int,int > j_j_next);
    void add_new_vertex( int i, int i_glob, int i_next_glob, int i_prev_glob, const double * P);
    bool intersect_segment_segment(const double * A,  const double * B, const double * C,
				   const double * D,  const double * E, double * V);


    //Not yet tested
    void convex_decomposition(const double* P, int N, double* n,  vector< int > subP, int NsubP, 
			      vector< map< int,int > >& components, vector< int >& components_index,
			      int& Ncomp, int sign, const double epsilon);
    void conv_hull(const double *P, int N, double * n,  map< int,int >& subP,
		   map< int,int >& not_in_hull, int& NsubP, const double epsilon);
  };
};

#include "PolygonAlgorithms.cxx"

#endif
