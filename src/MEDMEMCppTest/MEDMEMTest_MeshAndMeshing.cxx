// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
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
#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_Grid.hxx"

#include <sstream>
#include <cmath>

#include <cppunit/Message.h>
#include <cppunit/TestAssert.h>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

static void addMedFacesGroup (MESHING& meshing, int nFaces, const int *groupValue,
                              string groupName, const MED_EN::medGeometryElement *mytypes,
                              const int *index, const int *myNumberOfElements, int nbOfGeomTypes)
{
  GROUP *faces=new GROUP;
  faces->setName(groupName);
  faces->setMesh(&meshing);
  faces->setEntity(MED_EN::MED_FACE);
  faces->setNumberOfGeometricType(nbOfGeomTypes);
  faces->setGeometricType(mytypes);
  faces->setNumberOfElements(myNumberOfElements);
  faces->setNumber(index, groupValue);
  meshing.addGroup(*faces);
  faces->removeReference();
}

/*!
 *  Check methods (12), defined in MEDMEM_Meshing.hxx:
 *  class MESHING: public MESH 
{
 *   (+) MESHING();
 *   (+) ~MESHING();
 *   (+) void setCoordinates (const int SpaceDimension, const int NumberOfNodes,
 *                            const double * Coordinates,
 *                            const string System, const MED_EN::medModeSwitch Mode);
 *   (+) void setCoordinatesNames (const string * names);
 *   (+) void setCoordinateName (const string name, const int i);
 *   (+) void setCoordinatesUnits (const string * units);
 *   (+) void setCoordinateUnit (const string unit, const int i);
 *   (+) void setNumberOfTypes (const int NumberOfTypes,
 *                              const MED_EN::medEntityMesh Entity) throw (MEDEXCEPTION);
 *   (+) void setTypes (const MED_EN::medGeometryElement * Types,
 *                      const MED_EN::medEntityMesh Entity) throw (MEDEXCEPTION);
 *   (+) void setNumberOfElements (const int * NumberOfElements,
 *                                 const MED_EN::medEntityMesh Entity) throw (MEDEXCEPTION);
 *   (+) void setConnectivity (const int * Connectivity, const MED_EN::medEntityMesh Entity,
 *                             const MED_EN::medGeometryElement Type,
 *                             const int * PolyConnectivityIndex=0) throw (MEDEXCEPTION);
 *   (+) void addGroup (const GROUP & Group) throw (MEDEXCEPTION);
 *  
}
 */

/*!
 *  Check methods (67), defined in MEDMEM_Mesh.hxx:
 *  class MESH : public RCBASE 
{
 *   (+) void init();
 *   (+) MESH();
 *   (+) MESH(MESH &m);
 *   (+) MESH & operator=(const MESH &m);
 *   (+) virtual bool operator==(const MESH& other) const;
 *   (+) virtual bool deepCompare(const MESH& other) const;
 *   (+) MESH(driverTypes driverType, const string & fileName="",
 *                const string & meshName="") throw (MEDEXCEPTION);
 *   (+) virtual ~MESH();
 *   (+) friend ostream & operator<<(ostream &os, const MESH &my);
 *   (+) int  addDriver(driverTypes driverType,
 *                          const string & fileName="Default File Name.med",
 *                          const string & driverName="Default Mesh Name",
 *                          MED_EN::med_mode_acces access=MED_EN::MED_REMP);
 *   (+) int  addDriver(GENDRIVER & driver);
 *   (+) void rmDriver(int index=0);
 *   (+) virtual void read(int index=0);
 *   (+) inline void read(const GENDRIVER & genDriver);
 *   (+) inline void write(int index=0);
 *   (+) inline void write(const GENDRIVER & genDriver,
 *                         MED_EN::med_mode_acces medMode=MED_EN::WRONLY);
 *   (+) inline void write(driverTypes        driverType,
 *                         const std::string& filename,
 *                         const std::string& meshname="",
 *                         MED_EN::med_mode_acces medMode=MED_EN::WRONLY) const;
 *   (+) inline void setName(string name);
 *   (+) inline void setDescription(string description);
 *   (+) inline string getName() const;
 *   (+) inline string getDescription() const;
 *   (+) inline int getSpaceDimension() const;
 *   (+) inline int getMeshDimension() const;
 *   (+) inline bool getIsAGrid();
 *   (+) inline int getNumberOfNodes() const;
 *   (+) virtual inline const COORDINATE * getCoordinateptr() const;
 *   (+) inline string                     getCoordinatesSystem() const;
 *   (+) virtual inline const double *     getCoordinates(MED_EN::medModeSwitch Mode) const;
 *   (+) virtual inline const double       getCoordinate(int Number,int Axis) const;
 *   (+) inline const string *             getCoordinatesNames() const;
 *   (+) inline const string *             getCoordinatesUnits() const;
 *   (+) virtual inline int getNumberOfTypes(MED_EN::medEntityMesh Entity) const;
 *   (+) virtual inline const MED_EN::medGeometryElement * getTypes(MED_EN::medEntityMesh Entity) const;
 *   (+) virtual inline const CELLMODEL * getCellsTypes(MED_EN::medEntityMesh Entity) const;
 *   (+) virtual inline string * getCellTypeNames(MED_EN::medEntityMesh Entity) const;
 *   (+) virtual const int * getGlobalNumberingIndex(MED_EN::medEntityMesh Entity) const;
 *   (+) virtual inline int getNumberOfElements(MED_EN::medEntityMesh Entity,
 *                                        MED_EN::medGeometryElement Type) const;
 *   (+) virtual inline bool existConnectivity(MED_EN::medConnectivity ConnectivityType,
 *                                       MED_EN::medEntityMesh Entity) const;
 *   (+) virtual inline MED_EN::medGeometryElement getElementType
 *           (MED_EN::medEntityMesh Entity, int Number) const;
 *   (+) virtual inline void calculateConnectivity(MED_EN::medModeSwitch Mode,
 *                                            MED_EN::medConnectivity ConnectivityType,
 *                                            MED_EN::medEntityMesh Entity) const;
 *   (+) virtual inline int getConnectivityLength(MED_EN::medModeSwitch Mode,
 *                                             MED_EN::medConnectivity ConnectivityType,
 *                                             MED_EN::medEntityMesh Entity,
 *                                             MED_EN::medGeometryElement Type) const;
 *   (+) virtual inline const int * getConnectivity(MED_EN::medModeSwitch Mode,
 *                                             MED_EN::medConnectivity ConnectivityType,
 *                                             MED_EN::medEntityMesh Entity,
 *                                             MED_EN::medGeometryElement Type) const;
 *   (+) virtual inline const int * getConnectivityIndex
 *           (MED_EN::medConnectivity ConnectivityType, MED_EN::medEntityMesh Entity) const;
 *   (+) virtual int getElementNumber(MED_EN::medConnectivity ConnectivityType,
 *                                    MED_EN::medEntityMesh Entity, MED_EN::medGeometryElement Type,
 *                                    int * connectivity) const;
 *   (+) virtual inline int getReverseConnectivityLength
 *           (MED_EN::medConnectivity ConnectivityType, MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const;
 *   (+) virtual inline const int * getReverseConnectivity
 *           (MED_EN::medConnectivity ConnectivityType, MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const;
 *   (+) virtual inline int getReverseConnectivityIndexLength
 *           (MED_EN::medConnectivity ConnectivityType, MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const;
 *   (+) virtual inline const int * getReverseConnectivityIndex
 *           (MED_EN::medConnectivity ConnectivityType, MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const;
 *   (+) virtual int getNumberOfFamilies(MED_EN::medEntityMesh Entity) const;
 *   (+) virtual inline const vector<FAMILY*> getFamilies(MED_EN::medEntityMesh Entity) const;
 *   (+) virtual inline const FAMILY* getFamily(MED_EN::medEntityMesh Entity,int i) const;
 *   (+) virtual int getNumberOfGroups(MED_EN::medEntityMesh Entity) const;
 *   (+) virtual inline const vector<GROUP*> getGroups(MED_EN::medEntityMesh Entity) const;
 *   (+) virtual inline const GROUP* getGroup(MED_EN::medEntityMesh Entity,int i) const;
 *   (+) virtual inline const CONNECTIVITY* getConnectivityptr() const;
 *   (+) virtual SUPPORT * getBoundaryElements(MED_EN::medEntityMesh Entity) throw (MEDEXCEPTION);
 *   (+) SUPPORT * getSkin(const SUPPORT * Support3D) throw (MEDEXCEPTION);
 *   (+) virtual FIELD<double>* getVolume (const SUPPORT * Support) const throw (MEDEXCEPTION);
 *   (+) virtual FIELD<double>* getArea (const SUPPORT * Support) const throw (MEDEXCEPTION);
 *   (+) virtual FIELD<double>* getLength (const SUPPORT * Support) const throw (MEDEXCEPTION);
 *   (+) virtual FIELD<double>* getNormal (const SUPPORT * Support) const throw (MEDEXCEPTION);
 *   (+) virtual FIELD<double>* getBarycenter (const SUPPORT * Support) const throw (MEDEXCEPTION);
 *   (+) static SUPPORT * mergeSupports(const vector<SUPPORT *> Supports) throw (MEDEXCEPTION);
 *   (+) static SUPPORT * intersectSupports(const vector<SUPPORT *> Supports) throw (MEDEXCEPTION);
 *   (+) void createFamilies();
 *   (+) SUPPORT *buildSupportOnNodeFromElementList
 *            (const list<int>& listOfElt, MED_EN::medEntityMesh entity) const throw (MEDEXCEPTION);
 *   (do the same thing as buildSupportOnNodeFromElementList except that a SUPPORT is not created) void fillSupportOnNodeFromElementList
 *            (const list<int>& listOfElt, SUPPORT *supportToFill) const throw (MEDEXCEPTION);
 *   (+) SUPPORT *buildSupportOnElementsFromElementList
 *            (const list<int>& listOfElt, MED_EN::medEntityMesh entity) const throw (MEDEXCEPTION);
 *   (+) int getElementContainingPoint(const double *coord);
 *   (+) vector< vector<double> > getBoundingBox() const;
 *   (+) template<class T> static
 *       FIELD<T> * mergeFields(const vector< FIELD<T> * > & others, bool meshCompare=false);
 *   (Presently disconnected in C++) virtual void addReference() const;
 *   (Presently disconnected in C++) virtual void removeReference() const;
 *  
}
 */
void MEDMEMTest::testMeshAndMeshing()
{
  string filename = getResourceFile("pointe.med");
  string meshname = "maa1";
  string filenameout21        = makeTmpFile("myMeshWrite4_pointe21.med");
  string filename_profiles_wr = makeTmpFile("myMedProfilesFieldfile.med");
  string filenameout          = makeTmpFile("out.med");

  // To remove tmp files from disk
  MEDMEMTest_TmpFilesRemover aRemover;
  aRemover.Register(filenameout21);
  aRemover.Register(filename_profiles_wr);
  aRemover.Register(filenameout);

  ////////////
  // TEST 1 //
  ////////////

  MESH * myMesh= new MESH();
  myMesh->setName("FIRST_MESH");
  CPPUNIT_ASSERT(myMesh != NULL);

  //test operator <<
  {
    ostringstream out;
    CPPUNIT_ASSERT_NO_THROW(out << *myMesh << endl);
  }

  //test operator =
  MESH *myMesh1 =new MESH( *myMesh);

  //deepCompare
  bool isEqual = false;
  CPPUNIT_ASSERT_NO_THROW(isEqual = myMesh1->deepCompare(*myMesh));
  CPPUNIT_ASSERT(isEqual);
  myMesh1->removeReference();

  //ensure it imposible to compare meshes
  MESH *myMeshPointer =  myMesh;
  //test operator ==
  CPPUNIT_ASSERT(*myMeshPointer == *myMesh);

  myMesh->removeReference();

  //set a MESH object
  MESHING *myMeshing=new MESHING;
  myMeshing->setName("meshing");
  // define coordinates

  int SpaceDimension = 3;
  int NumberOfNodes = 19;
  double Coordinates[57] = 
    {
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
      myMeshing->setCoordinates(SpaceDimension,NumberOfNodes,Coordinates,"CARTESIAN",MED_FULL_INTERLACE);
    }
  catch (const std::exception &e)
    {
      CPPUNIT_FAIL(e.what());
    }
  catch (...)
    {
      CPPUNIT_FAIL("Unknown exception");
    }

  string Names[3] = 
    {
      "X","Y","Z"
    };
  try
    {
      myMeshing->setCoordinatesNames(Names);
    }
  catch (const std::exception &e)
    {
      CPPUNIT_FAIL(e.what());
    }
  catch (...)
    {
      CPPUNIT_FAIL("Unknown exception");
    }

  string Units[3] = 
    {
      "cm","cm","cm"
    };
  try
    {
      myMeshing->setCoordinatesUnits(Units);
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

  const int NumberOfTypes = 3;
  medGeometryElement Types[NumberOfTypes] = 
    {
      MED_TETRA4,MED_PYRA5,MED_HEXA8
    };
  const int NumberOfElements[NumberOfTypes] = 
    {
      12,2,2
    };

  CPPUNIT_ASSERT_NO_THROW(myMeshing->setNumberOfTypes(NumberOfTypes,MED_CELL));

  CPPUNIT_ASSERT_NO_THROW(myMeshing->setTypes(Types,MED_CELL));

  CPPUNIT_ASSERT_NO_THROW(myMeshing->setNumberOfElements(NumberOfElements,MED_CELL));

  const int sizeTetra = 12*4;
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

  CPPUNIT_ASSERT_NO_THROW(myMeshing->setConnectivity(MED_CELL,MED_TETRA4,ConnectivityTetra));

  int ConnectivityPyra[2*5]=
    {
      7,8,9,10,2,
      15,18,17,16,19
    };

  CPPUNIT_ASSERT_NO_THROW(myMeshing->setConnectivity(MED_CELL,MED_PYRA5,ConnectivityPyra));

  int ConnectivityHexa[2*8]=
    {
      11,12,13,14,7,8,9,10,
      15,16,17,18,11,12,13,14
    };

  CPPUNIT_ASSERT_NO_THROW(myMeshing->setConnectivity(MED_CELL,MED_HEXA8,ConnectivityHexa));

  // face part
  const int NumberOfFacesTypes = 2;
  medGeometryElement FacesTypes[NumberOfFacesTypes] = 
    {
      MED_TRIA3,MED_QUAD4
    };
  const int NumberOfFacesElements[NumberOfFacesTypes] = 
    {
      4,4
    };

  CPPUNIT_ASSERT_NO_THROW(myMeshing->setNumberOfTypes(NumberOfFacesTypes,MED_FACE));
  CPPUNIT_ASSERT_NO_THROW(myMeshing->setTypes(FacesTypes,MED_FACE));
  CPPUNIT_ASSERT_NO_THROW(myMeshing->setNumberOfElements(NumberOfFacesElements,MED_FACE));
  const int nbTria = 4;
  const int sizeTria = nbTria*3;
  int ConnectivityTria[sizeTria]=
    {
      1,4,3,
      1,5,4,
      1,6,5,
      1,3,6
    };

  CPPUNIT_ASSERT_NO_THROW(myMeshing->setConnectivity(MED_FACE,MED_TRIA3,ConnectivityTria));
  const int nbQua = 4;
  int ConnectivityQua[nbQua*4]=
    {
      7,8,9,10,
      11,12,13,14,
      11,7,8,12,
      12,8,9,13
    };

  CPPUNIT_ASSERT_NO_THROW(myMeshing->setConnectivity(MED_FACE,MED_QUAD4,ConnectivityQua));

  int meshDimension = SpaceDimension; // because there 3D cells in the mesh
  //CPPUNIT_ASSERT_NO_THROW(myMeshing->setMeshDimension(meshDimension));

  // edge part

  // not yet implemented : if set, results are unpredictable.

  // Some groups :

  // Node :
  {
    GROUP *myGroup=new GROUP;
    myGroup->setName("SomeNodes");
    myGroup->setMesh(myMeshing);
    myGroup->setEntity(MED_NODE);
    myGroup->setNumberOfGeometricType(1);
    medGeometryElement myTypes[1] = 
      {
        MED_NONE
      };
    myGroup->setGeometricType(myTypes);
    const int myNumberOfElements[1] = 
      {
        4
      };
    myGroup->setNumberOfElements(myNumberOfElements);
    const int index[1+1] = 
      {
        1,5
      };
    const int value[4]= 
      {
        1,4,5,7
      };
    myGroup->setNumber(index,value);
    CPPUNIT_ASSERT_NO_THROW(myMeshing->addGroup(*myGroup));
    myGroup->removeReference();
  }
  {
    GROUP *myGroup=new GROUP;
    myGroup->setName("OtherNodes");
    myGroup->setMesh(myMeshing);
    myGroup->setEntity(MED_NODE);
    myGroup->setNumberOfGeometricType(1);
    medGeometryElement myTypes[1] = 
      {
        MED_NONE
      };
    myGroup->setGeometricType(myTypes);
    const int myNumberOfElements[1] = 
      {
        3
      };
    myGroup->setNumberOfElements(myNumberOfElements);
    const int index[1+1] = 
      {
        1,4
      };
    const int value[3]= 
      {
        2,3,6
      };
    myGroup->setNumber(index,value);
    CPPUNIT_ASSERT_NO_THROW(myMeshing->addGroup(*myGroup));
    myGroup->removeReference();
  }

  // Cell :
  {
    GROUP *myGroup=new GROUP;
    myGroup->setName("SomeCells");
    myGroup->setMesh(myMeshing);
    myGroup->setEntity(MED_CELL);
    myGroup->setNumberOfGeometricType(3);
    medGeometryElement myTypes[3] = 
      {
        MED_TETRA4,MED_PYRA5,MED_HEXA8
      };
    myGroup->setGeometricType(myTypes);
    const int myNumberOfElements[3] = 
      {
        4,1,2
      };
    myGroup->setNumberOfElements(myNumberOfElements);
    const int index[3+1] = 
      {
        1,5,6,8
      };
    const int value[4+1+2]=
      {
        2,7,8,12,
        13,
        15,16
      };
    myGroup->setNumber(index,value);
    CPPUNIT_ASSERT_NO_THROW(myMeshing->addGroup(*myGroup));
    myGroup->removeReference();
  }
  {
    GROUP *myGroup=new GROUP;
    myGroup->setName("OtherCells");
    myGroup->setMesh(myMeshing);
    myGroup->setEntity(MED_CELL);
    myGroup->setNumberOfGeometricType(2);
    medGeometryElement myTypes[] = 
      {
        MED_TETRA4,MED_PYRA5
      };
    myGroup->setGeometricType(myTypes);
    const int myNumberOfElements[] = 
      {
        4,1
      };
    myGroup->setNumberOfElements(myNumberOfElements);
    const int index[2+1] = 
      {
        1,5,6
      };
    const int value[4+1]=
      {
        3,4,5,9,
        14
      };
    myGroup->setNumber(index,value);
    CPPUNIT_ASSERT_NO_THROW(myMeshing->addGroup(*myGroup));
    myGroup->removeReference();
  }

  // Face :
  {
    GROUP *myGroup=new GROUP;
    myGroup->setName("SomeFaces");
    myGroup->setMesh(myMeshing);
    myGroup->setEntity(MED_FACE);
    myGroup->setNumberOfGeometricType(2);
    medGeometryElement myTypes[2] = 
      {
        MED_TRIA3,MED_QUAD4
      };
    myGroup->setGeometricType(myTypes);
    const int myNumberOfElements[2] = 
      {
        2,3
      };
    myGroup->setNumberOfElements(myNumberOfElements);
    const int index[2+1] = 
      {
        1,3,6
      };
    const int value[2+3]=
      {
        2,4,
        5,6,8
      };
    myGroup->setNumber(index,value);
    CPPUNIT_ASSERT_NO_THROW(myMeshing->addGroup(*myGroup));
    myGroup->removeReference();
  }
  {
    GROUP *myGroup=new GROUP;
    myGroup->setName("OtherFaces");
    myGroup->setMesh(myMeshing);
    myGroup->setEntity(MED_FACE);
    myGroup->setNumberOfGeometricType(1);
    medGeometryElement myTypes[1] = 
      {
        MED_TRIA3
      };
    myGroup->setGeometricType(myTypes);
    const int myNumberOfElements[1] = 
      {
        2
      };
    myGroup->setNumberOfElements(myNumberOfElements);
    const int index[1+1] = 
      {
        1,3
      };
    const int value[2]=
      {
        1,3
      };
    myGroup->setNumber(index,value);
    CPPUNIT_ASSERT_NO_THROW(myMeshing->addGroup(*myGroup));
    myGroup->removeReference();
  }

  //test Mesh(MESH &m)
  {
    MESH * myMesh2 = new MESH( *myMeshing );
    CPPUNIT_ASSERT(myMesh2->deepCompare(*myMeshing));
    myMeshing->removeReference();
    //cout<<*myMesh2<<endl;
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
      }

    const string * units;
    try
      {
        units = myMesh2->getCoordinatesUnits();
        for (int axe = 0; axe < SpaceDimension; axe++) 
          {
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
        for (int axe = 0; axe < SpaceDimension; axe++) 
          {
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

        for (int axe = 0; axe < SpaceDimension; axe++) 
          {
            try
              {
                for (int num = 0; num < NumberOfNodes; num++) 
                  {
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
    if (myMesh2->existConnectivity(MED_NODAL, entity))
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
            ostringstream out;
            const int * connectivity;
            const int * connectivity_index;
            CPPUNIT_ASSERT_NO_THROW(connectivity = myMesh2->getConnectivity (myMedConnect, entity, Types1[t]));
            connectivity_index = myMesh2->getConnectivityIndex(myMedConnect, entity);
            for (int j = 0; j < NumberOfElements1; j++) 
              {
                out<<"!!!!!!!!!!!!!!!"<<endl;
                for (int k = connectivity_index[j]; k < connectivity_index[j+1]; k++)
                  out << connectivity[k-1] << " ";
                out << endl;
              }
          }

        const CELLMODEL* myCellModel = myMesh2->getCellsTypes(entity);
        string* TypeNames;
        CPPUNIT_ASSERT_NO_THROW(TypeNames = myMesh2->getCellTypeNames(entity));

        for(int k = 0; k < NumberOfTypes; k++ )
          {
            CPPUNIT_ASSERT_EQUAL(TypeNames[k], myCellModel[k].getName());
          }
        delete [] TypeNames;

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
            //cout<<"Global number of first element of each geom type : "<<myGlobalNbIdx[i]<<endl;
          }
        //cout<<"Global number of first element of each geom type : "<<myGlobalNbIdx[i]<<endl;
      }
    {
      const int * ReverseNodalConnectivity;

      ((CONNECTIVITY*)myConnectivity)->setNumberOfNodes(NumberOfNodes);

      CPPUNIT_ASSERT_NO_THROW(ReverseNodalConnectivity = myMesh2->getReverseConnectivity(MED_NODAL, entity));
      CPPUNIT_ASSERT_NO_THROW(myMesh2->getReverseConnectivityLength(MED_NODAL, entity));
      const int * ReverseNodalConnectivityIndex = myMesh2->getReverseConnectivityIndex(MED_NODAL, entity);
      const int ReverseIdxLength = myMesh2->getReverseConnectivityIndexLength(MED_NODAL, entity);
      CPPUNIT_ASSERT(ReverseIdxLength == NumberOfNodes+1);
      ostringstream out;
      for (int i = 0; i < NumberOfNodes; i++) 
        {
          out << "Node "<< i+1 << " : ";
          for (int j = ReverseNodalConnectivityIndex[i]; j < ReverseNodalConnectivityIndex[i+1]; j++)
            out << ReverseNodalConnectivity[j-1] << " ";
          out << endl;
        }

      // Show Descending Connectivity
      int NumberOfElements1;
      const int * connectivity;
      const int * connectivity_index;
      myMesh2->calculateConnectivity( MED_DESCENDING, entity);
      try
        {
          NumberOfElements1 = myMesh2->getNumberOfElements(entity, MED_ALL_ELEMENTS);
          connectivity = myMesh2->getConnectivity( MED_DESCENDING, entity, MED_ALL_ELEMENTS);
          connectivity_index = myMesh2->getConnectivityIndex(MED_DESCENDING, entity);
        }
      catch (MEDEXCEPTION m) 
        {
          CPPUNIT_FAIL(m.what());
        }

      for (int j = 0; j < NumberOfElements1; j++) 
        {
          out << "Element " << j+1 << " : ";
          for (int k = connectivity_index[j]; k < connectivity_index[j+1]; k++)
            out << connectivity[k-1] << " ";
          out << endl;
        }

      // getElementNumber
      if (myMesh2->existConnectivity(MED_NODAL, MED_FACE)) 
        {
          int myTr[3] = 
            {
              1,5,4
            };
          CPPUNIT_ASSERT_NO_THROW(myMesh2->getElementNumber(MED_NODAL,MED_FACE,MED_TRIA3,myTr));
        }
    }

    //test family and group
    int NumberOfGroups;
    CPPUNIT_ASSERT_THROW(myMesh2->getNumberOfGroups(MED_ALL_ENTITIES), MEDEXCEPTION);
    CPPUNIT_ASSERT_NO_THROW(NumberOfGroups = myMesh2->getNumberOfGroups(MED_CELL));
    CPPUNIT_ASSERT_EQUAL(NumberOfGroups, 2);
    vector<GROUP*> groups;
    CPPUNIT_ASSERT_NO_THROW(groups = myMesh2->getGroups(MED_CELL));
    CPPUNIT_ASSERT(groups.size() != 0);
    for(int nb = 1; nb <= NumberOfGroups; nb++ )
      {
        const GROUP* group;
        CPPUNIT_ASSERT_NO_THROW(group = myMesh2->getGroup(MED_CELL, nb));
        CPPUNIT_ASSERT_EQUAL(group->getName(), groups[nb-1]->getName());
      }

    int NumberOfFamilies;
    CPPUNIT_ASSERT_NO_THROW(NumberOfFamilies = myMesh2->getNumberOfFamilies(MED_CELL));
    CPPUNIT_ASSERT_MESSAGE("Current mesh hasn't Families", NumberOfFamilies == 0);

    //create families - it's not possible to create, becase not all entities are defined
    // EAP: the problem has been fixed for ENSIGHT Industrialization project
    CPPUNIT_ASSERT_NO_THROW( myMesh2->createFamilies() );

    /*CPPUNIT_ASSERT_NO_THROW(NumberOfFamilies = myMesh2->getNumberOfFamilies(MED_CELL));
      CPPUNIT_ASSERT( NumberOfFamilies != 0);*/

    myMesh2->removeReference();
  }

  //////////////////////////////////////////////////////////////
  // TEST 2: Polygon and Polyhedron(only NODAL connectivity)  //
  /////////////////////////////////////////////////////////////

  double CoordinatesPoly[57] = 
    {
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
      5.0, 1.0, -3.0
    };

  const int REFnodalConnOfFaces[91] = 
    {
      1, 2, 3, 4, 5, 6, -1,// Polyhedron 1
      1, 7, 8, 2,       -1,
      2, 8, 9, 3,       -1,
      4, 3, 9, 10,      -1,
      5, 4, 10, 11,     -1,
      6, 5, 11, 12,     -1,
      1, 6, 12, 7,      -1,
      7, 12, 8, 10,     -1,
      9, 8, 12, 11,     

      13, 14, 15, 3, 2, -1,// Polyhedron 2
      13, 2, 8, 16,     -1,
      14, 13, 16, 17,   -1,
      15, 14, 17, 15,   -1,
      17, 18, 15,       -1,
      18, 9, 3,         -1,
      15, 9, 2,         -1,
      3, 9, 8,          -1,
      8, 9, 17, 16,     -1,
      9, 18, 17
    };
  const int NumberOfFaces = 19;
  const int NumberOfPolyhedron = 2;
  const int nbOfPolygons = 2;
  const int REFpolyIndex[NumberOfPolyhedron+1] = 
    {
      1,47,92
    };

  double PolygonCoordinates[27] = 
    {
      2.0, 3.0, 12.0,
      3.0, 2.0, 12.0,
      4.0, 1.0, 12.0,
      2.0, 0.0, 12.0,
      0.0, 1.0, 12.0,
      1.0, 2.0, 12.0,
      5.0, 3.0, 12.0,
      7.0, 2.0, 12.0,
      6.0, 0.0, 12.0
    };

  const int REFpolygonFaces[11] = 
    {
      1, 2, 3, 4, 5, 6, // Polygon 1
      7, 8, 9, 3, 2     // Polygon 2
    };

  const int REFpolygonIndex[nbOfPolygons+1] = 
    {
      1, 7, 12
    };

  MESHING *myMeshingPoly=new MESHING;
  myMeshingPoly->setName("meshingpoly");

  const int NbOfTypes = 2;
  medGeometryElement TypesPoly[NbOfTypes] = 
    {
      MED_TETRA4, MED_POLYHEDRA
    };
  const int NbOfElements[NbOfTypes] = 
    {
      1,2
    };

  CPPUNIT_ASSERT_NO_THROW(myMeshingPoly->setNumberOfTypes(NbOfTypes, MED_CELL));

  CPPUNIT_ASSERT_NO_THROW
    (myMeshingPoly->setCoordinates(SpaceDimension, NumberOfNodes, CoordinatesPoly,
                                   "CARTESIAN", MED_FULL_INTERLACE));

  //CPPUNIT_ASSERT_NO_THROW( myMeshingPoly->setSpaceDimension(SpaceDimension));

  CPPUNIT_ASSERT_NO_THROW(myMeshingPoly->setTypes(TypesPoly, MED_CELL));
  CPPUNIT_ASSERT_NO_THROW(myMeshingPoly->setNumberOfElements(NbOfElements, MED_CELL));

  string Unit ="cm";
  for(int i = 0; i < SpaceDimension; i++ )
    {
      CPPUNIT_ASSERT_NO_THROW( myMeshingPoly->setCoordinateName(Names[i],i) );
      CPPUNIT_ASSERT_NO_THROW( myMeshingPoly->setCoordinateUnit(Unit, i) );
    }

  int ConnectivityTetraPoly[4*1]=
    {
      17, 9, 18, 19
    };

  CPPUNIT_ASSERT_NO_THROW(myMeshingPoly->setConnectivity(MED_CELL, MED_TETRA4,ConnectivityTetraPoly));

  CPPUNIT_ASSERT_NO_THROW(myMeshingPoly->setConnectivity(MED_CELL, MED_POLYHEDRA,REFnodalConnOfFaces,REFpolyIndex));

  bool PolyConn = false;
  CPPUNIT_ASSERT_NO_THROW(PolyConn = myMeshingPoly->existConnectivity(MED_NODAL, MED_CELL));
  CPPUNIT_ASSERT(PolyConn);
  {
    CPPUNIT_ASSERT_EQUAL(NumberOfPolyhedron,
                         myMeshingPoly->getNumberOfElements(MED_CELL,MED_POLYHEDRA));
    CPPUNIT_ASSERT_NO_THROW( myMeshingPoly->calculateConnectivity (MED_NODAL,MED_FACE));
    CPPUNIT_ASSERT_EQUAL(NumberOfFaces-1, myMeshingPoly->getNumberOfElements(MED_FACE,MED_POLYGON)); // -1: one face is shared with tetra
    CPPUNIT_ASSERT_EQUAL(91,myMeshingPoly->getConnectivityLength(MED_NODAL,MED_CELL,MED_POLYHEDRA));
    const int * PolyConn;
    const int * PolyIdx;
    CPPUNIT_ASSERT_NO_THROW(PolyConn = myMeshingPoly->getConnectivity(MED_NODAL,MED_CELL,MED_ALL_ELEMENTS));
    CPPUNIT_ASSERT_NO_THROW(PolyIdx = myMeshingPoly->getConnectivityIndex(MED_NODAL,MED_CELL));
    for(int i = NbOfElements[0], iRef=0; i<NbOfElements[0]+NumberOfPolyhedron; i++)
      {
        int NodeIdxBegin = PolyIdx[i];
        int NodeIdxEnd = PolyIdx[i+1];
        for(int k = NodeIdxBegin; k < NodeIdxEnd; k++)
          CPPUNIT_ASSERT_EQUAL(REFnodalConnOfFaces[iRef++], PolyConn[k-1]);
      }
  }

    MESHING *myPolygonMeshing=new MESHING;
    myPolygonMeshing->setName("PolygonMeshing");

    medGeometryElement PolygonTypes[NbOfTypes] = 
      {
        MED_TRIA3,MED_POLYGON
      };
    const int PolygonNumberOfElements[NbOfTypes] = 
      {
        2,nbOfPolygons
      };

    CPPUNIT_ASSERT_NO_THROW(myPolygonMeshing->setNumberOfTypes(NbOfTypes, MED_CELL));

    try
    {
      myPolygonMeshing->setCoordinates(SpaceDimension, NumberOfNodes, PolygonCoordinates,
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

    CPPUNIT_ASSERT_EQUAL(SpaceDimension, myPolygonMeshing->getSpaceDimension());
    CPPUNIT_ASSERT_EQUAL(NumberOfNodes, myPolygonMeshing->getNumberOfNodes());

    CPPUNIT_ASSERT_NO_THROW(myPolygonMeshing->setTypes(PolygonTypes, MED_CELL));
    CPPUNIT_ASSERT_NO_THROW(myPolygonMeshing->setNumberOfElements(PolygonNumberOfElements, MED_CELL));
    CPPUNIT_ASSERT_EQUAL(2, myPolygonMeshing->getMeshDimension());

    try
    {
      myPolygonMeshing->setCoordinatesNames(Names);
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
      myPolygonMeshing->setCoordinatesUnits(Units);
    }
    catch (const std::exception &e)
    {
      CPPUNIT_FAIL(e.what());
    }
    catch (...)
    {
      CPPUNIT_FAIL("Unknown exception");
    }

    const int sizeTri = 3*2;
    int ConnectivityTri[sizeTri]=
      {
        1, 7, 2, 3, 9, 4
      };

    CPPUNIT_ASSERT_NO_THROW(myPolygonMeshing->setConnectivity(MED_CELL, MED_TRIA3,ConnectivityTri));
    CPPUNIT_ASSERT_NO_THROW(myPolygonMeshing->setConnectivity(MED_CELL, MED_POLYGON,REFpolygonFaces, REFpolygonIndex));

    bool PolygonConn = false;
    CPPUNIT_ASSERT_NO_THROW(PolygonConn = myPolygonMeshing->existConnectivity(MED_NODAL, MED_CELL));
    if(PolygonConn)
    {
      int Polytypes;
      CPPUNIT_ASSERT_NO_THROW(Polytypes = myPolygonMeshing->getNumberOfTypes(MED_CELL));
      CPPUNIT_ASSERT_EQUAL(NbOfTypes,Polytypes);

      const MED_EN::medGeometryElement * PolyTypes;
      CPPUNIT_ASSERT_NO_THROW(PolyTypes = myPolygonMeshing->getTypes(MED_CELL));
      CPPUNIT_ASSERT_EQUAL(PolyTypes[NbOfTypes-1],MED_POLYGON);

      for(int t = 0; t < Polytypes; t++)
      {
        CPPUNIT_ASSERT_NO_THROW( myPolygonMeshing->getNumberOfElements(MED_CELL, PolyTypes[t]));
      }
      medGeometryElement geomPolyElem;
      CPPUNIT_ASSERT_NO_THROW(geomPolyElem = myPolygonMeshing->getElementType(MED_CELL, 1));
      CPPUNIT_ASSERT_EQUAL(MED_TRIA3,geomPolyElem);

      CPPUNIT_ASSERT_EQUAL(myPolygonMeshing->getNumberOfElements(MED_CELL,MED_POLYGON),nbOfPolygons);
      CPPUNIT_ASSERT_NO_THROW(myPolygonMeshing->getConnectivityLength(MED_NODAL,MED_CELL,MED_POLYGON));
      myPolygonMeshing->removeReference();
      const int * PolygonConn;
      CPPUNIT_ASSERT_THROW(PolygonConn = myMeshingPoly->getConnectivity(MED_NODAL,MED_CELL,MED_POLYGON),MEDEXCEPTION);
    }
    myMeshingPoly->removeReference();
    ////////////////////////////////////////////////////////////
    // TEST : SUPPORT* sup = myMeshPointe->getSupportOnAll()) //
    ////////////////////////////////////////////////////////////
    {
      MESH * myMeshPointe = new MESH(MED_DRIVER, filename, meshname);
      const SUPPORT* sup = myMeshPointe->getSupportOnAll(MED_CELL);
      CPPUNIT_ASSERT( sup->isOnAllElements() );
      CPPUNIT_ASSERT_EQUAL( myMeshPointe->getNumberOfTypes( sup->getEntity() ),
                            sup->getNumberOfTypes());
      CPPUNIT_ASSERT( sup->getNumber( MED_ALL_ELEMENTS ));
      myMeshPointe->removeReference();
    }

    ////////////////////////////////////////////////////////
    // TEST 3: test MESH on  MEDMEMTest::createTestMesh()//
    ///////////////////////////////////////////////////////

    MESH* myMesh3 = MEDMEMTest_createTestMesh();

    int MeshDim  = myMesh3->getMeshDimension();
    medEntityMesh constituentEntity;
    if (MeshDim==3) 
    {
      constituentEntity = MED_CELL;
    }
    if (MeshDim==2) 
    {
      constituentEntity = MED_FACE;
    }
    if (MeshDim==1) 
    {
      constituentEntity = MED_EDGE;
    }

    int SpaceDim = myMesh3->getSpaceDimension();

    // Show Reverse Nodal Connectivity
    const int* ReverseNodalConnectivity;
    const int* ReverseNodalConnectivityIndex;
    int ReverseLength;
    int ReverseIdxLength;

    CONNECTIVITY* myConnectivity3 = (CONNECTIVITY*)myMesh3->getConnectivityptr();
    myConnectivity3->setNumberOfNodes(myMesh3->getNumberOfNodes());

    CPPUNIT_ASSERT_NO_THROW(ReverseNodalConnectivity= myMesh3->getReverseConnectivity(MED_NODAL, MED_CELL));
    CPPUNIT_ASSERT_NO_THROW(ReverseLength = myMesh3->getReverseConnectivityLength(MED_NODAL, MED_CELL));
    CPPUNIT_ASSERT_NO_THROW(ReverseNodalConnectivityIndex = myMesh3->getReverseConnectivityIndex(MED_NODAL, MED_CELL));
    CPPUNIT_ASSERT_NO_THROW(ReverseIdxLength = myMesh3->getReverseConnectivityIndexLength(MED_NODAL, MED_CELL));
    CPPUNIT_ASSERT(ReverseIdxLength == myMesh3->getNumberOfNodes()+1);

    ostringstream out;
    for (int i = 0; i < myMesh3->getNumberOfNodes(); i++) 
    {
      out << "Node "<< i+1 << " : ";
      for (int j = ReverseNodalConnectivityIndex[i]; j < ReverseNodalConnectivityIndex[i+1]; j++)
        out << ReverseNodalConnectivity[j-1] << " ";
      out << endl;
    }

    // Show Descending Connectivity
    int NumberOfElements1;
    const int * connectivity;
    const int * connectivity_index;
    myMesh3->calculateConnectivity(MED_DESCENDING, MED_EN::MED_CELL);
    try
    {
      NumberOfElements1 = myMesh3->getNumberOfElements(MED_CELL, MED_ALL_ELEMENTS);
      connectivity = myMesh3->getConnectivity( MED_DESCENDING, MED_CELL, MED_ALL_ELEMENTS);
      connectivity_index = myMesh3->getConnectivityIndex(MED_DESCENDING, MED_CELL);
    }
    catch (MEDEXCEPTION m) 
    {
      CPPUNIT_FAIL(m.what());
    }

    for (int j = 0; j < NumberOfElements1; j++) 
    {
      out << "Element " << j+1 << " : ";
      for (int k = connectivity_index[j]; k < connectivity_index[j+1]; k++)
        out << connectivity[k-1] << " ";
      out << endl;
    }

    //test 3D mesh
    for(int ind = SpaceDim; ind > 1; ind-- )
    {
      int NumberOfElem = myMesh3->getNumberOfElements (constituentEntity,MED_ALL_ELEMENTS);
      if(NumberOfElem < 1) continue;

      const SUPPORT * sup = myMesh3->getSupportOnAll( constituentEntity );

      if (ind == 2)
      {
        // test of normal(for 1d or 2d elements)
        FIELD<double>* normal;
        CPPUNIT_ASSERT_NO_THROW(normal = myMesh3->getNormal(sup));

        const int nbNormVals = 47*3;
        double refNormals[nbNormVals] = 
          {
            0, 1, 0    ,// #1 
            -1, 0, 0   ,// #2 
            -0, 0, 2   ,// #3 
            1, -1, -2  ,// #4 
            -1, 0, 0   ,// #5 
            0, 0, 2    ,// #6 
            1, 1, -2   ,// #7 
            0, -1, 0   ,// #8 
            0, -0, 2   ,// #9 
            -1, 1, -2  , // #10
            0, 0, 2    , // #11
            -1, -1, -2 , // #12
            -1, 0, 1   , // #13
            0, -1, 1   , // #14
            1, 1, 0    , // #15
            0, -1, 1   , // #16
            1, -0, 1   , // #17
            -1, 1, 0   , // #18
            1, 0, 1    , // #19
            0, 1, 1    , // #20
            -1, -1, 0  , // #21
            -0, 1, 1   , // #22
            -1, 0, 1   , // #23
            1, -1, 0   , // #24
            -1, 0, 1   , // #25
            1, 0, 1    , // #26
            0, -1, 1   , // #27
            -0, 1, 1   , // #28
            1, 0, 1    , // #29
            -1, 0, 1   , // #30
            0, 1, 1    , // #31
            0, -1, 1   , // #32
            1, 0, 1    , // #33
            0, -1, 1   , // #34
            -1, 0, 1   , // #35
            -0, 1, 1   , // #36
            0, 0, 4    , // #37
            -0, -0, -4 , // #38
            0, 0, 4    , // #39
            0, 2, 0    , // #40
            -2, -0, -0 , // #41
            0, -2, 0   , // #42
            2, -0, 0   , // #43
            0, 2, 0    , // #44
            -2, -0, -0 , // #45
            0, -2, 0   , // #46
            2, -0, 0     // #47
          };
        for ( int i = 0; i < nbNormVals; ++i )
          CPPUNIT_ASSERT_DOUBLES_EQUAL( refNormals[i], normal->getValue()[i], 1e-6);
        //       double normal_square, norm;
        //       double maxnorm=0.;
        //       double minnorm=0.;
        //       double tmp_value;
        //       for (int i = 1; i<=NumberOfElem; i++) {
        //         normal_square = 0.;
        //         cout << "Normal " << i << " ";
        //         for (int j=1; j<=SpaceDim; j++) {
        //           tmp_value = normal->getValueIJ(i,j);
        //           normal_square += tmp_value*tmp_value;
        //           cout << tmp_value << " ";
        //         }
        //         norm = sqrt(normal_square);
        //         maxnorm = dmax(maxnorm,norm);
        //         minnorm = dmin(minnorm,norm);
        //         cout << ", Norm = " << norm << endl;
        //       }
        //       cout << "Max Norm " << maxnorm << " Min Norm " << minnorm << endl;
        delete normal;

        // test of area(for 2d elements)
        FIELD<double>* area;
        CPPUNIT_ASSERT_NO_THROW(area = myMesh3->getArea(sup));

        //       double maxarea,minarea,areatot;
        //       maxarea = 0.;
        //       minarea = 0.;
        //       areatot = 0.0;
        //       for (int i = 1; i<=NumberOfElem;i++)
        //       {
        //         cout << "Area " << i << " " << area->getValueIJ(i,1) << endl;
        //         maxarea = dmax(maxarea,area->getValueIJ(i,1));
        //         minarea = dmin(minarea,area->getValueIJ(i,1));
        //         areatot = areatot + area->getValueIJ(i,1);
        //       }

        //       cout << "Max Area " << maxarea << " Min Area " << minarea << endl;
        //       cout << "Support Area " << areatot << endl;
        const int nbAreas = 47;
        double refArea [nbAreas] = 
          {
            1, 1, 2, 2.44949, 1, 2, 2.44949, 1, 2,  2.44949,  2,  2.44949,  1.41421,  1.41421,  1.41421,  1.41421,  1.41421,  1.41421,  1.41421,  1.41421,  1.41421,  1.41421,  1.41421,  1.41421,  1.41421,  1.41421,  1.41421,  1.41421,  1.41421,  1.41421,  1.41421,  1.41421,  1.41421,  1.41421,  1.41421,  1.41421,  4,  4,  4,  2,  2,  2,  2,  2,  2,  2,  2 
          };
        for ( int i = 0; i < nbAreas; ++i )
          CPPUNIT_ASSERT_DOUBLES_EQUAL( refArea[i], area->getValue()[i], 1e-5);
        delete area;
      }

      // test of barycenter(for 3d and 2d elements)
      FIELD<double>* barycenter;
      CPPUNIT_ASSERT_NO_THROW(barycenter = myMesh3->getBarycenter(sup));

      CPPUNIT_ASSERT_NO_THROW(NumberOfElem = myMesh3->getNumberOfElements(constituentEntity,MED_ALL_ELEMENTS));
      if ( ind == 3 )
      {
        double refBC[16*3] = 
          {
            0.5, -0.5, 0.75    ,// #1 
            0.5, 0.5, 0.75     ,// #2 
            -0.5, 0.5, 0.75    ,// #3 
            -0.5, -0.5, 0.75   ,// #4 
            0.75, 0.75, 1.25   ,// #5 
            -0.75, 0.75, 1.25  ,// #6 
            -0.75, -0.75, 1.25 ,// #7 
            0.75, -0.75, 1.25  ,// #8 
            1, 0, 1.5          ,// #9 
            0, 1, 1.5          , // #10
            -1, 0, 1.5         , // #11
            0, -1, 1.5         , // #12
            0, 0, 1.8          , // #13
            0, 0, 4.2          , // #14
            0, 0, 2.5          , // #15
            0, 0, 3.5          , // #16
          };
        for ( int i = 0; i < 16*3; ++i )
          CPPUNIT_ASSERT_DOUBLES_EQUAL( refBC[i], barycenter->getValue()[i], 1e-6);
      }
      if ( ind == 2 )
      {
        double refBC[47*3] = 
          {
            0.666667, 0, 0.666667          ,// #1 
            0, -0.666667, 0.666667         ,// #2 
            0.666667, -0.666667, 1         ,// #3 
            0.666667, -0.666667, 0.666667  ,// #4 
            0, 0.666667, 0.666667          ,// #5 
            0.666667, 0.666667, 1          ,// #6 
            0.666667, 0.666667, 0.666667   ,// #7 
            -0.666667, 0, 0.666667         ,// #8 
            -0.666667, 0.666667, 1         ,// #9 
            -0.666667, 0.666667, 0.666667 , // #10
            -0.666667, -0.666667, 1       , // #11
            -0.666667, -0.666667, 0.666667, // #12
            0.333333, 1, 1.33333          , // #13
            1, 0.333333, 1.33333          , // #14
            1, 1, 1.33333                 , // #15
            -1, 0.333333, 1.33333         , // #16
            -0.333333, 1, 1.33333         , // #17
            -1, 1, 1.33333                , // #18
            -0.333333, -1, 1.33333        , // #19
            -1, -0.333333, 1.33333        , // #20
            -1, -1, 1.33333               , // #21
            1, -0.333333, 1.33333         , // #22
            0.333333, -1, 1.33333         , // #23
            1, -1, 1.33333                , // #24
            0.666667, 0, 1.66667          , // #25
            1.33333, 0, 1.66667           , // #26
            0, 0.666667, 1.66667          , // #27
            0, 1.33333, 1.66667           , // #28
            -0.666667, 0, 1.66667         , // #29
            -1.33333, 0, 1.66667          , // #30
            0, -0.666667, 1.66667         , // #31
            0, -1.33333, 1.66667          , // #32
            0.666667, 0, 4.33333          , // #33
            0, -0.666667, 4.33333         , // #34
            -0.666667, 0, 4.33333         , // #35
            0, 0.666667, 4.33333          , // #36
            0, 0, 2                       , // #37
            0, 0, 4                       , // #38
            0, 0, 3                       , // #39
            0, 1, 2.5                     , // #40
            -1, 0, 2.5                    , // #41
            0, -1, 2.5                    , // #42
            1, 0, 2.5                     , // #43
            0, 1, 3.5                     , // #44
            -1, 0, 3.5                    , // #45
            0, -1, 3.5                    , // #46
            1, 0, 3.5                     , // #47
          };
        for ( int i = 0; i < 47*3; ++i )
        {
          CPPUNIT_ASSERT_DOUBLES_EQUAL( refBC[i], barycenter->getValue()[i], 1e-5);
        }
      }
      //     for (int i = 1; i<=NumberOfElem;i++)
      //     {
      //     cout << "Barycenter " << i << " " << barycenter->getValueIJ(i,1) << " " << barycenter->getValueIJ(i,2) << " " << barycenter->getValueIJ(i,3) << endl;
      //     }

      delete barycenter;

      // test of volume(for 3d elements)
      if (ind == 3)
      {
        FIELD<double>* volume;
        CPPUNIT_ASSERT_NO_THROW(volume= myMesh3->getVolume(sup));

        //       double maxvol,minvol,voltot;
        //       maxvol = 0.;
        //       minvol = 0.;
        //       voltot = 0.0;
        //       for (int i = 1; i<=NumberOfElem;i++)
        //       {
        //         cout << "Volume " << i << " " << volume->getValueIJ(i,1) << endl;
        //         maxvol = dmax(maxvol,volume->getValueIJ(i,1));
        //         minvol = dmin(minvol,volume->getValueIJ(i,1));
        //         voltot = voltot + volume->getValueIJ(i,1);
        //       }

        //       cout << "Max Volume " << maxvol << " Min Volume " << minvol << endl;
        //       cout << "Support Volume " << voltot << endl;

        double refVol[16] = 
          {
            0.666667, 0.666667, 0.666667, 0.666667, 0.666667, 0.666667, 0.666667, 0.666667, 0.666667,  0.666667,  0.666667,  0.666667,  1.333333,  1.333333,  4,  4 
          };
        for ( int i = 0; i < 16; ++i )
          CPPUNIT_ASSERT_DOUBLES_EQUAL( refVol[i], volume->getValue()[i], 1e-6);

        delete volume;

        // test of skin
        SUPPORT *skin;
        CPPUNIT_ASSERT_NO_THROW(skin = myMesh3->getSkin(sup));

        //test mergeSupports and intersectSupports. vactor contains only 1 elements
        vector<SUPPORT *> myVectSup;
        myVectSup.push_back(skin);

        //method return a copy of skin object
        SUPPORT *copyMergeSkin;
        CPPUNIT_ASSERT_NO_THROW(copyMergeSkin = myMesh3->mergeSupports(myVectSup));
        try
        {
          CPPUNIT_ASSERT(copyMergeSkin->deepCompare(*skin));
        }
        catch (const std::exception &e)
        {
          CPPUNIT_FAIL(e.what());
        }
        catch (...)
        {
          CPPUNIT_FAIL("Unknown exception");
        }

        //method return a copy of skin object
        SUPPORT *copyIntersectSkin;
        CPPUNIT_ASSERT_NO_THROW(copyIntersectSkin = myMesh3->intersectSupports(myVectSup));
        try
        {
          CPPUNIT_ASSERT(copyIntersectSkin->deepCompare(*skin));
        }
        catch (const std::exception &e)
        {
          CPPUNIT_FAIL(e.what());
        }
        catch (...)
        {
          CPPUNIT_FAIL("Unknown exception");
        }

        skin->removeReference();
        copyMergeSkin->removeReference();
        copyIntersectSkin->removeReference();
      }
      constituentEntity++;
    }

    // 0020911: [CEA 413] getBarycenter on polygons
    {
      MESHING* polygonMesh = new MESHING();
      MEDMEM::AutoDeref derefMesh( polygonMesh );

      polygonMesh->setName("polygonMesh");

      const int spaceDim = 2, nbNodes = 6;
      const double coords[spaceDim*nbNodes] = 
        {
          0,0, 1,0, 2,0,  0,1, 1,1, 2,1 
        };
      polygonMesh->setCoordinates( spaceDim, nbNodes, coords, "CART", MED_EN::MED_FULL_INTERLACE);

      const MED_EN::medGeometryElement type = MED_EN::MED_POLYGON;
      polygonMesh->setNumberOfTypes(1, MED_EN::MED_CELL);
      polygonMesh->setTypes( &type, MED_EN::MED_CELL );

      const int nbPolygons = 2;
      const int conn[nbPolygons*4] = 
        {
          1,2,5,4, 2,3,6,5 
        };
      const int index[nbPolygons+1] = 
        {
          1,5,9
        };
      polygonMesh->setNumberOfElements( &nbPolygons, MED_EN::MED_CELL );
      polygonMesh->setConnectivity( MED_EN::MED_CELL, type, conn, index );

      FIELD<double>* barycenter;
      const SUPPORT* sup = polygonMesh->getSupportOnAll(MED_CELL);
      CPPUNIT_ASSERT_NO_THROW(barycenter = polygonMesh->getBarycenter(sup));
      CPPUNIT_ASSERT_EQUAL( 2, barycenter->getNumberOfValues() );
      CPPUNIT_ASSERT_EQUAL( 2, barycenter->getNumberOfComponents() );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, barycenter->getValueIJ(1,1), 1e-10);
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, barycenter->getValueIJ(1,2), 1e-10);
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.5, barycenter->getValueIJ(2,1), 1e-10);
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, barycenter->getValueIJ(2,2), 1e-10);
      barycenter->removeReference();
    }


    // Testing length and normal vectors on 1d elements
    {
      // coordinates
      int NumberOfNodes3 = 4;

      string Names3[3] = 
        {
          "X","Y","Z"
        };
      string Units3[3] = 
        {
          "cm","cm","cm"
        };

      double Coordinates3[4*2] = 
        {
          0.0,  0.0,  // n1
          1.0,  1.0,  // n2
          0.0,  1.0,  // n3
          1.0,  0.0
        }; // n4

      const int NumberOfEdgeTypes = 1;
      MED_EN::medGeometryElement EdgeTypes[NumberOfEdgeTypes] = 
        {
          MED_SEG2
        };
      const int NumberOfEdges[NumberOfEdgeTypes] = 
        {
          4
        };
      int ConnectivityEdge[4*2] = 
        {
          1,2, 2,3, 3,4, 4,1
        };

      // CREATE THE MESH
      MEDMEM::MESHING* myMeshing3 = new MEDMEM::MESHING;
      myMeshing3->setName("meshing3");
      myMeshing3->setCoordinates(/*SpaceDimension*/2, NumberOfNodes3, Coordinates3,
                                 "CARTESIAN", MED_EN::MED_FULL_INTERLACE);
      myMeshing3->setCoordinatesNames(Names3);
      myMeshing3->setCoordinatesUnits(Units3);

      // define connectivities
      //      cell part
      const int NumberOfTypes3 = 1;
      medGeometryElement Types3[NumberOfTypes3] = 
        {
          MED_QUAD4
        };
      const int NumberOfElements3[NumberOfTypes3] = 
        {
          1
        };

      myMeshing3->setNumberOfTypes(NumberOfTypes3,MED_CELL);
      myMeshing3->setTypes(Types3,MED_CELL);
      myMeshing3->setNumberOfElements(NumberOfElements3,MED_CELL);

      int Connectivityquad[1*4] = 
        {
          1,2,3,4
        };

      myMeshing3->setConnectivity(MED_CELL,MED_QUAD4,Connectivityquad);

      myMeshing3->setNumberOfTypes(NumberOfEdgeTypes, MED_EDGE);
      myMeshing3->setTypes(EdgeTypes, MED_EDGE);
      myMeshing3->setNumberOfElements(NumberOfEdges, MED_EDGE);

      myMeshing3->setConnectivity( MED_EDGE, MED_SEG2,ConnectivityEdge);

      //test 2D mesh
      //int NumberOfElem = myMeshing3->getNumberOfElements (MED_EDGE, MED_ALL_ELEMENTS);

      const SUPPORT * sup = myMeshing3->getSupportOnAll( MED_EDGE );

      // test of normal(for 1d or 2d elements)
      FIELD<double>* normal;
      CPPUNIT_ASSERT_NO_THROW(normal = myMeshing3->getNormal(sup));

      //     double normal_square, norm;
      //     double maxnorm=0.;
      //     double minnorm=0.;
      //     double tmp_value;
      //     for (int i = 1; i<=NumberOfElem; i++) {
      //       normal_square = 0.;
      //       cout << "Normal " << i << " ";
      //       for (int j=1; j<=/*SpaceDimension*/2; j++) {
      //         tmp_value = normal->getValueIJ(i,j);
      //         normal_square += tmp_value*tmp_value;
      //         cout << tmp_value << " ";
      //       }
      //       norm = sqrt(normal_square);
      //       maxnorm = dmax(maxnorm,norm);
      //       minnorm = dmin(minnorm,norm);
      //       cout << ", Norm = " << norm << endl;
      //     }
      //     cout << "Max Norm " << maxnorm << " Min Norm " << minnorm << endl;
      {
        double refNormals[8] = 
          {
            -1, 1 ,
            -0, -1,
            1, 1 ,
            -0, -1
          };
        for ( int i = 0; i < 8; ++i )
          CPPUNIT_ASSERT_DOUBLES_EQUAL( refNormals[i], normal->getValue()[i], 1e-6);
      }
      // test of length(for 1d elements)
      FIELD<double>* length;
      CPPUNIT_ASSERT_NO_THROW(length = myMeshing3->getLength(sup));

      //     double length_value,maxlength,minlength;
      //     maxlength = 0;
      //     minlength = 0;
      //     for (int i = 1; i<=NumberOfElem;i++) {
      //       length_value = length->getValueIJ(i,1);
      //       cout << "Length " << i << " " << length_value << endl;
      //       maxlength = dmax(maxlength,length_value);
      //       minlength = dmin(minlength,length_value);
      //     }
      //     cout << "Max Length " << maxlength << " Min Length " << minlength << endl;
      double refLen[4] = 
        {
          1.41421,1,1.41421,1
        };
      for ( int i = 0; i < 4; ++i )
        CPPUNIT_ASSERT_DOUBLES_EQUAL( refLen[i], length->getValue()[i], 1e-5);

      vector< FIELD<double> *> myVectField1;
      myVectField1.push_back(normal);
      myVectField1.push_back(length);
      CPPUNIT_ASSERT_NO_THROW(myMeshing3->mergeFields(myVectField1)->removeReference());

      normal->removeReference();
      length->removeReference();
      {
        vector<SUPPORT *> myVectSupEmpty;
        CPPUNIT_ASSERT_THROW(myMesh3->mergeSupports(myVectSupEmpty), MEDEXCEPTION);
      }

      // test mergeFields method: Fields have the same value type
      //intersectSupports and mergeSupports methods
      {
        SUPPORT * sup1 = new SUPPORT;
        sup1->setMesh( myMeshing3 );
        sup1->setEntity( MED_EDGE );
        SUPPORT * sup2 = new SUPPORT;
        sup2->setMesh( myMeshing3 );
        sup2->setEntity( MED_EDGE );
        MED_EN::medGeometryElement gtEdges[1] = 
          {
            MED_SEG2
          };
        int nbEdges1[1] = 
          {
            1
          };
        int edges1[1] = 
          {
            1
          };
        int nbEdges2[1] = 
          {
            2
          };
        int edges2[2] = 
          {
            2,3
          };
        sup1->setpartial("description 1", 1, 1, gtEdges, nbEdges1, edges1);
        sup2->setpartial("description 1", 1, 2, gtEdges, nbEdges2, edges2);

        vector<SUPPORT *> myVectSup3;
        myVectSup3.push_back(sup1);
        myVectSup3.push_back(sup2);
        //method return a MergeSup on the union of all SUPPORTs in Supports.
        SUPPORT *MergeSup;
        CPPUNIT_ASSERT_NO_THROW(MergeSup = myMesh3->mergeSupports(myVectSup3));
        {
          ostringstream out;
          out << *MergeSup << endl;
        }
        MergeSup->removeReference();

        //method return a intersection of all SUPPORTs in IntersectSup
        SUPPORT *IntersectSup;
        CPPUNIT_ASSERT_NO_THROW(IntersectSup = myMesh3->intersectSupports(myVectSup3));
        {
          ostringstream out;
          if (IntersectSup != NULL) out<< *IntersectSup <<endl;
        }
        IntersectSup->removeReference();

        FIELD<double> * length1 = myMeshing3->getLength(sup1);
        FIELD<double> * length2 = myMeshing3->getLength(sup2);

        vector< FIELD<double> *> myVect12;
        myVect12.push_back(length1);
        myVect12.push_back(length2);

        FIELD<double> * length12;
        CPPUNIT_ASSERT_NO_THROW(length12 = myMeshing3->mergeFields(myVect12));
        length12->removeReference();

        sup1->removeReference();
        sup2->removeReference();
        length1->removeReference();
        length2->removeReference();
      }
      myMeshing3->removeReference();
    }
    myMesh3->removeReference();

    /////////////////////////////////////////////////////////
    // TEST 4: test MESH constructed from file pointe.med //
    ////////////////////////////////////////////////////////
    MESH * myMesh4 = new MESH();
    myMesh4->setName(meshname);
    MED_MESH_RDONLY_DRIVER myMeshDriver (filename, myMesh4);
    myMeshDriver.setMeshName(meshname);

    //Mesh has no driver->segmentation violation
    CPPUNIT_ASSERT_THROW(myMesh4->read(), MEDEXCEPTION);

    //Add an existing MESH driver.
    int myDriver4;
    CPPUNIT_ASSERT_NO_THROW(myDriver4 = myMesh4->addDriver(myMeshDriver));

    //read all objects in the file
    CPPUNIT_ASSERT_NO_THROW(myMesh4->read(myDriver4));

    if (myMesh4->getIsAGrid()) 
    {
      GRID* myGrid = dynamic_cast<GRID*>(myMesh4);
      CPPUNIT_ASSERT(myGrid);
    }

    //myDriver4->DRONLY->can't write
    CPPUNIT_ASSERT_THROW(myMesh4->write(myDriver4), MEDEXCEPTION);

    // add new driver
    int idMeshV21;
    CPPUNIT_ASSERT_NO_THROW(idMeshV21 = myMesh4->addDriver(MED_DRIVER,filenameout21));

    //Write all the content of the MESH using driver referenced by the integer handler index.
    CPPUNIT_ASSERT_NO_THROW(myMesh4->write(idMeshV21));

    // remove driver from mesh
    CPPUNIT_ASSERT_NO_THROW(myMesh4->rmDriver(myDriver4));

    // ensure exception is raised on second attempt to remove driver
    CPPUNIT_ASSERT_THROW(myMesh4->rmDriver(myDriver4),MEDEXCEPTION);

    // Create a MESH object using a MESH driver of type MED_DRIVER associated with file fileName.
    MESH* myMesh5;
    CPPUNIT_ASSERT_NO_THROW(myMesh5 = new MESH(MED_DRIVER, filename, meshname));
    if(myMesh5->getIsAGrid())
    {
      GRID* myGrid = dynamic_cast<GRID*>(myMesh4);
      CPPUNIT_ASSERT(myGrid);
    }

    //ensure two meshes constructed from one file in two different ways are equal
    CPPUNIT_ASSERT(myMesh5->deepCompare(*myMesh4));

    // test other variants of read() and write()
    {
      const string otherName1("otherName1"), otherName2("otherName2");
      MESH mesh1, mesh2, mesh3;

      //myMesh5 -> filenameout21
      // GMESH::write(driverTypes driverType, const string& filename,const string& meshname)
      myMesh5->write( MED_DRIVER, filenameout21, otherName1);
      CPPUNIT_ASSERT_THROW( myMesh5->write(myMeshDriver), MEDEXCEPTION); // write with RDONLY driver

      //filenameout21 -> mesh1
      // GMESH::read(driverTypes driverType, const string& filename, const string& meshname);
      CPPUNIT_ASSERT_THROW( mesh1.read(VTK_DRIVER,filenameout21,otherName1), MEDEXCEPTION);
      CPPUNIT_ASSERT_THROW( mesh1.read(MED_DRIVER, filenameout21,otherName2), MEDEXCEPTION);
      mesh1.read(MED_DRIVER,filenameout21,otherName1);
      CPPUNIT_ASSERT(myMesh5->deepCompare(mesh1));

      MED_MESH_RDWR_DRIVER driver;
      driver.setFileName( filenameout21 );
      driver.setMeshName( otherName2 );

      // mesh1 -> filenameout21
      // GMESH::write( const GENDRIVER& )
      mesh1.write( driver );

      // filenameout21 -> mesh2
      // GMESH::read( const GENDRIVER& )
      mesh2.read( driver );
      CPPUNIT_ASSERT(myMesh5->deepCompare(mesh2));
      // check that GMESH::write() clears the file by default
      CPPUNIT_ASSERT_THROW( mesh3.read( MED_DRIVER, filenameout21,otherName1), MEDEXCEPTION);
      // write by adding a mesh
      myMesh5->write( MED_DRIVER, filenameout21, otherName1, MED_EN::RDWR );
      CPPUNIT_ASSERT_NO_THROW( mesh3.read( MED_DRIVER, filenameout21,otherName1));
    }

    int myDriver6;
    MESH* myMesh6 = new MESH();
    myDriver6 = myMesh6->addDriver(MED_DRIVER, filename, meshname, RDONLY);

    myMesh6->read(myDriver6);

    //ensure two meshes constracted from one file in two different ways are equal
    CPPUNIT_ASSERT(myMesh6->deepCompare(*myMesh4));

    //test FAMILY
    int NumberOfFamilies4;
    CPPUNIT_ASSERT_NO_THROW(NumberOfFamilies4 = myMesh6->getNumberOfFamilies(MED_CELL));
    CPPUNIT_ASSERT_MESSAGE("Current mesh hasn't Families", NumberOfFamilies4 != 0);

    vector<FAMILY*> families4;
    CPPUNIT_ASSERT_NO_THROW(families4 = myMesh6->getFamilies(MED_CELL));
    CPPUNIT_ASSERT((int)families4.size() == NumberOfFamilies4);
    for(int nb = 1; nb <= NumberOfFamilies4; nb++ )
    {
      const FAMILY* family;
      CPPUNIT_ASSERT_NO_THROW(family = myMesh6->getFamily(MED_CELL, nb));
      CPPUNIT_ASSERT_EQUAL(family->getName(), families4[nb-1]->getName());
    }

    //get support which reference all elements on the boundary of mesh.
    SUPPORT*myBndSup;
    CPPUNIT_ASSERT_THROW(myMesh6->getBoundaryElements(MED_CELL), MEDEXCEPTION);
    //get only face in 3D.
    CPPUNIT_ASSERT_NO_THROW(myBndSup = myMesh6->getBoundaryElements(MED_FACE));

    //test buildSupportOnElementsFromElementList and buildSupportOnNodeFromElementList
    const int * myConnectivityValue6;
    CPPUNIT_ASSERT_NO_THROW(myConnectivityValue6 = myMesh6->getReverseConnectivity(MED_DESCENDING));
    const int * myConnectivityIndex6;
    CPPUNIT_ASSERT_NO_THROW(myConnectivityIndex6 = myMesh6->getReverseConnectivityIndex(MED_DESCENDING));
    int numberOfElem6;
    CPPUNIT_ASSERT_NO_THROW(numberOfElem6 = myMesh6->getNumberOfElements(MED_FACE,MED_ALL_ELEMENTS));
    list<int> myElementsList6;

    for (int i=0; i<numberOfElem6; i++)
      if (myConnectivityValue6[myConnectivityIndex6[i]] == 0) 
      {
        myElementsList6.push_back(i+1);
      }

    SUPPORT * mySupportOnNode;
    SUPPORT * mySupportOnElem;
    CPPUNIT_ASSERT_NO_THROW(mySupportOnElem = myMesh6->buildSupportOnElementsFromElementList(myElementsList6,MED_FACE));
    CPPUNIT_ASSERT(mySupportOnElem->deepCompare(*myBndSup));
    myBndSup->removeReference();
    CPPUNIT_ASSERT_EQUAL(MED_FACE, mySupportOnElem->getEntity());

    list<int>::const_iterator iteronelem = myElementsList6.begin();
    for (int i = 1; i <= 3; i++, iteronelem++) 
    {
      CPPUNIT_ASSERT_EQUAL(i, mySupportOnElem->getValIndFromGlobalNumber(*iteronelem));
    }
    mySupportOnElem->removeReference();
    CPPUNIT_ASSERT_NO_THROW(mySupportOnNode = myMesh6->buildSupportOnNodeFromElementList(myElementsList6,MED_FACE));
    SUPPORT *suppp=myMesh6->getBoundaryElements(MED_NODE);
    CPPUNIT_ASSERT(mySupportOnNode->deepCompare( *(suppp)));
    suppp->removeReference();
    mySupportOnNode->removeReference();

    //sets mesh fields to initial values
    myMesh6->init();

    //ensure two meshes constracted from one file in two different ways are equal
    CPPUNIT_ASSERT(!myMesh6->deepCompare(*myMesh4));

    //ensure mesh is empty
    CPPUNIT_ASSERT(myMesh6->getSpaceDimension() == MED_INVALID);
    CPPUNIT_ASSERT(myMesh6->getNumberOfNodes() == MED_INVALID);
    CPPUNIT_ASSERT(myMesh6->getCoordinateptr() == NULL);

    myMesh4->removeReference();
    myMesh5->removeReference();
    myMesh6->removeReference();

    MESH* myMesh7 = MEDMEMTest_createTestMesh();
    vector< vector<double> > myBndBox;
    try
    {
      myBndBox = myMesh7->getBoundingBox();
    }
    catch (const std::exception &e)
    {
      CPPUNIT_FAIL(e.what());
    }
    catch (...)
    {
      CPPUNIT_FAIL("Unknown exception");
    }

    //cout<<"Bounding box for createTestMesh()"<<endl;
    double refBox[6] = 
      {
        -2, -2, 0, 2, 2, 5 
      };
    for(int i = 0, iB = 0; i < myBndBox.size(); i++)
    {
      for(int j = 0; j < myBndBox[i].size(); j++)
        CPPUNIT_ASSERT_DOUBLES_EQUAL( refBox[iB++], myBndBox[i][j], 1e-6);
      //cout<<" "<< myBndBox[i][j]<<" ";
      //cout<<endl;
    }

    double CoorPoint[3] = 
      {
        0.0,  0.0, 1.0
      }; //n2
    int idxElem;
    try
    {
      idxElem = myMesh7->getElementContainingPoint(CoorPoint);
    }
    catch (const std::exception &e)
    {
      CPPUNIT_FAIL(e.what());
    }
    catch (...)
    {
      CPPUNIT_FAIL("Unknown exception");
    }
    CPPUNIT_ASSERT(idxElem != -1);

    double CoorNoPoint[3] = 
      {
        5.0,  0.0, -5.0
      }; //there is no such point
    int idxNoElem;
    try
    {
      idxNoElem = myMesh7->getElementContainingPoint(CoorNoPoint);
    }
    catch (const std::exception &e)
    {
      CPPUNIT_FAIL(e.what());
    }
    catch (...)
    {
      CPPUNIT_FAIL("Unknown exception");
    }
    myMesh7->removeReference();
    CPPUNIT_ASSERT(idxNoElem == -1);

    ///////////////////////////////////////////////////////
    // TEST 5: test desactivateFacesComputation() method //
    //         of driver: NPAL17670                      //
    ///////////////////////////////////////////////////////
    double coords[54] = 
      {
        -0.215040, -0.107520, +0.000000,
        +0.000000, -0.107520, +0.000000, 
        +0.000000, +0.107520, +0.000000, 
        -0.215040, +0.107520, +0.000000, 
        +0.215040, -0.107520, +0.000000, 
        +0.215040, +0.107520, +0.000000, 
        -0.215040, -0.107520, +1.500000, 
        -0.215040, -0.107520, +4.080623, 
        +0.000000, -0.107520, +1.500000, 
        +0.000000, -0.107520, +4.080623, 
        +0.000000, +0.107520, +1.500000, 
        +0.000000, +0.107520, +4.080623, 
        -0.215040, +0.107520, +1.500000, 
        -0.215040, +0.107520, +4.080623, 
        +0.215040, -0.107520, +1.500000, 
        +0.215040, -0.107520, +4.080623, 
        +0.215040, +0.107520, +1.500000, 
        +0.215040, +0.107520, +4.080623 
      };

    int connQuad4[] = 
      {
        2 ,         5   ,      15   ,       9 ,
        10,         16  ,       18  ,       12, 
        11 ,        13  ,       14  ,       12, 
        7   ,       9   ,      11   ,      13, 
        3   ,       4   ,      13   ,      11, 
        1   ,       2   ,       9   ,       7, 
        1   ,       2   ,       3   ,       4, 
        15   ,      17  ,       18  ,       16 ,
        5    ,      6   ,      17   ,      15, 
        9    ,     15   ,      17   ,      11 ,
        13    ,      7  ,        8  ,       14, 
        4     ,     1   ,       7   ,      13, 
        9     ,    11   ,      12   ,      10, 
        8     ,    10   ,      12   ,      14, 
        2     ,     5   ,       6   ,       3, 
        17    ,     11,         12  ,       18 ,
        2     ,     3  ,       11   ,       9, 
        6     ,     3  ,       11   ,      17,
        7     ,     9  ,       10   ,       8, 
        9     ,    15  ,       16   ,      10 
      };

    int connHexa8[] = 
      {
        3       ,   2      ,    1   ,       4    ,     11     ,     9    ,      7    ,     13, 
        17     ,    15     ,     9    ,     11   ,      18    ,     16   ,      10   ,      12, 
        11    ,      9     ,     7    ,     13   ,      12    ,     10   ,       8   ,       14 ,
        6 ,         5      ,    2     ,     3    ,     17     ,    15    ,      9    ,     11
      };

    int bottom[2] = 
      {
        7,15
      };
    MED_EN::medGeometryElement bottomTypes[1] = 
      {
        MED_EN::MED_QUAD4
      };
    int bottomIndex[2] = 
      {
        1,3
      };
    int bottomNbOfElts[1] = 
      {
        2
      };

    MESHING* meshing = new MESHING();
    meshing->setName( "TESTMESH" );
    const int nFaces=20;
    const int nNodes=18;
    meshing->setCoordinates(3, nNodes, coords, "CARTESIAN", MED_EN::MED_FULL_INTERLACE);
    string coordname[3] = 
      {
        "x", "y", "z" 
      };
    meshing->setCoordinatesNames(coordname);
    string coordunit[3] = 
      {
        "m", "m", "m" 
      };
    meshing->setCoordinatesUnits(coordunit);
    //Cell connectivity info for classical elts
    const MED_EN::medGeometryElement classicalTypesCell[1]=
      {
        MED_EN::MED_HEXA8
      };
    const int nbOfCellElts[1]=
      {
        4
      };
    meshing->setNumberOfTypes(1,MED_EN::MED_CELL);
    meshing->setTypes(classicalTypesCell,MED_EN::MED_CELL);
    meshing->setNumberOfElements(nbOfCellElts,MED_EN::MED_CELL);
    //Face connectivity info for classical elts
    const MED_EN::medGeometryElement classicalTypesFace[1]=
      {
        MED_EN::MED_QUAD4
      };
    const int nbOfFaceElts[1]=
      {
        nFaces
      };
    meshing->setNumberOfTypes(1,MED_EN::MED_FACE);
    meshing->setTypes(classicalTypesFace,MED_EN::MED_FACE);
    meshing->setNumberOfElements(nbOfFaceElts,MED_EN::MED_FACE);
    //All cell conn
    meshing->setConnectivity(MED_EN::MED_CELL,MED_EN::MED_HEXA8,connHexa8);
    //All face conn
    meshing->setConnectivity(MED_EN::MED_FACE,MED_EN::MED_QUAD4,connQuad4);
    //Adding some groups on faces
    addMedFacesGroup( *meshing, 4,  bottom, "BottomFaces",bottomTypes,bottomIndex,bottomNbOfElts,1) ;
    //addMedFacesGroupAll( *meshing, "AllFaces");
    //writing...
    int id=meshing->addDriver(MED_DRIVER,filenameout,meshing->getName());
    meshing->write(id);
    // Field writing
    const SUPPORT *sup=meshing->getSupportOnAll( MED_FACE );
    FIELD<double> * field = new FIELD<double>(sup, 1);
    field->setName("temperature");
    field->setComponentName(1,"T"); field->setMEDComponentUnit(1,"K");
    double *tab=(double *)field->getValue();
    for(int i=0;i<nFaces;i++)
      tab[i]=i*(1.22);
    field->setIterationNumber(0);
    field->setOrderNumber(-1);
    field->setTime(12.);
    id=field->addDriver(MED_DRIVER,filenameout,field->getName());
    field->write(id);
    field->removeReference();
    meshing->removeReference();
    //
    MESH mesh;
    MED_MESH_RDONLY_DRIVER drv(filenameout,&mesh);
    drv.desactivateFacesComputation();
    drv.setMeshName("TESTMESH");
    mesh.addDriver(drv);
    mesh.read();
    const int *conn=mesh.getConnectivity(MED_NODAL,MED_FACE,MED_ALL_ELEMENTS);
    for (int j = 0; j < nFaces; j++) 
    {
      for (int k = 0; k < 4; k++)
        CPPUNIT_ASSERT_EQUAL(conn[4*j+k], connQuad4[4*j+k]);
    }
    FIELD<double> f;
    f.addDriver(MED_DRIVER,filenameout,"temperature");
    f.setIterationNumber(0);
    f.setOrderNumber(-1);
    CPPUNIT_ASSERT_NO_THROW( f.read() );

    //////////////////////////////////////////////////////////
    // TEST 6: Test Reading of a Field with given Mesh.     //
    // Group from the Mesh must be taken for Field Support. //
    //////////////////////////////////////////////////////////
    {
      // mesh creation
      MESHING* mesh_prof = new MESHING();
      mesh_prof->setName("TESTMESH");
      mesh_prof->setCoordinates(3, nNodes, coords, "CARTESIAN", MED_EN::MED_FULL_INTERLACE);
      mesh_prof->setCoordinatesNames(coordname);
      mesh_prof->setCoordinatesUnits(coordunit);

      //Cell connectivity info for classical elts
      //mesh_prof->setNumberOfTypes(1,MED_EN::MED_CELL);
      //mesh_prof->setTypes(classicalTypesCell,MED_EN::MED_CELL);
      //mesh_prof->setNumberOfElements(nbOfCellElts,MED_EN::MED_CELL);
      //mesh_prof->setConnectivity(connHexa8,MED_EN::MED_CELL,MED_EN::MED_HEXA8);

      //Face connectivity info for classical elts
      //mesh_prof->setNumberOfTypes(1,MED_EN::MED_FACE);
      //mesh_prof->setTypes(classicalTypesFace,MED_EN::MED_FACE);
      //mesh_prof->setNumberOfElements(nbOfFaceElts,MED_EN::MED_FACE);
      //mesh_prof->setConnectivity(connQuad4,MED_EN::MED_FACE,MED_EN::MED_QUAD4);
      mesh_prof->setNumberOfTypes(1,MED_EN::MED_CELL);
      mesh_prof->setTypes(classicalTypesFace,MED_EN::MED_CELL);
      mesh_prof->setNumberOfElements(nbOfFaceElts,MED_EN::MED_CELL);
      mesh_prof->setConnectivity(MED_EN::MED_CELL,MED_EN::MED_QUAD4,connQuad4);

      //Adding some groups on faces
      GROUP *faces_prof=new GROUP;
      faces_prof->setName("BottomFaces");
      faces_prof->setMesh(mesh_prof);
      //faces_prof->setEntity(MED_EN::MED_FACE);
      faces_prof->setEntity(MED_EN::MED_CELL);
      faces_prof->setNumberOfGeometricType(1);
      faces_prof->setGeometricType(bottomTypes);
      faces_prof->setNumberOfElements(bottomNbOfElts);
      faces_prof->setNumber(bottomIndex, bottom);
      mesh_prof->addGroup(*faces_prof);

      // Field creation
      FIELD<double> * field_prof = new FIELD<double>(faces_prof, 1);
      faces_prof->removeReference();
      field_prof->setName("temperature");
      field_prof->setComponentName(1,"T");
      field_prof->setMEDComponentUnit(1,"K");
      double *tab = (double *)field_prof->getValue();
      for (int i = 0; i < 2; i++)
        tab[i] = i*(1.22);
      field_prof->setTime(12.);

      // Writing...
      int id_prof = mesh_prof->addDriver(MED_DRIVER, filename_profiles_wr, mesh_prof->getName());
      mesh_prof->write(id_prof);
      mesh_prof->rmDriver(id_prof);

      // Field writing
      id_prof = field_prof->addDriver(MED_DRIVER, filename_profiles_wr, field_prof->getName());
      field_prof->write(id_prof);

      field_prof->removeReference();
      mesh_prof->removeReference();

      // Reading...
      MESH *mesh_rd=new MESH(MED_DRIVER, filename_profiles_wr, "TESTMESH");
      FIELD<double> *field_rd=new FIELD<double>(MED_DRIVER, filename_profiles_wr, "temperature", -1, -1, mesh_rd);
      //const vector <GROUP*> groups_rd = mesh_rd.getGroups(MED_EN::MED_FACE);
      vector <GROUP*> groups_rd = mesh_rd->getGroups(MED_EN::MED_CELL);
      vector <SUPPORT*> supports_rd(groups_rd.size());//Because virtual inheritance
      int iii=0;
      for(vector <GROUP*>::iterator iter=groups_rd.begin();iter!=groups_rd.end();iter++,iii++)
        supports_rd[iii]=dynamic_cast<SUPPORT*>(*iter);
      CPPUNIT_ASSERT(find(supports_rd.begin(),supports_rd.end(),field_rd->getSupport()) != supports_rd.end());
      field_rd->removeReference();
      mesh_rd->removeReference();
    }
}

