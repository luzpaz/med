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
#include "MEDMEM_GaussLocalization.hxx"

//includes temporaires (attente release med fichier 2.3.1)
#include "MEDMEM_MEDMEMgaussEcr.hxx"
#include "MEDMEM_MEDMEMprofilEcr.hxx"
#include "MEDMEM_MEDMEMchampLire.hxx"

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
			  const string & fieldName,
			  med_2_2::med_int ndt,
			  med_2_2::med_int od,
			  SUPPORT & support,
			  string & meshName,
			  vector<int> & numberOfElementsOfTypeC,
			  vector<int> & numberOfGaussPoint,
			  int & totalNumberOfElWg
			  ) const throw (MEDEXCEPTION);

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

  Cette m�thode cr�e le SUPPORT du champ <fieldName> pour le
	<n�de pas de temps,n�d'it�ration>=<ndt,od>.

  Le SUPPORT cr�e � pour nom  <fieldName>Support et contient
  la liste des types g�om�triques sur le premier type
  d'entit� trouv� (en MEDMEM on inderdit aux champs de reposer
  sur plusieurs types d'entit�).
  Il contient �galement le nombre d'entit�s trouv�es pour chaque
  type g�om�trique.
  Par d�faut l'attribut onAll du SUPPORT est positionn� � true car
  cette routine ne lit rien de ce qui concerne les entit�s
  du maillage associ�.
  La m�thode renvoie true si elle r�ussit � cr�er le SUPPORT
  demand�.
  Le nom du maillage associ� ( en MEDMEM on ne
  supporte pas encore les maillages multiples ) est renvoy� dans <meshName>.
  Deux tableaux directements exploitables par MEDMEMnArray sont renvoy�s :
  - numberOfElementsOfTypeC : nombres d'entit�s cumul�s de chaque type g�om�trique
       avec numberOfElementsOfTypeC[0]=1 et de taille nombre de types+1
  - numberOfGaussPoint : nombre de points de Gauss par type g�om�trique
       avec numberOfGaussPoint[0]=1 et de taille  nombre de types+1
*/

template <class T> bool
MED_FIELD_DRIVER22<T>::createFieldSupportPart1(med_2_2::med_idt id,
					       const string & fieldName,
					       med_2_2::med_int ndt,
					       med_2_2::med_int od,
					       SUPPORT & support,
					       string & meshName,
					       vector<int> & numberOfElementsOfTypeC,
					       vector<int> & numberOfGaussPoint,
					       int & totalNumberOfElWg
					       ) const throw (MEDEXCEPTION)
{

  //EF : G�rer le meshName pour le driver 2.2
  const char * LOC="MED_FIELD_DRIVER<T>::createFieldSupportPart1(...)";

  BEGIN_OF(LOC);

  map<int, list<MED_EN::medGeometryElement> > CellAndNodeEntities;
  map<int, list<MED_EN::medGeometryElement> >::iterator currentEntity;
  CellAndNodeEntities[MED_EN::MED_CELL]  = MED_EN::meshEntities[MED_EN::MED_CELL];
  CellAndNodeEntities[MED_EN::MED_NODE] = MED_EN::meshEntities[MED_EN::MED_NODE];
  list< MED_EN::medGeometryElement >::const_iterator currentGeometry;

  MED_EN::medEntityMesh entityCurrent;
  MED_EN::medGeometryElement geometryCurrent;

  MED_EN::medEntityMesh entity;
  bool alreadyFoundAnEntity=false,alreadyFoundPdtIt = false;
  int  numberOfElements = 0;
  int  numberOfGeometricType = 0;
  MED_EN::medGeometryElement geometricType[MED_NBR_GEOMETRIE_MAILLE];
  int numberOfElementsOfType[MED_NBR_GEOMETRIE_MAILLE];
  numberOfElementsOfTypeC.clear();numberOfGaussPoint.clear();
  numberOfElementsOfTypeC.resize(MED_NBR_GEOMETRIE_MAILLE+1);
  numberOfGaussPoint.resize(MED_NBR_GEOMETRIE_MAILLE+1);

  med_2_2::med_int nmaa=0, ngauss=0, numdt=-1, numo=-1, nbPdtIt=0, nbPdtIt1=0, nbPdtIt2=0;
  char dtunit[MED_TAILLE_PNOM22+1];
  char maa[MED_TAILLE_NOM+1];
  med_2_2::med_float   dt=-1.0;
  med_2_2::med_booleen local;
  med_2_2::med_err     ret=1;
  numberOfElementsOfTypeC[0] = 1;
  numberOfGaussPoint[0] = 1;
  totalNumberOfElWg = 0;

  /* D�termine le type d'entit� et la liste des types g�om�triques associ�s
     au champ <fieldName> */
  for (currentEntity = CellAndNodeEntities.begin();
       currentEntity != CellAndNodeEntities.end(); currentEntity++) {
    for (currentGeometry  = (*currentEntity).second.begin();
	 currentGeometry != (*currentEntity).second.end(); currentGeometry++) {

      entityCurrent = (*currentEntity).first ;
      geometryCurrent = (*currentGeometry) ;

      // That is a difference between Med File and Med Memory (NB)
      if (geometryCurrent == MED_EN::MED_SEG2 || geometryCurrent == MED_EN::MED_SEG3)
	entityCurrent = MED_EN::MED_EDGE;

      if (geometryCurrent == MED_EN::MED_TRIA3 || geometryCurrent == MED_EN::MED_QUAD4 ||
	  geometryCurrent == MED_EN::MED_TRIA6 || geometryCurrent == MED_EN::MED_QUAD8)
	entityCurrent = MED_EN::MED_FACE;

      nbPdtIt1 = med_2_2::MEDnPasdetemps(id, const_cast <char*> ( fieldName.c_str() ),
					 (med_2_2::med_entite_maillage)   (*currentEntity).first,
					 (med_2_2::med_geometrie_element)  *currentGeometry );

      nbPdtIt2 = med_2_2::MEDnPasdetemps(id, const_cast <char*> ( fieldName.c_str() ),
					 (med_2_2::med_entite_maillage)   entityCurrent,
					 (med_2_2::med_geometrie_element)  geometryCurrent );

      if (nbPdtIt2 < nbPdtIt1) entityCurrent = (*currentEntity).first ;

      nbPdtIt = (nbPdtIt1>nbPdtIt2)?nbPdtIt1:nbPdtIt2;

      if ( nbPdtIt <=  0 )
	continue;

      /* Verifie que le champ n'est pas d�fini sur un autre type d'entit� */
      if ( alreadyFoundAnEntity )
	{
	  //if (entity != (*currentEntity).first )  (NB)
	  if ( entity != entityCurrent )
	    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Field |"  << fieldName
					 << "| with (ndt,or) = (" << ndt << ","
					 << od << ") must not be defined on nodes and cells" ));

	}
      else
	{ 
	  //entity=(*currentEntity).first; (NB)
	  entity=entityCurrent;
	  alreadyFoundAnEntity = true;
	};


      /* Cherche le champ pour le <ndt>,<ot> demand� et d�termine le nombre de points de Gauss*/
      ret = 0; alreadyFoundPdtIt = false; ngauss =0;
      for ( med_2_2::med_int j=1; j <= nbPdtIt; j++ ) {

	// Search how many <ngauss> (<fieldName>,<ndt>,<ot>) has   (NB)
	//ret += med_2_2::MEDpasdetempsInfo(id, const_cast <char*> ( fieldName.c_str() ),
	//				 (med_2_2::med_entite_maillage)   (*currentEntity).first,
	//				 (med_2_2::med_geometrie_element)  *currentGeometry,
	//				 j, &ngauss,  &numdt,  &numo, dtunit, &dt,
	//				  maa, &local, &nmaa);

	ret += med_2_2::MEDpasdetempsInfo(id, const_cast <char*> ( fieldName.c_str() ),
					  (med_2_2::med_entite_maillage)   entityCurrent,
					  (med_2_2::med_geometrie_element)  *currentGeometry,
					  j, &ngauss,  &numdt,  &numo, dtunit, &dt,
					  maa, &local, &nmaa);

	if ( ndt == numdt && numo == od ) {
	  alreadyFoundPdtIt = true;

	  if ( nmaa > 1 ) {
	    MESSAGE(LOC<<" Field |" << fieldName << "| with (ndt,or) = ("
		    << ndt << "," << od << ") for (entityType,geometricType)=("
		    << MED_EN::entNames[entityCurrent] << ","
		    << MED_EN::geoNames[*currentGeometry] << ")"
		    << "is defined on multiple meshes, using dafault mesh  |" << maa << "|" );
	  }

	  if ( !local) {
	    MESSAGE(" Field |" << fieldName << "| with (ndt,or) = ("
		    << ndt << "," << od << ") for (entityType,geometricType)=("
		    << MED_EN::entNames[entityCurrent] << ","
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
					   << MED_EN::entNames[entityCurrent] << ","
					   << MED_EN::geoNames[*currentGeometry] << ")"
					   << "is defined on mesh |" << maa << "| not on mesh |" << meshName ));
	    }
	  break;
	}

      }

      MESSAGE(LOC << " a (dt,it) is found ?? " << alreadyFoundPdtIt);

      if ( !alreadyFoundPdtIt )
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Field |" << fieldName << "| with (ndt,or) = ("
				     << ndt << "," << od << ") should be defined for (entityType,geometricType)=("
				     << MED_EN::entNames[entityCurrent] << ","
				     << MED_EN::geoNames[*currentGeometry] << ")" ));

      if ( (ret != 0)  || (ngauss < 1 ) )
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Error in MEDpasdetempsInfo for  Field |" << fieldName 
				     << "| with (ndt,or) = ("
				     << ndt << "," << od << ") for (entityType,geometricType)=("
				     << MED_EN::entNames[entityCurrent] << ","
				     << MED_EN::geoNames[*currentGeometry] << ")" )); ;

      if ( (numberOfElements =  med_2_2::MEDnVal(id, const_cast <char*> ( fieldName.c_str() ),
						(med_2_2::med_entite_maillage)   entityCurrent,
						(med_2_2::med_geometrie_element) *currentGeometry,
						 numdt, numo, maa, med_2_2::MED_COMPACT))  <=  0 )
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Error in MEDnVal for  Field |" << fieldName
				     << "| with (ndt,or) = ("
				     << ndt << "," << od << ") for (entityType,geometricType)=("
				     << MED_EN::entNames[entityCurrent] << ","
				     << MED_EN::geoNames[*currentGeometry] << ")" )); ;

      numberOfElementsOfType[numberOfGeometricType] = numberOfElements/ngauss;
      numberOfElementsOfTypeC[numberOfGeometricType+1]=
	numberOfElementsOfTypeC[numberOfGeometricType]
	+  numberOfElementsOfType[numberOfGeometricType];
      numberOfGaussPoint[numberOfGeometricType+1] = ngauss;
      geometricType[numberOfGeometricType]= *currentGeometry;
      numberOfGeometricType++;
      totalNumberOfElWg+=numberOfElements;

    } // End Second For

  } // End Premier For

  if ( alreadyFoundAnEntity) {
    support.setName(fieldName+"Support");
    support.setMeshName(string(maa)); // V�rifier que les diff�rents noms de maillages lus soient identiques
    support.setEntity(entity);
    // REM : Le nombre <numberOfGeometricType> dans la pr�c�dente version du Driver 
    //       �tait erron�e pour un champ qui ne reposait pas sur toutes les entit�s g�om�triques 
    //       du maillage mais dont le SUPPORT a �t� cr�e � partir des informations du maillage
    //       ( m�thode qui �tait largement utilis�e pour construire un SUPPORT).
    support.setNumberOfGeometricType(numberOfGeometricType);
    support.setGeometricType(geometricType); // Utile uniquement si setAll == false ?
    support.setNumberOfElements(numberOfElementsOfType);    //setNumberOfElements effectue une copie 
    // Par d�faut consid�re que le champ repose sur tous les type g�om�triques du maillage
    // Si ce n'est pas le cas les champs geometricType et numberOfElementsOfType du SUPPORT sont corrects
    support.setAll(true);
    numberOfElementsOfTypeC.resize(numberOfGeometricType+1);
    numberOfGaussPoint.resize(numberOfGeometricType+1);

    return alreadyFoundAnEntity;
  } else
    return false;
}

/*!

  Renvoie la liste <geoType> des types g�om�triques d�finis dans le maillage <meshName>
  pour le type d'entit� <entity>.
  * < nbOfElOfType > contient le nombre d'entit�s de chaque type
  * < numberOfElementsOfTypeC > contient le nombre d'entit�s cumul�es de chaque type
                              avec numberOfElementsOfTypeC[0]=0;

*/
template <class T> void
MED_FIELD_DRIVER22<T>::getMeshGeometricTypeFromFile(med_2_2::med_idt id,
					  string & meshName,
					  MED_EN::medEntityMesh  entity,
					  vector<MED_EN::medGeometryElement> & geoType,
					  vector<int> &nbOfElOfType,
					  vector<int> &nbOfElOfTypeC
					 ) const throw(MEDEXCEPTION)
{
  const char LOC[] = "MED_FIELD_DRIVER<T>::getMeshGeometricTypeFromFile(...)";

  int numberOfGeometricType=0;
  MED_EN::medGeometryElement geometricType[MED_NBR_GEOMETRIE_MAILLE];
  int numberOfElementsOfType [MED_NBR_GEOMETRIE_MAILLE];
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

//   for (int j =0 ; j<= numberOfGeometricType;++j)
//       cout << "nbOfElOfTypeC["<<j<<"]="<<nbOfElOfTypeC[j]<<endl;

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

  // Il est plus pratique de cr�er un support "onAll"
  // pour calculer les tableaux du nombre d'entit�s cumul�es

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
  typedef typename MEDMEM_ArrayInterface<T,NoInterlace,Gauss>::Array     ArrayNoWg;
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,NoGauss>::Array ArrayFull;
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,Gauss>::Array   ArrayFullWg;

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
    {
      SCRUTE(MED_FIELD_DRIVER<T>::_fieldName.size());
      SCRUTE(MED_TAILLE_NOM);

//       throw MEDEXCEPTION(LOCALIZED(STRING(LOC)
// 				   <<" <fieldName> size in object driver FIELD is > MED_TAILLE_NOM ."));

      MESSAGE(LOC << "Warning <fieldName> size in object driver FIELD is > MED_TAILLE_NOM .");
    }

  const string & fieldName = MED_FIELD_DRIVER<T>::_fieldName;

  MED_EN::medModeSwitch interlacingType = MED_FIELD_DRIVER<T>::_ptrField->getInterlacingType();
  bool isFullInterlace = ( interlacingType == MED_EN::MED_FULL_INTERLACE );

  MESSAGE("###### "<<LOC<<" fieldNameDRIVER : "<< fieldName << " fieldName : "<< MED_FIELD_DRIVER<T>::_ptrField->_name);

// EF :
//   Si un support a �t� donn�e au champ, pour des raisons de compatibilit� avec
//   les versions pr�c�dentes, ce support sera utilis� pour
//   - Obtenir le nom du maillage sur lequel on veut lire le champ
//     (eventuellement on pourrait l'utiliser pour selectionner un champ qui
//      repose sur plusieurs maillages cf HOMARD-ASTER, ce qui n'est pas g�r� dans MEDMEM)
//   -  v�rifier le type d'entit� (MED_NOEUD xor MED_MAILLE xor MED_FACE xor MED_ARETE ) sur lequel
//      il faut lire le champ qui est �galement retouv�.
//   - Si le support d�fini une liste d'entit� ( diff�rente de MED_ALL_ELEMENTS), celle-ci est ignor�e
//     � la lecture et �cras� par soit :
//            - onall, apr�s avoir v�rifi� que la liste des types g�om�triques utilis�s par le champ
//		       est �gale � la liste des type g�om�triques d�finis dans le maillage associ�
//                     pour tous le m�me type d'entit�.
//            - La sous liste des types g�om�triques utilis�s (onAll quand m�me, cf commenataire ci-dessous )  
//            - les listes de profils lus s'il en existe pour une sous liste de types
//              g�om�triques

//   Si aucun support n'a �t� donn� au champ :
//   - A la lecture : Un support est cr�e et le type d'entit� unique est lu
//                    (cf decision gt MED qu'un champ repose sur une entit� unique ?),
//                    l'ensemble des types g�om�triques est lu,
//                    l'ensemble des profils par type g�om�trique est lu
//                    Le nom du maillage associ� est lu mais le pointeur SUPPORT-MESH non initialis�


  char tmpFieldName[MED_TAILLE_NOM+1] ;
  int err ;
  int    numberOfComponents          = 0;
  char * componentName               = (char *) MED_NULL;
  char * unitName                    = (char *) MED_NULL;
  med_2_2::med_type_champ type ;
  med_2_2::med_idt id = MED_FIELD_DRIVER22<T>::_medIdt;
  bool needConversionToDouble = false,needConversionToInt64 = false;

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
	    MESSAGE(LOC<<"Be careful there is no compound for field n°"<<i<<"in file |"<<MED_FIELD_DRIVER<T>::_fileName<<"| !");

	  componentName = new char[numberOfComponents*MED_TAILLE_PNOM22+1] ;
	  unitName      = new char[numberOfComponents*MED_TAILLE_PNOM22+1] ;

	  err = med_2_2::MEDchampInfo(id, i, tmpFieldName, &type, componentName,
				      unitName, numberOfComponents) ;

	  MESSAGE("Field "<<i<<" : #" << tmpFieldName <<"# et recherche #"<<fieldName.c_str()<<"#");
	  if ( !strcmp(tmpFieldName,fieldName.c_str()) ) {
	    MESSAGE("FOUND FIELD "<< tmpFieldName <<" : "<<i);
	    MED_FIELD_DRIVER<T>::_fieldNum = i ;
	    break ;
	  }
	  // not found : release memory and search next field !
	  delete[] componentName ;
	  delete[] unitName ;
	}
    }

  //delete[] tmpFieldName ;

  // Si aucun champ ne correspond les variables <componentName> et <unitName> ont �t� correctement
  // d�sallou�es dans la boucle de recherche
  if (MED_FIELD_DRIVER<T>::_fieldNum==MED_INVALID)
    throw MEDEXCEPTION(LOCALIZED( STRING(LOC) << ": Field "<<  fieldName
				   << " not found in file " << MED_FIELD_DRIVER<T>::_fileName) );

  MESSAGE ("FieldNum : "<<MED_FIELD_DRIVER<T>::_fieldNum);

  if (numberOfComponents < 1) {
    delete[] componentName; delete[] unitName;
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" no component found for field "
				 << fieldName)) ;
  }

  // Verifie que l'on essaye pas de lire un champ double dans un FIELD<int>
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
#if defined(IRIX64) || defined(OSF1) ||defined(VPP5000)
    if (_ptrField->_valueType==MED_EN::MED_INT32 )
      needConversionToInt64=true;
#endif
    break;
  case med_2_2::MED_FLOAT64 :
    if (type != med_2_2::MED_FLOAT64)
      needConversionToDouble=true;
    break;
  default:
    break;
  }

  string meshName="";
  MESH * ptrMesh = 0;
  bool   haveSupport = false;
  bool   haveMesh    = false;
  if ( MED_FIELD_DRIVER<T>::_ptrField->getSupport() ) {
    // Verif � faire sur la taille du meshName
    ptrMesh = MED_FIELD_DRIVER<T>::_ptrField->getSupport()->getMesh();
    if ( ptrMesh) {
      meshName =  MED_FIELD_DRIVER<T>::_ptrField->getSupport()->getMesh()->getName() ;
      haveMesh = true;
    }
    haveSupport = true;
  }

  // Cherche le type d'entit�, le nombre d'entit�  par type g�om�trique sur le type d'entit�
  // (MED_MAILLE ou MED_NOEUD uniquement car MEDMEMOIRE ne g�re pas la connectivit� descendante).
  // et cr�e le support correspondant.
  SUPPORT *   mySupport = new SUPPORT();
  vector<int> numberOfElementsOfTypeC;
  vector<int> numberOfGaussPoint;
  int         totalNumberOfElWg=0;

  bool found = createFieldSupportPart1(id,fieldName,
				       MED_FIELD_DRIVER<T>::_ptrField->_iterationNumber,
				       MED_FIELD_DRIVER<T>::_ptrField->_orderNumber,
				       *mySupport, meshName,
				       numberOfElementsOfTypeC, numberOfGaussPoint,totalNumberOfElWg);

  if ( !found ) {
    delete mySupport; delete[] componentName; delete[] unitName;
    MED_FIELD_DRIVER<T>::_fieldNum = MED_INVALID ;
     throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"  Can't find any entity for field |"
				 << fieldName
				 << "| with (it,or) = ("
				  << MED_FIELD_DRIVER<T>::_ptrField->_iterationNumber << ","
				 << MED_FIELD_DRIVER<T>::_ptrField->_orderNumber << "), on mesh "
				 << meshName << "|" ));
  }


  MED_EN::medEntityMesh entityType = mySupport->getEntity();
  //Si un SUPPORT �tait donn�, r�cup�re son nom, sa description et
  //	 le pointeur du maillage associ�
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
				   << fieldName
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
    this->getMeshGeometricTypeFromMESH(ptrMesh,entityType,MESHgeoType,
                                       MESHnbOfElOfType,MESHnbOfElOfTypeC);

  int fileHasMesh = ( med_2_2::MEDdimLire(id, const_cast<char *>(meshName.c_str())) > 0);
  vector< MED_EN::medGeometryElement >  meshGeoType;
  vector< int >  meshNbOfElOfType;
  vector< int >  meshNbOfElOfTypeC;
  // Si le maillage n'est pas trouv� les tableaux renvoy�s sont vides
  if (fileHasMesh)
    {
      MED_EN::medEntityMesh entityTypeLoc = entityType;
      if (entityType == MED_EN::MED_FACE || entityType == MED_EN::MED_EDGE) entityTypeLoc = MED_EN::MED_CELL;

      this->getMeshGeometricTypeFromFile(id,meshName,entityTypeLoc,meshGeoType,
					 meshNbOfElOfType,meshNbOfElOfTypeC);
    }

  SCRUTE(meshGeoType.size());
  SCRUTE(MESHgeoType.size());
  SCRUTE(meshNbOfElOfTypeC.size());
  SCRUTE(MESHnbOfElOfTypeC.size());

  if (meshGeoType.size() != MESHgeoType.size())
    {
      for (int i = 0; i<meshGeoType.size();i++)
	MESSAGE("debug meshGeotype " << meshGeoType[i]);

      for (int i = 0; i<MESHgeoType.size();i++)
	MESSAGE("debug MESHgeoType. " << MESHgeoType[i]);
    }

  if (meshNbOfElOfTypeC.size() == MESHnbOfElOfTypeC.size())
    {
      for (int i = 0; i<meshNbOfElOfTypeC.size();i++)
	MESSAGE("debug meshNbOfElOfTypeC " << meshNbOfElOfTypeC[i]);

      for (int i = 0; i<MESHnbOfElOfTypeC.size();i++)
	MESSAGE("debug MESHnbOfElOfTypeC " << MESHnbOfElOfTypeC[i]);
    }

  if (fileHasMesh && haveSupport )
    if ( ( meshGeoType != MESHgeoType ) || (meshNbOfElOfTypeC != MESHnbOfElOfTypeC) )
      {
	MESSAGE("Warning MedField driver 21 while getting mesh information from file for FIELD "<< fieldName
		<< " on entity " << MED_EN::entNames[entityType]
		<< " with (it,or) = ("
		<< MED_FIELD_DRIVER<T>::_ptrField->_iterationNumber << ","
		<< MED_FIELD_DRIVER<T>::_ptrField->_orderNumber << ")"
		<< " on mesh " << meshName
		<< " : geometric types or number of elements by type differs from MESH object !");

// 	throw MEDEXCEPTION(LOCALIZED( STRING(LOC) <<": Error while getting mesh information from file for FIELD "<< fieldName
// 				      << " on entity " << MED_EN::entNames[entityType]
// 				      << " with (it,or) = ("
// 				      << MED_FIELD_DRIVER<T>::_ptrField->_iterationNumber << ","
// 				      << MED_FIELD_DRIVER<T>::_ptrField->_orderNumber << ")"
// 				      << " on mesh " << meshName
// 				      << " : geometric types or number of elements by type differs from MESH object !"
// 				      )
// 			   );
      }

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


  // Test si le Support du Champ repose ou non sur toutes les entit�s g�om�triques
  // du maillage associ� et positionne ou non l'attribut onAll du SUPPORT.
  // Il ne s'agit pas de la gestion des profils
  vector < MED_EN::medGeometryElement > v1(  mySupport->getTypes(),
					     mySupport->getTypes()+mySupport->getNumberOfTypes() );
  vector<int> v2(numberOfElementsOfTypeC.size());
  transform(numberOfElementsOfTypeC.begin(),
	    numberOfElementsOfTypeC.end(),v2.begin(), bind2nd(plus<int>(),1));

  if ( ( meshGeoType != v1 )  || meshNbOfElOfTypeC != v2 ) {
    // ATTENTION : mySupport->setAll(false);
    // Pb : On a envie de positionner onAll � faux si le champ n'est pas d�fini sur tous les
    //      types g�om�triques du maillage associ�.
    //      Mais si onAll est false et si aucun profil n'est d�tect� par la suite,
    //      l'attribut SUPPORT->_number est cens� �tre positionn� quand m�me ! Que faire ?
    // Si on veut �tre compatible avec la signification premi�re de onAll,
    //  il faudrait cr�er des profils contenant toutes les entit�s pour chaque type g�om�trique
    //  du SUPPORT  mais d'une part c'est dommage d'un point de vue de l'emcombrement m�moire
    //  et d'autre part, � la r��criture du fichier MED on stockera des profils 
    //  alors qu'il n'y en avait pas � l'origine (fichier MED diff�rent apr�s lecture/�criture) !
    // Si on laisse setAll � vrai il faut �tre s�r que les utilisateurs prennent les
    //  informations sur les types g�m�trique au niveau du support et non pas du maillage.
    // Solution : Signification du onAll -> onAllElements des type g�om�triques d�finis
    // dans le SUPPORT et non du maillage associ� (dans la plupart des cas si le fichier ne
    // contient pas de profil, le champ est d�fini sur toutes les entit�s de tous les types
    // g�om�triques d�finis dans le maillage).
  }


  // If an error occurs while reading the field, these allocated FIELD member will be deleted

  MED_FIELD_DRIVER<T>::_ptrField->_name                   = fieldName;
  MED_FIELD_DRIVER<T>::_ptrField->_numberOfComponents     = numberOfComponents ;
  MED_FIELD_DRIVER<T>::_ptrField->_componentsTypes        = new int   [numberOfComponents] ;
  MED_FIELD_DRIVER<T>::_ptrField->_componentsNames        = new string[numberOfComponents] ;
  MED_FIELD_DRIVER<T>::_ptrField->_componentsUnits        = new UNIT  [numberOfComponents] ;
  MED_FIELD_DRIVER<T>::_ptrField->_componentsDescriptions = new string[numberOfComponents] ;
  MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits     = new string[numberOfComponents] ;
  for (int i=0; i<numberOfComponents; i++) {
      MED_FIELD_DRIVER<T>::_ptrField->_componentsTypes[i] = 1 ;
      MED_FIELD_DRIVER<T>::_ptrField->_componentsNames[i] = string(componentName,i*MED_TAILLE_PNOM22,MED_TAILLE_PNOM22) ;
      SCRUTE(MED_FIELD_DRIVER<T>::_ptrField->_componentsNames[i]);
      MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits[i] = string(unitName,i*MED_TAILLE_PNOM22,MED_TAILLE_PNOM22) ;
      SCRUTE(MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits[i]);
  }
  delete[] componentName;
  delete[] unitName;

  int NumberOfTypes                       = mySupport->getNumberOfTypes() ;
  const MED_EN::medGeometryElement *types = mySupport->getTypes() ;
  T * myValues = new T[totalNumberOfElWg*numberOfComponents];
  const int * nbOfElOfType = mySupport->getNumberOfElements() ;
  bool anyProfil = false;
  int  pflSize=0,index=0;
  // Le vecteur de profil est dimensionn� par rapport aux nombres de types
  // g�om�triques du champ m�me si le champ n'a pas de profil MED FICHIER sur
  // tous ses types g�om�triques car dans MEDMEM si onAllElement 
  // du SUPPORT est false il faut positionner un profil pour tous les types g�om�triques 
  // du SUPPORT
  int profilSizeC = 0;
  vector < int   >        profilSize    (NumberOfTypes,0);
  vector < vector<int>  > profilList    (NumberOfTypes);
  vector < string >       profilNameList(NumberOfTypes);
  char * profilName = new char[MED_TAILLE_NOM+1];

  MESSAGE ("NumberOfTypes      : "<< NumberOfTypes);
  MED_FIELD_DRIVER<T>::_ptrField->_numberOfValues=0 ;


  for (int typeNo=0; typeNo<NumberOfTypes; typeNo++) {

    int numberOfValuesWc= nbOfElOfType[typeNo]*numberOfGaussPoint[typeNo+1]*numberOfComponents;
    char * gaussModelName = new char[MED_TAILLE_NOM+1];

    MESSAGE ("FIELD_NAME         : "<< fieldName.c_str());
    MESSAGE ("MESH_NAME          : "<< meshName.c_str());
    MESSAGE ("MED_ENTITE         : "<< MED_EN::entNames[entityType]);
    MESSAGE ("MED_GEOM           : "<< MED_EN::geoNames[types[typeNo]]);
    MESSAGE ("Iteration          : "<< MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber());
    MESSAGE ("Order              : "<< MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber());
    MESSAGE ("NumberOfElements   : "<< nbOfElOfType[typeNo]);
    MESSAGE ("NumberOfComponents : "<< numberOfComponents);
    MESSAGE ("NumberOfGaussPts   : "<< numberOfGaussPoint[typeNo+1]);
    MESSAGE ("NumberOfValuesWg   : "<< nbOfElOfType[typeNo]*numberOfGaussPoint[typeNo+1]);
    MESSAGE ("NumberOfValuesWgWc : "<< numberOfValuesWc);
    MESSAGE ("Index              : "<< index);
    med_2_2::med_err ret=-1;

    med_2_2::med_int * myValuesTmp=0;
    unsigned char* ptrTmp=0;
    if (needConversionToDouble || needConversionToInt64 ) {
      myValuesTmp = new med_2_2::med_int[numberOfValuesWc];
      ptrTmp = (unsigned char*) myValuesTmp;
    } else
      ptrTmp = (unsigned char*) &myValues[index];

    //VERIFIER LE NBRE
    ret=med_2_2::MEDMEMchampLire(id,const_cast <char*> (meshName.c_str() ),
			         const_cast <char*> (fieldName.c_str()),
				(unsigned char*) ptrTmp,
				med_2_2::MED_FULL_INTERLACE,
				MED_ALL,
				gaussModelName,
				profilName,
				med_2_2::MED_COMPACT,
				(med_2_2::med_entite_maillage) entityType,
				(med_2_2::med_geometrie_element)types[typeNo],
				MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber(),
				MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber()
				);

      if (needConversionToDouble || needConversionToInt64 ) {

      if (needConversionToInt64 )  //utiliser un trait
	for(int i=0;i<numberOfValuesWc;++i)
	  myValues[index+i]=(int)(myValuesTmp[i]);
      else
	for(int i=0;i<numberOfValuesWc;++i)
	  myValues[index+i]=myValuesTmp[i];
      delete[] myValuesTmp;
    }

    if (ret < 0)
      {
	// The Field can't be read then we must delete all previously allocated members in FIELD
	//for(int j=0; j<=i;j++)
	//  delete[] myValues[j];
	delete[] myValues;
	//delete[] NumberOfValues ;
	delete[] profilName;
	delete[] gaussModelName;
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
	throw MEDEXCEPTION( LOCALIZED( STRING(LOC) <<": ERROR while reading values")) ;
      }
    index += numberOfValuesWc;
    // Le support prend en compte le nombre de valeurs li� aux profils
    MED_FIELD_DRIVER<T>::_ptrField->_numberOfValues+=
      nbOfElOfType[typeNo];// Ne doit pas prendre en compte les points de Gauss

    // second et troisi�me test li� � un bug medfichier
    if ( strcmp(gaussModelName,MED_NOGAUSS) && strcmp(gaussModelName,string(MED_TAILLE_NOM,' ').c_str() )
	 && strcmp(gaussModelName,string(16,' ').c_str() )  ) {
 
	int type_geo = (int) types[typeNo];
	int t1       = (type_geo%100)*(type_geo/100);
	int ngauss   = numberOfGaussPoint[typeNo+1];
	int t2       = ngauss*(type_geo/100);
	med_2_2::med_float * refcoo = new med_2_2::med_float[t1];
	med_2_2::med_float * gscoo  = new med_2_2::med_float[t2];
	med_2_2::med_float * wg     = new med_2_2::med_float[ngauss];

	if (MEDgaussLire(id, refcoo, gscoo, wg, (med_2_2::med_mode_switch) interlacingType,
			 gaussModelName ) < 0)
	  throw MEDEXCEPTION(LOCALIZED( STRING(LOC) <<": Error while reading Gauss Model |"
				      << gaussModelName << "| for FIELD "<< fieldName
				      << " on geometric type " << MED_EN::geoNames[types[typeNo]]
				      )
			   );
	if (isFullInterlace ) { //serait inutile avec un driver template du type d'entrelacement
	  GAUSS_LOCALIZATION<FullInterlace> * loc;
	  loc = new GAUSS_LOCALIZATION<FullInterlace>(gaussModelName,types[typeNo],ngauss, refcoo,gscoo, wg);
	  MED_FIELD_DRIVER<T>::_ptrField->_gaussModel[types[typeNo]]=loc;
	} else {
	  GAUSS_LOCALIZATION<NoInterlace> * loc;
	  loc = new GAUSS_LOCALIZATION<NoInterlace>(gaussModelName,types[typeNo],ngauss, refcoo,gscoo, wg);
	  MED_FIELD_DRIVER<T>::_ptrField->_gaussModel[types[typeNo]]=loc;
	}
//	cout << *MED_FIELD_DRIVER<T>::_ptrField->_gaussModel[types[typeNo]] << endl;
	delete [] refcoo;delete [] gscoo; delete [] wg;
    }
    delete[] gaussModelName ;

    if ( strcmp(profilName,MED_NOPFL) ) {
      anyProfil = true;
      pflSize = med_2_2::MEDnValProfil(id,profilName);
      if ( pflSize  <= 0)
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Error while reading the profil size of |"
				     << profilName << "|" ));

      profilSize[typeNo]=pflSize;
      profilList[typeNo].resize(pflSize);
      ret = med_2_2::MEDprofilLire(id,&profilList[typeNo][0],profilName); // cf item 16 Effective STL
      profilNameList[typeNo]=string(profilName);
    }
  }

  delete[] profilName ;

  //MESSAGE ("Index              : "<< index);
  assert(index == totalNumberOfElWg*numberOfComponents);
  assert(MED_FIELD_DRIVER<T>::_ptrField->_numberOfValues ==  mySupport->getNumberOfElements(MED_ALL_ELEMENTS));

  if (anyProfil) {

    for (int typeNo=0; typeNo < NumberOfTypes; typeNo++) {

      // Trouve l'index du type g�om�trique dans la liste des types g�om�triques du maillage
      // correspondant au type g�om�trique du champ trait�
      vector<MED_EN::medGeometryElement>::iterator meshTypeNoIt =
	find(meshGeoType.begin(),meshGeoType.end(),types[typeNo]); //G�rer l'exception
      if ( meshTypeNoIt ==  meshGeoType.end() )
	throw MEDEXCEPTION(LOCALIZED( STRING(LOC) <<": Can't find "<< MED_EN::geoNames[types[typeNo]]
				      << " on entity " << MED_EN::entNames[entityType]
				      << " in geometric type list of mesh " << meshName
				      )
			   );
      int meshTypeNo = meshTypeNoIt -  meshGeoType.begin();

      if (! profilList[typeNo].empty() ) {

// 	for (int j =0 ; j< meshGeoType.size();++j)
// 	  cout << "--MeshTypeNo : "<<meshTypeNo<<"-> meshNbOfElOfTypeC["<<j<<"]="<<meshNbOfElOfTypeC[j]<<endl;
// 	cout << "--typeNo--" << typeNo << endl;
// 	cout << "meshNbOfElOfTypeC["<<meshTypeNo<<"]=" << meshNbOfElOfTypeC[meshTypeNo] <<endl;

	// Transformer les num�ros locaux d'entit�s medfichier en num�ro global medm�moire
	for (int i = 0 ; i < profilList[typeNo].size(); i++ ) {
	// Les num�ros des entit�s commencent � 1 dans MEDfichier comme dans MEDm�moire
	// meshNbOfElOfTypeC[0]=0 ...meshNbOfEltOfTypeC[meshTypeNo]=
	// meshNbOfElOfTypeC[meshTypeNo-1]+nbrOfElem of meshTypeNo type
	// rem1 : Si le meshTypeNo trouv� est 0 (premier type g�om�trique du maillage
	// il ne faut pas d�caler les num�ros du profils qui commencent � 1 dans MEDFICHIER
	// rem2 : meshNbOfElOfTypeC[NumberOfTypes] ne devrait jamais �tre utilis�
	  profilList[typeNo][i]+=meshNbOfElOfTypeC[meshTypeNo];
	}
      } else {
	// Cr�er le profil <MED_ALL> pour ce type g�om�trique
	// uniquement pour renseigner le tableau skyline avec des accesseurs directs
	// par type g�om�triques
	// REM : Une cons�quence est qu'� la r�ecriture le fichier contiendra des
	// profils sur certains types g�om�triques alors qu'� la lecture il n'y en avait pas !
	// Solution : Stocker les noms des profils et les utiliser pour savoir si il y avait ou non
	//            un profil
	int pflSize   = meshNbOfElOfType[meshTypeNo];
	// profil    = new int[pflSize];

	profilList[typeNo].resize(pflSize);
	profilSize[typeNo]=pflSize;

	for (int j = 1; j <= pflSize; j++) {
	  profilList[typeNo][j-1]=meshNbOfElOfTypeC[meshTypeNo] + j ; // index MEDMEM commence � 1
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
//    cout << "Valeurs du skyline du SUPPORT partiel cr�e : " << *skyLine << endl;
  }

  // Cr�er un driver sp�cifique pour les modes MED_FULL_INTERLACE et MED_NO_INTERLACE
  // serait plus efficace.
  bool anyGauss = (numberOfGaussPoint != vector<int>(numberOfGaussPoint.size(),1));
  SCRUTE(anyGauss);
  MEDMEM_Array_ * Values;
  if (anyGauss) {
    SCRUTE(mySupport->getNumberOfElements(MED_ALL_ELEMENTS) );
    SCRUTE(NumberOfTypes);
    SCRUTE(numberOfElementsOfTypeC[NumberOfTypes]-1);
    assert(mySupport->getNumberOfElements(MED_ALL_ELEMENTS) == (numberOfElementsOfTypeC[NumberOfTypes]-1) );
    Values = new ArrayFullWg(myValues,
			     numberOfComponents,
			     numberOfElementsOfTypeC[NumberOfTypes]-1,
			     // Up : Prend en compte les profils et
			     // Ne prend pas en compte le nbre de composantes et
			     // le nombre de points de Gauss
			     NumberOfTypes,
			     &numberOfElementsOfTypeC[0],
			     &numberOfGaussPoint[0],
			     true,true);
//     cout << "Valeurs du ArrayFullWg cr�e : " << endl <<
//       *(static_cast<ArrayFullWg*>(Values))  << endl;
  } else
    Values = new ArrayFull(myValues,numberOfComponents,totalNumberOfElWg,
				       true,true);
  if (MED_FIELD_DRIVER<T>::_ptrField->_value != NULL)
    delete MED_FIELD_DRIVER<T>::_ptrField->_value;

  if ( MED_FIELD_DRIVER<T>::_ptrField->getInterlacingType() == MED_EN::MED_NO_INTERLACE )
    {
      if (Values->getGaussPresence())
	MED_FIELD_DRIVER<T>::_ptrField->_value=ArrayConvert(*static_cast<ArrayFullWg*>(Values));
      else
	MED_FIELD_DRIVER<T>::_ptrField->_value=ArrayConvert(*static_cast<ArrayNo*    >(Values));
      delete Values;
    }
  else
    MED_FIELD_DRIVER<T>::_ptrField->_value=Values;

  MED_FIELD_DRIVER<T>::_ptrField->_isRead = true ;

  MED_FIELD_DRIVER<T>::_ptrField->_support=mySupport; //Pr�venir l'utilisateur ?


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
  typedef typename MEDMEM_ArrayInterface<T,NoInterlace,NoGauss>::Array   ArrayNo;
  typedef typename MEDMEM_ArrayInterface<T,NoInterlace,Gauss>::Array     ArrayNoWg;
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,NoGauss>::Array ArrayFull;
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,Gauss>::Array   ArrayFullWg;

  typedef map<MED_EN::medGeometryElement,GAUSS_LOCALIZATION<FullInterlace>*> locMapFull;
  typedef map<MED_EN::medGeometryElement,GAUSS_LOCALIZATION<NoInterlace>*>   locMapNo;
  typedef map<MED_EN::medGeometryElement,GAUSS_LOCALIZATION_*>   locMap;

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
  const locMap & gaussModel = MED_FIELD_DRIVER<T>::_ptrField->_gaussModel;


  string meshName = mySupport->getMeshName();
  SCRUTE(meshName);
  if ( meshName.size() > MED_TAILLE_NOM ) {
    meshName = meshName.substr(0,MED_TAILLE_NOM);
    MESSAGE( "Be careful <meshName> size must not be > MED_TAILLE_NOM, using meshName : |"<< meshName <<"|." );
  }
  MED_EN::medEntityMesh entityType = mySupport->getEntity();

  // Reconstruit les listes contigues des noms de composantes et des unit�s
  // Les noms sont tronqu�s � MED_TAILLE_PNOM22
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

  // V�rifier si le champ existe d�j�
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
  // de doubler l'utilisation de la taille m�moire si le champ n'est pas dans
  // le bon mode.
  FIELD<T,FullInterlace> * myField = 0;
  if ( MED_FIELD_DRIVER<T>::_ptrField->getInterlacingType() == MED_EN::MED_FULL_INTERLACE )
    myField = MED_FIELD_DRIVER<T>::_ptrField;
  else
    myField = FieldConvert( *( dynamic_cast< FIELD<T,NoInterlace> * > (MED_FIELD_DRIVER<T>::_ptrField )
			       )
			    );


  // Il est necessaire de calculer le tableau
  // du nombre d'entit�s cumul�es de chaque type g�om�trique du maillage
  // pour convertir les profils de la num�rotation globale
  // � la num�rotation locale au type g�om�trique.
  // Pour cel� on �tablit ce tableau � partir de l'objet MESH si la relation SUPPORT-MESH existe.
  // Si le maillage existe dans le fichier MED  on essaye �galement de reconstituer ce tableau
  // pour v�rifier la coh�rence des informations.
  // Si la relation SUPPRT-MESH n'esiste pas  on constitue le tableau uniquement � partir du fichier qui
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
      this->getMeshGeometricTypeFromFile(id, meshName,
                                         entityType,
                                         fileMeshGeoType,fileMeshNbOfElOfType,fileMeshNbOfElOfTypeC);

    if (meshPtr) {
      this->getMeshGeometricTypeFromMESH( meshPtr, entityType,meshGeoType,
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

  const MED_EN::medGeometryElement * types = mySupport->getTypes() ;
  int numberOfTypes = mySupport->getNumberOfTypes() ;
  int numberOfElForMED = -1;
  const T   * value   = NULL;
  int index = 1 ;
  // on boucle sur tout les types pour ecrire les tableaux de valeur
  for (int typeNo=0;typeNo<numberOfTypes;typeNo++) {

    int numberOfElements = mySupport->getNumberOfElements(types[typeNo]) ;
    //UP : prend en compte les profils, pas les points de Gauss

    //value = MED_FIELD_DRIVER<T>::_ptrField->getRow(index) ;
    // rem 1 : le getRow du Array est diff�rent de celui du FIELD si le SUPPORT contient
    //         des profils (les indices des valeurs ne se suivent pas forc�ment)
    // rem 2 : Afin de respecter la norme MEDFICHIER, les indices contenus dans les
    //         profils doivent �tre croissant
    if (onAll) {
      value = myField->getRow(index);
      profilName=MED_NOPFL;
      numberOfElForMED = numberOfElements;
    } else {
      value = myField->getRow(number[index-1]);
      profilName = profilNameList[typeNo].substr(0,MED_TAILLE_NOM);
      // Rem : Si le SUPPORT n'est pas onAll mais que pour un type g�om�trique donn� le nom
      // du profil associ� est MED_NOPFL alors le profil n'est pas �crit dans le fichier MED.
      // Car en MEDMEMOIRE si le champ repose sur des �l�ments de deux types g�om�triques
      // diff�rents et est d�fini sur tous les �l�ments d'un type g�om�trique
      // mais pas de l'autre, il existe tout de m�me des profils sur les deux types g�om�triques.
      // Ce n'est pas le cas en MEDFICHIER.
      vector<int> profil(&number[index-1],&(number[index-1])+numberOfElements);

      // Trouve l'index du type g�om�trique dans la liste des types g�om�triques du maillage
      // correspondant au type g�om�trique du champ en cours de traitement
      vector<MED_EN::medGeometryElement>::iterator meshTypeNoIt =
	find(meshGeoType.begin(),meshGeoType.end(),types[typeNo]);
      if ( meshTypeNoIt ==  meshGeoType.end() )
	throw MEDEXCEPTION(LOCALIZED( STRING(LOC) <<": Can't find "<< MED_EN::geoNames[types[typeNo]]
				      << " on entity " << MED_EN::entNames[entityType]
				      << " in geometric type list of mesh " << meshName
				      )
			   );

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

      //REM : Ce n'est pas �vident, mais lorsqu'il y a un profil, le nombre de valeurs
      //      que l'on indique � MEDchampEcr est le nombre de valeurs sans profil, d'o�
      //      le nombre d'�l�ments du maillage sur le type g�om�trique courant.
      numberOfElForMED = meshNbOfElOfType[meshTypeNo];

      for (int ind=0;ind < numberOfElements;++ind) {
// 	cout << "number["<<index-1<<"]="<<number[index-1]<<endl;
// 	cout << "profil1["<<ind<<"]="<<profil[ind]<<endl;
	profil[ind]-=meshNbOfElOfTypeC[meshTypeNo];
//  	cout << "profil2["<<ind<<"]="<<profil[ind]<<endl;
      }

      if ( profil[numberOfElements-1] > numberOfElForMED )
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

      if ( med_2_2::MEDMEMprofilEcr(id,
				 &profil[0],
				 numberOfElements,
				 const_cast<char *>(profilName.c_str())) < 0)

	throw MEDEXCEPTION(LOCALIZED( STRING(LOC) <<": Error while writing "<< numberOfElements
				      << " values for MED profil "<< profilName
				    )
			 );
    }


    string locName=MED_NOGAUSS;
    if (myField->getGaussPresence()) {
//       cout << endl << "Nombre de points de Gauss � l'�criture de " << fieldName
// 	   << " pour le type g�om�trique : " << MED_EN::geoNames[types[typeNo]]
// 	   << " : " << myField->getNumberOfGaussPoints(types[typeNo]) << endl;
//       cout << *mySupport << endl;

      const GAUSS_LOCALIZATION_ * locPtr=0;
      locMap::const_iterator it;
      if ( ( it = gaussModel.find(types[typeNo])) != gaussModel.end() )
	locPtr = (*it).second;
      else
	throw MEDEXCEPTION(LOCALIZED( STRING(LOC) <<": Error while creating Gauss Model for FIELD "<< fieldName 
				      << " on entity " << MED_EN::entNames[entityType]
				      << " and geometric type " << MED_EN::geoNames[types[typeNo]] << " with (it,or) = ("
				      << MED_FIELD_DRIVER<T>::_ptrField->_iterationNumber << ","
				      << MED_FIELD_DRIVER<T>::_ptrField->_orderNumber << "), with profilName "
				      << profilName << " on mesh " << meshName
				      << " : Can't find a Gauss localisation model for this geometric type" 
			    )
			 );

      int ngauss = -1;
      if ( locPtr->getInterlacingType() == MED_EN::MED_FULL_INTERLACE ) {
	const GAUSS_LOCALIZATION<FullInterlace> & loc=*(static_cast<const GAUSS_LOCALIZATION<FullInterlace> * >(locPtr));
	ngauss = loc.getNbGauss();
	locName=loc.getName();
	err=med_2_2::MEDMEMgaussEcr(id,
			       (med_2_2::med_geometrie_element) loc.getType(),
			       (med_2_2::med_float *)           loc.getRefCoo().getPtr(),
			                                        med_2_2::MED_FULL_INTERLACE,
			       (med_2_2::med_int)               ngauss,
			       (med_2_2::med_float *)           loc.getGsCoo().getPtr(),
			       (med_2_2::med_float *)           (&loc.getWeight()[0]),
			       const_cast<char *>               (locName.c_str())
			       );
      } else {
	const GAUSS_LOCALIZATION<NoInterlace> & loc=*(static_cast<const GAUSS_LOCALIZATION<NoInterlace> * >(locPtr));
	ngauss = loc.getNbGauss();
	locName=loc.getName();
	err=med_2_2::MEDMEMgaussEcr(id,
			       (med_2_2::med_geometrie_element) loc.getType(),
			       (med_2_2::med_float *)           loc.getRefCoo().getPtr(),
				                                med_2_2::MED_NO_INTERLACE,
			       (med_2_2::med_int)               ngauss,
			       (med_2_2::med_float *)           loc.getGsCoo().getPtr(),
			       (med_2_2::med_float *)           (&loc.getWeight()[0]),
			       const_cast<char *>               (locName.c_str())
			       );

      };

      if ( err != 0 )
	throw MEDEXCEPTION(LOCALIZED( STRING(LOC) <<": Error while writing Gauss Model for FIELD "<< fieldName
				      << " on entity " << MED_EN::entNames[entityType]
				      << " and geometric type " << MED_EN::geoNames[types[typeNo]] 
			    )
			 );

	//numberOfElForMED *= mySupport->getNumberOfGaussPoints(types[typeNo]); //Deplacer la m�thode dans FIELD
	numberOfElForMED *= ngauss;
    }

    MESSAGE("MED_FIELD_DRIVER22<T>::_medIdt                       : "<<id);
    MESSAGE("meshName.c_str()                : "<<meshName.c_str());
    MESSAGE("MED_FIELD_DRIVER<T>::_ptrField->getName()            : "<<MED_FIELD_DRIVER<T>::_ptrField->getName());
    MESSAGE("MED_FIELD_DRIVER<T>::_fieldName                      : "<<MED_FIELD_DRIVER<T>::_fieldName);
    MESSAGE("value                           : "<<value);
    MESSAGE("numberOfElements                : "<<numberOfElements);
    MESSAGE("numberOfElForMED                : "<<numberOfElForMED);
    MESSAGE("entityType                      : "<<MED_EN::entNames[entityType]);
    MESSAGE("types[i]                        : "<<MED_EN::geoNames[types[typeNo]]);
    MESSAGE("NumberOfGaussPoint[i]           : "<<myField->getNumberOfGaussPoints(types[typeNo]));
    MESSAGE("MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber() : "<<MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber());
    MESSAGE("MED_FIELD_DRIVER<T>::_ptrField->getTime()            : "<<MED_FIELD_DRIVER<T>::_ptrField->getTime());
    MESSAGE("MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber()     : "<<MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber());

    // Rem 1 : le nombre d'�l�ments pass� � MEDchampEcr ne doit pas tenir compte de la taille
    //         des profils : c'est la taille du champ sans profil.
    err=med_2_2::MEDchampEcr(id,
			     const_cast <char*> ( meshName.c_str()) ,
			     const_cast <char*> ( fieldName.c_str()),
			     (unsigned char*)value, med_2_2::MED_FULL_INTERLACE,
			     numberOfElForMED,
			     //UP : prend en compte le nombre de points de Gauss mais 
			     //     pas le nombre de composantes
			     const_cast <char*> ( locName.c_str()),
			     MED_ALL,
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

    index += numberOfElements ; //Ne doit pas prendre en compte le nombre de points de GAUSS
                                //ni les composantes.

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
