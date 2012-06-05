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

#ifndef _FAMILYCLIENT_HXX
#define _FAMILYCLIENT_HXX

#include <SALOMEconfig.h>
#include <utilities.h>
#include "MEDMEM_Family.hxx"
#include CORBA_CLIENT_HEADER(MED)
#include "SUPPORTClient.hxx"


namespace MEDMEM {
class FAMILYClient : 
  public SUPPORTClient, public FAMILY {

private :

  const SALOME_MED::FAMILY_var IOR_Family;

  mutable bool _complete;

public :

  FAMILYClient(const SALOME_MED::FAMILY_ptr S, 
               GMESH * M = NULL);
  virtual ~FAMILYClient();

  virtual void blankCopy(bool blankSupport = true);
  virtual void fillCopy(bool fillSupport = true);
};

};

#endif
