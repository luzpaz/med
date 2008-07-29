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
  
  std::string filename = "/dn22/SALOME/series3x/SAMPLES/SAMPLES_SRC/MedFiles/pointe_import22.med";

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
