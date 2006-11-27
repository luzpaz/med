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
//

#include "MEDMEMTest.hxx"
#include <cppunit/TestAssert.h>

#include "MEDMEM_define.hxx"
#include "MEDMEM_Grid.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Med.hxx"
#include "MEDMEM_MedMedDriver.hxx"

#include <sstream>
#include <cmath>

/*!
 *  Check methods (44), defined in MEDMEM_Grid.hxx:
 *  class GRID: public MESH {
 *   (+) GRID();
 *   (yetno) GRID(const MED_EN::med_grid_type type);
 *   (yetno) GRID(const GRID &m);
 *   (yetno) GRID(driverTypes driverType, const string & fileName="",const string & meshName="");
 *   (yetno) GRID(const std::vector<std::vector<double> >& xyz_array,
 *                const std::vector<std::string>& coord_name,
 *                const std::vector<std::string>& coord_unit,
 *                const MED_EN::med_grid_type type=MED_EN::MED_CARTESIAN);
 *   (yetno) GRID & operator=(const GRID &m);
 *   (yetno) virtual ~GRID();
 *   (yetno) virtual void init();
 *   (yetno) void fillCoordinates() const;
 *   (yetno) void fillConnectivity() const;
 *   (yetno) inline void makeUnstructured();
 *   (yetno) void fillMeshAfterRead();
 *   (yetno) void writeUnstructured(int index=0, const string & driverName = "");
 *   (yetno) void read(int index=0);
 *   (+) inline int getNodeNumber(const int i, const int j=0, const int k=0) const;
 *   (+) inline int getCellNumber(const int i, const int j=0, const int k=0) const;
 *   (+) int getEdgeNumber
 *           (const int Axis, const int i, const int j=0, const int k=0) const throw (MEDEXCEPTION);
 *   (+) int getFaceNumber
 *           (const int Axis, const int i, const int j=0, const int k=0) const throw (MEDEXCEPTION);
 *   (+) void getNodePosition(const int Number, int& i, int& j, int& k) const throw (MEDEXCEPTION);
 *   (+) void getCellPosition(const int Number, int& i, int& j, int& k) const throw (MEDEXCEPTION);
 *   (+) void getEdgePosition
 *           (const int Number, int& Axis, int& i, int& j, int& k) const throw (MEDEXCEPTION);
 *   (+) void getFacePosition
 *           (const int Number, int& Axis, int& i, int& j, int& k) const throw (MEDEXCEPTION);
 *   (+) inline MED_EN::med_grid_type getGridType() const;
 *   (+) int getArrayLength(const int Axis) const throw (MEDEXCEPTION);
 *   (yetno) const double getArrayValue (const int Axis, const int i) const throw (MEDEXCEPTION);
 *   (yetno) inline const COORDINATE * getCoordinateptr() const;
 *   (+) inline const double * getCoordinates(MED_EN::medModeSwitch Mode) const;
 *   (+) inline const double getCoordinate(int Number,int Axis) const;
 *   (+) inline int getNumberOfTypes(MED_EN::medEntityMesh Entity) const;
 *   (yetno) inline int getNumberOfTypesWithPoly(MED_EN::medEntityMesh Entity) const;
 *   (+) inline const MED_EN::medGeometryElement * getTypes(MED_EN::medEntityMesh Entity) const;
 *   (yetno) MED_EN::medGeometryElement * getTypesWithPoly(MED_EN::medEntityMesh Entity) const;
 *   (yetno) inline const CELLMODEL * getCellsTypes(MED_EN::medEntityMesh Entity) const;
 *   (+) const int * getGlobalNumberingIndex(MED_EN::medEntityMesh Entity) const;
 *   (+) inline int getNumberOfElements
 *                (MED_EN::medEntityMesh Entity, MED_EN::medGeometryElement Type) const;
 *   (yetno) inline int getNumberOfElementsWithPoly
 *                (MED_EN::medEntityMesh Entity, MED_EN::medGeometryElement Type) const;
 *   (+) inline bool existConnectivity
 *                (MED_EN::medConnectivity ConnectivityType, MED_EN::medEntityMesh Entity) const;
 *   (+) inline MED_EN::medGeometryElement getElementType
 *                (MED_EN::medEntityMesh Entity, int Number) const;
 *   (yetno) inline MED_EN::medGeometryElement getElementTypeWithPoly
 *                (MED_EN::medEntityMesh Entity, int Number) const;
 *   (+) inline void calculateConnectivity(MED_EN::medModeSwitch Mode,
 *                                             MED_EN::medConnectivity ConnectivityType,
 *                                               MED_EN::medEntityMesh Entity) const;
 *   (yetno) inline const CONNECTIVITY* getConnectivityptr() const;
 *   (+) inline const int * getConnectivity
 *             (MED_EN::medModeSwitch Mode, MED_EN::medConnectivity ConnectivityType,
 *                MED_EN::medEntityMesh Entity, MED_EN::medGeometryElement Type) const;
 *   (+) inline const int * getConnectivityIndex(MED_EN::medConnectivity ConnectivityType,
 *                                          MED_EN::medEntityMesh Entity) const;
 *   (+) inline const int * getReverseConnectivity(MED_EN::medConnectivity ConnectivityType,
 *                                            MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const;
 *   (+) inline const int * getReverseConnectivityIndex(MED_EN::medConnectivity ConnectivityType,
 *                                                 MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const;
 *   (yetno) inline void setGridType(MED_EN::med_grid_type gridType);
 *  }
 */
void MEDMEMTest::testGrid()
{
  string datadir  = getenv("DATA_DIR");
  string filename = datadir + "/MedFiles/test19.med" ;

  MED * myMed= new MED() ;
  MED_MED_RDONLY_DRIVER myMeshDriver(filename,myMed);
  myMeshDriver.open();
  myMeshDriver.readFileStruct();
  myMeshDriver.close();

  int nbMeshes = myMed->getNumberOfMeshes();
  CPPUNIT_ASSERT(nbMeshes);

  //////////////////////////////
  // test1 "CARTESIAN GRID"    //
  //////////////////////////////

  deque<string> mesh_names = myMed->getMeshNames();
  CPPUNIT_ASSERT(mesh_names.size() != 0);
  MESH* myMesh = NULL;

  myMesh = myMed->getMesh(mesh_names[0]);
  myMesh->read();
  
  CPPUNIT_ASSERT(myMesh != NULL);
  CPPUNIT_ASSERT(myMesh->getIsAGrid());

  GRID* myGrid = dynamic_cast<GRID*>(myMesh);
  CPPUNIT_ASSERT(myGrid);

  CPPUNIT_ASSERT_THROW(myGrid->getArrayLength(0), MEDEXCEPTION);

  int I, J, K;
  CPPUNIT_ASSERT_NO_THROW(I = myGrid->getArrayLength(1));
  CPPUNIT_ASSERT_NO_THROW(J = myGrid->getArrayLength(2));
  CPPUNIT_ASSERT_NO_THROW(K = myGrid->getArrayLength(3));

  CPPUNIT_ASSERT(I);
  CPPUNIT_ASSERT(J);

  med_grid_type grid_type = myGrid->getGridType();
  CPPUNIT_ASSERT_MESSAGE("Wrong grid type", grid_type == MED_CARTESIAN);

  const double * coordinates = myGrid->getCoordinates(MED_FULL_INTERLACE);
  int SpaceDimension = myGrid->getSpaceDimension();
  for (int axe = 0; axe < SpaceDimension; axe++) {
    try
    {
      for (int num = 0; num < myGrid->getNumberOfNodes(); num++) {
	try
        {
	  const double coordinate = myGrid->getCoordinate(num + 1, axe + 1);
	  cout<<"coordinate = "<<coordinate<<endl;
	  CPPUNIT_ASSERT(fabs(coordinate - coordinates[(num * SpaceDimension)+axe]) < 0.001);
	}
	catch (const std::exception &e)
        {
	  CPPUNIT_FAIL(e.what());
	}
	catch (...)
        {
	  CPPUNIT_FAIL("Unknown exception");
	}
      }
    }
    catch (const std::exception &e)
    {
      CPPUNIT_FAIL(e.what());
    }
    catch (...)
    {
      CPPUNIT_FAIL("Unknown exception");
    }
  }

  for (int axe = 0; axe < SpaceDimension; axe++) {
    try
    {
      for (int num = 0; num < myGrid->getArrayLength(axe+1); num++) {
	const double arrayValue = myGrid->getArrayValue(axe + 1, num);
	//cout<<"arrayValue = "<<arrayValue<<"coordinates[(num+axe) * SpaceDimension] = "<<coordinates[(num+axe) * SpaceDimension]<<endl;
	//CPPUNIT_ASSERT(fabs(coordinates[(num+axe) * SpaceDimension)] - arrayValue) < 0.001);//!!!!!!!
      }
    }
    catch (const std::exception &e)
    {
      CPPUNIT_FAIL(e.what());
    }
    catch (...)
    {
      CPPUNIT_FAIL("Unknown exception");
    }
  }

  int nbTypesCell = myGrid->getNumberOfTypes(MED_CELL);
  CPPUNIT_ASSERT(nbTypesCell == 1);

  const medGeometryElement* types;
  CPPUNIT_ASSERT_NO_THROW( types = myGrid->getTypes(MED_CELL) );
  CPPUNIT_ASSERT( types[0] == MED_QUAD4);

  int nbElem = 0;
  try{
    nbElem = myGrid->getNumberOfElements(MED_CELL,types[0]);
    CPPUNIT_ASSERT(nbElem);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  int nbNodes = myGrid->getNumberOfNodes();
  CPPUNIT_ASSERT(nbNodes);

  int ijkNode[3];
  int NodeNumber;
  for(int nbNode = 1; nbNode <= nbNodes; nbNode++)
  {
    CPPUNIT_ASSERT_NO_THROW(myGrid->getNodePosition(nbNode, ijkNode[0], ijkNode[1], ijkNode[2]));
    try{
      NodeNumber = myGrid->getNodeNumber(ijkNode[0], ijkNode[1], ijkNode[2]);
    }
    catch (const std::exception &e)
    {
      CPPUNIT_FAIL(e.what());
    }
    catch (...)
    {
      CPPUNIT_FAIL("Unknown exception");
    }
    CPPUNIT_ASSERT(NodeNumber == nbNode);
  }

  int nbCells = myGrid->getNumberOfElements(MED_CELL, MED_ALL_ELEMENTS);
  CPPUNIT_ASSERT(nbCells);

  int ijkCell[3];
  int CellNumber;
  for(int nbCell = 1; nbCell <= nbCells; nbCell++)
  {
    try{
      myGrid->getCellPosition(nbCell, ijkCell[0], ijkCell[1], ijkCell[2]);
    }
    catch (const std::exception &e)
    {
      CPPUNIT_FAIL(e.what());
    }
    catch (...)
    {
      CPPUNIT_FAIL("Unknown exception");
    }

    try{
      CellNumber = myGrid->getCellNumber(ijkCell[0], ijkCell[1], ijkCell[2]);
    }
    catch (const std::exception &e)
    {
      CPPUNIT_FAIL(e.what());
    }
    catch (...)
    {
      CPPUNIT_FAIL("Unknown exception");
    }
    CPPUNIT_ASSERT(CellNumber == nbCell);
  }

  int nbEdges = myGrid->getNumberOfElements(MED_EDGE, MED_ALL_ELEMENTS);
  CPPUNIT_ASSERT(nbEdges);

  int ijkAxisEdge[4];
  int EdgeNumber;
  for(int nbEdge = 1; nbEdge <= nbEdges; nbEdge++)
  {    
    CPPUNIT_ASSERT_NO_THROW( myGrid->getEdgePosition(nbEdge, ijkAxisEdge[0], ijkAxisEdge[1], ijkAxisEdge[2], ijkAxisEdge[3]));
    CPPUNIT_ASSERT_NO_THROW( EdgeNumber = myGrid->getEdgeNumber(ijkAxisEdge[0], ijkAxisEdge[1], ijkAxisEdge[2], ijkAxisEdge[3]));
    CPPUNIT_ASSERT(EdgeNumber == nbEdge);
  }

  int nbFaces = myGrid->getNumberOfElements(MED_FACE, MED_ALL_ELEMENTS);

  CPPUNIT_ASSERT(nbFaces == 0);

  CPPUNIT_FAIL("ERROR: nbFaces == 0, bat getFacePosition(AnyNumber, ...) - return position(i,j,k)");
  int ijkAxisFace[4];
  CPPUNIT_ASSERT_NO_THROW( myGrid->getFacePosition(6,ijkAxisFace[0], ijkAxisFace[1], ijkAxisFace[2], ijkAxisFace[3]));
  CPPUNIT_ASSERT(ijkAxisFace[0]);
  CPPUNIT_ASSERT(ijkAxisFace[1]);
  CPPUNIT_ASSERT(ijkAxisFace[2]);

  /* int FaceNumber;
  for(int nbFace = 1; nbFace <= nbFaces; nbFace++)
  {    
    CPPUNIT_ASSERT_NO_THROW( myGrid->getFacePosition(nbFace, ijkAxisFace[0], ijkAxisFace[1], ijkAxisFace[2], ijkAxisFace[3]));
    CPPUNIT_ASSERT_NO_THROW( EdgeNumber = myGrid->getEdgeNumber(ijkAxisFace[0], ijkAxisFace[1], ijkAxisFace[2], ijkAxisFace[3]));
    CPPUNIT_ASSERT(FaceNumber == nbFace);
  }*/


  bool existConnect = false;
  CPPUNIT_ASSERT_NO_THROW(existConnect = myGrid->existConnectivity(MED_NODAL, MED_CELL));
  if(!existConnect){
    CPPUNIT_ASSERT_NO_THROW(myGrid->calculateConnectivity(MED_FULL_INTERLACE, MED_NODAL, MED_CELL));
    CPPUNIT_ASSERT(myGrid->existConnectivity(MED_NODAL, MED_CELL));
  }


  const int* Connectivity;
  const int* connectivity_index;
  CPPUNIT_ASSERT_NO_THROW(Connectivity = myGrid->getConnectivity(MED_FULL_INTERLACE,MED_NODAL,MED_CELL,types[0]));
  CPPUNIT_ASSERT_NO_THROW(connectivity_index = myGrid->getConnectivityIndex(MED_NODAL, MED_CELL));
  cout<<"Nodal connectivity"<<endl;
  for (int j = 0; j < nbElem; j++) {
    cout << "Element "<< j+1 << " : ";
    for (int k = connectivity_index[j]; k < connectivity_index[j+1]; k++)
      cout << Connectivity[k-1] << " ";
    cout << endl;
  }

  const int * ReverseNodalConnectivity;
  const int * ReverseConnectivityIndex;
  CPPUNIT_ASSERT_NO_THROW(ReverseNodalConnectivity = myGrid->getReverseConnectivity(MED_NODAL));
  CPPUNIT_ASSERT_NO_THROW(ReverseConnectivityIndex = myGrid->getReverseConnectivityIndex(MED_NODAL));
  for (int i = 0; i < nbNodes; i++) {
    cout << "Node "<< i+1 << " : ";
    for (int j = ReverseConnectivityIndex[i]; j < ReverseConnectivityIndex[i+1]; j++)
      cout << ReverseNodalConnectivity[j-1] << " ";
    cout << endl;
  }

  const int* myGlobalNbIdx;
  CPPUNIT_ASSERT_NO_THROW(myGlobalNbIdx = myGrid->getGlobalNumberingIndex(MED_CELL));
  for(int i = 0; i <= nbTypesCell; i++)
  {
    if( i == nbTypesCell ){
      CPPUNIT_ASSERT_THROW(myGrid->getElementType(MED_CELL, myGlobalNbIdx[i]), MEDEXCEPTION);
      break;
    }
    medGeometryElement aElem;
    CPPUNIT_ASSERT_NO_THROW(aElem = myGrid->getElementType(MED_CELL, myGlobalNbIdx[i]));
    CPPUNIT_ASSERT(types[0] == aElem);
  }

  CPPUNIT_ASSERT_NO_THROW(existConnect = myGrid->existConnectivity(MED_DESCENDING, MED_CELL));
  if(!existConnect){
    CPPUNIT_ASSERT_NO_THROW(myGrid->calculateConnectivity(MED_FULL_INTERLACE, MED_DESCENDING, MED_CELL));
    CPPUNIT_ASSERT(myGrid->existConnectivity(MED_DESCENDING, MED_CELL));
  }

  const int* ConnectivityDes;
  const int* connectivity_index_des;
  CPPUNIT_ASSERT_NO_THROW(ConnectivityDes = myGrid->getConnectivity(MED_FULL_INTERLACE,MED_DESCENDING,MED_CELL,MED_ALL_ELEMENTS));
  CPPUNIT_ASSERT_NO_THROW(connectivity_index_des = myGrid->getConnectivityIndex(MED_DESCENDING, MED_CELL));
  cout<<"Descending connectivity"<<endl;
  for (int j = 0; j < nbElem; j++) {
    cout << "Element "<< j+1 << " : ";
    for (int k = connectivity_index_des[j]; k < connectivity_index_des[j+1]; k++)
      cout << ConnectivityDes[k-1] << " ";
    cout << endl;
  }

  const int * ReverseDesConnectivity;
  const int * ReverseConnectivityIndexDes;
  CPPUNIT_ASSERT_NO_THROW(ReverseDesConnectivity = myGrid->getReverseConnectivity(MED_DESCENDING));
  CPPUNIT_ASSERT_NO_THROW(ReverseConnectivityIndexDes = myGrid->getReverseConnectivityIndex(MED_DESCENDING));
  for (int i = 0; i < nbNodes; i++) {
    cout << "Node "<< i+1 << " : ";
    for (int j = ReverseConnectivityIndexDes[i]; j < ReverseConnectivityIndexDes[i+1]; j++)
      cout << ReverseDesConnectivity[j-1] << " ";
    cout << endl;
  }

  //////////////////////////////
  // test2 "MED_BODY_FITTED"  //
  //////////////////////////////
  MESH* myMesh1 = NULL;

  myMesh1 = myMed->getMesh(mesh_names[1]);
  myMesh1->read();
  
  CPPUNIT_ASSERT(myMesh1 != NULL);
  CPPUNIT_ASSERT(myMesh1->getIsAGrid());

  GRID* myGrid1 = dynamic_cast<GRID*>(myMesh1);
  CPPUNIT_ASSERT(myGrid1);

  CPPUNIT_ASSERT_NO_THROW(I = myGrid1->getArrayLength(1));
  CPPUNIT_ASSERT_NO_THROW(J = myGrid1->getArrayLength(2));
  CPPUNIT_ASSERT_NO_THROW(K = myGrid1->getArrayLength(3));

  CPPUNIT_ASSERT(I == 2);
  CPPUNIT_ASSERT(J == 2);

  grid_type = myGrid1->getGridType();
  CPPUNIT_ASSERT_MESSAGE("Wrong grid type", grid_type == MED_BODY_FITTED);

  nbTypesCell = myGrid1->getNumberOfTypes(MED_CELL);
  CPPUNIT_ASSERT(nbTypesCell == 1);

  const medGeometryElement* types1;
  CPPUNIT_ASSERT_NO_THROW( types1 = myGrid1->getTypes(MED_CELL) );
  CPPUNIT_ASSERT( types1[0] == MED_QUAD4);

  try{
    nbElem = myGrid1->getNumberOfElements(MED_CELL,types[0]);
    CPPUNIT_ASSERT(nbElem);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  const int* BodyConnectivity;
  const int* body_connectivity_index;
  int ijkNodeBody[3];
  CPPUNIT_ASSERT_NO_THROW(BodyConnectivity = myGrid1->getConnectivity(MED_FULL_INTERLACE,MED_NODAL,MED_CELL,types1[0]));
  CPPUNIT_ASSERT_NO_THROW(body_connectivity_index = myGrid1->getConnectivityIndex(MED_NODAL, MED_CELL));
  cout<<"Nodal connectivity"<<endl;
  for (int j = 0; j < nbElem; j++) {
    cout << "Element "<< j+1 << " : ";
    for (int k = body_connectivity_index[j]; k < body_connectivity_index[j+1]; k++){
      CPPUNIT_ASSERT_NO_THROW(myGrid->getNodePosition(BodyConnectivity[k-1], ijkNodeBody[0], ijkNodeBody[1], ijkNodeBody[2]));
      cout << BodyConnectivity[k-1] << " ";
    }
    cout << endl;
  }


  ///////////////////////////////////////////////////
  // test3 "maa1" which in fact is not a pure GRID //
  ///////////////////////////////////////////////////
  MESH* myMesh2 = NULL;

  myMesh2 = myMed->getMesh(mesh_names[2]);
  myMesh2->read();
  
  CPPUNIT_ASSERT(myMesh2 != NULL);
  CPPUNIT_ASSERT(!(myMesh2->getIsAGrid()));

  delete myMed;

  ////////////////////////////
  // test4 create new GRID  //
  ////////////////////////////

}
