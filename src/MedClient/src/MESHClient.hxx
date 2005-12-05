// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
#ifndef _MESHCLIENT_HXX
#define _MESHCLIENT_HXX

#include <SALOMEconfig.h>
#include <utilities.h>
#include "MEDMEM_Mesh.hxx"
#include CORBA_CLIENT_HEADER(MED)

namespace MEDMEM {
class MESHClient : public MESH {

private :

  const SALOME_MED::MESH_var IOR_Mesh;

  mutable bool _complete;
  
  mutable int _refCounter;

public :

  MESHClient(const SALOME_MED::MESH_ptr m);
  virtual bool operator==(const MESH& other) const;

  void blankCopy();
  void fillCopy();
  virtual void addReference() const;
  virtual void removeReference() const;

private:
  virtual ~MESHClient();
};
}

#endif

