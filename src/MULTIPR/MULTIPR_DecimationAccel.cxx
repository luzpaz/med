//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
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
 * \file    MULTIPR_DecimationAccel.cxx
 *
 * \brief   see MULTIPR_DecimationAccel.hxx
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

//*****************************************************************************
// Includes section
//*****************************************************************************

#include "MULTIPR_DecimationAccel.hxx"
#include "MULTIPR_PointOfField.hxx"
#include "MULTIPR_Exceptions.hxx"

#include <cmath>
#include <iostream>

using namespace std;


namespace multipr
{


//*****************************************************************************
// Class DecimationAccel implementation
//*****************************************************************************


ostream& operator<<(ostream& pOs, DecimationAccel& pA)
{
    pOs << "DecimationAccel:" << endl;
    return pOs;
}


//*****************************************************************************
// Class DecimationAccelGrid
//*****************************************************************************

DecimationAccelGrid::DecimationAccelGrid() 
{
    mGrid = NULL;
    
    reset();
}


DecimationAccelGrid::~DecimationAccelGrid()  
{ 
    reset();
}


void DecimationAccelGrid::reset()
{
    mNum = 0;
    
    mMin[0] = numeric_limits<med_float>::quiet_NaN();
    mMin[1] = numeric_limits<med_float>::quiet_NaN();
    mMin[2] = numeric_limits<med_float>::quiet_NaN();
    
    mMax[0] = numeric_limits<med_float>::quiet_NaN();
    mMax[1] = numeric_limits<med_float>::quiet_NaN();
    mMax[2] = numeric_limits<med_float>::quiet_NaN();
    
    mInvLen[0] = numeric_limits<med_float>::quiet_NaN();
    mInvLen[1] = numeric_limits<med_float>::quiet_NaN();
    mInvLen[2] = numeric_limits<med_float>::quiet_NaN();
    
    mSize[0] = 0;
    mSize[1] = 0;
    mSize[2] = 0;
    
    if (mGrid != NULL)
    {
        delete[] mGrid;
        mGrid = NULL;
    }
    
    mFlagPrintAll = false;
}


void DecimationAccelGrid::create(const std::vector<PointOfField>& pPts)
{
    // check if grid have been initialized
    if (mSize[0] == 0) throw IllegalStateException("call setSize() before", __FILE__, __LINE__);
    
    // compute bbox of the grid
    computeBBox(pPts);
    
    // allocate the grid
    int size = mSize[0] * mSize[1] * mSize[2];
    mGrid = new vector<PointOfField>[size];
    
    // fill the grid
    mNum = pPts.size();
    for (int i = 0 ; i < mNum ; i++)
    {
        vector<PointOfField>& cell = getCell(pPts[i]);
        cell.push_back(pPts[i]);
    }
}


void DecimationAccelGrid::configure(const char* pArgv)
{
    // check arguments
    if (pArgv == NULL) throw NullArgumentException("", __FILE__, __LINE__);
    
    int sizeX = 0;
    int sizeY = 0;
    int sizeZ = 0;
    
    int ret = sscanf(pArgv, "%d %d %d", &sizeX, &sizeY, &sizeZ);
    
    if (ret != 3) throw IllegalArgumentException("expected 3 parameters", __FILE__, __LINE__);
    if (sizeX <= 0) throw IllegalArgumentException("number of cells along X-axis must be > 0", __FILE__, __LINE__);
    if (sizeY <= 0) throw IllegalArgumentException("number of cells along Y-axis must be > 0", __FILE__, __LINE__);
    if (sizeZ <= 0) throw IllegalArgumentException("number of cells along Z-axis must be > 0", __FILE__, __LINE__);
    
    reset();
    
    mSize[0] = sizeX;
    mSize[1] = sizeY;
    mSize[2] = sizeZ;
}


void DecimationAccelGrid::getCellCoord(
        med_float pX, med_float pY, med_float pZ,
        int* pIx, int* pIy, int* pIz) const
{
    med_float cx = (pX - mMin[0]) * mInvLen[0];
    med_float cy = (pY - mMin[1]) * mInvLen[1];
    med_float cz = (pZ - mMin[2]) * mInvLen[2];
    
    // clamp all indices to avoid overflow
    *pIx = med_int(cx);
    if (*pIx >= mSize[0]) *pIx = mSize[0] - 1;
    else if (*pIx < 0) *pIx = 0;
    
    *pIy = med_int(cy);
    if (*pIy >= mSize[1]) *pIy = mSize[1] - 1;
    else if (*pIy < 0) *pIy = 0;
    
    *pIz = med_int(cz);
    if (*pIz >= mSize[2]) *pIz = mSize[2] - 1;
    else if (*pIz < 0) *pIz = 0;
}


int DecimationAccelGrid::getCellIndex(int pI, int pJ, int pK) const
{    
    int index = pK * (mSize[0] * mSize[1]) + pJ * mSize[0] + pI;
    
    return index;
}


vector<PointOfField>& DecimationAccelGrid::getCell(const PointOfField& pPt)
{
    int ix, iy, iz;
    
    getCellCoord(
        pPt.mXYZ[0], pPt.mXYZ[1], pPt.mXYZ[2],
        &ix, &iy, &iz);
    
    int index = getCellIndex(ix, iy, iz);
    
    return mGrid[index];
}


vector<PointOfField> DecimationAccelGrid::findNeighbours(
    med_float pCenterX,
    med_float pCenterY,
    med_float pCenterZ,
    med_float pRadius) const
{    
    //---------------------------------------------------------------------
    // Determine the axis aligned bounding box of the sphere ((x, y, z), r)
    //---------------------------------------------------------------------
    med_float sphereBBoxMin[3];
    med_float sphereBBoxMax[3];
    
    sphereBBoxMin[0] = pCenterX - pRadius;
    sphereBBoxMin[1] = pCenterY - pRadius;
    sphereBBoxMin[2] = pCenterZ - pRadius;

    sphereBBoxMax[0] = pCenterX + pRadius;
    sphereBBoxMax[1] = pCenterY + pRadius;
    sphereBBoxMax[2] = pCenterZ + pRadius;

    //---------------------------------------------------------------------
    // Determine the cells of the grid intersected by the sphere ((x, y, z), r)
    // => range of cells are [iMinCell[0], iMaxCell[0]] x [iMinCell[1], iMaxCell[1]] x [iMinCell[2], iMaxCell[2]]
    //---------------------------------------------------------------------
    int iMinCell[3];
    int iMaxCell[3];

    getCellCoord(sphereBBoxMin[0], sphereBBoxMin[1], sphereBBoxMin[2], 
            &iMinCell[0], &iMinCell[1], &iMinCell[2]);

    getCellCoord(sphereBBoxMax[0], sphereBBoxMax[1], sphereBBoxMax[2], 
            &iMaxCell[0], &iMaxCell[1], &iMaxCell[2]);

    //---------------------------------------------------------------------
    // Collect points of the field which are in the sphere
    //---------------------------------------------------------------------
    vector<PointOfField> res;
    
    if (isnan(pCenterX) || isnan(pCenterY) || isnan(pCenterZ))
    {
        return res;
    }
    
    // for all the cells in the grid intersected by the sphere ((x, y, z), r)
    for (int i = iMinCell[0] ; i <= iMaxCell[0] ; i++)
    {
        for (int j = iMinCell[1] ; j <= iMaxCell[1] ; j++)
        {
            for (int k = iMinCell[2] ; k <= iMaxCell[2] ; k++)
            {
                int idCell = getCellIndex(i, j, k);

                vector<PointOfField>& cell = mGrid[idCell];
            
                // for all the points in the current cell
                for (vector<PointOfField>::const_iterator itPoint = cell.begin() ; itPoint != cell.end() ; itPoint++)
                {
                    const PointOfField& currentPt = *itPoint;
                    
                    // test if currentPt is in the sphere ((x, y, z), r)
                    med_float vecX = currentPt.mXYZ[0] - pCenterX;
                    med_float vecY = currentPt.mXYZ[1] - pCenterY;
                    med_float vecZ = currentPt.mXYZ[2] - pCenterZ;
                    
                    med_float norm = med_float( sqrt( vecX*vecX + vecY*vecY + vecZ*vecZ ) );
                    if (norm < pRadius)
                    {
                        // only add the point if it is different from (x, y, z)
                        if ((currentPt.mXYZ[0] != pCenterX) ||
                            (currentPt.mXYZ[1] != pCenterY) ||
                            (currentPt.mXYZ[2] != pCenterZ))
                        {
                            res.push_back(currentPt);
                        }
                    }
                }
            }
        }
    }

    return res;
}


void DecimationAccelGrid::computeBBox(const std::vector<PointOfField>& pPts)
{
    for (int itDim = 0 ; itDim < 3 ; itDim++) 
    { 
        mMin[itDim] = numeric_limits<med_float>::max();
        mMax[itDim] = -mMin[itDim];
    }
    
    for (unsigned i = 0 ; i < pPts.size() ; i++)
    {
        for (int itDim = 0 ; itDim < 3 ; itDim++)
        {
            med_float coord = pPts[i].mXYZ[itDim];
            if (coord < mMin[itDim]) mMin[itDim] = coord;
            if (coord > mMax[itDim]) mMax[itDim] = coord;
        }
    }

    mInvLen[0] = med_float(mSize[0]) / (mMax[0] - mMin[0]);
    mInvLen[1] = med_float(mSize[1]) / (mMax[1] - mMin[1]);
    mInvLen[2] = med_float(mSize[2]) / (mMax[2] - mMin[2]);
}


ostream& operator<<(ostream& pOs, DecimationAccelGrid& pG)
{
    pOs << "DecimationAccelGrid:" << endl;
    pOs << "    Num=" << pG.mNum << endl;
    pOs << "    Size=" << pG.mSize[0] << " x " << pG.mSize[1] << " x " << pG.mSize[2] << endl;
    pOs << "    BBox=[" << pG.mMin[0] << " ; " << pG.mMax[0] << "] x [" << pG.mMin[1] << " ; " << pG.mMax[1] << "] x [" << pG.mMin[2] << " ; " << pG.mMax[2] << "]" << endl;
    pOs << "    Inv len.=" << pG.mInvLen[0] << " ; " << pG.mInvLen[1] << " ; " << pG.mInvLen[2] << endl;
    
    if (pG.mFlagPrintAll)
    {
        int checkNumCells = 0;
        int numCells = pG.mSize[0] * pG.mSize[1] * pG.mSize[2];
        for (int i = 0 ; i < numCells ; i++)
        {
            vector<PointOfField>& cell = pG.mGrid[i];
            cout << "    Cell " << i << ": #=" << cell.size() << ": " << endl;
            for (unsigned j = 0 ; j < cell.size() ; j++)
            {
                cout << "        " << cell[j] << endl;
            }
            checkNumCells += cell.size();
        }
        
        if (pG.mNum != checkNumCells) throw IllegalStateException("", __FILE__, __LINE__);
    }
    
    return pOs;
}


} // namespace multipr

// EOF
