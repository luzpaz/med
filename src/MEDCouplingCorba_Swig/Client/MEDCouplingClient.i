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

%module MEDCouplingClient

%{
#include "MEDCouplingFieldDoubleClient.hxx"
#include "MEDCouplingFieldTemplateClient.hxx"
#include "MEDCouplingMultiFieldsClient.hxx"
#include "MEDCouplingFieldOverTimeClient.hxx"
#include "MEDCouplingUMeshClient.hxx"
#include "MEDCouplingExtrudedMeshClient.hxx"
#include "MEDCouplingCMeshClient.hxx"
#include "DataArrayDoubleClient.hxx"
#include "DataArrayIntClient.hxx"

#include <iostream>

using namespace ParaMEDMEM;
%}

%include "MEDCoupling.i"

%newobject ParaMEDMEM::MEDCouplingFieldDoubleClient::New;
%newobject ParaMEDMEM::MEDCouplingFieldTemplateClient::New;
%newobject ParaMEDMEM::MEDCouplingUMeshClient::New;
%newobject ParaMEDMEM::MEDCouplingExtrudedMeshClient::New;
%newobject ParaMEDMEM::MEDCouplingCMeshClient::New;
%newobject ParaMEDMEM::MEDCouplingMultiFieldsClient::New;
%newobject ParaMEDMEM::MEDCouplingFieldOverTimeClient::New;
%newobject ParaMEDMEM::DataArrayDoubleClient::New;
%newobject ParaMEDMEM::DataArrayIntClient::New;

%nodefaultctor;
%nodefaultdtor;

namespace ParaMEDMEM
{
  class MEDCouplingFieldDoubleClient
  {
  public:
    %extend
      {
        static MEDCouplingFieldDouble *New(PyObject *fieldPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorField=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",fieldPtr);
          char *ior=PyString_AsString(iorField);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var fieldPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_var fieldPtrCppC=SALOME_MED::MEDCouplingFieldDoubleCorbaInterface::_narrow(fieldPtrCpp);
          if(CORBA::is_nil(fieldPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCouplingFieldDoubleCorbaInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorField);
          MEDCouplingFieldDouble *ret=MEDCouplingFieldDoubleClient::New(fieldPtrCppC);
          return ret;
        } 
      }
  };

  class MEDCouplingFieldTemplateClient
  {
  public:
    %extend
      {
        static MEDCouplingFieldTemplate *New(PyObject *fieldPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorField=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",fieldPtr);
          char *ior=PyString_AsString(iorField);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var fieldPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_var fieldPtrCppC=SALOME_MED::MEDCouplingFieldTemplateCorbaInterface::_narrow(fieldPtrCpp);
          if(CORBA::is_nil(fieldPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCouplingFieldTemplateCorbaInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorField);
          MEDCouplingFieldTemplate *ret=MEDCouplingFieldTemplateClient::New(fieldPtrCppC);
          return ret;
        } 
      }
  };

  class MEDCouplingMultiFieldsClient
  {
  public:
    %extend
      {
        static MEDCouplingMultiFields *New(PyObject *fieldPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorField=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",fieldPtr);
          char *ior=PyString_AsString(iorField);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var fieldPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCouplingMultiFieldsCorbaInterface_var fieldPtrCppC=SALOME_MED::MEDCouplingMultiFieldsCorbaInterface::_narrow(fieldPtrCpp);
          if(CORBA::is_nil(fieldPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCouplingMultiFieldsCorbaInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorField);
          MEDCouplingMultiFields *ret=MEDCouplingMultiFieldsClient::New(fieldPtrCppC);
          return ret;
        } 
      }
  };

  class MEDCouplingFieldOverTimeClient : public MEDCouplingFieldOverTime
  {
  public:
    std::vector<double> getTimeSteps() const;
    %extend
      {
        static MEDCouplingFieldOverTimeClient *New(PyObject *fieldPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorField=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",fieldPtr);
          char *ior=PyString_AsString(iorField);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var fieldPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCouplingFieldOverTimeCorbaInterface_var fieldPtrCppC=SALOME_MED::MEDCouplingFieldOverTimeCorbaInterface::_narrow(fieldPtrCpp);
          if(CORBA::is_nil(fieldPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCouplingFieldOverTimeCorbaInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorField);
          MEDCouplingFieldOverTimeClient *ret=MEDCouplingFieldOverTimeClient::New(fieldPtrCppC);
          return ret;
        } 
      }
  };

  class MEDCouplingUMeshClient
  {
  public:
    %extend
      {
        static MEDCouplingUMesh *New(PyObject *meshPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorMesh=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",meshPtr);
          char *ior=PyString_AsString(iorMesh);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var meshPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCouplingUMeshCorbaInterface_var meshPtrCppC=SALOME_MED::MEDCouplingUMeshCorbaInterface::_narrow(meshPtrCpp);
          if(CORBA::is_nil(meshPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCouplingUMeshInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorMesh);
          MEDCouplingUMesh *ret=MEDCouplingUMeshClient::New(meshPtrCppC);
          return ret;
        } 
      }
  };

  class MEDCouplingExtrudedMeshClient
  {
  public:
    %extend
      {
        static MEDCouplingExtrudedMesh *New(PyObject *meshPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorMesh=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",meshPtr);
          char *ior=PyString_AsString(iorMesh);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var meshPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface_var meshPtrCppC=SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface::_narrow(meshPtrCpp);
          if(CORBA::is_nil(meshPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCouplingExtrudedMeshInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorMesh);
          MEDCouplingExtrudedMesh *ret=MEDCouplingExtrudedMeshClient::New(meshPtrCppC);
          return ret;
        } 
      }
  };

  class MEDCouplingCMeshClient
  {
  public:
    %extend
      {
        static MEDCouplingCMesh *New(PyObject *meshPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorMesh=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",meshPtr);
          char *ior=PyString_AsString(iorMesh);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var meshPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCouplingCMeshCorbaInterface_var meshPtrCppC=SALOME_MED::MEDCouplingCMeshCorbaInterface::_narrow(meshPtrCpp);
          if(CORBA::is_nil(meshPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCouplingCMeshInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorMesh);
          MEDCouplingCMesh *ret=MEDCouplingCMeshClient::New(meshPtrCppC);
          return ret;
        } 
      }
  };

  class DataArrayDoubleClient
  {
  public:
    %extend
      {
        static DataArrayDouble *New(PyObject *meshPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorMesh=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",meshPtr);
          char *ior=PyString_AsString(iorMesh);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var meshPtrCpp=orb->string_to_object(ior);
          SALOME_MED::DataArrayDoubleCorbaInterface_var meshPtrCppC=SALOME_MED::DataArrayDoubleCorbaInterface::_narrow(meshPtrCpp);
          if(CORBA::is_nil(meshPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.DataArrayDoubleInterface_ptr ");
          Py_DECREF(pdict);
          Py_DECREF(iorMesh);
          DataArrayDouble *ret=DataArrayDoubleClient::New(meshPtrCppC);
          return ret;
        } 
      }
  };

  class DataArrayIntClient
  {
  public:
    %extend
      {
        static DataArrayInt *New(PyObject *meshPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorMesh=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",meshPtr);
          char *ior=PyString_AsString(iorMesh);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var meshPtrCpp=orb->string_to_object(ior);
          SALOME_MED::DataArrayIntCorbaInterface_var meshPtrCppC=SALOME_MED::DataArrayIntCorbaInterface::_narrow(meshPtrCpp);
          if(CORBA::is_nil(meshPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.DataArrayIntInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorMesh);
          DataArrayInt *ret=DataArrayIntClient::New(meshPtrCppC);
          return ret;
        } 
      }
  };
}
