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

#include "MEDMEMTest_Utils.hxx"
#include <cstdlib>
#include <list>
#include <stdexcept>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

//================================================================================
/*!
 * \brief Get path to the resources file.
 *
 * When running 'make test' source file is taken from MED_SRC/resources folder.
 * Otherwise, file is searched in ${MED_ROOT_DIR}/share/salome/resources/med folder.
 * 
 * \param filename name of the resource file (should not include a path)
 * \return full path to the resource file
 */
//================================================================================

std::string getResourceFile( const std::string& filename )
{
  std::string resourceFile = "";

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
  return fixSlashes( resourceFile );
}

//================================================================================
/*!
 * \brief Returns writable temporary directory
 * \return full path to the temporary directory
 */
//================================================================================

std::string getTmpDirectory()
{
  std::string path;

  std::list<std::string> dirs;
  if ( getenv("TEMP") )   dirs.push_back( getenv("TEMP" ));
  if ( getenv("TMP") )    dirs.push_back( getenv("TMP" ));
  if ( getenv("TMPDIR") ) dirs.push_back( getenv("TMPDIR" ));
  dirs.push_back( "/tmp" );

  std::string tmpd = "";
  for ( std::list<std::string>::iterator dir = dirs.begin(); dir != dirs.end() && tmpd == "" ; ++dir ) {
#ifdef WIN32
    if ( GetFileAttributes(dir->data()) & FILE_ATTRIBUTE_DIRECTORY )
#else
    if ( access( dir->data(), W_OK ) == 0 )
#endif
      tmpd = dir->data();
  }

  if ( tmpd == "" )
    throw std::runtime_error("Can't find writable temporary directory. Set TMP environment variable");

  return tmpd;
}

//================================================================================
/*!
 * \brief Creates a copy of source file (if source file is specified) 
 * in the temporary directory and returns a path to the tmp file
 *
 * \param tmpfile name of the temporary file (without path)
 * \param srcfile source file
 * \return path to the temporary file
 */
//================================================================================

std::string makeTmpFile( const std::string& tmpfile, const std::string& srcfile )
{
  std::string tmpf = fixSlashes( getTmpDirectory() + "/" + tmpfile );
  if ( srcfile != "" ) {
#ifdef WIN32
    std::string cmd  = "copy " + fixSlashes( srcfile ) + " " + tmpf;
#else
    std::string cmd  = "cp " + srcfile + " " + tmpf + " ; chmod +w " + tmpf;
#endif
    system( cmd.c_str() );
  }
  return tmpf;
}

//================================================================================
/*!
 * \brief On WINDOWS, replaces all forward slashes by back slashes
 * to avoid mixed slashes in one path
 */
//================================================================================

std::string fixSlashes( const std::string& path)
{
#ifdef WIN32
  std::string fixedpath = path;
  for ( int i=0; i < path.size(); ++i )
    if (path[i] == '/')
      fixedpath[i] = '\\';
  return fixedpath;
#else
  return path;
#endif
}
