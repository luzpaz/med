// Copyright (C) 2011-2015  CEA/DEN, EDF R&D
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

#include "MEDPresentationManager_i.hxx"

MEDPresentationManager_i* MEDPresentationManager_i::_instance = NULL;

MEDPresentationManager_i*
MEDPresentationManager_i::getInstance() {
  if ( _instance == NULL )
    _instance = new MEDPresentationManager_i();
  return _instance;
}

MEDPresentationManager_i::MEDPresentationManager_i()
{
}

MEDPresentationManager_i::~MEDPresentationManager_i()
{
}

#include <iostream>

void
MEDPresentationManager_i::MakeScalarMap(const MEDOP::ScalarMapParameters& params)
{
  std::cout << "View mode: " << params.viewMode << std::endl;
  std::cout << "MEDPresentationManager_i::MakeScalarMap: Not implemented yet\n";
}
