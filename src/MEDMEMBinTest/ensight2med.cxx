//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include<string>
#include<deque>
#include<vector>

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

#include "MEDMEM_Med.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_EnsightMedDriver.hxx"

using namespace std;
using namespace MEDMEM;
void usage(char * name)
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

    MED myMed(ENSIGHT_DRIVER,filenameIN) ;
    myMed.read() ;

    int id = myMed.addDriver(MED_DRIVER,filenameOUT);
    myMed.write(id);
  }
  else if ( argc == 4 && strncmp(argv[3], "I", 1 )==0 )
  {
    // we read all variables into INT32 fields
    // (we need such fields for test_operation_fieldint)

    filenameIN  = argv[1] ;
    filenameOUT = argv[2] ;
    MED myMed(ENSIGHT_DRIVER,filenameIN);

    // read-write the mesh
    if ( myMed.getNumberOfMeshes() < 1 ) {
      cout << "No meshes found in EnSight file " << filenameIN << endl;
      return -1;
    }
    vector<string> meshNames( myMed.getNumberOfMeshes() );
    myMed.getMeshNames( &meshNames[0] );
    MESH * mesh = myMed.getMesh( meshNames[0] );
    mesh->read();
    int drv = mesh->addDriver(MED_DRIVER, filenameOUT, mesh->getName() );
    mesh->write( drv );

    // read-write fields
    int nbFields = myMed.getNumberOfFields();
    vector<string> fieldNames( nbFields );
    myMed.getFieldNames( &fieldNames[0] );
    for ( int i = 0; i < nbFields; ++i )
    {
      deque<DT_IT_> dis = myMed.getFieldIteration( fieldNames[i] );
      for ( deque<DT_IT_>::iterator d_i = dis.begin(); d_i != dis.end(); ++d_i )
      {
        FIELD_* f = myMed.getField( fieldNames[i], d_i->dt, d_i->it );
        FIELD<int> *intF=new FIELD<int>(ENSIGHT_DRIVER, filenameIN, f->getName() );
        // replace zero values as theses fields are used for division
        int nbVals = intF->getValueLength();
        int* values = const_cast<int*>(intF->getValue());
        while ( nbVals-- ) {
          if ( values[nbVals]==0 )
            values[nbVals]= nbVals%5 + 1;
        }
        int drv = intF->addDriver(MED_DRIVER, filenameOUT, f->getName() );
        intF->write( drv );
        intF->removeReference();
      }
    }
  }
  else usage(argv[0]);

}


