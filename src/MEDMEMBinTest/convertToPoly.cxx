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
#include <iostream>

using namespace MED_EN;
using namespace MEDMEM;
using namespace std;

int main(int argc, char** argv)
{
    if( argc != 4 )
    {
      cout << "Usage: convertToPoly input_med_file output_med_file mesh_name" << endl;
      return -1;
    }
  else
    {
      string filename(argv[1]);
      string meshname(argv[3]);
      string outputfilename(argv[2]);
      
      MESH *mesh=new MESH(MED_DRIVER, filename, meshname);

      mesh->convertToPoly();

      int id = mesh->addDriver(MED_DRIVER,outputfilename,meshname);
      mesh->write(id);
      mesh->removeReference();
      return 0;
    }
}
