/*
 File Field.hxx
 $Header$
*/

#ifndef FIELD_HXX
#define FIELD_HXX

#include <vector>
#include <algorithm>
#include <cmath>

#include "utilities.h"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

#include "MEDMEM_Support.hxx"
#include "MEDMEM_Unit.hxx"
#include "MEDMEM_Array.hxx"
#include "MEDMEM_GenDriver.hxx"

#include "MEDMEM_MedFieldDriver.hxx"
#include "MEDMEM_MedMedDriver.hxx"

#include "MEDMEM_VtkFieldDriver.hxx"
#include "MEDMEM_VtkMedDriver.hxx"

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

  // _valueType should be a static const. Here is an initialization exemple 
  // template < classType T > struct SET_VALUE_TYPE { static const med_type_champ _valueType = 0; }
  // template < > struct SET_VALUE_TYPE<double> { static const med_type_champ _valueType = MED_EN::MED_REEL64; }
  // template < > struct SET_VALUE_TYPE<int> { static const med_type_champ _valueType = MED_EN::MED_INT32; }
  // static const med_type_champ _valueType = SET_VALUE_TYPE <T>::_valueType;
  med_type_champ _valueType ;

  vector<GENDRIVER *> _drivers; // Storage of the drivers currently in use
  static void _checkFieldCompatibility(const FIELD_& m, const FIELD_& n ) throw (MEDEXCEPTION);
  void _checkNormCompatibility(const FIELD<double>* p_field_volume=NULL) const  throw (MEDEXCEPTION);
  FIELD<double>* _getFieldSize() const;

public:

  friend class MED_MED_RDONLY_DRIVER;
  friend class MED_MED_WRONLY_DRIVER;
  friend class MED_MED_RDWR_DRIVER;

  friend class VTK_MED_DRIVER;

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
  virtual ~FIELD_();

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
  virtual void openAppend( void );
  virtual  void     write(const GENDRIVER &);
  virtual  void     write(int index=0, const string & driverName="");

  virtual  void     writeAppend(const GENDRIVER &);
  virtual  void     writeAppend(int index=0, const string & driverName="");

  inline void     setName(const string Name);
  inline string   getName() const;
  inline void     setDescription(const string Description);
  inline string   getDescription() const;
  inline const SUPPORT * getSupport() const;
  inline void     setSupport(const SUPPORT * support);
  inline void     setNumberOfComponents(const int NumberOfComponents);
  inline int      getNumberOfComponents() const;
  inline void     setNumberOfValues(const int NumberOfValues);
  inline int      getNumberOfValues() const;
  //    inline void     setComponentType(int *ComponentType);
  //    inline int *    getComponentType() const;
  //    inline int      getComponentTypeI(int i) const;
  inline void     setComponentsNames(const string * ComponentsNames);
  inline void     setComponentName(int i, const string ComponentName);
  inline const string * getComponentsNames() const;
  inline string   getComponentName(int i) const;
  inline void     setComponentsDescriptions(const string * ComponentsDescriptions);
  inline void     setComponentDescription(int i, const string ComponentDescription);
  inline const string * getComponentsDescriptions() const;
  inline string   getComponentDescription(int i) const;

  // provisoire : en attendant de regler le probleme des unites !
  inline void     setComponentsUnits(const UNIT * ComponentsUnits);
  inline const UNIT *   getComponentsUnits() const;
  inline const UNIT *   getComponentUnit(int i) const;
  inline void     setMEDComponentsUnits(const string * MEDComponentsUnits);
  inline void     setMEDComponentUnit(int i, const string MEDComponentUnit);
  inline const string * getMEDComponentsUnits() const;
  inline string   getMEDComponentUnit(int i) const;

  inline void     setIterationNumber(int IterationNumber);
  inline int      getIterationNumber() const;
  inline void     setTime(double Time);
  inline double   getTime() const;
  inline void     setOrderNumber(int OrderNumber);
  inline int      getOrderNumber() const;

  inline void     setValueType (const med_type_champ ValueType) ;
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
inline void FIELD_::setName(const string Name)
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
inline void FIELD_::setDescription(const string Description)
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
inline void FIELD_::setComponentsNames(const string * ComponentsNames)
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
inline void FIELD_::setComponentName(int i, const string ComponentName)
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
inline void FIELD_::setComponentsDescriptions(const string * ComponentsDescriptions)
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
inline void FIELD_::setComponentDescription(int i,const string ComponentDescription)
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
inline void FIELD_::setComponentsUnits(const UNIT * ComponentsUnits)
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
inline void FIELD_::setMEDComponentsUnits(const string * MEDComponentsUnits)
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
inline void FIELD_::setMEDComponentUnit(int i, const string MEDComponentUnit)
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
inline void FIELD_::setSupport(const SUPPORT * support)
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
inline void FIELD_::setValueType (const med_type_champ ValueType)
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
  static INSTANCE_DE<VTK_FIELD_DRIVER<T> >   inst_vtk ;

  static const INSTANCE * const instances[] ;

  // ------ End of Drivers Management Part

  // array of value of type T
  MEDARRAY<T> *_value ;

private:
  void _operation(const FIELD& m,const FIELD& n, const medModeSwitch mode, char* Op);
  void _operationInitialize(const FIELD& m,const FIELD& n, char* Op);
  void _add_in_place(const FIELD& m,const FIELD& n, const medModeSwitch mode);
  void _sub_in_place(const FIELD& m,const FIELD& n, const medModeSwitch mode);
  void _mul_in_place(const FIELD& m,const FIELD& n, const medModeSwitch mode);
  void _div_in_place(const FIELD& m,const FIELD& n, const medModeSwitch mode);
  //setValueType() ;

public:
  FIELD();
  FIELD(const FIELD &m);
  FIELD & operator=(const FIELD &m); 	// A FAIRE
  FIELD(const SUPPORT * Support, const int NumberOfComponents, const medModeSwitch Mode=MED_FULL_INTERLACE)  throw (MEDEXCEPTION) ; // Ajout NB Constructeur FIELD avec allocation de memoire de tous ses attribut
  FIELD(const SUPPORT * Support, driverTypes driverType,
	const string & fileName="", const string & fieldName="",
	const int iterationNumber = -1, const int orderNumber = -1)
    throw (MEDEXCEPTION);
  ~FIELD();

  const FIELD operator+(const FIELD& m) const;
  const FIELD operator-(const FIELD& m) const;
  const FIELD operator*(const FIELD& m) const;
  const FIELD operator/(const FIELD& m) const;
  const FIELD operator-() const;
  FIELD& operator+=(const FIELD& m);
  FIELD& operator-=(const FIELD& m);
  FIELD& operator*=(const FIELD& m);
  FIELD& operator/=(const FIELD& m);
  static FIELD* add(const FIELD& m, const FIELD& n);
  static FIELD* sub(const FIELD& m, const FIELD& n);
  static FIELD* mul(const FIELD& m, const FIELD& n);
  static FIELD* div(const FIELD& m, const FIELD& n);
  double normMax() const throw (MEDEXCEPTION);
  double norm2() const throw (MEDEXCEPTION);
  void   applyLin(T a, T b);
  template <T T_function(T)> void applyFunc();
  static FIELD* scalarProduct(const FIELD& m, const FIELD& n);
  double normL2(int component, const FIELD<double> * p_field_volume=NULL) const;
  double normL2(const FIELD<double> * p_field_volume=NULL) const;
  double normL1(int component, const FIELD<double> * p_field_volume=NULL) const;
  double normL1(const FIELD<double> * p_field_volume=NULL) const;

  friend class MED_FIELD_RDONLY_DRIVER<T>;
  friend class MED_FIELD_WRONLY_DRIVER<T>;

  friend class VTK_FIELD_DRIVER<T>;
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

  inline void writeAppend(int index=0, const string & driverName = "");
  inline void writeAppend(const GENDRIVER &);

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
  Constructor with no parameter, most of the attribut members are set to NULL.
*/
template <class T>  FIELD<T>::FIELD():
 _value((MEDARRAY<T>*)NULL)
{
  MESSAGE("Constructeur FIELD sans parametre");
}

/*!
  Constructor with parameters such that all attrribut are set but the _value
  attrribut is allocated but not set.
*/
template <class T>  FIELD<T>::FIELD(const SUPPORT * Support,
				    const int NumberOfComponents, const medModeSwitch Mode) throw (MEDEXCEPTION) :
  FIELD_(Support, NumberOfComponents)
{
  BEGIN_OF("FIELD<T>::FIELD(const SUPPORT * Support, const int NumberOfComponents, const medModeSwitch Mode)");
  SCRUTE(this);

  try {
    _numberOfValues = Support->getNumberOfElements(MED_ALL_ELEMENTS);
  }
  catch (MEDEXCEPTION &ex) {
    MESSAGE("No value defined ! ("<<ex.what()<<")");
    _value = (MEDARRAY<T>*)NULL ;
  }
  MESSAGE("FIELD : constructeur : "<< _numberOfValues <<" et "<< NumberOfComponents);
  if (0<_numberOfValues) {
    _value = new MEDARRAY<T>(_numberOfComponents,_numberOfValues,Mode);
    _isRead = true ;
  } else
    _value = (MEDARRAY<T>*)NULL ;

  END_OF("FIELD<T>::FIELD(const SUPPORT * Support, const int NumberOfComponents, const medModeSwitch Mode)");
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
  MESSAGE("Constructeur FIELD de recopie");
  if (m._value != NULL)
    {
      // copie only default !
      _value = new MEDARRAY<T>(* m._value,false);
    }
  else
    _value = (MEDARRAY<T> *) NULL;
  //_drivers = m._drivers;
}

/*!
  
*/
template <class T> FIELD<T> & FIELD<T>::operator=(const FIELD &m)
{
  MESSAGE("Appel de FIELD<T>::operator=");
}

/*!
     Overload addition operator.
     This operation is authorized only for compatible fields that have the same support.
     The compatibility checking includes equality tests of the folowing data members:/n
     - _support
     - _numberOfComponents
     - _numberOfValues
     - _componentsTypes
     - _MEDComponentsUnits.

     The data members of the returned field are initialized, based on the first field, except for the name, 
     which is the combination of the two field's names and the operator.
     Advised Utilisation in C++ : <tt> FIELD<T> c = a + b; </tt> /n
     In this case, the (recent) compilators perform optimisation and don't call the copy constructor.
     When using python, this operator calls the copy constructor in any case.
     The user has to be aware that when using operator + in associatives expressions like
     <tt> a = b + c + d +e; </tt> /n
     no optimisation is performed : the evaluation of last expression requires the construction of
     3 temporary fields.
*/
template <class T>
const FIELD<T> FIELD<T>::operator+(const FIELD & m) const
{
    BEGIN_OF("FIELD<T>::operator+(const FIELD & m)");
    FIELD_::_checkFieldCompatibility(*this, m); // may throw exception

    // Select mode : avoid if possible any calculation of other mode for fields m or *this
    medModeSwitch mode;
    if(this->getvalue()->getMode()==m.getvalue()->getMode() || this->getvalue()->isOtherCalculated())
	mode=m.getvalue()->getMode();
    else
	mode=this->getvalue()->getMode();
    
    // Creation of the result - memory is allocated by FIELD constructor
    FIELD<T> result(this->getSupport(),this->getNumberOfComponents(),mode);
    //result._operation(*this,m,mode,"+"); // perform Atribute's initialization & addition
    result._operationInitialize(*this,m,"+"); // perform Atribute's initialization
    result._add_in_place(*this,m,mode); // perform addition

    END_OF("FIELD<T>::operator+(const FIELD & m)");
    return result;
}

/*!  Overloaded Operator +=
 *   Operations are directly performed in the first field's array.
 *   This operation is authorized only for compatible fields that have the same support.
 */
template <class T>
FIELD<T>& FIELD<T>::operator+=(const FIELD & m)
{
    BEGIN_OF("FIELD<T>::operator+=(const FIELD & m)");
    FIELD_::_checkFieldCompatibility(*this, m); // may throw exception

    // We choose to keep *this mode, even if it may cost a re-calculation for m
    medModeSwitch mode=this->getvalue()->getMode();
    const T* value1=m.getValue(mode); // get pointers to the values we are adding

    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue(mode));
    const int size=getNumberOfValues()*getNumberOfComponents(); // size of array
    const T* endV=value+size; // pointer to the end of value
    for(;value!=endV; value1++,value++)
	*value += *value1;
    // if it exists, the alternate mode needs to be cleared because it is not up-to-date anymore
    this->getvalue()->clearOtherMode();
    END_OF("FIELD<T>::operator+=(const FIELD & m)");
    return *this;
}


/*! Addition of fields. Static member function.
 *  The function return a pointer to a new created field that holds the addition.
 *  Data members are checked for compatibility and initialized.
 *  The user is in charge of memory deallocation.
 */
template <class T>
FIELD<T>* FIELD<T>::add(const FIELD& m, const FIELD& n)
{
    BEGIN_OF("FIELD<T>::add(const FIELD & m, const FIELD& n)");
    FIELD_::_checkFieldCompatibility(m, n); // may throw exception

    // Select mode : avoid if possible any calculation of other mode for fields m or *this
    medModeSwitch mode;
    if(m.getvalue()->getMode()==n.getvalue()->getMode() || n.getvalue()->isOtherCalculated())
	mode=m.getvalue()->getMode();
    else
	mode=n.getvalue()->getMode();
    
    // Creation of a new field
    FIELD<T>* result = new FIELD<T>(m.getSupport(),m.getNumberOfComponents(),mode);
    result->_operationInitialize(m,n,"+"); // perform Atribute's initialization
    result->_add_in_place(m,n,mode); // perform addition

    END_OF("FIELD<T>::add(const FIELD & m, const FIELD& n)");
    return result;
}

/*!
     Overload substraction operator.
     This operation is authorized only for compatible fields that have the same support.
     The compatibility checking includes equality tests of the folowing data members:/n
     - _support
     - _numberOfComponents
     - _numberOfValues
     - _componentsTypes
     - _MEDComponentsUnits.

     The data members of the returned field are initialized, based on the first field, except for the name, 
     which is the combination of the two field's names and the operator.
     Advised Utilisation in C++ : <tt> FIELD<T> c = a - b; </tt> /n
     In this case, the (recent) compilators perform optimisation and don't call the copy constructor.
     When using python, this operator calls the copy constructor in any case.
     The user has to be aware that when using operator - in associatives expressions like
     <tt> a = b - c - d -e; </tt> /n
     no optimisation is performed : the evaluation of last expression requires the construction of
     3 temporary fields.
*/
template <class T>
const FIELD<T> FIELD<T>::operator-(const FIELD & m) const
{
    BEGIN_OF("FIELD<T>::operator-(const FIELD & m)");
    FIELD_::_checkFieldCompatibility(*this, m); // may throw exception

    // Select mode : avoid if possible any calculation of other mode for fields m or *this
    medModeSwitch mode;
    if(this->getvalue()->getMode()==m.getvalue()->getMode() || this->getvalue()->isOtherCalculated())
	mode=m.getvalue()->getMode();
    else
	mode=this->getvalue()->getMode();
    
    // Creation of the result - memory is allocated by FIELD constructor
    FIELD<T> result(this->getSupport(),this->getNumberOfComponents(),mode);
    //result._operation(*this,m,mode,"-"); // perform Atribute's initialization & substraction
    result._operationInitialize(*this,m,"-"); // perform Atribute's initialization
    result._sub_in_place(*this,m,mode); // perform substracion

    END_OF("FIELD<T>::operator-(const FIELD & m)");
    return result;
}

template <class T>
const FIELD<T> FIELD<T>::operator-() const
{
    BEGIN_OF("FIELD<T>::operator-()");

    medModeSwitch mode=this->getvalue()->getMode();
    // Creation of the result - memory is allocated by FIELD constructor
    FIELD<T> result(this->getSupport(),this->getNumberOfComponents(),mode);
    // Atribute's initialization 
    result.setName("- "+getName());
    result.setComponentsNames(getComponentsNames());
    // not yet implemented    setComponentType(getComponentType());
    result.setComponentsDescriptions(getComponentsDescriptions());
    result.setMEDComponentsUnits(getMEDComponentsUnits());
    result.setComponentsUnits(getComponentsUnits());
    result.setIterationNumber(getIterationNumber());
    result.setTime(getTime());
    result.setOrderNumber(getOrderNumber());
    result.setValueType(getValueType());

    const T* value1=getValue(mode); 
    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (result.getValue(mode));
    const int size=getNumberOfValues()*getNumberOfComponents(); // size of array
    const T* endV=value+size; // pointer to the end of value

    for(;value!=endV; value1++,value++)
	*value = -(*value1);
    END_OF("FIELD<T>::operator-=(const FIELD & m)");
    return result;
}

/*!  Overloaded Operator -=
 *   Operations are directly performed in the first field's array.
 *   This operation is authorized only for compatible fields that have the same support.
 */
template <class T>
FIELD<T>& FIELD<T>::operator-=(const FIELD & m)
{
    BEGIN_OF("FIELD<T>::operator-=(const FIELD & m)");
    FIELD_::_checkFieldCompatibility(*this, m); // may throw exception

    // We choose to keep *this mode, even if it may cost a re-calculation for m
    medModeSwitch mode=this->getvalue()->getMode();
    const T* value1=m.getValue(mode); // get pointers to the values we are adding

    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue(mode));
    const int size=getNumberOfValues()*getNumberOfComponents(); // size of array
    const T* endV=value+size; // pointer to the end of value

    for(;value!=endV; value1++,value++)
	*value -= *value1;
    // if it exists, the alternate mode needs to be cleared because it is not up-to-date anymore
    this->getvalue()->clearOtherMode();
    END_OF("FIELD<T>::operator-=(const FIELD & m)");
    return *this;
}


/*! Substraction of fields. Static member function.
 *  The function return a pointer to a new created field that holds the substraction.
 *  Data members are checked for compatibility and initialized.
 *  The user is in charge of memory deallocation.
 */
template <class T>
FIELD<T>* FIELD<T>::sub(const FIELD& m, const FIELD& n)
{
    BEGIN_OF("FIELD<T>::sub(const FIELD & m, const FIELD& n)");
    FIELD_::_checkFieldCompatibility(m, n); // may throw exception

    // Select mode : avoid if possible any calculation of other mode for fields m or *this
    medModeSwitch mode;
    if(m.getvalue()->getMode()==n.getvalue()->getMode() || n.getvalue()->isOtherCalculated())
	mode=m.getvalue()->getMode();
    else
	mode=n.getvalue()->getMode();
    
    // Creation of a new field
    FIELD<T>* result = new FIELD<T>(m.getSupport(),m.getNumberOfComponents(),mode);
    result->_operationInitialize(m,n,"-"); // perform Atribute's initialization
    result->_sub_in_place(m,n,mode); // perform substraction

    END_OF("FIELD<T>::sub(const FIELD & m, const FIELD& n)");
    return result;
}

/*!
     Overload multiplication operator.
     This operation is authorized only for compatible fields that have the same support.
     The compatibility checking includes equality tests of the folowing data members:/n
     - _support
     - _numberOfComponents
     - _numberOfValues
     - _componentsTypes
     - _MEDComponentsUnits.

     The data members of the returned field are initialized, based on the first field, except for the name, 
     which is the combination of the two field's names and the operator.
     Advised Utilisation in C++ : <tt> FIELD<T> c = a * b; </tt> /n
     In this case, the (recent) compilators perform optimisation and don't call the copy constructor.
     When using python, this operator calls the copy constructor in any case.
     The user has to be aware that when using operator * in associatives expressions like
     <tt> a = b * c * d *e; </tt> /n
     no optimisation is performed : the evaluation of last expression requires the construction of
     3 temporary fields.
*/
template <class T>
const FIELD<T> FIELD<T>::operator*(const FIELD & m) const
{
    BEGIN_OF("FIELD<T>::operator*(const FIELD & m)");
    FIELD_::_checkFieldCompatibility(*this, m); // may throw exception

    // Select mode : avoid if possible any calculation of other mode for fields m or *this
    medModeSwitch mode;
    if(this->getvalue()->getMode()==m.getvalue()->getMode() || this->getvalue()->isOtherCalculated())
	mode=m.getvalue()->getMode();
    else
	mode=this->getvalue()->getMode();
    
    // Creation of the result - memory is allocated by FIELD constructor
    FIELD<T> result(this->getSupport(),this->getNumberOfComponents(),mode);
    //result._operation(*this,m,mode,"*"); // perform Atribute's initialization & multiplication
    result._operationInitialize(*this,m,"*"); // perform Atribute's initialization
    result._mul_in_place(*this,m,mode); // perform multiplication

    END_OF("FIELD<T>::operator*(const FIELD & m)");
    return result;
}

/*!  Overloaded Operator *=
 *   Operations are directly performed in the first field's array.
 *   This operation is authorized only for compatible fields that have the same support.
 */
template <class T>
FIELD<T>& FIELD<T>::operator*=(const FIELD & m)
{
    BEGIN_OF("FIELD<T>::operator*=(const FIELD & m)");
    FIELD_::_checkFieldCompatibility(*this, m); // may throw exception

    // We choose to keep *this mode, even if it may cost a re-calculation for m
    medModeSwitch mode=this->getvalue()->getMode();
    const T* value1=m.getValue(mode); // get pointers to the values we are adding

    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue(mode));
    const int size=getNumberOfValues()*getNumberOfComponents(); // size of array
    const T* endV=value+size; // pointer to the end of value

    for(;value!=endV; value1++,value++)
	*value *= *value1;
    // if it exists, the alternate mode needs to be cleared because it is not up-to-date anymore
    this->getvalue()->clearOtherMode();
    END_OF("FIELD<T>::operator*=(const FIELD & m)");
    return *this;
}


/*! Multiplication of fields. Static member function.
 *  The function return a pointer to a new created field that holds the multiplication.
 *  Data members are checked for compatibility and initialized.
 *  The user is in charge of memory deallocation.
 */
template <class T>
FIELD<T>* FIELD<T>::mul(const FIELD& m, const FIELD& n)
{
    BEGIN_OF("FIELD<T>::mul(const FIELD & m, const FIELD& n)");
    FIELD_::_checkFieldCompatibility(m, n); // may throw exception

    // Select mode : avoid if possible any calculation of other mode for fields m or *this
    medModeSwitch mode;
    if(m.getvalue()->getMode()==n.getvalue()->getMode() || n.getvalue()->isOtherCalculated())
	mode=m.getvalue()->getMode();
    else
	mode=n.getvalue()->getMode();
    
    // Creation of a new field
    FIELD<T>* result = new FIELD<T>(m.getSupport(),m.getNumberOfComponents(),mode);
    result->_operationInitialize(m,n,"*"); // perform Atribute's initialization
    result->_mul_in_place(m,n,mode); // perform multiplication

    END_OF("FIELD<T>::mul(const FIELD & m, const FIELD& n)");
    return result;
}


/*!
     Overload division operator.
     This operation is authorized only for compatible fields that have the same support.
     The compatibility checking includes equality tests of the folowing data members:/n
     - _support
     - _numberOfComponents
     - _numberOfValues
     - _componentsTypes
     - _MEDComponentsUnits.

     The data members of the returned field are initialized, based on the first field, except for the name, 
     which is the combination of the two field's names and the operator.
     Advised Utilisation in C++ : <tt> FIELD<T> c = a / b; </tt> /n
     In this case, the (recent) compilators perform optimisation and don't call the copy constructor.
     When using python, this operator calls the copy constructor in any case.
     The user has to be aware that when using operator / in associatives expressions like
     <tt> a = b / c / d /e; </tt> /n
     no optimisation is performed : the evaluation of last expression requires the construction of
     3 temporary fields.
*/
template <class T>
const FIELD<T> FIELD<T>::operator/(const FIELD & m) const
{
    BEGIN_OF("FIELD<T>::operator/(const FIELD & m)");
    FIELD_::_checkFieldCompatibility(*this, m); // may throw exception

    // Select mode : avoid if possible any calculation of other mode for fields m or *this
    medModeSwitch mode;
    if(this->getvalue()->getMode()==m.getvalue()->getMode() || this->getvalue()->isOtherCalculated())
	mode=m.getvalue()->getMode();
    else
	mode=this->getvalue()->getMode();
    
    // Creation of the result - memory is allocated by FIELD constructor
    FIELD<T> result(this->getSupport(),this->getNumberOfComponents(),mode);
    //result._operation(*this,m,mode,"/"); // perform Atribute's initialization & division
    result._operationInitialize(*this,m,"/"); // perform Atribute's initialization
    result._div_in_place(*this,m,mode); // perform division

    END_OF("FIELD<T>::operator/(const FIELD & m)");
    return result;
}


/*!  Overloaded Operator /=
 *   Operations are directly performed in the first field's array.
 *   This operation is authorized only for compatible fields that have the same support.
 */
template <class T>
FIELD<T>& FIELD<T>::operator/=(const FIELD & m)
{
    BEGIN_OF("FIELD<T>::operator/=(const FIELD & m)");
    FIELD_::_checkFieldCompatibility(*this, m); // may throw exception

    // We choose to keep *this mode, even if it may cost a re-calculation for m
    medModeSwitch mode=this->getvalue()->getMode();
    const T* value1=m.getValue(mode); // get pointers to the values we are adding

    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue(mode));
    const int size=getNumberOfValues()*getNumberOfComponents(); // size of array
    const T* endV=value+size; // pointer to the end of value

    for(;value!=endV; value1++,value++)
	*value /= *value1;
    // if it exists, the alternate mode needs to be cleared because it is not up-to-date anymore
    this->getvalue()->clearOtherMode();
    END_OF("FIELD<T>::operator/=(const FIELD & m)");
    return *this;
}


/*! Division of fields. Static member function.
 *  The function return a pointer to a new created field that holds the division.
 *  Data members are checked for compatibility and initialized.
 *  The user is in charge of memory deallocation.
 */
template <class T>
FIELD<T>* FIELD<T>::div(const FIELD& m, const FIELD& n)
{
    BEGIN_OF("FIELD<T>::div(const FIELD & m, const FIELD& n)");
    FIELD_::_checkFieldCompatibility(m, n); // may throw exception

    // Select mode : avoid if possible any calculation of other mode for fields m or *this
    medModeSwitch mode;
    if(m.getvalue()->getMode()==n.getvalue()->getMode() || n.getvalue()->isOtherCalculated())
	mode=m.getvalue()->getMode();
    else
	mode=n.getvalue()->getMode();
    
    // Creation of a new field
    FIELD<T>* result = new FIELD<T>(m.getSupport(),m.getNumberOfComponents(),mode);
    result->_operationInitialize(m,n,"/"); // perform Atribute's initialization
    result->_div_in_place(m,n,mode); // perform division

    END_OF("FIELD<T>::div(const FIELD & m, const FIELD& n)");
    return result;
}


/*!
  \if developper
  This internal method initialize the members of a new field created to hold the result of the operation Op .
  Initialization is based on the first field, except for the name, which is the combination of the two field's names
  and the operator.
  \endif
*/
template <class T>
void FIELD<T>::_operationInitialize(const FIELD& m,const FIELD& n, char* Op)
{
    MESSAGE("Appel methode interne _add" << Op);

    // Atribute's initialization (copy of the first field's attributes)
    // Other data members (_support, _numberOfValues) are initialized in the field's constr.
    setName(m.getName()+" "+Op+" "+n.getName());
    setComponentsNames(m.getComponentsNames());
    // not yet implemented    setComponentType(m.getComponentType());
    setComponentsDescriptions(m.getComponentsDescriptions());
    setMEDComponentsUnits(m.getMEDComponentsUnits());

    // The following data member may differ from field m to n.
    // The initialization is done based on the first field.
    setComponentsUnits(m.getComponentsUnits());
    setIterationNumber(m.getIterationNumber());
    setTime(m.getTime());
    setOrderNumber(m.getOrderNumber());
    setValueType(m.getValueType());
}


/*!
  \if developper
  Internal method called by FIELD<T>::operator+ and FIELD<T>::add to perform addition "in place".
  This method is applied to a just created field with medModeSwitch mode.
  For this reason, the alternate mode doesn't need to be set to 0 after performing operation : 
  it doesn't exist!
  \endif
*/
template <class T>
void FIELD<T>::_add_in_place(const FIELD& m,const FIELD& n, const medModeSwitch mode)
{
    // get pointers to the values we are adding
    const T* value1=m.getValue(mode);
    const T* value2=n.getValue(mode);
    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue(mode));

    const int size=getNumberOfValues()*getNumberOfComponents();
    SCRUTE(size);
    const T* endV1=value1+size;
    for(;value1!=endV1; value1++,value2++,value++)
	*value=(*value1)+(*value2);
}

/*!
  \if developper
  Internal method called by FIELD<T>::operator- and FIELD<T>::sub to perform substraction "in place".
  This method is applied to a just created field with medModeSwitch mode.
  For this reason, the alternate mode doesn't need to be set to 0 after performing operation : 
  it doesn't exist!
  \endif
*/
template <class T>
void FIELD<T>::_sub_in_place(const FIELD& m,const FIELD& n, const medModeSwitch mode)
{
    // get pointers to the values we are adding
    const T* value1=m.getValue(mode);
    const T* value2=n.getValue(mode);
    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue(mode));

    const int size=getNumberOfValues()*getNumberOfComponents();
    SCRUTE(size);
    const T* endV1=value1+size;
    for(;value1!=endV1; value1++,value2++,value++)
	*value=(*value1)-(*value2);
}

/*!
  \if developper
  Internal method called by FIELD<T>::operator* and FIELD<T>::mul to perform multiplication "in place".
  This method is applied to a just created field with medModeSwitch mode.
  For this reason, the alternate mode doesn't need to be set to 0 after performing operation : 
  it doesn't exist!
  \endif
*/
template <class T>
void FIELD<T>::_mul_in_place(const FIELD& m,const FIELD& n, const medModeSwitch mode)
{
    // get pointers to the values we are adding
    const T* value1=m.getValue(mode);
    const T* value2=n.getValue(mode);
    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue(mode));

    const int size=getNumberOfValues()*getNumberOfComponents();
    SCRUTE(size);
    const T* endV1=value1+size;
    for(;value1!=endV1; value1++,value2++,value++)
	*value=(*value1)*(*value2);
}

/*!
  \if developper
  Internal method called by FIELD<T>::operator/ and FIELD<T>::div to perform division "in place".
  This method is applied to a just created field with medModeSwitch mode.
  For this reason, the alternate mode doesn't need to be set to 0 after performing operation : 
  it doesn't exist!
  \endif
*/
template <class T>
void FIELD<T>::_div_in_place(const FIELD& m,const FIELD& n, const medModeSwitch mode)
{
    // get pointers to the values we are adding
    const T* value1=m.getValue(mode);
    const T* value2=n.getValue(mode);
    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue(mode));

    const int size=getNumberOfValues()*getNumberOfComponents();
    SCRUTE(size);
    const T* endV1=value1+size;
    for(;value1!=endV1; value1++,value2++,value++)
	*value=(*value1)/(*value2);
}

/*!  Return Max Norm 
 */
template <class T> double FIELD<T>::normMax() const throw (MEDEXCEPTION)
{
    const T* value=getValue(getvalue()->getMode()); // get pointer to the values
    const int size=getNumberOfValues()*getNumberOfComponents();
    if (size <= 0) // Size of array has to be strictly positive
    {
	string diagnosis;
	diagnosis="FIELD<T>::normMax() : cannot compute the norm of "+getName()+
	    " : it size is non positive!";
	throw MEDEXCEPTION(diagnosis.c_str());
    }
    const T* lastvalue=value+size; // get pointer just after last value
    const T* pMax=value; // pointer to the max value
    const T* pMin=value; // pointer to the min value

    // get pointers to the max & min value of array
    while ( ++value != lastvalue )
    {
	if ( *pMin > *value )
	    pMin=value;
	if ( *pMax < *value )
	    pMax=value;
    }

    T Max= *pMax>(T) 0 ? *pMax : -*pMax; // Max=abs(*pMax)
    T Min= *pMin>(T) 0 ? *pMin : -*pMin; // Min=abs(*pMin)

    return Max>Min ? static_cast<double>(Max) : static_cast<double>(Min);
}

/*!  Return Euclidien norm 
 */
template <class T> double FIELD<T>::norm2() const throw (MEDEXCEPTION)
{
    const T* value=this->getValue(this->getvalue()->getMode()); // get const pointer to the values
    const int size=getNumberOfValues()*getNumberOfComponents(); // get size of array
    if (size <= 0) // Size of array has to be strictly positive
    {
	string diagnosis;
	diagnosis="FIELD<T>::norm2() : cannot compute the norm of "+getName()+
	    " : it size is non positive!";
	throw MEDEXCEPTION(diagnosis.c_str());
    }
    const T* lastvalue=value+size; // point just after last value
    
    T result((T)0); // init
    for( ; value!=lastvalue ; ++value)
	result += (*value) * (*value);
    
    return std::sqrt(static_cast<double> (result));
}


/*!  Apply to each (scalar) field component the template parameter T_function, 
 *   which is a pointer to function.
 *   Since the pointer is known at compile time, the function is inlined into the inner loop!
 *   calculation is done "in place".
 *   Use examples : 
 *   
 *   \code  myField.applyFunc<std::sqrt>();  // apply sqare root function \endcode
 *     \code myField.applyFunc<myFunction>(); // apply your own created function \endcode
 */
template <class T> template <T T_function(T)> 
void FIELD<T>::applyFunc()
{
    medModeSwitch mode=getvalue()->getMode();

    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue(mode));
    const int size=getNumberOfValues()*getNumberOfComponents(); // size of array

    if (size>0) // for a negative size, there is nothing to do
    {
	const T* lastvalue=value+size; // pointer to the end of value
	for(;value!=lastvalue; ++value) // apply linear transformation
	    *value = T_function(*value);
	// if it exists, the alternate mode needs to be cleared because it is not up-to-date anymore
	getvalue()->clearOtherMode();
    }
}
    
  
/*!  Apply to each (scalar) field component the linear function x -> ax+b.
 *   calculation is done "in place".
 */
template <class T> void FIELD<T>::applyLin(T a, T b)
{
    medModeSwitch mode=getvalue()->getMode();

    // get a non const pointer to the inside array of values and perform operation in place
    T * value=const_cast<T *> (getValue(mode));
    const int size=getNumberOfValues()*getNumberOfComponents(); // size of array

    if (size>0) // for a negative size, there is nothing to do
    {
	const T* lastvalue=value+size; // pointer to the end of value
	for(;value!=lastvalue; ++value) // apply linear transformation
	    *value = a*(*value)+b;
	// if it exists, the alternate mode needs to be cleared because it is not up-to-date anymore
	getvalue()->clearOtherMode();
    }
}


/*!
 *   Return a pointer to a new field that holds the scalar product. Static member function.
 *   This operation is authorized only for compatible fields that have the same support.
 *   The compatibility checking includes equality tests of the folowing data members:/n
 *   - _support
 *   - _numberOfComponents
 *   - _numberOfValues
 *   - _componentsTypes
 *   - _MEDComponentsUnits.
 *   Data members are initialized.
 *   The new field point to the same support and has one component.
 *   Each value of it is the scalar product of the two argument's fields.
 *   The user is in charge of memory deallocation.
 */
template <class T> FIELD<T>* FIELD<T>::scalarProduct(const FIELD & m, const FIELD & n)
{
    FIELD_::_checkFieldCompatibility( m, n); // may throw exception
    // we need a MED_FULL_INTERLACE representation of m & n to compute the scalar product
    const medModeSwitch mode=MED_FULL_INTERLACE; 

    const int numberOfElements=m.getNumberOfValues(); // strictly positive
    const int NumberOfComponents=m.getNumberOfComponents(); // strictly positive

    // Creation & init of a the result field on the same suppot, with one component
    FIELD<T>* result = new FIELD<T>(m.getSupport(),1,mode);
    result->setName( "scalarProduct ( " + m.getName() + " , " + n.getName() + " )" );
    result->setIterationNumber(m.getIterationNumber());
    result->setTime(m.getTime());
    result->setOrderNumber(m.getOrderNumber());
    result->setValueType(m.getValueType());

    const T* value1=m.getValue(mode); // get const pointer to the values
    const T* value2=n.getValue(mode); // get const pointer to the values
    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (result->getValue(mode));
    
    const T* lastvalue=value+numberOfElements; // pointing just after last value of result
    for ( ; value!=lastvalue ; ++value ) // loop on all elements
    {
	*value=(T)0; // initialize value
	const T* endofRow=value1+NumberOfComponents; // pointing just after end of row
	for ( ; value1 != endofRow; ++value1, ++value2) // computation of dot product
	    *value += (*value1) * (*value2);
    }
    return result;
}

/*!  Return L2 Norm  of the field's component.
 *   Cannot be applied to a field with a support on nodes.
 *   If the optional p_field_volume argument is furnished, the volume is not re-calculated.
 */
template <class T> double FIELD<T>::normL2(int component, const FIELD<double> * p_field_volume) const
{
    _checkNormCompatibility(p_field_volume); // may throw exception
    if ( component<1 || component>getNumberOfComponents() )
	throw MEDEXCEPTION(STRING("FIELD<T>::normL2() : The component argument should be between 1 and the number of components"));

    const FIELD<double> * p_field_size=p_field_volume;
    if(!p_field_volume) // if the user don't supply the volume
	p_field_size=_getFieldSize(); // we calculate the volume [PROVISOIRE, en attendant l'implémentation dans mesh]

    // get pointer to the element's volumes. MED_FULL_INTERLACE is the default mode for p_field_size
    const double* vol=p_field_size->getValue(MED_FULL_INTERLACE); 
    const T* value=getValueI( MED_NO_INTERLACE, component); // get pointer to the component's values
    const T* lastvalue=value+getNumberOfValues(); // pointing just after the end of column

    double integrale=0.0;
    double totVol=0.0;
    for (; value!=lastvalue ; ++value ,++vol) 
    {
	integrale += static_cast<double>((*value) * (*value)) * (*vol);
	totVol+=*vol;
    }

    if(!p_field_volume) // if the user didn't supply the volume
	delete p_field_size; // delete temporary volume field
    if( totVol <= 0)
	throw MEDEXCEPTION(STRING("cannot compute sobolev norm : volume is not positive!"));

    return integrale/totVol;
}

/*!  Return L2 Norm  of the field.
 *   Cannot be applied to a field with a support on nodes.
 *   If the optional p_field_volume argument is furnished, the volume is not re-calculated.
 */
template <class T> double FIELD<T>::normL2(const FIELD<double> * p_field_volume) const
{
    _checkNormCompatibility(p_field_volume); // may throw exception
    const FIELD<double> * p_field_size=p_field_volume;
    if(!p_field_volume) // if the user don't supply the volume
	p_field_size=_getFieldSize(); // we calculate the volume [PROVISOIRE, en attendant l'implémentation dans mesh]

    // get pointer to the element's volumes. MED_FULL_INTERLACE is the default mode for p_field_size
    const double* vol=p_field_size->getValue(MED_FULL_INTERLACE); 
    const double* lastvol=vol+getNumberOfValues(); // pointing just after the end of vol
    const T* value=getValue( MED_NO_INTERLACE); // get pointer to the field's values

    double totVol=0.0;
    const double* p_vol=vol;
    for (p_vol=vol; p_vol!=lastvol ; ++p_vol) // calculate total volume
	totVol+=*p_vol;
    
    double integrale=0.0;
    for (int i=1; i<=getNumberOfComponents(); ++i) // compute integral on all components
	for (p_vol=vol; p_vol!=lastvol ; ++value ,++p_vol) 
	    integrale += static_cast<double>((*value) * (*value)) * (*p_vol);

    if(!p_field_volume) // if the user didn't supply the volume
	delete p_field_size; // delete temporary volume field
    if( totVol <= 0)
	throw MEDEXCEPTION(STRING("cannot compute sobolev norm : volume is not positive!"));

    return integrale/totVol;
}

/*!  Return L1 Norm  of the field's component.
 *   Cannot be applied to a field with a support on nodes.
 *   If the optional p_field_volume argument is furnished, the volume is not re-calculated.
 */
template <class T> double FIELD<T>::normL1(int component, const FIELD<double> * p_field_volume) const
{
    _checkNormCompatibility(p_field_volume); // may throw exception
    if ( component<1 || component>getNumberOfComponents() )
	throw MEDEXCEPTION(STRING("FIELD<T>::normL2() : The component argument should be between 1 and the number of components"));

    const FIELD<double> * p_field_size=p_field_volume;
    if(!p_field_volume) // if the user don't supply the volume
	p_field_size=_getFieldSize(); // we calculate the volume [PROVISOIRE, en attendant l'implémentation dans mesh]

    // get pointer to the element's volumes. MED_FULL_INTERLACE is the default mode for p_field_size
    const double* vol=p_field_size->getValue(MED_FULL_INTERLACE); 
    const T* value=getValueI( MED_NO_INTERLACE, component); // get pointer to the component's values
    const T* lastvalue=value+getNumberOfValues(); // pointing just after the end of column

    double integrale=0.0;
    double totVol=0.0;
    for (; value!=lastvalue ; ++value ,++vol) 
    {
	integrale += std::abs( static_cast<double>(*value) ) * (*vol);
	totVol+=*vol;
    }

    if(!p_field_volume) // if the user didn't supply the volume
	delete p_field_size; // delete temporary volume field
    if( totVol <= 0)
	throw MEDEXCEPTION(STRING("cannot compute sobolev norm : volume is not positive!"));

    return integrale/totVol;
}

/*!  Return L1 Norm  of the field.
 *   Cannot be applied to a field with a support on nodes.
 *   If the optional p_field_volume argument is furnished, the volume is not re-calculated.
 */
template <class T> double FIELD<T>::normL1(const FIELD<double> * p_field_volume) const
{
    _checkNormCompatibility(p_field_volume); // may throw exception
    const FIELD<double> * p_field_size=p_field_volume;
    if(!p_field_volume) // if the user don't supply the volume
	p_field_size=_getFieldSize(); // we calculate the volume [PROVISOIRE, en attendant l'implémentation dans mesh]

    // get pointer to the element's volumes. MED_FULL_INTERLACE is the default mode for p_field_size
    const double* vol=p_field_size->getValue(MED_FULL_INTERLACE); 
    const double* lastvol=vol+getNumberOfValues(); // pointing just after the end of vol
    const T* value=getValue( MED_NO_INTERLACE); // get pointer to the field's values

    double totVol=0.0;
    const double* p_vol=vol;
    for (p_vol=vol; p_vol!=lastvol ; ++p_vol) // calculate total volume
	totVol+=*p_vol;
    
    double integrale=0.0;
    for (int i=1; i<=getNumberOfComponents(); ++i) // compute integral on all components
	for (p_vol=vol; p_vol!=lastvol ; ++value ,++p_vol) 
	    integrale += std::abs( static_cast<double>(*value) ) * (*p_vol);

    if(!p_field_volume) // if the user didn't supply the volume
	delete p_field_size; // delete temporary volume field
    if( totVol <= 0)
	throw MEDEXCEPTION(STRING("cannot compute sobolev norm : volume is not positive!"));

    return integrale/totVol;
}




/*!
  Constructor with parameters; the object is set via a file and its associated
  driver. For the moment only the MED_DRIVER is considered and if the last two
  argument (iterationNumber and orderNumber) are not set; their default value
  is -1. If the field fieldDriverName with the iteration number
  iterationNumber and the order number orderNumber does not exist in the file
  fieldDriverName; the constructor raises an exception.
*/
template <class T> FIELD<T>::FIELD(const SUPPORT * Support,
				   driverTypes driverType,
				   const string & fileName/*=""*/,
				   const string & fieldDriverName/*=""*/,
				   const int iterationNumber,
				   const int orderNumber)
  throw (MEDEXCEPTION)
{
  const char * LOC = "template <class T> FIELD<T>::FIELD(const SUPPORT * Support, driverTypes driverType, const string & fileName=\"\", const string & fieldName=\"\", const int iterationNumber=-1, const int orderNumber=-1) : ";

  int current;

  BEGIN_OF(LOC);

  init();

  _support = Support;
  _value = (MEDARRAY<T>*)NULL;

  _iterationNumber = iterationNumber;
  _time = 0.0;
  _orderNumber = orderNumber;

  switch(driverType)
    {
    case MED_DRIVER :
      {
	MED_FIELD_RDONLY_DRIVER<T> myDriver(fileName,this);
	myDriver.setFieldName(fieldDriverName);
	current = addDriver(myDriver);
	break;
      }
//   current = addDriver(driverType,fileName,fieldDriverName);
//   switch(_drivers[current]->getAccessMode() ) {
//   case MED_WRONLY : {
//     MESSAGE("FIELD<T>::FIELD(driverTypes driverType, .....) : driverType must have a MED_RDONLY or MED_RDWR accessMode");
//     rmDriver(current);
//     break;}
//   default : {
//   }
//   }
    default :
      {
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Driver type unknown : can't create driver!"));
	break;
      }
    }

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
  SCRUTE(this);
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

    _value = new MEDARRAY<T>(_numberOfComponents,_numberOfValues);

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
  _value = new MEDARRAY<T>(_numberOfComponents,_numberOfValues);
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

template <class T>       FIELD<T>::INSTANCE_DE<VTK_FIELD_DRIVER<T> >     FIELD<T>::inst_vtk  ;

template <class T> const typename FIELD<T>::INSTANCE * const FIELD<T>::instances[] = { &FIELD<T>::inst_med,  &FIELD<T>::inst_vtk} ;


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
  int itDriver = (int) NO_DRIVER;

  BEGIN_OF(LOC);

  SCRUTE(driverType);

  SCRUTE(instances[driverType]);

  switch(driverType)
    {
    case MED_DRIVER : {
      itDriver = (int) driverType ;
      break ;
    }

    case VTK_DRIVER : {
      itDriver = 1 ;
      break ;
    }

    case GIBI_DRIVER : {
      throw MED_EXCEPTION (LOCALIZED(STRING(LOC)<< "driverType other than MED_DRIVER and VTK_DRIVER has been specified to the method which is not allowed for the object FIELD"));
      break;
    }

    case NO_DRIVER : {
      throw MED_EXCEPTION (LOCALIZED(STRING(LOC)<< "driverType other than MED_DRIVER and VTK_DRIVER has been specified to the method which is not allowed for the object FIELD"));
      break;
    }
    }

  if (itDriver == ((int) NO_DRIVER))
    throw MED_EXCEPTION (LOCALIZED(STRING(LOC)<< "driverType other than MED_DRIVER and VTK_DRIVER has been specified to the method which is not allowed for the object FIELD"));

  driver = instances[itDriver]->run(fileName, this) ;

  _drivers.push_back(driver);

  current = _drivers.size()-1;

  _drivers[current]->setFieldName(driverName);

  END_OF(LOC);

  return current;
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
                                     << "The <index given is invalid, index must be between  0 and  |"
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
  Write FIELD in the file specified in the driver given by its index. Use this
  method for ASCII drivers (e.g. VTK_DRIVER)
*/
template <class T> inline void FIELD<T>::writeAppend(int index/*=0*/, const string & driverName /*= ""*/)
{
  const char * LOC = "FIELD<T>::write(int index=0, const string & driverName = \"\") : ";
  BEGIN_OF(LOC);

  if( _drivers[index] ) {
    _drivers[index]->openAppend();
    if (driverName != "") _drivers[index]->setFieldName(driverName);
    _drivers[index]->writeAppend();
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

  for (unsigned int index=0; index < _drivers.size(); index++ )
    if ( *_drivers[index] == genDriver ) {
      _drivers[index]->open();
      _drivers[index]->write();
      _drivers[index]->close();
    }

  END_OF(LOC);

}

/*!
  \internal
  Write FIELD with the driver which is equal to the given driver.

  Use by MED object. Use this method for ASCII drivers (e.g. VTK_DRIVER).
*/
template <class T> inline void FIELD<T>::writeAppend(const GENDRIVER & genDriver)
{
  const char * LOC = " FIELD<T>::write(const GENDRIVER &) : ";
  BEGIN_OF(LOC);

  for (unsigned int index=0; index < _drivers.size(); index++ )
    if ( *_drivers[index] == genDriver ) {
      _drivers[index]->openAppend();
      _drivers[index]->writeAppend();
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

  for (unsigned int index=0; index < _drivers.size(); index++ )
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
