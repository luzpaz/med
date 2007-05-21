// Project MULTIPR, IOLS WP1.2.1 - EDF/CS
// Partitioning/decimation module for the SALOME v3.2 platform

/**
 * \file    MULTIPR_Profil.hxx
 *
 * \brief   Class Profil used to managed MED profil.
 *          Profil is just a list of indices related to elements (NODES, CELLS).
 *          !!! currently not used !!!
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

#ifndef MULTIPR_PROFIL_HXX
#define MULTIPR_PROFIL_HXX

//*****************************************************************************
// Includes section
//*****************************************************************************

extern "C"
{
    #include "med.h"
}

#include <vector>


namespace multipr
{


//*****************************************************************************
// Class Profil
//*****************************************************************************

class Profil
{
public:

    /** 
     * Builds an empty profil (default constructor).
     */
    Profil();
    
    /**
     * Destructor. Removes everything.
     */
    ~Profil();
    
    /**
     * Resets this object in its state by default (empty). Cleans memory.
     */
    void reset();
    
    /**
     * Creates a profil from its name (reset before).
     * \param  pName name of the profil to be created.
     * \throw  NullArgumentException if pName is NULL.
     */
    void create(const char* pName);
    
    //---------------------------------------------------------------------
    // Basic accessors/mutators
    //---------------------------------------------------------------------
    
    /**
     * Returns the name of this profil.
     * \return the name of this profil.
     */
    const char* getName() const;
    
    /**
     * Returns the nth elements of this profil.
     * \param  pIndex index of the element to get; must be in [0..NUMBER_OF_ELEMENTS-1].
     * \return the nth elements of this profil.
     * \throw  IndexOutOfBoundsException if index is invalid.
     */
    med_int get(med_int pIndex) const;
    
    /**
     * Adds a new element to this profil.
     * \param  pElt element to be added; must be >= 1.
     */
    void add(med_int pElt);
    
    //---------------------------------------------------------------------
    // I/O
    //---------------------------------------------------------------------
    
    /**
     * Reads a Profil from a MED file.
     * \param  pMEDfile     any valid MED file opened for reading.
     * \param  pIndexProfil index of the profil to be read; must be >= 1.
     * \throw  IOException  if any i/o error occurs.
     */
    void readMED(med_idt pMEDfile, med_int pIndexProfil);
    
    /**
     * Writes this profil to a MED file.
     * \param  pMEDfile    any valid MED file opened for writing.
     * \throw  IOException if any i/o error occurs.
     */
    void writeMED(med_idt pMEDfile);
    
    /**
     * Dumps any Profil to the given output stream.
     * \param  pOs any output stream.
     * \param  pP  any Profil.
     * \return the output stream pOs.
     */
    friend std::ostream& operator<<(std::ostream& pOs, Profil& pP);
    
private:
    
    char                  mName[MED_TAILLE_NOM + 1];  /**< Name of the profil. */
    std::vector<med_int>  mTable;                     /**< Table of elements. */
    
private:

    // do not allow copy constructor
    Profil(const Profil&);
    
    // do not allow copy
    Profil& operator=(const Profil&);
    
    // do not allow operator ==
    bool operator==(const Profil&); 
    
}; // class Profil


} // namespace MULTIPR


#endif // MULTIPR_PROFIL_HXX

// EOF
