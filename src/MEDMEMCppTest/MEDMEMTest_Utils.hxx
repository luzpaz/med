// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef _MEDMEMTEST_UTILS_HXX_
#define _MEDMEMTEST_UTILS_HXX_

#include "MEDMEMCppTestExport.hxx"

#include <string>

MEDMEMCPPTEST_EXPORT std::string getResourceFile( const std::string& );
MEDMEMCPPTEST_EXPORT std::string getTmpDirectory();
MEDMEMCPPTEST_EXPORT std::string makeTmpFile( const std::string&, const std::string& = "" );
MEDMEMCPPTEST_EXPORT std::string fixSlashes( const std::string& path);

#endif
