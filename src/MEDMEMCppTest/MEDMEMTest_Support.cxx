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

#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_MedMeshDriver.hxx"

#include <cppunit/TestAssert.h>
#include <sstream>
#include <cmath>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;

// #52: MEDMEM_Support.hxx  }  MEDMEMTest_Support.cxx

/*!
 *  Check methods (48), defined in MEDMEM_Support.hxx:
 *  class SUPPORT : public RCBASE 
{
 *   (+)     SUPPORT();
 *   (+)     SUPPORT(MESH* Mesh, string Name="", MED_EN::medEntityMesh Entity=MED_EN::MED_CELL);
 *   (+)     SUPPORT(const SUPPORT & m);
 *   (+)     virtual ~SUPPORT();
 *
 *   (+)     friend ostream & operator<<(ostream &os,const SUPPORT &my);
 *   (+)     SUPPORT& operator=(const SUPPORT &support);
 *   (+)     bool operator == (const SUPPORT &support) const;
 *   (+)     bool deepCompare(const SUPPORT &support) const;
 *
 *   (+)     void update();
 *
 *   (+)     inline void setName(string Name);
 *   (+)     inline void setDescription(string Description);
 *   (+)     void setMesh(MESH *Mesh) const;
 *   (+)     inline void setMeshName(const string & meshName);
 *   (+)     inline void setAll(bool All);
 *   (+)     inline void setEntity(MED_EN::medEntityMesh Entity);
 *
 *   (+)     inline void setNumberOfGeometricType(int NumberOfGeometricType);
 *   (+)     inline void setGeometricType(const MED_EN::medGeometryElement *GeometricType);
 *   (+)     inline void setNumberOfElements(const int *NumberOfElements);
 *   (+)     inline void setTotalNumberOfElements(int TotalNumberOfElements);
 *   (+)     inline void setNumber(MEDSKYLINEARRAY * Number);
 *   (+)     inline void setNumber(const int * index, const int* value, bool shallowCopy=false);
 *
 *   (+)     inline string getName() const;
 *   (+)     inline string getDescription() const;
 *   (+)     virtual inline MESH * getMesh() const;
 *   (+)     string getMeshName() const;
 *   (+)     inline MED_EN::medEntityMesh getEntity() const;
 *   (+)     inline bool   isOnAllElements() const;
 *
 *   (+)     inline int getNumberOfTypes() const;
 *   (+)     inline const MED_EN::medGeometryElement* getTypes() const;
 *   (+)     inline int getNumberOfElements
 *                      (MED_EN::medGeometryElement GeometricType) const throw (MEDEXCEPTION);
 *   (+)     inline const int * getNumberOfElements() const throw (MEDEXCEPTION);
 *   (+)     virtual inline MEDSKYLINEARRAY * getnumber() const throw (MEDEXCEPTION);
 *   (NO METHOD IN THIS BRANCH) virtual inline MEDSKYLINEARRAY * getnumberFromFile() const throw (MEDEXCEPTION);
 *   (+)     virtual inline const int * getNumber
 *                   (MED_EN::medGeometryElement GeometricType) const throw (MEDEXCEPTION);
 *   (NO METHOD IN THIS BRANCH) virtual inline const int * getNumberFromFile
 *                   (MED_EN::medGeometryElement GeometricType) const throw (MEDEXCEPTION);
 *   (+)     virtual inline const int * getNumberIndex() const throw (MEDEXCEPTION);
 *   (+)     virtual int getValIndFromGlobalNumber(const int number) const throw (MEDEXCEPTION);
 *
 *   (+)     void blending(SUPPORT * mySupport) throw (MEDEXCEPTION);
 *
 *   (+)     void setpartial(string Description, int NumberOfGeometricType,
 *                           int TotalNumberOfEntity, MED_EN::medGeometryElement *GeometricType,
 *                           int *NumberOfEntity, int *NumberValue);
 *   (+)     void setpartial(MEDSKYLINEARRAY * number, bool shallowCopy=false) throw (MEDEXCEPTION);
 *   (NO METHOD IN THIS BRANCH) void setpartial_fromfile(MEDSKYLINEARRAY * number, bool shallowCopy=false) throw (MEDEXCEPTION);
 *
 *   (+)     void   setProfilNames(vector<string> profilNames) throw (MEDEXCEPTION);
 *   (+)     vector<string> getProfilNames() const throw (MEDEXCEPTION);
 *
 *   (+)     void getBoundaryElements() throw (MEDEXCEPTION);
 *
 *   (+)     void changeElementsNbs(MED_EN::medEntityMesh entity, const int *renumberingFromOldToNew,
 *                                  int limitNbClassicPoly, const int *renumberingFromOldToNewPoly=0);
 *   (+)     void intersecting(SUPPORT * mySupport) throw (MEDEXCEPTION);
 *   (+)     bool belongsTo(const SUPPORT& other, bool deepCompare=false) const;
 *
 *   (+)     SUPPORT *getComplement() const;
 *   (+)     SUPPORT *substract(const SUPPORT& other) const throw (MEDEXCEPTION);
 *
 *   (+)     SUPPORT *getBoundaryElements(MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION);
 *
 *   (+)     void fillFromNodeList(const list<int>& listOfNode) throw (MEDEXCEPTION);
 *   (+)     void fillFromElementList(const list<int>& listOfElt) throw (MEDEXCEPTION);
 *
 *   (+)     void clearDataOnNumbers();
 *
 *   (reference counter presently disconnected in C++) virtual void addReference() const;
 *   (reference counter presently disconnected in C++) virtual void removeReference() const;
 *  
}
 */
void MEDMEMTest::testSupport()
{
  MESH * aMesh = MEDMEMTest_createTestMesh();
  MESH * aMeshOneMore = MEDMEMTest_createTestMesh();

  ////////////
  // TEST 1 //
  ////////////

  SUPPORT *aSupportOnFaces1 = new SUPPORT;
  aSupportOnFaces1->setMesh( aMesh );
  aSupportOnFaces1->setEntity( MED_EN::MED_FACE );
  aSupportOnFaces1->setAll( true );
  aSupportOnFaces1->update();

  SUPPORT *aSupportOnCells1 = new SUPPORT;
  aSupportOnCells1->setMesh( aMesh );
  aSupportOnCells1->setAll( true );
  aSupportOnCells1->update();

  SUPPORT *aSupportOnFaces2=new SUPPORT;
  SUPPORT *aSupportOnCells2=new SUPPORT;

  SUPPORT *aSupportOnFaces3 = new SUPPORT;
  aSupportOnFaces3->setMesh( aMeshOneMore );
  aSupportOnFaces3->setName( "Support On Faces 3" );
  aSupportOnFaces3->setEntity( MED_EN::MED_FACE );
  aSupportOnFaces3->setAll( true );
  aSupportOnFaces3->update();
  // entity
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_FACE, aSupportOnFaces1->getEntity());
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_CELL, aSupportOnCells1->getEntity());

  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_CELL, aSupportOnFaces2->getEntity());
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_CELL, aSupportOnCells2->getEntity());

  aSupportOnFaces2->setEntity(MED_EN::MED_FACE);
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_FACE, aSupportOnFaces2->getEntity());

  // isOnAllElements
  CPPUNIT_ASSERT(aSupportOnFaces1->isOnAllElements());
  CPPUNIT_ASSERT(aSupportOnCells1->isOnAllElements());

  CPPUNIT_ASSERT(!aSupportOnFaces2->isOnAllElements());
  CPPUNIT_ASSERT(!aSupportOnCells2->isOnAllElements());

  // mesh and mesh name
  CPPUNIT_ASSERT_THROW(aSupportOnFaces1->setMeshName("Mesh 1"), MEDEXCEPTION);

  aSupportOnFaces2->setMeshName("Mesh 1");
  aSupportOnCells2->setMeshName("Mesh 1");
  CPPUNIT_ASSERT(aSupportOnFaces2->getMeshName() == "Mesh 1");
  CPPUNIT_ASSERT(aSupportOnCells2->getMeshName() == "Mesh 1");

  aSupportOnFaces2->setMesh(aMesh);
  CPPUNIT_ASSERT_THROW(aSupportOnFaces2->setMeshName("Mesh 1"), MEDEXCEPTION);

  aSupportOnCells1->setMesh(NULL);
  aSupportOnCells1->setMeshName("Mesh 1");

  CPPUNIT_ASSERT(aSupportOnFaces1->getMeshName() == "meshing");
  CPPUNIT_ASSERT(aSupportOnFaces2->getMeshName() == "meshing");
  CPPUNIT_ASSERT(aSupportOnCells1->getMeshName() == "Mesh 1");
  CPPUNIT_ASSERT(aSupportOnCells2->getMeshName() == "Mesh 1");

  CPPUNIT_ASSERT(aSupportOnFaces1->getMesh() == aMesh);
  CPPUNIT_ASSERT(aSupportOnFaces2->getMesh() == aMesh);
  CPPUNIT_ASSERT(aSupportOnCells1->getMesh() == NULL);
  CPPUNIT_ASSERT(aSupportOnCells2->getMesh() == NULL);

  // _number
  {
    int iElem = 1, nbTypes = aSupportOnFaces1->getNumberOfTypes();
    const MED_EN::medGeometryElement* types = aSupportOnFaces1->getTypes();
    for ( int iType = 0; iType < nbTypes; ++iType )
      {
        const int nb = aSupportOnFaces1->getNumberOfElements( types[iType] );
        const int* nums = aSupportOnFaces1->getNumber       ( types[iType] );
        for ( int i = 0; i < nb; ++i )
          CPPUNIT_ASSERT_EQUAL( iElem++, nums[i] );
      }
    iElem = 1, nbTypes = aSupportOnCells1->getNumberOfTypes();
    types = aSupportOnCells1->getTypes();
    for ( int iType = 0; iType < nbTypes; ++iType )
      {
        const int nb = aSupportOnCells1->getNumberOfElements( types[iType] );
        const int* nums = aSupportOnCells1->getNumber       ( types[iType] );
        for ( int i = 0; i < nb; ++i )
          CPPUNIT_ASSERT_EQUAL( iElem++, nums[i] );
      }
  }

  // operator ==
  aSupportOnFaces2->setName("Support On Faces 2");
  aSupportOnCells2->setName("Support On Cells 2");

  // because aSupportOnFaces1 is on all elements, but aSupportOnFaces2 not
  CPPUNIT_ASSERT(!(aSupportOnFaces1 == aSupportOnFaces2));

  aSupportOnCells1->setAll(false);
  CPPUNIT_ASSERT(!aSupportOnCells1->isOnAllElements());
  // this is only sets flag _isOnAllElts to false, but not clear elements and types
  // information, so aSupportOnCells1 is not equal to aSupportOnCells2 at this moment

  //#ifdef ENABLE_FAULTS
  // (BUG) SUPPORT::operator= gives "Segmentation Fault" on supports with NULL meshes!!!
  //CPPUNIT_ASSERT(!(aSupportOnCells1 == aSupportOnCells2));
  //#endif
  //#ifdef ENABLE_FORCED_FAILURES
  //CPPUNIT_FAIL("SUPPORT::operator= gives Segmentation Fault on supports with NULL meshes!!!");
  //#endif

  aSupportOnFaces2->setAll(true);
  CPPUNIT_ASSERT(aSupportOnFaces2->isOnAllElements());
  // flag _isOnAllElts now is set to true, but we need also to call update()
  // to set elements and types information from mesh to this support
  CPPUNIT_ASSERT(!(*aSupportOnFaces1 == *aSupportOnFaces2));
  aSupportOnFaces2->update();
  CPPUNIT_ASSERT(*aSupportOnFaces1 == *aSupportOnFaces2);

  // deep compare
  CPPUNIT_ASSERT(aSupportOnFaces1->deepCompare(*aSupportOnFaces2));

  CPPUNIT_ASSERT(!(*aSupportOnFaces1 == *aSupportOnFaces3)); // different mesh pointers
  CPPUNIT_ASSERT(aSupportOnFaces1->deepCompare(*aSupportOnFaces3)); // meshes are really equal

  // description
  aSupportOnFaces1->setDescription("Support on all faces with mesh pointer");
  aSupportOnFaces2->setDescription("Support on all faces with mesh pointer");

  aSupportOnCells1->setDescription("Support on a part of cells with mesh name");
  aSupportOnCells2->setDescription("Support on a part of cells with mesh name");

  CPPUNIT_ASSERT(aSupportOnFaces1->getDescription() == "Support on all faces with mesh pointer");
  CPPUNIT_ASSERT(aSupportOnFaces2->getDescription() == "Support on all faces with mesh pointer");
  CPPUNIT_ASSERT(aSupportOnCells1->getDescription() == "Support on a part of cells with mesh name");
  CPPUNIT_ASSERT(aSupportOnCells2->getDescription() == "Support on a part of cells with mesh name");
  aSupportOnFaces2->removeReference();
  // operator<<
  ostringstream ostr1;
  ostr1 << *aSupportOnFaces1;
  CPPUNIT_ASSERT(ostr1.str() != "");

  ostringstream ostr2;
  ostr2 << *aSupportOnCells2;
  CPPUNIT_ASSERT(ostr2.str() != "");

  // set/get elements and types information

  // 1. aSupportOnCells1 is not on all elements, but now it contains information
  //    about all cells of aMesh, because it was created on mesh and with parameters.
  CPPUNIT_ASSERT_EQUAL(3, aSupportOnCells1->getNumberOfTypes());
  CPPUNIT_ASSERT(aSupportOnCells1->getTypes() != NULL);

  aSupportOnCells1->setNumberOfGeometricType(0);
  CPPUNIT_ASSERT_EQUAL(0, aSupportOnCells1->getNumberOfTypes());
  CPPUNIT_ASSERT(aSupportOnCells1->getTypes() == NULL);

  MED_EN::medGeometryElement aSCTypes[2] = 
    {
      MED_EN::MED_PYRA5, MED_EN::MED_PENTA15
    };
  aSupportOnCells1->setNumberOfGeometricType(2);
  aSupportOnCells1->setGeometricType(aSCTypes);

  CPPUNIT_ASSERT_EQUAL(2, aSupportOnCells1->getNumberOfTypes());
  CPPUNIT_ASSERT(aSupportOnCells1->getTypes() != NULL);
  CPPUNIT_ASSERT(aSupportOnCells1->getNumberOfElements() == NULL); // reset by setNumberOfGeometricType

  int nbEltsSC[2] = 
    {
      2, 1
    };
  aSupportOnCells1->setNumberOfElements(nbEltsSC);

  const int * nbEltsSCBack = aSupportOnCells1->getNumberOfElements();
  CPPUNIT_ASSERT_EQUAL(2, aSupportOnCells1->getNumberOfElements(MED_EN::MED_PYRA5));
  CPPUNIT_ASSERT_EQUAL(2, nbEltsSCBack[0]);
  CPPUNIT_ASSERT_EQUAL(1, aSupportOnCells1->getNumberOfElements(MED_EN::MED_PENTA15));
  CPPUNIT_ASSERT_EQUAL(1, nbEltsSCBack[1]);
  CPPUNIT_ASSERT_EQUAL(3, aSupportOnCells1->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS));

  // old types
  CPPUNIT_ASSERT_THROW(aSupportOnCells1->getNumberOfElements(MED_EN::MED_TETRA4), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aSupportOnCells1->getNumberOfElements(MED_EN::MED_HEXA8), MEDEXCEPTION);

  //_number= new MEDSKYLINEARRAY(_numberOfGeometricType,_totalNumberOfElements,index,value,shallowCopy);
  int indexSC[3] = 
    {
      1,3,4
    }; // length = nb.types + 1
  int valueSC[3] = 
    {
      21,22,47
    }; // length = total nb. of elements
  aSupportOnCells1->setNumber(indexSC, valueSC);

  // check number
  MEDSKYLINEARRAY * aNumberSCBack = aSupportOnCells1->getnumber();

  CPPUNIT_ASSERT_EQUAL(2, aNumberSCBack->getNumberOf());
  CPPUNIT_ASSERT_EQUAL(3, aNumberSCBack->getLength());
  const int * ind = aNumberSCBack->getIndex();
  const int * val = aNumberSCBack->getValue();
  CPPUNIT_ASSERT_EQUAL(1, ind[0]);
  CPPUNIT_ASSERT_EQUAL(3, ind[1]);
  CPPUNIT_ASSERT_EQUAL(4, ind[2]);
  CPPUNIT_ASSERT_EQUAL(21, val[0]);
  CPPUNIT_ASSERT_EQUAL(22, val[1]);
  CPPUNIT_ASSERT_EQUAL(47, val[2]);

  // 2. aSupportOnCells2 is not on all elements, and now it contains
  //    no information about elements of a mesh
  CPPUNIT_ASSERT_EQUAL(0, aSupportOnCells2->getNumberOfTypes());
  CPPUNIT_ASSERT(aSupportOnCells2->getTypes() == NULL);

  aSupportOnCells2->setNumberOfGeometricType(2);
  aSupportOnCells2->setGeometricType(aSCTypes);

  CPPUNIT_ASSERT_EQUAL(2, aSupportOnCells2->getNumberOfTypes());
  CPPUNIT_ASSERT(aSupportOnCells2->getTypes() != NULL);

  aSupportOnCells2->setNumberOfElements(nbEltsSC);

  // we create MEDSKYLINEARRAY as pointer, because aSupportOnCells2 will
  // become an owner of it, and we must not delete it.
  MEDSKYLINEARRAY * aNumberSC = new MEDSKYLINEARRAY(2, 3, indexSC, valueSC);
  aSupportOnCells2->setNumber(aNumberSC);

  // check number
  const int * aNumberIndexSC = aSupportOnCells2->getNumberIndex();
  CPPUNIT_ASSERT_EQUAL(1, aNumberIndexSC[0]);
  CPPUNIT_ASSERT_EQUAL(3, aNumberIndexSC[1]);
  CPPUNIT_ASSERT_EQUAL(4, aNumberIndexSC[2]);

  const int * aNbPYRA5 = aSupportOnCells2->getNumber(MED_EN::MED_PYRA5);
  const int * aNbPENTA15 = aSupportOnCells2->getNumber(MED_EN::MED_PENTA15);
  CPPUNIT_ASSERT_THROW(aSupportOnCells2->getNumber(MED_EN::MED_TETRA4), MEDEXCEPTION);

  const int * aNbC = aSupportOnCells2->getNumber(MED_EN::MED_ALL_ELEMENTS);

  CPPUNIT_ASSERT_EQUAL(21, aNbPYRA5[0]);
  CPPUNIT_ASSERT_EQUAL(22, aNbPYRA5[1]);
  CPPUNIT_ASSERT_EQUAL(47, aNbPENTA15[0]);

  CPPUNIT_ASSERT_EQUAL(21, aNbC[0]);
  CPPUNIT_ASSERT_EQUAL(22, aNbC[1]);
  CPPUNIT_ASSERT_EQUAL(47, aNbC[2]);

  // getValIndFromGlobalNumber
  CPPUNIT_ASSERT_EQUAL(1, aSupportOnCells2->getValIndFromGlobalNumber(21));
  CPPUNIT_ASSERT_EQUAL(2, aSupportOnCells2->getValIndFromGlobalNumber(22));
  CPPUNIT_ASSERT_EQUAL(3, aSupportOnCells2->getValIndFromGlobalNumber(47));
  // not existing element
  CPPUNIT_ASSERT_THROW(aSupportOnCells2->getValIndFromGlobalNumber(10), MEDEXCEPTION);

  // compare aSupportOnCells1 and aSupportOnCells2

  // temporary set mesh (because of bug with operator==)
  aSupportOnCells1->setMesh(aMesh);
  aSupportOnCells2->setMesh(aMesh);

  CPPUNIT_ASSERT_EQUAL(*aSupportOnCells1, *aSupportOnCells2);

  // restore mesh name
  aSupportOnCells1->setMesh(NULL);
  aSupportOnCells2->setMesh(NULL);
  aSupportOnCells1->setMeshName("Mesh 1");
  aSupportOnCells2->setMeshName("Mesh 1");
  aSupportOnCells2->removeReference();

  // 3. aSupportOnFacesN is on all elements, and it contains
  //    information about all faces of aMesh
  CPPUNIT_ASSERT_EQUAL(2, aSupportOnFaces1->getNumberOfTypes());
  const MED_EN::medGeometryElement* aSF1Types = aSupportOnFaces1->getTypes();
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_TRIA3, aSF1Types[0]);
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_QUAD4, aSF1Types[1]);
  //#ifdef ENABLE_FORCED_FAILURES
  // (BUG) Comment to method SUPPORT::getTypes() says:
  // "If isOnAllElements is false, returns an array of %medGeometryElement types used by the support."
  // But in fact it returns types in any case (comment to SUPPORT::setAll() gives more information).
  //CPPUNIT_FAIL("Comment and implementation does not match for SUPPORT::getTypes()");
  // This concerns also method SUPPORT::update(), being called from constructor,
  // which fills internal SUPPORT structures with information about mesh elements
  // in case of isOnAllElements == true.
  // Also comments and implementation does not match for SUPPORT::getNumberOfElements().
  // It seems that in case "isOnAllElements == true" calls like getNumberOfElements()
  // must be redirected to mesh, but what if mesh is not set (mesh name used instead)?.
  //#endif
  const int * nbEltsSF1 = aSupportOnFaces1->getNumberOfElements();
  CPPUNIT_ASSERT_EQUAL(4, aSupportOnFaces1->getNumberOfElements(MED_EN::MED_TRIA3));
  CPPUNIT_ASSERT_EQUAL(4, nbEltsSF1[0]);
  CPPUNIT_ASSERT_EQUAL(4, aSupportOnFaces1->getNumberOfElements(MED_EN::MED_QUAD4));
  CPPUNIT_ASSERT_EQUAL(4, nbEltsSF1[1]);
  CPPUNIT_ASSERT_EQUAL(8, aSupportOnFaces1->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS));
  CPPUNIT_ASSERT_THROW(aSupportOnFaces1->getNumberOfElements(MED_EN::MED_TRIA6), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aSupportOnFaces1->getNumberOfElements(MED_EN::MED_QUAD8), MEDEXCEPTION);

  // check type of nodal support
  // (021199: MEDMEM::SUPPORT : geometric type when support is on node)
  {
    const SUPPORT* s = aMesh->getSupportOnAll( MED_EN::MED_NODE );
    CPPUNIT_ASSERT_EQUAL( MED_EN::MED_NONE, s->getTypes()[0] );

    SUPPORT* s2 = new SUPPORT( *s );
    CPPUNIT_ASSERT_EQUAL( MED_EN::MED_NONE, s2->getTypes()[0] );
    const MED_EN::medGeometryElement point1 = MED_EN::MED_POINT1;
    s2->setNumberOfGeometricType( 1 );
    CPPUNIT_ASSERT_THROW( s2->setGeometricType( &point1 ), MEDEXCEPTION );
    s2->removeReference();
  }

  //checking makeMesh()
  MESH *meshFromSupport=aSupportOnFaces1->makeMesh();
  CPPUNIT_ASSERT_EQUAL(4,meshFromSupport->getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_TRIA3));
  CPPUNIT_ASSERT_EQUAL(4,meshFromSupport->getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_QUAD4));
  int nbnodes=  meshFromSupport->getNumberOfNodes();
  const int* conn=meshFromSupport->getConnectivity( MED_EN::MED_NODAL, MED_EN::MED_CELL, MED_EN::MED_TRIA3);
  for (int i=0; i<12;i++)
    CPPUNIT_ASSERT(conn[i]>0 && conn[i]<=nbnodes);
  meshFromSupport->removeReference();
  const SUPPORT *nodal_support=aSupportOnFaces1->getMesh()->getSupportOnAll( MED_EN::MED_NODE );
  CPPUNIT_ASSERT_THROW(nodal_support->makeMesh(), MEDEXCEPTION);
  //checking makeMesh() on polygonal support
  {
    // "poly3D" mesh contains:
    // 19 MED_NODE
    // 6 MED_TRIA3
    // 8 MED_QUAD4
    // 3 MED_POLYGON
    // 1 MED_TETRA4
    // 2 MED_POLYHEDRA
    string med_file = getResourceFile("poly3D.med");
    MESH *poly_mesh=new MESH;
    MED_MESH_RDONLY_DRIVER meshDrv(med_file,poly_mesh);
    meshDrv.setMeshName( "poly3D" );
    meshDrv.desactivateFacesComputation(); // to have nb of faces like in the file
    meshDrv.open();
    meshDrv.read();
    meshDrv.close();
    if ( poly_mesh->getNumberOfElements(MED_EN::MED_FACE, MED_EN::MED_ALL_ELEMENTS) != 17 )
      CPPUNIT_FAIL("Nb of faces in mesh from 'poly3D.med' resource file != 17");

    // support on 3 cells and the mesh made from it
    const SUPPORT *poly_supp=poly_mesh->getSupportOnAll( MED_CELL );
    MESH *poly_supp_mesh=poly_supp->makeMesh();
    CPPUNIT_ASSERT_EQUAL(3,poly_supp_mesh->getNumberOfElements(MED_EN::MED_CELL,
                                                               MED_EN::MED_ALL_ELEMENTS));
    CPPUNIT_ASSERT_EQUAL(1,poly_supp_mesh->getNumberOfElements(MED_EN::MED_CELL,
                                                               MED_EN::MED_TETRA4));
    CPPUNIT_ASSERT_EQUAL(2,poly_supp_mesh->getNumberOfElements(MED_EN::MED_CELL,
                                                               MED_EN::MED_POLYHEDRA));
    int nbnodes=poly_supp_mesh->getNumberOfNodes();
    const int* conn=poly_supp_mesh->getConnectivity(MED_EN::MED_NODAL,
                                                    MED_EN::MED_CELL, MED_EN::MED_ALL_ELEMENTS);
    int con_len = 1 * MED_EN::MED_TETRA4 % 100;
    for (int i=0; i<con_len;i++)
      CPPUNIT_ASSERT(conn[i]>0 && conn[i]<=nbnodes);

    conn = poly_supp_mesh->getConnectivity(MED_EN::MED_NODAL,
                                           MED_EN::MED_CELL,MED_EN::MED_POLYHEDRA);
    con_len = poly_supp_mesh->getConnectivityLength(MED_EN::MED_NODAL,
                                                    MED_EN::MED_CELL,MED_EN::MED_POLYHEDRA);
    for (int i=0; i<con_len;i++)
      CPPUNIT_ASSERT((conn[i]>0 && conn[i]<=nbnodes) || conn[i]==-1 );
    poly_supp_mesh->removeReference();
    // support on 17 faces and the mesh made from it
    SUPPORT *polygon_supp= new SUPPORT( *poly_mesh->getSupportOnAll( MED_EN::MED_FACE ));
    poly_mesh->removeReference();
    MESH *polygon_supp_mesh=polygon_supp->makeMesh();
    CPPUNIT_ASSERT_EQUAL(17,polygon_supp_mesh->getNumberOfElements(MED_EN::MED_CELL,
                                                                   MED_EN::MED_ALL_ELEMENTS));
    CPPUNIT_ASSERT_EQUAL(6,polygon_supp_mesh->getNumberOfElements(MED_EN::MED_CELL,
                                                                  MED_EN::MED_TRIA3));
    CPPUNIT_ASSERT_EQUAL(8,polygon_supp_mesh->getNumberOfElements(MED_EN::MED_CELL,
                                                                  MED_EN::MED_QUAD4));
    CPPUNIT_ASSERT_EQUAL(3,polygon_supp_mesh->getNumberOfElements(MED_EN::MED_CELL,
                                                                  MED_EN::MED_POLYGON));
    nbnodes = polygon_supp_mesh->getNumberOfNodes();
    conn=polygon_supp_mesh->getConnectivity(MED_EN::MED_NODAL,
                                            MED_EN::MED_CELL, MED_EN::MED_ALL_ELEMENTS);
    con_len = 6 * MED_EN::MED_TRIA3 % 100 + 8 * MED_EN::MED_QUAD4 % 100;
    for (int i=0; i<con_len;i++)
      CPPUNIT_ASSERT(conn[i]>0 && conn[i]<=nbnodes);

    conn = polygon_supp_mesh->getConnectivity(MED_EN::MED_NODAL,
                                              MED_EN::MED_CELL,MED_EN::MED_POLYGON);
    con_len = polygon_supp_mesh->getConnectivityLength(MED_EN::MED_NODAL,
                                                       MED_EN::MED_CELL,MED_EN::MED_POLYGON);
    for (int i=0; i<con_len;i++)
      CPPUNIT_ASSERT(conn[i]>0 && conn[i]<=nbnodes);
    polygon_supp_mesh->removeReference();
    // make polygon_supp patrial: make it contain 4 QUAD4 and 2 POLYGON
    const int nb_types = 2;
    const int nb_elems = 4 + 2;
    int index[nb_types+1] = 
      {
        1, 1+4, 1+4+2 
      };
    int elems[nb_elems] = 
      {
        8, 10, 11, 13,  16, 17 
      };
    MED_EN::medGeometryElement types[nb_types] = 
      {
        MED_EN::MED_QUAD4, MED_EN::MED_POLYGON 
      };
    MEDSKYLINEARRAY* array = new MEDSKYLINEARRAY(nb_types, nb_elems, index, elems);
    polygon_supp->setNumberOfGeometricType( nb_types );
    polygon_supp->setGeometricType( types );
    polygon_supp->setpartial( array, /*shallowCopy=*/true);

    // 0020912: [CEA 414] setPartial and polygons/polyhedra
    // do the same but using another version of setpartial()
    const int nb_of_elems[nb_types] = 
      {
        4, 2 
      };
    polygon_supp->setpartial("issue 0020912",nb_types, nb_elems, types, nb_of_elems, elems);

    // make mesh from partial support containing polygons
    MESH *partial_supp_mesh=polygon_supp->makeMesh();
    polygon_supp->removeReference();
    CPPUNIT_ASSERT_EQUAL(6,partial_supp_mesh->getNumberOfElements(MED_EN::MED_CELL,
                                                                  MED_EN::MED_ALL_ELEMENTS));
    CPPUNIT_ASSERT_EQUAL(4,partial_supp_mesh->getNumberOfElements(MED_EN::MED_CELL,types[0]));
    CPPUNIT_ASSERT_EQUAL(2,partial_supp_mesh->getNumberOfElements(MED_EN::MED_CELL,types[1]));

    nbnodes = partial_supp_mesh->getNumberOfNodes();
    int spaceDimension = partial_supp_mesh->getSpaceDimension();
    int meshDimension = partial_supp_mesh->getMeshDimension();
    const double *coords = partial_supp_mesh->getCoordinates(MED_FULL_INTERLACE);
    const double coordsExpected[45]=
      {
        3., 2., 2., 3., 2., 0., 4., 1., 0., 4., 1., 2., 0., 1., 2., 2., 0., 2., 2., 0., 0., 0.,
        1., 0., 1., 2., 2., 1., 2., 0., 7., 2., 2., 5., 3., 2., 6., 3., 0., 7., 2., 0., 6., 0., 2.
      };
    CPPUNIT_ASSERT_EQUAL(3,spaceDimension);
    CPPUNIT_ASSERT_EQUAL(2,meshDimension);
    CPPUNIT_ASSERT_EQUAL(15,nbnodes);
    for(int i=0 ; i<spaceDimension*nbnodes; i++)
      CPPUNIT_ASSERT_DOUBLES_EQUAL(coordsExpected[i],coords[i],1e-15);
    conn=partial_supp_mesh->getConnectivity(MED_EN::MED_NODAL,
                                            MED_EN::MED_CELL, MED_EN::MED_ALL_ELEMENTS);
    con_len = 4 * MED_EN::MED_QUAD4 % 100;
    int connExpected[16]=
      {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 5, 8, 10, 11, 12, 13, 14
      };
    for (int i=0; i<con_len;i++)
      CPPUNIT_ASSERT_EQUAL(connExpected[i],conn[i]);

    conn = partial_supp_mesh->getConnectivity(MED_EN::MED_NODAL,
                                              MED_EN::MED_CELL,MED_EN::MED_POLYGON);
    con_len = partial_supp_mesh->getConnectivityLength(MED_EN::MED_NODAL,
                                                       MED_EN::MED_CELL,MED_EN::MED_POLYGON);
    int connExpectedP[10]=
      {
        7, 3, 2, 10, 8, 12, 11, 15, 4, 1
      };
    for (int i=0; i<con_len;i++)
      CPPUNIT_ASSERT_EQUAL(connExpectedP[i],conn[i]);
    partial_supp_mesh->removeReference();
  }


  // check number
  CPPUNIT_ASSERT_NO_THROW(aSupportOnFaces1->getNumberIndex());
  CPPUNIT_ASSERT_NO_THROW(aSupportOnFaces1->getNumber(MED_EN::MED_TRIA3));
  CPPUNIT_ASSERT_NO_THROW(aSupportOnFaces1->getNumber(MED_EN::MED_QUAD4));
  CPPUNIT_ASSERT_NO_THROW(aSupportOnFaces1->getNumber(MED_EN::MED_ALL_ELEMENTS));
  CPPUNIT_ASSERT_NO_THROW(aSupportOnFaces1->getnumber());

  // getValIndFromGlobalNumber
  CPPUNIT_ASSERT_EQUAL(8, aMesh->getNumberOfElements(MED_EN::MED_FACE, MED_EN::MED_ALL_ELEMENTS));
  for (int i = 1; i <= 8; i++) 
    {
      CPPUNIT_ASSERT_EQUAL(i, aSupportOnFaces1->getValIndFromGlobalNumber(i));
    }

  //#ifdef ENABLE_FORCED_FAILURES
  // Why methods setNumber do not throw exception in case "isOnAllElements == true",
  // like methods getNumber do?
  //skl???  CPPUNIT_ASSERT_THROW(aSupportOnFaces1->setNumber(indexSF1, valueSF1), MEDEXCEPTION);
  //#endif

  //#ifdef ENABLE_FORCED_FAILURES
  // It seems to be dangerous and useless to have method SUPPORT::setTotalNumberOfElements()
  //CPPUNIT_FAIL("It seems to be dangerous and useless to have method SUPPORT::setTotalNumberOfElements()");
  //#endif

  // following test is commented since method
  // setTotalNumberOfElements() is removed.
  // setTotalNumberOfElements
  //aSupportOnFaces1->setTotalNumberOfElements(1000);
  //CPPUNIT_ASSERT_EQUAL(1000, aSupportOnFaces1->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS));
  aSupportOnFaces1->removeReference();
  // clearDataOnNumbers
  aSupportOnCells1->clearDataOnNumbers();
  CPPUNIT_ASSERT_EQUAL(0, aSupportOnCells1->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS));
  CPPUNIT_ASSERT_EQUAL(0, aSupportOnCells1->getNumberOfTypes());
  CPPUNIT_ASSERT_THROW(aSupportOnCells1->getnumber(), MEDEXCEPTION);
  aSupportOnCells1->removeReference();

  /////////////////////////////////////
  // test SUPPORT* buildSupportOnNode()
  /////////////////////////////////////

  for (medEntityMesh entity = 0; entity < MED_ALL_ENTITIES; ++entity )
    {
      if ( entity != MED_NODE && !aMesh->existConnectivity( MED_NODAL, entity ))
        continue;
      // buildSupportOnNode() from full entity support
      const SUPPORT *allEntitySupport=aMesh->getSupportOnAll( entity );
      SUPPORT *testSupport=allEntitySupport->buildSupportOnNode();
      CPPUNIT_ASSERT(testSupport->isOnAllElements());
      CPPUNIT_ASSERT_EQUAL(1,testSupport->getNumberOfTypes());
      CPPUNIT_ASSERT_EQUAL(19,testSupport->getNumberOfElements(MED_ALL_ELEMENTS));

      // buildSupportOnNode() from partial support
      SUPPORT *partialSupport=new SUPPORT(*allEntitySupport);
      partialSupport->setAll( false );
      partialSupport->setNumberOfGeometricType( 1 );
      partialSupport->setGeometricType( allEntitySupport->getTypes() );

      // no element numbers set
      CPPUNIT_ASSERT_THROW(partialSupport->buildSupportOnNode(), MEDEXCEPTION);

      int index[] = 
        {
          1,2
        };
      int numbers[] = 
        {
          1
        };
      partialSupport->setNumberOfElements(numbers);
      partialSupport->setNumber( index, numbers );

      // mesh not set
      partialSupport->setMesh(0);
      CPPUNIT_ASSERT_THROW(partialSupport->buildSupportOnNode(), MEDEXCEPTION);
      partialSupport->setMesh(aMesh);

      testSupport->removeReference();
      testSupport =( partialSupport->buildSupportOnNode() );
      partialSupport->removeReference();
      CPPUNIT_ASSERT(!testSupport->isOnAllElements());
      CPPUNIT_ASSERT_EQUAL(1,testSupport->getNumberOfTypes());
      int nbNodes = testSupport->getNumberOfElements(MED_ALL_ELEMENTS);
      if ( entity == MED_NODE )
        CPPUNIT_ASSERT_EQUAL(1, nbNodes);
      else
        CPPUNIT_ASSERT_EQUAL(aMesh->getCellsTypes(entity)[0].getNumberOfNodes(), nbNodes);
      const int * nodes = testSupport->getNumber( MED_ALL_ELEMENTS );
      switch ( entity )
        {
        case MED_CELL:
          CPPUNIT_ASSERT_EQUAL(1,nodes[0]);
          CPPUNIT_ASSERT_EQUAL(2,nodes[1]);
          CPPUNIT_ASSERT_EQUAL(3,nodes[2]);
          CPPUNIT_ASSERT_EQUAL(6,nodes[3]);
          break;
        case MED_FACE:
          CPPUNIT_ASSERT_EQUAL(1,nodes[0]);
          CPPUNIT_ASSERT_EQUAL(3,nodes[1]);
          CPPUNIT_ASSERT_EQUAL(4,nodes[2]);
          break;
        case MED_NODE:
          CPPUNIT_ASSERT_EQUAL(1,nodes[0]);
          break;
        default:;
        }
      testSupport->removeReference();
    }

  ////////////
  // TEST 2 //
  ////////////

  list<int> nodes137;
  nodes137.push_back(1);
  nodes137.push_back(3);
  nodes137.push_back(7);

  list<int> nodes248;
  nodes248.push_back(2);
  nodes248.push_back(4);
  nodes248.push_back(8);

  list<int> nodes27;
  nodes27.push_back(2);
  nodes27.push_back(7);

  SUPPORT *aNodes137 = new SUPPORT;
  SUPPORT *aNodes248 = new SUPPORT;
  SUPPORT *aNodes27 = new SUPPORT;
  aNodes137->setMesh( aMesh );
  aNodes248->setMesh( aMesh );
  aNodes27->setMesh( aMesh );
  aNodes137->setName( "Support On Nodes 1,3,7" );
  aNodes248->setName( "Support On Nodes 2,4,8" );
  aNodes27->setName( "Support On Nodes 2,7" );

  aNodes137->fillFromNodeList(nodes137);
  aNodes248->fillFromNodeList(nodes248);
  aNodes27->fillFromNodeList(nodes27);

  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_NODE, aNodes137->getEntity());
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_NODE, aNodes248->getEntity());

  list<int>::const_iterator iter137 = nodes137.begin();
  list<int>::const_iterator iter248 = nodes248.begin();
  for (int i = 1; i <= 3; i++, iter137++, iter248++) 
    {
      CPPUNIT_ASSERT_EQUAL(i, aNodes137->getValIndFromGlobalNumber(*iter137));
      CPPUNIT_ASSERT_EQUAL(i, aNodes248->getValIndFromGlobalNumber(*iter248));
    }

  ////////////

  list<int> faces123;
  faces123.push_back(1);
  faces123.push_back(2);
  faces123.push_back(3);

  list<int> faces135;
  faces135.push_back(1);
  faces135.push_back(3);
  faces135.push_back(5);

  SUPPORT *aFaces123 = new SUPPORT;
  aFaces123->setMesh( aMesh );
  aFaces123->setName( "Support On Faces 1,2,3" );
  aFaces123->setEntity( MED_EN::MED_FACE );
  SUPPORT *aFaces135 = new SUPPORT;
  aFaces135->setMesh( aMesh );
  aFaces135->setName( "Support On Faces 1,3,5" );
  aFaces135->setEntity( MED_EN::MED_FACE );

  aFaces123->fillFromElementList(faces123);
  aFaces135->fillFromElementList(faces135);

  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_FACE, aFaces123->getEntity());
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_FACE, aFaces135->getEntity());

  list<int>::const_iterator iter123 = faces123.begin();
  list<int>::const_iterator iter135 = faces135.begin();
  for (int i = 1; i <= 3; i++, iter123++, iter135++) 
    {
      CPPUNIT_ASSERT_EQUAL(i, aFaces123->getValIndFromGlobalNumber(*iter123));
      CPPUNIT_ASSERT_EQUAL(i, aFaces135->getValIndFromGlobalNumber(*iter135));
    }

  // substract
  SUPPORT * aFaces2 = aFaces123->substract(*aFaces135); // => 2
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_FACE, aFaces2->getEntity());
  CPPUNIT_ASSERT_EQUAL(1, aFaces2->getValIndFromGlobalNumber(2));
  CPPUNIT_ASSERT_EQUAL(1, aFaces2->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS));

  CPPUNIT_ASSERT_THROW(aFaces123->substract(*aNodes137), MEDEXCEPTION);

  //#ifdef ENABLE_FORCED_FAILURES
  // (BUG) SUPPORT::substract() does not work with supports on nodes,
  //       because it uses method MESH::buildSupportOnNodeFromElementList(),
  //       which does not work if entity = MED_NODE
  //SUPPORT * aNodes137mesh = aMesh->buildSupportOnNodeFromElementList(nodes137, MED_EN::MED_NODE);
  //delete aNodes137mesh;
  SUPPORT * aNodes137sub = aNodes137->substract(*aNodes248); // => 1,3,7
  CPPUNIT_ASSERT_EQUAL(*aNodes137sub, *aNodes137);
  aNodes137sub->removeReference();
  //#endif

  // belongsTo
  CPPUNIT_ASSERT(aFaces2->belongsTo(*aFaces123));
  CPPUNIT_ASSERT(!aFaces2->belongsTo(*aFaces135));

  CPPUNIT_ASSERT(!aFaces2->belongsTo(*aSupportOnFaces3));
  CPPUNIT_ASSERT(aFaces2->belongsTo(*aSupportOnFaces3, /*deepCompare*/true));

  aFaces2->removeReference();

  // getComplement
  SUPPORT * aFaces_135 = aFaces135->getComplement(); // => 2,4,6,7,8
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_FACE, aFaces_135->getEntity());
  CPPUNIT_ASSERT_EQUAL(5, aFaces_135->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS));

  //#ifdef ENABLE_FORCED_FAILURES
  // (BUG) Memory problem in SUPPORT::sub()
  // Invalid read of size 4
  //    at 0x34902492: MEDMEM::SUPPORT::sub(int, int, int const*, int) (MEDMEM_Support.cxx:852)
  //    by 0x3490296B: MEDMEM::SUPPORT::getComplement() const (MEDMEM_Support.cxx:916)
  //    by 0x34398F1E: MEDMEMTest::testSupport() (MEDMEMTest_Support.cxx:514)
  //  Address 0x352DB91C is 0 bytes after a block of size 12 alloc'd
  //    at 0x3414C8E2: operator new[](unsigned) (vg_replace_malloc.c:138)
  //    by 0x3490243A: MEDMEM::SUPPORT::sub(int, int, int const*, int) (MEDMEM_Support.cxx:847)
  //    by 0x3490296B: MEDMEM::SUPPORT::getComplement() const (MEDMEM_Support.cxx:916)
  //    by 0x34398F1E: MEDMEMTest::testSupport() (MEDMEMTest_Support.cxx:514)
  //CPPUNIT_FAIL("Memory problem in SUPPORT::sub(): Invalid read of size 4 from array <temp>");
  //#endif

  //#ifdef ENABLE_FORCED_FAILURES
  // (BUG) SUPPORT::getComplement() does not work with support on nodes,
  //       because it uses method MESH::buildSupportOnNodeFromElementList(),
  //       which does not work if entity = MED_NODE
  SUPPORT * aNodes_137 = aNodes137->getComplement(); // => 2,4,5,6,8,9,10-19
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_NODE, aNodes_137->getEntity());
  CPPUNIT_ASSERT_EQUAL(16, aNodes_137->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS));
  //#endif

  // intersecting
  aFaces_135->intersecting(aFaces123); // => 2
  aFaces123->removeReference();
  CPPUNIT_ASSERT_EQUAL(1, aFaces_135->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS));
  CPPUNIT_ASSERT_EQUAL(1, aFaces_135->getValIndFromGlobalNumber(2));

  //aNodes_137->intersecting(&aNodes248); // => 2,4,8
  //CPPUNIT_ASSERT_EQUAL(3, aNodes_137->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS));
  //CPPUNIT_ASSERT_EQUAL(*aNodes_137, aNodes248);
  aNodes137->intersecting(aNodes248); // => 
  {
  }
  CPPUNIT_ASSERT_EQUAL(0, aNodes137->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS));
  aNodes137->removeReference();
  // blending
  aFaces_135->blending(aFaces135); // => 1,2,3,5
  CPPUNIT_ASSERT_EQUAL(4, aFaces_135->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS));

  aNodes248->blending(aNodes27); // => 2,4,7,8
  aNodes27->removeReference();
  CPPUNIT_ASSERT_EQUAL(4, aNodes248->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS));

  // operator=
  SUPPORT *aCopy1=new SUPPORT;
  *aCopy1 = *aNodes248;
  CPPUNIT_ASSERT_EQUAL(*aNodes248, *aCopy1);
  aCopy1->removeReference();
  aNodes248->removeReference();
  // copy constructor
  SUPPORT *aCopy2=new SUPPORT (*aFaces135);
  CPPUNIT_ASSERT_EQUAL(*aFaces135, *aCopy2);
  aCopy2->removeReference();

  aFaces_135->removeReference();
  //#ifdef ENABLE_FORCED_FAILURES
  aNodes_137->removeReference();
  //#endif

  // profil names
  vector<string> aPrN2 (2); // nb. types
  aPrN2[0] = "Profil 1";
  aPrN2[1] = "Profil 2";
  aFaces135->setProfilNames(aPrN2);

  // because on all elements
  CPPUNIT_ASSERT_THROW(aSupportOnFaces3->setProfilNames(aPrN2), MEDEXCEPTION);

  // because length of vector is not equal to nb.types
  vector<string> aPrN3 (3);
  CPPUNIT_ASSERT_THROW(aFaces135->setProfilNames(aPrN3), MEDEXCEPTION);

  vector<string> aPrN2Back = aFaces135->getProfilNames();
  CPPUNIT_ASSERT_EQUAL(aPrN2[0], aPrN2Back[0]);
  CPPUNIT_ASSERT_EQUAL(aPrN2[1], aPrN2Back[1]);
  aFaces135->removeReference();
  //#ifdef ENABLE_FORCED_FAILURES
  // (BUG) profil names are not copied by copy constructor and operator=,
  // not compared by operator==, not dumped in stream by operator<<
  //CPPUNIT_FAIL("Not complete implementation for Profil Names");
  //#endif

  ////////////
  // TEST 3 //
  ////////////

  SUPPORT *aPartialCells = new SUPPORT;
  aPartialCells->setMesh( aMesh );
  aPartialCells->setName( "Support On Cells 2,3,4" );
  aPartialCells->setEntity( MED_EN::MED_CELL );

  SUPPORT *aPartialNodes = new SUPPORT;
  aPartialNodes->setMesh( aMesh );
  aPartialNodes->setName( "Support On Nodes 2,3,4" );
  aPartialNodes->setEntity( MED_EN::MED_NODE );

  // setpartial
  MED_EN::medGeometryElement gtCells[1] = 
    {
      MED_EN::MED_TETRA4
    };
  int nbCells[1] = 
    {
      3
    };
  int cells[3] = 
    {
      2,3,4
    };
  aPartialCells->setpartial("with description", 1, 3, gtCells, nbCells, cells);
  CPPUNIT_ASSERT_EQUAL(1, aPartialCells->getValIndFromGlobalNumber(2));
  CPPUNIT_ASSERT_EQUAL(2, aPartialCells->getValIndFromGlobalNumber(3));
  CPPUNIT_ASSERT_EQUAL(3, aPartialCells->getValIndFromGlobalNumber(4));

  MED_EN::medGeometryElement gtNodes[1] = 
    {
      MED_EN::MED_NONE
    };
  aPartialNodes->setNumberOfGeometricType(1);
  aPartialNodes->setGeometricType(gtNodes);
  int indexSN[2] = 
    {
      1,4
    };
  int valueSN[3] = 
    {
      2,3,4
    };
  MEDSKYLINEARRAY * aNumberSN = new MEDSKYLINEARRAY(1, 3, indexSN, valueSN);
  aPartialNodes->setpartial(aNumberSN, /*shallowCopy*/false);
  delete aNumberSN;
  CPPUNIT_ASSERT_EQUAL(1, aPartialNodes->getValIndFromGlobalNumber(2));
  CPPUNIT_ASSERT_EQUAL(2, aPartialNodes->getValIndFromGlobalNumber(3));
  CPPUNIT_ASSERT_EQUAL(3, aPartialNodes->getValIndFromGlobalNumber(4));

  // changeElementsNbs

  // {1,2,3,4,5,6,7,8,9,10} -> {10,9,8,7,6,5,4,3,2,1}
  int renumberingFromOldToNew[10] = 
    {
      10,9,8,7,6,5,4,3,2,1
    };

  CPPUNIT_ASSERT_THROW(aPartialCells->changeElementsNbs
                       (MED_EN::MED_NODE, renumberingFromOldToNew), MEDEXCEPTION);

  aPartialCells->changeElementsNbs(MED_EN::MED_CELL, renumberingFromOldToNew);
  CPPUNIT_ASSERT_EQUAL(3, aPartialCells->getValIndFromGlobalNumber(9));
  CPPUNIT_ASSERT_EQUAL(2, aPartialCells->getValIndFromGlobalNumber(8));
  CPPUNIT_ASSERT_EQUAL(1, aPartialCells->getValIndFromGlobalNumber(7));

  // {1,2,3,4,5,6,7} -> {1,3,5,7,2,4,6}, {8,9,10,11} -> {8,10,9,11}
  int renumberingFromOldToNewP[11] = 
    {
      1,3,5,7,2,4,6,8,10,9,11
    };
  aPartialCells->changeElementsNbs(MED_EN::MED_CELL, renumberingFromOldToNewP);
  CPPUNIT_ASSERT_EQUAL(3, aPartialCells->getValIndFromGlobalNumber(10));
  CPPUNIT_ASSERT_EQUAL(2, aPartialCells->getValIndFromGlobalNumber(8));
  CPPUNIT_ASSERT_EQUAL(1, aPartialCells->getValIndFromGlobalNumber(6));
  aPartialCells->removeReference();
  // this method does not suit for nodes renumbering!
  // {1,2,3} -> {3,2,1}, {4,5,6,7} -> {5,6,7,4}
  //int renumberingFromOldToNewN[3] = {3,2,1};
  //int renumberingFromOldToNewPolyN[4] = {5,6,7,4};
  //aPartialNodes.changeElementsNbs(MED_EN::MED_NODE, renumberingFromOldToNewN,
  //                                3, renumberingFromOldToNewPolyN);
  aPartialNodes->removeReference();
  // getBoundaryElements
  //#ifdef ENABLE_FAULTS
  // (BUG) Segmentation Fault during SUPPORT::getBoundaryElements()
  // ??? May be mesh is not complete ???
  SUPPORT *aBoundaryFaces = new SUPPORT;
  aBoundaryFaces->setMesh( aMesh );
  aBoundaryFaces->setName( "meshing boundary faces" );
  aBoundaryFaces->setEntity( MED_EN::MED_FACE );
  aBoundaryFaces->getBoundaryElements();
  //cout << "aBoundaryFaces:" << endl;
  //cout << aBoundaryFaces << endl;
  aBoundaryFaces->removeReference();
  SUPPORT * aBoundaryNodes = aSupportOnFaces3->getBoundaryElements(MED_EN::MED_NODE);
  //cout << "aSupportOnFaces3:" << endl;
  //cout << aSupportOnFaces3 << endl;
  //cout << "aBoundaryNodes:" << endl;
  //cout << aBoundaryNodes << endl;
  aBoundaryNodes->removeReference();
  aSupportOnFaces3->removeReference();
  //#endif
  //#ifdef ENABLE_FORCED_FAILURES
  //CPPUNIT_FAIL("Segmentation Fault during SUPPORT::getBoundaryElements()");
  // Invalid write of size 4
  //   at 0x3439DFCE: void std::_Construct<int, int>(int*, int const&) (stl_construct.h:81)
  //   by 0x3476E4D1: std::vector<int, std::allocator<int> >::push_back(int const&) (stl_vector.h:560)
  //   by 0x34763DDB: MEDMEM::CONNECTIVITY::calculateReverseNodalConnectivity() (MEDMEM_Connectivity.cxx:1155)
  //   by 0x347655FF: MEDMEM::CONNECTIVITY::calculateDescendingConnectivity() (MEDMEM_Connectivity.cxx:1333)
  //   by 0x3476363E: MEDMEM::CONNECTIVITY::getReverseDescendingConnectivity() (MEDMEM_Connectivity.cxx:1090)
  //   by 0x34772AB6: MEDMEM::CONNECTIVITY::getReverseConnectivity(long, long) (MEDMEM_Connectivity.hxx:503)
  //   by 0x348F4F4B: MEDMEM::MESH::getReverseConnectivity(long, long) const (MEDMEM_Mesh.hxx:848)
  //   by 0x348FF559: MEDMEM::SUPPORT::getBoundaryElements() (MEDMEM_Support.cxx:483)
  //   by 0x3439C8E5: MEDMEMTest::testSupport() (MEDMEMTest_Support.cxx:641)
  //  Address 0x61666A2F is not stack'd, malloc'd or (recently) free'd
  //#endif
  aMesh->removeReference();
  aMeshOneMore->removeReference();
}
