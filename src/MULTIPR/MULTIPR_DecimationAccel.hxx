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
 * \file    MULTIPR_DecimationAccel.hxx
 *
 * \brief   Interface DecimationAccel: acceleration structure used for decimation.
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

#ifndef MULTIPR_DECIMATION_ACCEL_HXX
#define MULTIPR_DECIMATION_ACCEL_HXX

//*****************************************************************************
// Includes section
//*****************************************************************************

extern "C"
{
    #include "med.h"
}

#include <iostream>
#include <vector>
#include <math.h>

namespace multipr
{

//*****************************************************************************
// Predeclaration
//*****************************************************************************

class PointOfField;


//*****************************************************************************
// Interface DecimationAccel
//*****************************************************************************

class DecimationAccel
{    
public:
    
    /** 
     * Builds an empty DecimationAccel (default constructor).
     */
    DecimationAccel() { /* do nothing */ }
    
    /**
     * Destructor. Removes everything.
     */
    virtual ~DecimationAccel() { /* do nothing */ } 
    
    //---------------------------------------------------------------------
    // Algorithms
    //---------------------------------------------------------------------
    
    /**
     * Interface. Configures this acceleration structure. String is used for genericity.
     * \param  pArgv all the configuration parameters in a string.
     */
    virtual void configure(const char* pArgv) = 0;
    
    /**
     * Interface. Creates a new acceleration structure and fills it with the given list of points.
     * \param  pPts list of points to be inserted in the acceleration structure.
     */
    virtual void create(const std::vector<PointOfField>& pPts) = 0;
    
    /**
     * Interface. Finds all the points in a sphere defined by its center (x,y,z) and its radius.
     * \param  pCenterX x-coordinates of the center of the sphere.
     * \param  pCenterY y-coordinates of the center of the sphere.
     * \param  pCenterZ z-coordinates of the center of the sphere.
     * \param  pRadius  radius of the sphere.
     * \return all the points in a sphere defined by its center (x,y,z) and its radius.
     */
    virtual std::vector<PointOfField> findNeighbours(
        med_float pCenterX,
        med_float pCenterY,
        med_float pCenterZ,
        med_float pRadius) const = 0;
    
    //---------------------------------------------------------------------
    // I/O
    //---------------------------------------------------------------------
    
    /**
     * Sets the flag which control the stream operator <<.
     * \param  pFlag new flag value.
     */
    void setPrintAll(bool pFlag) { mFlagPrintAll = pFlag; } 
    
    /**
     * Dumps any GaussLoc to the given output stream.
     * \param  pOs any output stream.
     * \param  pA  any DecimationAccel.
     * \return the output stream pOs.
     */
    friend std::ostream& operator<<(std::ostream& pOs, DecimationAccel& pA);
    
protected:

    bool mFlagPrintAll;  /** Flag to control the behaviour of the stream operator <<. */
    
private:
    
    // do not allow copy constructor
    DecimationAccel(const DecimationAccel&);
    
    // do not allow copy
    DecimationAccel& operator=(const DecimationAccel&);
    
    // do not allow operator ==
    bool operator==(const DecimationAccel&); 

}; // class DecimationAccel


//*****************************************************************************
// Interface DecimationFilter and factory to build filters.
//*****************************************************************************

class DecimationAccelGrid : public DecimationAccel
{
public:

    /**
     * Builds an empty DecimationAccelGrid (default constructor).
     */
    DecimationAccelGrid();
    
    /**
     * Destructor. Removes everything.
     */
    virtual ~DecimationAccelGrid();
    
    /**
     * Resets this object in its state by default (empty). Cleans memory.
     */
    void reset();
    
    //---------------------------------------------------------------------
    // Algorithms
    //---------------------------------------------------------------------
    
    /**
     * Configures this acceleration structure. String is used for genericity.
     * \param  pArgv assumes "size_x size_y size_z": number of cells along each axis.
     */
    virtual void configure(const char* pArgv);
    
    /**
     * Creates a new acceleration structure and fills it with the given list of points.
     * setSize() must have been called before.
     * \param  pPts list of points to be inserted in the acceleration structure.
     * \throw  IllegalStateException if setSize() has not been called before.
     */
    virtual void create(const std::vector<PointOfField>& pPts);
    
    /**
     * Finds all the points in a sphere defined by its center (x,y,z) and its radius.
     * \param  pCenterX x-coordinates of the center of the sphere.
     * \param  pCenterY y-coordinates of the center of the sphere.
     * \param  pCenterZ z-coordinates of the center of the sphere.
     * \param  pRadius  radius of the sphere.
     * \return all the points in a sphere defined by its center (x,y,z) and its radius.
     */
    virtual std::vector<PointOfField> findNeighbours(
        med_float pCenterX,
        med_float pCenterY,
        med_float pCenterZ,
        med_float pRadius) const;
    
    /**
     * Returns the coordinates of the cell which contain the point (x,y,z).
     * Clamping is performed on (pIx, pIy, pIz) to avoid out of bounds.
     * \param  pX  (in) X-coordinates of the point.
     * \param  pY  (in) Y-coordinates of the point.
     * \param  pZ  (in) Z-coordinates of the point.
     * \param  pIx (out) X-index of the cell which contain the point (x,y,z).
     * \param  pIy (out) Y-index.
     * \param  pIz (out) Z-index.
     */
    void getCellCoord(
        med_float pX, med_float pY, med_float pZ,
        int* pIx, int* pIy, int* pIz) const;
    
    /**
     * Returns the index of the cell whose coordinates are (i, j, k).
     * \param  pI
     * \param  pJ
     * \param  pK
     * \return the index of the cell (i, j, k).
     */
    int getCellIndex(int pI, int pJ, int pK) const;
    
    /**
     * Returns the list of points contained in the cell of pPt.
     * \param  pPt any point which coordinates are in the bbox of this acceleration structure.
     * \return the list of points contained in the cell of pPt.
     */
    std::vector<PointOfField>& getCell(const PointOfField& pPt);
    
    //---------------------------------------------------------------------
    // I/O
    //---------------------------------------------------------------------
    
    /**
     * Dumps any GaussLoc to the given output stream.
     * \param  pOs any output stream.
     * \param  pG  any DecimationAccelGrid.
     * \return the output stream pOs.
     */
    friend std::ostream& operator<<(std::ostream& pOs, DecimationAccelGrid& pG);
    
private:
    
    /**
     * Computes the axis-aligned bounding box of a set of points.
     * Sets the fields mMin/mMax.
     * \param  pPts list of points.
     */
    void computeBBox(const std::vector<PointOfField>& pPts);
    
private:
    
    int                         mNum;        /**< Number of points in the grid. */
    med_float                   mMin[3];     /**< Bounding box, min corner. */
    med_float                   mMax[3];     /**< Bounding box, max corner. */
    med_float                   mInvLen[3];  /**< 1/length of cells, along each dimension; used to accelerate getCellCoord(). */
    med_int                     mSize[3];    /**< Number of cells along each dimension. */
    std::vector<PointOfField>*  mGrid;       /**< Flatten grid structure; each cell is a vector of PointOfField. */        
    
private:
    
    // do not allow copy constructor
    DecimationAccelGrid(const DecimationAccelGrid&);
    
    // do not allow copy
    DecimationAccelGrid& operator=(const DecimationAccelGrid&);
    
    // do not allow operator ==
    bool operator==(const DecimationAccelGrid&); 
    
}; // class DecimationAccelGrid


} // namespace MULTIPR


#endif // MULTIPR_DECIMATION_ACCEL_HXX

// EOF
