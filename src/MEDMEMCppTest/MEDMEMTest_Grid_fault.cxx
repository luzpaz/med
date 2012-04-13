// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "MEDMEMTest.hxx"
#include <cppunit/TestAssert.h>

#include "MEDMEM_define.hxx"
#include "MEDMEM_Grid.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_MedFileBrowser.hxx"
#include "MEDMEM_MedMeshDriver.hxx"

#include <sstream>
#include <cmath>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;


/*!
 *  Check methods (23), defined in MEDMEM_Grid.hxx:
 *  class GRID: public MESH {
 *   (+) GRID();
 *   (+) GRID(const MED_EN::med_grid_type type);
 *   (BUG:operator=() not implemented but init() not called) GRID(const GRID &m);
 *   (+) GRID(driverTypes driverType, const string & fileName="",const string & meshName="");
 *   (+) GRID(const std::vector<std::vector<double> >& xyz_array,
 *                const std::vector<std::string>& coord_name,
 *                const std::vector<std::string>& coord_unit,
 *                const MED_EN::med_grid_type type=MED_EN::MED_CARTESIAN);
 *   (NOT IMPLEMENTED) GRID & operator=(const GRID &m);
 *   (+) virtual ~GRID();
 *   (+) virtual void init();
 *   (+) virtual const MESH * convertInMESH() const
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
 *   (+) const double getArrayValue (const int Axis, const int i) const throw (MEDEXCEPTION);
 *   (+) inline int getNumberOfTypes(MED_EN::medEntityMesh Entity) const;
 *   (+) inline const MED_EN::medGeometryElement * getTypes(MED_EN::medEntityMesh Entity) const;
 *   (+) inline int getNumberOfElements
 *                (MED_EN::medEntityMesh Entity, MED_EN::medGeometryElement Type) const;
 *   (+) inline MED_EN::medGeometryElement getElementType
 *                (MED_EN::medEntityMesh Entity, int Number) const;
 *   (+) inline void setGridType(MED_EN::med_grid_type gridType);
 *  }
 */
static void MEDMEMTest_testGrid()
{
  string filename      = getResourceFile("test19.med") ;
  string filenameout21 = makeTmpFile("myGridWrite_grid21.med");
  ostringstream out;

  // To remove tmp files from disk
  MEDMEMTest_TmpFilesRemover aRemover;
  aRemover.Register(filenameout21);

  // Remove file in advance to ensure it does not exist at the moment of writing,
  // because files are not removed by the MEDMEMTest_TmpFilesRemover in case of
  // Segmentation Fault (in previous tests execution).
  {
    MEDMEMTest_TmpFilesRemover aRemover1;
    aRemover1.Register(filenameout21);
  }

  MEDFILEBROWSER * myMed = new MEDFILEBROWSER(filename);

  int nbMeshes = myMed->getNumberOfMeshes();
  CPPUNIT_ASSERT(nbMeshes);

  vector<string> mesh_names = myMed->getMeshNames();
  CPPUNIT_ASSERT(mesh_names.size() != 0);

  //////////////////////////////
  // test1 "CARTESIAN GRID"   //
  //////////////////////////////
  {
    CPPUNIT_ASSERT(myMed->isStructuredMesh(mesh_names[0]));

    CPPUNIT_ASSERT_THROW( MESH(MED_DRIVER, myMed->getFileName(), mesh_names[0]), MEDEXCEPTION);

    GMESH* myMesh = new GRID(MED_DRIVER, myMed->getFileName(), mesh_names[0]);
    std::auto_ptr<GMESH> meshDeleter(myMesh);

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

    const MESH* mesh = myGrid->convertInMESH();
    const double * coordinates = mesh->getCoordinates(MED_FULL_INTERLACE);
    int SpaceDimension = myGrid->getSpaceDimension();
    for (int axe = 0; axe < SpaceDimension; axe++) {
      for (int num = 0; num < myGrid->getNumberOfNodes(); num++) {
        double coordinate;
        CPPUNIT_ASSERT_NO_THROW(coordinate = mesh->getCoordinate(num + 1, axe + 1));
        //cout << "coordinate = " << coordinate << endl;
        CPPUNIT_ASSERT(fabs(coordinate - coordinates[(num * SpaceDimension)+axe]) < 0.001);
      }
    }

    int nbTypesCell = myGrid->getNumberOfTypes(MED_CELL);
    CPPUNIT_ASSERT(nbTypesCell == 1);

    const medGeometryElement* types;
    CPPUNIT_ASSERT_NO_THROW(types = myGrid->getTypes(MED_CELL));
//#ifdef ENABLE_FORCED_FAILURES
    // Compilation warning about GRID::getTypes():
    // "inline function
    // `virtual const MED_EN::medGeometryElement* MEDMEM::GRID::getTypes(MED_EN::medEntityMesh) const'
    // used but never defined".
    // In MEDMEM_Grid.hxx:
    // inline const MED_EN::medGeometryElement * getTypes(MED_EN::medEntityMesh Entity) const;
    // But implemented in MEDMEM_Grid.cxx:
    //        const MED_EN::medGeometryElement * GRID::getTypes(MED_EN::medEntityMesh entity) const
//    CPPUNIT_FAIL("Problem with GRID::getTypes() method implementation.");
//#endif
    CPPUNIT_ASSERT(types[0] == MED_QUAD4);

    int nbElem = 0;
    CPPUNIT_ASSERT_NO_THROW(nbElem = myGrid->getNumberOfElements(MED_CELL,types[0]));
    CPPUNIT_ASSERT(nbElem);

    int nbNodes = myGrid->getNumberOfNodes();
    CPPUNIT_ASSERT(nbNodes);

    int ijkNode[3];
    int NodeNumber;
    for (int nbNode = 1; nbNode <= nbNodes; nbNode++) {
      CPPUNIT_ASSERT_NO_THROW(myGrid->getNodePosition(nbNode, ijkNode[0], ijkNode[1], ijkNode[2]));
      CPPUNIT_ASSERT_NO_THROW(NodeNumber = myGrid->getNodeNumber(ijkNode[0], ijkNode[1], ijkNode[2]));
      CPPUNIT_ASSERT(NodeNumber == nbNode);
    }

    int nbCells = myGrid->getNumberOfElements(MED_CELL, MED_ALL_ELEMENTS);
    CPPUNIT_ASSERT(nbCells);

    int ijkCell[3];
    int CellNumber;
    for (int nbCell = 1; nbCell <= nbCells; nbCell++) {
      CPPUNIT_ASSERT_NO_THROW(myGrid->getCellPosition(nbCell, ijkCell[0], ijkCell[1], ijkCell[2]));
      CPPUNIT_ASSERT_NO_THROW(CellNumber = myGrid->getCellNumber(ijkCell[0], ijkCell[1], ijkCell[2]));
      CPPUNIT_ASSERT(CellNumber == nbCell);
    }

    int nbEdges = myGrid->getNumberOfElements(MED_EDGE, MED_ALL_ELEMENTS);
    CPPUNIT_ASSERT(nbEdges);

    int ijkAxisEdge[4];
    int EdgeNumber;
    for (int nbEdge = 1; nbEdge <= nbEdges; nbEdge++) {
      CPPUNIT_ASSERT_NO_THROW(myGrid->getEdgePosition(nbEdge, ijkAxisEdge[0], ijkAxisEdge[1],
                                                      ijkAxisEdge[2], ijkAxisEdge[3]));
      CPPUNIT_ASSERT_NO_THROW(EdgeNumber = myGrid->getEdgeNumber(ijkAxisEdge[0], ijkAxisEdge[1],
                                                                 ijkAxisEdge[2], ijkAxisEdge[3]));
      CPPUNIT_ASSERT(EdgeNumber == nbEdge);
    }

    int nbFaces = myGrid->getNumberOfElements(MED_FACE, MED_ALL_ELEMENTS);
    CPPUNIT_ASSERT(nbFaces == 0);

//#ifdef ENABLE_FORCED_FAILURES
    CPPUNIT_FAIL("ERROR: nbFaces == 0, but getFacePosition(AnyNumber, ...) - return position(i,j,k)");
//#endif
    int ijkAxisFace[4];
    CPPUNIT_ASSERT_NO_THROW(myGrid->getFacePosition(6, ijkAxisFace[0], ijkAxisFace[1],
                                                    ijkAxisFace[2], ijkAxisFace[3]));
    CPPUNIT_ASSERT(ijkAxisFace[0]);
    CPPUNIT_ASSERT(ijkAxisFace[1]);
    CPPUNIT_ASSERT(ijkAxisFace[2]);

    /*int FaceNumber;
    for(int nbFace = 1; nbFace <= nbFaces; nbFace++)
    {
      CPPUNIT_ASSERT_NO_THROW( myGrid->getFacePosition(nbFace, ijkAxisFace[0], ijkAxisFace[1],
                                                       ijkAxisFace[2], ijkAxisFace[3]));
      CPPUNIT_ASSERT_NO_THROW( EdgeNumber = myGrid->getEdgeNumber(ijkAxisFace[0], ijkAxisFace[1],
                                                                  ijkAxisFace[2], ijkAxisFace[3]));
      CPPUNIT_ASSERT(FaceNumber == nbFace);
    }*/

    bool existConnect = false;
    CPPUNIT_ASSERT_NO_THROW(existConnect = mesh->existConnectivity(MED_NODAL, MED_CELL));
    if (!existConnect) {
      CPPUNIT_ASSERT_NO_THROW(mesh->calculateConnectivity( MED_NODAL, MED_CELL));
      CPPUNIT_ASSERT(mesh->existConnectivity(MED_NODAL, MED_CELL));
    }

    const int* Connectivity;
    const int* connectivity_index;
    CPPUNIT_ASSERT_NO_THROW(Connectivity = mesh->getConnectivity( MED_NODAL,
                                                                 MED_CELL, types[0]));
    CPPUNIT_ASSERT_NO_THROW(connectivity_index = mesh->getConnectivityIndex(MED_NODAL, MED_CELL));
    out << "Nodal connectivity" << endl;
    for (int j = 0; j < nbElem; j++) {
      out << "Element "<< j+1 << " : ";
      for (int k = connectivity_index[j]; k < connectivity_index[j+1]; k++)
        out << Connectivity[k-1] << " ";
      out << endl;
    }

    const int * ReverseNodalConnectivity;
    const int * ReverseConnectivityIndex;
    CPPUNIT_ASSERT_NO_THROW(ReverseNodalConnectivity = mesh->getReverseConnectivity(MED_NODAL));
    CPPUNIT_ASSERT_NO_THROW(ReverseConnectivityIndex = mesh->getReverseConnectivityIndex(MED_NODAL));
    for (int i = 0; i < nbNodes; i++) {
      out << "Node "<< i+1 << " : ";
      for (int j = ReverseConnectivityIndex[i]; j < ReverseConnectivityIndex[i+1]; j++)
        out << ReverseNodalConnectivity[j-1] << " ";
      out << endl;
    }

    const int* myGlobalNbIdx;
    CPPUNIT_ASSERT_NO_THROW(myGlobalNbIdx = mesh->getGlobalNumberingIndex(MED_CELL));
    for (int i = 0; i <= nbTypesCell; i++) {
      if (i == nbTypesCell) {
        CPPUNIT_ASSERT_THROW(myGrid->getElementType(MED_CELL, myGlobalNbIdx[i]), MEDEXCEPTION);
        break;
      }
      medGeometryElement aElem;
      CPPUNIT_ASSERT_NO_THROW(aElem = myGrid->getElementType(MED_CELL, myGlobalNbIdx[i]));
      CPPUNIT_ASSERT(types[0] == aElem);
    }

    CPPUNIT_ASSERT_NO_THROW(existConnect = mesh->existConnectivity(MED_DESCENDING, MED_CELL));
    if (!existConnect) {
      CPPUNIT_ASSERT_NO_THROW(mesh->calculateConnectivity( MED_DESCENDING, MED_CELL));
      CPPUNIT_ASSERT(mesh->existConnectivity(MED_DESCENDING, MED_CELL));
    }

    const int* ConnectivityDes;
    const int* connectivity_index_des;
    CPPUNIT_ASSERT_NO_THROW(ConnectivityDes = mesh->getConnectivity( MED_DESCENDING,
                                                                    MED_CELL, MED_ALL_ELEMENTS));
    CPPUNIT_ASSERT_NO_THROW(connectivity_index_des =
                            mesh->getConnectivityIndex(MED_DESCENDING, MED_CELL));
    out<<"Descending connectivity"<<endl;
    for (int j = 0; j < nbElem; j++) {
      out << "Element "<< j+1 << " : ";
      for (int k = connectivity_index_des[j]; k < connectivity_index_des[j+1]; k++)
        out << ConnectivityDes[k-1] << " ";
      out << endl;
    }

    const int * ReverseDesConnectivity;
    const int * ReverseConnectivityIndexDes;
    CPPUNIT_ASSERT_NO_THROW(ReverseDesConnectivity = mesh->getReverseConnectivity(MED_DESCENDING));
    CPPUNIT_ASSERT_NO_THROW(ReverseConnectivityIndexDes =
                            mesh->getReverseConnectivityIndex(MED_DESCENDING));
    for (int i = 0; i < nbNodes; i++) {
      out << "Node "<< i+1 << " : ";
      for (int j = ReverseConnectivityIndexDes[i]; j < ReverseConnectivityIndexDes[i+1]; j++)
        out << ReverseDesConnectivity[j-1] << " ";
      out << endl;
    }
    mesh->removeReference();

  }

  //////////////////////////////
  // test2 "MED_BODY_FITTED"  //
  //////////////////////////////
  {
    CPPUNIT_ASSERT_THROW( MESH(MED_DRIVER, myMed->getFileName(), mesh_names[1]), MEDEXCEPTION);

    GMESH* myMesh1 = new GRID(MED_DRIVER,myMed->getFileName(),mesh_names[1]);
    std::auto_ptr<GMESH> meshDeleter(myMesh1);

    CPPUNIT_ASSERT(myMesh1 != NULL);
    CPPUNIT_ASSERT(myMesh1->getIsAGrid());

    GRID* myGrid1 = dynamic_cast<GRID*>(myMesh1);
    CPPUNIT_ASSERT(myGrid1);

    int I, J, K;
    CPPUNIT_ASSERT_NO_THROW(I = myGrid1->getArrayLength(1));
    CPPUNIT_ASSERT_NO_THROW(J = myGrid1->getArrayLength(2));
    CPPUNIT_ASSERT_NO_THROW(K = myGrid1->getArrayLength(3));

    CPPUNIT_ASSERT(I == 2);
    CPPUNIT_ASSERT(J == 2);

    med_grid_type grid_type = myGrid1->getGridType();
    CPPUNIT_ASSERT_MESSAGE("Wrong grid type", grid_type == MED_BODY_FITTED);

    int nbTypesCell = myGrid1->getNumberOfTypes(MED_CELL);
    CPPUNIT_ASSERT(nbTypesCell == 1);

    const medGeometryElement* types1;
    CPPUNIT_ASSERT_NO_THROW( types1 = myGrid1->getTypes(MED_CELL) );
    CPPUNIT_ASSERT( types1[0] == MED_QUAD4);

    int nbElem;
    CPPUNIT_ASSERT_NO_THROW(nbElem = myGrid1->getNumberOfElements(MED_CELL, types1[0]));
    CPPUNIT_ASSERT(nbElem);

    const int* BodyConnectivity;
    const int* body_connectivity_index;
    int ijkNodeBody[3];
    const MESH* mesh = myGrid1->convertInMESH();
    CPPUNIT_ASSERT_NO_THROW(BodyConnectivity = mesh->getConnectivity( MED_NODAL,
                                                                     MED_CELL, types1[0]));
    CPPUNIT_ASSERT_NO_THROW(body_connectivity_index = mesh->getConnectivityIndex(MED_NODAL, MED_CELL));
    out<<"Nodal connectivity"<<endl;
    for (int j = 0; j < nbElem; j++) {
      out << "Element "<< j+1 << " : ";
      for (int k = body_connectivity_index[j]; k < body_connectivity_index[j+1]; k++){
        CPPUNIT_ASSERT_NO_THROW(myGrid1->getNodePosition(BodyConnectivity[k-1], ijkNodeBody[0],
                                                         ijkNodeBody[1], ijkNodeBody[2]));
        out << BodyConnectivity[k-1] << " ";
      }
      out << endl;
    }
    mesh->removeReference();
  }

  ///////////////////////////////////////////////////
  // test3 "maa1" which in fact is not a pure GRID //
  ///////////////////////////////////////////////////
  {
    GMESH* myMesh2 = NULL;

    CPPUNIT_ASSERT_THROW( myMesh2 = new GRID( MED_DRIVER,myMed->getFileName(),mesh_names[2]),
                          MEDEXCEPTION );
    CPPUNIT_ASSERT_NO_THROW( myMesh2 = new MESH( MED_DRIVER,myMed->getFileName(),mesh_names[2]));

    CPPUNIT_ASSERT(myMesh2 != NULL);
    CPPUNIT_ASSERT(!(myMesh2->getIsAGrid()));
    myMesh2->removeReference();
  }

  delete myMed;

  ////////////////////////////
  // test4 create new GRID  //
  ////////////////////////////

  // Default constructor and destructor
  {
    GRID* myGrid2 = new GRID();
    CPPUNIT_ASSERT(myGrid2->getIsAGrid());
    CPPUNIT_ASSERT(myGrid2->getGridType() == MED_CARTESIAN);
    CPPUNIT_ASSERT(!myGrid2->getArrayLength(1));
    CPPUNIT_ASSERT(!myGrid2->getArrayLength(2));
    CPPUNIT_ASSERT(!myGrid2->getArrayLength(3));
    myGrid2->removeReference();
  }

  // Constructor with grid type, setGridType()
  {
    GRID* myGrid2 = new GRID(MED_POLAR);
    CPPUNIT_ASSERT(myGrid2->getGridType() == MED_POLAR);
    myGrid2->setGridType(MED_CARTESIAN);
    CPPUNIT_ASSERT(myGrid2->getGridType() == MED_CARTESIAN);
    myGrid2->removeReference();
  }

  // Constructor with coordinate values, getArrayValue(), init()
  {
    vector<vector<double> > xyz;
    const int nbCoords = 3;
    xyz.resize(nbCoords);
    for ( int i = 0; i < nbCoords; i++ )
    {
      xyz[i].resize(i + 2);
      for ( int j = 0; j < i + 2; j++ )
        xyz[i][j] = j;
    }
    vector<string> Coord_Names;
    Coord_Names.resize(nbCoords);
    Coord_Names[0] = "X";
    Coord_Names[1] = "Y";
    Coord_Names[2] = "Z";

    vector<string> Coord_Units;
    Coord_Units.resize(nbCoords);
    for(int i = 0; i < 3; i++)
      Coord_Units[i] = "cm";

    GRID* myGrid2;

    try{
      myGrid2 = new GRID(xyz, Coord_Names, Coord_Units, MED_CARTESIAN);
    }
    catch (const std::exception &e)
    {
      CPPUNIT_FAIL(e.what());
    }
    catch (...)
    {
      CPPUNIT_FAIL("Unknown exception");
    }

    // testing getCoordinateptr() and fillCoordinates()
    // We fill a map of all possible coordinate triples.
    // After iteration through all coordinates, this map should contain only "true" as data.
    // "true" in some map element during iteration means duplicated node position.
    // "false" in some map element after iteration means empty node position.
    map<int, bool> found;
    for ( unsigned i1 = 0; i1 < xyz[0].size(); i1++ )
      for ( unsigned i2 = 0; i2 < xyz[1].size(); i2++ )
        for ( unsigned i3 = 0; i3 < xyz[2].size(); i3++ )
          found[ xyz[0][i1] * 100 + xyz[1][i2] * 10 + xyz[2][i3] ] = false;

    const MESH* mesh = myGrid2->convertInMESH();
    COORDINATE* coords = (COORDINATE*)mesh->getCoordinateptr();
    CPPUNIT_ASSERT(coords);
    for (int num = 0; num < myGrid2->getNumberOfNodes(); num++) {
      int x = int(coords->getCoordinate(num + 1, 1));
      int y = int(coords->getCoordinate(num + 1, 2));
      int z = int(coords->getCoordinate(num + 1, 3));
      CPPUNIT_ASSERT(!found[x * 100 + y * 10 + z]);
      found[x * 100 + y * 10 + z] = true;
    }

    for ( map<int, bool>::iterator it = found.begin(); it != found.end(); it++ )
      CPPUNIT_ASSERT((*it).second);

    // Testing fillConnectivity() and getConnectivityptr()
    // Basic testing: presence of connectivity arrays, element types and number of elements
    CONNECTIVITY* conn = (CONNECTIVITY*)mesh->getConnectivityptr();
    CPPUNIT_ASSERT(conn);
    bool hasFaces = myGrid2->getArrayLength(3), hasEdges = myGrid2->getArrayLength(2);
    medGeometryElement aCellGeometry;
    if (hasFaces)      aCellGeometry = MED_HEXA8;
    else if (hasEdges) aCellGeometry = MED_QUAD4;
    else               aCellGeometry = MED_SEG2;
    CPPUNIT_ASSERT(conn->getElementType(MED_CELL, 1) == aCellGeometry);
    CPPUNIT_ASSERT(conn->existConnectivity(MED_NODAL,      MED_CELL));
    CPPUNIT_ASSERT(conn->existConnectivity(MED_DESCENDING, MED_CELL));
    //test getCellsTypes
    CELLMODEL* cellmodel = (CELLMODEL*)mesh->getCellsTypes(MED_CELL);
    CPPUNIT_ASSERT(cellmodel);

    int nbCells, nbFaces, nbEdges;

    int iLen     = myGrid2->getArrayLength(1);
    int jLen     = myGrid2->getArrayLength(2);
    int kLen     = myGrid2->getArrayLength(3);
    int iLenMin1 = myGrid2->getArrayLength(1)-1;
    int jLenMin1 = myGrid2->getArrayLength(2)-1;
    int kLenMin1 = myGrid2->getArrayLength(3)-1;
    const int* aCellCount = conn->getGlobalNumberingIndex(MED_CELL);
    nbCells = iLenMin1 * jLenMin1 * kLenMin1;
    CPPUNIT_ASSERT(aCellCount[1] - 1 == nbCells);

    if (hasFaces){
      CPPUNIT_ASSERT(conn->getElementType(MED_FACE, 1) == MED_QUAD4);
      nbFaces  = iLen * jLenMin1 * kLenMin1;
      nbFaces += jLen * kLenMin1 * iLenMin1;
      nbFaces += kLen * iLenMin1 * jLenMin1;
      const int* aFaceCount = conn->getGlobalNumberingIndex(MED_FACE);
      CPPUNIT_ASSERT(aFaceCount[1] - 1 == nbFaces);
      CPPUNIT_ASSERT(conn->existConnectivity(MED_NODAL, MED_FACE));
      //test getCellsTypes
      CELLMODEL* cellmodelF = (CELLMODEL*)mesh->getCellsTypes(MED_FACE);
      CPPUNIT_ASSERT(cellmodelF);
    }
    if (hasEdges){
      CPPUNIT_ASSERT(conn->getElementType(MED_EDGE, 1) == MED_SEG2);
      if (kLen) { // 3d grid
        nbEdges  = iLenMin1 * jLen * kLen;
        nbEdges += jLenMin1 * kLen * iLen;
        nbEdges += kLenMin1 * iLen * jLen;
      }
      else if (jLen) { // 2d
        nbEdges  = iLenMin1 * jLen;
        nbEdges += jLenMin1 * iLen;
      }
      const int* anEdgeCount = conn->getGlobalNumberingIndex(MED_EDGE);
      CPPUNIT_ASSERT(anEdgeCount[1] - 1 == nbEdges);
      CPPUNIT_ASSERT(conn->existConnectivity(MED_NODAL, MED_EDGE));
      //test getCellsTypes
      CELLMODEL* cellmodelE = (CELLMODEL*)mesh->getCellsTypes(MED_EDGE);
      CPPUNIT_ASSERT(cellmodelE);

    }

    // Testing getArrayValue()
    for ( int ii = 1; ii <= nbCoords; ii++ )
      for ( int jj = 0; jj < ii + 1; jj++ )
        CPPUNIT_ASSERT(myGrid2->getArrayValue(ii, jj) == xyz[ii - 1][jj]);

    CPPUNIT_ASSERT_THROW(myGrid2->getArrayValue(nbCoords + 1, 0), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(myGrid2->getArrayValue(1, myGrid2->getArrayLength(1) + 1), MEDEXCEPTION);
    myGrid2->setGridType(MED_POLAR);

    //testing read/write functions

    // add new driver
    int idGridV21;

    try
    {
      idGridV21 = const_cast<MESH*>(mesh)->addDriver(MED_DRIVER,filenameout21);
    }
    catch(MEDEXCEPTION &e)
    {
      CPPUNIT_FAIL(e.what());
    }
    catch( ... )
    {
      CPPUNIT_FAIL("Unknown exception");
    }

    // write this driver to file as an unstructured mesh
    CPPUNIT_ASSERT_NO_THROW(mesh->write(idGridV21));

    GRID* myGrid3 = new GRID();
    // add new driver for myGrid3
    int driver;
    CPPUNIT_ASSERT_NO_THROW(driver = myGrid3->addDriver(MED_DRIVER, filenameout21));

//#ifdef ENABLE_FORCED_FAILURES
    // ? (BUG) ? "The mesh dimension |-1| seems to be incorrect for the mesh : |Default Mesh Name|"
    CPPUNIT_ASSERT_NO_THROW(myGrid3->read(driver));

    // Testing getArrayValue()
    for ( int ii = 1; ii <= nbCoords; ii++ )
      for ( int jj = 0; jj < ii + 1; jj++ )
        CPPUNIT_ASSERT(myGrid3->getArrayValue(ii, jj) == xyz[ii - 1][jj]);

    CPPUNIT_ASSERT(myGrid3->getGridType() == MED_POLAR);
//#endif

    myGrid3->removeReference();

    //test init()
    CPPUNIT_ASSERT_NO_THROW(myGrid2->init());
    CPPUNIT_ASSERT(myGrid2->getGridType() == MED_CARTESIAN);
    CPPUNIT_ASSERT(myGrid2->getArrayLength(1) == 0);
    CPPUNIT_ASSERT(myGrid2->getArrayLength(2) == 0);
    CPPUNIT_ASSERT(myGrid2->getArrayLength(3) == 0);
//#ifdef ENABLE_FAULTS
    // (BUG) Segmentation Fault
    //myGrid2->makeUnstructured();
//#endif
//#ifdef ENABLE_FORCED_FAILURES
//    CPPUNIT_FAIL("ERROR:makeUnstructured() - there is no check if grid is empty or not");
//#endif

    myGrid2->removeReference();
    mesh->removeReference();
  }

//#ifdef ENABLE_FORCED_FAILURES
  {
    GRID* myGrid2;
    // ? (BUG) ? MED Exception in /dn20/salome/jfa/V3/SRC/MED_SRC/src/MEDMEM/MEDMEM_MedM
    //eshDriver21.cxx [430] : MED_MESH_RDONLY_DRIVER21::getCOORDINATE() : The number 
    //of nodes |0| seems to be incorrect for the mesh : |bodyfitted|
    CPPUNIT_ASSERT_NO_THROW(myGrid2 = new GRID(MED_DRIVER, filename, mesh_names[1]));

    // Check if something has been read - full mesh data testing is above
    CPPUNIT_ASSERT(myGrid2->getSpaceDimension());
    CPPUNIT_ASSERT(myGrid2->getNumberOfNodes());
    CPPUNIT_ASSERT(myGrid2->getNumberOfTypes(MED_CELL) == 1);
    const medGeometryElement* types2;
    CPPUNIT_ASSERT_NO_THROW(types2 = myGrid2->getTypes(MED_CELL));
    int nbElem;
    CPPUNIT_ASSERT_NO_THROW(nbElem = myGrid2->getNumberOfElements(MED_CELL,types2[0]));
    CPPUNIT_ASSERT(nbElem);
    myGrid2->removeReference();
  }
//#endif

  {
    GRID* myGrid4 = new GRID();
    filename = getResourceFile("pointe.med");
    myGrid4->setName("maa1");
    MED_MESH_RDONLY_DRIVER myMeshDriver(filename, myGrid4);
    myMeshDriver.setMeshName("maa1");

    // add new driver for myGrid4
    int driver;
    CPPUNIT_ASSERT_NO_THROW(driver = myGrid4->addDriver(myMeshDriver));

    // ??? ERROR:myGrid4->fillMeshAfterRead()- this method is incomplete:
    // currently it only resets _is_coordinates_filled and _is_connectivity_filled
    // flags that leads to grid reconstruction

    // MED Exception : MED_MESH_RDONLY_DRIVER21::getGRID() : The number
    // of nodes |-1| seems to be incorrect for the mesh : |maa1|
    // But this exception seems to be correct reaction on attempt
    // to read a grid from a file, which does not contain it.
    CPPUNIT_ASSERT_THROW(myGrid4->read(driver), MEDEXCEPTION);
    //CPPUNIT_ASSERT_NO_THROW(myGrid4->read(driver));
    /*CPPUNIT_ASSERT(myGrid4->getArrayLength(1) == 0);
    CPPUNIT_ASSERT(myGrid4->getArrayLength(2) == 0);
    CPPUNIT_ASSERT(myGrid4->getArrayLength(3) == 0);
    myGrid4->fillMeshAfterRead();
    CPPUNIT_ASSERT(myGrid4->getArrayLength(1) != 0);
    CPPUNIT_ASSERT(myGrid4->getArrayLength(2) != 0);
    CPPUNIT_ASSERT(myGrid4->getArrayLength(3) != 0);*/

    myGrid4->removeReference();
  }
}

int main (int argc, char** argv)
{
  MEDMEMTest_testGrid();
}
