// Copyright (C) 2007-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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
// Author : Anthony Geay (CEA/DEN)

#include "MEDCouplingMeshFieldFactoryComponentServ.hxx"
#include "MEDCouplingMeshFieldFactoryComponent.hxx"
#include "MEDCouplingFieldDoubleServant.hxx"
#include "MEDCouplingFieldTemplateServant.hxx"
#include "MEDCouplingMultiFieldsServant.hxx"
#include "MEDCouplingFieldOverTimeServant.hxx"
#include "MEDCouplingExtrudedMeshServant.hxx"
#include "MEDCouplingCurveLinearMeshServant.hxx"
#include "MEDCoupling1SGTUMeshServant.hxx"
#include "MEDCoupling1DGTUMeshServant.hxx"
#include "MEDCouplingCMeshServant.hxx"
#include "MEDCouplingIMeshServant.hxx"
#include "MEDCouplingUMeshServant.hxx"
#include "DataArrayDoubleServant.hxx"
#include "DataArrayIntServant.hxx"
#include "MEDCouplingFieldDouble.hxx"
#include "MEDCouplingFieldTemplate.hxx"
#include "MEDCouplingMultiFields.hxx"
#include "MEDCouplingFieldOverTime.hxx"
#include "MEDCouplingExtrudedMesh.hxx"
#include "MEDCouplingCurveLinearMesh.hxx"
#include "MEDCoupling1GTUMesh.hxx"
#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingCMesh.hxx"
#include "MEDCouplingIMesh.hxx"

namespace SALOME_TEST
{
  MEDCouplingMeshFieldFactoryComponent::MEDCouplingMeshFieldFactoryComponent(CORBA::ORB_ptr orb):_orb(orb)
  {
  }

  void MEDCouplingMeshFieldFactoryComponent::destroyFactory()
  {
    PortableServer::ObjectId_var oid = _default_POA()->servant_to_id(this);
    _default_POA()->deactivate_object(oid);
    _remove_ref();
  }

  void MEDCouplingMeshFieldFactoryComponent::shutdownOrb()
  {
    _orb->shutdown(0);
  }

  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::get1DMesh()
  {
    ParaMEDMEM::MEDCouplingUMesh *m1=MEDCouplingCorbaServBasicsTest::build1DMesh();
    ParaMEDMEM::MEDCouplingUMeshServant *m=new ParaMEDMEM::MEDCouplingUMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::get2DMesh()
  {
    ParaMEDMEM::MEDCouplingUMesh *m1=MEDCouplingCorbaServBasicsTest::build2DMesh();
    ParaMEDMEM::MEDCouplingUMeshServant *m=new ParaMEDMEM::MEDCouplingUMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::get3DMesh()
  {
    ParaMEDMEM::MEDCouplingUMesh *m1=MEDCouplingCorbaServBasicsTest::build3DMesh();
    ParaMEDMEM::MEDCouplingUMeshServant *m=new ParaMEDMEM::MEDCouplingUMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::get3DSurfMesh()
  {
    ParaMEDMEM::MEDCouplingUMesh *m1=MEDCouplingCorbaServBasicsTest::build3DSurfMesh();
    ParaMEDMEM::MEDCouplingUMeshServant *m=new ParaMEDMEM::MEDCouplingUMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::get0DMesh()
  {
    ParaMEDMEM::MEDCouplingUMesh *m1=MEDCouplingCorbaServBasicsTest::build0DMesh();
    ParaMEDMEM::MEDCouplingUMeshServant *m=new ParaMEDMEM::MEDCouplingUMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getM1DMesh()
  {
    ParaMEDMEM::MEDCouplingUMesh *m1=MEDCouplingCorbaServBasicsTest::buildM1DMesh();
    ParaMEDMEM::MEDCouplingUMeshServant *m=new ParaMEDMEM::MEDCouplingUMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getExtrudedMesh()
  {
    ParaMEDMEM::MEDCouplingUMesh *m2D;
    ParaMEDMEM::MEDCouplingExtrudedMesh *m1=MEDCouplingCorbaServBasicsTest::buildExtrudedMesh(m2D);
    m2D->decrRef();
    ParaMEDMEM::MEDCouplingExtrudedMeshServant *m=new ParaMEDMEM::MEDCouplingExtrudedMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingCMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getCMesh()
  {
    ParaMEDMEM::MEDCouplingCMesh *m1=MEDCouplingCorbaServBasicsTest::buildCMesh();
    ParaMEDMEM::MEDCouplingCMeshServant *m=new ParaMEDMEM::MEDCouplingCMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCouplingCMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }
  
  SALOME_MED::MEDCouplingIMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getIMesh()
  {
    ParaMEDMEM::MEDCouplingIMesh *m1=MEDCouplingCorbaServBasicsTest::buildIMesh();
    ParaMEDMEM::MEDCouplingIMeshServant *m=new ParaMEDMEM::MEDCouplingIMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCouplingIMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingCurveLinearMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getCLMesh()
  {
    ParaMEDMEM::MEDCouplingCurveLinearMesh *m1=MEDCouplingCorbaServBasicsTest::buildCLMesh();
    ParaMEDMEM::MEDCouplingCurveLinearMeshServant *m=new ParaMEDMEM::MEDCouplingCurveLinearMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCouplingCurveLinearMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCoupling1SGTUMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::get1SGTUMesh()
  {
    ParaMEDMEM::MEDCoupling1SGTUMesh *m1=MEDCouplingCorbaServBasicsTest::build1SGTUMesh();
    ParaMEDMEM::MEDCoupling1SGTUMeshServant *m=new ParaMEDMEM::MEDCoupling1SGTUMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCoupling1SGTUMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCoupling1DGTUMeshCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::get1DGTUMesh()
  {
    ParaMEDMEM::MEDCoupling1DGTUMesh *m1=MEDCouplingCorbaServBasicsTest::build1DGTUMesh();
    ParaMEDMEM::MEDCoupling1DGTUMeshServant *m=new ParaMEDMEM::MEDCoupling1DGTUMeshServant(m1);
    m1->decrRef();
    SALOME_MED::MEDCoupling1DGTUMeshCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldScalarOn2DNT()
  {
    ParaMEDMEM::MEDCouplingFieldDouble *field=MEDCouplingCorbaServBasicsTest::buildFieldScalarOn2DNT();
    ParaMEDMEM::MEDCouplingFieldDoubleServant *m=new ParaMEDMEM::MEDCouplingFieldDoubleServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldNodeScalarOn2DNT()
  {
    ParaMEDMEM::MEDCouplingFieldDouble *field=MEDCouplingCorbaServBasicsTest::buildFieldNodeScalarOn2DNT();
    ParaMEDMEM::MEDCouplingFieldDoubleServant *m=new ParaMEDMEM::MEDCouplingFieldDoubleServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldScalarOn3DNT()
  {
    ParaMEDMEM::MEDCouplingFieldDouble *field=MEDCouplingCorbaServBasicsTest::buildFieldScalarOn3DNT();
    ParaMEDMEM::MEDCouplingFieldDoubleServant *m=new ParaMEDMEM::MEDCouplingFieldDoubleServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldScalarOn3DSurfWT()
  {
    ParaMEDMEM::MEDCouplingFieldDouble *field=MEDCouplingCorbaServBasicsTest::buildFieldScalarOn3DSurfWT();
    ParaMEDMEM::MEDCouplingFieldDoubleServant *m=new ParaMEDMEM::MEDCouplingFieldDoubleServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr ret=m->_this();
    return ret;
  }
  
  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldScalarOn3DSurfCOTI()
  {
    ParaMEDMEM::MEDCouplingFieldDouble *field=MEDCouplingCorbaServBasicsTest::buildFieldScalarOn3DSurfCOTI();
    ParaMEDMEM::MEDCouplingFieldDoubleServant *m=new ParaMEDMEM::MEDCouplingFieldDoubleServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldScalarOn2DLT()
  {
    ParaMEDMEM::MEDCouplingFieldDouble *field=MEDCouplingCorbaServBasicsTest::buildFieldScalarOn2DLT();
    ParaMEDMEM::MEDCouplingFieldDoubleServant *m=new ParaMEDMEM::MEDCouplingFieldDoubleServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldGaussPt2DWT()
  {
    ParaMEDMEM::MEDCouplingFieldDouble *field=MEDCouplingCorbaServBasicsTest::buildFieldGaussPt2DWT();
    ParaMEDMEM::MEDCouplingFieldDoubleServant *m=new ParaMEDMEM::MEDCouplingFieldDoubleServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldGaussPtNE2DWT()
  {
    ParaMEDMEM::MEDCouplingFieldDouble *field=MEDCouplingCorbaServBasicsTest::buildFieldGaussPtNE2DWT();
    ParaMEDMEM::MEDCouplingFieldDoubleServant *m=new ParaMEDMEM::MEDCouplingFieldDoubleServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldVectorOnExtrudedWT()
  {
    ParaMEDMEM::MEDCouplingFieldDouble *field=MEDCouplingCorbaServBasicsTest::buildFieldVectorOnExtrudedWT();
    ParaMEDMEM::MEDCouplingFieldDoubleServant *m=new ParaMEDMEM::MEDCouplingFieldDoubleServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldVectorOnCMeshWT()
  {
    ParaMEDMEM::MEDCouplingFieldDouble *field=MEDCouplingCorbaServBasicsTest::buildFieldVectorOnCMeshWT();
    ParaMEDMEM::MEDCouplingFieldDoubleServant *m=new ParaMEDMEM::MEDCouplingFieldDoubleServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr ret=m->_this();
    return ret;
  }
  
  SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldTemplateCellOn2D()
  {
    ParaMEDMEM::MEDCouplingFieldTemplate *field=MEDCouplingCorbaServBasicsTest::buildFieldTemplateCellOn2D();
    ParaMEDMEM::MEDCouplingFieldTemplateServant *m=new ParaMEDMEM::MEDCouplingFieldTemplateServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr ret=m->_this();
    return ret;
  }
  
  SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldTemplateNodeOn2D()
  {
    ParaMEDMEM::MEDCouplingFieldTemplate *field=MEDCouplingCorbaServBasicsTest::buildFieldTemplateNodeOn2D();
    ParaMEDMEM::MEDCouplingFieldTemplateServant *m=new ParaMEDMEM::MEDCouplingFieldTemplateServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr ret=m->_this();
    return ret;
  }
  
  SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldTemplateGaussPtOn2D()
  {
    ParaMEDMEM::MEDCouplingFieldTemplate *field=MEDCouplingCorbaServBasicsTest::buildFieldTemplateGaussPtOn2D();
    ParaMEDMEM::MEDCouplingFieldTemplateServant *m=new ParaMEDMEM::MEDCouplingFieldTemplateServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getFieldTemplateGaussNEOn2D()
  {
    ParaMEDMEM::MEDCouplingFieldTemplate *field=MEDCouplingCorbaServBasicsTest::buildFieldTemplateGaussNEOn2D();
    ParaMEDMEM::MEDCouplingFieldTemplateServant *m=new ParaMEDMEM::MEDCouplingFieldTemplateServant(field);
    field->decrRef();
    SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr ret=m->_this();
    return ret;
  }

  SALOME_MED::MEDCouplingMultiFieldsCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getMultiFields1()
  {
    ParaMEDMEM::MEDCouplingMultiFields *fields=MEDCouplingCorbaServBasicsTest::buildMultiFields1();
    ParaMEDMEM::MEDCouplingMultiFieldsServant *s=new ParaMEDMEM::MEDCouplingMultiFieldsServant(fields);
    fields->decrRef();
    SALOME_MED::MEDCouplingMultiFieldsCorbaInterface_ptr ret=s->_this();
    return ret;
  }

  SALOME_MED::DataArrayDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getArrayDouble1()
  {
    ParaMEDMEM::DataArrayDouble *retCpp=MEDCouplingCorbaServBasicsTest::buildArrayDouble1();
    ParaMEDMEM::DataArrayDoubleServant *retServ=new ParaMEDMEM::DataArrayDoubleServant(retCpp);
    retCpp->decrRef();
    return retServ->_this();
  }

  SALOME_MED::DataArrayDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getArrayDouble2()
  {
    ParaMEDMEM::DataArrayDouble *retCpp=MEDCouplingCorbaServBasicsTest::buildArrayDouble2();
    ParaMEDMEM::DataArrayDoubleServant *retServ=new ParaMEDMEM::DataArrayDoubleServant(retCpp);
    retCpp->decrRef();
    return retServ->_this();
  }

  SALOME_MED::DataArrayDoubleCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getArrayDouble3()
  {
    ParaMEDMEM::DataArrayDouble *retCpp=MEDCouplingCorbaServBasicsTest::buildArrayDouble3();
    ParaMEDMEM::DataArrayDoubleServant *retServ=new ParaMEDMEM::DataArrayDoubleServant(retCpp);
    retCpp->decrRef();
    return retServ->_this();
  }

  SALOME_MED::DataArrayIntCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getArrayInt1()
  {
    ParaMEDMEM::DataArrayInt *retCpp=MEDCouplingCorbaServBasicsTest::buildArrayInt1();
    ParaMEDMEM::DataArrayIntServant *retServ=new ParaMEDMEM::DataArrayIntServant(retCpp);
    retCpp->decrRef();
    return retServ->_this();
  }

  SALOME_MED::DataArrayIntCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getArrayInt2()
  {
    ParaMEDMEM::DataArrayInt *retCpp=MEDCouplingCorbaServBasicsTest::buildArrayInt2();
    ParaMEDMEM::DataArrayIntServant *retServ=new ParaMEDMEM::DataArrayIntServant(retCpp);
    retCpp->decrRef();
    return retServ->_this();
  }

  SALOME_MED::DataArrayIntCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getArrayInt3()
  {
    ParaMEDMEM::DataArrayInt *retCpp=MEDCouplingCorbaServBasicsTest::buildArrayInt3();
    ParaMEDMEM::DataArrayIntServant *retServ=new ParaMEDMEM::DataArrayIntServant(retCpp);
    retCpp->decrRef();
    return retServ->_this();
  }

  SALOME_MED::MEDCouplingFieldOverTimeCorbaInterface_ptr MEDCouplingMeshFieldFactoryComponent::getMultiFields2()
  {
    ParaMEDMEM::MEDCouplingFieldOverTime *fot=MEDCouplingCorbaServBasicsTest::buildMultiFields2();
    ParaMEDMEM::MEDCouplingFieldOverTimeServant *retServ=new ParaMEDMEM::MEDCouplingFieldOverTimeServant(fot);
    fot->decrRef();
    return retServ->_this();
  }
}
