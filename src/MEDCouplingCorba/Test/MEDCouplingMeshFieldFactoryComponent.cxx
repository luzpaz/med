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
#include "MEDCouplingExtrudedMesh.hxx"
#include "MEDCouplingFieldDouble.hxx"
#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingCMesh.hxx"

#include <cmath>
#include <algorithm>

namespace SALOME_TEST
{
  ParaMEDMEM::MEDCouplingUMesh *MEDCouplingCorbaServBasicsTest::build1DMesh()
  {
    double coords[4]={ 0.0, 0.3, 0.75, 1.0 };
    int conn[2*3]={ 0,1, 1,2, 2,3 };
    ParaMEDMEM::MEDCouplingUMesh *mesh=ParaMEDMEM::MEDCouplingUMesh::New("1DMeshForCorba",1);
    mesh->allocateCells(3);
    mesh->insertNextCell(INTERP_KERNEL::NORM_SEG2,2,conn);
    mesh->insertNextCell(INTERP_KERNEL::NORM_SEG2,2,conn+2);
    mesh->insertNextCell(INTERP_KERNEL::NORM_SEG2,2,conn+4);
    mesh->finishInsertingCells();
    ParaMEDMEM::DataArrayDouble *myCoords=ParaMEDMEM::DataArrayDouble::New();
    myCoords->alloc(4,1);
    std::copy(coords,coords+4,myCoords->getPointer());
    mesh->setCoords(myCoords);
    myCoords->decrRef();
    mesh->changeSpaceDimension(3);
    myCoords=mesh->getCoords();
    myCoords->setInfoOnComponent(0,"X1D(m)");
    myCoords->setInfoOnComponent(1,"Y1D (dm)");
    myCoords->setInfoOnComponent(2,"Z1D (pm)");
    double center[3]={0.,0.,0.};
    double vector[3]={0,1,0};
    mesh->rotate(center,vector,-M_PI/2.);
    return mesh;
  }

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
    const int targetConn[]={0,1,2,3,4,5,7,6};
    ParaMEDMEM::MEDCouplingUMesh *targetMesh=ParaMEDMEM::MEDCouplingUMesh::New();
    targetMesh->setMeshDimension(0);
    targetMesh->allocateCells(8);
    targetMesh->setName("Wonderfull 0D mesh");
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT1,1,targetConn);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT1,1,targetConn+1);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT1,1,targetConn+2);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT1,1,targetConn+3);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT1,1,targetConn+4);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT1,1,targetConn+5);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT1,1,targetConn+6);
    targetMesh->insertNextCell(INTERP_KERNEL::NORM_POINT1,1,targetConn+7);
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

  ParaMEDMEM::MEDCouplingExtrudedMesh *MEDCouplingCorbaServBasicsTest::buildExtrudedMesh(ParaMEDMEM::MEDCouplingUMesh *&m2D)
  {
    m2D=build2DMesh();
    m2D->changeSpaceDimension(3);
    ParaMEDMEM::MEDCouplingUMesh *m1D=build1DMesh();
    ParaMEDMEM::MEDCouplingUMesh *retu=m2D->buildExtrudedMesh(m1D,0);
    m1D->decrRef();
    ParaMEDMEM::MEDCouplingExtrudedMesh *ret=ParaMEDMEM::MEDCouplingExtrudedMesh::New(retu,m2D,2);
    ret->setName("ExtrudedTestForCorbaTest");
    retu->decrRef();
    return ret;
  }

  ParaMEDMEM::MEDCouplingCMesh *MEDCouplingCorbaServBasicsTest::buildCMesh()
  {
    ParaMEDMEM::MEDCouplingCMesh *targetMesh=ParaMEDMEM::MEDCouplingCMesh::New();
    targetMesh->setName("Example of CMesh");
    ParaMEDMEM::DataArrayDouble *a1=ParaMEDMEM::DataArrayDouble::New();
    a1->alloc(5,1);
    a1->setInfoOnComponent(0,"SmthX");
    const double a1Data[5]={3.,4.,5.,6.,7.};
    std::copy(a1Data,a1Data+5,a1->getPointer());
    ParaMEDMEM::DataArrayDouble *a2=ParaMEDMEM::DataArrayDouble::New();
    a2->alloc(6,1);
    a2->setInfoOnComponent(0,"SmthZ");
    const double a2Data[6]={2.78,3.,4.,5.,6.,7.};
    std::copy(a2Data,a2Data+6,a2->getPointer());
    //
    targetMesh->setCoordsAt(0,a1);
    targetMesh->setCoordsAt(2,a2);
    //
    a1->decrRef();
    a2->decrRef();
    //
    targetMesh->checkCoherency();
    //
    return targetMesh;
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

  ParaMEDMEM::MEDCouplingFieldDouble *MEDCouplingCorbaServBasicsTest::buildFieldScalarOn2DLT()
  {
    ParaMEDMEM::MEDCouplingUMesh *mesh=build2DMesh();
    ParaMEDMEM::MEDCouplingFieldDouble *fieldOnCells=ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_CELLS,ParaMEDMEM::LINEAR_TIME);
    fieldOnCells->setName("toto27");
    fieldOnCells->setDescription("my wonderful 2D field toto27");
    fieldOnCells->setMesh(mesh);
    ParaMEDMEM::DataArrayDouble *array1=ParaMEDMEM::DataArrayDouble::New();
    array1->alloc(mesh->getNumberOfCells(),4);
    fieldOnCells->setArray(array1);
    double *tmp=array1->getPointer();
    array1->decrRef();
    const double arr1[20]={1.2,1.02,1.002,1.0002, 3.4,3.04,3.004,3.0004, 5.6,5.06,5.006,5.0006, 7.8,7.08,7.008,7.0008, 9.1,9.01,9.001,9.0001};
    std::copy(arr1,arr1+20,tmp);
    ParaMEDMEM::DataArrayDouble *array2=ParaMEDMEM::DataArrayDouble::New();
    array2->alloc(mesh->getNumberOfCells(),4);
    fieldOnCells->setEndArray(array2);
    tmp=array2->getPointer();
    array2->decrRef();
    mesh->decrRef();
    const double arr2[20]={71.2,71.02,71.002,71.0002, 73.4,73.04,73.004,73.0004, 75.6,75.06,75.006,75.0006, 77.8,77.08,77.008,77.0008, 79.1,79.01,79.001,79.0001};
    std::copy(arr2,arr2+20,tmp);
    fieldOnCells->setStartTime(6.7,25,26);
    fieldOnCells->setEndTime(17.2,125,126);
    fieldOnCells->checkCoherency();
    return fieldOnCells;
  }

  ParaMEDMEM::MEDCouplingFieldDouble *MEDCouplingCorbaServBasicsTest::buildFieldGaussPt2DWT()
  {
    const double _a=0.446948490915965;
    const double _b=0.091576213509771;
    const double _p1=0.11169079483905;
    const double _p2=0.0549758718227661;
    const double refCoo1[6]={ 0.,0., 1.,0., 0.,1. };
    const double gsCoo1[12]={ 2*_b-1, 1-4*_b, 2*_b-1, 2.07*_b-1, 1-4*_b,
                              2*_b-1, 1-4*_a, 2*_a-1, 2*_a-1, 1-4*_a, 2*_a-1, 2*_a-1 };
    const double wg1[6]={ 4*_p2, 4*_p2, 4*_p2, 4*_p1, 4*_p1, 4*_p1 };
    std::vector<double> _refCoo1(refCoo1,refCoo1+6);
    std::vector<double> _gsCoo1(gsCoo1,gsCoo1+12);
    std::vector<double> _wg1(wg1,wg1+6);
    ParaMEDMEM::MEDCouplingUMesh *m=build2DMesh();
    ParaMEDMEM::MEDCouplingFieldDouble *f=ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_GAUSS_PT,ParaMEDMEM::ONE_TIME);
    f->setTime(6.7,1,4);
    f->setMesh(m);
    m->decrRef();
    f->setGaussLocalizationOnType(INTERP_KERNEL::NORM_TRI3,_refCoo1,_gsCoo1,_wg1);
    const double refCoo2[8]={ 0.,0., 1.,0., 1.,1., 0.,1. };
    std::vector<double> _refCoo2(refCoo2,refCoo2+8);
    _gsCoo1.resize(4); _wg1.resize(2);
    f->setGaussLocalizationOnType(INTERP_KERNEL::NORM_QUAD4,_refCoo2,_gsCoo1,_wg1);
    ParaMEDMEM::DataArrayDouble *array=ParaMEDMEM::DataArrayDouble::New();
    array->alloc(18,2);
    array->setInfoOnComponent(0,"Power(MW)");
    array->setInfoOnComponent(1,"Density (kg/m^3)");
    double *ptr=array->getPointer();
    for(int i=0;i<18*2;i++)
      ptr[i]=(double)(i+1);
    f->setArray(array);
    f->setName("MyFirstFieldOnGaussPoint");
    f->setDescription("mmmmmmmmmmmm");
    array->decrRef();
    f->checkCoherency();
    return f;
  }

  ParaMEDMEM::MEDCouplingFieldDouble *MEDCouplingCorbaServBasicsTest::buildFieldGaussPtNE2DWT()
  {
    ParaMEDMEM::MEDCouplingUMesh *m=build2DMesh();
    ParaMEDMEM::MEDCouplingFieldDouble *f=ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_GAUSS_NE,ParaMEDMEM::ONE_TIME);
    f->setTime(6.8,11,8);
    f->setMesh(m);
    f->setName("MyFirstFieldOnNE");
    f->setDescription("MyDescriptionNE");
    ParaMEDMEM::DataArrayDouble *array=ParaMEDMEM::DataArrayDouble::New();
    array->alloc(18,2);
    array->setInfoOnComponent(0,"Power(MW)");
    array->setInfoOnComponent(1,"Density (kg/m^3)");
    double *ptr=array->getPointer();
    for(int i=0;i<18*2;i++)
      ptr[i]=(double)(i+7);
    f->setArray(array);
    array->decrRef();
    //
    f->checkCoherency();
    m->decrRef();
    return f;
  }

  ParaMEDMEM::MEDCouplingFieldDouble *MEDCouplingCorbaServBasicsTest::buildFieldVectorOnExtrudedWT()
  {
    ParaMEDMEM::MEDCouplingUMesh *m2D=0;
    ParaMEDMEM::MEDCouplingExtrudedMesh *ext=buildExtrudedMesh(m2D);
    //
    ParaMEDMEM::MEDCouplingFieldDouble *f=ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_CELLS,ParaMEDMEM::ONE_TIME);
    f->setTime(6.8,11,8);
    f->setMesh(ext);
    f->setName("MyFieldOnExtruM");
    f->setDescription("desc of MyFiOnExtruM");
    ParaMEDMEM::DataArrayDouble *array=ParaMEDMEM::DataArrayDouble::New();
    int nbOfCells=ext->getNumberOfCells();
    array->alloc(nbOfCells,2);
    array->setInfoOnComponent(0,"Power(MW)");
    array->setInfoOnComponent(1,"Density (kg/m^3)");
    double *ptr=array->getPointer();
    for(int i=0;i<nbOfCells*2;i++)
      ptr[i]=(double)(i/2+7)+(double)((i%2)*1000);
    f->setArray(array);
    array->decrRef();
    //
    f->checkCoherency();
    //
    m2D->decrRef();
    ext->decrRef();
    return f;
  }

  ParaMEDMEM::MEDCouplingFieldDouble *MEDCouplingCorbaServBasicsTest::buildFieldVectorOnCMeshWT()
  {
    ParaMEDMEM::MEDCouplingCMesh *m=buildCMesh();
    ParaMEDMEM::MEDCouplingFieldDouble *f=ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_CELLS,ParaMEDMEM::ONE_TIME);
    f->setTime(6.8,11,8);
    f->setMesh(m);
    m->decrRef();
    f->setName("MyFieldOnCMesh");
    f->setDescription("desc of MyFiOnCMesh");
    ParaMEDMEM::DataArrayDouble *array=ParaMEDMEM::DataArrayDouble::New();
    int nbOfCells=m->getNumberOfCells();
    array->alloc(nbOfCells,2);
    array->setInfoOnComponent(0,"Power(GW)");
    array->setInfoOnComponent(1,"Density (kg/m^3)");
    double *ptr=array->getPointer();
    for(int i=0;i<nbOfCells*2;i++)
      ptr[i]=(double)(i/2+7)+(double)((i%2)*1000);
    f->setArray(array);
    array->decrRef();
    //
    f->checkCoherency();
    //
    return f;
  }

  std::string MEDCouplingCorbaServBasicsTest::buildFileNameForIOR()
  {
    std::string ret;
    ret+=getenv("TMP");
    ret+="/entryPointMEDCouplingCorba.ior";
    return ret;
  }
}
