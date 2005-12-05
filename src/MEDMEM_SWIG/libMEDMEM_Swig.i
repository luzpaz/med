// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
%module libMEDMEM_Swig

%{
#include <sstream>
#include <string>
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_GibiMeshDriver.hxx"
#include "MEDMEM_PorflowMeshDriver.hxx"
#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_SkyLineArray.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Array.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_MedFieldDriver.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Med.hxx"
#include "MEDMEM_Unit.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_MedMedDriver.hxx"
#include "MEDMEM_Grid.hxx"
#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_DriverFactory.hxx"
#include "MEDMEM_SWIG_Templates.hxx"

  using namespace MEDMEM;
  using namespace MED_EN;

typedef FIELD <double> FIELDDOUBLE;
typedef FIELD <int> FIELDINT;

%}

// SWIG needs these typedefs to wrap FIELDDOUBLE*, for ex., to something like
// <C++ FIELD<double> instance at _d0709808_p_FIELDDOUBLE>, not to
// <SWIG Object at _d0709808_p_FIELDDOUBLE> which has no attributes
typedef FIELD <double> FIELDDOUBLE;
typedef FIELD <int> FIELDINT;

%include "typemaps.i"
%include "my_typemap.i"

/*
  managing C++ exception in the Python API
*/

%exception
{
  try
    {
      $action
    }
  catch(MEDEXCEPTION& exception)
    {
      PyErr_SetString(PyExc_RuntimeError,exception.what());
      return NULL;
    }
}

/*
  managing the use of operator= of any class by renaming it assign()
  because assignment can't be overloaded in python.

  In python, you would then use

  a.assign(b)              # a = b
*/

%rename(assign) *::operator=;

/*
  typemap for vector<FAMILY *> C++ object
*/

%typemap(python,in) vector<FAMILY *>, const vector<FAMILY *>
{
  /* typemap in for vector<FAMILY *> */
  /* Check if is a list */

  if (PyList_Check($input)) {
    int size = PyList_Size($input);
    $1.resize(size);

    for (int i=0; i < size; i++)
      {
	PyObject * tmp = PyList_GetItem($input,i);
	FAMILY * f;

	int err = SWIG_ConvertPtr(tmp, (void **) &f, $descriptor(FAMILY *),
				  SWIG_POINTER_EXCEPTION);

	if (err == -1)
	  {
	    char * message = "Error in typemap(python,in) for vector<FAMILY *> each component should be a FAMILY pointer";
	    PyErr_SetString(PyExc_RuntimeError, message);
	    return NULL;
	  }
	// mpv: for compatibility with SWIG 1.3.24 SwigValueWrapper
	// $1[i] = f;
	$1.at(i) = f;
      }
  }
  else
    {
      PyErr_SetString(PyExc_TypeError,"not a list");
      return NULL;
    }
}

%typemap(python,out) vector<FAMILY *>
{
  /* typemap out for vector<FAMILY *> */
  int size = $1.size();
  $result = PyList_New(size);

  for (int i=0;i<size;i++)
    {
      // mpv: for compatibility with SWIG 1.3.24 SwigValueWrapper
      //PyObject * tmp = SWIG_NewPointerObj($1[i],$descriptor(FAMILY *),0);
      PyObject * tmp = SWIG_NewPointerObj($1.at(i),$descriptor(FAMILY *),0);

      PyList_SetItem($result,i,tmp);
    }
}

/*
  typemap for vector<SUPPORT *> C++ object
*/

%typemap(python,in) vector<SUPPORT *>, const vector<SUPPORT *>
{
  /* typemap in for vector<SUPPORT *> */
  /* Check if is a list */

  if (PyList_Check($input)) {
    int size = PyList_Size($input);
    $1.resize(size);

    for (int i=0; i < size; i++)
      {
	PyObject * tmp = PyList_GetItem($input,i);
	SUPPORT * s;

	int err = SWIG_ConvertPtr(tmp, (void **) &s, $descriptor(SUPPORT *),
				  SWIG_POINTER_EXCEPTION);

	if (err == -1)
	  {
	    char * message = "Error in typemap(python,in) for vector<SUPPORT *> each component should be a SUPPORT pointer";
	    PyErr_SetString(PyExc_RuntimeError, message);
	    return NULL;
	  }
	// mpv: for compatibility with SWIG 1.3.24 SwigValueWrapper
	//$1[i] = s;
	$1.at(i) = s;
      }
  }
  else
    {
      PyErr_SetString(PyExc_TypeError,"not a list");
      return NULL;
    }
}

%typemap(python,out) vector<SUPPORT *>
{
  /* typemap out for vector<SUPPORT *> */
  int size = $1.size();
  $result = PyList_New(size);

  for (int i=0;i<size;i++)
    {
      // mpv: for compatibility with SWIG 1.3.24 SwigValueWrapper
      //PyObject * tmp = SWIG_NewPointerObj($1[i],$descriptor(SUPPORT *),0);
      PyObject * tmp = SWIG_NewPointerObj($1.at(i),$descriptor(SUPPORT *),0);

      PyList_SetItem($result,i,tmp);
    }
}

%typemap(python,in) vector< FIELD<double> * >, const vector< FIELD<double> * >
{
    /* typemap in for vector<FIELD<double> *> */
  /* Check if is a list */

  if (PyList_Check($input)) {
    int size = PyList_Size($input);
    $1.resize(size);

    for (int i=0; i < size; i++)
      {
	PyObject * tmp = PyList_GetItem($input,i);
	FIELDDOUBLE * s;

	int err = SWIG_ConvertPtr(tmp, (void **) &s, $descriptor(FIELDDOUBLE *),
				  SWIG_POINTER_EXCEPTION);

	if (err == -1)
	  {
	    char * message = "Error in typemap(python,in) for vector<FIELD<double> *> each component should be a SUPPORT pointer";
	    PyErr_SetString(PyExc_RuntimeError, message);
	    return NULL;
	  }

	// mpv: for compatibility with SWIG 1.3.24 SwigValueWrapper
	//$1[i] = s;
	$1.at(i) = s;
      }
  }
  else
    {
      PyErr_SetString(PyExc_TypeError,"not a list");
      return NULL;
    }
}

%typemap(python,out) vector<FIELD<double> *>
{
  /* typemap out for vector<FIELD<double> *> */
  int size = $1.size();
  $result = PyList_New(size);

  for (int i=0;i<size;i++)
    {
      // mpv: for compatibility with SWIG 1.3.24 SwigValueWrapper
      //PyObject * tmp = SWIG_NewPointerObj($1[i],$descriptor(FIELD<double> *),0);
      PyObject * tmp = SWIG_NewPointerObj($1.at(i),$descriptor(FIELD<double> *),0);

      PyList_SetItem($result,i,tmp);
    }
}

%typemap(python,in) vector<FIELD<int> *>, const vector<FIELD<int> *>
{
    /* typemap in for vector<FIELD<int> *> */
  /* Check if is a list */

  if (PyList_Check($input)) {
    int size = PyList_Size($input);
    $1.resize(size);

    for (int i=0; i < size; i++)
      {
	PyObject * tmp = PyList_GetItem($input,i);
	FIELD<int> * s;

	int err = SWIG_ConvertPtr(tmp, (void **) &s, $descriptor(FIELD<int> *),
				  SWIG_POINTER_EXCEPTION);

	if (err == -1)
	  {
	    char * message = "Error in typemap(python,in) for vector<FIELD<int> *> each component should be a SUPPORT pointer";
	    PyErr_SetString(PyExc_RuntimeError, message);
	    return NULL;
	  }

	// mpv: for compatibility with SWIG 1.3.24 SwigValueWrapper
	//$1[i] = s;
	$1.at(i) = s;
      }
  }
  else
    {
      PyErr_SetString(PyExc_TypeError,"not a list");
      return NULL;
    }
}

%typemap(python,out) vector<FIELD<int> *>
{
  /* typemap out for vector<FIELD<int> *> */
  int size = $1.size();
  $result = PyList_New(size);

  for (int i=0;i<size;i++)
    {
      // mpv: for compatibility with SWIG 1.3.24 SwigValueWrapper
      //PyObject * tmp = SWIG_NewPointerObj($1[i],$descriptor(FIELD<int> *),0);
      PyObject * tmp = SWIG_NewPointerObj($1.at(i),$descriptor(FIELD<int> *),0);

      PyList_SetItem($result,i,tmp);
    }
}

%typemap(python,out) char *
{
  /* typemap out for char * */

  $result = PyString_FromString($1);
}

%typemap(python,out) string {
    $result = PyString_FromString($1.c_str());
}

%typemap(python,in) string {
  $1=string(PyString_AsString($input));
}

/*
  typemap in for PyObject * fonction Python wrapping of a
  double or int fonction pointeur
*/

%typemap(python,in) PyObject * double_function, PyObject * integer_function
{
  /* typemap in for double or integer callable fonction pointeur */
  /* Check if it is a callable fonction pointer */

  if(PyCallable_Check($input) == 0)
    {
      char * message = "Error in typemap(python,in) for double or integer callable fonction pointeur : the argument should be a callable object";
      PyErr_SetString(PyExc_RuntimeError, message);
      return NULL;
    }

  $1 = $input;
}

/*
  MACRO converting C array <arrayvar> of length <size> into a PyList
  by calling type_converter() for each array element.
  It reports error in <method> in failure case
*/

%define TYPEMAP_OUTPUT_ARRAY(arrayvar, size, type_converter, method)
{
  PyObject *py_list = PyList_New(size);
  for (int i=0; i < size; i++)
  {
    int err = PyList_SetItem(py_list, i, type_converter( arrayvar[ i ]));
    if(err)
    {
      char * message = "Error in " #method;
      PyErr_SetString(PyExc_RuntimeError, message);
      return NULL;
    }
  }
  PyObject * result = Py_BuildValue("O", py_list);
  Py_DECREF(py_list);
  return result;
}
%enddef

/*
  Helper function to be used as type_converter in TYPEMAP_OUTPUT_ARRAY
*/

%{
  PyObject *PyString_FromStdString(const std::string &str) { return PyString_FromString(str.c_str()); }
%}


/*
  enum of the C++ MED used in the Python API
*/

typedef enum {MED_CARTESIAN, MED_POLAR, MED_BODY_FITTED} med_grid_type;

typedef enum {MED_FULL_INTERLACE, MED_NO_INTERLACE} medModeSwitch;

typedef enum {MED_LECT, MED_ECRI, MED_REMP} med_mode_acces;

typedef enum {ASCENDING=7,DESCENDING=77} med_sort_direc;

typedef enum {MED_CELL, MED_FACE, MED_EDGE, MED_NODE,
	      MED_ALL_ENTITIES} medEntityMesh;

typedef enum {MED_NONE=0, MED_POINT1=1, MED_SEG2=102, MED_SEG3=103,
	      MED_TRIA3=203, MED_QUAD4=204, MED_TRIA6=206, MED_QUAD8=208,
	      MED_TETRA4=304, MED_PYRA5=305, MED_PENTA6=306,
	      MED_HEXA8=308, MED_TETRA10=310, MED_PYRA13=313,
	      MED_PENTA15=315, MED_HEXA20=320, MED_POLYGON = 400, MED_POLYHEDRA = 500,
	      MED_ALL_ELEMENTS=999} medGeometryElement;

typedef enum {MED_NODAL, MED_DESCENDING} medConnectivity ;

typedef enum {MED_DRIVER=0, GIBI_DRIVER=1, PORFLOW_DRIVER = 2, VTK_DRIVER=254,
	      NO_DRIVER=255, ASCII_DRIVER = 3} driverTypes;

typedef enum {MED_REEL64=6, MED_INT32=24, MED_INT64=26} med_type_champ;

typedef struct { int dt; int it; } DT_IT_;

typedef enum {V21 = 26, V22 = 75} medFileVersion;

medFileVersion getMedFileVersionForWriting();

void setMedFileVersionForWriting(medFileVersion version);

%{
  medFileVersion getMedFileVersionForWriting()
    {
      return (medFileVersion) DRIVERFACTORY::getMedFileVersionForWriting();
    }

  void setMedFileVersionForWriting(medFileVersion version)
    {
      DRIVERFACTORY::setMedFileVersionForWriting((medFileVersion) version);
    }
%}

%extend DT_IT_ {
  int getdt()
    {
      return self->dt;
    }

  int getit()
    {
      return self->it;
    }
}
/*
  Class et methodes du MED++ que l'on utilise dans l'API Python
*/

class CELLMODEL
{
 public:
  CELLMODEL();

  CELLMODEL(medGeometryElement t);

  CELLMODEL(const CELLMODEL &m);

  int getNumberOfVertexes();

  int getNumberOfNodes();

  int getDimension();

  medGeometryElement getType();

  int getNumberOfConstituents(int dim);

  int getNodeConstituent(int dim,int num,int nodes_index);

  medGeometryElement getConstituentType(int dim,int num);

  int getNumberOfConstituentsType();

  ~CELLMODEL();

  %extend {
    %newobject getName();
    char * getName()
      {
	string tmp_str = self->getName();
	char * tmp = new char[strlen(tmp_str.c_str())+1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }

    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing CELLMODEL : " << *self << endl;
	return strdup(mess.str().c_str());
      }
  }
};

class SUPPORT
{
 public:
  SUPPORT();

  SUPPORT(const SUPPORT & m);

  ~SUPPORT();

  void update ( void );

  MESH * getMesh() const;

  void setMesh(MESH * Mesh);

  medEntityMesh getEntity() const;

  void setEntity(medEntityMesh Entity);

  bool isOnAllElements() const;

  void setAll(bool All);

  int getNumberOfTypes() const;

  void setNumberOfGeometricType(int NumberOfGeometricType);

  int getNumberOfElements(medGeometryElement GeometricType) const;

  void setGeometricType(medGeometryElement *GeometricType);

  void setNumberOfElements(int *NumberOfElements);

  void setTotalNumberOfElements(int TotalNumberOfElements);

  void getBoundaryElements();

  void setNumber(const int * index, const int* value);

  bool deepCompare(const SUPPORT &support) const;

  %extend {
    SUPPORT(MESH* Mesh, char * Name="", medEntityMesh Entity=MED_CELL)
      {
	return new SUPPORT(Mesh,string(Name),Entity);
      }

    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing SUPPORT : " << *self << endl;
	char * tmp = const_cast <char *> (mess.str().c_str());
	char * returned = strdup(tmp);
	return returned;
      }

    void setpartial(char * Description, int NumberOfGeometricType,
		    int TotalNumberOfElements, medGeometryElement *GeometricType,
		    int *NumberOfElements, int *NumberValue)
      {
	self->setpartial(string(Description), NumberOfGeometricType,
			 TotalNumberOfElements, GeometricType,
			 NumberOfElements, NumberValue);
      }

    void setName(char * Name)
      {
	self->setName(string(Name));
      }

    %newobject getName();
    const char * getName()
      {
	string tmp_str = self->getName();
	char * tmp = new char[strlen(tmp_str.c_str())+1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }

    void setDescription(char * Description)
      {
	self->setDescription(string(Description));
      }

    %newobject getDescription();
    const char * getDescription()
      {
	string tmp_str = self->getDescription();
	char * tmp = new char[strlen(tmp_str.c_str())+1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }

    PyObject * getTypes()
      {
	const medGeometryElement * types = self->getTypes();
	int size = self->getNumberOfTypes();
        TYPEMAP_OUTPUT_ARRAY( types, size, PyInt_FromLong, SUPPORT::getTypes );
      }

    PyObject * getNumber(medGeometryElement GeometricType)
      {
	const int * number = self->getNumber(GeometricType);
	int size = self->getNumberOfElements(GeometricType);
        TYPEMAP_OUTPUT_ARRAY( number, size, PyInt_FromLong, SUPPORT::getNumber );
      }

    PyObject * getNumberIndex()
      {
	const int * numberindex = self->getNumberIndex();
	int size = (self->getNumberOfElements(MED_ALL_ELEMENTS))+1;
        TYPEMAP_OUTPUT_ARRAY( numberindex, size, PyInt_FromLong, SUPPORT::getNumberIndex );
      }
    %newobject getComplement() const;
    SUPPORT *getComplement() const
      {
	return self->getComplement();
      }

    %newobject substract(const SUPPORT& other) const;
    SUPPORT *substract(const SUPPORT& other) const
      {
	return self->substract(other);
      }

    %newobject getBoundaryElements(medEntityMesh Entity) const;
    SUPPORT *getBoundaryElements(medEntityMesh Entity) const
      {
	return self->getBoundaryElements(Entity);
      }
  }
};

class FAMILY : public SUPPORT
{
 public:
  FAMILY();

  FAMILY(const FAMILY & m);

  ~FAMILY();

  FAMILY & operator=(const FAMILY &fam);

  void setIdentifier(int Identifier);

  void setNumberOfAttributes(int NumberOfAttribute);

  void setAttributesIdentifiers(int * AttributeIdentifier);

  void setAttributesValues(int * AttributeValue);

  void setAttributesDescriptions(string * AttributeDescription);

  void setNumberOfGroups(int NumberOfGroups);

  void setGroupsNames(string * GroupName);

  int getIdentifier() const;

  int getNumberOfAttributes() const;

  int getNumberOfGroups() const;

  %extend {
    FAMILY(MESH* Mesh, int Identifier, char * Name, int NumberOfAttribute,
	   int *AttributeIdentifier, int *AttributeValue,
	   char * AttributeDescription, int NumberOfGroup,
	   char * GroupName, int * MEDArrayNodeFamily,
	   int ** MEDArrayCellFamily, int ** MEDArrayFaceFamily,
	   int ** MEDArrayEdgeFamily)
      {
	return new FAMILY(Mesh,Identifier,string(Name),NumberOfAttribute,
			  AttributeIdentifier,AttributeValue,
			  string(AttributeDescription),NumberOfGroup,
			  string(GroupName), MEDArrayNodeFamily,
			  MEDArrayCellFamily, MEDArrayFaceFamily,
			  MEDArrayEdgeFamily);
      }

    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing Family : " << *self << endl;
	return strdup(mess.str().c_str());
      }

    %newobject getAttributeDescription(int );
    const char * getAttributeDescription(int i)
      {
	string tmp_str = self->getAttributeDescription(i);
	char * tmp = new char[strlen(tmp_str.c_str())+1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }

    %newobject getGroupName(int );
    const char * getGroupName(int i)
      {
	string tmp_str = self->getGroupName(i);
	char * tmp = new char[strlen(tmp_str.c_str())+1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }

    PyObject * getAttributesIdentifiers()
      {
	const int * attributesids = self->getAttributesIdentifiers();
	int size = self->getNumberOfAttributes();
        TYPEMAP_OUTPUT_ARRAY(attributesids,size,PyInt_FromLong,FAMILY::getAttributesIdentifiers );
      }

    PyObject * getAttributesValues()
      {
	const int * attributesvals = self->getAttributesValues();
	int size = self->getNumberOfAttributes();
        TYPEMAP_OUTPUT_ARRAY(attributesvals,size,PyInt_FromLong,FAMILY::getAttributesValues );
      }
  }
};




class FIELD_
{
public:
  //  FIELD_(const SUPPORT * Support, const int NumberOfComponents);

  ~FIELD_();

  void rmDriver(int index=0);

  void setIterationNumber (int IterationNumber);
  int getIterationNumber() const;

  void setTime(double Time);
  double   getTime() const;

  void setOrderNumber (int OrderNumber);
  int getOrderNumber() const;

  void setValueType(med_type_champ ValueType) ;
  med_type_champ getValueType() ;

  SUPPORT * getSupport();
  void setSupport(SUPPORT * support);

  void  setNumberOfComponents(int NumberOfComponents);
  int getNumberOfComponents() const;

  void setNumberOfValues(int NumberOfValues);
  int getNumberOfValues() const;

  string   getName() const;
  string   getDescription() const;
  string   getComponentName(int i) const;
  string   getComponentDescription(int i) const;
  string   getMEDComponentUnit(int i) const;
  void     setName(string Name);
  void     setComponentName(int i, string ComponentName);
  void     setMEDComponentUnit(int i, string MEDComponentUnit);
  void     setDescription(string Description);
  void     setComponentDescription(int i, string ComponentDescription);

  %extend {
    int addDriver(driverTypes driverType,
		  char * fileName="Default File Name.med",
		  char * driverName="Default Field Name",
		  med_mode_acces access=MED_REMP)
      {
	return self->addDriver(driverType,string(fileName),
			       string(driverName),access);
      }
    %newobject getSupportAndOwner();
    SUPPORT * getSupportAndOwner()
      {
	return (SUPPORT *)self->getSupport();
      }
  }
};

template< class T1 >
class FIELD : public FIELD_
{
public:
  ~FIELD();

  FIELD(const SUPPORT * Support, const int NumberOfComponents);

  /*
    WARNING:
    other constructor of FIELD (C++ FIELD<T1>) object.
    Only one constructor could be wrapped and
    the others commented out when using
    SWIG with a version lesser than 1.3
  */

  FIELD();

  FIELD(const FIELD & m);

  void read(int index=0);

  T1 getValueIJ(int i,int j) const;

  void setValue(medModeSwitch mode, T1* value);

  void setValueI(medModeSwitch mode, int i, T1* value);

  void setValueIJ(int i, int j, T1 value);

  void allocValue(const int NumberOfComponents);

  void deallocValue();

  void applyLin(T1 a, T1 n);

  void applyPow(T1 scalar);

  T1 normMax();
  T1 norm2();
  T1 normL2(int component, const FIELD<double> * p_field_volume=NULL) const;
  T1 normL2(const FIELD<double> * p_field_volume=NULL) const;
  T1 normL1(int component, const FIELD<double> * p_field_volume=NULL) const;
  T1 normL1(const FIELD<double> * p_field_volume=NULL) const;


  %extend {
    PyObject *  applyPyFunc( PyObject * func )
    {
	MESSAGE("Appel de applyPyFunc");
	if (!PyCallable_Check(func)) {
	    PyErr_SetString(PyExc_TypeError, "FIELD.applyPyFunc prend en argument une fonction");
	    return NULL;
	}

    	int nComp=self->getNumberOfComponents();
    	int nVal=self->getNumberOfValues();
	for (int i=1; i!=nVal+1; ++i)
	    for ( int j=1 ;j!=nComp+1 ;++j )
	    {
              self->setValueIJ(i,j, Binding<T1>::Functor( func, self->getValueIJ(i,j) ) );
	    }
	 PyObject * result = Binding<double>::Traducer(nComp*nVal);
	 return result;
    }

    %newobject __add__(const FIELD & );
    FIELD * __add__(const FIELD & m)
      {
	MESSAGE("operator +  : Creation of the addition of two FIELDs");

	FIELD<T1>* result = FIELD<T1>::add( *(FIELD<T1>*)self , (FIELD<T1>&)m );
	return (FIELD<T1>*) result;
      }

    %newobject __sub__(const FIELD & );
    FIELD * __sub__(const FIELD & m)
      {
	MESSAGE("operator -  : Creation of the substraction of two FIELDs");
	FIELD<T1>* result = FIELD<T1>::sub( *(FIELD<T1>*)self , (FIELD<T1>&)m );
	return (FIELD<T1>*) result;
      }

    %newobject __mul__(const FIELD & );
    FIELD * __mul__(const FIELD & m)
      {
	MESSAGE("operator *  : Creation of the multiplication of two FIELDs");
	FIELD<T1>* result = FIELD<T1>::mul( *(FIELD<T1>*)self , (FIELD<T1>&)m );
	return (FIELD<T1>*) result;
      }

    %newobject __div__(const FIELD & );
    FIELD * __div__(const FIELD & m)
      {
	MESSAGE("operator /  : Creation of the division of two FIELDs");
	FIELD<T1>* result = FIELD<T1>::div( *(FIELD<T1>*)self , (FIELD<T1>&)m );
	return (FIELD<T1>*) result;
      }

    %newobject addDeep(const FIELDINT & );
    FIELD * addDeep(const FIELD & m)
      {
	MESSAGE("operator +  : Creation of the addition of two FIELDINTs");
	FIELD<T1>* result = FIELD<T1>::addDeep( *(FIELD<T1>*)self , (FIELD<T1>&)m );
	return (FIELD<T1>*) result;
      }

    %newobject subDeep(const FIELD & );
    FIELD * subDeep(const FIELD & m)
      {
	MESSAGE("operator -  : Creation of the substraction of two FIELDs");
	FIELD<T1>* result = FIELD<T1>::subDeep( *(FIELD<T1>*)self , (FIELD<T1>&)m );
	return (FIELD<T1>*) result;
      }

    %newobject mulDeep(const FIELD & );
    FIELD * mulDeep(const FIELD & m)
      {
	MESSAGE("operator *  : Creation of the multiplication of two FIELDs");
	FIELD<T1>* result = FIELD<T1>::mulDeep( *(FIELD<T1>*)self , (FIELD<T1>&)m );
	return (FIELD<T1>*) result;
      }

    %newobject divDeep(const FIELD & );
    FIELD * divDeep(const FIELD & m)
      {
	MESSAGE("operator /  : Creation of the division of two FIELDs");
	FIELD<T1>* result = FIELD<T1>::divDeep( *(FIELD<T1>*)self , (FIELD<T1>&)m );
	return (FIELD<T1>*) result;
      }

    FIELD (const SUPPORT * Support, driverTypes driverType,
		 char * fileName, char * fieldName,
		 const int iterationNumber,
		 const int orderNumber)
      {
	return new FIELD<T1>(Support, driverType, string(fileName),
                             string(fieldName),iterationNumber,
                             orderNumber);
      }

    void write(int index=0, char * driverName="")
      {
	self->write(index, string(driverName));
      }

    void writeAppend(int index=0, char * driverName="")
      {
	self->writeAppend(index, string(driverName));
      }

    PyObject * getValue(medModeSwitch Mode)
      {
	int size = (self->getNumberOfComponents())*
	  ((self->getSupport())->getNumberOfElements(MED_ALL_ELEMENTS));

	const T1 * value = self->getValue(Mode);

        TYPEMAP_OUTPUT_ARRAY( value, size, Binding< T1 >::Traducer, FIELD::getValue );
      }

    %newobject getValueI(medModeSwitch , int );
    PyObject * getValueI(medModeSwitch Mode, int index)
      {
	int size = self->getNumberOfComponents();

	if ( Mode == MED_NO_INTERLACE ) size = (self->getSupport())->getNumberOfElements(MED_ALL_ELEMENTS);

	const T1 * value = self->getValueI(Mode,index);

        TYPEMAP_OUTPUT_ARRAY( value, size, Binding< T1 >::Traducer, FIELD::getValueI );
      }

    void allocValue2(int NumberOfComponents, int LengthValue)
      {
	self->allocValue(NumberOfComponents, LengthValue);
      }

    %newobject extract(const SUPPORT *subSupport);
    FIELD *extract(const SUPPORT *subSupport)
      {
	FIELD<T1>* result=self->extract(subSupport);
	return (FIELD<T1> *)result;
      }
  }
};
%template (FIELDDOUBLE) FIELD <double>;
%template (FIELDINT) FIELD <int>;


class GROUP : public SUPPORT
{
public:
  GROUP();

  ~GROUP();

  void setNumberOfFamilies(int numberOfFamilies);
  void setFamilies(vector<FAMILY*> Family);

  int getNumberOfFamilies() const ;
  vector<FAMILY*> getFamilies() const ;
  FAMILY * getFamily(int i) const ;
};

class MESH
{
public :
  /*
    WARNING:
    other constructor of MESH object.
    Only one constructor could be wrapped and
    the others commented out when using
    SWIG with a version lesser than 1.3
  */

  MESH();

  ~MESH();

  void rmDriver(int index=0);

  void read(int index=0);

  int getSpaceDimension();

  int getMeshDimension();

  int getNumberOfNodes();

  bool getIsAGrid();

  const double getCoordinate(int Number, int Axis);

  int getNumberOfTypes(medEntityMesh Entity);

  void calculateConnectivity(medModeSwitch Mode,medConnectivity ConnectivityType,medEntityMesh Entity);

  int  getNumberOfElements(medEntityMesh Entity,medGeometryElement Type);

  int getNumberOfFamilies(medEntityMesh Entity);

  int getElementNumber(medConnectivity ConnectivityType, medEntityMesh Entity, medGeometryElement Type, int * connectivity);

  CELLMODEL * getCellsTypes(medEntityMesh Entity);

  FAMILY* getFamily(medEntityMesh Entity,int i);

  int getNumberOfGroups(medEntityMesh Entity);

  GROUP * getGroup(medEntityMesh Entity,int i);

  medGeometryElement getElementType(medEntityMesh Entity,int Number);

  int getElementContainingPoint(const double *coord);

  int getNumberOfTypesWithPoly(medEntityMesh Entity);
  int getNumberOfPolygons();
  int getNumberOfPolyhedronFaces();
  int getNumberOfPolyhedron();
  int getNumberOfElementsWithPoly(medEntityMesh Entity,
                                  medGeometryElement Type);
  bool existPolygonsConnectivity(medConnectivity ConnectivityType,
                                 medEntityMesh Entity);
  bool existPolyhedronConnectivity(medConnectivity ConnectivityType,
                                   medEntityMesh Entity);
  medGeometryElement getElementTypeWithPoly(medEntityMesh Entity,int Number);
  
  %extend {
    %newobject getBoundaryElements(medEntityMesh );
    SUPPORT * getBoundaryElements(medEntityMesh Entity)
      {
	return self->getBoundaryElements(Entity);
      }

    %newobject getSkin(const SUPPORT * );
    SUPPORT * getSkin(const SUPPORT * Support3D)
      {
	return self->getSkin(Support3D);
      }

    %newobject mergeSupports(const vector<SUPPORT *> );
    SUPPORT * mergeSupports(const vector<SUPPORT *> Supports)
      {
	return self->mergeSupports(Supports);
      }

    %newobject intersectSupports(const vector<SUPPORT *> );
    SUPPORT * intersectSupports(const vector<SUPPORT *> Supports)
      {
	return self->intersectSupports(Supports);
      }

    %newobject mergeFieldsDouble(const vector< FIELD<double>* > others);
    FIELDDOUBLE * mergeFieldsDouble(const vector< FIELD<double>* > others)
      {
	return (FIELDDOUBLE *)self->mergeFields<double>(others);
      }

    %newobject mergeFieldsInt(const vector< FIELD<int>* > others);
    FIELDINT * mergeFieldsInt(const vector< FIELD<int>* > others)
      {
	return (FIELDINT *)self->mergeFields<int>(others);
      }

    CELLMODEL getCellType(medEntityMesh Entity,int i)
      {
	return self->getCellsTypes(Entity)[i];
      }

    MESH (driverTypes driverType, char * fileName, char * meshName)
      {
	return new MESH(driverType, string(fileName), string(meshName));
      }

    int addDriver(driverTypes driverType,
		  char * fileName="Default File Name.med",
		  char * driverName="Default Mesh Name",
		  med_mode_acces access=MED_REMP)
      {
	return self->addDriver(driverType,string(fileName),
			       string(driverName),access);
      }

    void write(int index=0, char * driverName="")
      {
	self->write(index, string(driverName));
      }

    void setName(char * name)
      {
	self->setName(string(name));
      }

    %newobject getName();
    const char * getName()
      {
	string tmp_str = self->getName();
	char * tmp = new char[strlen(tmp_str.c_str())+1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }

    %newobject getCoordinatesSystem();
    const char * getCoordinatesSystem()
      {
	string tmp_str = self->getCoordinatesSystem();
	char * tmp = new char[strlen(tmp_str.c_str())+1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }

    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing MESH : " << *self << endl;
	return strdup(mess.str().c_str());
      }

    %newobject getCoordinateName(int );
    const char * getCoordinateName(int i)
      {
	string tmp_str = self->getCoordinatesNames()[i];
	char * tmp = new char[strlen(tmp_str.c_str())+1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }

    %newobject getCoordinateUnit(int );
    const char * getCoordinateUnit(int i)
      {
	string tmp_str = self->getCoordinatesUnits()[i];
	char * tmp = new char[strlen(tmp_str.c_str())+1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }

    PyObject * getCoordinatesNames()
      {
	const string * array = self->getCoordinatesNames();
	int size = self->getSpaceDimension();
        TYPEMAP_OUTPUT_ARRAY( array, size, PyString_FromStdString, MESH::getCoordinatesNames );
      }

    PyObject * getCoordinatesUnits()
      {
	const string * array = self->getCoordinatesUnits();
	int size = self->getSpaceDimension();
        TYPEMAP_OUTPUT_ARRAY( array, size, PyString_FromStdString, MESH::getCoordinatesUnits );
      }

    PyObject * getCoordinates(medModeSwitch Mode)
      {
	const double * array = self->getCoordinates(Mode);
	int size = (self->getSpaceDimension())*(self->getNumberOfNodes());
        TYPEMAP_OUTPUT_ARRAY( array, size, PyFloat_FromDouble, MESH::getCoordinates );
      }

    PyObject * getTypes(medEntityMesh Entity)
      {
	const medGeometryElement * types = self->getTypes(Entity);
	int size = self->getNumberOfTypes(Entity);
        TYPEMAP_OUTPUT_ARRAY( types, size, PyInt_FromLong, MESH::getTypes );
      }

    PyObject * getConnectivity(medModeSwitch Mode,
			       medConnectivity ConnectivityType,
			       medEntityMesh Entity,
			       medGeometryElement Type)
      {
	const int * connectivity = self->getConnectivity(Mode,ConnectivityType,
						   Entity,Type);
	int size = self->getConnectivityLength(Mode,ConnectivityType,Entity,Type);
        TYPEMAP_OUTPUT_ARRAY( connectivity, size, PyInt_FromLong, MESH::getConnectivity );
      }

    PyObject * getConnectivityIndex(medConnectivity ConnectivityType,
				    medEntityMesh Entity)
      {
	const int * connectivity_index =
	  self->getConnectivityIndex(ConnectivityType,Entity);
	int size = (self->getNumberOfElements(Entity,MED_ALL_ELEMENTS))+1;
        TYPEMAP_OUTPUT_ARRAY(connectivity_index,size,PyInt_FromLong,MESH::getConnectivityIndex);
      }

    PyObject * getReverseConnectivity(medConnectivity ConnectivityType,
				      medEntityMesh Entity=MED_CELL)
      {
	const int * reverseconnectivity =
	  self->getReverseConnectivity(ConnectivityType,Entity);
	int size = self->getReverseConnectivityLength(ConnectivityType,Entity);
        TYPEMAP_OUTPUT_ARRAY(reverseconnectivity,size,PyInt_FromLong,MESH::getReverseConnectivity);
      }

    PyObject * getReverseConnectivityIndex(medConnectivity ConnectivityType,
					   medEntityMesh Entity=MED_CELL)
      {
	const int * reverseconnectivity_index =
	  self->getReverseConnectivityIndex(ConnectivityType,Entity);
	int size=self->getReverseConnectivityIndexLength(ConnectivityType,Entity);
        TYPEMAP_OUTPUT_ARRAY(reverseconnectivity_index,size,PyInt_FromLong,MESH::getReverseConnectivityIndex);
      }

    PyObject * getGlobalNumberingIndex(medEntityMesh Entity)
      {
	const int * numberingIndex = self->getGlobalNumberingIndex(Entity);
	int nbOfTypes = self->getNumberOfTypes(Entity);
	int size = nbOfTypes+1;
        TYPEMAP_OUTPUT_ARRAY(numberingIndex,size,PyInt_FromLong,MESH::getGlobalNumberingIndex);
      }


    PyObject * getPolygonsConnectivity(medConnectivity ConnectivityType,
                                       medEntityMesh   Entity)
      {
        const int * array = self->getPolygonsConnectivity(ConnectivityType,Entity);
        int size = self->getPolygonsConnectivityLength(ConnectivityType, Entity);
        TYPEMAP_OUTPUT_ARRAY(array,size,PyInt_FromLong,MESH::getPolygonsConnectivity);
      }
    PyObject * getPolygonsConnectivityIndex(medConnectivity ConnectivityType,
                                            medEntityMesh   Entity)
      {
        const int * array = self->getPolygonsConnectivityIndex(ConnectivityType,Entity);
        int size = self->getNumberOfPolygons() + 1;
        TYPEMAP_OUTPUT_ARRAY(array,size,PyInt_FromLong,MESH::getPolygonsConnectivity);
      }
    PyObject * getPolyhedronConnectivity(medConnectivity ConnectivityType)
      {
        const int * array = self->getPolyhedronConnectivity(ConnectivityType);
        int size = self->getPolyhedronConnectivityLength(ConnectivityType);
        TYPEMAP_OUTPUT_ARRAY(array,size,PyInt_FromLong,MESH::getPolygonsConnectivity);
      }
    PyObject * getPolyhedronIndex(medConnectivity ConnectivityType)
      {
        const int * array = self->getPolyhedronIndex(ConnectivityType);
        int size = self->getNumberOfPolyhedron() + 1;
        TYPEMAP_OUTPUT_ARRAY(array,size,PyInt_FromLong,MESH::getPolyhedronIndex);
      }
    PyObject * getPolyhedronFacesIndex()
      {
        const int * array = self->getPolyhedronFacesIndex();
        int size = self->getNumberOfPolyhedronFaces() + 1;
        TYPEMAP_OUTPUT_ARRAY(array,size,PyInt_FromLong,MESH::getPolyhedronFacesIndex);
      }
    PyObject * getTypesWithPoly(medEntityMesh Entity)
      {
        medGeometryElement * array = self->getTypesWithPoly(Entity);
        int size = self->getNumberOfTypesWithPoly(Entity);
        TYPEMAP_OUTPUT_ARRAY(array,size,PyInt_FromLong,MESH::getTypesWithPoly);
        delete [] array;
      }
      

    %newobject getVolume(const SUPPORT * );
    FIELDDOUBLE * getVolume(const SUPPORT * Support)
      {
	return (FIELDDOUBLE *) self->getVolume(Support);
      }

    %newobject getArea(const SUPPORT * );
    FIELDDOUBLE * getArea(const SUPPORT * Support)
      {
	return (FIELDDOUBLE *) self->getArea(Support);
      }

    %newobject getLength(const SUPPORT * );
    FIELDDOUBLE * getLength(const SUPPORT * Support)
      {
	return (FIELDDOUBLE *) self->getLength(Support);
      }

    %newobject getNormal(const SUPPORT * );
    FIELDDOUBLE * getNormal(const SUPPORT * Support)
      {
	return (FIELDDOUBLE *) self->getNormal(Support);
      }

    %newobject getBarycenter(const SUPPORT * );
    FIELDDOUBLE * getBarycenter(const SUPPORT * Support)
      {
	return (FIELDDOUBLE *) self->getBarycenter(Support);
      }
  }
} ;

class MESHING: public MESH
{
public :
  MESHING();
  ~MESHING();

  void setSpaceDimension   (const int SpaceDimension) ;

  void setNumberOfNodes    (const int NumberOfNodes) ;

  void setNumberOfTypes    (const int NumberOfTypes,
			    const medEntityMesh Entity) ;

  void setTypes            (const medGeometryElement * Types,
			    const medEntityMesh Entity) ;

  void setNumberOfElements (const int * NumberOfElements,
			    const medEntityMesh Entity) ;

  void setConnectivity     (const int * Connectivity,
			    const medEntityMesh Entity,
			    const medGeometryElement Type) ;

  void setConnectivities   (const int * ConnectivityIndex,
			    const int * ConnectivityValue,
			    const medConnectivity ConnectivityType,
			    const medEntityMesh Entity) ;

  void addGroup            (const GROUP & Group) ;

  void setPolygonsConnectivity     (const int * ConnectivityIndex,
				    const int * ConnectivityValue,
				    int nbOfPolygons,
				    const medEntityMesh Entity);

  void setPolyhedraConnectivity     (const int * PolyhedronIndex,
				     const int * FacesIndex,
				     const int * Nodes,
				     int nbOfPolyhedra,
				     const MED_EN::medEntityMesh Entity);

  %extend {
    void setCoordinates(const int SpaceDimension, const int NumberOfNodes,
			const double * Coordinates, const char * System,
			const medModeSwitch Mode)
      {
	self->setCoordinates(SpaceDimension, NumberOfNodes, Coordinates,
			     string(System), Mode);
      }

    void setCoordinatesSystem(const char * System)
      {
	self->setCoordinatesSystem(string(System));
      }

    void setCoordinateName (const char * name, const int i)
      {
	self->setCoordinateName(string(name), i);
      }

    void setCoordinateUnit (const char* unit, const int i)
    {
      self->setCoordinateUnit (string(unit), i) ;
    }
  }
};

class GRID : public MESH
{
 public:
  GRID();

  GRID(const GRID &m);

  ~GRID();

  int getNodeNumber(const int i, const int j=0, const int k=0) const ;

  int getCellNumber(const int i, const int j=0, const int k=0) const ;

  int getEdgeNumber(const int Axis, const int i, const int j=0, const int k=0) const ;

  int getFaceNumber(const int Axis, const int i, const int j=0, const int k=0) const ;

  med_grid_type getGridType();

  int getArrayLength( const int Axis );

  const double getArrayValue (const int Axis, const int i);

  void setGridType(med_grid_type gridType);

  %extend {
    GRID(driverTypes driverType, const char * fileName="", const char * meshName="")
      {
	return new GRID(driverType, string(fileName), string(meshName));
      }

    PyObject * getEntityPosition(const medEntityMesh Entity, const int Number)
    {
      int ijk[3], Axis;
      int i=0, size = self->getSpaceDimension();
      int j=0;
      switch (Entity) {
      case MED_CELL:
        self->getCellPosition(Number, ijk[0], ijk[1], ijk[2]);
        break;
      case MED_NODE:
        self->getNodePosition(Number, ijk[0], ijk[1], ijk[2]);
        break;
      case MED_FACE:
        self->getFacePosition(Number, Axis, ijk[0], ijk[1], ijk[2]);
        size++;
        i++;
        break;
      case MED_EDGE:
        self->getEdgePosition(Number, Axis, ijk[0], ijk[1], ijk[2]);
        size++;
        i++;
        break;
      default:
        char * message = "Error in GRID::getEntityPosition: wrong Entity";
        PyErr_SetString(PyExc_RuntimeError, message);
        return NULL;
      }
      PyObject *py_list = PyList_New( size );
      for (; i < size; ++i)
      {
        int err = PyList_SetItem(py_list, i,
                                 Py_BuildValue("i", ijk[j++]));
        if(err)
        {
          char * message = "Error in GRID::getNodePosition";
          PyErr_SetString(PyExc_RuntimeError, message);
          return NULL;
        }
      }
      if (Entity == MED_FACE || Entity == MED_EDGE)
        PyList_SetItem(py_list, 0, Py_BuildValue("i", Axis));

      PyObject * result = Py_BuildValue("O", py_list);
      Py_DECREF(py_list);
      return result;
    }
  }
};

class MED
{
 public:
  MED();

  ~MED();

  void rmDriver (int index=0);

  int getNumberOfMeshes ( void ) const;

  int getNumberOfFields ( void ) const;

  void updateSupport () ;

  void write (int index=0);

  void addField ( FIELD_  * const ptrField  );

  void addMesh  ( MESH    * const ptrMesh   );

  %extend {
    /*
      WARNING:
      other constructor of MED object.
      Only one constructor could be wrapped and
      the others commented out when using
      SWIG with a version lesser than 1.3
    */

    MED(driverTypes driverType, char * fileName)
      {
	return new MED(driverType,string(fileName));
      }

    int addDriver(driverTypes driverType,
		  char * fileName="Default File Name.med",
		  med_mode_acces access=MED_REMP)
      {
	return self->addDriver(driverType,string(fileName),access);
      }

    %newobject getMeshName(int );
    const char * getMeshName(int i)
      {
	deque<string> list_string = self->getMeshNames();
	char * tmp = new char[strlen(list_string[i].c_str())+1];
	strcpy(tmp,list_string[i].c_str());
	return tmp;
      }

    %newobject getFieldName(int );
    const char * getFieldName(int i)
      {
	deque<string> list_string = self->getFieldNames();
	char * tmp = new char[strlen(list_string[i].c_str())+1];
	strcpy(tmp,list_string[i].c_str());
	return tmp;
      }

    MESH * getMesh(char * meshName)
      {
	MESH * myMesh = self->getMesh(string(meshName));
	return myMesh;
      }

    int getFieldNumberOfIteration(char * fieldName)
      {
	deque<DT_IT_> list_dtit =
	  self->getFieldIteration(string(fieldName));
	return list_dtit.size();
      }

    DT_IT_ getFieldIteration(char * fieldName, int i)
      {
	deque<DT_IT_> list_dtit =
	  self->getFieldIteration(string(fieldName));
	return list_dtit[i];
      }

    FIELD_ * getField(char * fieldName, int dt, int it)
      {
	return self->getField(string(fieldName),dt,it);
      }

    FIELD_ * getField2(char * fieldName,double time, int it=0)
      {
	return self->getField2(string(fieldName),time,it);
      }

    SUPPORT * getSupport(char * meshName, medEntityMesh entity)
      {
	return self->getSupport(string(meshName),entity);
      }
  }
};

/*
  API de MED_MED_[RDONLY,WRONLY,RDWR]_DRIVER
*/

class MED_MED_RDONLY_DRIVER
{
 public :
  void open();
  void close();

  void read           ( void ) ;
  void readFileStruct ( void ) ;

  virtual ~MED_MED_RDONLY_DRIVER();

  %extend {
    MED_MED_RDONLY_DRIVER(char * fileName,  MED * ptrMed)
      {
	return new MED_MED_RDONLY_DRIVER(string(fileName), ptrMed);
      }

    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing MED_MED_RDONLY_DRIVER : " << *self << endl;
	return strdup(mess.str().c_str());
      }
  }
};

class MED_MED_WRONLY_DRIVER
{
 public :
  void open();
  void close();

  void write          ( void ) const ;
  void writeFrom      ( void ) const ;

  virtual ~MED_MED_WRONLY_DRIVER();

  %extend {
    MED_MED_WRONLY_DRIVER(char * fileName,  MED * ptrMed)
      {
	return new MED_MED_WRONLY_DRIVER(string(fileName), ptrMed);
      }

    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing MED_MED_WRONLY_DRIVER : " << *self << endl;
	return strdup(mess.str().c_str());
      }
  }
};

class MED_MED_RDWR_DRIVER : public virtual MED_MED_RDONLY_DRIVER,
			    public virtual MED_MED_WRONLY_DRIVER
{
 public :
  void open();
  void close();

  void write          ( void ) const ;
  void writeFrom      ( void ) const ;
  void read           ( void ) ;
  void readFileStruct ( void ) ;

  ~MED_MED_RDWR_DRIVER();

  %extend {
    MED_MED_RDWR_DRIVER(char * fileName,  MED * ptrMed)
      {
	return new MED_MED_RDWR_DRIVER(string(fileName), ptrMed);
      }

    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing MED_MED_RDWR_DRIVER : " << *self << endl;
	return strdup(mess.str().c_str());
      }
  }
};

/*
  API de GIBI_MESH_[RDONLY,WRONLY,RDWR]_DRIVER
*/

class GIBI_MESH_RDONLY_DRIVER
{
public :
  GIBI_MESH_RDONLY_DRIVER() ;

  GIBI_MESH_RDONLY_DRIVER(const GIBI_MESH_RDONLY_DRIVER & driver) ;

  ~GIBI_MESH_RDONLY_DRIVER() ;

  void open();

  void write( void );

  void read ( void );

  void close();

  %extend {
    GIBI_MESH_RDONLY_DRIVER(char * fileName, MESH * ptrMesh)
      {
	return new GIBI_MESH_RDONLY_DRIVER(string(fileName), ptrMesh) ;
      }

    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing GIBI_MESH_RDONLY_DRIVER : " << *self << endl;
	return strdup(mess.str().c_str());
      }

    void setMeshName(char * meshName)
      {
	self->setMeshName(string(meshName));
      }

    %newobject getMeshName();
    char * getMeshName()
      {
	string tmp_str = self->getMeshName();
	char * tmp = new char[strlen(tmp_str.c_str()) + 1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

class GIBI_MESH_WRONLY_DRIVER
{
public :
  GIBI_MESH_WRONLY_DRIVER() ;

  GIBI_MESH_WRONLY_DRIVER(const GIBI_MESH_WRONLY_DRIVER & driver) ;

  ~GIBI_MESH_WRONLY_DRIVER() ;

  void open();

  void write( void );

  void read ( void );

  void close();

  %extend {
    GIBI_MESH_WRONLY_DRIVER(char * fileName, MESH * ptrMesh)
      {
	return new GIBI_MESH_WRONLY_DRIVER(string(fileName), ptrMesh) ;
      }

    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing GIBI_MESH_WRONLY_DRIVER : " << *self << endl;
	return strdup(mess.str().c_str());
      }

    void setMeshName(char * meshName)
      {
	self->setMeshName(string(meshName));
      }

    %newobject getMeshName();
    char * getMeshName()
      {
	string tmp_str = self->getMeshName();
	char * tmp = new char[strlen(tmp_str.c_str()) + 1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

class GIBI_MESH_RDWR_DRIVER : public virtual GIBI_MESH_RDONLY_DRIVER,
			      public virtual GIBI_MESH_WRONLY_DRIVER
{
public :
  GIBI_MESH_RDWR_DRIVER() ;

  GIBI_MESH_RDWR_DRIVER(const GIBI_MESH_RDWR_DRIVER & driver) ;

  ~GIBI_MESH_RDWR_DRIVER() ;

  void open();

  void write( void );

  void read ( void );

  void close();

  %extend {
    GIBI_MESH_RDWR_DRIVER(char * fileName, MESH * ptrMesh)
      {
	return new GIBI_MESH_RDWR_DRIVER(string(fileName), ptrMesh) ;
      }

    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing GIBI_MESH_RDWR_DRIVER : " << *self << endl;
	return strdup(mess.str().c_str());
      }

    void setMeshName(char * meshName)
      {
	self->setMeshName(string(meshName));
      }

    %newobject getMeshName();
    char * getMeshName()
      {
	string tmp_str = self->getMeshName();
	char * tmp = new char[strlen(tmp_str.c_str()) + 1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

class GIBI_MED_RDONLY_DRIVER
{
public :
  GIBI_MED_RDONLY_DRIVER() ;

  GIBI_MED_RDONLY_DRIVER(const GIBI_MED_RDONLY_DRIVER & driver) ;

  ~GIBI_MED_RDONLY_DRIVER() ;

  void open();

  void write( void );

  void read ( void );

  void close();

  %extend {
    GIBI_MED_RDONLY_DRIVER(char * fileName, MED * ptrMed)
      {
	return new GIBI_MED_RDONLY_DRIVER(string(fileName), ptrMed) ;
      }

    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing GIBI_MED_RDONLY_DRIVER : " << *self << endl;
	return strdup(mess.str().c_str());
      }

  }
};

class GIBI_MED_WRONLY_DRIVER
{
public :
  GIBI_MED_WRONLY_DRIVER() ;

  GIBI_MED_WRONLY_DRIVER(const GIBI_MED_WRONLY_DRIVER & driver) ;

  ~GIBI_MED_WRONLY_DRIVER() ;

  void open();

  void write( void );

  void read ( void );

  void close();

  %extend {
    GIBI_MED_WRONLY_DRIVER(char * fileName, MED * ptrMed, MESH * ptrMesh)
      {
	return new GIBI_MED_WRONLY_DRIVER(string(fileName), ptrMed, ptrMesh) ;
      }

    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing GIBI_MED_WRONLY_DRIVER : " << *self << endl;
	return strdup(mess.str().c_str());
      }

  }
};

/*
  API de PORFLOW_MESH_[RDONLY,WRONLY,RDWR]_DRIVER
*/

class PORFLOW_MESH_RDONLY_DRIVER
{
public :
  PORFLOW_MESH_RDONLY_DRIVER() ;

  PORFLOW_MESH_RDONLY_DRIVER(const PORFLOW_MESH_RDONLY_DRIVER & driver) ;

  ~PORFLOW_MESH_RDONLY_DRIVER() ;

  void open();

  void write( void );

  void read ( void );

  void close();

  %extend {
    PORFLOW_MESH_RDONLY_DRIVER(char * fileName, MESH * ptrMesh)
      {
	return new PORFLOW_MESH_RDONLY_DRIVER(string(fileName), ptrMesh) ;
      }

    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing PORFLOW_MESH_RDONLY_DRIVER : " << *self << endl;
	return strdup(mess.str().c_str());
      }

    void setMeshName(char * meshName)
      {
	self->setMeshName(string(meshName));
      }

    %newobject getMeshName();
    char * getMeshName()
      {
	string tmp_str = self->getMeshName();
	char * tmp = new char[strlen(tmp_str.c_str()) + 1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

class PORFLOW_MESH_WRONLY_DRIVER
{
public :
  PORFLOW_MESH_WRONLY_DRIVER() ;

  PORFLOW_MESH_WRONLY_DRIVER(const PORFLOW_MESH_WRONLY_DRIVER & driver) ;

  ~PORFLOW_MESH_WRONLY_DRIVER() ;

  void open();

  void write( void );

  void read ( void );

  void close();

  %extend {
    PORFLOW_MESH_WRONLY_DRIVER(char * fileName, MESH * ptrMesh)
      {
	return new PORFLOW_MESH_WRONLY_DRIVER(string(fileName), ptrMesh) ;
      }

    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing PORFLOW_MESH_WRONLY_DRIVER : " << *self << endl;
	return strdup(mess.str().c_str());
      }

    void setMeshName(char * meshName)
      {
	self->setMeshName(string(meshName));
      }

    %newobject getMeshName();
    char * getMeshName()
      {
	string tmp_str = self->getMeshName();
	char * tmp = new char[strlen(tmp_str.c_str()) + 1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

class PORFLOW_MESH_RDWR_DRIVER : public virtual PORFLOW_MESH_RDONLY_DRIVER,
				 public virtual PORFLOW_MESH_WRONLY_DRIVER
{
public :
  PORFLOW_MESH_RDWR_DRIVER() ;

  PORFLOW_MESH_RDWR_DRIVER(const PORFLOW_MESH_RDWR_DRIVER & driver) ;

  ~PORFLOW_MESH_RDWR_DRIVER() ;

  void open();

  void write( void );

  void read ( void );

  void close();

  %extend {
    PORFLOW_MESH_RDWR_DRIVER(char * fileName, MESH * ptrMesh)
      {
	return new PORFLOW_MESH_RDWR_DRIVER(string(fileName), ptrMesh) ;
      }

    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing PORFLOW_MESH_RDWR_DRIVER : " << *self << endl;
	return strdup(mess.str().c_str());
      }

    void setMeshName(char * meshName)
      {
	self->setMeshName(string(meshName));
      }

    %newobject getMeshName();
    char * getMeshName()
      {
	string tmp_str = self->getMeshName();
	char * tmp = new char[strlen(tmp_str.c_str()) + 1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

/*
  API de MED_MESH_[RDONLY,WRONLY,RDWR]_DRIVER
*/

class MED_MESH_RDONLY_DRIVER
{
 public :

  ~MED_MESH_RDONLY_DRIVER();

  void open();

  void close();

  void write( void ) ;

  void read ( void ) ;

  %extend {
    MED_MESH_RDONLY_DRIVER(char * fileName,  MESH * ptrMesh)
      {
	return new MED_MESH_RDONLY_DRIVER(string(fileName), ptrMesh);
      }

    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing MED_MESH_RDONLY_DRIVER : " << *self << endl;
	return strdup(mess.str().c_str());
      }

    void setMeshName(char * meshName)
      {
	self->setMeshName(string(meshName));
      }

    %newobject getMeshName();
    char * getMeshName()
      {
	string tmp_str = self->getMeshName();
	char * tmp = new char[strlen(tmp_str.c_str()) + 1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

class MED_MESH_WRONLY_DRIVER
{
 public :
  ~MED_MESH_WRONLY_DRIVER();

  void write( void ) const ;

  void read ( void ) ;

  void open();

  void close();

  %extend {
    MED_MESH_WRONLY_DRIVER(char * fileName,  MESH * ptrMesh)
      {
	return new MED_MESH_WRONLY_DRIVER(string(fileName), ptrMesh);
      }

    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing MED_MESH_WRONLY_DRIVER : " << *self << endl;
	return strdup(mess.str().c_str());
      }

    void setMeshName(char * meshName)
      {
	self->setMeshName(string(meshName));
      }

    %newobject getMeshName();
    char * getMeshName()
      {
	string tmp_str = self->getMeshName();
	char * tmp = new char[strlen(tmp_str.c_str()) + 1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

class MED_MESH_RDWR_DRIVER : public virtual MED_MESH_RDONLY_DRIVER,
			     public virtual MED_MESH_WRONLY_DRIVER
{
 public :

  ~MED_MESH_RDWR_DRIVER();

  void write(void) const ;

  void read (void)       ;

  void open();

  void close();

  %extend {
    MED_MESH_RDWR_DRIVER(char * fileName,  MESH * ptrMesh)
      {
	return new MED_MESH_RDWR_DRIVER(string(fileName), ptrMesh);
      }

    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing MED_MESH_RDWR_DRIVER : " << *self << endl;
	return strdup(mess.str().c_str());
      }

    void setMeshName(char * meshName)
      {
	self->setMeshName(string(meshName));
      }

    %newobject getMeshName();
    char * getMeshName()
      {
	string tmp_str = self->getMeshName();
	char * tmp = new char[strlen(tmp_str.c_str()) + 1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

/*
  API de MED_FIELD_[RDONLY,WRONLY,RDWR]_DRIVER
*/

template< class T1 >
class MED_FIELD_RDONLY_DRIVER
{
public:

  ~MED_FIELD_RDONLY_DRIVER();

  void open();

  void close();

  void write( void ) const ;

  void read ( void ) ;

  %extend {
    MED_FIELD_RDONLY_DRIVER(char * fileName, FIELD< T1 > * ptrField)
      {
	return new MED_FIELD_RDONLY_DRIVER< T1 >(string(fileName), ptrField);
      }

    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing MED_FIELD_RDONLY_DRIVER : " << *self << endl;
	return strdup(mess.str().c_str());
      }

    void setFieldName(char * fieldName)
      {
	self->setFieldName(string(fieldName));
      }

    %newobject getFieldName();
    char * getFieldName()
      {
	string tmp_str = self->getFieldName();
	char * tmp = new char[strlen(tmp_str.c_str()) + 1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};
%template ( MED_FIELDDOUBLE_RDONLY_DRIVER ) MED_FIELD_RDONLY_DRIVER< double >;
%template ( MED_FIELDINT_RDONLY_DRIVER ) MED_FIELD_RDONLY_DRIVER< int >;


template < class T1 >
class MED_FIELD_WRONLY_DRIVER
{
public:

  ~MED_FIELD_WRONLY_DRIVER();

  void open();

  void close();

  void write( void ) const ;

  void read ( void ) ;

  %extend {
    MED_FIELD_WRONLY_DRIVER(char * fileName, FIELD< T1 > * ptrField)
      {
	return new MED_FIELD_WRONLY_DRIVER< T1 >(string(fileName), ptrField);
      }

    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing MED_FIELD_WRONLY_DRIVER : " << *self << endl;
	return strdup(mess.str().c_str());
      }

    void setFieldName(char * fieldName)
      {
	self->setFieldName(string(fieldName));
      }

    %newobject getFieldName();
    char * getFieldName()
      {
	string tmp_str = self->getFieldName();
	char * tmp = new char[strlen(tmp_str.c_str()) + 1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};
%template ( MED_FIELDDOUBLE_WRONLY_DRIVER ) MED_FIELD_WRONLY_DRIVER< double >;
%template ( MED_FIELDINT_WRONLY_DRIVER ) MED_FIELD_WRONLY_DRIVER< int >;


template< class T1 >
class MED_FIELD_RDWR_DRIVER : public virtual MED_FIELD_RDONLY_DRIVER< T1 >, public virtual MED_FIELD_WRONLY_DRIVER< T1 >
{
public:

  ~MED_FIELD_RDWR_DRIVER();

  void open();

  void close();

  void write( void ) const ;

  void read ( void ) ;

  %extend {
    MED_FIELD_RDWR_DRIVER(char * fileName, FIELD< T1 > * ptrField)
      {
	return new MED_FIELD_RDWR_DRIVER< T1 >(string(fileName), ptrField);
      }

    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing MED_FIELD_RDWR_DRIVER : " << *self << endl;
	return strdup(mess.str().c_str());
      }

    void setFieldName(char * fieldName)
      {
	self->setFieldName(string(fieldName));
      }

    %newobject getFieldName();
    char * getFieldName()
      {
	string tmp_str = self->getFieldName();
	char * tmp = new char[strlen(tmp_str.c_str()) + 1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};
%template ( MED_FIELDDOUBLE_RDWR_DRIVER ) MED_FIELD_RDWR_DRIVER< double >;
%template ( MED_FIELDINT_RDWR_DRIVER ) MED_FIELD_RDWR_DRIVER< int >;


template< class T1 >
class ASCII_FIELD_DRIVER {
public:
  ~ASCII_FIELD_DRIVER();

  void open();

  void close();

  void write( void ) const ;


  %extend {
    ASCII_FIELD_DRIVER(const char *fileName, FIELD<T1> * ptrField, med_sort_direc direction, const char *priority)
      {
	return new ASCII_FIELD_DRIVER<T1>(string(fileName), ptrField, (MED_EN::med_sort_direc)direction, priority);
      }
  }
};
%template (ASCII_FIELDDOUBLE_DRIVER) ASCII_FIELD_DRIVER< double >;
%template (ASCII_FIELDINT_DRIVER) ASCII_FIELD_DRIVER< int >;



%{
template <class T> FIELD<T> * createFieldScalarProduct(FIELD<T> * field1, FIELD<T> * field2) {
  return (FIELD<T> *) FIELD<T>::scalarProduct( (FIELD<T>)*field1, (FIELD<T>)*field2);
}
template <class T> FIELD<T> * createFieldScalarProductDeep(FIELD<T> * field1, FIELD<T> * field2) {
  return (FIELD<T>*) FIELD<T>::scalarProduct( (FIELD<T>)*field1, (FIELD<T>)*field2, true);
}
template<class T> FIELD<T> * createFieldFromField(FIELD_ * field) {
  MESSAGE("createFieldFromField : Constructor (for Python API) FIELD<T> with parameter FIELD_");
  MESSAGE("Its returns a proper cast of the input pointer :: FIELD_ --> FIELD<T>");
  return (FIELD<T> *) field;
}
%}

template<class T> FIELD<T> * createFieldFromField(FIELD_ * field);
%template ( createFieldDoubleFromField ) createFieldFromField < double >;
%template ( createFieldIntFromField ) createFieldFromField < int >;

template <class T> FIELD<T> * createFieldScalarProduct(FIELD<T> * field1, FIELD<T> * field2);
%newobject createFieldDoubleScalarProduct(FIELDDOUBLE * field1, FIELDDOUBLE * field2) ;
%newobject createFieldIntScalarProduct(FIELDINT * field1, FIELDINT * field2) ;
%template ( createFieldDoubleScalarProduct ) createFieldScalarProduct < double >;
%template ( createFieldIntScalarProduct ) createFieldScalarProduct < int >;

template <class T> FIELD<T> * createFieldScalarProductDeep(FIELD<T> * field1, FIELD<T> * field2);
%newobject createFieldDoubleScalarProductDeep(FIELDDOUBLE * field1, FIELDDOUBLE * field2) ;
%newobject createFieldIntScalarProductDeep(FIELDINT * field1, FIELDINT * field2) ;
%template ( createFieldDoubleScalarProductDeep ) createFieldScalarProductDeep < double >;
%template ( createFieldIntScalarProductDeep ) createFieldScalarProductDeep < int >;

%newobject createFieldDoubleFromAnalytic(SUPPORT * , int , PyObject *);
%newobject createFieldIntFromAnalytic(SUPPORT * , int , PyObject *);
FIELDDOUBLE * createFieldDoubleFromAnalytic(SUPPORT * Support,
                                            int NumberOfComponents,
                                            PyObject * double_function);
FIELDINT * createFieldIntFromAnalytic(SUPPORT * Support,
                                      int NumberOfComponents,
                                      PyObject * integer_function);


GRID * createGridFromMesh( MESH * aMesh );
%{
  GRID * createGridFromMesh( MESH * aMesh )
    {
      MESSAGE("createGridFromMesh : Constructor (for Python API) GRID with parameter MESH *");
      MESSAGE("Its returns a proper cast of the input pointer :: MESH --> GRID");

      if (aMesh->getIsAGrid())
        return (GRID *) aMesh;

      char * message = "Error in GRID(mesh): mesh is not a grid";
      PyErr_SetString(PyExc_RuntimeError, message);
      return NULL;
    }

  FIELDDOUBLE * createFieldDoubleFromAnalytic(SUPPORT * Support,
					      int NumberOfComponents,
					      PyObject * double_function)
    {
      MESSAGE("createFieldDoubleFromAnalytic : Constructor (for Python API) FIELDDOUBLE from an analytic fonction");
      FIELDDOUBLE * fieldDouble  = new FIELDDOUBLE(Support,NumberOfComponents);
      MyFunction<double>::_pyFunc=double_function;
      MyFunction<double>::_nbOfComponent=NumberOfComponents;
      MyFunction<double>::_spaceDim=Support->getMesh()->getSpaceDimension();
      fieldDouble->fillFromAnalytic< MyFunction<double>::EvalPy2Cpp >();
      return fieldDouble;
    }

  FIELDINT * createFieldIntFromAnalytic(SUPPORT * Support,
					int NumberOfComponents,
					PyObject * integer_function)
    {
      MESSAGE("createFieldIntFromAnalytic : Constructor (for Python API) FIELDINT from an analytic fonction");
      FIELDINT * fieldInt  = new FIELDINT(Support,NumberOfComponents);
      MyFunction<int>::_pyFunc=integer_function;
      MyFunction<int>::_nbOfComponent=NumberOfComponents;
      MyFunction<int>::_spaceDim=Support->getMesh()->getSpaceDimension();
      fieldInt->fillFromAnalytic< MyFunction<int>::EvalPy2Cpp >();
      return fieldInt;
    }
%}
