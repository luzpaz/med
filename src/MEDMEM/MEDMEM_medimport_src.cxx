//  Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "MEDMEM_medimport_src.hxx"

#include <exception>
#include <iostream>

using namespace MEDMEM;


char * MEDMEM::med2_1_To_med2_3(char * fileNameIn) throw (MEDEXCEPTION)
{
//  char * fileNameOut = new char[strlen(fileNameIn)+strlen("2.3")+1];
  const char * fileNameOut = "";
  int ret;

//  strcpy(fileNameOut,fileNameIn);
//  strcat(fileNameOut,"2.3");
  
  try {
    ret=MEDimport(fileNameIn,(char *)fileNameOut) ;
  } catch ( std::exception &ex) {
      std::cout << "An error eoccured durrnig execution of medImport" << std::endl;
      std::cout << ex.what() << std::endl;
      throw MEDEXCEPTION("An error occured during execution of  medImport ");
      
  }
  
  return (char*)fileNameOut;
}
