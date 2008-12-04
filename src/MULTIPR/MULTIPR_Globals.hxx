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
 * \file    MULTIPR_Globals.hxx
 *
 * \brief   Some useful macros.
 *
 * \author  Olivier LE ROUX - CS, Virtual Reality Dpt
 * 
 * \date    01/2007
 */

  
#ifndef MULTIPR_GLOBALS_HXX
#define MULTIPR_GLOBALS_HXX

#include <vector>
#include <set>

//*****************************************************************************
// Some useful macros
//*****************************************************************************

#define ENABLE_LOG 0
#define MULTIPR_LOG(STR) if (ENABLE_LOG) cout << STR;

/**
 * Pair of index.
 * See definition of GaussIndexList.
 */
typedef std::pair < std::set<int>*, std::set<int> > IndexPair;

/**
 * List of gauss index for optimized domain split.
 * The first element of the pair is the element index vector (size is 
 * eMaxMedMesh) and the second the node index.
 */
typedef std::vector< IndexPair > GaussIndexList;

#endif // MULTIPR_GLOBALS_HXX


// EOF
