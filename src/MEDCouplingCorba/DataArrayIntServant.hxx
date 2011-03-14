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

#ifndef __DATAARRAYINTSERVANT_HXX__
#define __DATAARRAYINTSERVANT_HXX__

#include "SALOMEconfig.h"
#ifdef WNT
#define NOMINMAX
#endif
#include CORBA_SERVER_HEADER(MEDCouplingCorbaServant)
#include "MEDCouplingCorba.hxx"
#include "MEDCouplingRefCountServant.hxx"

namespace ParaMEDMEM
{
  class RefCountObject;
  class DataArrayInt;

  class MEDCOUPLINGCORBA_EXPORT DataArrayIntServant : public MEDCouplingRefCountServant , public virtual POA_SALOME_MED::DataArrayIntCorbaInterface
  {
  public:
    typedef DataArrayInt CppType;
    DataArrayIntServant(const DataArrayInt *cppPointer);
  protected:
    const DataArrayInt *getPointer() const { return (const DataArrayInt *)(_cpp_pointer); }
    SALOME::StringSeq *GetExportableFormats();
    CORBA::Boolean ExportDataAs(const char *format, SALOME::GenericObj_out exporter);
  public:
    void getTinyInfo(SALOME_TYPES::ListOfLong_out la, SALOME_TYPES::ListOfString_out sa);
    void getSerialisationData(SALOME_TYPES::ListOfLong_out da);
  };
}

#endif
