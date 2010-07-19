//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
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

#ifndef __MEDCOUPLINGPOINTSETSERVANT_HXX__
#define __MEDCOUPLINGPOINTSETSERVANT_HXX__

#include "SALOMEconfig.h"

#include CORBA_SERVER_HEADER(MEDCouplingCorbaServant)
#include "MEDCouplingMeshServant.hxx"
#include "MEDCouplingCorba.hxx"

#include <vector>

namespace ParaMEDMEM
{
  class MEDCouplingPointSet;
  class DataArrayInt;
  class DataArrayDouble;
  
  class MEDCOUPLINGCORBA_EXPORT MEDCouplingPointSetServant : public MEDCouplingMeshServant, public virtual POA_SALOME_MED::MEDCouplingPointSetCorbaInterface
  {
  protected:
    MEDCouplingPointSetServant(const MEDCouplingPointSet *cppPointerOfMesh);
    ~MEDCouplingPointSetServant();
  private:
    const MEDCouplingPointSet *getPointer() const { return (const MEDCouplingPointSet *)(_cpp_pointer); }
  protected:
    void getTinyInfo(SALOME_TYPES::ListOfLong_out la, SALOME_TYPES::ListOfString_out sa);
    void getSerialisationData(SALOME_TYPES::ListOfLong_out la, SALOME_TYPES::ListOfDouble_out da);
  };
}

#endif
