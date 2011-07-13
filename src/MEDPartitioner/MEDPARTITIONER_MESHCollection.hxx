//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
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
#ifndef __MEDPARTITIONER_MESHCOLLECTION_HXX__
#define __MEDPARTITIONER_MESHCOLLECTION_HXX__

#include "MEDPARTITIONER.hxx"

#include "MEDPARTITIONER_Graph.hxx"
//#include "MEDPARTITIONER_FaceModel.hxx"
//#include "boost/shared_ptr.hpp"
#include <vector>
#include <map>
#include <string>
namespace ParaMEDMEM
{
  class MEDCouplingUMesh;
  class DataArrayInt;
}


namespace MEDPARTITIONER
{

  class Topology;
  class MESHCollectionDriver;
  class ParaDomainSelector;
  class MEDSKYLINEARRAY;
  class CONNECTZONE;
  class JointFinder;
  typedef enum{MedAscii, MedXML, Undefined} DriverType;

  typedef std::multimap<std::pair<int,int>, std::pair<int,int> > NodeMapping ;
  typedef std::vector<std::pair<int,int> >  NodeList;
  
  class MEDPARTITIONER_EXPORT MESHCollection
  {

  public:

    //Default constructor
    MESHCollection();

    //Constructing from an existing mesh and a new topology
    MESHCollection(MESHCollection&, Topology*, bool family_splitting=false, bool create_empty_groups=false);

    //Constructing the mesh collection from a file
    MESHCollection(const std::string& filename);

    //Constructing the mesh collection from a file
    MESHCollection(const std::string& filename, ParaDomainSelector& domainSelector);

    //Constructing the mesh collection from a file
    MESHCollection(const std::string& filename, const std::string& meshname);

    ~MESHCollection();
        
    bool isParallelMode() const { return _domain_selector; }
        
    //writing to a distributed file
    void write(const std::string& filename);

    //getting the driver
    MESHCollectionDriver* retrieveDriver();
    MESHCollectionDriver* getDriver() const;
    void setDriverType(MEDPARTITIONER::DriverType type) {_driver_type=type;}

    //creation of the cell graph
    void buildCellGraph(MEDPARTITIONER::MEDSKYLINEARRAY* & array,int *& edgeweights );

    //creation and partition of the associated graph
    Topology* createPartition(int nbdomain, Graph::splitter_type type = Graph::METIS,
                              const std::string& ="", int* edgeweights=0, int* verticesweights=0);

    //creation of a user specified partition
    Topology* createPartition(const int* partition);

   //  //retrieving list of types
//       void getTypeList(int* cell_list,int nb_cells,MED_EN::medEntityMesh entity,
//                      MED_EN::medGeometryElement* type_list) const ;

//     //getting list of coordinates
//     void getCoordinates(int* node_list,int nb_nodes, double* coordinates) const ;

//     //getting connectivities
//     void getNodeConnectivity( const int*  cell_list,int nb_cells,MED_EN::medEntityMesh,MED_EN::medGeometryElement type, int* type_connectivity) const ;
//     void getPolygonNodeConnectivity(const int* cell_list,int nb_cells,MED_EN::medEntityMesh entity,
//                                     vector<int>& type_connectivity, vector<int>& connectivity_index) const;
//     void getPolyhedraNodeConnectivity(const int* cell_list,int nb_cells,MED_EN::medEntityMesh entity,
//                                       vector<int>& type_connectivity, vector<int>& connectivity_index, vector<int>& face_connectivity_index) const;

//     void getFaceConnectivity( const int*  cell_list,int nb_cells,MED_EN::medEntityMesh,MED_EN::medGeometryElement type, int* type_connectivity) const ;

//     //void getFaceConnectivity( const int*  cell_list,int nb_cells,MED_EN::medGeometryElement type, int* type_connectivity) const ;

//     //getting mesh dimension
     int getMeshDimension() const ;

//     //getting space dimension
//     int getSpaceDimension() const ;

//     //getting system of coordinates
//     std::string getSystem() const;

//     //getting name of the mesh
//     std::string getMeshName() const;

//     //return constituent entity
//     MED_EN::medEntityMesh getSubEntity() const;

//     //getting a reference to mesh vector
     std::vector<ParaMEDMEM::MEDCouplingUMesh*>& getMesh() ;
     std::vector<ParaMEDMEM::MEDCouplingUMesh*>& getFaceMesh() ;
     std::vector<std::vector<ParaMEDMEM::MEDCouplingUMesh*> >&getGroupMeshes();
 
     ParaMEDMEM::MEDCouplingUMesh* getMesh(int idomain) const;
     ParaMEDMEM::MEDCouplingUMesh* getFaceMesh(int idomain);
     std::vector<ParaMEDMEM::MEDCouplingUMesh*>& getGroupMeshes(int idomain);

     std::vector<ParaMEDMEM::DataArrayInt*>& getCellFamilyIds() {return _cellFamilyIds;}
     std::vector<ParaMEDMEM::DataArrayInt*>& getFaceFamilyIds() {return _faceFamilyIds;}

     std::map<std::string,int>& getFamilyInfo(){return _familyInfo;}
     std::map<std::string, std::vector<std::string> >& getGroupInfo(){return _groupInfo;}
 
//     //getting a reference to connect zones vector
    std::vector<MEDPARTITIONER::CONNECTZONE*>& getCZ();

    //getting a pointer to topology
    Topology* getTopology() const ;
    ParaDomainSelector* getParaDomainSelector() const{return _domain_selector;}
  //settig a new topology
    void setTopology(Topology* topology);

    //getting/setting the name of the global mesh (as opposed 
    //to the name of a subdomain \a nn, which is name_nn) 
    std::string getName() const {return _name;}
    void setName(const std::string& name){_name=name;}
    void setDomainNames(const std::string& name);

    //getting/setting the description of the global mesh
    std::string getDescription() const {return _description;}
    void setDescription(const std::string& name) { _description=name;}

    //creates the node mapping between an old collection and the present one
    void createNodeMapping( MESHCollection& initialCollection, std::multimap<std::pair<int,int>,std::pair<int,int> >& nodeMapping);

    void castCellMeshes(MESHCollection& initialCollection, std::vector<std::vector<std::vector<int> > >& new2oldIds);
    //creates faces on the new collection
    void castFaceMeshes( MESHCollection& initialCollection,const std::multimap<std::pair<int,int>,std::pair<int,int> >& nodeMapping);

    //creates faces on the new collection
    void castMeshes(std::vector<ParaMEDMEM::MEDCouplingUMesh*>& meshCastFrom,std::vector<ParaMEDMEM::MEDCouplingUMesh*>& meshCastTo,   MESHCollection& initialCollection,const std::multimap<std::pair<int,int>,std::pair<int,int> >& nodeMapping, std::vector<std::vector<std::vector<int> > >& old2newIds);
    

    //!transfers families from an old MESHCollection to new mesh
   //    void castFamilies(const MESHCollection& old_collection);

    //void castSupport(const MESHCollection& old_collection, std::vector<const MEDMEM::SUPPORT*>& old_support, std::vector<MEDMEM::SUPPORT*>& new_support);

//     //!casts all the fields to the new mesh collection
//     void castAllFields(const MESHCollection& old_collection);

//     //!casts one specific field to the new mesh collection
//     void castField(const MESHCollection& old_collection, const string& fieldname, int itnumber, int ordernumber);

//     //choosing a group to be indivisible
//     void setIndivisibleGroup(const string& a);

//     //!constructing connect zones
//     // void buildConnectZones(int idomain);
//     void buildConnectZones();
//     void buildConnectZonesBetweenProcs(std::vector <std::map <MED_EN::medGeometryElement, std::vector<MEDPARTITIONER_FaceModel*> > > &, map< pair<int,int>, MEDPARTITIONER::MEDSKYLINEARRAY*> & local_cell_cell_correspondency);

//     void addJointGroup(const std::vector<int>&, int idomain, int idistant);

//     static bool isDimensionOK(MED_EN::medGeometryElement type, int dim)
//     {
//       return ((type/100 == dim) || (dim==2 && type == MED_EN::MED_POLYGON) || (dim==3 && type == MED_EN::MED_POLYHEDRA));
//     }
//     void setSubdomainBoundaryCreates(bool flag) {  _subdomain_boundary_creates=flag;}
//     bool getSubdomainBoundaryCreates(){return _subdomain_boundary_creates;}

//     void setFamilySplitting(bool flag){_family_splitting=flag;}
//     bool getFamilySplitting(){return _family_splitting;}

//     void setCreateEmptyGroups(bool flag){_create_empty_groups=flag;}
//     bool getCreateEmptyGroups(){return _create_empty_groups;}


  private:

 //    //!creates connectivities for a domain and an entity (face or cell)
//     void createNodalConnectivity(const MESHCollection & initial_collection, int idomain, MED_EN::medEntityMesh entity);


//     //!projects a field from an old collection to the present one
//     //!field is identified by (name, dt, it)

    
    void castIntField(std::vector<ParaMEDMEM::MEDCouplingUMesh*>& meshesCastFrom,std::vector<ParaMEDMEM::MEDCouplingUMesh*>& meshesCastTo,  std::vector<ParaMEDMEM::DataArrayInt*>& arrayFrom,  std::vector<ParaMEDMEM::DataArrayInt*>& arrayTo, std::vector<std::vector< std::vector<int> > > & old2newMapping);

    void findCommonDistantNodes(std::vector<std::vector<std::multimap<int,int> > >& commonDistantNodes);
  //     template <class T>
    //     void castFields(const MESHCollection& old_collection, const string& fieldname, int itnumber, int ordernumber);
    
    //     void getFaces(int idomain, std::map<MED_EN::medGeometryElement, std::vector<MEDPARTITIONER_FaceModel*> >&);

//     MEDPARTITIONER_FaceModel* getCommonFace(int ip1,int ilocal1,int ip2,int ilocal2,int face_index);

//     template<class TID2CONN>
//     void fillGlobalConnectivity(TID2CONN & node2cell, TID2CONN & cell2node );

    //!link to mesh_collection topology
    Topology*                         _topology;

    //!control over topology
    bool                              _owns_topology;

    //!link to graph
    //    Graph*          _cell_graph;

    //! Driver for read/write operations
    MESHCollectionDriver*             _driver;

    //! Parallelizer - mark of parallel execution mode
    ParaDomainSelector*               _domain_selector;

    //!links to meshes
    std::vector<ParaMEDMEM::MEDCouplingUMesh*>        _mesh;
    std::vector<ParaMEDMEM::MEDCouplingUMesh*>        _faceMesh;
   
 //!index of a non empty mesh within _mesh (in parallel mode all of meshes can be empty)
    int                               _i_non_empty_mesh;

    //!links to connectzones
    std::vector<MEDPARTITIONER::CONNECTZONE*> _connect_zones;

    //!family ids storages
    std::vector<ParaMEDMEM::DataArrayInt*> _cellFamilyIds;
    std::vector<ParaMEDMEM::DataArrayInt*> _faceFamilyIds;

    //!group family conversion
    std::map<std::string,int> _familyInfo;
    std::map<std::string, std::vector<std::string> > _groupInfo;
  
    //!list of groups that are not to be splitted
    std::vector<std::string>          _indivisible_regions;

    //!name of global mesh
    std::string                            _name;

    //!description of global mesh
    std::string                            _description;

    //! specifies the driver associated to the collection
    DriverType                        _driver_type;

    /*! flag specifying that the splitter should create boundary constituent entity
      so that they are written in joints*/
    bool                              _subdomain_boundary_creates;

        /*! flag specifying that families must be preserved by the
                splitting*/
    bool                              _family_splitting;

        /*! flag specifying that groups must be created on all domains, 
                even if they are empty*/
    bool                              _create_empty_groups;

    JointFinder* _joint_finder;
  };

}//of namespace

#endif /*MESHCOLLECTION_*/
