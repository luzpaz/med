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
#ifndef MED_FIELD_DRIVER_HXX
#define MED_FIELD_DRIVER_HXX

#include <string>

#include "MEDMEM_define.hxx"

#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_Utilities.hxx"

#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
// #include "MEDMEM_Unit.hxx"
// #include "MEDMEM_Array.hxx"
// #include "MEDMEM_Support.hxx"
// #include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Compatibility21_22.hxx"
#include "MEDMEM_FieldForward.hxx"

namespace MEDMEM {
template <class T> class MED_FIELD_RDWR_DRIVER;
template <class T> class MED_FIELD_RDONLY_DRIVER;
template <class T> class MED_FIELD_WRONLY_DRIVER;

template <class T> class MED_FIELD_DRIVER : public GENDRIVER
{
protected:
  // Developement plus propre :
  // - Il faudrait soit utiliser le type FIELD_ et ajouter à cette classe
  //   les accesseurs de FIELD<> utilisées dans les drivers
  // - Ou bien avoir des drivers à deux paramètres template (le top)
  // - Remarquez l'affreux cast dans le second constructeur :
  //      _ptrField( (FIELD<T> *) ptrField )
  //   Cela cast toujours le ptrField en FullInterlace
  //   Cela ne pose cependant pas de pb de fonctionement aux drivers
  FIELD<T> *     _ptrField;
  string         _fieldName;
  int            _fieldNum;

public :

  /*!
    Constructor.
  */
  MED_FIELD_DRIVER():_ptrField((FIELD<T> *) MED_NULL),
                     _fieldName(""),_fieldNum(MED_INVALID)
  {}
  /*!
    Constructor.
  */
  template <class INTERLACING_TAG>
  MED_FIELD_DRIVER(const string & fileName, FIELD<T, INTERLACING_TAG> * ptrField,
		   MED_EN::med_mode_acces accessMode)
    : GENDRIVER(fileName,accessMode),
      _ptrField((FIELD<T> *) ptrField),
      _fieldName(""),_fieldNum(MED_INVALID)
  {
  }

  /*!
    Copy constructor.
  */
  MED_FIELD_DRIVER(const MED_FIELD_DRIVER & fieldDriver):
    GENDRIVER(fieldDriver),
    _ptrField(fieldDriver._ptrField),
    _fieldName(fieldDriver._fieldName),
    _fieldNum(fieldDriver._fieldNum)
  {
  }

  /*!
    Destructor.
  */
  virtual ~MED_FIELD_DRIVER()
  {
    MESSAGE("MED_FIELD_DRIVER::~MED_FIELD_DRIVER() has been destroyed");
  }

  virtual void open() throw (MEDEXCEPTION) = 0;
  virtual void close() =0;
  virtual void write( void ) const = 0 ;
  virtual void read ( void ) = 0 ;
  /*!
    Set the name of the FIELD asked in file.

    It could be different than the name of the FIELD object.
  */
  virtual void   setFieldName(const string & fieldName) { _fieldName = fieldName; }
  /*!
    Get the name of the FIELD asked in file.
  */
  virtual string getFieldName() const { return _fieldName; }
protected:
  virtual GENDRIVER * copy ( void ) const = 0 ;
  friend class MED_FIELD_RDWR_DRIVER<T>;
  friend class MED_FIELD_RDONLY_DRIVER<T>;
  friend class MED_FIELD_WRONLY_DRIVER<T>;
};

/*!

  Driver Med for FIELD : Read only.

  Implement read method.

*/

template <class T> class IMED_FIELD_RDONLY_DRIVER : public virtual MED_FIELD_DRIVER<T>
{

public :

  /*!
    Constructor.
  */
  IMED_FIELD_RDONLY_DRIVER() {}

  /*!
    Constructor.
  */
  template <class INTERLACING_TAG>
  IMED_FIELD_RDONLY_DRIVER(const string & fileName,
			   FIELD<T, INTERLACING_TAG> * ptrField):
    MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::MED_RDONLY)
  {
    BEGIN_OF("IMED_FIELD_RDONLY_DRIVER::IMED_FIELD_RDONLY_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
    END_OF("IMED_FIELD_RDONLY_DRIVER::IMED_FIELD_RDONLY_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
  }

  /*!
    Copy constructor.
  */
  IMED_FIELD_RDONLY_DRIVER(const IMED_FIELD_RDONLY_DRIVER & fieldDriver):
    MED_FIELD_DRIVER<T>(fieldDriver) {}

  /*!
    Destructor.
  */
  virtual ~IMED_FIELD_RDONLY_DRIVER() {}

  // CREER UNE METHODE POUR LIRE LA LISTE DES MAILLAGES .....

  /*!
    Return a MEDEXCEPTION : it is the read-only driver.
  */
  void write( void ) const throw (MEDEXCEPTION) ;

  friend class MED_FIELD_RDONLY_DRIVER<T>;
};

/*!

  Driver Med for FIELD : Write only.

  Implement write method.

*/

template <class T> class IMED_FIELD_WRONLY_DRIVER : public virtual MED_FIELD_DRIVER<T> {

public :

  /*!
    Constructor.
  */
  IMED_FIELD_WRONLY_DRIVER() {}

  /*!
    Constructor.
  */
  template <class INTERLACING_TAG>
  IMED_FIELD_WRONLY_DRIVER(const string & fileName,
			   FIELD<T, INTERLACING_TAG> * ptrField):
    MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::MED_WRONLY)
  {
    BEGIN_OF("MED_FIELD_WRONLY_DRIVER::MED_FIELD_WRONLY_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
    END_OF("MED_FIELD_WRONLY_DRIVER::MED_FIELD_WRONLY_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
  }

  /*!
    Copy constructor.
  */
  IMED_FIELD_WRONLY_DRIVER(const IMED_FIELD_WRONLY_DRIVER & fieldDriver):
    MED_FIELD_DRIVER<T>(fieldDriver) {}

  /*!
    Destructor.
  */
  virtual ~IMED_FIELD_WRONLY_DRIVER() {};
  /*!
    Return a MEDEXCEPTION : it is the write-only driver.
  */
  void read ( void ) throw (MEDEXCEPTION) ;

  friend class MED_FIELD_WRONLY_DRIVER<T>;
};


/*!

  Driver Med for FIELD : Read write.
  - Use read method from MED_FIELD_RDONLY_DRIVER
  - Use write method from MED_FIELD_WDONLY_DRIVER

*/

template <class T> class IMED_FIELD_RDWR_DRIVER : public virtual IMED_FIELD_RDONLY_DRIVER<T>,
						  public virtual IMED_FIELD_WRONLY_DRIVER<T>
{

public :

  /*!
    Constructor.
  */
  IMED_FIELD_RDWR_DRIVER() {}

  /*!
    Constructor.
  */
  template <class INTERLACING_TAG>
  IMED_FIELD_RDWR_DRIVER(const string & fileName,
			 FIELD<T, INTERLACING_TAG> * ptrField):
    IMED_FIELD_RDONLY_DRIVER<T>(fileName,ptrField),
    IMED_FIELD_WRONLY_DRIVER<T>(fileName,ptrField),
    MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::MED_RDWR)
  {
    BEGIN_OF("MED_FIELD_RDWR_DRIVER::MED_FIELD_RDWR_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
    //_accessMode = MED_RDWR ;
    END_OF("MED_FIELD_RDWR_DRIVER::MED_FIELD_RDWR_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
  }

  /*!
    Copy constructor.
  */
  IMED_FIELD_RDWR_DRIVER(const IMED_FIELD_RDWR_DRIVER & fieldDriver):
    IMED_FIELD_RDONLY_DRIVER<T>(fieldDriver),
    IMED_FIELD_WRONLY_DRIVER<T>(fieldDriver),
    MED_FIELD_DRIVER<T>(fieldDriver)
  {
  }

  /*!
    Destructor.
  */
  ~IMED_FIELD_RDWR_DRIVER() {}

  friend class MED_FIELD_RDWR_DRIVER<T>;
};

}

namespace MEDMEM {

template <class T> class MED_FIELD_RDONLY_DRIVER : public virtual IMED_FIELD_RDONLY_DRIVER<T>
{

public :
  MED_FIELD_RDONLY_DRIVER();

  template <class INTERLACING_TAG>
  MED_FIELD_RDONLY_DRIVER(const string & fileName,
			  FIELD<T, INTERLACING_TAG> * ptrField);

  MED_FIELD_RDONLY_DRIVER(const MED_FIELD_RDONLY_DRIVER & fieldDriver):IMED_FIELD_RDONLY_DRIVER<T>(fieldDriver) { _concreteFieldDrv = fieldDriver._concreteFieldDrv->copy(); }
  virtual ~MED_FIELD_RDONLY_DRIVER() {     if (_concreteFieldDrv) delete _concreteFieldDrv; }
  virtual void read ( void ) throw (MEDEXCEPTION) { _concreteFieldDrv->read(); }
  virtual MED_EN::med_mode_acces getAccessMode() const { return _concreteFieldDrv->getAccessMode(); }
  virtual void write( void ) const throw (MEDEXCEPTION) { _concreteFieldDrv->write(); }
  virtual void open() throw (MEDEXCEPTION) { _concreteFieldDrv->open(); }
  virtual void close() { _concreteFieldDrv->close(); }
  virtual void   setFieldName(const string & fieldName) { _concreteFieldDrv->setFieldName(fieldName); }
  virtual string getFieldName() const { return MED_FIELD_DRIVER<T>::getFieldName(); }
  
private:
  virtual GENDRIVER * copy ( void ) const { return new MED_FIELD_RDONLY_DRIVER<T>(*this); }
protected:
  GENDRIVER * _concreteFieldDrv;
};

template <class T> class MED_FIELD_WRONLY_DRIVER : public virtual IMED_FIELD_WRONLY_DRIVER<T> {
public :
  MED_FIELD_WRONLY_DRIVER();

  template <class INTERLACING_TAG>
  MED_FIELD_WRONLY_DRIVER(const string & fileName,
			  FIELD<T, INTERLACING_TAG> * ptrField);

  MED_FIELD_WRONLY_DRIVER(const MED_FIELD_WRONLY_DRIVER & fieldDriver):IMED_FIELD_WRONLY_DRIVER<T>(fieldDriver) { _concreteFieldDrv = fieldDriver._concreteFieldDrv->copy(); }
  virtual ~MED_FIELD_WRONLY_DRIVER() {    if (_concreteFieldDrv) delete _concreteFieldDrv;}
  virtual void read ( void ) throw (MEDEXCEPTION) { _concreteFieldDrv->read(); }
  virtual MED_EN::med_mode_acces getAccessMode() const { return _concreteFieldDrv->getAccessMode(); }
  virtual void write( void ) const throw (MEDEXCEPTION) { _concreteFieldDrv->write(); }
  virtual void open() throw (MEDEXCEPTION) { _concreteFieldDrv->open(); }
  virtual void close() { _concreteFieldDrv->close(); }
  virtual void   setFieldName(const string & fieldName) { _concreteFieldDrv->setFieldName(fieldName); }
  virtual string getFieldName() const { return MED_FIELD_DRIVER<T>::getFieldName(); }
private:
  virtual GENDRIVER * copy ( void ) const { return new MED_FIELD_WRONLY_DRIVER<T>(*this); }
protected:
  GENDRIVER * _concreteFieldDrv;
};

template <class T> class MED_FIELD_RDWR_DRIVER : public virtual IMED_FIELD_RDWR_DRIVER<T> {
public:
  MED_FIELD_RDWR_DRIVER();
  /*!
    Constructor.
  */
  template <class INTERLACING_TAG>
  MED_FIELD_RDWR_DRIVER(const string & fileName,
			FIELD<T, INTERLACING_TAG> * ptrField);

  MED_FIELD_RDWR_DRIVER(const MED_FIELD_RDWR_DRIVER & fieldDriver):
    IMED_FIELD_RDWR_DRIVER<T>(fieldDriver)
  { _concreteFieldDrv = fieldDriver._concreteFieldDrv->copy(); }
  ~MED_FIELD_RDWR_DRIVER() {    if (_concreteFieldDrv) delete _concreteFieldDrv;}
  virtual void read ( void ) throw (MEDEXCEPTION) { _concreteFieldDrv->read(); }
  virtual MED_EN::med_mode_acces getAccessMode() const { return _concreteFieldDrv->getAccessMode(); }
  virtual void write( void ) const throw (MEDEXCEPTION) { _concreteFieldDrv->write(); }
  virtual void open() throw (MEDEXCEPTION) { _concreteFieldDrv->open(); }
  virtual void close() { _concreteFieldDrv->close(); }
  virtual void   setFieldName(const string & fieldName) { _concreteFieldDrv->setFieldName(fieldName); }
  virtual string getFieldName() const { return MED_FIELD_DRIVER<T>::getFieldName(); }
private:
  virtual GENDRIVER * copy ( void ) const { return new MED_FIELD_RDWR_DRIVER<T>(*this); }
protected:
  GENDRIVER * _concreteFieldDrv;
};
}

/*-------------------------*/
/* template implementation */
/*-------------------------*/
/*--------------------- DRIVER PART -------------------------------*/

// template <class T> void MED_FIELD_DRIVER<T>::setFieldName(const string & fieldName)
// {
//   _fieldName = fieldName;
// }

// template <class T> string  MED_FIELD_DRIVER<T>::getFieldName() const
// {
//   return _fieldName;
// }

#include "MEDMEM_DriverFactory.hxx"
#include "MEDMEM_MedFieldDriver21.hxx"
#include "MEDMEM_MedFieldDriver22.hxx"


/*--------------------- RDONLY PART -------------------------------*/
namespace MEDMEM {

template <class T> void IMED_FIELD_RDONLY_DRIVER<T>::write( void ) const
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("MED_FIELD_RDONLY_DRIVER::write : Can't write with a RDONLY driver !");
}

template <class T>  MED_FIELD_RDONLY_DRIVER<T>::MED_FIELD_RDONLY_DRIVER() {
    MESSAGE("You are using the default constructor of the Field read only Driver and it is 2.1 one");
    _concreteFieldDrv=new MED_FIELD_RDONLY_DRIVER21<T>();
}

template <class T> template < class INTERLACING_TAG >
MED_FIELD_RDONLY_DRIVER<T>::MED_FIELD_RDONLY_DRIVER(const string & fileName,
						    FIELD<T, INTERLACING_TAG> * ptrField):
  MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::MED_RDONLY),
  IMED_FIELD_RDONLY_DRIVER<T>(fileName,ptrField)
{
    BEGIN_OF("MED_FIELD_RDONLY_DRIVER::MED_FIELD_RDONLY_DRIVER(const string & fileName, const FIELD<T> * ptrField)");

    _concreteFieldDrv = DRIVERFACTORY::buildFieldDriverFromFile(fileName,ptrField,MED_EN::MED_LECT);

    END_OF("MED_FIELD_RDONLY_DRIVER::MED_FIELD_RDONLY_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
}

/*--------------------- WRONLY PART -------------------------------*/

template <class T> void IMED_FIELD_WRONLY_DRIVER<T>::read (void)
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("MED_FIELD_WRONLY_DRIVER::read : Can't read with a WRONLY driver !");
}

template <class T>  MED_FIELD_WRONLY_DRIVER<T>::MED_FIELD_WRONLY_DRIVER() {
    MESSAGE("You are using the default constructor of the Field write only Driver and it is 2.1 one");

    _concreteFieldDrv=new MED_FIELD_WRONLY_DRIVER21<T>();
}

template <class T> template < class INTERLACING_TAG >
MED_FIELD_WRONLY_DRIVER<T>::MED_FIELD_WRONLY_DRIVER(const string & fileName,
						    FIELD<T, INTERLACING_TAG> * ptrField):
  MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::MED_WRONLY),
  IMED_FIELD_WRONLY_DRIVER<T>( fileName, ptrField)
{
  BEGIN_OF("MED_FIELD_WRONLY_DRIVER::MED_FIELD_WRONLY_DRIVER(const string & fileName, const FIELD<T> * ptrField)");

  _concreteFieldDrv = DRIVERFACTORY::buildFieldDriverFromFile(fileName,ptrField,MED_EN::MED_ECRI);

  END_OF("MED_FIELD_RDONLY_DRIVER::MED_FIELD_RDONLY_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
}

/*--------------------- RDWR PART -------------------------------*/

template <class T>  MED_FIELD_RDWR_DRIVER<T>::MED_FIELD_RDWR_DRIVER() {
    MESSAGE("You are using the default constructor of the Field read/write Driver and it is 2.1 one");

    _concreteFieldDrv=new MED_FIELD_RDWR_DRIVER21<T>();
}

template <class T> template < class INTERLACING_TAG >
MED_FIELD_RDWR_DRIVER<T>::MED_FIELD_RDWR_DRIVER(const string & fileName,
						FIELD<T, INTERLACING_TAG> * ptrField):
  MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::MED_REMP),
  IMED_FIELD_RDWR_DRIVER<T>(fileName,ptrField)
{
  BEGIN_OF("MED_FIELD_RDWR_DRIVER::MED_FIELD_RDWR_DRIVER(const string & fileName, const FIELD<T> * ptrField)");

  _concreteFieldDrv = DRIVERFACTORY::buildFieldDriverFromFile(fileName,ptrField,MED_EN::MED_REMP);

  END_OF("MED_FIELD_RDWR_DRIVER::MED_FIELD_RDWR_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
}


}//End namespace MEDMEM
/*-----------------------------------------------------------------*/

#endif /* MED_FIELD_DRIVER_HXX */
