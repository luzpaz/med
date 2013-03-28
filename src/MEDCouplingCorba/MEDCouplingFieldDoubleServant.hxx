// Copyright (C) 2007-2013  CEA/DEN, EDF R&D
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
// Author : Anthony Geay (CEA/DEN)

#ifndef __MEDCOUPLINGFIELDDOUBLESERVANT_HXX__
#define __MEDCOUPLINGFIELDDOUBLESERVANT_HXX__

#include "SALOMEconfig.h"
#ifdef WIN32
#define NOMINMAX
#endif
#include CORBA_SERVER_HEADER(MEDCouplingCorbaServant)
#include "MEDCouplingCorba.hxx"
#include "MEDCouplingFieldServant.hxx"
#include "MEDCouplingMeshServant.hxx"

#include <vector>

namespace ParaMEDMEM
{
  class RefCountObject;
  class MEDCouplingFieldDouble;
  class DataArrayInt;
  class DataArrayDouble;

  class MEDCOUPLINGCORBA_EXPORT MEDCouplingFieldDoubleServant : public MEDCouplingFieldServant, public virtual POA_SALOME_MED::MEDCouplingFieldDoubleCorbaInterface
  {
  public:
    typedef MEDCouplingFieldDouble CppType;
    MEDCouplingFieldDoubleServant(const MEDCouplingFieldDouble *cppPointerOfMesh);
  protected:
    const MEDCouplingFieldDouble *getPointer() const { return (const MEDCouplingFieldDouble *)(_cpp_pointer); }
  protected:
    char *getName();
    SALOME_TYPES::ListOfString *getInfoOnComponents();
    void getTinyInfo(SALOME_TYPES::ListOfLong_out la, SALOME_TYPES::ListOfDouble_out da, SALOME_TYPES::ListOfString_out sa);
    void getSerialisationData(SALOME_TYPES::ListOfLong_out la, SALOME_TYPES::ListOfDouble2_out da2);
    CORBA::Boolean ExportDataAs(const char *format, SALOME::GenericObj_out exporter);
  };
}

#endif
