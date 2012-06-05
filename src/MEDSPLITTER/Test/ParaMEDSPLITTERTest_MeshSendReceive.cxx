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
// File      : ParaMEDSPLITTERTest_MeshSendReceive.cxx
// Created   : Mon Jul  6 14:14:29 2009
// Author    : Edward AGAPOV (eap)

#include "ParaMEDSPLITTERTest.hxx"
#include "MEDSPLITTERTest_Utils.hxx"

#include "MEDSPLITTER_MESHCollection.hxx"
#include "MEDSPLITTER_MeshSendReceive.hxx"
#include "MEDSPLITTER_Topology.hxx"

#include "MEDMEM_MeshFuse.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_DriverFactory.hxx"

using namespace std;
using namespace MEDSPLITTER;
using namespace MED_EN;

//================================================================================
/*!
 * \brief Test intended to run on one proc
 */
//================================================================================

void ParaMEDSPLITTERTest::testMeshSendReceive()
{
  MEDMEM::STRING filename = MEDSPLITTERTest_Utils::getResourceFile("pointe.med");
  MEDMEM::MESH* meshSent = 0;
  MEDMEM::MESH* meshRecv = 0;

  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank) ;

  MeshSendReceive sender, receiver;
  vector<int> nums_sent_1(3,7), nums_sent_2(4,8), nums_sent_3(5,9);
  vector<int> nums_recv_1, nums_recv_2, nums_recv_3;

  if ( rank == 0 )
  {
    meshSent = new MEDMEM::MESH;
    MEDMEM::MED_MESH_RDONLY_DRIVER drv(filename, meshSent );
    drv.setMeshName( "maa1" );
    drv.desactivateFacesComputation();
    drv.open();
    drv.read();
    drv.close();
    CPPUNIT_ASSERT_NO_THROW( sender.send( size-1, 1,meshSent,nums_sent_1,nums_sent_2,nums_sent_3));
  }
  if ( rank == size-1 )
  {
    CPPUNIT_ASSERT_NO_THROW( meshRecv = receiver.recv( 0, 1, nums_recv_1,nums_recv_2,nums_recv_3));
    CPPUNIT_ASSERT( meshRecv );
    // test by printing that minimally needed data is present
    MEDMEM::STRING out;
    CPPUNIT_ASSERT_NO_THROW( out << *meshRecv );
  }
  if ( size == 1 )
  {
    CPPUNIT_ASSERT( nums_recv_1 == nums_sent_1 );
    CPPUNIT_ASSERT( nums_recv_2 == nums_sent_2 );
    CPPUNIT_ASSERT( nums_recv_3 == nums_sent_3 );

    if ( ! meshSent->deepCompare( *meshRecv ) )
    {
#ifdef _DEBUG_
      MEDMEM::STRING outfilename("/tmp/pointe");
      outfilename << (void*) meshRecv << ".med";
      int drv = meshRecv->addDriver(MEDMEM::MED_DRIVER, outfilename, meshRecv->getName() );
      meshRecv->write( drv );
      CPPUNIT_FAIL( MEDMEM::STRING("received mesh differs from the sent one ") << filename <<
                                   ", check the received mesh in " << outfilename);
#else
      CPPUNIT_FAIL( "received mesh differs from the sent one ");
#endif
    }
  }

  delete meshRecv;
  delete meshSent;
}

//================================================================================
/*!
 * \brief Split pointe.med and fuse back
 */
//================================================================================

void ParaMEDSPLITTERTest::testMeshFuse()
{
  string filename  = MEDSPLITTERTest_Utils::getResourceFile("pointe.med");
  string meshname  = "maa1";

  // Splitting

  MESHCollection collection(filename,meshname);

  vector<int> partition;
  partition.reserve( collection.getMesh()[0]->getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS));
  for ( int i = 0; i < partition.capacity()/2; ++i )
    partition.push_back( 0 );
  while ( partition.size() < partition.capacity() )
    partition.push_back( 1 );

  Topology* new_topo = collection.createPartition( &partition[0] );
  MESHCollection new_collection(collection, new_topo);

  // test splits of the mesh by printing
  MEDMEM::STRING out;
  CPPUNIT_ASSERT_NO_THROW( out << *(new_collection.getMesh()[0]));
  CPPUNIT_ASSERT_NO_THROW( out << *(new_collection.getMesh()[1]));

  // global numbers
  vector<int> glob_nb_left ( new_topo->getNodeNumber(0) );
  vector<int> glob_nb_right( new_topo->getNodeNumber(1) );
  new_topo->getNodeList( 0, & glob_nb_left[0] );
  new_topo->getNodeList( 1, & glob_nb_right[0] );

  // Fusion

  // test that minimally needed data present by printing
  MEDMEM::MeshFuse fusion;
  fusion.concatenate(new_collection.getMesh()[0], glob_nb_left);
  CPPUNIT_ASSERT_NO_THROW( out << ( (const MEDMEM::MESH &) fusion ));
  fusion.concatenate(new_collection.getMesh()[1], glob_nb_right);
  CPPUNIT_ASSERT_NO_THROW( out << ( (const MEDMEM::MESH &) fusion ));

  // compare
  MEDMEM::MESH init_mesh( MEDMEM::MED_DRIVER, filename, meshname);
  if ( !init_mesh.deepCompare( fusion ))
  {
//     fusion.addDriver( MEDMEM::MED_DRIVER, "/tmp/fusion.med", fusion.getName() );
//     fusion.write();

//     new_collection.getMesh()[0]->addDriver( MEDMEM::MED_DRIVER,"/tmp/split_1.med", "maa1");
//     new_collection.getMesh()[1]->addDriver( MEDMEM::MED_DRIVER,"/tmp/split_2.med", "maa1");
//     new_collection.getMesh()[0]->write();
//     new_collection.getMesh()[1]->write();
//     new_collection.write( "/tmp/split" );
    
    //CPPUNIT_ASSERT( init_mesh.getName() == fusion.getName() );
    CPPUNIT_ASSERT( init_mesh.getSpaceDimension() == fusion.getSpaceDimension() );
    CPPUNIT_ASSERT( init_mesh.getMeshDimension() == fusion.getMeshDimension() );
    CPPUNIT_ASSERT( init_mesh.getIsAGrid() == fusion.getIsAGrid() );
    CPPUNIT_ASSERT( init_mesh.getNumberOfNodes() == fusion.getNumberOfNodes() );
    //CPPUNIT_ASSERT( init_mesh.getCoordinatesSystem() == fusion.() );
    CPPUNIT_ASSERT( init_mesh.getNumberOfTypes(MED_CELL) == fusion.getNumberOfTypes(MED_CELL) );
    for ( int i = 0; i < init_mesh.getNumberOfTypes(MED_CELL); ++i )
      CPPUNIT_ASSERT( init_mesh.getTypes(MED_CELL)[i] == fusion.getTypes(MED_CELL)[i] );
    CPPUNIT_ASSERT( init_mesh.getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS)
                    == fusion.getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS) );

//     CPPUNIT_ASSERT( init_mesh.() == fusion.() );
//     CPPUNIT_ASSERT( init_mesh.() == fusion.() );
//     CPPUNIT_ASSERT( init_mesh.() == fusion.() );
//     CPPUNIT_ASSERT( init_mesh.() == fusion.() );
//     CPPUNIT_ASSERT( init_mesh.() == fusion.() );
//     CPPUNIT_ASSERT( init_mesh.() == fusion.() );
  }

  delete new_topo;

}
