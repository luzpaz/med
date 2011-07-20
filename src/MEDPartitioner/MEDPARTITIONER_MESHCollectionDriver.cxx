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
//#include "MEDPARTITIONER_ConnectZone.hxx"
#include "MEDPARTITIONER_Topology.hxx"
#include "MEDPARTITIONER_ParallelTopology.hxx"
#include "MEDPARTITIONER_SequentialTopology.hxx"
#include "MEDPARTITIONER_MESHCollectionDriver.hxx"
#include "MEDPARTITIONER_MESHCollection.hxx"
#include "MEDPARTITIONER_ParaDomainSelector.hxx"

using namespace MEDPARTITIONER;

//template inclusion
//#include "MEDPARTITIONER_MESHCollectionDriver.H"

// med_geometrie_element typmai[MED_NBR_GEOMETRIE_MAILLE+2] = { MED_POINT1,
//                                                              MED_SEG2,
//                                                              MED_SEG3,
//                                                              MED_TRIA3,
//                                                              MED_TRIA6,
//                                                              MED_QUAD4,
//                                                              MED_QUAD8,
//                                                              MED_TETRA4,
//                                                              MED_TETRA10,
//                                                              MED_HEXA8,
//                                                              MED_HEXA20,
//                                                              MED_PENTA6,
//                                                              MED_PENTA15,
//                                                              MED_PYRA5,
//                                                              MED_PYRA13,
//                                                              MED_POLYGONE,
//                                                              MED_POLYEDRE };

MESHCollectionDriver::MESHCollectionDriver(MESHCollection* collection):_collection(collection)
{
}


/*!reads a unique MED File v>=2.1
 * and mounts the corresponding mesh in memory
 *\param filename binary file
 *\param meshname mesh name in the MED file
 * */
int MESHCollectionDriver::readSeq(const char* filename, const char* meshname)
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
  string meshname=_meshname[idomain];
  string file=_filename[idomain];

  cout << "Reading "<<_meshname[idomain]<<" in "<<_filename[idomain]<<endl;

  ParaMEDMEM::MEDFileUMesh* mfm=ParaMEDMEM::MEDFileUMesh::New(file.c_str(),meshname.c_str());
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

  mfm->decrRef();
}


void MESHCollectionDriver::writeMedFile(int idomain, const string& distfilename)
{
  vector<const ParaMEDMEM::MEDCouplingUMesh*> meshes;
  ParaMEDMEM::MEDCouplingUMesh* cellMesh=_collection->getMesh(idomain);
  ParaMEDMEM::MEDCouplingUMesh*faceMesh =_collection->getFaceMesh(idomain);
  
  meshes.push_back(cellMesh);
  // faceMesh->zipCoords();
  faceMesh->checkCoherency();
  faceMesh->tryToShareSameCoordsPermute(*cellMesh,1e-10);
  meshes.push_back(faceMesh);
  MEDLoader::WriteUMeshes(distfilename.c_str(), meshes,true);       
  ParaMEDMEM::MEDFileUMesh* mfm=ParaMEDMEM::MEDFileUMesh::New(distfilename.c_str(),_collection->getMesh(idomain)->getName());
  mfm->setFamilyFieldArr(-1,(_collection->getFaceFamilyIds())[idomain]);
  mfm->setFamilyInfo(_collection->getFamilyInfo());
  mfm->setGroupInfo(_collection->getGroupInfo());
  mfm->write(distfilename.c_str(),0);
  mfm->decrRef();
  //  writeSubdomain(idomain, nbdomains, distfilename.c_str(), domainSelector);
}
