#include "MEDMEM_Mesh.hxx"
#include "Interpolation3DSurf.hxx"
#include "PlanarIntersector.hxx"
#include "PlanarIntersector.H"
#include "TriangulationIntersector.hxx"
#include "TriangulationIntersector.cxx"
#include "ConvexIntersector.hxx"
#include "ConvexIntersector.cxx"
#include "GenericIntersector.hxx"
#include "GenericIntersector.cxx"
#include "BBTree.H"
#include<time.h>

using namespace MED_EN;

namespace MEDMEM
{

  Interpolation3DSurf::Interpolation3DSurf()
  { 
    _Intersection_type= MEDMEM::Triangulation;
    _MedianPlane=0.5;
    _Do_rotate=true;
    _Precision=1.0E-12;
    _DimCaracteristic=1;
    _Surf3DAdjustmentEps=0.0001;
    _PrintLevel=0;
  }
  
  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
  /*   Options :                                        */
  /*     Precision : for geometric computation          */
  /*     PrintLevel : between 0 and 3                   */
  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
  void Interpolation3DSurf::setOptions(double Precision, int PrintLevel, double MedianPlane, IntersectionType intersection_type, bool do_rotate)
  {
    _Intersection_type=intersection_type;
    _MedianPlane=MedianPlane;
    _Do_rotate=do_rotate;
    _Precision=Precision;
    _PrintLevel=PrintLevel;
 }
  
  
  /***************************************************************/
  /* Main function to interpolate triangular or quadratic meshes */
  /***************************************************************/
  vector<map<int,double> > Interpolation3DSurf::interpolateMeshes(const MEDMEM::MESH& myMesh_S,
								   const MEDMEM::MESH& myMesh_P)
  {
    long global_start =clock();    
    /***********************************************************/
    /* Check both meshes are made of triangles and quadrangles */
    /***********************************************************/
 
    int NumberOfCellsTypes_S = myMesh_S.getNumberOfTypes(MED_EN::MED_CELL);
    int NumberOfCellsTypes_P = myMesh_P.getNumberOfTypes(MED_EN::MED_CELL);
    if  ( NumberOfCellsTypes_S > 2  || NumberOfCellsTypes_P >2)
      cout<<"Interpolation3DSurf::interpolateMeshes: one of the two meshes contains more than two types of cells"<<endl;
    string* Type_S = myMesh_S.getCellTypeNames(MED_EN::MED_CELL);
    string* Type_P = myMesh_P.getCellTypeNames(MED_EN::MED_CELL);
    
    if((Type_S[0] != "MED_TRIA3" && Type_S[0] != "MED_QUAD4") || (Type_P[0] != "MED_TRIA3" && Type_P[0] != "MED_QUAD4"))
      cout<<"Interpolation3DSurf::interpolateMeshes: one of the two meshes is neither linear triangular nor linear quadratic"<<endl;
    //VB
    delete[] Type_S;
    delete[] Type_P;
    int nbMaille_S =myMesh_S.getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
    int nbMaille_P =myMesh_P.getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
    
    /**************************************************/
    /* Search the characteristic size of the meshes   */
    /**************************************************/
    
    vector<vector<double> > BoxS=myMesh_S.getBoundingBox();
    vector<vector<double> > BoxP=myMesh_P.getBoundingBox();
    double diagonalS=sqrt((BoxS[1][0]-BoxS[0][0])*(BoxS[1][0]-BoxS[0][0])
			+(BoxS[1][1]-BoxS[0][1])*(BoxS[1][1]-BoxS[0][1])
                        +(BoxS[1][2]-BoxS[0][2])*(BoxS[1][2]-BoxS[0][2]));
    double DimCaracteristic_S=diagonalS/nbMaille_S;
    double diagonalP=sqrt((BoxP[1][0]-BoxP[0][0])*(BoxP[1][0]-BoxP[0][0])
			+(BoxP[1][1]-BoxP[0][1])*(BoxP[1][1]-BoxP[0][1])
                        +(BoxP[1][2]-BoxP[0][2])*(BoxP[1][2]-BoxP[0][2]));
    double DimCaracteristic_P=diagonalP/nbMaille_P;
    
    _DimCaracteristic=min(DimCaracteristic_S, DimCaracteristic_P);
    if (_PrintLevel>=1)
      {
	cout<<"  - Characteristic size of the source mesh : "<< DimCaracteristic_S<<endl;
	cout<<"  - Characteristic size of the target mesh: "<< DimCaracteristic_P<<endl;
	cout << "Interpolation3DSurf::computation of the intersections"<<endl;
      }
    
    PlanarIntersector* intersector;
    
    switch (_Intersection_type)
      {
      case  MEDMEM::Triangulation:
	intersector=new TriangulationIntersector<3>(myMesh_P,myMesh_S, _DimCaracteristic,_Precision, 
						    _MedianPlane, _PrintLevel);
	break;
      case MEDMEM::Convex:
	intersector=new ConvexIntersector<3>(myMesh_P,myMesh_S, _DimCaracteristic, _Precision,
					     _MedianPlane, _Do_rotate, _PrintLevel);
	break;
      case MEDMEM::Generic:
	intersector=new GenericIntersector<3>(myMesh_P,myMesh_S, _DimCaracteristic,_Precision,
					      _MedianPlane, _Do_rotate, _PrintLevel);
	break;
      }

    /****************************************************************/
    /* Create a search tree based on the bounding boxes             */
    /* Instanciate the intersector and initialise the result vector */
    /****************************************************************/
 
    long start_filtering=clock();
 
    vector<double> bbox;
    intersector->createBoundingBoxes<3>(myMesh_S,bbox); // create the bounding boxes
    intersector->adjustBoundingBoxes<3>(bbox, _Surf3DAdjustmentEps); // expand them so that no intersection is missed  
    BBTree<3> my_tree(&bbox[0], 0, 0,nbMaille_S );//creating the search structure 

    long end_filtering=clock();

    map<int,double> MAP_init;
    vector<map<int,double> > result_areas(nbMaille_P,MAP_init);//on initialise.

    /****************************************************/
    /* Loop on the target cells - core of the algorithm */
    /****************************************************/
    const MED_EN::medGeometryElement* types = myMesh_P.getTypes(MED_EN::MED_CELL);
    int type_max_index=0;//maximum cell number for a given type
    int type_min_index=0;//minimum cell number for a given type
    int i_P=0;//global index of cell

    long start_intersection=clock();

    for (int itype = 0; itype<NumberOfCellsTypes_P; itype++)
      {
	int nb_nodesP=types[itype]%100;
	int nbelem_type = myMesh_P.getNumberOfElements(MED_EN::MED_CELL, types[itype]);
	type_max_index +=  nbelem_type;
	for( i_P=type_min_index; i_P<type_max_index ; i_P++)
	  {
	    vector<int> intersecting_elems;
	    double bb[6];
	    intersector->getElemBB<3>(bb,myMesh_P,i_P+1,nb_nodesP);
	    my_tree.getIntersectingElems(bb, intersecting_elems);
	    int nb_intersecting_elems = intersecting_elems.size();	    
	    //browsing all the i_S (from mesh S) elems that can 
	    //intersect elem i_P (from mesh P)
	    for (int ielem=0; ielem<nb_intersecting_elems;ielem++)
	      {
		//BBTree structure returns numbers between 0 and n-1
		int i_S=intersecting_elems[ielem]; //MN: Global number of cell ?
		int nb_nodesS=myMesh_S.getElementType(MED_EN::MED_CELL,i_S+1)%100;
		double surf=intersector->intersectCells(i_P+1,i_S+1,nb_nodesP,nb_nodesS);
		(result_areas[i_P]).insert(make_pair(i_S+1,surf));
		//rempli_vect_d_intersect(i_P+1,i_S+1,MaP,MaS,result_areas);
	      }
	    intersecting_elems.clear();
	  }
	type_min_index = type_max_index;
      }
    delete intersector;

    /***********************************/
    /*        DEBUG prints             */
    /***********************************/

    if (_PrintLevel >=1)
      {
	long end_intersection=clock();
	if (_PrintLevel >=2)
	  {
	    cout<<endl<<"Printing intersection areas:"<<endl<<endl;
	    cout<<"(source cell, target cell): intersection areas"<<endl;
	    double total=0.0;
	    double total_interm=0.0;
	    int nb_result_areas = result_areas.size();
	    for(int i=0; i< nb_result_areas;i++)
	      { 
		map<int,double>::iterator surface;
		total_interm=0.0;
		for( surface=result_areas[i].begin();surface!=result_areas[i].end();surface++)
		  {
		    cout<<"    ("<<i+1<<" , "<<(*surface).first<<")"<<" : "<<(*surface).second<<endl;
		    total_interm +=(*surface).second;
		  }
		cout<< " elem " << i+1 << " area= " << total_interm << endl;
		total+=total_interm;
	      }
	    cout << "total area "<<total<<endl;
	  }
	cout<< "Filtering time= " << end_filtering-start_filtering<< endl;
	cout<< "Intersection time= " << end_intersection-start_intersection<< endl;
	long global_end =clock();    
	cout<< "Global time= " << global_end - global_start << endl;
      }
    
    return result_areas;
  }
};
