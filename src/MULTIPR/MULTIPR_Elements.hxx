//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// Partitioning/decimation module for the SALOME v3.2 platform
//
/**
 * \file    MULTIPR_Elements.hxx
 *
 * \brief   Class Elements = table of elements in a mesh.
 *          All elements share the same type (e.g. MED_MAILLE) and the same geometric description (e.g. TETRA10).
 *          Each element has a family and a table of connectivity.
 *          Optional: each element has a name and an id.
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

#ifndef MULTIPR_ELEMENTS_HXX
#define MULTIPR_ELEMENTS_HXX

//*****************************************************************************
// Includes section
//*****************************************************************************

extern "C"
{
    #include "med.h"
}

#include <iostream>
#include <set>
#include <vector>


namespace multipr
{

//*****************************************************************************
// Predeclaration
//*****************************************************************************

class Nodes;


//*****************************************************************************
// Class Elements
//*****************************************************************************

class Elements
{
public:

    /** 
     * Builds an empty set of elements (default constructor).
     */
    Elements();

    /** 
     * Builds an empty set of elements with of the defined geometry type.
     * \param pGeomType The geometry type.
     */
    Elements(med_geometrie_element pGeomType);

        
    /**
     * Destructor. Removes everything.
     */
    ~Elements();
    
    /**
     * Resets this object in its state by default (empty). Cleans memory.
     */
    void reset();
    
    //---------------------------------------------------------------------
    // Basic accessors/mutators
    //---------------------------------------------------------------------
    
    /**
     * Returns true iff this object contains no elements (table of elements is empty).
     * \return true iff this object contains no elements.
     */
    bool isEmpty() const { return (mNum == 0); }
    
    /**
     * Returns true iff elements have a name.
     * \return true iff elements have a name.
     */
    bool isNames() const { return (mNames != NULL); }
    
    /**
     * Returns true iff elements have an identifier (= a number).
     * \return true iff elements have an identifier (= a number).
     */
    bool isIdentifiers() const { return (mId != NULL); }    
    
    /**
     * Returns the number of elements.
     * \return the number of elements.
     */
    int getNumberOfElements() const { return mNum; }
    
    /**
     * Returns the type of primitives (e.g. MED_TETRA10).
     * \return the type of primitives
     */
    med_geometrie_element getTypeOfPrimitives() const { return mGeom; }
    
    /**
     * Returns the number of nodes that composed an element.
     * \return the number of nodes that composed an element.
     */
     int getNumberOfNodesByElement() const { return mNumNodesByElt; }
     
    /**
     * Returns the family associated with an element.
     * \param  pIndex index of any element in [0..NUMBER_OF_ELEMENTS-1].
     * \return the family associated with an element.
     * \throw  IndexOutOfBoundsException if pIndex is invalid.
     */
    med_int getFamilyIdentifier(med_int pIndex) const;
    
    /**
     * Returns the connectivity of one element.
     * \param  pIndex must be in [0..NUMBER_OF_ELEMENTS-1].
     * \return a pointer towards the table of connectivity of the element.
     * \throw  IndexOutOfBoundsException if pIndex is invalid.
     */
    const med_int* getConnectivity(int pIndex) const;
    
    /**
     * Returns the coordinates of all the nodes of one element.
     * \param  pIndexElt must be in [0..NUMBER_OF_ELEMENTS-1].
     * \param  pNodes    table of nodes, used to retrieve coordinates.
     * \param  pCoo      (out) table of coordinates of nodes (e.g. 30 = 3 * 10 med_float for a TETRA10).
     * \param  pFirst    only get the pFirst coordinates.
     * \throw  IndexOutOfBoundsException if pIndex is invalid.
     */
    void getCoordinates(med_int pIndexElt, const Nodes* pNodes, med_float* pCoo, int pFirst = 100) const;
    
    //---------------------------------------------------------------------
    // Algorithms
    //---------------------------------------------------------------------
    
    /**
     * Constructor. Creates a subset of this set of elements from a set of indices.
     * \param  pSetIndices set of indices (start at 1).
     * \return a restriction of this set of elements.
     */
    Elements*   extractSubSet(const std::set<med_int>& pSetIndices) const;
    
    /**
     * Create a subset of elements containing the given nodes. 
     * \param pSetOfNodes The set of nodes.
     * \param pSubSetOfElements The subset of elements to fill.
     * \throw IllegalStateException if pSetOfNodes and pSubSetOfElements are not different.
     */
    void        extractSubSetFromNodes(const std::set<med_int>& pSetOfNodes, std::set<med_int>& pSubSetOfElements) const;
    
    /**
     * Returns the set of indices (starting at 1) of all nodes used by this set of elements.
     * \return the set of indices of all nodes used by this set of elements.
     */
    const std::set<med_int>& getSetOfNodes();
    
    /**
     * Returns the set of families referenced by this set of elements.
     * Each family is described by its identifier.
     * \return Returns the set of families referenced by this set of elements.
     */
    std::set<med_int> getSetOfFamilies() const;
    
    /**
     * Remaps this set of elements such that indices of nodes are continous 1 2 3 4 5 ... *
     * This method is intended to be used after extractSubSet().
     */
    void remap(void);
    
        
    /**
     * Remaps this set of elements such that indices of nodes are continous 1 2 3 4 5 ... *
     * This method is intended to be used after extractSubSet().
         * This remap method allows to use the same set of nodes with several mesh type.
         * \param pSetOfNodes The set of nodes index extracted from the original med file.
     */
    void remap(std::set<med_int>& pSetOfNodes);
        
        
    /**
     * Builds a new set of elements by merging two set of elements (this and pElements) if possible.
     * Merge is partial because Id and Names are not take into account.
     * \param  pElements any set of nodes to be merged with this.
     * \param  pOffset
     * \return a new set of nodes.
     * \throw  IllegalStateException if the two sets of elements are incompatible.
     */
    Elements* mergePartial(Elements* pElements, int pOffset);
    Elements* mergePartial(std::vector<Elements*> pElements, std::vector<int>& pOffsets);
    
    //---------------------------------------------------------------------
    // I/O
    //---------------------------------------------------------------------
    
    /**
     * Reads a set of elements from a MED file.
     * You must give: name of mesh, dimension of the mesh, type of entity and type of elements.
     * \param  pMEDfile   any valid MED file opened for reading.
     * \param  pMeshName  name of the mesh.
     * \param  pMeshDim   dimension of the mesh.
     * \param  pEntity    entity to be read (e.g. MED_MAILLE).
     * \param  pGeom      type of primitves to be read (e.g. MED_TETRA10).
     * \throw  IOException if any i/o error occurs.
     */
    void readMED(
        med_idt               pMEDfile, 
        char*                 pMeshName, 
        med_int               pMeshDim,
        med_entite_maillage   pEntity, 
        med_geometrie_element pGeom);
    
    /**
     * Writes this set of elements to a MED file.
     * WARNING: mesh must have been created and added to the MED file before.
     * \param  pMEDfile  any valid MED file opened for writing.
     * \param  pMeshName name of the mesh.
     * \param  pMeshDim  dimension of the mesh.
     * \throw  IOException if any i/o error occurs.
     */
    void writeMED(med_idt pMEDfile, char* pMeshName, med_int pMeshDim);
    
    /**
     * Sets the flag which control the stream operator <<.
     * \param  pFlag new flag value.
     */
    void setPrintAll(bool pFlag) { mFlagPrintAll = pFlag; } 
    
    /**
     * Dumps any Elements to the given output stream.
     * \param  pOs any output stream.
     * \param  pE  any Elements.
     * \return the output stream pOs.
     */
    friend std::ostream& operator<<(std::ostream& pOs, Elements& pE);

private:
    
    /**
     * Builds the set of nodes used by this set of elements.
     * If the set already exist, then it is cleared and a new set is computed.
     */
    void buildSetOfNodes();
    
private:
    
    med_int               mNum;           /**< Number of elements. */
    med_entite_maillage   mEntity;        /**< Type of entity, e.g. MED_MAILLE. */
    med_geometrie_element mGeom;          /**< Type of primitive, e.g. MED_TETRA10. */
    int                   mDim;           /**< Dimension of elements = mGeom / 100. */
    int                   mNumNodesByElt; /**< Number of nodes by element = mGeom % 100. */
    med_int*              mId;            /**< Optional; for each element, its identifier; NULL if undefined. */
    med_int*              mFamIdent;      /**< For each element, its family (identifier). */
    char*                 mNames;         /**< Optional; for each element, its name; NULL if undefined. */
    med_int*              mCon;           /**< For each element, list of nodes (index in 1..*) = table of connectivity. */
    std::set<med_int>     mSetOfNodes;    /**< Set of all nodes used by this set of elements. */
    
    bool                  mFlagPrintAll;  /** Flag to control the behaviour of the stream operator <<. */
    
private:

    // do not allow copy constructor
    Elements(const Elements&);
    
    // do not allow copy
    Elements& operator=(const Elements&);
    
    // do not allow operator ==
    bool operator==(const Elements&); 
    
}; // class Elements


} // namespace MULTIPR


#endif // MULTIPR_NODES_HXX

// EOF
