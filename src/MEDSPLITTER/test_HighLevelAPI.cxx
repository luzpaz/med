#include "MEDSPLITTER_API.hxx"
#include <string>

using namespace std;
int main()
{
	string testname="MEDSPLITTER - test #3 -";
	
	char filename[100] = "../../share/salome/resources/carre_en_quad4_import22.med";
	char meshname[20]  = "carre_en_quad4";
	char output[100]="./tests/API_test";
	int is_distributed=0;
	int nprocs=4;
	int meshonly=0;
	int method=0;
	return medsplitter(filename, 
					   meshname,
			   	 	   output,  
			    	   is_distributed,
			    	   nprocs,
			           method,
			           meshonly);
}
	
