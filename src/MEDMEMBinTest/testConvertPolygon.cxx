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

#include "MEDMEM_Mesh.hxx"
#ifdef WIN32
#include <io.h>
#define access _access
#define W_OK 02
#else
#include <unistd.h>
#endif

int main()
{
  // Get file to convert.
  string file;
  // During 'make check' $srcdir exists
  if ( getenv("srcdir") ) {
    file = string( getenv("srcdir")) + "/../../resources/";
  }
  else if ( getenv( "MED_ROOT_DIR")) {
    // after installation
    file = string( getenv( "MED_ROOT_DIR" )) + "/share/salome/resources/med/";
  }
  else
  {
    return -1;
  }
  file += "BoxTetra2.med";
  cout << "File to cnvert: " << file << endl;
  // convertion
  MEDMEM::MESH *mesh=new MEDMEM::MESH(MEDMEM::MED_DRIVER, file.c_str(), "BoxTetra2");
  mesh->convertToPoly();

  // File to store conversion result
  if ( getenv("TMP") && access(getenv("TMP"),W_OK)==0 )
    file = getenv("TMP");
  else if ( getenv("TMPDIR") && access(getenv("TMPDIR"),W_OK)==0 )
    file = getenv("TMPDIR");
  else
    file = "/tmp";
  file += "/pointe_testConvertPolygon.med";

  int id=mesh->addDriver(MEDMEM::MED_DRIVER,file.c_str(),"mesh");
  mesh->write(id);
  mesh->removeReference();

  remove(file.c_str());

  return 0;
}
