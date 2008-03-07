// Project MULTIPR, IOLS WP1.2.1 - EDF/CS
// Partitioning/decimation module for the SALOME v3.2 platform

/**
 * \file    MULTIPR_DecimationFilter.hxx
 *
 * \brief   Interface DecimationFilter: filter used for decimation.
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

#ifndef MULTIPR_DECIMATION_FILTER_HXX
#define MULTIPR_DECIMATION_FILTER_HXX

//*****************************************************************************
// Includes section
//*****************************************************************************

extern "C"
{
    #include "med.h"
}

#include <iostream>
#include <vector>


namespace multipr
{

//*****************************************************************************
// Predeclaration
//*****************************************************************************

class Mesh;
class PointOfField;


//*****************************************************************************
// Interface DecimationFilter and factory to build filters.
// Should be derivated to implement new filter.
//*****************************************************************************

class DecimationFilter
{
public:
    /**
     * Decimation filter factory. Must be used to build any new filter from its name.
     * \param  pFilterName name of the filter to be instanciated.
     * \return a new instance of the given filter. 
     */
    static DecimationFilter* create(const char* pFilterName);
    
public:
    
    /** 
     * Builds an empty DecimationFilter (default constructor).
     */
    DecimationFilter() { /* do nothing */ }
    
    /**
     * Destructor. Removes everything.
     */
    virtual ~DecimationFilter() { /* do nothing */ } 
    
    /**
     * Interface. Creates a new Mesh by decimating the given Mesh with this DecimationFilter.
     * \param  pMesh        any mesh to be decimated; must not be NULL.
     * \param  pArgv        all the arguments of the filter as a string; must not be NULL.
     * \param  pNameNewMesh name of the decimated mesh; must not be NULL.
     * \return the decimated mesh.
     */
    virtual Mesh* apply(Mesh* pMesh, const char* pArgv, const char* pNameNewMesh) = 0;
    
private:
    
    // do not allow copy constructor
    DecimationFilter(const DecimationFilter&);
    
    // do not allow copy
    DecimationFilter& operator=(const DecimationFilter&);
    
    // do not allow operator ==
    bool operator==(const DecimationFilter&); 

}; // class DecimationFilter


//*****************************************************************************
// Filter : Average Gradient
//*****************************************************************************

class DecimationFilterGradAvg : public DecimationFilter
{
public:

    /**
     * Builds an empty DecimationFilterGradAvg (default constructor).
     */
    DecimationFilterGradAvg();
    
    /**
     * Destructor. Removes everything.
     */
    virtual ~DecimationFilterGradAvg();
    
    /**
     * Creates a new Mesh by decimating the given Mesh with this DecimationFilter.
     * 
     * For each element
     *     Keep this element if all its points (related to a field) have a |gradient| > threshold.
     *
     * For each point, gradient is computed by considering all the neighbours in a sphere of radius r.
     *
     * \param  pMesh        any mesh to be decimated; must not be NULL.
     * \param  pArgv        all the arguments of the filter as a string; must not be NULL.
     *                      for GradAvg, expected "fieldName fieldIt threshold radius boxing" (5 parameters).
     * \param  pNameNewMesh name of the decimated mesh; must not be NULL.
     * \return the decimated mesh.
     */
    virtual Mesh* apply(Mesh* pMesh, const char* pArgv, const char* pNameNewMesh);
    
    /**
     * Returns information about gradient.
     */
    void getGradientInfo(
        Mesh*       pMesh, 
        const char* pFieldName, 
        int         pFieldIt, 
        double      pRadius,
        int         pBoxing,
        double*     pOutGradMin,
        double*     pOutGradAvg,
        double*     pOutGradMax);
    
private:

    /**
     * Returns the norm of the gradient of the field at the given point.
     * Sub-method used by apply().
     * \param  pPt        any point in the field.
     * \param  pNeigbours neighbourhood of pPt.
     * \return the norm of the gradient of the field in pPt.
     */
    med_float computeNormGrad(const PointOfField& pPt, const std::vector<PointOfField>& pNeighbours) const;
    
private:
    
    // do not allow copy constructor
    DecimationFilterGradAvg(const DecimationFilterGradAvg&);
    
    // do not allow copy
    DecimationFilterGradAvg& operator=(const DecimationFilterGradAvg&);
    
    // do not allow operator ==
    bool operator==(const DecimationFilterGradAvg&); 
    
}; // class DecimationFilterGradAvg

//*****************************************************************************
// Filter : Treshold
//*****************************************************************************

class DecimationFilterTreshold : public DecimationFilter
{
public:

    /**
     * Builds an empty DecimationFilterTreshold (default constructor).
     */
    DecimationFilterTreshold();
    
    /**
     * Destructor. Removes everything.
     */
    virtual ~DecimationFilterTreshold();
    
    /**
     * Creates a new Mesh by decimating the given Mesh with this DecimationFilter.
     * 
     * For each element
     *     Keep this element if all its points (related to a field) have a value > threshold.
     *
     * \param  pMesh        any mesh to be decimated; must not be NULL.
     * \param  pArgv        all the arguments of the filter as a string; must not be NULL.
     *                      for GradAvg, expected "fieldName fieldIt threshold radius boxing" (5 parameters).
     * \param  pNameNewMesh name of the decimated mesh; must not be NULL.
     * \return the decimated mesh.
     */
    virtual Mesh* apply(Mesh* pMesh, const char* pArgv, const char* pNameNewMesh);

    
private:
    
    // do not allow copy constructor
    DecimationFilterTreshold(const DecimationFilterTreshold&);
    
    // do not allow copy
    DecimationFilterTreshold& operator=(const DecimationFilterTreshold&);
    
    // do not allow operator ==
    bool operator==(const DecimationFilterTreshold&); 
    
}; // class DecimationFilterTreshold

} // namespace MULTIPR


#endif // MULTIPR_DECIMATION_FILTER_HXX

// EOF
