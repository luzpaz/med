using namespace std;
#include<string>
#include<deque>

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

#include "MEDMEM_Med.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Field.hxx"


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
  
  MED * myMed = new MED() ;
  MED_MED_DRIVER myMedDriver(filenameIN,myMed) ;

  // we read all meshes and fields in filenameIN
  try {
    
    int read ;
    myMedDriver.open();
    myMedDriver.readFileStruct();
    myMedDriver.close();
    
    // read all mesh
    MESSAGE("Read all meshes :") ;
    int NumberOfMeshes = myMed->getNumberOfMeshes() ;
    MESSAGE("Number of meshes : "<<NumberOfMeshes) ;
    deque<string> MeshName = myMed->getMeshNames() ;
    map<string,MESH*> _meshes ;
    for (int i=0; i<NumberOfMeshes; i++) {
      _meshes[MeshName[i]]=myMed->getMesh(MeshName[i]) ;
      _meshes[MeshName[i]]->read();
      MESSAGE("  - Mesh "<<i+1<<", named "<<MeshName[i]<<" read !");
      MED_MESH_WRONLY_DRIVER myMeshDriver(filenameOUT,_meshes[MeshName[i]]);
      myMeshDriver.setMeshName(MeshName[i]);
      myMeshDriver.open() ;
      myMeshDriver.write() ;
      myMeshDriver.close() ;
    }

    // set support : support must be calculated with mesh information !!!
    myMed->updateSupport() ;
    
    // read all field
    MESSAGE("Read all fields :") ;
    int NumberOfFields = myMed->getNumberOfFields() ;
    MESSAGE("Number of fields : "<<NumberOfFields);
    deque<string> FieldName = myMed->getFieldNames() ;
    map<string,FIELD_*> _fields ;
    for (int i=0; i<NumberOfFields; i++) {
      deque<DT_IT_> FieldIteration = myMed->getFieldIteration(FieldName[i]) ;
      MESSAGE("  - Field "<<i+1<<", named "<<FieldName[i]<<" :");
      int NumberOfIteration = FieldIteration.size() ;
      MESSAGE("    Number of iteration pair : "<<NumberOfIteration);
      for (int j=0; j<NumberOfIteration; j++) {
	MESSAGE("    FieldName[i] "<<FieldName[i]<<",Iteration "<< FieldIteration[j].dt<<", order number "<<FieldIteration[j].it<<" j="<<j<<",i="<<i<<" :");
	FIELD_ * myField = myMed->getField(FieldName[i],FieldIteration[j].dt,FieldIteration[j].it) ;
	
	med_type_champ type = myField->getValueType() ;
	switch (type) {
	case MED_FR::MED_INT32: {
	  //	if (type == MED_FR::MED_INT32) {
	  MESSAGE("    * Field named "<<((FIELD<int>*)myField)->getName());
	  ((FIELD<int>*)myField)->read() ;
	  MESSAGE("    * Iteration "<<FieldIteration[j].dt<<", order number "<<FieldIteration[j].it<<" read !");
	  MED_FIELD_WRONLY_DRIVER<int> myFieldDriver(filenameOUT,(FIELD<int>*)myField);
	  myFieldDriver.setFieldName(FieldName[i]);
	  myFieldDriver.open() ;
	  myFieldDriver.write() ;
	  myFieldDriver.close() ;
  	  break ;
	}
	  //	else
  	case MED_FR::MED_REEL64: {
	  //	  if (type == MED_FR::MED_REEL64) {
	  FIELD<double>* myField2 = (FIELD<double>*)myField ;
	  MESSAGE("    * Field named "<<myField2->getName());
	  myField2->read() ;
	  MESSAGE("    * Iteration "<<FieldIteration[j].dt<<", order number "<<FieldIteration[j].it<<" read !");	  
	  MED_FIELD_WRONLY_DRIVER<double> myFieldDriver(filenameOUT,myField2);
	  myFieldDriver.setFieldName(FieldName[i]);
	  myFieldDriver.open() ;
	  myFieldDriver.write() ;
	  myFieldDriver.close() ;
	  break ;
	}
	  //	else {
  	default: {
	  MESSAGE("    * Iteration "<<FieldIteration[j].dt<<", order number "<<FieldIteration[j].it<<" not read : wrond type "<<type);
	  break ;
	}
	}
      }
    }
  } catch (MEDEXCEPTION& ex){
    MESSAGE(ex.what()) ;
  }

  // we write all in file filenameOUT :
//    try {
    
//      MED_MED_DRIVER myMedDriverOUT("/tmp/test.med",myMed) ;
//      myMedDriverOUT.open() ;
//      myMedDriverOUT.write() ;
//      myMedDriverOUT.close() ;
//    } catch (MEDEXCEPTION& ex){
//      MESSAGE(ex.what()) ;
//    }

  delete myMed ;

}
