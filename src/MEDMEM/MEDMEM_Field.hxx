/*
 File Field.hxx
 $Header$
*/

#ifndef FIELD_HXX
#define FIELD_HXX

#include <vector>

#include "utilities.h"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

#include "MEDMEM_Support.hxx"
#include "MEDMEM_Unit.hxx"
#include "MEDMEM_Array.hxx"
#include "MEDMEM_GenDriver.hxx"

#include "MEDMEM_MedFieldDriver.hxx"
#include "MEDMEM_MedMedDriver.hxx"

using namespace MED_EN;

//class GENDRIVER;
class FIELD_    // GENERIC POINTER TO a template <class T> class FIELD
{

protected:

  string    _name ;
  string    _description ;
  const SUPPORT * _support ;
  int       _numberOfComponents ;
  int *     _componentsTypes ; // array of size _numberOfComponents 
  // (constant, scalar, vector, tensor)
  // we could use an array of integer to store 
  // numbers of values
  //           1 for scalar, 
  //           space dimension for vector, 
  //           space dimension square for tensor.
  // so numbers of values per entities are 
  // sum of _componentsTypes array
  // Do not use yet! All type are scalar !
  string * _componentsNames;   	  // array of size _numberOfComponents
  string * _componentsDescriptions; // array of size _numberOfComponents
  UNIT *   _componentsUnits;        // array of size _numberOfComponents 
  string * _MEDComponentsUnits;     // array of size _numberOfComponents : 
  // get unit from med file

  int      _iterationNumber ;
  double   _time;
  int      _orderNumber ;

  /*MED_EN::*/med_type_champ _valueType ;

public:
 
  FIELD_ ();
  FIELD_(const SUPPORT * Support, const int NumberOfComponents);
  FIELD_(const FIELD_ &m);

  ~FIELD_();

//    virtual  void     setIterationNumber (int IterationNumber);
//    virtual  void     setOrderNumber     (int OrderNumber); 
//    virtual  void     setFieldName       (string& fieldName); 

  virtual  void     rmDriver(int index);
  virtual   int     addDriver(driverTypes driverType, 
                              const string & fileName,
                              const string & driverFieldName) ;
  virtual  int      addDriver( GENDRIVER & driver);
  virtual  void     write(const GENDRIVER &);
  virtual  void     read (const GENDRIVER &);

  //  virtual  void     getValueType (MED_FR::med_type_champ ValueType) ;
  //  virtual  void     setValueType (/*MED_EN::*/med_type_champ ValueType) ;
  //  virtual  med_type_champ getValueType () ;

  inline void     setName(string Name);
  inline string   getName() const;
  inline void     setDescription(string Description);
  inline string   getDescription()const;
  inline const SUPPORT * getSupport() const;
  inline void      setSupport(SUPPORT * support);
  inline void     setNumberOfComponents(int NumberOfComponents);
  inline int      getNumberOfComponents() const;
  //    inline void     setComponentType(int *ComponentType);
  //    inline int *    getComponentType() const;
  //    inline int      getComponentTypeI(int i) const;
  inline void     setComponentsNames(string * ComponentsNames);
  inline void setComponentName(int i, string ComponentName);
  inline string * getComponentsNames() const;
  inline string   getComponentName(int i) const;
  inline void     setComponentsDescriptions(string *ComponentsDescriptions);
  inline void     setComponentDescription(int i, string ComponentDescription);
  inline string * getComponentsDescriptions() const;
  inline string   getComponentDescription(int i) const;

  // provisoire : en attendant de regler le probleme des unites !
  inline void     setComponentsUnits(UNIT * ComponentsUnits);
  inline UNIT *   getComponentsUnits() const;
  inline UNIT *   getComponentUnit(int i) const;
  inline void     setMEDComponentsUnits(string * MEDComponentsUnits);
  inline void     setMEDComponentUnit(int i, string MEDComponentUnit);
  inline string * getMEDComponentsUnits() const;
  inline string   getMEDComponentUnit(int i) const;

  inline void     setIterationNumber(int IterationNumber);
  inline int      getIterationNumber() const;
  inline void     setTime(double Time);
  inline double   getTime() const;
  inline void     setOrderNumber(int OrderNumber); 
  inline int      getOrderNumber() const;

  inline void     setValueType (/*MED_EN::*/med_type_champ ValueType) ;
  inline /*MED_EN::*/med_type_champ getValueType () ;

};

// ---------------------------------
// Implemented Methods : constructor
// ---------------------------------

// -----------------
// Methodes Inline
// -----------------

inline void FIELD_::setName(string Name)
{
  _name=Name;
}
inline string FIELD_::getName() const 
{
  return _name;
}
inline void FIELD_::setDescription(string Description)
{
  _description=Description;
}
inline string FIELD_::getDescription() const 
{
  return _description;
}
inline void FIELD_::setNumberOfComponents(int NumberOfComponents)
{
  _numberOfComponents=NumberOfComponents;
} 
inline int FIELD_::getNumberOfComponents() const 
{ 
  return _numberOfComponents ; 
}
//  inline void FIELD_::setComponentType(int *ComponentType) 
//  { 
//    _componentsTypes=ComponentType ; 
//  }
//  inline int * FIELD_::getComponentType() const 
//  { 
//    return _componentsTypes ; 
//  }
//  inline int FIELD_::getComponentTypeI(int i) const 
//  { 
//    return _componentsTypes[i-1] ; 
//  }
inline void FIELD_::setComponentsNames(string * ComponentsNames) 
{ 
  _componentsNames=ComponentsNames ; 
}
inline void FIELD_::setComponentName(int i, string ComponentName) 
{ 
  _componentsNames[i-1]=ComponentName ; 
}
inline string * FIELD_::getComponentsNames() const 
{ 
  return _componentsNames ; 
}
inline string FIELD_::getComponentName(int i) const 
{ 
  return _componentsNames[i-1] ; 
}
inline void FIELD_::setComponentsDescriptions(string *ComponentsDescriptions) 
{ 
  _componentsDescriptions=ComponentsDescriptions ; 
}
inline void FIELD_::setComponentDescription(int i,string ComponentDescription)
{ 
  _componentsDescriptions[i-1]=ComponentDescription ; 
}
inline string * FIELD_::getComponentsDescriptions() const
{ 
  return _componentsDescriptions ; 
}
inline string FIELD_::getComponentDescription(int i) const
{ 
  return _componentsDescriptions[i-1]; 
}
inline void FIELD_::setComponentsUnits(UNIT * ComponentsUnits) 
{ 
  _componentsUnits=ComponentsUnits ; 
}
inline UNIT * FIELD_::getComponentsUnits() const 
{ 
  return _componentsUnits ; 
}
inline UNIT * FIELD_::getComponentUnit(int i) const 
{ 
  return &_componentsUnits[i-1] ; 
}
inline void FIELD_::setMEDComponentsUnits(string * MEDComponentsUnits) 
{ 
  _MEDComponentsUnits=MEDComponentsUnits ; 
}
inline void FIELD_::setMEDComponentUnit(int i, string MEDComponentUnit) 
{ 
  _MEDComponentsUnits[i-1]=MEDComponentUnit ; 
}
inline string * FIELD_::getMEDComponentsUnits() const 
{ 
  return _MEDComponentsUnits ; 
}
inline string FIELD_::getMEDComponentUnit(int i) const 
{ 
  return _MEDComponentsUnits[i-1] ; 
}
inline void FIELD_::setIterationNumber(int IterationNumber) 
{ 
  _iterationNumber=IterationNumber; 
} 
inline int FIELD_::getIterationNumber() const              
{ 
  return _iterationNumber ; 
} 
inline void FIELD_::setTime(double Time) 
{
  _time=Time ;   
} 
inline double FIELD_::getTime() const      
{ 
  return _time ; 
}
inline void FIELD_::setOrderNumber(int OrderNumber) 
{ 
  _orderNumber=OrderNumber ; 
}
inline int FIELD_::getOrderNumber() const                      
{ 
  return _orderNumber ;
}
inline  const SUPPORT * FIELD_::getSupport() const
{
  return _support ;
}

inline void FIELD_::setSupport(SUPPORT * support)
{
  _support = support ;
}

inline /*MED_EN::*/med_type_champ FIELD_::getValueType () 
{
  return _valueType ;
}

inline void FIELD_::setValueType (/*MED_EN::*/med_type_champ ValueType) 
{
  _valueType = ValueType ;
}

/////////////////////////
// END OF CLASS FIELD_ //
/////////////////////////

template <class T> class FIELD : public FIELD_
{

  // ------- Drivers Management Part
protected:

 //-----------------------//
   class INSTANCE 
  //-----------------------//
  {
  public:
    virtual GENDRIVER * run(const string & fileName, FIELD<T> * ptrFIELD) const = 0 ;
  } ;
  
  //-------------------------------------------------------//
  template <class T2> class INSTANCE_DE : public INSTANCE 
  //-------------------------------------------------------//
  {
  public :
    GENDRIVER * run(const string & fileName, FIELD<T> * ptrFIELD) const 
    { 
      return new T2(fileName,ptrFIELD); 
    }
  } ;
  
  //    static INSTANCE_DE<MED_FIELD_RDONLY_DRIVER> inst_med_rdonly ;
  static INSTANCE_DE<MED_FIELD_RDWR_DRIVER<T> >   inst_med ;
  static const INSTANCE * const instances[] ;

  // ------ End of Drivers Management Part
  
  vector<GENDRIVER *> _drivers; // Storage of the drivers currently in use

  // array of value of type T
  MEDARRAY<T> *_value ;

private:
  //setValueType() ;

public:
  FIELD();
  FIELD(const FIELD &m);
  FIELD & operator=(const FIELD &m); 	// A FAIRE
  FIELD(const SUPPORT * Support, const int NumberOfComponents); // Ajout NB Constructeur FIELD avec allocation de memoire de tous ses attribut
  FIELD(const SUPPORT * Support, driverTypes driverType,
	const string & fileName="", const string & fieldName="");
  ~FIELD();   

  friend class MED_FIELD_RDONLY_DRIVER<T>;
  friend class MED_FIELD_WRONLY_DRIVER<T>;
  friend class MED_FIELD_RDWR_DRIVER  <T>;

  void init ();
  void rmDriver(int index=0);
  int  addDriver(driverTypes driverType, 
		 const string & fileName="Default File Name.med",
		 const string & driverFieldName="Default Field Name") ;
  int  addDriver(GENDRIVER & driver);

  void allocValue(const int NumberOfComponents);
  void allocValue(const int NumberOfComponents, const int LengthValue);

  void deallocValue();
  
  inline void read(int index=0);  
  inline void read(const GENDRIVER & genDriver); 
  inline void write(int index=0, const string & driverName = "");
  inline void write(const GENDRIVER &);

  inline void     setValue(MEDARRAY<T> *Value);

  inline T*       getValue(medModeSwitch Mode) const;
  inline T*       getValueI(medModeSwitch Mode,int i) const;
  inline T        getValueIJ(int i,int j) const;

  inline void setValue(medModeSwitch mode, T* value);
  inline void setValueI(medModeSwitch mode, int i, T* value);
  inline void setValueIJ(int i, int j, T value);
};

// --------------------
// Implemented Methods 
// --------------------

template <class T>  FIELD<T>::FIELD(): 
 _value((MEDARRAY<T>*)NULL)
{
  MESSAGE("Constructeur FIELD sans parametre");
}

template <class T>  FIELD<T>::FIELD(const SUPPORT * Support,
				    const int NumberOfComponents):
  FIELD_(Support, NumberOfComponents)
{
  BEGIN_OF("FIELD<T>::FIELD(const SUPPORT * Support, const int NumberOfComponents)");
  
  int length = 0 ;
  try {
    length = Support->getNumberOfElements(MED_ALL_ELEMENTS);
  }
  catch (MEDEXCEPTION &ex) {
    MESSAGE("No value defined ! ("<<ex.what()<<")");
    _value = (MEDARRAY<T>*)NULL ;
  }
  MESSAGE("FIELD : constructeur : "<<length <<" et "<< NumberOfComponents);
  if (0<length)
    _value = new MEDARRAY<T>::MEDARRAY(NumberOfComponents,length);
  else
    _value = (MEDARRAY<T>*)NULL ;

  END_OF("FIELD<T>::FIELD(const SUPPORT * Support, const int NumberOfComponents)");
}

template <class T> void FIELD<T>::init ()
{
}

template <class T> FIELD<T>::FIELD(const FIELD & m):
  FIELD_((FIELD_) m)
{
  if (m._value != NULL)
    {
      _value = new MEDARRAY<T>::MEDARRAY(* m._value);
    }
  else
    _value = (MEDARRAY<T> *) NULL;
  _drivers = m._drivers;
}

template <class T> FIELD<T> & FIELD<T>::FIELD::operator=(const FIELD &m)
{
}

template <class T> FIELD<T>::FIELD(const SUPPORT * Support,
				   driverTypes driverType, 
				   const string & fileName/*=""*/, 
				   const string & fieldDriverName/*=""*/)
{
  const char * LOC = "template <class T> FIELD<T>::FIELD(const SUPPORT * Support, driverTypes driverType, const string & fileName="", const string & fieldName="") : ";
  
  int current;
  
  BEGIN_OF(LOC);
  
  init();

  _support = Support;
  _value = (MEDARRAY<T>*)NULL;

  current = addDriver(driverType,fileName,fieldDriverName);
  switch(_drivers[current]->getAccessMode() ) {
  case MED_RDONLY : {
    MESSAGE("MESH::MESH(driverTypes driverType, .....) : driverType must have a MED_RDWR accessMode");
    rmDriver(current);
    break;}
  default : {
  }
  }
  _drivers[current]->open();   
  _drivers[current]->read();
  _drivers[current]->close();
  END_OF(LOC);

}

template <class T> FIELD<T>::~FIELD() 
{   
  BEGIN_OF(" Destructeur FIELD<T>::~FIELD()");
  if (_value) delete _value;
  END_OF(" Destructeur FIELD<T>::~FIELD()");
}

template <class T> void FIELD<T>::allocValue(const int NumberOfComponents)
{
  BEGIN_OF("void FIELD<T>::allocValue(const int NumberOfComponents)");

  _numberOfComponents = NumberOfComponents ;
  if (_componentsTypes == NULL)
    _componentsTypes = new int[NumberOfComponents] ;
  if (_componentsNames == NULL)
    _componentsNames = new string[NumberOfComponents];
  if (_componentsDescriptions == NULL)
    _componentsDescriptions = new string[NumberOfComponents];
  if (_componentsUnits == NULL)
    _componentsUnits = new UNIT[NumberOfComponents];
  if (_MEDComponentsUnits == NULL)
    _MEDComponentsUnits = new string[NumberOfComponents];
  for (int i=0;i<NumberOfComponents;i++) {
    _componentsTypes[i] = 0 ;
  }

  try {
    int length = _support->getNumberOfElements(MED_ALL_ELEMENTS);
    MESSAGE("FIELD : constructeur : "<<length <<" et "<< NumberOfComponents);
    
    _value = new MEDARRAY<T>::MEDARRAY(NumberOfComponents,length);
  }
  catch (MEDEXCEPTION &ex) {
    MESSAGE("No value defined, problem with NumberOfComponents (and may be _support) size of MEDARRAY<T>::_value !");
    _value = (MEDARRAY<T>*)NULL ;
  }

  SCRUTE(_value);
  END_OF("void FIELD<T>::allocValue(const int NumberOfComponents)");
}

template <class T> void FIELD<T>::allocValue(const int NumberOfComponents, const int LengthValue)
{
  BEGIN_OF("void FIELD<T>::allocValue(const int NumberOfComponents,const int LengthValue)");

  _numberOfComponents = NumberOfComponents ;
  if (_componentsTypes == NULL)
    _componentsTypes = new int[NumberOfComponents] ;
  if (_componentsNames == NULL)
    _componentsNames = new string[NumberOfComponents];
  if (_componentsDescriptions == NULL)
    _componentsDescriptions = new string[NumberOfComponents];
  if (_componentsUnits == NULL)
    _componentsUnits = new UNIT[NumberOfComponents];
  if (_MEDComponentsUnits == NULL)
    _MEDComponentsUnits = new string[NumberOfComponents];
  for (int i=0;i<NumberOfComponents;i++) {
    _componentsTypes[i] = 0 ;
  }

  MESSAGE("FIELD : constructeur : "<<LengthValue <<" et "<< NumberOfComponents);
    
  _value = new MEDARRAY<T>::MEDARRAY(NumberOfComponents,LengthValue);

  SCRUTE(_value);
  END_OF("void FIELD<T>::allocValue(const int NumberOfComponents,const int LengthValue)");
}

template <class T> void FIELD<T>::deallocValue()
{
  BEGIN_OF("void FIELD<T>::deallocValue()");

  delete _value;

  END_OF("void FIELD<T>::deallocValue()");
}

// -----------------
// Methodes Inline
// -----------------


template <class T>       FIELD<T>::INSTANCE_DE<MED_FIELD_RDWR_DRIVER<T> >     FIELD<T>::inst_med  ;
template <class T> const FIELD<T>::INSTANCE * const FIELD<T>::instances[] = { &FIELD<T>::inst_med } ;


template <class T> int FIELD<T>::addDriver(driverTypes driverType, 
					   const string & fileName/*="Default File Name.med"*/,
					   const string & driverName/*="Default Field Name"*/)
{
  const char * LOC = "FIELD<T>::addDriver(driverTypes driverType, const string & fileName=\"Default File Name.med\",const string & driverName=\"Default Field Name\") : ";
  
  GENDRIVER * driver;
  int current;

  BEGIN_OF(LOC);
  
  driver = instances[driverType]->run(fileName, this) ;
  _drivers.push_back(driver);
  current = _drivers.size()-1;
  
  _drivers[current]->setFieldName(driverName);
  return current;

  END_OF(LOC);

}


template <class T> inline int FIELD<T>::addDriver (GENDRIVER & driver )
{
  const char * LOC = "FIELD<T>::addDriver(GENDRIVER &) : ";
  BEGIN_OF(LOC);
  
  _drivers.push_back(&driver);
  return _drivers.size() -1 ;
  
  END_OF(LOC);
};

template <class T> void FIELD<T>::rmDriver (int index/*=0*/)
{
  const char * LOC = "FIELD<T>::rmDriver (int index=0): ";
  BEGIN_OF(LOC);
  
  if ( _drivers[index] ) {
    //_drivers.erase(&_drivers[index]); 
    // why not ????
    MESSAGE ("detruire");
  }
  else
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "The index given is invalid, index must be between  0 and  |" 
                                     << _drivers.size() 
                                     )
                          );   
  
  END_OF(LOC);
}
  
template <class T> inline  void FIELD<T>::read(int index/*=0*/)  
{  
  const char * LOC = "FIELD<T>::read(int index=0) : ";
  BEGIN_OF(LOC);

  if ( _drivers[index] ) {
    _drivers[index]->open();   
    _drivers[index]->read();
    _drivers[index]->close();
  }
  else
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "The index given is invalid, index must be between  0 and |" 
                                     << _drivers.size() 
                                     )
                          ); 
  END_OF(LOC);
}

template <class T> inline void FIELD<T>::write(int index/*=0*/, const string & driverName /*= ""*/) 
{ 
  const char * LOC = "FIELD<T>::write(int index=0, const string & driverName = \"\") : ";
  BEGIN_OF(LOC);

  if( _drivers[index] ) {
    _drivers[index]->open();   
    if (driverName != "") _drivers[index]->setFieldName(driverName); 
    _drivers[index]->write();
    _drivers[index]->close();
  } 
  else
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "The index given is invalid, index must be between  0 and |" 
                                     << _drivers.size() 
                                     )
                          ); 
  END_OF(LOC);
}

template <class T> inline void FIELD<T>::write(const GENDRIVER & genDriver) 
{ 
  const char * LOC = " FIELD<T>::write(const GENDRIVER &) : ";
  BEGIN_OF(LOC);

  for (int index=0; index < _drivers.size(); index++ )
    if ( *_drivers[index] == genDriver ) { 
      _drivers[index]->open();   
      _drivers[index]->write(); 
      _drivers[index]->close();
    }
  
  END_OF(LOC);
 
} 

template <class T> inline void FIELD<T>::read(const GENDRIVER & genDriver) 
{ 
  const char * LOC = " FIELD<T>::read(const GENDRIVER &) : ";
  BEGIN_OF(LOC);

  for (int index=0; index < _drivers.size(); index++ )
    if ( *_drivers[index] == genDriver ) { 
      _drivers[index]->open();   
      _drivers[index]->read(); 
      _drivers[index]->close();
    }
  
  END_OF(LOC);
 
} 

template <class T> inline void FIELD<T>::setValue(MEDARRAY<T> *Value)  
{ 
  _value=Value ;  
}
template <class T> inline T* FIELD<T>::getValue(medModeSwitch Mode) const 
{ 
  return _value->get(Mode) ; 
}
template <class T> inline T* FIELD<T>::getValueI(medModeSwitch Mode,int i) const       
{
  return _value->getI(Mode,i) ; 
}
template <class T> inline T FIELD<T>::getValueIJ(int i,int j) const 
{
  return _value->getIJ(i,j) ; 
}

template <class T> inline void FIELD<T>::setValue(medModeSwitch mode, T* value)
{
  _value->set(mode,value);
}

template <class T> inline void FIELD<T>::setValueI(medModeSwitch mode, int i, T* value)
{
  _value->setI(mode,i,value);
}

template <class T> inline void FIELD<T>::setValueIJ(int i, int j, T value)
{
  _value->setIJ(i,j,value);
}

#endif /* FIELD_HXX */
