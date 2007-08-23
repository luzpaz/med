#ifndef __INTERPOLATION_3D_HXX__
#define __INTERPOLATION_3D_HXX__

// MEDMEM includes
#include "Interpolation.hxx"
#include "MEDMEM_Mesh.hxx"

// standard includes
#include <vector>
#include <map>

// NOTE : Method comments should be moved to source file during the implementation phase

// typedefs
typedef std::vector< std::map< int, double > > IntersectionMatrix;

namespace INTERP_UTILS
{
  class MeshElement;
  class MeshRegion;
};

class Interpolation3DTest;

namespace MEDMEM 
{
  /**
   * Class used to calculate the volumes of intersection between the elements of two 3D meshes.
   * 
   */
  class Interpolation3D : public Interpolation
  {

    friend class ::Interpolation3DTest;


  public :

    /**
     * Default constructor
     * 
     */
    Interpolation3D();

    
    /**
     * Destructor
     *
     */
    virtual ~Interpolation3D();

    /**
     * Main interface method of the class, which verifies that the meshes are valid for the calculation,
     * and then returns the matrix of intersection volumes.
     *
     * @param srcMesh     3-dimensional source mesh
     * @param targetMesh  3-dimesional target mesh, containing only tetraedra
     * @returns           vector containing for each element i of the source mesh, a map giving for each element j
     *                    of the target mesh which i intersects, the volume of the intersection
     */
    virtual IntersectionMatrix interpol_maillages(const MEDMEM::MESH& srcMesh, const MEDMEM::MESH& targetMesh);

  private:
    
    /**
     * Performs a depth-first search over srcMesh, using bounding boxes to recursively eliminate the elements of targetMesh
     * which cannot intersect smaller and smaller regions of srcMesh. At each level, each region is divided in two, forming
     * a binary search tree with leaves consisting of only one element of the source mesh together with the elements of the
     * target mesh that can intersect it. The recursion is implemented with a stack of SearchNodes, each one containing a 
     * source region and a target region. Each region has an associated bounding box and a vector of pointers to the elements 
     * that belong to it. Each element contains a bounding box, an element type and an index in the MEDMEM ConnectivityIndex array.
     *
     * @param srcMesh     3-dimensional source mesh
     * @param targetMesh  3-dimesional target mesh, containing only tetraedra
     * @param matrix      vector of maps in which the result is stored 
     *
     */
    void calculateIntersectionVolumes(const MEDMEM::MESH& srcMesh, const MEDMEM::MESH& targetMesh, IntersectionMatrix& matrix);

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
    //    double calculateIntersectionVolume(const INTERP_UTILS::MeshElement& srcElement, const INTERP_UTILS::MeshElement& targetElement);

  };

};

#endif
