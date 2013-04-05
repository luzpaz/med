// Copyright (C) 2007-2013  CEA/DEN, EDF R&D
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
// Author : Anthony Geay (CEA/DEN)

#include "MEDCouplingBasicsTest.hxx"
#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingCMesh.hxx"
#include "MEDCouplingExtrudedMesh.hxx"
#include "MEDCouplingFieldDouble.hxx"
#include "MEDCouplingMemArray.hxx"
#include "MEDCouplingMultiFields.hxx"

void CppExample_MEDCouplingPointSet_getCoordsAt()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingCMesh_getCoordsAt_1]
  const double coords[3] = {1.,2.,4.};
  DataArrayDouble* x = DataArrayDouble::New();
  x->useExternalArrayWithRWAccess( coords, 3, 1 );
  MEDCouplingCMesh *mesh=MEDCouplingCMesh::New();
  mesh->setCoordsAt(0,x);
  const DataArrayDouble* x2=mesh->getCoordsAt(0);
  CPPUNIT_ASSERT( x2->isEqual( *x, 1e-13 ));
  //! [CppSnippet_MEDCouplingCMesh_getCoordsAt_1]
}

void CppExample_MEDCouplingUMesh_areCellsIncludedIn()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_areCellsIncludedIn_1]
  MEDCouplingUMesh *mesh1=MEDCouplingUMesh::New();
  mesh1->setMeshDimension(2);
  mesh1->allocateCells(5);
  const int conn[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
  mesh1->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn);    // #0
  mesh1->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+4);  // #1
  mesh1->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+7);  // #2
  mesh1->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+10); // #3
  mesh1->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+14); // #4
  mesh1->finishInsertingCells();
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->alloc(9,2);
  const double coords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
  std::copy(coords,coords+18,coordsArr->getPointer());
  mesh1->setCoords(coordsArr);
  //! [CppSnippet_MEDCouplingUMesh_areCellsIncludedIn_1]
  //! [CppSnippet_MEDCouplingUMesh_areCellsIncludedIn_2]
  const int cells2[3] = { 4,2,0 }; // even cells selected
  MEDCouplingUMesh* mesh2 = (MEDCouplingUMesh*) mesh1->buildPartOfMySelf( cells2, cells2+3, true );
  //! [CppSnippet_MEDCouplingUMesh_areCellsIncludedIn_2]
  //! [CppSnippet_MEDCouplingUMesh_areCellsIncludedIn_3]
  int compType = 0; // the strongest policy
  DataArrayInt *corr2to1, *corr1to2;
  // a larger mesh1 includes a smaller mesh2
  CPPUNIT_ASSERT( mesh1->areCellsIncludedIn( mesh2, compType, corr2to1 ));
  CPPUNIT_ASSERT( std::equal( cells2, cells2+3, corr2to1->getConstPointer() ));
  //! [CppSnippet_MEDCouplingUMesh_areCellsIncludedIn_3]
  //! [CppSnippet_MEDCouplingUMesh_areCellsIncludedIn_4]
  // the smaller mesh2 does NOT include the larger mesh1
  CPPUNIT_ASSERT( ! mesh2->areCellsIncludedIn( mesh1, compType, corr1to2 ));
  const int corr1to2Expected[5] = {2, 3, 1, 4, 0};
  CPPUNIT_ASSERT(std::equal( corr1to2Expected, corr1to2Expected+5, corr1to2->getConstPointer() ));
  //! [CppSnippet_MEDCouplingUMesh_areCellsIncludedIn_4]
  mesh1->decrRef();
  mesh2->decrRef();
  corr2to1->decrRef();
  corr1to2->decrRef();
}

void CppExample_MEDCouplingUMesh_findAndCorrectBadOriented3DExtrudedCells()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_findAndCorrectBadOriented3DExtrudedCells_1]
  // 2D coordinates of 5 base nodes
  const double coords[5*2]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2 };
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->useExternalArrayWithRWAccess( coords, 5, 2 );
  // coordinates of 5 top nodes
  DataArrayDouble *coordsArr2 = coordsArr->deepCpy();
  // 3D coordinates of base + top nodes
  coordsArr  = coordsArr-> changeNbOfComponents( 3, 0 );
  coordsArr2 = coordsArr2->changeNbOfComponents( 3, 1 );
  coordsArr = DataArrayDouble::Aggregate( coordsArr, coordsArr2 );
  // mesh
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setCoords(coordsArr);coordsArr->decrRef();
  mesh->setMeshDimension(3);
  mesh->allocateCells(2);
  // connectivity of reversed HEXA8 and PENTA6
  const int conn[8+6]={0,1,4,3, 5,6,9,8, 1,2,4, 6,7,9};
  mesh->insertNextCell(INTERP_KERNEL::NORM_HEXA8, 8,conn+0);
  mesh->insertNextCell(INTERP_KERNEL::NORM_PENTA6,6,conn+8);
  mesh->finishInsertingCells();
  //! [CppSnippet_MEDCouplingUMesh_findAndCorrectBadOriented3DExtrudedCells_1]
  //! [CppSnippet_MEDCouplingUMesh_findAndCorrectBadOriented3DExtrudedCells_2]
  DataArrayInt* fixedCells = mesh->findAndCorrectBadOriented3DExtrudedCells();
  CPPUNIT_ASSERT( fixedCells->getNumberOfTuples() == 2 ); // 2 cells fixed
  fixedCells = mesh->findAndCorrectBadOriented3DExtrudedCells();
  CPPUNIT_ASSERT( fixedCells->getNumberOfTuples() == 0 ); // no bad cells
  //! [CppSnippet_MEDCouplingUMesh_findAndCorrectBadOriented3DExtrudedCells_2]
}

void CppExample_MEDCouplingUMesh_arePolyhedronsNotCorrectlyOriented()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_arePolyhedronsNotCorrectlyOriented_1]
  // 2D coordinates of 5 base nodes
  const double coords[5*2]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2 };
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->useExternalArrayWithRWAccess( coords, 5, 2 );
  // coordinates of 5 top nodes
  DataArrayDouble *coordsArr2 = coordsArr->deepCpy();
  // 3D coordinates of base + top nodes
  coordsArr  = coordsArr-> changeNbOfComponents( 3, 0 );
  coordsArr2 = coordsArr2->changeNbOfComponents( 3, 1 );
  coordsArr = DataArrayDouble::Aggregate( coordsArr, coordsArr2 );
  // mesh
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setCoords(coordsArr);coordsArr->decrRef();
  mesh->setMeshDimension(3);
  mesh->allocateCells(2);
  // connectivity of a HEXA8 + a reversed PENTA6
  const int conn[8+6]={0,3,4,1, 5,8,9,6, 1,2,4, 6,7,9};
  mesh->insertNextCell(INTERP_KERNEL::NORM_POLYHED,8,conn); //  "extruded" polyhedron
  mesh->insertNextCell(INTERP_KERNEL::NORM_POLYHED,6,conn+8);
  mesh->finishInsertingCells();
  // fix connectivity of NORM_POLYHED's
  mesh->convertExtrudedPolyhedra();
  //! [CppSnippet_MEDCouplingUMesh_arePolyhedronsNotCorrectlyOriented_1]
  //! [CppSnippet_MEDCouplingUMesh_arePolyhedronsNotCorrectlyOriented_2]
  std::vector<int> badCellIds;
  mesh->arePolyhedronsNotCorrectlyOriented( badCellIds );
  CPPUNIT_ASSERT( badCellIds.size() == 1 ); //  one polyhedron is KO
  // fix invalid rolyherdons
  mesh->orientCorrectlyPolyhedrons();
  // re-check orientation
  badCellIds.clear(); // as badCellIds is not cleared by arePolyhedronsNotCorrectlyOriented()
  mesh->arePolyhedronsNotCorrectlyOriented( badCellIds );
  CPPUNIT_ASSERT( badCellIds.size() == 0 ); // connectivity is OK
  //! [CppSnippet_MEDCouplingUMesh_arePolyhedronsNotCorrectlyOriented_2]
}

void CppExample_MEDCouplingUMesh_are2DCellsNotCorrectlyOriented()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_are2DCellsNotCorrectlyOriented_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  mesh->allocateCells(5);
  const int conn[18]={0,3,4,1, 1,2,4, 4,5,2, 6,7,4,3, 7,8,5,4};
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn);    // 0
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+4);  // 1
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+7);  // 2
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+10); // 3
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+14); // 4
  mesh->finishInsertingCells();
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->alloc(9,2);
  const double coords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
  std::copy(coords,coords+18,coordsArr->getPointer());
  mesh->setCoords(coordsArr);
  mesh->changeSpaceDimension(3);
  //! [CppSnippet_MEDCouplingUMesh_are2DCellsNotCorrectlyOriented_1]
  //! [CppSnippet_MEDCouplingUMesh_are2DCellsNotCorrectlyOriented_2]
  const double vec[3] = {0.,0.,-1.};
  std::vector<int> badCellIds;
  mesh->are2DCellsNotCorrectlyOriented( vec, false, badCellIds );
  CPPUNIT_ASSERT( badCellIds.size() == 1 ); //  one cell is reversed
  // fix orientation
  mesh->orientCorrectly2DCells( vec, false );
  // re-check orientation
  badCellIds.clear(); // as badCellIds is not cleared by are2DCellsNotCorrectlyOriented()
  mesh->are2DCellsNotCorrectlyOriented( vec, false, badCellIds );
  CPPUNIT_ASSERT( badCellIds.size() == 0 ); // the orientation is OK
  //! [CppSnippet_MEDCouplingUMesh_are2DCellsNotCorrectlyOriented_2]
}

void CppExample_MEDCouplingUMesh_getCellsContainingPoints()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_getCellsContainingPoints_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  mesh->allocateCells(5);
  const int conn[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn);    // 0
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+4);  // 1
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+7);  // 2
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+10); // 3
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+14); // 4
  mesh->finishInsertingCells();
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->alloc(9,2);
  const double coords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
  std::copy(coords,coords+18,coordsArr->getPointer());
  mesh->setCoords(coordsArr);
  //! [CppSnippet_MEDCouplingUMesh_getCellsContainingPoints_1]
  //! [CppSnippet_MEDCouplingUMesh_getCellsContainingPoints_2]
  const double pos[3*2] = { 10., 10,               // point out of the mesh
                            0.3, 0.3,              // point located somewhere inside the mesh
                            coords[2], coords[3]}; // point at the node #1
  const double eps = 1e-4; // ball radius
  std::vector<int> cells, cellsIndex;
  mesh->getCellsContainingPoints( pos, 3, eps, cells, cellsIndex );
  const int cellsExpected[3]={4, 0, 1};
  const int cellsIndexExpected[4]={0, 0, 1, 3};
  CPPUNIT_ASSERT(std::equal( cellsExpected,      cellsExpected+3,      &cells[0]));
  CPPUNIT_ASSERT(std::equal( cellsIndexExpected, cellsIndexExpected+4, &cellsIndex[0]));
  //! [CppSnippet_MEDCouplingUMesh_getCellsContainingPoints_2]
}

void CppExample_MEDCouplingUMesh_getCellsContainingPoint()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_getCellsContainingPoint_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  mesh->allocateCells(5);
  const int conn[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn);    // 0
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+4);  // 1
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+7);  // 2
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+10); // 3
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+14); // 4
  mesh->finishInsertingCells();
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->alloc(9,2);
  const double coords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
  std::copy(coords,coords+18,coordsArr->getPointer());
  mesh->setCoords(coordsArr);
  //! [CppSnippet_MEDCouplingUMesh_getCellsContainingPoint_1]
  //! [CppSnippet_MEDCouplingUMesh_getCellsContainingPoint_2]
  const double* coords4  = coords + 4*2; // coordinates of the node #4
  const double eps = 1e-4; // ball radius
  const double pos[2] = { coords4[0] + eps, coords4[1] - eps }; // ball center
  std::vector<int> cellIds;
  mesh->getCellsContainingPoint( pos, eps, cellIds );
  CPPUNIT_ASSERT ( cellIds.size() == mesh->getNumberOfCells() );
  //! [CppSnippet_MEDCouplingUMesh_getCellsContainingPoint_2]
}

void CppExample_MEDCouplingUMesh_buildPartOrthogonalField()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_buildPartOrthogonalField_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  mesh->allocateCells(5);
  const int conn[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn);    // 0
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+4);  // 1
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+7);  // 2
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+10); // 3
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+14); // 4
  mesh->finishInsertingCells();
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->alloc(9,2);
  const double coords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
  std::copy(coords,coords+18,coordsArr->getPointer());
  mesh->setCoords(coordsArr);
  //! [CppSnippet_MEDCouplingUMesh_buildPartOrthogonalField_1]
  //! [CppSnippet_MEDCouplingUMesh_buildPartOrthogonalField_2]
  const int part[4] = {1,2,3,4}; // cell #0 is omitted
  MEDCouplingFieldDouble* vecField=mesh->buildPartOrthogonalField( part, part+4 );
  CPPUNIT_ASSERT ( vecField->getArray()->getNumberOfTuples() == 4 );
  CPPUNIT_ASSERT ( vecField->getArray()->getNumberOfComponents() == 3 );
  //! [CppSnippet_MEDCouplingUMesh_buildPartOrthogonalField_2]
}

void CppExample_MEDCouplingUMesh_getPartMeasureField()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_getPartMeasureField_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  mesh->allocateCells(5);
  const int conn[18]={0,3,4,1, 1,2,4, 4,5,2, 6,7,4,3, 7,8,5,4};
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn);    // 0
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+4);  // 1
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+7);  // 2
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+10); // 3
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+14); // 4
  mesh->finishInsertingCells();
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->alloc(9,2);
  const double coords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
  std::copy(coords,coords+18,coordsArr->getPointer());
  mesh->setCoords(coordsArr);
  //! [CppSnippet_MEDCouplingUMesh_getPartMeasureField_1]
  //! [CppSnippet_MEDCouplingUMesh_getPartMeasureField_2]
  const bool isAbs = true;
  const int part[4] = {1,2,3,4}; // cell #0 is omitted
  DataArrayDouble* areaArr=mesh->getPartMeasureField( isAbs, part, part+4 );
  CPPUNIT_ASSERT( areaArr->getIJ(0,0) > 0 ); // orientation ignored
  areaArr=mesh->getPartMeasureField( !isAbs, part, part+4 );
  CPPUNIT_ASSERT( areaArr->getIJ(0,0) < 0 ); // orientation considered
  CPPUNIT_ASSERT ( areaArr->getNumberOfTuples() == 4 );
  //! [CppSnippet_MEDCouplingUMesh_getPartMeasureField_2]
  //! [CppSnippet_MEDCouplingUMesh_getPartMeasureField_3]
  const int cellIds[4] = {1,2,3,4}; // cell #0 is omitted
  DataArrayDouble* baryCenters=mesh->getPartBarycenterAndOwner( cellIds, cellIds+4 );
  CPPUNIT_ASSERT( baryCenters->getNumberOfTuples() == 4 );
  CPPUNIT_ASSERT( baryCenters->getNumberOfComponents() == mesh->getSpaceDimension() );
  //! [CppSnippet_MEDCouplingUMesh_getPartMeasureField_3]
}

void CppExample_MEDCouplingUMesh_getCellsInBoundingBox()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_getCellsInBoundingBox_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  mesh->allocateCells(1);
  const double coords[3*2]={0.,0., 0.,1., 1.,1};
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->useExternalArrayWithRWAccess(coords, 3,2);
  mesh->setCoords(coordsArr);
  mesh->allocateCells(1);
  const int conn[3]={0,1,2};
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3,conn);
  mesh->finishInsertingCells();
  //! [CppSnippet_MEDCouplingUMesh_getCellsInBoundingBox_1]
  //! [CppSnippet_MEDCouplingUMesh_getCellsInBoundingBox_2]
  const double bbox[] = {1., 1., 1.001,1.001}; // xMin, xMax, yMin, yMax
  DataArrayInt* cellIdsArr = mesh->getCellsInBoundingBox( bbox, 0.0 );
  CPPUNIT_ASSERT( cellIdsArr->getNumberOfTuples() == 0 );
  cellIdsArr = mesh->getCellsInBoundingBox( bbox, 0.1 );
  CPPUNIT_ASSERT( cellIdsArr->getNumberOfTuples() == 1 );
  //! [CppSnippet_MEDCouplingUMesh_getCellsInBoundingBox_2]
}

void CppExample_MEDCouplingUMesh_renumberNodesInConn()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_renumberNodesInConn_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  mesh->allocateCells(1);
  const int conn[4]={4,3,2,1};
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn);
  mesh->finishInsertingCells();
  //! [CppSnippet_MEDCouplingUMesh_renumberNodesInConn_1]
  //! [CppSnippet_MEDCouplingUMesh_renumberNodesInConn_2]
  const int old2newIds[] = {-1,3,2,1,0};
  mesh->renumberNodesInConn( old2newIds );
  const int nodes0Expected[] = {0,1,2,3};
  std::vector<int> nodes0;
  mesh->getNodeIdsOfCell( 0, nodes0 );
  CPPUNIT_ASSERT(std::equal( nodes0Expected, nodes0Expected+4, &nodes0[0] ));
  //! [CppSnippet_MEDCouplingUMesh_renumberNodesInConn_2]
}

void CppExample_MEDCouplingUMesh_renumberNodes()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_renumberNodes_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  const double coords[4*2]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.3};
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->useExternalArrayWithRWAccess(coords, 4,2);
  mesh->setCoords(coordsArr);
  mesh->allocateCells(0);
  mesh->finishInsertingCells();
  //! [CppSnippet_MEDCouplingUMesh_renumberNodes_1]
  //! [CppSnippet_MEDCouplingUMesh_renumberNodes_2]
  const int newIds[] = { 2,1,0,-1 };
  mesh->renumberNodes(newIds, 3);
  coordsArr = mesh->getCoords(); // get a shorten array
  const double coordsExpected[3*2]={0.7,-0.3, 0.2,-0.3, -0.3,-0.3};
  DataArrayDouble *coordsExpectedArr=DataArrayDouble::New();
  coordsExpectedArr->useExternalArrayWithRWAccess(coordsExpected, 3,2);
  CPPUNIT_ASSERT( coordsExpectedArr->isEqual( *coordsArr, 1e-13 ));
  //! [CppSnippet_MEDCouplingUMesh_renumberNodes_2]
  //! [CppSnippet_MEDCouplingUMesh_renumberNodes_3]
  coordsArr->useExternalArrayWithRWAccess(coords, 4,2); // restore old nodes
  const int newIds2[] = { 2,1,0,2 };
  mesh->renumberNodes2(newIds2, 3);
  coordsArr = mesh->getCoords(); // get a shorten array
  const double coordsExpected2[3*2]={0.7,-0.3, 0.2,-0.3, -0.3, 0.0};
  DataArrayDouble *coordsExpectedArr2=DataArrayDouble::New();
  coordsExpectedArr2->useExternalArrayWithRWAccess(coordsExpected, 3,2);
  CPPUNIT_ASSERT( coordsExpectedArr2->isEqual( *coordsArr, 1e-13 ));
  //! [CppSnippet_MEDCouplingUMesh_renumberNodes_3]
  coordsExpectedArr->decrRef();
  coordsExpectedArr2->decrRef();
  coordsArr->decrRef();
  mesh->decrRef();
}

void CppExample_MEDCouplingUMesh_findBoundaryNodes()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_findBoundaryNodes_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  mesh->allocateCells(5);
  const int conn[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn);    // 0
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+4);  // 1
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+7);  // 2
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+10); // 3
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+14); // 4
  mesh->finishInsertingCells();
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->alloc(9,2);
  const double coords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
  std::copy(coords,coords+18,coordsArr->getPointer());
  mesh->setCoords(coordsArr);
  //! [CppSnippet_MEDCouplingUMesh_findBoundaryNodes_1]
  //! [CppSnippet_MEDCouplingUMesh_findBoundaryNodes_2]
  DataArrayInt* nodeIdsArr=mesh->findBoundaryNodes();
  CPPUNIT_ASSERT( nodeIdsArr->getNumberOfTuples() == mesh->getNumberOfNodes() - 1 );
  //! [CppSnippet_MEDCouplingUMesh_findBoundaryNodes_2]
  nodeIdsArr->decrRef();
  coordsArr->decrRef();
  mesh->decrRef();
}

void CppExample_MEDCouplingUMesh_buildBoundaryMesh()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_buildBoundaryMesh_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  mesh->allocateCells(5);
  const int conn[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn);    // 0
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+4);  // 1
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+7);  // 2
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+10); // 3
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+14); // 4
  mesh->finishInsertingCells();
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->alloc(9,2);
  const double coords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
  std::copy(coords,coords+18,coordsArr->getPointer());
  mesh->setCoords(coordsArr);
  //! [CppSnippet_MEDCouplingUMesh_buildBoundaryMesh_1]
  //! [CppSnippet_MEDCouplingUMesh_buildBoundaryMesh_2]
  MEDCouplingPointSet* mesh1=mesh->buildBoundaryMesh(true);
  MEDCouplingPointSet* mesh2=mesh->buildBoundaryMesh(false);
  CPPUNIT_ASSERT(  coordsArr->isEqual( *mesh1->getCoords(), 1e-13 )); // same nodes
  CPPUNIT_ASSERT( !coordsArr->isEqual( *mesh2->getCoords(), 1e-13 )); // different nodes
  //! [CppSnippet_MEDCouplingUMesh_buildBoundaryMesh_2]
  coordsArr->decrRef();
  mesh->decrRef();
  mesh1->decrRef();
  mesh2->decrRef();
}

void CppExample_MEDCouplingUMesh_buildFacePartOfMySelfNode()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_buildFacePartOfMySelfNode_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  mesh->allocateCells(5);
  const int conn[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn);    // 0
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+4);  // 1
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+7);  // 2
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+10); // 3
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+14); // 4
  mesh->finishInsertingCells();
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->alloc(9,2);
  const double coords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
  std::copy(coords,coords+18,coordsArr->getPointer());
  mesh->setCoords(coordsArr);
  coordsArr->decrRef();
  //! [CppSnippet_MEDCouplingUMesh_buildFacePartOfMySelfNode_1]
  //! [CppSnippet_MEDCouplingUMesh_buildFacePartOfMySelfNode_2]
  std::vector<int> nodes;
  mesh->getNodeIdsOfCell( 0, nodes );
  const bool allNodes = true;
  MEDCouplingUMesh* mesh1 = (MEDCouplingUMesh*)mesh->buildFacePartOfMySelfNode( &nodes[0], &nodes[0]+nodes.size(), allNodes);
  CPPUNIT_ASSERT( mesh1->getNumberOfCells() == 4 ); // 4 segments bounding QUAD4 #0 only
  MEDCouplingUMesh* mesh2 = (MEDCouplingUMesh*)mesh->buildFacePartOfMySelfNode( &nodes[0], &nodes[0]+nodes.size(),!allNodes);
  CPPUNIT_ASSERT( mesh2->getNumberOfCells() == 9 ); // more segments added
  //! [CppSnippet_MEDCouplingUMesh_buildFacePartOfMySelfNode_2]
  mesh->decrRef();
  mesh1->decrRef();
  mesh2->decrRef();
}

void CppExample_MEDCouplingUMesh_buildPartOfMySelfNode()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_buildPartOfMySelfNode_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  mesh->allocateCells(5);
  const int conn[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn);    // 0
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+4);  // 1
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+7);  // 2
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+10); // 3
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+14); // 4
  mesh->finishInsertingCells();
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->alloc(9,2);
  const double coords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
  std::copy(coords,coords+18,coordsArr->getPointer());
  mesh->setCoords(coordsArr);
  coordsArr->decrRef();
  //! [CppSnippet_MEDCouplingUMesh_buildPartOfMySelfNode_1]
  //! [CppSnippet_MEDCouplingUMesh_buildPartOfMySelfNode_2]
  std::vector<int> nodes;
  mesh->getNodeIdsOfCell( 0, nodes );
  const bool allNodes = true;
  MEDCouplingUMesh* mesh1 = (MEDCouplingUMesh*)mesh->buildPartOfMySelfNode( &nodes[0], &nodes[0]+nodes.size(), allNodes);
  MEDCouplingUMesh* mesh2 = (MEDCouplingUMesh*)mesh->buildPartOfMySelfNode( &nodes[0], &nodes[0]+nodes.size(),!allNodes);
  CPPUNIT_ASSERT_EQUAL( mesh1->getNumberOfCells(), 1 );
  CPPUNIT_ASSERT_EQUAL( mesh2->getNumberOfCells(), mesh->getNumberOfCells() );
  //! [CppSnippet_MEDCouplingUMesh_buildPartOfMySelfNode_2]
  mesh->decrRef();
  mesh1->decrRef();
  mesh2->decrRef();
}

void CppExample_MEDCouplingUMesh_getCellIdsLyingOnNodes()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_getCellIdsLyingOnNodes_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  mesh->allocateCells(5);
  const int conn[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn);    // 0
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+4);  // 1
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+7);  // 2
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+10); // 3
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+14); // 4
  mesh->finishInsertingCells();
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->alloc(9,2);
  const double coords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
  std::copy(coords,coords+18,coordsArr->getPointer());
  mesh->setCoords(coordsArr);
  coordsArr->decrRef();
  //! [CppSnippet_MEDCouplingUMesh_getCellIdsLyingOnNodes_1]
  //! [CppSnippet_MEDCouplingUMesh_getCellIdsLyingOnNodes_2]
  std::vector<int> nodes;
  mesh->getNodeIdsOfCell( 0, nodes );
  const bool allNodes = true;
  DataArrayInt* cellIdsArr1 = mesh->getCellIdsLyingOnNodes( &nodes[0], &nodes[0]+nodes.size(), allNodes);
  DataArrayInt* cellIdsArr2 = mesh->getCellIdsLyingOnNodes( &nodes[0], &nodes[0]+nodes.size(),!allNodes);
  CPPUNIT_ASSERT_EQUAL( cellIdsArr1->getNumberOfTuples(), 1 );
  CPPUNIT_ASSERT_EQUAL( cellIdsArr2->getNumberOfTuples(), mesh->getNumberOfCells() );
  //! [CppSnippet_MEDCouplingUMesh_getCellIdsLyingOnNodes_2]
  mesh->decrRef();
  cellIdsArr1->decrRef();
  cellIdsArr2->decrRef();
}

void CppExample_MEDCouplingUMesh_getCellIdsFullyIncludedInNodeIds()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_getCellIdsFullyIncludedInNodeIds_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  mesh->allocateCells(5);
  const int conn[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn);    // 0
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+4);  // 1
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+7);  // 2
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+10); // 3
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+14); // 4
  mesh->finishInsertingCells();
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->alloc(9,2);
  const double coords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
  std::copy(coords,coords+18,coordsArr->getPointer());
  mesh->setCoords(coordsArr);
  coordsArr->decrRef();
  //! [CppSnippet_MEDCouplingUMesh_getCellIdsFullyIncludedInNodeIds_1]
  //! [CppSnippet_MEDCouplingUMesh_getCellIdsFullyIncludedInNodeIds_2]
  const int cellIds[2]={1,2};
  std::vector<int> nodes;
  mesh->getNodeIdsOfCell( cellIds[0], nodes );
  mesh->getNodeIdsOfCell( cellIds[1], nodes );
  DataArrayInt* cellIdsArr = mesh->getCellIdsFullyIncludedInNodeIds( &nodes[0], &nodes[0]+nodes.size());
  CPPUNIT_ASSERT(std::equal( cellIds, cellIds+2, cellIdsArr->getPointer() ));
  //! [CppSnippet_MEDCouplingUMesh_getCellIdsFullyIncludedInNodeIds_2]
  mesh->decrRef();
  cellIdsArr->decrRef();
}

void CppExample_MEDCouplingUMesh_buildPartOfMySelf()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_buildPartOfMySelf_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  mesh->allocateCells(5);
  const int conn[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn);    // 0
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+4);  // 1
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+7);  // 2
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+10); // 3
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+14); // 4
  mesh->finishInsertingCells();
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->alloc(9,2);
  const double coords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
  std::copy(coords,coords+18,coordsArr->getPointer());
  mesh->setCoords(coordsArr);
  //! [CppSnippet_MEDCouplingUMesh_buildPartOfMySelf_1]
  //! [CppSnippet_MEDCouplingUMesh_buildPartOfMySelf_2]
  const int cellIds[2]={1,2};
  MEDCouplingUMesh* mesh2=(MEDCouplingUMesh*)mesh->buildPartOfMySelf(cellIds,cellIds+2,true);
  MEDCouplingUMesh* mesh3=(MEDCouplingUMesh*)mesh->buildPartOfMySelf(cellIds,cellIds+2,false);
  CPPUNIT_ASSERT(  coordsArr->isEqual( *mesh2->getCoords(), 1e-13 )); // same nodes
  CPPUNIT_ASSERT( !coordsArr->isEqual( *mesh3->getCoords(), 1e-13 )); // different nodes
  for ( int i = 0; i < 2; ++i )
    {
      std::vector<int> nodes1, nodes2;
      mesh ->getNodeIdsOfCell(cellIds[i], nodes1);
      mesh2->getNodeIdsOfCell(i, nodes2);
      CPPUNIT_ASSERT( nodes1 == nodes2 ); // cell #cellIds[i] was copied
    }
  //! [CppSnippet_MEDCouplingUMesh_buildPartOfMySelf_2]
  coordsArr->decrRef();
  mesh->decrRef();
  mesh2->decrRef();
  mesh3->decrRef();
}

void CppExample_MEDCouplingUMesh_mergeNodes()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_mergeNodes_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  mesh->allocateCells(5);
  const int conn[18]={0,3,4,1, 1,4,2, 4,5,2};
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn);
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+4);
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+7);
  mesh->finishInsertingCells();
  const double coords[6*2]={0.3,-0.301,  // #0
                            0.2,-0.3,    // #1
                            0.3,-0.302,  // #2 ~~ #0
                            1.1,0.0,     // #3
                            1.1,0.0,     // #4 == #3
                            0.3,-0.303}; // #5 ~~ #0
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->alloc(6,2);
  std::copy(coords,coords+6*2,coordsArr->getPointer());
  mesh->setCoords(coordsArr);
  coordsArr->decrRef();
  //! [CppSnippet_MEDCouplingUMesh_mergeNodes_1]
  //! [CppSnippet_MEDCouplingUMesh_mergeNodes_2]
  bool areNodesMerged; int newNbOfNodes;
  DataArrayInt *arr=mesh->mergeNodes(0.004,areNodesMerged,newNbOfNodes);
  const int idsExpected[6] = {0, 1, 0, 2, 2, 0};
  CPPUNIT_ASSERT(std::equal(idsExpected,idsExpected+6,arr->getPointer()));
  CPPUNIT_ASSERT( areNodesMerged );
  CPPUNIT_ASSERT_EQUAL( 3, newNbOfNodes );
  //! [CppSnippet_MEDCouplingUMesh_mergeNodes_2]
  //! [CppSnippet_MEDCouplingUMesh_mergeNodes_3]
  const double* baryCoords2 = coords + 2*2; // initial coordinates of node #2
  coordsArr=mesh->getCoords(); // retrieve a new shorten coord array
  CPPUNIT_ASSERT( fabs( baryCoords2[1] - coordsArr->getIJ(0,1)) > 1e-4 ); // Y of node #0 differs from that of baryCoords2
  // restore coordinates
  coordsArr->alloc(6,2);
  std::copy(coords,coords+6*2,coordsArr->getPointer());
  mesh->setCoords(coordsArr);
  // call mergeNodes2()
  arr = mesh->mergeNodes2(0.004,areNodesMerged,newNbOfNodes);
  coordsArr=mesh->getCoords(); // retrieve a new shorten coord array
  CPPUNIT_ASSERT_DOUBLES_EQUAL( baryCoords2[1], coordsArr->getIJ(0,1), 13 ); // Y of node #0 equals to that of baryCoords2
  //! [CppSnippet_MEDCouplingUMesh_mergeNodes_3]
  coordsArr->decrRef();
  mesh->decrRef();
  arr->decrRef();
}

void CppExample_MEDCouplingUMesh_zipConnectivityTraducer()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_zipConnectivityTraducer_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  mesh->allocateCells(5);
  const int conn[11]={0,3,4,1, 1,4,2, 4,1,0,3};
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+0); // 0     
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+4); // 1     
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+4); // 2 == 1
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+0); // 3 == 0
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+7); // 4 ~~ 0
  mesh->finishInsertingCells();
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->alloc(9,2);
  const double coords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
  std::copy(coords,coords+18,coordsArr->getPointer());
  mesh->setCoords(coordsArr);
  coordsArr->decrRef();
  //! [CppSnippet_MEDCouplingUMesh_zipConnectivityTraducer_1]
  //! [CppSnippet_MEDCouplingUMesh_zipConnectivityTraducer_2]
  const int oldNbCells = mesh->getNumberOfCells();
  DataArrayInt *arr = mesh->zipConnectivityTraducer(0);
  CPPUNIT_ASSERT_EQUAL( oldNbCells-2, mesh->getNumberOfCells() );
  const int idsExpected[5] = {0, 1, 1, 0, 2};
  CPPUNIT_ASSERT(std::equal(idsExpected,idsExpected+5,arr->getPointer()));
  //! [CppSnippet_MEDCouplingUMesh_zipConnectivityTraducer_2]
  mesh->decrRef();
  arr->decrRef();
}

void CppExample_MEDCouplingUMesh_zipCoordsTraducer()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_zipCoordsTraducer_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  mesh->allocateCells(5);
  const int conn[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn);    // 0
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+4);  // 1
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+7);  // 2
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+10); // 3
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+14); // 4
  mesh->finishInsertingCells();
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->alloc(9,2);
  const double coords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
  std::copy(coords,coords+18,coordsArr->getPointer());
  mesh->setCoords(coordsArr);
  coordsArr->decrRef();
  //! [CppSnippet_MEDCouplingUMesh_zipCoordsTraducer_1]
  //! [CppSnippet_MEDCouplingUMesh_zipCoordsTraducer_2]
  const int cellIds[2]={1,2};
  MEDCouplingUMesh* mesh2=(MEDCouplingUMesh*)mesh->buildPartOfMySelf(cellIds,cellIds+2,true);
  DataArrayInt *arr=mesh2->zipCoordsTraducer();
  CPPUNIT_ASSERT_EQUAL( 4, mesh2->getNumberOfNodes() ); // nb of nodes decreased
  CPPUNIT_ASSERT_EQUAL( mesh->getNumberOfNodes(), arr->getNumberOfTuples() );
  const int idsExpected[9] = {-1,0,1,-1,2,3,-1,-1,-1}; // -1 for unused nodes
  CPPUNIT_ASSERT(std::equal(idsExpected,idsExpected+9,arr->getPointer()));
  //! [CppSnippet_MEDCouplingUMesh_zipCoordsTraducer_2]
  mesh->decrRef();
  mesh2->decrRef();
  arr->decrRef();
}

void CppExample_MEDCouplingUMesh_getNodeIdsInUse()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_getNodeIdsInUse_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  mesh->allocateCells(5);
  const int conn[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn);    // 0
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+4);  // 1
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+7);  // 2
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+10); // 3
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+14); // 4
  mesh->finishInsertingCells();
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->alloc(9,2);
  const double coords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
  std::copy(coords,coords+18,coordsArr->getPointer());
  mesh->setCoords(coordsArr);
  coordsArr->decrRef();
  //! [CppSnippet_MEDCouplingUMesh_getNodeIdsInUse_1]
  //! [CppSnippet_MEDCouplingUMesh_getNodeIdsInUse_2]
  const int cellIds[2]={1,2};
  MEDCouplingUMesh* mesh2=(MEDCouplingUMesh*)mesh->buildPartOfMySelf(cellIds,cellIds+2,true);
  int newNbOfNodes = 0;
  DataArrayInt *arr=mesh2->getNodeIdsInUse( newNbOfNodes );
  const int idsExpected[9] = {-1,0,1,-1,2,3,-1,-1,-1};
  CPPUNIT_ASSERT(std::equal(idsExpected,idsExpected+9,arr->getPointer()));
  //! [CppSnippet_MEDCouplingUMesh_getNodeIdsInUse_2]
  //! [CppSnippet_MEDCouplingUMesh_getNodeIdsInUse_3]
  DataArrayInt *arr2=arr->invertArrayO2N2N2O(newNbOfNodes);
  const int idsExpected2[4] = {1,2,4,5};
  CPPUNIT_ASSERT(std::equal(idsExpected2,idsExpected2+4,arr2->getPointer()));
  //! [CppSnippet_MEDCouplingUMesh_getNodeIdsInUse_3]
  mesh->decrRef();
  mesh2->decrRef();
  arr->decrRef();
  arr2->decrRef();
}

void CppExample_MEDCouplingUMesh_convertToPolyTypes()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_convertToPolyTypes_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  mesh->allocateCells(5);
  const int conn[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn);    // 0
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+4);  // 1
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+7);  // 2
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+10); // 3
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+14); // 4
  mesh->finishInsertingCells();
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->alloc(9,2);
  const double coords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
  std::copy(coords,coords+18,coordsArr->getPointer());
  mesh->setCoords(coordsArr);
  coordsArr->decrRef();
  //! [CppSnippet_MEDCouplingUMesh_convertToPolyTypes_1]
  //! [CppSnippet_MEDCouplingUMesh_convertToPolyTypes_2]
  const int cells[2]={1,3};
  mesh->convertToPolyTypes(cells, cells+2);
  CPPUNIT_ASSERT( mesh->getTypeOfCell(0) == INTERP_KERNEL::NORM_QUAD4 );
  CPPUNIT_ASSERT( mesh->getTypeOfCell(1) == INTERP_KERNEL::NORM_POLYGON );
  CPPUNIT_ASSERT( mesh->getTypeOfCell(2) == INTERP_KERNEL::NORM_TRI3 );
  CPPUNIT_ASSERT( mesh->getTypeOfCell(3) == INTERP_KERNEL::NORM_POLYGON );
  //! [CppSnippet_MEDCouplingUMesh_convertToPolyTypes_2]
}

void CppExample_MEDCouplingUMesh_buildDescendingConnectivity2()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_buildDescendingConnectivity2_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  mesh->allocateCells(5);
  const int conn[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn);    // 0
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+4);  // 1
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+7);  // 2
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+10); // 3
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+14); // 4
  mesh->finishInsertingCells();
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->alloc(9,2);
  const double coords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
  std::copy(coords,coords+18,coordsArr->getPointer());
  mesh->setCoords(coordsArr);
  coordsArr->decrRef();
  //! [CppSnippet_MEDCouplingUMesh_buildDescendingConnectivity2_1]
  //! [CppSnippet_MEDCouplingUMesh_buildDescendingConnectivity2_2]
  DataArrayInt *desc       =DataArrayInt::New();
  DataArrayInt *descIndx   =DataArrayInt::New();
  DataArrayInt *revDesc    =DataArrayInt::New();
  DataArrayInt *revDescIndx=DataArrayInt::New();
  MEDCouplingUMesh * mesh2 = mesh->buildDescendingConnectivity2(desc,descIndx,revDesc,revDescIndx);
  const int descExpected[]        = {0,1,2,3, 2,4,5, 6,7,4, 8,9,1,10, 11,12,6,9};
  const int descIndxExpected[]    = {0,4,7,10,14,18};
  const int revDescExpected[]     = {0, 0,3, 0,1, 0, 1,2, 1, 2,4, 2, 3, 3,4, 3, 4, 4};
  const int revDescIndxExpected[] = {0,1,3,5,6,8,9,11,12,13,15,16,17,18};
  CPPUNIT_ASSERT(std::equal(descExpected,descExpected+18,desc->getPointer()));
  CPPUNIT_ASSERT(std::equal(descIndxExpected,descIndxExpected+6,descIndx->getPointer()));
  CPPUNIT_ASSERT(std::equal(revDescExpected,revDescExpected+18,revDesc->getPointer()));
  CPPUNIT_ASSERT(std::equal(revDescIndxExpected,revDescIndxExpected+14,revDescIndx->getPointer()));
  //! [CppSnippet_MEDCouplingUMesh_buildDescendingConnectivity2_2]
  //! [CppSnippet_MEDCouplingUMesh_buildDescendingConnectivity2_3]
  const int cell2ConnExpect[] = {4,1};
  std::vector<int> cell2Conn;
  mesh2->getNodeIdsOfCell( 3-1, cell2Conn ); // cell #3 in FORTRAN mode
  CPPUNIT_ASSERT(std::equal(cell2ConnExpect,cell2ConnExpect+2,&cell2Conn[0]));
  //! [CppSnippet_MEDCouplingUMesh_buildDescendingConnectivity2_3]
  desc->decrRef();
  descIndx->decrRef();
  revDesc->decrRef();
  revDescIndx->decrRef();
  mesh->decrRef();
  mesh2->decrRef();
}

void CppExample_MEDCouplingUMesh_buildDescendingConnectivity()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_buildDescendingConnectivity_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  mesh->allocateCells(5);
  const int conn[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn);    // 0
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+4);  // 1
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+7);  // 2
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+10); // 3
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+14); // 4
  mesh->finishInsertingCells();
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->alloc(9,2);
  const double coords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
  std::copy(coords,coords+18,coordsArr->getPointer());
  mesh->setCoords(coordsArr);
  coordsArr->decrRef();
  //! [CppSnippet_MEDCouplingUMesh_buildDescendingConnectivity_1]
  //! [CppSnippet_MEDCouplingUMesh_buildDescendingConnectivity_2]
  DataArrayInt *desc       =DataArrayInt::New();
  DataArrayInt *descIndx   =DataArrayInt::New();
  DataArrayInt *revDesc    =DataArrayInt::New();
  DataArrayInt *revDescIndx=DataArrayInt::New();
  MEDCouplingUMesh * mesh2 = mesh->buildDescendingConnectivity(desc,descIndx,revDesc,revDescIndx);
  const int descExpected[]        = {0,1,2,3, 2,4,5, 6,7,4, 8,9,1,10, 11,12,6,9};
  const int descIndxExpected[]    = {0,4,7,10,14,18};
  const int revDescExpected[]     = {0, 0,3, 0,1, 0, 1,2, 1, 2,4, 2, 3, 3,4, 3, 4, 4};
  const int revDescIndxExpected[] = {0,1,3,5,6,8,9,11,12,13,15,16,17,18};
  CPPUNIT_ASSERT(std::equal(descExpected,descExpected+18,desc->getPointer()));
  CPPUNIT_ASSERT(std::equal(descIndxExpected,descIndxExpected+6,descIndx->getPointer()));
  CPPUNIT_ASSERT(std::equal(revDescExpected,revDescExpected+18,revDesc->getPointer()));
  CPPUNIT_ASSERT(std::equal(revDescIndxExpected,revDescIndxExpected+14,revDescIndx->getPointer()));
  //! [CppSnippet_MEDCouplingUMesh_buildDescendingConnectivity_2]
  desc->decrRef();
  descIndx->decrRef();
  revDesc->decrRef();
  revDescIndx->decrRef();
  mesh->decrRef();
  mesh2->decrRef();
}

void CppExample_MEDCouplingUMesh_getReverseNodalConnectivity()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_getReverseNodalConnectivity_1]
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setMeshDimension(2);
  mesh->allocateCells(5);
  const int conn[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn);    // 0
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+4);  // 1
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+7);  // 2
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+10); // 3
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,conn+14); // 4
  mesh->finishInsertingCells();
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->alloc(9,2);
  const double coords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
  std::copy(coords,coords+18,coordsArr->getPointer());
  mesh->setCoords(coordsArr);
  coordsArr->decrRef();
  //! [CppSnippet_MEDCouplingUMesh_getReverseNodalConnectivity_1]
  //! [CppSnippet_MEDCouplingUMesh_getReverseNodalConnectivity_2]
  DataArrayInt *revNodal=DataArrayInt::New();
  DataArrayInt *revNodalIndx=DataArrayInt::New();
  mesh->getReverseNodalConnectivity(revNodal,revNodalIndx);
  const int revNodalExpected[18]={0,0,1,1,2,0,3,0,1,2,3,4,2,4,3,3,4,4};
  const int revNodalIndexExpected[10]={0,1,3,5,7,12,14,15,17,18};
  CPPUNIT_ASSERT(std::equal(revNodalExpected,revNodalExpected+18,revNodal->getPointer()));
  CPPUNIT_ASSERT(std::equal(revNodalIndexExpected,revNodalIndexExpected+10,revNodalIndx->getPointer()));
  //! [CppSnippet_MEDCouplingUMesh_getReverseNodalConnectivity_2]
  revNodal->decrRef();
  revNodalIndx->decrRef();
  mesh->decrRef();
}

void CppExample_MEDCouplingUMesh_checkDeepEquivalWith()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingUMesh_checkDeepEquivalWith_1]
  // mesh 1
  MEDCouplingUMesh *mesh1=MEDCouplingUMesh::New();
  {
    mesh1->setMeshDimension(2);
    const double coords[4*2]={0.0,0.0,  // #0
                              1.0,0.0,  // #1
                              1.0,1.0,  // #2
                              0.0,1.0}; // #3
    DataArrayDouble *coordsArr=DataArrayDouble::New();
    coordsArr->useExternalArrayWithRWAccess( coords, 4, 2 );
    mesh1->setCoords(coordsArr);
    coordsArr->decrRef();
    mesh1->allocateCells(2);
    const int conn[6]={0,1,2, 1,2,3};
    mesh1->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+0);  // #0
    mesh1->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+3);  // #1
    mesh1->finishInsertingCells();
  }
  // mesh 2
  MEDCouplingUMesh *mesh2=MEDCouplingUMesh::New();
  {
    mesh2->setMeshDimension(2);
    const double coords[4*2]={0.0,1.0,    // #0 = #3
                              0.0,0.0,    // #1 = #0
                              1.0,0.0,    // #2 = #1
                              1.0,1.001}; // #3 ~ #2
    DataArrayDouble *coordsArr=DataArrayDouble::New();
    coordsArr->useExternalArrayWithRWAccess( coords, 4, 2 );
    mesh2->setCoords(coordsArr);
    coordsArr->decrRef();
    mesh2->allocateCells(2);
    const int conn[6]={2,3,0, 3,1,2};
    mesh2->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+0);  // #0 = #1
    mesh2->insertNextCell(INTERP_KERNEL::NORM_TRI3,3, conn+3);  // #1 ~ #0
    mesh2->finishInsertingCells();
  }
  //! [CppSnippet_MEDCouplingUMesh_checkDeepEquivalWith_1]
  //! [CppSnippet_MEDCouplingUMesh_checkDeepEquivalWith_2]
  int cellCompPol = 1; // "permuted same orientation" - policy of medium severity
  DataArrayInt *nOld2New, *cOld2New;
  mesh1->checkDeepEquivalWith( mesh2, cellCompPol, 0.002, cOld2New, nOld2New );
  const int nOld2NewExpected[4] = { 3, 0, 1, 2 };
  const int cOld2NewExpected[2] = { 1, 0 };
  CPPUNIT_ASSERT(std::equal(nOld2NewExpected,nOld2NewExpected+4,nOld2New->getConstPointer()));
  CPPUNIT_ASSERT(std::equal(cOld2NewExpected,cOld2NewExpected+2,cOld2New->getConstPointer()));
  //! [CppSnippet_MEDCouplingUMesh_checkDeepEquivalWith_2]
  //! [CppSnippet_MEDCouplingUMesh_checkDeepEquivalWith_3]
  cOld2New->decrRef(); // else memory leaks
  CPPUNIT_ASSERT_THROW ( mesh1->checkDeepEquivalOnSameNodesWith( mesh2, cellCompPol, 0.002, cOld2New ), INTERP_KERNEL::Exception );
  mesh2->setCoords( mesh1->getCoords() ); // make meshes share the same coordinates array
  mesh2->allocateCells(2);
  const int conn[6]={1,2,3, 1,0,2};
  mesh2->insertNextCell(INTERP_KERNEL::NORM_TRI3,3,conn+0); // #0 = #1
  mesh2->insertNextCell(INTERP_KERNEL::NORM_TRI3,3,conn+3); // #1 ~ #0
  mesh2->finishInsertingCells();
  cellCompPol = 2; // the weakest policy
  cOld2New->decrRef();
  mesh1->checkDeepEquivalOnSameNodesWith( mesh2, cellCompPol, 0, cOld2New );
  //! [CppSnippet_MEDCouplingUMesh_checkDeepEquivalWith_3]
  nOld2New->decrRef();
  cOld2New->decrRef();
  mesh1->decrRef();
  mesh2->decrRef();
}

void CppExample_MEDCouplingPointSet_scale()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingPointSet_scale_1]
  double coords[4*2]={0.0,0.0, 1.0,0.0, 1.0,1.0, 0.0,1.0}; // 2D coordinates of 4 nodes
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->useExternalArrayWithRWAccess(coords, 4,2);
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setCoords(coordsArr);
  DataArrayDouble *initCoords = coordsArr->deepCpy();
  //! [CppSnippet_MEDCouplingPointSet_scale_1]
  //! [CppSnippet_MEDCouplingPointSet_scale_2]
  const double center[2] = {0.,0.};
  const double factor = 2.;
  mesh->scale( center, factor );
  //! [CppSnippet_MEDCouplingPointSet_scale_2]
  //! [CppSnippet_MEDCouplingPointSet_scale_3]
  const DataArrayDouble * coordsArr2 = mesh->getCoords();
  CPPUNIT_ASSERT( coordsArr2->isEqualWithoutConsideringStr( *initCoords, 1.0 ));
  CPPUNIT_ASSERT( !coordsArr2->isEqualWithoutConsideringStr( *initCoords, 0.9 ));
  // release data
  mesh->decrRef();
  coordsArr->decrRef();
  initCoords->decrRef();
  //! [CppSnippet_MEDCouplingPointSet_scale_3]
}

void CppExample_MEDCouplingPointSet_translate()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingPointSet_translate_1]
  double coords[4*2]={0.0,0.0, 1.0,0.0, 1.0,1.0, 0.0,1.0}; // 2D coordinates of 4 nodes
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->useExternalArrayWithRWAccess(coords, 4,2);
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setCoords(coordsArr);
  DataArrayDouble *initCoords = coordsArr->deepCpy();
  //! [CppSnippet_MEDCouplingPointSet_translate_1]
  //! [CppSnippet_MEDCouplingPointSet_translate_2]
  double vector[2] = {1.,1.};
  mesh->translate( vector );
  //! [CppSnippet_MEDCouplingPointSet_translate_2]
  //! [CppSnippet_MEDCouplingPointSet_translate_3]
  const DataArrayDouble * coordsArr2 = mesh->getCoords();
  CPPUNIT_ASSERT( coordsArr2->isEqualWithoutConsideringStr( *initCoords, 1.0 ));
  CPPUNIT_ASSERT( !coordsArr2->isEqualWithoutConsideringStr( *initCoords, 0.9 ));
  // release data
  mesh->decrRef();
  coordsArr->decrRef();
  initCoords->decrRef();
  //! [CppSnippet_MEDCouplingPointSet_translate_3]
}

void CppExample_MEDCouplingPointSet_rotate()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingPointSet_rotate_1]
  double coords[4*2]={0.0,0.0, 0.1,0.0, 0.1,0.1, 0.0,0.1}; // 2D coordinates of 4 nodes
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->useExternalArrayWithRWAccess(coords, 4,2);
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setCoords(coordsArr);
  //! [CppSnippet_MEDCouplingPointSet_rotate_1]
  //! [CppSnippet_MEDCouplingPointSet_rotate_2]
  double center[3] = {0.,0.,0.}; // it suits for 2D as well
  double vector[3] = {0.,0.,1.}; // it is not used in 2D
  mesh->rotate( center, vector, -M_PI/2);
  //! [CppSnippet_MEDCouplingPointSet_rotate_2]
  //! [CppSnippet_MEDCouplingPointSet_rotate_3]
  mesh->changeSpaceDimension(3);
  mesh->rotate( center, vector, +M_PI/2);
  //! [CppSnippet_MEDCouplingPointSet_rotate_3]
  //! [CppSnippet_MEDCouplingPointSet_rotate_4]
  mesh->changeSpaceDimension(2);
  const DataArrayDouble * coordsArr2 = mesh->getCoords();
  CPPUNIT_ASSERT( coordsArr2->isEqualWithoutConsideringStr( *coordsArr, 1e-13 ));
  // release data
  mesh->decrRef();
  coordsArr->decrRef();
  //! [CppSnippet_MEDCouplingPointSet_rotate_4]
}

void CppExample_MEDCouplingPointSet_getBoundingBox()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingPointSet_getBoundingBox_1]
  double cc[2*3]={0.0, 0.1, 0.2, // 3D coordinates of 2 nodes
                  2.0, 2.1, 2.2};
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->useExternalArrayWithRWAccess(cc, 2,3);
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setCoords(coordsArr);
  coordsArr->decrRef();
  //! [CppSnippet_MEDCouplingPointSet_getBoundingBox_1]
  //! [CppSnippet_MEDCouplingPointSet_getBoundingBox_2]
  double bbox[3][2];
  mesh->getBoundingBox( (double*) bbox );
  mesh->decrRef();

  // check the returned coordinates of extremum points of the bounding box
  for ( int i = 0; i < 2; ++i )   // point id
    for ( int j = 0; j < 3; ++j ) // component
      CPPUNIT_ASSERT_DOUBLES_EQUAL( cc[ i*3 + j ], bbox[j][i], 1e-13);
  //! [CppSnippet_MEDCouplingPointSet_getBoundingBox_2]
}

void CppExample_MEDCouplingPointSet_getNodeIdsNearPoint()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingPointSet_getNodeIdsNearPoint_1]
  // 2D coordinates of 5 nodes
  double coords[5*2]={0.3,-0.301, // #0
                      0.2,-0.3,   // #1
                      0.3,-0.302, // #2
                      1.1,0.0,    // #3
                      0.3,-0.303};// #4
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->useExternalArrayWithRWAccess(coords, 5,2);
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setCoords(coordsArr);
  coordsArr->decrRef();
  //! [CppSnippet_MEDCouplingPointSet_getNodeIdsNearPoint_1]
  //! [CppSnippet_MEDCouplingPointSet_getNodeIdsNearPoint_2]
  double point [2]={0.3, -0.3}; // point close to nodes #0, #2 and #4
  DataArrayInt *ids = mesh->getNodeIdsNearPoint(point, 1e-13);

  // check found ids
  const int expectedIDs[3] = {0,2,4};
  DataArrayInt * okIDs = ids->getIdsEqualList ( expectedIDs, expectedIDs+3 );
  CPPUNIT_ASSERT_EQUAL(3, okIDs->getNumberOfTuples());

  // release data
  mesh->decrRef();
  ids->decrRef();
  okIDs->decrRef();
  //! [CppSnippet_MEDCouplingPointSet_getNodeIdsNearPoint_2]
}
void CppExample_MEDCouplingPointSet_getNodeIdsNearPoints()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingPointSet_getNodeIdsNearPoints_1]
  // 2D coordinates of 7 nodes
  double coords[7*2]={0.3,-0.301, // #0
                      0.2,-0.3,   // #1
                      0.3,-0.302, // #2
                      1.1,0.0,    // #3
                      1.1,0.0,    // #4
                      1.1,0.002,  // #5
                      0.3,-0.303};// #6
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->useExternalArrayWithRWAccess(coords, 7,2);
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setCoords(coordsArr);
  coordsArr->decrRef();
  //! [CppSnippet_MEDCouplingPointSet_getNodeIdsNearPoints_1]
  //! [CppSnippet_MEDCouplingPointSet_getNodeIdsNearPoints_2]
  const int nbOfPoints = 3;
  double points [nbOfPoints*2]={0.2,-0.301,  // ~ node #1
                                0.0, 0.0,
                                1.1, 0.002}; // ~ nodes #3, #4 and #5
  DataArrayInt *ids, *idsIndex;
  mesh->getNodeIdsNearPoints(points, nbOfPoints, 1e-13,ids,idsIndex);

  // check found ids (i.e. contents of 'ids' array)
  const int expectedIDs[4] = {1, 3, 4, 5};
  DataArrayInt * okIDs = ids->getIdsEqualList ( expectedIDs, expectedIDs+4 );
  CPPUNIT_ASSERT_EQUAL(4, okIDs->getNumberOfTuples());

  // release data
  mesh->decrRef();
  ids->decrRef();
  idsIndex->decrRef();
  okIDs->decrRef();
  //! [CppSnippet_MEDCouplingPointSet_getNodeIdsNearPoints_2]
}

void CppExample_MEDCouplingPointSet_findCommonNodes()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingPointSet_findCommonNodes_1]
  double coords[6*2]={0.3,-0.301, // 0
                      0.2,-0.3,   // 1
                      0.3,-0.302, // 2
                      1.1,0.0,    // 3
                      1.1,0.0,    // 4
                      0.3,-0.303};// 5
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->useExternalArrayWithRWAccess(coords, 6,2);
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setCoords(coordsArr);
  coordsArr->decrRef();
  //! [CppSnippet_MEDCouplingPointSet_findCommonNodes_1]
  //! [CppSnippet_MEDCouplingPointSet_findCommonNodes_2]
  DataArrayInt *com, *comI;
  mesh->findCommonNodes(1e-13,-1,com,comI);
  CPPUNIT_ASSERT_EQUAL(2, com->getNumberOfTuples());
  mesh->findCommonNodes(0.004,-1,com,comI);
  CPPUNIT_ASSERT_EQUAL(5, com->getNumberOfTuples());
  //! [CppSnippet_MEDCouplingPointSet_findCommonNodes_2]
  mesh->decrRef();
  com->decrRef();
  comI->decrRef();
}

void CppExample_MEDCouplingPointSet_getCoordinatesOfNode()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_MEDCouplingPointSet_getCoordinatesOfNode_1]
  double coords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3};
  DataArrayDouble *coordsArr=DataArrayDouble::New();
  coordsArr->useExternalArrayWithRWAccess(coords, 3,2);
  MEDCouplingUMesh *mesh=MEDCouplingUMesh::New();
  mesh->setCoords(coordsArr);
  coordsArr->decrRef();
  //! [CppSnippet_MEDCouplingPointSet_getCoordinatesOfNode_1]
  //! [CppSnippet_MEDCouplingPointSet_getCoordinatesOfNode_2]
  std::vector<double> coords2;
  mesh->getCoordinatesOfNode(1,coords2);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(coords[0],coords2[0],1e-13);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(coords[1],coords2[1],1e-13);
  //! [CppSnippet_MEDCouplingPointSet_getCoordinatesOfNode_2]
  mesh->decrRef();
}

void CppExample_DataArrayInt_buildPermutationArr()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_DataArrayInt_buildPermutationArr_1]
  DataArrayInt *a=DataArrayInt::New();
  const int vala[5]={4,5,6,7,8};
  a->alloc(5,1);
  std::copy(vala,vala+5,a->getPointer());
  DataArrayInt *b=DataArrayInt::New();
  const int valb[5]={5,4,8,6,7};
  b->alloc(5,1);
  std::copy(valb,valb+5,b->getPointer());
  DataArrayInt *c=a->buildPermutationArr(*b);
  //! [CppSnippet_DataArrayInt_buildPermutationArr_1]
  const int expect1[5]={1,0,4,2,3};
  CPPUNIT_ASSERT_EQUAL(5,c->getNumberOfTuples());
  CPPUNIT_ASSERT_EQUAL(1,c->getNumberOfComponents());
  CPPUNIT_ASSERT(std::equal(expect1,expect1+5,c->getConstPointer()));
  CPPUNIT_ASSERT(a->isEqualWithoutConsideringStrAndOrder(*b));
  a->decrRef();
  b->decrRef();
  c->decrRef();
}

void CppExample_DataArrayInt_invertArrayO2N2N2O()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_DataArrayInt_invertArrayO2N2N2O_1]
  const int arr1[6]={2,0,4,1,5,3};
  DataArrayInt *da=DataArrayInt::New();
  da->alloc(6,1);
  std::copy(arr1,arr1+6,da->getPointer());
  DataArrayInt *da2=da->invertArrayO2N2N2O(6);
  const int expected1[6]={1,3,0,5,2,4};
  for(int i=0;i<6;i++)
    CPPUNIT_ASSERT_EQUAL(expected1[i],da2->getIJ(i,0));
  //! [CppSnippet_DataArrayInt_invertArrayO2N2N2O_1]
  da->decrRef();
  da2->decrRef();
}

void CppExample_DataArrayInt_invertArrayN2O2O2N()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_DataArrayInt_invertArrayN2O2O2N_1]
  const int arr1[6]={2,0,4,1,5,3};
  DataArrayInt *da=DataArrayInt::New();
  da->alloc(6,1);
  std::copy(arr1,arr1+6,da->getPointer());
  DataArrayInt *da2=da->invertArrayN2O2O2N(6);
  const int expected1[6]={1,3,0,5,2,4};
  for(int i=0;i<6;i++)
    CPPUNIT_ASSERT_EQUAL(expected1[i],da2->getIJ(i,0));
  //! [CppSnippet_DataArrayInt_invertArrayN2O2O2N_1]
  da->decrRef();
  da2->decrRef();
}

void CppExample_DataArrayDouble_getIdsInRange()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_DataArrayDouble_getIdsInRange_1]
  DataArrayDouble *da=DataArrayDouble::New();
  da->alloc(10,1);
  da->iota();

  DataArrayInt* da2 = da->getIdsInRange( 2.5, 6 );
  //! [CppSnippet_DataArrayDouble_getIdsInRange_1]
  da->decrRef();
  da2->decrRef();
}

void CppExample_DataArrayDouble_findCommonTuples()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_DataArrayDouble_findCommonTuples1]
  DataArrayDouble *da=DataArrayDouble::New();
  da->alloc(6,2);
  const double array2[12]={2.3,2.3, // 0
                           1.2,1.2, // 1
                           1.3,1.3, // 2
                           2.3,2.3, // 3
                           2.301,   // 4
                           2.301,   // 5
                           0.8,0.8};// 6
  std::copy(array2,array2+12,da->getPointer());
  //! [CppSnippet_DataArrayDouble_findCommonTuples1]
  //! [CppSnippet_DataArrayDouble_findCommonTuples2]
  DataArrayInt *c=0,*cI=0;
  da->findCommonTuples(1e-1,-1,c,cI);

  const int expected3[5]={0,3,4,1,2};
  const int expected4[3]={0,3,5};
  CPPUNIT_ASSERT(std::equal(expected3,expected3+5,c->getConstPointer()));
  CPPUNIT_ASSERT(std::equal(expected4,expected4+3,cI->getConstPointer()));
  c->decrRef();
  cI->decrRef();
  da->decrRef();
  //! [CppSnippet_DataArrayDouble_findCommonTuples2]
}

void CppExample_DataArrayDouble_Meld1()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_DataArrayDouble_Meld1_1]
  const int sameNbTuples = 7;

  DataArrayDouble *da1=DataArrayDouble::New();
  da1->alloc(sameNbTuples,2);
  da1->fillWithValue(7.);
  da1->setInfoOnComponent(0,"c0da1");
  da1->setInfoOnComponent(1,"c1da1");

  DataArrayDouble *da2=DataArrayDouble::New();
  da2->alloc(sameNbTuples,1);
  da2->iota(0.);
  da2->setInfoOnComponent(0,"c0da2");

  da1->meldWith(da2);
  //! [CppSnippet_DataArrayDouble_Meld1_1]
  //! [CppSnippet_DataArrayDouble_Meld1_2]
  da1->decrRef();
  da2->decrRef();
  //! [CppSnippet_DataArrayDouble_Meld1_2]
}

void CppExample_DataArrayInt_Meld1()
{
  using namespace ParaMEDMEM;
  //! [CppSnippet_DataArrayInt_Meld1_1]
  const int sameNbTuples = 7;

  DataArrayInt *da1=DataArrayInt::New();
  da1->alloc(sameNbTuples,2);
  da1->fillWithValue(7);
  da1->setInfoOnComponent(0,"c0da1");
  da1->setInfoOnComponent(1,"c1da1");

  DataArrayInt *da2=DataArrayInt::New();
  da2->alloc(sameNbTuples,1);
  da2->iota(0);
  da2->setInfoOnComponent(0,"c0da2");

  da1->meldWith(da2);
  //! [CppSnippet_DataArrayInt_Meld1_1]
  //! [CppSnippet_DataArrayInt_Meld1_2]
  da1->decrRef();
  da2->decrRef();
  //! [CppSnippet_DataArrayInt_Meld1_2]
}

void CppExampleFieldDoubleBuildSubPart1()
{
  //! [CppSnippetFieldDoubleBuildSubPart1_1]
  ParaMEDMEM::MEDCouplingUMesh *mesh1=ParaMEDMEM::MEDCouplingBasicsTest::build2DTargetMesh_1();
  ParaMEDMEM::MEDCouplingFieldDouble *f1=ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_CELLS,ParaMEDMEM::ONE_TIME);
  f1->setTime(2.3,5,6);
  f1->setMesh(mesh1);
  ParaMEDMEM::DataArrayDouble *array=ParaMEDMEM::DataArrayDouble::New();
  array->alloc(mesh1->getNumberOfCells(),2);
  const double arr1[10]={3.,103.,4.,104.,5.,105.,6.,106.,7.,107.};
  std::copy(arr1,arr1+10,array->getPointer());
  f1->setArray(array);
  array->decrRef();
  //! [CppSnippetFieldDoubleBuildSubPart1_1]
  //! [CppSnippetFieldDoubleBuildSubPart1_2]
  const int part1[3]={2,1,4};
  ParaMEDMEM::MEDCouplingFieldDouble *f2=f1->buildSubPart(part1,part1+3);
  //! [CppSnippetFieldDoubleBuildSubPart1_2]
  f2->zipCoords();
  CPPUNIT_ASSERT_EQUAL(3,f2->getMesh()->getNumberOfCells());
  CPPUNIT_ASSERT_EQUAL(6,f2->getMesh()->getNumberOfNodes());
  CPPUNIT_ASSERT_EQUAL(2,f2->getMesh()->getSpaceDimension());
  CPPUNIT_ASSERT_EQUAL(2,f2->getMesh()->getMeshDimension());
  ParaMEDMEM::MEDCouplingUMesh *m2C=dynamic_cast<ParaMEDMEM::MEDCouplingUMesh *>(const_cast<ParaMEDMEM::MEDCouplingMesh *>(f2->getMesh()));
  CPPUNIT_ASSERT_EQUAL(13,m2C->getMeshLength());
  const double expected2[12]={0.2, -0.3, 0.7, -0.3, 0.2, 0.2, 0.7, 0.2, 0.2, 0.7, 0.7, 0.7};
  for(int i=0;i<12;i++)
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected2[i],m2C->getCoords()->getIJ(0,i),1.e-12);
  const double expected3[13]={3,2,3,1,3,0,2,1,4,4,5,3,2};
  CPPUNIT_ASSERT(std::equal(expected3,expected3+13,m2C->getNodalConnectivity()->getConstPointer()));
  const double expected4[4]={0,4,8,13};
  CPPUNIT_ASSERT(std::equal(expected4,expected4+4,m2C->getNodalConnectivityIndex()->getConstPointer()));
  f2->decrRef();
  f1->decrRef();
  //! [CppSnippetFieldDoubleBuildSubPart1_3]
  f1=ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_NODES,ParaMEDMEM::ONE_TIME);
  f1->setTime(2.3,5,6);
  f1->setMesh(mesh1);
  array=ParaMEDMEM::DataArrayDouble::New();
  array->alloc(mesh1->getNumberOfNodes(),2);
  const double arr2[18]={3.,103.,4.,104.,5.,105.,6.,106.,7.,107.,8.,108.,9.,109.,10.,110.,11.,111.};
  std::copy(arr2,arr2+18,array->getPointer());  
  f1->setArray(array);
  array->decrRef();
  //! [CppSnippetFieldDoubleBuildSubPart1_3]
  //! [CppSnippetFieldDoubleBuildSubPart1_4]
  const int part2[2]={1,2};
  f2=f1->buildSubPart(part2,part2+2);
  //! [CppSnippetFieldDoubleBuildSubPart1_4]
  f2->decrRef();
  //idem previous because nodes of cell#4 are not fully present in part3 
  const int part3[2]={1,2};
  ParaMEDMEM::DataArrayInt *arrr=ParaMEDMEM::DataArrayInt::New();
  arrr->alloc(2,1);
  std::copy(part3,part3+2,arrr->getPointer());
  f2=f1->buildSubPart(arrr);
  arrr->decrRef();
  f2->decrRef();
  //
  const int part4[3]={1,2,4};
  f2=f1->buildSubPart(part4,part4+3);
  f2->decrRef();
  //
  f1->decrRef();
  mesh1->decrRef();
  return;
}

void CppSnippetUMeshStdBuild1()
{
  //! [CppSnippetUMeshStdBuild1_1]
  double coords[27]={-0.3,-0.3,0.,   0.2,-0.3,0.,   0.7,-0.3,0.,   -0.3,0.2,0.,   0.2,0.2,0., 
                     0.7,0.2,0.,    -0.3,0.7,0.,    0.2,0.7,0.,     0.7,0.7,0. };
  int nodalConnPerCell[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
  //! [CppSnippetUMeshStdBuild1_1]
  //! [CppSnippetUMeshStdBuild1_2]
  ParaMEDMEM::MEDCouplingUMesh *mesh=ParaMEDMEM::MEDCouplingUMesh::New("My2DMesh",2);
  //! [CppSnippetUMeshStdBuild1_2]
  //! [CppSnippetUMeshStdBuild1_3]
  mesh->allocateCells(5);//You can put more than 5 if you want but not less.
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,nodalConnPerCell);
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3,nodalConnPerCell+4);
  mesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3,nodalConnPerCell+7);
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,nodalConnPerCell+10);
  mesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,nodalConnPerCell+14);
  mesh->finishInsertingCells();
  //! [CppSnippetUMeshStdBuild1_3]
  //! [CppSnippetUMeshStdBuild1_4]
  ParaMEDMEM::DataArrayDouble *coordsArr=ParaMEDMEM::DataArrayDouble::New();
  coordsArr->alloc(9,3);//here coordsArr are declared to have 3 components, mesh will deduce that its spaceDim==3. 
  std::copy(coords,coords+27,coordsArr->getPointer());
  mesh->setCoords(coordsArr);//coordsArr contains 9 tuples, that is to say mesh contains 9 nodes.
  coordsArr->decrRef();
  //! [CppSnippetUMeshStdBuild1_4]
  mesh->checkCoherency();
  //! [CppSnippetUMeshStdBuild1_5]
  mesh->decrRef();
  //! [CppSnippetUMeshStdBuild1_5]
}

void CppSnippetCMeshStdBuild1()
{
  //! [CppSnippetCMeshStdBuild1_1]
  double XCoords[9]={-0.3,0.,0.1,0.3,0.45,0.47,0.49,1.,1.22};
  double YCoords[7]={0.,0.1,0.37,0.45,0.47,0.49,1.007};
  ParaMEDMEM::DataArrayDouble *arrX=ParaMEDMEM::DataArrayDouble::New();
  arrX->alloc(9,1);
  std::copy(XCoords,XCoords+9,arrX->getPointer());
  arrX->setInfoOnComponent(0,"X [m]");
  ParaMEDMEM::DataArrayDouble *arrY=ParaMEDMEM::DataArrayDouble::New();
  arrY->alloc(7,1);
  std::copy(YCoords,YCoords+7,arrY->getPointer());
  arrY->setInfoOnComponent(0,"Y [m]");
  //! [CppSnippetCMeshStdBuild1_1]
  //! [CppSnippetCMeshStdBuild1_2]
  ParaMEDMEM::MEDCouplingCMesh *mesh=ParaMEDMEM::MEDCouplingCMesh::New("My2D_CMesh");
  mesh->setCoords(arrX,arrY);
  arrX->decrRef();
  arrY->decrRef();
  //! [CppSnippetCMeshStdBuild1_2]
  //! [CppSnippetCMeshStdBuild1_3]
  CPPUNIT_ASSERT_EQUAL(8*6,mesh->getNumberOfCells());
  CPPUNIT_ASSERT_EQUAL(9*7,mesh->getNumberOfNodes());
  CPPUNIT_ASSERT_EQUAL(2,mesh->getSpaceDimension());
  CPPUNIT_ASSERT_EQUAL(2,mesh->getMeshDimension());
  //! [CppSnippetCMeshStdBuild1_3]
  mesh->decrRef();
  mesh=ParaMEDMEM::MEDCouplingCMesh::New("My2D_CMesh");
  arrX=ParaMEDMEM::DataArrayDouble::New(); arrX->alloc(9,1); std::copy(XCoords,XCoords+9,arrX->getPointer()); arrX->setInfoOnComponent(0,"X [m]");
  arrY=ParaMEDMEM::DataArrayDouble::New(); arrY->alloc(7,1); std::copy(YCoords,YCoords+7,arrY->getPointer()); arrY->setInfoOnComponent(0,"Y [m]");
  //! [CppSnippetCMeshStdBuild1_2bis]
  mesh->setCoordsAt(0,arrX);
  arrX->decrRef();
  mesh->setCoordsAt(1,arrY);
  arrY->decrRef();
  //! [CppSnippetCMeshStdBuild1_2bis]
  CPPUNIT_ASSERT_EQUAL(8*6,mesh->getNumberOfCells());
  CPPUNIT_ASSERT_EQUAL(9*7,mesh->getNumberOfNodes());
  CPPUNIT_ASSERT_EQUAL(2,mesh->getSpaceDimension());
  CPPUNIT_ASSERT_EQUAL(2,mesh->getMeshDimension());
  //! [CppSnippetCMeshStdBuild1_4]
  mesh->decrRef();
  //! [CppSnippetCMeshStdBuild1_4]
}

void CppSnippetUMeshAdvBuild1()
{
  //! [CppSnippetUMeshAdvBuild1_1]
  double coords[27]={-0.3,-0.3,0.,   0.2,-0.3,0.,   0.7,-0.3,0.,   -0.3,0.2,0.,   0.2,0.2,0., 
                     0.7,0.2,0.,    -0.3,0.7,0.,    0.2,0.7,0.,     0.7,0.7,0. };
  int nodalConnPerCell[23]={4,0,3,4,1, 3,1,4,2, 3,4,5,2, 4,6,7,4,3, 4,7,8,5,4};
  int nodalConnPerCellIndex[6]={0,5,9,13,18,23};
  //! [CppSnippetUMeshAdvBuild1_1]
  //! [CppSnippetUMeshAdvBuild1_2]
  ParaMEDMEM::MEDCouplingUMesh *mesh=ParaMEDMEM::MEDCouplingUMesh::New("My2DMesh",2);
  //! [CppSnippetUMeshAdvBuild1_2]
  //! [CppSnippetUMeshAdvBuild1_3]
  ParaMEDMEM::DataArrayInt *nodalConn=ParaMEDMEM::DataArrayInt::New();
  nodalConn->alloc(23,1);
  std::copy(nodalConnPerCell,nodalConnPerCell+23,nodalConn->getPointer());
  ParaMEDMEM::DataArrayInt *nodalConnI=ParaMEDMEM::DataArrayInt::New();
  nodalConnI->alloc(6,1);
  std::copy(nodalConnPerCellIndex,nodalConnPerCellIndex+6,nodalConnI->getPointer());
  mesh->setConnectivity(nodalConn,nodalConnI,true);
  nodalConn->decrRef();// nodalConn DataArrayInt instance is owned by mesh after call to setConnectivity method. No more need here -> decrRef()
  nodalConnI->decrRef();// nodalConnI DataArrayInt instance is owned by mesh after call to setConnectivity method. No more need here -> decrRef()
  //! [CppSnippetUMeshAdvBuild1_3]
  //! [CppSnippetUMeshAdvBuild1_4]
  ParaMEDMEM::DataArrayDouble *coordsArr=ParaMEDMEM::DataArrayDouble::New();
  coordsArr->alloc(9,3);//here coordsArr are declared to have 3 components, mesh will deduce that its spaceDim==3. 
  std::copy(coords,coords+27,coordsArr->getPointer());
  mesh->setCoords(coordsArr);//coordsArr contains 9 tuples, that is to say mesh contains 9 nodes.
  coordsArr->decrRef();
  //! [CppSnippetUMeshAdvBuild1_4]
  mesh->checkCoherency();
  //! [CppSnippetUMeshAdvBuild1_5]
  mesh->decrRef();
  //! [CppSnippetUMeshAdvBuild1_5]
}

void CppSnippetDataArrayBuild1()
{
  //! [CppSnippetDataArrayBuild1_0]
  const int nbOfNodes=12;
  double coords[3*nbOfNodes]={2.,3.,4.,3.,4.,5.,4.,5.,6.,5.,6.,7.,6.,7.,8.,7.,8.,9.,8.,9.,10.,9.,10.,11.,10.,11.,12.,11.,12.,13.,12.,13.,14.,13.,14.,15.};
  //
  ParaMEDMEM::DataArrayDouble *coordsArr=0;
  double *tmp=0;
  //! [CppSnippetDataArrayBuild1_0]
  //
  //! [CppSnippetDataArrayBuild1_1]
  coordsArr=ParaMEDMEM::DataArrayDouble::New();
  coordsArr->useArray(coords,false,ParaMEDMEM::CPP_DEALLOC,nbOfNodes,3);
  //now use coordsArr as you need
  //...
  //coordsArr is no more useful here : release it
  coordsArr->decrRef();
  //! [CppSnippetDataArrayBuild1_1]
  //! [CppSnippetDataArrayBuild1_2]
  coordsArr=ParaMEDMEM::DataArrayDouble::New();
  tmp=new double[3*nbOfNodes];
  std::copy(coords,coords+3*nbOfNodes,tmp);
  coordsArr->useArray(tmp,true,ParaMEDMEM::CPP_DEALLOC,nbOfNodes,3);
  //now use coordsArr as you need
  //...
  //coordsArr is no more useful, release it
  coordsArr->decrRef();
  //! [CppSnippetDataArrayBuild1_2]
  //! [CppSnippetDataArrayBuild1_3]
  coordsArr=ParaMEDMEM::DataArrayDouble::New();
  tmp=(double *)malloc(3*nbOfNodes*sizeof(double));
  std::copy(coords,coords+3*nbOfNodes,tmp);
  coordsArr->useArray(tmp,true,ParaMEDMEM::C_DEALLOC,nbOfNodes,3);
  //now use coordsArr as you need
  //...
  //coordsArr is no more useful here : release it
  coordsArr->decrRef();
  //! [CppSnippetDataArrayBuild1_3]
  //! [CppSnippetDataArrayBuild1_4]
  coordsArr=ParaMEDMEM::DataArrayDouble::New();
  coordsArr->alloc(nbOfNodes,3);
  tmp=coordsArr->getPointer();
  std::copy(coords,coords+3*nbOfNodes,tmp);
  coordsArr->declareAsNew();//you have modified data pointed by internal pointer notify object
  //now use coordsArr as you need
  //...
  //coordsArr is no more useful here : release it
  coordsArr->decrRef();
  //! [CppSnippetDataArrayBuild1_4]
  coordsArr=ParaMEDMEM::DataArrayDouble::New();
  coordsArr->alloc(nbOfNodes,3);
  tmp=coordsArr->getPointer();
  std::copy(coords,coords+3*nbOfNodes,tmp);
  ParaMEDMEM::DataArrayDouble *coordsArrCpy=0;
  //! [CppSnippetDataArrayBuild1_5]
  coordsArrCpy=coordsArr->deepCpy();
  //! [CppSnippetDataArrayBuild1_5]
  //! [CppSnippetDataArrayBuild1_6]
  CPPUNIT_ASSERT(coordsArrCpy->isEqual(*coordsArr,1e-12));
  coordsArrCpy->setIJ(0,0,1000.);
  CPPUNIT_ASSERT(!coordsArrCpy->isEqual(*coordsArr,1e-12));//coordsArrCpy only has been modified
  //! [CppSnippetDataArrayBuild1_6]
  //! [CppSnippetDataArrayBuild1_7]
  coordsArrCpy->decrRef();
  //! [CppSnippetDataArrayBuild1_7]
  //! [CppSnippetDataArrayBuild1_5bis]
  coordsArrCpy=coordsArr->performCpy(true);
  //! [CppSnippetDataArrayBuild1_5bis]
  CPPUNIT_ASSERT(coordsArrCpy->isEqual(*coordsArr,1e-12));
  coordsArrCpy->setIJ(0,0,1000.);
  CPPUNIT_ASSERT(!coordsArrCpy->isEqual(*coordsArr,1e-12));//coordsArrCpy only has been modified
  coordsArrCpy->decrRef();
  //! [CppSnippetDataArrayBuild1_8]
  coordsArrCpy=coordsArr->performCpy(false);
  //! [CppSnippetDataArrayBuild1_8]
  //! [CppSnippetDataArrayBuild1_9]
  CPPUNIT_ASSERT(coordsArrCpy->isEqual(*coordsArr,1e-12));
  coordsArrCpy->setIJ(0,0,1000.);
  CPPUNIT_ASSERT(coordsArrCpy->isEqual(*coordsArr,1e-12));//coordsArr and coordsArrCpy have been modified simultaneously
  //! [CppSnippetDataArrayBuild1_9]
  //! [CppSnippetDataArrayBuild1_10]
  coordsArrCpy->decrRef();
  //! [CppSnippetDataArrayBuild1_10]
  //! [CppSnippetDataArrayBuild1_11]
  coordsArrCpy=ParaMEDMEM::DataArrayDouble::New();
  //! [CppSnippetDataArrayBuild1_11]
  //! [CppSnippetDataArrayBuild1_12]
  coordsArrCpy->cpyFrom(*coordsArr);
  //! [CppSnippetDataArrayBuild1_12]
  //! [CppSnippetDataArrayBuild1_13]
  CPPUNIT_ASSERT(coordsArrCpy->isEqual(*coordsArr,1e-12));
  coordsArrCpy->setIJ(0,0,2000.);
  CPPUNIT_ASSERT(!coordsArrCpy->isEqual(*coordsArr,1e-12));//coordsArrCpy only has been modified
  //! [CppSnippetDataArrayBuild1_13]
  //! [CppSnippetDataArrayBuild1_14]
  coordsArrCpy->decrRef();
  //! [CppSnippetDataArrayBuild1_14]
  coordsArr->decrRef();
  //! [CppSnippetDataArrayBuild1_14]
}

void CppSnippetFieldDoubleBuild1()
{
  double XCoords[9]={-0.3,0.07,0.1,0.3,0.45,0.47,0.49,1.,1.22};
  double YCoords[7]={0.07,0.1,0.37,0.45,0.47,0.49,1.007};
  ParaMEDMEM::DataArrayDouble *arrX=ParaMEDMEM::DataArrayDouble::New(); arrX->alloc(9,1); std::copy(XCoords,XCoords+9,arrX->getPointer()); arrX->setInfoOnComponent(0,"X [m]");
  ParaMEDMEM::DataArrayDouble *arrY=ParaMEDMEM::DataArrayDouble::New(); arrY->alloc(7,1); std::copy(YCoords,YCoords+7,arrY->getPointer()); arrY->setInfoOnComponent(0,"Y [m]"); 
  ParaMEDMEM::MEDCouplingCMesh *mesh=ParaMEDMEM::MEDCouplingCMesh::New("My2D_CMesh");
  mesh->setCoords(arrX,arrY); arrX->decrRef(); arrY->decrRef();
  //! [CppSnippetFieldDoubleBuild1_1]
  ParaMEDMEM::MEDCouplingFieldDouble* fieldOnCells=ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_CELLS,ParaMEDMEM::NO_TIME);
  fieldOnCells->setName("MyTensorFieldOnCellNoTime");
  fieldOnCells->setMesh(mesh);
  mesh->decrRef(); // no more need of mesh because mesh has been attached to fieldOnCells
  ParaMEDMEM::DataArrayDouble *array=ParaMEDMEM::DataArrayDouble::New();
  array->alloc(fieldOnCells->getMesh()->getNumberOfCells(),9);//Implicitely fieldOnCells will be a 9 components field.
  array->fillWithValue(7.);
  fieldOnCells->setArray(array);
  array->decrRef();
  // fieldOnCells is now usable
  // ...
  // fieldOnCells is no more useful here : release it
  fieldOnCells->decrRef();
  //! [CppSnippetFieldDoubleBuild1_1]
  arrX=ParaMEDMEM::DataArrayDouble::New(); arrX->alloc(9,1); std::copy(XCoords,XCoords+9,arrX->getPointer()); arrX->setInfoOnComponent(0,"X [m]");
  arrY=ParaMEDMEM::DataArrayDouble::New(); arrY->alloc(7,1); std::copy(YCoords,YCoords+7,arrY->getPointer()); arrY->setInfoOnComponent(0,"Y [m]"); 
  mesh=ParaMEDMEM::MEDCouplingCMesh::New("My2D_CMesh");
  mesh->setCoords(arrX,arrY); arrX->decrRef(); arrY->decrRef();
  //! [CppSnippetFieldDoubleBuild1_2]
  ParaMEDMEM::MEDCouplingFieldDouble *f1=mesh->fillFromAnalytic(ParaMEDMEM::ON_CELLS,1,"x*x+y*y*3+2.*x");//f1 is scalar
  ParaMEDMEM::MEDCouplingFieldDouble *f2=mesh->fillFromAnalytic(ParaMEDMEM::ON_CELLS,1,"cos(x+y/x)");//f2 is scalar too
  ParaMEDMEM::MEDCouplingFieldDouble *f2bis=mesh->fillFromAnalytic(ParaMEDMEM::ON_CELLS,2,"x*x*IVec+3*y*JVec");//f2bis is a vectors field
  ParaMEDMEM::MEDCouplingFieldDouble *f3=(*f1)+(*f2);//f3 scalar
  ParaMEDMEM::MEDCouplingFieldDouble *f4=(*f3)/(*f2);//f4 scalar
  f2bis->applyFunc(1,"sqrt(x*x+y*y)");//f2bis becomes scalar
  ParaMEDMEM::MEDCouplingFieldDouble *f5=(*f2bis)*(*f4);//f5 scalar
  const double pos1[2]={0.48,0.38};
  double res;
  f4->getValueOn(pos1,&res);//f4 is scalar so the returned value is of size 1.
  // ...
  //! [CppSnippetFieldDoubleBuild1_2]
  mesh->decrRef();
  //! [CppSnippetFieldDoubleBuild1_3]
  // f1, f2, f2bis, f3, f4, f5 are no more useful here : release them
  f1->decrRef();
  f2->decrRef();
  f2bis->decrRef();
  f3->decrRef();
  f4->decrRef();
  f5->decrRef();
  //! [CppSnippetFieldDoubleBuild1_3]
}

void CppSnippetFieldDoubleBuild2()
{
  double XCoords[9]={-0.3,0.,0.1,0.3,0.45,0.47,0.49,1.,1.22};
  double YCoords[7]={0.,0.1,0.37,0.45,0.47,0.49,1.007};
  ParaMEDMEM::DataArrayDouble *arrX=ParaMEDMEM::DataArrayDouble::New(); arrX->alloc(9,1); std::copy(XCoords,XCoords+9,arrX->getPointer()); arrX->setInfoOnComponent(0,"X [m]");
  ParaMEDMEM::DataArrayDouble *arrY=ParaMEDMEM::DataArrayDouble::New(); arrY->alloc(7,1); std::copy(YCoords,YCoords+7,arrY->getPointer()); arrY->setInfoOnComponent(0,"Y [m]"); 
  ParaMEDMEM::MEDCouplingCMesh *mesh=ParaMEDMEM::MEDCouplingCMesh::New("My2D_CMesh");
  mesh->setCoords(arrX,arrY); arrX->decrRef(); arrY->decrRef();
  //! [CppSnippetFieldDoubleBuild2_1]
  ParaMEDMEM::MEDCouplingFieldDouble* fieldOnNodes=ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_NODES,ParaMEDMEM::NO_TIME);
  fieldOnNodes->setName("MyScalarFieldOnNodeNoTime");
  fieldOnNodes->setMesh(mesh);
  mesh->decrRef(); // no more need of mesh because mesh has been attached to fieldOnNodes
  ParaMEDMEM::DataArrayDouble *array=ParaMEDMEM::DataArrayDouble::New();
  array->alloc(fieldOnNodes->getMesh()->getNumberOfNodes(),1);//Implicitely fieldOnNodes will be a 1 component field.
  array->fillWithValue(8.);
  fieldOnNodes->setArray(array);
  array->decrRef();
  // fieldOnNodes is now usable
  // ...
  // fieldOnNodes is no more useful here : release it
  fieldOnNodes->decrRef();
  //! [CppSnippetFieldDoubleBuild2_1]
}

void CppSnippetFieldDoubleBuild3()
{
  double XCoords[9]={-0.3,0.,0.1,0.3,0.45,0.47,0.49,1.,1.22};
  double YCoords[7]={0.,0.1,0.37,0.45,0.47,0.49,1.007};
  ParaMEDMEM::DataArrayDouble *arrX=ParaMEDMEM::DataArrayDouble::New(); arrX->alloc(9,1); std::copy(XCoords,XCoords+9,arrX->getPointer()); arrX->setInfoOnComponent(0,"X [m]");
  ParaMEDMEM::DataArrayDouble *arrY=ParaMEDMEM::DataArrayDouble::New(); arrY->alloc(7,1); std::copy(YCoords,YCoords+7,arrY->getPointer()); arrY->setInfoOnComponent(0,"Y [m]"); 
  ParaMEDMEM::MEDCouplingCMesh *mesh=ParaMEDMEM::MEDCouplingCMesh::New("My2D_CMesh");
  mesh->setCoords(arrX,arrY); arrX->decrRef(); arrY->decrRef();
  //! [CppSnippetFieldDoubleBuild3_1]
  ParaMEDMEM::MEDCouplingFieldDouble* fieldOnCells=ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_CELLS,ParaMEDMEM::ONE_TIME);
  fieldOnCells->setName("MyTensorFieldOnCellNoTime");
  fieldOnCells->setTimeUnit("ms"); // Time unit is ms.
  fieldOnCells->setTime(4.22,2,-1); // Time attached is 4.22 ms, iteration id is 2 and order id (or sub iteration id) is -1
  fieldOnCells->setMesh(mesh);
  mesh->decrRef(); // no more need of mesh because mesh has been attached to fieldOnCells
  ParaMEDMEM::DataArrayDouble *array=ParaMEDMEM::DataArrayDouble::New();
  array->alloc(fieldOnCells->getMesh()->getNumberOfCells(),2);//Implicitely fieldOnCells will be a 2 components field.
  array->fillWithValue(7.);
  fieldOnCells->setArray(array);
  array->decrRef();
  // fieldOnCells is now usable
  // ...
  // fieldOnCells is no more useful here : release it
  fieldOnCells->decrRef();
  //! [CppSnippetFieldDoubleBuild3_1]
}

void CppSnippetFieldDoubleBuild4()
{
  double XCoords[9]={-0.3,0.,0.1,0.3,0.45,0.47,0.49,1.,1.22};
  double YCoords[7]={0.,0.1,0.37,0.45,0.47,0.49,1.007};
  ParaMEDMEM::DataArrayDouble *arrX=ParaMEDMEM::DataArrayDouble::New(); arrX->alloc(9,1); std::copy(XCoords,XCoords+9,arrX->getPointer()); arrX->setInfoOnComponent(0,"X [m]");
  ParaMEDMEM::DataArrayDouble *arrY=ParaMEDMEM::DataArrayDouble::New(); arrY->alloc(7,1); std::copy(YCoords,YCoords+7,arrY->getPointer()); arrY->setInfoOnComponent(0,"Y [m]"); 
  ParaMEDMEM::MEDCouplingCMesh *mesh=ParaMEDMEM::MEDCouplingCMesh::New("My2D_CMesh");
  mesh->setCoords(arrX,arrY); arrX->decrRef(); arrY->decrRef();
  //! [CppSnippetFieldDoubleBuild4_1]
  ParaMEDMEM::MEDCouplingFieldDouble* fieldOnNodes=ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_NODES,ParaMEDMEM::CONST_ON_TIME_INTERVAL);
  fieldOnNodes->setName("MyVecFieldOnNodeWithConstTime");
  fieldOnNodes->setTimeUnit("ms"); // Time unit is ms.
  fieldOnNodes->setStartTime(4.22,2,-1);
  fieldOnNodes->setEndTime(6.44,4,-1); // fieldOnNodes is defined in interval [4.22 ms,6.44 ms] 
  fieldOnNodes->setMesh(mesh);
  mesh->decrRef(); // no more need of mesh because mesh has been attached to fieldOnNodes
  ParaMEDMEM::DataArrayDouble *array=ParaMEDMEM::DataArrayDouble::New();
  array->alloc(fieldOnNodes->getMesh()->getNumberOfNodes(),3);//Implicitely fieldOnNodes will be a 3 components field.
  array->fillWithValue(8.);
  fieldOnNodes->setArray(array);
  array->decrRef();
  // fieldOnNodes is now usable
  // ...
  // fieldOnNodes is no more useful here : release it
  fieldOnNodes->decrRef();
  //! [CppSnippetFieldDoubleBuild4_1]
}

int main(int argc, char *argv[])
{
  CppExample_MEDCouplingPointSet_getCoordsAt();
  CppExample_MEDCouplingUMesh_areCellsIncludedIn();
  CppExample_MEDCouplingUMesh_findAndCorrectBadOriented3DExtrudedCells();
  CppExample_MEDCouplingUMesh_arePolyhedronsNotCorrectlyOriented();
  CppExample_MEDCouplingUMesh_are2DCellsNotCorrectlyOriented();
  CppExample_MEDCouplingUMesh_getCellsContainingPoints();
  CppExample_MEDCouplingUMesh_getCellsContainingPoint();
  CppExample_MEDCouplingUMesh_buildPartOrthogonalField();
  CppExample_MEDCouplingUMesh_getPartMeasureField();
  CppExample_MEDCouplingUMesh_getCellsInBoundingBox();
  CppExample_MEDCouplingUMesh_renumberNodesInConn();
  CppExample_MEDCouplingUMesh_renumberNodes();
  CppExample_MEDCouplingUMesh_findBoundaryNodes();
  CppExample_MEDCouplingUMesh_buildBoundaryMesh();
  CppExample_MEDCouplingUMesh_buildFacePartOfMySelfNode();
  CppExample_MEDCouplingUMesh_buildPartOfMySelfNode();
  CppExample_MEDCouplingUMesh_getCellIdsLyingOnNodes();
  CppExample_MEDCouplingUMesh_getCellIdsFullyIncludedInNodeIds();
  CppExample_MEDCouplingUMesh_buildPartOfMySelf();
  CppExample_MEDCouplingUMesh_mergeNodes();
  CppExample_MEDCouplingUMesh_zipConnectivityTraducer();
  CppExample_MEDCouplingUMesh_zipCoordsTraducer();
  CppExample_MEDCouplingUMesh_getNodeIdsInUse();
  CppExample_MEDCouplingUMesh_convertToPolyTypes();
  CppExample_MEDCouplingUMesh_buildDescendingConnectivity2();
  CppExample_MEDCouplingUMesh_buildDescendingConnectivity();
  CppExample_MEDCouplingUMesh_getReverseNodalConnectivity();
  CppExample_MEDCouplingUMesh_checkDeepEquivalWith();
  CppExample_MEDCouplingPointSet_scale();
  CppExample_MEDCouplingPointSet_translate();
  CppExample_MEDCouplingPointSet_rotate();
  CppExample_MEDCouplingPointSet_getBoundingBox();
  CppExample_MEDCouplingPointSet_getNodeIdsNearPoint();
  CppExample_MEDCouplingPointSet_getNodeIdsNearPoints();
  CppExample_MEDCouplingPointSet_findCommonNodes();
  CppExample_MEDCouplingPointSet_getCoordinatesOfNode();
  CppExample_DataArrayInt_buildPermutationArr();
  CppExample_DataArrayInt_invertArrayO2N2N2O();
  CppExample_DataArrayInt_invertArrayN2O2O2N();
  CppExample_DataArrayDouble_getIdsInRange();
  CppExample_DataArrayDouble_findCommonTuples();
  CppExample_DataArrayDouble_Meld1();
  CppExampleFieldDoubleBuildSubPart1();
  CppSnippetUMeshStdBuild1();
  CppSnippetUMeshAdvBuild1();
  CppSnippetDataArrayBuild1();
  CppSnippetCMeshStdBuild1();
  CppSnippetFieldDoubleBuild1();
  CppSnippetFieldDoubleBuild2();
  CppSnippetFieldDoubleBuild3();
  CppSnippetFieldDoubleBuild4();
  return 0;
}
