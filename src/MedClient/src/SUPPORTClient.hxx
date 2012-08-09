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

#ifndef _SUPPORTCLIENT_HXX
#define _SUPPORTCLIENT_HXX

#include <SALOMEconfig.h>
#include <utilities.h>
#include "MEDClient.hxx"
#include "MEDMEM_Support.hxx"
#include CORBA_CLIENT_HEADER(MED)


namespace MEDMEM {
class MEDCLIENT_EXPORT SUPPORTClient : virtual public SUPPORT {

private :

  const SALOME_MED::SUPPORT_var IOR_Support;

  mutable bool _complete_support;

//  mutable int _refCounter;

protected:

  virtual ~SUPPORTClient();

public :

  SUPPORTClient(const SALOME_MED::SUPPORT_ptr S, 
                GMESH * M = NULL);

  void blankCopy();
  void fillCopy();

  MEDSKYLINEARRAY *  getnumber()
    const throw (MEDEXCEPTION);
  const int *  getNumber(MED_EN::medGeometryElement GeometricType) 
    const throw (MEDEXCEPTION);
  const int *  getNumberIndex()
    const throw (MEDEXCEPTION);
  int getValIndFromGlobalNumber(const int number) 
    const throw (MEDEXCEPTION);
//  void addReference() const;
//  void removeReference() const;
};
}


#endif

