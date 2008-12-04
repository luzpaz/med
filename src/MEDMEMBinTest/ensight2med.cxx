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
  cout << "  " << name << " <input ensight file> <output med file> " << endl ;
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
  else usage(argv[0]);

}


