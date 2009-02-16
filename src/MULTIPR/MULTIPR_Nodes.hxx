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
 * \file    MULTIPR_Nodes.hxx
 *
 * \brief   Class Nodes = table of nodes.
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

#ifndef MULTIPR_NODES_HXX
#define MULTIPR_NODES_HXX

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
// Class Nodes
//*****************************************************************************

class Nodes
{
public:

    /** 
     * Builds an empty set of nodes (default constructor).
     */
    Nodes();
    
    /**
     * Destructor. Removes everything.
     */
    ~Nodes();
    
    /**
     * Resets this object in its state by default (empty). Cleans memory.
     */
    void reset();
    
    //---------------------------------------------------------------------
    // Basic accessors/mutators
    //---------------------------------------------------------------------
    
    /**
     * Returns true if nodes have a name.
     * \return true if nodes have a name.
     */
    bool isNames() const { return (mNames != NULL); }
    
    /**
     * Returns true if elements have an identifier (= a number).
     * \return true if elements have an identifier (= a number).
     */
    bool isIdentifiers() const { return (mId != NULL); }
      
    /**
     * Returns the coordinates of one node according to its index.
     * \param  pIndexNode index of node in [0..NUMBER_OF_NODES-1].
     * \return the coordinates of one node.
     * \throw  IndexOutOfBoundsException if pIndexNode is invalid.
     */
    const med_float* getCoordinates(med_int pIndexNode) const;
    
    /**
     * Returns the family of one node.
     * \param  pIndexNode index of node in [0..NUMBER_OF_NODES-1].
     * \return the family of the given node.
     * \throw  IndexOutOfBoundsException if pIndexNode is invalid.
     */
    med_int getFamIdent(med_int pIndexNode) const;
    
    /**
     * Returns the number of nodes.
     * \return the number of nodes.
     */
    int getNumberOfNodes() const { return mNum; }
    
    //---------------------------------------------------------------------
    // Algorithms
    //---------------------------------------------------------------------
    
    /**
     * Returns the axis-aligned bounding box (CARTESIAN coordinates system) of this set of nodes.
     * \param  pMin (out) coordinates of the min-corner of the bbox.
     * \param  pMin (out) coordinates of the max-corner of the bbox.
     */
    void getBBox(med_float pMin[3], med_float pMax[3]) const;

    /**
     * Returns the set of families referenced by this set of nodes.
     * Each family is described by its identifier.
     * \return the set of families referenced by this set of nodes.
     */
    std::set<med_int> getSetOfFamilies() const;
    
    /**
     * Constructor. Creates a subset of this set of nodes from a set of indices.
     * \param  pSetOfIndices set of indices; each index must be >= 1.
     * \return a subset of this set of nodes from a set of indices.
     */
    Nodes* extractSubSet(const std::set<med_int>& pSetIndices) const;
    
    /**
     * Builds a new set of nodes by merging two set of nodes (this and pNodes) if possible.
     * Merge is partial because Id and Names are not take into account.
     * \param  pNodes any set of nodes to be merged with this.
     * \return a new set of nodes.
     * \throw  IllegalStateException if the two sets of nodes are incompatible.
     */
    Nodes* mergePartial(Nodes* pNodes);
    Nodes* mergePartial(std::vector<Nodes*> pNodes);
    
    //---------------------------------------------------------------------
    // I/O
    //---------------------------------------------------------------------

    /**
     * Reads all nodes of a mesh from a MED file.
     * \param  pMEDfile   any valid MED file opened for reading.
     * \param  pMeshName  name of the mesh to be read.
     * \param  pDim       dimension of the mesh.
     * \throw  IOException if any i/o error occurs.
     */
    void readMED(med_idt pMEDfile, char* pMeshName, med_int pDim);
    
    /**
     * Writes this set of nodes to a MED file. Nodes are associated with the given mesh.
     * WARNING: mesh must have been created and added to the MED file before.
     * \param  pMEDfile  any valid MED file opened for writing.
     * \param  pMeshName any mesh of the MED file. 
     * \throw  IOException if any i/o error occurs.
     */
    void writeMED(med_idt pMEDfile, char* pMeshName) const;
     
    /**
     * Sets the flag which control the stream operator <<.
     * \param  pFlag new flag value.
     */
    void setPrintAll(bool pFlag) { mFlagPrintAll = pFlag; } 
     
    /**
     * Dumps any Nodes to the given output stream.
     * \param  pOs any output stream.
     * \param  pN  any Nodes.
     * \return the output stream pOs.
     */
    friend std::ostream& operator<<(std::ostream& pOs, Nodes& pN);
    
private:
    
    med_int     mNum;            /**< Number of nodes in this set. */
    med_int     mDim;            /**< Dimension of nodes. */
    med_repere  mCoordSystem;    /**< Type of coordinates system; should be MED_CART for cartesian. */
    
    med_int*    mId;             /**< Optional; for each node, its identifier; NULL if undefined. */
    med_int*    mFamIdent;       /**< For each node, its identifier of its family. */
    char*       mNames;          /**< Optional; for each node, its name; NULL if undefined. */
    med_float*  mCoo;            /**< For each node, coordinates of node; array is interlaced: x1 y1 z1 x2 y2 z2 ...*/
    char*       mNamesCoo;       /**< Names of axis of the coordinates system. */     
    char*       mNamesUnitCoo;   /**< Unit of axis of the coordinates system.  */
    
    bool        mFlagPrintAll;   /** Flag to control the behaviour of the stream operator <<. */
    
private:

    // do not allow copy constructor
    Nodes(const Nodes&);
    
    // do not allow copy
    Nodes& operator=(const Nodes&);
    
    // do not allow operator ==
    bool operator==(const Nodes&); 
    
}; // class Nodes


} // namespace MULTIPR


#endif // MULTIPR_NODES_HXX

// EOF
