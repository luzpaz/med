//  MED MEDMEM : MED files in memory
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
//  File   : MEDMEM_Field.hxx
//  Module : MED

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

/*!

  This class contains all the informations related with a template class FIELD :
  - Components descriptions
  - Time step description
  - Location of the values (a SUPPORT class)

*/

class FIELD_    // GENERIC POINTER TO a template <class T> class FIELD
{
protected:

  bool            _isRead ;

  /*!
    \if developper
    Field name.
    \endif
  */
  string          _name ;
  /*!
    \if developper
    Field description.
    \endif
  */
  string          _description ;
  /*!
    \if developper
    Pointer to the support the field deals with.
    \endif
  */
  const SUPPORT * _support ;

  /*!
    \if developper
    Number of field's components.
    \endif
  */
  int             _numberOfComponents ;
  /*!
    \if developper
    Number of field's values.
    \endif
  */
  int             _numberOfValues ;

  /*!
    \if developper
    Array of size _numberOfComponents. /n
    (constant, scalar, vector, tensor)/n
    We could use an array of integer to store
    numbers of values: /n
    - 1 for scalar,/n
    - space dimension for vector,/n
    - space dimension square for tensor./n
    So numbers of values per entities would be
    sum of _componentsTypes array.

    Not implemented yet! All type are scalar !
    \endif
  */
  int *           _componentsTypes ;
  /*!
    \if developper
    Array of size _numberOfComponents
    storing components names if any.
    \endif
  */
  string * 	  _componentsNames;   	
  /*!
    \if developper
    Array of size _numberOfComponents
    storing components descriptions if any.
    \endif
 */
  string *        _componentsDescriptions;
  /*!
    \if developper
    Array of size _numberOfComponents
    storing components units if any.
    \endif
 */
  UNIT *          _componentsUnits;
  /*!
    \if developper
    Array of size _numberOfComponents
    storing components units if any.
    \endif
  */
  string *        _MEDComponentsUnits;
  int             _iterationNumber ;
  double   	  _time;
  int      	  _orderNumber ;

  med_type_champ _valueType ;

  vector<GENDRIVER *> _drivers; // Storage of the drivers currently in use

public:

  friend class MED_MED_RDONLY_DRIVER;
  friend class MED_MED_WRONLY_DRIVER;
  friend class MED_MED_RDWR_DRIVER;

  /*!
    Constructor.
  */
  FIELD_ ();
  /*!
    Constructor.
  */
  FIELD_(const SUPPORT * Support, const int NumberOfComponents);
  /*!
    Copy constructor.
  */
  FIELD_(const FIELD_ &m);

  /*!
    Destructor.
  */
  ~FIELD_();

//    virtual  void     setIterationNumber (int IterationNumber);
//    virtual  void     setOrderNumber     (int OrderNumber);
//    virtual  void     setFieldName       (string& fieldName);

  virtual  void     rmDriver(int index);
  virtual   int     addDriver(driverTypes driverType,
                              const string & fileName="Default File Name.med",
			      const string & driverFieldName="Default Field Nam") ;
  virtual  int      addDriver( GENDRIVER & driver);
  virtual  void     read (const GENDRIVER &);
  virtual  void     read(int index=0);
  virtual  void     write(const GENDRIVER &);
  virtual  void     write(int index=0, const string & driverName="");

  inline void     setName(string Name);
  inline string   getName() const;
  inline void     setDescription(string Description);
  inline string   getDescription() const;
  inline const SUPPORT * getSupport() const;
  inline void     setSupport(SUPPORT * support);
  inline void     setNumberOfComponents(int NumberOfComponents);
  inline int      getNumberOfComponents() const;
  inline void     setNumberOfValues(int NumberOfValues);
  inline int      getNumberOfValues() const;
  //    inline void     setComponentType(int *ComponentType);
  //    inline int *    getComponentType() const;
  //    inline int      getComponentTypeI(int i) const;
  inline void     setComponentsNames(string * ComponentsNames);
  inline void     setComponentName(int i, string ComponentName);
  inline const string * getComponentsNames() const;
  inline string   getComponentName(int i) const;
  inline void     setComponentsDescriptions(string *ComponentsDescriptions);
  inline void     setComponentDescription(int i, string ComponentDescription);
  inline const string * getComponentsDescriptions() const;
  inline string   getComponentDescription(int i) const;

  // provisoire : en attendant de regler le probleme des unites !
  inline void     setComponentsUnits(UNIT * ComponentsUnits);
  inline const UNIT *   getComponentsUnits() const;
  inline const UNIT *   getComponentUnit(int i) const;
  inline void     setMEDComponentsUnits(string * MEDComponentsUnits);
  inline void     setMEDComponentUnit(int i, string MEDComponentUnit);
  inline const string * getMEDComponentsUnits() const;
  inline string   getMEDComponentUnit(int i) const;

  inline void     setIterationNumber(int IterationNumber);
  inline int      getIterationNumber() const;
  inline void     setTime(double Time);
  inline double   getTime() const;
  inline void     setOrderNumber(int OrderNumber);
  inline int      getOrderNumber() const;

  inline void     setValueType (med_type_champ ValueType) ;
  inline med_type_champ getValueType () const;

};

// ---------------------------------
// Implemented Methods : constructor
// ---------------------------------

// -----------------
// Methodes Inline
// -----------------
/*!
  Set FIELD name.
*/
inline void FIELD_::setName(string Name)
{
  _name=Name;
}
/*!
  Get FIELD name.
*/
inline string FIELD_::getName() const
{
  return _name;
}
/*!
  Set FIELD description.
*/
inline void FIELD_::setDescription(string Description)
{
  _description=Description;
}
/*!
  Get FIELD description.
*/
inline string FIELD_::getDescription() const
{
  return _description;
}
/*!
  Set FIELD number of components.
*/
inline void FIELD_::setNumberOfComponents(int NumberOfComponents)
{
  _numberOfComponents=NumberOfComponents;
}
/*!
  Get FIELD number of components.
*/
inline int FIELD_::getNumberOfComponents() const
{
  return _numberOfComponents ;
}
/*!
  Set FIELD number of values.

  It must be the same than in the associated SUPPORT object.
*/
inline void FIELD_::setNumberOfValues(int NumberOfValues)
{
  _numberOfValues=NumberOfValues;
}
/*!
  Get FIELD number of value.
*/
inline int FIELD_::getNumberOfValues() const
{
  return _numberOfValues ;
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

/*!
  Set FIELD components names.

  Duplicate the ComponentsNames string array to put components names in
  FIELD. ComponentsNames size must be equal to number of components.
*/
inline void FIELD_::setComponentsNames(string * ComponentsNames)
{
  if (NULL == _componentsNames)
    _componentsNames = new string[_numberOfComponents] ;
  for (int i=0; i<_numberOfComponents; i++)
    _componentsNames[i]=ComponentsNames[i] ;
}
/*!
  Set FIELD i^th component name.

  i must be >=1 and <= number of components.
*/
inline void FIELD_::setComponentName(int i, string ComponentName)
{
  _componentsNames[i-1]=ComponentName ;
}
/*!
  Get a reference to the string array which contain the components names.

  This Array size is equal to number of components
*/
inline const string * FIELD_::getComponentsNames() const
{
  return _componentsNames ;
}
/*!
  Get the name of the i^th component.
*/
inline string FIELD_::getComponentName(int i) const
{
  return _componentsNames[i-1] ;
}
/*!
  Set FIELD components descriptions.

  Duplicate the ComponentsDescriptions string array to put components 
  descriptions in FIELD.
  ComponentsDescriptions size must be equal to number of components.
*/
inline void FIELD_::setComponentsDescriptions(string *ComponentsDescriptions)
{
  if (NULL == _componentsDescriptions)
    _componentsDescriptions = new string[_numberOfComponents] ;
  for (int i=0; i<_numberOfComponents; i++)
    _componentsDescriptions[i]=ComponentsDescriptions[i] ;
}
/*!
  Set FIELD i^th component description.

  i must be >=1 and <= number of components.
*/
inline void FIELD_::setComponentDescription(int i,string ComponentDescription)
{
  _componentsDescriptions[i-1]=ComponentDescription ;
}
/*!
  Get a reference to the string array which contain the components descriptions.

  This Array size is equal to number of components
*/
inline const string * FIELD_::getComponentsDescriptions() const
{
  return _componentsDescriptions ;
}
/*!
  Get the description of the i^th component.
*/
inline string FIELD_::getComponentDescription(int i) const
{
  return _componentsDescriptions[i-1];
}

/*!
  \todo
  Set FIELD components UNIT.

  Duplicate the ComponentsUnits UNIT array to put components 
  units in FIELD.
  ComponentsUnits size must be equal to number of components.
*/
inline void FIELD_::setComponentsUnits(UNIT * ComponentsUnits)
{
  if (NULL == _componentsUnits)
    _componentsUnits = new UNIT[_numberOfComponents] ;
  for (int i=0; i<_numberOfComponents; i++)
    _componentsUnits[i]=ComponentsUnits[i] ;
}
/*!
  Get a reference to the UNIT array which contain the components units.

  This Array size is equal to number of components
*/
inline const UNIT * FIELD_::getComponentsUnits() const
{
  return _componentsUnits ;
}
/*!
  Get the UNIT of the i^th component.
*/
inline const UNIT * FIELD_::getComponentUnit(int i) const
{
  return &_componentsUnits[i-1] ;
}
/*!
  Set FIELD components unit.

  Duplicate the MEDComponentsUnits string array to put components 
  units in FIELD.
  MEDComponentsUnits size must be equal to number of components.
  
*/
inline void FIELD_::setMEDComponentsUnits(string * MEDComponentsUnits)
{
  if (NULL == _MEDComponentsUnits)
    _MEDComponentsUnits = new string[_numberOfComponents] ;
  for (int i=0; i<_numberOfComponents; i++)
    _MEDComponentsUnits[i]=MEDComponentsUnits[i] ;
}
/*!
  Set FIELD i^th component unit.

  i must be >=1 and <= number of components.
*/
inline void FIELD_::setMEDComponentUnit(int i, string MEDComponentUnit)
{
  _MEDComponentsUnits[i-1]=MEDComponentUnit ;
}
/*!
  Get a reference to the string array which contain the components units.

  This Array size is equal to number of components
*/
inline const string * FIELD_::getMEDComponentsUnits() const
{
  return _MEDComponentsUnits ;
}
/*!
  Get the string for unit of the i^th component.
*/
inline string FIELD_::getMEDComponentUnit(int i) const
{
  return _MEDComponentsUnits[i-1] ;
}
/*!
  Set the iteration number where FIELD has been calculated.
*/
inline void FIELD_::setIterationNumber(int IterationNumber)
{
  _iterationNumber=IterationNumber;
}
/*!
  Get the iteration number where FIELD has been calculated.
*/
inline int FIELD_::getIterationNumber() const
{
  return _iterationNumber ;
}
/*!
  Set the time (in second) where FIELD has been calculated.
*/
inline void FIELD_::setTime(double Time)
{
  _time=Time ;
}
/*!
  Get the time (in second) where FIELD has been calculated.
*/
inline double FIELD_::getTime() const
{
  return _time ;
}
/*!
  Set the order number where FIELD has been calculated.

  It corresponds to internal iteration during one time step.
*/
inline void FIELD_::setOrderNumber(int OrderNumber)
{
  _orderNumber=OrderNumber ;
}
/*!
  Get the order number where FIELD has been calculated.
*/
inline int FIELD_::getOrderNumber() const
{
  return _orderNumber ;
}
/*!
  Get a reference to the SUPPORT object associated to FIELD.
*/
inline  const SUPPORT * FIELD_::getSupport() const
{
  return _support ;
}
/*!
  Set the reference to the SUPPORT object associated to FIELD.

  Reference is not duplicate, so it must not be deleted.
*/
inline void FIELD_::setSupport(SUPPORT * support)
{
  _support = support ;
}
/*!
  Get the FIELD med value type (MED_INT32 or MED_REEL64).
*/
inline med_type_champ FIELD_::getValueType () const
{
  return _valueType ;
}
/*!
  Set the FIELD med value type (MED_INT32 or MED_REEL64).
*/
inline void FIELD_::setValueType (med_type_champ ValueType)
{
  _valueType = ValueType ;
}

/////////////////////////
// END OF CLASS FIELD_ //
/////////////////////////

/*!

  This template class contains informations related with a FIELD :
  - Values of the field

*/

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
  //friend class MED_FIELD_RDWR_DRIVER  <T>;

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

  inline MEDARRAY<T>* getvalue() const;
  inline const T*       getValue(medModeSwitch Mode) const;
  inline const T*       getValueI(medModeSwitch Mode,int i) const;
  inline T        getValueIJ(int i,int j) const;

  inline void setValue(medModeSwitch mode, T* value);
  inline void setValueI(medModeSwitch mode, int i, T* value);
  inline void setValueIJ(int i, int j, T value);

  /*!
    This fonction feeds the FIELD<double> private attributs _value with the
    volume of each cells belonging to the argument Support. The field has to be
    initialised via the constructor FIELD<double>(const SUPPORT * , const int )
    with Support as SUPPORT argument, 1 has the number of components, and Support
    has be a SUPPORT on 3D cells. This initialisation could be done by the empty
    constructor followed by a setSupport and setNumberOfComponents call but it has
    be followed by a setValueType(MED_REEL64) call.
   */
  void getVolume() const throw (MEDEXCEPTION) ;
  /*!
    This fonction feeds the FIELD<double> private attributs _value with the
    area of each cells (or faces) belonging to the attribut _support. The field
    has to be initialised via the constructor FIELD<double>(const SUPPORT * ,
    const int ) with 1 has the number of components, and _support has be a
    SUPPORT on 2D cells or 3D faces. This initialisation could be done by the
    empty constructor followed by a setSupport and setNumberOfComponents call but
    it has be followed by a setValueType(MED_REEL64) call.
   */
  void getArea() const throw (MEDEXCEPTION) ;
  /*!
    This fonction feeds the FIELD<double> private attributs _value with the
    length of each segments belonging to the attribut _support. The field has
    to be initialised via the constructor FIELD<double>(const SUPPORT * ,
    const int ) with 1 has the number of components, and _support has be a
    SUPPORT on 3D edges or 2D faces. This initialisation could be done by the
    empty constructor followed by a setSupport and setNumberOfComponents call but
    it has be followed by a setValueType(MED_REEL64) call.
   */
  void getLength() const throw (MEDEXCEPTION) ;
  /*!
    This fonction feeds the FIELD<double> private attributs _value with the
    normal vector of each faces belonging to the attribut _support. The field
    has to be initialised via the constructor FIELD<double>(const SUPPORT * ,
    const int ) with the space dimension has the number of components, and
    _support has be a SUPPORT on 3D or 2D faces. This initialisation could be done
    by the empty constructor followed by a setSupport and setNumberOfComponents
    call but it has be followed by a setValueType(MED_REEL64) call.
   */
  void getNormal() const throw (MEDEXCEPTION) ;
  /*!
    This fonction feeds the FIELD<double> private attributs _value with the
    barycenter of each faces or cells or edges belonging to the attribut _support.
    The field has to be initialised via the constructor
    FIELD<double>(const SUPPORT * ,const int ) with the space dimension has the
    number of components, and _support has be a SUPPORT on 3D cells or 2D faces.
    This initialisation could be done by the empty constructor followed by a
    setSupport and setNumberOfComponents call but it has be followed by a
    setValueType(MED_REEL64) call.
   */
  void getBarycenter() const throw (MEDEXCEPTION) ;
};

// --------------------
// Implemented Methods
// --------------------

/*!
  Constructor.
*/
template <class T>  FIELD<T>::FIELD():
 _value((MEDARRAY<T>*)NULL)
{
  MESSAGE("Constructeur FIELD sans parametre");
}

/*!
  Constructor.
*/
template <class T>  FIELD<T>::FIELD(const SUPPORT * Support,
				    const int NumberOfComponents):
  FIELD_(Support, NumberOfComponents)
{
  BEGIN_OF("FIELD<T>::FIELD(const SUPPORT * Support, const int NumberOfComponents)");

  try {
    _numberOfValues = Support->getNumberOfElements(MED_ALL_ELEMENTS);
  }
  catch (MEDEXCEPTION &ex) {
    MESSAGE("No value defined ! ("<<ex.what()<<")");
    _value = (MEDARRAY<T>*)NULL ;
  }
  MESSAGE("FIELD : constructeur : "<< _numberOfValues <<" et "<< NumberOfComponents);
  if (0<_numberOfValues) {
    _value = new MEDARRAY<T>::MEDARRAY(_numberOfComponents,_numberOfValues);
    _isRead = true ;
  } else
    _value = (MEDARRAY<T>*)NULL ;

  END_OF("FIELD<T>::FIELD(const SUPPORT * Support, const int NumberOfComponents)");
}

/*!
  \if developper
  \endif
*/
template <class T> void FIELD<T>::init ()
{
}

/*!
  Copy constructor.
*/
template <class T> FIELD<T>::FIELD(const FIELD & m):
  FIELD_((FIELD_) m)
{
  if (m._value != NULL)
    {
      // copie only default !
      _value = new MEDARRAY<T>::MEDARRAY(* m._value,false);
    }
  else
    _value = (MEDARRAY<T> *) NULL;
  //_drivers = m._drivers;
}

/*!
  
*/
template <class T> FIELD<T> & FIELD<T>::operator=(const FIELD &m)
{
}

/*!
  Constructor.
*/
template <class T> FIELD<T>::FIELD(const SUPPORT * Support,
				   driverTypes driverType,
				   const string & fileName/*=""*/,
				   const string & fieldDriverName/*=""*/)
{
  const char * LOC = "template <class T> FIELD<T>::FIELD(const SUPPORT * Support, driverTypes driverType, const string & fileName=\"\", const string & fieldName=\"\") : ";

  int current;

  BEGIN_OF(LOC);

  init();

  _support = Support;
  _value = (MEDARRAY<T>*)NULL;

  MED_FIELD_RDONLY_DRIVER<T> myDriver(fileName,this);
  myDriver.setFieldName(fieldDriverName);
  current = addDriver(myDriver);
//   current = addDriver(driverType,fileName,fieldDriverName);
//   switch(_drivers[current]->getAccessMode() ) {
//   case MED_WRONLY : {
//     MESSAGE("FIELD<T>::FIELD(driverTypes driverType, .....) : driverType must have a MED_RDONLY or MED_RDWR accessMode");
//     rmDriver(current);
//     break;}
//   default : {
//   }
//   }
  _drivers[current]->open();
  _drivers[current]->read();
  _drivers[current]->close();

  END_OF(LOC);
}

/*!
  Destructor.
*/
template <class T> FIELD<T>::~FIELD()
{
  BEGIN_OF(" Destructeur FIELD<T>::~FIELD()");
  if (_value) delete _value;
  END_OF(" Destructeur FIELD<T>::~FIELD()");
}

/*!
  
*/
template <class T> void FIELD<T>::allocValue(const int NumberOfComponents)
{
  const char* LOC = "FIELD<T>::allocValue(const int NumberOfComponents)" ;
  BEGIN_OF(LOC);

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
    _numberOfValues = _support->getNumberOfElements(MED_ALL_ELEMENTS);
    MESSAGE(LOC <<" : "<<_numberOfValues <<" et "<< NumberOfComponents);

    _value = new MEDARRAY<T>::MEDARRAY(_numberOfComponents,_numberOfValues);

    _isRead = true ;
  }
  catch (MEDEXCEPTION &ex) {
    MESSAGE("No value defined, problem with NumberOfComponents (and may be _support) size of MEDARRAY<T>::_value !");
    _value = (MEDARRAY<T>*)NULL ;
  }

  SCRUTE(_value);
  END_OF("void FIELD<T>::allocValue(const int NumberOfComponents)");
}

/*!
  
*/
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
  _numberOfValues = LengthValue ;
  _value = new MEDARRAY<T>::MEDARRAY(_numberOfComponents,_numberOfValues);
  _isRead = true ;

  SCRUTE(_value);
  END_OF("void FIELD<T>::allocValue(const int NumberOfComponents,const int LengthValue)");
}

/*!
  
*/
template <class T> void FIELD<T>::deallocValue()
{
  BEGIN_OF("void FIELD<T>::deallocValue()");
  _numberOfValues = 0 ;
  _numberOfComponents = 0 ;
  if (_value != NULL)
    delete _value;

  END_OF("void FIELD<T>::deallocValue()");
}

// -----------------
// Methodes Inline
// -----------------


template <class T>       FIELD<T>::INSTANCE_DE<MED_FIELD_RDWR_DRIVER<T> >     FIELD<T>::inst_med  ;
template <class T> const FIELD<T>::INSTANCE * const FIELD<T>::instances[] = { &FIELD<T>::inst_med } ;


/*!
  Create the specified driver and return its index reference to path to 
  read or write methods.
*/
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


/*!
  Duplicate the given driver and return its index reference to path to 
  read or write methods.
*/
template <class T> inline int FIELD<T>::addDriver (GENDRIVER & driver )
{
  const char * LOC = "FIELD<T>::addDriver(GENDRIVER &) : ";
  BEGIN_OF(LOC);

  // duplicate driver to delete it with destructor !
  GENDRIVER * newDriver = driver.copy() ;

  _drivers.push_back(newDriver);
  return _drivers.size() -1 ;

  END_OF(LOC);
};

/*!
  Remove the driver referenced by its index.
*/
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

/*!
  Read FIELD in the file specified in the driver given by its index.
*/
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

/*!
  Write FIELD in the file specified in the driver given by its index.
*/
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

/*!
  \internal
  Write FIELD with the driver which is equal to the given driver.

  Use by MED object.
*/
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

/*!
  \internal
  Read FIELD with the driver which is equal to the given driver.

  Use by MED object.
*/
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

/*!
  \if developper
  Destroy the MEDARRAY<T> in FIELD and put the new one without copy.
  \endif
*/
template <class T> inline void FIELD<T>::setValue(MEDARRAY<T> *Value)
{
  if (NULL != _value) delete _value ;
  _value=Value ;
}

/*!
  \if developper
  Return a reference to  the MEDARRAY<T> in FIELD.
  \endif  
*/
template <class T> inline MEDARRAY<T>* FIELD<T>::getvalue() const
{
  return _value ;
}

/*!
  Return a reference to values array to read them.
*/
template <class T> inline const T* FIELD<T>::getValue(medModeSwitch Mode) const
{
  return _value->get(Mode) ;
}

/*!
  Return a reference to i^{th} row or column - component - (depend on Mode value)
  of FIELD values array.
*/
template <class T> inline const T* FIELD<T>::getValueI(medModeSwitch Mode,int i) const
{
 if ( Mode == MED_FULL_INTERLACE )
 {
 	 return _value->getRow(i) ;
 }
 ASSERT (  Mode == MED_NO_INTERLACE);
 return _value->getColumn(i);
}

/*!
  Return the value of i^{th} element and j^{th} component.
*/
template <class T> inline T FIELD<T>::getValueIJ(int i,int j) const
{
  return _value->getIJ(i,j) ;
}

/*!
  Copy new values array in FIELD according to the given mode.

  Array must have right size. If not results are unpredicable.
*/
template <class T> inline void FIELD<T>::setValue(medModeSwitch mode, T* value)
{
  _value->set(mode,value);
}

/*!
  Update values array in FIELD with the given ones according to specified mode.
*/
template <class T> inline void FIELD<T>::setValueI(medModeSwitch mode, int i, T* value)
{
  // PROVISOIRE :
  if (MED_FULL_INTERLACE == mode)
    _value->setI(i,value);
  else if (MED_NO_INTERLACE == mode)
    _value->setJ(i,value);
  else
    throw MEDEXCEPTION(LOCALIZED("FIELD<T>::setValueI : bad medModeSwitch")) ;
}

/*!
  Set the value of i^{th} element and j^{th} component with the given one.
*/
template <class T> inline void FIELD<T>::setValueIJ(int i, int j, T value)
{
  _value->setIJ(i,j,value);
}

/*
  METHODS
*/

/*!
  Fill values array with volume values.
*/
template <class T> void FIELD<T>::getVolume() const throw (MEDEXCEPTION)
{
  const char * LOC = "FIELD<double>::getVolume() const : ";
  BEGIN_OF(LOC);

  // The field has to be initilised by a non empty support and a
  // number of components = 1 and its value type has to be set to MED_REEL64
  // (ie a FIELD<double>)

  if ((_support == (SUPPORT *) NULL) || (_numberOfComponents != 1) || (_valueType != MED_REEL64))
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"The field has to be initialised with a non empty support, a number of components set to 1 and a value type set to MED_REEL64"));

  END_OF(LOC);
}

/*!
  Fill values array with area values.
*/
template <class T> void FIELD<T>::getArea() const throw (MEDEXCEPTION)
{
  const char * LOC = "FIELD<double>::getArea() const : ";
  BEGIN_OF(LOC);

  // The field has to be initilised by a non empty support and a
  // number of components = 1 and its value type has to be set to MED_REEL64
  // (ie a FIELD<double>)

  if ((_support == (SUPPORT *) NULL) || (_numberOfComponents != 1) || (_valueType != MED_REEL64))
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"The field has to be initialised with a non empty support, a number of components set to 1 and a value type set to MED_REEL64"));

  END_OF(LOC);
}

/*!
  Fill values array with length values.
*/
template <class T> void FIELD<T>::getLength() const throw (MEDEXCEPTION)
{
  const char * LOC = "FIELD<double>::getLength() const : ";
  BEGIN_OF(LOC);

  // The field has to be initilised by a non empty support and a
  // number of components = 1 and its value type has to be set to MED_REEL64
  // (ie a FIELD<double>)

  if ((_support == (SUPPORT *) NULL) || (_numberOfComponents != 1) || (_valueType != MED_REEL64))
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"The field has to be initialised with a non empty support, a number of components set to 1 and a value type set to MED_REEL64"));

  END_OF(LOC);
}

/*!
  Fill values array with normal values.
*/
template <class T> void FIELD<T>::getNormal() const throw (MEDEXCEPTION)
{
  const char * LOC = "FIELD<double>::getNormal() const : ";
  BEGIN_OF(LOC);

  // The field has to be initilised by a non empty support and a
  // number of components = 1 and its value type has to be set to MED_REEL64
  // (ie a FIELD<double>)

  if (_support == (SUPPORT *) NULL)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"The field has to be initialised with a non empty support, a number of components set to the space dimension and a value type set to MED_REEL64"));

  int dim_space = _support->getMesh()->getSpaceDimension();

  if ((_numberOfComponents != dim_space) || (_valueType != MED_REEL64))
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"The field has to be initialised with a non empty support, a number of components set to the space dimension and a value type set to MED_REEL64"));

  END_OF(LOC);
}

/*!
  Fill values array with barycenter values.
*/
template <class T> void FIELD<T>::getBarycenter() const throw (MEDEXCEPTION)
{
  const char * LOC = "FIELD<double>::getBarycenter() const : ";
  BEGIN_OF(LOC);

  // The field has to be initilised by a non empty support and a number of
  //components = space dimension and its value type has to be set to MED_REEL64
  // (ie a FIELD<double>)

  if (_support == (SUPPORT *) NULL)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"The field has to be initialised with a non empty support, a number of components set to the space dimension and a value type set to MED_REEL64"));

  int dim_space = _support->getMesh()->getSpaceDimension();

  if ((_numberOfComponents != dim_space) || (_valueType != MED_REEL64))
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"The field has to be initialised with a non empty support, a number of components set to the space dimension and a value type set to MED_REEL64"));

  END_OF(LOC);
}

#endif /* FIELD_HXX */
