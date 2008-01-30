#ifndef MEDSPLITTER_MESHCOLLECTIONDRIVER_H
#define MEDSPLITTER_MESHCOLLECTIONDRIVER_H


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
void MESHCollectionDriver::readFields(vector<MEDMEM::FIELD<T>* >& fields,char* fieldname, int itnumber, int ordernumber)
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
void MESHCollectionDriver::writeFields(vector<MEDMEM::FIELD<T>* >& fields,char* fieldname)
{
	xmlDocPtr master_doc=xmlParseFile(m_master_filename.c_str());
	
	if (!master_doc)    
		throw MEDEXCEPTION("MEDSPLITTER writeFields - Master File does not exist");
	
	//number of domains
	
	xmlXPathContextPtr xpathCtx = xmlXPathNewContext(master_doc);
	xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST "//mapping/mesh", xpathCtx);
	//assuming there is only one mesh in the XML file
	xmlNodePtr mesh_node= xpathObj->nodesetval->nodeTab[0];
	xmlNodePtr field_node = xmlNewChild(mesh_node, 0, BAD_CAST "field",0);
	xmlNewProp(field_node,BAD_CAST "name",BAD_CAST fieldname);
	
	
	for (int i=0; i<m_collection->getMesh().size(); i++)
		{
			char filename[256];
			strcpy(filename,m_filename[i].c_str());
			int driverid = fields[i]->addDriver(MEDMEM::MED_DRIVER, filename, fieldname);
			fields[i]->write(driverid);
			xmlNodePtr chunk_node= xmlNewChild(field_node,0,BAD_CAST "chunk",0);
			char id[8];
			sprintf(id,"%d",i+1);
			xmlNewProp(chunk_node,BAD_CAST "subdomain",BAD_CAST id);
			xmlNewProp(chunk_node,BAD_CAST "name", BAD_CAST fieldname);
			
		}
	xmlSaveFormatFileEnc(m_master_filename.c_str(), master_doc, "UTF-8", 1);
 	xmlFreeDoc(master_doc);
	xmlCleanupParser();

}

#endif
