#ifndef MED_FIELD_DRIVER21_HXX
#define MED_FIELD_DRIVER21_HXX

#include <string>

#include "MEDMEM_define.hxx"

#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_MedFieldDriver.hxx"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Unit.hxx"

#include "MEDMEM_ArrayInterface.hxx"
#include "MEDMEM_ArrayConvert.hxx"

#include "MEDMEM_Support.hxx"
#include "MEDMEM_Mesh.hxx"

namespace MEDMEM {

/*!

  Driver Med for FIELD.

  Generic part : implement open and close methods.

*/

template <class T> class MED_FIELD_DRIVER21 : public virtual MED_FIELD_DRIVER<T>
{
protected:
  med_2_1::med_idt        _medIdt;
 
  bool createFieldSupport(med_2_1::med_idt id,
			  string & fieldName,
			  med_2_1::med_int ndt,
			  med_2_1::med_int od,
			  SUPPORT & support,
			  string & meshName) const throw (MEDEXCEPTION);

  void getMeshGeometricType(med_2_1::med_idt id,
			    string & meshName,
			    MED_EN::medEntityMesh  entite,
			    vector<MED_EN::medGeometryElement> & geoType,
			    vector<int> &nbOfElOfType) const;

public :
  
  /*!
    Constructor.
  */
  MED_FIELD_DRIVER21():MED_FIELD_DRIVER<T>(),_medIdt(MED_INVALID)
  {}
  /*!
    Constructor.
  */
  template <class INTERLACING_TAG>
  MED_FIELD_DRIVER21(const string & fileName,
		     FIELD<T, INTERLACING_TAG> * ptrField, 
		     MED_EN::med_mode_acces accessMode)
    : MED_FIELD_DRIVER<T>(fileName,ptrField,accessMode),_medIdt(MED_INVALID)
  {
  }

  /*!
    Copy constructor.
  */
  MED_FIELD_DRIVER21(const MED_FIELD_DRIVER21 & fieldDriver):
    MED_FIELD_DRIVER<T>(fieldDriver), _medIdt(fieldDriver._medIdt)
  {
  }

  /*!
    Destructor.
  */
  virtual ~MED_FIELD_DRIVER21() { 
  }

  void open() throw (MEDEXCEPTION)
  {
    const char * LOC = "MED_FIELD_DRIVER21::open() ";
    BEGIN_OF(LOC);

    // we must set fieldname before open, because we must find field number in file (if it exist !!!)
    if ( MED_FIELD_DRIVER<T>::_fileName == "" )
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
				       << "_fileName is |\"\"|, please set a correct fileName before calling open()"
				       )
			    );

    MESSAGE(LOC<<"_fileName.c_str : "<< MED_FIELD_DRIVER<T>::_fileName.c_str()<<",mode : "<< MED_FIELD_DRIVER<T>::_accessMode);
    _medIdt = med_2_1::MEDouvrir( (const_cast <char *> (MED_FIELD_DRIVER<T>::_fileName.c_str())),(med_2_1::med_mode_acces) MED_FIELD_DRIVER<T>::_accessMode);
    MESSAGE(LOC<<"_medIdt : "<< _medIdt );
    if (_medIdt > 0) 
      MED_FIELD_DRIVER<T>::_status=MED_OPENED;
    else {
      MED_FIELD_DRIVER<T>::_status = MED_INVALID;
      MED_FIELD_DRIVER21<T>::_medIdt = MED_INVALID;
      throw MED_EXCEPTION (LOCALIZED( STRING(LOC) 
				      << "Can't open |"  << MED_FIELD_DRIVER<T>::_fileName 
				      << "|, _medIdt : " << MED_FIELD_DRIVER21<T>::_medIdt
				      )
			   );
    }

    END_OF(LOC);
  }
  
  void close() {
    BEGIN_OF("MED_FIELD_DRIVER21::close()");
    med_2_1::med_int err = 0;
    if (MED_FIELD_DRIVER<T>::_status == MED_OPENED) {
      err=med_2_1::MEDfermer(MED_FIELD_DRIVER21<T>::_medIdt);
      //H5close(); // If we call H5close() all the files are closed.
      MED_FIELD_DRIVER<T>::_status = MED_CLOSED;
      MED_FIELD_DRIVER21<T>::_medIdt = MED_INVALID;
      MESSAGE(" MED_FIELD_DRIVER21::close() : MEDfermer : MED_FIELD_DRIVER21<T>::_medIdt= " << _medIdt );
      MESSAGE(" MED_FIELD_DRIVER21::close() : MEDfermer : err    = " << err );
    }
    END_OF("MED_FIELD_DRIVER21::close()");
  }


};

/*!

  Driver Med for FIELD : Read only.

  Implement read method.

*/

  template <class T> class MED_FIELD_RDONLY_DRIVER21 : public virtual MED_FIELD_DRIVER21<T>, public virtual IMED_FIELD_RDONLY_DRIVER<T>
{
 
public :
  
  /*!
    Constructor.
  */
  MED_FIELD_RDONLY_DRIVER21():MED_FIELD_DRIVER<T>() {};
  
  /*!
    Constructor.
  */
  template <class INTERLACING_TAG>
  MED_FIELD_RDONLY_DRIVER21(const string & fileName,
			    FIELD<T, INTERLACING_TAG> * ptrField):
    MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::MED_RDONLY),
    MED_FIELD_DRIVER21<T>(fileName,ptrField,MED_EN::MED_RDONLY),
    IMED_FIELD_RDONLY_DRIVER<T>(fileName,ptrField)
  { 
    BEGIN_OF("MED_FIELD_RDONLY_DRIVER21::MED_FIELD_RDONLY_DRIVER21(const string & fileName, const FIELD<T,INTERLACING_TAG> * ptrField)");
    END_OF("MED_FIELD_RDONLY_DRIVER21::MED_FIELD_RDONLY_DRIVER21(const string & fileName, const FIELD<T,INTERLACING_TAG> * ptrField)");
  }
  
  /*!
    Copy constructor.
  */
  MED_FIELD_RDONLY_DRIVER21(const MED_FIELD_RDONLY_DRIVER21 & fieldDriver):
    IMED_FIELD_RDONLY_DRIVER<T>(fieldDriver),
    MED_FIELD_DRIVER21<T>(fieldDriver),
    MED_FIELD_DRIVER<T>(fieldDriver)
  {};
  
  /*!
    Destructor.
  */
  virtual ~MED_FIELD_RDONLY_DRIVER21() {};

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

template <class T> class MED_FIELD_WRONLY_DRIVER21 : public virtual MED_FIELD_DRIVER21<T>, public virtual IMED_FIELD_WRONLY_DRIVER<T> {
  
public :
  
  /*!
    Constructor.
  */
  MED_FIELD_WRONLY_DRIVER21():MED_FIELD_DRIVER<T>() {}
  
  /*!
    Constructor.
  */
  template <class INTERLACING_TAG>
  MED_FIELD_WRONLY_DRIVER21(const string & fileName,
			    FIELD<T, INTERLACING_TAG> * ptrField):
    IMED_FIELD_WRONLY_DRIVER<T>(fileName,ptrField),
    MED_FIELD_DRIVER21<T>(fileName,ptrField,MED_EN::MED_WRONLY),
    MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::MED_WRONLY)
  {
    BEGIN_OF("MED_FIELD_WRONLY_DRIVER21::MED_FIELD_WRONLY_DRIVER21(const string & fileName, const FIELD<T,INTERLACING_TAG> * ptrField)");
    END_OF("MED_FIELD_WRONLY_DRIVER21::MED_FIELD_WRONLY_DRIVER21(const string & fileName, const FIELD<T,INTERLACING_TAG> * ptrField)");
  }

  /*!
    Copy constructor.
  */
  MED_FIELD_WRONLY_DRIVER21(const MED_FIELD_WRONLY_DRIVER21 & fieldDriver):
    IMED_FIELD_WRONLY_DRIVER<T>(fieldDriver),
    MED_FIELD_DRIVER21<T>(fieldDriver),
    MED_FIELD_DRIVER<T>(fieldDriver)
  {}
  
  /*!
    Destructor.
  */
  virtual ~MED_FIELD_WRONLY_DRIVER21() {};

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

template <class T> class MED_FIELD_RDWR_DRIVER21 : public MED_FIELD_RDONLY_DRIVER21<T>, public MED_FIELD_WRONLY_DRIVER21<T>, public IMED_FIELD_RDWR_DRIVER<T> {
  
public :
  
  /*!
    Constructor.
  */
  MED_FIELD_RDWR_DRIVER21():MED_FIELD_DRIVER21<T>() {}
  
  /*!
    Constructor.
  */
  template <class INTERLACING_TAG>
  MED_FIELD_RDWR_DRIVER21(const string & fileName,
			  FIELD<T, INTERLACING_TAG> * ptrField):
    MED_FIELD_WRONLY_DRIVER21<T>(fileName,ptrField),
    MED_FIELD_RDONLY_DRIVER21<T>(fileName,ptrField),
    IMED_FIELD_RDONLY_DRIVER<T>(fileName,ptrField),
    IMED_FIELD_WRONLY_DRIVER<T>(fileName,ptrField),
    MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::MED_RDWR),
    IMED_FIELD_RDWR_DRIVER<T>(fileName,ptrField)
  {
    BEGIN_OF("MED_FIELD_RDWR_DRIVER21::MED_FIELD_RDWR_DRIVER21(const string & fileName, const FIELD<T> * ptrField)");
    //_accessMode = MED_RDWR ;
    END_OF("MED_FIELD_RDWR_DRIVER21::MED_FIELD_RDWR_DRIVER21(const string & fileName, const FIELD<T> * ptrField)");
  };

  /*!
    Copy constructor.
  */
  MED_FIELD_RDWR_DRIVER21(const MED_FIELD_RDWR_DRIVER21 & fieldDriver):
    MED_FIELD_WRONLY_DRIVER21<T>(fieldDriver),
    MED_FIELD_RDONLY_DRIVER21<T>(fieldDriver),
    IMED_FIELD_RDWR_DRIVER<T>(fieldDriver),
    IMED_FIELD_RDONLY_DRIVER<T>(fieldDriver),
    IMED_FIELD_WRONLY_DRIVER<T>(fieldDriver),
    MED_FIELD_DRIVER<T>(fieldDriver)
  {};
  
  /*!
    Destructor.
  */
  ~MED_FIELD_RDWR_DRIVER21() {};

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

/*--------------------- DRIVER PART -------------------------------*/

template <class T> bool
MED_FIELD_DRIVER21<T>::createFieldSupport(med_2_1::med_idt id,
					string & fieldName,
					med_2_1::med_int ndt,
					med_2_1::med_int od,
					SUPPORT & support,
					string & meshName) const throw (MEDEXCEPTION)
{

  //EF : G�rer le meshName pour le driver 2.2
  const char * LOC="MED_FIELD_DRIVER<T>::search_field(...)";

  map<int, list<MED_EN::medGeometryElement> > CellAndNodeEntities;
  map<int, list<MED_EN::medGeometryElement> >::iterator currentEntity;
  CellAndNodeEntities[MED_EN::MED_CELL]  = MED_EN::meshEntities[MED_EN::MED_CELL];
  CellAndNodeEntities[MED_EN::MED_NODE] = MED_EN::meshEntities[MED_EN::MED_NODE];
  list< MED_EN::medGeometryElement >::const_iterator currentGeometry;

  //med_2_1::med_entite_maillage
  MED_EN::medEntityMesh entity;
  bool alreadyFoundAnEntity=false, alreadyFoundPdtIt = false, anyGauss=false;
  int  numberOfElements = 0;
  int  numberOfGeometricType = 0;
  //med_2_1::med_geometrie_element..
  MED_EN::medGeometryElement geometricType[MED_NBR_GEOMETRIE_MAILLE];
  int numberOfElementsOfType[MED_NBR_GEOMETRIE_MAILLE];
  int numberOfGaussPoint[MED_NBR_GEOMETRIE_MAILLE];

  med_2_1::med_int ngauss=0, numdt=-1, numo=-1, nbPdtIt=0; //nmaa=0
  char dtunit[MED_TAILLE_PNOM21+1], maa[MED_TAILLE_NOM+1];
  med_2_1::med_float   dt=-1.0;
  //med_2_1::med_booleen local;
  med_2_1::med_err     ret=1;

  for (currentEntity = CellAndNodeEntities.begin();
       currentEntity != CellAndNodeEntities.end(); currentEntity++) {
    for (currentGeometry  = (*currentEntity).second.begin();
	 currentGeometry != (*currentEntity).second.end(); currentGeometry++) {

      if ( (numberOfElements =  med_2_1::MEDnVal(id, const_cast <char*> ( fieldName.c_str() ),
						(med_2_1::med_entite_maillage)   (*currentEntity).first,
						(med_2_1::med_geometrie_element) *currentGeometry,
						ndt, od))  <=  0 )
	continue;

      if ( alreadyFoundAnEntity ) {
	if (entity != (*currentEntity).first )
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Field |"  << fieldName
				       << "| with (ndt,or) = (" << ndt << ","
				       << od << ") must not be defined on nodes and cells" ));

      } else { entity=(*currentEntity).first; alreadyFoundAnEntity = true; };

      nbPdtIt = med_2_1::MEDnPasdetemps(id, const_cast <char*> ( fieldName.c_str() ),
					(med_2_1::med_entite_maillage)   (*currentEntity).first,
					(med_2_1::med_geometrie_element)  *currentGeometry );
      if ( nbPdtIt < 0 )
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Field |" << fieldName << "| with (ndt,or) = ("
				     << ndt << "," << od << ") should be defined for (entityType,geometricType)=("
				     << MED_EN::entNames[(*currentEntity).first] << ","
				     << MED_EN::geoNames[*currentGeometry] << ")" ));

      ret = 0; alreadyFoundPdtIt = false; ngauss =0;
      for ( med_2_1::med_int j=1; j <= nbPdtIt; j++ ) {

	// Search how many <ngauss> (<fieldName>,<ndt>,<ot>) has
	ret = med_2_1::MEDpasdetempsInfo(id, const_cast <char*> ( fieldName.c_str() ),
					 (med_2_1::med_entite_maillage)   (*currentEntity).first,
					 (med_2_1::med_geometrie_element)  *currentGeometry,
					 j,maa,&ngauss,&numdt,dtunit,&dt,&numo);

	// 	    ret = med_2_2::MEDpasdetempsInfo(id, const_cast <char*> ( fieldName.c_str() ),
	// 				    (med_2_1::med_entite_maillage)   (*currentEntity).first,
	// 				    (med_2_1::med_geometrie_element)  *currentGeometry,
	// 				    j, &ngauss,  &numdt,  &numo, &dtunit, &dt, &maa, &local, &nmaa)

	if ( ndt == numdt && numo == od ) {
	  alreadyFoundPdtIt = true;
	  if ( ! meshName.empty() )
	    if ( meshName != maa ) {

	      //		  if ( nmaa > 1 )
	      {
		//EF : G�rer le meshName pour le driver 2.2
		// 		      MESSAGE(STRING(LOC)<<" Field |" << fieldName << "| with (ndt,or) = (" << ndt << ","
		// 			      << ot << ") is  defined on " << nmaa << " meshes, using mesh |"
		// 			      << maa << "|");
		// 		    }
		throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Field |" << fieldName << "| with (ndt,or) = ("
					     << ndt << "," << od << ") for (entityType,geometricType)=("
					     << MED_EN::entNames[(*currentEntity).first] << ","
					     << MED_EN::geoNames[*currentGeometry] << ")"
					     << "is defined on mesh |" << maa << "| not on mesh |" << meshName ));
	      }
	    }
	  break;
	}
      }

      if ( !alreadyFoundPdtIt )
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Field |" << fieldName << "| with (ndt,or) = ("
				     << ndt << "," << od << ") should be defined for (entityType,geometricType)=("
				     << MED_EN::entNames[(*currentEntity).first] << ","
				     << MED_EN::geoNames[*currentGeometry] << ")" ));

      if ( (ret != 0)  || (ngauss < 1 ) )
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Error in MEDpasdetempsInfo for  Field |" << fieldName 
				     << "| with (ndt,or) = ("
				     << ndt << "," << od << ") for (entityType,geometricType)=("
				     << MED_EN::entNames[(*currentEntity).first] << ","
				     << MED_EN::geoNames[*currentGeometry] << ")" )); ;

      //totalNumberOfElements+=numberOfElements;
      numberOfElementsOfType[numberOfGeometricType] = numberOfElements/ngauss;
      numberOfGaussPoint[numberOfGeometricType] = ngauss;
      anyGauss = (anyGauss || (ngauss-1) );
      geometricType[numberOfGeometricType]= *currentGeometry;
      numberOfGeometricType++;

    } // End Second For

  } // End Premier For

  if ( alreadyFoundAnEntity) {
    support.setName(fieldName+"Support");
    support.setMeshName(string(maa)); // V�rifier que les diff�rents noms de maillages lus soient identiques
    support.setEntity(entity);
    // REM : Signification pr�c�dente erron�e pour un champ qui ne repose pas sur toutes les entit�s g�om�triques 
    // du maillage mais dont le SUPPORT a �t� cr�e � partir des informations d'un maillage, comme
    // cel� f�t la plupart du temps.
    support.setNumberOfGeometricType(numberOfGeometricType);
    support.setGeometricType(geometricType); // Utile uniquement si setAll == false
    support.setNumberOfElements(numberOfElementsOfType);    //setNumberOfElements effectue une copie
    support.setAll(true);
    if (anyGauss)
      support.setNumberOfGaussPoint(numberOfGaussPoint);

    return alreadyFoundAnEntity;
  } else
    return false;
}

template <class T> void
MED_FIELD_DRIVER21<T>::getMeshGeometricType(med_2_1::med_idt id,
					  string & meshName,
					  MED_EN::medEntityMesh  entity,
					  vector<MED_EN::medGeometryElement> & geoType,
					  vector<int> &nbOfElOfType) const
{
  const char LOC[] = "MED_FIELD_DRIVER<T>::getMeshGeometricType(...)";

  int numberOfGeometricType=0;
  MED_EN::medGeometryElement geometricType[MED_NBR_GEOMETRIE_MAILLE];
  int numberOfElementsOfType[MED_NBR_GEOMETRIE_MAILLE];
  med_2_1::med_int   numberOfElements=0;
  med_2_1::med_table quoi;
  if (entity == MED_EN::MED_CELL) quoi=med_2_1::MED_CONN;
  else
    if (entity == MED_EN::MED_NODE) quoi=med_2_1::MED_COOR;
    else
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Support Creation from Mesh |"  << meshName
				   << "| on entity " << MED_EN::entNames[entity]
				   << "| is impossible,  must be  on MED_NODE or MED_CELL" ));

  list<MED_EN::medGeometryElement>::const_iterator currentGeometry;
  bool alreadyFoundAnEntity = false;

  for (currentGeometry  = (MED_EN::meshEntities[entity]).begin();
       currentGeometry != (MED_EN::meshEntities[entity]).end(); currentGeometry++) {


    if ( (numberOfElements =
	  med_2_1::MEDnEntMaa(id,
			      const_cast<char*> (meshName.c_str()),
			      quoi,
			      (med_2_1::med_entite_maillage)   entity,
			      (med_2_1::med_geometrie_element)  *currentGeometry,
			      med_2_1::MED_NOD) ) <= 0)
      continue;

    alreadyFoundAnEntity = true;
    numberOfElementsOfType[numberOfGeometricType] = numberOfElements;
    geometricType[numberOfGeometricType] = *currentGeometry;
    numberOfGeometricType++;

  }
  
  geoType = vector<MED_EN::medGeometryElement>(geometricType,geometricType+numberOfGeometricType);
  nbOfElOfType = vector<int> (numberOfElementsOfType,numberOfElementsOfType+numberOfGeometricType);

}

/*--------------------- RDONLY PART -------------------------------*/

template <class T> GENDRIVER * MED_FIELD_RDONLY_DRIVER21<T>::copy(void) const
{
  return new MED_FIELD_RDONLY_DRIVER21<T>(*this);
}

template <class T> void MED_FIELD_RDONLY_DRIVER21<T>::read(void)
  throw (MEDEXCEPTION)
{
  const char * LOC = " MED_FIELD_RDONLY_DRIVER21::read() " ;
  BEGIN_OF(LOC);

  typedef typename MEDMEM_ArrayInterface<T,NoInterlace,NoGauss>::Array   ArrayNo;
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


  MESSAGE("###### "<<LOC<<" fieldNameDRIVER : "<< MED_FIELD_DRIVER<T>::_fieldName << 
	  " fieldName : "<<MED_FIELD_DRIVER<T>::_fieldName);


  if (MED_FIELD_DRIVER<T>::_status!=MED_OPENED)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": Method open must be called before method read.")) ;

// EF :
//   Si un support a �t� donn�e au champ, pour des raisons de compatibilit� avec
//   les versions pr�c�dentes, ce support sera utilis� pour
//   - Obtenir le nom du maillage sur lequel on veut lire le champ
//     (eventuellement on pourrait l'utiliser pour selectionner un champ qui
//      repose sur plusieurs maillages cf HOMARD-ASTER)
//   -  v�rifier le type d'entit� (MED_NOEUD xor  MED_MAILLE xor MED_FACE xor MED_ARETE ) sur lequel
//      il faut lire le champ qui est �galement  retouv�.
//   - Si le support d�fini une liste d'entit� ( diff�rente de MED_ALL_ELEMENTS), celle-ci est ignor�e
//     � la lecture et �cras� par les listes de profils lus s'il en existe

//   Si aucun support n'a �t� donn� au champ :
//   - A la lecture : Un support est cr�e et le type d'entit� unique est lu
//                    (cf decision gt MED qu'un champ repose sur une entit� unique ?),
//                    l'ensemble des types g�om�triques est lu,
//                    l'ensemble des profils par type g�om�trique est lu
//                    Le nom du maillage associ� est lu mais le pointeur SUPPORT-MESH non initialis�

  char * fieldName = new char[MED_TAILLE_NOM+1] ;

  int err ;
  int    numberOfComponents          = 0;
  char * componentName               = (char *) MED_NULL;
  char * unitName                    = (char *) MED_NULL;
  med_2_1::med_type_champ type ;
  med_2_1::med_idt id = MED_FIELD_DRIVER21<T>::_medIdt;

  // we search for the field med number of <fieldName>
  // Having found <fieldName> variables <numberOfComponents>,
  // <componentName>, <unitname>, <type> and attribute <_fieldNum> are set.
  if (MED_FIELD_DRIVER<T>::_fieldNum==MED_INVALID)
    {
      int    numberOfFields              = 0;      //MED_INVALID
      numberOfFields = med_2_1::MEDnChamp(id,0) ;
      if ( numberOfFields <= 0 ) 
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": No Field found !"));

      for (int i=1;i<=numberOfFields;i++)
	{
	  numberOfComponents = med_2_1::MEDnChamp(id,i) ;
	  if ( numberOfComponents <= 0 ) 
	    //  	      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
	    //  					       <<  "Be careful there is no compound for field n�" 
	    //  					       << i << "in file |"<<_fileName<<"| !"));
	    MESSAGE(LOC<<"Be careful there is no compound for field n�"<<i<<"in file |"<<MED_FIELD_DRIVER<T>::_fileName<<"| !");

	  componentName = new char[numberOfComponents*MED_TAILLE_PNOM21+1] ;
	  unitName      = new char[numberOfComponents*MED_TAILLE_PNOM21+1] ;   
	    
	  err = med_2_1::MEDchampInfo(id, i, fieldName, &type, componentName, 
				      unitName, numberOfComponents) ;

	  MESSAGE("Champ "<<i<<" : #" << fieldName <<"# et recherche #"<<MED_FIELD_DRIVER<T>::_fieldName.c_str()<<"#");
	  if ( !strcmp(fieldName,MED_FIELD_DRIVER<T>::_fieldName.c_str()) )
	    {
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

  if (numberOfComponents < 1)
    {
      delete[] componentName; delete[] unitName;
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"no component found for field "
				   << MED_FIELD_DRIVER<T>::_fieldName)) ;
    }

  // Pourquoi _ptrField est toujour null et non MED_FIELD_DRIVER<T>::_ptrField
  switch ( (med_2_1::med_type_champ) MED_FIELD_DRIVER<T>::_ptrField->_valueType ) {
  case  med_2_1::MED_INT :
  case  med_2_1::MED_INT32 :
  case  med_2_1::MED_INT64 :
    if ( type == ( med_2_1::MED_REEL64 ) ) {
      delete[] componentName; delete[] unitName;
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Field Type in file (" << type
				   <<") differs from FIELD object type (" <<
				   MED_FIELD_DRIVER<T>::_ptrField->_valueType << ")" )) ;
    }
    break;
  default:
    break;
  }


  string meshName="";
  bool   haveSupport = false;
  if ( MED_FIELD_DRIVER<T>::_ptrField->getSupport() ) {
    // Verif sur la taille du meshName
    meshName =  MED_FIELD_DRIVER<T>::_ptrField->getSupport()->getMesh()->getName() ;
    haveSupport = true;
  }

  // Cherche le type d'entit�, le nombre d'entit�  par type g�om�trique sur le type d'entit�
  // (MED_MAILLE ou MED_NOEUD uniquement car MEDMEMOIRE ne g�re pas la connectivit� descendante).
  // et cr�e le support correspondant.
  SUPPORT * mySupport = new SUPPORT();
  bool found = createFieldSupport(id,MED_FIELD_DRIVER<T>::_fieldName,
				  MED_FIELD_DRIVER<T>::_ptrField->_iterationNumber,
				  MED_FIELD_DRIVER<T>::_ptrField->_orderNumber,
				  *mySupport, meshName) ;
  if ( !found ) {
    delete mySupport; delete[] componentName; delete[] unitName;
    MED_FIELD_DRIVER<T>::_fieldNum = MED_INVALID ;
     throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"  Can't find any entity for field |"
				 << MED_FIELD_DRIVER<T>::_fieldName
				 << "| with (it,or) = ("
				  << MED_FIELD_DRIVER<T>::_ptrField->_iterationNumber << ","
				 << MED_FIELD_DRIVER<T>::_ptrField->_orderNumber << "), on mesh "
				 << meshName << "|" ));
  }

  if (! haveSupport)
    meshName = mySupport->getMeshName();
  else {
    if ( mySupport->getEntity() != MED_FIELD_DRIVER<T>::_ptrField->getSupport()->getEntity() ) {
      MED_EN::medEntityMesh ent = mySupport->getEntity();
      delete mySupport; delete[] componentName; delete[] unitName;
      MED_FIELD_DRIVER<T>::_fieldNum = MED_INVALID ;
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"  Given entity |"
				   << MED_EN::entNames[MED_FIELD_DRIVER<T>::_ptrField->
						       getSupport()->getEntity()]
				   << "| for field |"
				   << MED_FIELD_DRIVER<T>::_fieldName
				   << "| with (it,or) = ("
				   << MED_FIELD_DRIVER<T>::_ptrField->_iterationNumber << ","
				   << MED_FIELD_DRIVER<T>::_ptrField->_orderNumber << "), on mesh "
				   << meshName << "| differs from found entity |"
				   << MED_EN::entNames[ent] << "|."
				   ));
    }
    mySupport->setName( MED_FIELD_DRIVER<T>::_ptrField->getSupport()->getName() );
    mySupport->setMesh( MED_FIELD_DRIVER<T>::_ptrField->getSupport()->getMesh() );
    mySupport->setDescription(MED_FIELD_DRIVER<T>::_ptrField->getSupport()->getDescription());
  }

  // Test si le Support du Champ repose ou non sur toutes les entit�s g�om�triques du maillage.
  // Pour tester les profils aussi ?
  vector< MED_EN::medGeometryElement >  meshGeoType;
  vector< int >  meshNbOfElOfType;
  getMeshGeometricType(id,meshName,mySupport->getEntity(),meshGeoType,meshNbOfElOfType);
  vector < MED_EN::medGeometryElement > v1(  mySupport->getTypes(),
					     mySupport->getTypes()+mySupport->getNumberOfTypes() );
  vector < int > v2(mySupport->getNumberOfElements(),
		    mySupport->getNumberOfElements()+mySupport->getNumberOfTypes() );
  if ( ( meshGeoType != v1 )  || meshNbOfElOfType != v2  ) {
    mySupport->setAll(false);
  }

  //??support->setNumber(MEDSKYLINEARRAY * Number);
  //??support->setNumber(const int * index, const int* value, bool shallowCopy=false);

  // If an error occurs while reading the field, these allocated FIELD member will be deleted
  MED_FIELD_DRIVER<T>::_ptrField->_name =  MED_FIELD_DRIVER<T>::_fieldName;
  MED_FIELD_DRIVER<T>::_ptrField->_numberOfComponents = numberOfComponents ;
  MED_FIELD_DRIVER<T>::_ptrField->_componentsTypes = new int[numberOfComponents] ;
  MED_FIELD_DRIVER<T>::_ptrField->_componentsNames = new string[numberOfComponents] ;
  MED_FIELD_DRIVER<T>::_ptrField->_componentsUnits = new UNIT[numberOfComponents] ;
  MED_FIELD_DRIVER<T>::_ptrField->_componentsDescriptions = new string[numberOfComponents] ;
  MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits = new string[numberOfComponents] ;

  for (int i=0; i<numberOfComponents; i++)
    {
      MED_FIELD_DRIVER<T>::_ptrField->_componentsTypes[i] = 1 ;
      MED_FIELD_DRIVER<T>::_ptrField->_componentsNames[i] = string(componentName,i*MED_TAILLE_PNOM21,MED_TAILLE_PNOM21) ;
      SCRUTE(MED_FIELD_DRIVER<T>::_ptrField->_componentsNames[i]);
      MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits[i] = string(unitName,i*MED_TAILLE_PNOM21,MED_TAILLE_PNOM21) ;
      SCRUTE(MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits[i]);
    }

  delete[] componentName;
  delete[] unitName;

  // read values for each geometric type in _support
  int NumberOfTypes = mySupport->getNumberOfTypes() ;
  const MED_EN::medGeometryElement *Types = mySupport->getTypes() ;
  T ** myValues = new T*[NumberOfTypes] ;
  int * NumberOfValues = new int[NumberOfTypes] ;
  int TotalNumberOfValues = 0 ; // Profils a gerer en 2.2 Rmq from EF
  MESSAGE ("NumberOfTypes :"<< NumberOfTypes);
  MED_FIELD_DRIVER<T>::_ptrField->_numberOfValues=0 ;

  for (int i=0; i<NumberOfTypes; i++) {
    MESSAGE ("Type["<<i+1<<"] :"<< Types[i]);
    MESSAGE ("Entity :"<< mySupport->getEntity());

//     NumberOfValues[i] = 
//       MEDnVal(MED_FIELD_DRIVER21<T>::_medIdt,
// 	      const_cast <char*> (MED_FIELD_DRIVER<T>::_fieldName.c_str()),
// 	      (med_2_1::med_entite_maillage)MED_FIELD_DRIVER<T>::_ptrField->_support->getEntity(),
// 	      (med_2_1::med_geometrie_element)Types[i],
// 	      MED_FIELD_DRIVER<T>::_ptrField->_iterationNumber,
// 	      MED_FIELD_DRIVER<T>::_ptrField->_orderNumber) ;

    NumberOfValues[i] = mySupport->getNumberOfElements(Types[i])
      * mySupport->getNumberOfGaussPoint(Types[i]);

    myValues[i] = new T[ NumberOfValues[i]*numberOfComponents ] ;
    TotalNumberOfValues+=NumberOfValues[i] ;
    char * ProfilName = new char[MED_TAILLE_NOM+1];
    MESSAGE ("NumberOfValues :"<< NumberOfValues[i]);
    MESSAGE ("NumberOfComponents :"<< numberOfComponents);
    MESSAGE ("MESH_NAME :"<< meshName.c_str());
    MESSAGE ("FIELD_NAME :"<< MED_FIELD_DRIVER<T>::_fieldName.c_str());
    MESSAGE ("MED_ENTITE :"<< (med_2_1::med_entite_maillage) mySupport->getEntity());
    MESSAGE("MED_GEOM :"<<(med_2_1::med_geometrie_element)Types[i]);
    MESSAGE("Iteration :"<<MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber());
    MESSAGE("Order :"<<MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber());
    MED_FIELD_DRIVER<T>::_ptrField->_numberOfValues+=mySupport->getNumberOfElements(Types[i]); // Ne doit pas prendre en compte les points de Gauss

    med_2_1::med_err ret;
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000)
    int lgth2=NumberOfValues[i]*numberOfComponents;
    if(MED_FIELD_DRIVER<T>::_ptrField->getValueType()==MED_EN::MED_INT32)
      {
	med_2_1::med_int *temp=new med_2_1::med_int[lgth2];
	ret=med_2_1::MEDchampLire(id,const_cast <char*> (meshName.c_str()),
				  const_cast <char*> (MED_FIELD_DRIVER<T>::_fieldName.c_str()),
				  (unsigned char*) temp,
				  med_2_1::MED_NO_INTERLACE,
				  MED_ALL,
				  ProfilName,
				  (med_2_1::med_entite_maillage) mySupport->getEntity(),
				  (med_2_1::med_geometrie_element) Types[i],
				  MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber(),
				  MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber()
				  );
	for(int i2=0;i2<lgth2;i2++)
	  myValues[i][i2]=(int)(temp[i2]);
	delete [] temp;
      }
    else
#endif
      ret=med_2_1::MEDchampLire(id,const_cast <char*> (meshName.c_str()),
				const_cast <char*> (MED_FIELD_DRIVER<T>::_fieldName.c_str()),
				(unsigned char*) myValues[i],
				med_2_1::MED_NO_INTERLACE,
				MED_ALL,
				ProfilName,
				(med_2_1::med_entite_maillage) mySupport->getEntity()
				,(med_2_1::med_geometrie_element)Types[i],
				MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber(),
				MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber()
				); 
    if (ret < 0)
      {
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
  // Creer un driver sp�cifique pour les modes MED_FULL_INTERLACE et MED_NO_INTERLACE
  // serait plus efficicace.
  ArrayNo * Values = new ArrayNo(numberOfComponents,TotalNumberOfValues);

  for (int i=0; i<numberOfComponents; i++)
    {
      //T * ValuesT = Values->getRow(i+1) ;
      int Count = 1 ;
      for (int j=0; j<NumberOfTypes; j++)
	{
	  T * myValue = myValues[j] ;
	  int NumberOf = NumberOfValues[j] ;
	  //	  MED_FIELD_DRIVER<T>::_ptrField->_numberOfValues+=NumberOf; // problem with gauss point : _numberOfValues != TotalNumberOfValues !!!!!!!
	  int offset = NumberOf*i ;
	  for (int k=0 ; k<NumberOf; k++) {
	    //ValuesT[Count]=myValue[k+offset] ;
	    Values->setIJ(Count,i+1,myValue[k+offset]);
	    //jfa 22.07.2005:SCRUTE(Count);
	    //jfa 22.07.2005:SCRUTE(Values->getIJ(Count,i+1));
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

  if ( MED_FIELD_DRIVER<T>::_ptrField->getInterlacingType() == MED_EN::MED_FULL_INTERLACE )
    {
      // dynamic_cast inutile
      MED_FIELD_DRIVER<T>::_ptrField->_value=dynamic_cast<ArrayFull *>(ArrayConvert(*Values));
      delete Values;
    }
  else
    MED_FIELD_DRIVER<T>::_ptrField->_value=Values;
  
  MED_FIELD_DRIVER<T>::_ptrField->_isRead = true ;

  MED_FIELD_DRIVER<T>::_ptrField->_support=mySupport; //Pr�venir l'utilisateur ?
      
  END_OF(LOC);
}

template <class T> void MED_FIELD_RDONLY_DRIVER21<T>::write( void ) const
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("MED_FIELD_RDONLY_DRIVER21::write : Can't write with a RDONLY driver !");
}

/*--------------------- WRONLY PART -------------------------------*/

template <class T> GENDRIVER * MED_FIELD_WRONLY_DRIVER21<T>::copy(void) const
{
  return new MED_FIELD_WRONLY_DRIVER21<T>(*this);
}

template <class T> void MED_FIELD_WRONLY_DRIVER21<T>::read (void)
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("MED_FIELD_WRONLY_DRIVER21::read : Can't read with a WRONLY driver !");
}

template <class T> void MED_FIELD_WRONLY_DRIVER21<T>::write(void) const
  throw (MEDEXCEPTION)
{
  const char * LOC = "MED_FIELD_WRONLY_DRIVER21::write(void) const " ;
  BEGIN_OF(LOC);

  typedef typename MEDMEM_ArrayInterface<T,NoInterlace,NoGauss>::Array ArrayNo;
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,NoGauss>::Array ArrayFull;

  if (MED_FIELD_DRIVER<T>::_status==MED_OPENED)
    {
      int err ;

      int component_count=MED_FIELD_DRIVER<T>::_ptrField->getNumberOfComponents();
      string   component_name(component_count*MED_TAILLE_PNOM21,' ') ;
      string   component_unit(component_count*MED_TAILLE_PNOM21,' ') ;

      const string * listcomponent_name=MED_FIELD_DRIVER<T>::_ptrField->getComponentsNames() ;
      const string * listcomponent_unit=MED_FIELD_DRIVER<T>::_ptrField->getMEDComponentsUnits() ;
      int length ;
      for (int i=0; i < component_count ; i++) {
	length = min(MED_TAILLE_PNOM21,(int)listcomponent_name[i].size());
	component_name.replace(i*MED_TAILLE_PNOM21,length,
			       listcomponent_name[i],0,length);
	length = min(MED_TAILLE_PNOM21,(int)listcomponent_unit[i].size());
	component_unit.replace(i*MED_TAILLE_PNOM21,length,
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
      int n = med_2_1::MEDnChamp(MED_FIELD_DRIVER21<T>::_medIdt,0);
      int nbComp ;
      for (int i=1; i<=n; i++) {
	nbComp = med_2_1::MEDnChamp(MED_FIELD_DRIVER21<T>::_medIdt,i);
	compName = new char[MED_TAILLE_PNOM21*nbComp+1];
	compUnit = new char[MED_TAILLE_PNOM21*nbComp+1];
	err = med_2_1::MEDchampInfo(MED_FIELD_DRIVER21<T>::_medIdt,i,champName,&type,compName,compUnit,nbComp);
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
        med_2_1::med_idt gid =  H5Gopen(MED_FIELD_DRIVER21<T>::_medIdt, dataGroupName.c_str() );
        
        if ( gid < 0 ) {
          // create field :
          err=med_2_1::MEDchampCr(MED_FIELD_DRIVER21<T>::_medIdt, 
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
      
      MESH * myMesh = mySupport->getMesh() ;
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
	// ( les infos _ptrField-> sont les m�mes )
	myArray = ArrayConvert( *( dynamic_cast< ArrayNo * >
				   (MED_FIELD_DRIVER<T>::_ptrField->getArrayNoGauss()
				    ))
				);
      }

      for (int i=0;i<NumberOfType;i++) {
	int NumberOfElements = mySupport->getNumberOfElements(Types[i]) ;
	
// 	const T * value = MED_FIELD_DRIVER<T>::_ptrField->getValueI(MED_EN::MED_FULL_INTERLACE,Index) ;

	value = myArray->getRow(Index) ;
	
	MESSAGE("MED_FIELD_DRIVER21<T>::_medIdt                         : "<<MED_FIELD_DRIVER21<T>::_medIdt);
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
	    err=med_2_1::MEDchampEcr(MED_FIELD_DRIVER21<T>::_medIdt, 
				    const_cast <char*> ( MeshName.c_str()) ,                         //( string(mesh_name).resize(MED_TAILLE_NOM).c_str())
				    const_cast <char*> ( (MED_FIELD_DRIVER<T>::_ptrField->getName()).c_str()),
				    (unsigned char*)temp, 
				    med_2_1::MED_FULL_INTERLACE,
				    NumberOfElements,
				    NumberOfGaussPoint[i],
				    MED_ALL,
				    MED_NOPFL,
				    med_2_1::MED_REMP,  // PROFIL NON GERE, mode de remplacement non g�r�
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
	err=med_2_1::MEDchampEcr(MED_FIELD_DRIVER21<T>::_medIdt, 
				const_cast <char*> ( MeshName.c_str()) ,                         //( string(mesh_name).resize(MED_TAILLE_NOM).c_str())
				const_cast <char*> ( (MED_FIELD_DRIVER<T>::_ptrField->getName()).c_str()),
				(unsigned char*)value, 
				med_2_1::MED_FULL_INTERLACE,
				NumberOfElements,
				NumberOfGaussPoint[i],
				MED_ALL,
				MED_NOPFL,
				med_2_1::MED_REMP,  // PROFIL NON GERE, mode de remplacement non g�r�
				(med_2_1::med_entite_maillage)mySupport->getEntity(),
				(med_2_1::med_geometrie_element)Types[i],
				MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber(),
				"        ",
				MED_FIELD_DRIVER<T>::_ptrField->getTime(),
				MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber()
				);
	if (err < MED_VALID )
	  {
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

template <class T> GENDRIVER * MED_FIELD_RDWR_DRIVER21<T>::copy(void) const
{
  return new MED_FIELD_RDWR_DRIVER21<T>(*this);
}

template <class T> void MED_FIELD_RDWR_DRIVER21<T>::write(void) const
  throw (MEDEXCEPTION)
{
  BEGIN_OF("MED_FIELD_RDWR_DRIVER21::write(void)");
  MED_FIELD_WRONLY_DRIVER21<T>::write(); 
  END_OF("MED_FIELD_RDWR_DRIVER21::write(void)");
} 

template <class T> void MED_FIELD_RDWR_DRIVER21<T>::read (void)
  throw (MEDEXCEPTION)
{
  BEGIN_OF("MED_FIELD_RDWR_DRIVER21::read(void)");
  MED_FIELD_RDONLY_DRIVER21<T>::read();
  END_OF("MED_FIELD_RDWR_DRIVER21::read(void)");
}
}//End namespace MEDMEM
/*-----------------------------------------------------------------*/

#endif /* MED_FIELD_DRIVER_HXX */

