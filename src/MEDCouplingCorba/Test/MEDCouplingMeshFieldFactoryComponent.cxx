//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
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

#include "MEDCouplingMeshFieldFactoryComponent.hxx"
#include "MEDCouplingFieldDouble.hxx"
#include "MEDCouplingUMesh.hxx"

#include <algorithm>

namespace SALOME_TEST
{
  ParaMEDMEM::MEDCouplingUMesh *MEDCouplingCorbaServBasicsTest::build2DMesh()
  {
    double targetCoords[18]={-0.3,-0.3, 0.2,-0.3, 0.7,-0.3, -0.3,0.2, 0.2,0.2, 0.7,0.2, -0.3,0.7, 0.2,0.7, 0.7,0.7 };
    int targetConn[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
    ParaMEDMEM::MEDCouplingUMesh *targetMesh=ParaMEDMEM::MEDCouplingUMesh::New();
    targetMesh->setMeshDimension(2);
    targetMesh->setName("MyMesh2D");
    targetMesh->allocateCells(5);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,targetConn);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3,targetConn+4);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3,targetConn+7);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,targetConn+10);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,targetConn+14);
    targetMesh->finishInsertingCells();
    ParaMEDMEM::DataArrayDouble *myCoords=ParaMEDMEM::DataArrayDouble::New();
    myCoords->alloc(9,2);
    std::copy(targetCoords,targetCoords+18,myCoords->getPointer());
    targetMesh->setCoords(myCoords);
    myCoords->decrRef();
    return targetMesh;
  }

  ParaMEDMEM::MEDCouplingUMesh *MEDCouplingCorbaServBasicsTest::build3DMesh()
  {
    double targetCoords[81]={ 0., 0., 0., 50., 0., 0. , 200., 0., 0.  , 0., 50., 0., 50., 50., 0. , 200., 50., 0.,   0., 200., 0., 50., 200., 0. , 200., 200., 0. ,
                              0., 0., 50., 50., 0., 50. , 200., 0., 50.  , 0., 50., 50., 50., 50., 50. , 200., 50., 50.,   0., 200., 50., 50., 200., 50. , 200., 200., 50. ,
                              0., 0., 200., 50., 0., 200. , 200., 0., 200.  , 0., 50., 200., 50., 50., 200. , 200., 50., 200.,   0., 200., 200., 50., 200., 200. , 200., 200., 200. };
    int targetConn[64]={0,1,4,3,9,10,13,12, 1,2,5,4,10,11,14,13, 3,4,7,6,12,13,16,15, 4,5,8,7,13,14,17,16,
                        9,10,13,12,18,19,22,21, 10,11,14,13,19,20,23,22, 12,13,16,15,21,22,25,24, 13,14,17,16,22,23,26,25};
    ParaMEDMEM::MEDCouplingUMesh *targetMesh=ParaMEDMEM::MEDCouplingUMesh::New();
    targetMesh->setMeshDimension(3);
    targetMesh->setName("MyMesh3D");
    targetMesh->allocateCells(12);
    for(int i=0;i<8;i++)
      targetMesh->insertNextCell(INTERP_KERNEL::NORM_HEXA8,8,targetConn+8*i);
    targetMesh->finishInsertingCells();
    ParaMEDMEM::DataArrayDouble *myCoords=ParaMEDMEM::DataArrayDouble::New();
    myCoords->alloc(27,3);
    std::copy(targetCoords,targetCoords+81,myCoords->getPointer());
    targetMesh->setCoords(myCoords);
    myCoords->decrRef();
    return targetMesh;
  }

  ParaMEDMEM::MEDCouplingUMesh *MEDCouplingCorbaServBasicsTest::build3DSurfMesh()
  {
    double targetCoords[27]={-0.3,-0.3,0.5, 0.2,-0.3,1., 0.7,-0.3,1.5, -0.3,0.2,0.5, 0.2,0.2,1., 0.7,0.2,1.5, -0.3,0.7,0.5, 0.2,0.7,1., 0.7,0.7,1.5};
    int targetConn[18]={0,3,4,1, 1,4,2, 4,5,2, 6,7,4,3, 7,8,5,4};
    ParaMEDMEM::MEDCouplingUMesh *targetMesh=ParaMEDMEM::MEDCouplingUMesh::New();
    targetMesh->setMeshDimension(2);
    targetMesh->setName("MyMesh3DSurf");
    targetMesh->allocateCells(5);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,targetConn);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3,targetConn+4);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_TRI3,3,targetConn+7);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,targetConn+10);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_QUAD4,4,targetConn+14);
    targetMesh->finishInsertingCells();
    ParaMEDMEM::DataArrayDouble *myCoords=ParaMEDMEM::DataArrayDouble::New();
    myCoords->alloc(9,3);
    std::copy(targetCoords,targetCoords+27,myCoords->getPointer());
    targetMesh->setCoords(myCoords);
    myCoords->setInfoOnComponent(0,"X (m)");
    myCoords->setInfoOnComponent(1,"X (dm)");
    myCoords->setInfoOnComponent(2,"X (m)");
    myCoords->decrRef();
    return targetMesh;
  }

  ParaMEDMEM::MEDCouplingUMesh *MEDCouplingCorbaServBasicsTest::build0DMesh()
  {
    double targetCoords[27]={-0.3,-0.3,0.5, 0.2,-0.3,1., 0.7,-0.3,1.5, -0.3,0.2,0.5, 0.2,0.2,1., 0.7,0.2,1.5, -0.3,0.7,0.5, 0.2,0.7,1., 0.7,0.7,1.5};
    int *targetConn=0;
    ParaMEDMEM::MEDCouplingUMesh *targetMesh=ParaMEDMEM::MEDCouplingUMesh::New();
    targetMesh->setMeshDimension(0);
    targetMesh->allocateCells(8);
    targetMesh->setName("Wonderfull 0D mesh");
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT0,0,targetConn);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT0,0,targetConn);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT0,0,targetConn);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT0,0,targetConn);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT0,0,targetConn);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT0,0,targetConn);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT0,0,targetConn);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT0,0,targetConn);
    targetMesh->finishInsertingCells();
    ParaMEDMEM::DataArrayDouble *myCoords=ParaMEDMEM::DataArrayDouble::New();
    myCoords->alloc(9,3);
    std::copy(targetCoords,targetCoords+27,myCoords->getPointer());
    targetMesh->setCoords(myCoords);
    myCoords->setInfoOnComponent(0,"X (m)");
    myCoords->setInfoOnComponent(1,"YY (Pm)");
    myCoords->setInfoOnComponent(2,"ZZZ (m)");
    myCoords->decrRef();
    //
    targetMesh->checkCoherency();
    return targetMesh;
  }

  ParaMEDMEM::MEDCouplingUMesh *MEDCouplingCorbaServBasicsTest::buildM1DMesh()
  {
    ParaMEDMEM::MEDCouplingUMesh *meshM1D=ParaMEDMEM::MEDCouplingUMesh::New("wonderfull -1 D mesh",-1);
    meshM1D->checkCoherency();
    return meshM1D;
  }

  ParaMEDMEM::MEDCouplingFieldDouble *MEDCouplingCorbaServBasicsTest::buildFieldScalarOn2DNT()
  {
    ParaMEDMEM::MEDCouplingUMesh *mesh=build2DMesh();
    ParaMEDMEM::MEDCouplingFieldDouble *fieldOnCells=ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_CELLS,ParaMEDMEM::NO_TIME);
    fieldOnCells->setName("toto");
    fieldOnCells->setMesh(mesh);
    ParaMEDMEM::DataArrayDouble *array=ParaMEDMEM::DataArrayDouble::New();
    array->alloc(mesh->getNumberOfCells(),6);
    fieldOnCells->setArray(array);
    double *tmp=array->getPointer();
    array->decrRef();
    std::fill(tmp,tmp+mesh->getNumberOfCells()*6,7.);
    mesh->decrRef();
    fieldOnCells->checkCoherency();
    return fieldOnCells;
  }

  ParaMEDMEM::MEDCouplingFieldDouble *MEDCouplingCorbaServBasicsTest::buildFieldNodeScalarOn2DNT()
  {
    ParaMEDMEM::MEDCouplingUMesh *mesh=build2DMesh();
    ParaMEDMEM::MEDCouplingFieldDouble *fieldOnNodes=ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_NODES,ParaMEDMEM::NO_TIME);
    fieldOnNodes->setName("toto2");
    fieldOnNodes->setDescription("my wonderful field toto2");
    fieldOnNodes->setMesh(mesh);
    ParaMEDMEM::DataArrayDouble *array=ParaMEDMEM::DataArrayDouble::New();
    array->alloc(mesh->getNumberOfNodes(),5);
    fieldOnNodes->setArray(array);
    double *tmp=array->getPointer();
    array->decrRef();
    std::fill(tmp,tmp+mesh->getNumberOfNodes()*5,7.1234);
    mesh->decrRef();
    fieldOnNodes->checkCoherency();
    return fieldOnNodes;
  }

  ParaMEDMEM::MEDCouplingFieldDouble *MEDCouplingCorbaServBasicsTest::buildFieldScalarOn3DNT()
  {
    ParaMEDMEM::MEDCouplingUMesh *mesh=build3DMesh();
    ParaMEDMEM::MEDCouplingFieldDouble *fieldOnCells=ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_CELLS,ParaMEDMEM::NO_TIME);
    fieldOnCells->setNature(ParaMEDMEM::ConservativeVolumic);
    fieldOnCells->setName("toto");
    fieldOnCells->setDescription("my wonderful 3D field toto2");
    fieldOnCells->setMesh(mesh);
    ParaMEDMEM::DataArrayDouble *array=ParaMEDMEM::DataArrayDouble::New();
    array->alloc(mesh->getNumberOfCells(),6);
    fieldOnCells->setArray(array);
    double *tmp=array->getPointer();
    array->decrRef();
    std::fill(tmp,tmp+mesh->getNumberOfCells()*6,7.);
    mesh->decrRef();
    fieldOnCells->checkCoherency();
    return fieldOnCells;
  }

  ParaMEDMEM::MEDCouplingFieldDouble *MEDCouplingCorbaServBasicsTest::buildFieldScalarOn3DSurfWT()
  {
    ParaMEDMEM::MEDCouplingUMesh *mesh=build3DSurfMesh();
    ParaMEDMEM::MEDCouplingFieldDouble *fieldOnCells=ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_CELLS,ParaMEDMEM::ONE_TIME);
    fieldOnCells->setName("toto25");
    fieldOnCells->setDescription("my wonderful 3D surf field toto25");
    fieldOnCells->setMesh(mesh);
    ParaMEDMEM::DataArrayDouble *array=ParaMEDMEM::DataArrayDouble::New();
    array->alloc(mesh->getNumberOfCells(),3);
    fieldOnCells->setArray(array);
    double *tmp=array->getPointer();
    array->decrRef();
    std::fill(tmp,tmp+mesh->getNumberOfCells()*3,7.);
    mesh->decrRef();
    fieldOnCells->setTime(6.7,1,4);
    fieldOnCells->checkCoherency();
    return fieldOnCells;
  }

  ParaMEDMEM::MEDCouplingFieldDouble *MEDCouplingCorbaServBasicsTest::buildFieldScalarOn3DSurfCOTI()
  {
    ParaMEDMEM::MEDCouplingUMesh *mesh=build3DSurfMesh();
    ParaMEDMEM::MEDCouplingFieldDouble *fieldOnCells=ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_CELLS,ParaMEDMEM::CONST_ON_TIME_INTERVAL);
    fieldOnCells->setName("toto26");
    fieldOnCells->setDescription("my wonderful 3D surf field toto26");
    fieldOnCells->setMesh(mesh);
    ParaMEDMEM::DataArrayDouble *array=ParaMEDMEM::DataArrayDouble::New();
    array->alloc(mesh->getNumberOfCells(),3);
    fieldOnCells->setArray(array);
    double *tmp=array->getPointer();
    array->decrRef();
    std::fill(tmp,tmp+mesh->getNumberOfCells()*3,7.);
    mesh->decrRef();
    fieldOnCells->setStartTime(6.7,1,4);
    fieldOnCells->setEndTime(7.2,2,8);
    fieldOnCells->checkCoherency();
    return fieldOnCells;
  }

  std::string MEDCouplingCorbaServBasicsTest::buildFileNameForIOR()
  {
    std::string ret;
    ret+=getenv("TMP");
    ret+="/entryPointMEDCouplingCorba.ior";
    return ret;
  }
}
