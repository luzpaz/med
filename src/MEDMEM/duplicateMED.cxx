//  MED MEDMEM : MED files in memory
//
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
//  File   : duplicateMED.cxx
//  Module : MED

using namespace std;
#include<string>

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Med.hxx"

void usage(char * name)
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
    
    MED myMed(MED_DRIVER,filenameIN) ; // do readFilestruct !

    // we read all meshes and fields in filenameIN
    myMed.read() ;

    // we write all in file filenameOUT :
    int id = myMed.addDriver(MED_DRIVER,filenameOUT) ;
    myMed.write(id);

  } catch (MEDEXCEPTION& ex) {
    MESSAGE(ex.what()) ;
  }

}
