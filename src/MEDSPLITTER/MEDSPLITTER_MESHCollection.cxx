#include <vector>
#include <string>
#ifndef WNT
# include <ext/hash_map>
#else
# include <hash_map>
#endif
#include <set>

#include <iostream>
#include <fstream>
 
#include "MEDMEM_ConnectZone.hxx"
#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_GaussLocalization.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_Group.hxx"

#include "MEDMEM_Exception.hxx"

#include "MEDSPLITTER_utils.hxx" 

#include "MEDSPLITTER_Graph.hxx"

#include "MEDSPLITTER_Topology.hxx"
#include "MEDSPLITTER_ParallelTopology.hxx"
#include "MEDSPLITTER_SequentialTopology.hxx"

#include "MEDSPLITTER_METISGraph.hxx"
#include "MEDSPLITTER_SCOTCHGraph.hxx"
#include "MEDSPLITTER_UserGraph.hxx"

#include "MEDSPLITTER_MESHCollection.hxx"
#include "MEDSPLITTER_MESHCollectionDriver.hxx"

using namespace MEDSPLITTER;

#ifndef WNT
using namespace __gnu_cxx;
#else
using namespace std;
#endif

//template inclusion
#include "MEDSPLITTER_MESHCollection.H"
#include "MEDSPLITTER_MESHCollectionDriver.H"



MESHCollection::MESHCollection():m_topology(0),m_driver(0){}

/*!constructor creating a new mesh collection (mesh series + topology)
 *from an old collection and a new topology
 * 
 * On output, the constructor has built the meshes corresponding to the new mesh collection.
 * The new topology has been updated so that face and node mappings are included.
 * The families have been cast to their projections in the new topology.
 * 
 * \param initial_collection collection from which the data (coordinates, connectivity) are taken
 * \param topology topology containing the cell mappings
 */

MESHCollection::MESHCollection(const MESHCollection& initial_collection, Topology* topology)
	:m_name(initial_collection.m_name),m_topology(topology),m_owns_topology(false),m_cell_graph(topology->getGraph()),m_driver(0)
{
	string mesh_name = initial_collection.getName();
	m_mesh.resize(m_topology->nbDomain());
	for (int idomain=0; idomain < m_topology->nbDomain(); idomain++)
		{
			//creating the new mesh
			MEDMEM::MESHING* mesh_builder=new MEDMEM::MESHING;
			m_mesh[idomain]= static_cast<MEDMEM::MESH*> (mesh_builder);
			ostringstream osname;
			osname << mesh_name<<"_"<<idomain+1;
			mesh_builder->setName(osname.str());
		
			createNodalConnectivity(initial_collection,idomain, MED_EN::MED_CELL);
			mesh_builder->setMeshDimension(initial_collection.getMeshDimension());
			mesh_builder->setSpaceDimension(initial_collection.getSpaceDimension());
		}
	
	m_topology->createFaceMapping(initial_collection);	
	for (int idomain=0; idomain<m_topology->nbDomain(); idomain++)
		{
			switch (getMeshDimension())
				{
				case 3:
					createNodalConnectivity(initial_collection,idomain, MED_EN::MED_FACE);
					break;
				case 2:
					createNodalConnectivity(initial_collection,idomain, MED_EN::MED_EDGE);
					break;
				default : 
					cerr<<"MEDSPLITTER : Mesh dimension must be 2 or 3"<<endl;
				}
		}
	
  castFamilies(initial_collection);
}

/*! constructing the MESH collection from a distributed file
 * 
 * \param filename name of the master file containing the list of all the MED files
 */
MESHCollection::MESHCollection(const string& filename):m_topology(0),m_owns_topology(true),m_driver(0)
{
	char filenamechar[256];
	strcpy(filenamechar,filename.c_str());
	retrieveDriver()->read (filenamechar);
}

/*! constructing the MESH collection from a sequential MED-file
 * 
 * \param filename MED file
 * \param meshname name of the mesh that is to be read
 */
MESHCollection::MESHCollection(const string& filename, const string& meshname):m_name(meshname),m_topology(0),m_owns_topology(true),m_driver(0)
{
	char filenamechar[256];
	char meshnamechar[256];
	strcpy(filenamechar,filename.c_str());
	strcpy(meshnamechar,meshname.c_str());
	retrieveDriver()->readSeq (filenamechar,meshnamechar);
}

MESHCollection::~MESHCollection()
{
	for (int i=0; i<m_mesh.size();i++)
	  if (m_mesh[i]!=0) {delete m_mesh[i]; m_mesh[i]=0;}
	for (int i=0; i<m_connect_zones.size();i++)
	  if (m_connect_zones[i]!=0) {delete m_connect_zones[i]; m_connect_zones[i]=0;}
	if (m_driver !=0) {delete m_driver; m_driver=0;}
	if (m_topology!=0 && m_owns_topology) {delete m_topology; m_topology=0;}
	
}

/*! constructing the MESH collection from a file
 * 
 * The method creates as many MED-files as there are domains in the 
 * collection. It also creates a master file that lists all the MED files.
 * The MED files created in ths manner contain joints that describe the 
 * connectivity between subdomains.
 * 
 * \param filename name of the master file that will contain the list of the MED files
 * 
 */
void MESHCollection::write(const string& filename)
{
	char filenamechar[256];
	strcpy(filenamechar,filename.c_str());
	retrieveDriver()->write (filenamechar);
}

/*! creates or gets the link to the collection driver
 */
MESHCollectionDriver* MESHCollection::retrieveDriver()
{
	if (m_driver==0)
		{
			m_driver=new MESHCollectionDriver(this);
		}
	
	return m_driver;
}

/*! gets an existing driver
 * 
 */
MESHCollectionDriver* MESHCollection::getDriver() const
{
	return m_driver;
}


/*! gets the list of types for global numbers cell_list
 * 
 * \param cell_list list of global numbers
 * \param entity entity type
 * \param type_list on output, list of types for the cells given in cell_list
 */
void MESHCollection::getTypeList(int* cell_list,int nb_cells,
																 MED_EN::medEntityMesh entity,
																 MED_EN::medGeometryElement* type_list) const 
{
	int *local=new int[nb_cells];
	int *ip=new int[nb_cells];
	switch (entity)
		{
		case MED_EN::MED_CELL:
			m_topology->convertGlobalCellList(cell_list,nb_cells,local,ip);
			break;
		case MED_EN::MED_FACE:
		case MED_EN::MED_EDGE:
			m_topology->convertGlobalFaceList(cell_list,nb_cells,local,ip);
			break;
		}
	
	for (int icell=0; icell<nb_cells; icell++)
		{
			type_list[icell]=m_mesh[ip[icell]]->getElementType(entity,local[icell]);													
		}
	delete[]local;
	delete[]ip;
}

/*!gets the connectivity for a certain type
 *
 * The output array type_connectivity should have been allocated
 * at dimension nbnode_per_type* nb_cells before the call
 * 
 * \param cell_list list of elements (global cell numbers) for which the connectivity is required
 * \param nb_cells number of elements
 * \param entity type of entity for which the nodal connectivity is required
 * \param type type of the elements for which the connectivity is required
 * \param type_connectivity on output contains the connectivity of all the elements of the list
 * */ 
 
void MESHCollection::getNodeConnectivity(const int* cell_list,int nb_cells,MED_EN::medEntityMesh entity,
																				 MED_EN::medGeometryElement type, int* type_connectivity) const
{
	int *local=new int[nb_cells];
	int *ip=new int[nb_cells];
	switch (entity)
		{
		case MED_EN::MED_CELL:
			m_topology->convertGlobalCellList(cell_list,nb_cells,local,ip);
			break;
		case MED_EN::MED_FACE:
		case MED_EN::MED_EDGE:
			m_topology->convertGlobalFaceList(cell_list,nb_cells,local,ip);
			break;
		}
	
	
	int nbnode_per_type=(int)type%100;
	vector<int> number_of_types_array(m_topology->nbDomain(),0);
	for (int i=0; i<m_topology->nbDomain(); i++)
		number_of_types_array[i]=m_mesh[i]->getNumberOfTypes(entity);
		
	//defining a connectivity table for different domains	
	vector  <const int*> conn_ip(m_topology->nbDomain());
	for (int i=0; i<m_topology->nbDomain();i++)
		{
			int nb_elem = m_mesh[i]->getNumberOfElements(entity,type);
			if (nb_elem>0)
				conn_ip[i]=m_mesh[i]->getConnectivity(MED_EN::MED_FULL_INTERLACE,
																							MED_EN::MED_NODAL,entity,type);				
			else
				conn_ip[i]=0;											
		}
	
	for (int icell=0; icell<nb_cells; icell++)
		{
			int number_of_types = number_of_types_array[ip[icell]];
			const MEDMEM::CELLMODEL* types =  m_mesh[ip[icell]]->getCellsTypes(entity);
			int type_offset=0;
			for (int itype=0; itype< number_of_types; itype++)
				{
					if (types[itype].getType() < type) type_offset+=m_mesh[ip[icell]]->getNumberOfElements(entity,types[itype].getType());
				}
			const int* conn=conn_ip[ip[icell]];	
		
			for (int inode=0; inode<nbnode_per_type; inode++)
				{
					type_connectivity[icell*nbnode_per_type+inode]=
						m_topology->convertNodeToGlobal(ip[icell],conn[(local[icell]-type_offset-1)*nbnode_per_type+inode]);												
				}
		}
	
	delete[]local;
	delete[]ip;
}


/*!gets the descending connectivity for a certain type
 *
 * The output array type_connectivity should have been allocated
 * at dimension nbnode_per_type* nb_cells before the call
 * 
 * \param cell_list list of elements (global cell numbers) for which the connectivity is required
 * \param nb_cells number of elements
 * \param entity type of entity for which the nodal connectivity is required
 * \param type type of the elements for which the connectivity is required
 * \param type_connectivity on output contains the connectivity of all the elements of the list
 * */ 
 
void MESHCollection::getFaceConnectivity(const int* cell_list,int nb_cells,MED_EN::medEntityMesh entity,
																				 MED_EN::medGeometryElement type, int* type_connectivity) const
{
	int *local=new int[nb_cells];
	int *ip=new int[nb_cells];
	switch (entity)
		{
		case MED_EN::MED_CELL:
			m_topology->convertGlobalCellList(cell_list,nb_cells,local,ip);
			break;
		case MED_EN::MED_FACE:
		case MED_EN::MED_EDGE:
			m_topology->convertGlobalFaceList(cell_list,nb_cells,local,ip);
			break;
		}
	
	
	int nbface_per_type;
	switch (type){
	case 308:
	  nbface_per_type=6;
	  break;
	case 304:
	  nbface_per_type=4;
	  break;
	case 306:
	  nbface_per_type=5;
	  break;
	}
	
	vector<int> number_of_types_array(m_topology->nbDomain(),0);
	for (int i=0; i<m_topology->nbDomain(); i++)
		number_of_types_array[i]=m_mesh[i]->getNumberOfTypes(entity);
		
	//defining a connectivity table for different domains	
	vector  <const int*> conn_ip(m_topology->nbDomain());
	for (int i=0; i<m_topology->nbDomain();i++)
		{
			int nb_elem = m_mesh[i]->getNumberOfElements(entity,type);
			if (nb_elem>0)
				conn_ip[i]=m_mesh[i]->getConnectivity(MED_EN::MED_FULL_INTERLACE,						     MED_EN::MED_DESCENDING,entity,type);				
			else
				conn_ip[i]=0;											
		}
	
	for (int icell=0; icell<nb_cells; icell++)
		{
			int number_of_types = number_of_types_array[ip[icell]];
			const MEDMEM::CELLMODEL* types =  m_mesh[ip[icell]]->getCellsTypes(entity);
			int type_offset=0;
			for (int itype=0; itype< number_of_types; itype++)
				{
					if (types[itype].getType() < type) type_offset+=m_mesh[ip[icell]]->getNumberOfElements(entity,types[itype].getType());
				}
			const int* conn=conn_ip[ip[icell]];						
			for (int iface=0; iface<nbface_per_type; iface++)
				{
					type_connectivity[icell*nbface_per_type+iface]=
						m_topology->convertFaceToGlobal(ip[icell],abs(conn[(local[icell]-type_offset-1)*nbface_per_type+iface]));						
				}
		}
	
	delete[]local;
	delete[]ip;
}

/*! gets the list of coordinates for a given list of global node numbers
 * 
 * The vector containing the coordinates on output should
 * have been allocated at a dimension m_space_dimension * nb_nodes
 * before the call
 * 
 * \param node_list list of global node numbers
 * \param nb_nodes number of nodes in the list
 * \param coordinates on output, contains the coordinates
 */
 
void MESHCollection::getCoordinates(int* node_list,int nb_nodes, double* coordinates) const
{
	int* local=new int[nb_nodes];
	int* ip=new int[nb_nodes];
	int space_dimension= getSpaceDimension();
	m_topology->convertGlobalNodeList(node_list,nb_nodes,local,ip);
	for (int i=0; i< nb_nodes; i++)
		{
			const double* coord=m_mesh[ip[i]]->getCoordinates(MED_EN::MED_FULL_INTERLACE);
			for (int icoord=0; icoord<space_dimension; icoord++)
				coordinates[i*space_dimension+icoord]=coord[(local[i]-1)*space_dimension+icoord];
		}
	delete[]local;
	delete[] ip;
}

/*! retrieves the space dimension*/
int MESHCollection::getSpaceDimension() const
{
	return m_mesh[0]->getSpaceDimension();
}
/*! retrieves the mesh dimension*/
int MESHCollection::getMeshDimension() const
{
	return m_mesh[0]->getMeshDimension();
}

/*! retrieves the type of coordinates system*/
string MESHCollection::getSystem() const
{
	return m_mesh[0]->getCoordinatesSystem();
}

/*!retrieves the name of the mesh*/
string MESHCollection::getMeshName() const
{
	return m_mesh[0]->getName();
}

vector<MEDMEM::MESH*>& MESHCollection::getMesh() 
{
	return m_mesh;
}

MEDMEM::MESH* MESHCollection::getMesh(int idomain) const
{
	return m_mesh[idomain];
}

vector<MEDMEM::CONNECTZONE*>& MESHCollection::getCZ()
{
	return m_connect_zones;
}

Topology* MESHCollection::getTopology() const 
{
	return m_topology;
}

void MESHCollection::setTopology(Topology* topo)
{
	if (m_topology!=0)
		{
			throw MED_EXCEPTION(STRING("Erreur : topology is already set"));
		}
	else
		m_topology= topo;
}

void MESHCollection::setIndivisibleGroup(const string& name)
{
	m_indivisible_regions.push_back(name);
	
}

/*! Browses the domains and the regions that have 
 * been marked as indivisible in order to create a vector 
 * the dimlension of which is the total number of cells, and
 * that contains 0 if the cell belongs to no indivisible group
 * and that contains an integer corresponding to the group otherwise.
 * 
 * \param   indivisible_tag on input is an int* allocated as int[nbcells]
 *		    on output contains the tags 
 */


void MESHCollection::treatIndivisibleRegions(int* indivisible_tag)
{
	//tag 0 is positioned on all the cells that are not affected by these tags
	for (int i=0; i<m_topology->nbCells(); i++)
		indivisible_tag[i]=0;

	//treating cell groups		
	for (int idomain=0; idomain<m_topology->nbDomain();idomain++)
		for (int igroup=0; igroup<m_mesh[idomain]->getNumberOfGroups(MED_EN::MED_CELL); igroup++)
			for (int i=0; i<m_indivisible_regions.size(); i++)
				{
					const MEDMEM::GROUP* group = m_mesh[idomain]->getGroup(MED_EN::MED_CELL,igroup+1);
					string groupname = group->getName();
					if (trim(groupname)==trim(m_indivisible_regions[i]))
						{
							int nbcells=group->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS);
							const int* numbers=group->getNumber(MED_EN::MED_ALL_ELEMENTS);
							int* global=new int[nbcells];
							m_topology->convertCellToGlobal(idomain,numbers,nbcells,global);
							for (int icell=0; icell<nbcells; icell++)
								indivisible_tag[global[icell]-1]=i+1;
							delete[] global;
						} 
				}
				
}	

/*! Method creating the cell graph
 * 
 * \param array returns the pointer to the structure that contains the graph 
 * \param edgeweight returns the pointer to the table that contains the edgeweights
 *        (only used if indivisible regions are required)
 */
 
void MESHCollection::buildCellGraph(MEDMEM::MEDSKYLINEARRAY* & array,int *& edgeweights )
{
		
  int cell_number=0;
  int node_number=0;
  for (int i=0; i<m_topology->nbDomain(); i++)
    {
      cell_number+=m_topology->getCellNumber(i);
      node_number+=m_topology->getNodeNumber(i);
    }
  //list of cells for a given node
  hash_multimap <int,int> node2cell(cell_number);
  
  //list of nodes for a given cell
  hash_multimap <int,int> cell2node(node_number);
  
  map<MED_EN::medGeometryElement,int*> type_cell_list;
  
  //tagging for the indivisible regions
  int* indivisible_tag=0;
  bool has_indivisible_regions=false;
  if (!m_indivisible_regions.empty())
		{
			has_indivisible_regions=true;
			indivisible_tag=new int[m_topology->nbCells()];
			treatIndivisibleRegions(indivisible_tag);
		}
  
  
  //browsing through all domains to create cell->node and node->cell connectivities
  for (int idomain=0; idomain<m_topology->nbDomain(); idomain++)
    {
      //	MED_EN::medGeometryElement* type_array;
      int nb_cells= m_topology->nbCells(idomain);
      int* cell_list = new int[nb_cells];
      MED_EN::medGeometryElement* type_list= new MED_EN::medGeometryElement[nb_cells];
      
      //retrieving global id list
      m_topology->getCellList(idomain, cell_list);
      
      //		cout << "retrieving type list"<<endl;
      //retrieving cell_types
      getTypeList(cell_list,nb_cells,MED_EN::MED_CELL,type_list);
      //cout<<"end of retrieval"<<endl;
      //number of elements per type
      std::map<MED_EN::medGeometryElement,int> type_numbers;
      type_numbers.clear();
      
			//vector containing the number of cells per type
      for (int icell=0; icell<nb_cells; icell++)
				{
					map <MED_EN::medGeometryElement,int>::iterator iter = type_numbers.find(type_list[icell]);
					if (iter!=type_numbers.end())
						(iter->second)++;
					else
						type_numbers.insert(make_pair(type_list[icell],1));
				}
      
      //setting the list of cells for each type
      map<MED_EN::medGeometryElement,int> index;
      
      MED_EN::MESH_ENTITIES::const_iterator currentEntity;
      list<MED_EN::medGeometryElement>::const_iterator iter;
      currentEntity  = MED_EN::meshEntities.find(MED_EN::MED_CELL);
      
      for (iter = (*currentEntity).second.begin();iter != (*currentEntity).second.end(); iter++)		
				{
					MED_EN::medGeometryElement type = *iter;
					if (type/100 != getMeshDimension()) continue;
					index[type]=0;
					if (type_numbers[type]>0)
						type_cell_list[type]=new int[type_numbers[type]];
					else
						type_cell_list[type]=0;
				}
      for (int icell=0; icell<nb_cells; icell++)
				{
					type_cell_list[type_list[icell]][index[type_list[icell]]++]=cell_list[icell];
				}
      
      delete[] cell_list;
      delete[] type_list;
      
      //retrieving connectivity in global numbering for each type
      map<MED_EN::medGeometryElement,int*> type_connectivity;
      
      int offset=0;
      for (iter = (*currentEntity).second.begin();iter != (*currentEntity).second.end(); iter++)		
				{
					MED_EN::medGeometryElement type = *iter;
					if (type/100 != getMeshDimension()) continue;
					int nbnode_per_type = type%100;
	  
					int connsize = type_numbers[type]*nbnode_per_type;
					if (connsize==0) continue;
					type_connectivity[type]=new int[connsize];
	  
					MESSAGE("Beginning of getNodeConnectivity");
					getNodeConnectivity(type_cell_list[type],type_numbers[type],MED_EN::MED_CELL,type,type_connectivity[type]);
					offset+=type_numbers[type];
					//	cout << " type "<<type <<" nb"<< type_numbers[type]<<endl;
					for (int i=0; i<type_numbers[type]; i++)
						{
							int cellid=type_cell_list[type][i];
							for (int inode=0; inode<nbnode_per_type; inode++)
								{
									int nodeid= type_connectivity[type][i*nbnode_per_type+inode]; 
									cell2node.insert(make_pair(cellid,nodeid));
									node2cell.insert(make_pair(nodeid,cellid));
									//		    		cout << "cell "<<cellid<<" node "<<nodeid<<endl;
								}
						}
					MESSAGE("End of multimap insertion");
	  
					delete[] type_cell_list[type];
					delete[] type_connectivity[type];
	  
				}
      
    }
	
	//creating the MEDMEMSKYLINEARRAY containing the graph
  
  int* size = new int[m_topology->nbCells()];
	int** temp=new int*[m_topology->nbCells()];
	int** temp_edgeweight=0;
	if (has_indivisible_regions)
		temp_edgeweight=new int*[m_topology->nbCells()];
	
	//going across all cells
	map<int,int> cells_neighbours;
	for (int i=0; i< m_topology->nbCells(); i++)
		{
			//	map<int,int> cells_neighbours;
			typedef hash_multimap<int,int>::const_iterator mmap_iter;
			pair <mmap_iter,mmap_iter>  noderange = cell2node.equal_range(i+1);
		
			for (mmap_iter iternode = noderange.first; iternode != noderange.second; iternode++)
				{
		    
					int nodeid=iternode->second;
					pair <mmap_iter,mmap_iter>  cellrange = node2cell.equal_range(nodeid);
		    
					for (mmap_iter itercell = cellrange.first; itercell != cellrange.second; itercell++)
						{				
							map<int,int>::iterator iter_map = cells_neighbours.find(itercell->second);
							if (iter_map == cells_neighbours.end())
								cells_neighbours.insert(make_pair(itercell->second,1));
							else
								(iter_map->second)++;
						}
				}
			size[i]=0;
			int dimension = getMeshDimension();
			for (map<int,int>::const_iterator iter=cells_neighbours.begin(); iter != cells_neighbours.end(); iter++)	
				{
					if (iter->second >= dimension && iter->first != i+1) size[i]++;
				}
		
			temp[i]=new int[size[i]];
			if (has_indivisible_regions)
				temp_edgeweight[i]=new int[size[i]];
		
			int itemp=0;
			for (map<int,int>::const_iterator iter=cells_neighbours.begin(); iter != cells_neighbours.end(); iter++)	
				{
					if (iter->second >=dimension && iter->first != i+1) 
						{
							temp[i][itemp]=iter->first;
							if (has_indivisible_regions)
								{
									int tag1 = indivisible_tag[(i+1)-1];
									int tag2 = indivisible_tag[iter->first-1];
									if (tag1==tag2 && tag1!=0)
										temp_edgeweight[i][itemp]=m_topology->nbCells()*100000;
									else
										temp_edgeweight[i][itemp]=1;
								}	
							itemp++;
						}
				}
			cells_neighbours.clear();
		}
    
	int* index=new int[m_topology->nbCells()+1];
	index[0]=1;
	for (int i=0; i<m_topology->nbCells(); i++)
		index[i+1]=index[i]+size[i];
		
	node2cell.clear();
	cell2node.clear();
	if (indivisible_tag!=0) delete [] indivisible_tag;
    	
	//SKYLINEARRAY structure holding the cell graph
	array= new MEDMEM::MEDSKYLINEARRAY(m_topology->nbCells(),index[m_topology->nbCells()]);
	array->setIndex(index);
	//delete [] index;
		
	for (int i=0; i<m_topology->nbCells(); i++)
		{
			array->setI(i+1,temp[i]);
			delete[]temp[i];
		}
	
	
	if (has_indivisible_regions)
		{
			edgeweights=new int[array->getLength()];
			for (int i=0; i<m_topology->nbCells(); i++)
				{
					for (int j=index[i]; j<index[i+1];j++)
						edgeweights[j-1]=temp_edgeweight[i][j-index[i]];
					delete[] temp_edgeweight[i];	
				}
			delete[]temp_edgeweight;
			//		for (int i=0; i<array->getLength(); i++)
			//			cout<<edgeweights[i]<<endl;
		}
	delete[] index;
	delete[] temp;
	delete[] size;
	
	
}


/*! Method creating the cell graph
 * 
 * \param array returns the pointer to the structure that contains the graph 
 * \param edgeweight returns the pointer to the table that contains the edgeweights
 *        (only used if indivisible regions are required)
 */
 
void MESHCollection::buildCellGraph2ndversion(MEDMEM::MEDSKYLINEARRAY* & array,int *& edgeweights )
{
  		
	//list of cells for a given face
	multimap <int,int> face2cell;
	
	//list of faces for a given cell
	multimap <int,int> cell2face;

		
	map<MED_EN::medGeometryElement,int*> type_cell_list;
	
	//tagging for the indivisible regions
	int* indivisible_tag;
	bool has_indivisible_regions=false;
	if (!m_indivisible_regions.empty())
		{
			has_indivisible_regions=true;
			indivisible_tag=new int[m_topology->nbCells()];
			treatIndivisibleRegions(indivisible_tag);
		}
	
	
	//browsing through all domains to create cell->face and face->cell connectivities
	for (int idomain=0; idomain<m_topology->nbDomain(); idomain++)
		{
			//	MED_EN::medGeometryElement* type_array;
			int nb_cells= m_topology->nbCells(idomain);
			int* cell_list = new int[nb_cells];
			MED_EN::medGeometryElement* type_list= new MED_EN::medGeometryElement[nb_cells];
		
			//retrieving global id list
			m_topology->getCellList(idomain, cell_list);
		
			//retrieving cell_types
			getTypeList(cell_list,nb_cells,MED_EN::MED_CELL,type_list);
		
			//number of elements per type
			std::map<MED_EN::medGeometryElement,int> type_numbers;
			type_numbers.clear();
		
			//vector containing the number of cells per type
			for (int icell=0; icell<nb_cells; icell++)
				{
					map <MED_EN::medGeometryElement,int>::iterator iter = type_numbers.find(type_list[icell]);
					if (iter!=type_numbers.end())
						(iter->second)++;
					else
						type_numbers.insert(make_pair(type_list[icell],1));
				}
		
			//setting the list of cells for each type
			map<MED_EN::medGeometryElement,int> index;
			
			MED_EN::MESH_ENTITIES::const_iterator currentEntity;
  		list<MED_EN::medGeometryElement>::const_iterator iter;
			currentEntity  = MED_EN::meshEntities.find(MED_EN::MED_CELL);
		
		
			for (iter = (*currentEntity).second.begin();iter != (*currentEntity).second.end(); iter++)		
				{
					MED_EN::medGeometryElement type = *iter;
					if (type/100 != getMeshDimension()) continue;
					index[type]=0;
					if (type_numbers[type]>0)
						type_cell_list[type]=new int[type_numbers[type]];
					else
						type_cell_list[type]=0;
				}
			for (int icell=0; icell<nb_cells; icell++)
				{
					type_cell_list[type_list[icell]][index[type_list[icell]]++]=cell_list[icell];
				}
		
			delete[] cell_list;
			delete[] type_list;
		
			//retrieving connectivity in global numbering for each type
			map<MED_EN::medGeometryElement,int*> type_connectivity;
		
			int offset=0;

		
			for (iter = (*currentEntity).second.begin();iter != (*currentEntity).second.end(); iter++)		
				{
					MED_EN::medGeometryElement type = *iter;
					if (type/100 != getMeshDimension()) continue;

					int nbface_per_type;
					switch (type){
					case 308:
						nbface_per_type=6;
						break;
					case 304:
						nbface_per_type=4;
						break;
					case 306:
						nbface_per_type=5;
						break;
					}
			
			
					int connsize = type_numbers[type]*nbface_per_type;
					if (connsize==0) continue;
					type_connectivity[type]=new int[connsize];
			
					//cout <<"Beginning of getFaceConnectivity"<<endl;


					getFaceConnectivity(type_cell_list[type],type_numbers[type],MED_EN::MED_CELL,type,type_connectivity[type]);
					offset+=type_numbers[type];
					//cout << " type "<<type <<" nb"<< type_numbers[type]<<endl;
					for (int i=0; i<type_numbers[type]; i++)
						{
							int cellid=type_cell_list[type][i];
							for (int iface=0; iface<nbface_per_type; iface++)
								{
									int faceid= type_connectivity[type][i*nbface_per_type+iface]; 
									cell2face.insert(make_pair(cellid,faceid));
									face2cell.insert(make_pair(faceid,cellid));
									//cout << "cell "<<cellid<<" face "<<faceid<<endl;
								}
						}
					//cout <<"End of multimap insertion"<<endl;
					delete[] type_cell_list[type];
					delete[] type_connectivity[type];
		    
				}
		
		}
	
	//creating the MEDMEMSKYLINEARRAY containing the graph
	
	int* size = new int[m_topology->nbCells()];
	int** temp=new int*[m_topology->nbCells()];
	int** temp_edgeweight=0;
	if (has_indivisible_regions)
		temp_edgeweight=new int*[m_topology->nbCells()];
	
	//going across all cells
	for (int i=0; i< m_topology->nbCells(); i++)
		{
			set<int> cells_neighbours;
			typedef multimap<int,int>::const_iterator mmap_iter;
			pair <mmap_iter,mmap_iter>  facerange = cell2face.equal_range(i+1);
		
			for (mmap_iter iterface = facerange.first; iterface != facerange.second; iterface++)
				{
			
					int faceid=iterface->second;
					pair <mmap_iter,mmap_iter>  cellrange = face2cell.equal_range(faceid);			
					for (mmap_iter itercell = cellrange.first; itercell != cellrange.second; itercell++)
						{		
							cells_neighbours.insert(itercell->second);
						}
				}
			size[i]=0;
		
			for (set<int>::iterator iter=cells_neighbours.begin(); iter != cells_neighbours.end(); iter++)	
				{
					if (*iter != i+1) size[i]++;
				}
		
			temp[i]=new int[size[i]];
			if (has_indivisible_regions)
				temp_edgeweight[i]=new int[size[i]];
		
			int itemp=0;
			for (set<int>::iterator iter=cells_neighbours.begin(); iter != cells_neighbours.end(); iter++)	
				{
					if (*iter != i+1) 
						{
							temp[i][itemp]=*iter;
							if (has_indivisible_regions)
								{
									int tag1 = indivisible_tag[(i+1)-1];
									int tag2 = indivisible_tag[*iter-1];
									if (tag1==tag2 && tag1!=0)
										temp_edgeweight[i][itemp]=m_topology->nbCells()*100000;
									else
										temp_edgeweight[i][itemp]=1;
								}	
							itemp++;
						}
				}
		}
	int* index=new int[m_topology->nbCells()+1];
	index[0]=1;
	for (int i=0; i<m_topology->nbCells(); i++)
		index[i+1]=index[i]+size[i];
		
	face2cell.clear();
	cell2face.clear();
			
	//SKYLINEARRAY structure holding the cell graph
	array= new MEDMEM::MEDSKYLINEARRAY(m_topology->nbCells(),index[m_topology->nbCells()]);
	array->setIndex(index);
	//delete [] index;
		
	for (int i=0; i<m_topology->nbCells(); i++)
		{
			array->setI(i+1,temp[i]);
			delete[]temp[i];
		}
	
	
	if (has_indivisible_regions)
		{
			edgeweights=new int[array->getLength()];
			for (int i=0; i<m_topology->nbCells(); i++)
				{
					for (int j=index[i]; j<index[i+1];j++)
						edgeweights[j-1]=temp_edgeweight[i][j-index[i]];
					delete[] temp_edgeweight[i];	
				}
			delete[]temp_edgeweight;
			//		for (int i=0; i<array->getLength(); i++)
			//			cout<<edgeweights[i]<<endl;
		}
	delete[] index;
	delete[] temp;
	delete[] size;
	
	
}

/*! Creates the partition cocoutrresponding to the cell graph and the partition number
 * 
 * \param nbdomain number of subdomains for the newly created graph
 * 
 * returns a topology based on the new graph
 */
Topology* MESHCollection::createPartition(int nbdomain, 
																					Graph::splitter_type split, 
																					const string& options_string,
																					int* user_edge_weights,
																					int* user_vertices_weights)
{

	if (nbdomain <1) throw MEDEXCEPTION("Number of subdomains must be >0");
	MEDMEM::MEDSKYLINEARRAY* array=0;
	int* edgeweights=0;
	
	MESSAGE("Building cell graph");
	buildCellGraph(array,edgeweights);
	
	switch (split)
		{
		case Graph::METIS:
			m_cell_graph=boost::shared_ptr<Graph>(new METISGraph(array,edgeweights));
			break;
		case Graph::SCOTCH:
			m_cell_graph=boost::shared_ptr<Graph>(new SCOTCHGraph(array,edgeweights));
			break;
		}

	//!user-defined weights
	if (user_edge_weights!=0) 
	  m_cell_graph->setEdgesWeights(user_edge_weights);
	if (user_vertices_weights!=0)
	  m_cell_graph->setVerticesWeights(user_vertices_weights);

  
	MESSAGE("Partitioning graph");
	m_cell_graph->partGraph(nbdomain,options_string);
	
	MESSAGE("Building new topology");
	//m_cell_graph is a shared pointer 
	Topology* topology = new ParallelTopology (m_cell_graph, nbdomain, getMeshDimension());
	
	//cleaning
	if (edgeweights!=0) delete[] edgeweights;
	//if (array!=0) delete array;
	MESSAGE("End of partition creation");
	return topology;
}

/*! Creates a topology for a partition specified by the user
 * 
 * \param table user-specified partition (for each cell contains the domain number from 0 to n-1)
 * 
 * returns a topology based on the new partition
 */
Topology* MESHCollection::createPartition(const int* partition)
{
	MEDMEM::MEDSKYLINEARRAY* array=0;
	int* edgeweights=0;
	
	buildCellGraph(array,edgeweights);
	
	set<int> domains;
	for (int i=0; i<m_topology->nbCells(); i++)
		{
			domains.insert(partition[i]);
		}
	int nbdomain=domains.size();
	
	m_cell_graph=boost::shared_ptr<Graph>(new UserGraph(array, partition, m_topology->nbCells()));
	
	//m_cell_graph is a shared pointer 
	Topology* topology = new ParallelTopology (m_cell_graph, nbdomain, getMeshDimension());
	
	//if (array!=0) delete array;
	return topology;
}


/*! building Connect Zones for storing the informations
 * of the connectivity 
 * 
 * The connect zones are created for every domain that has common nodes with 
 * domain \a idomain
 * 
 * \param idomain domain number for which the connect zones are created
 * */
 
void MESHCollection::buildConnectZones(int idomain)
{
	// constructing node/node correspondencies
	vector<MEDMEM::MEDSKYLINEARRAY*> node_node_correspondency;
	node_node_correspondency.resize(m_topology->nbDomain());
	m_topology->computeNodeNodeCorrespondencies(idomain, node_node_correspondency );
	
	for (int idistant=0; idistant< m_topology->nbDomain(); idistant++)
		{
			// on regarde si une correspondance noeud/noeud a été trouvée 
			// entre idomain et idistant
			// si oui, on crée une connectzone
			if (node_node_correspondency[idistant]!=0)
				{
					MEDMEM::CONNECTZONE* cz= new MEDMEM::CONNECTZONE();
					cz->setLocalMesh(m_mesh[idomain]);
					cz->setDistantMesh(m_mesh[idistant]);
					cz->setLocalDomainNumber(idomain);
					cz->setDistantDomainNumber(idistant);
					cz-> setName ("Connect zone defined by SPLITTER");
					cz->setNodeCorresp(node_node_correspondency[idistant]);
					m_connect_zones.push_back(cz);	
				}
		}
	vector<MEDMEM::MEDSKYLINEARRAY*> cell_cell_correspondency;
	cell_cell_correspondency.resize(m_topology->nbDomain());
	m_topology->computeCellCellCorrespondencies(idomain, cell_cell_correspondency, m_cell_graph.get());
	
	for (int idistant=0; idistant< m_topology->nbDomain(); idistant++)
		{
			//the connect zone has been created by the node/node computation
			if (cell_cell_correspondency[idistant]!=0)
				{
					MEDMEM::CONNECTZONE* cz=0;
					for (int icz=0; icz<m_connect_zones.size();icz++)
						if (m_connect_zones[icz]->getLocalDomainNumber()==idomain &&
								m_connect_zones[icz]->getDistantDomainNumber()==idistant)
							cz = m_connect_zones[icz];
					if (cz!=0)	
						cz->setEntityCorresp(MED_EN::MED_CELL,MED_EN::MED_CELL, cell_cell_correspondency[idistant]);
					else 
						throw MEDEXCEPTION("MESHCollectio::buildConnectZones() -A connect zone should exist");   
					//delete cell_cell_correspondency[idistant];
				}
        
		}
}

/*! projects old collection families on new collection families
 */
void MESHCollection::castFamilies(const MESHCollection& old_collection)
{
	vector <list<int> > element_array  (m_topology->nbDomain());
	
	//loop on old domains
	
	//definition of the entities array which 
	//defines the entities over which the information is cast
	MED_EN::medEntityMesh entities[3];
	entities[0]=MED_EN::MED_NODE;
	entities[2]=MED_EN::MED_CELL;
	switch (getMeshDimension())
		{
		case 3:
			entities[1]= MED_EN::MED_FACE;	
			break;
		case 2:
			entities[1]= MED_EN::MED_EDGE;	
			break;
		}

	for (int ientity=0; ientity<=2;ientity++)
		{
        	
			//int nbgroups = old_collection.getMesh(0)->getNumberOfGroups(entities[ientity]);
        
			map <string, set<int> > group_map;
			for (int idomain=0; idomain < old_collection.m_topology->nbDomain(); idomain++)
				{
					for (int igroup=0; igroup<old_collection.getMesh(idomain)->getNumberOfGroups(entities[ientity]); igroup++)
            {
							//retrieves a group
              MEDMEM::GROUP* group = (old_collection.getMesh(idomain)->getGroups(entities[ientity]))[igroup];
							//increments the number of groups if it is a new group
              //if (group_map.find(group->getName())==group_map.end())
               
							group_map[group->getName()].insert(idomain);
							//   group_map.insert(make_pair(group->getName(), idomain);
              
            }   
				}
			int nbgroups=group_map.size();
			vector <int> igroupold(old_collection.m_topology->nbDomain(),0);
			map<string,set<int> >::const_iterator iter=group_map.begin();
         
			for (int igroup=0; igroup<nbgroups; igroup++)
		    {
					vector <const MEDMEM::SUPPORT*> old_supports(old_collection.m_topology->nbDomain());
		      string group_name = iter->first;
					iter++; 
			
					//parameters stored for passing group description
					// form the old meshes to the new ones
           
					for (int idomain=0; idomain < old_collection.m_topology->nbDomain(); idomain++)
						{
							//                for (set<int>::iterator iter=group_map[group_name].begin(); iter!=group_map[group_name].end(); iter++)
							//                cout << *iter<<" ";
							//                cout <<endl;
							if (group_map[group_name].find(idomain)==group_map[group_name].end()) continue;
                                    
							//retrieves the group igroup on domain idomain
							MEDMEM::GROUP* group = (old_collection.getMesh(idomain)->getGroups(entities[ientity]))[igroupold[idomain]];
							old_supports[idomain] = static_cast<const MEDMEM::SUPPORT*> (group);
							igroupold[idomain]++;
						}
			
					vector <MEDMEM::GROUP*>new_groups(m_topology->nbDomain());
					vector <MEDMEM::SUPPORT*> new_supports(m_topology->nbDomain());
					for (int i=0; i<m_topology->nbDomain(); i++)
						{
							new_groups[i]=new MEDMEM::GROUP();
							new_supports[i]=static_cast<MEDMEM::SUPPORT*>(new_groups[i]);
						}
					castSupport(old_collection,old_supports,new_supports);			
			
					//creating new groups from the previous list of elements
					for (int idomain=0; idomain <m_topology->nbDomain(); idomain++)
						{
							MEDMEM::MESHING* mesh_builder=static_cast<MEDMEM::MESHING*> (m_mesh[idomain]);
							mesh_builder->addGroup(*new_groups[idomain]);
						}
					//groups are copied by the addGroup method,
					//so they can be safely deleted here
					for (int i=0; i<m_topology->nbDomain(); i++)
						{
							delete new_groups[i];
						}
		
				}// on groups
		}//on entities
}


void MESHCollection::castSupport(const MESHCollection& old_collection, vector<const MEDMEM::SUPPORT*> old_support, vector<MEDMEM::SUPPORT*> new_support)
{
	
	if (old_collection.m_topology->nbDomain() != old_support.size())
		{
			throw MED_EXCEPTION(STRING("Error : wrong call to MESHCollection::castSupport"));
		}
	vector <list<int> > element_array  (m_topology->nbDomain());
	
	//parameters stored for passing description
	// form the old meshes to the new ones
	string name;
	string description;
	MED_EN::medEntityMesh entity;
	vector <string> support_name(1);
	support_name[0]="support";
	for (int inew=0; inew< m_topology->nbDomain(); inew++)
		element_array[inew].clear();
		
	for (int idomain=0; idomain < old_collection.m_topology->nbDomain(); idomain++)
		{
			//retrieves the group igroup on domain idomain
			const MEDMEM::SUPPORT* support = old_support[idomain];
			if (old_support[idomain]==0) continue;
			name = support->getName();
			description=support->getDescription();
			int nbelem = support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS);
			if (nbelem==0) continue;
		
			int* list_of_elems;
			if (support->isOnAllElements())
				{
					list_of_elems = new int[nbelem];
					for (int i=0; i<nbelem;i++)
						list_of_elems[i]=i+1;
				}
			else
				list_of_elems = const_cast<int*> (support->getNumber(MED_EN::MED_ALL_ELEMENTS));

			int* array=new int[nbelem];
			int* ip=0;
			int* local=0;
			int* full_array=0;
			entity = support->getEntity();
			int size;

			switch (entity)
				{
				case MED_EN::MED_CELL :
					ip=new int[nbelem];
					local= new int[nbelem];
					size=nbelem;
					old_collection.getTopology()->convertCellToGlobal(idomain,list_of_elems,nbelem,array);
					m_topology->convertGlobalCellList(array,nbelem,local,ip);
					for (int i=0; i<nbelem; i++)
						//							cell_arrays[ip[i]][local[i]]=id;
						{
							//					cout <<"(glob,ip,iloc)/nbelem"<<array[i]<<" "<<ip[i]<<" "<<local[i]<<"/"<<nbelem<<endl;
							element_array[ip[i]].push_back(local[i]);
						}
					break;
				case MED_EN::MED_FACE :
				case MED_EN::MED_EDGE :
					old_collection.getTopology()->convertFaceToGlobal(idomain,list_of_elems,nbelem,array);
					m_topology->convertGlobalFaceListWithTwins(array,nbelem,local,ip,full_array,size);
					for (int i=0; i<size; i++)
						element_array[ip[i]].push_back(local[i]);
					delete[] full_array;	
					break;
				case MED_EN::MED_NODE :
					old_collection.getTopology()->convertNodeToGlobal(idomain,list_of_elems,nbelem,array);
					m_topology->convertGlobalNodeListWithTwins(array,nbelem,local,ip,full_array,size);
					for (int i=0; i<size; i++)
						element_array[ip[i]].push_back(local[i]);
					delete[] full_array;
					break;
				
				}
			delete[] ip;
			delete[] local;
			delete[] array;
        
			if (support->isOnAllElements()) delete[] list_of_elems;
		}
	
	//creating new groups from the previous list of elements
	for (int idomain=0; idomain <m_topology->nbDomain(); idomain++)
		{
			MEDMEM::SUPPORT* support= new_support[idomain];
			support->setName(name);
			support->setMesh(m_mesh[idomain]);
			support->setDescription(description);
			support->setEntity(entity);

			element_array[idomain].sort();
			element_array[idomain].unique();
		
			if (entity != MED_EN::MED_NODE)
				support->fillFromElementList(element_array[idomain]);
			else
				{
					support->fillFromNodeList(element_array[idomain]);
				}
		}
}

void MESHCollection::castField(const MESHCollection& old_collection, const string& fieldname, int itnumber, int ordernumber)
{
  int type=old_collection.getDriver()->getFieldType(fieldname);
  char field_char[80];
  strcpy(field_char,fieldname.c_str());
  
  if (type ==0)
    castFields<int>(old_collection, field_char, itnumber, ordernumber);
  else
    castFields<double>(old_collection, field_char, itnumber, ordernumber);
}

void MESHCollection::castAllFields(const MESHCollection& initial_collection)
{
	vector <string> field_names;
	vector <int> iternumber;
	vector <int> ordernumber;
	vector <int> types;
	initial_collection.getDriver()->readFileStruct(field_names,iternumber,ordernumber,types);
	
	for (int i=0; i<field_names.size(); i++)
		{
			char field_char[80];
			strcpy(field_char,field_names[i].c_str());
		
			// choosing whether the field is of int or double type
			if (types[i] ==0)
        castFields<int>(initial_collection, field_char, iternumber[i], ordernumber[i]);
			else
			  castFields<double>(initial_collection, field_char, iternumber[i], ordernumber[i]);
		}
}

void MESHCollection::createNodalConnectivity(const MESHCollection& initial_collection,int idomain, MED_EN::medEntityMesh entity)
{
	int dimension=0;
	int nb_elems=0;
	MEDMEM::MESHING* mesh_builder = static_cast<MEDMEM::MESHING*>(m_mesh[idomain]);
	
	//number of elements per type
	std::map<MED_EN::medGeometryElement,int> type_numbers;
	
	//creating arrays for storing global numbers and cell types
	switch (entity)
		{
		case MED_EN::MED_CELL:
			dimension=initial_collection.getMeshDimension();
			nb_elems=m_topology->getCellNumber(idomain);
			break;
		case MED_EN::MED_EDGE:
		case MED_EN::MED_FACE:
			dimension=initial_collection.getMeshDimension()-1;
			nb_elems=m_topology->getFaceNumber(idomain);	
			break;
		default:
			nb_elems=0;
			break;
		}
	
	int *list= new int[nb_elems];
	MED_EN::medGeometryElement *cell_type_list= new MED_EN::medGeometryElement[nb_elems];
	
	//	cout << "Beginning of retrieval "<<endl;
	//retrieving global id list
	switch (entity)
		{
		case MED_EN::MED_CELL:
			m_topology->getCellList(idomain,list);
			break;
		case MED_EN::MED_EDGE:
		case MED_EN::MED_FACE:
			m_topology->getFaceList(idomain,list);
			break;
		default:
			
			break;
		}
	
	//retrieving cell_types
	initial_collection.getTypeList(list,nb_elems,entity,cell_type_list);
	//	cout <<"end of type retrieval"<<endl;
	//vector containing the number of cells per type
	type_numbers.clear();
	for (int icell=0; icell<nb_elems; icell++)
		{
			map<MED_EN::medGeometryElement,int>::iterator iter= type_numbers.find(cell_type_list[icell]);
			if (iter!=type_numbers.end())
				(iter->second)++;
			else	
				type_numbers[cell_type_list[icell]]=1;
		
		}
	//cout << "Nombre de tetras"<<type_numbers[304]<<endl;
	int nb_present_types=type_numbers.size();
	
	//setting the list of cells for each type
	map<MED_EN::medGeometryElement,int> index;
	
	map<MED_EN::medGeometryElement,int*> type_cell_list;
	
	MED_EN::MESH_ENTITIES::const_iterator currentEntity;
	std::list<MED_EN::medGeometryElement>::const_iterator iter;
	currentEntity  = MED_EN::meshEntities.find(entity);
	for (iter = (*currentEntity).second.begin();iter != (*currentEntity).second.end(); iter++)	
		{
			if (*iter/100 != dimension) continue;
			if (type_numbers[*iter]==0) continue;
			index[*iter]=0;
			type_cell_list[*iter]=new int[type_numbers[*iter]];
		}
	
	for (int icell=0; icell<nb_elems; icell++)
		{
			type_cell_list[cell_type_list[icell]][index[cell_type_list[icell]]++]=list[icell];
		}
	
	delete[]list;
	delete[]cell_type_list;
	
	//setting the list of present ypes
	int* present_type_numbers=new int[nb_present_types];
	MED_EN::medGeometryElement* type_array = new MED_EN::medGeometryElement[nb_present_types];
	MESSAGE("Nb de types presents "<<nb_present_types);
	int itype=0;
	for (iter = (*currentEntity).second.begin();iter != (*currentEntity).second.end(); iter++)	
		{
			if (*iter/100 != dimension) continue;
			if (type_numbers[*iter]==0) continue;
			type_array[itype]=*iter;
		
			present_type_numbers[itype]=type_numbers[*iter];
		
			MESSAGE("Nombre d'elements de type "<<*iter<<" : "<<type_numbers[*iter]);
			itype++;
		}
		
	//retrieving connectivity in global numbering for each typeinitial_collection.getMesh(iold)->get
	map<MED_EN::medGeometryElement,int*> type_connectivity;
		
	//Treating nodes
		
	for (iter = (*currentEntity).second.begin();iter != (*currentEntity).second.end(); iter++)
		{
			MED_EN::medGeometryElement type=*iter;
			if (type/100 != dimension) continue;
			if (type_numbers[type]==0) continue;
			int nbnode_per_type = (int)type%100;
			type_connectivity[type]=new int[type_numbers[type]*nbnode_per_type];
	    initial_collection.getNodeConnectivity(type_cell_list[type],type_numbers[type],entity,type,type_connectivity[type]);
	    delete[] type_cell_list[type];
		}	
		
	//creating node mapping 
	//!TODO : compute the total number of nodes 
	if (entity==MED_EN::MED_CELL)
		m_topology->createNodeMapping(type_connectivity,type_numbers,idomain);
			
	//converting node global numberings to local numberings
	m_topology->convertToLocal(type_connectivity,type_numbers,idomain,entity);
		
		
	//writing coordinates
	if (entity==MED_EN::MED_CELL)	
		{
			//setting coordinates from initial_collection coordinates
			int nbnode=m_topology->getNodeNumber(idomain);
			MESSAGE("Number of nodes on domain "<< idomain <<" : "<<nbnode);
		
			double* coordinates=new double[initial_collection.getSpaceDimension()*nbnode];
			int* node_list=new int[nbnode];
			m_topology->getNodeList(idomain,node_list);
			initial_collection.getCoordinates(node_list,nbnode,coordinates);	
			delete[] node_list;
		
			// redundant specification of number of nodes is required!! MED imperfection, sorry...	
		
			mesh_builder->setNumberOfNodes(nbnode);
			//TODO : change MEDMEM so that it accepts a direct setting of coordinates
			// (in the present version, it is deep-copied)
			mesh_builder->setCoordinates(initial_collection.getSpaceDimension(), 
																	 m_topology->getNodeNumber(idomain), coordinates, initial_collection.getSystem(),
																	 MED_EN::MED_FULL_INTERLACE);
			delete [] coordinates;
		}
			
	mesh_builder->setNumberOfTypes(nb_present_types,entity);
	mesh_builder->setTypes(type_array,entity);
	mesh_builder->setNumberOfElements(present_type_numbers,entity);
	delete[]present_type_numbers;
	delete[]type_array;
	//setting node connectivities
	for (iter = (*currentEntity).second.begin();iter != (*currentEntity).second.end(); iter++)
		{
			if (*iter/100 != dimension) continue;
			if (type_numbers[*iter]==0) continue;
	    mesh_builder->setConnectivity(type_connectivity[*iter],entity,*iter);
	    delete[] type_connectivity[*iter];
		}
		
}

