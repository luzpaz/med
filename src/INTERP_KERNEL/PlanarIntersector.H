#include "PlanarIntersector.hxx"
#include "MEDMEM_Mesh.hxx"

namespace MEDMEM {
/*!
    \brief creates the bounding boxes for all the cells of mesh \a mesh

    The method accepts mixed meshes (containing triangles and quadrangles).
    The vector returned is of dimension 6*nb_elems with bounding boxes stored as xmin1, xmax1, ymin1, ymax1, zmin1, zmax1, xmin2, xmax2, ymin2,... 
    The returned pointer must be deleted by the calling code.
    
    \param mesh structure pointing to the mesh
    \param bbox vector containing the bounding boxes
   */
  template <int DIM>
  void PlanarIntersector::createBoundingBoxes(const MEDMEM::MESH& mesh, vector<double>& bbox)
  {
    /* We build the segment tree for locating possible matching intersections*/
    int nbelems = mesh.getNumberOfElements(MED_EN::MED_CELL, MED_EN::MED_ALL_ELEMENTS);
    int nbtypes = mesh.getNumberOfTypes(MED_EN::MED_CELL);
    const MED_EN::medGeometryElement* types = mesh.getTypes(MED_EN::MED_CELL);
    bbox.resize(2*DIM* nbelems);
    const double* coords = mesh.getCoordinates(MED_EN::MED_FULL_INTERLACE);
    const int* conn = mesh.getConnectivity(MED_EN::MED_FULL_INTERLACE, 
					   MED_EN::MED_NODAL,MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
      const int* conn_index =  mesh.getConnectivityIndex(MED_EN::MED_NODAL, MED_EN::MED_CELL);  
    int ibox=0;
    int type_max_index;//maximum cell number for a given type
    int type_min_index=1;//minimum cell number for a given type
    int icell;
    for (int itype = 0; itype<nbtypes; itype++)
      {
	int nbelem_type = mesh.getNumberOfElements(MED_EN::MED_CELL, types[itype]);
	int nb_nodes_per_elem = types[itype]%100;
	type_max_index = type_min_index + nbelem_type;
	
	for (icell=type_min_index; icell<type_max_index; icell++)
	  {
	    //initializing bounding box limits
	    for(int idim=0; idim<DIM; idim++)
	      {
		bbox[2*DIM*ibox+2*idim]   =  HUGE;
		bbox[2*DIM*ibox+2*idim+1] = -HUGE;
	      }
	    //updating the bounding box with each node of the element
	    for (int j=0; j<nb_nodes_per_elem; j++)
	      {
		const double* coord_node=coords + DIM*(conn[conn_index[icell-1]-1+j]-1);
		for(int idim=0; idim<DIM; idim++)
		  {		
		    double x=*(coord_node+idim);
		    bbox[ibox*2*DIM + 2*idim]   = (bbox[ibox*2*DIM + 2*idim]  <x)?bbox[ibox*2*DIM + 2*idim  ]:x;
		    bbox[ibox*2*DIM + 2*idim+1] = (bbox[ibox*2*DIM + 2*idim+1]>x)?bbox[ibox*2*DIM + 2*idim+1]:x;
		  }
	      }
	    ibox++;
	  }
	type_min_index = type_max_index;
      }			      
  }
  /*
    Computes the bouding box of a given element
  */
  template <int DIM>
  inline void PlanarIntersector::getElemBB(double* bb, const MEDMEM::MESH& mesh, int iP, int nb_nodes)
  {
    const double* coords = mesh.getCoordinates(MED_EN::MED_FULL_INTERLACE);
    const int* conn_index =  mesh.getConnectivityIndex(MED_EN::MED_NODAL, MED_EN::MED_CELL);
    const int* conn = mesh.getConnectivity(MED_EN::MED_FULL_INTERLACE, 
					   MED_EN::MED_NODAL,MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
    //initializing bounding box limits
    for(int idim=0; idim<DIM; idim++)
      {
	bb[2*idim  ] =  HUGE;
	bb[2*idim+1] = -HUGE;
      }
    
    for (int i=0; i<nb_nodes; i++)
      {
	//MN: iP= cell index, not node index, use of connectivity array ?
	const double* coord_node=coords + DIM*(conn[conn_index[iP-1]-1+i]-1);
	for(int idim=0; idim<DIM; idim++)
	  {		
	    double x = *(coord_node+idim);
	    //double y = *(mesh.getCoordinates(MED_FULL_INTERLACE)+3*(iP+i)+1);
	    bb[2*idim  ] = (x<bb[2*idim  ])?x:bb[2*idim  ];
	    bb[2*idim+1] = (x>bb[2*idim+1])?x:bb[2*idim+1];
	  }		
      }
  }

  /*! Readjusts a set of bounding boxes so that they are extended
    in all dimensions for avoiding missing interesting intersections

    \param bbox vector containing the bounding boxes
  */

  template <int DIM>
  void  PlanarIntersector::adjustBoundingBoxes(vector<double>& bbox, double Surf3DAdjustmentEps)
  {
    /* We build the segment tree for locating possible matching intersections*/

    int size = bbox.size()/(2*DIM);
    for (int i=0; i<size; i++)
      {
	double max=-HUGE;
	for(int idim=0; idim<DIM; idim++)
	  {		
	    double Dx=bbox[i*2*DIM+1+2*idim]-bbox[i*2*DIM+2*idim];
	    max=(max<Dx)?Dx:max;
	  }
	for(int idim=0; idim<DIM; idim++)
	  {		
	    bbox[i*2*DIM+2*idim  ] -= Surf3DAdjustmentEps*max;
	    bbox[i*2*DIM+2*idim+1] += Surf3DAdjustmentEps*max;
	  }
      }
  }
}
