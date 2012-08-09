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

//  File   : readEntete.cxx
//  Module : MED
//
#include<string>
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_DriversDef.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

static void usage(char * name)
{
  cout << "    " << name <<" <file name>"<< " <mesh name> " << " <interlace mode>" << endl;
  cout << "    " << "displays all Nodes Coordinates in mdump mode" << endl;
  cout << endl;
  cout << "    " << "mesh name is mandatory. Try mdump if necessary" << endl;
  cout << "    " << "values for interlace mode are  : " << endl;
  cout << "    " << "MED_FULL_INTERLACE (default value) or MED_NO_INTERLACE" << endl;
  exit(-1);
}

int main (int argc, char ** argv) {
  
  if (argc < 3) usage(argv[0]);
  if (argc > 4) usage(argv[0]);

  string fileName = argv[1];
  string meshName = argv[2];

  medModeSwitch Mode = MED_FULL_INTERLACE;
  if (argc==4)
  {
        string comp=argv[3];
        if ( comp == "MED_NO_INTERLACE" ) Mode = MED_NO_INTERLACE;
        else if ( comp != "MED_FULL_INTERLACE") usage(argv[0]);
  }


  MESH * myMesh= new MESH;
  myMesh->setName(meshName);
  MED_MESH_RDONLY_DRIVER myMeshDriver(fileName,myMesh);
  try
  {
       myMeshDriver.setMeshName(meshName);
       myMeshDriver.open();
  }
  catch (const exception & ex)
  {
         MESSAGE_MED("Catch Exception : ");
         SCRUTE_MED(ex.what());
  };
  MESSAGE_MED("Open done");

  try
  {
       myMeshDriver.read();
  }
  catch (const exception & ex)
  {
         MESSAGE_MED("Catch Exception : ");
         SCRUTE_MED(ex.what());
  };
  MESSAGE_MED("Read done");
  
  int SpaceDimension = myMesh->getSpaceDimension();
  int MeshDimension  = myMesh->getMeshDimension();
  int NumberOfNodes  = myMesh->getNumberOfNodes();

  cout << "(****************************)" << endl;
  cout << "(* INFORMATIONS GENERALES : *)" << endl;
  cout << "(****************************)" << endl;

  //  string chainevide(MED_TAILLE_PNOM+2,' ');
  cout << "- Nom du maillage : <<"    << meshName       << ">>" << endl;
  cout << "- Dimension d'espace : "<< SpaceDimension <<  endl;
  cout << "- Dimension du maillage : "<< MeshDimension <<  endl;
  cout << "- Pas de nom universel "   << endl ;
  cout << "- Nombre de noeuds : "     << NumberOfNodes  << " " << endl;

  int NumberOfTypes           = myMesh->getNumberOfTypes(MED_CELL);
  const medGeometryElement  * Types;

  cout << "- Nombre de Type de mailles : " << NumberOfTypes << endl;

  if (NumberOfTypes > 0)
    {
      Types = myMesh->getTypes(MED_CELL);

      cout << "  Types : ";
      for (int itype=0; itype<NumberOfTypes; itype++) cout << Types[itype] << " ";
      cout << endl;

      const list<medGeometryElement> currentEntity = meshEntities[MED_CELL];
      list<medGeometryElement>::const_iterator currentGeometry;

      for (currentGeometry  = currentEntity.begin();
           currentGeometry != currentEntity.end(); 
           currentGeometry++) 
        {
          cout << "- Nombre de mailles de type "; 
          cout << geoNames[(*currentGeometry)] << " : ";
          cout << myMesh->getNumberOfElements(MED_CELL,(MED_EN::medGeometryElement)(*currentGeometry));
          cout << " " << endl;
        }
    }

  NumberOfTypes = myMesh->getNumberOfTypes(MED_FACE);
  
  cout << "- Nombre de Type de faces : " << NumberOfTypes << endl;

  if (NumberOfTypes > 0)
    {
      Types = myMesh->getTypes(MED_FACE);

      cout << "  Types : ";
      for (int itype=0; itype<NumberOfTypes; itype++) cout << Types[itype] << " ";
      cout << endl;

      const list<medGeometryElement> currentEntity2 = meshEntities[MED_FACE];
      list<medGeometryElement>::const_iterator currentGeometry;

      for (currentGeometry  = currentEntity2.begin();
           currentGeometry != currentEntity2.end(); 
           currentGeometry++) 
        {
          cout << "- Nombre de faces de type "; 
          cout << geoNames[(*currentGeometry)] << " : ";
          cout << myMesh->getNumberOfElements(MED_FACE,(*currentGeometry));
          cout << " " << endl;
        }
    }

  NumberOfTypes = myMesh->getNumberOfTypes(MED_EDGE);
  
  cout << "- Nombre de Type de aretes : " << NumberOfTypes << endl;

  if (NumberOfTypes > 0)
    {
      Types = myMesh->getTypes(MED_EDGE);

      cout << "  Types : ";
      for (int itype=0; itype<NumberOfTypes; itype++) cout << Types[itype] << " ";
      cout << endl;

      const list<medGeometryElement> currentEntity3 = meshEntities[MED_EDGE];
      list<medGeometryElement>::const_iterator currentGeometry;

      for (currentGeometry  = currentEntity3.begin();
           currentGeometry != currentEntity3.end(); 
           currentGeometry++) 
        {
          cout << "- Nombre d'aretes de type "; 
          cout << geoNames[(*currentGeometry)] << " : ";
          cout << myMesh->getNumberOfElements(MED_EDGE,(*currentGeometry));
          cout << " " << endl;
        }
    }

  myMesh->removeReference();
}
