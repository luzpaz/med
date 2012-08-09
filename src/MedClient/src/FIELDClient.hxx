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

#ifndef _FIELDCLIENT_HXX
#define _FIELDCLIENT_HXX

#include <SALOMEconfig.h>
#include <utilities.h>
#include "MEDMEM_Field.hxx"
#include "SUPPORTClient.hxx"
#include "ReceiverFactory.hxx"
#include "SenderFactory.hxx"
#include "MEDMEM_TraitsForFields.hxx"
#include "MEDMEM_InterlacingTraits.hxx"
#include CORBA_CLIENT_HEADER(MED)

namespace MEDMEM{

template<class T, class INTERLACING_TAG=FullInterlace>
class FIELDClient : public MEDMEM::FIELD<T,INTERLACING_TAG>
{
private:
  typename FIELDI_TRAITS<T,INTERLACING_TAG>::SimpleFieldCorbaPtrType _fieldPtr;
  int _refCounter;
public:
  FIELDClient(typename FIELDI_TRAITS<T,INTERLACING_TAG>::SimpleFieldCorbaPtrType ptrCorba,MEDMEM::SUPPORT * S = NULL);
  ~FIELDClient();
private:
  void fillCopy();
};

#include "FIELDClient.cxx"

}
#endif
