#ifndef MEDSPLITTER_MESHCOLLECTIONMEDASCIIDRIVER_H
#define MEDSPLITTER_MESHCOLLECTIONMEDASCIIDRIVER_H


#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

/*!reads a distributed field
 * 
 * \param fields vector of fields (one field per subdomain)
 * \param fieldname name of the field
 * \param itnumber number of iteration
 * \param ordernumber internal number inside the iteration
 * */
template <class T>
void MESHCollectionMedAsciiDriver::_readFields(vector<MEDMEM::FIELD<T>* >& fields,char* fieldname, int itnumber, int ordernumber)
{
  for (int i=0; i<m_collection->getMesh().size(); i++)
    {
      char filename[256];
      strcpy(filename,m_filename[i].c_str());
      cout << "maillage : " << filename << " champ : " << fieldname << endl;
      //		MEDMEM::FIELD<T>* field = new MEDMEM::FIELD<T>(MEDMEM::MED_DRIVER,filename,fieldname,itnumber,ordernumber);
      fields.push_back (new MEDMEM::FIELD<T>(MEDMEM::MED_DRIVER,filename,fieldname,itnumber,ordernumber));
    }
}


/*!writes a distributed field
 * 
 * \param fields vector of fields (one field per subdomain)
 * \param fieldname name of the field
 * */
template <class T>
void MESHCollectionMedAsciiDriver::_writeFields(vector<MEDMEM::FIELD<T>* >& fields,char* fieldname)
{
  for (int i=0; i<m_collection->getMesh().size(); i++)
    {
      char filename[256];
      strcpy(filename,m_filename[i].c_str());
      int driverid = fields[i]->addDriver(MEDMEM::MED_DRIVER, filename, fieldname);
      fields[i]->write(driverid);		 
    }
}

#endif
