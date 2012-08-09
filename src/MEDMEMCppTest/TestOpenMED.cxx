// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "MEDMEMTest_Utils.hxx"

#include <string>
#include <iostream>

using namespace std;

namespace med_2_3 {
  extern "C" { 
    #include <med.h>
    #include <med_proto.h>
    extern med_idt _MEDdatagroupOuvrir(med_idt pid, char *nom);
    extern med_err _MEDdatagroupFermer(med_idt id);
  }
}

int main(int argc, char** argv)
{
  cout<<"test start"<<endl;

  std::string filename = getResourceFile("pointe.med");

  med_2_3::med_idt medIdt = med_2_3::MEDouvrir( (const_cast <char *> (filename.c_str())),
                                                 med_2_3::MED_LECTURE_ECRITURE);

  cout<<"The idt of file "<<filename<<" is : "<<medIdt<<endl;

  if(medIdt > 0) {
    cout<<"File is opened in mode MED_LECTURE_ECRITURE"<<endl;
    int err = med_2_3::MEDfermer(medIdt);
    if (err != 0)
      cout<<"Error when closing file !"<<endl;
  }
  else {
    cout<<"Could not open file in mode MED_LECTURE_ECRITURE"<<endl;
  }

  medIdt = med_2_3::MEDouvrir( (const_cast <char *> (filename.c_str())),
                               med_2_3::MED_LECTURE);

  cout<<"The idt of file "<<filename<<" is : "<<medIdt<<endl;

  if(medIdt > 0) {
    cout<<"File is opened in mode MED_LECTURE"<<endl;
    int err = med_2_3::MEDfermer(medIdt);
    if (err != 0)
      cout<<"Error when closing file !"<<endl;
  }
  else {
    cout<<"Could not open file in mode MED_LECTURE"<<endl;
  }

  cout<<"test end"<<endl;

  return 1;
}
