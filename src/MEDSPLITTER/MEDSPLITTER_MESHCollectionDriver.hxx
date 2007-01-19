#ifndef MESHCOLLECTIONDRIVER_HXX_
#define MESHCOLLECTIONDRIVER_HXX_


namespace MEDSPLITTER{
	
class MESHCollection;

class MESHCollectionDriver
{
public:
  
  MESHCollectionDriver(MESHCollection*);
  virtual ~MESHCollectionDriver(){}
  
  int read(char*);
  
  int readSeq(char*,char*);
  
  void write(char*);
  
  template <class T>
  void readFields(vector <MEDMEM::FIELD<T> *>& filenames, char* fieldname,
		  int itnumber, int ordernumber);
  
  template <class T>
  void writeFields(vector <MEDMEM::FIELD<T> *>& filenames, char* fieldname);
  
  void readFileStruct(vector <string>&  field_names,vector<int>& iternumber,vector <int>&  ordernumber,vector <int> & types);
  
  int getFieldType(const std::string& fieldname);
  //	void exportFamily(vector<int*>,MED_EN::medEntityMesh, const string& name);
    
private :
  
  MESHCollection* m_collection;
  
  std::vector <std::string> m_filename;
};

}


#endif /*MESHCOLLECTIONDRIVER_HXX_*/
