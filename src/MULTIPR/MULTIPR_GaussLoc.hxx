// Project MULTIPR, IOLS WP1.2.1 - EDF/CS
// Partitioning/decimation module for the SALOME v3.2 platform

/**
 * \file    MULTIPR_GaussLoc.hxx
 *
 * \brief   Class GaussLoc. Allow to locate all the Gauss points in an element of reference (e.g. a TETRA10).
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

#ifndef MULTIPR_GAUSSLOC_HXX
#define MULTIPR_GAUSSLOC_HXX

//*****************************************************************************
// Includes section
//*****************************************************************************

extern "C"
{
    #include "med.h"
}

#include <iostream>


namespace multipr
{


//*****************************************************************************
// Class GaussLoc
//*****************************************************************************

class GaussLoc
{
public:

    /** 
     * Builds an empty Gauss reference (default constructor).
     */
    GaussLoc();
    
    /**
     * Copy constructor (deep copy).
     * \param  pGaussLoc the GaussLoc to copy.
     */
    GaussLoc(const GaussLoc& pGaussLoc);
    
    /**
     * Destructor. Removes everything.
     */
    ~GaussLoc();
    
    /**
     * Resets this object in its state by default (empty). Cleans memory.
     */
    void reset();
    
    //---------------------------------------------------------------------
    // Basic accessors/mutators
    //--------------------------------------------------------------------
    
    /**
     * Returns the name of this GaussLoc.
     * \return the name of this GaussLoc.
     */
    std::string getName() const { return mName; }
    
    /**
     * Returns dimension of Gauss points which is also the dimension of reference nodes.
     * \return dimension of Gauss points.
     */
    int getDim() const { return mDim; }
     
    /**
     * Returns number of Gauss points.
     * \return number of Gauss points.
     */
    int getNumGaussPoints() const { return mNumGauss; }
      
    //---------------------------------------------------------------------
    // Algorithms
    //--------------------------------------------------------------------
    
    /**
     * Returns the coordinates of all the Gauss points for a given element.
     * \param  pCooElt (in)  coordinates of nodes of an element.
     * \param  pCoo    (out) coordinates of all the Gauss points (interlaced); memory must have been allocated.
     */
    void getCoordGaussPoints(const med_float* pCooElt, med_float* pCooGaussPoints) const;
    
    //---------------------------------------------------------------------
    // I/O
    //---------------------------------------------------------------------
    
    /**
     * Reads a GaussLoc object from a MED file.
     * \param  pMEDfile any valid MED file opened for reading.
     * \param  pIndex   index of the gauss localization to be read; must be >= 1.
     * \throw  IOException if any i/o error occurs.
     */
    void readMED(med_idt pMEDfile, med_int pIndex);
    
    /**
     * Writes this GaussLoc object to the given MED file.
     * \param  pMEDfile any valid MED file opened for writing.
     * \throw  IOException if any i/o error occurs.
     */
    void writeMED(med_idt pMEDfile);

    /**
     * Dumps any GaussLoc to the given output stream.
     * \param  pOs any output stream.
     * \param  pF  any Field.
     * \return the output stream pOs.
     */
    friend std::ostream& operator<<(std::ostream& pOs, GaussLoc& pG);

private:
    
    char                  mName[MED_TAILLE_NOM + 1];  /**< Name of the Gauss info. */
    med_geometrie_element mGeom;                      /**< Type of elements, e.g. TETRA10 (code is 310). */
    int                   mDim;                       /**< Dimension of nodes, e.g. 3 for a TETRA10. */
    int                   mNumNodes;                  /**< Number of nodes in the reference element, e.g. 10 for a TETRA10. */
    med_int               mNumGauss;                  /**< Number of Gauss points. */
    med_float*            mRefCoo;                    /**< Table of coordinates of nodes.
                                                           Example: for a TETRA10: 10 nodes, 3 components => 30 med_float. */
    med_float*            mGaussCoo;                  /**< Table of coordinates of Gauss points. */
    med_float*            mWeight;                    /**< Table of weights of Gauss points. */
    
private:
    
    // do not allow copy
    GaussLoc& operator=(const GaussLoc&);
    
    // do not allow operator ==
    bool operator==(const GaussLoc&); 
    
}; // class GaussLoc


} // namespace MULTIPR


#endif // MULTIPR_GAUSSLOC_HXX

// EOF
