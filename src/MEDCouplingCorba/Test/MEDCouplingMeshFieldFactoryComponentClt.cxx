// Copyright (C) 2007-2011  CEA/DEN, EDF R&D
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

#include "MEDCouplingMeshFieldFactoryComponentClt.hxx"
#include "MEDCouplingMeshFieldFactoryComponent.hxx"
#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingUMeshClient.hxx"
#include "MEDCouplingExtrudedMesh.hxx"
#include "MEDCouplingExtrudedMeshClient.hxx"
#include "MEDCouplingCMesh.hxx"
#include "MEDCouplingCMeshClient.hxx"
#include "MEDCouplingFieldDouble.hxx"
#include "MEDCouplingFieldDoubleClient.hxx"
#include "MEDCouplingFieldTemplate.hxx"
#include "MEDCouplingFieldTemplateClient.hxx"
#include "MEDCouplingMultiFields.hxx"
#include "MEDCouplingMultiFieldsClient.hxx"
#include "MEDCouplingFieldOverTime.hxx"
#include "MEDCouplingFieldOverTimeClient.hxx"
#include "DataArrayDoubleClient.hxx"
#include "DataArrayIntClient.hxx"
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



void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkContentOfFetched1DMesh()
{
  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr meshPtr=_objC->get1DMesh();
  _mesh_from_distant=ParaMEDMEM::MEDCouplingUMeshClient::New(meshPtr);
  meshPtr->UnRegister();
  CORBA::release(meshPtr);
  CPPUNIT_ASSERT_EQUAL(3,_mesh_from_distant->getSpaceDimension());
  CPPUNIT_ASSERT_EQUAL(1,_mesh_from_distant->getMeshDimension());
  ParaMEDMEM::MEDCouplingUMesh *meshRef=SALOME_TEST::MEDCouplingCorbaServBasicsTest::build1DMesh();
  CPPUNIT_ASSERT(_mesh_from_distant->isEqual(meshRef,1e-12));
  meshRef->decrRef();
  _mesh_from_distant->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaFetching2D()
{
  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr meshPtr=_objC->get2DMesh();
  _mesh_from_distant=ParaMEDMEM::MEDCouplingUMeshClient::New(meshPtr);
  meshPtr->UnRegister();
  CORBA::release(meshPtr);
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
      meshPtr->UnRegister();
      CORBA::release(meshPtr);
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
  meshPtr->UnRegister();
  CORBA::release(meshPtr);
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
  meshPtr->UnRegister();
  CORBA::release(meshPtr);
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
  meshPtr->UnRegister();
  CORBA::release(meshPtr);
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
  meshPtr->UnRegister();
  CORBA::release(meshPtr);
  ParaMEDMEM::MEDCouplingUMesh *meshRef=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildM1DMesh();
  CPPUNIT_ASSERT_EQUAL(-1,_mesh_from_distant->getMeshDimension());
  CPPUNIT_ASSERT(_mesh_from_distant->isEqual(meshRef,1e-12));
  meshRef->decrRef();
  _mesh_from_distant->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaFetchingExtruded()
{
  SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface_ptr meshPtr=_objC->getExtrudedMesh();
  ParaMEDMEM::MEDCouplingExtrudedMesh *meshFromDistant=ParaMEDMEM::MEDCouplingExtrudedMeshClient::New(meshPtr);
  meshPtr->UnRegister();
  CORBA::release(meshPtr);
  ParaMEDMEM::MEDCouplingUMesh *meshRef2;
  ParaMEDMEM::MEDCouplingExtrudedMesh *meshRef=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildExtrudedMesh(meshRef2);
  CPPUNIT_ASSERT(meshFromDistant->isEqual(meshRef,1e-12));
  CPPUNIT_ASSERT(meshFromDistant->getMesh2D()->isEqual(meshRef2,1e-12));
  meshRef2->decrRef();
  meshRef->decrRef();
  meshFromDistant->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaFetchingCMesh()
{
  SALOME_MED::MEDCouplingCMeshCorbaInterface_ptr meshPtr=_objC->getCMesh();
  ParaMEDMEM::MEDCouplingCMesh *meshFromDistant=ParaMEDMEM::MEDCouplingCMeshClient::New(meshPtr);
  meshPtr->UnRegister();
  CORBA::release(meshPtr);
  ParaMEDMEM::MEDCouplingCMesh *meshRef=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildCMesh();
  CPPUNIT_ASSERT(meshFromDistant->isEqual(meshRef,1e-12));
  meshRef->decrRef();
  meshFromDistant->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaField2DNTFetching()
{
  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr=_objC->getFieldScalarOn2DNT();
  ParaMEDMEM::MEDCouplingFieldDouble *fieldCpp=ParaMEDMEM::MEDCouplingFieldDoubleClient::New(fieldPtr);
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
  ParaMEDMEM::MEDCouplingFieldDouble *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildFieldScalarOn2DNT();
  CPPUNIT_ASSERT(fieldCpp->isEqual(refField,1.e-12,1.e-15));
  CPPUNIT_ASSERT_THROW(fieldCpp->setTime(1.23,4,5),INTERP_KERNEL::Exception);
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaField2DNTMultiFetching()
{
  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr=_objC->getFieldScalarOn2DNT();
  ParaMEDMEM::MEDCouplingFieldDouble *fieldCpp=ParaMEDMEM::MEDCouplingFieldDoubleClient::New(fieldPtr);
  ParaMEDMEM::MEDCouplingFieldDouble *fieldCpp2=ParaMEDMEM::MEDCouplingFieldDoubleClient::New(fieldPtr);
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
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
  CORBA::release(fieldPtr);
  return fieldCpp;
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaField2DNTMultiFetchingMT()
{
  const int nbOfThreads=8;
  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr=_objC->getFieldScalarOn2DNT();
  //doing nbOfThreads-1 Register.
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
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
  CORBA::string_free(ior);
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaFieldNode2DNTFetching()
{
  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr=_objC->getFieldNodeScalarOn2DNT();
  ParaMEDMEM::MEDCouplingFieldDouble *fieldCpp=ParaMEDMEM::MEDCouplingFieldDoubleClient::New(fieldPtr);
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
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
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
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
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
  ParaMEDMEM::MEDCouplingFieldDouble *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildFieldScalarOn3DSurfWT();
  CPPUNIT_ASSERT(fieldCpp->isEqual(refField,1.e-12,1.e-15));
  int dt,it;
  fieldCpp->getTime(dt,it);
  CPPUNIT_ASSERT_EQUAL(1,dt);
  CPPUNIT_ASSERT_EQUAL(4,it);
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaField3DSurfCOTIFetching()
{
  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr=_objC->getFieldScalarOn3DSurfCOTI();
  ParaMEDMEM::MEDCouplingFieldDouble *fieldCpp=ParaMEDMEM::MEDCouplingFieldDoubleClient::New(fieldPtr);
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
  ParaMEDMEM::MEDCouplingFieldDouble *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildFieldScalarOn3DSurfCOTI();
  CPPUNIT_ASSERT(fieldCpp->isEqual(refField,1.e-12,1.e-15));
  int dt,it;
  fieldCpp->getStartTime(dt,it);
  CPPUNIT_ASSERT_EQUAL(1,dt);
  CPPUNIT_ASSERT_EQUAL(4,it);
  fieldCpp->getEndTime(dt,it);
  CPPUNIT_ASSERT_EQUAL(2,dt);
  CPPUNIT_ASSERT_EQUAL(8,it);
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaField2DLTFetching()
{
  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr=_objC->getFieldScalarOn2DLT();
  ParaMEDMEM::MEDCouplingFieldDouble *fieldCpp=ParaMEDMEM::MEDCouplingFieldDoubleClient::New(fieldPtr);
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
  double res[4];
  const double pos[2]={0.3,-0.2};
  fieldCpp->getValueOn(pos,10.,res);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(25.4,res[0],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(25.04,res[1],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(25.004,res[2],1e-12);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(25.0004,res[3],1e-12);
  ParaMEDMEM::MEDCouplingFieldDouble *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildFieldScalarOn2DLT();
  CPPUNIT_ASSERT(fieldCpp->isEqual(refField,1.e-12,1.e-15));
  int dt,it;
  CPPUNIT_ASSERT_DOUBLES_EQUAL(6.7,fieldCpp->getStartTime(dt,it),1e-14);
  CPPUNIT_ASSERT_EQUAL(25,dt);
  CPPUNIT_ASSERT_EQUAL(26,it);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(17.2,fieldCpp->getEndTime(dt,it),1e-14);
  CPPUNIT_ASSERT_EQUAL(125,dt);
  CPPUNIT_ASSERT_EQUAL(126,it);
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaFieldGaussPt2DWTFetching()
{
  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr=_objC->getFieldGaussPt2DWT();
  ParaMEDMEM::MEDCouplingFieldDouble *fieldCpp=ParaMEDMEM::MEDCouplingFieldDoubleClient::New(fieldPtr);
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
  //
  ParaMEDMEM::MEDCouplingFieldDouble *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildFieldGaussPt2DWT();
  CPPUNIT_ASSERT(fieldCpp->isEqual(refField,1.e-12,1.e-15));
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaFieldGaussPtNE2DWTFetching()
{
  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr=_objC->getFieldGaussPtNE2DWT();
  ParaMEDMEM::MEDCouplingFieldDouble *fieldCpp=ParaMEDMEM::MEDCouplingFieldDoubleClient::New(fieldPtr);
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
  //
  ParaMEDMEM::MEDCouplingFieldDouble *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildFieldGaussPtNE2DWT();
  CPPUNIT_ASSERT(fieldCpp->isEqual(refField,1.e-12,1.e-15));
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaFieldVectorOnExtrudedWT()
{
  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr=_objC->getFieldVectorOnExtrudedWT();
  ParaMEDMEM::MEDCouplingFieldDouble *fieldCpp=ParaMEDMEM::MEDCouplingFieldDoubleClient::New(fieldPtr);
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
  //
  ParaMEDMEM::MEDCouplingFieldDouble *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildFieldVectorOnExtrudedWT();
  CPPUNIT_ASSERT(fieldCpp->isEqual(refField,1.e-12,1.e-15));
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaFieldVectorOnCMeshWT()
{
  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtr=_objC->getFieldVectorOnCMeshWT();
  ParaMEDMEM::MEDCouplingFieldDouble *fieldCpp=ParaMEDMEM::MEDCouplingFieldDoubleClient::New(fieldPtr);
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
  //
  ParaMEDMEM::MEDCouplingFieldDouble *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildFieldVectorOnCMeshWT();
  CPPUNIT_ASSERT(fieldCpp->isEqual(refField,1.e-12,1.e-15));
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaFieldTemplateCellOn2D()
{
  SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr fieldPtr=_objC->getFieldTemplateCellOn2D();
  ParaMEDMEM::MEDCouplingFieldTemplate *fieldCpp=ParaMEDMEM::MEDCouplingFieldTemplateClient::New(fieldPtr);
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
  //
  ParaMEDMEM::MEDCouplingFieldTemplate *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildFieldTemplateCellOn2D();
  CPPUNIT_ASSERT(fieldCpp->isEqual(refField,1.e-12,1.e-15));
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaFieldTemplateNodeOn2D()
{
  SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr fieldPtr=_objC->getFieldTemplateNodeOn2D();
  ParaMEDMEM::MEDCouplingFieldTemplate *fieldCpp=ParaMEDMEM::MEDCouplingFieldTemplateClient::New(fieldPtr);
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
  //
  ParaMEDMEM::MEDCouplingFieldTemplate *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildFieldTemplateNodeOn2D();
  CPPUNIT_ASSERT(fieldCpp->isEqual(refField,1.e-12,1.e-15));
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaFieldTemplateGaussPtOn2D()
{
  SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr fieldPtr=_objC->getFieldTemplateGaussPtOn2D();
  ParaMEDMEM::MEDCouplingFieldTemplate *fieldCpp=ParaMEDMEM::MEDCouplingFieldTemplateClient::New(fieldPtr);
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
  //
  ParaMEDMEM::MEDCouplingFieldTemplate *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildFieldTemplateGaussPtOn2D();
  CPPUNIT_ASSERT(fieldCpp->isEqual(refField,1.e-12,1.e-15));
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaFieldTemplateGaussNEOn2D()
{
  SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr fieldPtr=_objC->getFieldTemplateGaussNEOn2D();
  ParaMEDMEM::MEDCouplingFieldTemplate *fieldCpp=ParaMEDMEM::MEDCouplingFieldTemplateClient::New(fieldPtr);
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
  //
  ParaMEDMEM::MEDCouplingFieldTemplate *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildFieldTemplateGaussNEOn2D();
  CPPUNIT_ASSERT(fieldCpp->isEqual(refField,1.e-12,1.e-15));
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaMultiFields1()
{
  SALOME_MED::MEDCouplingMultiFieldsCorbaInterface_ptr fieldPtr=_objC->getMultiFields1();
  ParaMEDMEM::MEDCouplingMultiFields *fieldCpp=ParaMEDMEM::MEDCouplingMultiFieldsClient::New(fieldPtr);
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
  //
  ParaMEDMEM::MEDCouplingMultiFields *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildMultiFields1();
  CPPUNIT_ASSERT(fieldCpp->isEqual(refField,1.e-12,1.e-15));
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaArrayDouble1()
{
  SALOME_MED::DataArrayDoubleCorbaInterface_ptr fieldPtr=_objC->getArrayDouble1();
  ParaMEDMEM::DataArrayDouble *fieldCpp=ParaMEDMEM::DataArrayDoubleClient::New(fieldPtr);
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
  //
  ParaMEDMEM::DataArrayDouble *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildArrayDouble1();
  CPPUNIT_ASSERT(fieldCpp->isEqual(*refField,1.e-12));
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaArrayDouble2()
{
  SALOME_MED::DataArrayDoubleCorbaInterface_ptr fieldPtr=_objC->getArrayDouble2();
  ParaMEDMEM::DataArrayDouble *fieldCpp=ParaMEDMEM::DataArrayDoubleClient::New(fieldPtr);
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
  //
  ParaMEDMEM::DataArrayDouble *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildArrayDouble2();
  CPPUNIT_ASSERT(fieldCpp->isEqual(*refField,1.e-12));
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaArrayDouble3()
{
  SALOME_MED::DataArrayDoubleCorbaInterface_ptr fieldPtr=_objC->getArrayDouble3();
  ParaMEDMEM::DataArrayDouble *fieldCpp=ParaMEDMEM::DataArrayDoubleClient::New(fieldPtr);
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
  //
  ParaMEDMEM::DataArrayDouble *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildArrayDouble3();
  CPPUNIT_ASSERT(fieldCpp->isEqual(*refField,1.e-12));
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaArrayInt1()
{
  SALOME_MED::DataArrayIntCorbaInterface_ptr fieldPtr=_objC->getArrayInt1();
  ParaMEDMEM::DataArrayInt *fieldCpp=ParaMEDMEM::DataArrayIntClient::New(fieldPtr);
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
  //
  ParaMEDMEM::DataArrayInt *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildArrayInt1();
  CPPUNIT_ASSERT(fieldCpp->isEqual(*refField));
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaArrayInt2()
{
  SALOME_MED::DataArrayIntCorbaInterface_ptr fieldPtr=_objC->getArrayInt2();
  ParaMEDMEM::DataArrayInt *fieldCpp=ParaMEDMEM::DataArrayIntClient::New(fieldPtr);
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
  //
  ParaMEDMEM::DataArrayInt *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildArrayInt2();
  CPPUNIT_ASSERT(fieldCpp->isEqual(*refField));
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaArrayInt3()
{
  SALOME_MED::DataArrayIntCorbaInterface_ptr fieldPtr=_objC->getArrayInt3();
  ParaMEDMEM::DataArrayInt *fieldCpp=ParaMEDMEM::DataArrayIntClient::New(fieldPtr);
  fieldPtr->UnRegister();
  CORBA::release(fieldPtr);
  //
  ParaMEDMEM::DataArrayInt *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildArrayInt3();
  CPPUNIT_ASSERT(fieldCpp->isEqual(*refField));
  refField->decrRef();
  fieldCpp->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaFetchingCoords1()
{
  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr meshPtr=_objC->get3DSurfMesh();
  SALOME_MED::DataArrayDoubleCorbaInterface_ptr cooPtr=meshPtr->getCoords();
  meshPtr->UnRegister();
  CORBA::release(meshPtr);
  ParaMEDMEM::DataArrayDouble *cooCpp=ParaMEDMEM::DataArrayDoubleClient::New(cooPtr);
  ParaMEDMEM::MEDCouplingUMesh *meshRef=SALOME_TEST::MEDCouplingCorbaServBasicsTest::build3DSurfMesh();
  //
  CPPUNIT_ASSERT(meshRef->getCoords()->isEqual(*cooCpp,1e-12));
  //
  cooPtr->UnRegister();
  CORBA::release(cooPtr);
  cooCpp->decrRef();
  meshRef->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::checkCorbaMultiFields2()
{
  SALOME_MED::MEDCouplingFieldOverTimeCorbaInterface_ptr fotPtr=_objC->getMultiFields2();
  ParaMEDMEM::MEDCouplingFieldOverTimeClient *fotc=ParaMEDMEM::MEDCouplingFieldOverTimeClient::New(fotPtr);
  fotPtr->UnRegister();
  CORBA::release(fotPtr);
  //
  ParaMEDMEM::MEDCouplingFieldOverTime *refField=SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildMultiFields2();
  CPPUNIT_ASSERT(fotc->isEqual(refField,1.e-12,1.e-15));
  refField->decrRef();
  std::vector<double> ts=fotc->getTimeSteps();
  CPPUNIT_ASSERT_EQUAL(6,(int)ts.size());
  const double expected[6]={0.2,0.7,1.2,1.35,1.7,2.7};
  for(int i=0;i<6;i++)
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected[i],ts[i],1e-12);
  fotc->decrRef();
}

void SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::shutdownServer()
{
  _objC->shutdownOrb();
}
