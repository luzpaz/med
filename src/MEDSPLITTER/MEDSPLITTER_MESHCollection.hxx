#ifndef MESHCOLLECTION_
#define MESHCOLLECTION_

#include "MEDSPLITTER.hxx"

//#include "MESHCollectionDriver.hxx"
#include "MEDSPLITTER_Graph.hxx"
#include "boost/shared_ptr.hpp"

namespace MEDMEM{
class MESH;
class CONNECTZONE;
}

namespace MEDSPLITTER{

class Topology;
class MESHCollectionDriver;

class MEDSPLITTER_EXPORT MESHCollection
{
	
public:
	
//Default constructor
MESHCollection();

//Constructing from an existing mesh and a new topology
MESHCollection(const MESHCollection&, Topology*);

//Constructing the mesh collection from a file
MESHCollection(const std::string& filename);

//Constructing the mesh collection from a file
MESHCollection(const std::string& filename, const std::string& meshname);

~MESHCollection();

//writing to a distributed file
void write(const std::string& filename);

//getting the driver
MESHCollectionDriver* retrieveDriver();
MESHCollectionDriver* getDriver() const;

//creation of the cell graph
void buildCellGraph(MEDMEM::MEDSKYLINEARRAY* & array,int *& edgeweights );

void buildCellGraph2ndversion(MEDMEM::MEDSKYLINEARRAY* & array,int *& edgeweights );


//creation and partition of the associated graph
  Topology* createPartition(int nbdomain, Graph::splitter_type type = Graph::METIS,
			    const string& ="", int* edgeweights=0, int* verticesweights=0);

//creation of a user specified partition
Topology* createPartition(const int* partition);

//retrieving list of types
void getTypeList(int* cell_list,int nb_cells,MED_EN::medEntityMesh entity,
				  MED_EN::medGeometryElement* type_list) const ;

//getting list of coordinates
void getCoordinates(int* node_list,int nb_nodes, double* coordinates) const ;

//getting connectivities
void getNodeConnectivity( const int*  cell_list,int nb_cells,MED_EN::medEntityMesh,MED_EN::medGeometryElement type, int* type_connectivity) const ;
void getFaceConnectivity( const int*  cell_list,int nb_cells,MED_EN::medEntityMesh,MED_EN::medGeometryElement type, int* type_connectivity) const ;

  //void getFaceConnectivity( const int*  cell_list,int nb_cells,MED_EN::medGeometryElement type, int* type_connectivity) const ;

//getting mesh dimension
int getMeshDimension() const ;

//getting space dimension
int getSpaceDimension() const ;

//getting system of coordinates
std::string getSystem() const;

//getting name of the mesh
std::string getMeshName() const;

//getting a reference to mesh vector
std::vector<MEDMEM::MESH*>& getMesh() ;

//getting a reference to mesh vector
MEDMEM::MESH* getMesh(int) const;

//getting a reference to connect zones vector
std::vector<MEDMEM::CONNECTZONE*>& getCZ();

//getting a pointer to topology
Topology* getTopology() const ;


//settig a new topology
void setTopology(Topology* topology);

//!transfers families from an old MESHCollection to new mesh
void castFamilies(const MESHCollection& old_collection);

void castSupport(const MESHCollection& old_collection, std::vector<const MEDMEM::SUPPORT*> old_support, std::vector<MEDMEM::SUPPORT*> new_support);

//!casts all the fields to the new mesh collection
void castAllFields(const MESHCollection& old_collection);

//!casts one specific field to the new mesh collection
void castField(const MESHCollection& old_collection, const string& fieldname, int itnumber, int ordernumber);

//choosing a group to be indivisible
void setIndivisibleGroup(const string& a);

//!constructing connect zones
void buildConnectZones(int idomain);

private:
//!creates connectivities for a domain and an entity (face or cell)
void createNodalConnectivity(const MESHCollection & initial_collection, int idomain, MED_EN::medEntityMesh entity);

//!creates the tags for indivisible groups
void treatIndivisibleRegions(int* tag);

template <class T>
void castFields(const MESHCollection& old_collection, const string& fieldname, int itnumber, int ordernumber, T type_of_template);

//!link to mesh_collection topology
Topology* m_topology;

//!link to graph
boost::shared_ptr<Graph> m_cell_graph;

//! Driver for read/write operations
MESHCollectionDriver* m_driver;

//!links to meshes
std::vector<MEDMEM::MESH*> m_mesh;

//!links to connectzones
std::vector<MEDMEM::CONNECTZONE*> m_connect_zones;

//!list of groups that are not to be splitted
std::vector<std::string> m_indivisible_regions;


};

}//of namespace

#endif /*MESHCOLLECTION_*/
