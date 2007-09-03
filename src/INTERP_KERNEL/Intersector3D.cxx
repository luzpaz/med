#include "Intersector3D.hxx"

#include "TetraAffineTransform.hxx"
#include "TransformedTriangle.hxx"
#include "MeshUtils.hxx"
#include "VectorUtils.hxx"
#include "Log.hxx"

#include <cmath>
#include <cassert>

using namespace MEDMEM;
using namespace MED_EN;
using namespace INTERP_UTILS;

namespace MEDMEM
{
  /*
   * Constructor
   * 
   * @param srcMesh     mesh containing the source elements
   * @param targetMesh  mesh containing the target elements
   *
   */
  Intersector3D::Intersector3D(const MESH& srcMesh, const MESH& targetMesh)
    : _srcMesh(srcMesh), _targetMesh(targetMesh), filtered(0)
  {
  }

  /*
   * Destructor
   *
   */
  Intersector3D::~Intersector3D()
  {
  }

  /*
   * Calculates the volume of intersection of an element in the source mesh and an element 
   * in the target mesh. The method is based on the algorithm of Grandy. It first calculates the transformation
   * that takes the target tetrahedron into the unit tetrahedron. After that, the 
   * faces of the source element are triangulated and the calculated transformation is applied 
   * to each triangle. The algorithm of Grandy, implemented in INTERP_UTILS::TransformedTriangle is used
   * to calculate the contribution to the volume from each triangle. The volume returned is the sum of these contributions
   * divided by the determinant of the transformation.
   *
   * @pre The element in _targetMesh referenced by targetCell is of type MED_TETRA4.
   * @param srcCell      global number of the source element (1 <= srcCell < # source cells)
   * @param targetCell   global number of the target element (1 <= targetCell < # target cells) - this element must be a tetrahedron
   */
  double Intersector3D::intersectCells(int srcCell, int targetCell)
  {
    // get type of cell
    const medGeometryElement srcType = _srcMesh.getElementType(MED_CELL, srcCell);
    const medGeometryElement targetType = _targetMesh.getElementType(MED_CELL, targetCell);

    // maybe we should do something more civilized here
    assert(targetType == MED_TETRA4);

    // get array of points of target tetraeder
    const double* tetraCorners[4];
    for(int i = 0 ; i < 4 ; ++i)
      {
	tetraCorners[i] = getCoordsOfNode(i + 1, targetCell, _targetMesh);
      }
    
    // create AffineTransform from tetrahedron
    TetraAffineTransform T( tetraCorners );

    // check if we have planar tetra element
    if(T.determinant() == 0.0)
      {
	// tetra is planar
	LOG(2, "Planar tetra -- volume 0");
	return 0.0;
      }

    // triangulate source element faces
    vector<TransformedTriangle> triangles;
    triangulate(srcType, srcCell, triangles, T);
	
    double volume = 0.0;

    LOG(4, "num triangles = " << triangles.size());
    int i = 0;
    
    // add up volumes
    // ? potential stability issue - maybe volumes should be added in increasing order of magnitude?
    for(vector<TransformedTriangle>::iterator iter = triangles.begin() ; iter != triangles.end(); ++iter)
      {
	LOG(2, "= > Triangle " << ++i);

#if LOG_LEVEL >= 2
	iter->dumpCoords();
#endif

	volume += iter->calculateIntersectionVolume();
      }

    // reset if it is very small to keep the matrix sparse
    // is this a good idea?
    if(epsilonEqual(volume, 0.0, 1.0e-11))
      {
    	volume = 0.0;
      }
    
    LOG(2, "Volume = " << volume << ", det= " << T.determinant());

    // NB : fault in article, Grandy, [8] : it is the determinant of the inverse transformation 
    // that should be used (which is equivalent to dividing by the determinant)
    return std::abs(1.0 / T.determinant() * volume) ;
  }
  
  /**
   * Triangulate the faces of a source element and apply an affine Transform to the triangles
   *
   * @param  type       type of the element
   * @param  element    global number of the element
   * @param  triangles  vector in which the transformed triangles are stored
   * @param  T          affine transform that is applied to the nodes of the triangles
   */
#ifdef OPTIMIZE_FILTER // optimized version
  void Intersector3D::triangulate(const medGeometryElement type, const int element, std::vector<TransformedTriangle>& triangles, const TetraAffineTransform& T) const
  {

    // get cell model for the element
    CELLMODEL cellModel(type);

    assert(cellModel.getDimension() == 3);
    assert(element >= 1);
    assert(element <= _srcMesh.getNumberOfElements(MED_CELL, MED_ALL_ELEMENTS));

    // try to avoid reallocations with push_back()
    // 2 * the number of faces should be plenty
    triangles.reserve(2 * cellModel.getNumberOfConstituents(1));

    // loop over faces
    double** transformedNodes = new double*[cellModel.getNumberOfNodes()];
    bool isOutside[8] = {true, true, true, true, true, true, true, true};
    bool isTargetOutside = false;
    
    // calculate the coordinates of the nodes
    for(int i = 1; i <= cellModel.getNumberOfNodes() ; ++i)
      {
	const double* node = getCoordsOfNode(i, element, _srcMesh);
	transformedNodes[i-1] = new double[3];
	assert(transformedNodes[i-1] != 0);
	
	T.apply(transformedNodes[i - 1], node);

	//	LOG(4, "Node " << i << " = " << vToStr(node) << " transformed to " << vToStr(&transformedNodes[i - 1]);

	checkIsOutside(transformedNodes[i - 1], isOutside);
      }
    // std:cout << "here1" << std::endl;
    // check if we need to calculate intersection volume
    for(int i = 0; i < 8; ++i)
      {
	if(isOutside[i])
	  {
	    isTargetOutside = true;
	  }
      }
    // std:cout << "here2" << std::endl;

    if(!isTargetOutside)
      {
	for(int i = 1 ; i <= cellModel.getNumberOfConstituents(1) ; ++i)
	  {
	    const medGeometryElement faceType = cellModel.getConstituentType(1, i);
	    CELLMODEL faceModel(faceType);
	
	    assert(faceModel.getDimension() == 2);
	    
	    int nodes[faceModel.getNumberOfNodes()];
	
	    // get the nodes of the face
	    for(int j = 1; j <= faceModel.getNumberOfNodes(); ++j)
	      {
		nodes[j-1] = cellModel.getNodeConstituent(1, i, j) - 1;
		
		assert(nodes[j-1] +1 >= 0);
		assert(nodes[j-1] +1 <= cellModel.getNumberOfNodes());
	      }
	    //	     std::cout << "here3 : " << nodes[0] << "," <<  nodes[1] << ", " <<  nodes[2] << std::endl;
	    // create transformed triangles from face
	    switch(faceType)
	      {
	      case MED_TRIA3:
		// simple take the triangle as it is
		//std::cout << "here3.1 : " << transformedNodes[nodes[0]] << "," <<  transformedNodes[nodes[1]] << ", " << transformedNodes[nodes[2]]  << std::endl;
		triangles.push_back(TransformedTriangle(transformedNodes[nodes[0]], transformedNodes[nodes[1]], transformedNodes[nodes[2]]));
		break;
		
	      case MED_QUAD4:
		// simple triangulation of faces along a diagonal :
		// 
		// 2 ------ 3
		// |      / |
		// |     /  |
		// |    /   |
		// |   /    |
		// |  /     |
		// | /      |
		// 1 ------ 4
		//
		//? not sure if this always works 
		
		// local nodes 1, 2, 3
		triangles.push_back(TransformedTriangle(transformedNodes[nodes[0]], transformedNodes[nodes[1]], transformedNodes[nodes[2]]));
		
		// local nodes 1, 3, 4
		triangles.push_back(TransformedTriangle(transformedNodes[nodes[0]], transformedNodes[nodes[2]], transformedNodes[nodes[3]]));
		
		break;
		
	      default:
		std::cout << "+++ Error : Only elements with triangular and quadratilateral faces are supported at the moment." << std::endl;
		assert(false);
	      }
	    // std:cout << "here4" << std::endl;
	  }
      }
    else
      {
	++filtered;
      }

    for(int i = 0 ; i < cellModel.getNumberOfNodes() ; ++i)
      {
	delete[] transformedNodes[i];
      }
    delete[] transformedNodes;
    // std:cout << "here5" << std::endl;
  }
    
#else // un-optimized version
  
  void Intersector3D::triangulate(const medGeometryElement type, const int element, std::vector<TransformedTriangle>& triangles, const TetraAffineTransform& T) const
  {
    // get cell model for the element
    CELLMODEL cellModel(type);

    assert(element >= 1);
    assert(element <= _srcMesh.getNumberOfElements(MED_CELL, MED_ALL_ELEMENTS));
    assert(cellModel.getDimension() == 3);

    // loop over faces
    for(int i = 1 ; i <= cellModel.getNumberOfConstituents(1) ; ++i)
      {
	medGeometryElement faceType = cellModel.getConstituentType(1, i);
	CELLMODEL faceModel(faceType);
	
	assert(faceModel.getDimension() == 2);
	
	double transformedNodes[3 * faceModel.getNumberOfNodes()];

	// loop over nodes of face
	for(int j = 1; j <= faceModel.getNumberOfNodes(); ++j)
	  {
	    // offset of node from cellIdx
	    int localNodeNumber = cellModel.getNodeConstituent(1, i, j);

	    assert(localNodeNumber >= 1);
	    assert(localNodeNumber <= cellModel.getNumberOfNodes());

	    const double* node = getCoordsOfNode(localNodeNumber, element, _srcMesh);
	    
	    // transform 
	    //{ not totally efficient since we transform each node once per face
	    T.apply(&transformedNodes[3*(j-1)], node);

	    LOG(4, "Node " << localNodeNumber << " = " << vToStr(node) << " transformed to " 
		<< vToStr(&transformedNodes[3*(j-1)]));

	  }

	// create transformed triangles from face
	switch(faceType)
	  {
	  case MED_TRIA3:
	    triangles.push_back(TransformedTriangle(&transformedNodes[0], &transformedNodes[3], &transformedNodes[6]));
	    break;

	    // add other cases here to treat hexahedra, pyramides, etc
	  case MED_QUAD4:
	    // simple triangulation of faces along a diagonal :
		// 
		// 2 ------ 3
		// |      / |
		// |     /  |
		// |    /   |
		// |   /    |
		// |  /     |
		// | /      |
		// 1 ------ 4
		//
		//? not sure if this always works 
		
		// local nodes 1, 2, 3
		triangles.push_back(TransformedTriangle(&transformedNodes[0], &transformedNodes[3], &transformedNodes[6]));
		
		// local nodes 1, 3, 4
		triangles.push_back(TransformedTriangle(&transformedNodes[0], &transformedNodes[6], &transformedNodes[9]));
		
		break;
	  default:
	    std::cout << "+++ Error : Only elements with triangular faces are supported at the moment." << std::endl;
	    ;
	  }
      }
  }
    
#endif 


 
};
