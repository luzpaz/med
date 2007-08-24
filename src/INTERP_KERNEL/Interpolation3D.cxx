#include "Interpolation3D.hxx"

#include <stack>

#include "MeshElement.hxx"
#include "MeshRegion.hxx"
#include "RegionNode.hxx"
#include "TetraAffineTransform.hxx"
#include "TransformedTriangle.hxx"
#include "VectorUtils.hxx"
#include "Intersector3D.hxx"
#include "Log.hxx"


using namespace INTERP_UTILS;
using namespace MEDMEM;
using namespace MED_EN;

#define USE_RECURSIVE_BBOX_FILTER

#ifndef USE_RECURSIVE_BBOX_FILTER
#include "BBTree.H"
#endif

namespace MEDMEM
{

  /**
   * Default constructor
   * 
   */
  Interpolation3D::Interpolation3D()
  {
    // not implemented
  }
    
  /**
   * Destructor
   *
   */
  Interpolation3D::~Interpolation3D()
  {
    // not implemented
  }

  /**
   * Main interface method of the class, which verifies that the meshes are valid for the calculation,
   * and then returns the matrix of intersection volumes.
   *
   * @param srcMesh     3-dimensional source mesh
   * @param targetMesh  3-dimesional target mesh, containing only tetraedra
   * @returns           vector containing for each element i of the source mesh, a map giving for each element j
   *                    of the target mesh which i intersects, the volume of the intersection
   */
  IntersectionMatrix Interpolation3D::interpol_maillages(const MEDMEM::MESH& srcMesh, const MEDMEM::MESH& targetMesh)
  {
    //} it seems wasteful to make a copy here
    IntersectionMatrix matrix;
	
    // we should maybe do more sanity checking here - eliminating meshes that
    // are too complicated
	
    calculateIntersectionVolumes(srcMesh, targetMesh, matrix);
    return matrix;
  }
    
  /**
   * Performs a depth-first search over srcMesh, using bounding boxes to recursively eliminate the elements of targetMesh
   * which cannot intersect smaller and smaller regions of srcMesh. At each level, each region is divided in two, forming
   * a binary search tree with leaves consisting of only one element of the source mesh together with the elements of the
   * target mesh that can intersect it. The recursion is implemented with a stack of RegionNodes, each one containing a 
   * source region and a target region. Each region has an associated bounding box and a vector of pointers to the elements 
   * that belong to it. Each element contains a bounding box, an element type and an index in the MEDMEM ConnectivityIndex array.
   *
   * @param srcMesh     3-dimensional source mesh
   * @param targetMesh  3-dimesional target mesh, containing only tetraedra
   * @param matrix      vector of maps in which the result is stored 
   *
   */
  void Interpolation3D::calculateIntersectionVolumes(const MEDMEM::MESH& srcMesh, const MEDMEM::MESH& targetMesh, IntersectionMatrix& matrix)
  {
    // calculate descending connectivities
    //      srcMesh.calculateConnectivity(MED_FULL_INTERLACE, MED_DESCENDING, MED_CELL);
    //      targetMesh.calculateConnectivity(MED_FULL_INTERLACE, MED_DESCENDING, MED_CELL);

    // create MeshElement objects corresponding to each element of the two meshes
      
    const int numSrcElems = srcMesh.getNumberOfElements(MED_CELL, MED_ALL_ELEMENTS);
    const int numTargetElems = targetMesh.getNumberOfElements(MED_CELL, MED_ALL_ELEMENTS);

    Intersector3D* intersector = new Intersector3D(srcMesh, targetMesh);

    LOG(2, "Source mesh has " << numSrcElems << " elements and target mesh has " << numTargetElems << " elements ");

    // create empty maps for all source elements
    matrix.resize(numSrcElems);


    MeshElement* srcElems[numSrcElems];
    MeshElement* targetElems[numTargetElems];
      
    std::map<MeshElement*, int> indices;

    for(int i = 0 ; i < numSrcElems ; ++i)
      {
	//const int index = srcMesh.getConnectivityIndex(MED_NODAL, MED_CELL)[i];
	const medGeometryElement type = srcMesh.getElementType(MED_CELL, i + 1);
	srcElems[i] = new MeshElement(i + 1, type, srcMesh);
	  
      }

    for(int i = 0 ; i < numTargetElems ; ++i)
      {
	//	  const int index = targetMesh.getConnectivityIndex(MED_NODAL, MED_CELL)[i];
	const medGeometryElement type = targetMesh.getElementType(MED_CELL, i + 1);
	targetElems[i] = new MeshElement(i + 1, type, targetMesh);
      }

#ifdef USE_RECURSIVE_BBOX_FILTER
      
    // create initial RegionNode and fill up its source region with all the source mesh elements and
    // its target region with all the target mesh elements whose bbox intersects that of the source region

    RegionNode* firstNode = new RegionNode();
      
    MeshRegion& srcRegion = firstNode->getSrcRegion();

    for(int i = 0 ; i < numSrcElems ; ++i)
      {
	srcRegion.addElement(srcElems[i], srcMesh);
      }

    MeshRegion& targetRegion = firstNode->getTargetRegion();

    for(int i = 0 ; i < numTargetElems ; ++i)
      {
	if(!srcRegion.isDisjointWithElementBoundingBox( *(targetElems[i]) ))
	  {
	    targetRegion.addElement(targetElems[i], targetMesh);
	  }
      }

    // using a stack, descend recursively, creating at each step two new RegionNodes having as source region the left and
    // right part of the source region of the current node (determined using MeshRegion::split()) and as target region all the 
    // elements of the target mesh whose bounding box intersects the corresponding part
    // Continue until the source region contains only one element, at which point the intersection volumes are
    // calculated with all the remaining target mesh elements and stored in the matrix if they are non-zero.

    stack<RegionNode*> nodes;
    nodes.push(firstNode);

    while(!nodes.empty())
      {
	RegionNode* currNode = nodes.top();
	nodes.pop();
	LOG(4, "Popping node ");

	if(currNode->getSrcRegion().getNumberOfElements() == 1)
	  {
	    LOG(4, " - One element");

	    // volume calculation
	    MeshElement* srcElement = *(currNode->getSrcRegion().getBeginElements());
	      
	    // NB : srcElement indices are from 0 .. numSrcElements - 1
	    // targetElement indicies from 1 .. numTargetElements
	    // maybe this is not ideal ...
	    const int srcIdx = srcElement->getIndex();
	    std::map< int, double >* volumes = &(matrix[srcIdx - 1]);

	    for(vector<MeshElement*>::const_iterator iter = currNode->getTargetRegion().getBeginElements() ; 
		iter != currNode->getTargetRegion().getEndElements() ; ++iter)
	      {
		const int targetIdx = (*iter)->getIndex();
		const double vol = intersector->intersectCells(srcIdx, targetIdx);
		if(!epsilonEqual(vol, 0.0))
		  {
		    volumes->insert(make_pair(targetIdx, vol));
		    LOG(2, "Result : V (" << srcIdx << ", " << targetIdx << ") = " << matrix[srcIdx - 1][targetIdx]);
		  }
	      }
	  } 
	else // recursion 
	  {

	    LOG(4, " - Recursion");

	    RegionNode* leftNode = new RegionNode();
	    RegionNode* rightNode = new RegionNode();
	      
	    // split current source region
	    //} decide on axis
	    static BoundingBox::BoxCoord axis = BoundingBox::XMAX;
	      
	    currNode->getSrcRegion().split(leftNode->getSrcRegion(), rightNode->getSrcRegion(), axis, srcMesh);

	    LOG(5, "After split, left src region has " << leftNode->getSrcRegion().getNumberOfElements()
		<< " elements and right src region has " << rightNode->getSrcRegion().getNumberOfElements() 
		<< " elements");

	    // ugly hack to avoid problem with enum which does not start at 0
	    // I guess I ought to implement ++ for it instead ...
	    // Anyway, it basically chooses the next axis, circually
	    axis = (axis != BoundingBox::ZMAX) ? static_cast<BoundingBox::BoxCoord>(axis + 1) : BoundingBox::XMAX;

	    // add target elements of curr node that overlap the two new nodes
	    LOG(5, " -- Adding target elements");
	    int numLeftElements = 0;
	    int numRightElements = 0;
	    for(vector<MeshElement*>::const_iterator iter = currNode->getTargetRegion().getBeginElements() ; 
		iter != currNode->getTargetRegion().getEndElements() ; ++iter)
	      {
		LOG(6, " --- New target node");
		  
		if(!leftNode->getSrcRegion().isDisjointWithElementBoundingBox(**iter))
		  {
		    leftNode->getTargetRegion().addElement(*iter, targetMesh);
		    ++numLeftElements;
		  }
		  
		if(!rightNode->getSrcRegion().isDisjointWithElementBoundingBox(**iter))
		  {
		    rightNode->getTargetRegion().addElement(*iter, targetMesh);
		    ++numRightElements;
		  }
		  
	      }

	    LOG(5, "Left target region has " << numLeftElements << " elements and right target region has " 
		<< numRightElements << " elements");

	    if(numLeftElements != 0)
	      {
		nodes.push(leftNode);
	      }
	    else
	      {
		delete leftNode;
	      }

	    if(numRightElements != 0)
	      {
		nodes.push(rightNode);
	      }
	    else
	      {
		delete rightNode;
	      }
	  }
	      
	delete currNode;
	LOG(4, "Next iteration. Nodes left : " << nodes.size());
      }

      

#else // use BBTree
      
      // create BBTree structure
      // - get bounding boxes
    double bboxes[6 * numSrcElems];
    int srcElemIdx[numSrcElems];
    for(int i = 0; i < numSrcElems ; ++i)
      {
	// get source bboxes in right order
	const BoundingBox* box = srcElems[i]->getBoundingBox();
	bboxes[6*i+0] = box->getCoordinate(BoundingBox::XMIN);
	bboxes[6*i+1] = box->getCoordinate(BoundingBox::XMAX);
	bboxes[6*i+2] = box->getCoordinate(BoundingBox::YMIN);
	bboxes[6*i+3] = box->getCoordinate(BoundingBox::YMAX);
	bboxes[6*i+4] = box->getCoordinate(BoundingBox::ZMIN);
	bboxes[6*i+5] = box->getCoordinate(BoundingBox::ZMAX);
	srcElemIdx[i] = srcElems[i]->getIndex() - 1;
      }
      
    BBTree<3> tree(bboxes, srcElemIdx, 0, numSrcElems);

    // for each target element, get source elements with which to calculate intersection
    // - calculate intersection by calling intersectCells
    for(int i = 0; i < numTargetElems; ++i)
      {
	const BoundingBox* box = targetElems[i]->getBoundingBox();
	const int targetIdx = targetElems[i]->getIndex();

	// get target bbox in right order
	double targetBox[6];
	targetBox[0] = box->getCoordinate(BoundingBox::XMIN);
	targetBox[1] = box->getCoordinate(BoundingBox::XMAX);
	targetBox[2] = box->getCoordinate(BoundingBox::YMIN);
	targetBox[3] = box->getCoordinate(BoundingBox::YMAX);
	targetBox[4] = box->getCoordinate(BoundingBox::ZMIN);
	targetBox[5] = box->getCoordinate(BoundingBox::ZMAX);

	vector<int> intersectElems;

	tree.getIntersectingElems(targetBox, intersectElems);

	for(vector<int>::const_iterator iter = intersectElems.begin() ; iter != intersectElems.end() ; ++iter)
	  {
	    const int srcIdx = *iter + 1;
	    const double vol = intersector->intersectCells(srcIdx, targetIdx);

	    if(!epsilonEqual(vol, 0.0))
	      {
		matrix[srcIdx - 1].insert(make_pair(targetIdx, vol));
	      }
	  }
      }
    
#endif

    // free allocated memory
    for(int i = 0 ; i < numSrcElems ; ++i)
      {
	delete srcElems[i];
      }
    for(int i = 0 ; i < numTargetElems ; ++i)
      {
	delete targetElems[i];
      }

    delete intersector;

  }

  /**
   * Calculates volume of intersection between an element of the source mesh and an element of the target mesh.
   * The calculation passes by the following steps : 
   * a) test if srcElement is in the interior of targetElement -> if yes, return volume of srcElement
   *    --> test by call to Element::isElementIncludedIn
   * b) test if targetElement is in the interior of srcElement -> if yes return volume of targetElement
   *    --> test by call to Element::isElementIncludedIn
   * c) test if srcElement and targetElement are disjoint -> if yes return 0.0 [this test is only possible if srcElement is convex]
   *    --> test by call to Element::isElementTriviallyDisjointWith
   * d) (1) find transformation M that takes the target element (a tetraeder) to the unit tetraeder
   *    --> call calculateTransform()
   *    (2) divide srcElement in triangles
   *    --> call triangulateElement()
   *    (3) for each triangle in element, 
   *        (i) find polygones --> calculateIntersectionPolygones()
   *        (ii) calculate volume under polygones --> calculateVolumeUnderPolygone()
   *        (iii) add volume to sum
   *    (4) return det(M)*sumVolume (det(M) = 6*vol(targetElement))
   *
   * @param      srcElement
   * @param      targetElement 
   * @returns    volume of intersection between srcElement and targetElement
   *
   */
#if 0
  double Interpolation3D::calculateIntersectionVolume(const MeshElement& srcElement, const MeshElement& targetElement)
  {

    //std::cout << "Intersecting elems " << srcElement.getIndex() << " and " << targetElement.getIndex() << std::endl;
    // (a), (b) and (c) not yet implemented

    // (d) : without fine-level filtering (a) - (c) for the time being
    // std::cout << "------------------" << std::endl;
    std::cout << "Source : ";
    srcElement.dumpCoords();
    std::cout << "Target : ";
    targetElement.dumpCoords();

    // get array of points of target tetraeder
    const double* tetraCorners[4];
    for(int i = 0 ; i < 4 ; ++i)
      {
	tetraCorners[i] = targetElement.getCoordsOfNode(i + 1);
      }

    // create AffineTransform
    TetraAffineTransform T( tetraCorners );
	
    // check if we have planar tetra element
    if(epsilonEqual(T.determinant(), 0.0, 1.0e-16))
      {
	// tetra is planar
	// std::cout << "Planar tetra -- volume 0" << std::endl;
	return 0.0;
      }

    // std::cout << "Transform : " << std::endl;
    // T.dump();
    // std::cout << std::endl;

    // triangulate source element faces (assumed tetraeder for the time being)
    // do nothing
    vector<TransformedTriangle> triangles;
    srcElement.triangulate(triangles, T);
	
    double volume = 0.0;

    // std::cout << "num triangles = " << triangles.size() << std::endl;
    int i = 0;
    for(vector<TransformedTriangle>::iterator iter = triangles.begin() ; iter != triangles.end(); ++iter)
      {
	std::cout << std::endl << "= > Triangle " << ++i << std::endl;  
	iter->dumpCoords();
	volume += iter->calculateIntersectionVolume();
      }

    std::cout << "Volume = " << volume << ", det= " << T.determinant() << std::endl;

    //? trying without abs to see if the sign of the determinant will always cancel that of the volume
    //? but maybe we should take abs( det ( T ) ) or abs ( 1 / det * vol )

    //? fault in article, Grandy, [8] : it is the determinant of the inverse transformation that 
    // should be used

    return std::abs(1.0 / T.determinant() * volume) ;
	
  }
#endif
}
