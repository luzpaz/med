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

#include "MEDCouplingMeshFieldFactoryComponentServ.hxx"
#include "MEDCouplingMeshFieldFactoryComponent.hxx"
#include "MEDCouplingFieldDoubleServant.hxx"
#include "MEDCouplingFieldTemplateServant.hxx"
#include "MEDCouplingExtrudedMeshServant.hxx"
#include "MEDCouplingCMeshServant.hxx"
#include "MEDCouplingUMeshServant.hxx"
#include "MEDCouplingFieldDouble.hxx"
#include "MEDCouplingFieldTemplate.hxx"
#include "MEDCouplingExtrudedMesh.hxx"
#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingCMesh.hxx"

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
}
