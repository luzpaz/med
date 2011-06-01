// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_medimport_src.hxx"
#include <cstdlib>

using namespace std;
using namespace MEDMEM;

int main (int argc, char ** argv)
{
  if (argc != 2)
    {
      cerr << "Usage : " << argv[0] 
           << " fileNameIn" << endl ;
      cerr << "Where - fileNameIn is .med V2.1 file supplied by the user including all the directory path" << endl;
      cerr << "fileNameOut is .med V2.3.2 file produced by this command in the directory $HOME/TMP_Med2_3Files/" << endl;
      cerr << "This command is supposed to be the same as the med File V2.2 medimport utility" << endl << endl;
      cerr << "It will produce a med V2.3.2 file fileNameOut which will be in the directory $HOME/TMP_Med2_3Files/" << endl;
      cerr << "with the same root name as fileNameIn: " << endl;
      cerr << "If fileNameIn is /aPath/file.med so fileNameOut will be $HOME/TMP_Med2_3Files/file.med" << endl;
      exit(-1);
    }

  string fileNameIn = argv[1] ;
  char * fileNameOut = med2_1_To_med2_3((const_cast <char *>
                                         (fileNameIn.c_str())));

  if (fileNameOut == ((char *) NULL))
    {
      cerr << "A problem has occured in the importation of the file " << fileNameIn.c_str() << " into " << fileNameOut << endl;
    }
  else if(fileNameOut != string(fileNameIn).c_str())
    {
      cerr << "the convertion has been successfull !! The resulting file is " << string(fileNameOut).c_str() << endl;
    }
  else
    {
      cerr << "No nedd to converte because the file is al ready a med 2.3.2 file" << endl;
    }

  return 0;
}
