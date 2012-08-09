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

// File      : dumpInterpolation.cxx
// Created   : Mon Jan 11 16:46:10 2010
// Author    : Edward AGAPOV (eap)
//
#include <MEDMEM_MedFileBrowser.hxx>
#include <MEDMEM_Remapper.hxx>

using namespace MEDMEM;
using namespace std;


static string getMeshName(const char* file, int mesh_index)
{
  MEDFILEBROWSER med(file);
  if ( mesh_index >= med.getNumberOfMeshes() )
    {
      cout << "Invalid index of mesh, it must be less than " << med.getNumberOfMeshes() << endl;
      return "Invalid mesh index";
    }
  vector<string> mesh_names = med.getMeshNames();
  return mesh_names[mesh_index];
}

//================================================================================
/*!
 * \brief Perform interpolation of two meshes and dumps result. For format of output,
 * see comment to operator<<() in InterpKernelMatrix.hxx
 */
//================================================================================

int main(int argc, char** argv)
{
  if( argc < 3 )
    {
      cout << "Dumps result of interpolation of two meshes" << endl
           << "For format of output, see comment to operator<<() in InterpKernelMatrix.hxx" <<endl;
      cout << "Usage: "<<argv[0]<<" med_file1 med_file2 [mesh_index1=0 mesh_index2=0]" << endl;
      return -1;
    }
  string mesh_name1 = getMeshName( argv[1], (argc > 3) ? atoi(argv[3]) : 0 );
  string mesh_name2 = getMeshName( argv[2], (argc > 4) ? atoi(argv[4]) : 0 );

  MESH *mesh1=new MESH(MED_DRIVER, argv[1], mesh_name1.data());
  MESH *mesh2=new MESH(MED_DRIVER, argv[2], mesh_name2.data());

  MEDMEM_REMAPPER aREMAPPER;
  aREMAPPER.prepare( *mesh1, *mesh2, "P0P0" );
  aREMAPPER.printMatrixInfo();
  mesh1->removeReference();
  mesh2->removeReference();
  return 0;
}
