#ifndef MED_FIELD_DRIVER_HXX
#define MED_FIELD_DRIVER_HXX

#include <string>

#include "MEDMEM_define.hxx"

#include "MEDMEM_GenDriver.hxx"
#include "utilities.h"

#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Unit.hxx"
#include "MEDMEM_Array.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Mesh.hxx"

namespace MEDMEM {
template <class T> class FIELD;

template <class T> class MED_FIELD_DRIVER : public GENDRIVER
{
protected:
  FIELD<T> *     _ptrField;
  string         _fieldName;
  int            _fieldNum;
 
  void search_field() ;
  
public :

  // all MED cell type ?? Classe de Définition ??
  //   static const medGeometryElement all_cell_type[MED_NBR_GEOMETRIE_MAILLE];
  
  //   static const char * const all_cell_type_tab [MED_NBR_GEOMETRIE_MAILLE];
  
  /*!
    Constructor.
  */
  MED_FIELD_DRIVER():_ptrField((FIELD<T> *)MED_NULL),
                     _fieldName(""),_fieldNum(MED_INVALID) {}
  /*!
    Constructor.
  */
  MED_FIELD_DRIVER(const string & fileName, FIELD<T> * ptrField, 
		   MED_EN::med_mode_acces accessMode)
    : GENDRIVER(fileName,accessMode),
      _ptrField((FIELD<T> *) ptrField), 
      _fieldName(fileName),_fieldNum(MED_INVALID) 
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
  virtual ~MED_FIELD_DRIVER() { 
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
  friend class MED_FIELD_RDWR_DRIVER;
  friend class MED_FIELD_RDONLY_DRIVER;
  friend class MED_FIELD_WRONLY_DRIVER;
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
  IMED_FIELD_RDONLY_DRIVER(const string & fileName,  FIELD<T> * ptrField):
    MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::MED_RDONLY) { 
    BEGIN_OF("MED_FIELD_RDONLY_DRIVER::MED_FIELD_RDONLY_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
    END_OF("MED_FIELD_RDONLY_DRIVER::MED_FIELD_RDONLY_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
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

  friend class MED_FIELD_RDONLY_DRIVER;
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
  IMED_FIELD_WRONLY_DRIVER(const string & fileName, FIELD<T> * ptrField):
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

  friend class MED_FIELD_WRONLY_DRIVER;
};


/*!

  Driver Med for FIELD : Read write.
  - Use read method from MED_FIELD_RDONLY_DRIVER
  - Use write method from MED_FIELD_WDONLY_DRIVER

*/

template <class T> class IMED_FIELD_RDWR_DRIVER : public IMED_FIELD_RDONLY_DRIVER<T>, public IMED_FIELD_WRONLY_DRIVER<T> {
  
public :
  
  /*!
    Constructor.
  */
  IMED_FIELD_RDWR_DRIVER() {}
  
  /*!
    Constructor.
  */
  IMED_FIELD_RDWR_DRIVER(const string & fileName, FIELD<T> * ptrField):
    IMED_FIELD_RDONLY_DRIVER<T>(fileName,ptrField),IMED_FIELD_WRONLY_DRIVER<T>(fileName,ptrField),MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::MED_RDWR)
  {
    BEGIN_OF("MED_FIELD_RDWR_DRIVER::MED_FIELD_RDWR_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
    //_accessMode = MED_RDWR ;
    END_OF("MED_FIELD_RDWR_DRIVER::MED_FIELD_RDWR_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
  }

  /*!
    Copy constructor.
  */
  IMED_FIELD_RDWR_DRIVER(const IMED_FIELD_RDWR_DRIVER & fieldDriver):
    IMED_FIELD_RDONLY_DRIVER<T>(fieldDriver),IMED_FIELD_WRONLY_DRIVER<T>(fieldDriver),MED_FIELD_DRIVER<T>(fieldDriver) {}
  
  /*!
    Destructor.
  */
  ~IMED_FIELD_RDWR_DRIVER() {}

  friend class MED_FIELD_RDWR_DRIVER;
};

}

#include "MEDMEM_MedFieldDriver21.hxx"

namespace MEDMEM {

template <class T> class MED_FIELD_RDONLY_DRIVER : public virtual IMED_FIELD_RDONLY_DRIVER<T>
{
 
public :
  MED_FIELD_RDONLY_DRIVER() { 
    _concreteDriverRd=new MED_FIELD_RDONLY_DRIVER21<T>; 
}
  MED_FIELD_RDONLY_DRIVER(const string & fileName,  FIELD<T> * ptrField):
    IMED_FIELD_RDONLY_DRIVER<T>(fileName,ptrField) { 
    BEGIN_OF("MED_FIELD_RDONLY_DRIVER::MED_FIELD_RDONLY_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
    _concreteDriverRd=new MED_FIELD_RDONLY_DRIVER21<T>(fileName,ptrField);
    END_OF("MED_FIELD_RDONLY_DRIVER::MED_FIELD_RDONLY_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
  }
  MED_FIELD_RDONLY_DRIVER(const MED_FIELD_RDONLY_DRIVER & fieldDriver) { 
    _concreteDriverRd=new MED_FIELD_RDONLY_DRIVER21<T>; 
}
  virtual ~MED_FIELD_RDONLY_DRIVER() { delete _concreteDriverRd; }
  virtual void read ( void ) throw (MEDEXCEPTION) { _concreteDriverRd->read(); }
  virtual void write( void ) const throw (MEDEXCEPTION) { _concreteDriverRd->write(); }
  virtual void open() throw (MEDEXCEPTION) { _concreteDriverRd->open(); }
  virtual void close() { _concreteDriverRd->close(); }
  virtual void   setFieldName(const string & fieldName) { _concreteDriverRd->setFieldName(fieldName); }
  virtual string getFieldName() const { return _concreteDriverRd->getFieldName(); }
private:
  virtual GENDRIVER * copy ( void ) const { return _concreteDriverRd->copy(); }
protected:
  IMED_FIELD_RDONLY_DRIVER<T> *_concreteDriverRd;
};

template <class T> class MED_FIELD_WRONLY_DRIVER : public virtual IMED_FIELD_WRONLY_DRIVER<T> { 
public :
  MED_FIELD_WRONLY_DRIVER() { _concreteDriverWr=new MED_FIELD_WRONLY_DRIVER21<T>; }
  MED_FIELD_WRONLY_DRIVER(const string & fileName, FIELD<T> * ptrField)
  {
    BEGIN_OF("MED_FIELD_WRONLY_DRIVER::MED_FIELD_WRONLY_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
    _concreteDriverWr=new MED_FIELD_WRONLY_DRIVER21<T>(fileName,ptrField);
    END_OF("MED_FIELD_WRONLY_DRIVER::MED_FIELD_WRONLY_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
  }
  MED_FIELD_WRONLY_DRIVER(const MED_FIELD_WRONLY_DRIVER & fieldDriver) { _concreteDriverWr=new MED_FIELD_WRONLY_DRIVER21<T>; }
  virtual ~MED_FIELD_WRONLY_DRIVER() { delete _concreteDriverWr; }
  virtual void read ( void ) throw (MEDEXCEPTION) { _concreteDriverWr->read(); }
  virtual void write( void ) const throw (MEDEXCEPTION) { _concreteDriverWr->write(); }
  virtual void open() throw (MEDEXCEPTION) { _concreteDriverWr->open(); }
  virtual void close() { _concreteDriverWr->close(); }
  virtual void   setFieldName(const string & fieldName) { _concreteDriverWr->setFieldName(fieldName); }
  virtual string getFieldName() const { return _concreteDriverWr->getFieldName(); }
private:
  virtual GENDRIVER * copy ( void ) const { return _concreteDriverWr->copy(); }
protected:
  IMED_FIELD_WRONLY_DRIVER<T> *_concreteDriverWr;
};

template <class T> class MED_FIELD_RDWR_DRIVER : public virtual IMED_FIELD_RDWR_DRIVER<T> {
public:
  MED_FIELD_RDWR_DRIVER() {
    _concreteDriverRdWr=new MED_FIELD_RDWR_DRIVER21<T>;
  }
  /*!
    Constructor.
  */
  MED_FIELD_RDWR_DRIVER(const string & fileName, FIELD<T> * ptrField)
  {
    BEGIN_OF("MED_FIELD_RDWR_DRIVER::MED_FIELD_RDWR_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
    _concreteDriverRdWr=new MED_FIELD_RDWR_DRIVER21<T>(fileName,ptrField);
    END_OF("MED_FIELD_RDWR_DRIVER::MED_FIELD_RDWR_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
  }
  MED_FIELD_RDWR_DRIVER(const MED_FIELD_RDWR_DRIVER & fieldDriver)
  {
    _concreteDriverRdWr=new MED_FIELD_RDWR_DRIVER21<T>;//(fieldDriver);
  }
  ~MED_FIELD_RDWR_DRIVER() { delete _concreteDriverRdWr; }
  virtual void read ( void ) throw (MEDEXCEPTION) { _concreteDriverRdWr->read(); }
  virtual void write( void ) const throw (MEDEXCEPTION) { _concreteDriverRdWr->write(); }
  virtual void open() throw (MEDEXCEPTION) { _concreteDriverRdWr->open(); }
  virtual void close() { _concreteDriverRdWr->close(); }
  virtual void   setFieldName(const string & fieldName) { _concreteDriverRdWr->setFieldName(fieldName); }
  virtual string getFieldName() const { return _concreteDriverRdWr->getFieldName(); }
private:
  virtual GENDRIVER * copy ( void ) const { return _concreteDriverRdWr->copy(); }
protected:
  IMED_FIELD_RDWR_DRIVER<T> *_concreteDriverRdWr;
};

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
  
/*--------------------- RDONLY PART -------------------------------*/

template <class T> void IMED_FIELD_RDONLY_DRIVER<T>::write( void ) const
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("MED_FIELD_RDONLY_DRIVER::write : Can't write with a RDONLY driver !");
}

/*--------------------- WRONLY PART -------------------------------*/

template <class T> void IMED_FIELD_WRONLY_DRIVER<T>::read (void)
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("MED_FIELD_WRONLY_DRIVER::read : Can't read with a WRONLY driver !");
}

/*--------------------- RDWR PART -------------------------------*/


}//End namespace MEDMEM
/*-----------------------------------------------------------------*/

#endif /* MED_FIELD_DRIVER_HXX */

