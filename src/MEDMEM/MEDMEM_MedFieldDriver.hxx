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
//#include "MEDMEM_Field.hxx"

//using namespace MED_FR ;

template <class T> class FIELD;

// A QD LA CLASSE MED_ALL_ELEMENTS_DRIVER.... :) pour mutualiser le open ..... avec led _medIdt...
template <class T> class MED_FIELD_DRIVER : public GENDRIVER
{
protected:
  
  FIELD<T> *     _ptrField;
  MED_FR::med_idt        _medIdt;
  string         _fieldName;
  int            _fieldNum;
 
  void search_field() ;
  
public :

  // all MED cell type ?? Classe de Définition ??
  //   static const medGeometryElement all_cell_type[MED_NBR_GEOMETRIE_MAILLE];
  
  //   static const char * const all_cell_type_tab [MED_NBR_GEOMETRIE_MAILLE];
  
  MED_FIELD_DRIVER():GENDRIVER(),
                     _ptrField((FIELD<T> *)MED_NULL),_medIdt(MED_INVALID),
                     _fieldName(""),_fieldNum(MED_INVALID) {}
  MED_FIELD_DRIVER(const string & fileName, FIELD<T> * ptrField, 
		   med_mode_acces accessMode)
    : GENDRIVER(fileName,accessMode),
      _ptrField((FIELD<T> *) ptrField),_medIdt(MED_INVALID), 
      _fieldName(""),_fieldNum(MED_INVALID) 
  {
  }
  
  void open() {
    const char * LOC = "MED_FIELD_DRIVER::open() ";
    BEGIN_OF(LOC);

    // we must set fieldname before open, because we must find field number in file (if it exist !!!)

    MESSAGE(LOC<<"_fileName.c_str : "<< _fileName.c_str()<<",mode : "<< _accessMode);
    _medIdt = MED_FR::MEDouvrir( (const_cast <char *> (_fileName.c_str())),(MED_FR::med_mode_acces) _accessMode);
    MESSAGE(LOC<<"_medIdt : "<< _medIdt );
    if (_medIdt > 0) _status=MED_OPENED; else {
      _status = MED_INVALID;
    }

    END_OF(LOC);
  }
  
  void close() {
    BEGIN_OF("MED_FIELD_DRIVER::close()");
    MED_FR::med_int err = 0;
    if (_status == MED_OPENED) {
      err=MED_FR::MEDfermer(_medIdt);
      H5close(); // If we call H5close() all the files are closed.
      _status = MED_CLOSED;
      _medIdt = MED_INVALID;
      MESSAGE(" MED_FIELD_DRIVER::close() : MEDfermer : _medIdt= " << _medIdt );
      MESSAGE(" MED_FIELD_DRIVER::close() : MEDfermer : err    = " << err );
    }
    END_OF("MED_FIELD_DRIVER::close()");
  }

  virtual void write( void ) const = 0 ;
  virtual void read ( void ) = 0 ;
  void   setFieldName(const string & fieldName) ;
  string getFieldName() const ;
};

template <class T> class MED_FIELD_RDONLY_DRIVER : public virtual MED_FIELD_DRIVER<T>
{
 
public :
  
  MED_FIELD_RDONLY_DRIVER():MED_FIELD_DRIVER<T>() {};
  
  MED_FIELD_RDONLY_DRIVER(const string & fileName,  FIELD<T> * ptrField):
    MED_FIELD_DRIVER<T>(fileName,ptrField,MED_RDONLY) { 
    BEGIN_OF("MED_FIELD_RDONLY_DRIVER::MED_FIELD_RDONLY_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
    END_OF("MED_FIELD_RDONLY_DRIVER::MED_FIELD_RDONLY_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
  }
  
  ~MED_FIELD_RDONLY_DRIVER() { 
    BEGIN_OF("MED_FIELD_RDONLY_DRIVER::~MED_FIELD_RDONLY_DRIVER()");
    END_OF("MED_FIELD_RDONLY_DRIVER::~MED_FIELD_RDONLY_DRIVER()");
  } 

  // CREER UNE METHODE POUR LIRE LA LISTE DES MAILLAGES .....

  void write( void ) const ;
  void read ( void ) ;
};

template <class T> class MED_FIELD_WRONLY_DRIVER : public virtual MED_FIELD_DRIVER<T> {
  
public :
  
  MED_FIELD_WRONLY_DRIVER():MED_FIELD_DRIVER<T>() {}
  
  MED_FIELD_WRONLY_DRIVER(const string & fileName, FIELD<T> * ptrField):
    MED_FIELD_DRIVER<T>(fileName,ptrField,MED_WRONLY)
  {
    BEGIN_OF("MED_FIELD_WRONLY_DRIVER::MED_FIELD_WRONLY_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
    END_OF("MED_FIELD_WRONLY_DRIVER::MED_FIELD_WRONLY_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
  };

  ~MED_FIELD_WRONLY_DRIVER() { } 

  void write( void ) const ;
  void read ( void ) ;
};


template <class T> class MED_FIELD_RDWR_DRIVER : public MED_FIELD_RDONLY_DRIVER<T>, public MED_FIELD_WRONLY_DRIVER<T> {
  
public :
  
  MED_FIELD_RDWR_DRIVER():MED_FIELD_DRIVER<T>() {}
  
  MED_FIELD_RDWR_DRIVER(const string & fileName, FIELD<T> * ptrField):
    MED_FIELD_DRIVER<T>(fileName,ptrField,MED_RDWR)
  {
    BEGIN_OF("MED_FIELD_RDWR_DRIVER::MED_FIELD_RDWR_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
    END_OF("MED_FIELD_RDWR_DRIVER::MED_FIELD_RDWR_DRIVER(const string & fileName, const FIELD<T> * ptrField)");
  };

  ~MED_FIELD_RDWR_DRIVER() { } 

  void write(void) const ;
  void read (void)       ;
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
//        MED_FR::med_type_champ type ;
//        numberOfFields = MED_FR::MEDnChamp(_medIdt,0) ;
//        if ( numberOfFields <= 0 ) 
//  	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": No Field found !"));
//        for (int i=1;i<=numberOfFields;i++) {
	  
//  	numberOfComponents = MED_FR::MEDnChamp(_medIdt,i) ;
//  	if ( numberOfComponents <= 0 ) 
//  	  throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
//  					   <<  "Be careful there is no compound for field n°" 
//  					   << i << "in file |"<<_fileName<<"| !"));
	  
//  	componentName = new char[numberOfComponents*MED_TAILLE_PNOM+1] ;
//  	unitName      = new char[numberOfComponents*MED_TAILLE_PNOM+1] ;   
	  
//  	err = MED_FR::MEDchampInfo(_medIdt, i, fieldName, &type, componentName, 
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

template <class T> void MED_FIELD_RDONLY_DRIVER<T>::read(void)
{
  const char * LOC = " MED_FIELD_RDONLY_DRIVER::read() " ;
  BEGIN_OF(LOC);

  if (_ptrField->_name=="")
    _ptrField->_name = _fieldName ; 
  else
    _fieldName = _ptrField->_name; // bug indetermine ! apres avoir fait readfilestruct, lorsque je recupere le champ, tout est bon sauf le nom du champ dans le driver !!!!!

  MESSAGE("###### "<<LOC<<" fieldNameDRIVER : "<<_fieldName<<" fieldName : "<<_ptrField->_name);

  string MeshName =  _ptrField->getSupport()->getMesh()->getName() ;

  if (_status==MED_OPENED)
    {

      //        search_field() ;

      char *  fieldName ;
      fieldName = new char[MED_TAILLE_NOM+1] ;
      int err ;
      int    numberOfComponents          = 0;
      char * componentName               = (char *) MED_NULL;
      char * unitName                    = (char *) MED_NULL;
      MED_FR::med_type_champ type ;

      // we search the field number !!!!
      if (_fieldNum==MED_INVALID) {
	int    numberOfFields              = 0;      //MED_INVALID
	numberOfFields = MED_FR::MEDnChamp(_medIdt,0) ;
	if ( numberOfFields <= 0 ) 
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": No Field found !"));
	for (int i=1;i<=numberOfFields;i++) {
	    
	  numberOfComponents = MED_FR::MEDnChamp(_medIdt,i) ;
	  if ( numberOfComponents <= 0 ) 
	    //  	      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
	    //  					       <<  "Be careful there is no compound for field n°" 
	    //  					       << i << "in file |"<<_fileName<<"| !"));
	    MESSAGE(LOC<<"Be careful there is no compound for field n°"<<i<<"in file |"<<_fileName<<"| !");

	  componentName = new char[numberOfComponents*MED_TAILLE_PNOM+1] ;
	  unitName      = new char[numberOfComponents*MED_TAILLE_PNOM+1] ;   
	    
	  err = MED_FR::MEDchampInfo(_medIdt, i, fieldName, &type, componentName, 
				     unitName, numberOfComponents) ;
	    
	  MESSAGE("Champ "<<i<<" : #" << fieldName <<"# et recherche #"<<_fieldName.c_str()<<"#");
	  if ( !strcmp(fieldName,_fieldName.c_str()) ) {
	    MESSAGE("FOUND FIELD "<< fieldName <<" : "<<i);
	    _fieldNum = i ;
	    break ;
	  }
	}
      }
      
      delete[] fieldName ;

      if (_fieldNum==MED_INVALID)
	throw MEDEXCEPTION(LOCALIZED( STRING(LOC) << ": Field "<<_fieldName << " not found in file " << _fileName ) );
      MESSAGE ("FieldNum : "<<_fieldNum);

      //        int err ;
      //        int NumberOfComponents = MED_FR::MEDnChamp(_medIdt,_fieldNum) ;
      if (numberOfComponents < 1) 
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"no component")) ; // use iostring !
      // test type to check if it is rigth !!!???
      _ptrField->_numberOfComponents = numberOfComponents ;
      _ptrField->_componentsTypes = new int[numberOfComponents] ;
      _ptrField->_componentsNames = new string[numberOfComponents] ;
      _ptrField->_componentsUnits = new UNIT[numberOfComponents] ;
      _ptrField->_componentsDescriptions = new string[numberOfComponents] ;
      _ptrField->_MEDComponentsUnits = new string[numberOfComponents] ;
      for (int i=0; i<numberOfComponents; i++) {
	_ptrField->_componentsTypes[i] = 1 ;

	// PG : what about space !!!
	_ptrField->_componentsNames[i] = string(componentName,i*MED_TAILLE_PNOM,MED_TAILLE_PNOM) ;
	SCRUTE(_ptrField->_componentsNames[i]);
	_ptrField->_MEDComponentsUnits[i] = string(unitName,i*MED_TAILLE_PNOM,MED_TAILLE_PNOM) ;
	SCRUTE(_ptrField->_MEDComponentsUnits[i]);
      }
      delete[] componentName;
      delete[] unitName;

      // read values for each geometric type in _support
      int NumberOfTypes = _ptrField->_support->getNumberOfTypes() ;
      medGeometryElement *Types = _ptrField->_support->getTypes() ;
      T ** myValues = new (T*)[NumberOfTypes] ;
      int * NumberOfValues = new int[NumberOfTypes] ;
      int TotalNumberOfValues = 0 ;
      MESSAGE ("NumberOfTypes :"<< NumberOfTypes);
      for (int i=0; i<NumberOfTypes; i++) {
	MESSAGE ("Type["<<i+1<<"] :"<< Types[i]);
	MESSAGE ("Entity :"<<_ptrField->_support->getEntity());
	NumberOfValues[i] = 
	  MEDnVal(_medIdt,
		  const_cast <char*> (_fieldName.c_str()),
		  (MED_FR::med_entite_maillage)_ptrField->_support->getEntity(),
		  (MED_FR::med_geometrie_element)Types[i],
		  _ptrField->_iterationNumber,
		  _ptrField->_orderNumber) ; // no time step ! prend en compte le nbre de pt de gauss
	// test if NumberOfValues is the same in _support !!! TODO that !!
	// we suppose it is
	// we could allocate array
	myValues[i] = new T[ NumberOfValues[i]*numberOfComponents ] ;
	TotalNumberOfValues+=NumberOfValues[i] ;// diviser par le nombre de point de gauss 
	char * ProfilName = new char[MED_TAILLE_NOM+1];
	MESSAGE ("NumberOfValues :"<< NumberOfValues[i]);
	MESSAGE ("NumberOfComponents :"<< numberOfComponents);
	MESSAGE ("MESH_NAME :"<< MeshName.c_str());
	MESSAGE ("FIELD_NAME :"<< _fieldName.c_str());
	MESSAGE ("MED_ENTITE :"<< (MED_FR::med_entite_maillage) _ptrField->_support->getEntity());
	MESSAGE("MED_GEOM :"<<(MED_FR::med_geometrie_element)Types[i]);
	MESSAGE("Iteration :"<<_ptrField->getIterationNumber());
	MESSAGE("Order :"<<_ptrField->getOrderNumber());
	if ( MED_FR::MEDchampLire(_medIdt,const_cast <char*> (MeshName.c_str()),
				  const_cast <char*> (_fieldName.c_str()),
				  (unsigned char*) myValues[i],
				  MED_FR::MED_NO_INTERLACE,
				  MED_ALL,
				  ProfilName,
				  (MED_FR::med_entite_maillage) _ptrField->_support->getEntity(),(MED_FR::med_geometrie_element)Types[i],
				  _ptrField->getIterationNumber(),
				  _ptrField->getOrderNumber()
				  ) < 0) {
	  // we must do some delete !!!
	  _fieldNum = MED_INVALID ; // we have not found right field, so reset the field number 
 	  throw MEDEXCEPTION( LOCALIZED( STRING(LOC) <<": ERROR when read value")) ;
	}

	delete[] ProfilName ;
      }
      // allocate _value
      // probleme avec les points de gauss : voir lorsqu-il y en a (!= 1)
      //      MEDARRAY<T> * Values = new MEDARRAY<T>(_ptrField->getNumberOfComponents(),TotalNumberOfValues/_ptrField->getNumberOfComponents(),MED_EN::MED_NO_INTERLACE);

      if (_ptrField->_value==NULL)
	_ptrField->_value=new MEDARRAY<T>(numberOfComponents,TotalNumberOfValues,MED_EN::MED_NO_INTERLACE);

      MEDARRAY<T> * Values = _ptrField->_value ; // create by constructor ???
      // check if dimensions are right
      if (Values->getLeadingValue() != numberOfComponents)
	throw MEDEXCEPTION( LOCALIZED( STRING(LOC) <<": leading dimension are false : "<<Values->getLeadingValue()<<" and "<<numberOfComponents) ) ;
      if (Values->getLengthValue() != TotalNumberOfValues)
	throw MEDEXCEPTION( LOCALIZED( STRING(LOC) <<": length dimension are false : "<<Values->getLengthValue()<<" and "<<TotalNumberOfValues) ) ;

      for (int i=0; i<numberOfComponents; i++) {
	T * ValuesT = Values->getI(MED_NO_INTERLACE,i+1) ;
	int Count = 0 ;
	for (int j=0; j<NumberOfTypes; j++) {
	  T * myValue = myValues[j] ;
	  int NumberOf = NumberOfValues[j] ;
	  int offset = NumberOf*i ;
	  for (int k=0 ; k<NumberOf; k++) {
	    ValuesT[Count]=myValue[k+offset] ;
	    Count++;
	  }
	}
      }
      
      for (int j=0; j<NumberOfTypes; j++)
	delete[] myValues[j] ;
      delete[] myValues ;
      delete[] NumberOfValues ;
    }
  END_OF(LOC);
}

template <class T> void MED_FIELD_RDONLY_DRIVER<T>::write( void ) const
{
  throw MEDEXCEPTION("MED_FIELD_RDONLY_DRIVER::write : Can't write with a RDONLY driver !");
}

/*--------------------- WRONLY PART -------------------------------*/

template <class T> void MED_FIELD_WRONLY_DRIVER<T>::read (void)
{
  throw MEDEXCEPTION("MED_FIELD_WRONLY_DRIVER::write : Can't read with a WRONLY driver !");
}

template <class T> void MED_FIELD_WRONLY_DRIVER<T>::write(void) const
{
  const char * LOC = "MED_FIELD_WRONLY_DRIVER::write(void) const " ;
  BEGIN_OF(LOC);
  if (_status==MED_OPENED)
    {
      int err ;

      int component_count=_ptrField->getNumberOfComponents();
      string   component_name(component_count*MED_TAILLE_PNOM,' ') ;
      string   component_unit(component_count*MED_TAILLE_PNOM,' ') ;

      string * listcomponent_name=_ptrField->getComponentsNames() ;
      string * listcomponent_unit=_ptrField->getMEDComponentsUnits() ;
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

      MED_EN::med_type_champ ValueType=_ptrField->getValueType() ;
      
      // le champ existe deja ???
      char * champName = new char[MED_TAILLE_NOM+1] ;
      MED_FR::med_type_champ type ;
      char * compName ;
      char * compUnit ;
      bool Find = false ;
      int n = MED_FR::MEDnChamp(_medIdt,0);
      int nbComp ;
      for (int i=1; i<=n; i++) {
	nbComp = MED_FR::MEDnChamp(_medIdt,i);
	compName = new char[MED_TAILLE_PNOM*nbComp+1];
	compUnit = new char[MED_TAILLE_PNOM*nbComp+1];
	err = MED_FR::MEDchampInfo(_medIdt,i,champName,&type,compName,compUnit,nbComp);
	if (err == 0)
	  if (strcmp(champName,_ptrField->getName().c_str())==0) { // Found !
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
        dataGroupName        += _ptrField->getName();
        MESSAGE(LOC << "|" << dataGroupName << "|" );
        med_idt gid =  H5Gopen(_medIdt, dataGroupName.c_str() );
        
        if ( gid < 0 ) {
          // create field :
          err=MED_FR::MEDchampCr(_medIdt, 
                                 const_cast <char*> ((_ptrField->getName()).c_str()),
                                 (MED_FR::med_type_champ) ValueType,
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

      const SUPPORT * mySupport = _ptrField->getSupport() ;

      if (! mySupport->isOnAllElements())
	throw MEDEXCEPTION( LOCALIZED (STRING(LOC) 
				       <<": Field must be on all entity"
				       )
			    );
      
      MESH * myMesh = mySupport->getMesh() ;
      string MeshName = myMesh->getName() ;
      //MED_EN::medModeSwitch Mode = _ptrField->_value->getMode() ;
      // on boucle sur tout les types pour ecrire les tableaux de valeur
      int NumberOfType = mySupport->getNumberOfTypes() ;
      int Index = 1 ;
      medGeometryElement * Types = mySupport->getTypes() ;
      int * NumberOfGaussPoint = mySupport->getNumberOfGaussPoint() ;
      for (int i=0;i<NumberOfType;i++) {
	int NumberOfElements = mySupport->getNumberOfElements(Types[i]) ;
	
	MESSAGE(" "<<_ptrField->getName());
	MESSAGE(" "<<NumberOfElements);
	MESSAGE(" "<<NumberOfGaussPoint[i]);
	MESSAGE(" "<<mySupport->getEntity());
	MESSAGE(" "<<Types[i]);
	MESSAGE(" "<<_ptrField->getIterationNumber());
	MESSAGE(" "<<_ptrField->getTime());
	MESSAGE(" "<<_ptrField->getOrderNumber());
	MESSAGE("MEDchampEcr :"<<MeshName.c_str());
	
	T * value = _ptrField->getValueI(MED_EN::MED_FULL_INTERLACE,Index) ;

	err=MED_FR::MEDchampEcr(_medIdt, const_cast <char*> ( MeshName.c_str()) ,                         //( string(mesh_name).resize(MED_TAILLE_NOM).c_str())
				const_cast <char*> ( (_ptrField->getName()).c_str()),
				(unsigned char*)value, MED_FR::MED_FULL_INTERLACE,
				NumberOfElements,
				NumberOfGaussPoint[i],MED_ALL, MED_NOPFL, MED_FR::MED_REMP,  // PROFIL NON GERE, mode de remplacement non géré
				(MED_FR::med_entite_maillage)mySupport->getEntity(),
				(MED_FR::med_geometrie_element)Types[i],
				_ptrField->getIterationNumber(),"        ",
				_ptrField->getTime(),_ptrField->getOrderNumber()
				);
	if (err < MED_VALID )
	  throw MEDEXCEPTION(LOCALIZED( STRING(LOC)
					<<": Error in writing Field "<< _ptrField->getName() <<", type "<<Types[i]
					)
			     );
	Index += NumberOfElements ;
	
      }
    }
  
  END_OF(LOC);
}

/*--------------------- RDWR PART -------------------------------*/

template <class T> void MED_FIELD_RDWR_DRIVER<T>::write(void) const
{
  BEGIN_OF("MED_FIELD_RDWR_DRIVER::write(void)");
  MED_FIELD_WRONLY_DRIVER<T>::write(); 
  END_OF("MED_FIELD_RDWR_DRIVER::write(void)");
} 

template <class T> void MED_FIELD_RDWR_DRIVER<T>::read (void)
{
  BEGIN_OF("MED_FIELD_RDWR_DRIVER::read(void)");
  MED_FIELD_RDONLY_DRIVER<T>::read();
  END_OF("MED_FIELD_RDWR_DRIVER::read(void)");
}

/*-----------------------------------------------------------------*/

#endif /* MED_FIELD_DRIVER_HXX */

