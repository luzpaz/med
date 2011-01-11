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
#include <map>
#include <set>

#include <iostream>
#include <fstream>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <sys/time.h>
#include "MEDCouplingUMesh.hxx"
#include "MEDLoader.hxx"
#include "MEDFileMesh.hxx"
#include "MEDMEM_Exception.hxx"
extern "C" {
#include "med.h"
}
//MEDPARTITIONER includes
#include "MEDPARTITIONER_ConnectZone.hxx"
#include "MEDPARTITIONER_Topology.hxx"
#include "MEDPARTITIONER_ParallelTopology.hxx"
#include "MEDPARTITIONER_SequentialTopology.hxx"
#include "MEDPARTITIONER_MESHCollectionDriver.hxx"
#include "MEDPARTITIONER_MESHCollection.hxx"
#include "MEDPARTITIONER_ParaDomainSelector.hxx"

using namespace MEDPARTITIONER;

//template inclusion
//#include "MEDPARTITIONER_MESHCollectionDriver.H"

med_geometrie_element typmai[MED_NBR_GEOMETRIE_MAILLE+2] = { MED_POINT1,
                                                             MED_SEG2,
                                                             MED_SEG3,
                                                             MED_TRIA3,
                                                             MED_TRIA6,
                                                             MED_QUAD4,
                                                             MED_QUAD8,
                                                             MED_TETRA4,
                                                             MED_TETRA10,
                                                             MED_HEXA8,
                                                             MED_HEXA20,
                                                             MED_PENTA6,
                                                             MED_PENTA15,
                                                             MED_PYRA5,
                                                             MED_PYRA13,
                                                             MED_POLYGONE,
                                                             MED_POLYEDRE };

MESHCollectionDriver::MESHCollectionDriver(MESHCollection* collection):_collection(collection)
{
}


/*!reads a unique MED File v>=2.1
 * and mounts the corresponding mesh in memory
 *\param filename binary file
 *\param meshname mesh name in the MED file
 * */
int MESHCollectionDriver::readSeq(char* filename, char* meshname)
{
  //const char* LOC = "MEDPARTITIONER::MESHCollectionDriver::readSeq()";
  //  BEGIN_OF_MED(LOC);

  _filename.resize(1);
  _filename[0]=string(filename);

  ParaMEDMEM::MEDFileUMesh* mfm=ParaMEDMEM::MEDFileUMesh::New(filename,meshname);
  //puts the only mesh in the mesh vector
  (_collection->getMesh()).push_back(mfm->getLevel0Mesh(false));
  (_collection->getFaceMesh()).push_back(mfm->getLevelM1Mesh(false));

  //reading family ids

  ParaMEDMEM::DataArrayInt* cellIds(mfm->getFamilyFieldAtLevel(0)->deepCpy());
  ParaMEDMEM::DataArrayInt* faceIds(mfm->getFamilyFieldAtLevel(-1)->deepCpy());
  (_collection->getCellFamilyIds()).push_back(cellIds);
  (_collection->getFaceFamilyIds()).push_back(faceIds); 

  //reading groups
  (_collection->getFamilyInfo())=mfm->getFamilyInfo();
  (_collection->getGroupInfo())=mfm->getGroupInfo();
  
// (_collection->getGroupMeshes()).resize(groupNames.size());
  
//   for (int i=0; i< groupNames.size();i++)
//     {
//       vector<string> myGroup;
//       myGroup.push_back(groupNames[i]);
//       (_collection->getGroupMeshes())[i].push_back(MEDLoader::ReadUMeshFromGroups(filename,meshname,-1,myGroup));
//     }


  (_collection->getCZ()).clear();
  vector<int*> cellglobal,nodeglobal,faceglobal;
  cellglobal.resize(1);
  nodeglobal.resize(1);
  faceglobal.resize(1);
  cellglobal[0]=0;
  nodeglobal[0]=0;
  faceglobal[0]=0;
  //creation of topology from mesh 
  //connectzone argument is 0
  ParallelTopology* aPT = new ParallelTopology
    ((_collection->getMesh()), (_collection->getCZ()), cellglobal, nodeglobal, faceglobal);
  _collection->setTopology(aPT);
  _collection->setName(meshname);
  _collection->setDomainNames(meshname);

  //  END_OF_MED(LOC);
  return 0;
}


//================================================================================
/*!
 * \brief Return mesh dimension from distributed med file had being read
 */
//================================================================================


void MESHCollectionDriver::readSubdomain(vector<int*>& cellglobal,
                                         vector<int*>& faceglobal,
                                         vector<int*>& nodeglobal, int idomain)
{
//   const char* LOC = "MEDPARTITIONER::MESHCollectionDriver::readSubdomain()";
//   BEGIN_OF_MED(LOC);
  char file[256];
  char meshname[MED_TAILLE_NOM+1];

  strcpy(meshname,_meshname[idomain].c_str());
  strcpy(file,_filename[idomain].c_str());
  cout << "Reading "<<_meshname[idomain]<<" in "<<_filename[idomain]<<endl;
  //(_collection->getMesh())[idomain]=new MEDMEM::MESH(MEDMEM::MED_DRIVER,file, meshname);
 ParaMEDMEM::MEDFileUMesh* mfm=ParaMEDMEM::MEDFileUMesh::New(file,meshname);
 (_collection->getMesh())[idomain]=mfm->getLevel0Mesh(false);
  (_collection->getFaceMesh())[idomain]=mfm->getLevelM1Mesh(false);

  //reading families
 
  ParaMEDMEM::DataArrayInt* cellIds(mfm->getFamilyFieldAtLevel(0)->deepCpy());
  ParaMEDMEM::DataArrayInt* faceIds(mfm->getFamilyFieldAtLevel(-1)->deepCpy());

  (_collection->getCellFamilyIds())[idomain]=cellIds;
  (_collection->getFaceFamilyIds())[idomain]=faceIds; 

  //reading groups
  _collection->getFamilyInfo()=mfm->getFamilyInfo();
  _collection->getGroupInfo()=mfm->getGroupInfo();

  cout <<"End of Read"<<endl;
  //reading MEDPARTITIONER::CONNECTZONEs NODE/NODE and CELL/CELL
  med_idt fid = MEDouvrir(file,MED_LECTURE);
  med_int njoint = MEDnJoint(fid, meshname);
  for (int ijoint=0; ijoint<njoint; ijoint++)
  {
    int distant;
    char joint_description[MED_TAILLE_DESC+1];
    char name[MED_TAILLE_NOM+1];
    char name_distant[MED_TAILLE_NOM+1];

    int ncorr = MEDjointInfo(fid,meshname, ijoint+1, name, 
                                      joint_description,
                                      &distant, name_distant);

    for (int ic=0; ic<ncorr; ic++)
    {
      med_entite_maillage cor_typent_local;
      med_geometrie_element cor_typgeo_local;
      med_entite_maillage cor_typent_dist;
      med_geometrie_element cor_typgeo_dist;


      int ncouples;
      ncouples = MEDjointTypeCorres(fid, meshname, name, ic+1,
                                             &cor_typent_local,  &cor_typgeo_local,
                                             &cor_typent_dist, &cor_typgeo_dist
                                             );
      int* node_corresp=new int[ncouples];
      if (cor_typent_local == MED_NOEUD && cor_typent_dist == MED_NOEUD)
      {

        MEDjointLire(fid, meshname, name,
                              node_corresp,ncouples,
                              cor_typent_local,  cor_typgeo_local,
                              cor_typent_dist, cor_typgeo_dist
                              );
      }
      //constructing the connect zone and adding it to the connect zone list
      MEDPARTITIONER::CONNECTZONE* cz = new MEDPARTITIONER::CONNECTZONE();
      cz->setName(string(name));
      cz->setDescription(joint_description);
      cz->setLocalDomainNumber(idomain);
      cz->setDistantDomainNumber(distant);
      cz->setLocalMesh((_collection->getMesh())[idomain]);
      cz->setDistantMesh((_collection->getMesh())[distant]);
      cz->setNodeCorresp(node_corresp,ncouples);
      (_collection->getCZ()).push_back(cz);

    }//loop on correspom_topology->nbDomain())ndances
  }//loop on joints 

  // Reading global numbering
  // à faire dans MEDLoader 

  //  MEDCouplingFieldDouble globalcell=MEDLoader::GetGlobalNumbering(file,meshname,0);


   MEDfermer(fid);

   //  END_OF_MED(LOC);
}

void MESHCollectionDriver::writeSubdomain(int idomain, int nbdomains, char* distfilename,
                                          ParaDomainSelector* domain_selector)
{
  //build connect zones
  //      if (nbdomains>1)
  //  _collection->buildConnectZones(idomain);

  //  MESSAGE_MED(" Number of connect zones "<<(_collection->getCZ()).size());

  //writing connect zones in joints

  med_idt fid = MEDouvrir(distfilename,MED_LECTURE_ECRITURE);  

  int index_joint=0;


  for (int icz=0; icz<(_collection->getCZ()).size(); icz++)
  {
    if ((_collection->getCZ())[icz]->getLocalDomainNumber()==idomain)
    {
      med_err error;
      int idistant=(_collection->getCZ())[icz]->getDistantDomainNumber();
      char joint_name[MED_TAILLE_NOM+1];
      sprintf(joint_name,"joint_%i",idistant+1);
      char desc[MED_TAILLE_DESC+1];
      sprintf(desc,"connect_zone_%d",icz+1);

      char distant_name[MED_TAILLE_NOM+1];
      //sprintf(distant_name,"domain_%i",(_collection->getCZ())[icz]->getDistantDomainNumber());

      //        sprintf(distant_name,(_collection->getMesh())[idistant]->getName().c_str());
      sprintf(distant_name,"domain_%i",idistant);
      char mesh_name[MED_TAILLE_NOM+1];

      strcpy (mesh_name, (_collection->getMesh())[idomain]->getName());
      //      SCRUTE_MED((_collection->getMesh())[idomain]->getName());
      error = MEDjointCr(fid,mesh_name, joint_name, desc, 
                                  idistant, distant_name);
      if (error==-1) cout << "erreur creation de joint "<<endl;

      /////////////////////////////////////////
      //writing node/node correspondency
      /////////////////////////////////////////
      int nbnodes=(_collection->getCZ())[icz]->getNodeNumber();
      int* node_corresp=const_cast<int*>((_collection->getCZ())[icz]->getNodeCorrespValue());

      /* Nodes are reordered so that the ordering on the local and the distant domain
         correspond. The chosen order is the natural ordering on the domain
         with lowest proc id*/
      // if (_collection->getSubdomainBoundaryCreates())
//         if (idomain<idistant)
//           jointSort(node_corresp, nbnodes, true);
//         else
//           jointSort(node_corresp, nbnodes, false);

      error=
        MEDjointEcr(fid, mesh_name, joint_name, node_corresp, nbnodes,
                             MED_NOEUD, MED_POINT1,MED_NOEUD, MED_POINT1);
      if (error==-1) cout << "erreur creation de joint "<<endl;

      //writing cell/cell joint      
//      writeElementJoint(MED_EN::MED_CELL, icz, idomain, idistant, mesh_name,joint_name,fid);
      //writing face/face joint
//       if (_collection->getSubdomainBoundaryCreates())
//       {
//         MED_EN::medEntityMesh constituent_entity =
//           (_collection->getMeshDimension()==3)?MED_EN::MED_FACE:MED_EN::MED_EDGE;
//         writeElementJoint(constituent_entity, icz, idomain, idistant, mesh_name,joint_name,fid);                 
//       }                   
      index_joint++;
    }
  }
  
  char meshchar[MED_TAILLE_NOM+1];
  strcpy(meshchar,(_collection->getMesh())[idomain]->getName());
  

  MEDfermer(fid);
  std::cout<<"End of writing"<<std::endl;

}
