// Copyright (C) 2007-2015  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

#ifndef _MED_PRESENTATION_MANAGER_I_HXX_
#define _MED_PRESENTATION_MANAGER_I_HXX_

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MEDDataManager)
#include CORBA_SERVER_HEADER(MEDPresentationManager)
#include "SALOME_GenericObj_i.hh"

#include "MEDDataManager_i.hxx"
#include "MEDOP.hxx"

class MEDOP_EXPORT MEDPresentationManager_i: public POA_MEDOP::MEDPresentationManager,
                                             public SALOME::GenericObj_i
{
 public:

  static MEDPresentationManager_i* getInstance();

  void MakeScalarMap(CORBA::Long fieldHandlerId, const char*);

 private:

  MEDPresentationManager_i();
  virtual ~MEDPresentationManager_i();

 private :

  // The MEDPresentationManager is a singleton, whose instance can be obtained
  // using the getInstance static method.
  static MEDPresentationManager_i * _instance;

};

#endif // _MED_PRESENTATION_MANAGER_I_HXX_
