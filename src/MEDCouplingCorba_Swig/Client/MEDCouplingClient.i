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

%module MEDCouplingClient

%{
#include "MEDCouplingFieldDoubleClient.hxx"
#include "MEDCouplingFieldTemplateClient.hxx"
#include "MEDCouplingMultiFieldsClient.hxx"
#include "MEDCouplingFieldOverTimeClient.hxx"
#include "MEDCouplingUMeshClient.hxx"
#include "MEDCoupling1SGTUMeshClient.hxx"
#include "MEDCoupling1DGTUMeshClient.hxx"
#include "MEDCouplingExtrudedMeshClient.hxx"
#include "MEDCouplingCMeshClient.hxx"
#include "MEDCouplingIMeshClient.hxx"
#include "MEDCouplingCurveLinearMeshClient.hxx"
#include "DataArrayDoubleClient.hxx"
#include "DataArrayIntClient.hxx"

#include <iostream>

using namespace MEDCoupling;
%}

%include "MEDCouplingCommon.i"

%newobject MEDCoupling::MEDCouplingFieldDoubleClient::New;
%newobject MEDCoupling::MEDCouplingFieldTemplateClient::New;
%newobject MEDCoupling::MEDCouplingUMeshClient::New;
%newobject MEDCoupling::MEDCoupling1SGTUMeshClient::New;
%newobject MEDCoupling::MEDCoupling1DGTUMeshClient::New;
%newobject MEDCoupling::MEDCouplingExtrudedMeshClient::New;
%newobject MEDCoupling::MEDCouplingCMeshClient::New;
%newobject MEDCoupling::MEDCouplingIMeshClient::New;
%newobject MEDCoupling::MEDCouplingCurveLinearMeshClient::New;
%newobject MEDCoupling::MEDCouplingMultiFieldsClient::New;
%newobject MEDCoupling::MEDCouplingFieldOverTimeClient::New;
%newobject MEDCoupling::DataArrayDoubleClient::New;
%newobject MEDCoupling::DataArrayIntClient::New;

%nodefaultctor;
%nodefaultdtor;

namespace MEDCoupling
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
          if(!iorField)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCouplingFieldDoubleClient.New appears to differ from CORBA reference ! Expecting a FieldDouble CORBA reference !");
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
          if(!iorField)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCouplingFieldTemplateClient.New appears to differ from CORBA reference ! Expecting a FieldTemplate CORBA reference !");
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
          if(!iorField)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCouplingFieldTemplateClient.New appears to differ from CORBA reference ! Expecting a MultiFields CORBA reference !");
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
          if(!iorField)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCouplingFieldOverTimeClient.New appears to differ from CORBA reference ! Expecting a FieldOverTime CORBA reference !");
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

  class MEDCouplingMeshClient
  {
  public:
    %extend
      {
        static MEDCouplingMesh *New(PyObject *meshPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorMesh=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",meshPtr);
          if(!iorMesh)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCouplingMeshClient.New appears to differ from CORBA reference ! Expecting a MeshCorbaInterface CORBA reference !");
          char *ior=PyString_AsString(iorMesh);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var meshPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCouplingUMeshCorbaInterface_var meshPtrCppC1=SALOME_MED::MEDCouplingUMeshCorbaInterface::_narrow(meshPtrCpp);
          if(!CORBA::is_nil(meshPtrCppC1))
            {
              Py_DECREF(pdict);
              Py_DECREF(iorMesh);
              return MEDCouplingUMeshClient::New(meshPtrCppC1);
            }
          SALOME_MED::MEDCouplingCMeshCorbaInterface_var meshPtrCppC2=SALOME_MED::MEDCouplingCMeshCorbaInterface::_narrow(meshPtrCpp);
          if(!CORBA::is_nil(meshPtrCppC2))
            {
              Py_DECREF(pdict);
              Py_DECREF(iorMesh);
              return MEDCouplingCMeshClient::New(meshPtrCppC2);
            }
          SALOME_MED::MEDCouplingIMeshCorbaInterface_var meshPtrCppC7=SALOME_MED::MEDCouplingIMeshCorbaInterface::_narrow(meshPtrCpp);
          if(!CORBA::is_nil(meshPtrCppC7))
            {
              Py_DECREF(pdict);
              Py_DECREF(iorMesh);
              return MEDCouplingIMeshClient::New(meshPtrCppC7);
            }
          SALOME_MED::MEDCouplingCurveLinearMeshCorbaInterface_var meshPtrCppC3=SALOME_MED::MEDCouplingCurveLinearMeshCorbaInterface::_narrow(meshPtrCpp);
          if(!CORBA::is_nil(meshPtrCppC3))
            {
              Py_DECREF(pdict);
              Py_DECREF(iorMesh);
              return MEDCouplingCurveLinearMeshClient::New(meshPtrCppC3);
            }
          SALOME_MED::MEDCoupling1SGTUMeshCorbaInterface_var meshPtrCppC4=SALOME_MED::MEDCoupling1SGTUMeshCorbaInterface::_narrow(meshPtrCpp);
          if(!CORBA::is_nil(meshPtrCppC4))
            {
              Py_DECREF(pdict);
              Py_DECREF(iorMesh);
              return MEDCoupling1SGTUMeshClient::New(meshPtrCppC4);
            }
          SALOME_MED::MEDCoupling1DGTUMeshCorbaInterface_var meshPtrCppC5=SALOME_MED::MEDCoupling1DGTUMeshCorbaInterface::_narrow(meshPtrCpp);
          if(!CORBA::is_nil(meshPtrCppC5))
            {
              Py_DECREF(pdict);
              Py_DECREF(iorMesh);
              return MEDCoupling1DGTUMeshClient::New(meshPtrCppC5);
            }
          SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface_var meshPtrCppC6=SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface::_narrow(meshPtrCpp);
          if(!CORBA::is_nil(meshPtrCppC6))
            {
              Py_DECREF(pdict);
              Py_DECREF(iorMesh);
              return MEDCouplingExtrudedMeshClient::New(meshPtrCppC6);
            }
          throw INTERP_KERNEL::Exception("Error input corba pointer is not a managed mesh type !");
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
          if(!iorMesh)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCouplingUMeshClient.New appears to differ from CORBA reference ! Expecting a UMeshCorbaInterface CORBA reference !");
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

  class MEDCoupling1SGTUMeshClient
  {
  public:
    %extend
      {
        static MEDCoupling1SGTUMesh *New(PyObject *meshPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorMesh=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",meshPtr);
          if(!iorMesh)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCoupling1SGTUMeshClient.New appears to differ from CORBA reference ! Expecting a 1SGTUMeshCorbaInterface CORBA reference !");
          char *ior=PyString_AsString(iorMesh);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var meshPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCoupling1SGTUMeshCorbaInterface_var meshPtrCppC=SALOME_MED::MEDCoupling1SGTUMeshCorbaInterface::_narrow(meshPtrCpp);
          if(CORBA::is_nil(meshPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCoupling1SGTUMeshInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorMesh);
          MEDCoupling1SGTUMesh *ret=MEDCoupling1SGTUMeshClient::New(meshPtrCppC);
          return ret;
        } 
      }
  };

  class MEDCoupling1DGTUMeshClient
  {
  public:
    %extend
      {
        static MEDCoupling1DGTUMesh *New(PyObject *meshPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorMesh=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",meshPtr);
          if(!iorMesh)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCoupling1DGTUMeshClient.New appears to differ from CORBA reference ! Expecting a 1DGTUMeshCorbaInterface CORBA reference !");
          char *ior=PyString_AsString(iorMesh);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var meshPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCoupling1DGTUMeshCorbaInterface_var meshPtrCppC=SALOME_MED::MEDCoupling1DGTUMeshCorbaInterface::_narrow(meshPtrCpp);
          if(CORBA::is_nil(meshPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCoupling1DGTUMeshInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorMesh);
          MEDCoupling1DGTUMesh *ret=MEDCoupling1DGTUMeshClient::New(meshPtrCppC);
          return ret;
        } 
      }
  };

  class MEDCouplingExtrudedMeshClient
  {
  public:
    %extend
      {
        static MEDCouplingMappedExtrudedMesh *New(PyObject *meshPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorMesh=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",meshPtr);
          if(!iorMesh)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCouplingExtrudedMeshClient.New appears to differ from CORBA reference ! Expecting an ExtrudedMeshCorbaInterface CORBA reference !");
          char *ior=PyString_AsString(iorMesh);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var meshPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface_var meshPtrCppC=SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface::_narrow(meshPtrCpp);
          if(CORBA::is_nil(meshPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCouplingExtrudedMeshInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorMesh);
          MEDCouplingMappedExtrudedMesh *ret=MEDCouplingExtrudedMeshClient::New(meshPtrCppC);
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
          if(!iorMesh)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCouplingCMeshClient.New appears to differ from CORBA reference ! Expecting a CMeshCorbaInterface CORBA reference !");
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

  class MEDCouplingIMeshClient
  {
  public:
    %extend
      {
        static MEDCouplingIMesh *New(PyObject *meshPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorMesh=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",meshPtr);
          if(!iorMesh)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCouplingIMeshClient.New appears to differ from CORBA reference ! Expecting a IMeshCorbaInterface CORBA reference !");
          char *ior=PyString_AsString(iorMesh);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var meshPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCouplingIMeshCorbaInterface_var meshPtrCppC=SALOME_MED::MEDCouplingIMeshCorbaInterface::_narrow(meshPtrCpp);
          if(CORBA::is_nil(meshPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCouplingIMeshInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorMesh);
          MEDCouplingIMesh *ret=MEDCouplingIMeshClient::New(meshPtrCppC);
          return ret;
        } 
      }
  };

  class MEDCouplingCurveLinearMeshClient
  {
  public:
    %extend
      {
        static MEDCouplingCurveLinearMesh *New(PyObject *meshPtr) throw(INTERP_KERNEL::Exception)
        {
          PyObject* pdict=PyDict_New();
          PyDict_SetItemString(pdict,"__builtins__",PyEval_GetBuiltins());
          PyRun_String("import MEDCouplingCorbaServant_idl",Py_single_input,pdict, pdict);
          PyRun_String("import CORBA",Py_single_input,pdict, pdict);
          PyRun_String("orbTmp15634=CORBA.ORB_init([''])", Py_single_input,pdict, pdict);
          PyObject *orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
          // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string
          PyObject *iorMesh=PyObject_CallMethod(orbPython,(char*)"object_to_string",(char*)"O",meshPtr);
          if(!iorMesh)
            throw INTERP_KERNEL::Exception("Error : the input parameter of MEDCouplingCurveLinearMeshClient.New appears to differ from CORBA reference ! Expecting a CurveLinearMeshCorbaInterface CORBA reference !");
          char *ior=PyString_AsString(iorMesh);
          int argc=0;
          CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
          CORBA::Object_var meshPtrCpp=orb->string_to_object(ior);
          SALOME_MED::MEDCouplingCurveLinearMeshCorbaInterface_var meshPtrCppC=SALOME_MED::MEDCouplingCurveLinearMeshCorbaInterface::_narrow(meshPtrCpp);
          if(CORBA::is_nil(meshPtrCppC))
            throw INTERP_KERNEL::Exception("error corba pointer is not a SALOME_MED.MEDCouplingCurveLinearMeshInterface_ptr !");
          Py_DECREF(pdict);
          Py_DECREF(iorMesh);
          MEDCouplingCurveLinearMesh *ret=MEDCouplingCurveLinearMeshClient::New(meshPtrCppC);
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
          if(!iorMesh)
            throw INTERP_KERNEL::Exception("Error : the input parameter of DataArrayDoubleClient.New appears to differ from CORBA reference ! Expecting a DataArrayDoubleCorbaInterface CORBA reference !");
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
          if(!iorMesh)
            throw INTERP_KERNEL::Exception("Error : the input parameter of DataArrayIntClient.New appears to differ from CORBA reference ! Expecting a DataArrayIntCorbaInterface CORBA reference !");
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

%pythoncode %{
def MEDCouplingDataArrayDoublenew(cls,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayDouble____new___(cls,args)
def MEDCouplingDataArrayDoubleIadd(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayDouble____iadd___(self, self, *args)
def MEDCouplingDataArrayDoubleIsub(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayDouble____isub___(self, self, *args)
def MEDCouplingDataArrayDoubleImul(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayDouble____imul___(self, self, *args)
def MEDCouplingDataArrayDoubleIdiv(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayDouble____idiv___(self, self, *args)
def MEDCouplingDataArrayDoubleIpow(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayDouble____ipow___(self, self, *args)
def MEDCouplingFieldDoublenew(cls,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.MEDCouplingFieldDouble____new___(cls,args)
def MEDCouplingFieldDoubleIadd(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.MEDCouplingFieldDouble____iadd___(self, self, *args)
def MEDCouplingFieldDoubleIsub(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.MEDCouplingFieldDouble____isub___(self, self, *args)
def MEDCouplingFieldDoubleImul(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.MEDCouplingFieldDouble____imul___(self, self, *args)
def MEDCouplingFieldDoubleIdiv(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.MEDCouplingFieldDouble____idiv___(self, self, *args)
def MEDCouplingFieldDoubleIpow(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.MEDCouplingFieldDouble____ipow___(self, self, *args)
def MEDCouplingDataArrayIntnew(cls,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayInt____new___(cls,args)
def MEDCouplingDataArrayIntIadd(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayInt____iadd___(self, self, *args)
def MEDCouplingDataArrayIntIsub(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayInt____isub___(self, self, *args)
def MEDCouplingDataArrayIntImul(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayInt____imul___(self, self, *args)
def MEDCouplingDataArrayIntIdiv(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayInt____idiv___(self, self, *args)
def MEDCouplingDataArrayIntImod(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayInt____imod___(self, self, *args)
def MEDCouplingDataArrayIntIpow(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayInt____ipow___(self, self, *args)
def MEDCouplingDataArrayBytenew(cls,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayByte____new___(cls,args)
def MEDCouplingDataArrayDoubleTupleIadd(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayDoubleTuple____iadd___(self, self, *args)
def MEDCouplingDataArrayDoubleTupleIsub(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayDoubleTuple____isub___(self, self, *args)
def MEDCouplingDataArrayDoubleTupleImul(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayDoubleTuple____imul___(self, self, *args)
def MEDCouplingDataArrayDoubleTupleIdiv(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayDoubleTuple____idiv___(self, self, *args)
def MEDCouplingDataArrayIntTupleIadd(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayIntTuple____iadd___(self, self, *args)
def MEDCouplingDataArrayIntTupleIsub(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayIntTuple____isub___(self, self, *args)
def MEDCouplingDataArrayIntTupleImul(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayIntTuple____imul___(self, self, *args)
def MEDCouplingDataArrayIntTupleIdiv(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayIntTuple____idiv___(self, self, *args)
def MEDCouplingDataArrayIntTupleImod(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DataArrayIntTuple____imod___(self, self, *args)
def ParaMEDMEMDenseMatrixIadd(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DenseMatrix____iadd___(self, self, *args)
def ParaMEDMEMDenseMatrixIsub(self,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.DenseMatrix____isub___(self, self, *args)
def MEDCouplingUMeshnew(cls,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.MEDCouplingUMesh____new___(cls,args)
def MEDCoupling1DGTUMeshnew(cls,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.MEDCoupling1DGTUMesh____new___(cls,args)
def MEDCoupling1SGTUMeshnew(cls,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.MEDCoupling1SGTUMesh____new___(cls,args)
def MEDCouplingCurveLinearMeshnew(cls,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.MEDCouplingCurveLinearMesh____new___(cls,args)
def MEDCouplingCMeshnew(cls,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.MEDCouplingCMesh____new___(cls,args)
def MEDCouplingIMeshnew(cls,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.MEDCouplingIMesh____new___(cls,args)
def MEDCouplingExtrudedMeshnew(cls,*args):
    import _MEDCouplingClient
    return _MEDCouplingClient.MEDCouplingMappedExtrudedMesh____new___(cls,args)
%}

%include "MEDCouplingFinalize.i"
