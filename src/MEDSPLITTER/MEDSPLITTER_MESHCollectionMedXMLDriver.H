#ifndef MEDSPLITTER_MESHCOLLECTIONMEDXMLDRIVER_H
#define MEDSPLITTER_MESHCOLLECTIONMEDXMLDRIVER_H


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
void MESHCollectionMedXMLDriver::_readFields(vector<MEDMEM::FIELD<T>* >& fields,char* fieldname, int itnumber, int ordernumber)
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
void MESHCollectionMedXMLDriver::_writeFields(vector<MEDMEM::FIELD<T>* >& fields,char* fieldname)
{
	xmlDocPtr master_doc=xmlParseFile(m_master_filename.c_str());
	
	if (!master_doc)    
		throw MEDEXCEPTION("MEDSPLITTER writeFields - Master File does not exist");
	
	//number of domains
	
	xmlXPathContextPtr xpathCtx = xmlXPathNewContext(master_doc);
	xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST "//mapping/mesh", xpathCtx);
	//assuming there is only one mesh in the XML file
	xmlNodePtr mesh_node= xpathObj->nodesetval->nodeTab[0];

	//adds the field to the master file if necessary
	bool exist_field =false;
	xpathObj = xmlXPathEvalExpression(BAD_CAST "//mapping/mesh/field", xpathCtx);
	//assuming there is only one mesh in the XML file
	int field_nr =  xpathObj->nodesetval->nodeNr;
	for (int i=0; i<field_nr; i++)
		{
			//field node has only one property
			if ( strcmp((const char*)xpathObj->nodesetval->nodeTab[i]->properties->children->content, fieldname)==0)
				exist_field = true;
		}

	xmlNodePtr field_node;
	if (!exist_field)
		{
			field_node = xmlNewChild(mesh_node, 0, BAD_CAST "field",0);
			xmlNewProp(field_node,BAD_CAST "name",BAD_CAST fieldname);
		}
	
	
	for (int i=0; i<m_collection->getMesh().size(); i++)
		{
			char filename[256];
			strcpy(filename,m_filename[i].c_str());
			int driverid = fields[i]->addDriver(MEDMEM::MED_DRIVER, filename, fieldname);
			fields[i]->write(driverid);

			//adds the partition to the master file if the field had not been 
			//added already
			if (!exist_field)
			{
			  xmlNodePtr chunk_node= xmlNewChild(field_node,0,BAD_CAST "chunk",0);
			  char id[8];
			  sprintf(id,"%d",i+1);
			  xmlNewProp(chunk_node,BAD_CAST "subdomain",BAD_CAST id);
			  //xmlNewProp(chunk_node,BAD_CAST "name", BAD_CAST fieldname);
			  //xmlNodePtr fieldname_node=
			  xmlNewChild(chunk_node,0,BAD_CAST "name", BAD_CAST fieldname);
			}
		}
  //Writing file
	xmlKeepBlanksDefault(0);
	xmlSaveFormatFileEnc(m_master_filename.c_str(), master_doc, "UTF-8", 1);
  
 	//Clean up
	xmlXPathFreeContext(xpathCtx); 
  xmlFreeDoc(master_doc);
  xmlCleanupParser();

}

#endif
