//  MED MEDMEM_SWIG : binding of C++ implementation and Python
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : libMEDMEM_Swig.i
//  Module : MED

%module libMEDMEM_Swig

%{
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
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
#include "MEDMEM_SWIG_FieldDouble.hxx"
#include "MEDMEM_SWIG_FieldInt.hxx"
#include "MEDMEM_SWIG_MedFieldDoubleDriver.hxx"
#include "MEDMEM_SWIG_MedFieldIntDriver.hxx"
#include "MEDMEM_Meshing.hxx"
%}

%include "typemaps.i"
%include "my_typemap.i"

/*
  managing C++ exception in the Python API
*/

%exception
{
  try
    {
      $action   ;
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
  enum of the C++ MED used in the Python API
*/

typedef enum {MED_CARTESIAN, MED_POLAR, MED_BODY_FITTED} med_grid_type; 

typedef enum {MED_FULL_INTERLACE, MED_NO_INTERLACE} medModeSwitch; 

typedef enum {MED_LECT, MED_ECRI, MED_REMP} med_mode_acces;

typedef enum {MED_CELL, MED_FACE, MED_EDGE, MED_NODE,
	      MED_ALL_ENTITIES} medEntityMesh; 

typedef enum {MED_NONE=0, MED_POINT1=1, MED_SEG2=102, MED_SEG3=103,
	      MED_TRIA3=203, MED_QUAD4=204, MED_TRIA6=206, MED_QUAD8=208,
	      MED_TETRA4=304, MED_PYRA5=305, MED_PENTA6=306,
	      MED_HEXA8=308, MED_TETRA10=310, MED_PYRA13=313,
	      MED_PENTA15=315, MED_HEXA20=320,
	      MED_ALL_ELEMENTS=999} medGeometryElement;

typedef enum {MED_NODAL, MED_DESCENDING} medConnectivity ; 

typedef enum {MED_DRIVER=0, VTK_DRIVER=1, NO_DRIVER=255} driverTypes;

typedef enum {MED_REEL64=6, MED_INT32=24, MED_INT64=26,
	      MED_INT} med_type_champ;

typedef struct { int dt; int it; } DT_IT_;

%addmethods DT_IT_ {
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



/*   int* getNodesConstituent(int dim,int num) const; */









/*   medGeometryElement*  getConstituentsType(int dim) const; */











/*   set<medGeometryElement>  getAllConstituentsType() const; */


/*   map<medGeometryElement,int>  getNumberOfConstituentsForeachType() const; */




































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
	ostrstream line;
	line << "Python Printing CELLMODEL : " << *self << endl;
	char * tmp = new char[strlen(line.str())+1];
	strcpy(tmp,line.str());
	return tmp;
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

  %addmethods {
    SUPPORT(MESH* Mesh, char * Name="", medEntityMesh Entity=MED_CELL)
      {
	return new SUPPORT(Mesh,string(Name),Entity);
      }

    %newobject __str__();
    const char* __str__()
      {
	ostrstream line;
	line << "Python Printing SUPPORT : " << *self << endl;
	char * tmp = new char[strlen(line.str())+1];
	strcpy(tmp,line.str());
	return tmp;
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
	PyObject *py_list;
       
	const medGeometryElement * types = self->getTypes();
	int size = self->getNumberOfTypes();
	py_list = PyList_New(size);
	for (int i=0; i < size; i++)
	  {
	    int err = PyList_SetItem(py_list, i,
				     Py_BuildValue("i", (int) types[i]));
	    if(err)
	      {
		char * message = "Error in SUPPORT::getTypes";
		PyErr_SetString(PyExc_RuntimeError, message);
		return NULL;
	      }
	  }
	PyObject * result = Py_BuildValue("O", py_list);
	return result;
      }
    */

    PyObject * getNumber(medGeometryElement GeometricType)
      {
	PyObject *py_list;

	const int * number = self->getNumber(GeometricType);
	int size = self->getNumberOfElements(GeometricType);

	py_list = PyList_New(size);
	for (int i=0; i < size; i++)
	  {
	    int err = PyList_SetItem(py_list, i,
				     Py_BuildValue("i", number[i]));
	    if(err)
	      {
		char * message = "Error in SUPPORT::getNumber";
		PyErr_SetString(PyExc_RuntimeError, message);
		return NULL;
	      }
	  }
	PyObject * result = Py_BuildValue("O", py_list);
	return result;
      }

    PyObject * getNumberIndex()
      {
	PyObject *py_list;

	const int * numberindex = self->getNumberIndex();
	int size = (self->getNumberOfElements(MED_ALL_ELEMENTS))+1;

	py_list = PyList_New(size);
	for (int i=0; i < size; i++)
	  {
	    int err = PyList_SetItem(py_list, i,
				     Py_BuildValue("i", numberindex[i]));
	    if(err)
	      {
		char * message = "Error in SUPPORT::getNumberIndex";
		PyErr_SetString(PyExc_RuntimeError, message);
		return NULL;
	      }
	  }
	PyObject * result = Py_BuildValue("O", py_list);
	return result;
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

  %addmethods {
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
	ostrstream line;
	line << "Python Printing Family : " << *self << endl;
	char * tmp = new char[strlen(line.str())+1];
	strcpy(tmp,line.str());
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
	PyObject *py_list;

	const int * attributesids = self->getAttributesIdentifiers();
	int size = self->getNumberOfAttributes();

	py_list = PyList_New(size);
	for (int i=0; i < size; i++)
	  {
	    int err = PyList_SetItem(py_list, i,
				     Py_BuildValue("i",attributesids[i]));
	    if(err)
	      {
		char * message = "Error in FAMILY::getAttributesIdentifiers";
		PyErr_SetString(PyExc_RuntimeError, message);
		return NULL;
	      }
	  }
	PyObject * result = Py_BuildValue("O", py_list);
	return result;
      }

    PyObject * getAttributesValues()
      {
	PyObject *py_list;

	const int * attributesvals = self->getAttributesValues();
	int size = self->getNumberOfAttributes();

	py_list = PyList_New(size);
	for (int i=0; i < size; i++)
	  {
	    int err = PyList_SetItem(py_list, i,
				     Py_BuildValue("i",attributesvals[i]));
	    if(err)
	      {
		char * message = "Error in FAMILY::getAttributesValues";
		PyErr_SetString(PyExc_RuntimeError, message);
		return NULL;
	      }
	  }
	PyObject * result = Py_BuildValue("O", py_list);
	return result;
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

  void setValueType(med_type_champ ValueType) ;
  med_type_champ getValueType() ;

  SUPPORT * getSupport();
  void setSupport(SUPPORT * support);

  void  setNumberOfComponents(int NumberOfComponents);
  int getNumberOfComponents() const;

  void setNumberOfValues(int NumberOfValues);
  int getNumberOfValues() const;

  %extend {
    int addDriver(driverTypes driverType,
		  char * fileName="Default File Name.med",
		  char * driverName="Default Field Name")
      {
	return self->addDriver(driverType,string(fileName),
			       string(driverName));
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

    void setDescription(char * Description)
      {
	self->setDescription(string(Description));
      }

    %newobject getDescription();
    const char * getDescription() const
      {
	string tmp_str = self->getDescription();
	char * tmp = new char[strlen(tmp_str.c_str())+1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }

    %newobject getComponentName(int );
    const char * getComponentName(int i)
      {
	string tmp_str = self->getComponentName(i);
	char * tmp = new char[strlen(tmp_str.c_str())+1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;	
      }

    void setComponentName(int i, char * ComponentName)
      {
	self->setComponentName(i,string(ComponentName));
      }

    %newobject getComponentDescription(int );
    const char * getComponentDescription(int i)
      {
	string tmp_str = self->getComponentDescription(i);
	char * tmp = new char[strlen(tmp_str.c_str())+1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;	
      }

    void setComponentDescription(int i, char * ComponentDescription)
      {
	self->setComponentDescription(i,string(ComponentDescription));
      }

    %newobject getMEDComponentUnit(int );
    const char * getMEDComponentUnit(int i)
      {
	string tmp_str = self->getMEDComponentUnit(i);
	char * tmp = new char[strlen(tmp_str.c_str())+1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }

    void setMEDComponentUnit(int i, char * MEDComponentUnit)
      {
	self->setMEDComponentUnit(i,string(MEDComponentUnit));
      }
  }
}; 

class FIELDDOUBLE : public FIELD_
{
public:
  ~FIELDDOUBLE();

  FIELDDOUBLE(const SUPPORT * Support, const int NumberOfComponents);

  /*
    WARNING:
    other constructor of FIELDDOUBLE (C++ FIELD<double>) object.
    Only one constructor could be wrapped and
    the others commented out when using 
    SWIG with a version lesser than 1.3
  */

  FIELDDOUBLE();    

  void read(int index=0);

  double getValueIJ(int i,int j) const;

  void setValue(medModeSwitch mode, double* value);

  void setValueI(medModeSwitch mode, int i, double* value);

  void setValueIJ(int i, int j, double value);

  void allocValue(const int NumberOfComponents);

  void deallocValue();

  %extend {
    FIELDDOUBLE (const SUPPORT * Support, driverTypes driverType,
		 char * fileName, char * fieldName)
      {
	return new FIELDDOUBLE(Support, driverType, string(fileName),
			       string(fieldName));
      }

    void write(int index=0, char * driverName="")
      {
	self->write(index, string(driverName));
      }

    PyObject * getValue(medModeSwitch Mode)
      {
	PyObject *py_list;

	int size = (self->getNumberOfComponents())*
	  ((self->getSupport())->getNumberOfElements(MED_ALL_ELEMENTS));

	const double * value = self->getValue(Mode);

	py_list = PyList_New(size);
	for (int i=0; i < size; i++)
	  {
	    int err = PyList_SetItem(py_list, i,
				     Py_BuildValue("d", value[i]));
	    if(err)
	      {
		char * message = "Error in FIELDDOUBLE::getValue";
		PyErr_SetString(PyExc_RuntimeError, message);
		return NULL;
	      }
	  }
	PyObject * result = Py_BuildValue("O", py_list);
	return result;
      }

    %newobject getValueI(medModeSwitch , int );
    PyObject * getValueI(medModeSwitch Mode, int index)
      {
	PyObject *py_list;

	int size = self->getNumberOfComponents();

	if ( Mode == MED_NO_INTERLACE ) size = (self->getSupport())->getNumberOfElements(MED_ALL_ELEMENTS);

	const double * value = self->getValueI(Mode,index);

	py_list = PyList_New(size);
	for (int i=0; i < size; i++)
	  {
	    int err = PyList_SetItem(py_list, i,
				     Py_BuildValue("d", value[i]));
	    if(err)
	      {
		char * message = "Error in FIELDDOUBLE::getValueI";
		PyErr_SetString(PyExc_RuntimeError, message);
		return NULL;
	      }
	  }
	PyObject * result = Py_BuildValue("O", py_list);
	return result;
      }

    void allocValue2(int NumberOfComponents, int LengthValue)
      {
	self->allocValue(NumberOfComponents, LengthValue);
      }
  }
};

class FIELDINT : public FIELD_
{
public:
  ~FIELDINT();

  FIELDINT(const SUPPORT * Support, const int NumberOfComponents);

  /*
    WARNING:
    other constructor of FIELDINT (C++ FIELD<int>) object.
    other constructor of MED object.
    Only one constructor could be wrapped and
    the others commented out when using 
    SWIG with a version lesser than 1.3
  */

  FIELDINT();

  void read(int index=0);

  int getValueIJ(int i,int j) const;

  void setValue(medModeSwitch mode, int* value);

  void setValueI(medModeSwitch mode, int i, int* value);

  void setValueIJ(int i, int j, int value);

  void allocValue(const int NumberOfComponents);

  void deallocValue();

  %extend {
    FIELDINT(const SUPPORT * Support, driverTypes driverType,
             char * fileName, char * fieldName)
      {
	return new FIELDINT(Support, driverType, string(fileName),
			    string(fieldName));
      }

    void write(int index=0, char * driverName="")
      {
	self->write(index, string(driverName));
      }

    PyObject * getValue(medModeSwitch Mode)
      {
	PyObject *py_list;

	int size = (self->getNumberOfComponents())*
	  ((self->getSupport())->getNumberOfElements(MED_ALL_ELEMENTS));

	const int * value = self->getValue(Mode);

	py_list = PyList_New(size);
	for (int i=0; i < size; i++)
	  {
	    int err = PyList_SetItem(py_list, i,
				     Py_BuildValue("i", value[i]));
	    if(err)
	      {
		char * message = "Error in FIELDINT::getValue";
		PyErr_SetString(PyExc_RuntimeError, message);
		return NULL;
	      }
	  }
	PyObject * result = Py_BuildValue("O", py_list);
	return result;
      }

    PyObject * getValueI(medModeSwitch Mode, int index)
      {
	PyObject *py_list;

	int size = self->getNumberOfComponents();

	if ( Mode == MED_NO_INTERLACE ) size = (self->getSupport())->getNumberOfElements(MED_ALL_ELEMENTS);

	const int * value = self->getValueI(Mode,index);

	py_list = PyList_New(size);
	for (int i=0; i < size; i++)
	  {
	    int err = PyList_SetItem(py_list, i,
				     Py_BuildValue("i", value[i]));
	    if(err)
	      {
		char * message = "Error in FIELDINT::getValueI";
		PyErr_SetString(PyExc_RuntimeError, message);
		return NULL;
	      }
	  }
	PyObject * result = Py_BuildValue("O", py_list);
	return result;
      }

    void allocValue2(int NumberOfComponents, int LengthValue)
      {
	self->allocValue(NumberOfComponents, LengthValue);
      }
  }
};

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
		  char * driverName="Default Mesh Name")
      {
	return self->addDriver(driverType,string(fileName),
			       string(driverName));
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
	PyObject *py_list;
	const string * array = self->getCoordinatesNames();
	int size = self->getSpaceDimension();
	py_list = PyList_New(size);
	for (int i=0; i < size; i++)
	  {
	    int err = PyList_SetItem(py_list, i,
				     Py_BuildValue("s", array[i].c_str()));
	    if(err)
	      {
		char * message = "Error in MESH::getCoordinatesNames";
		PyErr_SetString(PyExc_RuntimeError, message);
		return NULL;
	      }
	  }
	PyObject * result = Py_BuildValue("O", py_list);
	return result;
      }

    PyObject * getCoordinatesUnits()
      {
	PyObject *py_list;
	const string * array = self->getCoordinatesUnits();
	int size = self->getSpaceDimension();
	py_list = PyList_New(size);
	for (int i=0; i < size; i++)
	  {
	    int err = PyList_SetItem(py_list, i,
				     Py_BuildValue("s", array[i].c_str()));
	    if(err)
	      {
		char * message = "Error in MESH::getCoordinatesUnits";
		PyErr_SetString(PyExc_RuntimeError, message);
		return NULL;
	      }
	  }
	PyObject * result = Py_BuildValue("O", py_list);
	return result;
      }

    PyObject * getCoordinates(medModeSwitch Mode)
      {
	PyObject *py_list;
	const double * array = self->getCoordinates(Mode);
	int size = (self->getSpaceDimension())*(self->getNumberOfNodes());
	py_list = PyList_New(size);
	for (int i=0; i < size; i++)
	  {
	    int err = PyList_SetItem(py_list, i,
				     Py_BuildValue("d", array[i]));
	    if(err)
	      {
		char * message = "Error in MESH::getCoordinates";
		PyErr_SetString(PyExc_RuntimeError, message);
		return NULL;
	      }
	  }
	PyObject * result = Py_BuildValue("O", py_list);
	return result;
      }

    PyObject * getTypes(medEntityMesh Entity)
      {
	PyObject *py_list;
       
	const medGeometryElement * types = self->getTypes(Entity);
	int size = self->getNumberOfTypes(Entity);
	py_list = PyList_New(size);
	for (int i=0; i < size; i++)
	  {
	    int err = PyList_SetItem(py_list, i,
				     Py_BuildValue("i", (int) types[i]));
	    if(err)
	      {
		char * message = "Error in MESH::getTypes";
		PyErr_SetString(PyExc_RuntimeError, message);
		return NULL;
	      }
	  }
	PyObject * result = Py_BuildValue("O", py_list);
	return result;
      }

    PyObject * getConnectivity(medModeSwitch Mode,
			       medConnectivity ConnectivityType,
			       medEntityMesh Entity,
			       medGeometryElement Type)
      {
	PyObject *py_list;
	const int * connectivity = self->getConnectivity(Mode,ConnectivityType,
						   Entity,Type);
	int nbOfElm = self->getNumberOfElements(Entity,Type);
	int size;

	if (Type == MED_ALL_ELEMENTS)
	  {
	    size = self->getConnectivityIndex(ConnectivityType,Entity)[nbOfElm]-1;
	  }
	else
	  {
	    size = nbOfElm*(((int) Type)%100);
	  }

	py_list = PyList_New(size);
	for (int i=0; i < size; i++)
	  {
	    int err = PyList_SetItem(py_list, i,
				     Py_BuildValue("i", connectivity[i]));
	    if(err)
	      {
		char * message = "Error in MESH::getConnectivity";
		PyErr_SetString(PyExc_RuntimeError, message);
		return NULL;
	      }
	  }
	PyObject * result = Py_BuildValue("O", py_list);
	return result;
      }

    PyObject * getConnectivityIndex(medConnectivity ConnectivityType,
				    medEntityMesh Entity)
      {
	PyObject *py_list;
	const int * connectivity_index =
	  self->getConnectivityIndex(ConnectivityType,Entity);
	int size = (self->getNumberOfElements(Entity,MED_ALL_ELEMENTS))+1;

	py_list = PyList_New(size);
	for (int i=0; i < size; i++)
	  {
	    int err = PyList_SetItem(py_list, i,
				     Py_BuildValue("i",
						   connectivity_index[i]));
	    if(err)
	      {
		char * message = "Error in MESH::getConnectivityIndex";
		PyErr_SetString(PyExc_RuntimeError, message);
		return NULL;
	      }
	  }
	PyObject * result = Py_BuildValue("O", py_list);
	return result;
      }

    PyObject * getReverseConnectivity(medConnectivity ConnectivityType)
      {
	PyObject *py_list;
	const int * reverseconnectivity =
	  self->getReverseConnectivity(ConnectivityType);
	int spaceDim = self->getSpaceDimension();
	int nb;

	if (ConnectivityType == MED_NODAL)
	  {
	    nb = (self->getNumberOfNodes());
	  }
	else
	  {
	    if (spaceDim == 2)
	      nb = (self->getNumberOfElements(MED_EDGE,
					      MED_ALL_ELEMENTS));
	    else if (spaceDim == 3)
	      nb = (self->getNumberOfElements(MED_FACE,
					      MED_ALL_ELEMENTS));
	  }

	int size = self->getReverseConnectivityIndex(ConnectivityType)[nb]-1;

	py_list = PyList_New(size);
	for (int i=0; i < size; i++)
	  {
	    int err =
	      PyList_SetItem(py_list,i,
			     Py_BuildValue("i",reverseconnectivity[i]));
	    if(err)
	      {
		char * message = "Error in MESH::getReverseConnectivity";
		PyErr_SetString(PyExc_RuntimeError, message);
		return NULL;
	      }
	  }
	PyObject * result = Py_BuildValue("O", py_list);
	return result;
      }

    PyObject * getReverseConnectivityIndex(medConnectivity
					   ConnectivityType)
      {
	PyObject *py_list;
	const int * reverseconnectivity_index =
	  self->getReverseConnectivityIndex(ConnectivityType);

	int size;
	int spaceDim = self->getSpaceDimension();

	if (ConnectivityType == MED_NODAL)
	  {
	    size = (self->getNumberOfNodes())+1;
	  }
	else
	  {
	    if (spaceDim == 2)
	      size = (self->getNumberOfElements(MED_EDGE,MED_ALL_ELEMENTS))+1;
	    else if (spaceDim == 3)
	      size = (self->getNumberOfElements(MED_FACE,MED_ALL_ELEMENTS))+1;
	  }

	py_list = PyList_New(size);
	for (int i=0; i < size; i++)
	  {
	    int err =
	      PyList_SetItem(py_list,i,
			     Py_BuildValue("i",
					   reverseconnectivity_index[i]));
	    if(err)
	      {
		char * message = "Error in MESH::getReverseConnectivityIndex";
		PyErr_SetString(PyExc_RuntimeError, message);
		return NULL;
	      }
	  }
	PyObject * result = Py_BuildValue("O", py_list);
	return result;
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

  int getNodeNumber(const int i, const int j=0, const int k=0);

  int getCellNumber(const int i, const int j=0, const int k=0) ;

  int getEdgeNumber(const int Axis, const int i, const int j=0, const int k=0);

  int getFaceNumber(const int Axis, const int i, const int j=0, const int k=0);
  
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
		  char * fileName="Default File Name.med")
      {
	return self->addDriver(driverType,string(fileName));
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

    void setMeshName(char * meshName)
      {
	self->setMeshName(string(meshName));
      }

    %newobject getMeshName();
    char * getMeshName()
      {
	string tmp_str = self->getMeshName();
	char * tmp = new char[strlen(tmp_str.c_str())];
	strcat(tmp,tmp_str.c_str());
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

    void setMeshName(char * meshName)
      {
	self->setMeshName(string(meshName));
      }

    %newobject getMeshName();
    char * getMeshName()
      {
	string tmp_str = self->getMeshName();
	char * tmp = new char[strlen(tmp_str.c_str())];
	strcat(tmp,tmp_str.c_str());
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

    void setMeshName(char * meshName)
      {
	self->setMeshName(string(meshName));
      }

    %newobject getMeshName();
    char * getMeshName()
      {
	string tmp_str = self->getMeshName();
	char * tmp = new char[strlen(tmp_str.c_str())];
	strcat(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

/*
  API de MED_FIELDDOUBLE_[RDONLY,WRONLY,RDWR]_DRIVER
*/

class MED_FIELDDOUBLE_RDONLY_DRIVER
{
public:

  ~MED_FIELDDOUBLE_RDONLY_DRIVER();

  void open();

  void close();

  void write( void ) const ;

  void read ( void ) ;

  %extend {
    MED_FIELDDOUBLE_RDONLY_DRIVER(char * fileName, FIELDDOUBLE * ptrField)
      {
	return new MED_FIELDDOUBLE_RDONLY_DRIVER(string(fileName), ptrField);
      }

    void setFieldName(char * fieldName)
      {
	self->setFieldName(string(fieldName));
      }

    %newobject getFieldName();
    char * getFieldName()
      {
	string tmp_str = self->getFieldName();
	char * tmp = new char[strlen(tmp_str.c_str())];
	strcat(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

class MED_FIELDDOUBLE_WRONLY_DRIVER
{
public:

  ~MED_FIELDDOUBLE_WRONLY_DRIVER();

  void open();

  void close();

  void write( void ) const ;

  void read ( void ) ;

  %extend {
    MED_FIELDDOUBLE_WRONLY_DRIVER(char * fileName, FIELDDOUBLE * ptrField)
      {
	return new MED_FIELDDOUBLE_WRONLY_DRIVER(string(fileName), ptrField);
      }

    void setFieldName(char * fieldName)
      {
	self->setFieldName(string(fieldName));
      }

    %newobject getFieldName();
    char * getFieldName()
      {
	string tmp_str = self->getFieldName();
	char * tmp = new char[strlen(tmp_str.c_str())];
	strcat(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

class MED_FIELDDOUBLE_RDWR_DRIVER : public virtual MED_FIELDDOUBLE_RDONLY_DRIVER, public virtual MED_FIELDDOUBLE_WRONLY_DRIVER
{
public:

  ~MED_FIELDDOUBLE_RDWR_DRIVER();

  void open();

  void close();

  void write( void ) const ;

  void read ( void ) ;

  %extend {
    MED_FIELDDOUBLE_RDWR_DRIVER(char * fileName, FIELDDOUBLE * ptrField)
      {
	return new MED_FIELDDOUBLE_RDWR_DRIVER(string(fileName), ptrField);
      }

    void setFieldName(char * fieldName)
      {
	self->setFieldName(string(fieldName));
      }

    %newobject getFieldName();
    char * getFieldName()
      {
	string tmp_str = self->getFieldName();
	char * tmp = new char[strlen(tmp_str.c_str())];
	strcat(tmp,tmp_str.c_str());
	return tmp;
      }

    void setMeshName(char * meshName)
      {
	self->setMeshName(string(meshName));
      }

    char * getMeshName()
      {
	string tmp_str = self->getMeshName();
	char * tmp = new char[strlen(tmp_str.c_str())];
	strcat(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

/*
  API de MED_FIELDINT_[RDONLY,WRONLY,RDWR]_DRIVER
*/

class MED_FIELDINT_RDONLY_DRIVER
{
public:

  ~MED_FIELDINT_RDONLY_DRIVER();

  void open();

  void close();

  void write( void ) const ;

  void read ( void ) ;

  %extend {
    MED_FIELDINT_RDONLY_DRIVER(char * fileName, FIELDINT * ptrField)
      {
	return new MED_FIELDINT_RDONLY_DRIVER(string(fileName), ptrField);
      }

    void setFieldName(char * fieldName)
      {
	self->setFieldName(string(fieldName));
      }

    %newobject getFieldName();
    char * getFieldName()
      {
	string tmp_str = self->getFieldName();
	char * tmp = new char[strlen(tmp_str.c_str())];
	strcat(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

class MED_FIELDINT_WRONLY_DRIVER
{
public:

  ~MED_FIELDINT_WRONLY_DRIVER();

  void open();

  void close();

  void write( void ) const ;

  void read ( void ) ;

  %extend {
    MED_FIELDINT_WRONLY_DRIVER(char * fileName, FIELDINT * ptrField)
      {
	return new MED_FIELDINT_WRONLY_DRIVER(string(fileName), ptrField);
      }

    void setFieldName(char * fieldName)
      {
	self->setFieldName(string(fieldName));
      }

    %newobject getFieldName();
    char * getFieldName()
      {
	string tmp_str = self->getFieldName();
	char * tmp = new char[strlen(tmp_str.c_str())];
	strcat(tmp,tmp_str.c_str());
	return tmp;
      }

    void setMeshName(char * meshName)
      {
	self->setMeshName(string(meshName));
      }

    char * getMeshName()
      {
	string tmp_str = self->getMeshName();
	char * tmp = new char[strlen(tmp_str.c_str())];
	strcat(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

class MED_FIELDINT_RDWR_DRIVER : public virtual MED_FIELDINT_RDONLY_DRIVER, public virtual MED_FIELDINT_WRONLY_DRIVER
{
public:

  ~MED_FIELDINT_RDWR_DRIVER();

  void open();

  void close();

  void write( void ) const ;

  void read ( void ) ;

  %extend {
    MED_FIELDINT_RDWR_DRIVER(char * fileName, FIELDINT * ptrField)
      {
	return new MED_FIELDINT_RDWR_DRIVER(string(fileName), ptrField);
      }

    void setFieldName(char * fieldName)
      {
	self->setFieldName(string(fieldName));
      }

    %newobject getFieldName();
    char * getFieldName()
      {
	string tmp_str = self->getFieldName();
	char * tmp = new char[strlen(tmp_str.c_str())];
	strcat(tmp,tmp_str.c_str());
	return tmp;
      }

    void setMeshName(char * meshName)
      {
	self->setMeshName(string(meshName));
      }

    char * getMeshName()
      {
	string tmp_str = self->getMeshName();
	char * tmp = new char[strlen(tmp_str.c_str())];
	strcat(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

/*
  API du driver MED_FIELDDOUBLE
*/

/*
class MED_FIELDDOUBLE_DRIVER
{
public:
  void open();

  void close();

  %addmethods {
    MED_FIELDDOUBLE_DRIVER(char * fileName, FIELDDOUBLE * ptrField, 
			   med_mode_acces accessMode)
      {
	return new MED_FIELDDOUBLE_DRIVER(string(fileName), ptrField,
					  accessMode);
      }

    void setFieldName(char * fieldName)
      {
	self->setFieldName(string(fieldName));
      }

    char * getFieldName()
      {
	string tmp_str = self->getFieldName();
	char * tmp = new char[strlen(tmp_str.c_str())];
	strcat(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};
*/

class MED_FIELDDOUBLE_RDONLY_DRIVER : public virtual MED_FIELDDOUBLE_DRIVER
{
public:

  ~MED_FIELDDOUBLE_RDONLY_DRIVER();

  void open();

  void close();

  void write( void ) const ;

  void read ( void ) ;

  %addmethods {
    MED_FIELDDOUBLE_RDONLY_DRIVER(char * fileName, FIELDDOUBLE * ptrField)
      {
	return new MED_FIELDDOUBLE_RDONLY_DRIVER(string(fileName), ptrField);
      }

    void setFieldName(char * fieldName)
      {
	self->setFieldName(string(fieldName));
      }

    char * getFieldName()
      {
	string tmp_str = self->getFieldName();
	char * tmp = new char[strlen(tmp_str.c_str())];
	strcat(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

class MED_FIELDDOUBLE_WRONLY_DRIVER : public virtual MED_FIELDDOUBLE_DRIVER
{
public:

  ~MED_FIELDDOUBLE_WRONLY_DRIVER();

  void open();

  void close();

  void write( void ) const ;

  void read ( void ) ;

  %addmethods {
    MED_FIELDDOUBLE_WRONLY_DRIVER(char * fileName, FIELDDOUBLE * ptrField)
      {
	return new MED_FIELDDOUBLE_WRONLY_DRIVER(string(fileName), ptrField);
      }

    void setFieldName(char * fieldName)
      {
	self->setFieldName(string(fieldName));
      }

    char * getFieldName()
      {
	string tmp_str = self->getFieldName();
	char * tmp = new char[strlen(tmp_str.c_str())];
	strcat(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

class MED_FIELDDOUBLE_RDWR_DRIVER : public virtual MED_FIELDDOUBLE_RDONLY_DRIVER, public virtual MED_FIELDDOUBLE_WRONLY_DRIVER
{
public:

  ~MED_FIELDDOUBLE_RDWR_DRIVER();

  void open();

  void close();

  void write( void ) const ;

  void read ( void ) ;

  %addmethods {
    MED_FIELDDOUBLE_RDWR_DRIVER(char * fileName, FIELDDOUBLE * ptrField)
      {
	return new MED_FIELDDOUBLE_RDWR_DRIVER(string(fileName), ptrField);
      }

    void setFieldName(char * fieldName)
      {
	self->setFieldName(string(fieldName));
      }

    char * getFieldName()
      {
	string tmp_str = self->getFieldName();
	char * tmp = new char[strlen(tmp_str.c_str())];
	strcat(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

/*
  API du driver MED_FIELDINT
*/

/*
class MED_FIELDINT_DRIVER
{
public:
  void open();

  void close();

  %addmethods {
    MED_FIELDINT_DRIVER(char * fileName, FIELDINT * ptrField, 
		        med_mode_acces accessMode)
      {
	return new MED_FIELDINT_DRIVER(string(fileName), ptrField,
					  accessMode);
      }

    void setFieldName(char * fieldName)
      {
	self->setFieldName(string(fieldName));
      }

    char * getFieldName()
      {
	string tmp_str = self->getFieldName();
	char * tmp = new char[strlen(tmp_str.c_str())];
	strcat(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};
*/

class MED_FIELDINT_RDONLY_DRIVER : public virtual MED_FIELDINT_DRIVER
{
public:

  ~MED_FIELDINT_RDONLY_DRIVER();

  void open();

  void close();

  void write( void ) const ;

  void read ( void ) ;

  %addmethods {
    MED_FIELDINT_RDONLY_DRIVER(char * fileName, FIELDINT * ptrField)
      {
	return new MED_FIELDINT_RDONLY_DRIVER(string(fileName), ptrField);
      }

    void setFieldName(char * fieldName)
      {
	self->setFieldName(string(fieldName));
      }

    char * getFieldName()
      {
	string tmp_str = self->getFieldName();
	char * tmp = new char[strlen(tmp_str.c_str())];
	strcat(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

class MED_FIELDINT_WRONLY_DRIVER : public virtual MED_FIELDINT_DRIVER
{
public:

  ~MED_FIELDINT_WRONLY_DRIVER();

  void open();

  void close();

  void write( void ) const ;

  void read ( void ) ;

  %addmethods {
    MED_FIELDINT_WRONLY_DRIVER(char * fileName, FIELDINT * ptrField)
      {
	return new MED_FIELDINT_WRONLY_DRIVER(string(fileName), ptrField);
      }

    void setFieldName(char * fieldName)
      {
	self->setFieldName(string(fieldName));
      }

    char * getFieldName()
      {
	string tmp_str = self->getFieldName();
	char * tmp = new char[strlen(tmp_str.c_str())];
	strcat(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

class MED_FIELDINT_RDWR_DRIVER : public virtual MED_FIELDINT_RDONLY_DRIVER, public virtual MED_FIELDINT_WRONLY_DRIVER
{
public:

  ~MED_FIELDINT_RDWR_DRIVER();

  void open();

  void close();

  void write( void ) const ;

  void read ( void ) ;

  %addmethods {
    MED_FIELDINT_RDWR_DRIVER(char * fileName, FIELDINT * ptrField)
      {
	return new MED_FIELDINT_RDWR_DRIVER(string(fileName), ptrField);
      }

    void setFieldName(char * fieldName)
      {
	self->setFieldName(string(fieldName));
      }

    char * getFieldName()
      {
	string tmp_str = self->getFieldName();
	char * tmp = new char[strlen(tmp_str.c_str())];
	strcat(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

FIELDDOUBLE * createFieldDoubleFromField(FIELD_ * field) ;

FIELDINT * createFieldIntFromField(FIELD_ * field) ;

GRID * createGridFromMesh( MESH * aMesh );

%{
  FIELDDOUBLE * createFieldDoubleFromField(FIELD_ * field)
    {
      MESSAGE("creatFieldDoubleFromField : Constructor (for Python API) FIELDDOUBLE with parameter FIELD_");
      MESSAGE("Its returns a proper cast of the input pointer :: FIELD_ --> FIELDDOUBLE");
      return (FIELDDOUBLE *) field;
    }

  FIELDINT * createFieldIntFromField(FIELD_ * field)
    {
      MESSAGE("creatFieldIntFromField : Constructor (for Python API) FIELDINT with parameter FIELD_");
      MESSAGE("Its returns a proper cast of the input pointer :: FIELD_ --> FIELDINT");
      return (FIELDINT *) field;
    }

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
%}
