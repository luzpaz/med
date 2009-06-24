//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef MESHCOLLECTION_
#define MESHCOLLECTION_

#include "MEDSPLITTER.hxx"

//#include "MESHCollectionDriver.hxx"
#include "MEDSPLITTER_Graph.hxx"
#include "MEDSPLITTER_FaceModel.hxx"
#include "boost/shared_ptr.hpp"
#include <vector>


namespace MEDMEM
{
  class MESH;
  class CONNECTZONE;
  class MEDSKYLINEARAY;
  class SUPPORT;
}

namespace MEDSPLITTER
{

  class Topology;
  class MESHCollectionDriver;

  typedef enum{MedAscii, MedXML, Undefined} DriverType;

  class MEDSPLITTER_EXPORT MESHCollection
  {

  public:

    //Default constructor
    MESHCollection();

    //Constructing from an existing mesh and a new topology
    MESHCollection(const MESHCollection&, Topology*, bool family_splitting=false, bool create_empty_groups=false);

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
    void setDriverType(MEDSPLITTER::DriverType type) {m_driver_type=type;}

    //creation of the cell graph
    void buildCellGraph(MEDMEM::MEDSKYLINEARRAY* & array,int *& edgeweights );

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
    void getPolygonNodeConnectivity(const int* cell_list,int nb_cells,MED_EN::medEntityMesh entity,
                                    vector<int>& type_connectivity, vector<int>& connectivity_index) const;
    void getPolyhedraNodeConnectivity(const int* cell_list,int nb_cells,MED_EN::medEntityMesh entity,
                                      vector<int>& type_connectivity, vector<int>& connectivity_index, vector<int>& face_connectivity_index) const;

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

    //getting/setting the name of the global mesh (as opposed 
    //to the name of a subdomain \a nn, which is name_nn) 
    string getName() const {return m_name;}
    void setName(const string& name) {m_name=name;}

    //getting/setting the description of the global mesh
    string getDescription() const {return m_description;}
    void setDescription(const string& name) { m_description=name;}

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
    void buildConnectZones();

    bool isDimensionOK(MED_EN::medGeometryElement type, int dim)
    {
      return ((type/100 == dim) || (dim==2 && type == MED_EN::MED_POLYGON) || (dim==3 && type == MED_EN::MED_POLYHEDRA));
    }
    void setSubdomainBoundaryCreates(bool flag) {  m_subdomain_boundary_creates=flag;}
    bool getSubdomainBoundaryCreates(){return m_subdomain_boundary_creates;}

    void setFamilySplitting(bool flag){m_family_splitting=flag;}
    bool getFamilySplitting(){return m_family_splitting;}

    void setCreateEmptyGroups(bool flag){m_create_empty_groups=flag;}
    bool getCreateEmptyGroups(){return m_create_empty_groups;}
  private:
    //!creates connectivities for a domain and an entity (face or cell)
    void createNodalConnectivity(const MESHCollection & initial_collection, int idomain, MED_EN::medEntityMesh entity);

    //!creates the tags for indivisible groups
    void treatIndivisibleRegions(int* tag);

    //!projects a field from an old collection to the present one
    //!field is identified by (name, dt, it)
    template <class T>
    void castFields(const MESHCollection& old_collection, const string& fieldname, int itnumber, int ordernumber);

    void getFaces(int idomain, std::map<MED_EN::medGeometryElement, vector<MEDSPLITTER_FaceModel*> >&);

    MEDSPLITTER_FaceModel* getCommonFace(int ip1,int ilocal1,int ip2,int ilocal2,int face_index);


    //!link to mesh_collection topology
    Topology* m_topology;

    //!control over topology
    bool m_owns_topology;

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

    //!name of global mesh
    string m_name;

    //!description of global mesh
    string m_description;

    //! specifies the driver associated to the collection
    DriverType m_driver_type;

    /*! flag specifying that the splitter should create boundary constituent entity
      so that they are written in joints*/
    bool m_subdomain_boundary_creates;

    /*! flag specifying that families must be preserved by the
      splitting*/
    bool m_family_splitting;

    /*! flag specifying that groups must be created on all domains, 
      even if they are empty*/
    bool m_create_empty_groups;
  };

}//of namespace

#endif /*MESHCOLLECTION_*/
