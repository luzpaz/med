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

#include<string>
#include<deque>

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Grid.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_EnsightMedDriver.hxx"
#include "MEDMEM_MedFileBrowser.hxx"
#include "MEDMEM_EnsightMeshDriver.hxx"

using namespace std;
using namespace MEDMEM;

static void usage(char * name)
{
  cout << " ERROR ABOUT SYNTAX " << endl ;
  cout << "  " << name << " <input med file> <output ensight file> " << endl ;
  exit(-1);
}

int main (int argc, char ** argv) {

  string filenameIN ;
  string filenameOUT;
  
  if ( argc == 3 ) {
    filenameIN  = argv[1] ;
    filenameOUT = argv[2] ;
    cout << "-> reading all into the Med file " << filenameIN << " and writing all into the Ensight file " << filenameOUT <<  endl ;

    MEDFILEBROWSER myMed(filenameIN) ;

    vector< FIELD_* > fields;
    vector< GMESH* > meshes;

    cout << "-> Read all meshes "  ;
    int NumberOfMeshes = myMed.getNumberOfMeshes() ;
    cout << "( "<<NumberOfMeshes << " ) :" << endl ;
    vector<string> MeshName = myMed.getMeshNames() ;
    for (int i=0; i<NumberOfMeshes; i++) {
      GMESH* mesh = myMed.isStructuredMesh( MeshName[i] ) ? (GMESH*) new GRID : (GMESH*) new MESH;
      mesh->addDriver(MED_DRIVER, filenameIN, MeshName[i], MED_EN::RDONLY);
      mesh->read();
      cout << "-> Mesh "<<i+1<<", named "<<MeshName[i]<<" is read !" << endl;
      meshes.push_back( mesh );
    }

    cout << "-> Read all fields " ;
    int NumberOfFields = myMed.getNumberOfFields() ;
    cout << "( "<<NumberOfFields << " ) :" << endl;
    vector<string> FieldName = myMed.getFieldNames() ;
    for (int i=0; i<NumberOfFields; i++) {
      vector<DT_IT_> FieldIteration = myMed.getFieldIteration(FieldName[i]) ;
      cout << "-> Field "<<i+1<<", named "<<FieldName[i] << " :" << endl ;
      int NumberOfIteration = FieldIteration.size() ;
      cout << "    Number of iteration pair : "<< NumberOfIteration << endl;
      for (int j=0; j<NumberOfIteration; j++) {
        FIELD_ * myField = 0;
        switch( myMed.getFieldType( FieldName[i] ))
        {
        case MED_REEL64: myField = new FIELD<double>; break;
        case MED_INT32:  
        case MED_INT64:  myField = new FIELD<int>; break;
        default:
          cout << "Unknown value type - skipped" << endl;
          continue;
        }
        myField->setIterationNumber( FieldIteration[j].dt );
        myField->setOrderNumber( FieldIteration[j].it );
        myField->addDriver( MED_DRIVER, filenameIN, FieldName[i], MED_EN::RDONLY);
        myField->read() ;
        cout << "    * Iteration "<<FieldIteration[j].dt<<" and  order number "<<FieldIteration[j].it<<" is read !" << endl;
        fields.push_back( myField );
        // set mesh to myField
        string meshName = myField->getSupport()->getMeshName();
        for ( unsigned i = 0; i < meshes.size(); ++ i)
          if ( meshes[i]->getName() == meshName )
            myField->getSupport()->setMesh( meshes[i] );
      }
    }

    if ( !fields.empty() )
    {
      vector< const FIELD_* > constFields( fields.begin(), fields.end() );
      ENSIGHT_MED_WRONLY_DRIVER medDriver( filenameOUT, constFields );
      medDriver.write();
    }
    else if ( !meshes.empty() )
    {
      ENSIGHT_MESH_WRONLY_DRIVER meshDriver( filenameOUT, meshes[0] );
      meshDriver.write() ;
    }

    for ( unsigned i = 0; i < meshes.size(); ++ i)
      meshes[i]->removeReference();
    for ( unsigned i = 0; i < fields.size(); ++ i)
      fields[i]->removeReference();
  }
  else usage(argv[0]);
}
