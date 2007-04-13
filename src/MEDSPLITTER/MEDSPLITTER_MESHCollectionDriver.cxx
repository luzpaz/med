
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

  int nbdomain;

  // reading ascii master file
  try {
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
    nbdomain=atoi(charbuffer);
    cout << "nb domain"<<nbdomain<<endl;
    //    asciiinput>>nbdomain;
    m_filename.resize(nbdomain);
    (m_collection->getMesh()).resize(nbdomain);
    cellglobal.resize(nbdomain);
    nodeglobal.resize(nbdomain);
    faceglobal.resize(nbdomain);
    for (int i=0; i<nbdomain; i++)
    {
      //reading information about the domain
      string mesh;
      int idomain;
      string host;
      string meshstring;
      char file[256];
      char meshname[MED_TAILLE_NOM];
      cellglobal[i]=0;
      faceglobal[i]=0;
      nodeglobal[i]=0;

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
          med_2_2::med_int* node_corresp = new med_2_2::med_int[ncouples];
          if (cor_typent_local == med_2_2::MED_NOEUD && cor_typent_dist == med_2_2::MED_NOEUD)
          {
            med_2_2::MEDjointLire(fid, meshname, name,
                                  node_corresp, ncouples,
                                  cor_typent_local, cor_typgeo_local,
                                  cor_typent_dist, cor_typgeo_dist
                                  );
          }
          int* int_node_corresp = new int[ncouples];
          for (int inc = 0; inc < ncouples; inc++)
          {
            int_node_corresp[inc] = node_corresp[inc];
          }
          //constructing the connect zone and adding it to the connect zone list
          MEDMEM::CONNECTZONE* cz = new MEDMEM::CONNECTZONE();
          cz->setName(string(name));
          cz->setDescription(joint_description);
          cz->setLocalDomainNumber(i);
          cz->setDistantDomainNumber(distant);
          cz->setLocalMesh((m_collection->getMesh())[i]);
          cz->setDistantMesh((m_collection->getMesh())[distant]);
          cz->setNodeCorresp(int_node_corresp, ncouples);
          (m_collection->getCZ()).push_back(cz);

          delete [] int_node_corresp;
          delete [] node_corresp;
        }//loop on correspondances
      }//loop on joints

      // Reading global numbering
      //
      int ncell=(m_collection->getMesh())[i]->getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
      med_2_2::med_int * array = new med_2_2::med_int[ncell];
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
        if (ntype == 0) continue;
        med_2_2::MEDglobalNumLire(fid, meshname, array+offset, ntype,
                                  med_2_2::MED_MAILLE, (med_2_2::med_geometrie_element)type);
        offset += ntype;
      }
      int * int_array = new int[ncell];
      for (int ic = 0; ic < ncell; ic++) {
        int_array[ic] = array[ic];
      }
      delete [] array;
      cellglobal[i] = int_array;

      MESSAGE("Reading node global numbering");
      int nnode = (m_collection->getMesh())[i]->getNumberOfNodes();
      array = new med_2_2::med_int[nnode];
      med_2_2::MEDglobalNumLire(fid, meshname, array, nnode, med_2_2::MED_NOEUD, med_2_2::MED_POINT1);
      int_array = new int[nnode];
      for (int in = 0; in < nnode; in++) {
        int_array[in] = array[in];
      }
      delete [] array;
      nodeglobal[i] = int_array;

      MESSAGE("Reading face global numbering for mesh "<<i);
      int nbface = (m_collection->getMesh())[i]->getNumberOfElements(MED_EN::MED_FACE,
                                                                     MED_EN::MED_ALL_ELEMENTS);
      if (nbface != 0)
      {
        array = new med_2_2::med_int[nbface];
        currentEntity = MED_EN::meshEntities.find(MED_EN::MED_FACE);
        offset = 0;
        for (iter = (*currentEntity).second.begin(); iter != (*currentEntity).second.end(); iter++)
        {
          MED_EN::medGeometryElement type = *iter;
          if (type/100 != m_collection->getMeshDimension()-1) continue;
          int ntype = (m_collection->getMesh())[i]->getNumberOfElements(MED_EN::MED_FACE, type);
          if (ntype == 0) continue;
          med_2_2::MEDglobalNumLire(fid, meshname, array+offset, ntype,
                                    med_2_2::MED_FACE, (med_2_2::med_geometrie_element)type);
          offset += ntype;
        }
        int_array = new int[nbface];
        for (int ifa = 0; ifa < nbface; ifa++) {
          int_array[ifa] = array[ifa];
        }
        delete [] array;
        faceglobal[i] = int_array;
      }
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
  m_collection->setTopology(new ParallelTopology((m_collection->getMesh()),(m_collection->getCZ()),
                                                 cellglobal,nodeglobal,faceglobal)
                            );

  for (int i=0; i<nbdomain; i++)
  {
    if (cellglobal[i]!=0) delete[] cellglobal[i];
    if (nodeglobal[i]!=0) delete[] nodeglobal[i];
    if (faceglobal[i]!=0) delete[] faceglobal[i];
  }

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
  m_collection->setTopology(new ParallelTopology((m_collection->getMesh()), (m_collection->getCZ()),
                                                 cellglobal, nodeglobal, faceglobal));

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
  for (int idomain=0; idomain<nbdomains;idomain++)
  {
    char distfilename[256];

    ostringstream suffix;
    suffix << filename<< idomain+1 <<".med";

    strcpy(distfilename,suffix.str().c_str());

    m_filename[idomain]=string(distfilename);

    MESSAGE("File name "<<string(distfilename));

    int id = (m_collection->getMesh())[idomain]->addDriver(MEDMEM::MED_DRIVER,distfilename,
                                                           (m_collection->getMesh())[idomain]->getName());

    MESSAGE("Start writing");
    (m_collection->getMesh())[idomain]->write(id);

    //updating the ascii description file
    file << m_collection->getName() <<" "<< idomain+1 << " "<< (m_collection->getMesh())[idomain]->getName() << " localhost " << distfilename << " "<<endl;

    //build connect zones
    if (nbdomains>1)
      m_collection->buildConnectZones(idomain);

    MESSAGE(" Number of connect zones "<<(m_collection->getCZ()).size());

    //writing connect zones in joints

    med_2_2::med_idt fid = med_2_2::MEDouvrir(distfilename,med_2_2::MED_LECTURE_ECRITURE);	

    int index_joint=0;
    for (int icz=0; icz<(m_collection->getCZ()).size(); icz++)
    {
      if ((m_collection->getCZ())[icz]->getLocalDomainNumber()==idomain)
      {
        med_2_2::med_err error;
        int idistant=(m_collection->getCZ())[icz]->getDistantDomainNumber();
        char joint_name[MED_TAILLE_NOM];
        sprintf(joint_name,"joint%i",index_joint+1);
        char desc[MED_TAILLE_DESC];
        sprintf(desc,"connect_zone_%d",icz+1);

        char distant_name[MED_TAILLE_NOM];
        sprintf(distant_name,"domain_%i",(m_collection->getCZ())[icz]->getDistantDomainNumber());
        char mesh_name[MED_TAILLE_NOM];

        strcpy (mesh_name, (m_collection->getMesh())[idomain]->getName().c_str());
        error = med_2_2::MEDjointCr(fid,mesh_name, joint_name, desc, 
                                    idistant, distant_name);
        if (error==-1) cout << "erreur creation de joint "<<endl;

        /////////////////////////////////////////
        //writing node/node correspondency
        /////////////////////////////////////////
        int nbnodes = (m_collection->getCZ())[icz]->getNodeNumber();
        //int* node_corresp = const_cast<int*>((m_collection->getCZ())[icz]->getNodeCorrespValue());
        const int* int_node_corresp = (m_collection->getCZ())[icz]->getNodeCorrespValue();
        med_2_2::med_int* node_corresp = new med_2_2::med_int[nbnodes];
        for (int inc = 0; inc < nbnodes; inc++) {
          node_corresp[inc] = int_node_corresp[inc];
        }

        error = med_2_2::MEDjointEcr(fid, mesh_name, joint_name, node_corresp, nbnodes,
                                     med_2_2::MED_NOEUD, med_2_2::MED_POINT1,
                                     med_2_2::MED_NOEUD, med_2_2::MED_POINT1);
        if (error == -1) cout << "erreur creation de joint "<<endl;
        delete [] node_corresp;

        //////////////////////////////////////////
        //writing cell/cell correspondency
        //////////////////////////////////////////
        int nbcells = (m_collection->getCZ())[icz]->getEntityCorrespNumber(MED_EN::MED_CELL,
                                                                           MED_EN::MED_CELL);
        const int* index = (m_collection->getCZ())[icz]->getEntityCorrespIndex(MED_EN::MED_CELL,
                                                                               MED_EN::MED_CELL);
        const int* value = (m_collection->getCZ())[icz]->getEntityCorrespValue(MED_EN::MED_CELL,
                                                                               MED_EN::MED_CELL);

        map <pair <MED_EN::medGeometryElement, MED_EN::medGeometryElement> , vector<int> > cellmap;
        map <MED_EN::medGeometryElement, int> local_offset;
        map <MED_EN::medGeometryElement, int> distant_offset;

        //definition of the local offsets for the types present on local
        //and distant domains
        // for a mesh containing 2 triangles and 3 quads
        //local_offset[TRIA3]=0
        //local_offset[QUAD4]=2

        int nb_types_local = (m_collection->getMesh())[idomain]-> getNumberOfTypes(MED_EN::MED_CELL);
        const MED_EN::medGeometryElement* local_types =
          (m_collection->getMesh())[idomain]->getTypes(MED_EN::MED_CELL);
        const int* local_gni = (m_collection->getMesh())[idomain]->getGlobalNumberingIndex(MED_EN::MED_CELL);
        for (int i=0; i< nb_types_local; i++)
        {
          local_offset[local_types[i]] = local_gni[i]-1;
        }                                      

        int nb_types_distant=(m_collection->getMesh())[idistant]-> getNumberOfTypes(MED_EN::MED_CELL);
        const MED_EN::medGeometryElement* distant_types =
          (m_collection->getMesh())[idistant]->getTypes(MED_EN::MED_CELL);
        const int* distant_gni =
          (m_collection->getMesh())[idistant]->getGlobalNumberingIndex(MED_EN::MED_CELL);
        for (int i=0; i< nb_types_distant; i++)
        {
          distant_offset[distant_types[i]]=distant_gni[i]-1;
        } 

        //classifying all thje cell/cell relationships into geomtype/geomtype relationships
        //there exists a vector for each geomtype/geomtype pair
        // the vectors are stored in cellmap, a std::map with a pair<geomtype,geomtype> key 

        for (int i=0; i<nbcells; i++) {
          for (int icol = index[i]-1; icol<index[i+1]-1; icol++)
          {
            MED_EN::medGeometryElement local_type =
              (m_collection->getMesh())[idomain]->getElementType(MED_EN::MED_CELL,i+1);
            MED_EN::medGeometryElement distant_type =
              (m_collection->getMesh())[idistant]->getElementType(MED_EN::MED_CELL,value[icol]);

            cellmap[make_pair(local_type, distant_type)].push_back(i+1-local_offset[local_type]);
            cellmap[make_pair(local_type, distant_type)].push_back(value[icol]-distant_offset[distant_type]);
          }
        }
        map <pair <MED_EN::medGeometryElement, MED_EN::medGeometryElement> , vector<int> >::const_iterator iter;

        //going through all the (geom,geom) pairs and writing the joints
        for (iter = cellmap.begin(); iter != cellmap.end(); iter++)
        {
          int size = iter->second.size();
          med_2_2::med_int *corresp = new med_2_2::med_int[size];
          for (int ind = 0; ind < size; ind++)
            corresp[ind] = (iter->second)[ind];
          med_2_2::med_geometrie_element local_geo_elem=(med_2_2::med_geometrie_element)iter->first.first;
          med_2_2::med_geometrie_element distant_geo_elem=(med_2_2::med_geometrie_element)iter->first.second;
          int size_joint = size/2;
          error = med_2_2::MEDjointEcr(fid, mesh_name, joint_name, corresp, size_joint, med_2_2::MED_MAILLE,
                                       local_geo_elem, med_2_2::MED_MAILLE, distant_geo_elem);
          // MED v 2.3.1 returns an error code when
          // writing a joint that is already present in the file.
          // Also, it returns an error code if a joint 
          // concerns 3D elements.
          // Until these two items are not 
          // changed, the following line must be commented out

          //if (error==-1) throw MEDEXCEPTION("Error filling joint");

          delete [] corresp;              
        }

        index_joint++;
      }
    }

    // Writing cell global numbering
    //
    int ncell = m_collection->getTopology()->getCellNumber(idomain);
    int * int_array = new int[ncell];
    m_collection->getTopology()->getCellList(idomain, int_array);
    med_2_2::med_int * array = new med_2_2::med_int[ncell];
    for (int ic = 0; ic < ncell; ic++) {
      array[ic] = int_array[ic];
    }

    int offset = 0;
    MED_EN::MESH_ENTITIES::const_iterator currentEntity;
    list<MED_EN::medGeometryElement>::const_iterator iter;
    currentEntity  = MED_EN::meshEntities.find(MED_EN::MED_CELL);
    char meshchar[MED_TAILLE_NOM];
    strcpy(meshchar,(m_collection->getMesh())[idomain]->getName().c_str());
    for (iter = (*currentEntity).second.begin();iter != (*currentEntity).second.end(); iter++)
    {
      MED_EN::medGeometryElement type=*iter;
      if (type/100 != m_collection->getMeshDimension()) continue;
      int ntype = (m_collection->getMesh())[idomain]->getNumberOfElements(MED_EN::MED_CELL,type);
      if (ntype==0) continue;
      med_2_2::MEDglobalNumEcr(fid, meshchar, array+offset, ntype,
                               med_2_2::MED_MAILLE, (med_2_2::med_geometrie_element)type);
      offset += ntype;
    }
    delete [] int_array;
    delete [] array;

    //writing face global numbering

    int nface = m_collection->getTopology()->getFaceNumber(idomain);
    int_array = new int[nface];
    m_collection->getTopology()->getFaceList(idomain, int_array);
    array = new med_2_2::med_int[nface];
    for (int ifa = 0; ifa < nface; ifa++) {
      array[ifa] = int_array[ifa];
    }

    offset = 0;
    currentEntity = MED_EN::meshEntities.find(MED_EN::MED_FACE);
    strcpy(meshchar,(m_collection->getMesh())[idomain]->getName().c_str());
    for (iter = (*currentEntity).second.begin();iter != (*currentEntity).second.end(); iter++)
    {
      MED_EN::medGeometryElement type = *iter;
      if (type/100 != m_collection->getMeshDimension()-1) continue;
      int ntype = (m_collection->getMesh())[idomain]->getNumberOfElements(MED_EN::MED_FACE, type);
      if (ntype == 0) continue;
      med_2_2::MEDglobalNumEcr(fid, meshchar, array+offset, ntype,
                               med_2_2::MED_FACE, (med_2_2::med_geometrie_element)type);
      offset+=ntype;
    }
    delete [] int_array;
    delete [] array;

    //writing node global numbering

    int nnode = m_collection->getTopology()->getNodeNumber(idomain);
    int_array = new int[nnode];
    m_collection->getTopology()->getNodeList(idomain, int_array);
    array = new med_2_2::med_int[nnode];
    for (int in = 0; in < nnode; in++) {
      array[in] = int_array[in];
    }

    med_2_2::MEDglobalNumEcr(fid, meshchar, array, nnode,
                             med_2_2::MED_NOEUD, med_2_2::MED_POINT1);

    delete [] int_array;
    delete [] array;

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

  END_OF("MEDSPLITTER::MESHCollectionDriver::getFieldType()")
  
	if (dynamic_cast<MEDMEM::FIELD<double>*>(field))
	  return 1;
	else
	  return 0;			

	}
