#ifndef __MESH_ELEMENT_HXX__
#define __MESH_ELEMENT_HXX__

namespace INTERP_UTILS
{

  /**
   * Class representing a single element of a mesh together with its bounding box.
   * It permits access to the nodes of the element, to test for disunion and inclusion with other elements
   * and to triangulate its faces.
   */
  class MeshElement
  {
  public:
    
    /**
     * Constructor
     *
     * @param mesh    mesh that the element belongs to
     * @param type    geometric type of the element
     * @param index   connectivity index of element in the mesh
     */
    MeshElement(const int index, const MEDEN::medGeometryElement type, const MEDMEM::Mesh& mesh);
    
    /**
     * Destructor
     *
     */
    ~MeshElement();

    /**
     * Determines if this element is in the interior of another element 
     * by calculating the triple products for each point of this element with respect
     * to all the faces of the other object (faces must be triangulated ... ) If all triple
     * products have the same sign, then the element is in the interior of the other element
     *
     * @param otherElement the supposedly enclosing element
     * @returns true if this element is enclosed in the other element, false if not
     */
    bool isElementIncludedIn(const MeshElement& otherElement) const;

    /**
     * Determines whether the intersection of this element is trivially empty. This is done by checking for each
     * face of one element if it is such that all the vertices of the other element is on the same side of this face.
     * If there is such a face, then the intersection is trivially empty. If there is no such face, we do not know if 
     * the intersection is empty.
     *
     * @pre The elements are convex. If this is no true, we return false.
     * @param otherElement the element believed to be disjoint with this one
     * @returns true if the two elements are convex and there exists a face of this element such as described 
     *          above, false otherwise
     */
    bool isElementTriviallyDisjointWith(const MeshElement& otherElement) const;

    /**
     * Returns the number of nodes of this element
     *
     * @returns  the number of nodes of this element
     */
    int getNumberNodes() const;

    /**
     * Returns the coordinates of a node of this element
     * (1 <= node <= #nodes)
     *
     * @param      node  the node for which the coordinates are sought
     * @returns    pointer to an array of 3 doubles containing the coordinates
     */
    const double* getCoordsOfNode(int node);

  private:
    const int _index;
    const MEDMEM::Mesh* _mesh;
    const MEDEN::medGeometryElement _type;
    BoundingBox _box; // should not change after initialisation
    
  };

  // to be added : triangulation methods

};


#endif
