#include "Intersector3D.hxx"


#include "Log.hxx"

namespace MEDMEM
{

  Intersector3D::Intersector3D(const MEDMEM::MESH& srcMesh, const MEDMEM::MESH& targetMesh)
    : _srcMesh(srcMesh), _targetMesh(targetMesh)
  {
  }

  Intersector3D::~Intersector3D()
  {
  }

  double Intersector3D::intersectCells(int srcCell, int targetCell)
  {
    
    const medGeometryElement srcType = _srcMesh.getElementType(MED_CELL, srcCell);
    //    const medGeometryElement targetType = _targetMesh.getElementType(MED_CELL, targetCell);

    // get array of points of target tetraeder
    const double* tetraCorners[4];
    for(int i = 0 ; i < 4 ; ++i)
      {
	tetraCorners[i] = getCoordsOfNode(i + 1, targetCell, _targetMesh);
      }
    
    // create AffineTransform
    TetraAffineTransform T( tetraCorners );

    // check if we have planar tetra element
    if(T.determinant() == 0.0)
      {
	// tetra is planar
	LOG(2, "Planar tetra -- volume 0");
	return 0.0;
      }
    // triangulate source element faces (assumed tetraeder for the time being)
    // do nothing
    vector<TransformedTriangle> triangles;
    triangulate(srcType, srcCell, _srcMesh, triangles, T);
	
    double volume = 0.0;

    LOG(4, "num triangles = " << triangles.size());
    int i = 0;
    for(vector<TransformedTriangle>::iterator iter = triangles.begin() ; iter != triangles.end(); ++iter)
      {
	LOG(2, "= > Triangle " << ++i);
#ifdef LOG_ACTIVE
	if(LOG_LEVEL >= 2) 
	  {
	    iter->dumpCoords();
	  }
#endif
	volume += iter->calculateIntersectionVolume();
      }

    LOG(2, "Volume = " << volume << ", det= " << T.determinant());

    //? trying without abs to see if the sign of the determinant will always cancel that of the volume
    //? but maybe we should take abs( det ( T ) ) or abs ( 1 / det * vol )

    //? fault in article, Grandy, [8] : it is the determinant of the inverse transformation that 
    // should be used
    return std::abs(1.0 / T.determinant() * volume) ;

  }
  
  /**
   * Triangulate the faces of an element and apply an affine Transform to the triangles
   *
   * @param      triangles  vector in which triangles are stored
   * @param      T          affine transform that is applied to the nodes of the triangles
   */
  void Intersector3D::triangulate(const medGeometryElement type, const int element, const MEDMEM::MESH& mesh, std::vector<TransformedTriangle>& triangles, const TetraAffineTransform& T) const
  {
    // get cell model for the element
    CELLMODEL cellModel(type);

    assert(cellModel.getDimension() == 3);

    // start index in connectivity array for cell
    //    const int cellIdx = _mesh->getConnectivityIndex(MED_NODAL, MED_CELL)[_index] - 1;

    //    assert(cellIdx >= 0);
    //assert(cellIdx < _mesh->getNumberOfElements(MED_CELL, MED_ALL_ELEMENTS));

    // loop over faces
    for(int i = 1 ; i <= cellModel.getNumberOfConstituents(1) ; ++i)
      {
	medGeometryElement faceType = cellModel.getConstituentType(1, i);
	CELLMODEL faceModel(faceType);
	
	assert(faceModel.getDimension() == 2);
	assert(faceModel.getNumberOfNodes() == 3);
	
	double transformedNodes[3 * faceModel.getNumberOfNodes()];
	

	// loop over nodes of face
	for(int j = 1; j <= faceModel.getNumberOfNodes(); ++j)
	  {
	    // offset of node from cellIdx
	    int localNodeNumber = cellModel.getNodeConstituent(1, i, j);

	    assert(localNodeNumber >= 1);
	    assert(localNodeNumber <= cellModel.getNumberOfNodes());

	    const double* node = getCoordsOfNode(localNodeNumber, element, mesh);
	    
	    // transform 
	    //{ not totally efficient since we transform each node once per face
	    T.apply(&transformedNodes[3*(j-1)], node);

	    LOG(4, "Node " << localNodeNumber << " = " << vToStr(node) << " transformed to " 
		 << vToStr(&transformedNodes[3*(j-1)]));

	  }

	// to be removed
	assert(faceType == MED_TRIA3);

	// create transformed triangles from face
	switch(faceType)
	  {
	  case MED_TRIA3:
	    triangles.push_back(TransformedTriangle(&transformedNodes[0], &transformedNodes[3], &transformedNodes[6]));
	    break;

	    // add other cases here to treat hexahedra, pyramides, etc
	    
	  default:
	    std::cout << "+++ Error : Only elements with triangular faces are supported at the moment." << std::endl;
	    ;
	  }
      }
    

  }




};
