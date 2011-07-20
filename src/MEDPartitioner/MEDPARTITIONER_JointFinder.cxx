#include "MEDPARTITIONER_JointFinder.hxx"
#include "MEDPARTITIONER_MESHCollection.hxx"
#include "MEDPARTITIONER_Topology.hxx"
#include "MEDPARTITIONER_ParaDomainSelector.hxx"
#include "MEDCouplingUMesh.hxx"
#include "BBTree.txx"


/*! Method contributing to the distant cell graph
 */
using namespace MEDPARTITIONER;

JointFinder::JointFinder(const MESHCollection& mc):_mesh_collection(mc), _topology(mc.getTopology()),_domain_selector(mc.getParaDomainSelector()) 
{
}

void JointFinder::findCommonDistantNodes()
{
  int nbdomain=_topology->nbDomain();
  _distant_node_cell.resize(nbdomain);
  _node_node.resize(nbdomain);
  for (int i=0; i<nbdomain;i++) 
    {
      _distant_node_cell[i].resize(nbdomain);
      _node_node[i].resize(nbdomain);
    }
  int nbproc=_domain_selector->nbProcs();
  std::vector<BBTree<3>* > bbtree(nbdomain); 
  std::vector<ParaMEDMEM::DataArrayInt*> rev(nbdomain);
  std::vector<ParaMEDMEM::DataArrayInt*>revIndx(nbdomain);
  int meshDim;
  int spaceDim;
  
  for (int mydomain=0;mydomain<nbdomain;mydomain++)
    {
      if(! _domain_selector->isMyDomain(mydomain)) continue;
      const ParaMEDMEM::MEDCouplingUMesh* myMesh=_mesh_collection.getMesh(mydomain);
      
      meshDim=myMesh->getMeshDimension();
      spaceDim= myMesh->getSpaceDimension();
      rev[mydomain] = ParaMEDMEM::DataArrayInt::New();
      revIndx[mydomain] = ParaMEDMEM::DataArrayInt::New();
      myMesh->getReverseNodalConnectivity(rev[mydomain],revIndx[mydomain]);
      double* bbx=new double[2*spaceDim*myMesh->getNumberOfNodes()];
      for (int i=0; i<myMesh->getNumberOfNodes()*spaceDim;i++)
        {
          const double* coords=myMesh->getCoords()->getConstPointer();
          bbx[2*i]=(coords[i])-1e-12;
          bbx[2*i+1]=bbx[2*i]+2e-12;
        }
      bbtree[mydomain]=new BBTree<3> (bbx,0,0,myMesh->getNumberOfNodes(),-1e-12);
    }

  for (int isource=0;isource<nbdomain;isource++)
    for (int itarget=0;itarget<nbdomain;itarget++)
       {
            const ParaMEDMEM::MEDCouplingUMesh* sourceMesh=_mesh_collection.getMesh(isource);
        
            if (_domain_selector->isMyDomain(isource)&&_domain_selector->isMyDomain(itarget)) continue;
            if (_domain_selector->isMyDomain(isource))
              {
                //preparing data for treatment on target proc
                int targetProc = _domain_selector->getProcessorID(itarget);
            
                std::vector<double> vec(spaceDim*sourceMesh->getNumberOfNodes());
                std::copy(sourceMesh->getCoords()->getConstPointer(),sourceMesh->getCoords()->getConstPointer()+sourceMesh->getNumberOfNodes()*spaceDim,&vec[0]);
                _domain_selector->sendDoubleVec (vec,targetProc);
            
                //retrieving target data for storage in commonDistantNodes array
                std::vector<int> localCorrespondency;
                _domain_selector->recvIntVec(localCorrespondency, targetProc);
                for (int i=0; i<localCorrespondency.size()/2;i++)
                  _distant_node_cell[isource][itarget].insert(std::make_pair(localCorrespondency[2*i],localCorrespondency[2*i+1]));      
            
              }
            if (_domain_selector->isMyDomain(itarget))    
              {
                //receiving data from source proc
                int sourceProc = isource%nbproc;
                std::vector<double> recvVec;
                _domain_selector->recvDoubleVec(recvVec,sourceProc);
                std::map<int,int> commonNodes; // (local nodes, distant nodes) list
                for (int inode=0; inode<(recvVec.size()/meshDim);inode++)
                  {
                    double* bbox=new double[2*spaceDim];
                    for (int i=0; i<spaceDim;i++)
                      {
                        bbox[2*i]=recvVec[inode*spaceDim+i]-1e-12;
                        bbox[2*i+1]=bbox[2*i]+2e-12;
                      }
                    std::vector<int> inodes;
                    bbtree[itarget]->getIntersectingElems(bbox,inodes);
                    delete[] bbox;
                
                    if (inodes.size()>0) commonNodes.insert(std::make_pair(inodes[0],inode));
                  }
                std::vector<int> nodeCellCorrespondency;
                for (std::map<int,int>::iterator iter=commonNodes.begin();iter!=commonNodes.end();iter++)
                  {
                    _node_node[itarget][isource].push_back(std::make_pair(iter->first, iter->second));//storing node pairs in a vector
                    const int*revIndxPtr=revIndx[itarget]->getConstPointer();
                    const int*revPtr=rev[itarget]->getConstPointer();
                    for (int icell=revIndxPtr[iter->first];icell<revIndxPtr[iter->first+1];icell++)
                      {
                        nodeCellCorrespondency.push_back(iter->second);
                        nodeCellCorrespondency.push_back(revPtr[icell]);
                      }
                  }
                _domain_selector->sendIntVec(nodeCellCorrespondency, sourceProc);
              }
          }
    
}
std::vector<std::vector<std::multimap<int,int> > > & JointFinder::getDistantNodeCell()
{
  return _distant_node_cell;
}

std::vector<std::vector<std::vector<std::pair<int,int> > > >& JointFinder::getNodeNode()
{
  return _node_node;
}
