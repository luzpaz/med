
#include <vector>
#include <string>
#include <map>
#include <set>

#include <iostream>
#include <fstream>

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
#include "MEDSPLITTER_MESHCollectionDriver.H"


MESHCollectionDriver::MESHCollectionDriver(MESHCollection* collection):m_collection(collection)
{
}

/*!reads a MED File v>=2.3
 * and mounts the corresponding meshes in memory
 * the connect zones are created from the joints
 * 
 *\param filename ascii file containing the list of MED v2.3 files
 * */
 
int MESHCollectionDriver::read(char* filename)
{

  BEGIN_OF("MEDSPLITTER::MESHCollectionDriver::read()")
  
  //ditributed meshes
  vector<int*> cellglobal;
  vector<int*> nodeglobal;
  vector<int*> faceglobal;
  
  // reading ascii master file
  try{
  	MESSAGE("Start reading");
    ifstream asciiinput(filename);
    
    if (!asciiinput)     
        throw MEDEXCEPTION("MEDSPLITTER read - Master File does not exist");
    
    char charbuffer[512];
    asciiinput.getline(charbuffer,512);
    
    while (charbuffer[0]=='#')
      {
	asciiinput.getline(charbuffer,512);
     }

    //reading number of domains
    int nbdomain=atoi(charbuffer);
    cout << "nb domain"<<nbdomain<<endl;
    //    asciiinput>>nbdomain;
    m_filename.resize(nbdomain);
    (m_collection->getMesh()).resize(nbdomain);
    cellglobal.resize(nbdomain);
    nodeglobal.resize(nbdomain);
    faceglobal.resize(nbdomain);
    for (int i=0; i<nbdomain;i++)
      {
	//reading information about the domain
		string mesh;
		int idomain;
		string host;
      	string meshstring;
  		char file[256];
  		char meshname[MED_TAILLE_NOM];
  
		asciiinput >> mesh >> idomain >> meshstring >> host >> m_filename[i];
		
		//Setting the name of the global mesh (which is the same
		//for all the subdomains)
		if (i==0)
			m_collection->setName(mesh);
		
		if (idomain!=i+1)
		  {
		    cerr<<"Error : domain must be written from 1 to N in asciifile descriptor"<<endl;
		    return 1;
		  }
		strcpy(meshname,meshstring.c_str());
		strcpy(file,m_filename[i].c_str());
		(m_collection->getMesh())[i]=new MEDMEM::MESH(MEDMEM::MED_DRIVER,file, meshname);
		
		//reading MEDSPLITTER::CONNECTZONEs NODE/NODE and CELL/CELL
		med_2_2::med_idt fid = med_2_2::MEDouvrir(file,med_2_2::MED_LECTURE);
     	med_2_2::med_int njoint = med_2_2::MEDnJoint(fid, meshname);
     	for (int ijoint=0; ijoint<njoint; ijoint++)
     	{
			int distant;
			char joint_description[MED_TAILLE_DESC];
		    char name[MED_TAILLE_NOM];
		    char name_distant[MED_TAILLE_NOM];
		    
		    int ncorr = med_2_2::MEDjointInfo(fid,meshname, ijoint+1, name, 
		    	joint_description,
			       &distant, name_distant);
	
	    	for (int ic=0; ic<ncorr; ic++)
		    {
		      med_2_2::med_entite_maillage cor_typent_local;
		      med_2_2::med_geometrie_element cor_typgeo_local;
		      med_2_2::med_entite_maillage cor_typent_dist;
		      med_2_2::med_geometrie_element cor_typgeo_dist;
		  
		   
		      int ncouples;
		      ncouples = med_2_2::MEDjointTypeCorres(fid, meshname, name, ic+1,
						    &cor_typent_local,  &cor_typgeo_local,
						    &cor_typent_dist, &cor_typgeo_dist
						    );
		      int* node_corresp=new int[ncouples];
		      if (cor_typent_local == med_2_2::MED_NOEUD && cor_typent_dist == med_2_2::MED_NOEUD)
				{
		  
				  med_2_2::MEDjointLire(fid, meshname, name,
			       node_corresp,ncouples,
			       cor_typent_local,  cor_typgeo_local,
			       cor_typent_dist, cor_typgeo_dist
			       );
				}
				//constructing the connect zone and adding it to the connect zone list
				MEDMEM::CONNECTZONE* cz = new MEDMEM::CONNECTZONE();
				cz->setName(string(name));
				cz->setDescription(joint_description);
				cz->setLocalDomainNumber(i);
				cz->setDistantDomainNumber(distant);
				cz->setLocalMesh((m_collection->getMesh())[i]);
				cz->setDistantMesh((m_collection->getMesh())[distant]);
				cz->setNodeCorresp(node_corresp,ncouples);
				(m_collection->getCZ()).push_back(cz);
				
		    }//loop on correspom_topology->nbDomain())ndances
		}//loop on joints	
		
		// Reading global numbering
		// 
		int ncell=(m_collection->getMesh())[i]->getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
		int * array=new int[ncell];
		int offset=0;
		MESSAGE("Reading cell global numbering for mesh "<< i);
		MED_EN::MESH_ENTITIES::const_iterator currentEntity;
		list<MED_EN::medGeometryElement>::const_iterator iter;
		currentEntity  = MED_EN::meshEntities.find(MED_EN::MED_CELL);
		char meshchar[MED_TAILLE_NOM];
		strcpy(meshchar,(m_collection->getMesh())[i]->getName().c_str());
		for (iter = (*currentEntity).second.begin();iter != (*currentEntity).second.end(); iter++)
		{
			MED_EN::medGeometryElement type=*iter;
			if (type/100 != m_collection->getMeshDimension()) continue;
			int ntype = (m_collection->getMesh())[i]->getNumberOfElements(MED_EN::MED_CELL,type);
			if (ntype==0) continue;
			med_2_2::MEDglobalNumLire(fid,meshname, array+offset, ntype,
		        med_2_2::MED_MAILLE, (med_2_2::med_geometrie_element)type);
	        offset+=ntype;
		}
		cellglobal[i]=array;
		
		MESSAGE("Reading node global numbering");
		int nnode= (m_collection->getMesh())[i]->getNumberOfNodes();
		array=new int[nnode];
		med_2_2::MEDglobalNumLire(fid,meshname, array, nnode,
			        med_2_2::MED_NOEUD, med_2_2::MED_POINT1); 
		nodeglobal[i]=array;
		
		MESSAGE("Reading face global numbering for mesh "<<i);
		int nbface=(m_collection->getMesh())[i]->getNumberOfElements(MED_EN::MED_FACE,MED_EN::MED_ALL_ELEMENTS);
		array=new int[nbface];
		currentEntity  = MED_EN::meshEntities.find(MED_EN::MED_FACE);
		offset=0;
		for (iter = (*currentEntity).second.begin();iter != (*currentEntity).second.end(); iter++)
		{
			MED_EN::medGeometryElement type=*iter;
			if (type/100 != m_collection->getMeshDimension()-1) continue;
			int ntype = (m_collection->getMesh())[i]->getNumberOfElements(MED_EN::MED_FACE,type);
			if (ntype==0) continue;
			med_2_2::MEDglobalNumLire(fid,meshname, array+offset, ntype,
		        med_2_2::MED_FACE, (med_2_2::med_geometrie_element)type);
	        offset+=ntype;
		}
		faceglobal[i]=array;
//		faceglobal[i]=0;
	    med_2_2::MEDfermer(fid);
      }//loop on domains
      MESSAGE("end of read");
  }//of try
  catch(...)
    {
      cerr << "I/O error reading parallel MED file"<<endl;
      throw;
    }
    
  //creation of topology from mesh and connect zones
  m_collection->setTopology(
  	new ParallelTopology((m_collection->getMesh()),(m_collection->getCZ()),cellglobal,nodeglobal,faceglobal)
    );
    
    END_OF("MEDSPLITTER::MESHCollectionDriver::read()")
    return 0;
}

/*!reads a unique MED File v>=2.1
 * and mounts the corresponding mesh in memory
 *\param filename binary file
 *\param meshname mesh name in the MED file
 * */
int MESHCollectionDriver::readSeq(char* filename, char* meshname)
{
	
   BEGIN_OF("MEDSPLITTER::MESHCollectionDriver::readSeq()")
     
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
   m_collection->setTopology(
			     new ParallelTopology((m_collection->getMesh()),(m_collection->getCZ()),cellglobal,nodeglobal,faceglobal)
			     );
   END_OF("MEDSPLITTER::MESHCollectionDriver::readSeq()")
   return 0;
}

/*! writes the collection of meshes in a 
 * MED v2.3 file
 * with the connect zones being written as joints
 * \param filename name of the ascii file containing the meshes description
 */
void MESHCollectionDriver::write(char* filename)
{
	
  	BEGIN_OF("MEDSPLITTER::MESHCollectionDriver::write()")
 
	ofstream file(filename);

	file <<"#MED Fichier V 2.3"<<" "<<endl;
	file <<"#"<<" "<<endl;
	file<<m_collection->getMesh().size()<<" "<<endl;
	 
	int nbdomains= m_collection->getMesh().size();
	m_filename.resize(nbdomains);
	//loop on the domains
	for (int i=0; i<nbdomains;i++)
	{
		char distfilename[256];
	
		ostringstream suffix;
		suffix << filename<< i+1 <<".med";
		
		strcpy(distfilename,suffix.str().c_str());

		m_filename[i]=string(distfilename);
		
		MESSAGE("File name "<<string(distfilename));
		
		int id=(m_collection->getMesh())[i]->addDriver(MEDMEM::MED_DRIVER,distfilename,(m_collection->getMesh())[i]->getName());
		
		MESSAGE("Start writing");
		(m_collection->getMesh())[i]->write(id);
		
		//updating the ascii description file
		file << m_collection->getName() <<" "<< i+1 << " "<< (m_collection->getMesh())[i]->getName() << " localhost " << distfilename << " "<<endl;
	
	//build connect zones
		if (nbdomains>1)
			m_collection->buildConnectZones(i);
		
		MESSAGE(" Number of connect zones "<<(m_collection->getCZ()).size());
				
		//writing connect zones in joints
		
		med_2_2::med_idt fid = med_2_2::MEDouvrir(distfilename,med_2_2::MED_LECTURE_ECRITURE);	
		
		int index_joint=0;
		for (int icz=0; icz<(m_collection->getCZ()).size(); icz++)
		{
			if ((m_collection->getCZ())[icz]->getLocalDomainNumber()==i)
			{
				
				char joint_name[MED_TAILLE_NOM];
				sprintf(joint_name,"joint%i",index_joint+1);
				char desc[MED_TAILLE_DESC];
				sprintf(desc,"connect_zone_%d",icz+1);
				
				char distant_name[MED_TAILLE_NOM];
				sprintf(distant_name,"domain_%i",(m_collection->getCZ())[icz]->getDistantDomainNumber());
				char mesh_name[MED_TAILLE_NOM];

				strcpy (mesh_name, (m_collection->getMesh())[i]->getName().c_str());
				med_2_2::med_err err1 = med_2_2::MEDjointCr(fid,mesh_name, joint_name, desc, 
					(m_collection->getCZ())[icz]->getDistantDomainNumber(), distant_name);
				if (err1==1) cout << "erreur creation de joint "<<endl;
				int nbnodes=(m_collection->getCZ())[icz]->getNodeNumber();
				int* node_corresp=const_cast<int*>((m_collection->getCZ())[icz]->getNodeCorrespValue());
				
				med_err err2=
				med_2_2::MEDjointEcr(fid, mesh_name, joint_name, node_corresp, nbnodes,
	   			    med_2_2::MED_NOEUD, med_2_2::MED_POINT1,med_2_2::MED_NOEUD, med_2_2::MED_POINT1);
	   			if (err2==1) cout << "erreur creation de joint "<<endl;
	   			index_joint++;
			}
		}
		// Writing cell global numbering
		// 
		int ncell=m_collection->getTopology()->getCellNumber(i);
		int * array=new int[ncell];
		m_collection->getTopology()->getCellList(i,array);
		int offset=0;
		
		MED_EN::MESH_ENTITIES::const_iterator currentEntity;
		list<MED_EN::medGeometryElement>::const_iterator iter;
		currentEntity  = MED_EN::meshEntities.find(MED_EN::MED_CELL);
		char meshchar[MED_TAILLE_NOM];
		strcpy(meshchar,(m_collection->getMesh())[i]->getName().c_str());
		for (iter = (*currentEntity).second.begin();iter != (*currentEntity).second.end(); iter++)
		{
			MED_EN::medGeometryElement type=*iter;
			if (type/100 != m_collection->getMeshDimension()) continue;
			int ntype = (m_collection->getMesh())[i]->getNumberOfElements(MED_EN::MED_CELL,type);
			if (ntype==0) continue;
			med_2_2::MEDglobalNumEcr(fid,meshchar, array+offset, ntype,
		        med_2_2::MED_MAILLE, (med_2_2::med_geometrie_element)type);
	        offset+=ntype;
	        
		}
		delete[] array;
		
		//writing face global numbering
		
		offset=0;
		int nface= m_collection->getTopology()->getFaceNumber(i);
		array=new int[nface];
		m_collection->getTopology()->getFaceList(i,array);
		currentEntity  = MED_EN::meshEntities.find(MED_EN::MED_FACE);
		strcpy(meshchar,(m_collection->getMesh())[i]->getName().c_str());
		for (iter = (*currentEntity).second.begin();iter != (*currentEntity).second.end(); iter++)
		{
			MED_EN::medGeometryElement type=*iter;
			if (type/100 != m_collection->getMeshDimension()-1) continue;
			int ntype = (m_collection->getMesh())[i]->getNumberOfElements(MED_EN::MED_FACE,type);
			if (ntype==0) continue;
			med_2_2::MEDglobalNumEcr(fid,meshchar, array+offset, ntype,
		        med_2_2::MED_FACE, (med_2_2::med_geometrie_element)type);
	        offset+=ntype;
		}
		delete[] array;
		
		//writing node global numbering
		
		int nnode= m_collection->getTopology()->getNodeNumber(i);
		array=new int[nnode];
		m_collection->getTopology()->getNodeList(i,array);
		med_2_2::MEDglobalNumEcr(fid,meshchar, array, nnode,
			        med_2_2::MED_NOEUD, med_2_2::MED_POINT1);
		
		delete[] array;
		med_2_2::MEDfermer(fid);
		MESSAGE("End of writing");
	}
	
	END_OF("MEDSPLITTER::MESHCollectionDriver::write()");

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
 	BEGIN_OF("MEDSPLITTER::MESHCollectionDriver::readFileStruct()")
 
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
	END_OF("MEDSPLITTER::MESHCollectionDriver::readFileStruct()")
}

//!retrieves the type of a field for a given fieldname
int MESHCollectionDriver::getFieldType(const string& fieldname)
{
	BEGIN_OF("MEDSPLITTER::MESHCollectionDriver::getFieldType()")
	const MEDMEM::MED med_struct (MEDMEM::MED_DRIVER,m_filename[0]);

	deque<MEDMEM::DT_IT_> dtit=med_struct.getFieldIteration(fieldname);
	deque<MEDMEM::DT_IT_>::const_iterator iter =dtit.begin();
	
	
	// testing whether the field is of double or int type		
	MEDMEM::FIELD_* field = med_struct.getField(fieldname,iter->dt,iter->it);

	if (dynamic_cast<MEDMEM::FIELD<double>*>(field))
	  return 1;
	else
	  return 0;			
	   	
	END_OF("MEDSPLITTER::MESHCollectionDriver::getFieldType()")

}
