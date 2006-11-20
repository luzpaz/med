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

////#include "MEDMEM_nArray.hxx"
////#include "MEDMEM_ArrayConvert.hxx"
////#include "MEDMEM_Array.hxx"
//#include "MEDMEM_ArrayInterface.hxx"
//#include "MEDMEM_AsciiFieldDriver.hxx"
//#include "MEDMEM_CellModel.hxx"
//#include "MEDMEM_Compatibility21_22.hxx"
//#include "MEDMEM_Connectivity.hxx"
////#include "MEDMEM_Coordinate.hxx"
//#include "MEDMEM_define.hxx"
//#include "MEDMEM_DriverFactory.hxx"
//#include "MEDMEM_DriversDef.hxx"
//#include "MEDMEM_DriverTools.hxx"
#include "MEDMEM_Exception.hxx"
//#include "MEDMEM_Family.hxx"
//#include "MEDMEM_FieldConvert.hxx"
//#include "MEDMEM_Field.hxx"
//#include "MEDMEM_Formulae.hxx"
//#include "MEDMEM_GaussLocalization.hxx"
//#include "MEDMEM_GenDriver.hxx"
//#include "MEDMEM_GibiMeshDriver.hxx"
//#include "MEDMEM_Grid.hxx"
#include "MEDMEM_Group.hxx"
//#include "MEDMEM_IndexCheckingPolicy.hxx"
//#include "MEDMEM_InterlacingPolicy.hxx"
//#include "MEDMEM_InterlacingTraits.hxx"
//#include "MEDMEM_MedFieldDriver21.hxx"
//#include "MEDMEM_MedFieldDriver22.hxx"
//#include "MEDMEM_MedFieldDriver.hxx"
//#include "MEDMEM_Med.hxx"
//#include "MEDMEM_medimport_src.hxx"
//#include "MEDMEM_MedMedDriver21.hxx"
//#include "MEDMEM_MedMedDriver22.hxx"
//#include "MEDMEM_MedMedDriver.hxx"
//#include "MEDMEM_MEDMEMchampLire.hxx"
//#include "MEDMEM_MEDMEMgaussEcr.hxx"
//#include "MEDMEM_MEDMEMprofilEcr.hxx"
//#include "MEDMEM_MedMeshDriver21.hxx"
//#include "MEDMEM_MedMeshDriver22.hxx"
//#include "MEDMEM_MedMeshDriver.hxx"
//#include "MEDMEM_MedVersion.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Meshing.hxx"
//#include "MEDMEM_ModulusArray.hxx"
//#include "MEDMEM_PointerOf.hxx"
//#include "MEDMEM_PolyhedronArray.hxx"
//#include "MEDMEM_PorflowMeshDriver.hxx"
//#include "MEDMEM_RCBase.hxx"
//#include "MEDMEM_SetInterlacingType.hxx"
//#include "MEDMEM_SkyLineArray.hxx"
#include "MEDMEM_STRING.hxx"
//#include "MEDMEM_Support.hxx"
//#include "MEDMEM_Tags.hxx"
//#include "MEDMEM_TopLevel.hxx"
//#include "MEDMEM_TypeMeshDriver.hxx"
//#include "MEDMEM_Unit.hxx"
//#include "MEDMEM_Utilities.hxx"
//#include "MEDMEM_VtkFieldDriver.hxx"
//#include "MEDMEM_VtkMedDriver.hxx"
//#include "MEDMEM_VtkMeshDriver.hxx"

#include <sstream>
#include <cmath>

using namespace std;
//using namespace MEDMEM;

// ============================================================================
/*!
 *  Set up the environment
 */
// ============================================================================
void MEDMEMTest::setUp()
{
}

// ============================================================================
/*!
 *  - delete
 */
// ============================================================================
void MEDMEMTest::tearDown()
{
}

// #1: MEDMEM_Array.hxx           \
// #2: MEDMEM_ArrayConvert.hxx     }  MEDMEMTest_Array.cxx
// #3: MEDMEM_ArrayInterface.hxx  /
// #4:MEDMEM_AsciiFieldDriver.hxx  }  MEDMEMTest_AsciiFieldDriver.cxx

// #5:

/*!
 *  Check methods (not in spec), defined in MEDMEM_CellModel.hxx:
 *   {
 *   (yetno)
 *  }
 *
 *  Use code of MEDMEM/tests/testUCellModel.cxx
 */
void MEDMEMTest::testCellModel()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #6:

/*!
 *  Check methods (not in spec), defined in MEDMEM_Compatibility21_22.hxx:
 *   {
 *   (yetno) friend ostream & operator<<(ostream &os, CONNECTIVITY &connectivity);
 *   (yetno) CONNECTIVITY (MED_EN::medEntityMesh Entity=MED_EN::MED_CELL);
 *   (yetno) CONNECTIVITY (int numberOfTypes, MED_EN::medEntityMesh Entity=MED_EN::MED_CELL);
 *   (yetno) CONNECTIVITY (const CONNECTIVITY & m);
 *   (yetno) virtual ~CONNECTIVITY ();
 *   (yetno) void setConstituent (CONNECTIVITY * Constituent) throw (MEDEXCEPTION);
 *   (yetno) void setGeometricTypes (const MED_EN::medGeometryElement * Types,
 *                           const MED_EN::medEntityMesh Entity) throw (MEDEXCEPTION);
 *   (yetno) void setCount (const int * Count, const MED_EN::medEntityMesh Entity) throw (MEDEXCEPTION);
 *   (yetno) void setNodal (const int * Connectivity, const MED_EN::medEntityMesh Entity,
 *                 const MED_EN::medGeometryElement Type) throw (MEDEXCEPTION);
 *   (yetno) inline void setNumberOfNodes(int NumberOfNodes);
 *   (yetno) inline int getEntityDimension() const;
 *   (yetno) inline void setEntityDimension(int EntityDimension);
 *   (yetno) void setPolygonsConnectivity(MED_EN::medConnectivity ConnectivityType,
 *                               MED_EN::medEntityMesh Entity,
 *                               const int* PolygonsConnectivity,
 *                               const int* PolygonsConnectivityIndex,
 *                               int ConnectivitySize, int NumberOfPolygons);
 *   (yetno) void setPolyhedronConnectivity(MED_EN::medConnectivity ConnectivityType,
 *                                 const int* PolyhedronConnectivity,
 *                                 const int* PolyhedronIndex, int ConnectivitySize,
 *                                 int NumberOfPolyhedron,
 *                                 const int* PolyhedronFacesIndex= (int*) NULL,
 *                                 int NumberOfFaces=0);
 *   (yetno) inline bool existConnectivity (MED_EN::medConnectivity connectivityType,
 *                                          MED_EN::medEntityMesh Entity) const;
 *   (yetno) virtual bool existPolygonsConnectivity(MED_EN::medConnectivity connectivityType,
 *                                        MED_EN::medEntityMesh Entity) const;
 *   (yetno) virtual bool existPolyhedronConnectivity(MED_EN::medConnectivity connectivityType,
 *                                          MED_EN::medEntityMesh Entity) const;
 *   (yetno) virtual void calculateConnectivity (MED_EN::medConnectivity connectivityType,
 *                                               MED_EN::medEntityMesh Entity);
 *   (yetno) virtual void updateFamily (const vector<FAMILY*>& myFamilies);
 *   (yetno) inline MED_EN::medEntityMesh getEntity () const;
 *   (yetno) inline int getNumberOfTypes (MED_EN::medEntityMesh Entity) const;
 *   (yetno) int getNumberOfTypesWithPoly(MED_EN::medEntityMesh Entity) const;
 *   (yetno) const int * getConnectivityOfAnElementWithPoly(MED_EN::medConnectivity ConnectivityType,
 *                                                 MED_EN::medEntityMesh Entity, int Number, int &lgth);
 *   (yetno) int getNumberOfPolyType()  const;
 *   (yetno) int getNumberOfElementsWithPoly(MED_EN::medEntityMesh Entity,
 *                                           MED_EN::medGeometryElement Type) const;
 *   (yetno) int getNumberOfElementOfPolyType(MED_EN::medEntityMesh Entity)  const;
 *   (yetno) inline const MED_EN::medGeometryElement * getGeometricTypes
 *                    (MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION);
 *   (yetno) MED_EN::medGeometryElement * getGeometricTypesWithPoly
 *                    (MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION);
 *   (yetno) MED_EN::medGeometryElement getElementType (MED_EN::medEntityMesh Entity, int Number) const;
 *   (yetno) MED_EN::medGeometryElement getElementTypeWithPoly
 *                    (MED_EN::medEntityMesh Entity, int Number) const;
 *   (yetno) inline MED_EN::medGeometryElement getPolyTypeRelativeTo() const;
 *   (yetno) virtual inline const int * getGlobalNumberingIndex
 *                 (MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION);
 *   (yetno) virtual const int * getConnectivity (MED_EN::medConnectivity ConnectivityType,
 *                       MED_EN::medEntityMesh Entity, MED_EN::medGeometryElement Type);
 *   (yetno) virtual int getConnectivityLength (MED_EN::medConnectivity ConnectivityType,
 *                        MED_EN::medEntityMesh Entity, MED_EN::medGeometryElement Type);
 *   (yetno) virtual const int * getConnectivityIndex (MED_EN::medConnectivity ConnectivityType,
 *                       MED_EN::medEntityMesh Entity);
 *   (yetno) virtual const int* getPolygonsConnectivity(MED_EN::medConnectivity ConnectivityType,
 *                       MED_EN::medEntityMesh Entity);
 *   (yetno) virtual const int* getPolygonsConnectivityIndex(MED_EN::medConnectivity ConnectivityType,
 *                       MED_EN::medEntityMesh Entity);
 *   (yetno) virtual int getNumberOfPolygons(MED_EN::medEntityMesh Entity=MED_EN::MED_ALL_ENTITIES) const;
 *   (yetno) virtual const int* getPolyhedronConnectivity(MED_EN::medConnectivity ConnectivityType) const;
 *   (yetno) virtual const int* getPolyhedronFacesIndex() const;
 *   (yetno) virtual const int* getPolyhedronIndex(MED_EN::medConnectivity ConnectivityType) const;
 *   (yetno) virtual int getNumberOfPolyhedronFaces() const;
 *   (yetno) virtual int getNumberOfPolyhedron() const;
 *   (yetno) int *getNodesOfPolyhedron(int polyhedronId, int& lgthOfTab) const;
 *   (yetno) int **getNodesPerFaceOfPolyhedron(int polyhedronId, int& nbOfFaces,
 *                                             int* & nbOfNodesPerFaces) const;
 *   (yetno) const CELLMODEL & getType (MED_EN::medGeometryElement Type) const;
 *   (yetno) const CELLMODEL * getCellsTypes (MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION);
 *   (yetno) string * getCellTypeNames (MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION);
 *   (yetno) int getNumberOfNodesInType (MED_EN::medGeometryElement Type) const;
 *   (yetno) int getNumberOfSubCellInType (MED_EN::medGeometryElement Type) const;
 *   (yetno) virtual int getNumberOf (MED_EN::medEntityMesh Entity,
 *                                      MED_EN::medGeometryElement Type) const;
 *   (yetno) virtual const int* getValue (MED_EN::medConnectivity TypeConnectivity,
 *                                           MED_EN::medGeometryElement Type);
 *   (yetno) virtual const int* getValueIndex (MED_EN::medConnectivity TypeConnectivity);
 *   (yetno) virtual inline const int* getReverseConnectivity (MED_EN::medConnectivity ConnectivityType,
 *                     MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) throw (MEDEXCEPTION);
 *   (yetno) virtual inline const int* getReverseConnectivityIndex (MED_EN::medConnectivity ConnectivityType,
 *                      MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) throw (MEDEXCEPTION);
 *   (yetno) const int* getNeighbourhood() const;
 *   (yetno) void invertConnectivityForAFace(int faceId, const int *nodalConnForFace, bool polygonFace=false);
 *   (yetno) bool deepCompare(const CONNECTIVITY& other) const;
 *  }
 */
void MEDMEMTest::testCompatibility21_22()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #7: MEDMEM_Connectivity.hxx  }  MEDMEMTest_Connectivity.cxx
// #8: MEDMEM_Coordinate.hxx    }  MEDMEMTest_Coordinate.cxx
// #9: MEDMEM_DriverFactory.hxx }  MEDMEMTest_DriverFactory.cxx

// #10: MEDMEM_DriversDef.hxx   }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_DriversDef.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testDriversDef()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #11: MEDMEM_DriverTools.hxx   }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_DriverTools.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testDriverTools()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #12: MEDMEM_Exception.hxx          }  MEDMEMTest_Exception.cxx
// #13: MEDMEM_Family.hxx             }  MEDMEMTest.cxx
// #14: MEDMEM_Field.hxx              \
// #15: MEDMEM_FieldConvert.hxx       /  MEDMEMTest_Field.cxx
// #16: MEDMEM_FieldForward.hxx       }  no methods to test
// #17: MEDMEM_Formulae.hxx           }  MEDMEMTest_Formulae.cxx
// #18: MEDMEM_GaussLocalization.hxx  }  MEDMEMTest_GaussLocalization.cxx

// #19: MEDMEM_GenDriver.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_GenDriver.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testGenDriver()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #20: MEDMEM_GibiMeshDriver.hxx  }  MEDMEMTest_GibiMeshDriver.cxx

// #21: MEDMEM_Grid.cxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (44), defined in MEDMEM_Grid.hxx:
 *  class GRID: public MESH {
 *   (yetno) GRID();
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
 *   (yetno) inline int getNodeNumber(const int i, const int j=0, const int k=0) const;
 *   (yetno) inline int getCellNumber(const int i, const int j=0, const int k=0) const;
 *   (yetno) int getEdgeNumber
 *           (const int Axis, const int i, const int j=0, const int k=0) const throw (MEDEXCEPTION);
 *   (yetno) int getFaceNumber
 *           (const int Axis, const int i, const int j=0, const int k=0) const throw (MEDEXCEPTION);
 *   (yetno) void getNodePosition(const int Number, int& i, int& j, int& k) const throw (MEDEXCEPTION);
 *   (yetno) void getCellPosition(const int Number, int& i, int& j, int& k) const throw (MEDEXCEPTION);
 *   (yetno) void getEdgePosition
 *           (const int Number, int& Axis, int& i, int& j, int& k) const throw (MEDEXCEPTION);
 *   (yetno) void getFacePosition
 *           (const int Number, int& Axis, int& i, int& j, int& k) const throw (MEDEXCEPTION);
 *   (yetno) inline MED_EN::med_grid_type getGridType() const;
 *   (yetno) int getArrayLength(const int Axis) const throw (MEDEXCEPTION);
 *   (yetno) const double getArrayValue (const int Axis, const int i) const throw (MEDEXCEPTION);
 *   (yetno) inline const COORDINATE * getCoordinateptr() const;
 *   (yetno) inline const double * getCoordinates(MED_EN::medModeSwitch Mode) const;
 *   (yetno) inline const double getCoordinate(int Number,int Axis) const;
 *   (yetno) inline int getNumberOfTypes(MED_EN::medEntityMesh Entity) const;
 *   (yetno) inline int getNumberOfTypesWithPoly(MED_EN::medEntityMesh Entity) const;
 *   (yetno) inline const MED_EN::medGeometryElement * getTypes(MED_EN::medEntityMesh Entity) const;
 *   (yetno) MED_EN::medGeometryElement * getTypesWithPoly(MED_EN::medEntityMesh Entity) const;
 *   (yetno) inline const CELLMODEL * getCellsTypes(MED_EN::medEntityMesh Entity) const;
 *   (yetno) const int * getGlobalNumberingIndex(MED_EN::medEntityMesh Entity) const;
 *   (yetno) inline int getNumberOfElements
 *                (MED_EN::medEntityMesh Entity, MED_EN::medGeometryElement Type) const;
 *   (yetno) inline int getNumberOfElementsWithPoly
 *                (MED_EN::medEntityMesh Entity, MED_EN::medGeometryElement Type) const;
 *   (yetno) inline bool existConnectivity
 *                (MED_EN::medConnectivity ConnectivityType, MED_EN::medEntityMesh Entity) const;
 *   (yetno) inline MED_EN::medGeometryElement getElementType
 *                (MED_EN::medEntityMesh Entity, int Number) const;
 *   (yetno) inline MED_EN::medGeometryElement getElementTypeWithPoly
 *                (MED_EN::medEntityMesh Entity, int Number) const;
 *   (yetno) inline void calculateConnectivity(MED_EN::medModeSwitch Mode,
 *                                             MED_EN::medConnectivity ConnectivityType,
 *                                               MED_EN::medEntityMesh Entity) const;
 *   (yetno) inline const CONNECTIVITY* getConnectivityptr() const;
 *   (yetno) inline const int * getConnectivity
 *             (MED_EN::medModeSwitch Mode, MED_EN::medConnectivity ConnectivityType,
 *                MED_EN::medEntityMesh Entity, MED_EN::medGeometryElement Type) const;
 *   (yetno) inline const int * getConnectivityIndex(MED_EN::medConnectivity ConnectivityType,
 *                                          MED_EN::medEntityMesh Entity) const;
 *   (yetno) inline const int * getReverseConnectivity(MED_EN::medConnectivity ConnectivityType,
 *                                            MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const;
 *   (yetno) inline const int * getReverseConnectivityIndex(MED_EN::medConnectivity ConnectivityType,
 *                                                 MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const;
 *   (yetno) inline void setGridType(MED_EN::med_grid_type gridType);
 *  }
 */
void MEDMEMTest::testGrid()
{
  CPPUNIT_FAIL("Case Not Implemented");
}

// #22: MEDMEM_Group.cxx  }  MEDMEMTest_Group.cxx

// #23: MEDMEM_IndexCheckingPolicy.cxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (4), defined in MEDMEM_IndexCheckingPolicy.hxx:
 *  class IndexCheckPolicy {
 *   (yetno) void checkMoreThanZero(const std::string & classname, int index) const;
 *   (yetno) void checkLessOrEqualThan(const std::string & classname, int max, int index) const;
 *   (yetno) void checkInInclusiveRange(const std::string & classname, int min, int max, int index) const;
 *   (yetno) void checkEquality(const std::string & classname, int a, int b) const;
 *  }
 *  class NoIndexCheckPolicy {
 *   (yetno) void checkMoreThanZero(const string &classname, int index) const;
 *   (yetno) void checkLessOrEqualThan(const std::string & classname, int max, int index) const;
 *   (yetno) void checkInInclusiveRange(const std::string & classname, int min, int max, int index) const;
 *   (yetno) void checkEquality(const std::string & classname, int a, int b) const;
 *  }
 */
void MEDMEMTest::testIndexCheckingPolicy()
{
  CPPUNIT_FAIL("Case Not Implemented");
}

// #24: MEDMEM_Init.cxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (1), implemented in MEDMEM_Init.cxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testInit()
{
  CPPUNIT_FAIL("Case Not Implemented");
}

// #25: MEDMEM_InterlacingPolicy.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_InterlacingPolicy.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testInterlacingPolicy()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #26: MEDMEM_InterlacingTraits.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_InterlacingTraits.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testInterlacingTraits()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #27: MEDMEM_MedFieldDriver21.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (17), defined in MEDMEM_MedFieldDriver21.hxx:
 *  template <class T> class MED_FIELD_DRIVER21 : public virtual MED_FIELD_DRIVER<T> {
 *   (yetno) MED_FIELD_DRIVER21();
 *   (yetno) template <class INTERLACING_TAG> MED_FIELD_DRIVER21(const string & fileName,
                     FIELD<T, INTERLACING_TAG> * ptrField, MED_EN::med_mode_acces accessMode);
 *   (yetno) MED_FIELD_DRIVER21(const MED_FIELD_DRIVER21 & fieldDriver);
 *   (yetno) virtual ~MED_FIELD_DRIVER21();
 *   (yetno) void open() throw (MEDEXCEPTION);
 *   (yetno) void close();
 *  }
 *  template <class T> class MED_FIELD_RDONLY_DRIVER21 : public virtual MED_FIELD_DRIVER21<T>,
 *                                                       public virtual IMED_FIELD_RDONLY_DRIVER<T> {
 *   (yetno) MED_FIELD_RDONLY_DRIVER21();
 *   (yetno) template <class INTERLACING_TAG> MED_FIELD_RDONLY_DRIVER21
 *              (const string & fileName, FIELD<T, INTERLACING_TAG> * ptrField);
 *   (yetno) MED_FIELD_RDONLY_DRIVER21(const MED_FIELD_RDONLY_DRIVER21 & fieldDriver);
 *   (yetno) virtual ~MED_FIELD_RDONLY_DRIVER21();
 *   (yetno) void write(void) const throw (MEDEXCEPTION);
 *   (yetno) void read (void) throw (MEDEXCEPTION);
 *  }
 *  template <class T> class MED_FIELD_WRONLY_DRIVER21 : public virtual MED_FIELD_DRIVER21<T>,
 *                                                       public virtual IMED_FIELD_WRONLY_DRIVER<T> {
 *   (yetno) MED_FIELD_WRONLY_DRIVER21();
 *   (yetno) template <class INTERLACING_TAG> MED_FIELD_WRONLY_DRIVER21
 *              (const string & fileName, FIELD<T, INTERLACING_TAG> * ptrField);
 *   (yetno) MED_FIELD_WRONLY_DRIVER21(const MED_FIELD_WRONLY_DRIVER21 & fieldDriver);
 *   (yetno) virtual ~MED_FIELD_WRONLY_DRIVER21();
 *   (yetno) void write(void) const throw (MEDEXCEPTION);
 *   (yetno) void read (void) throw (MEDEXCEPTION);
 *  }
 *  template <class T> class MED_FIELD_RDWR_DRIVER21 : public MED_FIELD_RDONLY_DRIVER21<T>,
 *                           public MED_FIELD_WRONLY_DRIVER21<T>, public IMED_FIELD_RDWR_DRIVER<T> {
 *   (yetno) MED_FIELD_RDWR_DRIVER21();
 *   (yetno) template <class INTERLACING_TAG> MED_FIELD_RDWR_DRIVER21
 *              (const string & fileName, FIELD<T, INTERLACING_TAG> * ptrField);
 *   (yetno) MED_FIELD_RDWR_DRIVER21(const MED_FIELD_RDWR_DRIVER21 & fieldDriver);
 *   (yetno) ~MED_FIELD_RDWR_DRIVER21();
 *   (yetno) void write(void) const throw (MEDEXCEPTION);
 *   (yetno) void read (void) throw (MEDEXCEPTION);
 *  }
 */
void MEDMEMTest::testMedFieldDriver21()
{
  CPPUNIT_FAIL("Case Not Implemented");
}

// #28: MEDMEM_MedFieldDriver22.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (17), defined in MEDMEM_MedFieldDriver22.hxx:
 *  template <class T> class MED_FIELD_DRIVER22 : public virtual MED_FIELD_DRIVER<T> {
 *   (yetno) MED_FIELD_DRIVER22();
 *   (yetno) template <class INTERLACING_TAG> MED_FIELD_DRIVER22(const string & fileName,
 *          FIELD<T, INTERLACING_TAG> * ptrField, MED_EN::med_mode_acces accessMode);
 *   (yetno) MED_FIELD_DRIVER22(const MED_FIELD_DRIVER22 & fieldDriver);
 *   (yetno) virtual ~MED_FIELD_DRIVER22();
 *   (yetno) void open() throw (MEDEXCEPTION);
 *   (yetno) void close();
 *  }
 *  template <class T> class MED_FIELD_RDONLY_DRIVER22 : public virtual MED_FIELD_DRIVER22<T>,
 *                                                       public virtual IMED_FIELD_RDONLY_DRIVER<T> {
 *   (yetno) MED_FIELD_RDONLY_DRIVER22();
 *   (yetno) template <class INTERLACING_TAG> MED_FIELD_RDONLY_DRIVER22
 *                 (const string & fileName, FIELD<T, INTERLACING_TAG> * ptrField);
 *   (yetno) MED_FIELD_RDONLY_DRIVER22(const MED_FIELD_RDONLY_DRIVER22 & fieldDriver);
 *   (yetno) virtual ~MED_FIELD_RDONLY_DRIVER22();
 *   (yetno) void write(void) const throw (MEDEXCEPTION);
 *   (yetno) void read (void) throw (MEDEXCEPTION);
 *  }
 *  template <class T> class MED_FIELD_WRONLY_DRIVER22 : public virtual MED_FIELD_DRIVER22<T>,
 *                                                       public virtual IMED_FIELD_WRONLY_DRIVER<T> {
 *   (yetno) MED_FIELD_WRONLY_DRIVER22();
 *   (yetno) template <class INTERLACING_TAG> MED_FIELD_WRONLY_DRIVER22
 *                   (const string & fileName, FIELD<T, INTERLACING_TAG> * ptrField);
 *   (yetno) MED_FIELD_WRONLY_DRIVER22(const MED_FIELD_WRONLY_DRIVER22 & fieldDriver);
 *   (yetno) virtual ~MED_FIELD_WRONLY_DRIVER22();
 *   (yetno) void write(void) const throw (MEDEXCEPTION);
 *   (yetno) void read (void) throw (MEDEXCEPTION);
 *  }
 *  template <class T> class MED_FIELD_RDWR_DRIVER22 : public MED_FIELD_RDONLY_DRIVER22<T>,
 *                           public MED_FIELD_WRONLY_DRIVER22<T>, public IMED_FIELD_RDWR_DRIVER<T> {
 *   (yetno) MED_FIELD_RDWR_DRIVER22();
 *   (yetno) template <class INTERLACING_TAG> MED_FIELD_RDWR_DRIVER22
 *              (const string & fileName, FIELD<T, INTERLACING_TAG> * ptrField);
 *   (yetno) MED_FIELD_RDWR_DRIVER22(const MED_FIELD_RDWR_DRIVER22 & fieldDriver);
 *   (yetno) ~MED_FIELD_RDWR_DRIVER22();
 *   (yetno) void write(void) const throw (MEDEXCEPTION);
 *   (yetno) void read (void) throw (MEDEXCEPTION);
 *  }
 */
void MEDMEMTest::testMedFieldDriver22()
{
  CPPUNIT_FAIL("Case Not Implemented");
}

// #29: MEDMEM_MedFieldDriver.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_MedFieldDriver.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testMedFieldDriver()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #30: MEDMEM_Med.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (26), defined in MEDMEM_Med.hxx:
 *  class MED {
 *   (yetno) MED();
 *   (yetno) MED (driverTypes driverType, const string & fileName);
 *   (yetno) ~MED();
 *   (yetno) void addField (FIELD_ * const ptrField) throw (MED_EXCEPTION);
 *   (yetno) void addMesh  (MESH   * const ptrMesh) throw (MED_EXCEPTION);
 *   (yetno) int addDriver (driverTypes driverType, const string & fileName,
 *                          MED_EN::med_mode_acces access=MED_EN::MED_REMP);
 *   (yetno) int addDriver (GENDRIVER & driver);
 *   (yetno) void rmDriver (int index=0) throw (MEDEXCEPTION);
 *   (yetno) void readFileStruct(int index=0) throw (MEDEXCEPTION);
 *   (yetno) void read (int index=0) throw (MEDEXCEPTION);
 *   (yetno) void writeFrom (int index=0) throw (MEDEXCEPTION);
 *   (yetno) void write (int index=0) throw (MEDEXCEPTION);
 *   (yetno) int  getNumberOfMeshes (void) const;
 *   (yetno) int  getNumberOfFields (void) const;
 *   (yetno) void getMeshNames (string * meshNames) const throw (MEDEXCEPTION);
 *   (yetno) deque<string> getMeshNames   () const;
 *   (yetno) MESH * getMesh (const string & meshName) const throw (MEDEXCEPTION);
 *   (yetno) MESH * getMesh (const  FIELD_ * const field) const throw (MEDEXCEPTION);
 *   (yetno) void getFieldNames (string * fieldNames) const throw (MEDEXCEPTION);
 *   (yetno) deque<string> getFieldNames () const;
 *   (yetno) deque<DT_IT_> getFieldIteration (const string & fieldName) const throw (MEDEXCEPTION);
 *   (yetno) FIELD_ * getField (const string & fieldName, const int dt,
 *                              const int it) const throw (MEDEXCEPTION);
 *   (yetno) template<class T> FIELD<T> * getFieldT
 *           (const string & fieldName, const int dt,  const int it) const throw (MEDEXCEPTION);
 *   (yetno) FIELD_ * getField2 (const string & fieldName, double time, int it=0) const throw (MEDEXCEPTION);
 *   (yetno) const map<MED_EN::medEntityMesh,SUPPORT *> & getSupports
 *           (const string & meshName) const throw (MEDEXCEPTION);
 *   (yetno) SUPPORT *  getSupport (const string & meshName,
 *                                  MED_EN::medEntityMesh entity) const throw (MEDEXCEPTION);
 *   (yetno) void updateSupport ();
 *  }
 */
void MEDMEMTest::testMed()
{
  CPPUNIT_FAIL("Case Not Implemented");
}

// #31: MEDMEM_MedMedDriver21.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (21), defined in MEDMEM_MedMedDriver21.hxx:
 *  class MED_MED_DRIVER21 : public virtual MED_MED_DRIVER {
 *   (yetno) MED_MED_DRIVER21();
 *   (yetno) MED_MED_DRIVER21(const string & fileName,  MED * const ptrMed);
 *   (yetno) MED_MED_DRIVER21(const string & fileName,  MED * const ptrMed,
 *                            MED_EN::med_mode_acces accessMode);
 *   (yetno) MED_MED_DRIVER21(const MED_MED_DRIVER21 & driver);
 *   (yetno) virtual ~MED_MED_DRIVER21();
 *   (yetno) void open() throw (MEDEXCEPTION);
 *   (yetno) void close();
 *   (yetno) virtual void write          (void) const;
 *   (yetno) virtual void writeFrom      (void) const;
 *   (yetno) virtual void read           (void);
 *   (yetno) virtual void readFileStruct (void);
 *  }
 *  class MED_MED_RDONLY_DRIVER21 : public virtual IMED_MED_RDONLY_DRIVER,
 *                                  public virtual MED_MED_DRIVER21 {
 *   (yetno) MED_MED_RDONLY_DRIVER21();
 *   (yetno) MED_MED_RDONLY_DRIVER21(const string & fileName,  MED * const ptrMed);
 *   (yetno) MED_MED_RDONLY_DRIVER21(const MED_MED_RDONLY_DRIVER21 & driver);
 *   (yetno) virtual ~MED_MED_RDONLY_DRIVER21();
 *   (yetno) void write          (void) const throw (MEDEXCEPTION);
 *   (yetno) void writeFrom      (void) const throw (MEDEXCEPTION);
 *   (yetno) void read           (void) throw (MEDEXCEPTION);
 *   (yetno) void readFileStruct (void) throw (MEDEXCEPTION);
 *  }
 *  class MED_MED_WRONLY_DRIVER21 : public virtual IMED_MED_WRONLY_DRIVER,
 *                                  public virtual MED_MED_DRIVER21 {
 *   (yetno) MED_MED_WRONLY_DRIVER21();
 *   (yetno) MED_MED_WRONLY_DRIVER21(const string & fileName,  MED * const ptrMed);
 *   (yetno) MED_MED_WRONLY_DRIVER21(const MED_MED_WRONLY_DRIVER21 & driver);
 *   (yetno) virtual ~MED_MED_WRONLY_DRIVER21();
 *   (yetno) void write          (void) const throw (MEDEXCEPTION);
 *   (yetno) void writeFrom      (void) const throw (MEDEXCEPTION);
 *   (yetno) void read           (void) throw (MEDEXCEPTION);
 *   (yetno) void readFileStruct (void) throw (MEDEXCEPTION);
 *  }
 *  class MED_MED_RDWR_DRIVER21 : public MED_MED_RDONLY_DRIVER21,
 *                              public MED_MED_WRONLY_DRIVER21,
 *                              public IMED_MED_RDWR_DRIVER {
 *   (yetno) MED_MED_RDWR_DRIVER21();
 *   (yetno) MED_MED_RDWR_DRIVER21(const string & fileName,  MED * const ptrMed);
 *   (yetno) MED_MED_RDWR_DRIVER21(const MED_MED_RDWR_DRIVER21 & driver);
 *   (yetno) ~MED_MED_RDWR_DRIVER21();
 *   (yetno) void write          (void) const throw (MEDEXCEPTION);
 *   (yetno) void writeFrom      (void) const throw (MEDEXCEPTION);
 *   (yetno) void read           (void) throw (MEDEXCEPTION);
 *   (yetno) void readFileStruct (void) throw (MEDEXCEPTION);
 *  }
 */
void MEDMEMTest::testMedMedDriver21()
{
  CPPUNIT_FAIL("Case Not Implemented");
}

// #32: MEDMEM_MedMedDriver22.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (21), defined in MEDMEM_MedMedDriver22.hxx:
 *  class MED_MED_DRIVER22 : public virtual MED_MED_DRIVER {
 *   (yetno) MED_MED_DRIVER22();
 *   (yetno) MED_MED_DRIVER22(const string & fileName,  MED * const ptrMed);
 *   (yetno) MED_MED_DRIVER22(const string & fileName,  MED * const ptrMed,
 *                            MED_EN::med_mode_acces accessMode);
 *   (yetno) MED_MED_DRIVER22(const MED_MED_DRIVER22 & driver);
 *   (yetno) virtual ~MED_MED_DRIVER22();
 *   (yetno) void open() throw (MEDEXCEPTION);
 *   (yetno) void close();
 *   (yetno) virtual void write          (void) const;
 *   (yetno) virtual void writeFrom      (void) const;
 *   (yetno) virtual void read           (void);
 *   (yetno) virtual void readFileStruct (void);
 *  }
 *  class MED_MED_RDONLY_DRIVER22 : public virtual IMED_MED_RDONLY_DRIVER,
 *                                  public virtual MED_MED_DRIVER22 {
 *   (yetno) MED_MED_RDONLY_DRIVER22();
 *   (yetno) MED_MED_RDONLY_DRIVER22(const string & fileName,  MED * const ptrMed);
 *   (yetno) MED_MED_RDONLY_DRIVER22(const MED_MED_RDONLY_DRIVER22 & driver);
 *   (yetno) virtual ~MED_MED_RDONLY_DRIVER22();
 *   (yetno) void write          (void) const throw (MEDEXCEPTION);
 *   (yetno) void writeFrom      (void) const throw (MEDEXCEPTION);
 *   (yetno) void read           (void) throw (MEDEXCEPTION);
 *   (yetno) void readFileStruct (void) throw (MEDEXCEPTION);
 *  }
 *  class MED_MED_WRONLY_DRIVER22 : public virtual IMED_MED_WRONLY_DRIVER,
 *                                  public virtual MED_MED_DRIVER22 {
 *   (yetno) MED_MED_WRONLY_DRIVER22();
 *   (yetno) MED_MED_WRONLY_DRIVER22(const string & fileName,  MED * const ptrMed);
 *   (yetno) MED_MED_WRONLY_DRIVER22(const MED_MED_WRONLY_DRIVER22 & driver);
 *   (yetno) virtual ~MED_MED_WRONLY_DRIVER22();
 *   (yetno) void write          (void) const throw (MEDEXCEPTION);
 *   (yetno) void writeFrom      (void) const throw (MEDEXCEPTION);
 *   (yetno) void read           (void) throw (MEDEXCEPTION);
 *   (yetno) void readFileStruct (void) throw (MEDEXCEPTION);
 *  }
 *  class MED_MED_RDWR_DRIVER22 : public virtual MED_MED_RDONLY_DRIVER22,
 *                                public virtual MED_MED_WRONLY_DRIVER22,
 *                                public virtual IMED_MED_RDWR_DRIVER {
 *   (yetno) MED_MED_RDWR_DRIVER22();
 *   (yetno) MED_MED_RDWR_DRIVER22(const string & fileName,  MED * const ptrMed);
 *   (yetno) MED_MED_RDWR_DRIVER22(const MED_MED_RDWR_DRIVER22 & driver);
 *   (yetno) ~MED_MED_RDWR_DRIVER22();
 *   (yetno) void write          (void) const throw (MEDEXCEPTION);
 *   (yetno) void writeFrom      (void) const throw (MEDEXCEPTION);
 *   (yetno) void read           (void) throw (MEDEXCEPTION);
 *   (yetno) void readFileStruct (void) throw (MEDEXCEPTION);
 *  }
 */
void MEDMEMTest::testMedMedDriver22()
{
  CPPUNIT_FAIL("Case Not Implemented");
}

// #33: MEDMEM_MedMedDriver.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_MedMedDriver.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testMedMedDriver()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #34: MEDMEM_MEDMEMchampLire.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_MEDMEMchampLire.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testMEDMEMchampLire()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #35: MEDMEM_MEDMEMgaussEcr.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_MEDMEMgaussEcr.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testMEDMEMgaussEcr()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #36: MEDMEM_MEDMEMprofilEcr.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_MEDMEMprofilEcr.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testMEDMEMprofilEcr()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #37: MEDMEM_MedMeshDriver21.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (13), defined in MEDMEM_MedMeshDriver21.hxx:
 *  class MED_MESH_DRIVER21 : public virtual MED_MESH_DRIVER {
 *   (yetno) MED_MESH_DRIVER21();
 *   (yetno) MED_MESH_DRIVER21(const string & fileName, MESH * ptrMesh, MED_EN::med_mode_acces accessMode);
 *   (yetno) MED_MESH_DRIVER21(const MED_MESH_DRIVER21 & driver);
 *   (yetno) virtual ~MED_MESH_DRIVER21();
 *   (yetno) void open();
 *   (yetno) void close();
 *  }
 *  class MED_MESH_RDONLY_DRIVER21 : public virtual IMED_MESH_RDONLY_DRIVER,
 *                                   public virtual MED_MESH_DRIVER21 {
 *   (yetno) MED_MESH_RDONLY_DRIVER21();
 *   (yetno) MED_MESH_RDONLY_DRIVER21(const string & fileName, MESH * ptrMesh);
 *   (yetno) MED_MESH_RDONLY_DRIVER21(const MED_MESH_RDONLY_DRIVER21 & driver);
 *   (yetno) virtual ~MED_MESH_RDONLY_DRIVER21();
 *   (yetno) void read(void);
 *  }
 *  class MED_MESH_WRONLY_DRIVER21 : public virtual IMED_MESH_WRONLY_DRIVER,
 *                                   public virtual MED_MESH_DRIVER21 {
 *   (yetno) MED_MESH_WRONLY_DRIVER21();
 *   (yetno) MED_MESH_WRONLY_DRIVER21(const string & fileName, MESH * ptrMesh);
 *   (yetno) MED_MESH_WRONLY_DRIVER21(const MED_MESH_WRONLY_DRIVER21 & driver);
 *   (yetno) virtual ~MED_MESH_WRONLY_DRIVER21();
 *   (yetno) void write(void) const;
 *  }
 *  class MED_MESH_RDWR_DRIVER21 : public IMED_MESH_RDWR_DRIVER,
 *                                 public MED_MESH_RDONLY_DRIVER21, public MED_MESH_WRONLY_DRIVER21 {
 *   (yetno) MED_MESH_RDWR_DRIVER21();
 *   (yetno) MED_MESH_RDWR_DRIVER21(const string & fileName, MESH * ptrMesh);
 *   (yetno) MED_MESH_RDWR_DRIVER21(const MED_MESH_RDWR_DRIVER21 & driver);
 *   (yetno) ~MED_MESH_RDWR_DRIVER21();
 *   (yetno) void write(void) const;
 *   (yetno) void read (void);
 *  }
 */
void MEDMEMTest::testMedMeshDriver21()
{
  CPPUNIT_FAIL("Case Not Implemented");
}

// #38: MEDMEM_MedMeshDriver22.hxx  }  MEDMEMTest_MedMeshDriver22.cxx


// #39: MEDMEM_MedMeshDriver.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_MedMeshDriver.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testMedMeshDriver()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #40: MEDMEM_MedVersion.hxx  }  MEDMEMTest_MedVersion.cxx


// #41: MEDMEM_Mesh.hxx     \ 
// #42: MEDMEM_Meshing.hxx  /  MEDMEMTest_MeshAndMeshing.cxx

// #43: MEDMEM_ModulusArray.hxx  }  MEDMEMTest_ModulusArray.cxx

// #44: MEDMEM_nArray.hxx  }  MEDMEMTest_nArray.cxx

// #45: MEDMEM_PointerOf.hxx  }  MEDMEMTest_PointerOf.cxx

// #46: MEDMEM_PolyhedronArray.hxx  }  MEDMEMTest_PolyhedronArray.cxx

// #47: MEDMEM_PorflowMeshDriver.hxx  }  MEDMEMTest_PorflowMeshDriver.cxx

// #48: MEDMEM_RCBase.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_RCBase.hxx:
 *  class RCBASE {
 *   (yetno) virtual void addReference() const = 0;
 *   (yetno) virtual void removeReference() const = 0;
 *  }
 */
void MEDMEMTest::testRCBase()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #49: MEDMEM_SetInterlacingType.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_SetInterlacingType.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testSetInterlacingType()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #50: MEDMEM_SkyLineArray.hxx  }  MEDMEMTest_SkyLineArray.cxx

// #51: MEDMEM_STRING.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (2), defined in MEDMEM_STRING.hxx:
 *  class STRING : public string {
 *   (yetno) STRING()
 *   (yetno) ~STRING()
 *   (yetno) operator const char * () const
 *   (yetno) template <class T> STRING(const T &valeur)
 *   (yetno) template <class T> STRING & operator<<(const T &valeur)
 *  }
 */
void MEDMEMTest::testSTRING()
{
  CPPUNIT_FAIL("Case Not Implemented");
}

// #52: MEDMEM_Support.hxx  }  MEDMEMTest_Support.cxx

/*!
 *  Check methods (48), defined in MEDMEM_Support.hxx:
 *  class SUPPORT : public RCBASE {
 *   (yetno) SUPPORT();
 *   (yetno) SUPPORT(MESH* Mesh, string Name="", MED_EN::medEntityMesh Entity=MED_EN::MED_CELL);
 *   (yetno) SUPPORT(const SUPPORT & m);
 *   (yetno) virtual ~SUPPORT();
 *   (yetno) friend ostream & operator<<(ostream &os,const SUPPORT &my);
 *   (yetno) SUPPORT& operator=(const SUPPORT &support);
 *   (yetno) bool operator == (const SUPPORT &support) const;
 *   (yetno) bool deepCompare(const SUPPORT &support) const;
 *   (yetno) void update();
 *   (yetno) inline void setName(string Name);
 *   (yetno) inline void setDescription(string Description);
 *   (yetno) void setMesh(MESH *Mesh) const;
 *   (yetno) inline void setMeshName(const string & meshName);
 *   (yetno) inline void setAll(bool All);
 *   (yetno) inline void setEntity(MED_EN::medEntityMesh Entity);
 *   (yetno) inline void setNumberOfGeometricType(int NumberOfGeometricType);
 *   (yetno) inline void setGeometricType(const MED_EN::medGeometryElement *GeometricType);
 *   (yetno) inline void setNumberOfElements(const int *NumberOfElements);
 *   (yetno) inline void setTotalNumberOfElements(int TotalNumberOfElements);
 *   (yetno) inline void setNumber(MEDSKYLINEARRAY * Number);
 *   (yetno) inline void setNumber(const int * index, const int* value, bool shallowCopy=false);
 *   (yetno) inline string getName() const;
 *   (yetno) inline string getDescription() const;
 *   (yetno) virtual inline MESH * getMesh() const;
 *   (yetno) string getMeshName() const;
 *   (yetno) inline MED_EN::medEntityMesh getEntity() const;
 *   (yetno) inline bool   isOnAllElements() const;
 *   (yetno) inline int    getNumberOfTypes() const;
 *   (yetno) inline const MED_EN::medGeometryElement* getTypes() const;
 *   (yetno) inline int    getNumberOfElements(MED_EN::medGeometryElement GeometricType) const throw (MEDEXCEPTION);
 *   (yetno) inline  const int * getNumberOfElements() const throw (MEDEXCEPTION);
 *   (yetno) virtual inline MEDSKYLINEARRAY *  getnumber() const throw (MEDEXCEPTION);
 *   (yetno) virtual inline MEDSKYLINEARRAY *  getnumberFromFile() const throw (MEDEXCEPTION);
 *   (yetno) virtual inline const int *  getNumber(MED_EN::medGeometryElement GeometricType) const throw (MEDEXCEPTION);
 *   (yetno) virtual inline const int *  getNumberFromFile(MED_EN::medGeometryElement GeometricType) const throw (MEDEXCEPTION);
 *   (yetno) virtual inline const int *  getNumberIndex() const throw (MEDEXCEPTION);
 *   (yetno) virtual int getValIndFromGlobalNumber(const int number) const throw (MEDEXCEPTION);
 *   (yetno) void blending(SUPPORT * mySupport) throw (MEDEXCEPTION);
 *   (yetno) void setpartial(string Description, int NumberOfGeometricType,
 *                           int TotalNumberOfEntity, MED_EN::medGeometryElement *GeometricType,
 *                           int *NumberOfEntity, int *NumberValue);
 *   (yetno) void setpartial(MEDSKYLINEARRAY * number, bool shallowCopy=false) throw (MEDEXCEPTION);
 *   (yetno) void setpartial_fromfile(MEDSKYLINEARRAY * number, bool shallowCopy=false) throw (MEDEXCEPTION);
 *   (yetno) void   setProfilNames(vector<string> profilNames) throw (MEDEXCEPTION);
 *   (yetno) vector<string> getProfilNames() const throw (MEDEXCEPTION);
 *   (yetno) void getBoundaryElements() throw (MEDEXCEPTION);
 *   (yetno) void changeElementsNbs(MED_EN::medEntityMesh entity, const int *renumberingFromOldToNew, int limitNbClassicPoly, const int *renumberingFromOldToNewPoly=0);
 *   (yetno) void intersecting(SUPPORT * mySupport) throw (MEDEXCEPTION);
 *   (yetno) bool belongsTo(const SUPPORT& other, bool deepCompare=false) const;
 *   (yetno) SUPPORT *getComplement() const;
 *   (yetno) SUPPORT *substract(const SUPPORT& other) const throw (MEDEXCEPTION);
 *   (yetno) SUPPORT *getBoundaryElements(MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION);
 *   (yetno) void fillFromNodeList(const list<int>& listOfNode) throw (MEDEXCEPTION);
 *   (yetno) void fillFromElementList(const list<int>& listOfElt) throw (MEDEXCEPTION);
 *   (yetno) void clearDataOnNumbers();
 *   (yetno) virtual void addReference() const;
 *   (yetno) virtual void removeReference() const;
 *  }
 */
void MEDMEMTest::testSupport()
{
  CPPUNIT_FAIL("Case Not Implemented");
}

// #53: MEDMEM_Tags.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_Tags.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testTags()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #54: MEDMEM_TopLevel.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (6), defined in MEDMEM_TopLevel.hxx:
 *  (yetno) MED *readMedInFile(const std::string& fileName) throw(MEDEXCEPTION);
 *  (yetno) MESH *readMeshInFile(const std::string& fileName, const std::string& meshName);
 *  (yetno) template<class T> FIELD<T> *readFieldInFile
 *                            (const std::string& fileName, const std::string& fieldName);
 *  (yetno) void writeMedToFile(const MED *medObj, const std::string& fileName);
 *  (yetno) void writeMeshToFile(const MESH *meshObj, const std::string& fileName);
 *  (yetno) template<class T> void writeFieldToFile(const FIELD<T> *fieldObj, const std::string& fileName);
 */
void MEDMEMTest::testTopLevel()
{
  CPPUNIT_FAIL("Case Not Implemented");
}

// #55: MEDMEM_TypeMeshDriver.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (15), defined in MEDMEM_TypeMeshDriver.hxx:
 *  class TYPE_MESH_DRIVER : public GENDRIVER {
 *   (yetno) TYPE_MESH_DRIVER();
 *   (yetno) TYPE_MESH_DRIVER(const string & fileName, MESH * ptrMesh, med_mode_acces accessMode);
 *   (yetno) TYPE_MESH_DRIVER(const TYPE_MESH_DRIVER & driver);
 *   (yetno) virtual ~TYPE_MESH_DRIVER();
 *   (yetno) void open() throw (MEDEXCEPTION);
 *   (yetno) void close() throw (MEDEXCEPTION);
 *   (yetno) virtual void write(void) const = 0;
 *   (yetno) virtual void read (void) = 0;
 *   (yetno) void   setMeshName(const string & meshName);
 *   (yetno) string getMeshName() const;
 *  }
 *  class TYPE_MESH_RDONLY_DRIVER : public virtual TYPE_MESH_DRIVER {
 *   (yetno) TYPE_MESH_RDONLY_DRIVER();
 *   (yetno) TYPE_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh);
 *   (yetno) TYPE_MESH_RDONLY_DRIVER(const TYPE_MESH_RDONLY_DRIVER & driver);
 *   (yetno) virtual ~TYPE_MESH_RDONLY_DRIVER();
 *   (yetno) void write(void) const throw (MEDEXCEPTION);
 *   (yetno) void read (void) throw (MEDEXCEPTION);
 *  }
 *  class TYPE_MESH_WRONLY_DRIVER : public virtual TYPE_MESH_DRIVER {
 *   (yetno) TYPE_MESH_WRONLY_DRIVER();
 *   (yetno) TYPE_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh);
 *   (yetno) TYPE_MESH_WRONLY_DRIVER(const TYPE_MESH_WRONLY_DRIVER & driver);
 *   (yetno) virtual ~TYPE_MESH_WRONLY_DRIVER();
 *   (yetno) void write(void) const throw (MEDEXCEPTION);
 *   (yetno) void read (void) throw (MEDEXCEPTION);
 *  }
 *  class TYPE_MESH_RDWR_DRIVER : public TYPE_MESH_RDONLY_DRIVER, public TYPE_MESH_WRONLY_DRIVER {
 *   (yetno) TYPE_MESH_RDWR_DRIVER();
 *   (yetno) TYPE_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh);
 *   (yetno) TYPE_MESH_RDWR_DRIVER(const TYPE_MESH_RDWR_DRIVER & driver);
 *   (yetno) ~TYPE_MESH_RDWR_DRIVER();
 *   (yetno) void write(void) const throw (MEDEXCEPTION);
 *   (yetno) void read (void) throw (MEDEXCEPTION);
 *  }
 */
void MEDMEMTest::testTypeMeshDriver()
{
  CPPUNIT_FAIL("Case Not Implemented");
}

// #56: MEDMEM_Unit.hxx  }  MEDMEMTest_Unit.cxx


// #57: MEDMEM_VtkFieldDriver.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (14), defined in MEDMEM_VtkFieldDriver.hxx:
 *  template <class T> class VTK_FIELD_DRIVER : public GENDRIVER {
 *   (yetno) template <class INTERLACING_TAG> VTK_FIELD_DRIVER();
 *   (yetno) template <class INTERLACING_TAG> VTK_FIELD_DRIVER
 *               (const string & fileName, FIELD<T, INTERLACING_TAG> * ptrField)
 *   (yetno) VTK_FIELD_DRIVER(const VTK_FIELD_DRIVER & fieldDriver)
 *   (yetno) ~VTK_FIELD_DRIVER()
 *   (yetno) void openConst() const throw (MEDEXCEPTION)
 *   (yetno) void openConstAppend() const throw (MEDEXCEPTION)
 *   (yetno) void open() throw (MEDEXCEPTION)
 *   (yetno) void openAppend() throw (MEDEXCEPTION)
 *   (yetno) void closeConst() const throw (MEDEXCEPTION)
 *   (yetno) void close()
 *   (yetno) void setFieldName(const string & fieldName);
 *   (yetno) string getFieldName() const;
 *   (yetno) void read (void) throw (MEDEXCEPTION);
 *   (yetno) void write(void) const throw (MEDEXCEPTION);
 *   (yetno) void writeAppend(void) const throw (MEDEXCEPTION);
 *  }
 */
void MEDMEMTest::testVtkFieldDriver()
{
  CPPUNIT_FAIL("Case Not Implemented");
}

// #58: MEDMEM_VtkMedDriver.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (5), defined in MEDMEM_VtkMedDriver.hxx:
 *  class VTK_MED_DRIVER : public GENDRIVER {
 *   (yetno) VTK_MED_DRIVER(const string & fileName,  MED * const ptrMed);
 *   (yetno) VTK_MED_DRIVER(const VTK_MED_DRIVER & driver);
 *   (yetno) ~VTK_MED_DRIVER();
 *   (yetno) void write (void) const;
 *   (yetno) virtual void read (void)  {};
 *   (yetno) GENDRIVER * copy (void) const;
 *  }
 */
void MEDMEMTest::testVtkMedDriver()
{
  CPPUNIT_FAIL("Case Not Implemented");
}

// #59: MEDMEM_VtkMeshDriver.hxx  }  MEDMEMTest_VtkMeshDriver.cxx

// #60: MEDMEM_medimport_src.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_medimport_src.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testmedimport_src()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

//60
//21 not in spec

//////////////////////////////////////
// HELP METHODS, used in many tests //
//////////////////////////////////////

/*!
 *  Create a mesh with certain filling
 *                 .n4
 *                 |
 *                 |
 *  n16,n12,n8.---------.n7,n11,n15
 *            |    |    |
 *            |    |    |
 *     n5.----|----.----|----.n3
 *            |    |    |
 *            |    |    |
 *  n17,n13,n9.---------.n10,n14,n18
 *                 |
 *                 |
 *                 .n6
 *
 *                 .n19 (0,0,5)
 *                 |
 *                 |
 *                 |
 *                 |
 *           n16.--|-.----.n15
 *             /   |/    /
 *            .    .    .
 *           /    /|   /
 *       n17.----.----.n18
 *                 |
 *                 |
 *           n12.--|-.----.n11
 *             /   |/    /
 *            .    .    .
 *           /    /|   /
 *       n13.----.----.n14
 *                 |
 *                 |
 *            n8.--|-.----.n7
 *             /   |/    /
 *            .    .    .
 *           /    /|   /
 *        n9.----.----.n10
 *                 |   .n4 (0,2,1)
 *                 |  /
 *                 | .
 *                 |/
 *     n5.----.----.----.----.n3 (2,0,1)
 *   (-2,0,1)     /n2 (0,0,1)
 *               . |
 *              /  |
 *             .n6 |
 *                 |
 *                 |
 *                 .n1 (0,0,0)
 */
MEDMEM::MESH * MEDMEMTest::createTestMesh ()
{
  // MESH DATA
  int SpaceDimension = 3;
  int meshDimension = SpaceDimension; // because there 3D cells in the mesh

  // coordinates
  int NumberOfNodes = 19;

  string Names[3] = { "X","Y","Z" };
  string Units[3] = { "cm","cm","cm" };

  double Coordinates[57] = {
     0.0,  0.0, 0.0, // n1
     0.0,  0.0, 1.0, // n2
     2.0,  0.0, 1.0, // n3
     0.0,  2.0, 1.0, // n4
    -2.0,  0.0, 1.0, // n5
     0.0, -2.0, 1.0, // n6
     1.0,  1.0, 2.0, // n7
    -1.0,  1.0, 2.0, // n8
    -1.0, -1.0, 2.0, // n9
     1.0, -1.0, 2.0, // n10
     1.0,  1.0, 3.0, // n11
    -1.0,  1.0, 3.0, // n12
    -1.0, -1.0, 3.0, // n13
     1.0, -1.0, 3.0, // n14
     1.0,  1.0, 4.0, // n15
    -1.0,  1.0, 4.0, // n16
    -1.0, -1.0, 4.0, // n17
     1.0, -1.0, 4.0, // n18
     0.0,  0.0, 5.0  // n19
  };

  // cells connectivities
  const int NumberOfCellTypes = 3;
  MED_EN::medGeometryElement CellTypes[NumberOfCellTypes] =
    {MED_EN::MED_TETRA4, MED_EN::MED_PYRA5, MED_EN::MED_HEXA8};
  const int NumberOfCells[NumberOfCellTypes] = {12, 2, 2};

  int ConnectivityTetra[12 * 4] = {
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

  int ConnectivityPyra[2 * 5] = {
    7,8,9,10,2,
    15,18,17,16,19
  };

  int ConnectivityHexa[2 * 8] = {
    11,12,13,14,7,8,9,10,
    15,16,17,18,11,12,13,14
  };

  // faces connectivities
  const int NumberOfFaceTypes = 2;
  MED_EN::medGeometryElement FaceTypes[NumberOfFaceTypes] = {MED_EN::MED_TRIA3, MED_EN::MED_QUAD4};
  const int NumberOfFaces[NumberOfFaceTypes] = {4, 4};

  int ConnectivityTria[4 * 3] = {
    1,4,3,
    1,5,4,
    1,6,5,
    1,3,6
  };

  int ConnectivityQua[4 * 4] = {
    7,8,9,10,
    11,12,13,14,
    11,7,8,12,
    12,8,9,13
  };


  // CREATE THE MESH
  MEDMEM::MESHING* myMeshing = new MEDMEM::MESHING;

  myMeshing->setName("meshing");

  myMeshing->setCoordinates(SpaceDimension, NumberOfNodes, Coordinates,
                           "CARTESIAN", MED_EN::MED_FULL_INTERLACE);
  myMeshing->setCoordinatesNames(Names);
  myMeshing->setCoordinatesUnits(Units);

  // define connectivities
  myMeshing->setNumberOfTypes(NumberOfCellTypes, MED_EN::MED_CELL);
  myMeshing->setTypes(CellTypes, MED_EN::MED_CELL);
  myMeshing->setNumberOfElements(NumberOfCells, MED_EN::MED_CELL);
  
  myMeshing->setConnectivity(ConnectivityTetra, MED_EN::MED_CELL, MED_EN::MED_TETRA4);
  myMeshing->setConnectivity(ConnectivityPyra, MED_EN::MED_CELL, MED_EN::MED_PYRA5);
  myMeshing->setConnectivity(ConnectivityHexa, MED_EN::MED_CELL, MED_EN::MED_HEXA8);

  myMeshing->setNumberOfTypes(NumberOfFaceTypes, MED_EN::MED_FACE);
  myMeshing->setTypes(FaceTypes, MED_EN::MED_FACE);
  myMeshing->setNumberOfElements(NumberOfFaces, MED_EN::MED_FACE);
  
  myMeshing->setConnectivity(ConnectivityTria, MED_EN::MED_FACE, MED_EN::MED_TRIA3);
  myMeshing->setConnectivity(ConnectivityQua, MED_EN::MED_FACE, MED_EN::MED_QUAD4);

  // mesh dimension
  myMeshing->setMeshDimension(meshDimension);

  // edges connectivities
  // not yet implemented : if set, results are unpredictable.

  // groups of nodes
  {
    MEDMEM::GROUP myGroup;
    myGroup.setName("SomeNodes");
    myGroup.setMesh(myMeshing);
    myGroup.setEntity(MED_EN::MED_NODE);
    myGroup.setNumberOfGeometricType(1);
    MED_EN::medGeometryElement myTypes[1] = {MED_EN::MED_NONE};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[1] = {4};
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[1+1] = {1,5};
    const int value[4] = {1,4,5,7};
    myGroup.setNumber(index,value);
    myMeshing->addGroup(myGroup);
  }
  {
    MEDMEM::GROUP myGroup;
    myGroup.setName("OtherNodes");
    myGroup.setMesh(myMeshing);
    myGroup.setEntity(MED_EN::MED_NODE);
    myGroup.setNumberOfGeometricType(1);
    MED_EN::medGeometryElement myTypes[1] = {MED_EN::MED_NONE};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[1] = {3};
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[1+1] = {1,4};
    const int value[3] = {2,3,6};
    myGroup.setNumber(index,value);
    myMeshing->addGroup(myGroup);
  }

  // groups of cells
  {
    MEDMEM::GROUP myGroup;
    myGroup.setName("SomeCells");
    myGroup.setMesh(myMeshing);
    myGroup.setEntity(MED_EN::MED_CELL);
    myGroup.setNumberOfGeometricType(3);
    MED_EN::medGeometryElement myTypes[3] = {MED_EN::MED_TETRA4,MED_EN::MED_PYRA5,MED_EN::MED_HEXA8};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[3] = {4,1,2};
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[3+1] = {1,5,6,8};
    const int value[4+1+2] = {2,7,8,12,  13,  15,16};
    myGroup.setNumber(index,value);
    myMeshing->addGroup(myGroup);
  }
  {
    MEDMEM::GROUP myGroup;
    myGroup.setName("OtherCells");
    myGroup.setMesh(myMeshing);
    myGroup.setEntity(MED_EN::MED_CELL);
    myGroup.setNumberOfGeometricType(2);
    MED_EN::medGeometryElement myTypes[] = {MED_EN::MED_TETRA4,MED_EN::MED_PYRA5};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[] = {4,1};
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[2+1] = {1,5,6};
    const int value[4+1] = {3,4,5,9,  14};
    myGroup.setNumber(index,value);
    myMeshing->addGroup(myGroup);
  }

  // groups of faces
  {
    MEDMEM::GROUP myGroup;
    myGroup.setName("SomeFaces");
    myGroup.setMesh(myMeshing);
    myGroup.setEntity(MED_EN::MED_FACE);
    myGroup.setNumberOfGeometricType(2);
    MED_EN::medGeometryElement myTypes[2] = {MED_EN::MED_TRIA3,MED_EN::MED_QUAD4};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[2] = {2,3};
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[2+1] = {1,3,6};
    const int value[2+3] = {2,4,  5,6,8};
    myGroup.setNumber(index,value);
    myMeshing->addGroup(myGroup);
  }
  {
    MEDMEM::GROUP myGroup;
    myGroup.setName("OtherFaces");
    myGroup.setMesh(myMeshing);
    myGroup.setEntity(MED_EN::MED_FACE);
    myGroup.setNumberOfGeometricType(1);
    MED_EN::medGeometryElement myTypes[1] = {MED_EN::MED_TRIA3};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[1] = {2};
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[1+1] = {1,3};
    const int value[2] = {1,3};
    myGroup.setNumber(index,value);
    myMeshing->addGroup(myGroup);
  }

  return myMeshing;
}
