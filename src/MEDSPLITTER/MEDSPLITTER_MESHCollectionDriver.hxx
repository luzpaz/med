#ifndef MESHCOLLECTIONDRIVER_HXX_
#define MESHCOLLECTIONDRIVER_HXX_


namespace MEDSPLITTER{
	
class MESHCollection;

class MESHCollectionDriver
{
public:
  
  MESHCollectionDriver(MESHCollection*);
  virtual ~MESHCollectionDriver(){}

  virtual int read(char*)=0;
  int readSeq(char*,char*);
  
  virtual void write(char*)=0;
	virtual void readFields (vector <MEDMEM::FIELD<int> *>& filenames, char* fieldname,
															int itnumber, int ordernumber) =0;
	virtual void readFields (vector <MEDMEM::FIELD<double> *>& filenames, char* fieldname,
															int itnumber, int ordernumber) =0;
	virtual void writeFields(vector <MEDMEM::FIELD<int> *>& filenames, char* fieldname)=0;
	virtual void writeFields(vector <MEDMEM::FIELD<double> *>& filenames, char* fieldname)=0;

  void readFileStruct(vector <string>&  field_names,vector<int>& iternumber,vector <int>&  ordernumber,vector <int> & types);
  
  int getFieldType(const std::string& fieldname);
  //	void exportFamily(vector<int*>,MED_EN::medEntityMesh, const string& name);

protected:

 void readSubdomain(const string& meshname, vector<int*>& cellglobal,
vector<int*>& faceglobal,
vector<int*>& nodeglobal, int idomain);
 void writeSubdomain(int idomain,int nbdomain, char*filename);
 
void writeElementJoint(medEntityMesh entity ,
                       int icz, 
                       int idomain, 
                       int idistant, 
                       char* mesh_name, 
                       char* joint_name,  
                       med_2_2::med_idt fid );
void jointSort(int* elems, int nbelems, bool is_first);



  MESHCollection* m_collection;
  
  std::vector <std::string> m_filename;

};

}


#endif /*MESHCOLLECTIONDRIVER_HXX_*/
