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

//  File   : testUGeoNameMeshEntities.cxx
//  Module : MED
//
#include "MEDMEM_DriversDef.hxx"

using namespace std;
static void usage(char * name)
{
  cout << "    " << name << endl;
  cout << "    " << "displays all avalaible geometry elements" << endl;
  cout << "    " << "display then all avalaible entities, and for each" << endl;
  cout << "    " << "all possible geomety element" << endl;
  cout << endl;
  exit(-1);
}

int main (int argc, char ** argv) 
{
  
  if (argc < 1) usage(argv[0]);


  cout << "GEO_NAME Test" << endl ;
  cout << "-------------" << endl << endl;
  MED_EN::GEO_NAME::const_iterator currentGeom;
  for (currentGeom  = MED_EN::geoNames.begin();
       currentGeom != MED_EN::geoNames.end(); 
       currentGeom++) 
  {
                cout << (*currentGeom).second << endl;
  };

  MED_EN::MESH_ENTITIES::const_iterator currentEntity;
  list<MED_EN::medGeometryElement>::const_iterator currentGeometry;

  cout << endl; 
  cout << "MESH_ENTITIES Test" << endl ;
  cout << "------------------" << endl << endl;
  for (currentEntity  = MED_EN::meshEntities.begin();
       currentEntity != MED_EN::meshEntities.end(); 
       currentEntity++) 
  {
        cout << (*((MED_EN::entNames).find((*currentEntity).first))).second <<endl;
        for (currentGeometry  = (*currentEntity).second.begin();
             currentGeometry != (*currentEntity).second.end(); 
             currentGeometry++) 
        {
        cout << MED_EN::geoNames[(MED_EN::medGeometryElement)(*currentGeometry)] << endl;
        }
        cout << endl;
  }

}
