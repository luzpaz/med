//  Copyright (C) 2007-2011  CEA/DEN, EDF R&D
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
#include <set>
#include <iostream>

namespace multipr
{

enum eProfilBinding
{
    Undef,
    OnNodes,
    OnElements
};

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
     * Copy constructor.
     * \param pProfil The profil to copy.
     */
    Profil(const Profil& pProfil);
    
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
     * Get the complete set of elements of this profile.
     * \return The set of elements.
     */
    std::set< med_int>& getSet() { return mTable; }
    
    /**
     * Adds a new element to this profil.
     * \param  pElt element to be added; must be >= 1.
     */
    void add(med_int pElt);
    
    /**
     * Find the given element in the profile.
     * \param pElt The index of the element.
     * \return true if the element was found.
     */
    bool    find(med_int pElt);
    
    /**
     * Assign a set of element to this profile.
     * \param pElt The set of element to assign.
     */
    void    set(std::set< med_int>& pElt) { this->mTable = pElt; }
    
    /**
     * Select the elements of pIn contained in this profile. The two parameters pIn 
     * and pOut MUST be different.
     * \param pIn Set of elements to filter.
     * \param pOut The result.
     * \throw IllegalStateException if pIn and pOut are not different.
     */
    void    filterSetOfElement(std::set<med_int>& pIn, std::set<med_int>& pOut);
    
    /**
     * Extract a profil. The two parameters pIn 
     * and pOut MUST be different.
     * \param pIn Set of elements to extract.
     * \param pOut The result.
     * \throw IllegalStateException if pIn and pOut are not different.
     */
    void    extractSetOfElement(const std::set<med_int>& pIn, std::set<med_int>& pOut);
    
    /**
     * Returns the association of the profile.
     * \return OnNodes or OnElements or Undef.
     */
    eProfilBinding      getBinding() { return mBinding; }
    
    /**
     * Return the index of the geometry (if any) associated with this profile.
     * If this profile is on nodes it will return 0. Since the geometry index starts at
     * 0 use getBinding to know if its on elements.
     * \return the index of the geometry.
     */
    unsigned            getGeomIdx() { return mGeomIdx; }
    
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
     * Try to find in the MED file if this profile is associated to the nodes
     * or the elements. If the profile is associated with elements it will get the
     * index of the geometry.
     * \param pMEDfile any valid MED file opened for reading.
     * \param pMeshName The name of the mesh.
     * \throw IllegalStateException if the profile is empty.
     */
    void    readProfilBinding(med_idt pMEDfile, char* pMeshName);
        
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
    
    char                mName[MED_TAILLE_NOM + 1];  /**< Name of the profil. */
    std::set<med_int>   mTable;                     /**< Table of elements. */
    eProfilBinding      mBinding;                   /**< Profil of nodes or elements. */
    unsigned            mGeomIdx;                   /**< If this profile is on elements, the index of the geometry. */
    
private:
    
    // do not allow copy
    Profil& operator=(const Profil&);
    
    // do not allow operator ==
    bool operator==(const Profil&); 
    
}; // class Profil


} // namespace MULTIPR


#endif // MULTIPR_PROFIL_HXX

// EOF
