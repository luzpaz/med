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

// Author : Guillaume Boulant (EDF)

#ifndef _MEDFACTORY_CLIENT_HXX_
#define _MEDFACTORY_CLIENT_HXX_


#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(MEDFactory)
#include CORBA_CLIENT_HEADER(MEDDataManager)
#include CORBA_CLIENT_HEADER(MEDCalculator)

#include "MEDCALCGUI.hxx"

// This helper manages static variables binded on main objects of the
// XMED context, so that they could be initialized once and shared in
// the whole gui context. It prevents to defined an attribute in each
// class while this instance are singleton instances.

namespace MEDFactoryClient {
  MEDCALCGUI_EXPORT MEDCALC::MEDFactory_ptr getFactory();
  MEDCALCGUI_EXPORT MEDCALC::MEDDataManager_ptr getDataManager();
  MEDCALCGUI_EXPORT MEDCALC::MEDCalculator_ptr getCalculator();
}

#endif // _MEDFACTORY_CLIENT_HXX_
