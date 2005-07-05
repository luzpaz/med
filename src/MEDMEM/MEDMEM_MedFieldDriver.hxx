#ifndef MED_FIELD_DRIVER_HXX
#define MED_FIELD_DRIVER_HXX

#include <string>

#include "MEDMEM_define.hxx"
#include "MEDMEM_GenDriver.hxx"
#include "utilities.h"

#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Unit.hxx"

#include "MEDMEM_ArrayInterface.hxx"
#include "MEDMEM_ArrayConvert.hxx"

#include "MEDMEM_Support.hxx"
//#include "MEDMEM_Field.hxx"
#include "MEDMEM_Mesh.hxx"

namespace MEDMEM {
  //template <class T,class INTERLACING_TAG=FullInterlace> class FIELD;

// A QD LA CLASSE MED_ALL_ELEMENTS_DRIVER.... :) pour mutualiser le open ..... avec led _medIdt...

/*!

  Driver Med for FIELD.

  Generic part : implement open and close methods.

*/

template <class T> class MED_FIELD_DRIVER : public GENDRIVER
{
protected:

  FIELD<T> *     _ptrField;
  med_2_1::med_idt        _medIdt;
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
  MED_FIELD_DRIVER():GENDRIVER(),
                     _ptrField((FIELD<T> *)MED_NULL),_medIdt(MED_INVALID),
                     _fieldName(""),_fieldNum(MED_INVALID) {}
  /*!
    Constructor.
  */
  MED_FIELD_DRIVER(const string & fileName, FIELD<T> * ptrField,
		   MED_EN::med_mode_acces accessMode)
    : GENDRIVER(fileName,accessMode),
      _ptrField((FIELD<T> *) ptrField),_medIdt(MED_INVALID),
      _fieldName(fileName),_fieldNum(MED_INVALID)
  {
  }

  /*!
    Copy constructor.
  */
  MED_FIELD_DRIVER(const MED_FIELD_DRIVER & fieldDriver):
    GENDRIVER(fieldDriver),
    _ptrField(fieldDriver._ptrField),
    _medIdt(MED_INVALID),
    _fieldName(fieldDriver._fieldName),
    _fieldNum(fieldDriver._fieldNum)
  {
  }

  /*!
    Destructor.
  */
  virtual ~MED_FIELD_DRIVER() {
  }

  void open() throw (MEDEXCEPTION)
  {
    const char * LOC = "MED_FIELD_DRIVER::open() ";
    BEGIN_OF(LOC);

    // we must set fieldname before open, because we must find field number in file (if it exist !!!)
    if ( _fileName == "" )
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC)
				       << "_fileName is |\"\"|, please set a correct fileName before calling open()"
				       )
			    );

    MESSAGE(LOC<<"_fileName.c_str : "<< _fileName.c_str()<<",mode : "<< _accessMode);
    _medIdt = med_2_1::MEDouvrir( (const_cast <char *> (_fileName.c_str())),(med_2_1::med_mode_acces) _accessMode);
    MESSAGE(LOC<<"_medIdt : "<< _medIdt );
    if (_medIdt > 0)
      _status=MED_OPENED;
    else {
      _status = MED_INVALID;
      _medIdt = MED_INVALID;
      throw MED_EXCEPTION (LOCALIZED( STRING(LOC)
				      << "Can't open |"  << _fileName
				      << "|, _medIdt : " << _medIdt
				      )
			   );
    }

    END_OF(LOC);
  }

  void close() {
    BEGIN_OF("MED_FIELD_DRIVER::close()");
    med_2_1::med_int err = 0;
    if (_status == MED_OPENED) {
      err=med_2_1::MEDfermer(_medIdt);
      //H5close(); // If we call H5close() all the files are closed.
      _status = MED_CLOSED;
      _medIdt = MED_INVALID;
      MESSAGE(" MED_FIELD_DRIVER::close() : MEDfermer : _medIdt= " << _medIdt );
      MESSAGE(" MED_FIELD_DRIVER::close() : MEDfermer : err    = " << err );
    }
    END_OF("MED_FIELD_DRIVER::close()");
  }

  virtual void write( void ) const = 0 ;
  virtual void read ( void ) = 0 ;

  /*!
    Set the name of the FIELD asked in file.

    It could be different than the name of the FIELD object.
  */
  void   setFieldName(const string & fieldName) ;
  /*!
    Get the name of the FIELD asked in file.
  */
  string getFieldName() const ;

private:
  virtual GENDRIVER * copy ( void ) const = 0 ;

};

/*!

  Driver Med for FIELD : Read only.

  Implement read method.

*/

template <class T> class MED_FIELD_RDONLY_DRIVER : public virtual MED_FIELD_DRIVER<T>
{

public :

  /*!
    Constructor.
  */
  MED_FIELD_RDONLY_DRIVER():MED_FIELD_DRIVER<T>() {};

  /*!
    Constructor.
  */
  MED_FIELD_RDONLY_DRIVER(const string & fileName,  FIELD<T> * ptrField):
    MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::MED_RDONLY) {
    BEGIN_OF("MED_FIELD_RDONLY_DRIVER::MED_FIELD_RDONLY_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
    END_OF("MED_FIELD_RDONLY_DRIVER::MED_FIELD_RDONLY_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
  }

  /*!
    Copy constructor.
  */
  MED_FIELD_RDONLY_DRIVER(const MED_FIELD_RDONLY_DRIVER & fieldDriver):
    MED_FIELD_DRIVER<T>(fieldDriver) {};

  /*!
    Destructor.
  */
  virtual ~MED_FIELD_RDONLY_DRIVER() {};

  // CREER UNE METHODE POUR LIRE LA LISTE DES MAILLAGES .....

  /*!
    Return a MEDEXCEPTION : it is the read-only driver.
  */
  void write( void ) const throw (MEDEXCEPTION) ;
  /*!
    Read FIELD in the specified file.
  */
  void read ( void ) throw (MEDEXCEPTION) ;

private:
  GENDRIVER * copy( void ) const ;

};

/*!

  Driver Med for FIELD : Write only.

  Implement write method.

*/

template <class T> class MED_FIELD_WRONLY_DRIVER : public virtual MED_FIELD_DRIVER<T> {

public :

  /*!
    Constructor.
  */
  MED_FIELD_WRONLY_DRIVER():MED_FIELD_DRIVER<T>() {}

  /*!
    Constructor.
  */
  MED_FIELD_WRONLY_DRIVER(const string & fileName, FIELD<T> * ptrField):
    MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::MED_WRONLY)
  {
    BEGIN_OF("MED_FIELD_WRONLY_DRIVER::MED_FIELD_WRONLY_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
    END_OF("MED_FIELD_WRONLY_DRIVER::MED_FIELD_WRONLY_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
  };

  /*!
    Copy constructor.
  */
  MED_FIELD_WRONLY_DRIVER(const MED_FIELD_WRONLY_DRIVER & fieldDriver):
    MED_FIELD_DRIVER<T>(fieldDriver) {};

  /*!
    Destructor.
  */
  virtual ~MED_FIELD_WRONLY_DRIVER() {};

  /*!
    Write FIELD in the specified file.
  */
  void write( void ) const throw (MEDEXCEPTION) ;
  /*!
    Return a MEDEXCEPTION : it is the write-only driver.
  */
  void read ( void ) throw (MEDEXCEPTION) ;

private:
  GENDRIVER * copy( void ) const ;

};


/*!

  Driver Med for FIELD : Read write.
  - Use read method from MED_FIELD_RDONLY_DRIVER
  - Use write method from MED_FIELD_WDONLY_DRIVER

*/

template <class T> class MED_FIELD_RDWR_DRIVER : public MED_FIELD_RDONLY_DRIVER<T>, public MED_FIELD_WRONLY_DRIVER<T> {

public :

  /*!
    Constructor.
  */
  MED_FIELD_RDWR_DRIVER():MED_FIELD_DRIVER<T>() {}

  /*!
    Constructor.
  */
  MED_FIELD_RDWR_DRIVER(const string & fileName, FIELD<T> * ptrField):
    MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::MED_RDWR)
  {
    BEGIN_OF("MED_FIELD_RDWR_DRIVER::MED_FIELD_RDWR_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
    //_accessMode = MED_RDWR ;
    END_OF("MED_FIELD_RDWR_DRIVER::MED_FIELD_RDWR_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
  };

  /*!
    Copy constructor.
  */
  MED_FIELD_RDWR_DRIVER(const MED_FIELD_RDWR_DRIVER & fieldDriver):
    MED_FIELD_DRIVER<T>(fieldDriver) {};

  /*!
    Destructor.
  */
  ~MED_FIELD_RDWR_DRIVER() {};

  /*!
    Write FIELD in the specified file.
  */
  void write(void) const throw (MEDEXCEPTION) ;
  /*!
    Read FIELD in the specified file.
  */
  void read (void) throw (MEDEXCEPTION) ;

private:
  GENDRIVER * copy( void ) const ;

};


/*-------------------------*/
/* template implementation */
/*-------------------------*/
/*--------------------- DRIVER PART -------------------------------*/

template <class T> void MED_FIELD_DRIVER<T>::setFieldName(const string & fieldName)
{
  _fieldName = fieldName; 
}

template <class T> string  MED_FIELD_DRIVER<T>::getFieldName() const
{
  return _fieldName;
}

//  template <class T> void MED_FIELD_DRIVER<T>::search_field() {
//    const char * LOC = "template <class T> class MED_FIELD_DRIVER::search_field() :";
    
//    // we search the field number !!!!
//    if (_status==MED_OPENED)
//      if (_fieldNum==MED_INVALID) {
//        int err ;
//        int    numberOfFields              = 0;      //MED_INVALID
//        //	char   fieldName[MED_TAILLE_NOM+1] = "";
//        char   fieldName[MED_TAILLE_NOM+1] ;
//        int    numberOfComponents          = 0;
//        char * componentName               = (char *) MED_NULL;
//        char * unitName                    = (char *) MED_NULL;
//        med_2_1::med_type_champ type ;
//        numberOfFields = med_2_1::MEDnChamp(_medIdt,0) ;
//        if ( numberOfFields <= 0 ) 
//  	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": No Field found !"));
//        for (int i=1;i<=numberOfFields;i++) {
	  
//  	numberOfComponents = med_2_1::MEDnChamp(_medIdt,i) ;
//  	if ( numberOfComponents <= 0 ) 
//  	  throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
//  					   <<  "Be careful there is no compound for field n°" 
//  					   << i << "in file |"<<_fileName<<"| !"));
	  
//  	componentName = new char[numberOfComponents*MED_TAILLE_PNOM+1] ;
//  	unitName      = new char[numberOfComponents*MED_TAILLE_PNOM+1] ;   
	  
//  	err = med_2_1::MEDchampInfo(_medIdt, i, fieldName, &type, componentName, 
//  				   unitName, numberOfComponents) ;
	  
//  	delete[] componentName ;
//  	delete[] unitName ;
//  	MESSAGE("Champ "<<i<<" : #" << fieldName <<"# et recherche #"<<_fieldName.c_str()<<"#");
//  	if ( !strcmp(fieldName,_fieldName.c_str()) ) {
//  	  MESSAGE("FOUND FIELD "<< fieldName <<" : "<<i);
//  	  _fieldNum = i ;
//  	  break ;
//  	}
//        }
//      }
//  }
  
/*--------------------- RDONLY PART -------------------------------*/

template <class T> GENDRIVER * MED_FIELD_RDONLY_DRIVER<T>::copy(void) const
{
  MED_FIELD_RDONLY_DRIVER<T> * myDriver = 
    new MED_FIELD_RDONLY_DRIVER<T>(*this);
  return myDriver ;
}


template <class T> void MED_FIELD_RDONLY_DRIVER<T>::read(void)
  throw (MEDEXCEPTION)
{
  const char * LOC = " MED_FIELD_RDONLY_DRIVER::read() " ;
  BEGIN_OF(LOC);

  typedef typename MEDMEM_ArrayInterface<T,NoInterlace,NoGauss>::Array ArrayNo;
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,NoGauss>::Array ArrayFull;

  if ( ( MED_FIELD_DRIVER<T>::_fieldName.empty()       ) &&
       ( MED_FIELD_DRIVER<T>::_ptrField->_name.empty() )    )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)
				 <<" neither <fieldName> is set in driver nor in object FIELD.")) ;

  // If _fieldName is not set in driver, try to use _ptrfield->_fieldName
  if ( ( MED_FIELD_DRIVER<T>::_fieldName.empty()       ) &&
       ( !MED_FIELD_DRIVER<T>::_ptrField->_name.empty() )    )
    MED_FIELD_DRIVER<T>::_fieldName=MED_FIELD_DRIVER<T>::_ptrField->_name;

  if ( MED_FIELD_DRIVER<T>::_fieldName.size() > MED_TAILLE_NOM )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)
				 <<" <fieldName> size in object driver FIELD is > MED_TAILLE_NOM ."));


  MESSAGE("###### "<<LOC<<" fieldNameDRIVER : "<<MED_FIELD_DRIVER<T>::_fieldName<<" fieldName : "<<MED_FIELD_DRIVER<T>::_fieldName);


  // EF : A CHANGER, le champ doit posséder le nom du maillage
  string MeshName =  MED_FIELD_DRIVER<T>::_ptrField->getSupport()->getMesh()->getName() ;

  if (MED_FIELD_DRIVER<T>::_status!=MED_OPENED) {
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": Method open must be called before method read.")) ;
    return;
  }


  //        search_field() ;
  char * fieldName = new char[MED_TAILLE_NOM+1] ;
  int    err ;
  int    numberOfComponents          = 0;
  char * componentName               = (char *) MED_NULL;
  char * unitName                    = (char *) MED_NULL;
  med_2_1::med_type_champ type ;

  // we search the number of the field  <fieldName>
  // Having found <fieldName> variables <numberOfComponents>,
  // <componentName>, <unitname>, <type> and attribute <_fieldNum> are set.
  if (MED_FIELD_DRIVER<T>::_fieldNum==MED_INVALID) {

    int numberOfFields = med_2_1::MEDnChamp(MED_FIELD_DRIVER<T>::_medIdt,0) ;
    if ( numberOfFields <= 0 )
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": There is no field in file"));

    for (int i=1;i<=numberOfFields;i++) {

      numberOfComponents = med_2_1::MEDnChamp(MED_FIELD_DRIVER<T>::_medIdt,i) ;
      if ( numberOfComponents <= 0 )
	MESSAGE(LOC<<"Be careful there is no compound for field n°"<<i<<"in file |"<<_fileName<<"| !");

      componentName = new char[numberOfComponents*MED_TAILLE_PNOM+1] ;
      unitName      = new char[numberOfComponents*MED_TAILLE_PNOM+1] ;

      err = med_2_1::MEDchampInfo(MED_FIELD_DRIVER<T>::_medIdt,
				  i, fieldName,
				  &type, componentName,unitName, numberOfComponents) ;

      MESSAGE("Field "<<i<<" : #" << fieldName <<"# , searching for #"<<MED_FIELD_DRIVER<T>::_fieldName.c_str()<<"#");
      if ( !strcmp(fieldName,MED_FIELD_DRIVER<T>::_fieldName.c_str()) ) {
	MESSAGE("FOUND FIELD "<< fieldName <<" : "<<i);
	MED_FIELD_DRIVER<T>::_fieldNum = i ;
	break ;
      }

      delete[] componentName ;
      delete[] unitName ;
    }
  }
  delete[] fieldName ;

  if (MED_FIELD_DRIVER<T>::_fieldNum==MED_INVALID)
    throw MEDEXCEPTION(LOCALIZED( STRING(LOC) << ": Field "<<MED_FIELD_DRIVER<T>::_fieldName << " not found in file " << MED_FIELD_DRIVER<T>::_fileName ) );
  MESSAGE ("FieldNum : "<<MED_FIELD_DRIVER<T>::_fieldNum);




  if (numberOfComponents < 1)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" no component found fo field "
				 << MED_FIELD_DRIVER<T>::_fieldName)) ;

  switch ( (med_2_1::med_type_champ) _ptrField->_valueType ) {
  case  med_2_1::MED_INT :
  case  med_2_1::MED_INT32 :
  case  med_2_1::MED_INT64 :
    if ( type == ( med_2_1::MED_REEL64 ) )
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Field Type in file (" << type
				   <<") differs from FIELD object type (" <<
				   _ptrField->_valueType << ")" )) ;
    break;
  default:
    break;
  }

  // If an error occurs while reading the field, allocated FIELD membre will be deleted
  MED_FIELD_DRIVER<T>::_ptrField->_name =  MED_FIELD_DRIVER<T>::_fieldName;
  MED_FIELD_DRIVER<T>::_ptrField->_numberOfComponents = numberOfComponents ;
  MED_FIELD_DRIVER<T>::_ptrField->_componentsTypes = new int[numberOfComponents] ;
  MED_FIELD_DRIVER<T>::_ptrField->_componentsNames = new string[numberOfComponents] ;
  MED_FIELD_DRIVER<T>::_ptrField->_componentsUnits = new UNIT[numberOfComponents] ;
  MED_FIELD_DRIVER<T>::_ptrField->_componentsDescriptions = new string[numberOfComponents] ;
  MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits = new string[numberOfComponents] ;
  for (int i=0; i<numberOfComponents; i++) {
    MED_FIELD_DRIVER<T>::_ptrField->_componentsTypes[i] = 1 ;
    MED_FIELD_DRIVER<T>::_ptrField->_componentsNames[i] = string(componentName,i*MED_TAILLE_PNOM,MED_TAILLE_PNOM) ;
    SCRUTE(MED_FIELD_DRIVER<T>::_ptrField->_componentsNames[i]);
    MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits[i] = string(unitName,i*MED_TAILLE_PNOM,MED_TAILLE_PNOM) ;
    SCRUTE(MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits[i]);
  }
  delete[] componentName;
  delete[] unitName;


  // EF : A CHANGER, prendre en compte le SUPPORT seulement si il existe : NOUVEAUX CONSTRUCTEURS
  // read values for each geometric type in _support
  int NumberOfTypes = MED_FIELD_DRIVER<T>::_ptrField->_support->getNumberOfTypes() ;
  const MED_EN::medGeometryElement *Types = MED_FIELD_DRIVER<T>::_ptrField->_support->getTypes() ;
  T  ** myValues = new T*[NumberOfTypes] ;
  int * NumberOfValues = new int[NumberOfTypes] ;
  int TotalNumberOfValues = 0 ;
  MESSAGE ("NumberOfTypes :"<< NumberOfTypes);
  _ptrField->_numberOfValues=0 ;
  for (int i=0; i<NumberOfTypes; i++) {
    MESSAGE ("Type["<<i+1<<"] :"<< Types[i]);
    MESSAGE ("Entity :"<<MED_FIELD_DRIVER<T>::_ptrField->_support->getEntity());
    NumberOfValues[i] =
      MEDnVal(MED_FIELD_DRIVER<T>::_medIdt,
	      const_cast <char*> (MED_FIELD_DRIVER<T>::_fieldName.c_str()),
	      (med_2_1::med_entite_maillage)  MED_FIELD_DRIVER<T>::_ptrField->_support->getEntity(),
	      (med_2_1::med_geometrie_element)Types[i],
	      MED_FIELD_DRIVER<T>::_ptrField->_iterationNumber,
	      MED_FIELD_DRIVER<T>::_ptrField->_orderNumber) ; // no time step ! prend en compte le nbre de pt de gauss
    // test if NumberOfValues is the same in _support !!! TODO that !!
    // we suppose it is
    // we could allocate array

    myValues[i] = new T[ NumberOfValues[i]*numberOfComponents ] ; //Prend en compte le nb de points de Gauss//EF new T[0] ??
    TotalNumberOfValues+=NumberOfValues[i] ;// diviser par le nombre de point de gauss
    char * ProfilName = new char[MED_TAILLE_NOM+1];
    MESSAGE ("NumberOfValues :"<< NumberOfValues[i]);
    MESSAGE ("NumberOfComponents :"<< numberOfComponents);
    MESSAGE ("MESH_NAME :"<< MeshName.c_str());
    MESSAGE ("FIELD_NAME :"<< MED_FIELD_DRIVER<T>::_fieldName.c_str());
    MESSAGE ("MED_ENTITE :"<< (med_2_1::med_entite_maillage) MED_FIELD_DRIVER<T>::_ptrField->_support->getEntity());
    MESSAGE("MED_GEOM :"<<(med_2_1::med_geometrie_element)Types[i]);
    MESSAGE("Iteration :"<<MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber());
    MESSAGE("Order :"<<MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber());
    MED_FIELD_DRIVER<T>::_ptrField->_numberOfValues+=NumberOfValues[i]; // problem with gauss point : _numberOfValues != TotalNumberOfValues !!!!!!!
    med_2_1::med_err ret;

#if defined(IRIX64) || defined(OSF1) || defined(VPP5000)
    int lgth2=NumberOfValues[i]*numberOfComponents;
    if(_ptrField->getValueType()==MED_EN::MED_INT32)
      {
	med_2_1::med_int *temp=new med_2_1::med_int[lgth2];
	ret=med_2_1::MEDchampLire(MED_FIELD_DRIVER<T>::_medIdt,const_cast <char*> (MeshName.c_str()),
				  const_cast <char*> (MED_FIELD_DRIVER<T>::_fieldName.c_str()),
				  (unsigned char*) temp,
				  med_2_1::MED_NO_INTERLACE,
				  MED_ALL,
				  ProfilName,
				  (med_2_1::med_entite_maillage) MED_FIELD_DRIVER<T>::_ptrField->_support->getEntity(),
				  (med_2_1::med_geometrie_element)Types[i],
				  MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber(),
				  MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber()
				  );
	for(int i2=0;i2<lgth2;i2++)
	  myValues[i][i2]=(int)(temp[i2]);
	delete [] temp;
      }
    else
#endif
	ret=med_2_1::MEDchampLire(MED_FIELD_DRIVER<T>::_medIdt,const_cast <char*> (MeshName.c_str()),
				  const_cast <char*> (MED_FIELD_DRIVER<T>::_fieldName.c_str()),
				  (unsigned char*) myValues[i],
				  med_2_1::MED_NO_INTERLACE,
				  MED_ALL,
				  ProfilName,
				  (med_2_1::med_entite_maillage) MED_FIELD_DRIVER<T>::_ptrField->_support->getEntity(),
				  (med_2_1::med_geometrie_element)Types[i],
				  MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber(),
				  MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber()
				  );
    if (ret < 0) {
      // The Field can't be read then we mustdelete all previously allocated members in FIELD
      for(int j=0; j<=i;j++)
	delete[] myValues[j];
      delete[] myValues;
      delete[] NumberOfValues ;
      delete[] ProfilName;
      delete[] MED_FIELD_DRIVER<T>::_ptrField->_componentsTypes ;
      delete[] MED_FIELD_DRIVER<T>::_ptrField->_componentsNames ;
      delete[] MED_FIELD_DRIVER<T>::_ptrField->_componentsUnits ;
      delete[] MED_FIELD_DRIVER<T>::_ptrField->_componentsDescriptions ;
      delete[] MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits ;
      MED_FIELD_DRIVER<T>::_ptrField->_componentsTypes = NULL ;
      MED_FIELD_DRIVER<T>::_ptrField->_componentsNames = NULL ;
      MED_FIELD_DRIVER<T>::_ptrField->_componentsUnits = NULL ;
      MED_FIELD_DRIVER<T>::_ptrField->_componentsDescriptions = NULL ;
      MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits = NULL ;
      MED_FIELD_DRIVER<T>::_fieldNum = MED_INVALID ; // we have not found right field, so reset the field number
      throw MEDEXCEPTION( LOCALIZED( STRING(LOC) <<": ERROR when read value")) ;
    }

    delete[] ProfilName ;
  }

  // allocate _value
  // probleme avec les points de gauss : voir lorsqu-il y en a (!= 1)

  // Créer un driver spécifique pour les modes MED_FULL_INTERLACE et MED_NO_INTERLACE
  ArrayNo * Values = new ArrayNo(numberOfComponents,TotalNumberOfValues);

  for (int i=0; i<numberOfComponents; i++) {
    //T * ValuesT = Values->getRow(i+1) ;
    int Count = 1 ;
    for (int j=0; j<NumberOfTypes; j++) {
      T * myValue = myValues[j] ;
      int NumberOf = NumberOfValues[j] ;
      //	  MED_FIELD_DRIVER<T>::_ptrField->_numberOfValues+=NumberOf; // problem with gauss point : _numberOfValues != TotalNumberOfValues !!!!!!!
      int offset = NumberOf*i ;
      for (int k=0 ; k<NumberOf; k++) {
	//ValuesT[Count]=myValue[k+offset] ;
	Values->setIJ(Count,i+1,myValue[k+offset]);
	SCRUTE(Count);
	SCRUTE(Values->getIJ(Count,i+1));
	Count++;
      }
    }
  }

  for (int j=0; j<NumberOfTypes; j++)
    delete[] myValues[j] ;
  delete[] myValues ;
  delete[] NumberOfValues ;

  if (MED_FIELD_DRIVER<T>::_ptrField->_value != NULL)
    delete MED_FIELD_DRIVER<T>::_ptrField->_value;

  if ( MED_FIELD_DRIVER<T>::_ptrField->getInterlacingType() == MED_EN::MED_FULL_INTERLACE ) {
    // dynamic_cast inutile
    MED_FIELD_DRIVER<T>::_ptrField->_value=dynamic_cast<ArrayFull *>(ArrayConvert(*Values));
    delete Values;
  }
  else
    MED_FIELD_DRIVER<T>::_ptrField->_value=Values;


  MED_FIELD_DRIVER<T>::_ptrField->_isRead = true ;


  END_OF(LOC);
}

template <class T> void MED_FIELD_RDONLY_DRIVER<T>::write( void ) const
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("MED_FIELD_RDONLY_DRIVER::write : Can't write with a RDONLY driver !");
}

/*--------------------- WRONLY PART -------------------------------*/

template <class T> GENDRIVER * MED_FIELD_WRONLY_DRIVER<T>::copy(void) const
{
  MED_FIELD_WRONLY_DRIVER<T> * myDriver = 
    new MED_FIELD_WRONLY_DRIVER<T>(*this);
  return myDriver ;
}

template <class T> void MED_FIELD_WRONLY_DRIVER<T>::read (void)
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("MED_FIELD_WRONLY_DRIVER::read : Can't read with a WRONLY driver !");
}

template <class T> void MED_FIELD_WRONLY_DRIVER<T>::write(void) const
  throw (MEDEXCEPTION)
{
  const char * LOC = "MED_FIELD_WRONLY_DRIVER::write(void) const " ;
  BEGIN_OF(LOC);

  typedef typename MEDMEM_ArrayInterface<T,NoInterlace,NoGauss>::Array ArrayNo;
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,NoGauss>::Array ArrayFull;


  if (MED_FIELD_DRIVER<T>::_status==MED_OPENED)
    {
      int err ;

      int component_count=MED_FIELD_DRIVER<T>::_ptrField->getNumberOfComponents();
      string   component_name(component_count*MED_TAILLE_PNOM,' ') ;
      string   component_unit(component_count*MED_TAILLE_PNOM,' ') ;

      const string * listcomponent_name=MED_FIELD_DRIVER<T>::_ptrField->getComponentsNames() ;
      const string * listcomponent_unit=MED_FIELD_DRIVER<T>::_ptrField->getMEDComponentsUnits() ;
      int length ;
      for (int i=0; i < component_count ; i++) {
	length = min(MED_TAILLE_PNOM,(int)listcomponent_name[i].size());
	component_name.replace(i*MED_TAILLE_PNOM,length,
			       listcomponent_name[i],0,length);
	length = min(MED_TAILLE_PNOM,(int)listcomponent_unit[i].size());
	component_unit.replace(i*MED_TAILLE_PNOM,length,
			       listcomponent_unit[i],0,length);
      }

      MESSAGE("component_name=|"<<component_name<<"|");
      MESSAGE("component_unit=|"<<component_unit<<"|");

      MED_EN::med_type_champ ValueType=MED_FIELD_DRIVER<T>::_ptrField->getValueType() ;
      
      MESSAGE("Template Type =|"<<ValueType<<"|");
      
      // le champ existe deja ???
      char * champName = new char[MED_TAILLE_NOM+1] ;
      med_2_1::med_type_champ type ;
      char * compName ;
      char * compUnit ;
      bool Find = false ;
      int n = med_2_1::MEDnChamp(MED_FIELD_DRIVER<T>::_medIdt,0);
      int nbComp ;
      for (int i=1; i<=n; i++) {
	nbComp = med_2_1::MEDnChamp(MED_FIELD_DRIVER<T>::_medIdt,i);
	compName = new char[MED_TAILLE_PNOM*nbComp+1];
	compUnit = new char[MED_TAILLE_PNOM*nbComp+1];
	err = med_2_1::MEDchampInfo(MED_FIELD_DRIVER<T>::_medIdt,i,champName,&type,compName,compUnit,nbComp);
	if (err == 0)
	  if (strcmp(champName,MED_FIELD_DRIVER<T>::_ptrField->getName().c_str())==0) { // Found !
	    Find = true ;
	    break ;
	  }
	delete[] compName ;
	delete[] compUnit ;
      }
      delete[] champName ;
      if (Find) {
	// the same ?
	if (nbComp != component_count)
	  throw MEDEXCEPTION( LOCALIZED (STRING(LOC)
					 <<": Field exist in file, but number of component are different : "<<nbComp<<" in file and "<<component_count<<" in memory."
					 )
			      );
	// component name and unit
	MESSAGE(LOC<<" Component name in file : "<<compName);
	MESSAGE(LOC<<" Component name in memory : "<<component_name);
	MESSAGE(LOC<<" Component unit in file : "<<compUnit);
	MESSAGE(LOC<<" Component unit in memory : "<<component_unit);
	delete[] compName ;
	delete[] compUnit ;

      } else {
        // Verify the field doesn't exist

        string dataGroupName =  "/CHA/";
        dataGroupName        += MED_FIELD_DRIVER<T>::_ptrField->getName();
        MESSAGE(LOC << "|" << dataGroupName << "|" );
        MED_EN::med_idt gid =  H5Gopen(MED_FIELD_DRIVER<T>::_medIdt, dataGroupName.c_str() );
        
        if ( gid < 0 ) {
          // create field :
          err=med_2_1::MEDchampCr(MED_FIELD_DRIVER<T>::_medIdt, 
                                 const_cast <char*> ((MED_FIELD_DRIVER<T>::_ptrField->getName()).c_str()),
                                 (med_2_1::med_type_champ) ValueType,
                                 const_cast <char*> ( component_name.c_str() ),
                                 const_cast <char*> ( component_unit.c_str() ),
                                 component_count);
          if ( err < 0 )
            throw MEDEXCEPTION( LOCALIZED (STRING(LOC) 
                                           << ": Error MEDchampCr : "<<err
                                           )
                                );
        }
        else H5Gclose(gid);
      }

      const SUPPORT * mySupport = MED_FIELD_DRIVER<T>::_ptrField->getSupport() ;

      if (! mySupport->isOnAllElements())
	throw MEDEXCEPTION( LOCALIZED (STRING(LOC) 
				       <<": Field must be on all entity"
				       )
			    );

      MESH * myMesh   = mySupport->getMesh();
      string MeshName = myMesh->getName() ;
      //MED_EN::medModeSwitch Mode = MED_FIELD_DRIVER<T>::_ptrField->_value->getMode() ;
      // on boucle sur tout les types pour ecrire les tableaux de valeur
      int NumberOfType = mySupport->getNumberOfTypes() ;
      int Index = 1 ;
      const MED_EN::medGeometryElement * Types = mySupport->getTypes() ;
      const int * NumberOfGaussPoint = mySupport->getNumberOfGaussPoint() ;

      const T * value     = NULL;
      ArrayFull * myArray = NULL;
      if ( MED_FIELD_DRIVER<T>::_ptrField->getInterlacingType() == MED_EN::MED_FULL_INTERLACE )
	myArray = MED_FIELD_DRIVER<T>::_ptrField->getArrayNoGauss();
      else {
	// En attendant la convertion de FIELD, on utilise le ArrayConvert
	// ( les infos _ptrField-> sont les mêmes )
	myArray = ArrayConvert( *( dynamic_cast< ArrayNo * >
				   (MED_FIELD_DRIVER<T>::_ptrField->getArrayNoGauss()
				    ))
				);
      }

      for (int i=0;i<NumberOfType;i++) {

	int NumberOfElements = mySupport->getNumberOfElements(Types[i]) ;

	//value = MED_FIELD_DRIVER<T>::_ptrField->getRow(Index) ;
	value = myArray->getRow(Index) ;

	MESSAGE("MED_FIELD_DRIVER<T>::_medIdt                         : "<<MED_FIELD_DRIVER<T>::_medIdt);
	MESSAGE("MeshName.c_str()                : "<<MeshName.c_str());
	MESSAGE("MED_FIELD_DRIVER<T>::_ptrField->getName()            : "<<MED_FIELD_DRIVER<T>::_ptrField->getName());
	MESSAGE("value                           : "<<value);
	MESSAGE("NumberOfElements                : "<<NumberOfElements);
	MESSAGE("NumberOfGaussPoint[i]           : "<<NumberOfGaussPoint[i]);
	MESSAGE("mySupport->getEntity()          : "<<mySupport->getEntity());
	MESSAGE("Types[i]                        : "<<Types[i]);
	MESSAGE("MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber() : "<<MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber());
	MESSAGE("MED_FIELD_DRIVER<T>::_ptrField->getTime()            : "<<MED_FIELD_DRIVER<T>::_ptrField->getTime());
	MESSAGE("MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber()     : "<<MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber());

/*	char chanom[MED_TAILLE_NOM+1];
	char chacomp[MED_TAILLE_NOM+1];
	char chaunit[MED_TAILLE_NOM+1];
	med_2_1::med_type_champ chatype;
	med_int chancomp=1;

	err=med_2_1::MEDchampInfo(MED_FIELD_DRIVER<T>::_medIdt,1,chanom,&chatype,chacomp,chaunit,chancomp);

	if (err<0)
		{
		cout<<"=======================================================================> gros probleme"<<endl;
		exit(-1);
		}
	cout<<"==================> nom lu            = "<<chanom<<endl;
	cout<<"==================> type lu           = "<<chatype<<endl;
	cout<<"==================> nom composante lu = "<<chacomp<<endl;
	cout<<"==================> nom unit lu       = "<<chaunit<<endl;
	cout<<"==================> valeur de med_2_1::MED_REEL64 = "<<med_2_1::MED_REEL64<<endl;
*/	
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000)
	if(_ptrField->getValueType()==MED_EN::MED_INT32)
	  {
	    int lgth2=_ptrField->getNumberOfValues();
	    med_2_1::med_int *temp=new med_2_1::med_int[lgth2];
	    for(int i2=0;i2<lgth2;i2++)
	      temp[i2]=(int)(value[i2]);
	    err=med_2_1::MEDchampEcr(MED_FIELD_DRIVER<T>::_medIdt, 
				    const_cast <char*> ( MeshName.c_str()) ,                         //( string(mesh_name).resize(MED_TAILLE_NOM).c_str())
				    const_cast <char*> ( (MED_FIELD_DRIVER<T>::_ptrField->getName()).c_str()),
				    (unsigned char*)temp, 
				    med_2_1::MED_FULL_INTERLACE,
				    NumberOfElements,
				    NumberOfGaussPoint[i],
				    MED_ALL,
				    MED_NOPFL,
				    med_2_1::MED_REMP,  // PROFIL NON GERE, mode de remplacement non géré
				    (med_2_1::med_entite_maillage)mySupport->getEntity(),
				    (med_2_1::med_geometrie_element)Types[i],
				    MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber(),
				    "        ",
				    MED_FIELD_DRIVER<T>::_ptrField->getTime(),
				    MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber()
				    );
	    delete [] temp;
	  }
	else
#endif
	err=med_2_1::MEDchampEcr(MED_FIELD_DRIVER<T>::_medIdt, 
				const_cast <char*> ( MeshName.c_str()) ,  //( string(mesh_name).resize(MED_TAILLE_NOM).c_str())
				const_cast <char*> ( (MED_FIELD_DRIVER<T>::_ptrField->getName()).c_str()),
				(unsigned char*)value, 
				med_2_1::MED_FULL_INTERLACE,
				NumberOfElements,
				NumberOfGaussPoint[i],
				MED_ALL,
				MED_NOPFL,
				med_2_1::MED_REMP,  // PROFIL NON GERE, mode de remplacement non géré
				(med_2_1::med_entite_maillage)mySupport->getEntity(),
				(med_2_1::med_geometrie_element)Types[i],
				MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber(),
				"        ",
				MED_FIELD_DRIVER<T>::_ptrField->getTime(),
				MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber()
				);
	if (err < MED_VALID ) {
	  if ( MED_FIELD_DRIVER<T>::_ptrField->getInterlacingType() == MED_EN::MED_NO_INTERLACE ) delete myArray;
	  throw MEDEXCEPTION(LOCALIZED( STRING(LOC)
					<<": Error in writing Field "<< MED_FIELD_DRIVER<T>::_ptrField->getName() <<", type "<<Types[i]
					)
			     );

	}

	Index += NumberOfElements ;

      }

      if ( MED_FIELD_DRIVER<T>::_ptrField->getInterlacingType() == MED_EN::MED_NO_INTERLACE ) delete myArray;

    }

  END_OF(LOC);
}

/*--------------------- RDWR PART -------------------------------*/

template <class T> GENDRIVER * MED_FIELD_RDWR_DRIVER<T>::copy(void) const
{
  MED_FIELD_RDWR_DRIVER<T> * myDriver = 
    new MED_FIELD_RDWR_DRIVER<T>(*this);
  return myDriver ;
}

template <class T> void MED_FIELD_RDWR_DRIVER<T>::write(void) const
  throw (MEDEXCEPTION)
{
  BEGIN_OF("MED_FIELD_RDWR_DRIVER::write(void)");
  MED_FIELD_WRONLY_DRIVER<T>::write(); 
  END_OF("MED_FIELD_RDWR_DRIVER::write(void)");
} 

template <class T> void MED_FIELD_RDWR_DRIVER<T>::read (void)
  throw (MEDEXCEPTION)
{
  BEGIN_OF("MED_FIELD_RDWR_DRIVER::read(void)");
  MED_FIELD_RDONLY_DRIVER<T>::read();
  END_OF("MED_FIELD_RDWR_DRIVER::read(void)");
}
}//End namespace MEDMEM
/*-----------------------------------------------------------------*/

#endif /* MED_FIELD_DRIVER_HXX */

