//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : testUGeoNameMeshEntities.cxx
//  Module : MED

using namespace std;
#include "MEDMEM_DriversDef.hxx"

void usage(char * name)
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
  MED_FR::GEO_NAME::const_iterator currentGeom;
  for (currentGeom  = MED_FR::geoNames.begin();
       currentGeom != MED_FR::geoNames.end(); 
       currentGeom++) 
  {
		cout << (*currentGeom).second << endl;
  };

  MED_FR::MESH_ENTITIES::const_iterator currentEntity;
  list<MED_FR::med_geometrie_element>::const_iterator currentGeometry;

  cout << endl; 
  cout << "MESH_ENTITIES Test" << endl ;
  cout << "------------------" << endl << endl;
  for (currentEntity  = MED_FR::meshEntities.begin();
       currentEntity != MED_FR::meshEntities.end(); 
       currentEntity++) 
  {
	cout << (*((MED_FR::entNames).find((*currentEntity).first))).second <<endl;
  	for (currentGeometry  = (*currentEntity).second.begin();
       	     currentGeometry != (*currentEntity).second.end(); 
      	     currentGeometry++) 
	{
	cout << MED_FR::geoNames[(MED_FR::med_geometrie_element)(*currentGeometry)] << endl;
	}
	cout << endl;
  }

}
