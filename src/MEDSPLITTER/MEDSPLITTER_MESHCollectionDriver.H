// Copyright (C) 2007-2012  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
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
  for (int i=0; i<_collection->getMesh().size(); i++)
  {
    char filename[256];
    strcpy(filename,_filename[i].c_str());
    cout << "maillage : " << filename << " champ : " << fieldname << endl;
    //    MEDMEM::FIELD<T>* field = new MEDMEM::FIELD<T>(MEDMEM::MED_DRIVER,filename,fieldname,itnumber,ordernumber);
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
  xmlDocPtr master_doc=xmlParseFile(_master_filename.c_str());

  if (!master_doc)    
    throw MEDEXCEPTION("MEDSPLITTER writeFields - Master File does not exist");

  //number of domains

  xmlXPathContextPtr xpathCtx = xmlXPathNewContext(master_doc);
  xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST "//mapping/mesh", xpathCtx);
  //assuming there is only one mesh in the XML file
  xmlNodePtr mesh_node= xpathObj->nodesetval->nodeTab[0];
  xmlNodePtr field_node = xmlNewChild(mesh_node, 0, BAD_CAST "field",0);
  xmlNewProp(field_node,BAD_CAST "name",BAD_CAST fieldname);


  for (int i=0; i<_collection->getMesh().size(); i++)
  {
    char filename[256];
    strcpy(filename,_filename[i].c_str());
    int driverid = fields[i]->addDriver(MEDMEM::MED_DRIVER, filename, fieldname);
    fields[i]->write(driverid);
    xmlNodePtr chunk_node= xmlNewChild(field_node,0,BAD_CAST "chunk",0);
    char id[8];
    sprintf(id,"%d",i+1);
    xmlNewProp(chunk_node,BAD_CAST "subdomain",BAD_CAST id);
    xmlNewProp(chunk_node,BAD_CAST "name", BAD_CAST fieldname);

  }
  xmlSaveFormatFileEnc(_master_filename.c_str(), master_doc, "UTF-8", 1);
  xmlFreeDoc(master_doc);
  //xmlCleanupParser();

}

#endif
