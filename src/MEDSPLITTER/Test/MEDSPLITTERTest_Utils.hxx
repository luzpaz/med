// Copyright (C) 2007-2012  CEA/DEN, EDF R&D
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
// File      : MEDSPLITTERTest_TmpFilesRemover.hxx
// Created   : Mon Jul  6 14:08:12 2009
// Author    : Edward AGAPOV (eap)

#ifndef __MEDSPLITTERTest_UTILS_HXX__
#define __MEDSPLITTERTest_UTILS_HXX__

#include <set>
#include <string>
#include <ios>
#include <cstdlib>

// to automatically remove temporary files from disk
class MEDSPLITTERTest_TmpFilesRemover
{
public:

  MEDSPLITTERTest_TmpFilesRemover() {}
  bool Register(const std::string theTmpFile, const bool and_remove=true);
  ~MEDSPLITTERTest_TmpFilesRemover();

private:
  std::set<std::string> myTmpFiles;
};

namespace MEDSPLITTERTest_Utils
{
  std::string getResourceFile( const std::string& );
  std::string getTmpDirectory();
  std::string makeTmpFile( const std::string&, const std::string& = "" );
}

#endif
