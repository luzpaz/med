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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
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
#include "MEDMEM_FieldConvert.hxx"
#include "MEDMEM_MedMedDriver.hxx"
#include "MEDMEM_Grid.hxx"
#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_DriverFactory.hxx"
#include "MEDMEM_GaussLocalization.hxx"
#include "MEDMEM_ArrayInterface.hxx"
#include "MEDMEM_SWIG_Templates.hxx"

  using namespace MEDMEM;
  using namespace MED_EN;

  /*  typedef FIELD <double, FullInterlace> FIELDDOUBLEFULLINTERLACE;*/
  /*  typedef FIELD <int, FullInterlace> FIELDINTFULLINTERLACE;*/
  typedef FIELD <double, FullInterlace> FIELDDOUBLE;
  typedef FIELD <int, FullInterlace> FIELDINT;
  typedef FIELD <double, NoInterlace> FIELDDOUBLENOINTERLACE;
  typedef FIELD <int, NoInterlace> FIELDINTNOINTERLACE;

%}

/*
  SWIG needs these typedefs to wrap FIELDDOUBLE*, for ex., to something like
  <C++ FIELD<double> instance at _d0709808_p_FIELDDOUBLE>, not to
  <SWIG Object at _d0709808_p_FIELDDOUBLE> which has no attributes
*/

/*typedef FIELD <double, FullInterlace> FIELDDOUBLEFULLINTERLACE;*/
/*typedef FIELD <int, FullInterlace> FIELDINTFULLINTERLACE;*/
typedef FIELD <double, FullInterlace> FIELDDOUBLE;
typedef FIELD <int, FullInterlace> FIELDINT;
typedef FIELD <double, NoInterlace> FIELDDOUBLENOINTERLACE;
typedef FIELD <int, NoInterlace> FIELDINTNOINTERLACE;

%include "typemaps.i"
%include "my_typemap.i"

%typecheck(SWIG_TYPECHECK_POINTER) double *, const double *,
 const double * const, int *, const int *, const int * const, string *,
 const string *, const string * const, medGeometryElement *,
 const medGeometryElement *, const medGeometryElement * const
{
  $1 = ($input != 0);
}

/*
  mapping between stl string and python string 
*/

%include "std_string.i"


/*
  managing C++ exception in the Python API
*/

%exception
{
  //class PyAllowThreadsGuard {
  // public:
  //  PyAllowThreadsGuard() { _save = PyEval_SaveThread(); }
  //  ~PyAllowThreadsGuard() { PyEval_RestoreThread(_save); }
  // private:
  //  PyThreadState *_save;
  //};

  try
    {
      //PyAllowThreadsGuard guard;
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
  MACRO converting C++ MEDMEM::FIELD_ pointer into a PyObject pointer rightly
  casted following its value type (int or double) and its interlacing mode
  (FullInterlace or NoInterlace) by calling the function SWIG_NewPointer with
  the appropriate $descriptor(...)
*/

%define TYPEMAP_OUTPUT_FIELDT(myField)
{
  FIELD_ *arg1=myField;
  PyObject *myResult=NULL;
  if(arg1)
    {
      FIELD<double> *try1=dynamic_cast<FIELD<double> *>(arg1);
      if(try1)
        myResult = SWIG_NewPointerObj((void *) try1, $descriptor(FIELD<double, FullInterlace> *), 0);
      else
        {
          FIELD<int> *try2=dynamic_cast<FIELD<int> *>(arg1);
          if(try2)
            myResult = SWIG_NewPointerObj((void *) try2, $descriptor(FIELD<int, FullInterlace> *), 0);
          else
            {
              FIELD<double, NoInterlace> *try3=dynamic_cast<FIELD<double, NoInterlace> *>(arg1);
              if(try3)
                myResult = SWIG_NewPointerObj((void *) try3, $descriptor(FIELD<double, NoInterlace> *), 0);
              else
                {
                   FIELD<int, NoInterlace> *try4=dynamic_cast<FIELD<int, NoInterlace> *>(arg1);
                   if(try4)
                     myResult = SWIG_NewPointerObj((void *) try4, $descriptor(FIELD<int, NoInterlace> *), 0);
                   else
                     {
                       myResult = SWIG_NewPointerObj((void *) arg1, $descriptor(FIELD_ *), 0);
                     }
                }
            }
        }
     }
  return myResult;
}
%enddef

/*
  MACRO to transforme a C++ deque<string> object into a proper Python List
*/

%define TYPEMAP_OUTPUT_DEQUE_STRING(myDeque)
{
   PyObject *py_list = PyList_New(myDeque.size());
   deque<string>::iterator iter4;
   int i4=0;
   for(iter4=myDeque.begin();iter4!=myDeque.end();iter4++,i4++)
     {
         PyList_SetItem(py_list, i4, PyString_FromString((*iter4).c_str()));
     }
   return py_list;
}
%enddef

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
  PyObject *PyString_FromStdString(const std::string &str)
    {
      return PyString_FromString(str.c_str());
    }
%}

/**************************************************
  IN typemaps for some std::vector's
**************************************************/

/*  MACRO: IN typemap for std::vector<TYPE> C++ object */
%define TYPEMAP_INPUT_VECTOR_BY_VALUE( TYPE )
{
  /* typemap in for vector<TYPE> */
  /* Check if is a list */
  if (PyList_Check($input))
  {
    int size = PyList_Size($input);
    vector< TYPE > tmpVec(size);

    for (int i=0; i < size; i++)
    {
      PyObject * tmp = PyList_GetItem($input,i);
      TYPE elem;

      int err = SWIG_ConvertPtr(tmp, (void **) &elem, $descriptor(TYPE),
                                SWIG_POINTER_EXCEPTION);
      if (err == -1)
      {
        char * message = "Error in typemap(python,in) for vector<TYPE>"
          "each component should be a TYPE";
        PyErr_SetString(PyExc_RuntimeError, message);
        return NULL;
      }
      tmpVec[i] = elem;
    }
    $1 = tmpVec;
  }
  else
  {
    PyErr_SetString(PyExc_TypeError,"not a list");
    return NULL;
  }
}
%enddef

%typemap(python,in) vector< FAMILY* >, const vector< FAMILY* >
{ TYPEMAP_INPUT_VECTOR_BY_VALUE( FAMILY * ) }

%typemap(python,in) vector< SUPPORT* >, const vector< SUPPORT* >
{ TYPEMAP_INPUT_VECTOR_BY_VALUE( SUPPORT * ) }

%typemap(python,in) vector< FIELDDOUBLE* >, const vector< FIELDDOUBLE* >
{ TYPEMAP_INPUT_VECTOR_BY_VALUE( FIELDDOUBLE * ) }

%typemap(python,in) vector< FIELDINT* >, const vector< FIELDINT* >
{ TYPEMAP_INPUT_VECTOR_BY_VALUE( FIELDINT * ) }

%typemap(python,in) vector< FIELDDOUBLENOINTERLACE* >, const vector< FIELDDOUBLENOINTERLACE* >
{ TYPEMAP_INPUT_VECTOR_BY_VALUE( FIELDDOUBLENOINTERLACE * ) }

%typemap(python,in) vector< FIELDINTNOINTERLACE* >, const vector< FIELDINTNOINTERLACE* >
{ TYPEMAP_INPUT_VECTOR_BY_VALUE( FIELDINTNOINTERLACE * ) }


/**************************************************
  OUT typemaps for some std::vector's
**************************************************/

/*  MACRO: OUT typemap for std::vector<TYPE> C++ object */
%define TYPEMAP_OUTPUT_VECTOR_BY_VALUE( TYPE )
{
  /* typemap out for vector<TYPE> */
  int size = $1.size();
  $result = PyList_New(size);

  for (int i=0;i<size;i++)
  {
    PyObject * tmp = SWIG_NewPointerObj($1.at(i),$descriptor(TYPE),0);

    PyList_SetItem($result,i,tmp);
  }
}
%enddef

%typemap(python,out) vector< FAMILY* >
{ TYPEMAP_OUTPUT_VECTOR_BY_VALUE( FAMILY * ) }

%typemap(python,out) vector< SUPPORT* >
{ TYPEMAP_OUTPUT_VECTOR_BY_VALUE( SUPPORT * ) }

%typemap(python,out) vector< FIELDDOUBLE* >
{ TYPEMAP_OUTPUT_VECTOR_BY_VALUE( FIELDDOUBLE * ) }

%typemap(python,out) vector< FIELDINT* >
{ TYPEMAP_OUTPUT_VECTOR_BY_VALUE( FIELDINT * )  }

%typemap(python,out) vector< FIELDDOUBLENOINTERLACE* >
{ TYPEMAP_OUTPUT_VECTOR_BY_VALUE( FIELDDOUBLENOINTERLACE * ) }

%typemap(python,out) vector< FIELDINTNOINTERLACE* >
{ TYPEMAP_OUTPUT_VECTOR_BY_VALUE( FIELDINTNOINTERLACE * ) }


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

%typecheck(SWIG_TYPECHECK_POINTER) vector< SUPPORT * >, const vector< SUPPORT * >
{
  $1 = ($input != 0);
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

  std::string getName() const;

  ~CELLMODEL();

  %extend {
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

  std::string getMeshName() const;

  void setMesh(MESH * Mesh) const;

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

  SUPPORT(MESH* Mesh, std::string Name="", medEntityMesh Entity=MED_CELL);

  void setpartial(std::string Description, int NumberOfGeometricType,
		  int TotalNumberOfElements, medGeometryElement *GeometricType,
		  int *NumberOfElements, int *NumberValue);

  std::string getName() const;

  void setName(std::string Name);

  std::string getDescription();

  void setDescription(std::string Description);

  %extend {
    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing SUPPORT : " << *self << endl;
	char * tmp = const_cast <char *> (mess.str().c_str());
	char * returned = strdup(tmp);
	return returned;
      }

    PyObject * getTypes()
      {
	const medGeometryElement * types = self->getTypes();
	int size = self->getNumberOfTypes();
        TYPEMAP_OUTPUT_ARRAY(types, size, PyInt_FromLong, SUPPORT::getTypes);
      }

    PyObject * getNumber(medGeometryElement GeometricType)
      {
	const int * number = self->getNumber(GeometricType);
	int size = self->getNumberOfElements(GeometricType);
        TYPEMAP_OUTPUT_ARRAY(number, size, PyInt_FromLong,
			     SUPPORT::getNumber);
      }

    PyObject * getNumberIndex()
      {
	const int * numberindex = self->getNumberIndex();
	int size = (self->getNumberOfElements(MED_ALL_ELEMENTS))+1;
        TYPEMAP_OUTPUT_ARRAY(numberindex, size, PyInt_FromLong,
			     SUPPORT::getNumberIndex);
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

  FAMILY(MESH* Mesh, int Identifier, std::string Name, int NumberOfAttribute,
	 int *AttributeIdentifier, int *AttributeValue,
	 std::string AttributeDescription, int NumberOfGroup,
	 std::string GroupName, int * MEDArrayNodeFamily,
	 int ** MEDArrayCellFamily, int ** MEDArrayFaceFamily,
	 int ** MEDArrayEdgeFamily);

  std::string getAttributeDescription(int i);

  std::string getGroupName(int i);

  %extend {
    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing Family : " << *self << endl;
	return strdup(mess.str().c_str());
      }

    PyObject * getAttributesIdentifiers()
      {
	const int * attributesids = self->getAttributesIdentifiers();
	int size = self->getNumberOfAttributes();
        TYPEMAP_OUTPUT_ARRAY(attributesids,size,PyInt_FromLong,
			     FAMILY::getAttributesIdentifiers);
      }

    PyObject * getAttributesValues()
      {
	const int * attributesvals = self->getAttributesValues();
	int size = self->getNumberOfAttributes();
        TYPEMAP_OUTPUT_ARRAY(attributesvals,size,PyInt_FromLong,
			     FAMILY::getAttributesValues);
      }
  }
};

class FIELD_
{
public:
  FIELD_(const SUPPORT * Support, const int NumberOfComponents);

  ~FIELD_();

  void rmDriver(int index=0);

  void setIterationNumber (int IterationNumber);
  int getIterationNumber() const;

  void setTime(double Time);
  double   getTime() const;

  void setOrderNumber (int OrderNumber);
  int getOrderNumber() const;

  med_type_champ getValueType() ;

  medModeSwitch getInterlacingType();

  SUPPORT * getSupport();
  void setSupport(SUPPORT * support);

  void  setNumberOfComponents(int NumberOfComponents);
  int getNumberOfComponents() const;

  void setNumberOfValues(int NumberOfValues);
  int getNumberOfValues() const;

  std::string   getName() const;

  std::string   getDescription() const;

  std::string   getComponentName(int i) const;

  std::string   getComponentDescription(int i) const;

  std::string   getMEDComponentUnit(int i) const;

  void     setName(std::string Name);

  void     setComponentName(int i, std::string ComponentName);

  void     setMEDComponentUnit(int i, std::string MEDComponentUnit);

  void     setDescription(std::string Description);

  void     setComponentDescription(int i, std::string ComponentDescription);

  int addDriver(driverTypes driverType,
		const std::string& fileName="Default File Name.med",
                const std::string& driverName="Default Field Name",
		med_mode_acces access=MED_REMP);

  %extend {
    %newobject getSupportAndOwner();
    SUPPORT * getSupportAndOwner()
      {
	return (SUPPORT *)self->getSupport();
      }
  }
};

/*
  Class FIELD has now two template parameters T1 is a double or an int
  INTERLACING_TAG is FullInterlace or NoInterlace
*/

template<class T1, class INTERLACING_TAG> class FIELD : public FIELD_
{
public:
  ~FIELD();

  FIELD(const SUPPORT * Support, const int NumberOfComponents);

  FIELD();

  FIELD(const FIELD & m);

  FIELD(const SUPPORT * Support, driverTypes driverType,
	const std::string& fileName, const std::string& fieldName,
	const int iterationNumber, const int orderNumber);

  FIELD(driverTypes driverType,	const std::string& fileName,
	const std::string& fieldName, const int iterationNumber,
	const int orderNumber);

  void read(int index=0);

  T1 getValueIJ(int i,int j) const;

  void setValue(T1* value);

  void setRow( int i, T1 * value);

  void setColumn( int i, T1 * value);

  void setValueIJ(int i, int j, T1 value);

  void allocValue(const int NumberOfComponents);

  void deallocValue();

  void applyLin(T1 a, T1 n);

  void applyPow(T1 scalar);

  double normMax();

  double norm2();

  double normL2(int component,const FIELD<double, FullInterlace> *
		p_field_volume=NULL) const;

  double normL2(const FIELD<double, FullInterlace> *
		p_field_volume=NULL) const;

  double normL1(int component, const FIELD<double, FullInterlace> *
		p_field_volume=NULL) const;

  double normL1(const FIELD<double, FullInterlace> *
		p_field_volume=NULL) const;

  void write(int index=0, const std::string& driverName="");

  void writeAppend(int index=0, const std::string& driverName="");

  bool getGaussPresence();

  GAUSS_LOCALIZATION<INTERLACING_TAG> * getGaussLocalizationPtr(medGeometryElement geomElement);

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

    %newobject __add__(const FIELD<T1, INTERLACING_TAG> & );
    FIELD<T1, INTERLACING_TAG> * __add__(const FIELD<T1, INTERLACING_TAG> & m)
      {
	MESSAGE("operator +  : Creation of the addition of two FIELDs");
	
	FIELD<T1, INTERLACING_TAG>* result =
	  FIELD<T1, INTERLACING_TAG>::add( *(FIELD<T1, INTERLACING_TAG>*)self ,
					   (FIELD<T1, INTERLACING_TAG>&)m );
	return (FIELD<T1, INTERLACING_TAG>*) result;
      }

    %newobject __sub__(const FIELD<T1, INTERLACING_TAG> & );
    FIELD<T1, INTERLACING_TAG> * __sub__(const FIELD<T1, INTERLACING_TAG> & m)
      {
	MESSAGE("operator -  : Creation of the substraction of two FIELDs");
	FIELD<T1, INTERLACING_TAG>* result =
	  FIELD<T1, INTERLACING_TAG>::sub( *(FIELD<T1, INTERLACING_TAG>*)self ,
					   (FIELD<T1, INTERLACING_TAG>&)m );
	return (FIELD<T1, INTERLACING_TAG>*) result;
      }

    %newobject __mul__(const FIELD<T1, INTERLACING_TAG> & );
    FIELD<T1, INTERLACING_TAG> * __mul__(const FIELD<T1, INTERLACING_TAG> & m)
      {
	MESSAGE("operator *  : Creation of the multiplication of two FIELDs");
	FIELD<T1, INTERLACING_TAG>* result =
	  FIELD<T1, INTERLACING_TAG>::mul( *(FIELD<T1, INTERLACING_TAG>*)self ,
					   (FIELD<T1, INTERLACING_TAG>&)m );
	return (FIELD<T1, INTERLACING_TAG>*) result;
      }

    %newobject __div__(const FIELD<T1, INTERLACING_TAG> & );
    FIELD<T1, INTERLACING_TAG> * __div__(const FIELD<T1, INTERLACING_TAG> & m)
      {
	MESSAGE("operator /  : Creation of the division of two FIELDs");
	FIELD<T1, INTERLACING_TAG>* result =
	  FIELD<T1, INTERLACING_TAG>::div( *(FIELD<T1, INTERLACING_TAG>*)self ,
					   (FIELD<T1, INTERLACING_TAG>&)m );
	return (FIELD<T1, INTERLACING_TAG>*) result;
      }

    %newobject addDeep(const FIELD<T1, INTERLACING_TAG> & );
    FIELD<T1, INTERLACING_TAG> * addDeep(const FIELD<T1, INTERLACING_TAG> & m)
      {
	MESSAGE("operator +  : Creation of the addition of two FIELDINTs");
	FIELD<T1, INTERLACING_TAG>* result =
	  FIELD<T1, INTERLACING_TAG>::addDeep( *(FIELD<T1, INTERLACING_TAG>*)self ,
					       (FIELD<T1, INTERLACING_TAG>&)m );
	return (FIELD<T1, INTERLACING_TAG>*) result;
      }

    %newobject subDeep(const FIELD<T1, INTERLACING_TAG> & );
    FIELD<T1, INTERLACING_TAG> * subDeep(const FIELD<T1, INTERLACING_TAG> & m)
      {
	MESSAGE("operator -  : Creation of the substraction of two FIELDs");
	FIELD<T1, INTERLACING_TAG>* result =
	  FIELD<T1, INTERLACING_TAG>::subDeep( *(FIELD<T1, INTERLACING_TAG>*)self ,
					       (FIELD<T1, INTERLACING_TAG>&)m );
	return (FIELD<T1, INTERLACING_TAG>*) result;
      }

    %newobject mulDeep(const FIELD<T1, INTERLACING_TAG> & );
    FIELD<T1, INTERLACING_TAG> * mulDeep(const FIELD<T1, INTERLACING_TAG> & m)
      {
	MESSAGE("operator *  : Creation of the multiplication of two FIELDs");
	FIELD<T1, INTERLACING_TAG>* result =
	  FIELD<T1, INTERLACING_TAG>::mulDeep( *(FIELD<T1, INTERLACING_TAG>*)self ,
					       (FIELD<T1, INTERLACING_TAG>&)m );
	return (FIELD<T1, INTERLACING_TAG>*) result;
      }

    %newobject divDeep(const FIELD<T1, INTERLACING_TAG> & );
    FIELD<T1, INTERLACING_TAG> * divDeep(const FIELD<T1, INTERLACING_TAG> & m)
      {
	MESSAGE("operator /  : Creation of the division of two FIELDs");
	FIELD<T1, INTERLACING_TAG>* result =
	  FIELD<T1, INTERLACING_TAG>::divDeep( *(FIELD<T1, INTERLACING_TAG>*)self ,
					       (FIELD<T1, INTERLACING_TAG>&)m );
	return (FIELD<T1, INTERLACING_TAG>*) result;
      }

    PyObject * getValue()
      {
	int size = self->getValueLength();

	const T1 * value = self->getValue();

        TYPEMAP_OUTPUT_ARRAY(value, size, Binding< T1 >::Traducer,
			     FIELD::getValue);
      }

    // this method replaces getValueI() in FullInterlace mode
    /* %newobject getRow(int );*/
    PyObject * getRow(int index)
      {
	int size = self->getNumberOfComponents() * self->getNbGaussI( index );

	const T1 * value = self->getRow(index);

        TYPEMAP_OUTPUT_ARRAY(value, size, Binding< T1 >::Traducer,
			     FIELD::getRow);
      }

    // this method replaces getValueI() in NoInterlace mode
    /*%newobject getColum(int );*/
    PyObject * getColumn(int index)
      {
	int size = (self->getSupport())->getNumberOfElements(MED_ALL_ELEMENTS);

	const T1 * value = self->getColumn(index);

        TYPEMAP_OUTPUT_ARRAY(value, size, Binding< T1 >::Traducer,
			     FIELD::getColumn);
      }

    /*
    %newobject getValueI(int );
    PyObject * getValueI(int index)
      {
	int size = self->getNumberOfComponents();

	medModeSwitch  Mode = self->getInterlacingType();

	if ( Mode == MED_NO_INTERLACE ) size = (self->getSupport())->getNumberOfElements(MED_ALL_ELEMENTS);

	const T1 * value = self->getValueI(index);

        TYPEMAP_OUTPUT_ARRAY(value, size, Binding< T1 >::Traducer,
			     FIELD::getValueI);
      }
    */

    void allocValue2(int NumberOfComponents, int LengthValue)
      {
	self->allocValue(NumberOfComponents, LengthValue);
      }

    %newobject extract(const SUPPORT *subSupport);
    FIELD<T1, INTERLACING_TAG> *extract(const SUPPORT *subSupport)
      {
	FIELD<T1, INTERLACING_TAG>* result=self->extract(subSupport);
	return (FIELD<T1, INTERLACING_TAG> *)result;
      }
  }
};

/*%template(FIELDDOUBLEFULLINTERLACE) FIELD<double, FullInterlace>;*/
%template(FIELDDOUBLE) FIELD<double, FullInterlace>;
%template(FIELDDOUBLENOINTERLACE) FIELD<double, NoInterlace>;
/*%template(FIELDINTFULLINTERLACE) FIELD<int, FullInterlace>;*/
%template(FIELDINT) FIELD<int, FullInterlace>;
%template(FIELDINTNOINTERLACE) FIELD<int, NoInterlace>;

class GROUP : public SUPPORT
{
public:
  GROUP();

  ~GROUP();

  void setNumberOfFamilies(int numberOfFamilies);
  void setFamilies(vector< FAMILY * > Family);

  int getNumberOfFamilies() const ;
  vector< FAMILY * > getFamilies() const ;
  FAMILY * getFamily(int i) const ;
};

class MESH
{
public :
  MESH();
  MESH(MESH &m);

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

  int getNumberOfPolygons(medEntityMesh Entity=MED_ALL_ENTITIES);

  int getNumberOfPolyhedronFaces();

  int getNumberOfPolyhedron();

  int getNumberOfElementsWithPoly(medEntityMesh Entity,
                                  medGeometryElement Type);

  bool existPolygonsConnectivity(medConnectivity ConnectivityType,
                                 medEntityMesh Entity);

  bool existPolyhedronConnectivity(medConnectivity ConnectivityType,
                                   medEntityMesh Entity);

  medGeometryElement getElementTypeWithPoly(medEntityMesh Entity,int Number);

  std::string getName() const;
  
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

    %newobject mergeSupports(const vector< SUPPORT * > );
    SUPPORT * mergeSupports(const vector< SUPPORT * > Supports)
      {
	return self->mergeSupports(Supports);
      }

    %newobject intersectSupports(const vector< SUPPORT * > );
    SUPPORT * intersectSupports(const vector< SUPPORT * > Supports)
      {
	return self->intersectSupports(Supports);
      }

    %newobject mergeFieldsDouble(const vector< FIELDDOUBLE* > others);
    FIELD<double, FullInterlace> * mergeFieldsDouble(const vector< FIELDDOUBLE* > others)
      {
	return (FIELD<double, FullInterlace> *)self->mergeFields<double>(others);
      }

    %newobject mergeFieldsInt(const vector< FIELDINT* > others);
    FIELD<int, FullInterlace> * mergeFieldsInt(const vector< FIELDINT* > others)
      {
	return (FIELD<int, FullInterlace> *)self->mergeFields<int>(others);
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
        TYPEMAP_OUTPUT_ARRAY(array, size, PyString_FromStdString,
			     MESH::getCoordinatesNames);
      }

    PyObject * getCoordinatesUnits()
      {
	const string * array = self->getCoordinatesUnits();
	int size = self->getSpaceDimension();
        TYPEMAP_OUTPUT_ARRAY(array, size, PyString_FromStdString,
			     MESH::getCoordinatesUnits);
      }

    PyObject * getCoordinates(medModeSwitch Mode)
      {
	const double * array = self->getCoordinates(Mode);
	int size = (self->getSpaceDimension())*(self->getNumberOfNodes());
        TYPEMAP_OUTPUT_ARRAY(array, size, PyFloat_FromDouble,
			     MESH::getCoordinates);
      }

    PyObject * getTypes(medEntityMesh Entity)
      {
	const medGeometryElement * types = self->getTypes(Entity);
	int size = self->getNumberOfTypes(Entity);
        TYPEMAP_OUTPUT_ARRAY(types, size, PyInt_FromLong, MESH::getTypes);
      }

    PyObject * getConnectivity(medModeSwitch Mode,
			       medConnectivity ConnectivityType,
			       medEntityMesh Entity,
			       medGeometryElement Type)
      {
	const int * connectivity = self->getConnectivity(Mode,ConnectivityType,
						   Entity,Type);
	int size = self->getConnectivityLength(Mode,ConnectivityType,Entity,Type);
        TYPEMAP_OUTPUT_ARRAY(connectivity, size, PyInt_FromLong,
			     MESH::getConnectivity );
      }

    PyObject * getConnectivityIndex(medConnectivity ConnectivityType,
				    medEntityMesh Entity)
      {
	const int * connectivity_index =
	  self->getConnectivityIndex(ConnectivityType,Entity);
	int size = (self->getNumberOfElements(Entity,MED_ALL_ELEMENTS))+1;
        TYPEMAP_OUTPUT_ARRAY(connectivity_index,size,PyInt_FromLong,
			     MESH::getConnectivityIndex);
      }

    PyObject * getReverseConnectivity(medConnectivity ConnectivityType,
				      medEntityMesh Entity=MED_CELL)
      {
	const int * reverseconnectivity =
	  self->getReverseConnectivity(ConnectivityType,Entity);
	int size = self->getReverseConnectivityLength(ConnectivityType,Entity);
        TYPEMAP_OUTPUT_ARRAY(reverseconnectivity, size, PyInt_FromLong,
			     MESH::getReverseConnectivity);
      }

    PyObject * getReverseConnectivityIndex(medConnectivity ConnectivityType,
					   medEntityMesh Entity=MED_CELL)
      {
	const int * reverseconnectivity_index =
	  self->getReverseConnectivityIndex(ConnectivityType,Entity);
	int size=self->getReverseConnectivityIndexLength(ConnectivityType,Entity);
        TYPEMAP_OUTPUT_ARRAY(reverseconnectivity_index,size, PyInt_FromLong,
			     MESH::getReverseConnectivityIndex);
      }

    PyObject * getGlobalNumberingIndex(medEntityMesh Entity)
      {
	const int * numberingIndex = self->getGlobalNumberingIndex(Entity);
	int nbOfTypes = self->getNumberOfTypes(Entity);
	int size = nbOfTypes+1;
        TYPEMAP_OUTPUT_ARRAY(numberingIndex, size, PyInt_FromLong,
			     MESH::getGlobalNumberingIndex);
      }

    PyObject * getPolygonsConnectivity(medConnectivity ConnectivityType,
                                       medEntityMesh   Entity)
      {
        const int * array = self->getPolygonsConnectivity(ConnectivityType,Entity);
        int size = self->getPolygonsConnectivityLength(ConnectivityType, Entity);
        TYPEMAP_OUTPUT_ARRAY(array, size, PyInt_FromLong,
			     MESH::getPolygonsConnectivity);
      }

    PyObject * getPolygonsConnectivityIndex(medConnectivity ConnectivityType,
                                            medEntityMesh   Entity)
      {
        const int * array = self->getPolygonsConnectivityIndex(ConnectivityType,Entity);
        int size = self->getNumberOfPolygons(Entity) + 1;
        TYPEMAP_OUTPUT_ARRAY(array, size, PyInt_FromLong,
			     MESH::getPolygonsConnectivity);
      }

    PyObject * getPolyhedronConnectivity(medConnectivity ConnectivityType)
      {
        const int * array = self->getPolyhedronConnectivity(ConnectivityType);
        int size = self->getPolyhedronConnectivityLength(ConnectivityType);
        TYPEMAP_OUTPUT_ARRAY(array, size, PyInt_FromLong,
			     MESH::getPolygonsConnectivity);
      }

    PyObject * getPolyhedronIndex(medConnectivity ConnectivityType)
      {
        const int * array = self->getPolyhedronIndex(ConnectivityType);
        int size = self->getNumberOfPolyhedron() + 1;
        TYPEMAP_OUTPUT_ARRAY(array, size, PyInt_FromLong,
			     MESH::getPolyhedronIndex);
      }

    PyObject * getPolyhedronFacesIndex()
      {
        const int * array = self->getPolyhedronFacesIndex();
        int size = self->getNumberOfPolyhedronFaces() + 1;
        TYPEMAP_OUTPUT_ARRAY(array, size, PyInt_FromLong,
			     MESH::getPolyhedronFacesIndex);
      }

    PyObject * getTypesWithPoly(medEntityMesh Entity)
      {
        medGeometryElement * array = self->getTypesWithPoly(Entity);
        int size = self->getNumberOfTypesWithPoly(Entity);
        TYPEMAP_OUTPUT_ARRAY(array, size, PyInt_FromLong,
			     MESH::getTypesWithPoly);
        delete [] array;
      }
      

    %newobject getVolume(const SUPPORT * );
    FIELD<double, FullInterlace> * getVolume(const SUPPORT * Support)
      {
	return (FIELD<double, FullInterlace> *) self->getVolume(Support);
      }

    %newobject getArea(const SUPPORT * );
    FIELD<double, FullInterlace> * getArea(const SUPPORT * Support)
      {
	return (FIELD<double, FullInterlace> *) self->getArea(Support);
      }

    %newobject getLength(const SUPPORT * );
    FIELD<double, FullInterlace> * getLength(const SUPPORT * Support)
      {
	return (FIELD<double, FullInterlace> *) self->getLength(Support);
      }

    %newobject getNormal(const SUPPORT * );
    FIELD<double, FullInterlace> * getNormal(const SUPPORT * Support)
      {
	return (FIELD<double, FullInterlace> *) self->getNormal(Support);
      }

    %newobject getBarycenter(const SUPPORT * );
    FIELD<double, FullInterlace> * getBarycenter(const SUPPORT * Support)
      {
	return (FIELD<double, FullInterlace> *) self->getBarycenter(Support);
      }
  }
} ;

class MESHING: public MESH
{
public :
  MESHING();
  ~MESHING();

  void setSpaceDimension   (const int SpaceDimension) ;

  void setMeshDimension    (const int MeshDimension) ;

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
				     const medEntityMesh Entity);

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

  void read (int index=0);

  void readFileStruct (int index=0);

  void addField ( FIELD_  * const ptrField  );

  void addMesh  ( MESH    * const ptrMesh   );

  %extend {
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

    PyObject *getMeshNames()
      {
        deque<string> list_string = self->getMeshNames();
	TYPEMAP_OUTPUT_DEQUE_STRING(list_string);
      }

    %newobject getMeshName(int );
    const char * getMeshName(int i)
      {
	deque<string> list_string = self->getMeshNames();
	char * tmp = new char[strlen(list_string[i].c_str())+1];
	strcpy(tmp,list_string[i].c_str());
	return tmp;
      }

    PyObject *getFieldNames()
      {
        deque<string> list_string = self->getFieldNames();
        TYPEMAP_OUTPUT_DEQUE_STRING(list_string);
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

    PyObject *getFieldT(char * fieldName, int dt, int it)
      {
        FIELD_ *ret=self->getField(string(fieldName),dt,it);
	TYPEMAP_OUTPUT_FIELDT(ret);
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
    MED_FIELD_RDONLY_DRIVER(char * fileName, FIELD<T1, FullInterlace > * ptrField)
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
    MED_FIELD_WRONLY_DRIVER(char * fileName, FIELD<T1, FullInterlace> * ptrField)
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
    MED_FIELD_RDWR_DRIVER(char * fileName, FIELD<T1, FullInterlace> * ptrField)
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
    ASCII_FIELD_DRIVER(const char *fileName, FIELD<T1, FullInterlace> * ptrField, med_sort_direc direction, const char *priority)
      {
	return new ASCII_FIELD_DRIVER<T1>(string(fileName), ptrField, (MED_EN::med_sort_direc)direction, priority);
      }
  }
};

%template (ASCII_FIELDDOUBLE_DRIVER) ASCII_FIELD_DRIVER< double >;
%template (ASCII_FIELDINT_DRIVER) ASCII_FIELD_DRIVER< int >;

/*
*/


template <class INTERLACING_TAG> class GAUSS_LOCALIZATION
{
 public:

  GAUSS_LOCALIZATION();

  ~GAUSS_LOCALIZATION();

  std::string getName() const ;

  medGeometryElement getType() const ;

  int getNbGauss() const ;

  medModeSwitch  getInterlacingType() const;

  %extend {
    GAUSS_LOCALIZATION<INTERLACING_TAG>(char * locName,
					const medGeometryElement
					typeGeo,
					const int  nGauss,
					const double * cooRef,
					const double * cooGauss,
					const double * wg)
      {
	return new GAUSS_LOCALIZATION<INTERLACING_TAG>(string(locName),
						       typeGeo, nGauss, cooRef,
						       cooGauss, wg);
      }

    %newobject __str__();
    const char* __str__()
      {
	ostringstream mess;
	mess << "Python Printing GAUSS_LOCALIZATION : " << *self << endl;
	return strdup(mess.str().c_str());
      }

    PyObject * getRefCoo () const
      {
	typedef  MEDMEM_ArrayInterface<double,INTERLACING_TAG,NoGauss>::Array ArrayNoGauss;

	ArrayNoGauss cooRef = self->getRefCoo();
	int size = cooRef.getArraySize();
	double * cooRefPtr = cooRef.getPtr();
        TYPEMAP_OUTPUT_ARRAY(cooRefPtr, size, PyFloat_FromDouble,
			     GAUSS_LOCALIZATION::getRefCoo);
      }

    PyObject * getGsCoo () const
      {
	typedef  MEDMEM_ArrayInterface<double,INTERLACING_TAG,NoGauss>::Array ArrayNoGauss;
	ArrayNoGauss cooGauss = self->getGsCoo();
	int size = cooGauss.getArraySize();
	double * cooGaussPtr = cooGauss.getPtr();
        TYPEMAP_OUTPUT_ARRAY(cooGaussPtr, size, PyFloat_FromDouble,
			     GAUSS_LOCALIZATION::getGsCoo);
      }

    PyObject * getWeight () const
      {
	vector<double> wg = self->getWeight();
	int size = wg.size();
	double * wgPtr = new double [size];
	for (int index = 0; index<size; index++)
	  wgPtr[index] = wg[index];
        TYPEMAP_OUTPUT_ARRAY(wgPtr, size, PyFloat_FromDouble,
			     GAUSS_LOCALIZATION::getWeight);
	delete wgPtr;
      }
  }
};

%template (GAUSS_LOCALIZATION_FULL) GAUSS_LOCALIZATION<FullInterlace> ;
%template (GAUSS_LOCALIZATION_NO) GAUSS_LOCALIZATION<NoInterlace> ;

%{
  template <class T, class INTERLACING_TAG>
    FIELD<T, INTERLACING_TAG> * createFieldScalarProduct(FIELD<T, INTERLACING_TAG> * field1,
							 FIELD<T, INTERLACING_TAG> * field2)
    {
      return (FIELD<T, INTERLACING_TAG> *)
	FIELD<T, INTERLACING_TAG>::scalarProduct((FIELD<T, INTERLACING_TAG>)*field1,
						 (FIELD<T, INTERLACING_TAG>)*field2);
    }

  template <class T, class INTERLACING_TAG>
    FIELD<T, INTERLACING_TAG> * createFieldScalarProductDeep(FIELD<T, INTERLACING_TAG> * field1,
							     FIELD<T, INTERLACING_TAG> * field2)
    {
      return (FIELD<T, INTERLACING_TAG>*) FIELD<T, INTERLACING_TAG>::scalarProduct((FIELD<T, INTERLACING_TAG>)*field1,
										   (FIELD<T, INTERLACING_TAG>)*field2, true);
    }

  template<class T, class INTERLACING_TAG>
    FIELD<T, INTERLACING_TAG> * createTypedFieldFromField(FIELD_ * field)
    {
      MESSAGE("createTypedFieldFromField : Constructor (for Python API) FIELD<T> with parameter FIELD_");
      MESSAGE("Its returns a proper cast of the input pointer :: FIELD_ --> FIELD<T>");
      return (FIELD<T, INTERLACING_TAG> *) field;
    }

  template <class T> FIELD<T, FullInterlace> *
    createTypedFieldConvertFullInterlace(const FIELD<T, NoInterlace> & field )
    {
      return FieldConvert(field);
    }

  template <class T> FIELD<T, NoInterlace> *
    createTypedFieldConvertNoInterlace(const FIELD<T, FullInterlace> & field )
    {
      return FieldConvert(field);
    }
%}

template<class T, class INTERLACING_TAG> FIELD<T, INTERLACING_TAG> * createTypedFieldFromField(FIELD_ * field);
%template ( createFieldDoubleFromField ) createTypedFieldFromField < double, FullInterlace>;
%template ( createFieldIntFromField ) createTypedFieldFromField < int, FullInterlace >;
%template ( createFieldDoubleNoInterlaceFromField ) createTypedFieldFromField < double, NoInterlace>;
%template ( createFieldIntNoInterlaceFromField ) createTypedFieldFromField < int, NoInterlace >;

template <class T, class INTERLACING_TAG> FIELD<T, INTERLACING_TAG> * createFieldScalarProduct(FIELD<T, INTERLACING_TAG> * field1, FIELD<T, INTERLACING_TAG> * field2);
%newobject createFieldDoubleScalarProduct ;
%newobject createFieldIntScalarProduct ;
%newobject createFieldDoubleNoInterlaceScalarProduct ;
%newobject createFieldIntNoInterlaceScalarProduct ;
%template ( createFieldDoubleScalarProduct ) createFieldScalarProduct < double, FullInterlace >;
%template ( createFieldIntScalarProduct ) createFieldScalarProduct < int, FullInterlace >;
%template ( createFieldDoubleNoInterlaceScalarProduct ) createFieldScalarProduct < double, NoInterlace >;
%template ( createFieldIntNoInterlaceScalarProduct ) createFieldScalarProduct < int, NoInterlace >;

template <class T, class INTERLACING_TAG> FIELD<T, INTERLACING_TAG> * createFieldScalarProductDeep(FIELD<T, INTERLACING_TAG> * field1, FIELD<T, INTERLACING_TAG> * field2);
%newobject createFieldDoubleScalarProductDeep ;
%newobject createFieldIntScalarProductDeep ;
%newobject createFieldDoubleNoInterlaceScalarProductDeep ;
%newobject createFieldIntNoInterlaceScalarProductDeep ;
%template ( createFieldDoubleScalarProductDeep ) createFieldScalarProductDeep < double, FullInterlace >;
%template ( createFieldIntScalarProductDeep ) createFieldScalarProductDeep < int, FullInterlace >;
%template ( createFieldDoubleNoInterlaceScalarProductDeep ) createFieldScalarProductDeep < double, NoInterlace >;
%template ( createFieldIntNoInterlaceScalarProductDeep ) createFieldScalarProductDeep < int, NoInterlace >;

template<class T, class INTERLACING_TAG> FIELD<T, INTERLACING_TAG> * createFieldFromAnalytic(SUPPORT * Support, int NumberOfComponents, PyObject * double_function);

template <class T> FIELD<T, FullInterlace> * createTypedFieldConvertFullInterlace(const FIELD<T, NoInterlace> & field );

%template (createFieldDoubleConvertFullInterlace) createTypedFieldConvertFullInterlace<double>;

%template (createFieldIntConvertFullInterlace) createTypedFieldConvertFullInterlace<int>;

template <class T> FIELD<T, NoInterlace> * createTypedFieldConvertNoInterlace(const FIELD<T, FullInterlace> & field );

%template (createFieldDoubleConvertNoInterlace) createTypedFieldConvertNoInterlace<double>;

%template (createFieldIntConvertNoInterlace) createTypedFieldConvertNoInterlace<int>;

%newobject createFieldDoubleConvertFullInterlace ;
%newobject createFieldIntConvertFullInterlace ;

%newobject createFieldDoubleConvertNoInterlace ;
%newobject createFieldIntConvertNoInterlace ;

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

  template<class T, class INTERLACING_TAG>
    FIELD<T, INTERLACING_TAG> * createFieldFromAnalytic(SUPPORT * Support,
							int NumberOfComponents,
							PyObject * double_function)
    {
      MESSAGE("createFieldFromAnalytic : Constructor (for Python API) FIELD from an analytic fonction");

      FIELD<T, INTERLACING_TAG> * fieldAnalytic =
	new FIELD<T, INTERLACING_TAG>(Support, NumberOfComponents);

      MyFunction<T>::_pyFunc=double_function;
      MyFunction<T>::_nbOfComponent=NumberOfComponents;
      MyFunction<T>::_spaceDim=Support->getMesh()->getSpaceDimension();
      fieldAnalytic->fillFromAnalytic(MyFunction<T>::EvalPy2Cpp);
      return fieldAnalytic;
    }
%}

%template (createFieldDoubleFromAnalytic) createFieldFromAnalytic<double, FullInterlace>;
%template (createFieldIntFromAnalytic) createFieldFromAnalytic<int, FullInterlace>;
%template (createFieldDoubleNoInterlaceFromAnalytic) createFieldFromAnalytic<double, NoInterlace>;
%template (createFieldIntNoInterlaceFromAnalytic) createFieldFromAnalytic<int, NoInterlace>;
