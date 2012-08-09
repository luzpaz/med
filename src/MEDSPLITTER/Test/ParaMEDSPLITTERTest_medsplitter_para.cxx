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
// File      : ParaMEDSPLITTERTest_medsplitter_para.cxx
// Created   : Fri Jul 31 12:35:44 2009
// Author    : Edward AGAPOV (eap)
//

#include "ParaMEDSPLITTERTest.hxx"
#include "MEDSPLITTERTest_Utils.hxx"

#include "MEDSPLITTER_ParaDomainSelector.hxx"
#include "MEDSPLITTER_MESHCollection.hxx"
#include "MEDSPLITTER_ParaDomainSelector.hxx"
#include "MEDSPLITTER_Topology.hxx"
//#include "MEDSPLITTER_API.hxx"

#include <MEDMEM_Mesh.hxx>
#include <MEDMEM_PointerOf.hxx>

using namespace std;
using namespace MEDSPLITTER;
using namespace MED_EN;

//================================================================================
/*!
 * \brief call test_medsplitter_para with 2 files: blade.med et pointe.med
 */
//================================================================================

void ParaMEDSPLITTERTest::functional_validation()
{
  //test_medsplitter_para( "/dn25/salome/eap/salome/misc/tmp/meshing_REsplit1.med", "meshing_1");
  test_medsplitter_para( MEDSPLITTERTest_Utils::getResourceFile("TimeStamps.med"), "dom");
  test_medsplitter_para( MEDSPLITTERTest_Utils::getResourceFile("square1.med"), "Mesh_2");
  test_medsplitter_para( MEDSPLITTERTest_Utils::getResourceFile("pointe.med"), "maa1");
}

//================================================================================
/*!
 * \brief La validation consistera à prendre un maillage, à le découper en deux,
 * à passer de deux à trois sous-domaines, puis à revenir vers un sous-domaine.
 */
//================================================================================

void ParaMEDSPLITTERTest::test_medsplitter_para( const string& med_file, const string& meshname )
{
  ParaDomainSelector dom_sel;

  string tmp_dir = MEDSPLITTERTest_Utils::getTmpDirectory();
  // if splitter is running on different hosts, assure tmp_dir to be accessible from all procs
  bool diff_hosts = dom_sel.isOnDifferentHosts();
  if ( diff_hosts )
    if ( getenv("HOME"))
      tmp_dir = getenv("HOME");

  string file_2 = tmp_dir + "/test_medsplitter_para_2" ;
  string file_3 = tmp_dir + "/test_medsplitter_para_3" ;
  string file_1 = tmp_dir + "/test_medsplitter_para_1" ;
  string file_3c= tmp_dir + "/test_medsplitter_para_3c" ;
  MEDMEM::STRING rm_cmd("rm "); rm_cmd << tmp_dir << "/test_medsplitter_para_*";

  MPI_Barrier( MPI_COMM_WORLD ); // avoid removing files being read

  // remove all old (master and med) files
  if ( dom_sel.rank() == 0 )
    system( rm_cmd );

  // découper en deux
  if ( dom_sel.rank() == 0 )
  {
    MESHCollection collection_1(med_file,meshname);
    auto_ptr<Topology> new_topo_2( collection_1.createPartition( 2, Graph::SCOTCH ));

    MESHCollection collection_2w( collection_1, new_topo_2.get());
    collection_2w.setDriverType(MEDSPLITTER::MedAscii);
    collection_2w.write( file_2 );
  }
  MPI_Barrier( MPI_COMM_WORLD ); // wait for master file_2

  // passer de deux à trois sous-domaines
  MESHCollection collection_2r(file_2,dom_sel);
  auto_ptr<Topology> new_topo_3( collection_2r.createPartition( 3, Graph::METIS ));

  MESHCollection collection_3w( collection_2r, new_topo_3.get());
  collection_3w.setDriverType(MEDSPLITTER::MedAscii);
  collection_3w.write( file_3 );

  // check global face numbers of collection_3w
  {
    int total_nb_faces = 0;
    for ( int idomain=0; idomain < collection_3w.getMesh().size(); ++idomain )
      total_nb_faces += collection_3w.getMesh()[idomain]->getNumberOfElements(collection_3w.getSubEntity(), MED_ALL_ELEMENTS );

    for ( int idomain=0; idomain < collection_3w.getMesh().size(); ++idomain )
    {
      int nb_dom_faces = new_topo_3->getFaceNumber(idomain);
      MEDMEM::PointerOf<int> glob_ids( nb_dom_faces );
      new_topo_3->getFaceList(idomain, glob_ids);
      for ( int i = 0; i < nb_dom_faces; ++i )
        if ( glob_ids[i] < 1 || glob_ids[i] > total_nb_faces )
          CPPUNIT_FAIL(MEDMEM::STRING("Invalid global face id: ")<< glob_ids[i]);
    }
  }
  
  MPI_Barrier( MPI_COMM_WORLD ); // wait for master file_3

  // revenir vers un sous-domaine
  MESHCollection collection_3r(file_3,dom_sel);
  auto_ptr<Topology> new_topo_1( collection_3r.createPartition( 1, Graph::METIS ));

  MESHCollection collection_1w( collection_3r, new_topo_1.get());
  collection_1w.setDriverType(MEDSPLITTER::MedAscii);
  collection_1w.write( file_1 );

  // compare initial and final mesh

  if ( dom_sel.getProccessorID(0) == dom_sel.rank() )
  {
    MEDMEM::MESH init_mesh( MEDMEM::MED_DRIVER, med_file, meshname);
    MEDMEM::MESH& res_mesh = * collection_1w.getMesh()[0];

    // nb nodes
    int i_nb_nodes = init_mesh.getNumberOfNodes();
    int r_nb_nodes = res_mesh.getNumberOfNodes();
    CPPUNIT_ASSERT_EQUAL( i_nb_nodes, r_nb_nodes );

    // coord
    string i_coo_sys = init_mesh.getCoordinatesSystem();
    string r_coo_sys = res_mesh.getCoordinatesSystem();
    CPPUNIT_ASSERT_EQUAL( i_coo_sys, r_coo_sys );

    // types and nb elements
    int i_nb_types = init_mesh.getNumberOfTypes(MED_CELL);
    int r_nb_types = res_mesh.getNumberOfTypes(MED_CELL);
    CPPUNIT_ASSERT_EQUAL( i_nb_types, r_nb_types );
    {
      const medGeometryElement* i_types = init_mesh.getTypes(MED_CELL);
      const medGeometryElement* r_types = res_mesh.getTypes(MED_CELL);
      for ( int i=0; i<i_nb_types; ++i )
        CPPUNIT_ASSERT_EQUAL( i_types[i], r_types[i] );
      const int * i_nbs = init_mesh.getGlobalNumberingIndex( MED_CELL );
      const int * r_nbs = res_mesh.getGlobalNumberingIndex( MED_CELL );
      for ( int i=0; i<i_nb_types; ++i )
        CPPUNIT_ASSERT_EQUAL( i_nbs[i], r_nbs[i] );
    }
    i_nb_types = init_mesh.getNumberOfTypes(MED_FACE);
    r_nb_types = res_mesh.getNumberOfTypes(MED_FACE);
    CPPUNIT_ASSERT_EQUAL( i_nb_types, r_nb_types );
    if ( i_nb_types )
    {
      const medGeometryElement* i_types = init_mesh.getTypes(MED_FACE);
      const medGeometryElement* r_types = res_mesh.getTypes(MED_FACE);
      for ( int i=0; i<i_nb_types; ++i )
        CPPUNIT_ASSERT_EQUAL( i_types[i], r_types[i] );
      const int * i_nbs = init_mesh.getGlobalNumberingIndex( MED_FACE );
      const int * r_nbs = res_mesh.getGlobalNumberingIndex( MED_FACE );
      for ( int i=0; i<i_nb_types; ++i )
        CPPUNIT_ASSERT_EQUAL( i_nbs[i], r_nbs[i] );
    }
    i_nb_types = init_mesh.getNumberOfTypes(MED_EDGE);
    r_nb_types = res_mesh.getNumberOfTypes(MED_EDGE);
    CPPUNIT_ASSERT_EQUAL( i_nb_types, r_nb_types );
    if ( i_nb_types )
    {
      const medGeometryElement* i_types = init_mesh.getTypes(MED_EDGE);
      const medGeometryElement* r_types = res_mesh.getTypes(MED_EDGE);
      for ( int i=0; i<i_nb_types; ++i )
        CPPUNIT_ASSERT_EQUAL( i_types[i], r_types[i] );
      const int * i_nbs = init_mesh.getGlobalNumberingIndex( MED_EDGE );
      const int * r_nbs = res_mesh.getGlobalNumberingIndex( MED_EDGE );
      for ( int i=0; i<i_nb_types; ++i )
        CPPUNIT_ASSERT_EQUAL( i_nbs[i], r_nbs[i] );
    }
  }

  // Check C API and create_boundary_faces
//   medsplitter_para( file_2.c_str(),
//                     file_3c.c_str(), 
//                     /*nprocs                =*/ 3,
//                     /*method                =*/ 0,
//                     /*create_boundary_faces =*/ true,
//                     /*family_splitting      =*/ false);

//   CPPUNIT_ASSERT( access(file_3c.data(), F_OK) == 0);

  MPI_Barrier( MPI_COMM_WORLD ); // avoid removing files being read

  // remove all (master and med) files
  if ( dom_sel.rank() == 0 )
    system( rm_cmd );
}
