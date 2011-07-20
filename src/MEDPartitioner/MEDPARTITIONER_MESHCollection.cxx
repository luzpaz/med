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

#include "MEDPARTITIONER_ParallelTopology.hxx"
#include "MEDPARTITIONER_ParaDomainSelector.hxx"

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

#include <set>

#include <iostream>
#include <fstream>

using namespace MEDPARTITIONER;

#ifndef WNT
using namespace __gnu_cxx;
#else
using namespace std;
#endif


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
  _cellFamilyIds.resize(topology->nbDomain());
  //allocating family ids arrays
  for (int inew=0; inew<topology->nbDomain();inew++)
    {
      if(isParallelMode() && !_domain_selector->isMyDomain(inew)) continue;
      _cellFamilyIds[inew]=ParaMEDMEM::DataArrayInt::New();
      int nbCells=_mesh[inew]->getNumberOfCells();
      int* ptrCellIds=new int[nbCells];
      for (int i=0; i< nbCells;i++) ptrCellIds[i]=0;
      _cellFamilyIds[inew]->useArray(ptrCellIds,true, ParaMEDMEM::CPP_DEALLOC,nbCells,1);

      int nbFaces=_faceMesh[inew]->getNumberOfCells();
      _faceFamilyIds[inew]=ParaMEDMEM::DataArrayInt::New();
      int* ptrFaceIds=new int[nbFaces];
      for (int i=0; i<nbFaces;i++) ptrFaceIds[i]=0;
      _faceFamilyIds[inew]->useArray(ptrFaceIds,true, ParaMEDMEM::CPP_DEALLOC,nbFaces,1); 
    }

  //casting cell and face  families on new meshes
  castIntField(initialCollection.getMesh(), this->getMesh(),initialCollection.getCellFamilyIds(),_cellFamilyIds);
  castIntField(initialCollection.getFaceMesh(), this->getFaceMesh(),initialCollection.getFaceFamilyIds(),_faceFamilyIds);


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
              _domain_selector->sendMesh(*(splitMeshes[inew][iold]),_domain_selector->getProcessorID(inew));
            
            if (_domain_selector->isMyDomain(inew))
              _domain_selector->recvMesh(splitMeshes[inew][iold],_domain_selector->getProcessorID(iold));
            
          }
    }
  
  //fusing the split meshes
  for (int inew=0; inew<_topology->nbDomain()  ;inew++)
    {
      std::vector<const ParaMEDMEM::MEDCouplingUMesh*> meshes;
     
      for (int i=0; i< splitMeshes[inew].size();i++)
        if (splitMeshes[inew][i]!=0) meshes.push_back(splitMeshes[inew][i]);

      if (!isParallelMode()||_domain_selector->isMyDomain(inew))
        {
          _mesh[inew]=ParaMEDMEM::MEDCouplingUMesh::MergeUMeshes(meshes);
          bool areNodesMerged;
          int nbNodesMerged;
          ParaMEDMEM::DataArrayInt* array=_mesh[inew]->mergeNodes(1e-12,areNodesMerged,nbNodesMerged);
          array->decrRef(); // array is not used in this case
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
          int nvertices=initialCollection.getMesh(iold)->getNumberOfNodes();
          bbox=new double[nvertices*6];
          ParaMEDMEM::DataArrayDouble* coords = initialCollection.getMesh(iold)->getCoords();
          double* coordsPtr=coords->getPointer();
              
          for (int i=0; i<nvertices*3;i++)
            {
              bbox[i*2]=coordsPtr[i]-1e-6;
              bbox[i*2+1]=coordsPtr[i]+1e-6;
            }
          tree=new BBTree<3>(bbox,0,0,nvertices,1e-9);
        }
              
      for (int inew=0; inew<_topology->nbDomain(); inew++)
        {
          //sending meshes for parallel computation
          if (isParallelMode() && _domain_selector->isMyDomain(inew) && !_domain_selector->isMyDomain(iold))  
              _domain_selector->sendMesh(*(getMesh(inew)), _domain_selector->getProcessorID(iold));
          else if (isParallelMode() && !_domain_selector->isMyDomain(inew)&& _domain_selector->isMyDomain(iold))
            {
              ParaMEDMEM::MEDCouplingUMesh* mesh;
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
                  if (elems.size()==0) continue;              
                  nodeMapping.insert(make_pair(make_pair(iold,elems[0]),make_pair(inew,inode)));
                }
            }
        }
      if (!isParallelMode() || (_domain_selector->isMyDomain(iold)))
        {
          delete tree;
          delete[] bbox;
        }
    } 

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

          //analysis of element ielem
          //counters are set for the element
          //for each source node, the mapping is interrogated and the domain counters 
          //are incremented for each target node
          //the face is considered as going to target domains if the counter of the domain 
          //is equal to the number of nodes
          for (int inode=0;inode<nodes.size();inode++)
            {
              typedef multimap<pair<int,int>,pair<int,int> >::const_iterator MI;
              int mynode=nodes[inode];

              pair <MI,MI> myRange = nodeMapping.equal_range(make_pair(iold,mynode));
              for (MI iter=myRange.first; iter!=myRange.second; iter++)
                {
                  int inew=iter->second.first;
                  if (faces.find(inew)==faces.end())
                    faces[inew]=1;
                  else
                    faces[inew]++;
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
          splitMeshes[inew][iold]=(ParaMEDMEM::MEDCouplingUMesh*)(meshesCastFrom[iold]->buildPartOfMySelf(&new2oldIds[iold][inew][0],&new2oldIds[iold][inew][0]+new2oldIds[iold][inew].size(),true));
          splitMeshes[inew][iold]->zipCoords();
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
      //      meshesCastTo[inew]->zipCoords();
      for (int iold=0; iold < meshesCastFrom.size();iold++)
        if (splitMeshes[inew][iold]!=0) splitMeshes[inew][iold]->decrRef();
    }
}

void MESHCollection::castIntField(std::vector<ParaMEDMEM::MEDCouplingUMesh*>& meshesCastFrom,std::vector<ParaMEDMEM::MEDCouplingUMesh*>& meshesCastTo,  std::vector<ParaMEDMEM::DataArrayInt*>& arrayFrom,  std::vector<ParaMEDMEM::DataArrayInt*>& arrayTo)
{
  vector<vector<const ParaMEDMEM::DataArrayInt*> > splitIds;
  splitIds.resize(meshesCastTo.size());
 
  // send / recv operations
  for (int inew=0; inew < meshesCastTo.size();inew++)    
    for (int iold=0; iold < meshesCastFrom.size();iold++)
      {
        vector<int> recvIntVec;
        //sending arrays for distant domains
        if (isParallelMode() && _domain_selector->isMyDomain(iold) && !_domain_selector->isMyDomain(inew))
          {
            _domain_selector->sendMesh(*meshesCastFrom[iold],_domain_selector->getProcessorID(inew));
            int size=arrayFrom[iold]->getNumberOfTuples();
            vector<int>sendIds(size);
            std::copy(arrayFrom[iold]->getPointer(),arrayFrom[iold]->getPointer()+size,&sendIds[0]);
            _domain_selector->sendIntVec(sendIds, _domain_selector->getProcessorID(inew));
          }
        //receiving arrays from distant domains
        if (isParallelMode()&&!_domain_selector->isMyDomain(iold) && _domain_selector->isMyDomain(inew))
          {
            vector<int> recvIds;
            ParaMEDMEM::MEDCouplingUMesh* recvMesh;
            _domain_selector->recvMesh(recvMesh,_domain_selector->getProcessorID(iold));
            _domain_selector->recvIntVec(recvIds, _domain_selector->getProcessorID(iold));
            remapIntField(*recvMesh,*meshesCastTo[inew],&recvIds[0],arrayTo[inew]->getPointer());
          }
      }

  //local contributions and aggregation
  for (int inew=0; inew < meshesCastTo.size();inew++)    
    {
      for (int iold=0; iold < meshesCastFrom.size();iold++)
        if (!isParallelMode() || ( _domain_selector->isMyDomain(iold) && _domain_selector->isMyDomain(inew)))
            {
              remapIntField(*meshesCastFrom[iold],*meshesCastTo[inew],arrayFrom[iold]->getConstPointer(),arrayTo[inew]->getPointer());
            }     
    }
}

void MESHCollection::remapIntField(const ParaMEDMEM::MEDCouplingUMesh& sourceMesh,
                              const ParaMEDMEM::MEDCouplingUMesh& targetMesh,
                              const int* fromArray,
                              int* toArray)
{
  ParaMEDMEM::DataArrayDouble* sourceCoords=sourceMesh.getBarycenterAndOwner();
  ParaMEDMEM::DataArrayDouble* targetCoords=targetMesh.getBarycenterAndOwner();
   
  ParaMEDMEM::MEDCouplingUMesh* tmpMesh=ParaMEDMEM::MEDCouplingUMesh::New();
  tmpMesh->setCoords(sourceCoords);
  vector<int>c;
  vector<int> cI;
  tmpMesh->getNodeIdsNearPoints(targetCoords->getConstPointer(),targetMesh.getNumberOfCells(),1e-10,c,cI);
  if (cI.size()!= targetMesh.getNumberOfCells()+1) throw INTERP_KERNEL::Exception("Error in source/target projection");
  for (int itargetnode=0; itargetnode<targetMesh.getNumberOfCells();itargetnode++)    
    {
      if (cI[itargetnode]==cI[itargetnode+1]) continue;
      int isourcenode=c[cI[itargetnode]];
      toArray[itargetnode]=fromArray[isourcenode];
    } 
  sourceCoords->decrRef();
  targetCoords->decrRef();
  tmpMesh->decrRef();
    
    
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
  // char filenamechar[256];
  // strcpy(filenamechar,filename.c_str());
  try
    {
      _driver=new MESHCollectionMedXMLDriver(this);
      _driver->read (filename.c_str());
      _driver_type = MedXML;

    }
  catch(MEDMEM::MEDEXCEPTION){
    delete _driver;
    try
      {
        _driver=new MESHCollectionMedAsciiDriver(this);
        _driver->read (filename.c_str());
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
  //char filenamechar[256];
  //char meshnamechar[256];
  //  strcpy(filenamechar,filename.c_str());
  //strcpy(meshnamechar,meshname.c_str());
  try // avoid memory leak in case of inexistent filename
    {
      retrieveDriver()->readSeq (filename.c_str(),meshname.c_str());
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
    {
      if (_mesh[i]!=0)  _mesh[i]->decrRef(); 
      if (_cellFamilyIds[i]!=0) _cellFamilyIds[i]->decrRef();
    }
  
  for (int i=0; i<_faceMesh.size();i++)
    {
      if (_faceMesh[i]!=0)  _faceMesh[i]->decrRef();
      if (_faceFamilyIds[i]!=0) _faceFamilyIds[i]->decrRef();
    }
  
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
  //   if (_topology->nbDomain()>1)
  //     buildConnectZones();
  //suppresses link with driver so that it can be changed for writing
  if (_driver!=0)delete _driver;
  _driver=0;

  //char filenamechar[256];
  //  strcpy(filenamechar,filename.c_str());
  retrieveDriver()->write (filename.c_str(), _domain_selector);
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
                  multimap<int,int>::iterator iter;
                  for (iter=commonDistantNodes[idomain][mydomain].begin();iter!=commonDistantNodes[idomain][mydomain].end();iter++)
                    {
                      int ilocnode=iter->first;
                      int icell=iter->second;
                      
                      node2cell.insert(make_pair(globalNodeIds[ilocnode],icell+offset));     
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
