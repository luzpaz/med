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

#include "MEDSPLITTERTest.hxx"
#include "MEDSPLITTERTest_Utils.hxx"

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
#include "MEDSPLITTER_MESHCollection.hxx"
#include "MEDSPLITTER_MESHCollectionDriver.hxx"

#ifdef MED_ENABLE_METIS
#include "MEDSPLITTER_METISGraph.hxx"
#endif
//#include "MEDSPLITTER_SCOTCHGraph.hxx"

#include "MEDMEM_Exception.hxx"

#include <string>
#include <cppunit/TestAssert.h>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
#define ENABLE_FORCED_FAILURES


using namespace std;
using namespace MEDSPLITTER;
using namespace MEDMEM;
 
/*
 * Check methods defined in ParallelTopology.hxx
 * 
 ParallelTopology();
 ParallelTopology(vector<MEDMEM::MESH*>, vector<MEDMEM::CONNECTZONE*>,vector<int*>&, vector<int*>&, vector<int*>&);
 (+) ParallelTopology(boost::shared_ptr<Graph> graph, int nbdomain, int mesh_dimension);
 ~ParallelTopology();
 (+) void convertGlobalNodeList(const int*, int,int*,int*);
 (+) void convertGlobalNodeList(const int*, int,int*,int);
 (+) void convertGlobalNodeListWithTwins(const int* face_list, int nbnode, int*& local, int*& ip, int*& full_array, int& size);
 (+) void convertGlobalCellList(const int*, int , int*, int *);
 void convertGlobalFaceList(const int*, int , int*, int *);    
 void convertGlobalFaceList(const int*, int , int*, int);  
 void convertGlobalFaceListWithTwins(const int* face_list, int nbface, int*& local, int*& ip, int*& full_array,int& size);
 void createNodeMapping(std::map<MED_EN::medGeometryElement,int*>& type_connectivity,
 std::map<MED_EN::medGeometryElement,int>& present_type_numbers,
 int idomain);
 void createFaceMapping(const MESHCollection &);
 void createFaceMapping2ndversion(const MESHCollection &);
 void convertToLocal(std::map<MED_EN::medGeometryElement,int*>& type_connectivity,
 std::map<MED_EN::medGeometryElement,int>& present_type_numbers,
 int idomain,
 MED_EN::medEntityMesh entity);
 void computeNodeNodeCorrespondencies(int nbdomain,vector<MEDMEM::MEDSKYLINEARRAY*>& ) const;
 void computeCellCellCorrespondencies(int nbdomain,vector<MEDMEM::MEDSKYLINEARRAY*>&, const Graph* ) const;
 inline  int convertNodeToGlobal(int ip,int icell) const
 inline  int convertFaceToGlobal(int ip,int icell) const
 inline  int convertCellToGlobal(int ip,int icell) const
 inline  void convertNodeToGlobal(int ip, const int* local, int n, int* global)const
 (+)inline  void convertCellToGlobal(int ip, const int* local, int n, int* global)const
 inline  void convertFaceToGlobal(int ip, const int* local, int n, int* global)const
 (+) inline  int nbDomain() const
 int nbCells() const
 (+) inline  int nbCells( int idomain) const
 (+) inline  int getNodeNumber(int idomain) const
 inline  int getNodeNumber() const
 inline  void getNodeList(int idomain, int* list) const
 (+) inline  int getCellNumber(int idomain) const
 inline  int getCellDomainNumber(int global) const
 inline  void getCellList(int idomain, int* list) const
 inline int getFaceNumber(int idomain) const
 inline  int getFaceNumber() const
 inline  void getFaceList(int idomain, int* list) const
 boost::shared_ptr<Graph> getGraph() const
*/
 
void MEDSPLITTERTest::testParallelTopology_graph_constructor()
{
#ifndef MED_ENABLE_METIS
  CPPUNIT_FAIL("METIS is not available. Please, check your compilation.");
#else
  string filename_rd        = MEDSPLITTERTest_Utils::getResourceFile("carre_en_quad4.med");
  string filename_para_wr   = MEDSPLITTERTest_Utils::makeTmpFile("myWrField_seq_pointe22_");
  string filename_para_med0 = MEDSPLITTERTest_Utils::makeTmpFile("myWrField_seq_pointe22_1.med");
  string filename_para_med1 = MEDSPLITTERTest_Utils::makeTmpFile("myWrField_seq_pointe22_2.med");
  
  string meshname="carre_en_quad4";
  MESHCollection collection(filename_rd,meshname);
  
  MEDMEM::MEDSKYLINEARRAY* array=0;
  int* edgeweights=0;
    
  collection.buildCellGraph(array,edgeweights);
    
  boost::shared_ptr<Graph> cell_graph=boost::shared_ptr<Graph>(new METISGraph(array,edgeweights));
         
  cell_graph->partGraph(2,"");
  
  //cell_graph is a shared pointer 
  Topology* topology = new ParallelTopology (cell_graph, 2, collection.getMeshDimension());
  
  
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
 
  int iglobal[3]={1,2,3};
  int* iloc=new int[3];
  int* iproc=new int[3];
  int iloc_answer[3]={1,1,2};
  int iproc_answer[3]={0,1,0};
  topology->convertGlobalCellList(iglobal,3,iloc,iproc);
  for(int i=0; i<3; i++)
                { 
                        CPPUNIT_ASSERT_EQUAL(iloc_answer[i], iloc[i]);
                        CPPUNIT_ASSERT_EQUAL(iproc_answer[i],iproc[i]);
                }
  int* global_2 = new int[3];
  topology->convertCellToGlobal(0,iloc,3,global_2);
  int global_answer[3]={1,1,3};
  for (int i=0; i<3; i++)
                {
                        CPPUNIT_ASSERT_EQUAL(global_answer[i],global_2[i]);
                }
  
  CPPUNIT_ASSERT_EQUAL(topology->getCellNumber(0),2);  
  CPPUNIT_ASSERT_EQUAL(topology->getCellNumber(1),2);
  
    
  CPPUNIT_ASSERT_EQUAL(topology->nbCells(0),2);
  CPPUNIT_ASSERT_EQUAL(topology->nbCells(1),2);
  
  CPPUNIT_ASSERT_EQUAL(topology->nbDomain(),2);
  //node and face lists have not yet been computed
  CPPUNIT_ASSERT_THROW(topology->convertGlobalNodeList(iglobal,3,iloc,iproc),MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(topology->convertGlobalFaceList(iglobal,3,iloc,iproc),MEDEXCEPTION);
  
  MESHCollection new_collection(collection, topology);
  
  CPPUNIT_ASSERT_EQUAL(topology->getNodeNumber(0),6);  
  CPPUNIT_ASSERT_EQUAL(topology->getNodeNumber(1),6);
  
  topology->convertGlobalNodeList(iglobal,3,iloc,iproc);
   
  int iloc_node_answer[3]={4,3,3};
  int iproc_node_answer[3]={0,0,1};
  for(int i=0; i<3; i++)
                { 
                        CPPUNIT_ASSERT_EQUAL(iloc_node_answer[i], iloc[i]);
                        CPPUNIT_ASSERT_EQUAL(iproc_node_answer[i],iproc[i]);
                }
  int* local_nodes;
  int* ip_nodes;
  int* full_array;
  int size;
  topology->convertGlobalNodeListWithTwins(iglobal, 3, local_nodes, ip_nodes, full_array, size);
  CPPUNIT_ASSERT_EQUAL(size,4);
  int iloc_node_wt_answer[4]={4,3,4,3};
  int iproc_node_wt_answer[4]={0,0,1,1};
  for(int i=0; i<4; i++)
                { 
                        CPPUNIT_ASSERT_EQUAL(iloc_node_wt_answer[i], local_nodes[i]);
                        CPPUNIT_ASSERT_EQUAL(iproc_node_wt_answer[i],ip_nodes[i]);
                }
  delete topology;
  delete[] iloc;
  delete[]iproc;
  delete[] global_2;
  delete[] local_nodes;
  delete[] ip_nodes;
  delete[] full_array;
#endif // MED_ENABLE_METIS
}
