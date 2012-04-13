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
#include<vector>

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_EnsightMedDriver.hxx"
#include "MEDMEM_EnsightFieldDriver.hxx"
#include "MEDMEM_EnsightMeshDriver.hxx"

using namespace std;
using namespace MEDMEM;

static void usage(char * name)
{
  cout << " ERROR ABOUT SYNTAX " << endl ;
  cout << "  " << name << " <input ensight file> <output med file> [I - to read into MED_INT32 fields]"
       << endl ;
  exit(-1);
}

// ensight2med toto.case toto.med => input : toto.case => output : toto.med....

int main (int argc, char ** argv) {

  string filenameIN ;
  string filenameOUT;
 
  if ( argc == 3 ) {
    filenameIN  = argv[1] ;
    filenameOUT = argv[2] ;
    cout << " reading all into the Ensight file " << filenameIN << " and writing all into the Med file " << filenameOUT <<  endl ;

    vector< FIELD_* > fields;
    vector< const GMESH* > meshes;

    // Read

    ENSIGHT_MED_RDONLY_DRIVER medDriver(filenameIN, fields);
    medDriver.open();
    medDriver.read();
    medDriver.close();
    if ( !fields.empty() )
    {
      set<const GMESH*> uniqueMeshes;
      for ( unsigned i = 0; i < fields.size(); ++i )
        uniqueMeshes.insert( fields[i]->getSupport()->getMesh() );
      meshes.assign( uniqueMeshes.begin(), uniqueMeshes.end() );
    }
    else
    {
      // no fields but only meshes in the file
      MESH* mesh = 0;
      int meshIndex = 1;
      do
      {
        mesh = new MESH;
        meshes.push_back( mesh );
        ENSIGHT_MESH_RDONLY_DRIVER meshDriver( filenameIN, mesh, meshIndex++ );
        meshDriver.open();
        meshDriver.read();
        meshDriver.close();
      } while ( mesh->getNumberOfNodes() > 0 );
      meshes.back()->removeReference();
      meshes.pop_back();
    }

    // Write

    for ( unsigned i = 0; i < meshes.size(); ++i )
    {
      meshes[i]->write(MED_DRIVER,filenameOUT);
    }
    for ( unsigned i = 0; i < fields.size(); ++i )
    {
      fields[i]->write(MED_DRIVER,filenameOUT);
      fields[i]->removeReference();
    }
    for ( unsigned i = 0; i < meshes.size(); ++i )
      meshes[i]->removeReference();
  }
  else if ( argc == 4 && strncmp(argv[3], "I", 1 )==0 )
  {
    // we read all variables into INT32 fields
    // (we need such fields for test_operation_fieldint)

    filenameIN  = argv[1] ;
    filenameOUT = argv[2] ;

    vector< FIELD_* > fields;

    // Read

    ENSIGHT_MED_RDONLY_DRIVER medDriver(filenameIN, fields);
    medDriver.open();
    medDriver.readFileStruct();
    medDriver.close();

    if ( fields.empty() ) {
      cout << "No fileds found in EnSight file " << filenameIN << endl;
      return -1;
    }
    // read-write the mesh
    const GMESH* mesh = fields[0]->getSupport()->getMesh();
    const_cast<GMESH*>( mesh )->read();
    mesh->write(MED_DRIVER, filenameOUT );

    // read-write fields
    for ( unsigned i = 0; i < fields.size(); ++i )
    {
      for ( int timeStep = 1; ; ++timeStep )
      {
        FIELD<int> *intF=new FIELD<int>;
        intF->setName( fields[i]->getName() );
        try
        {
          ENSIGHT_FIELD_RDONLY_DRIVER fieldDriver( filenameIN, intF, timeStep );
          fieldDriver.open();
          fieldDriver.read();
          fieldDriver.close();
          // replace zero values as theses fields are used for division
          int nbVals = intF->getValueLength();
          int* values = const_cast<int*>(intF->getValue());
          while ( nbVals-- ) {
            if ( values[nbVals]==0 )
              values[nbVals]= nbVals%5 + 1;
          }
        }
        catch ( const MEDEXCEPTION& ex)
        {
          intF->removeReference();
          intF=0;
          break;
        }
        intF->write(MED_DRIVER,filenameOUT);
        intF->removeReference();
      }
      fields[i]->removeReference();
    }
    mesh->removeReference();
  }
  else usage(argv[0]);

}


