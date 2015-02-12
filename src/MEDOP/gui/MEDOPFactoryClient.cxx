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

#include "MEDOPFactoryClient.hxx"
#include "SALOME_KernelServices.hxx"

namespace MEDOPFactoryClient {

  /*!
   * This returns a singleton (static) instance of the XMED engine.
   */
  MEDOP::MEDOPFactory_ptr getFactory() {
    static MEDOP::MEDOPFactory_ptr engine;
    if(CORBA::is_nil(engine)){
      Engines::EngineComponent_var component =
  KERNEL::getLifeCycleCORBA()->FindOrLoad_Component( "FactoryServer","MEDOPFactory" );
      engine = MEDOP::MEDOPFactory::_narrow(component);
    }
    return engine;
  }

  /*!
   * This returns a singleton (static) instance of the XMED data manager.
   */
  MEDOP::MEDDataManager_ptr getDataManager() {
    static MEDOP::MEDDataManager_ptr dataManager;
    if(CORBA::is_nil(dataManager)){
      dataManager = getFactory()->getDataManager();
    }
    return dataManager;
  }

  /*!
   * This returns a singleton (static) instance of the XMED calculator.
   */
  MEDOP::MEDCalculator_ptr getCalculator() {
    static MEDOP::MEDCalculator_ptr calculator;
    if(CORBA::is_nil(calculator)){
      calculator = getFactory()->getCalculator();
    }
    return calculator;
  }
}
