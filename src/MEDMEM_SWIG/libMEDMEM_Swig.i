%module libMEDMEM_Swig

%include "typemaps.i"
%include "my_typemap.i"
 
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
#include "MEDMEM_SWIG_FieldDouble.hxx"
#include "MEDMEM_SWIG_FieldInt.hxx"
#include "MEDMEM_SWIG_MedFieldDoubleDriver.hxx"
#include "MEDMEM_SWIG_MedFieldIntDriver.hxx"
%}

/*
  enum du MED++ que l'on utilise dans l'API Python
*/

typedef enum {MED_FULL_INTERLACE, MED_NO_INTERLACE} medModeSwitch; 

typedef enum {MED_LECT,MED_ECRI,MED_REMP} med_mode_acces;

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
  medGeometryElement getType();

  int getNumberOfNodes();

  %addmethods {
    char * getName()
      {
	string tmp_str = self->getName();
	char * tmp = new char[strlen(tmp_str.c_str())+1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }
  }
};

class SUPPORT
{
 public:
  //  SUPPORT(MESH* Mesh, string Name="", medEntityMesh Entity=MED_CELL); 
  //  ~SUPPORT();

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

  /*  void setGeometricTypeNumber(int *GeometricTypeNumber); */

  void setNumberOfEntities(int *NumberOfEntities);

  void setTotalNumberOfEntities(int TotalNumberOfEntities);

  %addmethods {
    SUPPORT(MESH* Mesh, char * Name="", medEntityMesh Entity=MED_CELL)
      {
	return new SUPPORT(Mesh,string(Name),Entity);
      }

    void setpartial(char * Description, int NumberOfGeometricType,
		    int TotalNumberOfEntity, medGeometryElement *GeometricType,
		    int *NumberOfEntity, int *NumberValue)
      {
	self->setpartial(string(Description), NumberOfGeometricType,
			 TotalNumberOfEntity, GeometricType,
			 NumberOfEntity, NumberValue);
      }

    void setName(char * Name)
      {
	self->setName(string(Name));
      }

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
       
	medGeometryElement * types = self->getTypes();
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
	return Py_BuildValue("O", py_list);
      }

    /*
    PyObject * getGeometricTypeNumber()
      {
	PyObject *py_list;
       
	int * GeometricTypeNumber = self->getGeometricTypeNumber();
	int size = self->getNumberOfTypes();
	py_list = PyList_New(size);
	for (int i=0; i < size; i++)
	  {
	    int err = PyList_SetItem(py_list, i,
				     Py_BuildValue("i",
						   GeometricTypeNumber[i]));
	    if(err)
	      {
		char * message = "Error in SUPPORT::getGeometricTypeNumber";
		PyErr_SetString(PyExc_RuntimeError, message);
		return NULL;
	      }
	  }
	return Py_BuildValue("O", py_list);
      }
    */

    PyObject * getNumber(medGeometryElement GeometricType)
      {
	PyObject *py_list;

	int * number = self->getNumber(GeometricType);
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
	return Py_BuildValue("O", py_list);
      }

    PyObject * getNumberIndex()
      {
	PyObject *py_list;

	int * numberindex = self->getNumberIndex();
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
	return Py_BuildValue("O", py_list);
      }
  }
};

class FAMILY : public SUPPORT
{
 public:
  //  ~FAMILY();

  void setIdentifier(int Identifier);        
  void setNumberOfAttributes(int NumberOfAttribute);
  void setAttributesIdentifiers(int * AttributeIdentifier);
  void setAttributesValues(int * AttributeValue);
  void setAttributesDescriptions(string * AttributeDescription); 
  void setNumberOfGroups(int NumberOfGroups);
  void setGroupsNames(string * GroupName);

  int getIdentifier() const;
  int getNumberOfAttributes() const;
  /*  int * getAttributesIdentifiers() const;*/
  /*  int * getAttributesValues() const;*/
  /*  string * getAttributesDescriptions() const;*/
  int getNumberOfGroups() const;
  /*  string * getGroupsNames() const;*/

  %addmethods {
    FAMILY(MESH* Mesh, int Identifier, char * Name, int NumberOfAttribute,
	   int *AttributeIdentifier, int *AttributeValue,
	   char * AttributeDescription, int NumberOfGroup,
	   char * GroupName)
      {
	return new FAMILY(Mesh,Identifier,string(Name),NumberOfAttribute,
			  AttributeIdentifier,AttributeValue,
			  string(AttributeDescription),NumberOfGroup,
			  string(GroupName));
      }

    const char * getAttributeDescription(int i)
      {
	string tmp_str = self->getAttributeDescription(i);
	char * tmp = new char[strlen(tmp_str.c_str())+1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }

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

	int * attributesids = self->getAttributesIdentifiers();
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
	return Py_BuildValue("O", py_list);
      }

    PyObject * getAttributesValues()
      {
	PyObject *py_list;

	int * attributesvals = self->getAttributesValues();
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
	return Py_BuildValue("O", py_list);
      }
  }
}; 

class FIELD_
{
public:
  FIELD_(const SUPPORT * Support, const int NumberOfComponents);
  //  ~FIELD_();
  
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

  %addmethods {
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

    const char * getDescription()
      {
	string tmp_str = self->getDescription();
	char * tmp = new char[strlen(tmp_str.c_str())+1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;	
      }

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
  //  ~FIELDDOUBLE();

  FIELDDOUBLE(const SUPPORT * Support, const int NumberOfComponents);

  /*
    WARNING:
    other constructor of FIELDDOUBLE (C++ FIELD<double>) object.
    Only one constructor could be wrapped and
    the others commented out when using 
    SWIG with a version lesser than 1.3
  */

  //FIELDDOUBLE();    

  void read(int index=0);

  double getValueIJ(int i,int j) const;

  void setValue(medModeSwitch mode, double* value);

  void setValueI(medModeSwitch mode, int i, double* value);

  void setValueIJ(int i, int j, double value);

  void allocValue(const int NumberOfComponents);

  void deallocValue();

  %addmethods {
    /*
    FIELDDOUBLE (const SUPPORT * Support, driverTypes driverType,
		 char * fileName, char * fieldName)
      {
	return new FIELDDOUBLE(Support, driverType, string(fileName),
			       string(fieldName));
      }
    <*/

    void write(int index=0, char * driverName="")
      {
	self->write(index, string(driverName));
      }

    PyObject * getValue(medModeSwitch Mode)
      {
	PyObject *py_list;

	int size = (self->getNumberOfComponents())*
	  ((self->getSupport())->getNumberOfElements(MED_ALL_ELEMENTS));

	double * value = self->getValue(Mode);

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
	return Py_BuildValue("O", py_list);
      }

    PyObject * getValueI(medModeSwitch Mode, int index)
      {
	PyObject *py_list;

	int size = self->getNumberOfComponents();

	if ( Mode == MED_NO_INTERLACE ) size = (self->getSupport())->getNumberOfElements(MED_ALL_ELEMENTS);

	double * value = self->getValueI(Mode,index);

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
	return Py_BuildValue("O", py_list);
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
  //  ~FIELDINT();

  FIELDINT(const SUPPORT * Support, const int NumberOfComponents);

  /*
    WARNING:
    other constructor of FIELDINT (C++ FIELD<int>) object.
    other constructor of MED object.
    Only one constructor could be wrapped and
    the others commented out when using 
    SWIG with a version lesser than 1.3
  */

  /*
  FIELDINT();
  */

  void read(int index=0);

  int getValueIJ(int i,int j) const;

  void setValue(medModeSwitch mode, int* value);

  void setValueI(medModeSwitch mode, int i, int* value);

  void setValueIJ(int i, int j, int value);

  void allocValue(const int NumberOfComponents);

  void deallocValue();

  %addmethods {
    /*
    FIELDINT(const SUPPORT * Support, driverTypes driverType,
             char * fileName, char * fieldName)
      {
	return new FIELDDOUBLE(Support, driverType, string(fileName),
			       string(fieldName));
      }
    */

    void write(int index=0, char * driverName="")
      {
	self->write(index, string(driverName));
      }

    PyObject * getValue(medModeSwitch Mode)
      {
	PyObject *py_list;

	int size = (self->getNumberOfComponents())*
	  ((self->getSupport())->getNumberOfElements(MED_ALL_ELEMENTS));

	int * value = self->getValue(Mode);

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
	return Py_BuildValue("O", py_list);
      }

    PyObject * getValueI(medModeSwitch Mode, int index)
      {
	PyObject *py_list;

	int size = self->getNumberOfComponents();

	if ( Mode == MED_NO_INTERLACE ) size = (self->getSupport())->getNumberOfElements(MED_ALL_ELEMENTS);

	int * value = self->getValueI(Mode,index);

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
	return Py_BuildValue("O", py_list);
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
  //  ~GROUP();

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

  //  MESH();    

  void rmDriver(int index=0);

  void read(int index=0);

  int getSpaceDimension();

  int getMeshDimension();

  int getNumberOfNodes();

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

  SUPPORT * getBoundaryElements(medEntityMesh Entity) ;

  %addmethods {
    CELLMODEL getCellType(medEntityMesh Entity,int i)
      {
	return self->getCellsTypes(Entity)[i];
      };

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

    const char * getName()
      {
	string tmp_str = self->getName();
	char * tmp = new char[strlen(tmp_str.c_str())+1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }

    const char * getCoordinatesSystem()
      {
	string tmp_str = self->getCoordinatesSystem();
	char * tmp = new char[strlen(tmp_str.c_str())+1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }

    const char * getCoordinateName(int i)
      {
	string tmp_str = self->getCoordinatesNames()[i];
	char * tmp = new char[strlen(tmp_str.c_str())+1];
	strcpy(tmp,tmp_str.c_str());
	return tmp;
      }

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
	string * array = self->getCoordinatesNames();
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
	return Py_BuildValue("O", py_list);
      }

    PyObject * getCoordinatesUnits()
      {
	PyObject *py_list;
	string * array = self->getCoordinatesUnits();
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
	return Py_BuildValue("O", py_list);
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
	return Py_BuildValue("O", py_list);
      }

    PyObject * getTypes(medEntityMesh Entity)
      {
	PyObject *py_list;
       
	medGeometryElement * types = self->getTypes(Entity);
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
	return Py_BuildValue("O", py_list);
      }

    PyObject * getConnectivity(medModeSwitch Mode,
			       medConnectivity ConnectivityType,
			       medEntityMesh Entity,
			       medGeometryElement Type)
      {
	PyObject *py_list;
	int * connectivity = self->getConnectivity(Mode,ConnectivityType,
						   Entity,Type);
	int nbOfElm = self->getNumberOfElements(Entity,Type);
	int size;

	if (Type == MED_ALL_ELEMENTS)
	  {
	    size = self->getConnectivityIndex(ConnectivityType,Entity)[nbOfElm];
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
	return Py_BuildValue("O", py_list);
      }

    PyObject * getConnectivityIndex(medConnectivity ConnectivityType,
				    medEntityMesh Entity)
      {
	PyObject *py_list;
	int * connectivity_index =
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
	return Py_BuildValue("O", py_list);
      }

    PyObject * getReverseConnectivity(medConnectivity ConnectivityType)
      {
	PyObject *py_list;
	int * reverseconnectivity =
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

	int size = self->getReverseConnectivityIndex(ConnectivityType)[nb];

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
	return Py_BuildValue("O", py_list);
      }

    PyObject * getReverseConnectivityIndex(medConnectivity
					   ConnectivityType)
      {
	PyObject *py_list;
	int * reverseconnectivity_index =
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
	return Py_BuildValue("O", py_list);
      }

 FIELDDOUBLE * getVolume(const SUPPORT * Support)
    {
      return (FIELDDOUBLE *) self->getVolume(Support);
    }

  FIELDDOUBLE * getArea(const SUPPORT * Support)
    {
      return (FIELDDOUBLE *) self->getArea(Support);
    }

  FIELDDOUBLE * getLength(const SUPPORT * Support)
    {
      return (FIELDDOUBLE *) self->getLength(Support);
    }

  FIELDDOUBLE * getNormal(const SUPPORT * Support)
    {
      return (FIELDDOUBLE *) self->getNormal(Support);
    }

  FIELDDOUBLE * getBarycenter(const SUPPORT * Support)
    {
      return (FIELDDOUBLE *) self->getBarycenter(Support);
    }
  }
} ;

class MED
{
 public:
  MED();

  ~MED();

  void rmDriver (int index=0);

  int getNumberOfMeshes ( void ) const;       

  int getNumberOfFields ( void ) const;

  void updateSupport ( void ) ;

  void write (int index=0);

  %addmethods {
    /*
      WARNING:
      other constructor of MED object.
      Only one constructor could be wrapped and
      the others commented out when using 
      SWIG with a version lesser than 1.3
    */
    /*
    MED(driverTypes driverType, char * fileName)
      {
	return new MED(driverType,string(fileName));
      }
    */
    int addDriver(driverTypes driverType,
		  char * fileName="Default File Name.med")
      {
	return self->addDriver(driverType,string(fileName));
      }

    const char * getMeshName(int i)
      {
	deque<string> list_string = self->getMeshNames();
	char * tmp = new char[strlen(list_string[i].c_str())+1];
	strcpy(tmp,list_string[i].c_str());
	return tmp;
      }

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
  APPI du driver MED_MED
*/

class MED_MED_DRIVER
{
 public :
  void open();
  void close();

  virtual void write          ( void ) const ;
  virtual void writeFrom      ( void ) const ;
  virtual void read           ( void ) ;
  virtual void readFileStruct ( void ) ;

  %addmethods {
    MED_MED_DRIVER(char * fileName,  MED * ptrMed)
      {
	return new MED_MED_DRIVER(string(fileName), ptrMed);
      }
  }
};

class MED_MED_RDONLY_DRIVER : public virtual MED_MED_DRIVER
{
 public :
  void open();
  void close();

  void read           ( void ) ;
  void readFileStruct ( void ) ;

  %addmethods {
    MED_MED_RDONLY_DRIVER(char * fileName,  MED * ptrMed)
      {
	return new MED_MED_RDONLY_DRIVER(string(fileName), ptrMed);
      }
  }
};

class MED_MED_WRONLY_DRIVER : public virtual MED_MED_DRIVER
{
 public :
  void open();
  void close();

  void write          ( void ) const ;
  void writeFrom      ( void ) const ;

  %addmethods {
    MED_MED_WRONLY_DRIVER(char * fileName,  MED * ptrMed)
      {
	return new MED_MED_WRONLY_DRIVER(string(fileName), ptrMed);
      }
  }
};

class MED_MED_RDWR_DRIVER : public virtual MED_MED_RDONLY_DRIVER,
			    public virtual MED_MED_WRONLY_DRIVER,
			    public virtual MED_MED_DRIVER
{
 public :
  void open();
  void close();

  void write          ( void ) const ;
  void writeFrom      ( void ) const ;
  void read           ( void ) ;
  void readFileStruct ( void ) ;

  %addmethods {
    MED_MED_RDWR_DRIVER(char * fileName,  MED * ptrMed)
      {
	return new MED_MED_RDWR_DRIVER(string(fileName), ptrMed);
      }
  }
};

/*
  API du driver MED_MESH
*/

/*
class MED_MESH_DRIVER
{
public :

  void open();

  void close();

  virtual void write( void ) const = 0 ;

  virtual void read ( void ) = 0 ;

  %addmethods {
    MED_MESH_DRIVER(char * fileName,  MESH * ptrMesh,
		    med_mode_acces accessMode)
      {
	return new MED_MESH_DRIVER(string(fileName), ptrMesh, accessMode);
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
*/

class MED_MESH_RDONLY_DRIVER : public virtual MED_MESH_DRIVER
{
 public :
 
   // MED_MESH_RDONLY_DRIVER();

   //  MED_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh);
  
  ~MED_MESH_RDONLY_DRIVER();

  int getCOORDINATE();

  int getCONNECTIVITY();

  int getFAMILY();

  void open();

  void close();

  void write( void ) ;

  void read ( void ) ;

  %addmethods {
    MED_MESH_RDONLY_DRIVER(char * fileName,  MESH * ptrMesh)
      {
	return new MED_MESH_RDONLY_DRIVER(string(fileName), ptrMesh);
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

class MED_MESH_WRONLY_DRIVER : public virtual MED_MESH_DRIVER
{
 public :
  
   //  MED_MESH_WRONLY_DRIVER():MED_MESH_DRIVER() {}
  
   //  MED_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh);

  ~MED_MESH_WRONLY_DRIVER();

  void write( void ) const ;

  void read ( void ) ;

  void open();

  void close();

  int writeCoordinates    ()                         const;

  int writeConnectivities (medEntityMesh entity)     const;

  int writeFamilyNumbers  ()                         const;

  int writeFamilies       (vector<FAMILY*> & families) const;

  %addmethods {
    MED_MESH_WRONLY_DRIVER(char * fileName,  MESH * ptrMesh)
      {
	return new MED_MESH_WRONLY_DRIVER(string(fileName), ptrMesh);
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

class MED_MESH_RDWR_DRIVER : public virtual MED_MESH_RDONLY_DRIVER,
			     public virtual MED_MESH_WRONLY_DRIVER
{
 public :

   //  MED_MESH_RDWR_DRIVER();

   //  MED_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh);

  ~MED_MESH_RDWR_DRIVER();
  
  void write(void) const ;

  void read (void)       ;

  void open();

  void close();

  %addmethods {
    MED_MESH_RDWR_DRIVER(char * fileName,  MESH * ptrMesh)
      {
	return new MED_MESH_RDWR_DRIVER(string(fileName), ptrMesh);
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

%{
  FIELDDOUBLE * createFieldDoubleFromField(FIELD_ * field)
    {
      MESSAGE("creatFieldDoubleFromField : Constructor (for Python API) FIELDDOUBLE with parameter FIELD_");
      return (FIELDDOUBLE *) field;
    }

  FIELDINT * createFieldIntFromField(FIELD_ * field)
    {
      MESSAGE("creatFieldIntFromField : Constructor (for Python API) FIELDINT with parameter FIELD_");
      return (FIELDINT *) field;
    }
%}
