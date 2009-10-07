//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D
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
#include "MEDCouplingMeshFieldFactoryComponentClt.hxx"
#include "MEDCouplingMeshFieldFactoryComponent.hxx"
#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingUMeshClient.hxx"
#include "MEDCouplingFieldDouble.hxx"
#include "MEDCouplingFieldDoubleClient.hxx"
#include <fstream>
#include <pthread.h>

SALOME_TEST::MEDCouplingMeshFieldFactory_ptr SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::_objC;

ParaMEDMEM::MEDCouplingUMesh *SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::_mesh_from_distant=0;

int SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::_argc=0;

char **SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::_argv=0;

CORBA::ORB_var SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::_orb;

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkBaseCorbaFetching()
{
  _orb=CORBA::ORB_init(_argc,_argv);
  std::ifstream ifs(SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildFileNameForIOR().c_str());
  std::string ior;
  ifs >> ior;
  CORBA::Object_var obj=_orb->string_to_object(ior.c_str());
  _objC=SALOME_TEST::MEDCouplingMeshFieldFactory::_narrow(obj);
  CPPUNIT_ASSERT(!CORBA::is_nil(_objC));
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaFetching2D()
{
  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr meshPtr=_objC->get2DMesh();
  _mesh_from_distant=ParaMEDMEM::MEDCouplingUMeshClient::New(meshPtr);
  CPPUNIT_ASSERT_EQUAL(2,_mesh_from_distant->getSpaceDimension());
  CPPUNIT_ASSERT_EQUAL(2,_mesh_from_distant->getMeshDimension());
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkContentOfFetched2DMesh()
{
  ParaMEDMEM::MEDCouplingUMesh *meshRef=SALOME_TEST::MEDCouplingCorbaServBasicsTest::build2DMesh();
  CPPUNIT_ASSERT(_mesh_from_distant->isEqual(meshRef,1e-12));
  meshRef->decrRef();
  _mesh_from_distant->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkMultiFetchingToTestMemoryManagement()
{
  for(int i=0;i<1000;i++)
    {
      SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr meshPtr=_objC->get2DMesh();
      _mesh_from_distant=ParaMEDMEM::MEDCouplingUMeshClient::New(meshPtr);
      ParaMEDMEM::MEDCouplingUMesh *meshRef=SALOME_TEST::MEDCouplingCorbaServBasicsTest::build2DMesh();
      CPPUNIT_ASSERT(_mesh_from_distant->isEqual(meshRef,1e-12));
      meshRef->decrRef();
      _mesh_from_distant->decrRef();
    }
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaFetching3D()
{
  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr meshPtr=_objC->get3DMesh();
  _mesh_from_distant=ParaMEDMEM::MEDCouplingUMeshClient::New(meshPtr);
  CPPUNIT_ASSERT_EQUAL(3,_mesh_from_distant->getSpaceDimension());
  CPPUNIT_ASSERT_EQUAL(3,_mesh_from_distant->getMeshDimension());
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkContentOfFetched3DMesh()
{
  ParaMEDMEM::MEDCouplingUMesh *meshRef=SALOME_TEST::MEDCouplingCorbaServBasicsTest::build3DMesh();
  CPPUNIT_ASSERT(_mesh_from_distant->isEqual(meshRef,1e-12));
  meshRef->decrRef();
  _mesh_from_distant->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaFetching3DSurf()
{
  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr meshPtr=_objC->get3DSurfMesh();
  _mesh_from_distant=ParaMEDMEM::MEDCouplingUMeshClient::New(meshPtr);
  CPPUNIT_ASSERT_EQUAL(3,_mesh_from_distant->getSpaceDimension());
  CPPUNIT_ASSERT_EQUAL(2,_mesh_from_distant->getMeshDimension());
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkContentOfFetched3DSurfMesh()
{
  ParaMEDMEM::MEDCouplingUMesh *meshRef=SALOME_TEST::MEDCouplingCorbaServBasicsTest::build3DSurfMesh();
  CPPUNIT_ASSERT(_mesh_from_distant->isEqual(meshRef,1e-12));
  meshRef->decrRef();
  _mesh_from_distant->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaFetching0D()
{
  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr meshPtr=_objC->get0DMesh();
  _mesh_from_distant=ParaMEDMEM::MEDCouplingUMeshClient::New(meshPtr);
  ParaMEDMEM::MEDCouplingUMesh *meshRef=SALOME_TEST::MEDCouplingCorbaServBasicsTest::build0DMesh();
  CPPUNIT_ASSERT_EQUAL(3,_mesh_from_distant->getSpaceDimension());
  CPPUNIT_ASSERT_EQUAL(0,_mesh_from_distant->getMeshDimension());
  CPPUNIT_ASSERT(_mesh_from_distant->isEqual(meshRef,1e-12));
  meshRef->decrRef();
  _mesh_from_distant->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaFetchingM1D()
{
  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr meshPtr=_objC->getM1DMesh();
  _mesh_from_distant=ParaMEDMEM::MEDCouplingUMeshClient::New(meshPtr);
  ParaMEDMEM::MEDCouplingUMesh *meshRef=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildM1DMesh();
  CPPUNIT_ASSERT_EQUAL(-1,_mesh_from_distant->getMeshDimension());
  CPPUNIT_ASSERT(_mesh_from_distant->isEqual(meshRef,1e-12));
  meshRef->decrRef();
  _mesh_from_distant->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaField2DNTFetching()
{
  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr=_objC->getFieldScalarOn2DNT();
  ParaMEDMEM::MEDCouplingFieldDouble *fieldCpp=ParaMEDMEM::MEDCouplingFieldDoubleClient::New(fieldPtr);
  ParaMEDMEM::MEDCouplingFieldDouble *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildFieldScalarOn2DNT();
  CPPUNIT_ASSERT(fieldCpp->isEqual(refField,1.e-12,1.e-15));
  CPPUNIT_ASSERT_THROW(fieldCpp->setTime(1.23,4,5),INTERP_KERNEL::Exception);
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaField2DNTMultiFetching()
{
  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr=_objC->getFieldScalarOn2DNT();
  fieldPtr->Register();
  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface::_duplicate(fieldPtr);
  ParaMEDMEM::MEDCouplingFieldDouble *fieldCpp=ParaMEDMEM::MEDCouplingFieldDoubleClient::New(fieldPtr);
  ParaMEDMEM::MEDCouplingFieldDouble *fieldCpp2=ParaMEDMEM::MEDCouplingFieldDoubleClient::New(fieldPtr);
  CPPUNIT_ASSERT(fieldCpp->isEqual(fieldCpp2,1.e-12,1.e-15));
  fieldCpp->decrRef();
  fieldCpp2->decrRef();
}

void *SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaField2DNTMultiFetchingMTStatic(void *stack)
{
  char *stackS=(char *)stack;
  CORBA::Object_var obj=_orb->string_to_object(stackS);
  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr=SALOME_MED::MEDCouplingFieldDoubleCorbaInterface::_narrow(obj);
  ParaMEDMEM::MEDCouplingFieldDouble *fieldCpp=ParaMEDMEM::MEDCouplingFieldDoubleClient::New(fieldPtr);
  return fieldCpp;
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaField2DNTMultiFetchingMT()
{
  const int nbOfThreads=8;
  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr=_objC->getFieldScalarOn2DNT();
  //doing nbOfThreads-1 Register.
  for(int i=1;i<nbOfThreads;i++)
    fieldPtr->Register();
  char *ior=_orb->object_to_string(fieldPtr);
  pthread_t threads[nbOfThreads];
   ParaMEDMEM::MEDCouplingFieldDouble *rets[nbOfThreads];
  for(int i=0;i<nbOfThreads;i++)
    pthread_create(&threads[i],0,checkCorbaField2DNTMultiFetchingMTStatic,ior);
  for(int i=0;i<nbOfThreads;i++)
    pthread_join(threads[i],(void **)&rets[i]);
  for(int i=0;i<nbOfThreads-1;i++)
    CPPUNIT_ASSERT(rets[i]->isEqual(rets[i+1],1.e-12,1.e-15));
  for(int i=0;i<nbOfThreads;i++)
    rets[i]->decrRef();
  CORBA::release(fieldPtr);
  CORBA::string_free(ior);
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaFieldNode2DNTFetching()
{
  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr=_objC->getFieldNodeScalarOn2DNT();
  ParaMEDMEM::MEDCouplingFieldDouble *fieldCpp=ParaMEDMEM::MEDCouplingFieldDoubleClient::New(fieldPtr);
  ParaMEDMEM::MEDCouplingFieldDouble *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildFieldNodeScalarOn2DNT();
  CPPUNIT_ASSERT(fieldCpp->isEqual(refField,1.e-12,1.e-15));
  CPPUNIT_ASSERT_THROW(fieldCpp->setTime(1.23,4,5),INTERP_KERNEL::Exception);
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaField3DNTFetching()
{
  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr=_objC->getFieldScalarOn3DNT();
  ParaMEDMEM::MEDCouplingFieldDouble *fieldCpp=ParaMEDMEM::MEDCouplingFieldDoubleClient::New(fieldPtr);
  ParaMEDMEM::MEDCouplingFieldDouble *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildFieldScalarOn3DNT();
  CPPUNIT_ASSERT(fieldCpp->isEqual(refField,1.e-12,1.e-15));
  CPPUNIT_ASSERT_THROW(fieldCpp->setTime(1.23,4,5),INTERP_KERNEL::Exception);
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaField3DSurfWTFetching()
{
  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr=_objC->getFieldScalarOn3DSurfWT();
  ParaMEDMEM::MEDCouplingFieldDouble *fieldCpp=ParaMEDMEM::MEDCouplingFieldDoubleClient::New(fieldPtr);
  ParaMEDMEM::MEDCouplingFieldDouble *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildFieldScalarOn3DSurfWT();
  CPPUNIT_ASSERT(fieldCpp->isEqual(refField,1.e-12,1.e-15));
  int dt,it;
  fieldCpp->getTime(dt,it);
  CPPUNIT_ASSERT_EQUAL(1,dt);
  CPPUNIT_ASSERT_EQUAL(4,it);
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::shutdownServer()
{
  _objC->shutdownOrb();
}
