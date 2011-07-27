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

#ifndef __DATAARRAYDOUBLESERVANT_HXX__
#define __DATAARRAYDOUBLESERVANT_HXX__

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
  class DataArrayDouble;

  class MEDCOUPLINGCORBA_EXPORT DataArrayDoubleServant : public MEDCouplingRefCountServant,
                                                         public virtual POA_SALOME_MED::DataArrayDoubleCorbaInterface
  {
  public:
    typedef DataArrayDouble CppType;
    DataArrayDoubleServant(const DataArrayDouble *cppPointer);
  protected:
    const DataArrayDouble *getPointer() const { return (const DataArrayDouble *)(_cpp_pointer); }
  public:
    void getTinyInfo(SALOME_TYPES::ListOfLong_out la, SALOME_TYPES::ListOfString_out sa);
    void getSerialisationData(SALOME_TYPES::ListOfDouble_out da);
  };
}

#endif
