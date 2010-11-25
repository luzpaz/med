//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
// File      : MEDMEMTest_MeshFuse.cxx
// Created   : Mon Jul 13 10:35:22 2009
// Author    : Edward AGAPOV (eap)
//
#include "MEDMEMTest.hxx"
#include <cppunit/Message.h>
#include <cppunit/TestAssert.h>

#include "MEDMEM_Grid.hxx"
#include "MEDMEM_MeshFuse.hxx"
#include "MEDMEM_DriverFactory.hxx"

#include <vector>
#include <string>

using namespace MEDMEM;
using namespace MED_EN;
using namespace std;

namespace
{
  const char* bnd_elem_name = "Boundary elements";

  /*!
   * \brief Class allowing to add groups to GRID
   */
  struct _GRID : public GRID
  {
    _GRID(const vector<vector<double> >& xyz_array,
          const vector<string>&          coord_name,
          const vector<string>&          coord_unit):
      GRID( xyz_array, coord_name, coord_unit ) {}

    void addGroup(GROUP* g)
    {
      switch ( g->getEntity() ) {
      case MED_CELL: _groupCell.push_back( g ); break;
      case MED_FACE: _groupFace.push_back( g ); break;
      case MED_EDGE: _groupEdge.push_back( g ); break;
      case MED_NODE: _groupNode.push_back( g ); break;
      default :throw MEDEXCEPTION(LOCALIZED("Bad Entity !"));
      }
    }
  };
}

GRID* makeGrid(double step, double x0, double y0, int nb_x,  int nb_y, int nb_z = 0, double z0=0.)
{
  // make grid
  int dim = nb_z ? 3 : 2;
  vector<vector<double> > xyz_array(dim);
  vector<string>          coord_name(dim);
  vector<string>          unit(dim);

  double origin[] = { x0, y0, z0 };
  int    size  [] = { nb_x, nb_y, nb_z };
  char name = 'X';
  for ( int d = 0; d < dim; ++d, ++name )
  {
    vector<double>& coords = xyz_array[d];
    coords.reserve( size[d]+1 );
    coords.push_back( origin[d] );
    for ( int i = 0; i < size[d]; ++i )
      coords.push_back( coords.back() + step );

    coord_name.push_back(string(1,name));
    unit.push_back("m");
  }
  _GRID* grid = new _GRID( xyz_array, coord_name, unit );

//   if (dim==3)
//   {
//     grid->addDriver( MED_DRIVER, STRING("/dn25/salome/eap/salome/misc/tmp/grid3D_")
//                      << int(grid) <<".med", "grid");
//     grid->writeUnstructured();
//   }
  // make group of boundary elems

  vector<int> bnd_elems;
  medEntityMesh bnd_entity;
  if ( dim == 3 )
  {
    bnd_entity = MED_FACE;
    for ( int x = 0; x < nb_x; ++x )
      for ( int y = 0; y < nb_y; ++y )
      {
        bnd_elems.push_back( grid->getFaceNumber( 3, x, y, 0    ));
        bnd_elems.push_back( grid->getFaceNumber( 3, x, y, nb_z ));
      }
    for ( int x = 0; x < nb_x; ++x )
      for ( int z = 0; z < nb_z; ++z )
      {
        bnd_elems.push_back( grid->getFaceNumber( 2, x, 0,    z ));
        bnd_elems.push_back( grid->getFaceNumber( 2, x, nb_y, z ));
      }
    for ( int y = 0; y < nb_y; ++y )
      for ( int z = 0; z < nb_z; ++z )
      {
        bnd_elems.push_back( grid->getFaceNumber( 1, 0,    y, z ));
        bnd_elems.push_back( grid->getFaceNumber( 1, nb_x, y, z ));
      }
  }
  else
  {
    bnd_entity = MED_EDGE;
    for ( int x = 0; x < nb_x; ++x )
    {
      bnd_elems.push_back( grid->getEdgeNumber( 1, x, 0 ));
      bnd_elems.push_back( grid->getEdgeNumber( 1, x, nb_y ));
    }
    for ( int y = 0; y < nb_y; ++y )
    {
      bnd_elems.push_back( grid->getEdgeNumber( 2, y, 0 ));
      bnd_elems.push_back( grid->getEdgeNumber( 2, y, nb_x ));
    }
  }
  medGeometryElement types[] = { dim == 2 ? MED_SEG2 : MED_QUAD4 };
  int nb_elems[] = { bnd_elems.size() };
  GROUP* bnd_group = new GROUP;
  bnd_group->setMesh( grid );
  bnd_group->setEntity( bnd_entity );
  bnd_group->setName(bnd_elem_name);
  bnd_group->setpartial("Bnd",/*NumberOfGeometricType=*/1,
                         bnd_elems.size(), types,
                         nb_elems, & bnd_elems[0] );
  grid->addGroup( bnd_group );

  // make group on all bnd_entity's
  GROUP* all_bnd = new GROUP;
  all_bnd->setMesh( grid );
  all_bnd->setEntity( bnd_entity );
  all_bnd->setName(STRING("all bnd #")<<long(all_bnd));
  all_bnd->setAll( true );
  all_bnd->update();
  grid->addGroup( all_bnd );

  // compute families
  grid->createFamilies();

  return grid;
}

void MEDMEMTest::testMeshFuse()
{
  // fuse two grids:
  // first | second
  //
  //  5   6
  //   *---*
  //  3|  4| 10
  //   *---*---*
  //  1|  2|  9|
  //   *---*---*
  //      7|  8|
  //       *---*

  int gnl [] = {1,2,3,4,5,6 };
  int gnr [] = {7,8,2,9,4,10};
  const int nb_x = 1, nb_y = 2, nb_z = 3;

  // make grids
  double step = 10;
  GRID* grid_left  = makeGrid(step, 0,0,       nb_x,nb_y);
  GRID* grid_right = makeGrid(step, step,-step,nb_x,nb_y);

  // global node numbers
  vector<int> glob_nb_left ( gnl, ((int*)gnl) + 6 );
  vector<int> glob_nb_right( gnr, ((int*)gnr) + 6 );

  // fuse two 2D grids
  {
    MeshFuse fusion;
    fusion.concatenate(grid_left, glob_nb_left);
    fusion.concatenate(grid_right, glob_nb_right);

    //   DRIVERFACTORY::setMedFileVersionForWriting( V21 );
    //   fusion.addDriver( MED_DRIVER, "/tmp/fusion.med", "fusion");
    //   fusion.write();

    const medEntityMesh sub_entity = MED_EDGE;

    // check number of nodes and elements
    const int nb_common_nodes = 2;
    const int expect_nb_nodes = 2 * grid_left->getNumberOfNodes() - nb_common_nodes;
    CPPUNIT_ASSERT_EQUAL( expect_nb_nodes, fusion.getNumberOfNodes());

    const int expect_nb_cells = 2 * grid_left->getNumberOfElements(MED_CELL, MED_ALL_ELEMENTS);
    CPPUNIT_ASSERT_EQUAL( expect_nb_cells, fusion.getNumberOfElements(MED_CELL, MED_ALL_ELEMENTS));

    CPPUNIT_ASSERT_EQUAL( 7, grid_left->getNumberOfElements( sub_entity, MED_ALL_ELEMENTS ));

    const int nb_common_edges = 1;
    const int expect_nb_edges =
      2 * grid_left->getNumberOfElements(sub_entity, MED_ALL_ELEMENTS) - nb_common_edges;
    CPPUNIT_ASSERT_EQUAL( expect_nb_edges, fusion.getNumberOfElements(sub_entity, MED_ALL_ELEMENTS));

    // check groups
    const GROUP* bnd_edges = 0;
    CPPUNIT_ASSERT_NO_THROW( bnd_edges = fusion.getGroup( bnd_elem_name ));
    CPPUNIT_ASSERT( !bnd_edges->isOnAllElements() );

    const int nb_bnd_edges =
      grid_left->getGroup( bnd_elem_name )->getNumberOfElements(MED_ALL_ELEMENTS);
    const int expect_nb_bnd = 2 * nb_bnd_edges - nb_common_edges;
    CPPUNIT_ASSERT_EQUAL( expect_nb_bnd, bnd_edges->getNumberOfElements(MED_ALL_ELEMENTS));

    // check families
    set<string> oldFamNames;
    vector<FAMILY*> fams = grid_left->getFamilies(sub_entity);
    for (int i = 0; i< fams.size(); ++i ) oldFamNames.insert( fams[i]->getName());
    fams = grid_right->getFamilies(sub_entity);
    for (int i = 0; i< fams.size(); ++i ) oldFamNames.insert( fams[i]->getName());
    CPPUNIT_ASSERT_EQUAL( int(oldFamNames.size()), fusion.getNumberOfFamilies( sub_entity ));
  }
  grid_left->removeReference();
  grid_right->removeReference();


  // Fuse two 3D grids

  grid_left  = makeGrid(step, 0,0,       nb_x,nb_y, nb_z,0.);
  grid_right = makeGrid(step, step,-step,nb_x,nb_y, nb_z,step);

  {
    // global node numbers
    int nb_nodes = grid_left->getNumberOfNodes();
    glob_nb_left.resize( nb_nodes );
    glob_nb_right.resize( nb_nodes );
    for ( int n = 0; n < nb_nodes; ++n )
    {
      glob_nb_left[ n ] = n;
      glob_nb_right[ n ] = n + nb_nodes;
    }
    for ( int z = 0; z < nb_z; ++z )
      for ( int y = 1; y <= nb_y; ++y )
        glob_nb_right[ grid_right->getNodeNumber(0,y,z)-1 ] =
          glob_nb_left[ grid_left->getNodeNumber(1,y-1,z+1)-1 ];

    MeshFuse fusion;
    fusion.concatenate(grid_left,  glob_nb_left);
    fusion.concatenate(grid_right, glob_nb_right);

    //   DRIVERFACTORY::setMedFileVersionForWriting( V21 );
    //   fusion.addDriver( MED_DRIVER, "/tmp/fusion.med", "fusion");
    //   fusion.write();

    const medEntityMesh sub_entity = MED_FACE;

    // check number of nodes and elements
    const int nb_common_nodes = 6;
    const int expect_nb_nodes = 2 * grid_left->getNumberOfNodes() - nb_common_nodes;
    CPPUNIT_ASSERT_EQUAL( expect_nb_nodes, fusion.getNumberOfNodes());

    const int expect_nb_cells = 2 * grid_left->getNumberOfElements(MED_CELL, MED_ALL_ELEMENTS);
    CPPUNIT_ASSERT_EQUAL( expect_nb_cells, fusion.getNumberOfElements(MED_CELL, MED_ALL_ELEMENTS));

    const int grid_nb_faces =
      nb_x * nb_y * (nb_z+1)+
      nb_x * (nb_y+1) * nb_z +
      (nb_x+1) * nb_y * nb_z;
    CPPUNIT_ASSERT_EQUAL( grid_nb_faces,
                          grid_left->getNumberOfElements( sub_entity, MED_ALL_ELEMENTS ));

    const int nb_common_faces = 2;
    const int expect_nb_faces =
      2 * grid_left->getNumberOfElements(sub_entity, MED_ALL_ELEMENTS) - nb_common_faces;
    CPPUNIT_ASSERT_EQUAL( expect_nb_faces,
                          fusion.getNumberOfElements(sub_entity, MED_ALL_ELEMENTS));

    // check groups
    const GROUP* bnd_faces = 0;
    CPPUNIT_ASSERT_NO_THROW( bnd_faces = fusion.getGroup( bnd_elem_name ));
    CPPUNIT_ASSERT( !bnd_faces->isOnAllElements() );

    const int nb_bnd_faces =
      grid_left->getGroup( bnd_elem_name )->getNumberOfElements(MED_ALL_ELEMENTS);
    const int expect_nb_bnd = 2 * nb_bnd_faces - nb_common_faces;
    CPPUNIT_ASSERT_EQUAL( expect_nb_bnd, bnd_faces->getNumberOfElements(MED_ALL_ELEMENTS));

    // check families
    CPPUNIT_ASSERT_EQUAL( 3, fusion.getNumberOfFamilies( sub_entity ));
  }



  // Fuse one 3D grid and another converted to polygons

  grid_right->convertToPoly();
  CPPUNIT_ASSERT_EQUAL( 0, grid_right->getNumberOfElements(MED_CELL, MED_QUAD4));
  {
    MeshFuse fusion;
    fusion.concatenate(grid_left,  glob_nb_left);
    fusion.concatenate(grid_right, glob_nb_right);

    const medEntityMesh sub_entity = MED_FACE;

    // check number of nodes and elements
    const int nb_common_nodes = 6;
    const int expect_nb_nodes = 2 * grid_left->getNumberOfNodes() - nb_common_nodes;
    CPPUNIT_ASSERT_EQUAL( expect_nb_nodes, fusion.getNumberOfNodes());

    const int expect_nb_cells = 2 * grid_left->getNumberOfElements(MED_CELL, MED_ALL_ELEMENTS);
    CPPUNIT_ASSERT_EQUAL( expect_nb_cells,
                          fusion.getNumberOfElementsWithPoly(MED_CELL, MED_ALL_ELEMENTS));

    const int grid_nb_faces =
      nb_x * nb_y * (nb_z+1)+
      nb_x * (nb_y+1) * nb_z +
      (nb_x+1) * nb_y * nb_z;
    CPPUNIT_ASSERT_EQUAL( grid_nb_faces,
                          grid_left->getNumberOfElements( sub_entity, MED_ALL_ELEMENTS ));

    const int nb_common_faces = 2;
    const int expect_nb_faces =
      2 * grid_left->getNumberOfElements(sub_entity, MED_ALL_ELEMENTS) - nb_common_faces;
    CPPUNIT_ASSERT_EQUAL( expect_nb_faces,
                          fusion.getNumberOfElementsWithPoly(sub_entity, MED_ALL_ELEMENTS));

    // check groups
    const GROUP* bnd_faces = 0;
    CPPUNIT_ASSERT_NO_THROW( bnd_faces = fusion.getGroup( bnd_elem_name ));
    CPPUNIT_ASSERT( !bnd_faces->isOnAllElements() );

    const int nb_bnd_faces =
      grid_left->getGroup( bnd_elem_name )->getNumberOfElements(MED_ALL_ELEMENTS);
     // convertToPoly does not convert groups and families
    const int expect_nb_bnd = 2 * nb_bnd_faces/* - nb_common_faces*/;
    CPPUNIT_ASSERT_EQUAL( expect_nb_bnd, bnd_faces->getNumberOfElements(MED_ALL_ELEMENTS));

    // check families
    CPPUNIT_ASSERT_EQUAL( 3, fusion.getNumberOfFamilies( sub_entity ));
  }

  grid_left->removeReference();
  grid_right->removeReference();

}