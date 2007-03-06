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
  cout << "  " << name << " <input Ensight file> <output Med file> " <<endl ;
  cout << "    " << "(the two file name are mandatory )" << endl ;
  exit(-1);
}

// ensight2med toto.case toto.med => input : toto.case => output : toto.med....

int main (int argc, char ** argv) {

  if (argc != 3) usage(argv[0]);

  string filenameIN = argv[1];
  string filenameOUT= argv[2];
 
  cout << " encountering translation from Ensight" << filenameIN << " to Med " << filenameOUT <<  endl ;

  MED myMed(ENSIGHT_DRIVER,filenameIN) ;
  cout << " driver ensight start : call to read part " << endl ;
  //--------------
  myMed.read() ;
  //--------------
  cout << " driver ensight OK : call to med part " << endl ;
  int id = myMed.addDriver(MED_DRIVER,filenameOUT);
  myMed.write(id);
  cout << " med part OK " << endl ;

}
