//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File      : MEDMEM_GaussLocalization.cxx
// Created   : Thu Dec 20 12:26:33 2007
// Author    : Edward AGAPOV (eap)
//
#include "MEDMEM_GaussLocalization.hxx"

#include "MED_GaussDef.hxx"

#include <stdexcept>

using namespace MED;

//=======================================================================
/*!
 * Creates a localization filled with default values. The caller gets pointer ownership
 */
//=======================================================================

namespace MEDMEM {

  GAUSS_LOCALIZATION_*
  GAUSS_LOCALIZATION_::makeDefaultLocalization(const string &     locName,
                                               medGeometryElement typeGeo,
                                               int                nGauss) throw (MEDEXCEPTION)
  {
    TGaussDef gaussDef( typeGeo, nGauss, 1 );

    GAUSS_LOCALIZATION_ * gsloc = 0;
    try {
      gsloc = new GAUSS_LOCALIZATION<FullInterlace> ( locName.c_str(),
                                                      typeGeo,
                                                      nGauss,
                                                      &gaussDef.myRefCoords[0],
                                                      &gaussDef.myCoords[0],
                                                      &gaussDef.myWeights[0] );
    }
    catch ( const std::logic_error & e ) {
      throw MEDEXCEPTION( e.what() ) ;
    }

    return gsloc;
  }
}
