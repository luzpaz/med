#include "MEDMEM_Field.hxx"

// ---------------------------------
// FIELD_ : Constructors
// ---------------------------------
FIELD_::FIELD_(): 
  _name(""), _description(""), _support((SUPPORT *)NULL),
  _numberOfComponents(0), _componentsTypes((int *)NULL),
  _componentsNames((string *)NULL), 
  _componentsDescriptions((string *)NULL),
  _componentsUnits((UNIT*)NULL),
  _MEDComponentsUnits((string *)NULL),
  _iterationNumber(-1),_time(0.0),_orderNumber(-1)
{
  MESSAGE("Constructeur FIELD_ sans parametre");
}

FIELD_::FIELD_(const SUPPORT * Support, const int NumberOfComponents):
  _name(""), _description(""), _support(Support),
  _numberOfComponents(NumberOfComponents),
  _iterationNumber(-1),_time(0.0),_orderNumber(-1)
{
  MESSAGE("FIELD_(const SUPPORT * Support, const int NumberOfComponents)");

  _componentsTypes = new int[NumberOfComponents] ;
  _componentsNames = new string[NumberOfComponents];
  _componentsDescriptions = new string[NumberOfComponents];
  _componentsUnits = new UNIT[NumberOfComponents];
  _MEDComponentsUnits = new string[NumberOfComponents];
  for(int i=0;i<NumberOfComponents;i++) {
    _componentsTypes[i] = 0 ;
  }
}

FIELD_::FIELD_(const FIELD_ &m)
{
  _name = m._name;
  _description = m._description;
  _support = m._support;
  _numberOfComponents = m._numberOfComponents;
  _componentsTypes = m._componentsTypes;
  _componentsNames = m._componentsNames;
  _componentsDescriptions = m._componentsDescriptions;
  _componentsUnits = m._componentsUnits;
  _MEDComponentsUnits = m._MEDComponentsUnits;
  _iterationNumber = m._iterationNumber;
  _time = m._time;
  _orderNumber = m._orderNumber;
  _valueType = m._valueType;
}

FIELD_::~FIELD_()
{   
  MESSAGE("~FIELD_()");
  if ( _componentsTypes !=NULL)
    delete[] _componentsTypes ;
  if ( _componentsNames !=NULL)
    delete[] _componentsNames ;
  if ( _componentsDescriptions !=NULL)
    delete[] _componentsDescriptions ;
  if ( _componentsUnits !=NULL)
    delete[] _componentsUnits ;
  if ( _MEDComponentsUnits !=NULL)
    delete[] _MEDComponentsUnits ;
}

//  void     FIELD_::setIterationNumber (int IterationNumber)           {};
//  void     FIELD_::setOrderNumber     (int OrderNumber)               {}; 
//  void     FIELD_::setFieldName       (string& fieldName)             {}; 
         
void     FIELD_::rmDriver           (int index)                     {};
int      FIELD_::addDriver          (driverTypes driverType, 
                                   const string & fileName,
                                   const string & driverFieldName) {} ;
int      FIELD_::addDriver          (GENDRIVER & driver)            {};
void     FIELD_::write              (const GENDRIVER &)             {};

//  void                     FIELD_::setValueType(med_type_champ ValueType) {};
//  med_type_champ FIELD_::getValueType() {};
