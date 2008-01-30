#ifndef MESHCOLLECTIONMEDASCIIDRIVER_HXX_
#define MESHCOLLECTIONMEDASCIIDRIVER_HXX_

#include "MEDSPLITTER_MESHCollectionDriver.hxx"

namespace MEDSPLITTER{
	
class MESHCollection;
  
  class MESHCollectionMedAsciiDriver:public MESHCollectionDriver
{
public:
  
  MESHCollectionMedAsciiDriver(MESHCollection*);
  virtual ~MESHCollectionMedAsciiDriver(){}

  int read(char*);

  void write(char*);
  
	void readFields(vector <MEDMEM::FIELD<int> *>& filenames, char* fieldname,
									int itnumber, int ordernumber)
	{
		_readFields(filenames,fieldname,itnumber,ordernumber);
	}
	void readFields(vector <MEDMEM::FIELD<double> *>& filenames, char* fieldname,
									int itnumber, int ordernumber)
	{
		_readFields(filenames,fieldname,itnumber,ordernumber);
	}
	
	void writeFields(vector <MEDMEM::FIELD<int> *>& filenames, char* fieldname)
	{
		_writeFields( filenames, fieldname);
	}
	
	void writeFields(vector <MEDMEM::FIELD<double> *>& filenames, char* fieldname)
	{
		_writeFields( filenames, fieldname);
	}

    
private :
	template <class T> void _readFields(vector <MEDMEM::FIELD<T> *>& filenames, char* fieldname,
																			int itnumber, int ordernumber);
  
  template <class T>
  void _writeFields(vector <MEDMEM::FIELD<T> *>& filenames, char* fieldname);
  

  
	std::string m_master_filename;
};

}


#endif /*MESHCOLLECTIONDRIVER_HXX_*/
