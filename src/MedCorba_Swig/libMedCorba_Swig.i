// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

%module libMedCorba_Swig

%include "libMEDMEM_Swig.i"

%{
#include "MEDMEM_convert.hxx"
#include "MEDMEM_Family_i.hxx"
#include "MEDMEM_Field_i.hxx"
#include "MEDMEM_FieldTemplate_i.hxx"
#include "MEDMEM_Group_i.hxx"
#include "MEDMEM_Med_i.hxx"
#include "MEDMEM_GMesh_i.hxx"
#include "MEDMEM_Mesh_i.hxx"
#include "MEDMEM_Support_i.hxx"
#include "Med_Gen_i.hxx"

  using namespace MEDMEM;
  using namespace MED_EN;
%}

/*
  typemap in, out and typecheck for Corba Objects (MESH, FIELDDOUBLE, FIELDINT and
  Support) between C++ and Python

  WARNING (NB) to the user of those typmaps (SWIG wrapping for C++ routines
  -------------------------------------------------------------------------

  You have to be quite careful about the SWIG version your are currently
  using, because CORBA pointeur _ptr or _var could be wrapped by SWIG using
  their reference rather than the pointeur itself (differences detected using
  SWIG 1.1.x, SWIG 1.3.13 and SWIG 1.3.17)

  typecheck always says OK, as currently all overloaded methods HERE differ
  only in nb of parameters (there are default ones)
*/

#if defined(SWIGPYTHON)
%typemap(out) SALOME_MED::GMESH_ptr, SALOME_MED::MESH_ptr, SALOME_MED::FIELDDOUBLE_ptr,
   SALOME_MED::FIELDINT_ptr, SALOME_MED::SUPPORT_ptr,
   const SALOME_MED::GMESH_ptr, const SALOME_MED::MESH_ptr, const SALOME_MED::FIELDDOUBLE_ptr,
   const SALOME_MED::FIELDINT_ptr, const SALOME_MED::SUPPORT_ptr
{
  MESSAGE("typemap out sur Objet Corba version ptr");

  SCRUTE($1);

  // Get the orb corba python

  PyObject* pdict = PyDict_New();
  PyDict_SetItemString(pdict, "__builtins__", PyEval_GetBuiltins());
  PyRun_String("import CORBA", Py_single_input, pdict, pdict);
 
  PyRun_String("o = CORBA.ORB_init([''], CORBA.ORB_ID);", Py_single_input,
                   pdict, pdict);
 
  PyObject* orb = PyDict_GetItemString(pdict, "o");

  // Get the orb Corba C++

  int argc = 0;
  char *xargv = (char*)"";
  char **argv = &xargv;
  CORBA::ORB_var ORB = CORBA::ORB_init(argc, argv);

  string s =  ORB->object_to_string($1);
  SCRUTE(s);
  PyObject * tmp = PyString_FromString(s.c_str());
  SCRUTE(tmp);
  PyObject * corbaObj = PyObject_CallMethod(orb, (char*)"string_to_object", (char*)"O", tmp);
  $result = corbaObj;

  // cast CORBA object, if necessary

  // make target class name
//   string className = "$1_type";
//   className.replace( className.find(':'), 2, ".");
//   className.erase( className.find("_ptr"));

//   // get target class object
//   string getClassCmd = ( "cls = " + className );
//   PyRun_String("import SALOME_MED", Py_single_input, pdict, pdict);
//   PyRun_String(getClassCmd.c_str(), Py_single_input, pdict, pdict);
//   PyObject* cls = PyDict_GetItemString(pdict, "cls");

//   // cast
//   $result = PyObject_CallMethod(corbaObj, (char*)"_narrow", (char*)"O", cls);

  SCRUTE($result);
}
#endif

#if defined(SWIGPYTHON)
%typemap(out) SALOME_MED::GMESH_var, SALOME_MED::MESH_var, SALOME_MED::FIELDDOUBLE_var,
   SALOME_MED::FIELDINT_var, SALOME_MED::SUPPORT_var,
   const SALOME_MED::GMESH_var, const SALOME_MED::MESH_var, const SALOME_MED::FIELDDOUBLE_var,
   const SALOME_MED::FIELDINT_var, const SALOME_MED::SUPPORT_var
{
  MESSAGE("typemap out sur Objet Corba version var");

  // Get the orb corba python

  PyObject* pdict = PyDict_New();
  PyDict_SetItemString(pdict, "__builtins__", PyEval_GetBuiltins());
  PyRun_String("import CORBA", Py_single_input, pdict, pdict);
 
  PyRun_String("o = CORBA.ORB_init([''], CORBA.ORB_ID);", Py_single_input,
                   pdict, pdict);
 
  PyObject* orb = PyDict_GetItemString(pdict, "o");

  // Get the orb Corba C++

  int argc = 0;
  char *xargv = (char*)"";
  char **argv = &xargv;
  CORBA::ORB_var ORB = CORBA::ORB_init(argc, argv);

  string s =  ORB->object_to_string(*$1);
  SCRUTE(s);
  PyObject * tmp = PyString_FromString(s.c_str());
  SCRUTE(tmp);
  $result = PyObject_CallMethod(orb, (char*)"string_to_object", (char*)"O", tmp);
  SCRUTE($result);
}
#endif

#if defined(SWIGPYTHON)
%typemap(typecheck) SALOME_MED::GMESH_ptr, SALOME_MED::MESH_ptr, SALOME_MED::FIELDDOUBLE_ptr,
                    SALOME_MED::FIELDINT_ptr, SALOME_MED::SUPPORT_ptr,
                    const SALOME_MED::MESH_ptr, const SALOME_MED::FIELDDOUBLE_ptr,
                    const SALOME_MED::FIELDINT_ptr, const SALOME_MED::SUPPORT_ptr,
                    SALOME_MED::GMESH_var, SALOME_MED::MESH_var, SALOME_MED::FIELDDOUBLE_var,
                    SALOME_MED::FIELDINT_var, SALOME_MED::SUPPORT_var,
                    const SALOME_MED::MESH_var, const SALOME_MED::FIELDDOUBLE_var,
                    const SALOME_MED::FIELDINT_var, const SALOME_MED::SUPPORT_var
{
  $1 = 1;
}
#endif

#if defined(SWIGPYTHON)
%typemap(in) const SALOME_MED::MESH_ptr &, SALOME_MED::MESH_ptr &
{

  MESSAGE("typemap in sur Objet Corba MESH avec reference");

  PyObject* pdict = PyDict_New();
  PyDict_SetItemString(pdict, "__builtins__", PyEval_GetBuiltins());
  PyRun_String("import CORBA", Py_single_input, pdict, pdict);
 
  PyRun_String("o = CORBA.ORB_init([''], CORBA.ORB_ID);", Py_single_input,
                   pdict, pdict);
 
  PyObject* orb = PyDict_GetItemString(pdict, "o");

  // Ask omniORBpy to transform MESH (python Corba) ptr to IOR string

  PyObject* iorMesh
    = PyObject_CallMethod(orb, (char*)"object_to_string", (char*)"O", $input);
 
  if (iorMesh == Py_None)
    return NULL;
  char * s = PyString_AsString(PyObject_Str(iorMesh));
 
  // Ask omniORB to convert IOR string to MESH (C++ Corba) ptr

  int argc = 0;
  char *xargv = (char*)"";
  char **argv = &xargv;
  CORBA::ORB_var ORB = CORBA::ORB_init(argc, argv);
  CORBA::Object_var O =  ORB->string_to_object(s);
  SCRUTE(O);
  SALOME_MED::MESH_ptr t = SALOME_MED::MESH::_narrow(O);
  SCRUTE(CORBA::is_nil(t));

  $1 = &t;
  SCRUTE(*$1);
}
#endif

#if defined(SWIGPYTHON)
%typemap(in) const SALOME_MED::FIELDDOUBLE_ptr &,
                    SALOME_MED::FIELDDOUBLE_ptr &
{

  MESSAGE("typemap in sur Objet Corba FIELDDOUBLE avec reference");

  PyObject* pdict = PyDict_New();
  PyDict_SetItemString(pdict, "__builtins__", PyEval_GetBuiltins());
  PyRun_String("import CORBA", Py_single_input, pdict, pdict);
 
  PyRun_String("o = CORBA.ORB_init([''], CORBA.ORB_ID);", Py_single_input,
                   pdict, pdict);
 
  PyObject* orb = PyDict_GetItemString(pdict, "o");

  // Ask omniORBpy to transform FIELDDOUBLE (python Corba) ptr to IOR string

  PyObject* iorFieldDouble
    = PyObject_CallMethod(orb, (char*)"object_to_string", (char*)"O", $input);
 
  if (iorFieldDouble == Py_None)
    return NULL;
  char * s = PyString_AsString(PyObject_Str(iorFieldDouble));
 
  // Ask omniORB to convert IOR string to FIELDDOUBLE (C++ Corba) ptr

  int argc = 0;
  char *xargv = (char*)"";
  char **argv = &xargv;
  CORBA::ORB_var ORB = CORBA::ORB_init(argc, argv);
  CORBA::Object_var O =  ORB->string_to_object(s);
  SCRUTE(O);
  SALOME_MED::FIELDDOUBLE_ptr t = SALOME_MED::FIELDDOUBLE::_narrow(O);
  SCRUTE(CORBA::is_nil(t));

  $1 = &t;
  SCRUTE(*$1);
}
#endif

#if defined(SWIGPYTHON)
%typemap(in) const SALOME_MED::FIELDINT_ptr &,
                    SALOME_MED::FIELDINT_ptr &
{

  MESSAGE("typemap in sur Objet Corba FIELDINT avec reference");

  PyObject* pdict = PyDict_New();
  PyDict_SetItemString(pdict, "__builtins__", PyEval_GetBuiltins());
  PyRun_String("import CORBA", Py_single_input, pdict, pdict);
 
  PyRun_String("o = CORBA.ORB_init([''], CORBA.ORB_ID);", Py_single_input,
                   pdict, pdict);
 
  PyObject* orb = PyDict_GetItemString(pdict, "o");

  // Ask omniORBpy to transform FIELDINT (python Corba) ptr to IOR string

  PyObject* iorFieldInt
    = PyObject_CallMethod(orb, (char*)"object_to_string", (char*)"O", $input);
 
  if (iorFieldInt == Py_None)
    return NULL;
  char * s = PyString_AsString(PyObject_Str(iorFieldInt));
 
  // Ask omniORB to convert IOR string to FIELDINT (C++ Corba) ptr

  int argc = 0;
  char *xargv = (char*)"";
  char **argv = &xargv;
  CORBA::ORB_var ORB = CORBA::ORB_init(argc, argv);
  CORBA::Object_var O =  ORB->string_to_object(s);
  SCRUTE(O);
  SALOME_MED::FIELDINT_ptr t = SALOME_MED::FIELDINT::_narrow(O);
  SCRUTE(CORBA::is_nil(t));

  $1 = &t;
  SCRUTE(*$1);
}
#endif

#if defined(SWIGPYTHON)
%typemap(in) const SALOME_MED::FIELDDOUBLE_ptr,
                    SALOME_MED::FIELDDOUBLE_ptr
{

  MESSAGE("typemap in sur Objet Corba FIELDDOUBLE sans reference");

  PyObject* pdict = PyDict_New();
  PyDict_SetItemString(pdict, "__builtins__", PyEval_GetBuiltins());
  PyRun_String("import CORBA", Py_single_input, pdict, pdict);
 
  PyRun_String("o = CORBA.ORB_init([''], CORBA.ORB_ID);", Py_single_input,
                   pdict, pdict);
 
  PyObject* orb = PyDict_GetItemString(pdict, "o");

  // Ask omniORBpy to transform FIELDDOUBLE (python Corba) ptr to IOR string

  PyObject* iorFieldDouble
    = PyObject_CallMethod(orb, (char*)"object_to_string", (char*)"O", $input);
 
  if (iorFieldDouble == Py_None)
    return NULL;
  char * s = PyString_AsString(PyObject_Str(iorFieldDouble));
 
  // Ask omniORB to convert IOR string to FIELDDOUBLE (C++ Corba) ptr

  int argc = 0;
  char *xargv = (char*)"";
  char **argv = &xargv;
  CORBA::ORB_var ORB = CORBA::ORB_init(argc, argv);
  CORBA::Object_var O =  ORB->string_to_object(s);
  SCRUTE(O);
  SALOME_MED::FIELDDOUBLE_ptr t = SALOME_MED::FIELDDOUBLE::_narrow(O);
  SCRUTE(CORBA::is_nil(t));

  $1 = t;
  SCRUTE($1);
}
#endif

#if defined(SWIGPYTHON)
%typemap(in) const SALOME_MED::FIELDINT_ptr,
                    SALOME_MED::FIELDINT_ptr
{

  MESSAGE("typemap in sur Objet Corba FIELDINT sans reference");

  PyObject* pdict = PyDict_New();
  PyDict_SetItemString(pdict, "__builtins__", PyEval_GetBuiltins());
  PyRun_String("import CORBA", Py_single_input, pdict, pdict);
 
  PyRun_String("o = CORBA.ORB_init([''], CORBA.ORB_ID);", Py_single_input,
                   pdict, pdict);
 
  PyObject* orb = PyDict_GetItemString(pdict, "o");

  // Ask omniORBpy to transform FIELDINT (python Corba) ptr to IOR string

  PyObject* iorFieldInt
    = PyObject_CallMethod(orb, (char*)"object_to_string", (char*)"O", $input);
 
  if (iorFieldInt == Py_None)
    return NULL;
  char * s = PyString_AsString(PyObject_Str(iorFieldInt));
 
  // Ask omniORB to convert IOR string to FIELDINT (C++ Corba) ptr

  int argc = 0;
  char *xargv = (char*)"";
  char **argv = &xargv;
  CORBA::ORB_var ORB = CORBA::ORB_init(argc, argv);
  CORBA::Object_var O =  ORB->string_to_object(s);
  SCRUTE(O);
  SALOME_MED::FIELDINT_ptr t = SALOME_MED::FIELDINT::_narrow(O);
  SCRUTE(CORBA::is_nil(t));

  $1 = t;
  SCRUTE($1);
}
#endif

#if defined(SWIGPYTHON)
%typemap(in) const SALOME_MED::SUPPORT_ptr &, SALOME_MED::SUPPORT_ptr &
{

  MESSAGE("typemap in sur Objet Corba SUPPORT avec reference");

  PyObject* pdict = PyDict_New();
  PyDict_SetItemString(pdict, "__builtins__", PyEval_GetBuiltins());
  PyRun_String("import CORBA", Py_single_input, pdict, pdict);
 
  PyRun_String("o = CORBA.ORB_init([''], CORBA.ORB_ID);", Py_single_input,
                   pdict, pdict);
 
  PyObject* orb = PyDict_GetItemString(pdict, "o");

  // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string

  PyObject* iorSupport
    = PyObject_CallMethod(orb, (char*)"object_to_string", (char*)"O", $input);
 
  if (iorSupport == Py_None)
    return NULL;
  char * s = PyString_AsString(PyObject_Str(iorSupport));
 
  // Ask omniORB to convert IOR string to SUPPORT (C++ Corba) ptr

  int argc = 0;
  char *xargv = (char*)"";
  char **argv = &xargv;
  CORBA::ORB_var ORB = CORBA::ORB_init(argc, argv);
  CORBA::Object_var O =  ORB->string_to_object(s);
  SCRUTE(O);
  SALOME_MED::SUPPORT_ptr t = SALOME_MED::SUPPORT::_narrow(O);
  SCRUTE(CORBA::is_nil(t));

  $1 = &t;
  SCRUTE(*$1);
}
#endif

#if defined(SWIGPYTHON)
%typemap(in) const SALOME_MED::SUPPORT_ptr, SALOME_MED::SUPPORT_ptr
{

  MESSAGE("typemap in sur Objet Corba SUPPORT sans reference");

  SCRUTE($input);

  PyObject* pdict = PyDict_New();
  PyDict_SetItemString(pdict, "__builtins__", PyEval_GetBuiltins());
  PyRun_String("import CORBA", Py_single_input, pdict, pdict);
 
  PyRun_String("o = CORBA.ORB_init([''], CORBA.ORB_ID);", Py_single_input,
                   pdict, pdict);
 
  PyObject* orb = PyDict_GetItemString(pdict, "o");

  // Ask omniORBpy to transform SUPPORT (python Corba) ptr to IOR string

  PyObject* iorSupport
    = PyObject_CallMethod(orb, (char*)"object_to_string", (char*)"O", $input);
 
  if (iorSupport == Py_None)
    return NULL;
  char * s = PyString_AsString(PyObject_Str(iorSupport));
 
  // Ask omniORB to convert IOR string to SUPPORT (C++ Corba) ptr

  int argc = 0;
  char *xargv = (char*)"";
  char **argv = &xargv;
  CORBA::ORB_var ORB = CORBA::ORB_init(argc, argv);
  CORBA::Object_var O =  ORB->string_to_object(s);
  SCRUTE(O);
  SALOME_MED::SUPPORT_ptr t = SALOME_MED::SUPPORT::_narrow(O);
  SCRUTE(CORBA::is_nil(t));

  $1 = t;
  SCRUTE($1);
}
#endif

#if defined(SWIGPYTHON)
%typemap(in) const SALOME_MED::GMESH_ptr, SALOME_MED::GMESH_ptr
{

  MESSAGE("typemap in sur Objet Corba MESH sans reference");

  SCRUTE($input);

  PyObject* pdict = PyDict_New();
  PyDict_SetItemString(pdict, "__builtins__", PyEval_GetBuiltins());
  PyRun_String("import CORBA", Py_single_input, pdict, pdict);
 
  PyRun_String("o = CORBA.ORB_init([''], CORBA.ORB_ID);", Py_single_input,
                   pdict, pdict);
 
  PyObject* orb = PyDict_GetItemString(pdict, "o");

  // Ask omniORBpy to transform MESH (python Corba) ptr to IOR string

  PyObject* iorMesh
    = PyObject_CallMethod(orb, (char*)"object_to_string", (char*)"O", $input);
 
  if (iorMesh == Py_None)
    return NULL;
  char * s = PyString_AsString(PyObject_Str(iorMesh));
 
  // Ask omniORB to convert IOR string to MESH (C++ Corba) ptr

  int argc = 0;
  char *xargv = (char*)"";
  char **argv = &xargv;
  CORBA::ORB_var ORB = CORBA::ORB_init(argc, argv);
  CORBA::Object_var O =  ORB->string_to_object(s);
  SCRUTE(O);
  SALOME_MED::GMESH_ptr t = SALOME_MED::GMESH::_narrow(O);
  SCRUTE(CORBA::is_nil(t));

  $1 = t;
  SCRUTE($1);
}
#endif

#if defined(SWIGPYTHON)
%typemap(in) const SALOME_MED::MESH_ptr, SALOME_MED::MESH_ptr
{

  MESSAGE("typemap in sur Objet Corba MESH sans reference");

  SCRUTE($input);

  PyObject* pdict = PyDict_New();
  PyDict_SetItemString(pdict, "__builtins__", PyEval_GetBuiltins());
  PyRun_String("import CORBA", Py_single_input, pdict, pdict);
 
  PyRun_String("o = CORBA.ORB_init([''], CORBA.ORB_ID);", Py_single_input,
                   pdict, pdict);
 
  PyObject* orb = PyDict_GetItemString(pdict, "o");

  // Ask omniORBpy to transform MESH (python Corba) ptr to IOR string

  PyObject* iorMesh
    = PyObject_CallMethod(orb, (char*)"object_to_string", (char*)"O", $input);
 
  if (iorMesh == Py_None)
    return NULL;
  char * s = PyString_AsString(PyObject_Str(iorMesh));
 
  // Ask omniORB to convert IOR string to MESH (C++ Corba) ptr

  int argc = 0;
  char *xargv = (char*)"";
  char **argv = &xargv;
  CORBA::ORB_var ORB = CORBA::ORB_init(argc, argv);
  CORBA::Object_var O =  ORB->string_to_object(s);
  SCRUTE(O);
  SALOME_MED::MESH_ptr t = SALOME_MED::MESH::_narrow(O);
  SCRUTE(CORBA::is_nil(t));

  $1 = t;
  SCRUTE($1);
}
#endif

#if defined(SWIGPYTHON)
%typemap(in) const SALOME_MED::FIELDDOUBLE_ptr,
		    SALOME_MED::FIELDDOUBLE_ptr &
{

  MESSAGE("typemap in sur Objet Corba FIELDDOUBLE sans reference");

  SCRUTE($input);

  PyObject* pdict = PyDict_New();
  PyDict_SetItemString(pdict, "__builtins__", PyEval_GetBuiltins());
  PyRun_String("import CORBA", Py_single_input, pdict, pdict);
 
  PyRun_String("o = CORBA.ORB_init([''], CORBA.ORB_ID);", Py_single_input,
                   pdict, pdict);
 
  PyObject* orb = PyDict_GetItemString(pdict, "o");

  // Ask omniORBpy to transform FIELDDOUBLE (python Corba) ptr to IOR string

  PyObject* iorFieldDouble
    = PyObject_CallMethod(orb, (char*)"object_to_string", (char*)"O", $input);
 
  if (iorFieldDouble == Py_None)
    return NULL;
  char * s = PyString_AsString(PyObject_Str(iorFieldDouble));
 
  // Ask omniORB to convert IOR string to FIELDDOUBLE (C++ Corba) ptr

  int argc = 0;
  char *xargv = (char*)"";
  char **argv = &xargv;
  CORBA::ORB_var ORB = CORBA::ORB_init(argc, argv);
  CORBA::Object_var O =  ORB->string_to_object(s);
  SCRUTE(O);
  SALOME_MED::FIELDDOUBLE_ptr t = SALOME_MED::FIELDDOUBLE::_narrow(O);
  SCRUTE(CORBA::is_nil(t));

  $1 = t;
  SCRUTE($1);
}
#endif

#if defined(SWIGPYTHON)
%typemap(in) const SALOME_MED::FIELDINT_ptr,
                    SALOME_MED::FIELDINT_ptr
{

  MESSAGE("typemap in sur Objet Corba FIELDINT sans reference");

  SCRUTE($input);

  PyObject* pdict = PyDict_New();
  PyDict_SetItemString(pdict, "__builtins__", PyEval_GetBuiltins());
  PyRun_String("import CORBA", Py_single_input, pdict, pdict);
 
  PyRun_String("o = CORBA.ORB_init([''], CORBA.ORB_ID);", Py_single_input,
                   pdict, pdict);
 
  PyObject* orb = PyDict_GetItemString(pdict, "o");

  // Ask omniORBpy to transform FIELDINT (python Corba) ptr to IOR string

  PyObject* iorFieldInt
    = PyObject_CallMethod(orb, (char*)"object_to_string", (char*)"O", $input);
 
  if (iorFieldInt == Py_None)
    return NULL;
  char * s = PyString_AsString(PyObject_Str(iorFieldInt));
 
  // Ask omniORB to convert IOR string to FIELDINT (C++ Corba) ptr

  int argc = 0;
  char *xargv = (char*)"";
  char **argv = &xargv;
  CORBA::ORB_var ORB = CORBA::ORB_init(argc, argv);
  CORBA::Object_var O =  ORB->string_to_object(s);
  SCRUTE(O);
  SALOME_MED::FIELDINT_ptr t = SALOME_MED::FIELDINT::_narrow(O);
  SCRUTE(CORBA::is_nil(t));

  $1 = t;
  SCRUTE($1);
}
#endif


/*
  managing C++ exception in the Python API
*/
/*%exception
{
  class PyAllowThreadsGuard {
   public:
    PyAllowThreadsGuard() { _save = PyEval_SaveThread(); }
    ~PyAllowThreadsGuard() { PyEval_RestoreThread(_save); }
   private:
    PyThreadState *_save;
  };

  PyAllowThreadsGuard guard;

  $action
}*/

SALOME_MED::FIELDDOUBLE_ptr createCorbaFieldDouble(SALOME_MED::SUPPORT_ptr,
						   FIELDDOUBLE *,
						   bool ownCppPtr=false);

SALOME_MED::FIELDINT_ptr createCorbaFieldInt(SALOME_MED::SUPPORT_ptr,
					     FIELDINT *,
					     bool ownCppPtr=false);

SALOME_MED::SUPPORT_ptr createCorbaSupport(const SUPPORT *);

FIELDDOUBLE * createLocalFieldDouble(const int, const int);

FIELDINT * createLocalFieldInt(const int, const int);

SALOME_MED::MESH_ptr createCorbaMesh(MESH * mesh);

%{
  SALOME_MED::FIELDDOUBLE_ptr createCorbaFieldDouble(SALOME_MED::SUPPORT_ptr mySupportIOR,FIELDDOUBLE * field, bool ownCppPtr=false)
    {
      SCRUTE(field);

      // MT : Keep the local field name
      //      field->setName("Corba Double Field");

      field->setDescription("Got From A Local One");

      FIELDTEMPLATE_I<double,FullInterlace> *fieldimpl = new FIELDTEMPLATE_I<double,FullInterlace>(field, ownCppPtr);

      SALOME_MED::FIELDDOUBLE_ptr fieldcorba2 = fieldimpl->_this();

      SCRUTE(fieldimpl);

      SCRUTE(fieldcorba2);

      MESSAGE("Test de tirarge sur le pointeur Corba Field dans le cxx");

      char * name = fieldcorba2->getName();

      SCRUTE(name);

      delete [] name;

      // try to set support to field
      ::MEDMEM::SUPPORT * sup = 0;
      if ( SUPPORT_i * sup_i = Med_Gen_i::DownCast< SUPPORT_i * >( mySupportIOR ))
      {
        std::map < int,::MEDMEM::SUPPORT *>::iterator index_supp = 
          SUPPORT_i::supportMap.find( sup_i->getCorbaIndex() );
        if ( index_supp != SUPPORT_i::supportMap.end() )
          sup = index_supp->second;
      }
      SCRUTE( sup );
      if ( sup )
        field->setSupport( sup );

      return fieldcorba2;
    }

  SALOME_MED::FIELDINT_ptr createCorbaFieldInt(SALOME_MED::SUPPORT_ptr mySupportIOR,FIELDINT * field, bool ownCppPtr=false)
    {
      SCRUTE(field);

      // MT : Keep the local field name
      //    field->setName("Corba Integer Field");

      field->setDescription("Got From A Local One");

      FIELDTEMPLATE_I<int,FullInterlace> * fieldimpl = new FIELDTEMPLATE_I<int,FullInterlace>(field, ownCppPtr);

      SALOME_MED::FIELDINT_ptr fieldcorba2 = fieldimpl->_this();

      SCRUTE(fieldimpl);

      SCRUTE(fieldcorba2);

      MESSAGE("Test de tirarge sur le pointeur Corba Field dans le cxx");

      char * name = fieldcorba2->getName();

      SCRUTE(name);

      delete [] name;

      // try to set support to field
      ::MEDMEM::SUPPORT * sup = 0;
      if ( SUPPORT_i * sup_i = Med_Gen_i::DownCast< SUPPORT_i * >( mySupportIOR ))
      {
        std::map < int,::MEDMEM::SUPPORT *>::iterator index_supp = 
          SUPPORT_i::supportMap.find( sup_i->getCorbaIndex() );
        if ( index_supp != SUPPORT_i::supportMap.end() )
          sup = index_supp->second;
      }
      SCRUTE( sup );
      if ( sup )
        field->setSupport( sup );

      return fieldcorba2;
    }

  SALOME_MED::SUPPORT_ptr createCorbaSupport(const SUPPORT * const support)
    {
      SCRUTE(support);

      SUPPORT_i * supportimpl = new SUPPORT_i(support);

      SALOME_MED::SUPPORT_ptr supportcorba =
	supportimpl->POA_SALOME_MED::SUPPORT::_this();

      SCRUTE(supportimpl);

      SCRUTE(supportcorba);

      return supportcorba;
    }

  SALOME_MED::MESH_ptr createCorbaMesh(MESH * mesh)
    {
      SCRUTE(mesh);

      MESH_i * meshimpl = new MESH_i(mesh);

      SALOME_MED::MESH_ptr meshcorba =
	meshimpl->POA_SALOME_MED::MESH::_this();

      SCRUTE(meshimpl);

      SCRUTE(meshcorba);

      return meshcorba;
    }

  FIELDDOUBLE * createLocalFieldDouble(const int NumberOfComponents,
				      const int LengthValue)
    {
      SCRUTE(NumberOfComponents);
      SCRUTE(LengthValue);

      //FIELD<double> * fieldloc =  new FIELD<double>();
      FIELDDOUBLE * fieldloc =  new FIELDDOUBLE();
      fieldloc -> allocValue(NumberOfComponents,LengthValue);

      return fieldloc;
    }

  FIELDINT * createLocalFieldInt(const int NumberOfComponents,
				 const int LengthValue)
    {
      SCRUTE(NumberOfComponents);
      SCRUTE(LengthValue);

      //FIELD<int> * fieldloc =  new FIELD<int>();
      FIELDINT * fieldloc =  new FIELDINT();
      fieldloc -> allocValue(NumberOfComponents,LengthValue);

      return fieldloc;
    }
%}
