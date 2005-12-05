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
#ifndef MED_FIELD_DRIVER22_HXX
#define MED_FIELD_DRIVER22_HXX

#include <string>

#include "MEDMEM_define.hxx"

#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_MedFieldDriver.hxx"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Unit.hxx"
#include "MEDMEM_Array.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Mesh.hxx"

namespace MEDMEM {
template <class T> class FIELD;

// A QD LA CLASSE MED_ALL_ELEMENTS_DRIVER.... :) pour mutualiser le open ..... avec led _medIdt...

/*!

  Driver Med for FIELD.

  Generic part : implement open and close methods.

*/

template <class T> class MED_FIELD_DRIVER22 : public virtual MED_FIELD_DRIVER<T>
{
protected:
  
  med_2_2::med_idt        _medIdt;
 
  void search_field() ;
  
public :

  // all MED cell type ?? Classe de Définition ??
  //   static const medGeometryElement all_cell_type[MED_NBR_GEOMETRIE_MAILLE];
  
  //   static const char * const all_cell_type_tab [MED_NBR_GEOMETRIE_MAILLE];
  
  /*!
    Constructor.
  */
  MED_FIELD_DRIVER22():MED_FIELD_DRIVER<T>(),_medIdt(MED_INVALID)
  {}
  /*!
    Constructor.
  */
  MED_FIELD_DRIVER22(const string & fileName, FIELD<T> * ptrField, 
		     MED_EN::med_mode_acces accessMode)
    : MED_FIELD_DRIVER<T>(fileName,ptrField,accessMode),_medIdt(MED_INVALID)
  {
  }

  /*!
    Copy constructor.
  */
  MED_FIELD_DRIVER22(const MED_FIELD_DRIVER22 & fieldDriver):
    MED_FIELD_DRIVER<T>(fieldDriver),
    _medIdt(fieldDriver._medIdt)
  {
  }

  /*!
    Destructor.
  */
  virtual ~MED_FIELD_DRIVER22() { 
  }

  void open() throw (MEDEXCEPTION)
  {
    const char * LOC = "MED_FIELD_DRIVER22::open() ";
    BEGIN_OF(LOC);

    // we must set fieldname before open, because we must find field number in file (if it exist !!!)
    if ( MED_FIELD_DRIVER<T>::_fileName == "" )
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
				       << "_fileName is |\"\"|, please set a correct fileName before calling open()"
				       )
			    );

    MESSAGE(LOC<<"_fileName.c_str : "<< MED_FIELD_DRIVER<T>::_fileName.c_str()<<",mode : "<< MED_FIELD_DRIVER<T>::_accessMode);
    _medIdt = med_2_2::MEDouvrir( (const_cast <char *> (MED_FIELD_DRIVER<T>::_fileName.c_str())),(med_2_2::med_mode_acces) MED_FIELD_DRIVER<T>::_accessMode);
    MESSAGE(LOC<<"_medIdt : "<< _medIdt );
    if (_medIdt > 0) 
      MED_FIELD_DRIVER<T>::_status=MED_OPENED;
    else {
      MED_FIELD_DRIVER<T>::_status = MED_INVALID;
      _medIdt = MED_INVALID;
      throw MED_EXCEPTION (LOCALIZED( STRING(LOC) 
				      << "Can't open |"  << MED_FIELD_DRIVER<T>::_fileName 
				      << "|, _medIdt : " << _medIdt
				      )
			   );
    }

    END_OF(LOC);
  }
  
  void close() {
    BEGIN_OF("MED_FIELD_DRIVER22::close()");
    med_2_2::med_int err = 0;
    if (MED_FIELD_DRIVER<T>::_status == MED_OPENED) {
      err=med_2_2::MEDfermer(_medIdt);
      //H5close(); // If we call H5close() all the files are closed.
      MED_FIELD_DRIVER<T>::_status = MED_CLOSED;
      _medIdt = MED_INVALID;
      MESSAGE(" MED_FIELD_DRIVER22::close() : MEDfermer : _medIdt= " << _medIdt );
      MESSAGE(" MED_FIELD_DRIVER22::close() : MEDfermer : err    = " << err );
    }
    END_OF("MED_FIELD_DRIVER22::close()");
  }

};

/*!

  Driver Med for FIELD : Read only.

  Implement read method.

*/

  template <class T> class MED_FIELD_RDONLY_DRIVER22 : public virtual MED_FIELD_DRIVER22<T>, public virtual IMED_FIELD_RDONLY_DRIVER<T>
{
 
public :
  
  /*!
    Constructor.
  */
  MED_FIELD_RDONLY_DRIVER22():MED_FIELD_DRIVER<T>() {};
  
  /*!
    Constructor.
  */
  MED_FIELD_RDONLY_DRIVER22(const string & fileName,  FIELD<T> * ptrField):
    IMED_FIELD_RDONLY_DRIVER<T>(fileName,ptrField),
    MED_FIELD_DRIVER22<T>(fileName,ptrField,MED_EN::MED_RDONLY),
    MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::MED_RDONLY)
  { 
    BEGIN_OF("MED_FIELD_RDONLY_DRIVER22::MED_FIELD_RDONLY_DRIVER22(const string & fileName, const FIELD<T> * ptrField)");
    END_OF("MED_FIELD_RDONLY_DRIVER22::MED_FIELD_RDONLY_DRIVER22(const string & fileName, const FIELD<T> * ptrField)");
  }
  
  /*!
    Copy constructor.
  */
  MED_FIELD_RDONLY_DRIVER22(const MED_FIELD_RDONLY_DRIVER22 & fieldDriver):
    IMED_FIELD_RDONLY_DRIVER<T>(fieldDriver),
    MED_FIELD_DRIVER22<T>(fieldDriver),
    MED_FIELD_DRIVER<T>(fieldDriver)
  {};
  
  /*!
    Destructor.
  */
  virtual ~MED_FIELD_RDONLY_DRIVER22() {};

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

template <class T> class MED_FIELD_WRONLY_DRIVER22 : public virtual MED_FIELD_DRIVER22<T>, public virtual IMED_FIELD_WRONLY_DRIVER<T> {
  
public :
  
  /*!
    Constructor.
  */
  MED_FIELD_WRONLY_DRIVER22():MED_FIELD_DRIVER<T>() {}
  
  /*!
    Constructor.
  */
  MED_FIELD_WRONLY_DRIVER22(const string & fileName, FIELD<T> * ptrField):
    IMED_FIELD_WRONLY_DRIVER<T>(fileName,ptrField),
    MED_FIELD_DRIVER22<T>(fileName,ptrField,MED_EN::MED_WRONLY),
    MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::MED_WRONLY)
  {
    BEGIN_OF("MED_FIELD_WRONLY_DRIVER22::MED_FIELD_WRONLY_DRIVER22(const string & fileName, const FIELD<T> * ptrField)");
    END_OF("MED_FIELD_WRONLY_DRIVER22::MED_FIELD_WRONLY_DRIVER22(const string & fileName, const FIELD<T> * ptrField)");
  };

  /*!
    Copy constructor.
  */
  MED_FIELD_WRONLY_DRIVER22(const MED_FIELD_WRONLY_DRIVER22 & fieldDriver):
    IMED_FIELD_WRONLY_DRIVER<T>(fieldDriver),
    MED_FIELD_DRIVER22<T>(fieldDriver),
    MED_FIELD_DRIVER<T>(fieldDriver)
  {};
  
  /*!
    Destructor.
  */
  virtual ~MED_FIELD_WRONLY_DRIVER22() {};

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

template <class T> class MED_FIELD_RDWR_DRIVER22 : public MED_FIELD_RDONLY_DRIVER22<T>, public MED_FIELD_WRONLY_DRIVER22<T>, public IMED_FIELD_RDWR_DRIVER<T> {
  
public :
  
  /*!
    Constructor.
  */
  MED_FIELD_RDWR_DRIVER22():MED_FIELD_DRIVER22<T>() {}
  
  /*!
    Constructor.
  */
  MED_FIELD_RDWR_DRIVER22(const string & fileName, FIELD<T> * ptrField):
    MED_FIELD_WRONLY_DRIVER22<T>(fileName,ptrField),
    MED_FIELD_RDONLY_DRIVER22<T>(fileName,ptrField),
    IMED_FIELD_RDONLY_DRIVER<T>(fileName,ptrField),
    IMED_FIELD_WRONLY_DRIVER<T>(fileName,ptrField),
    MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::MED_RDWR),
    IMED_FIELD_RDWR_DRIVER<T>(fileName,ptrField)
  {
    BEGIN_OF("MED_FIELD_RDWR_DRIVER22::MED_FIELD_RDWR_DRIVER22(const string & fileName, const FIELD<T> * ptrField)");
    //_accessMode = MED_RDWR ;
    END_OF("MED_FIELD_RDWR_DRIVER22::MED_FIELD_RDWR_DRIVER22(const string & fileName, const FIELD<T> * ptrField)");
  };

  /*!
    Copy constructor.
  */
  MED_FIELD_RDWR_DRIVER22(const MED_FIELD_RDWR_DRIVER22 & fieldDriver):
    MED_FIELD_WRONLY_DRIVER22<T>(fieldDriver),
    MED_FIELD_RDONLY_DRIVER22<T>(fieldDriver),
    IMED_FIELD_RDWR_DRIVER<T>(fieldDriver),
    IMED_FIELD_RDONLY_DRIVER<T>(fieldDriver),
    IMED_FIELD_WRONLY_DRIVER<T>(fieldDriver),
    MED_FIELD_DRIVER<T>(fieldDriver)
  {};
  
  /*!
    Destructor.
  */
  ~MED_FIELD_RDWR_DRIVER22() {};

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
//        med_2_2::med_type_champ type ;
//        numberOfFields = med_2_2::MEDnChamp(_medIdt,0) ;
//        if ( numberOfFields <= 0 ) 
//  	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": No Field found !"));
//        for (int i=1;i<=numberOfFields;i++) {
	  
//  	numberOfComponents = med_2_2::MEDnChamp(_medIdt,i) ;
//  	if ( numberOfComponents <= 0 ) 
//  	  throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
//  					   <<  "Be careful there is no compound for field n°" 
//  					   << i << "in file |"<<_fileName<<"| !"));
	  
//  	componentName = new char[numberOfComponents*MED_TAILLE_PNOM+1] ;
//  	unitName      = new char[numberOfComponents*MED_TAILLE_PNOM+1] ;   
	  
//  	err = med_2_2::MEDchampInfo(_medIdt, i, fieldName, &type, componentName, 
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

template <class T> GENDRIVER * MED_FIELD_RDONLY_DRIVER22<T>::copy(void) const
{
  return new MED_FIELD_RDONLY_DRIVER22<T>(*this);
}

template <class T> void MED_FIELD_RDONLY_DRIVER22<T>::read(void)
  throw (MEDEXCEPTION)
{
  const char * LOC = " MED_FIELD_RDONLY_DRIVER22::read() " ;
  BEGIN_OF(LOC);

  if (MED_FIELD_DRIVER<T>::_ptrField->_name=="")
    MED_FIELD_DRIVER<T>::_ptrField->_name = MED_FIELD_DRIVER<T>::_fieldName ; 
  else
    MED_FIELD_DRIVER<T>::_fieldName = MED_FIELD_DRIVER<T>::_ptrField->_name; // bug indetermine ! apres avoir fait readfilestruct, lorsque je recupere le champ, tout est bon sauf le nom du champ dans le driver !!!!!

  MESSAGE("###### "<<LOC<<" fieldNameDRIVER : "<<MED_FIELD_DRIVER<T>::_fieldName<<" fieldName : "<<MED_FIELD_DRIVER<T>::_ptrField->_name);

  string MeshName =  MED_FIELD_DRIVER<T>::_ptrField->getSupport()->getMesh()->getName() ;

  if (MED_FIELD_DRIVER<T>::_status==MED_OPENED)
    {

      //        search_field() ;

      char *  fieldName ;
      fieldName = new char[MED_TAILLE_NOM+1] ;
      int err ;
      int    numberOfComponents          = 0;
      char * componentName               = (char *) MED_NULL;
      char * unitName                    = (char *) MED_NULL;
      med_2_2::med_type_champ type ;

      // we search the field number !!!!
      if (MED_FIELD_DRIVER<T>::_fieldNum==MED_INVALID) {
	int    numberOfFields              = 0;      //MED_INVALID
	numberOfFields = med_2_2::MEDnChamp(MED_FIELD_DRIVER22<T>::_medIdt,0) ;
	if ( numberOfFields <= 0 ) 
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": No Field found !"));
	for (int i=1;i<=numberOfFields;i++) {
	    
	  numberOfComponents = med_2_2::MEDnChamp(MED_FIELD_DRIVER22<T>::_medIdt,i) ;
	  if ( numberOfComponents <= 0 ) 
	    //  	      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
	    //  					       <<  "Be careful there is no compound for field n°" 
	    //  					       << i << "in file |"<<_fileName<<"| !"));
	    MESSAGE(LOC<<"Be careful there is no compound for field n°"<<i<<"in file |"<<MED_FIELD_DRIVER<T>::_fileName<<"| !");

	  componentName = new char[numberOfComponents*MED_TAILLE_PNOM22+1] ;
	  unitName      = new char[numberOfComponents*MED_TAILLE_PNOM22+1] ;   
	    
	  err = med_2_2::MEDchampInfo(MED_FIELD_DRIVER22<T>::_medIdt, i, fieldName, &type, componentName, 
				     unitName, numberOfComponents) ;
	    
	  MESSAGE("Champ "<<i<<" : #" << fieldName <<"# et recherche #"<<MED_FIELD_DRIVER<T>::_fieldName.c_str()<<"#");
	  if ( !strcmp(fieldName,MED_FIELD_DRIVER<T>::_fieldName.c_str()) ) {
	    MESSAGE("FOUND FIELD "<< fieldName <<" : "<<i);
	    MED_FIELD_DRIVER<T>::_fieldNum = i ;
	    break ;
	  }
	  // not found : release memory and search next field !
	  delete[] componentName ;
	  delete[] unitName ;
	}
      }
      
      delete[] fieldName ;

      if (MED_FIELD_DRIVER<T>::_fieldNum==MED_INVALID)
	throw MEDEXCEPTION(LOCALIZED( STRING(LOC) << ": Field "<<MED_FIELD_DRIVER<T>::_fieldName << " not found in file " << MED_FIELD_DRIVER<T>::_fileName ) );
      MESSAGE ("FieldNum : "<<MED_FIELD_DRIVER<T>::_fieldNum);

      //        int err ;
      //        int NumberOfComponents = med_2_2::MEDnChamp(MED_FIELD_DRIVER<T>::_medIdt,MED_FIELD_DRIVER<T>::_fieldNum) ;
      if (numberOfComponents < 1) 
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"no component")) ; // use iostring !
      // test type to check if it is rigth !!!???
      MED_FIELD_DRIVER<T>::_ptrField->_numberOfComponents = numberOfComponents ;
      MED_FIELD_DRIVER<T>::_ptrField->_componentsTypes = new int[numberOfComponents] ;
      MED_FIELD_DRIVER<T>::_ptrField->_componentsNames = new string[numberOfComponents] ;
      MED_FIELD_DRIVER<T>::_ptrField->_componentsUnits = new UNIT[numberOfComponents] ;
      MED_FIELD_DRIVER<T>::_ptrField->_componentsDescriptions = new string[numberOfComponents] ;
      MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits = new string[numberOfComponents] ;
      for (int i=0; i<numberOfComponents; i++) {
	MED_FIELD_DRIVER<T>::_ptrField->_componentsTypes[i] = 1 ;

	// PG : what about space !!!
	MED_FIELD_DRIVER<T>::_ptrField->_componentsNames[i] = string(componentName,i*MED_TAILLE_PNOM22,MED_TAILLE_PNOM22) ;
	SCRUTE(MED_FIELD_DRIVER<T>::_ptrField->_componentsNames[i]);
	MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits[i] = string(unitName,i*MED_TAILLE_PNOM22,MED_TAILLE_PNOM22) ;
	SCRUTE(MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits[i]);
      }
      delete[] componentName;
      delete[] unitName;

      // read values for each geometric type in _support
      int NumberOfTypes = MED_FIELD_DRIVER<T>::_ptrField->_support->getNumberOfTypes() ;
      const MED_EN::medGeometryElement *Types = MED_FIELD_DRIVER<T>::_ptrField->_support->getTypes() ;
      T ** myValues = new T*[NumberOfTypes] ;
      int * NumberOfValues = new int[NumberOfTypes] ;
      int TotalNumberOfValues = 0 ;
      MESSAGE ("NumberOfTypes :"<< NumberOfTypes);
      MED_FIELD_DRIVER<T>::_ptrField->_numberOfValues=0 ;
      for (int i=0; i<NumberOfTypes; i++) {
	MESSAGE ("Type["<<i+1<<"] :"<< Types[i]);
	MESSAGE ("Entity :"<<MED_FIELD_DRIVER<T>::_ptrField->_support->getEntity());
// 	NumberOfValues[i] = 
// 	  MEDnVal(_medIdt,
// 		  const_cast <char*> (MED_FIELD_DRIVER<T>::_fieldName.c_str()),
// 		  (med_2_2::med_entite_maillage)_ptrField->_support->getEntity(),
// 		  (med_2_2::med_geometrie_element)Types[i],
// 		  _ptrField->_iterationNumber,
// 		  _ptrField->_orderNumber) ; // no time step ! prend en compte le nbre de pt de gauss
	// test if NumberOfValues is the same in _support !!! TODO that !!
	// we suppose it is
	// we could allocate array
	// Be really carefull about the profil; especially the last arg of
	// MEDnVal

	NumberOfValues[i] =
	  MEDnVal(MED_FIELD_DRIVER22<T>::_medIdt,
		  const_cast <char*> (MED_FIELD_DRIVER<T>::_fieldName.c_str()),
		  (med_2_2::med_entite_maillage)MED_FIELD_DRIVER<T>::_ptrField->_support->getEntity(),
		  (med_2_2::med_geometrie_element) Types[i],
		  MED_FIELD_DRIVER<T>::_ptrField->_iterationNumber, MED_FIELD_DRIVER<T>::_ptrField->_orderNumber,
		  const_cast <char*> (MED_FIELD_DRIVER<T>::_ptrField->_support->getMesh()->getName().c_str()),
		  med_2_2::MED_COMPACT) ;

	myValues[i] = new T[ NumberOfValues[i]*numberOfComponents ] ;
	TotalNumberOfValues+=NumberOfValues[i] ;// diviser par le nombre de point de gauss 
	char * ProfilName = new char[MED_TAILLE_NOM+1];
	char * LocalGaussName = new char[MED_TAILLE_NOM+1];
	MESSAGE ("NumberOfValues :"<< NumberOfValues[i]);
	MESSAGE ("NumberOfComponents :"<< numberOfComponents);
	MESSAGE ("MESH_NAME :"<< MeshName.c_str());
	MESSAGE ("FIELD_NAME :"<< MED_FIELD_DRIVER<T>::_fieldName.c_str());
	MESSAGE ("MED_ENTITE :"<< (med_2_2::med_entite_maillage) MED_FIELD_DRIVER<T>::_ptrField->_support->getEntity());
	MESSAGE("MED_GEOM :"<<(med_2_2::med_geometrie_element)Types[i]);
	MESSAGE("Iteration :"<<MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber());
	MESSAGE("Order :"<<MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber());
        MED_FIELD_DRIVER<T>::_ptrField->_numberOfValues+=NumberOfValues[i]; // problem with gauss point : _numberOfValues != TotalNumberOfValues !!!!!!!

	err = MEDchampLire(MED_FIELD_DRIVER22<T>::_medIdt,const_cast <char*> (MeshName.c_str()),
			   const_cast <char*> (MED_FIELD_DRIVER<T>::_fieldName.c_str()),
				  (unsigned char*) myValues[i],
			   med_2_2::MED_NO_INTERLACE,MED_ALL,
			   LocalGaussName,ProfilName,
			   med_2_2::MED_NO_PFLMOD,
			   (med_2_2::med_entite_maillage) MED_FIELD_DRIVER<T>::_ptrField->_support->getEntity(),(med_2_2::med_geometrie_element)Types[i],
			   MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber(),
			   MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber());

	if ( err < 0) {
	  // we must do some delete !!!
	  for(int j=0; j<=i;j++)
	    delete[] myValues[j];
	  delete[] myValues;
	  delete[] NumberOfValues ;
	  delete[] ProfilName;
	  delete[] LocalGaussName;
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

	// At this time ProfilName should be med_2_2::MED_NOPFL and
	// LocalGaussName should be med_2_2::MED_NOGAUSS
	delete[] ProfilName ;
	delete[] LocalGaussName ;
      }
      // allocate _value
      // probleme avec les points de gauss : voir lorsqu-il y en a (!= 1)
      //      MEDARRAY<T> * Values = new MEDARRAY<T>(MED_FIELD_DRIVER<T>::_ptrField->getNumberOfComponents(),TotalNumberOfValues/MED_FIELD_DRIVER<T>::_ptrField->getNumberOfComponents(),MED_EN::MED_NO_INTERLACE);

      if (MED_FIELD_DRIVER<T>::_ptrField->_value==NULL)
	MED_FIELD_DRIVER<T>::_ptrField->_value=new MEDARRAY<T>(numberOfComponents,TotalNumberOfValues,MED_EN::MED_NO_INTERLACE);

      MEDARRAY<T> * Values = MED_FIELD_DRIVER<T>::_ptrField->_value ; // create by constructor ???
      // check if dimensions are right : inutile : c'est dans le constructeur !!!
      //if (Values->getLeadingValue() != numberOfComponents)
      //  throw MEDEXCEPTION( LOCALIZED( STRING(LOC) <<": leading dimension are false : "<<Values->getLeadingValue()<<" and "<<numberOfComponents) ) ;
      //if (Values->getLengthValue() != TotalNumberOfValues)
      //  throw MEDEXCEPTION( LOCALIZED( STRING(LOC) <<": length dimension are false : "<<Values->getLengthValue()<<" and "<<TotalNumberOfValues) ) ;

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

      MED_FIELD_DRIVER<T>::_ptrField->_isRead = true ;
    }

  END_OF(LOC);
}

template <class T> void MED_FIELD_RDONLY_DRIVER22<T>::write( void ) const
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("MED_FIELD_RDONLY_DRIVER22::write : Can't write with a RDONLY driver !");
}

/*--------------------- WRONLY PART -------------------------------*/

template <class T> GENDRIVER * MED_FIELD_WRONLY_DRIVER22<T>::copy(void) const
{
  return new MED_FIELD_WRONLY_DRIVER22<T>(*this);
}

template <class T> void MED_FIELD_WRONLY_DRIVER22<T>::read (void)
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("MED_FIELD_WRONLY_DRIVER22::read : Can't read with a WRONLY driver !");
}

template <class T> void MED_FIELD_WRONLY_DRIVER22<T>::write(void) const
  throw (MEDEXCEPTION)
{
  const char * LOC = "MED_FIELD_WRONLY_DRIVER22::write(void) const " ;
  BEGIN_OF(LOC);
  if (MED_FIELD_DRIVER<T>::_status==MED_OPENED)
    {
      int err ;

      int component_count=MED_FIELD_DRIVER<T>::_ptrField->getNumberOfComponents();
      string   component_name(component_count*MED_TAILLE_PNOM22,' ') ;
      string   component_unit(component_count*MED_TAILLE_PNOM22,' ') ;

      const string * listcomponent_name=MED_FIELD_DRIVER<T>::_ptrField->getComponentsNames() ;
      const string * listcomponent_unit=MED_FIELD_DRIVER<T>::_ptrField->getMEDComponentsUnits() ;
      int length ;
      for (int i=0; i < component_count ; i++) {
	length = min(MED_TAILLE_PNOM22,(int)listcomponent_name[i].size());
	component_name.replace(i*MED_TAILLE_PNOM22,length,
			       listcomponent_name[i],0,length);
	length = min(MED_TAILLE_PNOM22,(int)listcomponent_unit[i].size());
	component_unit.replace(i*MED_TAILLE_PNOM22,length,
			       listcomponent_unit[i],0,length);
      }

      MESSAGE("component_name=|"<<component_name<<"|");
      MESSAGE("component_unit=|"<<component_unit<<"|");

      MED_EN::med_type_champ ValueType=MED_FIELD_DRIVER<T>::_ptrField->getValueType() ;
      
      MESSAGE("Template Type =|"<<ValueType<<"|");
      
      // le champ existe deja ???
      char * champName = new char[MED_TAILLE_NOM+1] ;
      med_2_2::med_type_champ type ;
      char * compName ;
      char * compUnit ;
      bool Find = false ;
      int n = med_2_2::MEDnChamp(MED_FIELD_DRIVER22<T>::_medIdt,0);
      int nbComp ;
      for (int i=1; i<=n; i++) {
	nbComp = med_2_2::MEDnChamp(MED_FIELD_DRIVER22<T>::_medIdt,i);
	compName = new char[MED_TAILLE_PNOM22*nbComp+1];
	compUnit = new char[MED_TAILLE_PNOM22*nbComp+1];
	err = med_2_2::MEDchampInfo(MED_FIELD_DRIVER22<T>::_medIdt,i,champName,&type,compName,compUnit,nbComp);
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
        med_2_2::med_idt gid =  H5Gopen(MED_FIELD_DRIVER22<T>::_medIdt, dataGroupName.c_str() );
        
        if ( gid < 0 ) {
          // create field :
          err=med_2_2::MEDchampCr(MED_FIELD_DRIVER22<T>::_medIdt, 
                                 const_cast <char*> ((MED_FIELD_DRIVER<T>::_ptrField->getName()).c_str()),
                                 (med_2_2::med_type_champ) ValueType,
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
      
      MESH * myMesh = mySupport->getMesh() ;
      string MeshName = myMesh->getName() ;
      //MED_EN::medModeSwitch Mode = MED_FIELD_DRIVER<T>::_ptrField->_value->getMode() ;
      // on boucle sur tout les types pour ecrire les tableaux de valeur
      int NumberOfType = mySupport->getNumberOfTypes() ;
      int Index = 1 ;
      const MED_EN::medGeometryElement * Types = mySupport->getTypes() ;
      const int * NumberOfGaussPoint = mySupport->getNumberOfGaussPoint() ;
      for (int i=0;i<NumberOfType;i++) {
	int NumberOfElements = mySupport->getNumberOfElements(Types[i]) ;
	
	const T * value = MED_FIELD_DRIVER<T>::_ptrField->getValueI(MED_EN::MED_FULL_INTERLACE,Index) ;
	
	MESSAGE("MED_FIELD_DRIVER<T>22::_medIdt                         : "<<MED_FIELD_DRIVER22<T>::_medIdt);
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
	med_2_2::med_type_champ chatype;
	med_int chancomp=1;
	
	err=med_2_2::MEDchampInfo(MED_FIELD_DRIVER<T>::_medIdt,1,chanom,&chatype,chacomp,chaunit,chancomp);

	if (err<0) 
		{
		cout<<"=======================================================================> gros probleme"<<endl;
		exit(-1);
		}
	cout<<"==================> nom lu            = "<<chanom<<endl;
	cout<<"==================> type lu           = "<<chatype<<endl;
	cout<<"==================> nom composante lu = "<<chacomp<<endl;
	cout<<"==================> nom unit lu       = "<<chaunit<<endl;
	cout<<"==================> valeur de med_2_2::MED_REEL64 = "<<med_2_2::MED_REEL64<<endl;
*/	

// 	err=med_2_2::MEDchampEcr(_medIdt, 
// 				const_cast <char*> ( MeshName.c_str()) ,                         //( string(mesh_name).resize(MED_TAILLE_NOM).c_str())
// 				const_cast <char*> ( (_ptrField->getName()).c_str()),
// 				(unsigned char*)value, 
// 				med_2_2::MED_FULL_INTERLACE,
// 				NumberOfElements,
// 				NumberOfGaussPoint[i],
// 				MED_ALL,
// 				MED_NOPFL,
// 				med_2_2::MED_REMP,  // PROFIL NON GERE, mode de remplacement non géré
// 				(med_2_2::med_entite_maillage)mySupport->getEntity(),
// 				(med_2_2::med_geometrie_element)Types[i],
// 				_ptrField->getIterationNumber(),
// 				"        ",
// 				_ptrField->getTime(),
// 				_ptrField->getOrderNumber()
// 				);

	err=med_2_2::MEDchampEcr(MED_FIELD_DRIVER22<T>::_medIdt, 
				const_cast <char*> ( MeshName.c_str()) ,                         //( string(mesh_name).resize(MED_TAILLE_NOM).c_str())
				const_cast <char*> ( (MED_FIELD_DRIVER<T>::_ptrField->getName()).c_str()),
				(unsigned char*)value,
				med_2_2::MED_FULL_INTERLACE,
				NumberOfElements*NumberOfGaussPoint[i],
				MED_NOGAUSS, MED_ALL, MED_NOPFL,
				med_2_2::MED_NO_PFLMOD, // PROFIL NON GERE, mode de remplacement non géré
				(med_2_2::med_entite_maillage)mySupport->getEntity(),
				(med_2_2::med_geometrie_element)Types[i],
				MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber(),
				"        ",
				MED_FIELD_DRIVER<T>::_ptrField->getTime(),
				MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber()
				);

	if (err < MED_VALID )
	  throw MEDEXCEPTION(LOCALIZED( STRING(LOC)
					<<": Error in writing Field "<< MED_FIELD_DRIVER<T>::_ptrField->getName() <<", type "<<Types[i]
					)
			     );

	Index += NumberOfElements ;
	
      }
    }
  
  END_OF(LOC);
}

/*--------------------- RDWR PART -------------------------------*/

template <class T> GENDRIVER * MED_FIELD_RDWR_DRIVER22<T>::copy(void) const
{
  return new MED_FIELD_RDWR_DRIVER22<T>(*this);
}

template <class T> void MED_FIELD_RDWR_DRIVER22<T>::write(void) const
  throw (MEDEXCEPTION)
{
  BEGIN_OF("MED_FIELD_RDWR_DRIVER22::write(void)");
  MED_FIELD_WRONLY_DRIVER22<T>::write(); 
  END_OF("MED_FIELD_RDWR_DRIVER22::write(void)");
} 

template <class T> void MED_FIELD_RDWR_DRIVER22<T>::read (void)
  throw (MEDEXCEPTION)
{
  BEGIN_OF("MED_FIELD_RDWR_DRIVER22::read(void)");
  MED_FIELD_RDONLY_DRIVER22<T>::read();
  END_OF("MED_FIELD_RDWR_DRIVER22::read(void)");
}

}
/*-----------------------------------------------------------------*/

#endif /* MED_FIELD_DRIVER_HXX */

