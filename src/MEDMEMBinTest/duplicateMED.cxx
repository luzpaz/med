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

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Grid.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_MedFileBrowser.hxx"

using namespace std;
using namespace MEDMEM;
static void usage(char * name)
{
  cout << "  " << name << " <input med file> <output med file> " <<endl ;
  cout << "    " << "(the two file name are mandatory)" << endl ;
  exit(-1);
}

int main (int argc, char ** argv) {
  if (argc != 3) usage(argv[0]);
  
  string filenameIN = argv[1] ;
  string filenameOUT = argv[2] ;
  
  try {

    MEDFILEBROWSER myMed(filenameIN) ;

    std::vector< std::string > meshNames = myMed.getMeshNames ();
    for ( unsigned i = 0; i < meshNames.size(); ++i )
    {
      GMESH* mesh = myMed.isStructuredMesh( meshNames[i] ) ? (GMESH*) new GRID : (GMESH*) new MESH;
      int drv = mesh->addDriver(MED_DRIVER, filenameIN, meshNames[i] );
      mesh->read(drv);
      drv = mesh->addDriver(MED_DRIVER, filenameOUT, meshNames[i] );
      mesh->write(drv);
      mesh->removeReference();
    }
    
    vector<string> FieldName = myMed.getFieldNames() ;
    for (unsigned i=0; i<FieldName.size(); i++)
    {
      vector<DT_IT_> FieldIteration = myMed.getFieldIteration(FieldName[i]) ;
      for (unsigned j=0; j<FieldIteration.size(); j++)
      {
        FIELD_ * myField = 0;
        switch( myMed.getFieldType( FieldName[i] ))
        {
        case MED_REEL64: myField = new FIELD<double>; break;
        case MED_INT32:  
        case MED_INT64:  myField = new FIELD<int>; break;
        default:
          continue;
        }
        myField->setIterationNumber( FieldIteration[j].dt );
        myField->setOrderNumber    ( FieldIteration[j].it );
        int drv = myField->addDriver( MED_DRIVER, filenameIN, FieldName[i]);
        myField->read( drv ) ;
        drv = myField->addDriver( MED_DRIVER, filenameOUT, FieldName[i]);
        myField->write( drv );
        myField->removeReference();
      }
    }
  } catch (MEDEXCEPTION& ex) {
    MESSAGE_MED(ex.what()) ;
  }

}
