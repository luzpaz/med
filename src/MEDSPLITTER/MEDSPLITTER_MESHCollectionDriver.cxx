// Copyright (C) 2007-2012  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
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
#ifndef WIN32
#include <sys/time.h>
#else
#include <time.h>
#include <windows.h>
#endif
//Debug macros
#include "MEDMEM_Utilities.hxx"

//MEDMEM includes
#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_MedFileBrowser.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_SkyLineArray.hxx"
#include "MEDMEM_ConnectZone.hxx"
#include "MEDMEM_MeshFuse.hxx"
#include "MEDMEM_MedMeshDriver.hxx"

//MEDSPLITTER includes
#include "MEDSPLITTER_Topology.hxx"
#include "MEDSPLITTER_ParallelTopology.hxx"
#include "MEDSPLITTER_SequentialTopology.hxx"
#include "MEDSPLITTER_MESHCollectionDriver.hxx"
#include "MEDSPLITTER_MESHCollection.hxx"
#include "MEDSPLITTER_ParaDomainSelector.hxx"

using namespace MEDSPLITTER;

//template inclusion
//#include "MEDSPLITTER_MESHCollectionDriver.H"


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
  const char* LOC = "MEDSPLITTER::MESHCollectionDriver::readSeq()";
  BEGIN_OF_MED(LOC);

  _filename.resize(1);
  _filename[0]=string(filename);
  //puts the only mesh in the mesh vector
  //MEDMEM::MESH* new_mesh = new MEDMEM::MESH(MEDMEM::MED_DRIVER,filename, meshname);
  MEDMEM::MESH* new_mesh = new MEDMEM::MESH;
  MED_MESH_RDONLY_DRIVER meshDrv(filename,new_mesh);
  meshDrv.setMeshName( meshname );
  meshDrv.desactivateFacesComputation(); // we need not all faces
  meshDrv.open();
  meshDrv.read();
  meshDrv.close();
  (_collection->getMesh()).push_back(new_mesh);

  _collection->setName(meshname);
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
  END_OF_MED(LOC);
  return 0;
}

/*!
 * Reads the file structure to determine the list
 * of all the available fields
 *
 * \param field_names, vector<string> containing the field names
 * \param iternumber, vector<int> containing the iteration numbers
 * \param ordernumber, vector<int> containing the order numbers
 * \param types, vector<int> containing 0 for int fields and 1 for double fields
 *
 */

void MESHCollectionDriver::readFileStruct(vector <string>&  field_names,vector<int>& iternumber,vector <int>&  ordernumber, vector <int>& types)
{
  const char* LOC = "MEDSPLITTER::MESHCollectionDriver::readFileStruct()";
  BEGIN_OF_MED(LOC);

  const MEDMEM::MEDFILEBROWSER med_struct (_filename[0]);
  int nb_fields = med_struct.getNumberOfFields();

  MESSAGE_MED("found "<<nb_fields<<" fields in file");
  vector<string> names = med_struct.getFieldNames();
  for (int ifield = 0; ifield < nb_fields; ifield++)
  {
    MEDMEM::VEC_DT_IT_ dtit=med_struct.getFieldIteration(names[ifield]);
    for (unsigned i = 0; i < dtit.size(); i++)
    {
      field_names.push_back(names[ifield]);
      iternumber.push_back(dtit[i].dt);
      ordernumber.push_back(dtit[i].it);
      types.push_back( MED_REEL64 == med_struct.getFieldType( names[ifield] ));
    }
  }
  END_OF_MED(LOC);
}

//!retrieves the type of a field for a given fieldname
int MESHCollectionDriver::getFieldType(const string& fieldname)
{
  const char* LOC = "MEDSPLITTER::MESHCollectionDriver::getFieldType()";
  BEGIN_OF_MED(LOC);
  const MEDMEM::MEDFILEBROWSER med_struct (_filename[0]);

  int ret = ( MED_REEL64 == med_struct.getFieldType( fieldname ));
  END_OF_MED(LOC);

  return ret;
}
namespace
{
//================================================================================
/*!
 * \brief Structure used in the method below
 */
struct TJointData
{
  char             _name[MED_NAME_SIZE+1];
  int              _nb_couples;
  med_2_3::med_int _distant_domain;
  med_2_3::med_geometry_type _geo_local, _geo_dist;
};
}

//================================================================================
/*!
 * \brief Read CELL-CELL correspondences of joints with domains on other procs
 *  \param idomain - domain index to return correspondence for
 *  \param loc_domains - domians on this pocessor
 *  \param domain_selector - info on cell distribution among procs
 *  \param loc2glob_corr - out, correspondence pairs where distant ids are global
 */
//================================================================================

void MESHCollectionDriver::readLoc2GlobCellConnect(int                 idomain,
                                                   const set<int>&     loc_domains,
                                                   ParaDomainSelector* domain_selector,
                                                   vector<int> &       loc2glob_corr)
{
  using namespace med_2_3;
  med_2_3::med_err err;

  // find joints of domains touching idomain and loaded on other processors

  TJointData joint;
  list< TJointData > joints;
  int total_nb_couples = 0;

  MEDMEM::MESH* loc_mesh = _collection->getMesh()[idomain];
  char* meshname = (char*) _meshname[idomain].c_str();
  char* filename = (char*) _filename[idomain].c_str();
  //cout << "#" << domain_selector->rank() << ": mesh - " << meshname << endl;

  const int loc_mesh_dim = loc_mesh->getMeshDimension();

  const MED_EN::medGeometryElement* types = loc_mesh->getTypes(MED_EN::MED_CELL);
  int                             nbtypes = loc_mesh->getNumberOfTypes(MED_EN::MED_CELL);
  const list<MED_EN::medGeometryElement>& all_types = MED_EN::meshEntities[ MED_EN::MED_CELL ];

  med_idt fid = MEDfileOpen( filename, med_2_3::MED_ACC_RDONLY );
  int  njoint = MEDnSubdomainJoint(fid, meshname);
  for (int ijoint=0; ijoint<njoint; ijoint++)
  {
    char joint_description[MED_COMMENT_SIZE+1], name_distant[MED_NAME_SIZE+1];
    int nstep,nocstpncorr;
    err = med_2_3::MEDsubdomainJointInfo(fid,meshname, ijoint+1,
                                         joint._name, joint_description,
                                         &joint._distant_domain, name_distant,
                                         &nstep,&nocstpncorr);
    if ( err || loc_domains.count( joint._distant_domain ))
      continue;  // distant is on this proc

    for (int itype=0; itype<nbtypes;itype++)
    {
      joint._geo_local = (med_geometry_type) types[itype];
      list<MED_EN::medGeometryElement>::const_iterator dist_type = all_types.begin();
      for ( ; dist_type != all_types.end(); ++dist_type )
      {
        if ( !_collection->isDimensionOK( *dist_type, loc_mesh_dim ))
          continue;
        joint._geo_dist = (med_geometry_type) *dist_type;
        err = MEDsubdomainCorrespondenceSize(fid, meshname, joint._name,
                                             MED_NO_DT, MED_NO_DT,
                                             med_2_3::MED_CELL, joint._geo_local,
                                             med_2_3::MED_CELL, joint._geo_dist,
                                             & joint._nb_couples );
        if ( !err && joint._nb_couples > 0 )
        {
          joints.push_back( joint );
          total_nb_couples += joint._nb_couples;
        }
      }
    }
  }

  // read cell pairs of found joints and replace distant local ids with global ones

  loc2glob_corr.resize( 2 * total_nb_couples );
  if ( total_nb_couples > 0 )
  {
    int* cell_corresp = & loc2glob_corr[0];

    list< TJointData >::iterator jnt = joints.begin();
    for ( ; jnt != joints.end(); ++jnt )
    {
      // read cell couples
      err = MEDsubdomainCorrespondenceRd(fid, meshname, jnt->_name,
                                         MED_NO_DT,MED_NO_IT, 
                                         med_2_3::MED_CELL, jnt->_geo_local,
                                         med_2_3::MED_CELL, jnt->_geo_dist,
                                         cell_corresp);
      if ( err ) continue;

      // distant local ids -> global ids
      if ( int shift_to_global = domain_selector->getDomainShift( jnt->_distant_domain ))
        for ( int i = 0 ; i < jnt->_nb_couples; ++i )
          cell_corresp[ 2*i + 1 ] += shift_to_global;

      cell_corresp += 2 * jnt->_nb_couples;
    }
  }
  MEDfileClose( fid );
}

//================================================================================
/*!
 * \brief Return mesh dimension from the distributed med file having been read
 */
//================================================================================

int MESHCollectionDriver::readMeshDimension() const
{
  const char* LOC = "MESHCollectionDriver::readMeshDimension(): ";
  if ( _filename.empty() || _meshname.empty() )
    throw MED_EXCEPTION( MEDMEM::STRING(LOC) << "file name or mesh name not available");

  med_2_3::med_idt fid = med_2_3::MEDfileOpen(_filename[0].c_str(),med_2_3::MED_ACC_RDONLY);
  if ( fid < 0 )
    throw MED_EXCEPTION( MEDMEM::STRING(LOC) << "can't open file " << _filename[0]);

  med_2_3::med_int meshDimension, spaceDimension;
  med_2_3::med_mesh_type meshType;
  char commentp3[MED_COMMENT_SIZE+1];
  char dtunittp3[MED_LNAME_SIZE+1];
  med_2_3::med_sorting_type sttp3;
  int nstep;
  med_2_3::med_axis_type axtypp3;
  char *t1pp3=new char[10*MED_SNAME_SIZE+1]; // preview 10-dimensional space :)
  char *t2pp3=new char[10*MED_SNAME_SIZE+1];
  int err= med_2_3::MEDmeshInfoByName(fid,_meshname[0].c_str(),
                                      &spaceDimension, &meshDimension, &meshType,
                                      commentp3,dtunittp3,&sttp3,&nstep,&axtypp3,t1pp3,t2pp3);
  delete [] t1pp3;
  delete [] t2pp3;

  med_2_3::MEDfileClose(fid);

  if ( err )
    throw MED_EXCEPTION( MEDMEM::STRING(LOC) << "mesh name is invalid");

  return meshDimension;
}

void MESHCollectionDriver::readSubdomain(vector<int*>& cellglobal,
                                         vector<int*>& faceglobal,
                                         vector<int*>& nodeglobal, int idomain)
{
  const char* LOC = "MEDSPLITTER::MESHCollectionDriver::readSubdomain()";
  BEGIN_OF_MED(LOC);
  char file[256];
  char meshname[MED_NAME_SIZE+1];

  strcpy(meshname,_meshname[idomain].c_str());
  strcpy(file,_filename[idomain].c_str());
  cout << "Reading "<<_meshname[idomain]<<" in "<<_filename[idomain]<<endl;
  MEDMEM::MESH* mesh = (_collection->getMesh())[idomain]=new MEDMEM::MESH;
  MED_MESH_RDONLY_DRIVER meshDrv(file,mesh);
  meshDrv.setMeshName( _meshname[idomain] );
  meshDrv.desactivateFacesComputation(); // else global face numbering becomes invalid
  meshDrv.open();
  meshDrv.read();
  meshDrv.close();
  cout <<"End of Read"<<endl;
  //reading MEDSPLITTER::CONNECTZONEs NODE/NODE and CELL/CELL
  med_2_3::med_idt fid = med_2_3::MEDfileOpen(file,med_2_3::MED_ACC_RDONLY);
  med_2_3::med_int njoint = med_2_3::MEDnSubdomainJoint(fid, meshname);
  for (int ijoint=0; ijoint<njoint; ijoint++)
  {
    int distant;
    char joint_description[MED_COMMENT_SIZE+1];
    char name[MED_NAME_SIZE+1];
    char name_distant[MED_NAME_SIZE+1];
    int nstep,nocstpncorr;
    int ncorr = med_2_3::MEDsubdomainJointInfo(fid,meshname, ijoint+1, name, 
                                               joint_description,
                                               &distant, name_distant,&nstep,&nocstpncorr);
    for (int ic=0; ic<ncorr; ic++)
    {
      med_2_3::med_entity_type cor_typent_local;
      med_2_3::med_geometry_type cor_typgeo_local;
      med_2_3::med_entity_type cor_typent_dist;
      med_2_3::med_geometry_type cor_typgeo_dist;

      int ncouples;
      med_2_3::MEDsubdomainCorrespondenceSizeInfo(fid, meshname, name, MED_NO_DT,MED_NO_IT, ic+1,
                                                  &cor_typent_local,  &cor_typgeo_local,
                                                  &cor_typent_dist, &cor_typgeo_dist,&ncouples
                                                  );
      int* node_corresp=new int[ncouples*2];
      if (cor_typent_local == med_2_3::MED_NODE && cor_typent_dist == med_2_3::MED_NODE)
      {
        med_2_3::MEDsubdomainCorrespondenceRd(fid, meshname, name,
                                              MED_NO_DT,MED_NO_IT, 
                                              cor_typent_local, cor_typgeo_local,
                                              cor_typent_dist,  cor_typgeo_dist,
                                              node_corresp);
      }
      //constructing the connect zone and adding it to the connect zone list
      MEDMEM::CONNECTZONE* cz = new MEDMEM::CONNECTZONE();
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
  //
  int ncell=(_collection->getMesh())[idomain]->getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
  if (ncell>0)
  {
    int * array=new int[ncell];
    int offset=0;
    MESSAGE_MED("Reading cell global numbering for mesh "<< idomain);
    list<MED_EN::medGeometryElement>::const_iterator iter;
    char meshchar[MED_NAME_SIZE+1];
    strcpy(meshchar,(_collection->getMesh())[idomain]->getName().c_str());
    int nbtypes = (_collection->getMesh())[idomain]->getNumberOfTypes(MED_EN::MED_CELL);
    const MED_EN::medGeometryElement* types =(_collection->getMesh())[idomain]->getTypes(MED_EN::MED_CELL);
    for (int itype=0; itype<nbtypes;itype++)
    {
      MED_EN::medGeometryElement type=types[itype];
      if (!_collection->isDimensionOK(type,(_collection->getMesh())[idomain]->getMeshDimension()))
        continue;
      int ntype = (_collection->getMesh())[idomain]->getNumberOfElements(MED_EN::MED_CELL,type);
      if (ntype==0) continue;
      med_2_3::MEDmeshGlobalNumberRd(fid, meshname,
                                     MED_NO_DT, MED_NO_IT,
                                     med_2_3::MED_CELL, (med_2_3::med_geometry_type)type,
                                     array+offset );
      offset+=ntype;
    }
    cellglobal[idomain]=array;
  }

  MESSAGE_MED("Reading node global numbering");
  int nnode= (_collection->getMesh())[idomain]->getNumberOfNodes();
  {
    int* array=new int[nnode];
    med_2_3::MEDmeshGlobalNumberRd(fid,meshname,
                                   MED_NO_DT, MED_NO_IT,
                                   med_2_3::MED_NODE, MED_POINT1,
                                   array);
    nodeglobal[idomain]=array;
  }

  MESSAGE_MED("Reading face global numbering for mesh "<<idomain);
  MED_EN::medEntityMesh entity =
    (mesh->getMeshDimension()==3)?MED_EN::MED_FACE:MED_EN::MED_EDGE;
  int nbface=(_collection->getMesh())[idomain]->getNumberOfElements(entity,MED_EN::MED_ALL_ELEMENTS);
  if (nbface!=0)
  {
    int* array=new int[nbface];
    int offset=0;
    int nbtypes = mesh->getNumberOfTypes( entity );
    const MED_EN::medGeometryElement* types =mesh->getTypes(entity);

    for (int itype=0; itype< nbtypes; itype++)
    {
      MED_EN::medGeometryElement type = types[itype];
      if (!_collection->isDimensionOK(type,mesh->getMeshDimension()-1)) continue;

      int ntype = mesh->getNumberOfElements(entity,type);
      if (ntype==0) continue;
      med_2_3::MEDmeshGlobalNumberRd( fid, meshname,
                                      MED_NO_DT, MED_NO_IT,
                                      med_2_3::MED_CELL, (med_2_3::med_geometry_type)type,
                                      array+offset );
      offset+=ntype;
    }
    faceglobal[idomain]=array;
  }
  med_2_3::MEDfileClose(fid);

  END_OF_MED(LOC);
}

void MESHCollectionDriver::writeSubdomain(int idomain, int nbdomains, char* distfilename,
                                          ParaDomainSelector* /*domain_selector*/)
{
  MESSAGE_MED(" Number of connect zones "<<(_collection->getCZ()).size());

  //writing connect zones in joints

  med_2_3::med_idt fid = med_2_3::MEDfileOpen(distfilename,med_2_3::MED_ACC_RDWR);

  int index_joint=0;


  for (unsigned icz=0; icz<(_collection->getCZ()).size(); icz++)
  {
    if ((_collection->getCZ())[icz]->getLocalDomainNumber()==idomain)
    {
      med_2_3::med_err error;
      int idistant=(_collection->getCZ())[icz]->getDistantDomainNumber();
      char joint_name[MED_NAME_SIZE+1];
      sprintf(joint_name,"joint_%i",idistant+1);
      char desc[MED_COMMENT_SIZE+1];
      sprintf(desc,"connect_zone_%d",icz+1);

      char distant_name[MED_NAME_SIZE+1];
      //sprintf(distant_name,"domain_%i",(_collection->getCZ())[icz]->getDistantDomainNumber());

      //        sprintf(distant_name,(_collection->getMesh())[idistant]->getName().c_str());
      sprintf(distant_name,"domain_%i",idistant);
      char mesh_name[MED_NAME_SIZE+1];

      strcpy (mesh_name, _collection->getMesh(idomain)->getName().c_str());
      SCRUTE_MED(_collection->getMesh(idomain)->getName());
      error = med_2_3::MEDsubdomainJointCr(fid,mesh_name, joint_name, desc, 
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
      if (_collection->getSubdomainBoundaryCreates())
      {
        if (idomain<idistant)
          jointSort(node_corresp, nbnodes, true);
        else
          jointSort(node_corresp, nbnodes, false);
      }
      error=
        med_2_3::MEDsubdomainCorrespondenceWr(fid, mesh_name, joint_name,
                                              MED_NO_DT, MED_NO_IT,
                                              med_2_3::MED_NODE, MED_POINT1,
                                              med_2_3::MED_NODE, MED_POINT1,
                                              nbnodes, node_corresp);
      if (error==-1) cout << "erreur creation de joint "<<endl;

      //writing cell/cell joint      
      writeElementJoint(MED_EN::MED_CELL, icz, idomain, idistant, mesh_name,joint_name,fid);
      //writing face/face joint
      if (_collection->getSubdomainBoundaryCreates())
      {
        MED_EN::medEntityMesh constituent_entity =
          (_collection->getMeshDimension()==3)?MED_EN::MED_FACE:MED_EN::MED_EDGE;
        writeElementJoint(constituent_entity, icz, idomain, idistant, mesh_name,joint_name,fid);                 
      }                   
      index_joint++;
    }
  }

  char meshchar[MED_NAME_SIZE+1];
  strcpy(meshchar,(_collection->getMesh())[idomain]->getName().c_str());

  // Writing cell global numbering
  // 
  {
    int ncell=_collection->getTopology()->getCellNumber(idomain);
    int* array=new int[ncell];
    _collection->getTopology()->getCellList(idomain,array);
    int offset=0;

    MED_EN::MESH_ENTITIES::const_iterator currentEntity;
    list<MED_EN::medGeometryElement>::const_iterator iter;
    currentEntity  = MED_EN::meshEntities.find(MED_EN::MED_CELL);

    int nbtypes = (_collection->getMesh())[idomain]->getNumberOfTypes(MED_EN::MED_CELL);
    const MED_EN::medGeometryElement* types =(_collection->getMesh())[idomain]->getTypes(MED_EN::MED_CELL);
    for (int itype=0; itype<nbtypes;itype++)
    {
      MED_EN::medGeometryElement type=types[itype];
      if (!_collection->isDimensionOK(type,(_collection->getMesh())[idomain]->getMeshDimension()))
        continue;
      int ntype = (_collection->getMesh())[idomain]->getNumberOfElements(MED_EN::MED_CELL,type);
      if (ntype==0) continue;
      med_2_3::MEDmeshGlobalNumberWr(fid,meshchar,
                                     MED_NO_DT, MED_NO_IT,
                                     med_2_3::MED_CELL, (med_2_3::med_geometry_type)type,
                                     ntype, array+offset);
      offset+=ntype;
    }
    delete[] array;
  }

  MED_EN::medEntityMesh constituent_entity;
  if (_collection->getMeshDimension()==3)
    constituent_entity=MED_EN::MED_FACE;
  else if (_collection->getMeshDimension()==2)
    constituent_entity=MED_EN::MED_EDGE;
  else throw MEDEXCEPTION("Wrong dimension");


  //writing face global numbering
  {
    int offset=0;
    int nface= _collection->getTopology()->getFaceNumber(idomain);
    int * array=new int[nface];
    _collection->getTopology()->getFaceList(idomain,array);

    int nbfacetypes = 0;
    const MED_EN::medGeometryElement* facetypes = 0;
    if ( _collection->getMesh()[idomain]->getNumberOfElements( constituent_entity, MED_ALL_ELEMENTS))
      {
        nbfacetypes = (_collection->getMesh())[idomain]->getNumberOfTypes(constituent_entity);
        if (nbfacetypes>0)
          facetypes =(_collection->getMesh())[idomain]->getTypes(constituent_entity);
      }
    for (int itype=0; itype<nbfacetypes;itype++)
      {
        MED_EN::medGeometryElement type=facetypes[itype];
        if (!_collection->isDimensionOK(type,_collection->getMeshDimension()-1)) continue;

        int ntype = (_collection->getMesh())[idomain]->getNumberOfElements(constituent_entity,type);
        if (ntype==0) continue;
        med_2_3::MEDmeshGlobalNumberWr(fid,meshchar,
                                       MED_NO_DT, MED_NO_IT,
                                       med_2_3::MED_CELL, (med_2_3::med_geometry_type)type,
                                       ntype, array+offset );
        offset+=ntype;
      }
    delete[] array;
  }


  //writing node global numbering
  {
    int nnode=_collection->getTopology()->getNodeNumber(idomain);
    int* array = new int[nnode];
    _collection->getTopology()->getNodeList(idomain,array);

    med_2_3::MEDmeshGlobalNumberWr(fid,meshchar,
                                   MED_NO_DT, MED_NO_IT,
                                   med_2_3::MED_NODE, MED_POINT1,
                                   nnode, array);
    delete[] array;
  }

  med_2_3::MEDfileClose(fid);
  MESSAGE_MED("End of writing");

}

void MESHCollectionDriver::writeElementJoint(medEntityMesh entity ,
                                             int icz, 
                                             int idomain, 
                                             int idistant, 
                                             char* mesh_name, 
                                             char* joint_name,  
                                             med_2_3::med_idt fid )
{
  //////////////////////////////////////////
  //writing cell/cell correspondency
  //////////////////////////////////////////
  int nbcells=(_collection->getCZ())[icz]->getEntityCorrespNumber(entity,entity);
  const int* index = (_collection->getCZ())[icz]->getEntityCorrespIndex(entity,entity);
  const int* value = (_collection->getCZ())[icz]->getEntityCorrespValue(entity,entity);
  if ( nbcells==0 ) return; // e.g. domains have 1 common node

  map <pair <MED_EN::medGeometryElement, MED_EN::medGeometryElement> , vector<int> > cellmap;
  map <MED_EN::medGeometryElement, int> local_offset;
  map <MED_EN::medGeometryElement, int> distant_offset;

  //definition of the local offsets for the types present on local
  //and distant domains
  // for a mesh containing 2 triangles and 3 quads
  //local_offset[TRIA3]=0
  //local_offset[QUAD4]=2

  int nb_types_local=(_collection->getMesh())[idomain]-> getNumberOfTypes(entity);
  const MED_EN::medGeometryElement* local_types = (_collection->getMesh())[idomain]->getTypes(entity);
  const int* local_gni = (_collection->getMesh())[idomain]-> getGlobalNumberingIndex(entity);
  for (int i=0; i< nb_types_local; i++)
  {
    local_offset[local_types[i]]=local_gni[i]-1;
  }                                      

  int nb_types_distant=(_collection->getMesh())[idistant]-> getNumberOfTypes(entity);
  const MED_EN::medGeometryElement* distant_types = (_collection->getMesh())[idistant]->getTypes(entity);
  const int* distant_gni = (_collection->getMesh())[idistant]-> getGlobalNumberingIndex(entity);
  for (int i=0; i< nb_types_distant; i++)
  {
    distant_offset[distant_types[i]]=distant_gni[i]-1;
  } 

  if (nb_types_local==1 && nb_types_distant==1)
  {
    MED_EN::medGeometryElement local_type =  (_collection->getMesh())[idomain]->getElementType(entity,1);
    MED_EN::medGeometryElement distant_type = (_collection->getMesh())[idistant]->getElementType(entity,1);
    vector<int> corresp;
    for (int i=0; i<nbcells; i++)
      for (int icol = index[i]-1; icol<index[i+1]-1; icol++)
      {
        corresp.push_back(i+1);
        corresp.push_back(value[icol]);
      }
    int size_joint = corresp.size()/2;
    med_2_3::MEDsubdomainCorrespondenceWr(fid, mesh_name, joint_name,
                                          MED_NO_DT, MED_NO_IT,
                                          med_2_3::MED_CELL,(med_2_3::med_geometry_type)local_type,
                                          med_2_3::MED_CELL,(med_2_3::med_geometry_type)distant_type,
                                          size_joint,&corresp[0]);
  }
  else
  {
    //classifying all the cell/cell relationships into geomtype/geomtype relationships
    //there exists a vector for each geomtype/geomtype pair
    // the vectors are stored in cellmap, a std::map with a pair<geomtype,geomtype> key 

    for (int i=0; i<nbcells; i++)
      for (int icol = index[i]-1; icol<index[i+1]-1; icol++)
      {
        MED_EN::medGeometryElement local_type =  (_collection->getMesh())[idomain]->getElementType(entity,i+1);
        MED_EN::medGeometryElement distant_type = (_collection->getMesh())[idistant]->getElementType(entity,value[icol]);

        cellmap[make_pair(local_type, distant_type)].push_back(i+1-local_offset[local_type]);
        cellmap[make_pair(local_type, distant_type)].push_back(value[icol]-distant_offset[distant_type]);

      }
    map <pair <MED_EN::medGeometryElement, MED_EN::medGeometryElement> , vector<int> >::const_iterator iter;

    //going through all the (geom,geom) pairs and writing the joints
    for (iter= cellmap.begin(); iter != cellmap.end(); iter++)
    {
      int size= iter->second.size();
      int *corresp = new int[size];
      for (int ind=0; ind < size; ind++)
        corresp[ind]=(iter->second)[ind];
      med_2_3::med_geometry_type local_geo_elem=(med_2_3::med_geometry_type)iter->first.first;
      med_2_3::med_geometry_type distant_geo_elem=(med_2_3::med_geometry_type)iter->first.second;
      int size_joint=size/2;
      //med_2_3::med_err error =
      med_2_3::MEDsubdomainCorrespondenceWr(fid, mesh_name, joint_name,
                                            MED_NO_DT, MED_NO_IT,
                                            med_2_3::MED_CELL, local_geo_elem,
                                            med_2_3::MED_CELL, distant_geo_elem,
                                            size_joint, corresp);
      delete[] corresp;
    }
    // MED v 2.3.1 returns an error code when
    // writing a joint that is already present in the file.
    // Also, it returns an error code if a joint
    // concerns 3D elements.
    // Until these two items are not
    // changed, the following line must be commented out

    //if (error==-1) throw MEDEXCEPTION("Error filling joint");


  }
}

void MESHCollectionDriver::jointSort(int* elems, int nbelems, bool is_first)
{
  //filling an ordered structure with the elem ids
  map <int,int> nodemap;
  if (is_first)
    for (int i=0; i<nbelems; i++) 
      nodemap.insert(make_pair(elems[2*i],elems[2*i+1]));

  else
    for (int i=0; i<nbelems; i++)
      nodemap.insert(make_pair(elems[2*i+1],elems[2*i]));

  int* ptr_elems=elems;      

  //filling the vector in appropriate order 
  for (map<int,int>::const_iterator iter=nodemap.begin(); iter!=nodemap.end(); iter++)
  {
    if (is_first)
    {
      *ptr_elems++=iter->first;
      *ptr_elems++=iter->second;
    }
    else
    {
      *ptr_elems++=iter->second;
      *ptr_elems++=iter->first;
    }
  }     
}
