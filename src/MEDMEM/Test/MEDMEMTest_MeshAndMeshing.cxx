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
#include <cppunit/Message.h>
#include <cppunit/TestAssert.h>
#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_define.hxx"

#include <sstream>
#include <cmath>

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

/*!
 *  Check methods (87), defined in MEDMEM_Mesh.hxx:
 *  class MESH : public RCBASE {
 *   (yetno) void init();
 *   (yetno) MESH();
 *   (yetno) MESH(MESH &m);
 *   (yetno) MESH & operator=(const MESH &m);
 *   (yetno) virtual bool operator==(const MESH& other) const;
 *   (yetno) virtual bool deepCompare(const MESH& other) const;
 *   (yetno) MESH(driverTypes driverType, const string & fileName="",
 *                const string & meshName="") throw (MEDEXCEPTION);
 *   (yetno) virtual ~MESH();
 *   (yetno) friend ostream & operator<<(ostream &os, const MESH &my);
 *   (yetno) int  addDriver(driverTypes driverType,
 *                          const string & fileName="Default File Name.med",
 *                          const string & driverName="Default Mesh Name",
 *                          MED_EN::med_mode_acces access=MED_EN::MED_REMP);
 *   (yetno) int  addDriver(GENDRIVER & driver);
 *   (yetno) void rmDriver(int index=0);
 *   (yetno) virtual void read(int index=0);
 *   (yetno) inline void read(const GENDRIVER & genDriver);
 *   (yetno) inline void write(int index=0, const string & driverName = "");
 *   (yetno) inline void write(const GENDRIVER & genDriver);
 *   (yetno) inline void setName(string name);
 *   (yetno) inline void setDescription(string description);
 *   (yetno) inline string getName() const;
 *   (yetno) inline string getDescription() const;
 *   (yetno) inline int getSpaceDimension() const;
 *   (yetno) inline int getMeshDimension() const;
 *   (yetno) inline bool getIsAGrid();
 *   (yetno) inline int getNumberOfNodes() const;
 *   (yetno) virtual inline const COORDINATE * getCoordinateptr() const;
 *   (yetno) inline string                     getCoordinatesSystem() const;
 *   (yetno) virtual inline const double *     getCoordinates(MED_EN::medModeSwitch Mode) const;
 *   (yetno) virtual inline const double       getCoordinate(int Number,int Axis) const;
 *   (yetno) inline const string *             getCoordinatesNames() const;
 *   (yetno) inline const string *             getCoordinatesUnits() const;
 *   (yetno) virtual inline int getNumberOfTypes(MED_EN::medEntityMesh Entity) const;
 *   (yetno) virtual int getNumberOfTypesWithPoly(MED_EN::medEntityMesh Entity) const;
 *   (yetno) virtual inline const MED_EN::medGeometryElement * getTypes(MED_EN::medEntityMesh Entity) const;
 *   (yetno) virtual MED_EN::medGeometryElement * getTypesWithPoly(MED_EN::medEntityMesh Entity) const;
 *   (yetno) virtual inline const CELLMODEL * getCellsTypes(MED_EN::medEntityMesh Entity) const;
 *   (yetno) virtual inline string * getCellTypeNames(MED_EN::medEntityMesh Entity) const;
 *   (yetno) virtual const int * getGlobalNumberingIndex(MED_EN::medEntityMesh Entity) const;
 *   (yetno) virtual inline int getNumberOfElements(MED_EN::medEntityMesh Entity,
 *                                        MED_EN::medGeometryElement Type) const;
 *   (yetno) virtual int getNumberOfElementsWithPoly(MED_EN::medEntityMesh Entity,
 *                                        MED_EN::medGeometryElement Type) const;
 *   (yetno) virtual inline bool existConnectivity(MED_EN::medConnectivity ConnectivityType,
 *                                       MED_EN::medEntityMesh Entity) const;
 *   (yetno) inline bool existPolygonsConnectivity(MED_EN::medConnectivity ConnectivityType,
 *                                       MED_EN::medEntityMesh Entity) const;
 *   (yetno) inline bool existPolyhedronConnectivity(MED_EN::medConnectivity ConnectivityType,
 *                                         MED_EN::medEntityMesh Entity) const;
 *   (yetno) virtual inline MED_EN::medGeometryElement getElementType
 *           (MED_EN::medEntityMesh Entity, int Number) const;
 *   (yetno) virtual inline MED_EN::medGeometryElement getElementTypeWithPoly
 *           (MED_EN::medEntityMesh Entity, int Number) const;
 *   (yetno) virtual inline void calculateConnectivity(MED_EN::medModeSwitch Mode,
 *                                            MED_EN::medConnectivity ConnectivityType,
 *                                            MED_EN::medEntityMesh Entity) const;
 *   (yetno) virtual inline int getConnectivityLength(MED_EN::medModeSwitch Mode,
 *                                             MED_EN::medConnectivity ConnectivityType,
 *                                             MED_EN::medEntityMesh Entity,
 *                                             MED_EN::medGeometryElement Type) const;
 *   (yetno) virtual inline const int * getConnectivity(MED_EN::medModeSwitch Mode,
 *                                             MED_EN::medConnectivity ConnectivityType,
 *                                             MED_EN::medEntityMesh Entity,
 *                                             MED_EN::medGeometryElement Type) const;
 *   (yetno) virtual inline const int * getConnectivityIndex
 *           (MED_EN::medConnectivity ConnectivityType, MED_EN::medEntityMesh Entity) const;
 *   (yetno) inline int getPolygonsConnectivityLength
 *           (MED_EN::medConnectivity ConnectivityType, MED_EN::medEntityMesh Entity) const;
 *   (yetno) inline const int * getPolygonsConnectivity
 *           (MED_EN::medConnectivity ConnectivityType, MED_EN::medEntityMesh Entity) const;
 *   (yetno) inline const int * getPolygonsConnectivityIndex
 *           (MED_EN::medConnectivity ConnectivityType, MED_EN::medEntityMesh Entity) const;
 *   (yetno) inline int getNumberOfPolygons(MED_EN::medEntityMesh Entity=MED_EN::MED_ALL_ENTITIES) const;
 *   (yetno) inline int getPolyhedronConnectivityLength(MED_EN::medConnectivity ConnectivityType) const;
 *   (yetno) inline const int * getPolyhedronConnectivity(MED_EN::medConnectivity ConnectivityType) const;
 *   (yetno) inline const int * getPolyhedronFacesIndex() const;
 *   (yetno) inline const int * getPolyhedronIndex(MED_EN::medConnectivity ConnectivityType) const;
 *   (yetno) inline int getNumberOfPolyhedronFaces() const;
 *   (yetno) inline int getNumberOfPolyhedron() const;
 *   (yetno) virtual int getElementNumber(MED_EN::medConnectivity ConnectivityType,
 *                                        MED_EN::medEntityMesh Entity, MED_EN::medGeometryElement Type,
 *                                        int * connectivity) const;
 *   (yetno) virtual inline int getReverseConnectivityLength
 *           (MED_EN::medConnectivity ConnectivityType, MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const;
 *   (yetno) virtual inline const int * getReverseConnectivity
 *           (MED_EN::medConnectivity ConnectivityType, MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const;
 *   (yetno) virtual inline int getReverseConnectivityIndexLength
 *           (MED_EN::medConnectivity ConnectivityType, MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const;
 *   (yetno) virtual inline const int * getReverseConnectivityIndex
 *           (MED_EN::medConnectivity ConnectivityType, MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const;
 *   (yetno) virtual int getNumberOfFamilies(MED_EN::medEntityMesh Entity) const;
 *   (yetno) virtual inline const vector<FAMILY*> getFamilies(MED_EN::medEntityMesh Entity) const;
 *   (yetno) virtual inline const FAMILY* getFamily(MED_EN::medEntityMesh Entity,int i) const;
 *   (yetno) virtual int getNumberOfGroups(MED_EN::medEntityMesh Entity) const;
 *   (yetno) virtual inline const vector<GROUP*> getGroups(MED_EN::medEntityMesh Entity) const;
 *   (yetno) virtual inline const GROUP* getGroup(MED_EN::medEntityMesh Entity,int i) const;
 *   (yetno) virtual inline const CONNECTIVITY* getConnectivityptr() const;
 *   (yetno) virtual SUPPORT * getBoundaryElements(MED_EN::medEntityMesh Entity) throw (MEDEXCEPTION);
 *   (yetno) SUPPORT * getSkin(const SUPPORT * Support3D) throw (MEDEXCEPTION);
 *   (yetno) virtual FIELD<double>* getVolume (const SUPPORT * Support) const throw (MEDEXCEPTION);
 *   (yetno) virtual FIELD<double>* getArea (const SUPPORT * Support) const throw (MEDEXCEPTION);
 *   (yetno) virtual FIELD<double>* getLength (const SUPPORT * Support) const throw (MEDEXCEPTION);
 *   (yetno) virtual FIELD<double>* getNormal (const SUPPORT * Support) const throw (MEDEXCEPTION);
 *   (yetno) virtual FIELD<double>* getBarycenter (const SUPPORT * Support) const throw (MEDEXCEPTION);
 *   (yetno) static SUPPORT * mergeSupports(const vector<SUPPORT *> Supports) throw (MEDEXCEPTION);
 *   (yetno) static SUPPORT * intersectSupports(const vector<SUPPORT *> Supports) throw (MEDEXCEPTION);
 *   (yetno) void createFamilies();
 *   (yetno) SUPPORT *buildSupportOnNodeFromElementList
 *            (const list<int>& listOfElt, MED_EN::medEntityMesh entity) const throw (MEDEXCEPTION);
 *   (yetno) void fillSupportOnNodeFromElementList
 *            (const list<int>& listOfElt, SUPPORT *supportToFill) const throw (MEDEXCEPTION);
 *   (yetno) SUPPORT *buildSupportOnElementsFromElementList
 *            (const list<int>& listOfElt, MED_EN::medEntityMesh entity) const throw (MEDEXCEPTION);
 *   (yetno) int getElementContainingPoint(const double *coord);
 *   (yetno) vector< vector<double> > getBoundingBox() const;
 *   (yetno) template<class T> static
 *   (yetno) FIELD<T> * mergeFields(const vector< FIELD<T> * > & others, bool meshCompare=false);
 *   (yetno) virtual void addReference() const;
 *   (yetno) virtual void removeReference() const;
 *  }
 */
void MEDMEMTest::testMesh()
{
  MESH * myMesh= new MESH() ;
  myMesh->setName("FIRST_MESH");
  CPPUNIT_ASSERT(myMesh != NULL);
  try
  {
    //cout << * myMesh << endl;
  }
  catch (const std::exception &e)
  {
    //CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  MESH myMesh1 = * myMesh;
  //CPPUNIT_ASSERT(myMesh1 == myMesh);//????
  delete myMesh;

  //set a MESH object
  MESHING myMeshing ;
  myMeshing.setName("meshing") ;
  // define coordinates

  int SpaceDimension = 3 ;
  int NumberOfNodes = 19 ;
  double Coordinates[57] = {
    0.0, 0.0, 0.0, 
    0.0, 0.0, 1.0, 
    2.0, 0.0, 1.0, 
    0.0, 2.0, 1.0, 
    -2.0, 0.0, 1.0, 
    0.0, -2.0, 1.0, 
    1.0, 1.0, 2.0, 
    -1.0, 1.0, 2.0, 
    -1.0, -1.0, 2.0, 
    1.0, -1.0, 2.0, 
    1.0, 1.0, 3.0, 
    -1.0, 1.0, 3.0, 
    -1.0, -1.0, 3.0, 
    1.0, -1.0, 3.0, 
    1.0, 1.0, 4.0, 
    -1.0, 1.0, 4.0, 
    -1.0, -1.0, 4.0, 
    1.0, -1.0, 4.0,
    0.0, 0.0, 5.0
  };
  try
  {
    myMeshing.setCoordinates(SpaceDimension,NumberOfNodes,Coordinates,"CARTESIAN",MED_FULL_INTERLACE);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  string Names[3] = { "X","Y","Z" } ;
  try
  {
    myMeshing.setCoordinatesNames(Names);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  string Units[3] = { "cm","cm","cm" } ;
  try
  {
    myMeshing.setCoordinatesUnits(Units) ;
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  // define conectivities

  // cell part
  
  const int NumberOfTypes = 3 ;
  medGeometryElement Types[NumberOfTypes] = {MED_TETRA4,MED_PYRA5,MED_HEXA8} ;
  const int NumberOfElements[NumberOfTypes] = {12,2,2} ;

  CPPUNIT_ASSERT_NO_THROW(myMeshing.setNumberOfTypes(NumberOfTypes,MED_CELL));

  CPPUNIT_ASSERT_NO_THROW(myMeshing.setTypes(Types,MED_CELL));

  CPPUNIT_ASSERT_NO_THROW(myMeshing.setNumberOfElements(NumberOfElements,MED_CELL));

  const int sizeTetra = 12*4 ;
  int ConnectivityTetra[sizeTetra]=
  {
    1,2,3,6,
    1,2,4,3,
    1,2,5,4,
    1,2,6,5,
    2,7,4,3,
    2,8,5,4,
    2,9,6,5,
    2,10,3,6,
    2,7,3,10,
    2,8,4,7,
    2,9,5,8,
    2,10,6,9
  };
  
  CPPUNIT_ASSERT_NO_THROW(myMeshing.setConnectivity(ConnectivityTetra,MED_CELL,MED_TETRA4));

  int ConnectivityPyra[2*5]=
  {
    7,8,9,10,2,
    15,18,17,16,19
  };

  CPPUNIT_ASSERT_NO_THROW(myMeshing.setConnectivity(ConnectivityPyra,MED_CELL,MED_PYRA5));

  int ConnectivityHexa[2*8]=
  {
    11,12,13,14,7,8,9,10,
    15,16,17,18,11,12,13,14
  };

  CPPUNIT_ASSERT_NO_THROW(myMeshing.setConnectivity(ConnectivityHexa,MED_CELL,MED_HEXA8));

  // face part
  const int NumberOfFacesTypes = 2 ;
  medGeometryElement FacesTypes[NumberOfFacesTypes] = {MED_TRIA3,MED_QUAD4} ;
  const int NumberOfFacesElements[NumberOfFacesTypes] = {4,4} ;

  CPPUNIT_ASSERT_NO_THROW(myMeshing.setNumberOfTypes(NumberOfFacesTypes,MED_FACE));
  CPPUNIT_ASSERT_NO_THROW(myMeshing.setTypes(FacesTypes,MED_FACE));
  CPPUNIT_ASSERT_NO_THROW(myMeshing.setNumberOfElements(NumberOfFacesElements,MED_FACE));
  const int nbTria = 4;
  const int sizeTria = nbTria*3;
  int ConnectivityTria[sizeTria]=
  {
    1,4,3,
    1,5,4,
    1,6,5,
    1,3,6
  };
  
  CPPUNIT_ASSERT_NO_THROW(myMeshing.setConnectivity(ConnectivityTria,MED_FACE,MED_TRIA3));
  const int nbQua = 4;
  int ConnectivityQua[nbQua*4]=
  {
    7,8,9,10,
    11,12,13,14,
    11,7,8,12,
    12,8,9,13
  };

  CPPUNIT_ASSERT_NO_THROW(myMeshing.setConnectivity(ConnectivityQua,MED_FACE,MED_QUAD4));

  int meshDimension = SpaceDimension; // because there 3D cells in the mesh
  try
  {
    myMeshing.setMeshDimension(meshDimension);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  // edge part

  // not yet implemented : if set, results are unpredictable.

  // Some groups :

  // Node :
  {
    GROUP myGroup ;
    myGroup.setName("SomeNodes");
    myGroup.setMesh(&myMeshing);
    myGroup.setEntity(MED_NODE);
    myGroup.setNumberOfGeometricType(1);
    medGeometryElement myTypes[1] = {MED_NONE};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[1] = {4} ;
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[1+1] = {1,5} ;
    const int value[4]= { 1,4,5,7} ;
    myGroup.setNumber(index,value);
    CPPUNIT_ASSERT_NO_THROW(myMeshing.addGroup(myGroup));
  }
  {
    GROUP myGroup ;
    myGroup.setName("OtherNodes");
    myGroup.setMesh(&myMeshing);
    myGroup.setEntity(MED_NODE);
    myGroup.setNumberOfGeometricType(1);
    medGeometryElement myTypes[1] = {MED_NONE};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[1] = {3} ;
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[1+1] = {1,4} ;
    const int value[3]= { 2,3,6} ;
    myGroup.setNumber(index,value);
    CPPUNIT_ASSERT_NO_THROW(myMeshing.addGroup(myGroup));
  }

  // Cell :
  {
    GROUP myGroup ;
    myGroup.setName("SomeCells");
    myGroup.setMesh(&myMeshing);
    myGroup.setEntity(MED_CELL);
    myGroup.setNumberOfGeometricType(3);
    medGeometryElement myTypes[3] = {MED_TETRA4,MED_PYRA5,MED_HEXA8};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[3] = {4,1,2} ;
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[3+1] = {1,5,6,8} ;
    const int value[4+1+2]=
    {
      2,7,8,12,
      13,
      15,16
    };
    myGroup.setNumber(index,value);
    CPPUNIT_ASSERT_NO_THROW(myMeshing.addGroup(myGroup));
  }
  {
    GROUP myGroup ;
    myGroup.setName("OtherCells");
    myGroup.setMesh(&myMeshing);
    myGroup.setEntity(MED_CELL);
    myGroup.setNumberOfGeometricType(2);
    medGeometryElement myTypes[] = {MED_TETRA4,MED_PYRA5};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[] = {4,1} ;
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[2+1] = {1,5,6} ;
    const int value[4+1]=
    {
      3,4,5,9,
      14
    };
    myGroup.setNumber(index,value);
    CPPUNIT_ASSERT_NO_THROW(myMeshing.addGroup(myGroup));
  }

  // Face :
  {
    GROUP myGroup ;
    myGroup.setName("SomeFaces");
    myGroup.setMesh(&myMeshing);
    myGroup.setEntity(MED_FACE);
    myGroup.setNumberOfGeometricType(2);
    medGeometryElement myTypes[2] = {MED_TRIA3,MED_QUAD4};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[2] = {2,3} ;
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[2+1] = {1,3,6} ;
    const int value[2+3]=
    {
      2,4,
      5,6,8
    } ;
    myGroup.setNumber(index,value);
    CPPUNIT_ASSERT_NO_THROW(myMeshing.addGroup(myGroup));
  }
  {
    GROUP myGroup ;
    myGroup.setName("OtherFaces");
    myGroup.setMesh(&myMeshing);
    myGroup.setEntity(MED_FACE);
    myGroup.setNumberOfGeometricType(1);
    medGeometryElement myTypes[1] = {MED_TRIA3};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[1] = {2} ;
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[1+1] = {1,3} ;
    const int value[2]=
    {
      1,3
    } ;
    myGroup.setNumber(index,value);
    CPPUNIT_ASSERT_NO_THROW(myMeshing.addGroup(myGroup));
  }

  MESH * myMesh2 = new MESH( myMeshing );
  CPPUNIT_ASSERT(myMesh2->deepCompare(myMeshing));

  cout<<*myMesh2<<endl;
  ostringstream os;
  os << * myMesh2;
  CPPUNIT_ASSERT(os.str() != "");

  CPPUNIT_ASSERT_EQUAL(myMesh2->getName(),(string)"meshing");
  CPPUNIT_ASSERT((myMesh2->getDescription()).size() == 0);
  myMesh2->setDescription("This class contains all information related to a 'meshing' mesh ");
  CPPUNIT_ASSERT((myMesh2->getDescription()).size() != 0);

  CPPUNIT_ASSERT(myMesh2->getSpaceDimension() == SpaceDimension);
  CPPUNIT_ASSERT(myMesh2->getMeshDimension() == meshDimension);
  CPPUNIT_ASSERT(myMesh2->getNumberOfNodes() == NumberOfNodes);

  const COORDINATE* coord = myMesh2->getCoordinateptr();
  try
  {
    CPPUNIT_ASSERT(myMesh2->getCoordinatesSystem() != "catresian");
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  };

  const string * units;
  try
  {
    units = myMesh2->getCoordinatesUnits();
    for (int axe = 0; axe < SpaceDimension; axe++) {
      string verif = coord->getCoordinateUnit(axe+1);
      CPPUNIT_ASSERT(verif == units[axe]);
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

  const string * noms;
  try
  {
    noms = myMesh2->getCoordinatesNames();
    for (int axe = 0; axe < SpaceDimension; axe++) {
      string verif = coord->getCoordinateName(axe+1);
      CPPUNIT_ASSERT(verif == noms[axe]);
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

  try
  {
    const double * coor2 = myMesh2->getCoordinates(MED_FULL_INTERLACE);

    for (int axe = 0; axe < SpaceDimension; axe++) {
      try
      {
        for (int num = 0; num < NumberOfNodes; num++) {
          try
          {
            const double d = myMesh2->getCoordinate(num + 1, axe + 1);
            CPPUNIT_ASSERT(fabs(d - coor2[(num * SpaceDimension)+axe]) < 0.001);
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
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  const CONNECTIVITY * myConnectivity = myMesh2->getConnectivityptr();

  // MED_EN::MED_CELL
  MED_EN::medEntityMesh entity = myConnectivity->getEntity();
  CPPUNIT_ASSERT_EQUAL(MED_CELL, entity);

  int typesNb;
  CPPUNIT_ASSERT_NO_THROW(typesNb= myConnectivity->getNumberOfTypes(entity));
  CPPUNIT_ASSERT_EQUAL(NumberOfTypes, typesNb);

  const MED_EN::medGeometryElement * Types1;
  CPPUNIT_ASSERT_NO_THROW(Types1 = myMesh2->getTypes(entity));

  medConnectivity myMedConnect;
  bool existConnect = false;
  if(myMesh2->existConnectivity(MED_NODAL, entity))
  {
    existConnect = true;
    myMedConnect = MED_NODAL;
  }
  else if(myMesh2->existConnectivity(MED_DESCENDING, entity))
  {
    existConnect = true;
    myMedConnect = MED_DESCENDING;
  }

  for(int t = 0; t < NumberOfTypes; t++ )
  {
    CPPUNIT_ASSERT_EQUAL(Types1[t], Types[t]);
    int NumberOfElements1 = 0;
    CPPUNIT_ASSERT_NO_THROW(NumberOfElements1 = myMesh2->getNumberOfElements(entity, Types1[t]));
    CPPUNIT_ASSERT_EQUAL(NumberOfElements1, NumberOfElements[t]);
    if(existConnect)
    {
      const int * connectivity;
      const int * connectivity_index;
      CPPUNIT_ASSERT_NO_THROW(connectivity = myMesh2->getConnectivity(MED_FULL_INTERLACE, myMedConnect, entity, Types1[t]));
      connectivity_index = myMesh2->getConnectivityIndex(myMedConnect, entity);
      for (int j = 0; j < NumberOfElements1; j++) {
        cout<<"!!!!!!!!!!!!!!!"<<endl;
        for (int k = connectivity_index[j]; k < connectivity_index[j+1]; k++)//??????
          cout << connectivity[k-1] << " ";
        cout << endl;
      }
    }
  }

  const CELLMODEL* myCellModel = myMesh2->getCellsTypes(entity);
  string* TypeNames;
  CPPUNIT_ASSERT_NO_THROW(TypeNames = myMesh2->getCellTypeNames(entity));

  for(int k = 0; k < NumberOfTypes; k++ )
  {
    CPPUNIT_ASSERT_EQUAL(TypeNames[k], myCellModel[k].getName());
  }

  const int* myGlobalNbIdx;
  CPPUNIT_ASSERT_NO_THROW(myGlobalNbIdx = myMesh2->getGlobalNumberingIndex(MED_FACE));
  for(int i = 0; i <= NumberOfFacesTypes; i++)
  {
    if(i == NumberOfFacesTypes)
    {
      CPPUNIT_ASSERT_EQUAL(myGlobalNbIdx[i],nbTria+nbQua+1);
      CPPUNIT_ASSERT_THROW(myMesh2->getElementType(MED_FACE, myGlobalNbIdx[i]), MEDEXCEPTION);
      break;
    }
    cout<<"Global number of first element of each geom type : "<<myGlobalNbIdx[i]<<endl;
  }

  if(myMesh2->existConnectivity(MED_NODAL, MED_FACE)){
    int myTr[] = {1,5,4};
    //CPPUNIT_ASSERT_NO_THROW(myMesh2->getElementNumber(MED_NODAL,MED_FACE,MED_TRIA3,myTr));
  }

  // Show Reverse Nodal Connectivity
  /*const int * ReverseNodalConnectivity;
  CPPUNIT_ASSERT_NO_THROW(ReverseNodalConnectivity= myMesh2->getReverseConnectivity(MED_NODAL, entity));
  const int ReverseLength = myMesh2->getReverseConnectivityLength(MED_NODAL, entity);
  const int * ReverseNodalConnectivityIndex = myMesh2->getReverseConnectivityIndex(MED_NODAL, entity);
  const int ReverseIdxLength = myMesh2->getReverseConnectivityIndexLength(MED_NODAL, entity);
  CPPUNIT_ASSERT(ReverseIdxLength == NumberOfNodes+1);
  for (int i = 0; i < NumberOfNodes; i++) {
    cout << "Node "<< i+1 << " : ";
    for (int j = ReverseNodalConnectivityIndex[i]; j < ReverseNodalConnectivityIndex[i+1]; j++)
      cout << ReverseNodalConnectivity[j-1] << " ";
    cout << endl;
    }*/

  // Show Descending Connectivity
  /*  int NumberOfElements1;
  const int * connectivity;
  const int * connectivity_index;
  myMesh2->calculateConnectivity(MED_FULL_INTERLACE, MED_DESCENDING, entity);
  try {
    NumberOfElements1 = myMesh2->getNumberOfElements(entity, MED_ALL_ELEMENTS);
    connectivity = myMesh2->getConnectivity(MED_FULL_INTERLACE, MED_DESCENDING, entity, MED_ALL_ELEMENTS);
    connectivity_index = myMesh2->getConnectivityIndex(MED_DESCENDING, entity);
  }
  catch (MEDEXCEPTION m) {
    CPPUNIT_FAIL(m.what());
  }

  for (int j = 0; j < NumberOfElements1; j++) {
    cout << "Element " << j+1 << " : ";
    for (int k = connectivity_index[j]; k < connectivity_index[j+1]; k++)
      cout << connectivity[k-1] << " ";
    cout << endl;
    }*/

  //test family
  /*  vector<FAMILY*> families=myMesh->getFamilies(MED_CELL);
  vector<FAMILY *>::iterator iter=families.begin();
  int NumberOfFamilies;
  CPPUNIT_ASSERT_NO_THROW(NumberOfFamilies = myMesh2->getNumberOfFamilies(MED_CELL));
  for (int i=1; i<NumberOfFamilies+1;i++) {
    const FAMILY* myFamily = myMesh2->getFamily(MED_CELL,i);
    cout << "  - Identifier : "<<myFamily->getIdentifier()<<endl ;
    int NumberOfAttributes = myFamily->getNumberOfAttributes() ;
    cout << "  - Attributes ("<<NumberOfAttributes<<") :"<<endl;
    for (int j=1;j<NumberOfAttributes+1;j++)
      cout << "    * "<<myFamily->getAttributeIdentifier(j)<<" : "<<myFamily->getAttributeValue(j)<<", "<<myFamily->getAttributeDescription(j).c_str()<<endl ;
    int NumberOfGroups = myFamily->getNumberOfGroups() ;
    cout << "  - Groups ("<<NumberOfGroups<<") :"<<endl;
    for (int j=1;j<NumberOfGroups+1;j++)
      cout << "    * "<<myFamily->getGroupName(j).c_str()<<endl ;
      }*/

  delete TypeNames;
  delete myMesh2;
}

/*!
 *  Check methods (18), defined in MEDMEM_Meshing.hxx:
 *  class MESHING: public MESH {
 *   (+) MESHING();
 *   (yetno) ~MESHING();
 *   (+) void setSpaceDimension (const int SpaceDimension);
 *   (+) void setMeshDimension (const int MeshDimension);
 *   (+) void setNumberOfNodes (const int NumberOfNodes);
 *   (+) void setCoordinates (const int SpaceDimension, const int NumberOfNodes,
 *                                const double * Coordinates,
 *                                const string System, const MED_EN::medModeSwitch Mode);
 *   (+) void setCoordinatesSystem(const string System) throw (MEDEXCEPTION);
 *   (+) void setCoordinatesNames (const string * names);
 *   (+) void setCoordinateName (const string name, const int i);
 *   (+) void setCoordinatesUnits (const string * units);
 *   (+) void setCoordinateUnit (const string unit, const int i);
 *   (+) void setNumberOfTypes (const int NumberOfTypes,
 *                                  const MED_EN::medEntityMesh Entity) throw (MEDEXCEPTION);
 *   (+) void setTypes (const MED_EN::medGeometryElement * Types,
 *                          const MED_EN::medEntityMesh Entity) throw (MEDEXCEPTION);
 *   (+) void setNumberOfElements (const int * NumberOfElements,
 *                                 const MED_EN::medEntityMesh Entity) throw (MEDEXCEPTION);
 *   (+) void setConnectivity (const int * Connectivity, const MED_EN::medEntityMesh Entity,
 *                             const MED_EN::medGeometryElement Type) throw (MEDEXCEPTION);
 *   (+) void setPolygonsConnectivity (const int * ConnectivityIndex, const int * ConnectivityValue,
 *                                     int nbOfPolygons,
 *                                     const MED_EN::medEntityMesh Entity) throw (MEDEXCEPTION);
 *   (+) void setPolyhedraConnectivity (const int * PolyhedronIndex, const int * FacesIndex,
 *                                      const int * Nodes, int nbOfPolyhedra,
 *                                      const MED_EN::medEntityMesh Entity) throw (MEDEXCEPTION);
 *   (NOT YET IMPLEMENTED!!!) void setConnectivities (const int * ConnectivityIndex,
 *                                   const int * ConnectivityValue,
 *                                   const MED_EN::medConnectivity ConnectivityType,
 *                                   const MED_EN::medEntityMesh Entity) throw (MEDEXCEPTION);
 *   (+) void addGroup (const GROUP & Group) throw (MEDEXCEPTION);
 *  }
 */
void MEDMEMTest::testMeshing()
{
  ////////////
  // TEST 1 //
  ////////////

  MESHING myMeshing ;
  myMeshing.setName("meshing") ;

  // define coordinates

  int SpaceDimension = 3 ;
  int NumberOfNodes = 19 ;
  double Coordinates[57] = {
    0.0, 0.0, 0.0, 
    0.0, 0.0, 1.0, 
    2.0, 0.0, 1.0, 
    0.0, 2.0, 1.0, 
    -2.0, 0.0, 1.0, 
    0.0, -2.0, 1.0, 
    1.0, 1.0, 2.0, 
    -1.0, 1.0, 2.0, 
    -1.0, -1.0, 2.0, 
    1.0, -1.0, 2.0, 
    1.0, 1.0, 3.0, 
    -1.0, 1.0, 3.0, 
    -1.0, -1.0, 3.0, 
    1.0, -1.0, 3.0, 
    1.0, 1.0, 4.0, 
    -1.0, 1.0, 4.0, 
    -1.0, -1.0, 4.0, 
    1.0, -1.0, 4.0,
    0.0, 0.0, 5.0
  };
  try
  {
    myMeshing.setCoordinates(SpaceDimension,NumberOfNodes,Coordinates,"CARTESIAN",MED_FULL_INTERLACE);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  string Names[3] = { "X","Y","Z" } ;
  try
  {
    myMeshing.setCoordinatesNames(Names);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  string Units[3] = { "cm","cm","cm" } ;
  try
  {
    myMeshing.setCoordinatesUnits(Units) ;
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  // define conectivities

  // cell part
  
  const int NumberOfTypes = 3 ;
  medGeometryElement Types[NumberOfTypes] = {MED_TETRA4,MED_PYRA5,MED_HEXA8} ;
  const int NumberOfElements[NumberOfTypes] = {12,2,2} ;

  CPPUNIT_ASSERT_THROW(myMeshing.setNumberOfTypes(NumberOfTypes,MED_NODE),MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW(myMeshing.setNumberOfTypes(NumberOfTypes,MED_CELL));

  CPPUNIT_ASSERT_THROW(myMeshing.setTypes(Types,MED_NODE), MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW(myMeshing.setTypes(Types,MED_CELL));

  CPPUNIT_ASSERT_THROW(myMeshing.setNumberOfElements(NumberOfElements,MED_NODE), MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW(myMeshing.setNumberOfElements(NumberOfElements,MED_CELL));

  const int sizeTetra = 12*4 ;
  int ConnectivityTetra[sizeTetra]=
  {
    1,2,3,6,
    1,2,4,3,
    1,2,5,4,
    1,2,6,5,
    2,7,4,3,
    2,8,5,4,
    2,9,6,5,
    2,10,3,6,
    2,7,3,10,
    2,8,4,7,
    2,9,5,8,
    2,10,6,9
  };
  
  CPPUNIT_ASSERT_NO_THROW(myMeshing.setConnectivity(ConnectivityTetra,MED_CELL,MED_TETRA4));

  int ConnectivityPyra[2*5]=
  {
    7,8,9,10,2,
    15,18,17,16,19
  };

  CPPUNIT_ASSERT_NO_THROW(myMeshing.setConnectivity(ConnectivityPyra,MED_CELL,MED_PYRA5));

  int ConnectivityHexa[2*8]=
  {
    11,12,13,14,7,8,9,10,
    15,16,17,18,11,12,13,14
  };

  CPPUNIT_ASSERT_NO_THROW(myMeshing.setConnectivity(ConnectivityHexa,MED_CELL,MED_HEXA8));

  // face part

  const int NumberOfFacesTypes = 2 ;
  medGeometryElement FacesTypes[NumberOfFacesTypes] = {MED_TRIA3,MED_QUAD4} ;
  const int NumberOfFacesElements[NumberOfFacesTypes] = {4,4} ;

  CPPUNIT_ASSERT_NO_THROW(myMeshing.setNumberOfTypes(NumberOfFacesTypes,MED_FACE));
  CPPUNIT_ASSERT_NO_THROW(myMeshing.setTypes(FacesTypes,MED_FACE));
  CPPUNIT_ASSERT_NO_THROW(myMeshing.setNumberOfElements(NumberOfFacesElements,MED_FACE));

  const int sizeTria = 3*4 ;
  int ConnectivityTria[sizeTria]=
  {
    1,4,3,
    1,5,4,
    1,6,5,
    1,3,6
  };
  
  CPPUNIT_ASSERT_NO_THROW(myMeshing.setConnectivity(ConnectivityTria,MED_FACE,MED_TRIA3));

  int ConnectivityQua[4*4]=
  {
    7,8,9,10,
    11,12,13,14,
    11,7,8,12,
    12,8,9,13
  };

  CPPUNIT_ASSERT_NO_THROW(myMeshing.setConnectivity(ConnectivityQua,MED_FACE,MED_QUAD4));

  int meshDimension = SpaceDimension; // because there 3D cells in the mesh
  try
  {
    myMeshing.setMeshDimension(meshDimension);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  // edge part

  // not yet implemented : if set, results are unpredictable.

  // Some groups :

  // Node :
  {
    GROUP myGroup ;
    myGroup.setName("SomeNodes");
    myGroup.setMesh(&myMeshing);
    myGroup.setEntity(MED_NODE);
    myGroup.setNumberOfGeometricType(1);
    medGeometryElement myTypes[1] = {MED_NONE};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[1] = {4} ;
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[1+1] = {1,5} ;
    const int value[4]= { 1,4,5,7} ;
    myGroup.setNumber(index,value);
    CPPUNIT_ASSERT_NO_THROW(myMeshing.addGroup(myGroup));
  }
  {
    GROUP myGroup ;
    myGroup.setName("OtherNodes");
    myGroup.setMesh(&myMeshing);
    myGroup.setEntity(MED_NODE);
    myGroup.setNumberOfGeometricType(1);
    medGeometryElement myTypes[1] = {MED_NONE};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[1] = {3} ;
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[1+1] = {1,4} ;
    const int value[3]= { 2,3,6} ;
    myGroup.setNumber(index,value);
    CPPUNIT_ASSERT_NO_THROW(myMeshing.addGroup(myGroup));
  }

  // Cell :
  {
    GROUP myGroup ;
    myGroup.setName("SomeCells");
    myGroup.setMesh(&myMeshing);
    myGroup.setEntity(MED_CELL);
    myGroup.setNumberOfGeometricType(3);
    medGeometryElement myTypes[3] = {MED_TETRA4,MED_PYRA5,MED_HEXA8};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[3] = {4,1,2} ;
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[3+1] = {1,5,6,8} ;
    const int value[4+1+2]=
    {
      2,7,8,12,
      13,
      15,16
    };
    myGroup.setNumber(index,value);
    CPPUNIT_ASSERT_NO_THROW(myMeshing.addGroup(myGroup));
  }
  {
    GROUP myGroup ;
    myGroup.setName("OtherCells");
    myGroup.setMesh(&myMeshing);
    myGroup.setEntity(MED_CELL);
    myGroup.setNumberOfGeometricType(2);
    medGeometryElement myTypes[] = {MED_TETRA4,MED_PYRA5};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[] = {4,1} ;
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[2+1] = {1,5,6} ;
    const int value[4+1]=
    {
      3,4,5,9,
      14
    };
    myGroup.setNumber(index,value);
    CPPUNIT_ASSERT_NO_THROW(myMeshing.addGroup(myGroup));
  }

  // Face :
  {
    GROUP myGroup ;
    myGroup.setName("SomeFaces");
    myGroup.setMesh(&myMeshing);
    myGroup.setEntity(MED_FACE);
    myGroup.setNumberOfGeometricType(2);
    medGeometryElement myTypes[2] = {MED_TRIA3,MED_QUAD4};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[2] = {2,3} ;
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[2+1] = {1,3,6} ;
    const int value[2+3]=
    {
      2,4,
      5,6,8
    } ;
    myGroup.setNumber(index,value);
    CPPUNIT_ASSERT_NO_THROW(myMeshing.addGroup(myGroup));
  }
  {
    GROUP myGroup ;
    myGroup.setName("OtherFaces");
    myGroup.setMesh(&myMeshing);
    myGroup.setEntity(MED_FACE);
    myGroup.setNumberOfGeometricType(1);
    medGeometryElement myTypes[1] = {MED_TRIA3};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[1] = {2} ;
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[1+1] = {1,3} ;
    const int value[2]=
    {
      1,3
    } ;
    myGroup.setNumber(index,value);
    CPPUNIT_ASSERT_NO_THROW(myMeshing.addGroup(myGroup));
  }

  ////////////
  // TEST 2//
  ////////////

  double CoordinatesPoly[57] = {
    2.0, 3.0, 2.0,
    3.0, 2.0, 2.0,
    4.0, 1.0, 2.0,
    2.0, 0.0, 2.0,
    0.0, 1.0, 2.0,
    1.0, 2.0, 2.0,
    2.0, 3.0, 1.0,
    3.0, 2.0, 0.0,
    4.0, 1.0, 0.0,
    2.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    1.0, 2.0, 0.0,
    5.0, 3.0, 2.0,
    7.0, 2.0, 2.0,
    6.0, 0.0, 2.0,
    6.0, 3.0, 0.0,
    7.0, 2.0, 0.0,
    6.0, 0.0, -1.0,
    5.0, 1.0, -3.0};

  const int REFnodalConnOfFaces[74] = {
    1, 2, 3, 4, 5, 6, // Polyhedron 1
    1, 7, 8, 2, 
    2, 8, 9, 3, 
    4, 3, 9, 10, 
    5, 4, 10, 11, 
    6, 5, 11, 12, 
    1, 6, 12, 7, 
    7, 12, 8, 
    10, 9, 8, 12, 11,
                   
    13, 14, 15, 3, 2, // Polyhedron 2
    13, 2, 8, 16, 
    14, 13, 16, 17, 
    15, 14, 17, 
    15, 17, 18, 
    15, 18, 9, 
    3, 15, 9, 
    2, 3, 9, 8, 
    8, 9, 17, 16, 
    9, 18, 17};

  const int REFfacesIndex[20] = {
    1, 7, 11, 15, 19, 23, 27, 31, 34, 
    39, 44, 48, 52, 55, 58, 61, 64, 68, 72, 75};

  const int REFpolyIndex[3] = {1, 10, 20};

  double PolygonCoordinates[27] = {
    2.0, 3.0, 12.0,
    3.0, 2.0, 12.0,
    4.0, 1.0, 12.0,
    2.0, 0.0, 12.0,
    0.0, 1.0, 12.0,
    1.0, 2.0, 12.0,
    5.0, 3.0, 12.0,
    7.0, 2.0, 12.0,
    6.0, 0.0, 12.0};

  const int REFpolygonFaces[11] = {
    1, 2, 3, 4, 5, 6, // Polygon 1   
    7, 8, 9, 3, 2}; // Polygon 2

  const int REFpolygonIndex[3] = {1, 7, 12};

  MESHING myMeshingPoly;
  myMeshingPoly.setName("meshingpoly");

  int MeshDimension = 3;

  const int NbOfTypes = 1;
  medGeometryElement TypesPoly[NbOfTypes] = {MED_TETRA4};
  const int NbOfElements[NbOfTypes] = {1};

  CPPUNIT_ASSERT_NO_THROW(myMeshingPoly.setNumberOfTypes(NbOfTypes, MED_CELL));

  try
  {
    myMeshingPoly.setCoordinates(SpaceDimension, NumberOfNodes, CoordinatesPoly,
                 "CARTESIAN", MED_FULL_INTERLACE);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  try
  {
    myMeshingPoly.setSpaceDimension(SpaceDimension);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  try
  {
    myMeshingPoly.setMeshDimension(MeshDimension);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_NO_THROW(myMeshingPoly.setTypes(TypesPoly, MED_CELL));
  CPPUNIT_ASSERT_NO_THROW(myMeshingPoly.setNumberOfElements(NbOfElements, MED_CELL));

  string Unit ="cm";
  for(int i = 0; i < SpaceDimension; i++ )
  {
    try
    {
      myMeshingPoly.setCoordinateName(Names[i],i);
    }
    catch (const std::exception &e)
    {
      CPPUNIT_FAIL(e.what());
    }
    catch (...)
    {
      CPPUNIT_FAIL("Unknown exception");
    }
    try
    {
      myMeshingPoly.setCoordinateUnit(Unit, i);
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

  int ConnectivityTetraPoly[4*1]=
  {
    17, 9, 18, 19
  };
  
  CPPUNIT_ASSERT_NO_THROW(myMeshingPoly.setConnectivity(ConnectivityTetraPoly, MED_CELL, MED_TETRA4));

  CPPUNIT_ASSERT_NO_THROW(myMeshingPoly.setPolyhedraConnectivity(REFpolyIndex, REFfacesIndex, 
                                 REFnodalConnOfFaces, 2, MED_CELL));


  MESHING myPolygonMeshing;
  myPolygonMeshing.setName("PolygonMeshing");

  medGeometryElement PolygonTypes[NbOfTypes] = {MED_TRIA3};
  const int PolygonNumberOfElements[NbOfTypes] = {2};

  CPPUNIT_ASSERT_NO_THROW(myPolygonMeshing.setNumberOfTypes(NbOfTypes, MED_CELL));

  try
  {
    myPolygonMeshing.setCoordinates(SpaceDimension, NumberOfNodes, PolygonCoordinates,
                    "CARTESIAN", MED_FULL_INTERLACE);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  NumberOfNodes = 9;
  SpaceDimension = 3;
  MeshDimension = 2;

  myPolygonMeshing.setSpaceDimension(SpaceDimension);
  CPPUNIT_ASSERT(myPolygonMeshing.getSpaceDimension() == 3);
  myPolygonMeshing.setMeshDimension(MeshDimension);
  CPPUNIT_ASSERT(myPolygonMeshing.getMeshDimension() == 2);
  myPolygonMeshing.setNumberOfNodes(NumberOfNodes);
  CPPUNIT_ASSERT(myPolygonMeshing.getNumberOfNodes() == 9);
  CPPUNIT_ASSERT_NO_THROW(myPolygonMeshing.setTypes(PolygonTypes, MED_CELL));
  CPPUNIT_ASSERT_NO_THROW(myPolygonMeshing.setNumberOfElements(PolygonNumberOfElements, MED_CELL));
  CPPUNIT_ASSERT_NO_THROW(myPolygonMeshing.setCoordinatesSystem("CARTESIAN"));

  try
  {
    myPolygonMeshing.setCoordinatesNames(Names);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  try
  {
    myPolygonMeshing.setCoordinatesUnits(Units);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  const int sizeTri = 3*2 ;
  int ConnectivityTri[sizeTri]=
  {
    1, 7, 2, 3, 9, 4
  };

  CPPUNIT_ASSERT_NO_THROW(myPolygonMeshing.setConnectivity(ConnectivityTri, MED_CELL, MED_TRIA3));
  CPPUNIT_ASSERT_NO_THROW(myPolygonMeshing.setPolygonsConnectivity(REFpolygonIndex, REFpolygonFaces, 2, MED_CELL));
}


