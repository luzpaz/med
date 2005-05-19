#include "MEDMEM_Exception.hxx"
#include "MEDMEM_medimport_src.hxx"

using namespace std;
using namespace MEDMEM;

int main (int argc, char ** argv)
{
  if (argc != 2)
    {
      cerr << "Usage : " << argv[0] 
	   << " fileNameIn" << endl ;
      cerr << "Where - fileNameIn is .med V2.1 file supplied by the user including all the directory path" << endl;
      cerr << "fileNameOut is .med V2.2 file produced by this command in the directory $HOME/TMP_Med2_2Files/" << endl;
      cerr << "This command is supposed to be the same as the med File V2.2 medimport utility" << endl << endl;
      cerr << "It will produce a med V2.2 file fileNameOut which will be in the directory $HOME/TMP_Med2_2Files/" << endl;
      cerr << "with the same root name as fileNameIn: " << endl;
      cerr << "If fileNameIn is /aPath/file.med so fileNameOut will be $HOME/TMP_Med2_2Files/file.med" << endl;
      exit(-1);
    }

  string fileNameIn = argv[1] ;
  char * fileNameOut = med2_1_To_med2_2((const_cast <char *>
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
      cerr << "No nedd to converte because the file is al ready a med 2.2 file" << endl;
    }

  return 0;
}
