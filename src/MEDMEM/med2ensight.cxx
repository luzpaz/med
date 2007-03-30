// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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

    MED myMed(MED_DRIVER,filenameIN) ;

    cout << "-> Read all meshes "  ;
    int NumberOfMeshes = myMed.getNumberOfMeshes() ;
    cout << "( "<<NumberOfMeshes << " ) :" << endl ;
    deque<string> MeshName = myMed.getMeshNames() ;
    for (int i=0; i<NumberOfMeshes; i++) {
      myMed.getMesh(MeshName[i])->read() ;
      cout << "-> Mesh "<<i+1<<", named "<<MeshName[i]<<" is read !" << endl;
    }

    myMed.updateSupport() ;
    
    cout << "-> Read all fields " ;
    int NumberOfFields = myMed.getNumberOfFields() ;
    cout << "( "<<NumberOfFields << " ) :" << endl;
    deque<string> FieldName = myMed.getFieldNames() ;
    for (int i=0; i<NumberOfFields; i++) {
      deque<DT_IT_> FieldIteration = myMed.getFieldIteration(FieldName[i]) ;
      cout << "-> Field "<<i+1<<", named "<<FieldName[i] << " :" << endl ;
      int NumberOfIteration = FieldIteration.size() ;
      cout << "    Number of iteration pair : "<< NumberOfIteration << endl;
      for (int j=0; j<NumberOfIteration; j++) {
	FIELD_ * myField = myMed.getField(FieldName[i],FieldIteration[j].dt,FieldIteration[j].it) ;
	
	myField->read() ;
	cout << "    * Iteration "<<FieldIteration[j].dt<<" and  order number "<<FieldIteration[j].it<<" ) is read !" << endl;
      }
    }
    int id = myMed.addDriver(ENSIGHT_DRIVER,filenameOUT,MED_EN::MED_ECRI) ;
    myMed.write(id) ;
  }
  else usage(argv[0]);
}
