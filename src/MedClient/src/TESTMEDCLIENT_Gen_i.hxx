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

#ifndef _TESTMEDCLIENT_GEN_I_HXX_
#define _TESTMEDCLIENT_GEN_I_HXX_

#include <TESTMEDCLIENT_Gen.hh>
#include <iostream>
#include "SALOME_Component_i.hxx"

class TESTMEDCLIENT_Gen_i :
  public virtual POA_SALOME_TESTMEDCLIENT::TESTMEDCLIENT_Gen,
  public virtual Engines_Component_i 
{
public:
  TESTMEDCLIENT_Gen_i(CORBA::ORB_ptr orb,
            PortableServer::POA_ptr poa,
            PortableServer::ObjectId * contId, 
            const char *instanceName, 
            const char *interfaceName);
  virtual ~TESTMEDCLIENT_Gen_i();

  void go(SALOME_MED::MED_ptr objMed);
  void go2(SALOME_MED::MED_ptr objMed);
};

#endif
