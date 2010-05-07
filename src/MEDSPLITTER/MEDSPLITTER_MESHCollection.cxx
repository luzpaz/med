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

#include "MEDSPLITTER_MESHCollection.hxx"
#include "MEDSPLITTER_MESHCollectionDriver.hxx"
#include "MEDSPLITTER_MESHCollectionMedXMLDriver.hxx"
#include "MEDSPLITTER_MESHCollectionMedAsciiDriver.hxx"

#include "MEDSPLITTER_UserGraph.hxx"

#ifdef ENABLE_METIS
#include "MEDSPLITTER_METISGraph.hxx"
#endif
#ifdef ENABLE_SCOTCH
#include "MEDSPLITTER_SCOTCHGraph.hxx"
#endif

using namespace MEDSPLITTER;

#ifndef WNT
using namespace __gnu_cxx;
#else
using namespace std;
#endif

//template inclusion
#include "MEDSPLITTER_MESHCollection.H"
//#include "MEDSPLITTER_MESHCollectionDriver.H"



MESHCollection::MESHCollection()
  : m_topology(0),
    m_owns_topology(false),
    m_driver(0),
    m_driver_type(MEDSPLITTER::MedXML),
    m_subdomain_boundary_creates(false),
                m_family_splitting(false),
                m_create_empty_groups(false)
{
}

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

MESHCollection::MESHCollection(const MESHCollection& initial_collection, Topology* topology, bool family_splitting, bool create_empty_groups)
  : m_name(initial_collection.m_name),
    m_topology(topology),
    m_owns_topology(false),
    m_cell_graph(topology->getGraph()),
    m_driver(0),
    m_driver_type(MEDSPLITTER::MedXML),
    m_subdomain_boundary_creates(false),
                m_family_splitting(family_splitting),
                m_create_empty_groups(create_empty_groups)
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
      SCRUTE_MED(osname.str());
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
MESHCollection::MESHCollection(const string& filename)
  : m_topology(0),
    m_owns_topology(true),
    m_driver(0),
    m_driver_type(MEDSPLITTER::Undefined),
    m_subdomain_boundary_creates(false),
                m_family_splitting(false),
                m_create_empty_groups(false)
{
        char filenamechar[256];
        strcpy(filenamechar,filename.c_str());
        try
                {
                        m_driver=new MESHCollectionMedXMLDriver(this);
                        m_driver->read (filenamechar);
                        m_driver_type = MedXML;

                }
        catch(MEDEXCEPTION&){
                delete m_driver;
                try
                        {
                                m_driver=new MESHCollectionMedAsciiDriver(this);
                                m_driver->read (filenamechar);
                                m_driver_type=MedAscii;
                        }
                catch(MEDEXCEPTION&)
                        {
                                delete m_driver;
                                throw MEDEXCEPTION("file does not comply with any recognized format");
                        }
        }
}

/*! constructing the MESH collection from a sequential MED-file
 * 
 * \param filename MED file
 * \param meshname name of the mesh that is to be read
 */
MESHCollection::MESHCollection(const string& filename, const string& meshname)
  : m_name(meshname),
    m_topology(0),
    m_owns_topology(true),
    m_driver(0),
    m_driver_type(MEDSPLITTER::MedXML),
    m_subdomain_boundary_creates(false),
                m_family_splitting(false),
                m_create_empty_groups(false)
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
          if (m_mesh[i]!=0) {delete m_mesh[i]; }
        for (int i=0; i<m_connect_zones.size();i++)
          if (m_connect_zones[i]!=0) {delete m_connect_zones[i];}
        if (m_driver !=0) {delete m_driver; m_driver=0;}
        if (m_topology!=0 && m_owns_topology) {delete m_topology; m_topology=0;}
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
  
  
//  int nbnode_per_type=(int)type%100;
//  vector<int> number_of_types_array(m_topology->nbDomain(),0);
//  for (int i=0; i<m_topology->nbDomain(); i++)
//    number_of_types_array[i]=m_mesh[i]->getNumberOfTypes(entity);
    
  //defining a connectivity table for different domains 
  vector  <const int*> conn_ip(m_topology->nbDomain());
  vector  <const int*> conn_index_ip(m_topology->nbDomain());

  
  vector< map <MED_EN::medGeometryElement, int> > offset;
//  offset.resize(m_topology->nbDomain());
  
  for (int i=0; i<m_topology->nbDomain();i++)
    {
      
      int nb_elem = m_mesh[i]->getNumberOfElementsWithPoly(entity,type);
      if (nb_elem>0)
      {
        conn_ip[i]=m_mesh[i]->getConnectivity(MED_EN::MED_FULL_INTERLACE,
                                              MED_EN::MED_NODAL,entity,MED_EN::MED_ALL_ELEMENTS);
        conn_index_ip[i] = m_mesh[i]->getConnectivityIndex(MED_EN::MED_NODAL,entity);
 //       global_index= m_mesh[i]->getGlobalNumberingIndex(entity);
      }                                             
      else
      {
        conn_ip[i]=0;
        conn_index_ip[i]=0;
      }     
//      int number_of_types = number_of_types_array[i];
//      const MEDMEM::CELLMODEL* types =  m_mesh[ip[icell]]->getCellsTypes(entity);
//      for (int itype=0; itype<number_of_types; itype++) 
//        offset[i][types[itype].getType()]=global_index[itype]-1;
    }
  
  int* type_connectivity_ptr=type_connectivity;
  for (int icell=0; icell<nb_cells; icell++)
    {
//      int type_offset = offset[ip[icell]][type];
      const int* conn=conn_ip[ip[icell]]; 
      const int* conn_index=conn_index_ip[ip[icell]];
      for (int inode=conn_index[local[icell]-1]; inode<conn_index[local[icell]]; inode++)
        {
          *type_connectivity_ptr=
            m_topology->convertNodeToGlobal(ip[icell],conn[inode-1]);
          type_connectivity_ptr++;                       
        }
    }
  
  delete[]local;
  delete[]ip;
}

/*!gets the connectivity for MED_POLYGON type
 *
 * \param cell_list list of elements (global cell numbers) for which the connectivity is required
 * \param nb_cells number of elements
 * \param entity type of entity for which the nodal connectivity is required
 * \param type_connectivity on output contains the connectivity of all the elements of the list
 * \param connectivity_index on output contains the connectivity index for all polygons 
 * */ 
 
void MESHCollection::getPolygonNodeConnectivity(const int* cell_list,int nb_cells,MED_EN::medEntityMesh entity,
                                          vector<int>& type_connectivity, vector<int>& connectivity_index) const
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
  
  
  //defining a connectivity table for different domains 
  vector  <const int*> conn_ip(m_topology->nbDomain());
  vector  <const int*> conn_index_ip(m_topology->nbDomain());
  vector <const int* > conn_face_index(m_topology->nbDomain());
  vector<int> nb_plain_elems(m_topology->nbDomain());
  
  vector< map <MED_EN::medGeometryElement, int> > offset;
  
  for (int i=0; i<m_topology->nbDomain();i++)
    {
      nb_plain_elems[i] = m_mesh[i]->getNumberOfElements(entity, MED_EN::MED_ALL_ELEMENTS);
      int nb_elem = m_mesh[i]->getNumberOfElementsWithPoly(entity,MED_EN::MED_POLYGON);
      if (nb_elem>0)
      {
          conn_ip[i]=m_mesh[i]->getPolygonsConnectivity(MED_EN::MED_NODAL,entity);
          conn_index_ip[i] = m_mesh[i]->getPolygonsConnectivityIndex(MED_EN::MED_NODAL,entity);
      }                                             
      else
      {
        conn_ip[i]=0;
        conn_index_ip[i]=0;
      }     
    }
  
  connectivity_index.resize(nb_cells+1);
  connectivity_index[0]=1;
  for (int icell=0; icell<nb_cells; icell++)
    {
       int nb_plain= nb_plain_elems[ip[icell]];
      const int* conn=conn_ip[ip[icell]]; 
      const int* conn_index=conn_index_ip[ip[icell]];
      for (int inode=conn_index[local[icell]-1-nb_plain]; inode<conn_index[local[icell]-nb_plain]; inode++)
        {
          type_connectivity.push_back(
            m_topology->convertNodeToGlobal(ip[icell],conn[inode-1]));
        }
      connectivity_index[icell+1]=connectivity_index[icell]
         -conn_index[local[icell]-1-nb_plain]+conn_index[local[icell]-nb_plain];
    }
  
  delete[]local;
  delete[]ip;
}


/*!gets the connectivity for MED_POLYHEDRA type
 *
 * \param cell_list list of elements (global cell numbers) for which the connectivity is required
 * \param nb_cells number of elements
 * \param entity type of entity for which the nodal connectivity is required
 * \param type_connectivity on output contains the connectivity of all the elements of the list
 * \param connectivity_index on output contains the connectivity index for all polygons 
 * */ 
 
void MESHCollection::getPolyhedraNodeConnectivity(const int* cell_list,int nb_cells,MED_EN::medEntityMesh entity,
                                          vector<int>& type_connectivity, vector<int>& connectivity_index, vector<int>& face_connectivity_index) const
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
  
  
  //defining a connectivity table for different domains 
  vector  <const int*> conn_ip(m_topology->nbDomain());
  vector  <const int*> conn_index_ip(m_topology->nbDomain());
  vector  <const int*> conn_face_index_ip(m_topology->nbDomain());
  vector<int> nb_plain_elems(m_topology->nbDomain());
  
  vector< map <MED_EN::medGeometryElement, int> > offset;
  
  for (int i=0; i<m_topology->nbDomain();i++)
    {
      nb_plain_elems[i] = m_mesh[i]->getNumberOfElements(entity, MED_EN::MED_ALL_ELEMENTS);
      int nb_elem = m_mesh[i]->getNumberOfElementsWithPoly(entity,MED_EN::MED_POLYHEDRA);
      if (nb_elem>0)
      {
          conn_ip[i]=m_mesh[i]->getPolyhedronConnectivity(MED_EN::MED_NODAL);
          conn_index_ip[i] = m_mesh[i]->getPolyhedronIndex(MED_EN::MED_NODAL);
          conn_face_index_ip[i]= m_mesh[i]->getPolyhedronFacesIndex();
      }                                             
      else
      {
        conn_ip[i]=0;
        conn_index_ip[i]=0;
        conn_face_index_ip[i]=0;
      }     
    }
  
  connectivity_index.resize(nb_cells+1);
  connectivity_index[0]=1;
  face_connectivity_index.resize(1);
  face_connectivity_index[0]=1;
  for (int icell=0; icell<nb_cells; icell++)
    {
      const int* conn=conn_ip[ip[icell]]; 
      const int* conn_index=conn_index_ip[ip[icell]];
      const int* conn_face_index=conn_face_index_ip[ip[icell]];
      int local_in_poly = local[icell]-nb_plain_elems[ip[icell]];
      connectivity_index[icell+1]=connectivity_index[icell]+
         conn_index[local_in_poly]-conn_index[local_in_poly-1];
     
      for (int iface=conn_index[local_in_poly-1]; iface<conn_index[local_in_poly]; iface++)
        {
          int last_face= face_connectivity_index[face_connectivity_index.size()-1];
          face_connectivity_index.push_back(last_face+conn_face_index[iface]-conn_face_index[iface-1]);
          for (int inode=conn_face_index[iface-1];
                inode < conn_face_index[iface];
                inode++)
                type_connectivity.push_back(
                    m_topology->convertNodeToGlobal(ip[icell],conn[inode-1]));
        }
      
    }
  
  delete[]local;
  delete[]ip;
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
  //building the connect zones necessary for writing joints
        cout<<"Building Connect Zones"<<endl;
  if (m_topology->nbDomain()>1)
    buildConnectZones();
  cout <<"End of connect zones building"<<endl;
        //suppresses link with driver so that it can be changed for writing
        if (m_driver!=0)delete m_driver;
        m_driver=0;

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
                        switch(m_driver_type)
                                {
                                case MedXML:
                                        m_driver=new MESHCollectionMedXMLDriver(this);
                                        break;
                                case MedAscii:
                                        m_driver=new MESHCollectionMedAsciiDriver(this);
                                        break;
                                default:
                                        throw MEDEXCEPTION("Unrecognized driver");
                                }
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
  MESSAGE_MED (" Beginning of getTypeList with entity "<<entity);
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
                        type_list[icell]=m_mesh[ip[icell]]->getElementTypeWithPoly(entity,local[icell]);                                                                                
                }
        delete[]local;
        delete[]ip;
  MESSAGE_MED("end of getTypeList");
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
                number_of_types_array[i]=m_mesh[i]->getNumberOfTypesWithPoly(entity);
                
        //defining a connectivity table for different domains   
        vector  <const int*> conn_ip(m_topology->nbDomain());
        for (int i=0; i<m_topology->nbDomain();i++)
                {
                        int nb_elem = m_mesh[i]->getNumberOfElementsWithPoly(entity,type);
                        if (nb_elem>0)
                                conn_ip[i]=m_mesh[i]->getConnectivity(MED_EN::MED_FULL_INTERLACE,                                                    MED_EN::MED_DESCENDING,entity,type);                               
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
            if (types[itype].getType() < type)
              type_offset += m_mesh[ip[icell]]->getNumberOfElementsWithPoly(entity,types[itype].getType());
          }
          const int* conn=conn_ip[ip[icell]];                                           
          for (int iface=0; iface<nbface_per_type; iface++)
          {
            type_connectivity[icell*nbface_per_type+iface] = m_topology->convertFaceToGlobal
              (ip[icell], abs(conn[(local[icell] - type_offset - 1) * nbface_per_type + iface]));                                               
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
    m_topology = topo;
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
 *                  on output contains the tags 
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
    
  int cell_number=1;
  int node_number=1;
  for (int i=0; i<m_topology->nbDomain(); i++)
    {
      cell_number+=m_topology->getCellNumber(i);
      node_number+=m_topology->getNodeNumber(i);
    }
  //list of cells for a given node
  vector< vector<int> > node2cell(node_number);
  
  //list of nodes for a given cell
  vector< vector <int> > cell2node(cell_number);
  
//  map<MED_EN::medGeometryElement,int*> type_cell_list;
  
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
      //  MED_EN::medGeometryElement* type_array;
      int nb_cells= m_topology->nbCells(idomain);
      int* cell_list = new int[nb_cells];
      
      //retrieving global id list
      m_topology->getCellList(idomain, cell_list);

      int nb_plain_cells = m_mesh[idomain]->getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);

      if (nb_plain_cells >0)
      {          
        const int* conn_index = m_mesh[idomain]->getConnectivityIndex(MED_EN::MED_NODAL,
                                                                        MED_EN::MED_CELL);
        
        const int* conn =   m_mesh[idomain]->getConnectivity(MED_EN::MED_FULL_INTERLACE,
                                                                        MED_EN::MED_NODAL,
                                                                            MED_EN::MED_CELL,
                                                                        MED_EN::MED_ALL_ELEMENTS);                                                                    
        int nbnodes = conn_index[nb_plain_cells]-1;
        int* global_nodes =new int [nbnodes];
        m_topology->convertNodeToGlobal(idomain, conn, nbnodes, global_nodes);
        //cout << "global nodes"<<endl;
        //for (int inode=0; inode<nbnodes;inode++) cout << global_nodes[inode]<<" ";
        for (int icell=0; icell< nb_plain_cells; icell++)
        {
          //cout << " nb nodes"<< conn_index[icell+1]-conn_index[icell]<<endl;
          for (int inode=conn_index[icell]; inode < conn_index[icell+1]; inode++)
             {
              int node_global_id = global_nodes[inode-1];
              int cell_global_id = cell_list[icell];
              cell2node [cell_global_id].push_back(node_global_id);
              node2cell [node_global_id].push_back(cell_global_id);
             }
        }
        delete[] global_nodes;
      }
      if (m_mesh[idomain]->existPolygonsConnectivity(MED_EN::MED_CELL, MED_EN::MED_POLYGON))
      {
          
        const int* conn_index = m_mesh[idomain]->getPolygonsConnectivityIndex(MED_EN::MED_NODAL,
                                                                        MED_EN::MED_CELL);
        
        const int* conn =   m_mesh[idomain]->getPolygonsConnectivity(MED_EN::MED_NODAL,
                                                             MED_EN::MED_CELL);
        
        int nb_poly_cells =      m_mesh[idomain]->getNumberOfPolygons();                                                                  
        int nbnodes = conn_index[nb_poly_cells]-1;
        int* global_nodes =new int [nbnodes];
        m_topology->convertNodeToGlobal(idomain, conn, nbnodes, global_nodes);
        //cout << "global nodes"<<endl;
        //for (int inode=0; inode<nbnodes;inode++) cout << global_nodes[inode]<<" ";
        for (int icell=0; icell< nb_poly_cells; icell++)
        {
          //cout << " nb nodes"<< conn_index[icell+1]-conn_index[icell]<<endl;
          for (int inode=conn_index[icell]; inode < conn_index[icell+1]; inode++)
             {
              int node_global_id = global_nodes[inode-1];
              int cell_global_id = cell_list[icell];
              cell2node [cell_global_id].push_back(node_global_id);
              node2cell [node_global_id].push_back(cell_global_id);
             }
        }
        delete[] global_nodes;
      }
      if (m_mesh[idomain]->existPolyhedronConnectivity(MED_EN::MED_CELL, MED_EN::MED_POLYHEDRA))
      {
          
        const int* conn_index = m_mesh[idomain]->getPolyhedronIndex(MED_EN::MED_NODAL);
        const int* conn = m_mesh[idomain]->getPolyhedronConnectivity(MED_EN::MED_NODAL);
        const int* face_conn = m_mesh[idomain]->getPolyhedronFacesIndex();
        
        int nb_poly_cells =      m_mesh[idomain]->getNumberOfPolyhedron();                                                                  
        int nbfaces = face_conn[nb_poly_cells]-1;
        int nbnodes = conn_index[nbfaces]-1;
        int* global_nodes =new int [nbnodes];
        m_topology->convertNodeToGlobal(idomain, conn, nbnodes, global_nodes);
        //cout << "global nodes"<<endl;
        //for (int inode=0; inode<nbnodes;inode++) cout << global_nodes[inode]<<" ";
        for (int icell=0; icell< nb_poly_cells; icell++)
        {
          for (int iface=conn_index[icell]; iface < conn_index[icell+1]; iface++)
          {
            //cout << " nb nodes"<< conn_index[icell+1]-conn_index[icell]<<endl;
            for (int inode=face_conn[iface-1]; inode < face_conn[iface]; inode++)
               {
                int node_global_id = global_nodes[inode-1];
                int cell_global_id = cell_list[icell];
                cell2node [cell_global_id].push_back(node_global_id);
                node2cell [node_global_id].push_back(cell_global_id);
               }
           }
        }
        
        if (nbnodes>0) delete[] global_nodes;
      }
      if (nb_cells>0) delete[] cell_list;
    }
    
  cout << "beginning of skyline creation"<<endl;
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
        
     
      vector<int> cells(50);
     
//     /*// cout << "size cell2node "<<cell2node[i+1].size()<<endl;
//      for (vector<int>::const_iterator iternode=cell2node[i+1].begin();
//            iternode!=cell2node[i+1].end();
//            iternode++)
//        {
//          int nodeid=*iternode;
//       //   cout << "size node2cell "<<node2cell[nodeid].size()<<endl;
//          for (vector<int>::const_iterator iter=node2cell[nodeid].begin();
//             iter != node2cell[nodeid].end();
//             iter++)
//              cells_neighbours[*iter]++;
//        }*/
        
        for (int inode=0; inode< cell2node[i+1].size(); inode++)
        {
          int nodeid=cell2node[i+1][inode];
       //   cout << "size node2cell "<<node2cell[nodeid].size()<<endl;
          for (int icell=0; icell<node2cell[nodeid].size();icell++)
              cells_neighbours[node2cell[nodeid][icell]]++;
        }
      size[i]=0;
      int dimension = getMeshDimension();
      cells.clear();
   
      for (map<int,int>::const_iterator iter=cells_neighbours.begin(); iter != cells_neighbours.end(); iter++)  
        {
          if (iter->second >= dimension && iter->first != i+1) 
            {
           cells.push_back(iter->first);
       //       cells[isize++]=iter->first;
            }
        }
      size[i]=cells.size();
   //   size[i]=isize;
      
      //cout << cells.size()<<endl;
      //cout << cells_neighbours.size()<<endl;
      
      temp[i]=new int[size[i]];
     if (has_indivisible_regions)
      temp_edgeweight[i]=new int[size[i]];
//    
      int itemp=0;
      
     // memcpy(temp[i],cells,isize*sizeof(int));
      
        for (vector<int>::const_iterator iter=cells.begin(); iter!=cells.end();iter++)
        //for(int j=0; j<isize; j++)
        {
            temp[i][itemp]=*iter;
            //temp[i][itemp]=cells[j];
              if (has_indivisible_regions)
                {
                  int tag1 = indivisible_tag[(i+1)-1];
                  //int tag2 = indivisible_tag[iter->first-1];
                  int tag2 = indivisible_tag[*iter-1];
                  if (tag1==tag2 && tag1!=0)
                    temp_edgeweight[i][itemp]=m_topology->nbCells()*100000;
                  else
                    temp_edgeweight[i][itemp]=1;
                } 
              itemp++;
        }
      cells_neighbours.clear();
    }
  cout <<"end of graph definition"<<endl;
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
    }
  delete[] index;
  delete[] temp;
  delete[] size;
  
cout<< "end of graph creation"<<endl;
}

/*! Creates the partition corresponding to the cell graph and the partition number
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

        MESSAGE_MED("Building cell graph");
        buildCellGraph(array,edgeweights);

        switch (split)
                {
                case Graph::METIS:
#ifdef ENABLE_METIS
                        m_cell_graph=boost::shared_ptr<Graph>(new METISGraph(array,edgeweights));
#else
                        throw MEDEXCEPTION("METIS Graph is not available. Check your products, please.");
#endif
                        break;
                case Graph::SCOTCH:
#ifdef ENABLE_SCOTCH
                        m_cell_graph=boost::shared_ptr<Graph>(new SCOTCHGraph(array,edgeweights));
#else
                        throw MEDEXCEPTION("SCOTCH Graph is not available. Check your products, please.");
#endif
                        break;
                }

        //!user-defined weights
        if (user_edge_weights!=0) 
          m_cell_graph->setEdgesWeights(user_edge_weights);
        if (user_vertices_weights!=0)
          m_cell_graph->setVerticesWeights(user_vertices_weights);

        MESSAGE_MED("Partitioning graph");
        m_cell_graph->partGraph(nbdomain,options_string);

        MESSAGE_MED("Building new topology");
        //m_cell_graph is a shared pointer 
        Topology* topology = new ParallelTopology (m_cell_graph, nbdomain, getMeshDimension());

        //cleaning
        if (edgeweights!=0) delete[] edgeweights;
        //if (array!=0) delete array;
        MESSAGE_MED("End of partition creation");
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

        cout << "Computing node/node corresp"<<endl;

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
        cout << "Computing node/node corresp"<<endl;

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
                                                throw MEDEXCEPTION("MESHCollection::buildConnectZones() -A connect zone should exist");   
                                        //delete cell_cell_correspondency[idistant];
                                }
        
                }
}
/*! building Connect Zones for storing the informations
 * of the connectivity 
 * */
 
void MESHCollection::buildConnectZones()
{
  cout << "Computing node/node corresp"<<endl;


   //Creating nodes
  for (int idomain=0; idomain<m_topology->nbDomain(); idomain++)
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
  }
  cout << "Computing face corresp"<<endl;

 //creating faces if required 
  if (m_subdomain_boundary_creates)
  {
    int global_face_id = m_topology->getFaceNumber()+1;
    vector <map <MED_EN::medGeometryElement, vector<MEDSPLITTER_FaceModel*> > > face_map(m_topology->nbDomain());
    
     map <pair<int,int>, vector<int> > faces_in_joint;

 // taking faces that are already present in the mesh into account
    for (int idomain=0; idomain<m_topology->nbDomain(); idomain++)
    {
     getFaces(idomain,face_map[idomain]); 
    }  
    
 // creating faces that are located at the interface between
 // subdomains 
           
    for (int idomain=0; idomain<m_topology->nbDomain(); idomain++)
    {
      vector<MEDMEM::MEDSKYLINEARRAY*> cell_cell_correspondency;
      cell_cell_correspondency.resize(m_topology->nbDomain());
    
      m_topology->computeCellCellCorrespondencies(idomain, cell_cell_correspondency, m_cell_graph.get());

    for (int idistant=0; idistant< m_topology->nbDomain(); idistant++)
    {
      if (idistant <= idomain) continue;
        
      //the connect zone has been created by the node/node computation
     
      if (cell_cell_correspondency[idistant]!=0)
        {
          int nbcells = cell_cell_correspondency[idistant]->getNumberOf();
                                        //                                      MEDMEM::CELLMODEL_Map cellmodel_map;
          for (int ilocal=0; ilocal<nbcells; ilocal++)
          { 
                                                //            medGeometryElement local_type = m_mesh[idomain]->getElementType(MED_EN::MED_CELL,ilocal+1);
                                                //            MEDMEM::CELLMODEL  local_model = cellmodel_map.retrieveCellModel (local_type);
            const int* index = cell_cell_correspondency[idistant]->getIndex();
            const int* value = cell_cell_correspondency[idistant]->getValue();
            for (int icelldistant = index[ilocal]; icelldistant < index[ilocal+1]; icelldistant++)
            {
              int distant_id = value[icelldistant-1];
                                                        //               medGeometryElement distant_type = m_mesh[idistant]->getElementType(MED_EN::MED_CELL,distant_id);
                                                         //                                                      MEDMEM::CELLMODEL distant_model = cellmodel_map.retrieveCellModel (distant_type);
               MEDSPLITTER_FaceModel* face = getCommonFace(idomain,ilocal+1,idistant,distant_id,global_face_id);
               face_map[idomain][face->getType()].push_back(face);
               MEDSPLITTER_FaceModel* face2 = getCommonFace(idistant,distant_id,idomain, ilocal+1,global_face_id);
               face_map[idistant][face->getType()].push_back(face2);
               faces_in_joint[make_pair(idomain,idistant)].push_back(global_face_id);
               global_face_id++;
            } 
          }
        }
        
     }
     //cleaning up
     for (int idistant=0; idistant< m_topology->nbDomain(); idistant++)
       delete cell_cell_correspondency[idistant];
    }  
        
  
    m_topology->recreateFaceMapping(face_map);
    
     //transforming the face_map into a constituent entity connectivity
    for (int idomain=0; idomain< m_topology->nbDomain();idomain++) 
     {
       int nbtypes = face_map[idomain].size();
       vector<medGeometryElement> types;
       vector <int> nb_elems;
       vector <int*> conn;
       
       MEDMEM::MESHING* meshing = dynamic_cast<MEDMEM::MESHING*> (m_mesh[idomain]);
       MED_EN::medEntityMesh constituent_entity = (getMeshDimension()==3)?MED_EN::MED_FACE:MED_EN::MED_EDGE;
       
       for (map <medGeometryElement, vector<MEDSPLITTER_FaceModel*> >::const_iterator iter= face_map[idomain].begin();
            iter != face_map[idomain].end(); iter ++)
       {
         types.push_back(iter->first);
         int nb_elem_in_type = (iter->second).size();
         nb_elems.push_back(nb_elem_in_type);
         int nb_node_per_type=(iter->first)%100;
         int* connectivity= new int [nb_node_per_type*nb_elem_in_type];
         for (int ielem=0; ielem<nb_elem_in_type; ielem++)
         {
           for (int inode=0;  inode<nb_node_per_type; inode++)
           connectivity[ielem*nb_node_per_type+inode]=(*(iter->second)[ielem])[inode];
         }
         conn.push_back(connectivity);
                
       }
       //setting the faces in the mesh
       meshing->setNumberOfTypes(nbtypes,constituent_entity);
       meshing->setTypes(&types[0],constituent_entity);
       meshing->setNumberOfElements(&nb_elems[0],constituent_entity);
              
       for (int itype=0; itype<nbtypes; itype++)
       {
         meshing->setConnectivity(conn[itype], constituent_entity, types[itype]);
         delete[]conn[itype];
       }
       for (int idistant =0; idistant<m_topology->nbDomain(); idistant++)
        {
          map <pair<int,int>, vector<int> >::iterator iter;
          iter = faces_in_joint.find(make_pair(idomain,idistant));
          if (iter == faces_in_joint.end())
          {
            iter = faces_in_joint.find (make_pair(idistant,idomain));
            if (iter == faces_in_joint.end()) 
              continue;
          }
              
          int nbfaces = (iter->second).size();   
          vector<int> face_joint(nbfaces*2);
          MEDMEM::CONNECTZONE* cz=0;
          for (int icz=0; icz<m_connect_zones.size();icz++)
            if (m_connect_zones[icz]->getLocalDomainNumber()==idomain &&
              m_connect_zones[icz]->getDistantDomainNumber()==idistant)
              cz = m_connect_zones[icz];
          
          int nbtotalfaces= m_topology->getFaceNumber(idomain);
          
          //creating arrays for the MEDSKYLINEARRAY structure containing the joint
          int* index =new int[nbtotalfaces+1];
          for (int i=0; i<nbtotalfaces+1;i++)
            index[i]=0;
          int*value=new int[nbfaces];
          
          map<int,int> faces;
          for (int iface=0; iface<nbfaces; iface++)
          {
            int iglobal = (iter->second)[iface];
            int localid=m_topology->convertGlobalFace(iglobal,idomain);
            int distantid=m_topology->convertGlobalFace(iglobal,idistant);
            faces.insert(make_pair(localid,distantid));
          }
          
          int iloc=0;
          index[0]=1;
          for (map<int,int>::const_iterator iter=faces.begin(); 
          iter != faces.end();
          iter++)
          {
            index[iter->first]=1;
            value[iloc++]=iter->second;            
          }
          
          //creating a group for the faces constituting the joint
          ostringstream jointname ;
          jointname << "joint_"<<idistant+1;
          MEDMEM::GROUP joint_group;
          joint_group.setName(jointname.str().c_str());
          joint_group.setMesh(meshing);
          joint_group.setEntity(constituent_entity);
          map<MED_EN::medGeometryElement, vector<int> > joint_types;
          
          for (int i=0; i<nbfaces; i++)
            {    
              int iglobal = (iter->second)[i];
              int localid=m_topology->convertGlobalFace(iglobal,idomain);
              MED_EN::medGeometryElement type = meshing->getElementType(constituent_entity,localid);
              joint_types[type].push_back(localid);
              
            }                    
          joint_group.setNumberOfGeometricType(joint_types.size());
          MED_EN::medGeometryElement* types=new MED_EN::medGeometryElement[joint_types.size()];
          int* nb_in_types=new int[joint_types.size()];
          int* group_index=new int[joint_types.size()+1];
          
          group_index[0]=1;
          int itype=0;
          int iface =0;
          int* group_value=new int[nbfaces];
          for (map<MED_EN::medGeometryElement, vector<int> >::const_iterator iterj=joint_types.begin();
               iterj != joint_types.end();
               iterj++)
               {
                nb_in_types[itype]=(iterj->second).size();
                types[itype]=iterj->first;
                itype++;
                group_index[itype]=group_index[itype-1]+(iterj->second).size();
                for (int i=0; i<  (iterj->second).size(); i++)
                  group_value[iface++]=(iterj->second)[i];
               }
          joint_group.setGeometricType(types);
          joint_group.setNumberOfElements(nb_in_types);
          joint_group.setNumber(group_index, group_value);             
          delete[] types;
          delete[] nb_in_types;
          //delete[] group_index;
          meshing->addGroup(joint_group);      
          delete[] group_index;
          delete[] group_value;  
          //end of group creation
          
          for (int i=0; i<nbtotalfaces;i++)
            index[i+1]+=index[i];
          bool shallowcopy=true;  
          MEDMEM::MEDSKYLINEARRAY* skarray=new MEDMEM::MEDSKYLINEARRAY(nbtotalfaces,nbfaces,index,value,shallowcopy);  

          if (cz!=0)  
            cz->setEntityCorresp(constituent_entity,constituent_entity,skarray);              
          else 
            throw MEDEXCEPTION("MESHCollection::buildConnectZones() -A connect zone should exist");            
        }
     }
     
     for (int idomain=0; idomain<m_topology->nbDomain(); idomain++)
       for (map <medGeometryElement, vector<MEDSPLITTER_FaceModel*> >::const_iterator iter= face_map[idomain].begin();
            iter != face_map[idomain].end(); iter ++)
          for (int i=0; i<(iter->second).size();i++)
            delete (iter->second)[i];
  } 
  
 cout << "Computing cell/cell corresp"<<endl;

  //Creating cell/cell correspondencies
  for (int idomain=0;idomain<m_topology->nbDomain();idomain++)
  {
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
              throw MEDEXCEPTION("MESHCollection::buildConnectZones() -A connect zone should exist");   
            //delete cell_cell_correspondency[idistant];
          }
          
      }
  }
}

/*! projects old collection families on new collection families
 */
void MESHCollection::castFamilies(const MESHCollection& old_collection)
{
        vector <list<int> > element_array  (m_topology->nbDomain());
        
        //loop on old domains to create groups out of the existing families
        if (m_family_splitting)
                for (int idomain=0; idomain < old_collection.m_topology->nbDomain(); idomain++)
                                old_collection.getMesh(idomain)->createGroups();
                        
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
                        if (nbelem==0 && !m_create_empty_groups) continue;
                
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
                                                //                                                      cell_arrays[ip[i]][local[i]]=id;
                                                {
                                                        //                                      cout <<"(glob,ip,iloc)/nbelem"<<array[i]<<" "<<ip[i]<<" "<<local[i]<<"/"<<nbelem<<endl;
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
  MESSAGE_MED ("beginning of createNodalConnectivity for entity "<<entity);
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
 
  if (nb_elems == 0) return;
  SCRUTE_MED(nb_elems);
  
        
        int *list= new int[nb_elems];
        MED_EN::medGeometryElement *cell_type_list= new MED_EN::medGeometryElement[nb_elems];
        
  
        //      cout << "Beginning of retrieval "<<endl;
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
        //      cout <<"end of type retrieval"<<endl;
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
        std::map<MED_EN::medGeometryElement,int>::const_iterator iter;
        //currentEntity  = MED_EN::meshEntities.find(entity);
        for (iter = type_numbers.begin();iter != type_numbers.end(); iter++)    
                {
      MED_EN::medGeometryElement type = iter->first;
                        if (!isDimensionOK(type,dimension)) continue;
                        //if (iter->second==0) continue;
                        index[type]=0;
                        type_cell_list[type]=new int[type_numbers[type]];
     // cout << "type :"<<type<<" nb:"<<type_numbers[type]<<endl;
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
        MESSAGE_MED("Nb de types presents "<<nb_present_types);
        int itype=0;
   for (iter = type_numbers.begin();iter != type_numbers.end(); iter++)  
        {
      MED_EN::medGeometryElement type = iter->first;
      if (!isDimensionOK(type,dimension)) continue;
     
                        type_array[itype]=type;
                
                        present_type_numbers[itype]=type_numbers[type];
                
                        MESSAGE_MED("Nombre d'elements de type "<<type<<" : "<<type_numbers[type]);
                        itype++;
                }
                
        //retrieving connectivity in global numbering for each typeinitial_collection.getMesh(iold)->get
        map<MED_EN::medGeometryElement,int*> type_connectivity;
        vector<int> polygon_conn;
  vector<int> polygon_conn_index;
  vector<int> polyhedron_conn;
  vector<int> polyhedron_conn_index;
  vector<int> polyhedron_face_index;
  
        //Treating nodes
                
        
  for (iter = type_numbers.begin();iter != type_numbers.end(); iter++)  
    {
      MED_EN::medGeometryElement type = iter->first;
   
                
      if (!isDimensionOK(type,dimension)) continue;
                        //if (type_numbers[type]==0) continue;
      if (type != MED_EN::MED_POLYGON && type != MED_EN::MED_POLYHEDRA)
      { 
                          int nbnode_per_type = (int)type%100;
                          type_connectivity[type]=new int[type_numbers[type]*nbnode_per_type];
              initial_collection.getNodeConnectivity(type_cell_list[type],type_numbers[type],entity,type,type_connectivity[type]);
      }
      else if (type == MED_EN::MED_POLYGON && dimension==2)
      {
        initial_collection.getPolygonNodeConnectivity(type_cell_list[type],type_numbers[type],entity,polygon_conn,polygon_conn_index);
        //type_connectivity[type]=&polygon_conn[0];
      }
      else if (type == MED_EN::MED_POLYHEDRA && dimension==3)
      {
        initial_collection.getPolyhedraNodeConnectivity(type_cell_list[type],type_numbers[type],entity,polyhedron_conn,
                                                        polyhedron_conn_index, polyhedron_face_index);
        //type_connectivity[type]=&polygon_conn[0];
      }
      delete[] type_cell_list[type];
                }       
                
        //creating node mapping 
        //!TODO : compute the total number of nodes 
        if (entity==MED_EN::MED_CELL)
    {
                  m_topology->createNodeMapping(type_connectivity,type_numbers,polygon_conn,polygon_conn_index,
                                    polyhedron_conn,polyhedron_conn_index,polyhedron_face_index,idomain);
    }   
    
        //converting node global numberings to local numberings
  //for (iter = (*currentEntity).second.begin();iter != (*currentEntity).second.end(); iter++)
  for (iter = type_numbers.begin();iter != type_numbers.end(); iter++)  
    {
      MED_EN::medGeometryElement type = iter->first;
       
      if (!isDimensionOK(type, dimension)) continue;
      if (type_numbers[type]==0) continue;
      if (type != MED_EN::MED_POLYGON && type != MED_EN::MED_POLYHEDRA)
      { 
        int nbnode_per_type = (int)type%100;
        m_topology->convertToLocal2ndVersion(type_connectivity[type],type_numbers[type]*nbnode_per_type,idomain);
      }
      else if (type == MED_EN::MED_POLYGON && dimension==2)
      {
        int nbpoly = type_numbers[type]; 
        m_topology->convertToLocal2ndVersion(&polygon_conn[0], polygon_conn_index[nbpoly]-1, idomain);  
      }
      else if (type == MED_EN::MED_POLYHEDRA && dimension==3)
      {
        int nbpoly = type_numbers[type]; 
        m_topology->convertToLocal2ndVersion(&polyhedron_conn[0], polyhedron_face_index[polyhedron_conn_index[nbpoly]-1]-1, idomain);  
      }
      
    } 
                
                
        //writing coordinates
        if (entity==MED_EN::MED_CELL)   
                {
                        //setting coordinates from initial_collection coordinates
                        int nbnode=m_topology->getNodeNumber(idomain);
                        MESSAGE_MED("Number of nodes on domain "<< idomain <<" : "<<nbnode);
                
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

  int nb_plain_types=0;
  for (iter = type_numbers.begin();iter != type_numbers.end(); iter++) 
  { 
         MED_EN::medGeometryElement type = iter->first;
       
     if (!isDimensionOK(type, dimension)) continue;
     if (type_numbers[type]==0) continue;
     if (type != MED_EN::MED_POLYGON && type != MED_EN::MED_POLYHEDRA)
     nb_plain_types++;
  }
        mesh_builder->setNumberOfTypes(nb_plain_types,entity);
        mesh_builder->setTypes(type_array,entity);
        mesh_builder->setNumberOfElements(present_type_numbers,entity);
  if (entity==MED_EN::MED_CELL)
    mesh_builder->setMeshDimension(dimension);
  
        delete[]present_type_numbers;
        delete[]type_array;
        //setting node connectivities
  for (iter = type_numbers.begin();iter != type_numbers.end(); iter++)  
    {
      MED_EN::medGeometryElement type = iter->first;
   
                        if (!isDimensionOK(type,dimension)) continue;
            if (type_numbers[type]==0) continue;
       
      if (type != MED_EN::MED_POLYHEDRA && type != MED_EN::MED_POLYGON)
      {
             mesh_builder->setConnectivity(type_connectivity[type],entity,type);
             delete[] type_connectivity[type];
      }
      else if (type == MED_EN::MED_POLYGON && dimension ==2)
      {
        mesh_builder->setPolygonsConnectivity(&polygon_conn_index[0],
                                              &polygon_conn[0],
                                              type_numbers[type],
                                              entity);
      }
      else if (type == MED_EN::MED_POLYHEDRA && dimension ==3)
      {
        mesh_builder->setPolyhedraConnectivity(&polyhedron_conn_index[0],
                                                &polyhedron_face_index[0],
                                                &polyhedron_conn[0],
                                                type_numbers[type],
                                                entity);
                                                
      }
                }
                MESSAGE_MED("end of createNodalConnectivity");
}


/*! retrieves the faces that are present in a mesh and stores them in a 
 * dynamic structure made of a map of MEDSPLITTER_FaceModel
 * 
 * \param idomain domain id on which the faces are collected
 * \param face_map container storing the faces 
 */
void MESHCollection::getFaces(int idomain, 
                              map<MED_EN::medGeometryElement, vector<MEDSPLITTER_FaceModel*> >& face_map)                     
{
  MED_EN::medEntityMesh constituent_entity = (getMeshDimension()==3)?MED_EN::MED_FACE:MED_EN::MED_EDGE;
  const medGeometryElement* types;
  try
  {
    types = m_mesh[idomain]->getTypes(constituent_entity);
  }
  catch(MEDEXCEPTION&){ return;}
    
  int nbtypes  = m_mesh[idomain]->getNumberOfTypes(constituent_entity);
  const int* global_numbering= m_mesh[idomain]->getGlobalNumberingIndex(constituent_entity);
  int* conn = const_cast<int*> (m_mesh[idomain]->getConnectivity(MED_EN::MED_FULL_INTERLACE,MED_EN::MED_NODAL,constituent_entity, MED_EN::MED_ALL_ELEMENTS));
  for (int itype=0; itype<nbtypes; itype++)
  {
    for (int iface=global_numbering[itype]; iface<global_numbering[itype+1]; iface++)
    {
      MEDSPLITTER_FaceModel* face_model = new MEDSPLITTER_FaceModel();
      MED_EN::medGeometryElement type =  types[itype];
      face_model->setType(type);
      int nbnodes = type%100;
      face_model->setNbNodes(nbnodes);
      face_model->setGlobal(m_topology->convertFaceToGlobal(idomain,iface));
      for (int i=0; i<nbnodes; i++)
      {
        (*face_model)[i]=*conn++;
      }
      face_map[type].push_back(face_model);
    }
  }
}

/*! retrieves the face that is common to two cells located on two different processors
 * 
 * \param ip1 domain id for cell 1
 * \param ilocal1 cell id for cell 1
 * \param ip2 domain id for cell 2
 * \param ilocal2 cell id for cell 2
 * \param face_index global index for the newly created face 
 */
MEDSPLITTER_FaceModel* MESHCollection::getCommonFace(int ip1,int ilocal1,int ip2,int ilocal2,int face_index)
{
  MEDSPLITTER_FaceModel* face_model = new MEDSPLITTER_FaceModel();
  
  MED_EN::medGeometryElement type1 = m_mesh[ip1]->getElementType(MED_EN::MED_CELL,ilocal1);
  MEDMEM::CELLMODEL celltype1 (type1);
  
  const int* conn_index1 =  m_mesh[ip1]->getConnectivityIndex(MED_EN::MED_NODAL,MED_EN::MED_CELL);
  const int* conn1 = m_mesh[ip1]->getConnectivity(MED_EN::MED_FULL_INTERLACE,MED_EN::MED_NODAL,MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
  
 // MED_EN::medGeometryElement type2 = m_mesh[ip2]->getElementType(MED_EN::MED_CELL,ilocal2);
  //MEDMEM::CELLTYPE celltype2 (type2);
  const int* conn_index2 =  m_mesh[ip2]->getConnectivityIndex(MED_EN::MED_NODAL,MED_EN::MED_CELL);
  const int* conn2 = m_mesh[ip2]->getConnectivity(MED_EN::MED_FULL_INTERLACE,MED_EN::MED_NODAL,MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
  
  vector<int> nodes1;
  vector<int> nodes2;
  for (int i=  conn_index1[ilocal1-1]; i<conn_index1[ilocal1]; i++)
    nodes1.push_back(m_topology->convertNodeToGlobal(ip1,*(conn1+i-1)));
  for (int i=  conn_index2[ilocal2-1]; i<conn_index2[ilocal2]; i++)
    nodes2.push_back(m_topology->convertNodeToGlobal(ip2,*(conn2+i-1)));
   
 int nbfaces= celltype1.getNumberOfConstituents(1);
 int ** faces = celltype1.getConstituents(1);
 MED_EN::medGeometryElement* types = celltype1.getConstituentsType(1);
 int iface=0;
 
 while (iface<nbfaces)
 {
  //SCRUTE_MED (iface);
  int nbnodes= types[iface]%100;
  const int* nodes = celltype1.getNodesConstituent(1,iface+1);
  int common_nodes=0;
  int dimension=getMeshDimension();
  for (int i=0; i<nbnodes;i++)
    {
      for (int i2=0; i2<nodes2.size(); i2++)
      {
        if (nodes1[nodes[i]-1]==nodes2[i2]) common_nodes++;
      }     
    }
    if (common_nodes>=dimension) break;
    iface++;
 }
 
 if (iface==nbfaces)
  throw MEDEXCEPTION("MEDSPLITTER::buildCommonFace - No common face found !");
 face_model->setType(types[iface]);
 int nbnodes = types[iface]%100;
 face_model->setNbNodes(nbnodes);
 face_model->setGlobal(face_index); 
 for (int i=0; i<nbnodes; i++)
   (*face_model)[i]=m_topology->convertGlobalNode(nodes1[faces[iface][i]-1],ip1);
      
 return face_model;    
} 
