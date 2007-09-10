#include "IntersectorTetra.hxx"

#include "TetraAffineTransform.hxx"
#include "TransformedTriangle.hxx"
#include "MeshUtils.hxx"
#include "VectorUtils.hxx"
#include "Log.hxx"

#include <cmath>
#include <cassert>
#include <string>
#include <sstream>

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
  IntersectorTetra::IntersectorTetra(const MESH& srcMesh, const MESH& targetMesh, int targetCell)
    : _srcMesh(srcMesh), filtered(0)
  {
    const medGeometryElement targetType = targetMesh.getElementType(MED_CELL, targetCell);
    
    // maybe we should do something more civilized here
    assert(targetType == MED_TETRA4);
    
    // get array of points of target tetraeder
    const double* tetraCorners[4];
    for(int i = 0 ; i < 4 ; ++i)
      {
	tetraCorners[i] = getCoordsOfNode(i + 1, targetCell, targetMesh);
      }
    
    // create AffineTransform from tetrahedron
    _t = new TetraAffineTransform( tetraCorners );

  }

  /*
   * Destructor
   *
   */
  IntersectorTetra::~IntersectorTetra()
  {
    delete _t;

    for(hash_map< int, double*>::iterator iter = _nodes.begin(); iter != _nodes.end() ; ++iter)
      {
	delete[] iter->second;
      }
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
  double IntersectorTetra::intersectSourceCell(int element)
  {
        
    //{ could be done on outside
    // check if we have planar tetra element
    if(_t->determinant() == 0.0)
      {
	// tetra is planar
	LOG(2, "Planar tetra -- volume 0");
	return 0.0;
      }

    // get type of cell
    const medGeometryElement type = _srcMesh.getElementType(MED_CELL, element);
    
    // get cell model for the element
    const CELLMODEL cellModel(type);

    assert(cellModel.getDimension() == 3);
    assert(element >= 1);
    assert(element <= _srcMesh.getNumberOfElements(MED_CELL, MED_ALL_ELEMENTS));

    // halfspace filtering
    bool isOutside[8] = {true, true, true, true, true, true, true, true};
    bool isTargetOutside = false;

    // calculate the coordinates of the nodes
    for(int i = 1; i <= cellModel.getNumberOfNodes() ; ++i)
      {
	// we could store mapping local -> global numbers too, but not sure it is worth it
	const int globalNodeNum = getGlobalNumberOfNode(i, element, _srcMesh);
	if(_nodes.find(globalNodeNum) == _nodes.end())
	  {
	    calculateNode(globalNodeNum);
	  }

	checkIsOutside(_nodes[globalNodeNum], isOutside);
	
	// local caching of globalNodeNum 
	// not sure this is efficient
	//	globalNodeNumbers[i] = globalNodeNum;
      }

    // halfspace filtering check
    // NB : might not be beneficial for caching of triangles
    for(int i = 0; i < 8; ++i)
      {
	if(isOutside[i])
	  {
	    isTargetOutside = true;
	  }
      }

    double totalVolume = 0.0;
    
    if(!isTargetOutside)
      {
	for(int i = 1 ; i <= cellModel.getNumberOfConstituents(1) ; ++i)
	  {
	    const medGeometryElement faceType = cellModel.getConstituentType(1, i);
	    const CELLMODEL faceModel(faceType);
	
	    assert(faceModel.getDimension() == 2);

	    int faceNodes[faceModel.getNumberOfNodes()];

	    // get the nodes of the face
	    for(int j = 1; j <= faceModel.getNumberOfNodes(); ++j)
	      {
		const int locNodeNum = cellModel.getNodeConstituent(1, i, j);
		assert(locNodeNum >= 1);
		assert(locNodeNum <= cellModel.getNumberOfNodes());

		faceNodes[j-1] = getGlobalNumberOfNode(locNodeNum, element, _srcMesh);//globalNodeNumbers[locNodeNum];		
	      }

	    switch(faceType)
	      {
	      case MED_TRIA3:
		{
		  // create the face key
		  TriangleFaceKey key = TriangleFaceKey(faceNodes[0], faceNodes[1], faceNodes[2]);
	      
		  // calculate the triangle if needed
		  if(_volumes.find(key) == _volumes.end())
		    {
		      TransformedTriangle tri(_nodes[faceNodes[0]], _nodes[faceNodes[1]], _nodes[faceNodes[2]]);
		      calculateVolume(tri, key);
		      totalVolume += _volumes[key];
		    } else {    
		      // count negative as face has reversed orientation
		      totalVolume -= _volumes[key];
		    }
		}
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
		{
		  // calculate the triangles if needed

		  // local nodes 1, 2, 3
		  TriangleFaceKey key1 = TriangleFaceKey(faceNodes[0], faceNodes[1], faceNodes[2]);
		  if(_volumes.find(key1) == _volumes.end())
		    {
		      TransformedTriangle tri(_nodes[faceNodes[0]], _nodes[faceNodes[1]], _nodes[faceNodes[2]]);
		      calculateVolume(tri, key1);
		      totalVolume += _volumes[key1];
		    } else {
		      // count negative as face has reversed orientation
		      totalVolume -= _volumes[key1];
		    }

		  // local nodes 1, 3, 4
		  TriangleFaceKey key2 = TriangleFaceKey(faceNodes[0], faceNodes[2], faceNodes[3]);
		  if(_volumes.find(key2) == _volumes.end())
		    {
		      TransformedTriangle tri(_nodes[faceNodes[0]], _nodes[faceNodes[2]], _nodes[faceNodes[3]]);
		      calculateVolume(tri, key2);
		      totalVolume += _volumes[key2];
		    }
		  else
		    { 
		      // count negative as face has reversed orientation
		      totalVolume -= _volumes[key2];
		    }
		}
		break;

	      default:
		std::cout << "+++ Error : Only elements with triangular and quadratilateral faces are supported at the moment." << std::endl;
		assert(false);
	      }
	  }
      }
    else
      {
	++filtered;
      }

    // reset if it is very small to keep the matrix sparse
    // is this a good idea?
    if(epsilonEqual(totalVolume, 0.0, 1.0e-14))
      {
    	totalVolume = 0.0;
      }
    
    LOG(2, "Volume = " << totalVolume << ", det= " << _t->determinant());

    // NB : fault in article, Grandy, [8] : it is the determinant of the inverse transformation 
    // that should be used (which is equivalent to dividing by the determinant)
    return std::abs(1.0 / _t->determinant() * totalVolume) ;
  }

};
