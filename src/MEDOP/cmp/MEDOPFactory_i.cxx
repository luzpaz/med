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

// Authors : Guillaume Boulant (EDF) - 01/06/2011

#include "MEDOPFactory_i.hxx"
#include "utilities.h"

#include <string>

#include "MEDDataManager_i.hxx"
#include "MEDCalculator_i.hxx"

using namespace std;

//=============================================================================
/*!
 *  standard constructor
 */
//=============================================================================

MEDOPFactory_i::MEDOPFactory_i(CORBA::ORB_ptr orb,
  PortableServer::POA_ptr poa,
  PortableServer::ObjectId * contId,
  const char *instanceName,
  const char *interfaceName) :
  Engines_Component_i(orb, poa, contId, instanceName, interfaceName)
{
  MESSAGE("activate object");
  _thisObj = this ;
  _id = _poa->activate_object(_thisObj);
}

MEDOPFactory_i::~MEDOPFactory_i()
{
}

MEDOP::MEDDataManager_ptr MEDOPFactory_i::getDataManager() {
  MEDDataManager_i * medDataManager = MEDDataManager_i::getInstance();
  MEDOP::MEDDataManager_ptr medDataManagerPtr = medDataManager->_this();
  return medDataManagerPtr;
}

MEDOP::MEDCalculator_ptr MEDOPFactory_i::getCalculator() {
  MEDCalculator_i * medCalculator = MEDCalculator_i::getInstance();
  MEDOP::MEDCalculator_ptr medCalculatorPtr = medCalculator->_this();
  return medCalculatorPtr;
}

extern "C"
{
  PortableServer::ObjectId * MEDOPFactoryEngine_factory(
             CORBA::ORB_ptr orb,
             PortableServer::POA_ptr poa,
             PortableServer::ObjectId * contId,
             const char *instanceName,
                    const char *interfaceName)
  {
    MESSAGE("PortableServer::ObjectId * MEDOPEngine_factory()");
    SCRUTE(interfaceName);
    MEDOPFactory_i * factory = new MEDOPFactory_i(orb, poa, contId,
              instanceName,
              interfaceName);
    return factory->getId() ;
  }
}
