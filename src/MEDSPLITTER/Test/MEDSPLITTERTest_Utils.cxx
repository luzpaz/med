//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File      : MEDSPLITTERTest_TmpFilesRemover.hxx
// Created   : Mon Jul  6 14:08:12 2009
// Author    : Edward AGAPOV (eap)

#include "MEDSPLITTERTest_Utils.hxx"

using namespace std;

// to automatically remove temporary files from disk
bool MEDSPLITTERTest_TmpFilesRemover::Register(const string theTmpFile, const bool and_remove)
{
//   if ( and_remove && access( theTmpFile.data(), F_OK ) == 0 )
//     remove(theTmpFile.data());
  return (myTmpFiles.insert(theTmpFile)).second;
}

MEDSPLITTERTest_TmpFilesRemover::~MEDSPLITTERTest_TmpFilesRemover()
{
  set< string>::iterator it = myTmpFiles.begin();
  for (; it != myTmpFiles.end(); it++) {
    if (access((*it).data(), F_OK) == 0)
      remove((*it).data());
  }
  myTmpFiles.clear();
}

#include <MEDMEM_STRING.hxx>

#include <cppunit/TestAssert.h>
#include <list>
#include <cstdlib>

string getResourceFile( const string& filename )
{
  string resourceFile = "";

  if ( getenv("top_srcdir") ) {
    // we are in 'make check' step
    resourceFile = getenv("top_srcdir");
    resourceFile += "/resources/";
  }
  else if ( getenv("MED_ROOT_DIR") ) {
    // use MED_ROOT_DIR env.var
    resourceFile = getenv("MED_ROOT_DIR");
    resourceFile += "/share/salome/resources/med/";
  }
  resourceFile += filename;
  if (access(resourceFile.data(), F_OK) != 0)
    CPPUNIT_FAIL(MEDMEM::STRING("Resource file ")<< filename << " not found");
  
  return resourceFile;
}


//================================================================================
/*!
 * \brief Returns writable temporary directory
 */
//================================================================================

string getTmpDirectory()
{
  string path;

  list<string> dirs;
  if ( getenv("TMP") ) dirs.push_back( getenv("TMP" ));
  if ( getenv("TMPDIR") ) dirs.push_back( getenv("TMPDIR" ));
  dirs.push_back( "/tmp" );

  for ( list<string>::iterator dir = dirs.begin(); dir != dirs.end(); ++dir )
    if ( access( dir->data(), W_OK ) == 0 )
      return *dir;

  CPPUNIT_FAIL("Can't find writable temporary directory. Set TMP environment variable correctly");
}
