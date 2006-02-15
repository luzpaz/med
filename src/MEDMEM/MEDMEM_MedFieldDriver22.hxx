#ifndef MED_FIELD_DRIVER22_HXX
#define MED_FIELD_DRIVER22_HXX

#include <string>
#include <algorithm>

#include "MEDMEM_FieldConvert.hxx"
#include "MEDMEM_ArrayInterface.hxx"
#include "MEDMEM_ArrayConvert.hxx"

#include "MEDMEM_define.hxx"
#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"

#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_MedFieldDriver.hxx"
#include "MEDMEM_Unit.hxx"
#include "MEDMEM_Support.hxx"


namespace MEDMEM {

/*!

  Driver Med for FIELD.

  Generic part : implement open and close methods.

*/

template <class T> class MED_FIELD_DRIVER22 : public virtual MED_FIELD_DRIVER<T>
{
protected:

  med_2_2::med_idt        _medIdt;
 
  bool createFieldSupportPart1(med_2_2::med_idt id,
			  string & fieldName,
			  med_2_2::med_int ndt,
			  med_2_2::med_int od,
			  SUPPORT & support,
			  string & meshName) const throw (MEDEXCEPTION);

  void getMeshGeometricTypeFromFile(med_2_2::med_idt id,
			    string & meshName,
			    MED_EN::medEntityMesh  entite,
			    vector<MED_EN::medGeometryElement> & geoType,
			    vector<int> &nbOfElOfType,
			    vector<int> &nbOfElOfTypeC) const throw(MEDEXCEPTION);

  void getMeshGeometricTypeFromMESH( MESH * meshPtr,
				     MED_EN::medEntityMesh  entity,
				     vector<MED_EN::medGeometryElement> & geoType,
				     vector<int> &nbOfElOfType,
				     vector<int> &nbOfElOfTypeC) const throw(MEDEXCEPTION);

public :

  /*!
    Constructor.
  */
  MED_FIELD_DRIVER22():MED_FIELD_DRIVER<T>(),_medIdt(MED_INVALID)
  {}
  /*!
    Constructor.
  */
  template <class INTERLACING_TAG>
  MED_FIELD_DRIVER22(const string & fileName,
		     FIELD<T, INTERLACING_TAG> * ptrField, 
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
    MED_FIELD_DRIVER22<T>::_medIdt = med_2_2::MEDouvrir( (const_cast <char *> (MED_FIELD_DRIVER<T>::_fileName.c_str())),(med_2_2::med_mode_acces) MED_FIELD_DRIVER<T>::_accessMode);
    MESSAGE(LOC<<"_medIdt : "<< MED_FIELD_DRIVER22<T>::_medIdt );
    if (MED_FIELD_DRIVER22<T>::_medIdt > 0) 
      MED_FIELD_DRIVER<T>::_status=MED_OPENED;
    else {
      MED_FIELD_DRIVER<T>::_status = MED_INVALID;
      MED_FIELD_DRIVER22<T>::_medIdt = MED_INVALID;
      throw MED_EXCEPTION (LOCALIZED( STRING(LOC) 
				      << "Can't open |"  << MED_FIELD_DRIVER<T>::_fileName 
				      << "|, _medIdt : " << MED_FIELD_DRIVER22<T>::_medIdt
				      )
			   );
    }

    END_OF(LOC);
  }
  
  void close() {
    BEGIN_OF("MED_FIELD_DRIVER22::close()");
    med_2_2::med_int err = 0;
    if (MED_FIELD_DRIVER<T>::_status == MED_OPENED) {
      err=med_2_2::MEDfermer(MED_FIELD_DRIVER22<T>::_medIdt);
      //H5close(); // If we call H5close() all the files are closed.
      MED_FIELD_DRIVER<T>::_status = MED_CLOSED;
      MED_FIELD_DRIVER22<T>::_medIdt = MED_INVALID;
      MESSAGE(" MED_FIELD_DRIVER22::close() : MEDfermer : _medIdt= " << MED_FIELD_DRIVER22<T>::_medIdt );
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
  template <class INTERLACING_TAG>
  MED_FIELD_RDONLY_DRIVER22(const string & fileName,
			    FIELD<T, INTERLACING_TAG> * ptrField):
    IMED_FIELD_RDONLY_DRIVER<T>(fileName,ptrField),
    MED_FIELD_DRIVER22<T>(fileName,ptrField,MED_EN::MED_RDONLY),
    MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::MED_RDONLY)
  { 
    BEGIN_OF("MED_FIELD_RDONLY_DRIVER22::MED_FIELD_RDONLY_DRIVER22(const string & fileName, const FIELD<T,INTERLACING_TAG> * ptrField)");
    END_OF("MED_FIELD_RDONLY_DRIVER22::MED_FIELD_RDONLY_DRIVER22(const string & fileName, const FIELD<T,INTERLACING_TAG> * ptrField)");
  }
  
  /*!
    Copy constructor.
  */
  MED_FIELD_RDONLY_DRIVER22(const MED_FIELD_RDONLY_DRIVER22 & fieldDriver):
    IMED_FIELD_RDONLY_DRIVER<T>(fieldDriver),
    MED_FIELD_DRIVER22<T>(fieldDriver),
    MED_FIELD_DRIVER<T>(fieldDriver)
  {}
  
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
  template <class INTERLACING_TAG>
  MED_FIELD_WRONLY_DRIVER22(const string & fileName,
			    FIELD<T, INTERLACING_TAG> * ptrField):
    IMED_FIELD_WRONLY_DRIVER<T>(fileName,ptrField),
    MED_FIELD_DRIVER22<T>(fileName,ptrField,MED_EN::MED_WRONLY),
    MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::MED_WRONLY)
  {
    BEGIN_OF("MED_FIELD_WRONLY_DRIVER22::MED_FIELD_WRONLY_DRIVER22(const string & fileName, const FIELD<T,INTERLACING_TAG> * ptrField)");
    END_OF("MED_FIELD_WRONLY_DRIVER22::MED_FIELD_WRONLY_DRIVER22(const string & fileName, const FIELD<T,INTERLACING_TAG> * ptrField)");
  }

  /*!
    Copy constructor.
  */
  MED_FIELD_WRONLY_DRIVER22(const MED_FIELD_WRONLY_DRIVER22 & fieldDriver):
    IMED_FIELD_WRONLY_DRIVER<T>(fieldDriver),
    MED_FIELD_DRIVER22<T>(fieldDriver),
    MED_FIELD_DRIVER<T>(fieldDriver)
  {}
  
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
  template <class INTERLACING_TAG>
  MED_FIELD_RDWR_DRIVER22(const string & fileName,
			  FIELD<T, INTERLACING_TAG> * ptrField):
    MED_FIELD_WRONLY_DRIVER22<T>(fileName,ptrField),
    MED_FIELD_RDONLY_DRIVER22<T>(fileName,ptrField),
    IMED_FIELD_RDONLY_DRIVER<T>(fileName,ptrField),
    IMED_FIELD_WRONLY_DRIVER<T>(fileName,ptrField),
    MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::MED_RDWR),
    IMED_FIELD_RDWR_DRIVER<T>(fileName,ptrField)
  {
    BEGIN_OF("MED_FIELD_RDWR_DRIVER22::MED_FIELD_RDWR_DRIVER22(const string & fileName, const FIELD<T,INTERLACING_TAG> * ptrField)");
    //_accessMode = MED_RDWR ;
    END_OF("MED_FIELD_RDWR_DRIVER22::MED_FIELD_RDWR_DRIVER22(const string & fileName, const FIELD<T,INTERLACING_TAG> * ptrField)");
  }

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


/*!

  Cette méthode crée le SUPPORT du champ <fieldName> pour le 
	<n°de pas de temps,n°d'itération>=<ndt,od>. 
  
  Le SUPPORT crée à pour nom  <fieldName>Support et contient
  la liste des types géométriques sur le premier type
  d'entité trouvé (en MEDMEM on inderdit aux champs de reposer
  sur plusieurs types d'entité).
  Il contient également le nombre d'entités trouvées pour chaque 
  type géométrique.
  Par défaut l'attribut onAll du SUPPORT est positionné à true car
  cette routine ne lit rien de ce qui concerne les entités
  du maillage associé.
  La méthode renvoie true si elle réussit à créer le SUPPORT
  demandé. 
  Le nom du maillage associé ( en MEDMEM on ne
  supporte pas encore les maillages multiples ) est renvoyé dans <meshName>.
*/

template <class T> bool
MED_FIELD_DRIVER22<T>::createFieldSupportPart1(med_2_2::med_idt id,
					string & fieldName,
					med_2_2::med_int ndt,
					med_2_2::med_int od,
					SUPPORT & support,
					string & meshName) const throw (MEDEXCEPTION)
{

  //EF : Gérer le meshName pour le driver 2.2
  const char * LOC="MED_FIELD_DRIVER<T>::createFieldSupportPart1(...)";

  map<int, list<MED_EN::medGeometryElement> > CellAndNodeEntities;
  map<int, list<MED_EN::medGeometryElement> >::iterator currentEntity;
  CellAndNodeEntities[MED_EN::MED_CELL]  = MED_EN::meshEntities[MED_EN::MED_CELL];
  CellAndNodeEntities[MED_EN::MED_NODE] = MED_EN::meshEntities[MED_EN::MED_NODE];
  list< MED_EN::medGeometryElement >::const_iterator currentGeometry;

  //med_2_2::med_entite_maillage
  MED_EN::medEntityMesh entity;
  bool alreadyFoundAnEntity=false,alreadyFoundPdtIt = false, anyGauss=false;
  int  numberOfElements = 0;
  int  numberOfGeometricType = 0;
  //med_2_2::med_geometrie_element..
  MED_EN::medGeometryElement geometricType[MED_NBR_GEOMETRIE_MAILLE];
  int numberOfElementsOfType[MED_NBR_GEOMETRIE_MAILLE];
  int numberOfGaussPoint[MED_NBR_GEOMETRIE_MAILLE];

  med_2_2::med_int nmaa=0, ngauss=0, numdt=-1, numo=-1, nbPdtIt=0;
  char dtunit[MED_TAILLE_PNOM22+1];
  char maa[MED_TAILLE_NOM+1];
  med_2_2::med_float   dt=-1.0;
  med_2_2::med_booleen local;
  med_2_2::med_err     ret=1;

  /* Détermine le type d'entité et la liste des types géométriques associés
     au champ <fieldName> */
  for (currentEntity = CellAndNodeEntities.begin();
       currentEntity != CellAndNodeEntities.end(); currentEntity++) {
    for (currentGeometry  = (*currentEntity).second.begin();
	 currentGeometry != (*currentEntity).second.end(); currentGeometry++) {


      if ( (nbPdtIt = med_2_2::MEDnPasdetemps(id, const_cast <char*> ( fieldName.c_str() ),
					      (med_2_2::med_entite_maillage)   (*currentEntity).first,
					      (med_2_2::med_geometrie_element)  *currentGeometry ))  <=  0 )
	continue;

      /* Verifie que le champ n'est pas défini sur un autre type d'entité */
      if ( alreadyFoundAnEntity ) {
	if (entity != (*currentEntity).first )
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Field |"  << fieldName
				       << "| with (ndt,or) = (" << ndt << ","
				       << od << ") must not be defined on nodes and cells" ));

      } else { entity=(*currentEntity).first; alreadyFoundAnEntity = true; };


      /* Cherche le champ pour le <ndt>,<ot> demandé et détermine le nombre de points de Gauss*/
      ret = 0; alreadyFoundPdtIt = false; ngauss =0;
      for ( med_2_2::med_int j=1; j <= nbPdtIt; j++ ) {

	// Search how many <ngauss> (<fieldName>,<ndt>,<ot>) has
	ret += med_2_2::MEDpasdetempsInfo(id, const_cast <char*> ( fieldName.c_str() ),
					 (med_2_2::med_entite_maillage)   (*currentEntity).first,
					 (med_2_2::med_geometrie_element)  *currentGeometry,
					 j, &ngauss,  &numdt,  &numo, dtunit, &dt, 
					  maa, &local, &nmaa);

	if ( ndt == numdt && numo == od ) {
	  alreadyFoundPdtIt = true;

	  if ( nmaa > 1 ) {
	    MESSAGE(LOC<<" Field |" << fieldName << "| with (ndt,or) = ("
		    << ndt << "," << od << ") for (entityType,geometricType)=("
		    << MED_EN::entNames[(*currentEntity).first] << ","
		    << MED_EN::geoNames[*currentGeometry] << ")"
		    << "is defined on multiple meshes, using dafault mesh  |" << maa << "|" );
	  }

	  if ( !local) {
	    MESSAGE(" Field |" << fieldName << "| with (ndt,or) = ("
		    << ndt << "," << od << ") for (entityType,geometricType)=("
		    << MED_EN::entNames[(*currentEntity).first] << ","
		    << MED_EN::geoNames[*currentGeometry] << ")"
		    << "is using a mesh on a distant file (ignored)" );

// 	      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Field |" << fieldName << "| with (ndt,or) = ("
// 					   << ndt << "," << od << ") for (entityType,geometricType)=("
// 					   << MED_EN::entNames[(*currentEntity).first] << ","
// 					   << MED_EN::geoNames[*currentGeometry] << ")"
// 					   << "is using a mesh on a different file which is not yet supported" ));
	  }

	  if ( ! meshName.empty() )
	    if ( meshName != maa ) {
	      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Field |" << fieldName << "| with (ndt,or) = ("
					   << ndt << "," << od << ") for (entityType,geometricType)=("
					   << MED_EN::entNames[(*currentEntity).first] << ","
					   << MED_EN::geoNames[*currentGeometry] << ")"
					   << "is defined on mesh |" << maa << "| not on mesh |" << meshName ));
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

      if ( (numberOfElements =  med_2_2::MEDnVal(id, const_cast <char*> ( fieldName.c_str() ),
						(med_2_2::med_entite_maillage)   (*currentEntity).first,
						(med_2_2::med_geometrie_element) *currentGeometry,
						 numdt, numo, maa, med_2_2::MED_COMPACT))  <=  0 )
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Error in MEDnVal for  Field |" << fieldName
				     << "| with (ndt,or) = ("
				     << ndt << "," << od << ") for (entityType,geometricType)=("
				     << MED_EN::entNames[(*currentEntity).first] << ","
				     << MED_EN::geoNames[*currentGeometry] << ")" )); ;

      //totalNumberOfElements+=numberOfElements;
      numberOfElementsOfType[numberOfGeometricType] = numberOfElements/ngauss;
      numberOfGaussPoint[numberOfGeometricType] = ngauss;
      anyGauss = (anyGauss || (ngauss-1) ); // rem : pas de point de Gauss <=> ngauss=1
      geometricType[numberOfGeometricType]= *currentGeometry;
      numberOfGeometricType++;

    } // End Second For

  } // End Premier For

  if ( alreadyFoundAnEntity) {
    support.setName(fieldName+"Support");
    support.setMeshName(string(maa)); // Vérifier que les différents noms de maillages lus soient identiques
    support.setEntity(entity);
    // REM : Le nombre <numberOfGeometricType> dans la précédente version du Driver 
    //       était erronée pour un champ qui ne reposait pas sur toutes les entités géométriques 
    //       du maillage mais dont le SUPPORT a été crée à partir des informations du maillage
    //       ( méthode qui était largement utilisée pour construire un SUPPORT).
    support.setNumberOfGeometricType(numberOfGeometricType);
    support.setGeometricType(geometricType); // Utile uniquement si setAll == false ?
    support.setNumberOfElements(numberOfElementsOfType);    //setNumberOfElements effectue une copie 
    // Par défaut considère que le champ repose sur tous les type géométriques du maillage
    // Si ce n'est pas le cas les champs geometricType et numberOfElementsOfType du SUPPORT sont corrects
    support.setAll(true);
    if (anyGauss)
      support.setNumberOfGaussPoint(numberOfGaussPoint);

    return alreadyFoundAnEntity;
  } else
    return false;
}

/*!

  Renvoie la liste <geoType> des types géométriques définis dans le maillage <meshName>
  pour le type d'entité <entity>.
  * < nbOfElOfType > contient le nombre d'entités de chaque type
  * < numberOfElementsOfTypeC > contient le nombre d'entités cumulées de chaque type
                              avec numberOfElementsOfTypeC[0]=0;

*/
template <class T> void
MED_FIELD_DRIVER22<T>::getMeshGeometricTypeFromFile(med_2_2::med_idt id,
					  string & meshName,
					  MED_EN::medEntityMesh  entity,
					  vector<MED_EN::medGeometryElement> & geoType,
					  vector<int> &nbOfElOfType,
					  vector<int> &nbOfElOfTypeC) const throw(MEDEXCEPTION)
{
  const char LOC[] = "MED_FIELD_DRIVER<T>::getMeshGeometricTypeFromFile(...)";

  int numberOfGeometricType=0;
  MED_EN::medGeometryElement geometricType[MED_NBR_GEOMETRIE_MAILLE];
  int numberOfElementsOfType[MED_NBR_GEOMETRIE_MAILLE];
  int numberOfElementsOfTypeC[MED_NBR_GEOMETRIE_MAILLE+1];
  med_2_2::med_int   numberOfElements=0;
  med_2_2::med_table quoi;
  if (entity == MED_EN::MED_CELL) quoi=med_2_2::MED_CONN;
  else
    if (entity == MED_EN::MED_NODE) quoi=med_2_2::MED_COOR;
    else
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Support Creation from Mesh |"  << meshName
				   << "| on entity " << MED_EN::entNames[entity]
				   << "| is impossible,  must be  on MED_NODE or MED_CELL" ));

  list<MED_EN::medGeometryElement>::const_iterator currentGeometry;
  bool alreadyFoundAnEntity = false;
  numberOfElementsOfTypeC[0]=0;

  for (currentGeometry  = (MED_EN::meshEntities[entity]).begin();
       currentGeometry != (MED_EN::meshEntities[entity]).end(); currentGeometry++) {


    if ( (numberOfElements =
	  med_2_2::MEDnEntMaa(id,
			      const_cast<char*> (meshName.c_str()),
			      quoi,
			      (med_2_2::med_entite_maillage)   entity,
			      (med_2_2::med_geometrie_element)  *currentGeometry,
			      med_2_2::MED_NOD) ) <= 0)
      continue;

    alreadyFoundAnEntity = true;
    numberOfElementsOfType[numberOfGeometricType] = numberOfElements;
    numberOfElementsOfTypeC[numberOfGeometricType+1] =
      numberOfElementsOfTypeC[numberOfGeometricType]+numberOfElements;
    geometricType[numberOfGeometricType] = *currentGeometry;
    numberOfGeometricType++;

  }

  geoType = vector<MED_EN::medGeometryElement>(geometricType,geometricType+numberOfGeometricType);
  nbOfElOfType = vector<int> (numberOfElementsOfType,numberOfElementsOfType+numberOfGeometricType);
  nbOfElOfTypeC = vector<int> (numberOfElementsOfTypeC,numberOfElementsOfTypeC+numberOfGeometricType+1);

//    for (int j =0 ; j< numberOfGeometricType;++j)
//      cout << "nbOfElOfTypeC["<<j<<"]="<<nbOfElOfTypeC[j]<<endl;

}

template <class T> void
MED_FIELD_DRIVER22<T>::getMeshGeometricTypeFromMESH( MESH * meshPtr,
					  MED_EN::medEntityMesh  entity,
					  vector<MED_EN::medGeometryElement> & geoType,
					  vector<int> &nbOfElOfType,
					  vector<int> &nbOfElOfTypeC) const throw(MEDEXCEPTION)
{
  const char LOC[] = "MED_FIELD_DRIVER<T>::getMeshGeometricTypeFromMESH(...) : ";
  BEGIN_OF(LOC);

  if (!meshPtr)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"ptrMesh must be non null" )); ;

  // Il est plus pratique de créer un support "onAll"
  // pour calculer les tableaux du nombre d'entités cumulées

  SUPPORT mySupportFromMesh = SUPPORT(meshPtr,"Temporary Support From Associated Mesh",
				      entity);
  geoType = vector<MED_EN::medGeometryElement>(mySupportFromMesh.getTypes(),
			      mySupportFromMesh.getTypes()+mySupportFromMesh.getNumberOfTypes());
  nbOfElOfType.resize(mySupportFromMesh.getNumberOfTypes());
  nbOfElOfTypeC.resize(mySupportFromMesh.getNumberOfTypes()+1);
  nbOfElOfTypeC[0]=0;

  for (int j=1; j<=mySupportFromMesh.getNumberOfTypes(); ++j) {
    nbOfElOfType[j-1]=mySupportFromMesh.getNumberOfElements(geoType[j-1]);
    nbOfElOfTypeC[j]+=nbOfElOfTypeC[j-1]+nbOfElOfType[j-1];
  }

  END_OF(LOC);
}

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

  typedef typename MEDMEM_ArrayInterface<T,NoInterlace,NoGauss>::Array   ArrayNo;
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,NoGauss>::Array ArrayFull;

  if (MED_FIELD_DRIVER<T>::_status!=MED_OPENED)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": Method open must be called before method read.")) ;

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

// EF :
//   Si un support a été donnée au champ, pour des raisons de compatibilité avec
//   les versions précédentes, ce support sera utilisé pour
//   - Obtenir le nom du maillage sur lequel on veut lire le champ
//     (eventuellement on pourrait l'utiliser pour selectionner un champ qui
//      repose sur plusieurs maillages cf HOMARD-ASTER, ce qui n'est pas géré dans MEDMEM)
//   -  vérifier le type d'entité (MED_NOEUD xor MED_MAILLE xor MED_FACE xor MED_ARETE ) sur lequel
//      il faut lire le champ qui est également retouvé.
//   - Si le support défini une liste d'entité ( différente de MED_ALL_ELEMENTS), celle-ci est ignorée
//     à la lecture et écrasé par soit :
//            - onall, après avoir vérifié que la liste des types géométriques utilisés par le champ
//		       est égale à la liste des type géométriques définis dans le maillage associé
//                     pour tous le même type d'entité.
//            - La sous liste des types géométriques utilisés (onAll quand même, cf commenataire ci-dessous )  
//            - les listes de profils lus s'il en existe pour une sous liste de types
//              géométriques

//   Si aucun support n'a été donné au champ :
//   - A la lecture : Un support est crée et le type d'entité unique est lu
//                    (cf decision gt MED qu'un champ repose sur une entité unique ?),
//                    l'ensemble des types géométriques est lu,
//                    l'ensemble des profils par type géométrique est lu
//                    Le nom du maillage associé est lu mais le pointeur SUPPORT-MESH non initialisé


  char * fieldName = new char[MED_TAILLE_NOM+1] ;
  int err ;
  int    numberOfComponents          = 0;
  char * componentName               = (char *) MED_NULL;
  char * unitName                    = (char *) MED_NULL;
  med_2_2::med_type_champ type ;
  med_2_2::med_idt id = MED_FIELD_DRIVER22<T>::_medIdt;

  // we search for the "field med number" of <fieldName>
  // Having found <fieldName>, variables <numberOfComponents>,
  // <componentName>, <unitname>, <type> and attribute <_fieldNum> are set.
  if (MED_FIELD_DRIVER<T>::_fieldNum==MED_INVALID)
    {
      int numberOfFields = med_2_2::MEDnChamp(id,0) ;
      if ( numberOfFields <= 0 )
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": There is no field found in the file !"));

      for (int i=1;i<=numberOfFields;i++)
	{
	  numberOfComponents = med_2_2::MEDnChamp(id,i) ;

	  if ( numberOfComponents <= 0 )
	    MESSAGE(LOC<<"Be careful there is no compound for field nÂ°"<<i<<"in file |"<<MED_FIELD_DRIVER<T>::_fileName<<"| !");

	  componentName = new char[numberOfComponents*MED_TAILLE_PNOM22+1] ;
	  unitName      = new char[numberOfComponents*MED_TAILLE_PNOM22+1] ;

	  err = med_2_2::MEDchampInfo(id, i, fieldName, &type, componentName,
				      unitName, numberOfComponents) ;

	  MESSAGE("Field "<<i<<" : #" << fieldName <<"# et recherche #"<<MED_FIELD_DRIVER<T>::_fieldName.c_str()<<"#");
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

  // Si aucun champ ne correspond les variables <componentName> et <unitName> ont été correctement
  // désallouées dans la boucle de recherche
  if (MED_FIELD_DRIVER<T>::_fieldNum==MED_INVALID)
    throw MEDEXCEPTION(LOCALIZED( STRING(LOC) << ": Field "<<  MED_FIELD_DRIVER<T>::_fieldName
				   << " not found in file " << MED_FIELD_DRIVER<T>::_fileName) );

  MESSAGE ("FieldNum : "<<MED_FIELD_DRIVER<T>::_fieldNum);

  if (numberOfComponents < 1) {
    delete[] componentName; delete[] unitName;
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" no component found for field "
				 << MED_FIELD_DRIVER<T>::_fieldName)) ;
  }

  // REMARQUE : La lecture d'entier dans un FIELD<double> n'est pas gérée correctement
  switch ( (med_2_2::med_type_champ) MED_FIELD_DRIVER<T>::_ptrField->_valueType ) {
  case  med_2_2::MED_INT :
  case  med_2_2::MED_INT32 :
  case  med_2_2::MED_INT64 :
    if ( type == ( med_2_2::MED_FLOAT64 ) ) {
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
  MESH * ptrMesh = 0;
  bool   haveSupport = false;
  bool   haveMesh    = false;
  if ( MED_FIELD_DRIVER<T>::_ptrField->getSupport() ) {
    // Verif à faire sur la taille du meshName
    ptrMesh = MED_FIELD_DRIVER<T>::_ptrField->getSupport()->getMesh();
    if ( ptrMesh) {
      meshName =  MED_FIELD_DRIVER<T>::_ptrField->getSupport()->getMesh()->getName() ;
      haveMesh = true;
    }
    haveSupport = true;
  }

  // Cherche le type d'entité, le nombre d'entité  par type géométrique sur le type d'entité
  // (MED_MAILLE ou MED_NOEUD uniquement car MEDMEMOIRE ne gère pas la connectivité descendante).
  // et crée le support correspondant.
  SUPPORT * mySupport = new SUPPORT();
  bool found = createFieldSupportPart1(id,MED_FIELD_DRIVER<T>::_fieldName,
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

  MED_EN::medEntityMesh entityType = mySupport->getEntity();
  //Si un SUPPORT était donné, récupère son nom, sa description et
  //	 le pointeur du maillage associé 
  if (! haveSupport)
    meshName = mySupport->getMeshName();
  else {
    if ( mySupport->getEntity() != MED_FIELD_DRIVER<T>::_ptrField->getSupport()->getEntity() ) {
      delete mySupport; delete[] componentName; delete[] unitName;
      MED_FIELD_DRIVER<T>::_fieldNum = MED_INVALID ;
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Given entity |"
				   << MED_EN::entNames[MED_FIELD_DRIVER<T>::_ptrField->
						       getSupport()->getEntity()]
				   << "| for field |"
				   << MED_FIELD_DRIVER<T>::_fieldName
				   << "| with (it,or) = ("
				   << MED_FIELD_DRIVER<T>::_ptrField->_iterationNumber << ","
				   << MED_FIELD_DRIVER<T>::_ptrField->_orderNumber << "), on mesh "
				   << meshName << "| differs from found entity |"
				   << MED_EN::entNames[entityType] << "|."
				   ));
    }
    mySupport->setName( MED_FIELD_DRIVER<T>::_ptrField->getSupport()->getName() );
    mySupport->setMesh( MED_FIELD_DRIVER<T>::_ptrField->getSupport()->getMesh() );
    mySupport->setDescription(MED_FIELD_DRIVER<T>::_ptrField->getSupport()->getDescription());
  }

  vector< MED_EN::medGeometryElement >  MESHgeoType;
  vector< int >  MESHnbOfElOfType;
  vector< int >  MESHnbOfElOfTypeC;
  if ( haveMesh )
    getMeshGeometricTypeFromMESH(ptrMesh,entityType,MESHgeoType,
				 MESHnbOfElOfType,MESHnbOfElOfTypeC);

  int fileHasMesh = ( med_2_2::MEDdimLire(id, const_cast<char *>(meshName.c_str())) > 0);
  vector< MED_EN::medGeometryElement >  meshGeoType;
  vector< int >  meshNbOfElOfType;
  vector< int >  meshNbOfElOfTypeC;
  // Si le maillage n'est pas trouvé les tableaux renvoyés sont vides
  if (fileHasMesh)
    getMeshGeometricTypeFromFile(id,meshName,entityType,meshGeoType,
				 meshNbOfElOfType,meshNbOfElOfTypeC);


  if (fileHasMesh && haveSupport )
    if ( ( meshGeoType != MESHgeoType ) || (meshNbOfElOfTypeC != MESHnbOfElOfTypeC) )
      throw MEDEXCEPTION(LOCALIZED( STRING(LOC) <<": Error while getting mesh information from file for FIELD "<< fieldName
				    << " on entity " << MED_EN::entNames[entityType]
				    << " with (it,or) = ("
				    << MED_FIELD_DRIVER<T>::_ptrField->_iterationNumber << ","
				    << MED_FIELD_DRIVER<T>::_ptrField->_orderNumber << ")"
				    << " on mesh " << meshName
				    << " : geometric types or number of elements by type differs from MESH object !"
				    )
			 );

  if ( !fileHasMesh && !haveSupport )
    throw MEDEXCEPTION(LOCALIZED( STRING(LOC) <<": Error while getting mesh information for FIELD "<< fieldName
				  << " on entity " << MED_EN::entNames[entityType]
				  << " with (it,or) = ("
				  << MED_FIELD_DRIVER<T>::_ptrField->_iterationNumber << ","
				  << MED_FIELD_DRIVER<T>::_ptrField->_orderNumber << ")"
				  << " on mesh " << meshName
				  << " : SUPPORT must contain a valid MESH reference or file must contain the associated MESH."
				  )
		       );


  if (!fileHasMesh && haveSupport) {
    meshNbOfElOfTypeC = MESHnbOfElOfTypeC;
    meshGeoType       = MESHgeoType;
    meshNbOfElOfType  = MESHnbOfElOfType;
  }


  // Test si le Support du Champ repose ou non sur toutes les entités géométriques
  // du maillage associé et positionne ou non l'attribut onAll du SUPPORT.
  // Il ne s'agit pas de la gestion des profils
  vector < MED_EN::medGeometryElement > v1(  mySupport->getTypes(),
					     mySupport->getTypes()+mySupport->getNumberOfTypes() );
  vector < int > v2(mySupport->getNumberOfElements(),
		    mySupport->getNumberOfElements()+mySupport->getNumberOfTypes() );
  if ( ( meshGeoType != v1 )  || meshNbOfElOfTypeC != v2  ) {
    // ATTENTION : mySupport->setAll(false);
    // Pb : On a envie de positionner onAll à faux si le champ n'est pas défini sur tous les
    //      types géométriques du maillage associé.
    //      Mais si onAll est false et si aucun profil n'est détecté par la suite,
    //      l'attribut SUPPORT->_number est censé être positionné quand même ! Que faire ?
    // Si on veut être compatible avec la signification première de onAll,
    //  il faudrait créer des profils contenant toutes les entités pour chaque type géométrique
    //  du SUPPORT  mais d'une part c'est dommage d'un point de vue de l'emcombrement mémoire
    //  et d'autre part, à la réécriture du fichier MED on stockera des profils 
    //  alors qu'il n'y en avait pas à l'origine (fichier MED différent après lecture/écriture) !
    // Si on laisse setAll à vrai il faut être sûr que les utilisateurs prennent les
    //  informations sur les types gémétrique au niveau du support et non pas du maillage.
    // Solution : Signification du onAll -> onAllElements des type géométriques définis
    // dans le SUPPORT et non du maillage associé (dans la plupart des cas si le fichier ne
    // contient pas de profil, le champ est défini sur toutes les entités de tous les types
    // géométriques définis dans le maillage).
  }


  // If an error occurs while reading the field, these allocated FIELD member will be deleted

  MED_FIELD_DRIVER<T>::_ptrField->_name =  MED_FIELD_DRIVER<T>::_fieldName;
  MED_FIELD_DRIVER<T>::_ptrField->_numberOfComponents = numberOfComponents ;
  MED_FIELD_DRIVER<T>::_ptrField->_componentsTypes = new int[numberOfComponents] ;
  MED_FIELD_DRIVER<T>::_ptrField->_componentsNames = new string[numberOfComponents] ;
  MED_FIELD_DRIVER<T>::_ptrField->_componentsUnits = new UNIT[numberOfComponents] ;
  MED_FIELD_DRIVER<T>::_ptrField->_componentsDescriptions = new string[numberOfComponents] ;
  MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits = new string[numberOfComponents] ;
  for (int i=0; i<numberOfComponents; i++) {
      MED_FIELD_DRIVER<T>::_ptrField->_componentsTypes[i] = 1 ;
      MED_FIELD_DRIVER<T>::_ptrField->_componentsNames[i] = string(componentName,i*MED_TAILLE_PNOM22,MED_TAILLE_PNOM22) ;
      SCRUTE(MED_FIELD_DRIVER<T>::_ptrField->_componentsNames[i]);
      MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits[i] = string(unitName,i*MED_TAILLE_PNOM22,MED_TAILLE_PNOM22) ;
      SCRUTE(MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits[i]);
  }
  delete[] componentName;
  delete[] unitName;

  int NumberOfTypes = mySupport->getNumberOfTypes() ;
  const MED_EN::medGeometryElement *Types = mySupport->getTypes() ;
  T ** myValues = new T*[NumberOfTypes] ;
  int * NumberOfValues = new int[NumberOfTypes] ;
  int   TotalNumberOfValues = 0 ;
  bool anyProfil = false;
  int  pflSize=0;
  // Le vecteur de profil est dimensionné par rapport aux nombres de types
  // géométriques du champ même si le champ n'a pas de profil MED FICHIER sur
  // tous ses types géométriques car dans MEDMEM si onAllElement 
  // du SUPPORT est false il faut positionner un profil pour tous les types géométriques 
  // du SUPPORT
  vector < int   > profilSize(NumberOfTypes,0);
  int profilSizeC = 0;
  vector < vector<int>  > profilList(NumberOfTypes);
  vector < string > profilNameList(NumberOfTypes);
  MESSAGE ("NumberOfTypes :"<< NumberOfTypes);
  MED_FIELD_DRIVER<T>::_ptrField->_numberOfValues=0 ;

  char * profilName = new char[MED_TAILLE_NOM+1];

  for (int i=0; i<NumberOfTypes; i++) {

    NumberOfValues[i] = mySupport->getNumberOfElements(Types[i])
      * mySupport->getNumberOfGaussPoint(Types[i]);
    myValues[i] = new T[ NumberOfValues[i]*numberOfComponents ] ;
    TotalNumberOfValues+=NumberOfValues[i] ;
    char * LocalGaussName = new char[MED_TAILLE_NOM+1];

    MESSAGE ("Type["<<i+1<<"] :"<< Types[i]);
    MESSAGE ("Entity :"<<MED_EN::entNames[entityType]);
    MESSAGE ("NumberOfValues :"<< NumberOfValues[i]);
    MESSAGE ("NumberOfComponents :"<< numberOfComponents);
    MESSAGE ("MESH_NAME :"<< meshName.c_str());
    MESSAGE ("FIELD_NAME :"<< MED_FIELD_DRIVER<T>::_fieldName.c_str());
    MESSAGE ("MED_ENTITE :"<< (med_2_2::med_entite_maillage) entityType);
    MESSAGE("MED_GEOM :"<<MED_EN::geoNames[Types[i]]);
    MESSAGE("Iteration :"<<MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber());
    MESSAGE("Order :"<<MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber());

    // Le support prend en compte le nombre de valeurs lié aux profils
    MED_FIELD_DRIVER<T>::_ptrField->_numberOfValues+=mySupport->getNumberOfElements(Types[i]); // Ne doit pas prendre en compte les points de Gauss
    med_2_2::med_err ret;

#if defined(IRIX64) || defined(OSF1) || defined(VPP5000)
    int lgth2=NumberOfValues[i]*numberOfComponents;
    if(_ptrField->getValueType()==MED_EN::MED_INT32)
      {
	med_2_2::med_int *temp=new med_2_2::med_int[lgth2];
	ret=med_2_2::MEDchampLire(id,const_cast <char*> (meshName.c_str()),
				  const_cast <char*> (MED_FIELD_DRIVER<T>::_fieldName.c_str()),
				  (unsigned char*) temp,
				  med_2_2::MED_NO_INTERLACE,
				  MED_ALL,
				  profilName,
				  (med_2_2::med_entite_maillage) entityType,
				  (med_2_2::med_geometrie_element)Types[i],
				  MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber(),
				  MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber()
				  );
	for(int i2=0;i2<lgth2;i2++)
	  myValues[i][i2]=(int)(temp[i2]);
	delete [] temp;
      }
    else
#endif
      //VERIFIER LE NBRE
      ret=med_2_2::MEDchampLire(id,const_cast <char*> (meshName.c_str()),
				const_cast <char*> (MED_FIELD_DRIVER<T>::_fieldName.c_str()),
				(unsigned char*) myValues[i],
				med_2_2::MED_NO_INTERLACE,
				MED_ALL,
				LocalGaussName,
				profilName,
				med_2_2::MED_COMPACT,
				(med_2_2::med_entite_maillage)   entityType,
				(med_2_2::med_geometrie_element) Types[i],
				MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber(),
				MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber()
				);
    if (ret < 0)
      {
	// The Field can't be read then we must delete all previously allocated members in FIELD
	for(int j=0; j<=i;j++)
	  delete[] myValues[j];
	delete[] myValues;
	delete[] NumberOfValues ;
	delete[] profilName;
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

    delete[] LocalGaussName ;

    if ( strcmp(profilName,MED_NOPFL) ) {
      anyProfil = true;
      pflSize = med_2_2::MEDnValProfil(id,profilName);
      if ( pflSize  <= 0)
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Error while reading the profil size of |"
				     << profilName << "|" ));

      //profil = new int[pflSize];
      profilSize[i]=pflSize;
      profilList[i].resize(pflSize);
      ret = med_2_2::MEDprofilLire(id,&profilList[i][0],profilName); // cf item 16 Effective STL
      profilNameList[i]=string(profilName);
    }
  }
  delete[] profilName ;


  if (anyProfil) {

    for (int typeNo=0; typeNo < NumberOfTypes; typeNo++) {

      // Trouve l'index du type géométriques dans la liste des types géométriques du maillage
      // correspondant au type géométrique du champ traité
      vector<MED_EN::medGeometryElement>::iterator meshTypeNoIt =
	find(meshGeoType.begin(),meshGeoType.end(),Types[typeNo]); //Gérer l'exception
      int meshTypeNo = meshTypeNoIt -  meshGeoType.begin();

      if (! profilList[typeNo].empty() ) {

// 	for (int j =0 ; j< meshGeoType.size();++j)
// 	  cout << "--MeshTypeNo : "<<meshTypeNo<<"-> meshNbOfElOfTypeC["<<j<<"]="<<meshNbOfElOfTypeC[j]<<endl;
// 	cout << "--typeNo--" << typeNo << endl;
// 	cout << "meshNbOfElOfTypeC["<<meshTypeNo<<"]=" << meshNbOfElOfTypeC[meshTypeNo] <<endl;

	// Transformer les numéros locaux d'entités medfichier en numéro global medmémoire
	for (int i = 0 ; i < profilList[typeNo].size(); i++ ) {
	// Les numéros des entités commencent à 1 dans MEDfichier comme dans MEDmémoire
	// meshNbOfElOfTypeC[0]=0 ...meshNbOfEltOfTypeC[meshTypeNo]=
	// meshNbOfElOfTypeC[meshTypeNo-1]+nbrOfElem of meshTypeNo type
	// rem1 : Si le meshTypeNo trouvé est 0 (premier type géométrique du maillage
	// il ne faut pas décaler les numéros du profils qui commencent à 1 dans MEDFICHIER
	// rem2 : meshNbOfElOfTypeC[NumberOfTypes] ne devrait jamais être utilisé
	  profilList[typeNo][i]+=meshNbOfElOfTypeC[meshTypeNo];
//      cout << "profilList1["<<typeNo<<"]["<<i<<"]="<< profilList[typeNo][i] << endl;
	}
      } else {
	// Créer le profil <MED_ALL> pour ce type géométrique
	// uniquement pour renseigner le tableau skyline avec des accesseurs directs
	// par type géométriques
	// REM : Une conséquence est qu'à la réecriture le fichier contiendra des
	// profils sur certains types géométriques alors qu'à la lecture il n'y en avait pas !
	// Solution : Stocker les noms des profils et les utiliser pour savoir si il y avait ou non
	//            un profil
	int pflSize   = meshNbOfElOfType[meshTypeNo];
	// profil    = new int[pflSize];

	profilList[typeNo].resize(pflSize);
	profilSize[typeNo]=pflSize;

	for (int j = 1; j <= pflSize; j++) {
	  profilList[typeNo][j-1]=meshNbOfElOfTypeC[meshTypeNo] + j ; // index MEDMEM commence à 1
	  cout << "profilList2["<<typeNo<<"]["<<j-1<<"]="<< profilList[typeNo][j-1] << endl;
	}
	profilNameList[typeNo] = MED_NOPFL; //Information a utiliser pour la sauvegarde : PLUTOT MED_ALL
      }
      profilSizeC+=profilList[typeNo].size();
    }

    MEDSKYLINEARRAY * skyLine = new MEDSKYLINEARRAY(profilList.size(), profilSizeC );
    vector<int> index(NumberOfTypes+1,0);
    index[0]=1;
    for( int typeNo=0; typeNo < NumberOfTypes; typeNo++ )
      index[typeNo+1]=index[typeNo]+profilSize[typeNo];
    skyLine->setIndex(&index[0]);
    for (int i=1; i <= profilList.size() ; i++)
      skyLine->setI(i,&profilList[i-1][0]);

    mySupport->setAll(false);
    mySupport->setpartial(skyLine,true);
    mySupport->setProfilNames(profilNameList);

  }

  // allocate _value
  // probleme avec les points de gauss : voir lorsqu-il y en a (!= 1)

  // Créer un driver spécifique pour les modes MED_FULL_INTERLACE et MED_NO_INTERLACE
  // serait plus efficicace.
  ArrayNo * Values = new ArrayNo(numberOfComponents,TotalNumberOfValues);

  for (int i=0; i<numberOfComponents; i++)
    {
      //T * ValuesT = Values->getRow(i+1) ;
      int Count = 1 ;
      for (int j=0; j<NumberOfTypes; j++) {
	T * myValue = myValues[j] ;
	int NumberOf = NumberOfValues[j] ;
	//	  MED_FIELD_DRIVER<T>::_ptrField->_numberOfValues+=NumberOf; // problem with gauss point : _numberOfValues != TotalNumberOfValues !!!!!!!
	int offset = NumberOf*i ;
	for (int k=0 ; k<NumberOf; k++)
	  {
	    //ValuesT[Count]=myValue[k+offset] ;
	    Values->setIJ(Count,i+1,myValue[k+offset]); //Eviter de tout recopier
// 	    SCRUTE(Count);
// 	    SCRUTE(Values->getIJ(Count,i+1));
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

  MED_FIELD_DRIVER<T>::_ptrField->_support=mySupport; //Prévenir l'utilisateur ?


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

  typedef typename MEDMEM_ArrayInterface<T,NoInterlace,NoGauss>::Array ArrayNo;
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,NoGauss>::Array ArrayFull;
  med_2_2::med_idt id = MED_FIELD_DRIVER22<T>::_medIdt;

  if (MED_FIELD_DRIVER<T>::_status!=MED_OPENED)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": Method open must be called before method read.")) ;

  string fieldName;
  if ( ( MED_FIELD_DRIVER<T>::_fieldName.empty()       ) &&
       ( MED_FIELD_DRIVER<T>::_ptrField->_name.empty() )    )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)
				 <<" neither <fieldName> is set in driver nor in object FIELD.")) ;

  // If _fieldName is not set in driver, try to use _ptrfield->_fieldName
  if ( ( MED_FIELD_DRIVER<T>::_fieldName.empty()       ) &&
       ( !MED_FIELD_DRIVER<T>::_ptrField->_name.empty() )    )
    fieldName=MED_FIELD_DRIVER<T>::_ptrField->_name;
  else
    fieldName = MED_FIELD_DRIVER<T>::_fieldName;

  SCRUTE(fieldName);
  if ( fieldName.size() > MED_TAILLE_NOM ) {
    fieldName.substr(0,MED_TAILLE_NOM);
    MESSAGE( "Be careful <fieldName> size must not be > MED_TAILLE_NOM, using fieldName : |"<< fieldName <<"|." );
  }

  const SUPPORT * mySupport = MED_FIELD_DRIVER<T>::_ptrField->getSupport() ;
  if ( ! mySupport )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)
				 <<" There is no SUPPORT associated with FIELD : "
				 << fieldName << "."));

  bool onAll = mySupport->isOnAllElements();

  string meshName = mySupport->getMeshName();
  SCRUTE(meshName);
  if ( meshName.size() > MED_TAILLE_NOM ) {
    meshName = meshName.substr(0,MED_TAILLE_NOM);
    MESSAGE( "Be careful <meshName> size must not be > MED_TAILLE_NOM, using meshName : |"<< meshName <<"|." );
  }
  MED_EN::medEntityMesh entityType = mySupport->getEntity();

  // Reconstruit les listes contigues des noms de composantes et des unités
  // Les noms sont tronqués à MED_TAILLE_PNOM22
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

  MESSAGE("using component_name=|"<<component_name<<"|");
  MESSAGE("using component_unit=|"<<component_unit<<"|");

  MED_EN::med_type_champ ValueType=MED_FIELD_DRIVER<T>::_ptrField->getValueType() ;

  MESSAGE("Template Type =|"<<ValueType<<"|");

  // Vérifier si le champ existe déjà
  char   champName[MED_TAILLE_NOM+1];
  char * compName, * compUnit ;
  med_2_2::med_type_champ type ;
  bool Find = false ;
  int n = med_2_2::MEDnChamp(id,0);
  int nbComp = 0;
  for (int i=1; i<=n; i++) {
    nbComp   = med_2_2::MEDnChamp(id,i);
    compName = new char[MED_TAILLE_PNOM22*nbComp+1];
    compUnit = new char[MED_TAILLE_PNOM22*nbComp+1];
    err = med_2_2::MEDchampInfo(id,i,champName,&type,compName,compUnit,nbComp);
    if (err == 0)
      if (!strcmp(champName,fieldName.c_str()) ) {
	Find = true ;
	break ;
      }
    delete[] compName ;
    delete[] compUnit ;
  }

  if (Find) {
    // the same ?
    if (nbComp != component_count)
      throw MEDEXCEPTION( LOCALIZED (STRING(LOC)
				     <<": Field exist in file, but number of component are different : "
				     <<nbComp<<" in file and "
				     <<component_count<<" in memory."
				     )
			  );
    // component name and unit
    SCRUTE(nbComp);
    MESSAGE(LOC<<" Component name in file   : "<<compName);
    MESSAGE(LOC<<" Component name in memory : "<<component_name);
    MESSAGE(LOC<<" Component unit in file   : "<<compUnit);
    MESSAGE(LOC<<" Component unit in memory : "<<component_unit);
    delete[] compName ;
    delete[] compUnit ;

  } else {
    // Verify the field isn't yet created

    string dataGroupName =  "/CHA/";
    dataGroupName        += fieldName;
    MESSAGE(LOC << "|" << dataGroupName << "|" );
    med_2_2::med_idt gid =  H5Gopen(id, dataGroupName.c_str() );

    if ( gid < 0 ) {
      // create field :
      err=med_2_2::MEDchampCr(id,
			      const_cast <char*> (fieldName.c_str()),
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



  // On s'assure que le champ est dans le bon type d'entrelacement.
  // REM : Il faudrait un driver par type d'entrelacement, ce qui eviterait
  // de doubler l'utilisation de la taille mémoire si le champ n'est pas dans
  // le bon mode.
  FIELD<T,FullInterlace> * myField = 0;
  if ( MED_FIELD_DRIVER<T>::_ptrField->getInterlacingType() == MED_EN::MED_FULL_INTERLACE )
    myField = MED_FIELD_DRIVER<T>::_ptrField;
  else
    myField = FieldConvert( *( dynamic_cast< FIELD<T,NoInterlace> * > (MED_FIELD_DRIVER<T>::_ptrField )
			       )
			    );


  // Il est necessaire de calculer le tableau
  // du nombre d'entités cumulées de chaque type géométrique du maillage
  // pour convertir les profils de la numérotation globale
  // à la numérotation locale au type géométrique.
  // Pour celà on établit ce tableau à partir de l'objet MESH si la relation SUPPORT-MESH existe.
  // Si le maillage existe dans le fichier MED  on essaye également de reconstituer ce tableau
  // pour vérifier la cohérence des informations.
  // Si la relation SUPPRT-MESH n'esiste pas  on constitue le tableau uniquement à partir du fichier qui
  // doit alors obligatoirement contenir le maillage.
  const int * number, *numberIndex = 0;
  string         profilName;
  vector<string> profilNameList;
  vector<MED_EN::medGeometryElement> meshGeoType;
  vector<int> meshNbOfElOfType;
  vector<int> meshNbOfElOfTypeC;
  vector<MED_EN::medGeometryElement> fileMeshGeoType;
  vector<int> fileMeshNbOfElOfType;
  vector<int> fileMeshNbOfElOfTypeC;
  med_2_2::med_int fileHasMesh=0;

  if (!onAll) {

    number = mySupport->getNumber(MED_ALL_ELEMENTS);
    numberIndex = mySupport->getNumberIndex();
    profilNameList=mySupport->getProfilNames();

    fileHasMesh = ( med_2_2::MEDdimLire(id, const_cast<char *>(meshName.c_str())) > 0);
    MESH * meshPtr = mySupport->getMesh();

    if (fileHasMesh)
      getMeshGeometricTypeFromFile(id, meshName,
				   entityType,
				   fileMeshGeoType,fileMeshNbOfElOfType,fileMeshNbOfElOfTypeC);

    if (meshPtr) {

      getMeshGeometricTypeFromMESH( meshPtr, entityType,meshGeoType,
				    meshNbOfElOfType,
				    meshNbOfElOfTypeC);

      if (fileHasMesh)
	if ( ( fileMeshGeoType != meshGeoType ) || (fileMeshNbOfElOfTypeC != meshNbOfElOfTypeC) )
	  throw MEDEXCEPTION(LOCALIZED( STRING(LOC) <<": Error while getting mesh information from file for FIELD "<< fieldName
					<< " on entity " << MED_EN::entNames[entityType]
					<< " with (it,or) = ("
					<< MED_FIELD_DRIVER<T>::_ptrField->_iterationNumber << ","
					<< MED_FIELD_DRIVER<T>::_ptrField->_orderNumber << ")"
					<< " on mesh " << meshName
					<< " : geometric types or number of elements by type differs from MESH object !"
					)
			     );

    }

    if ( !fileHasMesh && meshPtr==0 )
      throw MEDEXCEPTION(LOCALIZED( STRING(LOC) <<": Error while getting mesh information for FIELD "<< fieldName
				    << " on entity " << MED_EN::entNames[entityType]
				    << " with (it,or) = ("
				    << MED_FIELD_DRIVER<T>::_ptrField->_iterationNumber << ","
				    << MED_FIELD_DRIVER<T>::_ptrField->_orderNumber << ")"
				    << " on mesh " << meshName
				    << " : SUPPORT must contain a valid MESH reference or file must contain the associated MESH."
				    )
			 );


    if (fileHasMesh && !meshPtr) {
      meshNbOfElOfTypeC = fileMeshNbOfElOfTypeC;
      meshGeoType       = fileMeshGeoType;
      meshNbOfElOfType  = fileMeshNbOfElOfType;
    }
  }

  int numberOfElForMED = -1;
  const T   * value   = NULL;
  // on boucle sur tout les types pour ecrire les tableaux de valeur
  int numberOfTypes = mySupport->getNumberOfTypes() ;
  int index = 1 ;
  const MED_EN::medGeometryElement * types = mySupport->getTypes() ;
  // A RETABLIR : const int * NumberOfGaussPoint = mySupport->getNumberOfGaussPoint() ;
  for (int typeNo=0;typeNo<numberOfTypes;typeNo++) {

    int numberOfElements = mySupport->getNumberOfElements(types[typeNo]) ;

    //value = MED_FIELD_DRIVER<T>::_ptrField->getRow(index) ;
    // rem 1 : le getRow du Array est différent de celui du FIELD si le SUPPORT contient
    //         des profils (les indices des valeurs ne se suivent pas forcément)
    // rem 2 : Afin de respecter la norme MEDFICHIER, les indices contenus dans les
    //         profils doivent être croissant
    if (onAll) {
      value = myField->getRow(index);
      profilName=MED_NOPFL;
      numberOfElForMED = numberOfElements;
    } else {
      value = myField->getRow(number[index-1]);
      profilName = profilNameList[typeNo].substr(0,MED_TAILLE_NOM);
      // Rem : Si le SUPPORT n'est pas onAll mais que pour un type géométrique donné le nom
      // du profil associé est MED_NOPFL alors le profil n'est pas écrit dans le fichier MED.
      // Car en MEDMEMOIRE si le champ repose sur des éléments de deux types géométriques
      // différents et est défini sur tous les éléments d'un type géométrique
      // mais pas de l'autre, il existe tout de même des profils sur les deux types géométriques.
      // Ce n'est pas le cas en MEDFICHIER.
      vector<int> profil(&number[index-1],&(number[index-1])+numberOfElements);

      // Trouve l'index du type géométrique dans la liste des types géométriques du maillage
      // correspondant au type géométrique du champ en cours de traiement
      vector<MED_EN::medGeometryElement>::iterator meshTypeNoIt =
	find(meshGeoType.begin(),meshGeoType.end(),types[typeNo]); //Gérer l'exception
      int meshTypeNo = meshTypeNoIt -  meshGeoType.begin();

      if ( profilName == MED_NOPFL && profil.size() != meshNbOfElOfType[meshTypeNo] )
	throw MEDEXCEPTION(LOCALIZED( STRING(LOC) <<": Error while creating profil for FIELD "<< fieldName 
				      << " on entity " << MED_EN::entNames[entityType]
				      << " and geometric type " << MED_EN::geoNames[types[typeNo]] << " with (it,or) = ("
				      << MED_FIELD_DRIVER<T>::_ptrField->_iterationNumber << ","
				      << MED_FIELD_DRIVER<T>::_ptrField->_orderNumber << "), with profilName "
				      << profilName << " on mesh " << meshName
				      << " : There is no profileName but profilsize (" <<profil.size()
				      << ") differs from number of elements in associated MESH ("
				      << meshNbOfElOfType[meshTypeNo] << ")."
			    )
			 );

      numberOfElForMED = meshNbOfElOfType[meshTypeNo];

      for (int ind=0;ind < numberOfElements;++ind) {
// 	cout << "number["<<index-1<<"]="<<number[index-1]<<endl;
// 	cout << "profil1["<<ind<<"]="<<profil[ind]<<endl;
	profil[ind]-=meshNbOfElOfTypeC[meshTypeNo];
//  	cout << "profil2["<<ind<<"]="<<profil[ind]<<endl;
      }

      if ( profil[numberOfElements-1] > numberOfElForMED ) //prendre en compte le nombre de points de gauss
	throw MEDEXCEPTION(LOCALIZED( STRING(LOC) <<": Error while creating profil for FIELD "<< fieldName 
				      << " on entity " << MED_EN::entNames[entityType]
				      << " and geometric type " << MED_EN::geoNames[types[typeNo]] << " with (it,or) = ("
				      << MED_FIELD_DRIVER<T>::_ptrField->_iterationNumber << ","
				      << MED_FIELD_DRIVER<T>::_ptrField->_orderNumber << "), with profilName "
				      << profilName << " on mesh " << meshName
				      << " : profil["<<numberOfElements-1<<"]=" << profil[numberOfElements-1]
				      << " must not be superior to field size without profil : "
				      << numberOfElForMED
			    )
			 );

      if ( med_2_2::MEDprofilEcr(id,
				 &profil[0],
				 numberOfElements,
				 const_cast<char *>(profilName.c_str())) < 0)

	throw MEDEXCEPTION(LOCALIZED( STRING(LOC) <<": Error while writing "<< numberOfElements
				      << " values for MED profil "<< profilName
				    )
			 );
    }

    MESSAGE("MED_FIELD_DRIVER22<T>::_medIdt                       : "<<id);
    MESSAGE("meshName.c_str()                : "<<meshName.c_str());
    MESSAGE("MED_FIELD_DRIVER<T>::_ptrField->getName()            : "<<MED_FIELD_DRIVER<T>::_ptrField->getName());
    MESSAGE("MED_FIELD_DRIVER<T>::_fieldName                      : "<<MED_FIELD_DRIVER<T>::_fieldName);
    MESSAGE("value                           : "<<value);
    MESSAGE("numberOfElements                : "<<numberOfElements);
    MESSAGE("numberOfElForMED                : "<<numberOfElForMED);
    //    MESSAGE("NumberOfGaussPoint[i]     : "<<NumberOfGaussPoint[typeNo]);
    MESSAGE("entityType                      : "<<MED_EN::entNames[entityType]);
    MESSAGE("types[i]                        : "<<MED_EN::geoNames[types[typeNo]]);
    MESSAGE("MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber() : "<<MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber());
    MESSAGE("MED_FIELD_DRIVER<T>::_ptrField->getTime()            : "<<MED_FIELD_DRIVER<T>::_ptrField->getTime());
    MESSAGE("MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber()     : "<<MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber());


    // Rem 1 : le nombre d'éléments passé à MEDchampEcr ne doit pas tenir compte de la taille
    //         des profils : c'est la taille du champ sans profil.
    err=med_2_2::MEDchampEcr(id,
			     const_cast <char*> ( meshName.c_str()) ,
			     const_cast <char*> ( fieldName.c_str()),
			     (unsigned char*)value, med_2_2::MED_FULL_INTERLACE,
			     numberOfElForMED, //numberOfElements,//*NumberOfGaussPoint[typeNo],
			     MED_NOGAUSS, MED_ALL,
			     const_cast <char *> (profilName.c_str()), med_2_2::MED_COMPACT,
			     (med_2_2::med_entite_maillage)entityType,
			     (med_2_2::med_geometrie_element)types[typeNo],
			     MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber(),
			     "        ",                     // A FAIRE : IMPLEMENTER L'UNITE DU PAS DE TEMPS!
			     MED_FIELD_DRIVER<T>::_ptrField->getTime(),
			     MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber()
			     );

    if (err < MED_VALID ) {
      if ( MED_FIELD_DRIVER<T>::_ptrField->getInterlacingType() == MED_EN::MED_NO_INTERLACE ) delete myField;
      throw MEDEXCEPTION(LOCALIZED( STRING(LOC) <<": Error while writing "<< numberOfElements << " values for FIELD "<< fieldName 
				   << " on entity " << MED_EN::entNames[entityType]
				    << " and geometric type " << MED_EN::geoNames[types[typeNo]]
				   << " with (it,or) = ("
				   << MED_FIELD_DRIVER<T>::_ptrField->_iterationNumber << ","
				   << MED_FIELD_DRIVER<T>::_ptrField->_orderNumber << "), with profilName "
				    << profilName << " on mesh " << meshName
				    )
			 );
    }

    index += numberOfElements ;

  }
  if ( MED_FIELD_DRIVER<T>::_ptrField->getInterlacingType() == MED_EN::MED_NO_INTERLACE ) delete myField;


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

} //End namespace MEDMEM
/*-----------------------------------------------------------------*/

#endif /* MED_FIELD_DRIVER_HXX */
