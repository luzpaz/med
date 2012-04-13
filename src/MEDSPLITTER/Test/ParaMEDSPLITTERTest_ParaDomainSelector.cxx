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
// File      : ParaMEDSPLITTERTest_ParaDomainSelector.cxx
// Created   : Tue Jul 21 16:25:50 2009
// Author    : Edward AGAPOV (eap)

#include "ParaMEDSPLITTERTest.hxx"
#include "MEDSPLITTERTest_Utils.hxx"

#include "MEDSPLITTER_ParaDomainSelector.hxx"
#include "MEDSPLITTER_UserGraph.hxx"
#include "MEDSPLITTER_JointExchangeData.hxx"
#include "MEDSPLITTER_FaceModel.hxx"

#include <MEDMEM_Meshing.hxx>
#include <MEDMEM_Grid.hxx>
#include <MEDMEM_ConnectZone.hxx>

#include <cppunit/TestAssert.h>
#include <set>

using namespace std;
using namespace MEDSPLITTER;
using namespace MED_EN;

//================================================================================
/*!
 * \brief Test ParaDomainSelector::gatherGraph( graph )
 */
//================================================================================

void ParaMEDSPLITTERTest::testParaDomainSelector_gatherGraph()
{
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD,&world_size);

  if ( world_size != 2 )
    return;

  // Combine two equal graphs:
  // index: [ 1, 3, 6 ]
  // value: [ 1, 2, 1, 2, 3]
  // partition: [ 0, 1 ]
  //
  // Expect to get:
  // index: [ 1, 3, 6, 8, 11 ]
  // value: [ 1, 2, 1, 2, 3, 1, 2, 1, 2, 3]
  // partition: [ 0, 1, 0, 1 ]

  ParaDomainSelector dom_sel;
  CPPUNIT_ASSERT_NO_THROW( dom_sel.isOnDifferentHosts() );
#ifdef _DEBUG_
  cout << "On different hosts = " << dom_sel.isOnDifferentHosts() << endl;
#endif

  // Let ParaDomainSelector know domain sizes

  MEDMEM::MESHING meshing;
  int nb_elems[] = { 2 }; medGeometryElement type = MED_POINT1;
  meshing.setNumberOfTypes   ( 1, MED_CELL );
  meshing.setTypes           ( &type, MED_CELL );
  meshing.setNumberOfElements( nb_elems, MED_CELL );

  vector<MEDMEM::MESH*> domain_meshes(2, (MEDMEM::MESH*)0);
  domain_meshes[ dom_sel.rank() ] = &meshing;
  dom_sel.gatherNbOf( MED_CELL, domain_meshes );

  // Make the graph

  int index[] = { 1, 3, 6 };
  int value[] = { 1, 2, 1, 2, 3 };
  int* partition = new int[2];
  partition[0]=0; partition[1] = 1;
  MEDMEM::MEDSKYLINEARRAY* array = new MEDMEM::MEDSKYLINEARRAY( 2, 5, index, value);
  auto_ptr<Graph> graph( new UserGraph( array, partition, 2 ));

  // Gather graphs

  auto_ptr<Graph> glob_graph( dom_sel.gatherGraph( graph.get() ));

  const int* ind = glob_graph->getGraph()->getIndex();
  const int* val = glob_graph->getGraph()->getValue();
  const int* part= glob_graph->getPart();

  CPPUNIT_ASSERT_EQUAL( 4, glob_graph->getGraph()->getNumberOf() );
  CPPUNIT_ASSERT_EQUAL( index[0], ind[0] );
  CPPUNIT_ASSERT_EQUAL( index[1], ind[1] );
  CPPUNIT_ASSERT_EQUAL( index[2], ind[2] );
  CPPUNIT_ASSERT_EQUAL( index[2]+ind[1]-ind[0], ind[3] );
  CPPUNIT_ASSERT_EQUAL( index[2]+ind[2]-ind[0], ind[4] );

  CPPUNIT_ASSERT_EQUAL( 10, glob_graph->getGraph()->getLength() );
  CPPUNIT_ASSERT_EQUAL( value[0], val[0] );
  CPPUNIT_ASSERT_EQUAL( value[1], val[1] );
  CPPUNIT_ASSERT_EQUAL( value[2], val[2] );
  CPPUNIT_ASSERT_EQUAL( value[3], val[3] );
  CPPUNIT_ASSERT_EQUAL( value[4], val[4] );
  CPPUNIT_ASSERT_EQUAL( value[0], val[5] );
  CPPUNIT_ASSERT_EQUAL( value[1], val[6] );
  CPPUNIT_ASSERT_EQUAL( value[2], val[7] );
  CPPUNIT_ASSERT_EQUAL( value[3], val[8] );
  CPPUNIT_ASSERT_EQUAL( value[4], val[9] );

  CPPUNIT_ASSERT_EQUAL( partition[0], part[0] );
  CPPUNIT_ASSERT_EQUAL( partition[1], part[1] );
  CPPUNIT_ASSERT_EQUAL( partition[0], part[2] );
  CPPUNIT_ASSERT_EQUAL( partition[1], part[3] );
}

//================================================================================
/*!
 * \brief test ParaDomainSelector::gatherNbOf( entity, domain_meshes )
 */
//================================================================================

void ParaMEDSPLITTERTest::testParaDomainSelector_gatherNbOf ()
{
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD,&world_size);

  if ( world_size != 2 )
    return;

  string filename = MEDSPLITTERTest_Utils::getResourceFile("square1_split1.med");
  string meshname  = "Mesh_2_1";

  ParaDomainSelector dom_sel;

  MEDMEM::MESH mesh_full( MEDMEM::MED_DRIVER, filename, meshname);
  MEDMEM::MESH mesh_empty;

  vector<MEDMEM::MESH*> meshes(2);
  meshes[0] = & mesh_full;
  meshes[1] = & mesh_empty;
  if ( dom_sel.rank() > 0 )
    swap( meshes[0], meshes[1] );

  // test gatherNbOf( MED_CELL )
  int nb_cells, init_nb_cells = mesh_full.getNumberOfElements( MED_CELL, MED_ALL_ELEMENTS );
  CPPUNIT_ASSERT_NO_THROW( nb_cells = dom_sel.gatherNbOf( MED_CELL, meshes ));
  CPPUNIT_ASSERT_EQUAL( 2 * init_nb_cells, nb_cells );

  // test gatherNbOf( MED_EDGE )
  int nb_edges, init_nb_edges = mesh_full.getNumberOfElements( MED_EDGE, MED_ALL_ELEMENTS );
  CPPUNIT_ASSERT_NO_THROW( nb_edges = dom_sel.gatherNbOf( MED_EDGE, meshes ));
  CPPUNIT_ASSERT_EQUAL( 2 * init_nb_edges, nb_edges );

  // test getNbVertOfProcs()
  int* nb_cell_by_proc;
  CPPUNIT_ASSERT_NO_THROW( nb_cell_by_proc = dom_sel.getNbVertOfProcs() );
  CPPUNIT_ASSERT_EQUAL( init_nb_cells, nb_cell_by_proc[1]-nb_cell_by_proc[0] );
  CPPUNIT_ASSERT_EQUAL( init_nb_cells, nb_cell_by_proc[2]-nb_cell_by_proc[1] );

  // test getProcShift()
  if ( dom_sel.rank() == 0 )
    CPPUNIT_ASSERT_EQUAL( 0, dom_sel.getProcShift() );
  else
    CPPUNIT_ASSERT_EQUAL( init_nb_cells, dom_sel.getProcShift() );

  CPPUNIT_ASSERT_EQUAL( 0, dom_sel.getDomainShift(0) );
  CPPUNIT_ASSERT_EQUAL( init_nb_cells, dom_sel.getDomainShift(1) );
}

//================================================================================
/*!
 * \brief Test ParaDomainSelector::gatherEntityTypesInfo( domain_meshes, entity )
 */
//================================================================================

void ParaMEDSPLITTERTest::testParaDomainSelector_gatherEntityTypesInfo ()
{
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD,&world_size);

  if ( world_size != 2 )
    return;

  string filename = MEDSPLITTERTest_Utils::getResourceFile("square1_split1.med");
  string meshname  = "Mesh_2_1";

  ParaDomainSelector dom_sel;

  MEDMEM::MESH mesh_full( MEDMEM::MED_DRIVER, filename, meshname);
  MEDMEM::MESH mesh_empty;

  vector<MEDMEM::MESH*> meshes(2);
  meshes[0] = & mesh_full;
  meshes[1] = & mesh_empty;
  if ( dom_sel.rank() > 0 )
    swap( meshes[0], meshes[1] );

  // ---------------
  // test for cells
  // ---------------

  const medGeometryElement * init_cell_types = mesh_full.getTypes               (MED_CELL);
  const int * init_cell_nb_index             = mesh_full.getGlobalNumberingIndex(MED_CELL);
  int nb_init_cell_types                     = mesh_full.getNumberOfTypes       (MED_CELL);

  // pass info on cell types from mesh_full on other proc to mesh_empty on this proc
  CPPUNIT_ASSERT_NO_THROW( dom_sel.gatherEntityTypesInfo( meshes, MED_CELL ));

  const medGeometryElement * got_cell_types = mesh_empty.getTypes               (MED_CELL);
  const int * got_cell_nb_index             = mesh_empty.getGlobalNumberingIndex(MED_CELL);
  int nb_got_cell_types                     = mesh_empty.getNumberOfTypes       (MED_CELL);

  // check that we got the same info
  CPPUNIT_ASSERT_EQUAL( nb_init_cell_types, nb_got_cell_types);
  CPPUNIT_ASSERT_EQUAL( nb_init_cell_types, mesh_full.getNumberOfTypes(MED_CELL));
  CPPUNIT_ASSERT_EQUAL( init_cell_types,    mesh_full.getTypes (MED_CELL));
  for ( int i = 0; i < nb_got_cell_types; ++i )
  {
    CPPUNIT_ASSERT_EQUAL( init_cell_types[i], got_cell_types[i] );
    CPPUNIT_ASSERT_EQUAL( mesh_full.getNumberOfElements(MED_CELL, MED_ALL_ELEMENTS ),
                          mesh_empty.getNumberOfElements(MED_CELL, MED_ALL_ELEMENTS ));
    CPPUNIT_ASSERT_EQUAL( init_cell_nb_index[ i+1 ], got_cell_nb_index[ i+1 ]);
  }

  // test for faces - there are no faces
  CPPUNIT_ASSERT_THROW( dom_sel.gatherEntityTypesInfo( meshes, MED_FACE ), MED_EXCEPTION);

  // ---------------
  // test for edges
  // ---------------

  const medGeometryElement * init_edge_types = mesh_full.getTypes               (MED_EDGE);
  const int * init_edge_nb_index             = mesh_full.getGlobalNumberingIndex(MED_EDGE);
  int nb_init_edge_types                     = mesh_full.getNumberOfTypes       (MED_EDGE);

  // pass info on edge types from mesh_full on other proc to mesh_empty on this proc
  CPPUNIT_ASSERT_NO_THROW( dom_sel.gatherEntityTypesInfo( meshes, MED_EDGE ));

  const medGeometryElement * got_edge_types = mesh_empty.getTypes               (MED_EDGE);
  const int * got_edge_nb_index             = mesh_empty.getGlobalNumberingIndex(MED_EDGE);
  int nb_got_edge_types                     = mesh_empty.getNumberOfTypes       (MED_EDGE);

  // check that we got the same info
  CPPUNIT_ASSERT_EQUAL( nb_init_edge_types, nb_got_edge_types);
  CPPUNIT_ASSERT_EQUAL( nb_init_edge_types, mesh_full.getNumberOfTypes(MED_EDGE));
  CPPUNIT_ASSERT_EQUAL( init_edge_types,    mesh_full.getTypes (MED_EDGE));
  for ( int i = 0; i < nb_got_edge_types; ++i )
  {
    CPPUNIT_ASSERT_EQUAL( init_edge_types[i], got_edge_types[i] );
    CPPUNIT_ASSERT_EQUAL( mesh_full.getNumberOfElements(MED_EDGE, MED_ALL_ELEMENTS ),
                          mesh_empty.getNumberOfElements(MED_EDGE, MED_ALL_ELEMENTS ));
    CPPUNIT_ASSERT_EQUAL( init_edge_nb_index[ i+1 ], got_edge_nb_index[ i+1 ]);
  }
}

void ParaMEDSPLITTERTest::testParaDomainSelector_gatherNbCellPairs ()
{
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD,&world_size);

  if ( world_size != 2 )
    return;

  ParaDomainSelector dom_sel;
  CPPUNIT_ASSERT_EQUAL( 2, dom_sel.nbProcs() );

  // imagine we have 5 domains
  const int nb_domains = 5;
  dom_sel.setNbDomains(nb_domains);

  // ----------------------------------------
  // test isMyDomain() and getProccessorID()
  // ----------------------------------------
  set<int> doms_here;
  for ( int idomain = 0; idomain < nb_domains; ++idomain )
    if ( dom_sel.isMyDomain( idomain ))
    {
      doms_here.insert( idomain );
      CPPUNIT_ASSERT_EQUAL( dom_sel.getProccessorID( idomain ), dom_sel.rank() );
    }
  CPPUNIT_ASSERT( doms_here.size() == 2 || doms_here.size() == 3 );

  // ---------------
  // test jointId()
  // ---------------
  set<int> joint_ids;
  set< pair< int, int> > dom_pairs;
  for ( int idom1 = 0; idom1 < nb_domains; ++idom1 )
    for ( int idom2 = 0; idom2 < nb_domains; ++idom2 )
      if ( idom1 != idom2 )
      {
        joint_ids.insert( dom_sel.jointId( idom1, idom2 ));
        dom_pairs.insert( idom1 < idom2 ? make_pair( idom1, idom2 ) : make_pair( idom2, idom1 ));
      }
  CPPUNIT_ASSERT_EQUAL( dom_pairs.size(), joint_ids.size() );

  // -------------------------
  // test gatherNbCellPairs()
  // -------------------------
  joint_ids.clear();
  int nb_interproc_pairs = 0, nb_local_pairs = 0, total_joint_size = 0;
  for ( int idom1 = 0; idom1 < nb_domains; ++idom1 )
    for ( int idom2 = idom1+1; idom2 < nb_domains; ++idom2 )
    {
      // gather data for further checks
      int nb_doms_here   = doms_here.count( idom1 ) + doms_here.count( idom2 );
      bool is_inter_proc = ( nb_doms_here == 1 );
      bool is_local      = ( nb_doms_here%2 == 0 );
      int nb_pairs       = dom_sel.jointId( idom1, idom2 );
      total_joint_size += nb_pairs;
      if ( is_inter_proc ) nb_interproc_pairs += nb_pairs;
      if ( is_local      ) nb_local_pairs     += nb_pairs;
      if ( nb_doms_here == 0 || idom1 == idom2 ) continue;
      // set data to ParaDomainSelector
      if ( is_inter_proc && doms_here.count( idom1 ) ||
           is_local      && nb_doms_here == 2 )
      {
        dom_sel.setNbCellPairs( nb_pairs, idom1, idom2 );
      }
    }
  dom_sel.gatherNbCellPairs();
  //CPPUNIT_ASSERT_EQUAL( nb_local_pairs, dom_sel.getNbSubentitiesWithoutProcJoints() );

  for ( int idom1 = 0; idom1 < nb_domains; ++idom1 )
    for ( int idom2 = 0; idom2 < nb_domains; ++idom2 )
    {
      //int nb_doms_here = doms_here.count( idom1 ) + doms_here.count( idom2 );
      int nb_pairs_expect = dom_sel.jointId( idom1, idom2 );
      if ( int nb_pairs = dom_sel.getNbCellPairs( idom1, idom2 ))
      {
        CPPUNIT_ASSERT_EQUAL( nb_pairs_expect, nb_pairs );
        int fisrt_id = dom_sel.getFisrtGlobalIdOfSubentity( idom1, idom2 );
        CPPUNIT_ASSERT(0 < fisrt_id && fisrt_id <= total_joint_size);
      }
    }
}

//================================================================================
/*!
 * \brief Test work of JointExchangeData and of ParaDomainSelector::exchangeJoint(joint)
 */
//================================================================================

void ParaMEDSPLITTERTest::testParaDomainSelector_exchangeJoint ()
{
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD,&world_size);

  if ( world_size != 2 )
    return;

  ParaDomainSelector dom_sel;

  // Exchange a joint between two grids with following global ids
  //    5     6      6     9
  //   *-----*      *-----*
  //   |     |      |     |
  //   |  2  |      |  4  |
  //   |3    |4     |4    |8
  //   *-----*      *-----*
  //   |     |      |     |
  //   |  1  |      |  3  |
  //   |1    |2     |2    |7
  //   *-----*      *-----*
  
  // make grids; actually we make one grid as their connectivities are same and
  // coordinates doesn't matter here
  const MEDMEM::MESH* grid = 0;
  {
    const int dim = 2;
    vector<vector<double> > xyz_array(dim);
    vector<string>          coord_name(dim,"coo");
    vector<string>          unit(dim,"m");

    xyz_array[0].resize(2, 0.);
    xyz_array[1].resize(3, 0.);
    MEDMEM::GRID tmpGrid( xyz_array, coord_name, unit );
    grid = tmpGrid.convertInMESH();
  }

  // global ids
  int cells1[] = { 1, 2 };
  int cells2[] = { 3, 4 };
  int nodes1[] = { 1, 2, 3, 4, 5, 6 };
  int nodes2[] = { 2, 7, 4, 8, 6, 9 };
  int* cells_here = dom_sel.rank() ? cells2 : cells1;
  int* cells_dist = dom_sel.rank() ? cells2 : cells1;
  int* nodes_here = dom_sel.rank() ? nodes2 : nodes1;
  //int* nodes_dist = dom_sel.rank() ? nodes2 : nodes1;

  int dom_here = dom_sel.isMyDomain( 0 ) ? 0 : 1;
  int dom_dist = 1 - dom_here;

  // store cell/cell correspondencies
  JointExchangeData joint;
  joint.addCellCorrespondence( grid, dom_dist, dom_here, cells_dist[0], cells_here[0], 1 );
  joint.addCellCorrespondence( grid, dom_dist, dom_here, cells_dist[1], cells_here[1], 2 );
  CPPUNIT_ASSERT_EQUAL( 2, joint.nbCellPairs());

  // prepare to exchangeJoint()
  joint.setMeshes( dom_dist, grid, dom_here, grid );
  CPPUNIT_ASSERT_EQUAL( dom_dist, joint.distantDomain() );
  CPPUNIT_ASSERT_EQUAL( dom_here, joint.localDomain() );

  joint.setConnectivity( nodes_here );
  // in general case we woul need to call dom_sel.gatherEntityTypesInfo() but grid
  // knows nb of elements without connectivity initialization
  CPPUNIT_ASSERT_THROW( dom_sel.exchangeJoint( & joint ), MED_EXCEPTION);
  dom_sel.setNbDomains( 2 );
  dom_sel.setNbCellPairs( 2, dom_dist, dom_here );
  dom_sel.gatherNbCellPairs();
  CPPUNIT_ASSERT_NO_THROW( dom_sel.exchangeJoint( & joint ));

  // get CONNECTZONE
  const int joint_edge_glob_id = 100;
  joint.setFisrtGlobalIdOfSubentity( joint_edge_glob_id );
  TGeom2FacesByDomian face_map(2);
  auto_ptr<MEDMEM::CONNECTZONE> cz( joint.makeConnectZone(face_map));
  CPPUNIT_ASSERT( cz.get() );

  // ---------
  // check cz
  // ---------

  CPPUNIT_ASSERT_EQUAL( dom_dist, cz->getDistantDomainNumber() );
  CPPUNIT_ASSERT_EQUAL( dom_here, cz->getLocalDomainNumber() );

  CPPUNIT_ASSERT_EQUAL( long(grid), long(cz->getLocalMesh()) );
  CPPUNIT_ASSERT_EQUAL( long(grid), long(cz->getDistantMesh()) );

  CPPUNIT_ASSERT( cz->isEntityCorrespPresent(MED_CELL, MED_CELL) );
  CPPUNIT_ASSERT( !cz->isEntityCorrespPresent(MED_NODE, MED_NODE) );
  CPPUNIT_ASSERT( !cz->isEntityCorrespPresent(MED_EDGE, MED_EDGE) );
  CPPUNIT_ASSERT( !cz->isEntityCorrespPresent(MED_FACE, MED_FACE) );

  CPPUNIT_ASSERT_EQUAL( 3, cz->getNodeNumber() );
  CPPUNIT_ASSERT_EQUAL( 2, cz->getEntityCorrespNumber(MED_CELL,MED_CELL));

  const int * node_index = cz->getNodeCorrespIndex();
  CPPUNIT_ASSERT(node_index);
  for ( int i = 0; i < 4; ++i )
    CPPUNIT_ASSERT_EQUAL( i + 1, node_index[ i ]);

  const int * node_value = cz->getNodeCorrespValue();
  CPPUNIT_ASSERT(node_value);
  int val[2][6] = {{ 2, 1, 4, 3, 6, 5 },
                   { 1, 2, 3, 4, 5, 6 }}; // local ids of neighbouring domain
  for ( int i = 0; i < 6; ++i )
    CPPUNIT_ASSERT_EQUAL( val[ dom_sel.rank()][ i ], node_value[ i ]);

  const int * cell_index = cz->getEntityCorrespIndex(MED_CELL, MED_CELL);
  CPPUNIT_ASSERT(cell_index);
  for ( int i = 0; i < 3; ++i )
    CPPUNIT_ASSERT_EQUAL( i + 1, cell_index[ i ]);

  const int * cell_value = cz->getEntityCorrespValue(MED_CELL, MED_CELL);
  CPPUNIT_ASSERT(cell_value);
  CPPUNIT_ASSERT_EQUAL( 1, cell_value[ 0 ]);
  CPPUNIT_ASSERT_EQUAL( 2, cell_value[ 1 ]);

  CPPUNIT_ASSERT_EQUAL( 1, int( face_map[ dom_sel.rank() ].size() ));
  CPPUNIT_ASSERT_EQUAL( 0, int( face_map[ 1 - dom_sel.rank() ].size() ));
  CPPUNIT_ASSERT_EQUAL( MED_SEG2, face_map[ dom_sel.rank() ].begin()->first );

  vector< MEDSPLITTER_FaceModel* >& faces = face_map[ dom_sel.rank() ][ MED_SEG2 ];
  CPPUNIT_ASSERT_EQUAL( 2, int( faces.size() ));
  CPPUNIT_ASSERT_EQUAL( MED_SEG2, faces[0]->getType());
  CPPUNIT_ASSERT_EQUAL( joint_edge_glob_id+0, faces[0]->getGlobal());
  CPPUNIT_ASSERT_EQUAL( joint_edge_glob_id+1, faces[1]->getGlobal());
  // edge connectivity
  CPPUNIT_ASSERT_EQUAL( dom_sel.rank() ? 1 : 4, (*faces[0])[0]);
  CPPUNIT_ASSERT_EQUAL( dom_sel.rank() ? 3 : 2, (*faces[0])[1]);
  CPPUNIT_ASSERT_EQUAL( dom_sel.rank() ? 3 : 6, (*faces[1])[0]);
  CPPUNIT_ASSERT_EQUAL( dom_sel.rank() ? 5 : 4, (*faces[1])[1]);

  grid->removeReference();
}

//================================================================================
/*!
 * \brief Test ParaDomainSelector::exchangeSubentityIds()
*/
//================================================================================

void ParaMEDSPLITTERTest::testParaDomainSelector_exchangeSubentityIds ()
{
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD,&world_size);

  if ( world_size != 2 )
    return;

  ParaDomainSelector dom_sel;
  vector<int> loc_ids_here(3, -373);
  int* got_ids;
  CPPUNIT_ASSERT_THROW
    ( dom_sel.exchangeSubentityIds( dom_sel.rank(), 1-dom_sel.rank(), loc_ids_here),MED_EXCEPTION);
  dom_sel.setNbDomains( 2 );
  CPPUNIT_ASSERT_NO_THROW
    ( got_ids = dom_sel.exchangeSubentityIds( dom_sel.rank(), 1-dom_sel.rank(), loc_ids_here));
  CPPUNIT_ASSERT_EQUAL( loc_ids_here[0], got_ids[0] );
  CPPUNIT_ASSERT_EQUAL( loc_ids_here[1], got_ids[1] );
  CPPUNIT_ASSERT_EQUAL( loc_ids_here[2], got_ids[2] );
  delete [] got_ids;
}

