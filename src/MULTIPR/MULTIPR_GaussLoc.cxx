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
 * \file    MULTIPR_GaussLoc.cxx
 *
 * \brief   see MULTIPR_GaussLoc.hxx
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

//*****************************************************************************
// Includes section
//*****************************************************************************

#include "MULTIPR_GaussLoc.hxx"
#include "MULTIPR_Utils.hxx"
#include "MULTIPR_Exceptions.hxx"

#include <iostream>
#include <cmath>
#include <cstring>

using namespace std;


namespace multipr
{


//*****************************************************************************
// Class GaussLoc implementation
//*****************************************************************************

GaussLoc::GaussLoc() 
{
    mRefCoo    = NULL;
    mGaussCoo  = NULL;
    mWeight    = NULL;
    
    reset(); 
}


GaussLoc::GaussLoc(const GaussLoc& pGaussLoc)
{
    mRefCoo   = NULL;
    mGaussCoo = NULL;
    mWeight   = NULL;
    
    strcpy(mName, pGaussLoc.mName);
    
    mGeom     = pGaussLoc.mGeom;
    mDim      = pGaussLoc.mDim;
    mNumNodes = pGaussLoc.mNumNodes;
    mNumGauss = pGaussLoc.mNumGauss;
    
    if (mDim != (mGeom / 100)) throw IllegalStateException("", __FILE__, __LINE__);
    if (mNumNodes != (mGeom % 100)) throw IllegalStateException("", __FILE__, __LINE__);
    
    if (pGaussLoc.mRefCoo != NULL)
    {
        mRefCoo = new med_float[mDim * mNumNodes];
        memcpy(mRefCoo, pGaussLoc.mRefCoo, sizeof(med_float) * mDim * mNumNodes);
    }
    
    if (pGaussLoc.mGaussCoo != NULL)
    {
        mGaussCoo  = new med_float[mDim * mNumGauss];
        memcpy(mGaussCoo, pGaussLoc.mGaussCoo, sizeof(med_float) * mDim * mNumGauss);
    }
    
    if (pGaussLoc.mWeight != NULL)
    {
        mWeight = new med_float[mNumGauss];
        memcpy(mWeight, pGaussLoc.mWeight, sizeof(med_float) * mNumGauss);
    }
}


GaussLoc::~GaussLoc()  
{ 
    reset();  
}


void GaussLoc::reset() 
{ 
    mName[0]  = '\0';
    mGeom     = MED_NONE;
    mDim      = 0;
    mNumNodes = 0;
    mNumGauss = 0;
    
    if (mRefCoo   != NULL) { delete[] mRefCoo;   mRefCoo   = NULL; }
    if (mGaussCoo != NULL) { delete[] mGaussCoo; mGaussCoo = NULL; }
    if (mWeight   != NULL) { delete[] mWeight;   mWeight   = NULL; }
}


void GaussLoc::getCoordGaussPoints(
    const med_float* pCooElt,  
    med_float* pCooGaussPoints,
        int nb_gauss) const
{
    med_float*              pt    = const_cast<med_float*>(pCooElt);
    const med_float*        coeff = mGaussCoo;
    med_float*              dest  = pCooGaussPoints;

    for (int i = 0; i < mNumGauss; ++i)
    {
                for (int j = 0; j < mDim; ++j)
                {
                        dest[j] = 0;
            for (int k = 1; k < nb_gauss; ++k)
            {
                dest[j] += pt[k * mDim + j];
                        }
            dest[j] /= nb_gauss;
                }
        coeff += mDim;
        dest += mDim;
        }
}


void GaussLoc::readMED(med_idt pMEDfile, med_int pIndex)
{
    if (pMEDfile == 0) throw IOException("pMEDfile should not be NULL", __FILE__, __LINE__);
    if (pIndex < 1) throw IllegalArgumentException("pIndex should be >= 1", __FILE__, __LINE__);
    
    reset();
    
    med_err ret = MEDgaussInfo(
        pMEDfile, 
        pIndex, 
        mName, 
        &mGeom, 
        &mNumGauss);
    
    if (ret != 0) throw IOException("i/o error while reading Gauss localization information in MED file", __FILE__, __LINE__);
    
    mDim      = mGeom / 100;
    mNumNodes = mGeom % 100;
    
    mRefCoo   = new med_float[mDim * mNumNodes];
    mGaussCoo = new med_float[mDim * mNumGauss];
    mWeight   = new med_float[mNumGauss];
    
    ret = MEDgaussLire(
        pMEDfile, 
        mRefCoo, 
        mGaussCoo, 
        mWeight, 
        MED_FULL_INTERLACE, 
        mName);
        
    if (ret != 0) throw IOException("i/o error while reading Gauss localization in MED file", __FILE__, __LINE__);
}


void GaussLoc::writeMED(med_idt pMEDfile)
{
    if (pMEDfile == 0) throw IOException("", __FILE__, __LINE__);
    if (mNumGauss < 0) throw IllegalStateException("", __FILE__, __LINE__);
    if (mRefCoo == NULL) throw IllegalStateException("", __FILE__, __LINE__);
    if (mGaussCoo == NULL) throw IllegalStateException("", __FILE__, __LINE__);
    if (mWeight == NULL) throw IllegalStateException("", __FILE__, __LINE__);
    if (strlen(mName) > MED_TAILLE_NOM) throw IllegalStateException("", __FILE__, __LINE__);
    
    med_err ret = MEDgaussEcr(
        pMEDfile,  
        mGeom, 
        mRefCoo, 
        MED_FULL_INTERLACE, 
        mNumGauss, 
        mGaussCoo,
        mWeight,
        mName); 
        
    if (ret != 0) throw IOException("i/o error while writing Gauss localization", __FILE__, __LINE__);
}


ostream& operator<<(ostream& pOs, GaussLoc& pG)
{
    pOs << "Gauss ref:" << endl;
    pOs << "    Name     =|" << pG.mName << "|" << endl;
    pOs << "    Geom     =" << pG.mGeom << endl;
    pOs << "    #Pt Gauss=" << pG.mNumGauss << endl;
    
    pOs << "    Ref nodes coords.: (#nodes=" << pG.mNumNodes << " dim=" << pG.mDim << ")" << endl;
    for (int itNode = 0 ; itNode < pG.mNumNodes ; itNode++)
    {
        pOs << "        Node " << (itNode + 1) << ": ";
        for (int itDim = 0; itDim < pG.mDim ; itDim++)
        {
            pOs << pG.mRefCoo[itNode * pG.mDim + itDim] << " ";
        }
        pOs << endl;
    }
    
    pOs << "    Gauss coords. and weight:" << endl;
    for (int itGauss = 0 ; itGauss < pG.mNumGauss ; itGauss++)
    {
        pOs << "        Pt " << (itGauss+1) << ": ";
        for (int itDim = 0; itDim < pG.mDim ; itDim++)
        {
            pOs << pG.mGaussCoo[itGauss * pG.mDim + itDim] << " ";
        }
        pOs << "weight=" << pG.mWeight[itGauss];
        pOs << endl;
    }

    // debug
    //med_float res[15];
    //pG.getCoordGaussPoints(pG.mRefCoo, res);
    //printArray2D(res, pG.mNumGauss, pG.mDim, "Gauss pt");
    
    return pOs;
}


} // namespace multipr

// EOF
