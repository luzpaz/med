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

#ifndef _MEDOPFactory_HXX_
#define _MEDOPFactory_HXX_

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MEDOPFactory)
#include CORBA_SERVER_HEADER(MEDDataManager)
#include CORBA_SERVER_HEADER(MEDCalculator)
#include "SALOME_Component_i.hxx"

#include "MEDOP.hxx"
class MEDOP_EXPORT MEDOPFactory_i: public POA_MEDOP::MEDOPFactory,
           public Engines_Component_i
{
  
public:
  MEDOPFactory_i(CORBA::ORB_ptr orb,
     PortableServer::POA_ptr poa,
     PortableServer::ObjectId * contId,
     const char *instanceName,
     const char *interfaceName);
  virtual ~MEDOPFactory_i();

  /*! Returns the singleton instance of the data manager */
  MEDOP::MEDDataManager_ptr getDataManager();
  /*! Returns the singleton instance of the calculator */
  MEDOP::MEDCalculator_ptr getCalculator();
  
};

extern "C"
MEDOP_EXPORT PortableServer::ObjectId * MEDOPFactoryEngine_factory(CORBA::ORB_ptr orb,
                 PortableServer::POA_ptr poa,
                 PortableServer::ObjectId * contId,
                 const char *instanceName,
                 const char *interfaceName);

#endif
