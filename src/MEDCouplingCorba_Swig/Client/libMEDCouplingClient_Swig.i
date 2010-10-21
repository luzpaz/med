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

%module libMEDCouplingClient_Swig

%{
#include "MEDCouplingFieldDoubleClient.hxx"
#include "MEDCouplingUMeshClient.hxx"
#include "MEDCouplingExtrudedMeshClient.hxx"
#include "MEDCouplingCMeshClient.hxx"

#include <iostream>

using namespace ParaMEDMEM;
%}

%include "libMEDCoupling_Swig.i"

%newobject ParaMEDMEM::MEDCouplingFieldDoubleClient::New;
%newobject ParaMEDMEM::MEDCouplingUMeshClient::New;
%newobject ParaMEDMEM::MEDCouplingExtrudedMeshClient::New;
%newobject ParaMEDMEM::MEDCouplingCMeshClient::New;
%nodefaultctor;
%nodefaultdtor;

namespace ParaMEDMEM
{
  class MEDCouplingFieldDoubleClient
  {
  public:
    %extend
      {
        static MEDCouplingFieldDouble *New(PyObject *fieldPtr)
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
          SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr fieldPtrCppC=SALOME_MED::MEDCouplingFieldDoubleCorbaInterface::_narrow(fieldPtrCpp);
          if(CORBA::is_nil(fieldPtrCppC))
            std::cerr << "error corba pointer is not a SALOME_MED.MEDCouplingFieldDoubleCorbaInterface_ptr !" << std::endl;
          Py_DECREF(pdict);
          MEDCouplingFieldDouble *ret=MEDCouplingFieldDoubleClient::New(fieldPtrCppC);
          return ret;
        } 
      }
  };

  class MEDCouplingUMeshClient
  {
  public:
    %extend
      {
        static MEDCouplingUMesh *New(PyObject *meshPtr)
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
          SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr meshPtrCppC=SALOME_MED::MEDCouplingUMeshCorbaInterface::_narrow(meshPtrCpp);
          if(CORBA::is_nil(meshPtrCppC))
            std::cerr << "error corba pointer is not a SALOME_MED.MEDCouplingUMeshInterface_ptr !" << std::endl;
          Py_DECREF(pdict);
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
        static MEDCouplingExtrudedMesh *New(PyObject *meshPtr)
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
          SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface_ptr meshPtrCppC=SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface::_narrow(meshPtrCpp);
          if(CORBA::is_nil(meshPtrCppC))
            std::cerr << "error corba pointer is not a SALOME_MED.MEDCouplingExtrudedMeshInterface_ptr !" << std::endl;
          Py_DECREF(pdict);
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
        static MEDCouplingCMesh *New(PyObject *meshPtr)
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
          SALOME_MED::MEDCouplingCMeshCorbaInterface_ptr meshPtrCppC=SALOME_MED::MEDCouplingCMeshCorbaInterface::_narrow(meshPtrCpp);
          if(CORBA::is_nil(meshPtrCppC))
            std::cerr << "error corba pointer is not a SALOME_MED.MEDCouplingCMeshInterface_ptr !" << std::endl;
          Py_DECREF(pdict);
          MEDCouplingCMesh *ret=MEDCouplingCMeshClient::New(meshPtrCppC);
          return ret;
        } 
      }
  };
}
