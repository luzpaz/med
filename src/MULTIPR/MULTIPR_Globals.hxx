// Project MULTIPR, IOLS WP1.2.1 - EDF/CS
// Partitioning/decimation module for the SALOME v3.2 platform

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
