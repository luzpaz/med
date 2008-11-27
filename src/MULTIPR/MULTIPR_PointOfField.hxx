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
//  Partitioning/decimation module for the SALOME v3.2 platform
//
/**
 * \file    MULTIPR_PointOfField.hxx
 *
 * \brief   Class PointOfField used for decimation. PointOfField = a point in a field = coordinates + value.
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

#ifndef MULTIPR_POINTOFFIELD_HXX
#define MULTIPR_POINTOFFIELD_HXX

//*****************************************************************************
// Includes section
//*****************************************************************************

extern "C"
{
    #include "med.h"
}


namespace multipr
{


//*****************************************************************************
// Class PointOfField
//*****************************************************************************

class PointOfField
{
public:

    /** 
     * Builds an empty set of elements (default constructor).
     */
    PointOfField() { reset(); }
    
    /**
     * Constructor.
     * \param  pX   x-coordinate of the point.
     * \param  pY   y-coordinate of the point.
     * \param  pZ   z-coordinate of the point.
     * \param  pVal value of the field at the given point.
     */
    PointOfField(med_float pX, med_float pY, med_float pZ, med_float pVal)
    {
        mXYZ[0] = pX;
        mXYZ[1] = pY;
        mXYZ[2] = pZ;
        mVal    = pVal;
    }
    
    /**
     * Destructor. Removes everything.
     */
    ~PointOfField() { reset(); }
    
    /**
     * Resets this object in its state by default (empty).
     */
    void reset()
    {
        mXYZ[0] = std::numeric_limits<med_float>::quiet_NaN();
        mXYZ[1] = std::numeric_limits<med_float>::quiet_NaN();
        mXYZ[2] = std::numeric_limits<med_float>::quiet_NaN();
        mVal    = std::numeric_limits<med_float>::quiet_NaN();
    }

    /**
     * Dumps any PointOfField to the given output stream.
     * \param  pOs any output stream.
     * \param  pP  any PointOfField.
     * \return the output stream pOs.
     */
    friend std::ostream& operator<<(std::ostream& pOs, PointOfField& pP)
    {
        pOs << "[ " << pP.mXYZ[0] << " ; " << pP.mXYZ[1] << " ; " << pP.mXYZ[2] << "]: " << pP.mVal;
        return pOs;
    }

public:

    med_float  mXYZ[3];    /**< 3D-position. */
    med_float  mVal;       /**< Value of the field at this point. */
    
}; // class PointOfField


} // namespace multipr


#endif // MULTIPR_FIELDPOINT_HXX 

// EOF
