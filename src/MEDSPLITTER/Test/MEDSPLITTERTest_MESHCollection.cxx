// Copyright (C) 2006  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com

#include "MEDSPLITTERTest.hxx"
#include <cppunit/TestAssert.h>

#include "MEDMEM_ConnectZone.hxx"
#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_GaussLocalization.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_Group.hxx"

#include "MEDSPLITTER_Graph.hxx"
#include "MEDSPLITTER_Topology.hxx"
#include "MEDSPLITTER_ParallelTopology.hxx"
#include "MEDSPLITTER_SequentialTopology.hxx"
#include "MEDSPLITTER_METISGraph.hxx"
#include "MEDSPLITTER_SCOTCHGraph.hxx"
#include "MEDSPLITTER_MESHCollection.hxx"
#include "MEDSPLITTER_MESHCollectionDriver.hxx"

#include "MEDMEM_Exception.hxx"

#include <string>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
#define ENABLE_FORCED_FAILURES


using namespace std;
using namespace MEDSPLITTER;
using namespace MEDMEM;
/*!
 *  Check methods (33), defined in MEDSPLITTER_MESHCollection.hxx:
 * (-) MESHCollection();
 * (-) MESHCollection(const MESHCollection&, Topology*);
 * (+) MESHCollection(const std::string& filename);
 * (+) MESHCollection(const std::string& filename, const std::string& meshname);
 * (-) ~MESHCollection();
 * (+) void write(const std::string& filename);
 * (-) MESHCollectionDriver* retrieveDriver();
 * (-) MESHCollectionDriver* getDriver() const;
 * (-) void buildCellGraph(MEDMEM::MEDSKYLINEARRAY* & array,int *& edgeweights );
 * (-) void buildCellGraph2ndversion(MEDMEM::MEDSKYLINEARRAY* & array,int *& edgeweights );
 * (+) Topology* createPartition(int nbdomain, Graph::splitter_type type = Graph::METIS,
 const string& ="", int* edgeweights=0, int* verticesweights=0);
 * (-) Topology* createPartition(const int* partition);
 * (-) void getTypeList(int* cell_list,int nb_cells,MED_EN::medEntityMesh entity,
 MED_EN::medGeometryElement* type_list) const ;
 * (-) void getCoordinates(int* node_list,int nb_nodes, double* coordinates) const ;
 * (-) void getNodeConnectivity( const int*  cell_list,int nb_cells,MED_EN::medEntityMesh,MED_EN::medGeometryElement type, int* type_connectivity) const ;
 * (-) void getFaceConnectivity( const int*  cell_list,int nb_cells,MED_EN::medEntityMesh,MED_EN::medGeometryElement type, int* type_connectivity) const ;
 * (+) int getMeshDimension() const ;
 * (+) int getSpaceDimension() const ;
 * (+) std::string getSystem() const;
 * (-) std::string getMeshName() const;
 * (-) std::vector<MEDMEM::MESH*>& getMesh() ;
 * (-) MEDMEM::MESH* getMesh(int) const;
 * (-) std::vector<MEDMEM::CONNECTZONE*>& getCZ();
 * (-) Topology* getTopology() const ;
 * (-) void setTopology(Topology* topology);
 * (+) string getName() const {return m_name;}
 * (+) void setName(const string& name) {m_name=name;}
 * (-) void castFamilies(const MESHCollection& old_collection);
 * (-) void castSupport(const MESHCollection& old_collection, std::vector<const MEDMEM::SUPPORT*> old_support, std::vector<MEDMEM::SUPPORT*> new_support);
 * (-) void castAllFields(const MESHCollection& old_collection);
 * (-) void castField(const MESHCollection& old_collection, const string& fieldname, int itnumber, int ordernumber);
 * (+) void setIndivisibleGroup(const string& a);
 * (-) void buildConnectZones(int idomain);
 */
 
 
void MEDSPLITTERTest::testMESHCollection_constructor()
{
  string data_dir                   = getenv("DATA_DIR");
  string filename_rd                = data_dir + "/MedFiles/carre_en_quad4_import22.med";
  string fileNotExist_rd            = "notExist22.med";
  string emptyfilename              = "";


  //--------------------------Test READ ONLY part------------------------------//

  /////////////////////////////////////////////////////////////
		//  TEST1a: Open not existing sequential and parallel file  //
		/////////////////////////////////////////////////////////////
    string meshname="non_existent";
    
    CPPUNIT_ASSERT_THROW(MESHCollection mc1(fileNotExist_rd, meshname), MEDEXCEPTION);
    
    CPPUNIT_ASSERT_THROW(MESHCollection mc2(fileNotExist_rd), MEDEXCEPTION);
    
    CPPUNIT_ASSERT_THROW(MESHCollection mc3(filename_rd, meshname), MEDEXCEPTION);

		//  //////////////////////////////////////////////
		//  //  TEST1b: Open file with empty file name  ///
		//  //////////////////////////////////////////////
		//
    CPPUNIT_ASSERT_THROW(MESHCollection mc4(emptyfilename, meshname), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(MESHCollection mc5(emptyfilename), MEDEXCEPTION);

}
 
 
//! Testing basic manipulation for a sequential file 
void MEDSPLITTERTest::testMESHCollection_read_seq()
{
  string data_dir                   = getenv("DATA_DIR");
  string tmp_dir                    = getenv("TMP");
  if (tmp_dir == "")
    tmp_dir = "/tmp";
  string filename_rd                = data_dir + "/MedFiles/pointe_import22.med";
  string filename_seq_wr            = tmp_dir + "/myWrField_seq_pointe22";
  string filename_seq_med           = tmp_dir + "/myWrField_seq_pointe221.med";
  
  // To remove tmp files from disk
  MEDSPLITTERTest_TmpFilesRemover aRemover;
  aRemover.Register(filename_seq_wr);
  aRemover.Register(filename_seq_med);
  
	//--------------------------Test READ ONLY part------------------------------//

	//  /////////////////////////////////////////////
	//  //  TEST2: Manipulation of a single MED file
	//  /////////////////////////////////////////////
  string meshname="maa1";
  MESHCollection collection(filename_rd,meshname);
  CPPUNIT_ASSERT_EQUAL(meshname,collection.getName());
  string newname = "New_name";
  collection.setName(newname);
  CPPUNIT_ASSERT_EQUAL(newname,collection.getName());
  string systemname="CARTESIAN";
  CPPUNIT_ASSERT_EQUAL(systemname,collection.getSystem());
  CPPUNIT_ASSERT_EQUAL(3,collection.getMeshDimension());
  CPPUNIT_ASSERT_EQUAL(3,collection.getSpaceDimension());
  
  collection.write(filename_seq_wr);
  
  //Reading from the master file
  MESHCollection collection2(filename_seq_wr);
  CPPUNIT_ASSERT_EQUAL(collection.getName(),collection2.getName());
  
  //Reading from the med file
  MESHCollection collection3(filename_seq_med,meshname);
  CPPUNIT_ASSERT_EQUAL(meshname,collection3.getName());
    
}

//! Testing basic manipulation for a parallel file 

void MEDSPLITTERTest::testMESHCollection_read_para()
{
  string data_dir                   = getenv("DATA_DIR");
  string tmp_dir                    = getenv("TMP");
  if (tmp_dir == "")
    tmp_dir = "/tmp";
  string filename_rd                = data_dir + "/MedFiles/pointe_import22.med";  
  string filename_para_wr            = tmp_dir + "/myWrField_seq_pointe22_";
  string filename_para_med0           = tmp_dir + "/myWrField_seq_pointe22_1.med";
  string filename_para_med1           = tmp_dir + "/myWrField_seq_pointe22_2.med";
  
  
  // To remove tmp files from disk
  MEDSPLITTERTest_TmpFilesRemover aRemover;
  aRemover.Register(filename_para_wr);
  aRemover.Register(filename_para_med0);
  aRemover.Register(filename_para_med1);
  
  string meshname="maa1";
  MESHCollection collection(filename_rd,meshname);
  Topology* topo=0;
  CPPUNIT_ASSERT_THROW(topo=collection.createPartition(0,Graph::METIS),MEDEXCEPTION);
  if (topo!=0) delete topo;
  topo=collection.createPartition(2,Graph::METIS);
  MESHCollection new_collection(collection, topo);
  new_collection.write(filename_para_wr);
    
  CPPUNIT_ASSERT_EQUAL(meshname,new_collection.getName());
  MESHCollection new_collection2(filename_para_wr);
  CPPUNIT_ASSERT_EQUAL(collection.getName(),new_collection2.getName());
  
  MESHCollection new_collection3(filename_para_med0,"maa1_1");
  CPPUNIT_ASSERT_EQUAL(collection.getName()+"_1",new_collection3.getName());
  delete topo;
}

//!Testing a normal scheme

/*
 * test_SPLITTER_square
 * 
 * computes a partitioning for the following geometry
 * 
 * 
 * 
 * 7------------8------------9
 * |            |            |
 * |            |            |
 * |     3      |     4      |
 * |            |            |
 * |            |            |
 * 4------------5------------6
 * |            |            |
 * |            |            |
 * |     1      |     2      |
 * |            |            |
 * |            |            |
 * 1------------2------------3 
 *
 * Result of the 2 domain split :
 *  
 * 5------------6 5------------6
 * |            | |            |
 * |            | |            |
 * |     2      | |     2      |
 * |            | |            |
 * |            | |            |
 * 1------------2 1------------2
 * |            | |            |
 * |            | |            |
 * |     1      | |     1      |
 * |            | |            |
 * |            | |            |
 * 4------------3 4------------3 
 */


void MEDSPLITTERTest::testMESHCollection_square()
{
  string data_dir                   = getenv("DATA_DIR");
  string tmp_dir                    = getenv("TMP");
  if (tmp_dir == "")
    tmp_dir = "/tmp";
  string filename_rd                = data_dir + "/MedFiles/carre_en_quad4_import22.med";
  string filename_wr                 = tmp_dir+"/carre_split";
  string filename_wr_1                = tmp_dir+"/carre_split1.med";
  string filename_wr_2                 = tmp_dir+"/carre_split2.med";
  
  // To remove tmp files from disk
  MEDSPLITTERTest_TmpFilesRemover aRemover;
  aRemover.Register(filename_wr);
  aRemover.Register(filename_wr_1);
  aRemover.Register(filename_wr_2);
  
  
  char meshname[20]  = "carre_en_quad4";
  char meshname1[20]  = "carre_en_quad4_1";
  char meshname2[20]  = "carre_en_quad4_2";
  
  
  MESHCollection collection(filename_rd,meshname);
  MEDSPLITTER::Topology* topo= collection.createPartition(2,Graph::METIS);
  MESHCollection new_collection(collection, topo);
    
  //collection.write("/export/home/test_splitter");
  new_collection.write(filename_wr);
  new_collection.castAllFields(collection);
    
  MEDMEM::MESH mesh1(MEDMEM::MED_DRIVER, filename_wr_1, meshname1);
  MEDMEM::MESH mesh2(MEDMEM::MED_DRIVER, filename_wr_2, meshname2);
    
  // testing number of elements for each partition
  int nbelem1=mesh1.getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
  int nbelem2=mesh2.getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
    
  CPPUNIT_ASSERT_EQUAL(nbelem1,2);
  CPPUNIT_ASSERT_EQUAL(nbelem2,2);
  
  
  //testing number of joints
  med_2_2::med_idt fid1 = med_2_2::MEDouvrir(const_cast<char*> (filename_wr_1.c_str()),med_2_2::MED_LECTURE);
  med_2_2::med_idt fid2 = med_2_2::MEDouvrir(const_cast<char*> (filename_wr_2.c_str()),med_2_2::MED_LECTURE);
  int nj1= med_2_2::MEDnJoint(fid1, meshname1);
  int nj2= med_2_2::MEDnJoint(fid2, meshname2);
  CPPUNIT_ASSERT_EQUAL(nj1,1);
  CPPUNIT_ASSERT_EQUAL(nj2,1);
       
  //testing distant domains
    
  char desc1[MED_TAILLE_DESC];
  char maa_dist1[MED_TAILLE_NOM], jn1[MED_TAILLE_NOM];
  char desc2[MED_TAILLE_DESC], maa_dist2[MED_TAILLE_NOM], jn2[MED_TAILLE_NOM];
  int dom1, dom2;
  med_2_2::MEDjointInfo(fid1, meshname1, 1, jn1, desc1, &dom1, maa_dist1);
  med_2_2::MEDjointInfo(fid2, meshname2, 1, jn2, desc2, &dom2, maa_dist2);
  CPPUNIT_ASSERT_EQUAL(dom1,1);
  CPPUNIT_ASSERT_EQUAL(dom2,0);
  
  // testing node-node correspondency
  med_2_2::med_entite_maillage typ_ent_loc=med_2_2::MED_NOEUD;
  med_2_2::med_entite_maillage typ_ent_dist=med_2_2::MED_NOEUD;
  med_2_2::med_geometrie_element typ_geo_loc= med_2_2::MED_POINT1;
  med_2_2::med_geometrie_element typ_geo_dist= med_2_2::MED_POINT1;
    
  int n1 =med_2_2::MEDjointnCorres(fid1,meshname1,jn1,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
  int* tab=new int[2*n1];
   
  med_2_2::MEDjointLire(fid1,meshname1,jn1,tab,n1,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
    
  int tabreference1[6]={2,1,3,4,6,5};
  for (int i=0; i<2*n1; i++)
		CPPUNIT_ASSERT_EQUAL(tab[i],tabreference1[i]);
    
  delete[] tab;

  int n2 =med_2_2::MEDjointnCorres(fid2,meshname2,jn2,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
  tab=new int[2*n2];
    
  med_2_2::MEDjointLire(fid2,meshname2,jn2,tab,n2,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
    
  int tabreference2[]={1,2,4,3,5,6};
  for (int i=0; i<2*n1; i++)
		CPPUNIT_ASSERT_EQUAL(tab[i],tabreference2[i]);
  delete[] tab;

  //testing nodes global numbering
  int* num=new int[6];
  cout <<"Reading global "<<
		MEDglobalNumLire(fid1, meshname1, num, 6, typ_ent_loc, typ_geo_loc)<<endl;
  
  int globnoderef1[]={4,5,2,1,7,8};
    
  for (int i=0; i<6; i++)
		CPPUNIT_ASSERT_EQUAL(num[i],globnoderef1[i]);
  delete[] num;
    
  //testing nodes global numbering
  num=new int[6];
  MEDglobalNumLire(fid2, meshname2, num, 6, typ_ent_loc, typ_geo_loc);
  int globnoderef2[]={5,6,3,2,8,9};
  for (int i=0; i<6; i++)
    CPPUNIT_ASSERT_EQUAL(num[i],globnoderef2[i]);
  delete[] num;
  
  //testing cell-cell correspondency
  typ_ent_loc=med_2_2::MED_MAILLE;
  typ_ent_dist=med_2_2::MED_MAILLE;
  typ_geo_loc= med_2_2::MED_QUAD4;
  typ_geo_dist= med_2_2::MED_QUAD4;
  //joint1
  n1=med_2_2::MEDjointnCorres(fid1,meshname1,jn1,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
  tab=new int[2*n1];
  med_2_2::MEDjointLire(fid1,meshname1,jn1,tab,n1,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
    
  int tabreferencecell1[4]={1,1,2,2};
  for (int i=0; i<2*n1; i++)
    CPPUNIT_ASSERT_EQUAL(tab[i],tabreferencecell1[i]);
            
  n2=med_2_2::MEDjointnCorres(fid2,meshname2,jn1,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
  delete[] tab;
  //joint2
  tab=new int[2*n2];
  med_2_2::MEDjointLire(fid2,meshname2,jn2,tab,n2,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
    
  int tabreferencecell2[4]={1,1,2,2};
  for (int i=0; i<n2; i++)
    CPPUNIT_ASSERT_EQUAL(tab[i],tabreferencecell2[i]);
            
  delete[]tab;
        
  //testing cells global numbering
  num=new int[2];
  MEDglobalNumLire(fid1, meshname1, num, 2, typ_ent_loc, typ_geo_loc);
  int* globcellref = new int[2];
  globcellref[0]=1;
  globcellref[1]=3;
  
  for (int i=0; i<2; i++)
    CPPUNIT_ASSERT_EQUAL(num[i],globcellref[i]);
  delete[] num;
    
  //testing cells global numbering
  num=new int[2];
  MEDglobalNumLire(fid2, meshname2, num, 2, typ_ent_loc, typ_geo_loc);
  globcellref[0]=2;
  globcellref[1]=4;
  for (int i=0; i<2; i++)
    CPPUNIT_ASSERT_EQUAL(num[i],globcellref[i]);
  delete[] num;
  delete[]globcellref;
  delete topo;   
      
}

/*
 * testing indivisibility functionality
 * 
 * computes a partitioning for the a geometry
 * with 8 TRIA3 and 4 QUAD4. A group containing the quads
 * is set to be indivisible. 
 * It is checked that on output, there is only one group 
 * containing quads.
 * SCOTCH is used for this partitioning.
 * */

void MEDSPLITTERTest::testMESHCollection_indivisible()
{
  string data_dir                   = getenv("DATA_DIR");
  string tmp_dir                    = getenv("TMP");
  if (tmp_dir == "")
    tmp_dir = "/tmp";
  string filename_rd                = data_dir + "/MedFiles/maill.00.med";
  string filename_wr                = tmp_dir + "/carre_indivisible";
  string filename_wr_1              = tmp_dir + "/carre_indivisible1.med";
  string filename_wr_2              = tmp_dir + "/carre_indivisible2.med";
  string filename_wr_3              = tmp_dir + "/carre_indivisible3.med";
  string filename_wr_4              = tmp_dir + "/carre_indivisible4.med";
  
	// To remove tmp files from disk
  MEDSPLITTERTest_TmpFilesRemover aRemover;
  aRemover.Register(filename_wr);
  aRemover.Register(filename_wr_1);
  aRemover.Register(filename_wr_2);
  aRemover.Register(filename_wr_3);
  aRemover.Register(filename_wr_4);
  
  
  char meshname[20]  = "MAILTRQU";
  char family[MED_TAILLE_NOM]="QUAD";
  MESHCollection collection(filename_rd,meshname);
  collection.setIndivisibleGroup(family);
  Topology* topo =  collection.createPartition(4,Graph::SCOTCH);
  MESHCollection new_collection(collection, topo);
  new_collection.write(filename_wr);
   
  char meshname1[20]="MAILTRQU_1";
  char meshname2[20]="MAILTRQU_2";
  char meshname3[20]="MAILTRQU_3";
  char meshname4[20]="MAILTRQU_4";
    
  MEDMEM::MESH mesh1(MEDMEM::MED_DRIVER, filename_wr_1, meshname1);
  MEDMEM::MESH mesh2(MEDMEM::MED_DRIVER, filename_wr_2, meshname2);
  MEDMEM::MESH mesh3(MEDMEM::MED_DRIVER, filename_wr_3, meshname3);
  MEDMEM::MESH mesh4(MEDMEM::MED_DRIVER, filename_wr_4, meshname4);
      
  // testing number of quads for each partition
  int nbquad1=    mesh1.getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_QUAD4);
  int nbquad2=    mesh2.getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_QUAD4);
  int nbquad3=    mesh3.getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_QUAD4);
  int nbquad4=    mesh4.getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_QUAD4);
  int nb_domain_with_quad=(nbquad1?1:0)+(nbquad2?1:0)+(nbquad3?1:0)+(nbquad4?1:0);

  CPPUNIT_ASSERT_EQUAL(nb_domain_with_quad,1);
  delete topo;  
}
  
/*
 * test_SPLITTER_user_partition
 * 
 * computes joints for the following geometry
 * with user specified partition
 * domain0: (1,4)/ domain1:(2,3)
 * 
 * 
 * 7------------8------------9
 * |            |            |
 * |            |            |
 * |     3      |     4      |
 * |            |            |
 * |            |            |
 * 4------------5------------6
 * |            |            |
 * |            |            |
 * |     1      |     2      |
 * |            |            |
 * |            |            |
 * 1------------2------------3 
 *
 * Result of the 2 domain split :
 *  
 * 5------------6 5------------6
 * |            | |            |
 * |            | |            |
 * |     2      | |     2      |
 * |            | |            |
 * |            | |            |
 * 7------------1 2------------7
 * 1------------2 1------------2
 * |            | |            |
 * |            | |            |
 * |     1      | |     1      |
 * |            | |            |
 * |            | |            |
 * 4------------3 4------------3 
 */


void MEDSPLITTERTest::testMESHCollection_user_partition()
{
  string data_dir                   = getenv("DATA_DIR");
  string tmp_dir                    = getenv("TMP");
  if (tmp_dir == "")
    tmp_dir = "/tmp";
  string filename_rd                = data_dir + "/MedFiles/carre_en_quad4_import22.med";
  string filename_wr                 = tmp_dir+"/carre_split_user";
  string filename_wr_1                = tmp_dir+"/carre_split_user1.med";
  string filename_wr_2                 = tmp_dir+"/carre_split_user2.med";
  
  // To remove tmp files from disk
  MEDSPLITTERTest_TmpFilesRemover aRemover;
  aRemover.Register(filename_wr);
  aRemover.Register(filename_wr_1);
  aRemover.Register(filename_wr_2);
  
  
  char meshname[20]  = "carre_en_quad4";
  char meshname1[20]  = "carre_en_quad4_1";
  char meshname2[20]  = "carre_en_quad4_2";
  
	//int partition[4]={0,1,1,0};
  int* partition=new int[4];
  partition[0]=0;
  partition[1]=1;
  partition[2]=1;
  partition[3]=0;
	//    
  MESHCollection collection(filename_rd,meshname);
  Topology* topo = collection.createPartition(partition); 
  MESHCollection new_collection(collection, topo);
    
  //collection.write("/export/home/test_splitter");
  new_collection.write(filename_wr);
  new_collection.castAllFields(collection);
    
    
  MEDMEM::MESH mesh1(MEDMEM::MED_DRIVER, filename_wr_1, meshname1);
  MEDMEM::MESH mesh2(MEDMEM::MED_DRIVER, filename_wr_2, meshname2);
    
    
  // testing number of elements for each partition
  
  int nbelem1=mesh1.getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
  int nbelem2=mesh2.getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
  CPPUNIT_ASSERT_EQUAL(nbelem1,2);
  CPPUNIT_ASSERT_EQUAL(nbelem2,2);
    
  //testing number of joints
  med_2_2::med_idt fid1 = med_2_2::MEDouvrir(const_cast<char*>(filename_wr_1.c_str()),med_2_2::MED_LECTURE);
  med_2_2::med_idt fid2 = med_2_2::MEDouvrir(const_cast<char*>(filename_wr_2.c_str()),med_2_2::MED_LECTURE);
  
  int nj1= med_2_2::MEDnJoint(fid1, meshname1);
  int nj2= med_2_2::MEDnJoint(fid2, meshname2);
  CPPUNIT_ASSERT_EQUAL(nj1,1);
  CPPUNIT_ASSERT_EQUAL(nj2,1);
  
    
	//testing distant domains
    
  char desc1[MED_TAILLE_DESC];
  char maa_dist1[MED_TAILLE_NOM], jn1[MED_TAILLE_NOM];
  char desc2[MED_TAILLE_DESC], maa_dist2[MED_TAILLE_NOM], jn2[MED_TAILLE_NOM];
  int dom1, dom2;
  med_2_2::MEDjointInfo(fid1, meshname1, 1, jn1, desc1, &dom1, maa_dist1);
  med_2_2::MEDjointInfo(fid2, meshname2, 1, jn2, desc2, &dom2, maa_dist2);
  CPPUNIT_ASSERT_EQUAL(dom1,1);
  CPPUNIT_ASSERT_EQUAL(dom2,0);
  
  // testing node-node correspondency
  med_2_2::med_entite_maillage typ_ent_loc=med_2_2::MED_NOEUD;
  med_2_2::med_entite_maillage typ_ent_dist=med_2_2::MED_NOEUD;
  med_2_2::med_geometrie_element typ_geo_loc= med_2_2::MED_POINT1;
  med_2_2::med_geometrie_element typ_geo_dist= med_2_2::MED_POINT1;
  
  int n1 =med_2_2::MEDjointnCorres(fid1,meshname1,jn1,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
  int* tab=new int[2*n1];
  
  med_2_2::MEDjointLire(fid1,meshname1,jn1,tab,n1,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
  
  int tabreference1[10]={1,7,2,1,3,4,5,6,7,2};
  for (int i=0; i<2*n1; i++)
    CPPUNIT_ASSERT_EQUAL(tab[i],tabreference1[i]);
  delete[] tab;

  int n2 =med_2_2::MEDjointnCorres(fid2,meshname2,jn2,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
  tab=new int[2*n2];
   
  med_2_2::MEDjointLire(fid2,meshname2,jn2,tab,n2,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
    
  int tabreference2[10]={1,2,2,7,4,3,6,5,7,1};
  for (int i=0; i<2*n2; i++)
		CPPUNIT_ASSERT_EQUAL(tab[i],tabreference2[i]);
  delete[] tab;
	//testing nodes global numbering
  int* num=new int[7];
  cout <<"Reading global "<<
		MEDglobalNumLire(fid1, meshname1, num, 7, typ_ent_loc, typ_geo_loc)<<endl;
  int globnoderef1[7]={4,5,2,1,8,9,6};
    
  for (int i=0; i<7; i++)
		CPPUNIT_ASSERT_EQUAL(num[i],globnoderef1[i]);
  
  delete[] num;
    
  //testing nodes global numbering
  num=new int[7];
  MEDglobalNumLire(fid2, meshname2, num, 7, typ_ent_loc, typ_geo_loc);
  int globnoderef2[7]={5,6,3,2,7,8,4};
  for (int i=0; i<7; i++)
		CPPUNIT_ASSERT_EQUAL(num[i],globnoderef2[i]);
  delete[]num;
   
  //testing cell-cell correspondency
  typ_ent_loc=med_2_2::MED_MAILLE;
  typ_ent_dist=med_2_2::MED_MAILLE;
  typ_geo_loc= med_2_2::MED_QUAD4;
  typ_geo_dist= med_2_2::MED_QUAD4;
  //joint1
  n1 =med_2_2::MEDjointnCorres(fid1,meshname1,jn1,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
  tab=new int[2*n1];
  med_2_2::MEDjointLire(fid1,meshname1,jn1,tab,n1,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
    
  int tabreferencecell1[8]={1,1,1,2,2,1,2,2};
  for (int i=0; i<2*n1; i++)
		CPPUNIT_ASSERT_EQUAL(tab[i],tabreferencecell1[i]);
          
  n2 =med_2_2::MEDjointnCorres(fid1,meshname1,jn1,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
  delete[] tab;
  //joint2
  tab=new int[2*n2];
  med_2_2::MEDjointLire(fid2,meshname2,jn2,tab,n2,typ_ent_loc, typ_geo_loc,typ_ent_dist, typ_geo_dist);
  
  int tabreferencecell2[8]={1,1,1,2,2,1,2,2};
  for (int i=0; i<n2; i++)
		CPPUNIT_ASSERT_EQUAL(tab[i],tabreferencecell2[i]);   
  delete[]tab;
      
  //testing cells global numbering
  num=new int[2];
  MEDglobalNumLire(fid1, meshname1, num, 2, typ_ent_loc, typ_geo_loc);
  int globcellref1[2]={1,4};
  for (int i=0; i<2; i++)
    CPPUNIT_ASSERT_EQUAL(num[i],globcellref1[i]);   
  delete[] num;
  
  //testing cells global numbering
  num=new int[2];
  MEDglobalNumLire(fid2, meshname2, num, 2, typ_ent_loc, typ_geo_loc);
  int globcellref2[2]={2,3};
  for (int i=0; i<2; i++)
		CPPUNIT_ASSERT_EQUAL(num[i],globcellref2[i]);   
 
  delete[] num;   
  delete topo;
  delete[]partition;
}


/*! Testing a complex scenario to involve several shared pointers
 * on similar structures
 */
 
void MEDSPLITTERTest::testMESHCollection_complete_sequence()
{
  string data_dir                   = getenv("DATA_DIR");
  string tmp_dir                    = getenv("TMP");
  if (tmp_dir == "")
    tmp_dir = "/tmp";
  string filename_rd                = data_dir + "/MedFiles/pointe_import22.med";  
  string filename_para_wr            = tmp_dir + "/myWrField_para_pointe22_";
  string filename_para_med1           = tmp_dir + "/myWrField_para_pointe22_1.med";
  string filename_para_med2           = tmp_dir + "/myWrField_para_pointe22_2.med";
  string filename_para2_wr            = tmp_dir + "/myWrField_para2_pointe22_";
  string filename_para2_med1           = tmp_dir + "/myWrField_para2_pointe22_1.med";
  string filename_para2_med2           = tmp_dir + "/myWrField_para2_pointe22_2.med";
  string filename_para2_med3           = tmp_dir + "/myWrField_para2_pointe22_3.med";
  string filename_seq_wr               = tmp_dir + "/myWrField_seq_pointe22_";
  string filename_seq_med               = tmp_dir + "/myWrField_seq_pointe22_1.med";
  
  
  // To remove tmp files from disk
  MEDSPLITTERTest_TmpFilesRemover aRemover;
  aRemover.Register(filename_para_wr);
  aRemover.Register(filename_para_med1);
  aRemover.Register(filename_para_med2);
  aRemover.Register(filename_para2_wr);
  aRemover.Register(filename_para2_med1);
  aRemover.Register(filename_para2_med2);
  aRemover.Register(filename_para2_med3);
  aRemover.Register(filename_seq_wr);
  aRemover.Register(filename_seq_med);
  
  
  string meshname="maa1";
  MESHCollection collection(filename_rd,meshname);
  Topology* topo2=collection.createPartition(2,Graph::METIS);
  MESHCollection new_collection(collection, topo2);
  new_collection.write(filename_para_wr);
    
  MESHCollection new_collection2(filename_para_wr);
  CPPUNIT_ASSERT_EQUAL(collection.getName(),new_collection2.getName());
  Topology* topo3=new_collection2.createPartition(3,Graph::SCOTCH);
  MESHCollection new_collection3(new_collection2,topo3);
  CPPUNIT_ASSERT_EQUAL(topo3->nbCells(),topo2->nbCells());
  new_collection3.write(filename_para2_wr);
  
  MESHCollection new_collection4(filename_para2_wr);
  Topology* topo1=new_collection4.createPartition(1,Graph::METIS);
  MESHCollection new_collection_seq(new_collection4,topo1);
  new_collection_seq.write(filename_seq_wr);
  MEDMEM::MESH* mesh_after = new_collection_seq.getMesh(0);
  MEDMEM::MESH* mesh_before = collection.getMesh(0);
  CPPUNIT_ASSERT_EQUAL(mesh_before->getNumberOfElements(MED_CELL, MED_ALL_ELEMENTS), mesh_after->getNumberOfElements(MED_CELL, MED_ALL_ELEMENTS));
  CPPUNIT_ASSERT_EQUAL(mesh_before->getNumberOfElements(MED_FACE, MED_ALL_ELEMENTS), mesh_after->getNumberOfElements(MED_FACE, MED_ALL_ELEMENTS));
  CPPUNIT_ASSERT_EQUAL(mesh_before->getNumberOfElements(MED_EDGE, MED_ALL_ELEMENTS), mesh_after->getNumberOfElements(MED_EDGE, MED_ALL_ELEMENTS));
  CPPUNIT_ASSERT_EQUAL(mesh_before->getNumberOfNodes(), mesh_after->getNumberOfNodes());
  delete topo2;
  delete topo3;
  delete topo1;
}
