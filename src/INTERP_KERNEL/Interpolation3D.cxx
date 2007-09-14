#include "Interpolation3D.hxx"
#include "MeshElement.hxx"
#include "TransformedTriangle.hxx"
//#include "VectorUtils.hxx"
#include "IntersectorTetra.hxx"
#include "IntersectorHexa.hxx"
#include "TargetIntersector.hxx"
#include "Log.hxx"


using namespace INTERP_UTILS;
using namespace MEDMEM;
using namespace MED_EN;

/// If defined, use recursion to traverse the binary search tree, else use the BBTree class
#define USE_RECURSIVE_BBOX_FILTER

#ifdef USE_RECURSIVE_BBOX_FILTER
#include "MeshRegion.hxx"
#include "RegionNode.hxx"
#include <stack>

#else // use BBTree class

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
  }
    
  /**
   * Destructor
   *
   */
  Interpolation3D::~Interpolation3D()
  {
  }

  /**
   * Main interface method of the class, which verifies that the meshes are valid for the calculation,
   * and then returns the matrix of intersection volumes.
   *
   * @param srcMesh     3-dimensional source mesh
   * @param targetMesh  3-dimesional target mesh, containing only tetraedra
   * @return            vector containing for each element i of the source mesh, a map giving for each element j
   *                    of the target mesh which i intersects, the volume of the intersection
   */
  IntersectionMatrix Interpolation3D::interpol_maillages(const MEDMEM::MESH& srcMesh, const MEDMEM::MESH& targetMesh)
  {
    //} it seems wasteful to make a copy here
    IntersectionMatrix matrix;
	
    // we should do more sanity checking here - eliminating meshes that
    // cannot be treated
	
    calculateIntersectionVolumes(srcMesh, targetMesh, matrix);
    return matrix;
  }
    
  /**
   * Calculates the matrix of volumes of intersection between the elements of srcMesh and the elements of targetMesh.
   * The calculation is done in two steps. First a filtering process reduces the number of pairs of elements for which the
   * calculation must be carried out by eliminating pairs that do not intersect based on their bounding boxes. Then, the 
   * volume of intersection is calculated by an object of type Intersector3D for the remaining pairs, and entered into the
   * intersection matrix. 
   * 
   * The matrix is partially sparse : it is a vector of maps of integer - double pairs. 
   * The length of the vector is equal to the number of target elements - for each target element there is a map, regardless
   * of whether the element intersects any source elements or not. But in the maps there are only entries for those source elements
   * which have a non-zero intersection volume with the target element. The vector has indices running from 
   * 0 to (#target elements - 1), meaning that the map for target element i is stored at index i - 1. In the maps, however,
   * the indexing is more natural : the intersection volume of the target element i with source element j is found at matrix[i-1][j].
   * 
   
   * @param srcMesh     3-dimensional source mesh
   * @param targetMesh  3-dimesional target mesh, containing only tetraedra
   * @param matrix      vector of maps in which the result is stored 
   *
   */
  void Interpolation3D::calculateIntersectionVolumes(const MEDMEM::MESH& srcMesh, const MEDMEM::MESH& targetMesh, IntersectionMatrix& matrix)  {

    // create MeshElement objects corresponding to each element of the two meshes
    const int numSrcElems = srcMesh.getNumberOfElements(MED_CELL, MED_ALL_ELEMENTS);
    const int numTargetElems = targetMesh.getNumberOfElements(MED_CELL, MED_ALL_ELEMENTS);

    LOG(2, "Source mesh has " << numSrcElems << " elements and target mesh has " << numTargetElems << " elements ");

    MeshElement* srcElems[numSrcElems];
    MeshElement* targetElems[numTargetElems];
    
    std::map<MeshElement*, int> indices;
    
    for(int i = 0 ; i < numSrcElems ; ++i)
      {
	//const medGeometryElement type = srcMesh.getElementType(MED_CELL, i + 1);
	srcElems[i] = new MeshElement(i + 1, srcMesh);	
      }
    
    for(int i = 0 ; i < numTargetElems ; ++i)
      {
	//const medGeometryElement type = targetMesh.getElementType(MED_CELL, i + 1);
	targetElems[i] = new MeshElement(i + 1, targetMesh);
      }
    
    // create empty maps for all source elements
    matrix.resize(numTargetElems);


#ifdef USE_RECURSIVE_BBOX_FILTER
    
    /*
     * Performs a depth-first search over srcMesh, using bounding boxes to recursively eliminate the elements of targetMesh
     * which cannot intersect smaller and smaller regions of srcMesh. At each level, each region is divided in two, forming
     * a binary search tree with leaves consisting of only one element of the source mesh together with the elements of the
     * target mesh that can intersect it. The recursion is implemented with a stack of RegionNodes, each one containing a 
     * source region and a target region. Each region has an associated bounding box and a vector of pointers to the elements 
     * that belong to it. Each MeshElement contains a bounding box and the global number of the corresponding element in the mesh.
     */

    // create initial RegionNode and fill up its source region with all the source mesh elements and
    // its target region with all the target mesh elements whose bounding box
    // intersects that of the source region

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

    // Using a stack, descend recursively, creating at each step two new RegionNodes having as source region the left and
    // right part of the source region of the current node (created using MeshRegion::split()) and as target region all the 
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

	if(currNode->getTargetRegion().getNumberOfElements() == 1)
	  {
	    // calculate volumes
	    LOG(4, " - One element");

	    MeshElement* targetElement = *(currNode->getTargetRegion().getBeginElements());
	      
	    // NB : srcElement indices are from 0 .. numSrcElements - 1
	    // targetElement indicies from 1 .. numTargetElements
	    // maybe this is not ideal ...
	    const int targetIdx = targetElement->getIndex();


	    TargetIntersector* intersector;

	    switch(targetElement->getType())
	      {
	      case MED_TETRA4:
		intersector = new IntersectorTetra(srcMesh, targetMesh, targetIdx);
		break;

	      case MED_HEXA8:
		intersector = new IntersectorHexa(srcMesh, targetMesh, targetIdx);
		break;
		
	      default:
		assert(false);
	      }



	    for(vector<MeshElement*>::const_iterator iter = currNode->getSrcRegion().getBeginElements() ; 
		iter != currNode->getSrcRegion().getEndElements() ; ++iter)
	      {
	     
		const int srcIdx = (*iter)->getIndex();
		const double vol = intersector->intersectSourceCell(srcIdx);

		if(vol != 0.0)
		  {
		    matrix[targetIdx - 1][srcIdx] = vol;
		    LOG(2, "Result : V (" << srcIdx << ", " << targetIdx << ") = " << matrix[srcIdx - 1][targetIdx]);
		    
		  }
	      }
	    
	    delete intersector;

	  } 
	else // recursion 
	  {

	    LOG(4, " - Recursion");

	    RegionNode* leftNode = new RegionNode();
	    RegionNode* rightNode = new RegionNode();
	      
	    // split current source region
	    //} decide on axis
	    static BoundingBox::BoxCoord axis = BoundingBox::XMAX;
	      
	    currNode->getTargetRegion().split(leftNode->getTargetRegion(), rightNode->getTargetRegion(), axis, targetMesh);

	    LOG(5, "After split, left target region has " << leftNode->getTargetRegion().getNumberOfElements()
		<< " elements and right target region has " << rightNode->getTargetRegion().getNumberOfElements() 
		<< " elements");

	    // ugly hack to avoid problem with enum which does not start at 0
	    // I guess I ought to implement ++ for it instead ...
	    // Anyway, it basically chooses the next axis, cyclically
	    axis = (axis != BoundingBox::ZMAX) ? static_cast<BoundingBox::BoxCoord>(axis + 1) : BoundingBox::XMAX;

	    // add source elements of current node that overlap the target regions of the new nodes
	    LOG(5, " -- Adding source elements");
	    int numLeftElements = 0;
	    int numRightElements = 0;
	    for(vector<MeshElement*>::const_iterator iter = currNode->getSrcRegion().getBeginElements() ; 
		iter != currNode->getSrcRegion().getEndElements() ; ++iter)
	      {
		LOG(6, " --- New target node");
		  
		if(!leftNode->getTargetRegion().isDisjointWithElementBoundingBox(**iter))
		  {
		    leftNode->getSrcRegion().addElement(*iter, srcMesh);
		    ++numLeftElements;
		  }
		  
		if(!rightNode->getTargetRegion().isDisjointWithElementBoundingBox(**iter))
		  {
		    rightNode->getSrcRegion().addElement(*iter, srcMesh);
		    ++numRightElements;
		  }
		  
	      }

	    LOG(5, "Left src region has " << numLeftElements << " elements and right src region has " 
		<< numRightElements << " elements");

	    // push new nodes on stack
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
	      
	// all nodes are deleted here
	delete currNode;

	LOG(4, "Next iteration. Nodes left : " << nodes.size());
      }

      

#else // Use BBTree
      
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

	// source indices have to begin with zero for BBox, I think
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

	// create intersector
	IntersectorTetra intersector(srcMesh, targetMesh, targetIdx);

	for(vector<int>::const_iterator iter = intersectElems.begin() ; iter != intersectElems.end() ; ++iter)
	  {

	    const int srcIdx = *iter + 1;
	    const double vol = intersector.intersectSourceCell(srcIdx);

	    if(vol != 0.0)
	      {
		matrix[targetIdx - 1].insert(make_pair(srcIdx, vol));
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


  }

}
