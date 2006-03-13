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
/*
 File Field.hxx
 $Header$
*/

#ifndef FIELD_HXX
#define FIELD_HXX

#include <vector>
#include <map>
#include <algorithm>
#include <math.h>
#include <cmath>

#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Unit.hxx"
#include "MEDMEM_nArray.hxx"
#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_ArrayInterface.hxx"
#include "MEDMEM_SetInterlacingType.hxx"
#include "MEDMEM_FieldForward.hxx"
#include "MEDMEM_GaussLocalization.hxx"

/*!

  This class contains all the informations related with a template class FIELD :
  - Components descriptions
  - Time step description
  - Location of the values (a SUPPORT class)

*/

namespace MEDMEM {

  template < typename T > struct SET_VALUE_TYPE {
    static const MED_EN::med_type_champ _valueType = MED_EN::MED_UNDEFINED_TYPE;};
  template < > struct SET_VALUE_TYPE<double> {
    static const MED_EN::med_type_champ _valueType = MED_EN::MED_REEL64; };
  template < > struct SET_VALUE_TYPE<int> {
    static const MED_EN::med_type_champ _valueType = MED_EN::MED_INT32; };

class MEDMEM_EXPORT FIELD_    // GENERIC POINTER TO a template <class T, class INTERLACING_TAG> class FIELD
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
    doesn't take care of _numberOfComponents
    and number of Gauss points.
    \endif
  */
  int             _numberOfValues ;

  /*!
    \if developper
    Array of size _numberOfComponents. \n
    (constant, scalar, vector, tensor)\n
    We could use an array of integer to store
    numbers of values: \n
    - 1 for scalar,\n
    - space dimension for vector,\n
    - space dimension square for tensor.\n
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
  /*!
    \if developper
    Iteration number of the field.
    \endif
  */
  int             _iterationNumber ;
  /*!
    \if developper
    Time of the field.
    \endif
  */
  double   	  _time;
  /*!
    \if developper
    Order number of the field.
    \endif
  */
  int      	  _orderNumber ;
  /*!
    \if developper
    At the initialization step of the field using the constructors; this attribute,
    the value type (integer or real) , is set automatically. There is a get method
    but not a set method for this attribute.
    \endif
  */
  MED_EN::med_type_champ _valueType ;
  /*!
    \if developper
    At the initialization step of the field using the constructors; this attribute,
    the interlacing type (full interlace or no interlace field value storage), is set
    automatically. There is a get method but not a set method for this attribute.
    \endif
  */
  MED_EN::medModeSwitch _interlacingType;

  vector<GENDRIVER *> _drivers; // Storage of the drivers currently in use
  static void _checkFieldCompatibility(const FIELD_& m, const FIELD_& n, bool checkUnit=true) throw (MEDEXCEPTION);
  static void _deepCheckFieldCompatibility(const FIELD_& m, const FIELD_& n, bool checkUnit=true) throw (MEDEXCEPTION);
  void _checkNormCompatibility(const FIELD<double>* p_field_volume=NULL) const  throw (MEDEXCEPTION);
  FIELD<double>* _getFieldSize() const;

public:

  friend class MED_MED_RDONLY_DRIVER21;
  friend class MED_MED_WRONLY_DRIVER21;
  friend class MED_MED_RDWR_DRIVER21;
  friend class MED_MED_RDONLY_DRIVER22;
  friend class MED_MED_WRONLY_DRIVER22;
  friend class MED_MED_RDWR_DRIVER22;
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

 FIELD_& operator=(const FIELD_ &m);

  virtual  void     rmDriver(int index=0);
  virtual   int     addDriver(driverTypes driverType,
                              const string & fileName="Default File Name.med",
			      const string & driverFieldName="Default Field Nam",
			      MED_EN::med_mode_acces access=MED_EN::MED_REMP) ;

  virtual  int      addDriver( GENDRIVER & driver);
  virtual  void     read (const GENDRIVER &);
  virtual  void     read(int index=0);
  virtual  void     openAppend( void );
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

  inline MED_EN::med_type_champ getValueType () const;
  inline MED_EN::medModeSwitch  getInterlacingType() const;
  virtual inline bool getGaussPresence() const throw (MEDEXCEPTION);
protected:
  void copyGlobalInfo(const FIELD_& m);
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
inline void FIELD_::setNumberOfComponents(const int NumberOfComponents)
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
inline void FIELD_::setNumberOfValues(const int NumberOfValues)
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
  //A.G. Addings for RC
  if(_support)
    _support->removeReference();
  _support = support ;
  if(_support)
    _support->addReference();
}
/*!
  Get the FIELD med value type (MED_INT32 or MED_REEL64).
*/
inline MED_EN::med_type_champ FIELD_::getValueType () const
{
  return _valueType ;
}

/*!
  Get the FIELD med interlacing type (MED_FULL_INTERLACE or MED_NO_INTERLACE).
*/
  inline MED_EN::medModeSwitch FIELD_::getInterlacingType () const
{
  return _interlacingType ;
}

/*!
  Get the FIELD gauss presence.
*/
  inline bool  FIELD_::getGaussPresence() const throw (MEDEXCEPTION)
{
  const char * LOC = "FIELD_::getGaussPresence() : ";
  throw MEDEXCEPTION(STRING(LOC) << " This FIELD_ doesn't rely on a FIELD<T>" );
}

} //End namespace MEDMEM

/////////////////////////
// END OF CLASS FIELD_ //
/////////////////////////

/*!

  This template class contains informations related with a FIELD :
  - Values of the field, their type (real or integer), the storage mode (full interlace or
    no interlace).

*/

namespace MEDMEM {

  template<class T2> class MED_FIELD_RDONLY_DRIVER21;
  template<class T2> class MED_FIELD_WRONLY_DRIVER21;
  template<class T2> class MED_FIELD_RDONLY_DRIVER22;
  template<class T2> class MED_FIELD_WRONLY_DRIVER22;
  template<class T2> class VTK_FIELD_DRIVER;

  template <class T,
	    class INTERLACING_TAG
	    > class FIELD : public FIELD_
{
protected:

  typedef typename MEDMEM_ArrayInterface<T,INTERLACING_TAG,NoGauss>::Array ArrayNoGauss;
  typedef typename MEDMEM_ArrayInterface<T,INTERLACING_TAG,Gauss>::Array   ArrayGauss;
  typedef typename MEDMEM_ArrayInterface<T,NoInterlace,NoGauss>::Array     ArrayNo;
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,NoGauss>::Array   ArrayFull;
  typedef MEDMEM_Array_ Array;
  typedef T ElementType;
  typedef INTERLACING_TAG InterlacingTag;
  typedef map<MED_EN::medGeometryElement,GAUSS_LOCALIZATION_*> locMap;

  // array of value of type T
  Array *_value ;

  map<MED_EN::medGeometryElement,GAUSS_LOCALIZATION_*> _gaussModel; //A changer quand les drivers seront template de l'entrelacement

  static T _scalarForPow;
  static T pow(T x);

private:
  void _operation(const FIELD& m,const FIELD& n, char* Op);
  void _operationInitialize(const FIELD& m,const FIELD& n, char* Op);
  void _add_in_place(const FIELD& m,const FIELD& n);
  void _sub_in_place(const FIELD& m,const FIELD& n);
  void _mul_in_place(const FIELD& m,const FIELD& n);
  void _div_in_place(const FIELD& m,const FIELD& n) throw (MEDEXCEPTION);
  //setValueType() ;

public:
  FIELD();
  FIELD(const FIELD &m);
  FIELD(const SUPPORT * Support, const int NumberOfComponents)  throw (MEDEXCEPTION) ;
  FIELD( driverTypes driverType,
	 const string & fileName, const string & fieldDriverName,
	 const int iterationNumber=-1, const int orderNumber=-1)
    throw (MEDEXCEPTION);
  FIELD(const SUPPORT * Support, driverTypes driverType,
	const string & fileName="", const string & fieldName="",
	const int iterationNumber = -1, const int orderNumber = -1)
    throw (MEDEXCEPTION);
  ~FIELD();

  FIELD & operator=(const FIELD &m);

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
  static FIELD* addDeep(const FIELD& m, const FIELD& n);
  static FIELD* sub(const FIELD& m, const FIELD& n);
  static FIELD* subDeep(const FIELD& m, const FIELD& n);
  static FIELD* mul(const FIELD& m, const FIELD& n);
  static FIELD* mulDeep(const FIELD& m, const FIELD& n);
  static FIELD* div(const FIELD& m, const FIELD& n);
  static FIELD* divDeep(const FIELD& m, const FIELD& n);
  double normMax() const throw (MEDEXCEPTION);
  double norm2() const throw (MEDEXCEPTION);
  void   applyLin(T a, T b);
  template <T T_function(T)> void applyFunc();
  void applyPow(T scalar);
  static FIELD* scalarProduct(const FIELD& m, const FIELD& n, bool deepCheck=false);
  double normL2(int component, const FIELD<double,FullInterlace> * p_field_volume=NULL) const;
  double normL2(const FIELD<double,FullInterlace> * p_field_volume=NULL) const;
  double normL1(int component, const FIELD<double,FullInterlace> * p_field_volume=NULL) const;
  double normL1(const FIELD<double,FullInterlace> * p_field_volume=NULL) const;
  FIELD* extract(const SUPPORT *subSupport) const throw (MEDEXCEPTION);

  friend class MED_FIELD_RDONLY_DRIVER21<T>;
  friend class MED_FIELD_WRONLY_DRIVER21<T>;
  friend class MED_FIELD_RDONLY_DRIVER22<T>;
  friend class MED_FIELD_WRONLY_DRIVER22<T>;
  friend class VTK_FIELD_DRIVER<T>;
  //friend class MED_FIELD_RDWR_DRIVER  <T>;

  void init ();
  void rmDriver(int index=0);
  int  addDriver(driverTypes driverType,
		 const string & fileName="Default File Name.med",
		 const string & driverFieldName="Default Field Name",
		 MED_EN::med_mode_acces access=MED_EN::MED_REMP) ;

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

  inline MEDMEM_Array_  * getArray()        const throw (MEDEXCEPTION);
  inline ArrayGauss     * getArrayGauss()   const throw (MEDEXCEPTION);
  inline ArrayNoGauss   * getArrayNoGauss() const throw (MEDEXCEPTION);
  inline bool            getGaussPresence() const throw (MEDEXCEPTION);

  inline int          getValueLength() const throw (MEDEXCEPTION);
  inline const T*     getValue()       const throw (MEDEXCEPTION);
  inline const T*     getRow(int i)    const throw (MEDEXCEPTION);
  inline const T*     getColumn(int j) const throw (MEDEXCEPTION);
  inline T            getValueIJ(int i,int j) const throw (MEDEXCEPTION);
  inline T            getValueIJK(int i,int j,int k) const throw (MEDEXCEPTION);

  bool                getValueOnElement(int eltIdInSup,T* retValues) const throw (MEDEXCEPTION);

  const int   getNumberOfGeometricTypes() const throw (MEDEXCEPTION);
  const GAUSS_LOCALIZATION<INTERLACING_TAG> & getGaussLocalization(MED_EN::medGeometryElement geomElement) const throw (MEDEXCEPTION);
  const int * getNumberOfGaussPoints() const throw (MEDEXCEPTION);
  const int   getNumberOfGaussPoints( MED_EN::medGeometryElement geomElement) const throw (MEDEXCEPTION);
  const int   getNbGaussI(int i)          const throw (MEDEXCEPTION);
  const int * getNumberOfElements()       const throw (MEDEXCEPTION);
  const MED_EN::medGeometryElement  * getGeometricTypes()  const throw (MEDEXCEPTION);
  bool        isOnAllElements()           const throw (MEDEXCEPTION);
 
  inline void setArray(MEDMEM_Array_ *value) throw (MEDEXCEPTION);
  inline void setValue( T* value) throw (MEDEXCEPTION);
  inline void setRow( int i, T* value) throw (MEDEXCEPTION);
  inline void setColumn( int i, T* value) throw (MEDEXCEPTION);
  inline void setValueIJ(int i, int j, T value) throw (MEDEXCEPTION);

  /*!
    This fonction feeds the FIELD<double> private attributs _value with the
    volume of each cells belonging to the argument Support. The field has to be
    initialised via the constructor FIELD<double>(const SUPPORT * , const int )
    with Support as SUPPORT argument, 1 has the number of components, and Support
    has be a SUPPORT on 3D cells. This initialisation could be done by the empty
    constructor followed by a setSupport and setNumberOfComponents call.
   */
  void getVolume() const throw (MEDEXCEPTION) ;
  /*!
    This fonction feeds the FIELD<double> private attributs _value with the
    area of each cells (or faces) belonging to the attribut _support. The field
    has to be initialised via the constructor FIELD<double>(const SUPPORT * ,
    const int ) with 1 has the number of components, and _support has be a
    SUPPORT on 2D cells or 3D faces. This initialisation could be done by the
    empty constructor followed by a setSupport and setNumberOfComponents call.
   */
  void getArea() const throw (MEDEXCEPTION) ;
  /*!
    This fonction feeds the FIELD<double> private attributs _value with the
    length of each segments belonging to the attribut _support. The field has
    to be initialised via the constructor FIELD<double>(const SUPPORT * ,
    const int ) with 1 has the number of components, and _support has be a
    SUPPORT on 3D edges or 2D faces. This initialisation could be done by the
    empty constructor followed by a setSupport and setNumberOfComponents call.
   */
  void getLength() const throw (MEDEXCEPTION) ;
  /*!
    This fonction feeds the FIELD<double> private attributs _value with the
    normal vector of each faces belonging to the attribut _support. The field
    has to be initialised via the constructor FIELD<double>(const SUPPORT * ,
    const int ) with the space dimension has the number of components, and
    _support has be a SUPPORT on 3D or 2D faces. This initialisation could be done
    by the empty constructor followed by a setSupport and setNumberOfComponents
    call.
   */
  void getNormal() const throw (MEDEXCEPTION) ;
  /*!
    This fonction feeds the FIELD<double> private attributs _value with the
    barycenter of each faces or cells or edges belonging to the attribut _support.
    The field has to be initialised via the constructor
    FIELD<double>(const SUPPORT * ,const int ) with the space dimension has the
    number of components, and _support has be a SUPPORT on 3D cells or 2D faces.
    This initialisation could be done by the empty constructor followed by a
    setSupport and setNumberOfComponents call.
   */
  void getBarycenter() const throw (MEDEXCEPTION) ;

  typedef void (*myFuncType)(const double *,T*);
  void fillFromAnalytic(myFuncType f) throw (MEDEXCEPTION);
};
}

#include "MEDMEM_DriverFactory.hxx"

namespace MEDMEM {

template <class T,class INTERLACING_TAG> T FIELD<T, INTERLACING_TAG>::_scalarForPow=1;

// --------------------
// Implemented Methods
// --------------------

/*!
  Constructor with no parameter, most of the attribut members are set to NULL.
*/
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>::FIELD():FIELD_()
{
  MESSAGE("Constructeur FIELD sans parametre");

  //INITIALISATION DE _valueType DS LE CONSTRUCTEUR DE FIELD_
  ASSERT(FIELD_::_valueType == MED_EN::MED_UNDEFINED_TYPE);
  FIELD_::_valueType=SET_VALUE_TYPE<T>::_valueType;

  //INITIALISATION DE _interlacingType DS LE CONSTRUCTEUR DE FIELD_
  ASSERT(FIELD_::_interlacingType == MED_EN::MED_UNDEFINED_INTERLACE);
  FIELD_::_interlacingType=SET_INTERLACING_TYPE<INTERLACING_TAG>::_interlacingType;

  _value = ( ArrayNoGauss * ) NULL;
}

/*!
  Constructor with parameters such that all attrribut are set but the _value
  attribut is allocated but not set.
*/
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>::FIELD(const SUPPORT * Support,
				    const int NumberOfComponents) throw (MEDEXCEPTION) :
  FIELD_(Support, NumberOfComponents),_value(NULL)
{
  BEGIN_OF("FIELD<T>::FIELD(const SUPPORT * Support, const int NumberOfComponents)");
  SCRUTE(this);

  //INITIALISATION DE _valueType DS LE CONSTRUCTEUR DE FIELD_
  ASSERT(FIELD_::_valueType == MED_EN::MED_UNDEFINED_TYPE)
  FIELD_::_valueType=SET_VALUE_TYPE<T>::_valueType;

  //INITIALISATION DE _interlacingType DS LE CONSTRUCTEUR DE FIELD_
  ASSERT(FIELD_::_interlacingType == MED_EN::MED_UNDEFINED_INTERLACE)
  FIELD_::_interlacingType=SET_INTERLACING_TYPE<INTERLACING_TAG>::_interlacingType;

  try {
    // becarefull about the numbre of gauss point
    _numberOfValues = Support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS);
  }
#ifdef _DEBUG_
  catch (MEDEXCEPTION &ex) {
#else
  catch (MEDEXCEPTION ) {
#endif
    MESSAGE("No value defined ! ("<<ex.what()<<")");
  }
  MESSAGE("FIELD : constructeur : "<< _numberOfValues <<" et "<< NumberOfComponents);
  if (0<_numberOfValues) {
    _value = new ArrayNoGauss (_numberOfComponents,_numberOfValues);
    _isRead = true ;
  }

  END_OF("FIELD<T>::FIELD(const SUPPORT * Support, const int NumberOfComponents)");
}

/*!
  \if developper
  \endif
*/
template <class T, class INTERLACING_TAG> void FIELD<T, INTERLACING_TAG>::init ()
{
}

/*!
  Copy constructor.
*/
template <class T, class INTERLACING_TAG> FIELD<T, INTERLACING_TAG>::FIELD(const FIELD & m):
  FIELD_((FIELD_) m)
{
  MESSAGE("Constructeur FIELD de recopie");

  // RECOPIE PROFONDE <> de l'operateur= Rmq from EF
  if (m._value != NULL)
    {
      if ( m.getGaussPresence() )
	_value = new ArrayGauss( *(dynamic_cast< ArrayGauss * > (m._value) ) ,false);
      else
	_value = new ArrayNoGauss( *(dynamic_cast< ArrayNoGauss * > (m._value)) ,false);
    }
  else
    _value = (ArrayNoGauss *) NULL;
  locMap::const_iterator it;
  for ( it = m._gaussModel.begin();it != m._gaussModel.end(); it++ )
    _gaussModel[dynamic_cast<const GAUSS_LOCALIZATION<INTERLACING_TAG> * > ((*it).second)->getType()]=
      new GAUSS_LOCALIZATION<INTERLACING_TAG>(
					      *dynamic_cast<const GAUSS_LOCALIZATION<INTERLACING_TAG> * > ( (*it).second )
					      );

  _valueType       = m._valueType;
  _interlacingType = m._interlacingType;
  //drivers = m._drivers;
}

/*!
  \if developper
  Not implemented.
  \endif
*/
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG> & FIELD<T, INTERLACING_TAG>::operator=(const FIELD &m)
{
  MESSAGE("Appel de FIELD<T>::operator=") ;
  if ( this == &m) return *this;

  // copy values array
  FIELD_::operator=(m);  // Driver are ignored & ?copie su pointeur de Support?

  _value           = m._value; //PROBLEME RECOPIE DES POINTEURS PAS COHERENT AVEC LE
                               //CONSTRUCTEUR PAR RECOPIE
                               //CF :Commentaire dans MEDMEM_Array
  locMap::const_iterator it;
  for ( it = m._gaussModel.begin();it != m._gaussModel.end(); it++ )
    _gaussModel[dynamic_cast<const GAUSS_LOCALIZATION<INTERLACING_TAG> * > ((*it).second)->getType()]=
      new GAUSS_LOCALIZATION<INTERLACING_TAG>(
					      *dynamic_cast<const GAUSS_LOCALIZATION<INTERLACING_TAG> * > ( (*it).second )
					      );

  _valueType       = m._valueType;
  _interlacingType = m._interlacingType;

  return *this;
}

/*!
     Overload addition operator.
     This operation is authorized only for compatible fields that have the same support.
     The compatibility checking includes equality tests of the folowing data members:\n
     - _support
     - _numberOfComponents
     - _numberOfValues
     - _componentsTypes
     - _MEDComponentsUnits.

     The data members of the returned field are initialized, based on the first field, except for the name,
     which is the combination of the two field's names and the operator.
     Advised Utilisation in C++ : <tt> FIELD<T> c = a + b; </tt> \n
     In this case, the (recent) compilators perform optimisation and don't call the copy constructor.
     When using python, this operator calls the copy constructor in any case.
     The user has to be aware that when using operator + in associatives expressions like
     <tt> a = b + c + d +e; </tt> \n
     no optimisation is performed : the evaluation of last expression requires the construction of
     3 temporary fields.
*/
template <class T, class INTERLACING_TAG>
const FIELD<T, INTERLACING_TAG> FIELD<T, INTERLACING_TAG>::operator+(const FIELD & m) const
{
    BEGIN_OF("FIELD<T>::operator+(const FIELD & m)");
    FIELD_::_checkFieldCompatibility(*this, m); // may throw exception

    // Creation of the result - memory is allocated by FIELD constructor
    FIELD<T, INTERLACING_TAG> result(this->getSupport(),this->getNumberOfComponents());
    //result._operation(*this,m,mode,"+"); // perform Atribute's initialization & addition
    result._operationInitialize(*this,m,"+"); // perform Atribute's initialization
    result._add_in_place(*this,m); // perform addition

    END_OF("FIELD<T>::operator+(const FIELD & m)");
    return result;
}

/*!  Overloaded Operator +=
 *   Operations are directly performed in the first field's array.
 *   This operation is authorized only for compatible fields that have the same support.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>& FIELD<T, INTERLACING_TAG>::operator+=(const FIELD & m)
{
    BEGIN_OF("FIELD<T>::operator+=(const FIELD & m)");
    FIELD_::_checkFieldCompatibility(*this, m); // may throw exception

    const T* value1=m.getValue(); // get pointers to the values we are adding

    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue());
    const int size=getNumberOfValues()*getNumberOfComponents(); // size of array
    const T* endV=value+size; // pointer to the end of value
    for(;value!=endV; value1++,value++)
	*value += *value1;
    END_OF("FIELD<T>::operator+=(const FIELD & m)");
    return *this;
}


/*! Addition of fields. Static member function.
 *  The function return a pointer to a new created field that holds the addition.
 *  Data members are checked for compatibility and initialized.
 *  The user is in charge of memory deallocation.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>* FIELD<T, INTERLACING_TAG>::add(const FIELD& m, const FIELD& n)
{
    BEGIN_OF("FIELD<T>::add(const FIELD & m, const FIELD& n)");
    FIELD_::_checkFieldCompatibility(m, n); // may throw exception

    // Creation of a new field
    FIELD<T, INTERLACING_TAG>* result = new FIELD<T, INTERLACING_TAG>(m.getSupport(),
								      m.getNumberOfComponents());
    result->_operationInitialize(m,n,"+"); // perform Atribute's initialization
    result->_add_in_place(m,n); // perform addition

    END_OF("FIELD<T>::add(const FIELD & m, const FIELD& n)");
    return result;
}

/*! Same as add method except that field check is deeper.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>* FIELD<T, INTERLACING_TAG>::addDeep(const FIELD& m, const FIELD& n)
{
    BEGIN_OF("FIELD<T>::addDeep(const FIELD & m, const FIELD& n)");
    FIELD_::_deepCheckFieldCompatibility(m, n); // may throw exception

    // Creation of a new field
    FIELD<T, INTERLACING_TAG>* result = new FIELD<T, INTERLACING_TAG>(m.getSupport(),
								      m.getNumberOfComponents());
    result->_operationInitialize(m,n,"+"); // perform Atribute's initialization
    result->_add_in_place(m,n); // perform addition

    END_OF("FIELD<T>::addDeep(const FIELD & m, const FIELD& n)");
    return result;
}

/*!
     Overload substraction operator.
     This operation is authorized only for compatible fields that have the same support.
     The compatibility checking includes equality tests of the folowing data members:\n
     - _support
     - _numberOfComponents
     - _numberOfValues
     - _componentsTypes
     - _MEDComponentsUnits.

     The data members of the returned field are initialized, based on the first field, except for the name,
     which is the combination of the two field's names and the operator.
     Advised Utilisation in C++ : <tt> FIELD<T> c = a - b; </tt> \n
     In this case, the (recent) compilators perform optimisation and don't call the copy constructor.
     When using python, this operator calls the copy constructor in any case.
     The user has to be aware that when using operator - in associatives expressions like
     <tt> a = b - c - d -e; </tt> \n
     no optimisation is performed : the evaluation of last expression requires the construction of
     3 temporary fields.
*/
template <class T, class INTERLACING_TAG>
const FIELD<T, INTERLACING_TAG> FIELD<T, INTERLACING_TAG>::operator-(const FIELD & m) const
{
    BEGIN_OF("FIELD<T>::operator-(const FIELD & m)");
    FIELD_::_checkFieldCompatibility(*this, m); // may throw exception

    // Creation of the result - memory is allocated by FIELD constructor
    FIELD<T, INTERLACING_TAG> result(this->getSupport(),this->getNumberOfComponents());
    //result._operation(*this,m,mode,"-"); // perform Atribute's initialization & substraction
    result._operationInitialize(*this,m,"-"); // perform Atribute's initialization
    result._sub_in_place(*this,m); // perform substracion

    END_OF("FIELD<T>::operator-(const FIELD & m)");
    return result;
}

template <class T, class INTERLACING_TAG>
const FIELD<T, INTERLACING_TAG> FIELD<T, INTERLACING_TAG>::operator-() const
{
    BEGIN_OF("FIELD<T>::operator-()");

    // Creation of the result - memory is allocated by FIELD constructor
    FIELD<T, INTERLACING_TAG> result(this->getSupport(),this->getNumberOfComponents());
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

    const T* value1=getValue();
    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (result.getValue());
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
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>& FIELD<T, INTERLACING_TAG>::operator-=(const FIELD & m)
{
    BEGIN_OF("FIELD<T>::operator-=(const FIELD & m)");
    FIELD_::_checkFieldCompatibility(*this, m); // may throw exception

    const T* value1=m.getValue();

    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue());
    const int size=getNumberOfValues()*getNumberOfComponents(); // size of array
    const T* endV=value+size; // pointer to the end of value

    for(;value!=endV; value1++,value++)
	*value -= *value1;

    END_OF("FIELD<T>::operator-=(const FIELD & m)");
    return *this;
}


/*! Substraction of fields. Static member function.
 *  The function return a pointer to a new created field that holds the substraction.
 *  Data members are checked for compatibility and initialized.
 *  The user is in charge of memory deallocation.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>* FIELD<T, INTERLACING_TAG>::sub(const FIELD& m, const FIELD& n)
{
    BEGIN_OF("FIELD<T>::sub(const FIELD & m, const FIELD& n)");
    FIELD_::_checkFieldCompatibility(m, n); // may throw exception

    // Creation of a new field
    FIELD<T, INTERLACING_TAG>* result = new FIELD<T, INTERLACING_TAG>(m.getSupport(),
								      m.getNumberOfComponents());
    result->_operationInitialize(m,n,"-"); // perform Atribute's initialization
    result->_sub_in_place(m,n); // perform substraction

    END_OF("FIELD<T>::sub(const FIELD & m, const FIELD& n)");
    return result;
}

/*! Same as sub method except that field check is deeper.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>* FIELD<T, INTERLACING_TAG>::subDeep(const FIELD& m, const FIELD& n)
{
    BEGIN_OF("FIELD<T>::subDeep(const FIELD & m, const FIELD& n)");
    FIELD_::_deepCheckFieldCompatibility(m, n); // may throw exception

    // Creation of a new field
    FIELD<T, INTERLACING_TAG>* result = new FIELD<T, INTERLACING_TAG>(m.getSupport(),
								      m.getNumberOfComponents());
    result->_operationInitialize(m,n,"-"); // perform Atribute's initialization
    result->_sub_in_place(m,n); // perform substraction

    END_OF("FIELD<T>::subDeep(const FIELD & m, const FIELD& n)");
    return result;
}

/*!
     Overload multiplication operator.
     This operation is authorized only for compatible fields that have the same support.
     The compatibility checking includes equality tests of the folowing data members:\n
     - _support
     - _numberOfComponents
     - _numberOfValues
     - _componentsTypes
     - _MEDComponentsUnits.

     The data members of the returned field are initialized, based on the first field, except for the name,
     which is the combination of the two field's names and the operator.
     Advised Utilisation in C++ : <tt> FIELD<T> c = a * b; </tt> \n
     In this case, the (recent) compilators perform optimisation and don't call the copy constructor.
     When using python, this operator calls the copy constructor in any case.
     The user has to be aware that when using operator * in associatives expressions like
     <tt> a = b * c * d *e; </tt> \n
     no optimisation is performed : the evaluation of last expression requires the construction of
     3 temporary fields.
*/
template <class T, class INTERLACING_TAG>
const FIELD<T, INTERLACING_TAG> FIELD<T, INTERLACING_TAG>::operator*(const FIELD & m) const
{
    BEGIN_OF("FIELD<T>::operator*(const FIELD & m)");
    FIELD_::_checkFieldCompatibility(*this, m, false); // may throw exception

    // Creation of the result - memory is allocated by FIELD constructor
    FIELD<T, INTERLACING_TAG> result(this->getSupport(),this->getNumberOfComponents());
    //result._operation(*this,m,mode,"*"); // perform Atribute's initialization & multiplication
    result._operationInitialize(*this,m,"*"); // perform Atribute's initialization
    result._mul_in_place(*this,m); // perform multiplication

    END_OF("FIELD<T>::operator*(const FIELD & m)");
    return result;
}

/*!  Overloaded Operator *=
 *   Operations are directly performed in the first field's array.
 *   This operation is authorized only for compatible fields that have the same support.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>& FIELD<T, INTERLACING_TAG>::operator*=(const FIELD & m)
{
    BEGIN_OF("FIELD<T>::operator*=(const FIELD & m)");
    FIELD_::_checkFieldCompatibility(*this, m, false); // may throw exception

    const T* value1=m.getValue();

    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue());
    const int size=getNumberOfValues()*getNumberOfComponents(); // size of array
    const T* endV=value+size; // pointer to the end of value

    for(;value!=endV; value1++,value++)
	*value *= *value1;

    END_OF("FIELD<T>::operator*=(const FIELD & m)");
    return *this;
}


/*! Multiplication of fields. Static member function.
 *  The function return a pointer to a new created field that holds the multiplication.
 *  Data members are checked for compatibility and initialized.
 *  The user is in charge of memory deallocation.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>* FIELD<T, INTERLACING_TAG>::mul(const FIELD& m, const FIELD& n)
{
    BEGIN_OF("FIELD<T>::mul(const FIELD & m, const FIELD& n)");
    FIELD_::_checkFieldCompatibility(m, n, false); // may throw exception

    // Creation of a new field
    FIELD<T, INTERLACING_TAG>* result = new FIELD<T, INTERLACING_TAG>(m.getSupport(),
								      m.getNumberOfComponents());
    result->_operationInitialize(m,n,"*"); // perform Atribute's initialization
    result->_mul_in_place(m,n); // perform multiplication

    END_OF("FIELD<T>::mul(const FIELD & m, const FIELD& n)");
    return result;
}

/*! Same as mul method except that field check is deeper.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>* FIELD<T, INTERLACING_TAG>::mulDeep(const FIELD& m, const FIELD& n)
{
    BEGIN_OF("FIELD<T>::mulDeep(const FIELD & m, const FIELD& n)");
    FIELD_::_deepCheckFieldCompatibility(m, n, false); // may throw exception

    // Creation of a new field
    FIELD<T, INTERLACING_TAG>* result = new FIELD<T,INTERLACING_TAG>(m.getSupport(),
								     m.getNumberOfComponents());
    result->_operationInitialize(m,n,"*"); // perform Atribute's initialization
    result->_mul_in_place(m,n); // perform multiplication

    END_OF("FIELD<T>::mulDeep(const FIELD & m, const FIELD& n)");
    return result;
}

/*!
     Overload division operator.
     This operation is authorized only for compatible fields that have the same support.
     The compatibility checking includes equality tests of the folowing data members:\n
     - _support
     - _numberOfComponents
     - _numberOfValues
     - _componentsTypes
     - _MEDComponentsUnits.

     The data members of the returned field are initialized, based on the first field, except for the name,
     which is the combination of the two field's names and the operator.
     Advised Utilisation in C++ : <tt> FIELD<T> c = a / b; </tt> \n
     In this case, the (recent) compilators perform optimisation and don't call the copy constructor.
     When using python, this operator calls the copy constructor in any case.
     The user has to be aware that when using operator / in associatives expressions like
     <tt> a = b / c / d /e; </tt> \n
     no optimisation is performed : the evaluation of last expression requires the construction of
     3 temporary fields.
*/
template <class T, class INTERLACING_TAG>
const FIELD<T, INTERLACING_TAG> FIELD<T, INTERLACING_TAG>::operator/(const FIELD & m) const
{
    BEGIN_OF("FIELD<T>::operator/(const FIELD & m)");
    FIELD_::_checkFieldCompatibility(*this, m, false); // may throw exception

    // Creation of the result - memory is allocated by FIELD constructor
    FIELD<T, INTERLACING_TAG> result(this->getSupport(),this->getNumberOfComponents());
    //result._operation(*this,m,mode,"/"); // perform Atribute's initialization & division
    result._operationInitialize(*this,m,"/"); // perform Atribute's initialization
    result._div_in_place(*this,m); // perform division

    END_OF("FIELD<T>::operator/(const FIELD & m)");
    return result;
}


/*!  Overloaded Operator /=
 *   Operations are directly performed in the first field's array.
 *   This operation is authorized only for compatible fields that have the same support.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>& FIELD<T, INTERLACING_TAG>::operator/=(const FIELD & m)
{
    BEGIN_OF("FIELD<T>::operator/=(const FIELD & m)");
    FIELD_::_checkFieldCompatibility(*this, m, false); // may throw exception

    const T* value1=m.getValue(); // get pointers to the values we are adding

    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue());
    const int size=getNumberOfValues()*getNumberOfComponents(); // size of array
    const T* endV=value+size; // pointer to the end of value

    for(;value!=endV; value1++,value++)
	*value /= *value1;

    END_OF("FIELD<T>::operator/=(const FIELD & m)");
    return *this;
}


/*! Division of fields. Static member function.
 *  The function return a pointer to a new created field that holds the division.
 *  Data members are checked for compatibility and initialized.
 *  The user is in charge of memory deallocation.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>* FIELD<T, INTERLACING_TAG>::div(const FIELD& m, const FIELD& n)
{
    BEGIN_OF("FIELD<T>::div(const FIELD & m, const FIELD& n)");
    FIELD_::_checkFieldCompatibility(m, n, false); // may throw exception

    // Creation of a new field
    FIELD<T, INTERLACING_TAG>* result = new FIELD<T, INTERLACING_TAG>(m.getSupport(),
								      m.getNumberOfComponents());
    result->_operationInitialize(m,n,"/"); // perform Atribute's initialization
    result->_div_in_place(m,n); // perform division

    END_OF("FIELD<T>::div(const FIELD & m, const FIELD& n)");
    return result;
}

/*! Same as div method except that field check is deeper.
 */
template <class T,class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>* FIELD<T, INTERLACING_TAG>::divDeep(const FIELD& m, const FIELD& n)
{
  BEGIN_OF("FIELD<T>::divDeep(const FIELD & m, const FIELD& n)");
  FIELD_::_deepCheckFieldCompatibility(m, n, false); // may throw exception

  // Creation of a new field
  FIELD<T, INTERLACING_TAG>* result = new FIELD<T, INTERLACING_TAG>(m.getSupport(),
								    m.getNumberOfComponents());
  result->_operationInitialize(m,n,"/"); // perform Atribute's initialization
  result->_div_in_place(m,n); // perform division

  END_OF("FIELD<T>::divDeep(const FIELD & m, const FIELD& n)");
  return result;
}

/*!
  \if developper
  This internal method initialize the members of a new field created to hold the result of the operation Op .
  Initialization is based on the first field, except for the name, which is the combination of the two field's names
  and the operator.
  \endif
*/
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::_operationInitialize(const FIELD& m,const FIELD& n, char* Op)
{
    MESSAGE("Appel methode interne " << Op);

    // Atribute's initialization (copy of the first field's attributes)
    // Other data members (_support, _numberOfValues) are initialized in the field's constr.
    setName(m.getName()+" "+Op+" "+n.getName());
    setComponentsNames(m.getComponentsNames());
    // not yet implemented    setComponentType(m.getComponentType());
    setComponentsDescriptions(m.getComponentsDescriptions());
    setMEDComponentsUnits(m.getMEDComponentsUnits());

    // The following data member may differ from field m to n.
    // The initialization is done based on the first field.

    if(m.getComponentsUnits() != NULL)
      setComponentsUnits(m.getComponentsUnits());
    else
      _componentsUnits = (UNIT *) NULL;

    setIterationNumber(m.getIterationNumber());
    setTime(m.getTime());
    setOrderNumber(m.getOrderNumber());
}


/*!
  \if developper
  Internal method called by FIELD<T>::operator+ and FIELD<T>::add to perform addition "in place".
  This method is applied to a just created field with medModeSwitch mode.
  For this reason, the alternate mode doesn't need to be set to 0 after performing operation :
  it doesn't exist!
  \endif
*/
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::_add_in_place(const FIELD& m,const FIELD& n)
{
    // get pointers to the values we are adding
    const T* value1=m.getValue();
    const T* value2=n.getValue();
    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue());

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
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::_sub_in_place(const FIELD& m,const FIELD& n)
{
    // get pointers to the values we are adding
    const T* value1=m.getValue();
    const T* value2=n.getValue();
    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue());

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
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::_mul_in_place(const FIELD& m,const FIELD& n)
{
    // get pointers to the values we are adding
    const T* value1=m.getValue();
    const T* value2=n.getValue();
    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue());

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
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::_div_in_place(const FIELD& m,const FIELD& n) throw (MEDEXCEPTION)
{
    // get pointers to the values we are adding
    const T* value1=m.getValue();
    const T* value2=n.getValue();
    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue());

    const int size=getNumberOfValues()*getNumberOfComponents();
    SCRUTE(size);
    const T* endV1=value1+size;
    for(;value1!=endV1; value1++,value2++,value++){
      if ( *value2 == 0 ) { // FAIRE PLUTOT UN TRY CATCH Rmq from EF
	  string diagnosis;
	  diagnosis="FIELD<T,INTERLACING_TAG>::_div_in_place(...) : Divide by zero !";
	  throw MEDEXCEPTION(diagnosis.c_str());
	}
	*value=(*value1)/(*value2);
    }
}

/*!  Return Max Norm
 */
template <class T, class INTERLACIN_TAG> double FIELD<T, INTERLACIN_TAG>::normMax() const throw (MEDEXCEPTION)
{
    const T* value=getValue(); // get pointer to the values
    const int size=getNumberOfValues()*getNumberOfComponents();
    if (size <= 0) // Size of array has to be strictly positive
    {
	string diagnosis;
	diagnosis="FIELD<T,INTERLACIN_TAG>::normMax() : cannot compute the norm of "+getName()+
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
template <class T, class INTERLACIN_TAG> double FIELD<T, INTERLACIN_TAG>::norm2() const throw (MEDEXCEPTION)
{
    const T* value=this->getValue(); // get const pointer to the values
    const int size=getNumberOfValues()*getNumberOfComponents(); // get size of array
    if (size <= 0) // Size of array has to be strictly positive
    {
	string diagnosis;
	diagnosis="FIELD<T,INTERLACIN_TAG>::norm2() : cannot compute the norm of "+getName()+
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
template <class T, class INTERLACIN_TAG> template <T T_function(T)>
void FIELD<T, INTERLACIN_TAG>::applyFunc()
{
  // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue());
    const int size=getNumberOfValues()*getNumberOfComponents(); // size of array

    if (size>0) // for a negative size, there is nothing to do
    {
	const T* lastvalue=value+size; // pointer to the end of value
	for(;value!=lastvalue; ++value) // apply linear transformation
	    *value = T_function(*value);
    }
}

template <class T, class INTERLACIN_TAG> T FIELD<T, INTERLACIN_TAG>::pow(T x)
{
  return (T)::pow(x,FIELD<T, INTERLACIN_TAG>::_scalarForPow);
}

/*!  Apply to each (scalar) field component the math function pow.
 *   calculation is done "in place".
 *   Use examples :
 *
 *   \code  myField.applyFunc<std::sqrt>();  // apply sqare root function \endcode
 *     \code myField.applyFunc<myFunction>(); // apply your own created function \endcode
 */
template <class T, class INTERLACIN_TAG> void FIELD<T, INTERLACIN_TAG>::applyPow(T scalar)
{
  FIELD<T, INTERLACIN_TAG>::_scalarForPow=scalar;
  applyFunc<FIELD<T, INTERLACIN_TAG>::pow>();
}

/*!  Apply to each (scalar) field component the linear function x -> ax+b.
 *   calculation is done "in place".
 */
template <class T, class INTERLACIN_TAG> void FIELD<T, INTERLACIN_TAG>::applyLin(T a, T b)
{
    // get a non const pointer to the inside array of values and perform operation in place
    T * value=const_cast<T *> (getValue());
    const int size=getNumberOfValues()*getNumberOfComponents(); // size of array

    if (size>0) // for a negative size, there is nothing to do
    {
	const T* lastvalue=value+size; // pointer to the end of value
	for(;value!=lastvalue; ++value) // apply linear transformation
	    *value = a*(*value)+b;
    }
}


/*!
 *   Return a pointer to a new field that holds the scalar product. Static member function.
 *   This operation is authorized only for compatible fields that have the same support.
 *   The compatibility checking includes equality tests of the folowing data members:\n
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
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>*
FIELD<T, INTERLACING_TAG>::scalarProduct(const FIELD & m, const FIELD & n, bool deepCheck)
{
    if(!deepCheck)
      FIELD_::_checkFieldCompatibility( m, n, false); // may throw exception
    else
      FIELD_::_deepCheckFieldCompatibility(m, n, false);

    // we need a MED_FULL_INTERLACE representation of m & n to compute the scalar product
    // result type imply INTERLACING_TAG=FullInterlace for m & n

    const int numberOfElements=m.getNumberOfValues(); // strictly positive
    const int NumberOfComponents=m.getNumberOfComponents(); // strictly positive

    // Creation & init of a the result field on the same support, with one component
    // You have to be careful about the interlacing mode, because in the computation step,
    // it seems to assume the the interlacing mode is the FullInterlacing

    FIELD<T, INTERLACING_TAG>* result = new FIELD<T, INTERLACING_TAG>(m.getSupport(),1);
    result->setName( "scalarProduct ( " + m.getName() + " , " + n.getName() + " )" );
    result->setIterationNumber(m.getIterationNumber());
    result->setTime(m.getTime());
    result->setOrderNumber(m.getOrderNumber());

    const T* value1=m.getValue(); // get const pointer to the values
    const T* value2=n.getValue(); // get const pointer to the values
    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (result->getValue());

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
template <class T, class INTERLACING_TAG>
double FIELD<T, INTERLACING_TAG>::normL2(int component,
					 const FIELD<double, FullInterlace> * p_field_volume) const
{
    _checkNormCompatibility(p_field_volume); // may throw exception
    if ( component<1 || component>getNumberOfComponents() )
	throw MEDEXCEPTION(STRING("FIELD<T>::normL2() : The component argument should be between 1 and the number of components"));

    const FIELD<double, FullInterlace> * p_field_size=p_field_volume;
    if(!p_field_volume) // if the user don't supply the volume
	p_field_size=_getFieldSize(); // we calculate the volume [PROVISOIRE, en attendant l'implémentation dans mesh]

    // get pointer to the element's volumes. MED_FULL_INTERLACE is the default mode for p_field_size
    const double* vol=p_field_size->getValue();
    // Il n'est vraiment pas optimal de mixer des champs dans des modes d'entrelacement
    // different juste pour le calcul

    const T * value     = NULL;
    ArrayNo * myArray   = NULL;
    if ( getInterlacingType() == MED_EN::MED_NO_INTERLACE )
      value = getValue();
    else {
      myArray = ArrayConvert( *( dynamic_cast< ArrayFull * > ( getArrayNoGauss() ) ));
      value   = myArray->getPtr();
    }

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
    if ( getInterlacingType() == MED_EN::MED_FULL_INTERLACE ) delete myArray;
    if( totVol <= 0)
	throw MEDEXCEPTION(STRING("cannot compute sobolev norm : volume is not positive!"));

    return integrale/totVol;
}

/*!  Return L2 Norm  of the field.
 *   Cannot be applied to a field with a support on nodes.
 *   If the optional p_field_volume argument is furnished, the volume is not re-calculated.
 */
template <class T, class INTERLACING_TAG>
double FIELD<T, INTERLACING_TAG>::normL2(const FIELD<double, FullInterlace> * p_field_volume) const
{
    _checkNormCompatibility(p_field_volume); // may throw exception
    const FIELD<double, FullInterlace> * p_field_size=p_field_volume;
    if(!p_field_volume) // if the user don't supply the volume
	p_field_size=_getFieldSize(); // we calculate the volume [PROVISOIRE, en attendant l'implÃÂ©mentation dans mesh]

    // get pointer to the element's volumes. MED_FULL_INTERLACE is the default mode for p_field_size
    const double* vol=p_field_size->getValue();
    const double* lastvol=vol+getNumberOfValues(); // pointing just after the end of vol

    const T * value     = NULL;
    ArrayNo * myArray   = NULL;
    if ( getInterlacingType() == MED_EN::MED_NO_INTERLACE )
      value = getValue();
    else {
      myArray = ArrayConvert( *( dynamic_cast< ArrayFull * > ( getArrayNoGauss() ) ));
      value   = myArray->getPtr();
    }

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
    if ( getInterlacingType() == MED_EN::MED_FULL_INTERLACE ) delete myArray;
    if( totVol <= 0)
	throw MEDEXCEPTION(STRING("cannot compute sobolev norm : volume is not positive!"));

    return integrale/totVol;
}

/*!  Return L1 Norm  of the field's component.
 *   Cannot be applied to a field with a support on nodes.
 *   If the optional p_field_volume argument is furnished, the volume is not re-calculated.
 */
template <class T, class INTERLACING_TAG>
double FIELD<T, INTERLACING_TAG>::normL1(int component,
					 const FIELD<double, FullInterlace > * p_field_volume) const
{
    _checkNormCompatibility(p_field_volume); // may throw exception
    if ( component<1 || component>getNumberOfComponents() )
	throw MEDEXCEPTION(STRING("FIELD<T,INTERLACING_TAG>::normL2() : The component argument should be between 1 and the number of components"));

    const FIELD<double,FullInterlace> * p_field_size=p_field_volume;
    if(!p_field_volume) // if the user don't supply the volume
	p_field_size=_getFieldSize(); // we calculate the volume [PROVISOIRE, en attendant l'implÃÂ©mentation dans mesh]

    // get pointer to the element's volumes. MED_FULL_INTERLACE is the default mode for p_field_size
    const double* vol=p_field_size->getValue();
    const T * value     = NULL;
    ArrayNo * myArray   = NULL;
    if ( getInterlacingType() == MED_EN::MED_NO_INTERLACE )
      value = getColumn(component);
    else {
      myArray = ArrayConvert( *( dynamic_cast< ArrayFull * > ( getArrayNoGauss() ) ));
      value   = myArray->getColumn(component);
    }

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
    if ( getInterlacingType() == MED_EN::MED_FULL_INTERLACE ) delete myArray;
    if( totVol <= 0)
	throw MEDEXCEPTION(STRING("cannot compute sobolev norm : volume is not positive!"));

    return integrale/totVol;
}

/*!  Return L1 Norm  of the field.
 *   Cannot be applied to a field with a support on nodes.
 *   If the optional p_field_volume argument is furnished, the volume is not re-calculated.
 */
template <class T, class INTERLACING_TAG>
double FIELD<T, INTERLACING_TAG>::normL1(const FIELD<double, FullInterlace> * p_field_volume) const
{
    _checkNormCompatibility(p_field_volume); // may throw exception
    const FIELD<double, FullInterlace> * p_field_size=p_field_volume;
    if(!p_field_volume) // if the user don't supply the volume
	p_field_size=_getFieldSize(); // we calculate the volume [PROVISOIRE, en attendant l'implÃÂ©mentation dans mesh]

    // get pointer to the element's volumes. MED_FULL_INTERLACE is the default mode for p_field_size
    const double* vol=p_field_size->getValue();
    const double* lastvol=vol+getNumberOfValues(); // pointing just after the end of vol

   const T * value     = NULL;
    ArrayNo * myArray   = NULL;
    if ( getInterlacingType() == MED_EN::MED_NO_INTERLACE )
      value = getValue();
    else {
      myArray = ArrayConvert( *( dynamic_cast< ArrayFull * > ( getArrayNoGauss() ) ));
      value   = myArray->getPtr();
    }

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
    if ( getInterlacingType() == MED_EN::MED_FULL_INTERLACE ) delete myArray;
    if( totVol <= 0)
	throw MEDEXCEPTION(STRING("cannot compute sobolev norm : volume is not positive!"));

    return integrale/totVol;
}

/*! Return a new field (to deallocate with delete) lying on subSupport that is included by
 *   this->_support with corresponding values extracting from this->_value.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>* FIELD<T, INTERLACING_TAG>::extract(const SUPPORT *subSupport) const throw (MEDEXCEPTION)
{
  if(!subSupport->belongsTo(*_support))
    throw MEDEXCEPTION("FIELD<T>::extract : subSupport not included in this->_support !");
  if(_support->isOnAllElements() && subSupport->isOnAllElements())
    return new FIELD<T, INTERLACING_TAG>(*this);

  FIELD<T, INTERLACING_TAG> *ret = new FIELD<T, INTERLACING_TAG>(subSupport,
								 _numberOfComponents);

  if(!ret->_value)
    throw MEDEXCEPTION("FIELD<T>::extract : unvalid support detected !");

  T* valuesToSet=(T*)ret->getValue();

  int nbOfEltsSub=subSupport->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS);
  const int *eltsSub=subSupport->getNumber(MED_EN::MED_ALL_ELEMENTS);
  T* tempVals=new T[_numberOfComponents];
  for(int i=0;i<nbOfEltsSub;i++)
    {
      if(!getValueOnElement(eltsSub[i],tempVals))
	throw MEDEXCEPTION("Problem in belongsTo function !!!");
      for(int j=0;j<_numberOfComponents;j++)
	valuesToSet[i*_numberOfComponents+j]=tempVals[j];
    }
  delete [] tempVals;

  ret->copyGlobalInfo(*this);
  return ret;
}

/*!
  Constructor with parameters; the object is set via a file and its associated
  driver. For the moment only the MED_DRIVER is considered and if the last two
  argument (iterationNumber and orderNumber) are not set; their default value
  is -1. If the field fieldDriverName with the iteration number
  iterationNumber and the order number orderNumber does not exist in the file
  fieldDriverName; the constructor raises an exception.
*/
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>::FIELD(const SUPPORT * Support,
				 driverTypes driverType,
				 const string & fileName/*=""*/,
				 const string & fieldDriverName/*=""*/,
				 const int iterationNumber,
				 const int orderNumber) throw (MEDEXCEPTION)
{
  const char * LOC = "template <class T> FIELD<T>::FIELD(const SUPPORT * Support, driverTypes driverType, const string & fileName=\"\", const string & fieldName=\"\", const int iterationNumber=-1, const int orderNumber=-1) : ";

  int current;

  BEGIN_OF(LOC);

  init();

  //INITIALISATION DE _valueType DS LE CONSTRUCTEUR DE FIELD_
  ASSERT(FIELD_::_valueType == MED_EN::MED_UNDEFINED_TYPE)
  FIELD_::_valueType=SET_VALUE_TYPE<T>::_valueType;

  //INITIALISATION DE _interlacingType DS LE CONSTRUCTEUR DE FIELD_
  ASSERT(FIELD_::_interlacingType == MED_EN::MED_UNDEFINED_INTERLACE)
  FIELD_::_interlacingType=SET_INTERLACING_TYPE<INTERLACING_TAG>::_interlacingType;

  _support = Support;
  //A.G. Addings for RC
  if(_support)
    _support->addReference();
  // OCC 10/03/2006 -- According to the rules defined with help of 
  // MEDMEM_IntrelacingTraits class, it is not allowed to instantiate
  // MEDMEM_Array<> template using INTERLACING_TAG parameter of 
  // FIELD template - MSVC++ 2003 compiler generated an error here.
  // _value = (MEDMEM_Array<T, INTERLACING_TAG> *) NULL;
  _value = NULL;

  _iterationNumber = iterationNumber;
  _time = 0.0;
  _orderNumber = orderNumber;

  current = addDriver(driverType,fileName,fieldDriverName,MED_EN::MED_LECT);

  _drivers[current]->open();
  _drivers[current]->read();
  _drivers[current]->close();

  END_OF(LOC);
}

/*!
  This constructor, at least, allows to create a FIELD without creating any
  SUPPORT then without having to load a MESH object, a support is created. It
  provides the meshName related mesh but doesn't not set a mesh in the created
  support.
*/
template <class T, class INTERLACING_TAG>
FIELD<T,INTERLACING_TAG>::FIELD(driverTypes driverType,
				const string & fileName,
				const string & fieldDriverName,
				const int iterationNumber,
				const int orderNumber)
  throw (MEDEXCEPTION) :FIELD_()
{
  int current;
  const char * LOC ="FIELD<T,INTERLACING_TAG>::FIELD( driverTypes driverType, const string & fileName, string & fieldDriverName, int iterationNumber, int orderNumber) : ";
  BEGIN_OF(LOC);

  init();

  //INITIALISATION DE _valueType DS LE CONSTRUCTEUR DE FIELD_
  ASSERT(FIELD_::_valueType == MED_EN::MED_UNDEFINED_TYPE)
  FIELD_::_valueType=SET_VALUE_TYPE<T>::_valueType;

  //INITIALISATION DE _interlacingType DS LE CONSTRUCTEUR DE FIELD_
  ASSERT(FIELD_::_interlacingType == MED_EN::MED_UNDEFINED_INTERLACE)
  FIELD_::_interlacingType=SET_INTERLACING_TYPE<INTERLACING_TAG>::_interlacingType;

  _support = (SUPPORT *) NULL;
  // OCC 10/03/2006 -- According to the rules defined with help of 
  // MEDMEM_IntrelacingTraits class, it is not allowed to instantiate
  // MEDMEM_Array<> template using INTERLACING_TAG parameter of 
  // FIELD template - MSVC++ 2003 compiler generated an error here.
  // _value = (MEDMEM_Array<T, INTERLACING_TAG> *) NULL;
  _value = NULL;

  _iterationNumber = iterationNumber;
  _time = 0.0;
  _orderNumber = orderNumber;

  current = addDriver(driverType,fileName,fieldDriverName,MED_EN::MED_LECT);

  _drivers[current]->open();
  _drivers[current]->read();
  _drivers[current]->close();

  END_OF(LOC);
}

/*!
  Destructor.
*/
template <class T, class INTERLACING_TAG> FIELD<T, INTERLACING_TAG>::~FIELD()
{
  BEGIN_OF(" Destructeur FIELD<T, INTERLACING_TAG>::~FIELD()");
  SCRUTE(this);
  if (_value) delete _value;
  locMap::const_iterator it;
  for ( it = _gaussModel.begin();it != _gaussModel.end(); it++ )
    delete (*it).second;

  END_OF(" Destructeur FIELD<T,INTERLACING_TAG>::~FIELD()");
}

/*!

*/
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::allocValue(const int NumberOfComponents)
{
  const char* LOC = "FIELD<T, INTERLACING_TAG>::allocValue(const int NumberOfComponents)" ;
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
    // becarefull about the number of gauss point
    _numberOfValues = _support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS);
    MESSAGE(LOC <<" : "<<_numberOfValues <<" et "<< NumberOfComponents);

    //EF : A modifier lors de l'intégration de la classe de localisation des points de gauss
    _value = new ArrayNoGauss(_numberOfComponents,_numberOfValues);

    _isRead = true ;
  }
  catch (MEDEXCEPTION &ex) {
    MESSAGE("No value defined, problem with NumberOfComponents (and may be _support) size of MEDARRAY<T>::_value !");
    // OCC 10/03/2006 -- According to the rules defined with help of 
    // MEDMEM_IntrelacingTraits class, it is not allowed to instantiate
    // MEDMEM_Array<> template using INTERLACING_TAG parameter of 
    // FIELD template - MSVC++ 2003 compiler generated an error here.
    // _value = (MEDMEM_Array<T, INTERLACING_TAG> *) NULL;
    _value = NULL;
  }

  SCRUTE(_value);
  END_OF("void FIELD<T, INTERLACING_TAG>::allocValue(const int NumberOfComponents)");
}

/*!

*/
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::allocValue(const int NumberOfComponents,
					   const int LengthValue)
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

  //EF : A modifier lors de l'intégration de la classe de localisation des points de gauss
  _value = new ArrayNoGauss(_numberOfComponents,_numberOfValues);

  _isRead = true ;

  SCRUTE(_value);
  END_OF("void FIELD<T, INTERLACING_TAG>::allocValue(const int NumberOfComponents,const int LengthValue)");
}

/*!

*/
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::deallocValue()
{
  BEGIN_OF("void FIELD<T, INTERLACING_TAG>::deallocValue()");
  _numberOfValues = 0 ;
  _numberOfComponents = 0 ;
  if (_value != NULL)
    delete _value;

  END_OF("void FIELD<T, INTERLACING_TAG>::deallocValue()");
}

// -----------------
// Methodes Inline
// -----------------

/*!
  Create the specified driver and return its index reference to path to
  read or write methods.
*/

template <class T, class INTERLACING_TAG>
int FIELD<T, INTERLACING_TAG>::addDriver(driverTypes driverType,
					 const string & fileName/*="Default File Name.med"*/,
					 const string & driverName/*="Default Field Name"*/,
					 MED_EN::med_mode_acces access)
{
  //jfa tmp (as last argument has no default value):const char * LOC = "FIELD<T>::addDriver(driverTypes driverType, const string & fileName=\"Default File Name.med\",const string & driverName=\"Default Field Name\",MED_EN::med_mode_acces access) : ";
  const char * LOC = "FIELD<T>::addDriver(driverTypes driverType, const string & fileName,const string & driverName,MED_EN::med_mode_acces access) :";//jfa tmp

  GENDRIVER * driver;

  BEGIN_OF(LOC);

  SCRUTE(driverType);

  driver = DRIVERFACTORY::buildDriverForField(driverType,fileName,this,access);

  _drivers.push_back(driver);

  int current = _drivers.size()-1;

  _drivers[current]->setFieldName(driverName);

  END_OF(LOC);

  return current;
}


/*!
  Duplicate the given driver and return its index reference to path to
  read or write methods.
*/
template <class T, class INTERLACING_TAG>
inline int FIELD<T, INTERLACING_TAG>::addDriver (GENDRIVER & driver )
{
  const char * LOC = "FIELD<T, INTERLACING_TAG>::addDriver(GENDRIVER &) : ";
  int current;

  BEGIN_OF(LOC);

  // duplicate driver to delete it with destructor !
  GENDRIVER * newDriver = driver.copy() ;

  _drivers.push_back(newDriver);

  current = _drivers.size()-1;
  SCRUTE(current);
  driver.setId(current);

  MESSAGE(LOC << " je suis la 1");
  END_OF(LOC);
  MESSAGE(LOC << " je suis la 2");

  return current ;
};

/*!
  Remove the driver referenced by its index.
*/
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::rmDriver (int index/*=0*/)
{
  const char * LOC = "FIELD<T, INTERLACING_TAG>::rmDriver (int index=0): ";
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
template <class T, class INTERLACING_TAG> inline void FIELD<T, INTERLACING_TAG>::read(int index/*=0*/)
{
  const char * LOC = "FIELD<T, INTERLACING_TAG>::read(int index=0) : ";
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
template <class T, class INTERLACING_TAG> inline void FIELD<T, INTERLACING_TAG>::write(int index/*=0*/, const string & driverName /*= ""*/)
{
  const char * LOC = "FIELD<T,INTERLACING_TAG>::write(int index=0, const string & driverName = \"\") : ";
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
template <class T, class INTERLACING_TAG> inline void FIELD<T, INTERLACING_TAG>::writeAppend(int index/*=0*/, const string & driverName /*= ""*/)
{
  const char * LOC = "FIELD<T,INTERLACING_TAG>::write(int index=0, const string & driverName = \"\") : ";
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
template <class T, class INTERLACING_TAG> inline void FIELD<T, INTERLACING_TAG>::write(const GENDRIVER & genDriver)
{
  const char * LOC = " FIELD<T, INTERLACING_TAG>::write(const GENDRIVER &) : ";
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
template <class T, class INTERLACING_TAG> inline void FIELD<T, INTERLACING_TAG>::writeAppend(const GENDRIVER & genDriver)
{
  const char * LOC = " FIELD<T, INTERLACING_TAG>::write(const GENDRIVER &) : ";
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
template <class T, class INTERLACING_TAG> inline void FIELD<T, INTERLACING_TAG>::read(const GENDRIVER & genDriver)
{
  const char * LOC = " FIELD<T, INTERLACING_TAG>::read(const GENDRIVER &) : ";
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
  Fills in already allocated retValues array the values related to eltIdInSup.
  If the element does not exist in this->_support false is returned, true otherwise.
 */
template <class T, class INTERLACING_TAG>
bool FIELD<T, INTERLACING_TAG>::getValueOnElement(int eltIdInSup,T* retValues)
  const throw (MEDEXCEPTION)
{

  if(eltIdInSup<1)
    return false;
  if(_support->isOnAllElements())
    {
      int nbOfEltsThis=_support->getMesh()->getNumberOfElements(_support->getEntity(),MED_EN::MED_ALL_ELEMENTS);
      if(eltIdInSup>nbOfEltsThis)
	return false;
      const T* valsThis=getValue();
      for(int j=0;j<_numberOfComponents;j++)
	retValues[j]=valsThis[(eltIdInSup-1)*_numberOfComponents+j];
      return true;
    }
  else
    {
      int nbOfEltsThis=_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS);
      const int *eltsThis=_support->getNumber(MED_EN::MED_ALL_ELEMENTS);
      int iThis;
      bool found=false;
      for(iThis=0;iThis<nbOfEltsThis && !found;)
	if(eltsThis[iThis]==eltIdInSup)
	  found=true;
	else
	  iThis++;
      if(!found)
	return false;
      const T* valsThis=getValue();
      for(int j=0;j<_numberOfComponents;j++)
	retValues[j]=valsThis[iThis*_numberOfComponents+j];
      return true;
    }
}

/*!
  \if developper
  Destroy the MEDARRAY<T> in FIELD and put the new one without copy.
  \endif
*/
template <class T, class INTERLACING_TAG>
inline void FIELD<T, INTERLACING_TAG>::setArray(MEDMEM_Array_ * Value)
  throw (MEDEXCEPTION)
{
  if (NULL != _value) delete _value ;
  _value=Value ;
}

/*!
  \if developper
  Return a reference to  the MEDARRAY<T, INTERLACING_TAG> in FIELD.
  \endif
*/
template <class T, class INTERLACING_TAG>
inline MEDMEM_Array_ * FIELD<T, INTERLACING_TAG>::getArray() const throw (MEDEXCEPTION)
{
  const char * LOC = "MEDMEM_Array_ * FIELD<T, INTERLACING_TAG>::getArray() : ";
  BEGIN_OF(LOC);
  END_OF(LOC);
  return _value ;
}
template <class T,class INTERLACING_TAG>  inline
typename MEDMEM_ArrayInterface<T,INTERLACING_TAG,Gauss>::Array *
FIELD<T, INTERLACING_TAG>::getArrayGauss() const throw (MEDEXCEPTION)
{
  const char * LOC = "FIELD<T, INTERLACING_TAG>::getArrayGauss() : ";
  BEGIN_OF(LOC);

  if ( getGaussPresence() )
    return dynamic_cast<ArrayGauss *> (_value);
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<
				 "The field has no Gauss Point"));

  END_OF(LOC);

}

template <class T,class INTERLACING_TAG>  inline
typename MEDMEM_ArrayInterface<T,INTERLACING_TAG,NoGauss>::Array *
FIELD<T, INTERLACING_TAG>::getArrayNoGauss() const throw (MEDEXCEPTION)
{
  const char * LOC = "FIELD<T, INTERLACING_TAG>::getArrayNoGauss() : ";
  BEGIN_OF(LOC);

  if ( ! getGaussPresence() )
    return dynamic_cast < ArrayNoGauss * > (_value);
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<
				 "The field has Gauss Point"));

  END_OF(LOC);
}


template <class T,class INTERLACING_TAG> inline bool
FIELD<T, INTERLACING_TAG>::getGaussPresence() const throw (MEDEXCEPTION)
{
  const char * LOC = "FIELD<T, INTERLACING_TAG>::getGaussPresence() const :";
  //BEGIN_OF(LOC);

  if (_value != NULL)
    return _value->getGaussPresence();
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Can't call getGaussPresence on a null _value"));

  //END_OF(LOC);
}

/*!
  Return the actual length of the reference to values array returned by getValue.
  Take care of number of components and number of Gauss points by geometric type
*/
template <class T, class INTERLACING_TAG>
inline int FIELD<T, INTERLACING_TAG>::getValueLength() const
  throw (MEDEXCEPTION)
{
  if ( getGaussPresence() )
    return dynamic_cast<ArrayGauss *>(_value)->getArraySize() ;
  else
    return dynamic_cast<ArrayNoGauss *>(_value)->getArraySize() ;
}

/*!
  Return a reference to values array to read them.
*/
template <class T, class INTERLACIN_TAG>
inline const T* FIELD<T, INTERLACIN_TAG>::getValue() const throw (MEDEXCEPTION)
{
  const char * LOC ="FIELD<T, INTERLACING_TAG>::getValue() : ";
  BEGIN_OF(LOC);
  if ( getGaussPresence() )
    return dynamic_cast<ArrayGauss *>(_value)->getPtr() ;
  else
    return dynamic_cast<ArrayNoGauss *>(_value)->getPtr() ;
}
/*!
  Return a reference to i^{th} row
  of FIELD values array.
  If a faster accessor is intended you may use getArray() once,
  then MEDMEM_Array accessors.
  Be careful if field support is not on all elements getRow
  use support->getValIndFromGlobalNumber(i).
*/
template <class T,class INTERLACING_TAG> inline
const T*
FIELD<T,INTERLACING_TAG>::getRow(int i) const throw (MEDEXCEPTION)
{
  const char * LOC = "FIELD<T,INTERLACING_TAG>::getRow(int i) : ";
  //BEGIN_OF(LOC);

  int valIndex=-1;
  if (_support)
    valIndex = _support->getValIndFromGlobalNumber(i);
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not define |" ));

  //cout << endl << "getRow Valindex : " << valIndex << endl;
  if ( getGaussPresence() )
    return static_cast<ArrayGauss *>(_value)->getRow(valIndex) ;
  else
    return static_cast<ArrayNoGauss *>(_value)->getRow(valIndex) ;
  //END_OF(LOC);
}

/*!
  Return a reference to j^{th} column
  of FIELD values array.
*/
template <class T,class INTERLACING_TAG> inline const T*
FIELD<T,INTERLACING_TAG>::getColumn(int j) const throw (MEDEXCEPTION)
{
  const char * LOC ="FIELD<T,INTERLACING_TAG>::getColumn(int j) : ";
  //BEGIN_OF(LOC);
  if ( getGaussPresence() )
    return static_cast<ArrayGauss *>(_value)->getColumn(j) ;
  else
    return static_cast<ArrayNoGauss *>(_value)->getColumn(j) ;
}

/*!
  Return the value of i^{th} element and j^{th} component.
*/
template <class T,class INTERLACING_TAG> inline T FIELD<T,INTERLACING_TAG>::getValueIJ(int i,int j) const throw (MEDEXCEPTION)
{
  const char * LOC = "getValueIJ(..)";
  //BEGIN_OF(LOC);
  int valIndex=-1;
  if (_support)
    valIndex = _support->getValIndFromGlobalNumber(i);
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not defined" ));

  if ( getGaussPresence() )
    return static_cast<ArrayGauss *>(_value)->getIJ(valIndex,j) ;
  else
    return static_cast<ArrayNoGauss *>(_value)->getIJ(valIndex,j) ;
}

/*!
  Return the j^{th} component of k^{th} gauss points of i^{th} value.
*/
template <class T,class INTERLACING_TAG> inline T FIELD<T,INTERLACING_TAG>::getValueIJK(int i,int j,int k) const throw (MEDEXCEPTION)
{
  const char * LOC = "getValueIJK(..)";
  //BEGIN_OF(LOC);
  int valIndex=-1;
  if (_support)
    valIndex = _support->getValIndFromGlobalNumber(i);
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not defined" ));

  if ( getGaussPresence() )
    return static_cast<ArrayGauss *>(_value)->getIJK(valIndex,j,k) ;
  else
    return static_cast<ArrayNoGauss *>(_value)->getIJK(valIndex,j,k) ;
}


template <class T,class INTERLACING_TAG> const int FIELD<T,INTERLACING_TAG>::getNumberOfGeometricTypes() const throw (MEDEXCEPTION)
{
  const char * LOC = "getNumberOfGeometricTypes(..)";
  BEGIN_OF(LOC);
  if (_support)
    return _support->getNumberOfTypes();
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not defined" ));
  END_OF(LOC);
};


template <class T,class INTERLACING_TAG> const GAUSS_LOCALIZATION<INTERLACING_TAG> &
FIELD<T,INTERLACING_TAG>::getGaussLocalization(MED_EN::medGeometryElement geomElement) const throw (MEDEXCEPTION)
{
  const char * LOC ="getGaussLocalization(MED_EN::medGeometryElement geomElement) : ";
  const GAUSS_LOCALIZATION_ * locPtr=0;

  locMap::const_iterator it;
  if ( ( it = _gaussModel.find(geomElement)) != _gaussModel.end() ) {
	locPtr = (*it).second;
	return  *static_cast<const GAUSS_LOCALIZATION<INTERLACING_TAG> *>(locPtr);
  }
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Can't find any GaussLocalization on this geometric type" ));

};

/*!
  Returns number of Gauss points for this medGeometryElement.

  Note :
  if there is no GAUSS_LOCALIZATION having this medGeometryElement but
  the medGeometryElement exist in the SUPPORT, getNumberOfGaussPoints
  return 1
*/
template <class T,class INTERLACING_TAG> const int FIELD<T,INTERLACING_TAG>::getNumberOfGaussPoints(MED_EN::medGeometryElement geomElement) const
  throw (MEDEXCEPTION)
{
  const char * LOC ="getNumberOfGaussPoints(MED_EN::medGeometryElement geomElement) : ";
  const GAUSS_LOCALIZATION_ * locPtr=0;

  locMap::const_iterator it;
  if ( ( it = _gaussModel.find(geomElement)) != _gaussModel.end() ) {
	locPtr = (*it).second;
	return  static_cast<const GAUSS_LOCALIZATION<INTERLACING_TAG> *>(locPtr)->getNbGauss();
  }
  else
    if (_support)
      try {
	if ( _support->getNumberOfElements(geomElement) ) return 1;
      } catch ( MEDEXCEPTION & ex) {
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<< "GeometricType not found !" )) ;
      }
    else
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not defined" ));

  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Should never execute this!" ));

}

/*!
  Returns number of Gauss points for each geometric type.

  Note :
  if there is no gauss points whatever the geometric type is
  it returns an exception. (renvoyer un tableau de 1 ?)
*/
template <class T,class INTERLACING_TAG> const int * FIELD<T,INTERLACING_TAG>::getNumberOfGaussPoints() const
  throw (MEDEXCEPTION)
{
  const char * LOC ="const int * getNumberOfGaussPoints(MED_EN::medGeometryElement geomElement) : ";

  if (_value)
    if ( getGaussPresence() ) {
      return dynamic_cast<ArrayGauss *>(_value)->getNbGaussGeo()+1;
    } else
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"value hasn't Gauss points " ));

    else
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Value not defined" ));
}

/*!
  Returns number of Gauss points for element n°i.
  The i index is a global index (take care of previous element
  on different geometric type).
*/
template <class T,class INTERLACING_TAG> const int FIELD<T,INTERLACING_TAG>::getNbGaussI(int i) const throw (MEDEXCEPTION)
{
  const char * LOC = "getNbGaussI(..)";
//   BEGIN_OF(LOC);

  int valIndex=-1;
  if (_support)
    valIndex = _support->getValIndFromGlobalNumber(i);
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not defined" ));

  if (_value)
   if ( getGaussPresence() )
     return static_cast<ArrayGauss *>(_value)->getNbGauss(valIndex) ;
   else
     return static_cast<ArrayNoGauss *>(_value)->getNbGauss(valIndex) ;
 else
   throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"_value not defined" ));
//   END_OF(LOC);
};

template <class T,class INTERLACING_TAG> const int * FIELD<T,INTERLACING_TAG>::getNumberOfElements() const throw (MEDEXCEPTION)
{
  const char * LOC = "getNumberOfElements(..)";
  BEGIN_OF(LOC);
  if (_support)
    return _support->getNumberOfElements();
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not defined" ));
   END_OF(LOC);
};

template <class T,class INTERLACING_TAG> const MED_EN::medGeometryElement  * FIELD<T,INTERLACING_TAG>::getGeometricTypes()  const throw (MEDEXCEPTION)
{
  const char * LOC = "getGeometricTypes(..)";
  BEGIN_OF(LOC);
  if (_support)
    return _support->getTypes();
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not defined" ));
   END_OF(LOC);
};
template <class T,class INTERLACING_TAG> bool  FIELD<T,INTERLACING_TAG>::isOnAllElements() const throw (MEDEXCEPTION)
{
  const char * LOC = "isOnAllElements(..)";
  BEGIN_OF(LOC);
  if (_support)
    return _support->isOnAllElements();
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not defined" ));
  END_OF(LOC);
};


/*!
  Copy new values array in FIELD according to the given mode.

  Array must have right size. If not results are unpredicable.
*/
template <class T,class INTERLACING_TAG> inline void FIELD<T,INTERLACING_TAG>::setValue( T* value) throw (MEDEXCEPTION) 
{
  if ( getGaussPresence() )
    return dynamic_cast<ArrayGauss *>(_value)->setPtr(value) ;
  else
    return dynamic_cast<ArrayNoGauss *>(_value)->setPtr(value) ;
}

/*!
  Update values array in the j^{th} row of FIELD values array with the given ones and
  according to specified mode.
*/
template <class T,class INTERLACING_TAG>
inline void FIELD<T,INTERLACING_TAG>::setRow( int i, T* value) throw (MEDEXCEPTION)
{
  const char * LOC = "FIELD<T,INTERLACING_TAG>::setRow(int i, T* value) : ";
  int valIndex=i;
  if (_support)
    valIndex = _support->getValIndFromGlobalNumber(i);
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not define |" ));

  if ( getGaussPresence() )
    return static_cast<ArrayGauss *>(_value)->setRow(valIndex, value) ;
  else
    return static_cast<ArrayNoGauss *>(_value)->setRow(valIndex, value) ;
}

/*!
  Update values array in the j^{th} column of FIELD values array with the given ones and
  according to specified mode.
*/
template <class T,class INTERLACING_TAG>
inline void FIELD<T,INTERLACING_TAG>::setColumn( int j, T* value) throw (MEDEXCEPTION)
{
  if ( getGaussPresence() )
    return static_cast<ArrayGauss *>(_value)->setColumn(j, value) ;
  else
    return static_cast<ArrayNoGauss *>(_value)->setColumn(j, value) ;
}

/*!
  Set the value of i^{th} element and j^{th} component with the given one.
*/
template <class T,class INTERLACING_TAG> inline void FIELD<T,INTERLACING_TAG>::setValueIJ(int i, int j, T value) throw (MEDEXCEPTION) 
{
  const char * LOC = "FIELD<T,INTERLACING_TAG>::setValueIJ(int i, int j, T value) : ";
  int valIndex=-1;
  if (_support)
    valIndex = _support->getValIndFromGlobalNumber(i);
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not define |" ));

  if ( getGaussPresence() )
    return static_cast<ArrayGauss *>(_value)->setIJ(valIndex,j,value) ;
  else
    return static_cast<ArrayNoGauss *>(_value)->setIJ(valIndex,j,value) ;
}

/*
  METHODS
*/

/*!
  Fill values array with volume values.
*/
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::getVolume() const throw (MEDEXCEPTION)
{
  const char * LOC = "FIELD<double>::getVolume() const : ";
  BEGIN_OF(LOC);

  // The field has to be initilised by a non empty support and a
  // number of components = 1 and its value type has to be set to MED_REEL64
  // (ie a FIELD<double>)

  if ((_support == (SUPPORT *) NULL) || (_numberOfComponents != 1) || (_valueType != MED_EN::MED_REEL64))
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"The field has to be initialised with a non empty support, a number of components set to 1 and a value type set to MED_REEL64"));

  END_OF(LOC);
}

/*!
  Fill values array with area values.
*/
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::getArea() const throw (MEDEXCEPTION)
{
  const char * LOC = "FIELD<double>::getArea() const : ";
  BEGIN_OF(LOC);

  // The field has to be initilised by a non empty support and a
  // number of components = 1 and its value type has to be set to MED_REEL64
  // (ie a FIELD<double>)

  if ((_support == (SUPPORT *) NULL) || (_numberOfComponents != 1) || (_valueType != MED_EN::MED_REEL64))
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"The field has to be initialised with a non empty support, a number of components set to 1 and a value type set to MED_REEL64"));

  END_OF(LOC);
}

/*!
  Fill values array with length values.
*/
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::getLength() const throw (MEDEXCEPTION)
{
  const char * LOC = "FIELD<double>::getLength() const : ";
  BEGIN_OF(LOC);

  // The field has to be initilised by a non empty support and a
  // number of components = 1 and its value type has to be set to MED_REEL64
  // (ie a FIELD<double>)

  if ((_support == (SUPPORT *) NULL) || (_numberOfComponents != 1) || (_valueType != MED_EN::MED_REEL64))
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"The field has to be initialised with a non empty support, a number of components set to 1 and a value type set to MED_REEL64"));

  END_OF(LOC);
}

/*!
  Fill values array with normal values.
*/
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::getNormal() const throw (MEDEXCEPTION)
{
  const char * LOC = "FIELD<double>::getNormal() const : ";
  BEGIN_OF(LOC);

  // The field has to be initilised by a non empty support and a
  // number of components = 1 and its value type has to be set to MED_REEL64
  // (ie a FIELD<double>)

  if (_support == (SUPPORT *) NULL)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"The field has to be initialised with a non empty support, a number of components set to the space dimension and a value type set to MED_REEL64"));

  int dim_space = _support->getMesh()->getSpaceDimension();

  if ((_numberOfComponents != dim_space) || (_valueType != MED_EN::MED_REEL64))
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"The field has to be initialised with a non empty support, a number of components set to the space dimension and a value type set to MED_REEL64"));

  END_OF(LOC);
}

/*!
  Fill values array with barycenter values.
*/
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::getBarycenter() const throw (MEDEXCEPTION)
{
  const char * LOC = "FIELD<double>::getBarycenter() const : ";
  BEGIN_OF(LOC);

  // The field has to be initilised by a non empty support and a number of
  //components = space dimension and its value type has to be set to MED_REEL64
  // (ie a FIELD<double>)

  if (_support == (SUPPORT *) NULL)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"The field has to be initialised with a non empty support, a number of components set to the space dimension and a value type set to MED_REEL64"));

  int dim_space = _support->getMesh()->getSpaceDimension();

  if ((_numberOfComponents != dim_space) || (_valueType != MED_EN::MED_REEL64))
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"The field has to be initialised with a non empty support, a number of components set to the space dimension and a value type set to MED_REEL64"));

  END_OF(LOC);
}

/*!
  Fill array by using T_Analytic.
  WARNING : "this" must have allocated its array by setting this->_support and this->_numberOfComponents properly.
  Typically you should use it on a field built with constructor FIELD<T>::FIELD<T>(SUPPORT *,int nbOfComponents)
 */
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::fillFromAnalytic(myFuncType f) throw (MEDEXCEPTION)
{
  const char * LOC = "void FIELD<T, INTERLACING_TAG>::fillFromAnalytic(myFuncType f) : ";
  int i,j;
  if (_support == (SUPPORT *) NULL)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"No Support defined."));

  MESH * mesh = _support->getMesh();
  int spaceDim = mesh->getSpaceDimension();
  const double * coord;

  const double * bary;
  FIELD<double,FullInterlace> * barycenterField=0;

  double ** xyz=new double* [spaceDim];
  bool deallocateXyz=false;
  if(_support->getEntity()==MED_EN::MED_NODE)
    {
      if (_support->isOnAllElements())
	{
	  coord=mesh->getCoordinates(MED_EN::MED_NO_INTERLACE);
	  for(i=0; i<spaceDim; i++)
	    xyz[i]=(double *)coord+i*_numberOfValues;
	}
      else
	{
	  coord = mesh->getCoordinates(MED_EN::MED_FULL_INTERLACE);
	  const int * nodesNumber=_support->getNumber(MED_EN::MED_ALL_ELEMENTS);
	  for(i=0; i<spaceDim; i++)
	    xyz[i]=new double[_numberOfValues];
	  deallocateXyz=true;
	  for(i=0;i<_numberOfValues;i++)
	    {
	      for(j=0;j<spaceDim;j++)
		xyz[j][i]=coord[(nodesNumber[i]-1)*spaceDim+j];
	    }
	}
    }
  else
    {
      barycenterField = mesh->getBarycenter(_support);
      bary=barycenterField->getValue();
      for(i=0; i<spaceDim; i++)
	xyz[i]=(double *)(bary+i*_numberOfValues);
    }
  T* valsToSet=(T*)getValue();
  double *temp=new double[spaceDim];
  for(i=0;i<_numberOfValues;i++)
  {
    for(j=0;j<spaceDim;j++)
      temp[j]=xyz[j][i];
    f(temp,valsToSet+i*_numberOfComponents);
  }
  delete [] temp;
  if(barycenterField)
    delete barycenterField;
  if(deallocateXyz)
    for(j=0;j<spaceDim;j++)
      delete [] xyz[j];
  delete [] xyz;
}

}//End namespace MEDMEM

#endif /* FIELD_HXX */
