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
 
#include "MEDMEM_Exception.hxx"

#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingNormalizedUnstructuredMesh.hxx"
#include "MEDCouplingMemArray.hxx"
#include "PointLocator3DIntersectorP0P0.hxx"
#include "BBTree.txx"
#include "MEDPARTITIONER_utils.hxx" 

#include "MEDPARTITIONER_Graph.hxx"

#include "MEDPARTITIONER_Topology.hxx"
#include "MEDPARTITIONER_ParallelTopology.hxx"
#include "MEDPARTITIONER_SequentialTopology.hxx"
#include "MEDPARTITIONER_ParaDomainSelector.hxx"
//#include "MEDPARTITIONER_MeshSendReceive.hxx"
//#include "MEDPARTITIONER_JointExchangeData.hxx"

#include "MEDPARTITIONER_MESHCollection.hxx"
#include "MEDPARTITIONER_MESHCollectionDriver.hxx"
#include "MEDPARTITIONER_MESHCollectionMedXMLDriver.hxx"
#include "MEDPARTITIONER_MESHCollectionMedAsciiDriver.hxx"
#include "MEDPARTITIONER_JointFinder.hxx"
#include "MEDPARTITIONER_UserGraph.hxx"


#ifdef ENABLE_METIS
#include "MEDPARTITIONER_METISGraph.hxx"
#endif
#ifdef ENABLE_SCOTCH
#include "MEDPARTITIONER_SCOTCHGraph.hxx"
#endif

#include <vector>
#include <string>
#include <limits>

#ifndef WNT
# include <ext/hash_map>
#else
# include <hash_map>
#endif
#include <set>

#include <iostream>
#include <fstream>

using namespace MEDPARTITIONER;

#ifndef WNT
using namespace __gnu_cxx;
#else
using namespace std;
#endif

//template inclusion
//#include "MEDPARTITIONER_MESHCollection.txx"

MESHCollection::MESHCollection()
  : _topology(0),
    _owns_topology(false),
    _driver(0),
    _domain_selector( 0 ),
    _i_non_empty_mesh(-1),
    _driver_type(MEDPARTITIONER::MedXML),
    _subdomain_boundary_creates(false),
    _family_splitting(false),
    _create_empty_groups(false)
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

MESHCollection::MESHCollection(MESHCollection& initialCollection, Topology* topology, bool family_splitting, bool create_empty_groups)
  : _name(initialCollection._name),
    _topology(topology),
    _owns_topology(false),
    _driver(0),
    _domain_selector( initialCollection._domain_selector ),
    _i_non_empty_mesh(-1),
    _driver_type(MEDPARTITIONER::MedXML),
    _subdomain_boundary_creates(false),
    _family_splitting(family_splitting),
    _create_empty_groups(create_empty_groups)
{
  
  std::vector<std::vector<std::vector<int> > > new2oldIds(initialCollection.getTopology()->nbDomain());
   castCellMeshes(initialCollection, new2oldIds);


  //casting cell families on new meshes
  _cellFamilyIds.resize(topology->nbDomain());
  castIntField(initialCollection.getMesh(), this->getMesh(),initialCollection.getCellFamilyIds(),_cellFamilyIds, new2oldIds);

  //defining the name for the collection and the underlying meshes
  setName(initialCollection.getName());

  /////////////////:
  // treating faces
  /////////////////

  NodeMapping nodeMapping;
  createNodeMapping(initialCollection, nodeMapping);
  std::vector<std::vector<std::vector<int> > > new2oldFaceIds;
  castMeshes(initialCollection.getFaceMesh(), this->getFaceMesh(),initialCollection, nodeMapping, new2oldFaceIds);



  ////////////////////
  //treating families
  ////////////////////

  _faceFamilyIds.resize(topology->nbDomain());

  //allocating family ids arrays
  for (int inew=0; inew<topology->nbDomain();inew++)
    {
      if(isParallelMode() && !_domain_selector->isMyDomain(inew)) continue;
      _cellFamilyIds[inew]=ParaMEDMEM::DataArrayInt::New();
      int* ptrCellIds=new int[_mesh[inew]->getNumberOfCells()];
      for (int i=0; i< _mesh[inew]->getNumberOfCells();i++) ptrCellIds[i]=0;
      _cellFamilyIds[inew]->useArray(ptrCellIds,true, ParaMEDMEM::CPP_DEALLOC,_mesh[inew]->getNumberOfCells(),1);
      _faceFamilyIds[inew]=ParaMEDMEM::DataArrayInt::New();
      int* ptrFaceIds=new int[_faceMesh[inew]->getNumberOfCells()];
      for (int i=0; i< _faceMesh[inew]->getNumberOfCells();i++) ptrFaceIds[i]=0;
      _faceFamilyIds[inew]->useArray(ptrFaceIds,true, ParaMEDMEM::CPP_DEALLOC,_faceMesh[inew]->getNumberOfCells(),1); 
    }

  castIntField(initialCollection.getFaceMesh(), this->getFaceMesh(),initialCollection.getFaceFamilyIds(),_faceFamilyIds,new2oldFaceIds);


  ///////////////////////
  ////treating groups
  //////////////////////
  _familyInfo=initialCollection.getFamilyInfo();
  _groupInfo=initialCollection.getGroupInfo();

}

/*!
Creates the meshes using the topology underlying he mesh collection and the mesh data coming from the ancient collection
\param initialCollection collection from which the data is extracted to create the new meshes
 */

void MESHCollection::castCellMeshes(MESHCollection& initialCollection, std::vector<std::vector<std::vector<int> > >& new2oldIds)
{
  if (_topology==0) throw INTERP_KERNEL::Exception("Topology has not been defined on call to castCellMeshes");
  _mesh.resize(_topology->nbDomain());
  
  //splitting the initial domains into smaller bits
  
  std::vector<std::vector<ParaMEDMEM::MEDCouplingUMesh*> > splitMeshes;
  splitMeshes.resize(_topology->nbDomain());
  for (int inew=0; inew<_topology->nbDomain();inew++)
    {
      splitMeshes[inew].resize(initialCollection.getTopology()->nbDomain());
      std::fill(&(splitMeshes[inew][0]),&(splitMeshes[inew][0])+splitMeshes[inew].size(),(ParaMEDMEM::MEDCouplingUMesh*)0);
    }
  
  for (int iold=0; iold<initialCollection.getTopology()->nbDomain();iold++)
    {
      if (!isParallelMode() || initialCollection._domain_selector->isMyDomain(iold))
        {
          int size=(initialCollection._mesh)[iold]->getNumberOfCells();
          std::vector<int> globalids(size);
          initialCollection.getTopology()->getCellList(iold, &globalids[0]);
          std::vector<int> ilocalnew(size);
          std::vector<int> ipnew(size);
          _topology->convertGlobalCellList(&globalids[0],size,&ilocalnew[0],&ipnew[0]);
          new2oldIds[iold].resize(_topology->nbDomain());
          for (int i=0; i<ilocalnew.size();i++)
            {
              new2oldIds[iold][ipnew[i]].push_back(i);
            }
          for (int inew=0;inew<_topology->nbDomain();inew++)
            {
              splitMeshes[inew][iold]=(ParaMEDMEM::MEDCouplingUMesh*)(initialCollection.getMesh())[iold]->buildPartOfMySelf(&new2oldIds[iold][inew][0],&new2oldIds[iold][inew][0]+new2oldIds[iold][inew].size(),true);
            }
        }
    }

  if (isParallelMode())
    {
      for (int iold=0; iold<initialCollection.getTopology()->nbDomain();iold++)
        for(int inew=0;inew<_topology->nbDomain();inew++)
          {
            if (initialCollection._domain_selector->isMyDomain(iold) && _domain_selector->isMyDomain(inew)) continue;
            if(initialCollection._domain_selector->isMyDomain(iold))
              {
              _domain_selector->sendMesh(*(splitMeshes[inew][iold]),_domain_selector->getProcessorID(inew));
              std::cout<<"send iold"<<iold<<" inew "<<inew<<" "<<splitMeshes[inew][iold]->getNumberOfCells()<<std::endl;
              
              }
            if (_domain_selector->isMyDomain(inew))
              {
                _domain_selector->recvMesh(splitMeshes[inew][iold],_domain_selector->getProcessorID(iold));
                std::cout<<"recv iold"<<iold<<" inew "<<inew<<std::endl;
              }
          }
    }
  
  //fusing the split meshes
  for (int inew=0; inew<_topology->nbDomain()  ;inew++)
    {
      std::vector<const ParaMEDMEM::MEDCouplingUMesh*> meshes;
     
      for (int i=0; i< splitMeshes[inew].size();i++)
        if (splitMeshes[inew][i]!=0) meshes.push_back(splitMeshes[inew][i]);
       std::cout<< "nb of meshes"<<meshes.size()<<std::endl;

      if (!isParallelMode()||_domain_selector->isMyDomain(inew))
        {
          _mesh[inew]=ParaMEDMEM::MEDCouplingUMesh::MergeUMeshes(meshes);
          _mesh[inew]->zipCoords();
        }
      for (int i=0; i< splitMeshes[inew].size();i++)
        if (splitMeshes[inew][i]!=0) splitMeshes[inew][i]->decrRef();
    }  
  
}

/*!
  \param initialCollection source mesh collection 
  \param nodeMapping structure containing the correspondency between nodes in the initial collection and the node(s) in the new collection
*/
void MESHCollection::createNodeMapping( MESHCollection& initialCollection, NodeMapping& nodeMapping)
{

  //  NodeMapping reverseNodeMapping;
  for (int iold=0; iold<initialCollection.getTopology()->nbDomain();iold++)
    {

      double* bbox;
      BBTree<3>* tree; 
      if (!isParallelMode() || (_domain_selector->isMyDomain(iold)))
        {
          //      std::map<pair<double,pair<double, double> >, int > nodeClassifier;
          int nvertices=getMesh(iold)->getNumberOfNodes();
          bbox=new double[nvertices*6];
          ParaMEDMEM::DataArrayDouble* coords = initialCollection.getMesh(iold)->getCoords();
          double* coordsPtr=coords->getPointer();
          for (int i=0; i<nvertices*3;i++)
            {
              bbox[i*2]=coordsPtr[i]-1e-9;
              bbox[i*2+1]=coordsPtr[i]+1e-9;
            }
          tree=new BBTree<3>(bbox,0,0,nvertices,1e-12);
        }
              
      for (int inew=0; inew<_topology->nbDomain(); inew++)
        {
          //sending meshes for parallel computation
          if (isParallelMode() && _domain_selector->isMyDomain(inew) && !_domain_selector->isMyDomain(iold))
            {
              std::cout<<"sendTo"<<_domain_selector->getProcessorID(iold)<<std::endl;
              _domain_selector->sendMesh(*(getMesh(inew)), _domain_selector->getProcessorID(iold));
              
            }
          else if (isParallelMode() && !_domain_selector->isMyDomain(inew)&& _domain_selector->isMyDomain(iold))
            {
              ParaMEDMEM::MEDCouplingUMesh* mesh;
              std::cout<<"recvFrom" << _domain_selector->getProcessorID(inew) <<std::endl;
              _domain_selector->recvMesh(mesh, _domain_selector->getProcessorID(inew));
              ParaMEDMEM::DataArrayDouble* coords = mesh->getCoords();
              for (int inode=0; inode<mesh->getNumberOfNodes();inode++)
                {
                  double* coordsPtr=coords->getPointer()+inode*3;
                  std::vector<int> elems;
                  tree->getElementsAroundPoint(coordsPtr,elems);
                  if (elems.size()==0) continue;         
                  nodeMapping.insert(make_pair(make_pair(iold,elems[0]),make_pair(inew,inode)));
                }
            }
          else if (!isParallelMode() || (_domain_selector->isMyDomain(inew) && _domain_selector->isMyDomain(iold)))
            {
              ParaMEDMEM::DataArrayDouble* coords = getMesh(inew)->getCoords();           
            
              for (int inode=0; inode<_mesh[inew]->getNumberOfNodes();inode++)
                {
                  
                  double* coordsPtr=coords->getPointer()+inode*3;
                  std::vector<int> elems;
                  tree->getElementsAroundPoint(coordsPtr,elems);
                  if (elems.size()==0) {continue;}              
                  nodeMapping.insert(make_pair(make_pair(iold,elems[0]),make_pair(inew,inode)));
                  cout << "inode :" <<inode<<" ("<<iold<<","<<elems[0]<<")-->("<<inew<<","<<inode<<")"<<endl;
                }
            }
        }
    } 
  std::cout<<"NodeMapping size"<<nodeMapping.size()<<std::endl;

}

/*!
  creates the face meshes on the new domains from the faces on the old domain and the node mapping
  faces at the interface are duplicated
*/

void MESHCollection::castMeshes(std::vector<ParaMEDMEM::MEDCouplingUMesh*>& meshesCastFrom,std::vector<ParaMEDMEM::MEDCouplingUMesh*>& meshesCastTo, MESHCollection& initialCollection,const NodeMapping& nodeMapping, std::vector<std::vector<std::vector<int> > >& new2oldIds)
{

  //splitMeshes structure will contain the partition of 
  //the old faces on the new ones
  // splitMeshes[4][2] contains the faces from old domain 2
  // that have to be added to domain 4

  vector< vector<ParaMEDMEM::MEDCouplingUMesh*> > splitMeshes;
  int newSize=_topology->nbDomain();
  splitMeshes.resize ( newSize );
  for (int inew=0;inew<newSize;inew++)
    {
      splitMeshes[inew].resize(meshesCastFrom.size());
    }


  new2oldIds.resize(meshesCastFrom.size());

  //loop over the old domains to analyse the faces and decide 
  //on which new domain they belong

  for (int iold=0; iold<meshesCastFrom.size();iold++)
    {
      if (isParallelMode() && !_domain_selector->isMyDomain(iold)) continue;
      new2oldIds[iold].resize(newSize);
      for (int ielem=0;ielem<meshesCastFrom[iold]->getNumberOfCells();ielem++)
        {
          std::vector<int> nodes;
          meshesCastFrom[iold]->getNodeIdsOfCell(ielem,nodes);
          
          map <int,int> faces;
          //      cout<<"----------------"<<endl;
          //analysis of one face
          for (int inode=0;inode<nodes.size();inode++)
            {
              typedef multimap<pair<int,int>,pair<int,int> >::const_iterator MI;
              int mynode=nodes[inode];
              if (mynode <0 || mynode > 1000000000) exit(1);
              pair <MI,MI> myRange = nodeMapping.equal_range(make_pair(iold,mynode));
              //                cout << iold <<" " <<nodes[inode]<<endl;
              for (MI iter=myRange.first; iter!=myRange.second; iter++)
                {
                  if (faces.find(iter->second.first)==faces.end())
                    faces[iter->second.first]=1;
                  else
                    faces[iter->second.first]++;
                  //                    cout<<"idomain" << iter->second.first<<" facemapping "<<faces[iter->second.first]<<endl;
                }
            }
          
          for (map<int,int>::iterator iter=faces.begin();
               iter!=faces.end();
               iter++)
            {
              if (iter->second==nodes.size())
                new2oldIds[iold][iter->first].push_back(ielem);
            }
        }        
      
      //creating the splitMeshes from the face ids
      for (int inew=0;inew<_topology->nbDomain();inew++)
        {
          cout<<"nb faces - iold "<<iold<<" inew "<<inew<<" : "<<new2oldIds[iold][inew].size()<<endl;
          splitMeshes[inew][iold]=(ParaMEDMEM::MEDCouplingUMesh*)(meshesCastFrom[iold]->buildPartOfMySelf(&new2oldIds[iold][inew][0],&new2oldIds[iold][inew][0]+new2oldIds[iold][inew].size(),true));
        }
    }
      
  // send/receive stuff
  if (isParallelMode())
  for (int iold=0; iold<meshesCastFrom.size();iold++)
    for (int inew=0; inew<newSize; inew++)
      {
        if (_domain_selector->isMyDomain(iold) && !_domain_selector->isMyDomain(inew))
          _domain_selector->sendMesh(*(splitMeshes[inew][iold]), _domain_selector->getProcessorID(inew));
        if (!_domain_selector->isMyDomain(iold) && _domain_selector->isMyDomain(inew))
           _domain_selector->recvMesh(splitMeshes[inew][iold], _domain_selector->getProcessorID(iold));
      }


  //recollecting the bits of splitMeshes to fuse them into one
  meshesCastTo.resize(newSize);
  for (int inew=0; inew < newSize;inew++)
    {
      vector<const ParaMEDMEM::MEDCouplingUMesh*> myMeshes;
      for (int iold=0; iold < meshesCastFrom.size();iold++)
        {
          if (splitMeshes[inew][iold] !=0)
            myMeshes.push_back(splitMeshes[inew][iold]);          
        }
      meshesCastTo[inew]=ParaMEDMEM::MEDCouplingUMesh::MergeUMeshes(myMeshes);
      meshesCastTo[inew]->zipCoords();
      for (int iold=0; iold < meshesCastFrom.size();iold++)
        if (splitMeshes[inew][iold]!=0) splitMeshes[inew][iold]->decrRef();
    }
}

void MESHCollection::castIntField(std::vector<ParaMEDMEM::MEDCouplingUMesh*>& meshesCastFrom,std::vector<ParaMEDMEM::MEDCouplingUMesh*>& meshesCastTo,  std::vector<ParaMEDMEM::DataArrayInt*>& arrayFrom,  std::vector<ParaMEDMEM::DataArrayInt*>& arrayTo, std::vector< std::vector< std::vector<int> > >& new2oldMapping)
{
  for (int inew=0; inew < meshesCastTo.size();inew++)
    {
      vector<const ParaMEDMEM::DataArrayInt*> splitIds;
      for (int iold=0; iold < meshesCastFrom.size();iold++)
        {
          if (isParallelMode() && !_domain_selector->isMyDomain(iold)) continue;
          int* ptr=&(new2oldMapping[iold][inew][0]);
          splitIds.push_back(arrayFrom[iold]->selectByTupleId(ptr,ptr+new2oldMapping[iold][inew].size()));
        }
      arrayTo[inew]=ParaMEDMEM::DataArrayInt::Aggregate(splitIds);
    }
}


/*! constructing the MESH collection from a distributed file
 *
 * \param filename name of the master file containing the list of all the MED files
 */
MESHCollection::MESHCollection(const string& filename)
  : _topology(0),
    _owns_topology(true),
    _driver(0),
    _domain_selector( 0 ),
    _i_non_empty_mesh(-1),
    _driver_type(MEDPARTITIONER::Undefined),
    _subdomain_boundary_creates(false),
    _family_splitting(false),
    _create_empty_groups(false)
{
  char filenamechar[256];
  strcpy(filenamechar,filename.c_str());
  try
    {
      _driver=new MESHCollectionMedXMLDriver(this);
      _driver->read (filenamechar);
      _driver_type = MedXML;

    }
  catch(MEDMEM::MEDEXCEPTION){
    delete _driver;
    try
      {
        _driver=new MESHCollectionMedAsciiDriver(this);
        _driver->read (filenamechar);
        _driver_type=MedAscii;
      }
    catch(MEDMEM::MEDEXCEPTION&)
      {
        delete _driver;
        throw MEDMEM::MEDEXCEPTION("file does not comply with any recognized format");
      }
  }
  for ( int idomain = 0; idomain < _mesh.size(); ++idomain )
    if ( _mesh[idomain] && _mesh[idomain]->getNumberOfNodes() > 0 )
      _i_non_empty_mesh = idomain;
}

/*! Constructing the MESH collection from selected domains of a distributed file
 * 
 * \param filename  - name of the master file containing the list of all the MED files
 * \param domainSelector - selector of domains to load
 */
MESHCollection::MESHCollection(const string& filename, ParaDomainSelector& domainSelector)
  : _topology(0),
    _owns_topology(true),
    _driver(0),
    _domain_selector( domainSelector.nbProcs() > 1 ? & domainSelector : 0 ),
    _i_non_empty_mesh(-1),
    _driver_type(MEDPARTITIONER::Undefined),
    _subdomain_boundary_creates(false),
    _family_splitting(false),
    _create_empty_groups(false)
{
  try
    {
      _driver=new MESHCollectionMedXMLDriver(this);
      _driver->read ( (char*)filename.c_str(), _domain_selector );
      _driver_type = MedXML;
    }
  catch(MEDMEM::MEDEXCEPTION&)
    {
      delete _driver;
      try
        {
          _driver=new MESHCollectionMedAsciiDriver(this);
          _driver->read ( (char*)filename.c_str(), _domain_selector );
          _driver_type=MedAscii;
        }
      catch(MEDMEM::MEDEXCEPTION&)
        {
          delete _driver;
          throw MEDMEM::MEDEXCEPTION("file does not comply with any recognized format");
        }
    }
  if ( isParallelMode() )
    // to know nb of cells on each proc to compute global cell ids from locally global
    _domain_selector->gatherNbOf( getMesh() );

  // find non-empty domain mesh
  for ( int idomain = 0; idomain < _mesh.size(); ++idomain )
    if ( _mesh[idomain] && _mesh[idomain]->getNumberOfNodes() > 0 )
      _i_non_empty_mesh = idomain;
}

/*! constructing the MESH collection from a sequential MED-file
 * 
 * \param filename MED file
 * \param meshname name of the mesh that is to be read
 */
MESHCollection::MESHCollection(const string& filename, const string& meshname)
  : _name(meshname),
    _topology(0),
    _owns_topology(true),
    _driver(0),
    _domain_selector( 0 ),
    _i_non_empty_mesh(-1),
    _driver_type(MEDPARTITIONER::MedXML),
    _subdomain_boundary_creates(false),
    _family_splitting(false),
    _create_empty_groups(false)
{
  char filenamechar[256];
  char meshnamechar[256];
  strcpy(filenamechar,filename.c_str());
  strcpy(meshnamechar,meshname.c_str());
  try // avoid memory leak in case of inexistent filename
    {
      retrieveDriver()->readSeq (filenamechar,meshnamechar);
    }
  catch ( MED_EXCEPTION& e )
    {
      if ( _driver ) delete _driver; _driver=0;
      throw e;
    }
  if ( _mesh[0] && _mesh[0]->getNumberOfNodes() > 0 )
    _i_non_empty_mesh = 0;
}

MESHCollection::~MESHCollection()
{
  for (int i=0; i<_mesh.size();i++)
    if (_mesh[i]!=0) {/*delete*/ _mesh[i]->decrRef(); }
  for (int i=0; i<_faceMesh.size();i++)
    if (_mesh[i]!=0) {/*delete*/ _faceMesh[i]->decrRef(); }
  for (int i=0; i<_connect_zones.size();i++)
    if (_connect_zones[i]!=0) {delete _connect_zones[i];}
  if (_driver !=0) {delete _driver; _driver=0;}
  if (_topology!=0 && _owns_topology) {delete _topology; _topology=0;}
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
  //   if (_topology->nbDomain()>1)
  //     buildConnectZones();
  cout <<"End of connect zones building"<<endl;
  //suppresses link with driver so that it can be changed for writing
  if (_driver!=0)delete _driver;
  _driver=0;

  char filenamechar[256];
  strcpy(filenamechar,filename.c_str());
  retrieveDriver()->write (filenamechar, _domain_selector);
}

/*! creates or gets the link to the collection driver
 */
MESHCollectionDriver* MESHCollection::retrieveDriver()
{
  if (_driver==0)
    {
      switch(_driver_type)
        {
        case MedXML:
          _driver=new MESHCollectionMedXMLDriver(this);
          break;
        case MedAscii:
          _driver=new MESHCollectionMedAsciiDriver(this);
          break;
        default:
          throw MEDMEM::MEDEXCEPTION("Unrecognized driver");
        }
    }

  return _driver;
}


/*! gets an existing driver
 * 
 */
MESHCollectionDriver* MESHCollection::getDriver() const
{
  return _driver;
}
// /*! retrieves the mesh dimension*/
int MESHCollection::getMeshDimension() const
{
  return _i_non_empty_mesh < 0 ? -1 : _mesh[_i_non_empty_mesh]->getMeshDimension();
}

vector<ParaMEDMEM::MEDCouplingUMesh*>& MESHCollection::getMesh() 
{
  return _mesh;
}
vector<ParaMEDMEM::MEDCouplingUMesh*>& MESHCollection::getFaceMesh() 
{
  return _faceMesh;
}
ParaMEDMEM::MEDCouplingUMesh* MESHCollection::getMesh(int idomain) const
{
  return _mesh[idomain];
}

ParaMEDMEM::MEDCouplingUMesh* MESHCollection::getFaceMesh(int idomain)
{
  return _faceMesh[idomain];
}
vector<MEDPARTITIONER::CONNECTZONE*>& MESHCollection::getCZ()
{
  return _connect_zones;
}

Topology* MESHCollection::getTopology() const 
{
  return _topology;
}

void MESHCollection::setTopology(Topology* topo)
{
  if (_topology!=0)
    {
      throw MEDMEM::MEDEXCEPTION("Erreur : topology is already set");
    }
  else
    _topology = topo;
}


/*! Method creating the cell graph
 * 
 * \param array returns the pointer to the structure that contains the graph 
 * \param edgeweight returns the pointer to the table that contains the edgeweights
 *        (only used if indivisible regions are required)
 */

void MESHCollection::buildCellGraph(MEDPARTITIONER::MEDSKYLINEARRAY* & array,int *& edgeweights )
{

  multimap< int, int > node2cell;
  multimap< int, int > cell2cell;

  vector<vector<multimap<int,int> > > commonDistantNodes;
  int nbdomain=_topology->nbDomain();
  if (isParallelMode())
    {
      _joint_finder=new JointFinder(*this);
      _joint_finder->findCommonDistantNodes();
      commonDistantNodes=_joint_finder->getDistantNodeCell();
    }

  //looking for reverse nodal connectivity i global numbering
  for (int idomain=0; idomain<nbdomain;idomain++)
    {
      if (isParallelMode() && !_domain_selector->isMyDomain(idomain)) continue;
      int offset=0, procOffset=0;
      if (isParallelMode())
        {
          offset=_domain_selector->getDomainShift(idomain);
          procOffset=_domain_selector->getProcShift()+1;
        }
      ParaMEDMEM::DataArrayInt* index=ParaMEDMEM::DataArrayInt::New();
      ParaMEDMEM::DataArrayInt* revConn=ParaMEDMEM::DataArrayInt::New();
      _mesh[idomain]->getReverseNodalConnectivity(revConn,index);
      int* index_ptr=index->getPointer();
      ParaMEDMEM::DataArrayInt* globalRevConn=revConn->deepCpy();
      _topology->convertCellToGlobal(idomain,
                                     revConn->getPointer(),
                                     index_ptr[_mesh[idomain]->getNumberOfNodes()],
                                     globalRevConn->getPointer());

      int min=std::numeric_limits<int>::max(),max=-1;
      for (int i=0; i< index_ptr[_mesh[idomain]->getNumberOfNodes()];i++)
        {
          if (revConn->getPointer()[i]<min) min=revConn->getPointer()[i];
          if (revConn->getPointer()[i]>max) max=revConn->getPointer()[i];
        }
      cout <<"min:"<<min<<" max:"<<max<<endl;
      int* globalNodeIds=new int[_mesh[idomain]->getNumberOfNodes()];
      _topology->getNodeList(idomain,globalNodeIds);
      
      int* globalRevConnPtr=globalRevConn->getPointer();
      for (int i=0; i<_mesh[idomain]->getNumberOfNodes();i++)
        {
          for (int icell=index_ptr[i]; icell<index_ptr[i+1];icell++)
            node2cell.insert(make_pair(globalNodeIds[i],globalRevConnPtr[icell]+procOffset));
        }
      if (isParallelMode())
        {
          for (int mydomain=0; mydomain<_topology->nbDomain();mydomain++)
            {
              if (_domain_selector->isMyDomain(mydomain)) continue;
              //            for (int idomain=0; idomain<_topology->nbDomain();idomain++)
              // {
              //          if (_domain_selector->isMyDomain(idomain)) continue;
                  multimap<int,int>::iterator iter;
                  for (iter=commonDistantNodes[idomain][mydomain].begin();iter!=commonDistantNodes[idomain][mydomain].end();iter++)
                    {
                      int ilocnode=iter->first;
                      int icell=iter->second;
                      
                      node2cell.insert(make_pair(globalNodeIds[ilocnode],icell+offset));     
                      //  cout<<"pair "<<globalNodeIds[ilocnode]<<" "<<icell+offset<<" "<<offset<<endl;
                    }
                  // }
            }
        }

      globalRevConn->decrRef();
      revConn->decrRef();
      index->decrRef();
      delete[] globalNodeIds;
    }

  //creating graph arcs (cell to cell relations)
  //arcs are stored in terms of (index,value) notation
  // 0 3 5 6 6
  // 1 2 3 2 3 3 
  // means 6 arcs (0,1), (0,2), (0,3), (1,2), (1,3), (2,3)
  // in present version arcs are not doubled but reflexive (1,1) arcs are present for each cell
 
  int mincell,maxcell;
  if (isParallelMode())
    {
      mincell=_domain_selector->getProcShift()+1;
      maxcell=mincell;
      for (int i=0; i<nbdomain;i++)
        if (_domain_selector->isMyDomain(i)) maxcell+=_mesh[i]->getNumberOfCells();
    
    }
  else
    {
      mincell=0;
      maxcell=_topology->nbCells();
    }
         cout<<"mincell"<<mincell<<" maxcell "<<maxcell<<endl;
  for (int inode=0; inode<_topology->nbNodes();inode++)
    {
      typedef multimap<int,int>::const_iterator MI;
      pair <MI,MI> myRange = node2cell.equal_range(inode);
      for (MI cell1=myRange.first; cell1!=myRange.second; cell1++)
        {
          for (MI cell2 = myRange.first; cell2!=myRange.second; cell2++)
            {
              if (cell1->second!=cell2->second&&cell1->second>=mincell&&cell1->second<maxcell) cell2cell.insert(make_pair(cell1->second,cell2->second));
            }
        }
    }

  //filling up index and value to create skylinearray structure

  multimap<int,int>::const_iterator iter;

  iter=cell2cell.begin();
  vector <int> index,value;
  index.push_back(0);
  while (iter != cell2cell.end())
    {
      multimap<int,int>::const_iterator next_iter = cell2cell.upper_bound(iter->first);
      int size=0;
      for (multimap<int,int>::const_iterator current_iter=iter; current_iter!=next_iter; current_iter++)
        {
          value.push_back(current_iter->second);
          size++;
        }
      index.push_back(index[index.size()-1]+size);

      
      iter=next_iter;
    }

  array=new MEDPARTITIONER::MEDSKYLINEARRAY(index,value);

  cout<< "end of graph creation"<<endl;
}

// /*! Method contributing to the distant cell graph
//  */
// void MESHCollection::findCommonDistantNodes(vector<vector<multimap<int,int> > >& commonDistantNodes)
// {
//   int nbdomain=_topology->nbDomain();
//   commonDistantNodes.resize(nbdomain);
//   for (int i=0; i<nbdomain;i++) commonDistantNodes[i].resize(nbdomain);
//   int nbproc=_domain_selector->nbProcs();
//   vector<BBTree<3>* > bbtree(nbdomain); 
//   vector<ParaMEDMEM::DataArrayInt*> rev(nbdomain);
//   vector<ParaMEDMEM::DataArrayInt*>revIndx(nbdomain);
//   int meshDim;
//   int spaceDim;

//   for (int mydomain=0;mydomain<nbdomain;mydomain++)
//     {
//       if(! _domain_selector->isMyDomain(mydomain)) continue;
//       meshDim=_mesh[mydomain]->getMeshDimension();
//       spaceDim= _mesh[mydomain]->getSpaceDimension();
//       rev[mydomain] = ParaMEDMEM::DataArrayInt::New();
//       revIndx[mydomain] = ParaMEDMEM::DataArrayInt::New();
//       _mesh[mydomain]->getReverseNodalConnectivity(rev[mydomain],revIndx[mydomain]);
//         double* bbx=new double[2*spaceDim*_mesh[mydomain]->getNumberOfNodes()];
//       for (int i=0; i<_mesh[mydomain]->getNumberOfNodes()*spaceDim;i++)
//         {
//           const double* coords=_mesh[mydomain]->getCoords()->getConstPointer();
//           bbx[2*i]=(coords[i])-1e-12;
//           bbx[2*i+1]=bbx[2*i]+2e-12;
//         }
//       bbtree[mydomain]=new BBTree<3> (bbx,0,0,_mesh[mydomain]->getNumberOfNodes(),-1e-12);
//     }
//   for (int isource=0;isource<nbdomain;isource++)
//     for (int itarget=0;itarget<nbdomain;itarget++)
//       {

//         if (_domain_selector->isMyDomain(isource)&&_domain_selector->isMyDomain(itarget)) continue;
//         if (_domain_selector->isMyDomain(isource))
//           {
//             //preparing data for treatment on target proc
//             int targetProc = _domain_selector->getProccessorID(itarget);
            
//             std::vector<double> vec(spaceDim*_mesh[isource]->getNumberOfNodes());
//             std::copy(_mesh[isource]->getCoords()->getConstPointer(),_mesh[isource]->getCoords()->getConstPointer()+_mesh[isource]->getNumberOfNodes()*spaceDim,&vec[0]);
//             _domain_selector->sendDoubleVec (vec,targetProc);
            
//             //retrieving target data for storage in commonDistantNodes array
//             vector<int> localCorrespondency;
//             _domain_selector->recvIntVec(localCorrespondency, targetProc);
//             cout<<"size "<<localCorrespondency.size()<<endl;
//              for (int i=0; i<localCorrespondency.size()/2;i++)
//               commonDistantNodes[isource][itarget].insert(make_pair(localCorrespondency[2*i],localCorrespondency[2*i+1]));      
            
//           }
//         if (_domain_selector->isMyDomain(itarget))    
//           {
//             //receiving data from source proc
//             int sourceProc = isource%nbproc;
//             std::vector<double> recvVec;
//             _domain_selector->recvDoubleVec(recvVec,sourceProc);
//             std::map<int,int> commonNodes; // (local nodes, distant nodes) list
//             for (int inode=0; inode<(recvVec.size()/meshDim);inode++)
//               {
//                 double* bbox=new double[2*spaceDim];
//                 for (int i=0; i<spaceDim;i++)
//                   {
//                     bbox[2*i]=recvVec[inode*spaceDim+i]-1e-12;
//                     bbox[2*i+1]=bbox[2*i]+2e-12;
//                   }
//                 vector<int> inodes;
//                 bbtree[itarget]->getIntersectingElems(bbox,inodes);
//                 delete[] bbox;
           
//                 if (inodes.size()>0) commonNodes.insert(make_pair(inodes[0],inode));
//               }
//             std::vector<int> nodeCellCorrespondency;
//             for (map<int,int>::iterator iter=commonNodes.begin();iter!=commonNodes.end();iter++)
//               {
//                 const int*revIndxPtr=revIndx[itarget]->getConstPointer();
//                 const int*revPtr=rev[itarget]->getConstPointer();
//                 for (int icell=revIndxPtr[iter->first];icell<revIndxPtr[iter->first+1];icell++)
//                   {
//                     nodeCellCorrespondency.push_back(iter->second);
//                     nodeCellCorrespondency.push_back(revPtr[icell]);
//                   }
//           }
//         _domain_selector->sendIntVec(nodeCellCorrespondency, sourceProc);
//       }
  
// }
//     }
      

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
  if (nbdomain <1) throw MEDMEM::MEDEXCEPTION("Number of subdomains must be >0");
  MEDPARTITIONER::MEDSKYLINEARRAY* array=0;
  int* edgeweights=0;

  cout<<"Building cell graph";
  //   if ( _domain_selector )
  //     buildCellGraphParallel(array,edgeweights);
  //   else
  buildCellGraph(array,edgeweights);
  Graph* cellGraph;
  switch (split)
    {
    case Graph::METIS:
#ifdef ENABLE_METIS
      cellGraph=(Graph*)(new METISGraph(array,edgeweights));
#else
      throw MEDMEM::MEDEXCEPTION("METIS Graph is not available. Check your products, please.");
#endif
      break;
    case Graph::SCOTCH:
#ifdef ENABLE_SCOTCH
      cellGraph=(Graph*)(new SCOTCHGraph(array,edgeweights));
#else
      throw MEDMEM::MEDEXCEPTION("SCOTCH Graph is not available. Check your products, please.");
#endif
      break;
    }

  //!user-defined weights
  if (user_edge_weights!=0) 
    cellGraph->setEdgesWeights(user_edge_weights);
  if (user_vertices_weights!=0)
    cellGraph->setVerticesWeights(user_vertices_weights);

  cout<<"Partitioning graph";
  cellGraph->partGraph(nbdomain,options_string,_domain_selector);

  // DEBUG
  //   MEDMEM::STRING out("RESULT GRAPH #");
  //   out << (_domain_selector?_domain_selector->rank():0) << ": ";
  //   const int* part = cellGraph->getPart();
  //   int n = cellGraph->nbVertices();
  //   for ( int e=0; e < n; ++e )
  //     out << part[e] <<" ";
  //   cout << out << endl;
  

  cout<<"Building new topology";
  //cellGraph is a shared pointer 
  Topology* topology = new ParallelTopology (cellGraph, nbdomain, getMeshDimension());

  //cleaning
  if (edgeweights!=0) delete[] edgeweights;
  // if (array!=0) delete array;
  cout<<"End of partition creation";
  delete cellGraph;

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
  MEDPARTITIONER::MEDSKYLINEARRAY* array=0;
  int* edgeweights=0;

  buildCellGraph(array,edgeweights);
  Graph* cellGraph;
  set<int> domains;
  for (int i=0; i<_topology->nbCells(); i++)
    {
      domains.insert(partition[i]);
    }
  int nbdomain=domains.size();
  
  cellGraph=(Graph*)(new UserGraph(array, partition, _topology->nbCells()));
  
  //cellGraph is a shared pointer 
  Topology* topology = new ParallelTopology (cellGraph, nbdomain, getMeshDimension());
  
  //  if (array!=0) delete array;
  delete cellGraph;
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

// void MESHCollection::buildConnectZones(int idomain)
// {
//   // constructing node/node correspondencies
//   vector<MEDMEM::MEDSKYLINEARRAY*> node_node_correspondency;
//   node_node_correspondency.resize(_topology->nbDomain());

//   cout << "Computing node/node corresp"<<endl;

//   _topology->computeNodeNodeCorrespondencies(idomain, node_node_correspondency );

//   for (int idistant=0; idistant< _topology->nbDomain(); idistant++)
//   {
//     // on regarde si une correspondance noeud/noeud a été trouvée 
//     // entre idomain et idistant
//     // si oui, on crée une connectzone
//     if (node_node_correspondency[idistant]!=0)
//     {
//       MEDMEM::CONNECTZONE* cz= new MEDMEM::CONNECTZONE();
//       cz->setLocalMesh(_mesh[idomain]);
//       cz->setDistantMesh(_mesh[idistant]);
//       cz->setLocalDomainNumber(idomain);
//       cz->setDistantDomainNumber(idistant);
//       cz-> setName ("Connect zone defined by PARTITIONER");
//       cz->setNodeCorresp(node_node_correspondency[idistant]);
//       _connect_zones.push_back(cz);  
//     }
//   }
//   cout << "Computing node/node corresp"<<endl;

//   vector<MEDMEM::MEDSKYLINEARRAY*> cell_cell_correspondency;
//   cell_cell_correspondency.resize(_topology->nbDomain());
//   _topology->computeCellCellCorrespondencies(idomain, cell_cell_correspondency, _cell_graph.get());

//   for (int idistant=0; idistant< _topology->nbDomain(); idistant++)
//   {
//     //the connect zone has been created by the node/node computation
//     if (cell_cell_correspondency[idistant]!=0)
//     {
//       MEDMEM::CONNECTZONE* cz=0;
//       for (int icz=0; icz<_connect_zones.size();icz++)
//         if (_connect_zones[icz]->getLocalDomainNumber()==idomain &&
//             _connect_zones[icz]->getDistantDomainNumber()==idistant)
//           cz = _connect_zones[icz];
//       if (cz!=0) 
//         cz->setEntityCorresp(MED_EN::MED_CELL,MED_EN::MED_CELL, cell_cell_correspondency[idistant]);
//       else 
//         throw MEDEXCEPTION("MESHCollection::buildConnectZones() -A connect zone should exist");   
//       //delete cell_cell_correspondency[idistant];
//     }

//   }
// }

// //================================================================================
// /*!
//  * \brief Adds a group of joint faces
//  *  \param loc_face_ids - local numbers of faces
//  *  \param idomian - domain index where faces are local
//  *  \param idistant - the other domain index
//  */
// //================================================================================

// void MESHCollection::addJointGroup(const std::vector<int>& loc_face_ids, int idomain, int idistant)
// {
//   MEDMEM::MESHING* meshing = dynamic_cast<MEDMEM::MESHING*> (_mesh[idomain]);
//   MED_EN::medEntityMesh constituent_entity = getSubEntity();

//   MEDMEM::STRING jointname("joint_");
//   jointname<<idistant+1;

//   MEDMEM::GROUP * tmp_grp = new GROUP, * joint_group = tmp_grp;
//   // try to find already present group with such a name
//   //  vector<MEDMEM::GROUP*> groups = meshing->getGroups( constituent_entity );
//   //  for ( int g = 0; g < groups.size(); ++g )
//   //    if ( groups[g]->getName() == jointname.str() )
//   //    {
//   //      joint_group = groups[g];
//   //      break;
//   //    }
//   // assure uniqueness of group name
//   bool unique = false;
//   vector<MEDMEM::GROUP*> groups = meshing->getGroups( constituent_entity );
//   do
//   {
//     unique = true;
//     for ( int g = 0; unique && g < groups.size(); ++g )
//       unique = ( groups[g]->getName() != jointname );
//     if ( !unique )
//       jointname << "_" << idomain+1;
//   }
//   while ( !unique );
//   joint_group->setName(jointname);
//   joint_group->setMesh(meshing);
//   joint_group->setEntity(constituent_entity);
//   map<MED_EN::medGeometryElement, vector<int> > joint_types;

//   int nbfaces = loc_face_ids.size();
//   for (int i=0; i<nbfaces; i++)
//   {    
//     MED_EN::medGeometryElement type = meshing->getElementType(constituent_entity,loc_face_ids[i]);
//     joint_types[type].push_back(loc_face_ids[i]);
//   }
//   joint_group->setNumberOfGeometricType(joint_types.size());
//   MED_EN::medGeometryElement* types=new MED_EN::medGeometryElement[joint_types.size()];
//   int* nb_in_types=new int[joint_types.size()];
//   int* group_index=new int[joint_types.size()+1];

//   group_index[0]=1;
//   int itype=0;
//   int iface =0;
//   int* group_value=new int[nbfaces];
//   for (map<MED_EN::medGeometryElement, vector<int> >::const_iterator iterj=joint_types.begin();
//        iterj != joint_types.end();
//        iterj++)
//   {
//     nb_in_types[itype]=(iterj->second).size();
//     types[itype]=iterj->first;
//     itype++;
//     group_index[itype]=group_index[itype-1]+(iterj->second).size();
//     for (int i=0; i<  (iterj->second).size(); i++)
//       group_value[iface++]=(iterj->second)[i];
//   }
//   joint_group->setGeometricType(types);
//   joint_group->setNumberOfElements(nb_in_types);
//   joint_group->setNumber(group_index, group_value, /*shallowCopy=*/true);
//   delete[] types;
//   delete[] nb_in_types;

//   if ( joint_group == tmp_grp )
//     meshing->addGroup(*tmp_grp);
//   tmp_grp->removeReference();
// }

// /*! building Connect Zones for storing the informations
//  * of the connectivity 
//  * */

// void MESHCollection::buildConnectZones()
// {
//   vector <map <MED_EN::medGeometryElement, vector<MEDPARTITIONER_FaceModel*> > > face_map(_topology->nbDomain());
//   map< pair<int,int>, MEDMEM::MEDSKYLINEARRAY*> cell_corresp_here;

//   MED_EN::medEntityMesh constituent_entity = getSubEntity();

//   if ( isParallelMode() )
//   {
//     buildConnectZonesBetweenProcs(face_map, cell_corresp_here);
//   }

//   cout << "Computing node/node corresp"<<endl;

//   //Creating nodes
//   for (int idomain=0; idomain<_topology->nbDomain(); idomain++)
//   {

//     // constructing node/node correspondencies
//     vector<MEDMEM::MEDSKYLINEARRAY*> node_node_correspondency(_topology->nbDomain());
//     _topology->computeNodeNodeCorrespondencies(idomain, node_node_correspondency );

//     for (int idistant=0; idistant< _topology->nbDomain(); idistant++)
//     {
//       // on regarde si une correspondance noeud/noeud a été trouvée 
//       // entre idomain et idistant
//       // si oui, on crée une connectzone
//       if (node_node_correspondency[idistant]!=0)
//       {
//         MEDMEM::CONNECTZONE* cz= new MEDMEM::CONNECTZONE();
//         cz->setLocalMesh(_mesh[idomain]);
//         cz->setDistantMesh(_mesh[idistant]);
//         cz->setLocalDomainNumber(idomain);
//         cz->setDistantDomainNumber(idistant);
//         cz-> setName ("Connect zone defined by PARTITIONER");
//         cz->setNodeCorresp(node_node_correspondency[idistant]);
//         _connect_zones.push_back(cz);  
//       }
//     }
//   }
//   cout << "Computing face corresp"<<endl;

//   //creating faces if required 
//   if (_subdomain_boundary_creates)
//   {
//     int global_face_id = _topology->getFaceNumber()+1;
//     //int global_face_id = _topology->getMaxGlobalFace()+1;

//     map <pair<int,int>, vector<int> > faces_in_joint;

//     if ( !isParallelMode() )
//       // taking faces that are already present in the mesh into account
//       for (int idomain=0; idomain<_topology->nbDomain(); idomain++)
//       {
//         getFaces(idomain,face_map[idomain]); 
//       }

//     // creating faces that are located at the interface between
//     // subdomains 

//     vector <int> nb_added_groups( _topology->nbDomain(), 0 );

//     for (int idomain=0; idomain<_topology->nbDomain(); idomain++)
//     {
//       vector<MEDMEM::MEDSKYLINEARRAY*> cell_cell_correspondency( _topology->nbDomain() );
//       if ( !isParallelMode() )
//         _topology->computeCellCellCorrespondencies(idomain, cell_cell_correspondency, _cell_graph.get());

//       for (int idistant=0; idistant< _topology->nbDomain(); idistant++)
//       {
//         if (idistant <= idomain) continue;

//         MEDMEM::MEDSKYLINEARRAY* cell_correspondency = 0;
//         if ( isParallelMode() )
//           cell_correspondency = cell_corresp_here[ make_pair (idomain,idistant)];
//         else
//           cell_correspondency = cell_cell_correspondency[idistant];

//         //the connect zone has been created by the node/node computation

//         if ( cell_correspondency )
//         {
//           int nbcells      = cell_correspondency->getNumberOf();
//           const int* index = cell_correspondency->getIndex();
//           const int* value = cell_correspondency->getValue();
//           if ( isParallelMode() )
//             global_face_id = _domain_selector->getFisrtGlobalIdOfSubentity( idomain, idistant );

//           for (int ilocal=0; ilocal<nbcells; ilocal++)
//           { 
//             for (int icelldistant = index[ilocal]; icelldistant < index[ilocal+1]; icelldistant++)
//             {
//               int distant_id = value[icelldistant-1];
//               MEDPARTITIONER_FaceModel* face = getCommonFace(idomain,ilocal+1,idistant,distant_id,global_face_id);
//               face_map[idomain][face->getType()].push_back(face);
//               MEDPARTITIONER_FaceModel* face2 = getCommonFace(idistant,distant_id,idomain, ilocal+1,global_face_id);
//               face_map[idistant][face->getType()].push_back(face2);
//               faces_in_joint[make_pair(idomain,idistant)].push_back(global_face_id);
//               global_face_id++;
//             } 
//           }
//         }

//       }
//       //cleaning up
//       for (int idistant=0; idistant< _topology->nbDomain(); idistant++)
//         delete cell_cell_correspondency[idistant];
//     }


//     _topology->recreateFaceMapping(face_map);

//     //transforming the face_map into a constituent entity connectivity
//     for (int idomain=0; idomain< _topology->nbDomain();idomain++) 
//     {
//       int nbtypes = face_map[idomain].size();
//       vector<medGeometryElement> types;
//       vector <int> nb_elems;
//       vector <int*> conn;

//       MEDMEM::MESHING* meshing = dynamic_cast<MEDMEM::MESHING*> (_mesh[idomain]);
//       if ( !meshing->getConnectivityptr() )
//         continue; // no cells in idomain

//       for (map <medGeometryElement, vector<MEDPARTITIONER_FaceModel*> >::const_iterator iter= face_map[idomain].begin();
//            iter != face_map[idomain].end(); iter ++)
//       {
//         types.push_back(iter->first);
//         int nb_elem_in_type = (iter->second).size();
//         nb_elems.push_back(nb_elem_in_type);
//         int nb_node_per_type=(iter->first)%100;
//         int* connectivity= new int [nb_node_per_type*nb_elem_in_type];
//         for (int ielem=0; ielem<nb_elem_in_type; ielem++)
//         {
//           for (int inode=0;  inode<nb_node_per_type; inode++)
//             connectivity[ielem*nb_node_per_type+inode]=(*(iter->second)[ielem])[inode];
//         }
//         conn.push_back(connectivity);

//       }
//       //setting the faces in the mesh
//       meshing->setNumberOfTypes(nbtypes,constituent_entity);
//       meshing->setTypes(&types[0],constituent_entity);
//       meshing->setNumberOfElements(&nb_elems[0],constituent_entity);

//       for (int itype=0; itype<nbtypes; itype++)
//       {
//         meshing->setConnectivity(conn[itype], constituent_entity, types[itype]);
//         delete[]conn[itype];
//       }
//       for (int idistant =0; idistant<_topology->nbDomain(); idistant++)
//       {
//         map <pair<int,int>, vector<int> >::iterator iter;
//         iter = faces_in_joint.find(make_pair(idomain,idistant));
//         if (iter == faces_in_joint.end())
//         {
//           iter = faces_in_joint.find (make_pair(idistant,idomain));
//           if (iter == faces_in_joint.end()) 
//             continue;
//         }

//         int nbfaces = (iter->second).size();   
//         vector<int> face_joint(nbfaces*2);
//         MEDMEM::CONNECTZONE* cz=0;
//         for (int icz=0; icz<_connect_zones.size();icz++)
//           if (_connect_zones[icz]->getLocalDomainNumber()==idomain &&
//               _connect_zones[icz]->getDistantDomainNumber()==idistant)
//             cz = _connect_zones[icz];

//         int nbtotalfaces= _topology->getFaceNumber(idomain);

//         //creating arrays for the MEDSKYLINEARRAY structure containing the joint
//         int* index =new int[nbtotalfaces+1];
//         for (int i=0; i<nbtotalfaces+1;i++)
//           index[i]=0;
//         int*value=new int[nbfaces];

//         map<int,int> faces;
//         vector<int> local_faces( nbfaces );
//         for (int iface=0; iface<nbfaces; iface++)
//         {
//           int iglobal = (iter->second)[iface];
//           int localid=_topology->convertGlobalFace(iglobal,idomain);
//           int distantid=_topology->convertGlobalFace(iglobal,idistant);
//           faces.insert(make_pair(localid,distantid));
//           local_faces[iface]=localid;
//         }

//         int iloc=0;
//         index[0]=1;
//         for (map<int,int>::const_iterator iter=faces.begin(); 
//              iter != faces.end();
//              iter++)
//         {
//           index[iter->first]=1;
//           value[iloc++]=iter->second;            
//         }

//         for (int i=0; i<nbtotalfaces;i++)
//           index[i+1]+=index[i];
//         bool shallowcopy=true;  
//         MEDMEM::MEDSKYLINEARRAY* skarray=new MEDMEM::MEDSKYLINEARRAY(nbtotalfaces,nbfaces,index,value,shallowcopy);  

//         if (cz!=0)  
//           cz->setEntityCorresp(constituent_entity,constituent_entity,skarray);              
//         else 
//           throw MEDEXCEPTION("MESHCollection::buildConnectZones() -A connect zone should exist");            
//         // Creating a group of the faces constituting the joint
//         addJointGroup( local_faces, idomain, idistant );
//         nb_added_groups[ idomain ]++;
//       }
//     }

//     if ( isParallelMode() )
//     {
//       // Now all faces have got local ids and we can receive local ids from other procs.
//       // Set face/face data to zones with other procs and create a group
//       for (int icz=0; icz<_connect_zones.size();icz++)
//       {
//         MEDMEM::CONNECTZONE* cz=_connect_zones[icz];
//         if ( _domain_selector->isMyDomain( cz->getDistantDomainNumber()) ) continue;
        
//         int glob_id = _domain_selector->getFisrtGlobalIdOfSubentity( cz->getLocalDomainNumber(),
//                                                                      cz->getDistantDomainNumber());
//         int nb_cz_faces = _domain_selector->getNbCellPairs( cz->getDistantDomainNumber(),
//                                                             cz->getLocalDomainNumber());
//         vector< int > loc_ids_here( nb_cz_faces );
//         for ( int i = 0; i < nb_cz_faces; ++i )
//           loc_ids_here[i] = _topology->convertGlobalFace(glob_id++,cz->getLocalDomainNumber());

//         int* loc_ids_dist = _domain_selector->exchangeSubentityIds( cz->getLocalDomainNumber(),
//                                                                     cz->getDistantDomainNumber(),
//                                                                     loc_ids_here );
//         int nb_faces_here= _topology->getFaceNumber(cz->getLocalDomainNumber());
//         int* face_index = new int[ nb_faces_here+1 ];
//         face_index[0]=1;
//         for ( int loc_id = 0, i = 0; loc_id < nb_faces_here; ++loc_id)
//         {
//           face_index[ loc_id+1 ] = face_index[ loc_id ];
//           if ( i < loc_ids_here.size() && loc_ids_here[i] == loc_id+1 )
//           {
//             face_index[ loc_id+1 ]++;
//             i++;
//           }
//         }
//         MEDMEM::MEDSKYLINEARRAY* skarray=
//           new MEDMEM::MEDSKYLINEARRAY(nb_faces_here, nb_cz_faces, face_index, loc_ids_dist, true);
//         cz->setEntityCorresp(constituent_entity,constituent_entity,skarray);

//         addJointGroup( loc_ids_here, cz->getLocalDomainNumber(), cz->getDistantDomainNumber());
//         nb_added_groups[ cz->getLocalDomainNumber() ]++;
//       }
//     }

//     for (int idomain=0; idomain<_topology->nbDomain(); idomain++)
//     {
//       // delete face_map
//       for (map <medGeometryElement, vector<MEDPARTITIONER_FaceModel*> >::const_iterator iter= face_map[idomain].begin();
//            iter != face_map[idomain].end(); iter ++)
//         for (int i=0; i<(iter->second).size();i++)
//           delete (iter->second)[i];

//       if ( nb_added_groups[ idomain ] > 0 &&
//            _mesh[idomain]->getNumberOfFamilies( constituent_entity ) > 0 )
//         // needed because if there were face families before, driver won't
//         // create families from just added groups (see MEDMEM_MedMeshDriver22.cxx:3330),
//         // actually it is a bug of driver - it must check presence of groups in families
//         _mesh[idomain]->createFamilies(); 
//     }
//   }

//   if ( isParallelMode() )
//     // Excange info on types of constituent_entity needed while writing joints
//     // to get ids local in geom type for distant procs
//     _domain_selector->gatherEntityTypesInfo( _mesh, constituent_entity );

//   cout << "Computing cell/cell corresp"<<endl;

//   //Creating cell/cell correspondencies
//   for (int idomain=0;idomain<_topology->nbDomain();idomain++)
//   {
//     vector<MEDMEM::MEDSKYLINEARRAY*> cell_cell_correspondency( _topology->nbDomain() );
//     if ( !isParallelMode() )
//       _topology->computeCellCellCorrespondencies(idomain,cell_cell_correspondency,_cell_graph.get());

//     for (int idistant=0; idistant< _topology->nbDomain(); idistant++)
//     {
//       MEDMEM::MEDSKYLINEARRAY* cell_correspondency = 0;
//       if ( isParallelMode() )
//         cell_correspondency = cell_corresp_here[ make_pair (idomain,idistant)];
//       else
//         cell_correspondency = cell_cell_correspondency[idistant];

//       //the connect zone has been created by the node/node computation
//       if ( cell_correspondency )
//       {
//         MEDMEM::CONNECTZONE* cz=0;
//         for (int icz=0; icz<_connect_zones.size();icz++)
//           if (_connect_zones[icz]->getLocalDomainNumber()==idomain &&
//               _connect_zones[icz]->getDistantDomainNumber()==idistant)
//             cz = _connect_zones[icz];
//         if (cz!=0)  
//           cz->setEntityCorresp(MED_EN::MED_CELL,MED_EN::MED_CELL, cell_correspondency);
//         else 
//           throw MEDEXCEPTION("MESHCollection::buildConnectZones() -A connect zone should exist");   
//       }
//     }
//   }
// }

// /*! building Connect Zones for storing the informations
//  * of the connectivity in the parallel mode
//  * */

// void MESHCollection::buildConnectZonesBetweenProcs(TGeom2FacesByDomian & face_map,
//                                                    map< pair<int,int>, MEDMEM::MEDSKYLINEARRAY*> & cell_cell_correspondency_here)
// {
//   using namespace MED_EN;

//   // graph over all procs
//   auto_ptr<Graph> global_graph( _domain_selector->gatherGraph( _cell_graph.get() ));

//   vector< vector< JointExchangeData > > joints_of_domain( _topology->nbDomain() );

//   for (int idomain=0; idomain<_topology->nbDomain(); idomain++)
//   {
//     if ( !_domain_selector->isMyDomain( idomain )) continue;

//     vector< JointExchangeData > & joints = joints_of_domain[ idomain ];
//     joints.resize( _topology->nbDomain() );

//     // Find corresponding cells on other procs

//     const int* gra_index = global_graph->getGraph()->getIndex();
//     const int* gra_value = global_graph->getGraph()->getValue();
//     const int* partition = global_graph->getPart();
//     const int dj = gra_index[0];

//     vector< int > glob_cells_here( _topology->getCellNumber( idomain ));
//     _topology->getCellList( idomain, & glob_cells_here[0]);
//     for ( int loc_here = 0; loc_here < glob_cells_here.size(); ++loc_here )
//     {
//       int glob_here = glob_cells_here[ loc_here ];
//       for ( int j = gra_index[ glob_here-1 ]; j < gra_index[ glob_here ]; ++j )
//       {
//         int glob_neighbor = gra_value[ j-dj ];
//         int neighbor_dom = partition[ glob_neighbor-1 ];
//         if ( neighbor_dom == idomain ) continue;

//         if ( _domain_selector->isMyDomain( neighbor_dom ))
//         {
//           joints[ neighbor_dom ].addCellCorrespondence
//             (_mesh[idomain], neighbor_dom, idomain, glob_neighbor, glob_here, loc_here + 1,
//              _topology->convertGlobalCell(glob_neighbor).second );
//         }
//         else
//         {
//           joints[ neighbor_dom ].addCellCorrespondence
//             (_mesh[idomain], neighbor_dom, idomain, glob_neighbor, glob_here, loc_here + 1 );
//         }
//       }
//     }
//   }
//   global_graph.reset(); // free memory

//   // set joints in a queue to exchange
//   typedef map< int, JointExchangeData* > TOrderedJoints;
//   TOrderedJoints queue;
//   for (int idomain=0; idomain<_topology->nbDomain(); idomain++)
//   {
//     if ( !_domain_selector->isMyDomain( idomain )) continue;

//     vector< JointExchangeData > & joints = joints_of_domain[ idomain ];
//     for (int idist=0; idist<_topology->nbDomain(); ++idist )
//     {
//       JointExchangeData& joint = joints[idist];

//       int nb_cell_pairs = joint.nbCellPairs();
//       if ( nb_cell_pairs == 0 )
//         continue;
//       else
//         _domain_selector->setNbCellPairs( nb_cell_pairs, idist, idomain );

//       joint.setMeshes( idist, _mesh[idist], idomain, _mesh[idomain] );

//       if ( _domain_selector->isMyDomain( idist ))
//       {
//         // a joint on this proc
//         cell_cell_correspondency_here[ make_pair( idomain, idist )] = joint.makeCellCorrespArray();
//       }
//       else
//       {
//         // a joint with distant proc
//         joint.setConnectivity( & ((MEDMEM::MeshFuse*)_mesh[idomain])->getNodeNumbers()[0] );
//         int order = _domain_selector->jointId( idomain, idist );
//         queue[ order ] = & joint;
//       }
//     }
//   }
//   // gather info on cell geom types needed to exchange joints
//   _domain_selector->gatherEntityTypesInfo( _mesh, MED_EN::MED_CELL );

//   // gather info on nb of sub-entities to compute their global numbers for joints
//   _domain_selector->gatherNbOf( getSubEntity(), _mesh );
//   _domain_selector->gatherNbCellPairs();
//   if ( _subdomain_boundary_creates )
//   {
//     // taking faces that are already present in the mesh into account
//     for (int idomain=0; idomain<_topology->nbDomain(); idomain++)
//       if ( _domain_selector->isMyDomain( idomain ))
//         getFaces(idomain,face_map[idomain]);
//   }
//   else
//   {
//     face_map.clear(); // mark for the joint not to create faces
//   }

//   // exchange joint data with other procs and make CONNECTZONEs
//   TOrderedJoints::iterator ord_joint = queue.begin();
//   for ( ; ord_joint != queue.end(); ++ord_joint )
//   {
//     JointExchangeData* joint = ord_joint->second;

//     _domain_selector->exchangeJoint( joint );
//     if ( _subdomain_boundary_creates )
//     {
//       int first_sub_id = _domain_selector->getFisrtGlobalIdOfSubentity( joint->localDomain(),
//                                                                         joint->distantDomain() );
//       joint->setFisrtGlobalIdOfSubentity( first_sub_id );
//     }
//     _connect_zones.push_back ( joint->makeConnectZone( face_map ));
//   }
// }

// /*! projects old collection families on new collection families
//  */
// void MESHCollection::castFamilies(const MESHCollection& old_collection)
// {
//   vector <list<int> > element_array  (_topology->nbDomain());

//   //loop on old domains to create groups out of the existing families
//   if (_family_splitting)
//     for (int idomain=0; idomain < old_collection._topology->nbDomain(); idomain++)
//       old_collection.getMesh(idomain)->createGroups();

//   //definition of the entities array which 
//   //defines the entities over which the information is cast
//   MED_EN::medEntityMesh entities[3];
//   entities[0]=MED_EN::MED_NODE;
//   entities[1]=getSubEntity();
//   entities[2]=MED_EN::MED_CELL;

//   for (int ientity=0; ientity<=2;ientity++)
//   {

//     //int nbgroups = old_collection.getMesh(0)->getNumberOfGroups(entities[ientity]);

//     map <string, set<int> > group_map;
//     for (int idomain=0; idomain < old_collection._topology->nbDomain(); idomain++)
//     {
//       if ( !old_collection.getMesh(idomain) ) continue;
//       for (int igroup=0; igroup<old_collection.getMesh(idomain)->getNumberOfGroups(entities[ientity]); igroup++)
//       {
//         //retrieves a group
//         MEDMEM::GROUP* group = (old_collection.getMesh(idomain)->getGroups(entities[ientity]))[igroup];
//         //increments the number of groups if it is a new group
//         //if (group_map.find(group->getName())==group_map.end())

//         group_map[group->getName()].insert(idomain);
//         //   group_map.insert(make_pair(group->getName(), idomain);

//       }   
//     }
//     int nbgroups=group_map.size();
//     vector <int> igroupold(old_collection._topology->nbDomain(),0);
//     map<string,set<int> >::const_iterator iter=group_map.begin();

//     for (int igroup=0; igroup<nbgroups; igroup++)
//     {
//       vector <const MEDMEM::SUPPORT*> old_supports(old_collection._topology->nbDomain());
//       string group_name = iter->first;
//       iter++; 

//       //parameters stored for passing group description
//       // from the old meshes to the new ones

//       for (int idomain=0; idomain < old_collection._topology->nbDomain(); idomain++)
//       {
//         //                for (set<int>::iterator iter=group_map[group_name].begin(); iter!=group_map[group_name].end(); iter++)
//         //                cout << *iter<<" ";
//         //                cout <<endl;
//         if (group_map[group_name].find(idomain)==group_map[group_name].end()) continue;

//         //retrieves the group igroup on domain idomain
//         MEDMEM::GROUP* group = (old_collection.getMesh(idomain)->getGroups(entities[ientity]))[igroupold[idomain]];
//         old_supports[idomain] = static_cast<const MEDMEM::SUPPORT*> (group);
//         igroupold[idomain]++;
//       }

//       vector <MEDMEM::GROUP*>new_groups(_topology->nbDomain());
//       vector <MEDMEM::SUPPORT*> new_supports(_topology->nbDomain());
//       for (int i=0; i<_topology->nbDomain(); i++)
//       {
//         new_groups[i]=new MEDMEM::GROUP();
//         new_supports[i]=static_cast<MEDMEM::SUPPORT*>(new_groups[i]);
//       }
//       castSupport(old_collection,old_supports,new_supports);      

//       //creating new groups from the previous list of elements
//       for (int idomain=0; idomain <_topology->nbDomain(); idomain++)
//       {
//         MEDMEM::MESHING* mesh_builder=static_cast<MEDMEM::MESHING*> (_mesh[idomain]);
//         if ( new_supports[idomain] )
//           mesh_builder->addGroup(*new_groups[idomain]);
//       }
//       //groups are copied by the addGroup method,
//       //so they can be safely deleted here
//       for (int i=0; i<_topology->nbDomain(); i++)
//       {
//         if ( new_supports[i] ) new_groups[i]->removeReference();
//       }

//     }// on groups
//   }//on entities
// }


// void MESHCollection::castSupport(const MESHCollection& old_collection, vector<const MEDMEM::SUPPORT*>& old_support, vector<MEDMEM::SUPPORT*>& new_support)
// {

//   if (old_collection._topology->nbDomain() != old_support.size())
//   {
//     throw MED_EXCEPTION(STRING("Error : wrong call to MESHCollection::castSupport"));
//   }
//   vector <list<int> > element_array  (_topology->nbDomain());

//   //parameters stored for passing description
//   // from the old meshes to the new ones
//   string name;
//   string description;
//   MED_EN::medEntityMesh entity;
//   vector <string> support_name(1);
//   support_name[0]="support";
//   for (int inew=0; inew< _topology->nbDomain(); inew++)
//     element_array[inew].clear();

//   for (int idomain=0; idomain < old_collection._topology->nbDomain(); idomain++)
//   {
//     //retrieves the group igroup on domain idomain
//     const MEDMEM::SUPPORT* support = old_support[idomain];
//     if (old_support[idomain]==0) continue;
//     name = support->getName();
//     description=support->getDescription();
//     int nbelem = support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS);
//     if (nbelem==0 && !_create_empty_groups) continue;

//     int* list_of_elems;
//     if (support->isOnAllElements())
//     {
//       list_of_elems = new int[nbelem];
//       for (int i=0; i<nbelem;i++)
//         list_of_elems[i]=i+1;
//     }
//     else
//       list_of_elems = const_cast<int*> (support->getNumber(MED_EN::MED_ALL_ELEMENTS));

//     int* array=new int[nbelem];
//     int* ip=0;
//     int* local=0;
//     int* full_array=0;
//     entity = support->getEntity();
//     int size;

//     switch (entity)
//     {
//     case MED_EN::MED_CELL :
//       ip=new int[nbelem];
//       local= new int[nbelem];
//       size=nbelem;
//       old_collection.getTopology()->convertCellToGlobal(idomain,list_of_elems,nbelem,array);
//       _topology->convertGlobalCellList(array,nbelem,local,ip);
//       for (int i=0; i<nbelem; i++)
//         //              cell_arrays[ip[i]][local[i]]=id;
//       {
//         //          cout <<"(glob,ip,iloc)/nbelem"<<array[i]<<" "<<ip[i]<<" "<<local[i]<<"/"<<nbelem<<endl;
//         element_array[ip[i]].push_back(local[i]);
//       }
//       break;
//     case MED_EN::MED_FACE :
//     case MED_EN::MED_EDGE :
//       old_collection.getTopology()->convertFaceToGlobal(idomain,list_of_elems,nbelem,array);
//       _topology->convertGlobalFaceListWithTwins(array,nbelem,local,ip,full_array,size);
//       for (int i=0; i<size; i++)
//         element_array[ip[i]].push_back(local[i]);
//       delete[] full_array;  
//       break;
//     case MED_EN::MED_NODE :
//       old_collection.getTopology()->convertNodeToGlobal(idomain,list_of_elems,nbelem,array);
//       _topology->convertGlobalNodeListWithTwins(array,nbelem,local,ip,full_array,size);
//       for (int i=0; i<size; i++)
//         element_array[ip[i]].push_back(local[i]);
//       delete[] full_array;
//       break;

//     }
//     delete[] ip;
//     delete[] local;
//     delete[] array;

//     if (support->isOnAllElements()) delete[] list_of_elems;
//   }

//   //creating new groups from the previous list of elements
//   for (int idomain=0; idomain <_topology->nbDomain(); idomain++)
//   {
//     if ( _mesh[idomain]->getNumberOfNodes() < 1 || 
//          (element_array[idomain].empty() && !_create_empty_groups))
//     {
//       new_support[idomain]->removeReference();
//       new_support[idomain]=0;
//       continue;
//     }
//     MEDMEM::SUPPORT* support= new_support[idomain];
//     support->setName(name);
//     support->setMesh(_mesh[idomain]);
//     support->setDescription(description);
//     support->setEntity(entity);

//     element_array[idomain].sort();
//     element_array[idomain].unique();

//     if ( element_array[idomain].empty() )
//     {
//       support->setNumberOfGeometricType(0);
//     }
//     else
//     {
//       if (entity != MED_EN::MED_NODE)
//         support->fillFromElementList(element_array[idomain]);
//       else
//       {
//         support->fillFromNodeList(element_array[idomain]);
//       }
//     }
//   }
// }

// void MESHCollection::castFieldDouble(const MESHCollection& old_collection, const string& fieldname, int itnumber, int ordernumber)
// {
//   int type=old_collection.getDriver()->getFieldType(fieldname);
//   char field_char[80];
//   strcpy(field_char,fieldname.c_str());
//   std::vector<ParaMEDMEM::TypeOfField> fieldTypes =MEDLoader::GetTypesOfField(fileName, fieldName,meshName);
  
// }

// void MESHCollection::castAllFields(const MESHCollection& initial_collection)
// {
//   vector <string> field_names;
//   vector <int> iternumber;
//   vector <int> ordernumber;
//   vector <int> types;

//   string filename=initial_collection.getDriver()->getFilename();
//   field_names=MEDLoader::GetAllFieldNames(filename.c_str());

// readFileStruct(field_names,iternumber,ordernumber,types);

//   for (int i=0; i<field_names.size(); i++)
//   {
//     char field_char[80];
//     strcpy(field_char,field_names[i].c_str());

//     castFieldDouble(initial_collection, field_char, iternumber[i], ordernumber[i]);
//   }
// }

// //! \param initial_collection mesh collection before the redistribution stage
// //! \param idomain domain number on the new collection
// //! \param entity dimension of the treated nodes (0 : cell, 1 : face in 3d, edge in 2d, 2: edge in 3d)
// void MESHCollection::createNodalConnectivity(const MESHCollection& initial_collection,int idomain, int entity)
// {

  
//   int dimension=0;
//   int nb_elems=0;
//   ParaMEDMEM::MEDCouplingUMesh* mesh_builder = m_mesh[idomain];

//   //creating arrays for storing global numbers and cell types
//   switch (entity)
//   {
//   case MED_EN::MED_CELL:
//     dimension=initial_collection.getMeshDimension();
//     nb_elems=m_topology->getCellNumber(idomain);
//     break;
//   case MED_EN::MED_EDGE:
//   case MED_EN::MED_FACE:
//     dimension=initial_collection.getMeshDimension()-1;
//     nb_elems=m_topology->getFaceNumber(idomain);
//     break;
//   default:
//     nb_elems=0;
//     break;
//   }

//   if (nb_elems == 0) return;
//   SCRUTE_MED(nb_elems);


//   int *list= new int[nb_elems];
//   MED_EN::medGeometryElement *cell_type_list= new MED_EN::medGeometryElement[nb_elems];


//   //      cout << "Beginning of retrieval "<<endl;
//   //retrieving global id list
//   switch (entity)
//   {
//   case MED_EN::MED_CELL:
//     m_topology->getCellList(idomain,list);
//     break;
//   case MED_EN::MED_EDGE:
//   case MED_EN::MED_FACE:
//     m_topology->getFaceList(idomain,list);
//     break;
//   default:

//     break;
//   }

//   //retrieving cell_types
//   initial_collection.getTypeList(list,nb_elems,entity,cell_type_list);
//   //      cout <<"end of type retrieval"<<endl;
//   //vector containing the number of cells per type
//   type_numbers.clear();
//   for (int icell=0; icell<nb_elems; icell++)
//   {
//     map<MED_EN::medGeometryElement,int>::iterator iter= type_numbers.find(cell_type_list[icell]);
//     if (iter!=type_numbers.end())
//       (iter->second)++;
//     else
//       type_numbers[cell_type_list[icell]]=1;

//   }
//   //cout << "Nombre de tetras"<<type_numbers[304]<<endl;
//   int nb_present_types=type_numbers.size();

//   //setting the list of cells for each type
//   map<MED_EN::medGeometryElement,int> index;

//   map<MED_EN::medGeometryElement,int*> type_cell_list;

//   MED_EN::MESH_ENTITIES::const_iterator currentEntity;
//   std::map<MED_EN::medGeometryElement,int>::const_iterator iter;
//   //currentEntity  = MED_EN::meshEntities.find(entity);
//   for (iter = type_numbers.begin();iter != type_numbers.end(); iter++)    
//   {
//     MED_EN::medGeometryElement type = iter->first;
//     if (!isDimensionOK(type,dimension)) continue;
//     //if (iter->second==0) continue;
//     index[type]=0;
//     type_cell_list[type]=new int[type_numbers[type]];
//     // cout << "type :"<<type<<" nb:"<<type_numbers[type]<<endl;
//   }

//   for (int icell=0; icell<nb_elems; icell++)
//   {
//     type_cell_list[cell_type_list[icell]][index[cell_type_list[icell]]++]=list[icell];
//   }

//   delete[]list;
//   delete[]cell_type_list;

//   //setting the list of present ypes
//   int* present_type_numbers=new int[nb_present_types];
//   MED_EN::medGeometryElement* type_array = new MED_EN::medGeometryElement[nb_present_types];
//   MESSAGE_MED("Nb de types presents "<<nb_present_types);
//   int itype=0;
//   for (iter = type_numbers.begin();iter != type_numbers.end(); iter++)  
//   {
//     MED_EN::medGeometryElement type = iter->first;
//     if (!isDimensionOK(type,dimension)) continue;

//     type_array[itype]=type;

//     present_type_numbers[itype]=type_numbers[type];

//     MESSAGE_MED("Nombre d'elements de type "<<type<<" : "<<type_numbers[type]);
//     itype++;
//   }

//   //retrieving connectivity in global numbering for each typeinitial_collection.getMesh(iold)->get
//   map<MED_EN::medGeometryElement,int*> type_connectivity;
//   vector<int> polygon_conn;
//   vector<int> polygon_conn_index;
//   vector<int> polyhedron_conn;
//   vector<int> polyhedron_conn_index;
//   vector<int> polyhedron_face_index;

//   //Treating nodes

//   DataArrayInt* conn_index = (initial_collection.getMesh())[idomain].getConnIndex();
//   DataArrayInt* index=(initial_collection.getMesh())[idomain].getIndex();
  
  
//   _topology->convertGlobalCellList(cell_list,nb_cells,local,ip);
//   for (iter = type_numbers.begin();iter != type_numbers.end(); iter++)  
//   {
//     MED_EN::medGeometryElement type = iter->first;


//     if (!isDimensionOK(type,dimension)) continue;
//     //if (type_numbers[type]==0) continue;
//     if (type != MED_EN::MED_POLYGON && type != MED_EN::MED_POLYHEDRA)
//     {
//       int nbnode_per_type = (int)type%100;
//       type_connectivity[type]=new int[type_numbers[type]*nbnode_per_type];
//       initial_collection.getNodeConnectivity(type_cell_list[type],type_numbers[type],entity,type,type_connectivity[type]);
//     }
//     else if (type == MED_EN::MED_POLYGON && dimension==2)
//     {
//       initial_collection.getPolygonNodeConnectivity(type_cell_list[type],type_numbers[type],entity,polygon_conn,polygon_conn_index);
//       //type_connectivity[type]=&polygon_conn[0];
//     }
//     else if (type == MED_EN::MED_POLYHEDRA && dimension==3)
//     {
//       initial_collection.getPolyhedraNodeConnectivity(type_cell_list[type],type_numbers[type],entity,polyhedron_conn,
//                                                       polyhedron_conn_index, polyhedron_face_index);
//       //type_connectivity[type]=&polygon_conn[0];
//     }
//     delete[] type_cell_list[type];
//   }

//   //creating node mapping 
//   //!TODO : compute the total number of nodes 
//   if (entity==MED_EN::MED_CELL)
//   {
//     m_topology->createNodeMapping(type_connectivity,type_numbers,polygon_conn,polygon_conn_index,
//                                   polyhedron_conn,polyhedron_conn_index,polyhedron_face_index,idomain);
//   }

//   //converting node global numberings to local numberings
//   //for (iter = (*currentEntity).second.begin();iter != (*currentEntity).second.end(); iter++)
//   for (iter = type_numbers.begin();iter != type_numbers.end(); iter++)  
//   {
//     MED_EN::medGeometryElement type = iter->first;

//     if (!isDimensionOK(type, dimension)) continue;
//     if (type_numbers[type]==0) continue;
//     if (type != MED_EN::MED_POLYGON && type != MED_EN::MED_POLYHEDRA)
//     {
//       int nbnode_per_type = (int)type%100;
//       m_topology->convertToLocal2ndVersion(type_connectivity[type],type_numbers[type]*nbnode_per_type,idomain);
//     }
//     else if (type == MED_EN::MED_POLYGON && dimension==2)
//     {
//       int nbpoly = type_numbers[type]; 
//       m_topology->convertToLocal2ndVersion(&polygon_conn[0], polygon_conn_index[nbpoly]-1, idomain);  
//     }
//     else if (type == MED_EN::MED_POLYHEDRA && dimension==3)
//     {
//       int nbpoly = type_numbers[type]; 
//       m_topology->convertToLocal2ndVersion(&polyhedron_conn[0], polyhedron_face_index[polyhedron_conn_index[nbpoly]-1]-1, idomain);  
//     }

//   }


//   //writing coordinates
//   if (entity==MED_EN::MED_CELL)   
//   {
//     //setting coordinates from initial_collection coordinates
//     int nbnode=m_topology->getNodeNumber(idomain);
//     MESSAGE_MED("Number of nodes on domain "<< idomain <<" : "<<nbnode);

//     double* coordinates=new double[initial_collection.getSpaceDimension()*nbnode];
//     int* node_list=new int[nbnode];
//     m_topology->getNodeList(idomain,node_list);
//     initial_collection.getCoordinates(node_list,nbnode,coordinates);
//     delete[] node_list;

//     // redundant specification of number of nodes is required!! MED imperfection, sorry...  

//     mesh_builder->setNumberOfNodes(nbnode);
//     //TODO : change MEDMEM so that it accepts a direct setting of coordinates
//     // (in the present version, it is deep-copied)
//     mesh_builder->setCoordinates(initial_collection.getSpaceDimension(),
//                                  m_topology->getNodeNumber(idomain), coordinates, initial_collection.getSystem(),
//                                  MED_EN::MED_FULL_INTERLACE);
//     delete [] coordinates;
//   }

//   int nb_plain_types=0;
//   for (iter = type_numbers.begin();iter != type_numbers.end(); iter++) 
//   {
//     MED_EN::medGeometryElement type = iter->first;

//     if (!isDimensionOK(type, dimension)) continue;
//     if (type_numbers[type]==0) continue;
//     if (type != MED_EN::MED_POLYGON && type != MED_EN::MED_POLYHEDRA)
//       nb_plain_types++;
//   }
//   mesh_builder->setNumberOfTypes(nb_plain_types,entity);
//   mesh_builder->setTypes(type_array,entity);
//   mesh_builder->setNumberOfElements(present_type_numbers,entity);
//   if (entity==MED_EN::MED_CELL)
//     mesh_builder->setMeshDimension(dimension);

//   delete[]present_type_numbers;
//   delete[]type_array;
//   //setting node connectivities
//   for (iter = type_numbers.begin();iter != type_numbers.end(); iter++)  
//   {
//     MED_EN::medGeometryElement type = iter->first;

//     if (!isDimensionOK(type,dimension)) continue;
//     if (type_numbers[type]==0) continue;

//     if (type != MED_EN::MED_POLYHEDRA && type != MED_EN::MED_POLYGON)
//     {
//       mesh_builder->setConnectivity(type_connectivity[type],entity,type);
//       delete[] type_connectivity[type];
//     }
//     else if (type == MED_EN::MED_POLYGON && dimension ==2)
//     {
//       mesh_builder->setPolygonsConnectivity(&polygon_conn_index[0],
//                                             &polygon_conn[0],
//                                             type_numbers[type],
//                                             entity);
//     }
//     else if (type == MED_EN::MED_POLYHEDRA && dimension ==3)
//     {
//       mesh_builder->setPolyhedraConnectivity(&polyhedron_conn_index[0],
//                                              &polyhedron_face_index[0],
//                                              &polyhedron_conn[0],
//                                              type_numbers[type],
//                                              entity);

//     }
//   }
//   MESSAGE_MED("end of createNodalConnectivity");
// }


// /*! retrieves the faces that are present in a mesh and stores them in a 
//  * dynamic structure made of a map of MEDPARTITIONER_FaceModel
//  * 
//  * \param idomain domain id on which the faces are collected
//  * \param face_map container storing the faces 
//  */
// void MESHCollection::getFaces(int idomain, 
//                               map<MED_EN::medGeometryElement, vector<MEDPARTITIONER_FaceModel*> >& face_map)                     
// {
//   MED_EN::medEntityMesh constituent_entity = getSubEntity();
//   const medGeometryElement* types;
//   try
//   {
//     types = _mesh[idomain]->getTypes(constituent_entity);
//   }
//   catch(MEDEXCEPTION&){ return;}

//   int nbtypes  = _mesh[idomain]->getNumberOfTypes(constituent_entity);
//   const int* global_numbering= _mesh[idomain]->getGlobalNumberingIndex(constituent_entity);
//   int* conn = const_cast<int*> (_mesh[idomain]->getConnectivity(MED_EN::MED_FULL_INTERLACE,MED_EN::MED_NODAL,constituent_entity, MED_EN::MED_ALL_ELEMENTS));
//   for (int itype=0; itype<nbtypes; itype++)
//   {
//     for (int iface=global_numbering[itype]; iface<global_numbering[itype+1]; iface++)
//     {
//       MEDPARTITIONER_FaceModel* face_model = new MEDPARTITIONER_FaceModel();
//       MED_EN::medGeometryElement type =  types[itype];
//       face_model->setType(type);
//       int nbnodes = type%100;
//       face_model->setNbNodes(nbnodes);
//       face_model->setGlobal(_topology->convertFaceToGlobal(idomain,iface));
//       for (int i=0; i<nbnodes; i++)
//       {
//         (*face_model)[i]=*conn++;
//       }
//       face_map[type].push_back(face_model);
//     }
//   }
// }

// /*! retrieves the face that is common to two cells located on two different processors
//  * 
//  * \param ip1 domain id for cell 1
//  * \param ilocal1 cell id for cell 1
//  * \param ip2 domain id for cell 2
//  * \param ilocal2 cell id for cell 2
//  * \param face_index global index for the newly created face 
//  */
// MEDPARTITIONER_FaceModel* MESHCollection::getCommonFace(int ip1,int ilocal1,int ip2,int ilocal2,int face_index)
// {
//   MED_EN::medGeometryElement type1 = _mesh[ip1]->getElementType(MED_EN::MED_CELL,ilocal1);
//   MEDMEM::CELLMODEL celltype1 (type1);

//   const int* conn_index1 =  _mesh[ip1]->getConnectivityIndex(MED_EN::MED_NODAL,MED_EN::MED_CELL);
//   const int* conn1 = _mesh[ip1]->getConnectivity(MED_EN::MED_FULL_INTERLACE,MED_EN::MED_NODAL,MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);

//   // MED_EN::medGeometryElement type2 = _mesh[ip2]->getElementType(MED_EN::MED_CELL,ilocal2);
//   //MEDMEM::CELLTYPE celltype2 (type2);
//   const int* conn_index2 =  _mesh[ip2]->getConnectivityIndex(MED_EN::MED_NODAL,MED_EN::MED_CELL);
//   const int* conn2 = _mesh[ip2]->getConnectivity(MED_EN::MED_FULL_INTERLACE,MED_EN::MED_NODAL,MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);

//   vector<int> nodes1, nodes1_local;
//   vector<int> nodes2;
//   for (int i=  conn_index1[ilocal1-1]; i<conn_index1[ilocal1]; i++)
//   {
//     nodes1.push_back(_topology->convertNodeToGlobal(ip1,*(conn1+i-1)));
//     nodes1_local.push_back( conn1[i-1] );
//   }
//   for (int i=  conn_index2[ilocal2-1]; i<conn_index2[ilocal2]; i++)
//     nodes2.push_back(_topology->convertNodeToGlobal(ip2,*(conn2+i-1)));

//   return MEDPARTITIONER_FaceModel::getCommonFace( &nodes1[0], &nodes1_local[0], celltype1,
//                                                &nodes2[0], nodes2.size(),  face_index);
// }

// //================================================================================
// /*!
//  * \brief Makes a face common for two given cells
//  *  \param nodes1 - globl nodes of the first cell
//  *  \param nodes1_local - local nodes of the first cell
//  *  \param celltype1 - cell model of the first cell
//  *  \param nodes2 - globl nodes of the second cell
//  *  \param nb_nodes2 - nb of nodes of the second cell
//  *  \param global_id - id of the new common face
//  */
// //================================================================================

// MEDPARTITIONER_FaceModel*
// MEDPARTITIONER_FaceModel::getCommonFace(const int*               nodes1,
//                                      const int*               nodes1_local,
//                                      const MEDMEM::CELLMODEL& celltype1,
//                                      const int*               nodes2,
//                                      int                      nb_nodes2,
//                                      int                      global_id)
// {
//   int nbfaces= celltype1.getNumberOfConstituents(1);
//   int ** faces = celltype1.getConstituents(1);
//   MED_EN::medGeometryElement* types = celltype1.getConstituentsType(1);
//   int iface=0;
//   int dimension=celltype1.getDimension();

//   while (iface<nbfaces)
//   {
//     //SCRUTE_MED (iface);
//     int nbnodes= types[iface]%100;
//     const int* nodes = celltype1.getNodesConstituent(1,iface+1);
//     int common_nodes=0;
//     for (int i=0; i<nbnodes;i++)
//     {
//       for (int i2=0; i2<nb_nodes2; i2++)
//       {
//         if (nodes1[nodes[i]-1]==nodes2[i2]) common_nodes++;
//       }     
//     }
//     if (common_nodes>=dimension) break;
//     iface++;
//   }

//   if (iface==nbfaces)
//     throw MEDEXCEPTION("MEDPARTITIONER::getCommonFace - No common face found !");

//   MEDPARTITIONER_FaceModel* face_model = new MEDPARTITIONER_FaceModel;
//   face_model->setType(types[iface]);
//   int nbnodes = types[iface]%100;
//   face_model->setNbNodes(nbnodes);
//   face_model->setGlobal(global_id); 
//   for (int i=0; i<nbnodes; i++)
//     (*face_model)[i]=nodes1_local[faces[iface][i]-1];

//   return face_model;
// }
void MESHCollection::setDomainNames(const std::string& name)
{
  for (int i=0; i<_topology->nbDomain(); i++)
    {
      ostringstream oss;
      oss<<name<<"_"<<i;
      if (!isParallelMode() || _domain_selector->isMyDomain(i))
      _mesh[i]->setName(oss.str().c_str());
    }
}
