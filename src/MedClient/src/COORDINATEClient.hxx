// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef _COORDINATE_CLIENT_HXX
#define _COORDINATE_CLIENT_HXX

#include <SALOMEconfig.h>
#include "MEDMEM_Coordinate.hxx"
#include CORBA_CLIENT_HEADER(MED)

namespace MEDMEM {
class COORDINATEClient : public COORDINATE {

protected:

  mutable bool _complete;
  SALOME_MED::MESH_var IOR_Mesh ;

public:

  /*!
  COORDINATEClient constructor
  
  Build an "empty" coordinate object (without the actual coordinate values).
  The object hold a Corba IOR and get data from it on user demand.

  Parameters in : m (Mesh Corba IOR)
                  mode (FULL_ or NO_ interlace)
 */
  COORDINATEClient(const SALOME_MED::MESH_ptr m, MED_EN::medModeSwitch Mode);


  /*!
  COORDINATEClient::blankCopy()
  
  "(Re)Blank" the coordinate object.
  
  Update fixed sized data from the Corba IOR (dimensions, strings).
  Get rid of variable sized data (coordinate values).
  Next call of getCoordinates (for instance) will update the
  object.
  */
  void blankCopy();


  /*!
  COORDINATEClient::fillCopy()
  
  (Re)fill the coordinate object from the Corba IOR 
  */
  void fillCopy();


  virtual ~COORDINATEClient() {};

  virtual const double *  getCoordinates(MED_EN::medModeSwitch Mode);
  virtual double          getCoordinate(int Number,int Axis);
  virtual const double *  getCoordinateAxis(int Axis);
  virtual const int*      getNodesNumbers() const;

};
};

#endif
