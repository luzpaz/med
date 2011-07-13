// Copyright (C) 2007-2011  CEA/DEN, EDF R&D
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

#ifndef __MEDCOUPLINGFIELDSERVANT_HXX__
#define __MEDCOUPLINGFIELDSERVANT_HXX__

#include "SALOMEconfig.h"
#ifdef WNT
#define NOMINMAX
#endif
#include CORBA_SERVER_HEADER(MEDCouplingCorbaServant)
#include "MEDCouplingCorba.hxx"
#include "MEDCouplingRefCountServant.hxx"

namespace ParaMEDMEM
{
  class MEDCouplingMesh;
  class MEDCouplingField;

  class MEDCOUPLINGCORBA_EXPORT MEDCouplingFieldServant : public MEDCouplingRefCountServant,
                                                          public virtual POA_SALOME_MED::MEDCouplingFieldCorbaInterface
  {
  protected:
    MEDCouplingFieldServant(const MEDCouplingField *cppPointerOfMesh);
    const MEDCouplingField *getPointer() const { return (const MEDCouplingField *)(_cpp_pointer); }
  public:
    static SALOME_MED::MEDCouplingMeshCorbaInterface_ptr BuildCorbaRefFromCppPointer(const MEDCouplingMesh *mesh);
  protected:
    SALOME_MED::MEDCouplingMeshCorbaInterface_ptr getMesh();
  };
}

#endif
