
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
