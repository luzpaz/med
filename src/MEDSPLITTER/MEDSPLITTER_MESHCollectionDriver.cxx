
#include <vector>
#include <string>
#include <map>
#include <set>

#include <iostream>
#include <fstream>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <sys/time.h>
//Debug macros
#include "MEDMEM_Utilities.hxx"

//MEDMEM includes
#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Med.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_SkyLineArray.hxx"
#include "MEDMEM_ConnectZone.hxx"

//MEDSPLITTER includes
#include "MEDSPLITTER_Topology.hxx"
#include "MEDSPLITTER_ParallelTopology.hxx"
#include "MEDSPLITTER_SequentialTopology.hxx"
#include "MEDSPLITTER_MESHCollectionDriver.hxx"
#include "MEDSPLITTER_MESHCollection.hxx"

using namespace MEDSPLITTER;

//template inclusion
//#include "MEDSPLITTER_MESHCollectionDriver.H"


MESHCollectionDriver::MESHCollectionDriver(MESHCollection* collection):m_collection(collection)
{
}


/*!reads a unique MED File v>=2.1
 * and mounts the corresponding mesh in memory
 *\param filename binary file
 *\param meshname mesh name in the MED file
 * */
int MESHCollectionDriver::readSeq(char* filename, char* meshname)
{
  const char* LOC = "MEDSPLITTER::MESHCollectionDriver::readSeq()";
  BEGIN_OF(LOC);

        m_filename.resize(1);
	m_filename[0]=string(filename);
	//puts the only mesh in the mesh vector
	MEDMEM::MESH* new_mesh = new MEDMEM::MESH(MEDMEM::MED_DRIVER,filename, meshname);
	(m_collection->getMesh()).push_back(new_mesh);

	m_collection->setName(meshname);
	(m_collection->getCZ()).clear();
	vector<int*> cellglobal,nodeglobal,faceglobal;
	cellglobal.resize(1);
	nodeglobal.resize(1);
	faceglobal.resize(1);
	cellglobal[0]=0;
	nodeglobal[0]=0;
	faceglobal[0]=0;
	//creation of topology from mesh 
	//connectzone argument is 0
        ParallelTopology* aPT = new ParallelTopology
          ((m_collection->getMesh()), (m_collection->getCZ()), cellglobal, nodeglobal, faceglobal);
	m_collection->setTopology(aPT);
  END_OF(LOC);
        return 0;
}

/*!
 * Reads the file structure to determine the list
 * of all the available fields
 *
 * \param field_names, vector<string> containing the field names
 * \param iternumber, vector<int> containing the iteration numbers
 * \param ordernumber, vector<int> containing the order numbers
 * \param types, vector<int> containing 0 for int fields and 1 for double fields
 * 
 */
    
void MESHCollectionDriver::readFileStruct(vector <string>&  field_names,vector<int>& iternumber,vector <int>&  ordernumber, vector <int>& types)
{
  const char* LOC = "MEDSPLITTER::MESHCollectionDriver::readFileStruct()";
  BEGIN_OF(LOC);
 
		const MEDMEM::MED med_struct (MEDMEM::MED_DRIVER,m_filename[0]);
	int nb_fields = med_struct.getNumberOfFields();
	
	MESSAGE("found "<<nb_fields<<" fields in file")
		deque<string> names = med_struct.getFieldNames();
	for (int ifield = 0; ifield < nb_fields; ifield++)
		{
			deque<MEDMEM::DT_IT_> dtit=med_struct.getFieldIteration(names[ifield]);
		
			for (deque<MEDMEM::DT_IT_>::const_iterator iter =dtit.begin(); iter!=dtit.end(); iter++)
				{
					field_names.push_back(names[ifield]);
					iternumber.push_back(iter->dt);
					ordernumber.push_back(iter->it);	

					// testing whether the field is of double or int type		
					MEDMEM::FIELD_* field = med_struct.getField(names[ifield],iter->dt,iter->it);
					if (dynamic_cast<MEDMEM::FIELD<double>*>(field))
						types.push_back(1);
					else
						types.push_back(0);
			
				}
		}
  END_OF(LOC);
	}

//!retrieves the type of a field for a given fieldname
int MESHCollectionDriver::getFieldType(const string& fieldname)
{
  const char* LOC = "MEDSPLITTER::MESHCollectionDriver::getFieldType()";
  BEGIN_OF(LOC);
		const MEDMEM::MED med_struct (MEDMEM::MED_DRIVER,m_filename[0]);

	deque<MEDMEM::DT_IT_> dtit=med_struct.getFieldIteration(fieldname);
	deque<MEDMEM::DT_IT_>::const_iterator iter =dtit.begin();
	
	// testing whether the field is of double or int type		
	MEDMEM::FIELD_* field = med_struct.getField(fieldname,iter->dt,iter->it);

  END_OF(LOC);
  
	if (dynamic_cast<MEDMEM::FIELD<double>*>(field))
	  return 1;
	else
	  return 0;			

	}


void MESHCollectionDriver::readSubdomain(const string& meshstring,
vector<int*>& cellglobal,
vector<int*>& faceglobal,
vector<int*>& nodeglobal, int idomain
)
{
  const char* LOC = "MEDSPLITTER::MESHCollectionDriver::readSubdomain()";
  BEGIN_OF(LOC);
  char file[256];
  char meshname[MED_TAILLE_NOM];
      
  strcpy(meshname,meshstring.c_str());
  strcpy(file,m_filename[idomain].c_str());
  cout << "Reading "<<meshstring<<" in "<<m_filename[idomain]<<endl;
  (m_collection->getMesh())[idomain]=new MEDMEM::MESH(MEDMEM::MED_DRIVER,file, meshname);
  cout <<"End of Read"<<endl;
  //reading MEDSPLITTER::CONNECTZONEs NODE/NODE and CELL/CELL
  med_2_3::med_idt fid = med_2_3::MEDouvrir(file,med_2_3::MED_LECTURE);
  med_2_3::med_int njoint = med_2_3::MEDnJoint(fid, meshname);
  for (int ijoint=0; ijoint<njoint; ijoint++)
    {
      int distant;
      char joint_description[MED_TAILLE_DESC];
      char name[MED_TAILLE_NOM];
      char name_distant[MED_TAILLE_NOM];
        
      int ncorr = med_2_3::MEDjointInfo(fid,meshname, ijoint+1, name, 
                joint_description,
                &distant, name_distant);
  
      for (int ic=0; ic<ncorr; ic++)
        {
    med_2_3::med_entite_maillage cor_typent_local;
    med_2_3::med_geometrie_element cor_typgeo_local;
    med_2_3::med_entite_maillage cor_typent_dist;
    med_2_3::med_geometrie_element cor_typgeo_dist;
      
       
    int ncouples;
    ncouples = med_2_3::MEDjointTypeCorres(fid, meshname, name, ic+1,
                   &cor_typent_local,  &cor_typgeo_local,
                   &cor_typent_dist, &cor_typgeo_dist
                   );
    int* node_corresp=new int[ncouples];
    if (cor_typent_local == med_2_3::MED_NOEUD && cor_typent_dist == med_2_3::MED_NOEUD)
      {
      
        med_2_3::MEDjointLire(fid, meshname, name,
            node_corresp,ncouples,
            cor_typent_local,  cor_typgeo_local,
            cor_typent_dist, cor_typgeo_dist
            );
      }
    //constructing the connect zone and adding it to the connect zone list
    MEDMEM::CONNECTZONE* cz = new MEDMEM::CONNECTZONE();
    cz->setName(string(name));
    cz->setDescription(joint_description);
    cz->setLocalDomainNumber(idomain);
    cz->setDistantDomainNumber(distant);
    cz->setLocalMesh((m_collection->getMesh())[idomain]);
    cz->setDistantMesh((m_collection->getMesh())[distant]);
    cz->setNodeCorresp(node_corresp,ncouples);
    (m_collection->getCZ()).push_back(cz);
        
        }//loop on correspom_topology->nbDomain())ndances
    }//loop on joints 
    
  // Reading global numbering
  // 
  int ncell=(m_collection->getMesh())[idomain]->getNumberOfElementsWithPoly(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
  if (ncell>0)
  {
    int * array=new int[ncell];
    int offset=0;
    MESSAGE("Reading cell global numbering for mesh "<< idomain);
    list<MED_EN::medGeometryElement>::const_iterator iter;
    char meshchar[MED_TAILLE_NOM];
    strcpy(meshchar,(m_collection->getMesh())[idomain]->getName().c_str());
    int nbtypes = (m_collection->getMesh())[idomain]->getNumberOfTypesWithPoly(MED_EN::MED_CELL);
    MED_EN::medGeometryElement* types =(m_collection->getMesh())[idomain]->getTypesWithPoly(MED_EN::MED_CELL);
    for (int itype=0; itype<nbtypes;itype++)
      {
        MED_EN::medGeometryElement type=types[itype];
        if (!m_collection->isDimensionOK(type,m_collection->getMeshDimension())) continue;
        int ntype = (m_collection->getMesh())[idomain]->getNumberOfElementsWithPoly(MED_EN::MED_CELL,type);
        if (ntype==0) continue;
        med_2_3::MEDglobalNumLire(fid,meshname, array+offset, ntype,
                med_2_3::MED_MAILLE, (med_2_3::med_geometrie_element)type);
        offset+=ntype;
      }
    cellglobal[idomain]=array;
    delete[] types;
  } 
        
  MESSAGE("Reading node global numbering");
  int nnode= (m_collection->getMesh())[idomain]->getNumberOfNodes();
  {
  int* array=new int[nnode];
  med_2_3::MEDglobalNumLire(fid,meshname, array, nnode,
          med_2_3::MED_NOEUD, med_2_3::MED_POINT1); 
  nodeglobal[idomain]=array;
  } 
        
  MESSAGE("Reading face global numbering for mesh "<<idomain);
  int nbface=(m_collection->getMesh())[idomain]->getNumberOfElementsWithPoly(MED_EN::MED_FACE,MED_EN::MED_ALL_ELEMENTS);
  if (nbface!=0)
    {
      int* array=new int[nbface];
      int offset=0;
      int nbtypes = (m_collection->getMesh())[idomain]->getNumberOfTypesWithPoly(MED_EN::MED_FACE);
      MED_EN::medGeometryElement* types =(m_collection->getMesh())[idomain]->getTypesWithPoly(MED_EN::MED_FACE);
  
      for (int itype=0; itype< nbtypes; itype++)
        {
    MED_EN::medGeometryElement type=types[itype];
      if (!m_collection->isDimensionOK(type,m_collection->getMeshDimension()-1)) continue;
    
    int ntype = (m_collection->getMesh())[idomain]->getNumberOfElementsWithPoly(MED_EN::MED_FACE,type);
    if (ntype==0) continue;
    med_2_3::MEDglobalNumLire(fid,meshname, array+offset, ntype,
            med_2_3::MED_FACE, (med_2_3::med_geometrie_element)type);
    offset+=ntype;
        }
      faceglobal[idomain]=array;
       delete[] types;
    }
  med_2_3::MEDfermer(fid);
 
  //        if (nbface!=0) delete[]array;
  END_OF(LOC);
  }
  
  void MESHCollectionDriver::writeSubdomain(int idomain, int nbdomains, char* distfilename)
  {
      //build connect zones
//      if (nbdomains>1)
//  m_collection->buildConnectZones(idomain);
    
      MESSAGE(" Number of connect zones "<<(m_collection->getCZ()).size());
        
      //writing connect zones in joints
    
      med_2_3::med_idt fid = med_2_3::MEDouvrir(distfilename,med_2_3::MED_LECTURE_ECRITURE);  
    
      int index_joint=0;
      
      for (int icz=0; icz<(m_collection->getCZ()).size(); icz++)
  {
    if ((m_collection->getCZ())[icz]->getLocalDomainNumber()==idomain)
      {
              med_2_3::med_err error;
        int idistant=(m_collection->getCZ())[icz]->getDistantDomainNumber();
        char joint_name[MED_TAILLE_NOM];
        sprintf(joint_name,"joint_%i",idistant+1);
        char desc[MED_TAILLE_DESC];
        sprintf(desc,"connect_zone_%d",icz+1);
        
        char distant_name[MED_TAILLE_NOM];
        //sprintf(distant_name,"domain_%i",(m_collection->getCZ())[icz]->getDistantDomainNumber());
        
        sprintf(distant_name,(m_collection->getMesh())[idistant]->getName().c_str());
        char mesh_name[MED_TAILLE_NOM];
              
        strcpy (mesh_name, m_collection->getMesh(idomain)->getName().c_str());
        SCRUTE(m_collection->getMesh(idomain)->getName());
        error = med_2_3::MEDjointCr(fid,mesh_name, joint_name, desc, 
            idistant, distant_name);
        if (error==-1) cout << "erreur creation de joint "<<endl;
              
        /////////////////////////////////////////
        //writing node/node correspondency
        /////////////////////////////////////////
        int nbnodes=(m_collection->getCZ())[icz]->getNodeNumber();
        int* node_corresp=const_cast<int*>((m_collection->getCZ())[icz]->getNodeCorrespValue());
        
        /* Nodes are reordered so that the ordering on the local and the distant domain
         correspond. The chosen order is the natural ordering on the domain
         with lowest proc id*/
         if (m_collection->getSubdomainBoundaryCreates())
           if (idomain<idistant)
             jointSort(node_corresp, nbnodes, true);
           else
             jointSort(node_corresp, nbnodes, false);
          
        error=
    med_2_3::MEDjointEcr(fid, mesh_name, joint_name, node_corresp, nbnodes,
             med_2_3::MED_NOEUD, med_2_3::MED_POINT1,med_2_3::MED_NOEUD, med_2_3::MED_POINT1);
        if (error==-1) cout << "erreur creation de joint "<<endl;
        
        //writing cell/cell joint      
        writeElementJoint(MED_EN::MED_CELL, icz, idomain, idistant, mesh_name,joint_name,fid);
        //writing face/face joint
        if (m_collection->getSubdomainBoundaryCreates())
        {
          MED_EN::medEntityMesh constituent_entity =
            (m_collection->getMeshDimension()==3)?MED_EN::MED_FACE:MED_EN::MED_EDGE;
          writeElementJoint(constituent_entity, icz, idomain, idistant, mesh_name,joint_name,fid);                 
        }                   
        index_joint++;
      }
  }
  
      // Writing cell global numbering
      // 
      int ncell=m_collection->getTopology()->getCellNumber(idomain);
      int * array=new int[ncell];
      m_collection->getTopology()->getCellList(idomain,array);
      int offset=0;
    
      MED_EN::MESH_ENTITIES::const_iterator currentEntity;
      list<MED_EN::medGeometryElement>::const_iterator iter;
      currentEntity  = MED_EN::meshEntities.find(MED_EN::MED_CELL);
      char meshchar[MED_TAILLE_NOM];
      strcpy(meshchar,(m_collection->getMesh())[idomain]->getName().c_str());
      int nbtypes = (m_collection->getMesh())[idomain]->getNumberOfTypesWithPoly(MED_EN::MED_CELL);
      MED_EN::medGeometryElement* types =(m_collection->getMesh())[idomain]->getTypesWithPoly(MED_EN::MED_CELL);
      for (int itype=0; itype<nbtypes;itype++)
    {
    MED_EN::medGeometryElement type=types[itype];
    if (!m_collection->isDimensionOK(type,m_collection->getMeshDimension())) continue;
    int ntype = (m_collection->getMesh())[idomain]->getNumberOfElementsWithPoly(MED_EN::MED_CELL,type);
    if (ntype==0) continue;
    med_2_3::MEDglobalNumEcr(fid,meshchar, array+offset, ntype,
           med_2_3::MED_MAILLE, (med_2_3::med_geometrie_element)type);
    offset+=ntype;
          
  }
      delete[] types;
      delete[] array;
    
      MED_EN::medEntityMesh constituent_entity;
      if (m_collection->getMeshDimension()==3)
       constituent_entity=MED_EN::MED_FACE;
       else if (m_collection->getMeshDimension()==2)
       constituent_entity=MED_EN::MED_EDGE;
       else throw MEDEXCEPTION("Wrong dimension");
       
       
      //writing face global numbering
    
      offset=0;
      int nface= m_collection->getTopology()->getFaceNumber(idomain);
      if (nface >0)
        array=new int[nface];
      m_collection->getTopology()->getFaceList(idomain,array);
      strcpy(meshchar,(m_collection->getMesh())[idomain]->getName().c_str());
      int nbfacetypes = (m_collection->getMesh())[idomain]->getNumberOfTypesWithPoly(constituent_entity);
      MED_EN::medGeometryElement* facetypes;
      if (nbfacetypes>0) 
        facetypes =(m_collection->getMesh())[idomain]->getTypesWithPoly(constituent_entity);
     
      for (int itype=0; itype<nbfacetypes;itype++)
    {
    MED_EN::medGeometryElement type=facetypes[itype];
    if (!m_collection->isDimensionOK(type,m_collection->getMeshDimension()-1)) continue;
  
    int ntype = (m_collection->getMesh())[idomain]->getNumberOfElementsWithPoly(constituent_entity,type);
    if (ntype==0) continue;
    med_2_3::MEDglobalNumEcr(fid,meshchar, array+offset, ntype,
           med_2_3::MED_MAILLE, (med_2_3::med_geometrie_element)type);
    offset+=ntype;
  }
  if (nface>0) delete[] array;
  if (nbfacetypes>0) delete[] facetypes;
  
  //writing node global numbering
  
  int nnode= m_collection->getTopology()->getNodeNumber(idomain);
  array=new int[nnode];
  m_collection->getTopology()->getNodeList(idomain,array);
  med_2_3::MEDglobalNumEcr(fid,meshchar, array, nnode,
         med_2_3::MED_NOEUD, med_2_3::MED_POINT1);
  
  delete[] array;
  med_2_3::MEDfermer(fid);
  MESSAGE("End of writing");
  
}

void MESHCollectionDriver::writeElementJoint(medEntityMesh entity ,
                                             int icz, 
                                             int idomain, 
                                             int idistant, 
                                             char* mesh_name, 
                                             char* joint_name,  
                                             med_2_3::med_idt fid )
{
  //////////////////////////////////////////
  //writing cell/cell correspondency
  //////////////////////////////////////////
  int nbcells=(m_collection->getCZ())[icz]->getEntityCorrespNumber(entity,entity);
  const int* index = (m_collection->getCZ())[icz]->getEntityCorrespIndex(entity,entity);
  const int* value = (m_collection->getCZ())[icz]->getEntityCorrespValue(entity,entity);
  
  map <pair <MED_EN::medGeometryElement, MED_EN::medGeometryElement> , vector<int> > cellmap;
  map <MED_EN::medGeometryElement, int> local_offset;
  map <MED_EN::medGeometryElement, int> distant_offset;
 
  //definition of the local offsets for the types present on local
  //and distant domains
  // for a mesh containing 2 triangles and 3 quads
  //local_offset[TRIA3]=0
  //local_offset[QUAD4]=2
 
  int nb_types_local=(m_collection->getMesh())[idomain]-> getNumberOfTypes(entity);
  const MED_EN::medGeometryElement* local_types = (m_collection->getMesh())[idomain]->getTypes(entity);
  const int* local_gni = (m_collection->getMesh())[idomain]-> getGlobalNumberingIndex(entity);
  for (int i=0; i< nb_types_local; i++)
    {
      local_offset[local_types[i]]=local_gni[i]-1;
    }                                      
  
  int nb_types_distant=(m_collection->getMesh())[idistant]-> getNumberOfTypes(entity);
  const MED_EN::medGeometryElement* distant_types = (m_collection->getMesh())[idistant]->getTypes(entity);
  const int* distant_gni = (m_collection->getMesh())[idistant]-> getGlobalNumberingIndex(entity);
  for (int i=0; i< nb_types_distant; i++)
    {
      distant_offset[distant_types[i]]=distant_gni[i]-1;
    } 
  
  //classifying all thje cell/cell relationships into geomtype/geomtype relationships
  //there exists a vector for each geomtype/geomtype pair
  // the vectors are stored in cellmap, a std::map with a pair<geomtype,geomtype> key 

  for (int i=0; i<nbcells; i++)
    for (int icol = index[i]-1; icol<index[i+1]-1; icol++)
    {
      MED_EN::medGeometryElement local_type =  (m_collection->getMesh())[idomain]->getElementType(entity,i+1);
      MED_EN::medGeometryElement distant_type = (m_collection->getMesh())[idistant]->getElementType(entity,value[icol]);

      cellmap[make_pair(local_type, distant_type)].push_back(i+1-local_offset[local_type]);
      cellmap[make_pair(local_type, distant_type)].push_back(value[icol]-distant_offset[distant_type]);
    }
  map <pair <MED_EN::medGeometryElement, MED_EN::medGeometryElement> , vector<int> >::const_iterator iter;

  //going through all the (geom,geom) pairs and writing the joints
  for (iter= cellmap.begin(); iter != cellmap.end(); iter++)
  {
    int size= iter->second.size();
    int *corresp = new int[size];
    for (int ind=0; ind < size; ind++)
      corresp[ind]=(iter->second)[ind];
    med_2_3::med_geometrie_element local_geo_elem=(med_2_3::med_geometrie_element)iter->first.first;
    med_2_3::med_geometrie_element distant_geo_elem=(med_2_3::med_geometrie_element)iter->first.second;
    int size_joint=size/2;
    //med_2_3::med_err error =
      med_2_3::MEDjointEcr(fid, mesh_name, joint_name, corresp, size_joint, med_2_3::MED_MAILLE,
                           local_geo_elem,med_2_3::MED_MAILLE, distant_geo_elem);

    // MED v 2.3.1 returns an error code when
    // writing a joint that is already present in the file.
    // Also, it returns an error code if a joint
    // concerns 3D elements.
    // Until these two items are not
    // changed, the following line must be commented out

    //if (error==-1) throw MEDEXCEPTION("Error filling joint");

    delete[]corresp;
  }
}

void MESHCollectionDriver::jointSort(int* elems, int nbelems, bool is_first)
{
  //filling an ordered structure with the elem ids
  map <int,int> nodemap;
  if (is_first)
    for (int i=0; i<nbelems; i++) 
      nodemap.insert(make_pair(elems[2*i],elems[2*i+1]));
 
  else
    for (int i=0; i<nbelems; i++)
      nodemap.insert(make_pair(elems[2*i+1],elems[2*i]));

  int* ptr_elems=elems;      
  
  //filling the vector in appropriate order 
  for (map<int,int>::const_iterator iter=nodemap.begin(); iter!=nodemap.end(); iter++)
  {
    if (is_first)
    {
      *ptr_elems++=iter->first; 
      *ptr_elems++=iter->second;
    }
    else
    {
      *ptr_elems++=iter->second;
      *ptr_elems++=iter->first;
    }
  }     
  
}
