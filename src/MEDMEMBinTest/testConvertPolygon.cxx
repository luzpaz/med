//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
#include "MEDMEM_Mesh.hxx"

int main()
{
  //  MEDMEM::MESH mesh(MEDMEM::MED_DRIVER, "/export/home/vb144235/MED_INTEGRATION/MED_INSTALL/share/salome/resources/med/poly3D.med","poly3D");
  MEDMEM::MESH mesh(MEDMEM::MED_DRIVER, "/export/home/bergeaud/Extruded_hexagon_fluidmesh.med","FluidMesh_1");
  mesh.convertToPoly();
  int id=mesh.addDriver(MEDMEM::MED_DRIVER,"/export/home/bergeaud/Zxtruded_hexagon_fluidmesh_polyhedra.med","Fluidmesh_1");
  mesh.write(id);
}
