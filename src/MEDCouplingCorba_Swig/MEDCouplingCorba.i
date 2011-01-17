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

%module MEDCouplingCorba

%{
#include "MEDCouplingFieldDoubleServant.hxx"  
#include "MEDCouplingFieldTemplateServant.hxx"
#include "MEDCouplingUMeshServant.hxx"
#include "MEDCouplingExtrudedMeshServant.hxx"
#include "MEDCouplingCMeshServant.hxx"

using namespace ParaMEDMEM;
%}

%include "MEDCoupling.i"

namespace ParaMEDMEM
{
  class MEDCouplingFieldDoubleServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingFieldDouble *cppPointerOfMesh)
         {
           int argc=0;
           MEDCouplingFieldDoubleServant *serv=new MEDCouplingFieldDoubleServant(cppPointerOfMesh);
           CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
           CORBA::Object_var obj=orb->resolve_initial_references("RootPOA");
           PortableServer::POA_var poa=PortableServer::POA::_narrow(obj);
           PortableServer::POAManager_var mgr=poa->the_POAManager();
           mgr->activate();
           SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_var ret=serv->_this();
           char *ior=orb->object_to_string(ret);
           PyObject *iorPython=PyString_FromString(ior);
           PyObject* pdict=PyDict_New();
           PyDict_SetItemString(pdict, "__builtins__", PyEval_GetBuiltins());
           PyRun_String("import MEDCouplingCorbaServant_idl", Py_single_input, pdict, pdict);
           PyRun_String("import CORBA", Py_single_input, pdict, pdict);
           PyRun_String("orbTmp15634=CORBA.ORB_init([''])",Py_single_input,pdict, pdict);
           PyObject* orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
           PyObject *corbaObj=PyObject_CallMethod(orbPython,(char*)"string_to_object",(char*)"O",iorPython);
           Py_DECREF(pdict);
           Py_DECREF(iorPython);
           CORBA::string_free(ior);
           return corbaObj;
         }
       }
  };

  class MEDCouplingFieldTemplateServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingFieldTemplate *cppPointerOfMesh)
         {
           int argc=0;
           MEDCouplingFieldTemplateServant *serv=new MEDCouplingFieldTemplateServant(cppPointerOfMesh);
           CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
           CORBA::Object_var obj=orb->resolve_initial_references("RootPOA");
           PortableServer::POA_var poa=PortableServer::POA::_narrow(obj);
           PortableServer::POAManager_var mgr=poa->the_POAManager();
           mgr->activate();
           SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_var ret=serv->_this();
           char *ior=orb->object_to_string(ret);
           PyObject *iorPython=PyString_FromString(ior);
           PyObject* pdict=PyDict_New();
           PyDict_SetItemString(pdict, "__builtins__", PyEval_GetBuiltins());
           PyRun_String("import MEDCouplingCorbaServant_idl", Py_single_input, pdict, pdict);
           PyRun_String("import CORBA", Py_single_input, pdict, pdict);
           PyRun_String("orbTmp15634=CORBA.ORB_init([''])",Py_single_input,pdict, pdict);
           PyObject* orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
           PyObject *corbaObj=PyObject_CallMethod(orbPython,(char*)"string_to_object",(char*)"O",iorPython);
           Py_DECREF(pdict);
           Py_DECREF(iorPython);
           CORBA::string_free(ior);
           return corbaObj;
         }
       }
  };

  class MEDCouplingUMeshServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingUMesh *cppPointerOfMesh)
         {
           int argc=0;
           MEDCouplingUMeshServant *serv=new MEDCouplingUMeshServant(cppPointerOfMesh);
           CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
           CORBA::Object_var obj=orb->resolve_initial_references("RootPOA");
           PortableServer::POA_var poa=PortableServer::POA::_narrow(obj);
           PortableServer::POAManager_var mgr=poa->the_POAManager();
           mgr->activate();
           SALOME_MED::MEDCouplingUMeshCorbaInterface_var ret=serv->_this();
           char *ior=orb->object_to_string(ret);
           PyObject *iorPython=PyString_FromString(ior);
           PyObject* pdict=PyDict_New();
           PyDict_SetItemString(pdict, "__builtins__", PyEval_GetBuiltins());
           PyRun_String("import MEDCouplingCorbaServant_idl", Py_single_input, pdict, pdict);
           PyRun_String("import CORBA", Py_single_input, pdict, pdict);
           PyRun_String("orbTmp15634=CORBA.ORB_init([''])",Py_single_input,pdict, pdict);
           PyObject* orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
           PyObject *corbaObj=PyObject_CallMethod(orbPython,(char*)"string_to_object",(char*)"O",iorPython);
           Py_DECREF(pdict);
           Py_DECREF(iorPython);
           CORBA::string_free(ior);
           return corbaObj;
         }
       }
  };

  class MEDCouplingExtrudedMeshServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingExtrudedMesh *cppPointerOfMesh)
         {
           int argc=0;
           MEDCouplingExtrudedMeshServant *serv=new MEDCouplingExtrudedMeshServant(cppPointerOfMesh);
           CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
           CORBA::Object_var obj=orb->resolve_initial_references("RootPOA");
           PortableServer::POA_var poa=PortableServer::POA::_narrow(obj);
           PortableServer::POAManager_var mgr=poa->the_POAManager();
           mgr->activate();
           SALOME_MED::MEDCouplingExtrudedMeshCorbaInterface_var ret=serv->_this();
           char *ior=orb->object_to_string(ret);
           PyObject *iorPython=PyString_FromString(ior);
           PyObject* pdict=PyDict_New();
           PyDict_SetItemString(pdict, "__builtins__", PyEval_GetBuiltins());
           PyRun_String("import MEDCouplingCorbaServant_idl", Py_single_input, pdict, pdict);
           PyRun_String("import CORBA", Py_single_input, pdict, pdict);
           PyRun_String("orbTmp15634=CORBA.ORB_init([''])",Py_single_input,pdict, pdict);
           PyObject* orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
           PyObject *corbaObj=PyObject_CallMethod(orbPython,(char*)"string_to_object",(char*)"O",iorPython);
           Py_DECREF(pdict);
           Py_DECREF(iorPython);
           CORBA::string_free(ior);
           return corbaObj;
         }
       }
  };

  class MEDCouplingCMeshServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingCMesh *cppPointerOfMesh)
         {
           int argc=0;
           MEDCouplingCMeshServant *serv=new MEDCouplingCMeshServant(cppPointerOfMesh);
           CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
           CORBA::Object_var obj=orb->resolve_initial_references("RootPOA");
           PortableServer::POA_var poa=PortableServer::POA::_narrow(obj);
           PortableServer::POAManager_var mgr=poa->the_POAManager();
           mgr->activate();
           SALOME_MED::MEDCouplingCMeshCorbaInterface_var ret=serv->_this();
           char *ior=orb->object_to_string(ret);
           PyObject *iorPython=PyString_FromString(ior);
           PyObject* pdict=PyDict_New();
           PyDict_SetItemString(pdict, "__builtins__", PyEval_GetBuiltins());
           PyRun_String("import MEDCouplingCorbaServant_idl", Py_single_input, pdict, pdict);
           PyRun_String("import CORBA", Py_single_input, pdict, pdict);
           PyRun_String("orbTmp15634=CORBA.ORB_init([''])",Py_single_input,pdict, pdict);
           PyObject* orbPython=PyDict_GetItemString(pdict,"orbTmp15634");
           PyObject *corbaObj=PyObject_CallMethod(orbPython,(char*)"string_to_object",(char*)"O",iorPython);
           Py_DECREF(pdict);
           Py_DECREF(iorPython);
           CORBA::string_free(ior);
           return corbaObj;
         }
       }
  };
}
