using namespace std;
#include<string>

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Med.hxx"

using namespace MEDMEM;
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
