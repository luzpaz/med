#include<string>
#include<deque>

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

#include "MEDMEM_Med.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_VtkMedDriver.hxx"

#ifdef _DEBUG_
#include "LocalTraceCollector.hxx"
#endif /* ifdef _DEBUG_*/

using namespace std;
using namespace MEDMEM;
void usage(char * name)
{
  cout << "  " << name << " <input med file> <output vtk file> " <<endl ;
  cout << "    " << "(the two file name are mandatory)" << endl ;
  exit(-1);
}

int main (int argc, char ** argv) {
#ifdef _DEBUG_
  LocalTraceCollector::instance();
#endif /* ifdef _DEBUG_*/

  if (argc != 3) usage(argv[0]);
  
  string filenameIN = argv[1] ;
  string filenameOUT = argv[2] ;
  
  try {
    /////////////////////////////////////////////////
    // we read all meshes and fields in filenameIN //
    /////////////////////////////////////////////////
    MED myMed(MED_DRIVER,filenameIN) ;
    
    // read all meshes
    ////////////////////

    cout << "Read all meshes "  ;
    int NumberOfMeshes = myMed.getNumberOfMeshes() ;
    cout << "( "<<NumberOfMeshes << " ) :" << endl ;
    deque<string> MeshName = myMed.getMeshNames() ;
    for (int i=0; i<NumberOfMeshes; i++) {
      myMed.getMesh(MeshName[i])->read() ;
      cout << "  - Mesh "<<i+1<<", named "<<MeshName[i]<<" is read !" << endl;
    }

    // PROVISOIRE
    ///////////////

    // set support : support must be calculated with mesh information !!!
    myMed.updateSupport() ;
    
    // read all fields
    ////////////////////

    cout << "Read all fields " ;
    int NumberOfFields = myMed.getNumberOfFields() ;
    cout << "( "<<NumberOfFields << " ) :" << endl;
    deque<string> FieldName = myMed.getFieldNames() ;
    for (int i=0; i<NumberOfFields; i++) {
      deque<DT_IT_> FieldIteration = myMed.getFieldIteration(FieldName[i]) ;
      cout << "  - Field "<<i+1<<", named "<<FieldName[i] << " :" << endl ;
      int NumberOfIteration = FieldIteration.size() ;
      cout << "    Number of iteration pair : "<< NumberOfIteration << endl;
      for (int j=0; j<NumberOfIteration; j++) {
	FIELD_ * myField = myMed.getField(FieldName[i],FieldIteration[j].dt,FieldIteration[j].it) ;
	
	myField->read() ;
	cout << "    * Iteration "<<FieldIteration[j].dt<<" and  order number "<<FieldIteration[j].it<<" ) is read !" << endl;
      }
    }

    //////////////////////////////////////////
    // we write all in VTK file filenameOUT //
    /////////////////////////////////////////
    int id = myMed.addDriver(VTK_DRIVER,filenameOUT) ;
    myMed.write(id) ;

  } 
  catch (MEDEXCEPTION& ex){
    cout << ex.what() << endl ;
  }

}
