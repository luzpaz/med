//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
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
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_GaussLocalization.hxx"

namespace MEDMEM {

/*!

  Driver Med for FIELD.

  Generic part : implement open and close methods.

*/

template <class T> class MED_FIELD_DRIVER22 : public virtual MED_FIELD_DRIVER<T>
{
protected:

  med_2_3::med_idt        _medIdt;

  bool createFieldSupportPart1(med_2_3::med_idt id,
                               const string &   fieldName,
                               med_2_3::med_int ndt,
                               med_2_3::med_int od,
                               SUPPORT &        support,
                               string &         meshName,
                               vector<int> &    numberOfElementsOfTypeC,
                               vector<int> &    numberOfGaussPoint,
                               int &            totalNumberOfElWg,
                               MED_EN::medEntityMesh & fieldMedFileEntity
                               ) const throw (MEDEXCEPTION);

  void getMeshGeometricTypeFromFile(med_2_3::med_idt      id,
                                    string &              meshName,
                                    MED_EN::medEntityMesh entite,
                                    vector<MED_EN::medGeometryElement> & geoType,
                                    vector<int> &         nbOfElOfType,
                                    vector<int> &         nbOfElOfTypeC
                                    ) const throw(MEDEXCEPTION);

  void getMeshGeometricTypeFromMESH( MESH * meshPtr,
				     MED_EN::medEntityMesh  entity,
				     vector<MED_EN::medGeometryElement> & geoType,
				     vector<int> &nbOfElOfType,
				     vector<int> &nbOfElOfTypeC
                                     ) const throw(MEDEXCEPTION);

  int getMeshDimensionFromFile(med_2_3::med_idt id, const string & meshName) const;
	
  medEntityMesh getMEDMEMEntityFromMEDType(medGeometryElement type, int mesh_dim) const;
	


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
    BEGIN_OF_MED(LOC);

    // we must set fieldname before open, because we must find field number in file (if it exist !!!)
    if ( MED_FIELD_DRIVER<T>::_fileName == "" )
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC)
				       << "_fileName is |\"\"|, please set a correct fileName before calling open()"
				       )
			    );
    if ( MED_FIELD_DRIVER<T>::_status==MED_OPENED )
      return;

    int accessMode = getMedAccessMode( MED_FIELD_DRIVER<T>::_accessMode, MED_EN::V22 );
    MESSAGE_MED(LOC<<"_fileName.c_str : "<< MED_FIELD_DRIVER<T>::_fileName.c_str()<<",mode : "<< MED_FIELD_DRIVER<T>::_accessMode);
    MED_FIELD_DRIVER22<T>::_medIdt = med_2_3::MEDouvrir( (const_cast <char *> (MED_FIELD_DRIVER<T>::_fileName.c_str())),(med_2_3::med_mode_acces) accessMode);
    MESSAGE_MED(LOC<<"_medIdt : "<< MED_FIELD_DRIVER22<T>::_medIdt );
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

  END_OF_MED(LOC);
  }

  void close() {
  const char* LOC = "MED_FIELD_DRIVER22::close()";
  BEGIN_OF_MED(LOC);
    med_2_3::med_int err = 0;
    if (MED_FIELD_DRIVER<T>::_status == MED_OPENED) {
      err=med_2_3::MEDfermer(MED_FIELD_DRIVER22<T>::_medIdt);
      //H5close(); // If we call H5close() all the files are closed.
      MED_FIELD_DRIVER<T>::_status = MED_CLOSED;
      MED_FIELD_DRIVER22<T>::_medIdt = MED_INVALID;
      MESSAGE_MED(" MED_FIELD_DRIVER22::close() : MEDfermer : _medIdt= " << MED_FIELD_DRIVER22<T>::_medIdt );
      MESSAGE_MED(" MED_FIELD_DRIVER22::close() : MEDfermer : err    = " << err );
    }
  END_OF_MED(LOC);
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
  MED_FIELD_RDONLY_DRIVER22():MED_FIELD_DRIVER<T>()
  {
    this->GENDRIVER::_accessMode = MED_EN::RDONLY;
  }

  /*!
    Constructor.
  */
  template <class INTERLACING_TAG>
  MED_FIELD_RDONLY_DRIVER22(const string & fileName,
			    FIELD<T, INTERLACING_TAG> * ptrField):
    IMED_FIELD_RDONLY_DRIVER<T>(fileName,ptrField),
    MED_FIELD_DRIVER22<T>(fileName,ptrField,MED_EN::RDONLY),
    MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::RDONLY)
  {
  const char* LOC = "MED_FIELD_RDONLY_DRIVER22::MED_FIELD_RDONLY_DRIVER22(const string & fileName, const FIELD<T,INTERLACING_TAG> * ptrField)";
  BEGIN_OF_MED(LOC);
  END_OF_MED(LOC);
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
  MED_FIELD_WRONLY_DRIVER22():MED_FIELD_DRIVER<T>()
  {
    this->GENDRIVER::_accessMode = MED_EN::WRONLY;
  }

  /*!
    Constructor.
  */
  template <class INTERLACING_TAG>
  MED_FIELD_WRONLY_DRIVER22(const string & fileName,
			    FIELD<T, INTERLACING_TAG> * ptrField):
    IMED_FIELD_WRONLY_DRIVER<T>(fileName,ptrField),
    MED_FIELD_DRIVER22<T>(fileName,ptrField,MED_EN::WRONLY),
    MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::WRONLY)
  {
  const char* LOC = "MED_FIELD_WRONLY_DRIVER22::MED_FIELD_WRONLY_DRIVER22(const string & fileName, const FIELD<T,INTERLACING_TAG> * ptrField)";
  BEGIN_OF_MED(LOC);
  END_OF_MED(LOC);
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
  MED_FIELD_RDWR_DRIVER22():MED_FIELD_DRIVER22<T>()
  {
    this->GENDRIVER::_accessMode = MED_EN::RDWR;
  }

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
    MED_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::RDWR),
    IMED_FIELD_RDWR_DRIVER<T>(fileName,ptrField)
  {
  const char* LOC = "MED_FIELD_RDWR_DRIVER22::MED_FIELD_RDWR_DRIVER22(const string & fileName, const FIELD<T,INTERLACING_TAG> * ptrField)";
  BEGIN_OF_MED(LOC);
    //_accessMode = MED_RDWR ;
  END_OF_MED(LOC);
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
  Deux tableaux directements exploitables par MEDMEMnArray sont renvoyés :
  - numberOfElementsOfTypeC : nombres d'entités cumulés de chaque type géométrique
       avec numberOfElementsOfTypeC[0]=1 et de taille nombre de types+1
  - numberOfGaussPoint : nombre de points de Gauss par type géométrique
       avec numberOfGaussPoint[0]=1 et de taille  nombre de types+1
*/

template <class T> bool
MED_FIELD_DRIVER22<T>::createFieldSupportPart1(med_2_3::med_idt        id,
					       const string &          fieldName,
					       med_2_3::med_int        ndt,
					       med_2_3::med_int        od,
					       SUPPORT &               support,
					       string &                meshName,
					       vector<int> &           numberOfElementsOfTypeC,
					       vector<int> &           numberOfGaussPoint,
					       int &                   totalNumberOfElWg,
                                               MED_EN::medEntityMesh & fieldMedFileEntity
					       ) const throw (MEDEXCEPTION)
{

  //EF : Gérer le meshName pour le driver 2.2
  const char * LOC="MED_FIELD_DRIVER<T>::createFieldSupportPart1(...)";

  BEGIN_OF_MED(LOC);

  map<int, list<MED_EN::medGeometryElement> > CellAndNodeEntities;
  map<int, list<MED_EN::medGeometryElement> >::iterator currentEntity;
  CellAndNodeEntities[MED_EN::MED_CELL]  = MED_EN::meshEntities[MED_EN::MED_CELL];
  CellAndNodeEntities[MED_EN::MED_NODE] = MED_EN::meshEntities[MED_EN::MED_NODE];
  list< MED_EN::medGeometryElement >::const_iterator currentGeometry;

  MED_EN::medEntityMesh entityCurrent;
  MED_EN::medGeometryElement geometryCurrent;

  MED_EN::medEntityMesh entity;
  MED_EN::medEntityMesh medmem_entity;

  bool alreadyFoundAnEntity=false,alreadyFoundPdtIt = false;
  int  numberOfElements = 0;
  int  numberOfGeometricType = 0;
  MED_EN::medGeometryElement geometricType[MED_NBR_GEOMETRIE_MAILLE];
  int numberOfElementsOfType[MED_NBR_GEOMETRIE_MAILLE];
  numberOfElementsOfTypeC.clear();numberOfGaussPoint.clear();
  numberOfElementsOfTypeC.resize(MED_NBR_GEOMETRIE_MAILLE+1);
  numberOfGaussPoint.resize(MED_NBR_GEOMETRIE_MAILLE+1);

  med_2_3::med_int nmaa=0, ngauss=0, numdt=-1, numo=-1, nbPdtIt=0, nbPdtIt1=0, nbPdtIt2=0;
  char dtunit[MED_TAILLE_PNOM22+1];
  char maa[MED_TAILLE_NOM+1];
  med_2_3::med_float   dt=-1.0;
  med_2_3::med_booleen local;
  med_2_3::med_err     ret=1;
  numberOfElementsOfTypeC[0] = 1;
  numberOfGaussPoint[0] = 1;
  totalNumberOfElWg = 0;
  int field_dim=0;

  /* Détermine le type d'entité et la liste des types géométriques associés
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
          geometryCurrent == MED_EN::MED_TRIA6 || geometryCurrent == MED_EN::MED_QUAD8 || 
          geometryCurrent == MED_EN::MED_POLYGON)
        entityCurrent = MED_EN::MED_FACE;

      nbPdtIt1 = med_2_3::MEDnPasdetemps(id, const_cast <char*> ( fieldName.c_str() ),
					 (med_2_3::med_entite_maillage)   (*currentEntity).first,
					 (med_2_3::med_geometrie_element)  *currentGeometry );

      nbPdtIt2 = med_2_3::MEDnPasdetemps(id, const_cast <char*> ( fieldName.c_str() ),
					 (med_2_3::med_entite_maillage)   entityCurrent,
					 (med_2_3::med_geometrie_element)  geometryCurrent );

      medmem_entity=entityCurrent;
      if (nbPdtIt2 < nbPdtIt1) entityCurrent = (*currentEntity).first ;

      nbPdtIt = (nbPdtIt1>nbPdtIt2)?nbPdtIt1:nbPdtIt2;

      if ( nbPdtIt <=  0 )
	continue;

      /* Verifie que le champ n'est pas défini sur un autre type d'entité */
      if ( alreadyFoundAnEntity )
      {
        //if (entity != (*currentEntity).first )  (NB)
        if ( medmem_entity != entity )
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Field |"  << fieldName
                                       << "| with (ndt,or) = (" << ndt << ","
                                       << od << ") must not be defined on different entity types" ));

      }
      else
      { 
        //entity=(*currentEntity).first; (NB)
        entity=medmem_entity;
        fieldMedFileEntity = entityCurrent;
        alreadyFoundAnEntity = true;

        //at this stage, the entity is only related to the geometry dimension
        // with no relation to the mesh dimension
        // therefore MED_CELL refer to 3D and MED_FACE to 2D
        // the correct entity (which depends on the mesh dimension, is only 
        // determined later

        switch (entity)
        {
        case MED_NODE:
          field_dim=0;
          break;
        case MED_EDGE:
          field_dim=1;
          break;
        case MED_FACE:
          field_dim=2;
          break;
        case MED_CELL:
          field_dim=3;
          break;
        }
      };


      /* Cherche le champ pour le <ndt>,<ot> demandé et détermine le nombre de points de Gauss*/
      ret = 0; alreadyFoundPdtIt = false; ngauss =0;
      for ( med_2_3::med_int j=1; j <= nbPdtIt; j++ ) {

        // Search how many <ngauss> (<fieldName>,<ndt>,<ot>) has   (NB)
        //ret += med_2_3::MEDpasdetempsInfo(id, const_cast <char*> ( fieldName.c_str() ),
        //                               (med_2_3::med_entite_maillage)   (*currentEntity).first,
        //                               (med_2_3::med_geometrie_element)  *currentGeometry,
	//				 j, &ngauss,  &numdt,  &numo, dtunit, &dt,
	//				  maa, &local, &nmaa);

	ret += med_2_3::MEDpasdetempsInfo(id, const_cast <char*> ( fieldName.c_str() ),
					  (med_2_3::med_entite_maillage)   entityCurrent,
					  (med_2_3::med_geometrie_element)  *currentGeometry,
					  j, &ngauss,  &numdt,  &numo, dtunit, &dt,
					  maa, &local, &nmaa);
		
	MED_FIELD_DRIVER<T>::_ptrField->setTime(dt); // PAL12664
	
	if ( ndt == numdt && numo == od ) {
	  alreadyFoundPdtIt = true;

	  if ( nmaa > 1 ) {
	    MESSAGE_MED(LOC<<" Field |" << fieldName << "| with (ndt,or) = ("
		    << ndt << "," << od << ") for (entityType,geometricType)=("
		    << MED_EN::entNames[entityCurrent] << ","
		    << MED_EN::geoNames[*currentGeometry] << ")"
		    << "is defined on multiple meshes, using dafault mesh  |" << maa << "|" );
	  }

	  if ( !local) {
	    MESSAGE_MED(" Field |" << fieldName << "| with (ndt,or) = ("
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

		//VB commented out to allow for fields on multiple meshes
	 //  if ( ! meshName.empty() )
// 	    if ( meshName != maa ) {
// 	      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Field |" << fieldName << "| with (ndt,or) = ("
// 					   << ndt << "," << od << ") for (entityType,geometricType)=("
// 					   << MED_EN::entNames[entityCurrent] << ","
// 					   << MED_EN::geoNames[*currentGeometry] << ")"
// 					   << "is defined on mesh |" << maa << "| not on mesh |" << meshName ));
// 	    }
 	  break;
	}

      }

      MESSAGE_MED(LOC << " a (dt,it) is found ?? " << alreadyFoundPdtIt);

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

      if ( meshName.empty() ) // PAL19635: error with TestMEDSPLITTER
        meshName = maa;

      if ( (numberOfElements = med_2_3::MEDnVal(id, const_cast <char*> ( fieldName.c_str() ),
                                                (med_2_3::med_entite_maillage)   entityCurrent,
                                                (med_2_3::med_geometrie_element) *currentGeometry,
                                                numdt, numo,(char *) meshName.c_str(), med_2_3::MED_COMPACT))  <=  0 )
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


  //retrieves the right medmem entity type from field_dim and mesh_dim
  int mesh_dim = MED_FIELD_DRIVER22<T>::getMeshDimensionFromFile(id,meshName);
  if (mesh_dim==2 && field_dim==2)
    entity=MED_CELL;

  if ( alreadyFoundAnEntity) {
    support.setName(fieldName+" Support");
    support.setMeshName(meshName); // Vérifier que les différents noms de maillages lus soient identiques
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
    numberOfElementsOfTypeC.resize(numberOfGeometricType+1);
    numberOfGaussPoint.resize(numberOfGeometricType+1);
		
    return alreadyFoundAnEntity;
  } else
    return false;
}

template <class T> MED_EN::medEntityMesh
MED_FIELD_DRIVER22<T>::getMEDMEMEntityFromMEDType(medGeometryElement type,
                                                  int mesh_dim) const
{
  int elem_dim = type/100;
  if (type==MED_POLYGON) elem_dim=2;
  if (type==MED_POLYHEDRA) elem_dim=3;

  if (elem_dim==3)
    return MED_CELL;
  if (elem_dim==2)
    if (mesh_dim==2)
      return MED_CELL;
    else if (mesh_dim==3)
      return MED_FACE;
  if (elem_dim==1)
    return MED_EDGE;
  if(elem_dim==0)
    return MED_NODE;
}


template <class T> int
MED_FIELD_DRIVER22<T>::getMeshDimensionFromFile(med_2_3::med_idt id,
                                                const string &   meshName) const
{
  const char* LOC = "MED_FIELD_DRIVER<T>::getMeshDimensionFromFile(...)";
  BEGIN_OF_MED(LOC);

  int numberOfGeometricType=0;
  MED_EN::medGeometryElement geometricType[MED_NBR_GEOMETRIE_MAILLE];
  int numberOfElementsOfType [MED_NBR_GEOMETRIE_MAILLE];
  int numberOfElementsOfTypeC[MED_NBR_GEOMETRIE_MAILLE+1];
  med_2_3::med_int   numberOfElements=0;
  med_2_3::med_table quoi;

  /*in MED file, all entities are regarded as MED_CELL
    (except for those related to descending connectivities),
    whereas in MEDMEM the distinction between MED_CELL, MED_FACE and MED_EDGE exists
    it is therefore necessary to distinguish the MED-file entity
    that will be used for the call to MED-file
    and the MEDMEM entity*/
  MED_EN::medEntityMesh entity=MED_EN::MED_CELL;
  quoi=med_2_3::MED_CONN;


  list<MED_EN::medGeometryElement>::const_iterator currentGeometry;
  bool alreadyFoundAnEntity = false;
  numberOfElementsOfTypeC[0]=0;

  for (currentGeometry  = (MED_EN::meshEntities[entity]).begin();
       currentGeometry != (MED_EN::meshEntities[entity]).end(); currentGeometry++)
  {
    numberOfElements =
      med_2_3::MEDnEntMaa(id,
                          const_cast<char*> (meshName.c_str()),
                          med_2_3::MED_CONN,
                          med_2_3::MED_MAILLE,
                          (med_2_3::med_geometrie_element) *currentGeometry,
                          med_2_3::MED_NOD);
    if (numberOfElements <= 0)
      continue;

    alreadyFoundAnEntity = true;
    numberOfElementsOfType[numberOfGeometricType] = numberOfElements;
    numberOfElementsOfTypeC[numberOfGeometricType+1] =
      numberOfElementsOfTypeC[numberOfGeometricType]+numberOfElements;
    MED_EN::medGeometryElement geomType;

    //MED_FILE uses MED_NONE as a geometricType to describe MED_NODE
    //MEDMEM uses MED_POINT1
    if ( *currentGeometry==MED_NONE)
      geomType=MED_POINT1;
    else
      geomType=*currentGeometry;
    geometricType[numberOfGeometricType] = geomType;

    numberOfGeometricType++;
  }

  //Because MEDFILE and MEDMEM differ on the definition of MED_CELL
  //it is necessary to remove the cells that do not
  //have maximum cell dimension in the range covered by geometricType
  int maxdim=0;
  for (int i=0; i<numberOfGeometricType; i++)
  {
    CELLMODEL model(geometricType[i]);
    int dim = model.getDimension();
    if (dim>maxdim) maxdim=dim;
  }

  return maxdim;

}

/*!

  Renvoie la liste <geoType> des types géométriques définis dans le maillage <meshName>
  pour le type d'entité <entity>.
  * < nbOfElOfType > contient le nombre d'entités de chaque type
  * < numberOfElementsOfTypeC > contient le nombre d'entités cumulées de chaque type
                              avec numberOfElementsOfTypeC[0]=0;
  * < allDimensions > controls dimension of returned types of entity == MED_CELL
*/
template <class T> void
MED_FIELD_DRIVER22<T>::getMeshGeometricTypeFromFile(med_2_3::med_idt      id,
                                                    string &              meshName,
                                                    MED_EN::medEntityMesh entity,
                                                    vector<MED_EN::medGeometryElement> & geoType,
                                                    vector<int> &         nbOfElOfType,
                                                    vector<int> &         nbOfElOfTypeC
                                                    ) const throw(MEDEXCEPTION)
{
  const char* LOC = "MED_FIELD_DRIVER<T>::getMeshGeometricTypeFromFile(...)";
  BEGIN_OF_MED(LOC);

  int numberOfGeometricType=0;
  MED_EN::medGeometryElement geometricType[MED_NBR_GEOMETRIE_MAILLE];
  int numberOfElementsOfType [MED_NBR_GEOMETRIE_MAILLE];
  int numberOfElementsOfTypeC[MED_NBR_GEOMETRIE_MAILLE+1];
  int dimOfType[MED_NBR_GEOMETRIE_MAILLE];
  int maxdim=0;
  med_2_3::med_int   numberOfElements=0;
  med_2_3::med_table quoi;

  /*in MED file, all entities are regarded as MED_CELL
    (except for those related to descending connectivities),
    whereas in MEDMEM the distinction between MED_CELL, MED_FACE and MED_EDGE exists
    it is therefore necessary to distinguish the MED-file entity
    that will be used for the call to MED-file
    and the MEDMEM entity*/
  MED_EN::medEntityMesh medfile_entity;
  if (entity==MED_EN::MED_NODE)
  {
    medfile_entity=MED_EN::MED_NODE;
    quoi=med_2_3::MED_COOR;
  }
  else
  {
    medfile_entity=MED_EN::MED_CELL;
    quoi=med_2_3::MED_CONN;
  }

  list<MED_EN::medGeometryElement>::const_iterator currentGeometry;
  bool alreadyFoundAnEntity = false;
  numberOfElementsOfTypeC[0]=0;

  for (currentGeometry  = (MED_EN::meshEntities[entity]).begin();
       currentGeometry != (MED_EN::meshEntities[entity]).end(); currentGeometry++)
  {
    numberOfElements =
      med_2_3::MEDnEntMaa(id,
                          const_cast<char*> (meshName.c_str()),
                          quoi,
                          (med_2_3::med_entite_maillage) medfile_entity,
                          (med_2_3::med_geometrie_element) *currentGeometry,
                          med_2_3::MED_NOD);
    if (numberOfElements <= 0)
      continue;

    alreadyFoundAnEntity = true;
    numberOfElementsOfType[numberOfGeometricType] = numberOfElements;
    numberOfElementsOfTypeC[numberOfGeometricType+1] =
      numberOfElementsOfTypeC[numberOfGeometricType]+numberOfElements;
    MED_EN::medGeometryElement geomType;

    //MED_FILE uses MED_NONE as a geometricType to describe MED_NODE
    //MEDMEM uses MED_POINT1
    if ( *currentGeometry==MED_NONE)
      geomType=MED_POINT1;
    else
      geomType=*currentGeometry;
    geometricType[numberOfGeometricType] = geomType;

    //Because MEDFILE and MEDMEM differ on the definition of MED_CELL
    //it is necessary to remove the cells that do not
    //have maximum cell dimension in the range covered by geometricType
    int dim = geomType / 100;
    if ( geomType==MED_POLYGON )   dim=2;
    if ( geomType==MED_POLYHEDRA ) dim=3;
    dimOfType[ numberOfGeometricType ] = dim;
    if (dim>maxdim) maxdim=dim;

    numberOfGeometricType++;
  }

  nbOfElOfTypeC.push_back(0);
  for (int i=0; i<numberOfGeometricType; i++)
  {
    if (dimOfType[i]==maxdim || entity != MED_CELL)
    {
      geoType.push_back(geometricType[i]);
      int nbelems = numberOfElementsOfType[i];
      nbOfElOfType.push_back(nbelems);
      nbOfElOfTypeC.push_back(nbOfElOfTypeC[nbOfElOfTypeC.size()-1]+nbelems);
    }
  }

  //  geoType = vector<MED_EN::medGeometryElement>(geometricType,geometricType+numberOfGeometricType);
  //  nbOfElOfType = vector<int> (numberOfElementsOfType,numberOfElementsOfType+numberOfGeometricType);
  //  nbOfElOfTypeC = vector<int> (numberOfElementsOfTypeC,numberOfElementsOfTypeC+numberOfGeometricType+1);

//   for (int j =0 ; j<= numberOfGeometricType;++j)
//       cout << "nbOfElOfTypeC["<<j<<"]="<<nbOfElOfTypeC[j]<<endl;

  END_OF_MED(LOC);
}

/*!
reads the MESH object in order to retrieve the list of geometric types for a given entity
\param[in] meshPtr pointer to MESH
\param[in] entity entity for which the geom types are required
\param[out] geoType list of geom types
\param[out] nbOfElOfType vector containing the number of elements per type (size : ntype)
\param[out] nbOfElOfTypeC accumulated version of nbOfElType (size : ntype+1)
 */

template <class T> void
MED_FIELD_DRIVER22<T>::getMeshGeometricTypeFromMESH( MESH * meshPtr,
					  MED_EN::medEntityMesh  entity,
					  vector<MED_EN::medGeometryElement> & geoType,
					  vector<int> &nbOfElOfType,
					  vector<int> &nbOfElOfTypeC) const throw(MEDEXCEPTION)
{
  const char LOC[] = "MED_FIELD_DRIVER<T>::getMeshGeometricTypeFromMESH(...) : ";
  BEGIN_OF_MED(LOC);

  if (!meshPtr)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"ptrMesh must be non null" )); ;

  // Il est plus pratique de créer un support "onAll"
  // pour calculer les tableaux du nombre d'entités cumulées

  SUPPORT mySupportFromMesh (meshPtr, "Temporary Support From Associated Mesh", entity);
  geoType = vector<MED_EN::medGeometryElement>(mySupportFromMesh.getTypes(),
			      mySupportFromMesh.getTypes()+mySupportFromMesh.getNumberOfTypes());
  nbOfElOfType.resize(mySupportFromMesh.getNumberOfTypes());
  nbOfElOfTypeC.resize(mySupportFromMesh.getNumberOfTypes()+1);
  nbOfElOfTypeC[0]=0;

  for (int j=1; j<=mySupportFromMesh.getNumberOfTypes(); ++j) {
    nbOfElOfType[j-1]=mySupportFromMesh.getNumberOfElements(geoType[j-1]);
    nbOfElOfTypeC[j]+=nbOfElOfTypeC[j-1]+nbOfElOfType[j-1];
  }

  END_OF_MED(LOC);
}

/*--------------------- RDONLY PART -------------------------------*/

template <class T> GENDRIVER * MED_FIELD_RDONLY_DRIVER22<T>::copy(void) const
{
  return new MED_FIELD_RDONLY_DRIVER22<T>(*this);
}

/*!
  In MEDMEM, FIELDs lie on support which can be plain SUPPORT, FAMILY
  or GROUP, while in MED-file, there is no link between the FAMILY and
  GROUP notions and the FIELDs. FIELDs lie on profiles.
  The problem arises from the fact that the MED write driver creates
  profiles when treating fields that lie on MEDMEM::SUPPORT,
  MEDMEM_FAMILY or MEDMEM::GROUP. The profile is named after the
  support name : nameOfSupport_<type_of_geometric_entity>.
  However, the read driver is unable to link supports and profiles
  and it recreates a new support that corresponds to the field profile.

  To avoid this support recreation, pass the mesh to the FIELD's
  constructor, and the field driver will find appropriate FAMILY or GROUP
  in the mesh and use it for the field.
 */
template <class T> void MED_FIELD_RDONLY_DRIVER22<T>::read(void)
  throw (MEDEXCEPTION)
{
  const char * LOC = " MED_FIELD_RDONLY_DRIVER22::read() " ;
  BEGIN_OF_MED(LOC);

  typedef typename MEDMEM_ArrayInterface<T,NoInterlace,NoGauss>::Array       ArrayNo;
  typedef typename MEDMEM_ArrayInterface<T,NoInterlace,Gauss>::Array         ArrayNoWg;
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,NoGauss>::Array     ArrayFull;
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,Gauss>::Array       ArrayFullWg;
  typedef typename MEDMEM_ArrayInterface<T,NoInterlaceByType,NoGauss>::Array ArrayByType;
  typedef typename MEDMEM_ArrayInterface<T,NoInterlaceByType,Gauss>::Array   ArrayByTypeWg;

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
      SCRUTE_MED(MED_FIELD_DRIVER<T>::_fieldName.size());
      SCRUTE_MED(MED_TAILLE_NOM);

//       throw MEDEXCEPTION(LOCALIZED(STRING(LOC)
// 				   <<" <fieldName> size in object driver FIELD is > MED_TAILLE_NOM ."));

      MESSAGE_MED(LOC << "Warning <fieldName> size in object driver FIELD is > MED_TAILLE_NOM .");
    }

  const string & fieldName = MED_FIELD_DRIVER<T>::_fieldName;

  MED_EN::medModeSwitch interlacingType = MED_FIELD_DRIVER<T>::_ptrField->getInterlacingType();
  bool isFullInterlace     = ( interlacingType == MED_EN::MED_FULL_INTERLACE );
  bool isNoInterlaceByType = ( interlacingType == MED_EN::MED_NO_INTERLACE_BY_TYPE );//PAL17011

  MESSAGE_MED("###### "<<LOC<<" fieldNameDRIVER : "<< fieldName << " fieldName : "<< MED_FIELD_DRIVER<T>::_ptrField->_name);

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


  char tmpFieldName[MED_TAILLE_NOM+1] ;
  int err ;
  int    numberOfComponents          = 0;
  char * componentName               = (char *) MED_NULL;
  char * unitName                    = (char *) MED_NULL;
  med_2_3::med_type_champ type ;
  med_2_3::med_idt id = MED_FIELD_DRIVER22<T>::_medIdt;
  bool needConversionToDouble = false,needConversionToInt64 = false;

  // we search for the "field med number" of <fieldName>
  // Having found <fieldName>, variables <numberOfComponents>,
  // <componentName>, <unitname>, <type> and attribute <_fieldNum> are set.
  if (MED_FIELD_DRIVER<T>::_fieldNum==MED_INVALID)
    {
      int numberOfFields = med_2_3::MEDnChamp(id,0) ;
      if ( numberOfFields <= 0 )
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": There is no field found in the file !"));

      for (int i=1;i<=numberOfFields;i++)
	{
	  numberOfComponents = med_2_3::MEDnChamp(id,i) ;

	  if ( numberOfComponents <= 0 )
	    MESSAGE_MED(LOC<<"Be careful there is no compound for field nÂ°"<<i<<"in file |"<<MED_FIELD_DRIVER<T>::_fileName<<"| !");

	  componentName = new char[numberOfComponents*MED_TAILLE_PNOM22+1] ;
	  unitName      = new char[numberOfComponents*MED_TAILLE_PNOM22+1] ;

	  err = med_2_3::MEDchampInfo(id, i, tmpFieldName, &type, componentName,
				      unitName, numberOfComponents) ;

	  MESSAGE_MED("Field "<<i<<" : #" << tmpFieldName <<"# et recherche #"<<fieldName.c_str()<<"#");
	  if ( !strcmp(tmpFieldName,fieldName.c_str()) ) {
	    MESSAGE_MED("FOUND FIELD "<< tmpFieldName <<" : "<<i);
	    MED_FIELD_DRIVER<T>::_fieldNum = i ;
	    break ;
	  }
	  // not found : release memory and search next field !
	  delete[] componentName ;
	  delete[] unitName ;
	}
    }

  //delete[] tmpFieldName ;

  // Si aucun champ ne correspond les variables <componentName> et <unitName> ont été correctement
  // désallouées dans la boucle de recherche
  if (MED_FIELD_DRIVER<T>::_fieldNum==MED_INVALID)
    throw MEDEXCEPTION(LOCALIZED( STRING(LOC) << ": Field "<<  fieldName
				   << " not found in file " << MED_FIELD_DRIVER<T>::_fileName) );

  MESSAGE_MED ("FieldNum : "<<MED_FIELD_DRIVER<T>::_fieldNum);

  if (numberOfComponents < 1) {
    delete[] componentName; delete[] unitName;
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" no component found for field "
				 << fieldName)) ;
  }

  // Verifie que l'on essaye pas de lire un champ double dans un FIELD<int>
  switch ( (med_2_3::med_type_champ) MED_FIELD_DRIVER<T>::_ptrField->_valueType ) {
  case  med_2_3::MED_INT :
  case  med_2_3::MED_INT32 :
  case  med_2_3::MED_INT64 :
    if ( type == ( med_2_3::MED_FLOAT64 ) ) {
      delete[] componentName; delete[] unitName;
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Field Type in file (" << type
				   <<") differs from FIELD object type (" <<
				   MED_FIELD_DRIVER<T>::_ptrField->_valueType << ")" )) ;
    }
#if defined(IRIX64) || defined(OSF1) ||defined(VPP5000) || defined(PCLINUX64)
    if (MED_FIELD_DRIVER<T>::_ptrField->_valueType==MED_EN::MED_INT32 )
      needConversionToInt64=true;
#endif
    break;
  case med_2_3::MED_FLOAT64 :
    if (type != med_2_3::MED_FLOAT64)
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
  SUPPORT *   mySupport = new SUPPORT();
  vector<int> numberOfElementsOfTypeC;
  vector<int> numberOfGaussPoint;
  int         totalNumberOfElWg=0;
  MED_EN::medEntityMesh fieldMedFileEntity;

  bool found = createFieldSupportPart1(id,fieldName,
				       MED_FIELD_DRIVER<T>::_ptrField->_iterationNumber,
				       MED_FIELD_DRIVER<T>::_ptrField->_orderNumber,
				       *mySupport, meshName,
				       numberOfElementsOfTypeC, numberOfGaussPoint,
                                       totalNumberOfElWg, fieldMedFileEntity);


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


  //int mesh_dim = MED_FIELD_DRIVER22<T>::getMeshDimensionFromFile(id,meshName);

  MED_EN::medEntityMesh entityType = mySupport->getEntity();
  //Si un SUPPORT était donné, récupère son nom, sa description et
  //	 le pointeur du maillage associé
  if (! haveSupport)
    meshName = mySupport->getMeshName();
  else {
    // for bug 19782. Entity of support in field was set by med driver and was taken
    // from the file without any analysis. It can differ from entity the support will
    // have in MEDMEM.
//     if ( entityType != MED_FIELD_DRIVER<T>::_ptrField->getSupport()->getEntity() ) {
//       delete mySupport; delete[] componentName; delete[] unitName;
//       MED_FIELD_DRIVER<T>::_fieldNum = MED_INVALID ;
//       throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Given entity |"
// 				   << MED_EN::entNames[MED_FIELD_DRIVER<T>::_ptrField->
// 						       getSupport()->getEntity()]
// 				   << "| for field |"
// 				   << fieldName
// 				   << "| with (it,or) = ("
// 				   << MED_FIELD_DRIVER<T>::_ptrField->_iterationNumber << ","
// 				   << MED_FIELD_DRIVER<T>::_ptrField->_orderNumber << "), on mesh "
// 				   << meshName << "| differs from found entity |"
// 				   << MED_EN::entNames[entityType] << "|."
// 				   ));
//     }
    if ( entityType == MED_FIELD_DRIVER<T>::_ptrField->getSupport()->getEntity() )
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

  int fileHasMesh = ( med_2_3::MEDdimLire(id, const_cast<char *>(meshName.c_str())) > 0);
  vector< MED_EN::medGeometryElement >  meshGeoType;
  vector< int >  meshNbOfElOfType;
  vector< int >  meshNbOfElOfTypeC;
  // Si le maillage n'est pas trouvé les tableaux renvoyés sont vides
  if (fileHasMesh)
  {
//       MED_EN::medEntityMesh entityTypeLoc = entityType;
//       if (entityType == MED_EN::MED_FACE || entityType == MED_EN::MED_EDGE) entityTypeLoc = MED_EN::MED_CELL;

    this->getMeshGeometricTypeFromFile(id,meshName,entityType,meshGeoType,
                                       meshNbOfElOfType,meshNbOfElOfTypeC);
  }

  SCRUTE_MED(meshGeoType.size());
  SCRUTE_MED(MESHgeoType.size());
  SCRUTE_MED(meshNbOfElOfTypeC.size());
  SCRUTE_MED(MESHnbOfElOfTypeC.size());

  if (meshGeoType.size() != MESHgeoType.size())
    {
      for (int i = 0; i<meshGeoType.size();i++)
	MESSAGE_MED("debug meshGeotype " << meshGeoType[i]);

      for (int i = 0; i<MESHgeoType.size();i++)
	MESSAGE_MED("debug MESHgeoType. " << MESHgeoType[i]);
    }

  if (meshNbOfElOfTypeC.size() == MESHnbOfElOfTypeC.size())
    {
      for (int i = 0; i<meshNbOfElOfTypeC.size();i++)
	MESSAGE_MED("debug meshNbOfElOfTypeC " << meshNbOfElOfTypeC[i]);

      for (int i = 0; i<MESHnbOfElOfTypeC.size();i++)
	MESSAGE_MED("debug MESHnbOfElOfTypeC " << MESHnbOfElOfTypeC[i]);
    }

  if (fileHasMesh && haveSupport )
    if ( ( meshGeoType != MESHgeoType ) || (meshNbOfElOfTypeC != MESHnbOfElOfTypeC) )
      {
	MESSAGE_MED("Warning MedField driver 21 while getting mesh information from file for FIELD "<< fieldName
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

  // Test si le Support du Champ repose ou non sur toutes les entités géométriques
  // du maillage associé et positionne ou non l'attribut onAll du SUPPORT.
  // Il ne s'agit pas de la gestion des profils
  vector < MED_EN::medGeometryElement > v1(  mySupport->getTypes(),
					     mySupport->getTypes()+mySupport->getNumberOfTypes() );
  vector<int> v2(numberOfElementsOfTypeC.size());
  transform(numberOfElementsOfTypeC.begin(),
	    numberOfElementsOfTypeC.end(),v2.begin(), bind2nd(plus<int>(),1));

  if ( ( meshGeoType != v1 )  || meshNbOfElOfTypeC != v2 ) {
    // ATTENTION : mySupport->setAll(false);
    // Pb : On a envie de positionner onAll à faux si le champ n'est pas défini sur tous les
    //      types géométriques du maillage associé.
    //      Mais si onAll est false et si aucun profil n'est détecté par la suite,
    //      l'attribut SUPPORT->_number est censé être positionné quand même ! Que faire ?
    // Si on veut être compatible avec la signification première de onAll,
    //  il faudrait créer des profils contenant toutes les entités pour chaque type géométrique
    //  du SUPPORT  mais d'une part c'est dommage d'un point de vue de l'encombrement mémoire
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

  MED_FIELD_DRIVER<T>::_ptrField->_name                   = healName( fieldName );
  MED_FIELD_DRIVER<T>::_ptrField->_numberOfComponents     = numberOfComponents ;
  //MED_FIELD_DRIVER<T>::_ptrField->_componentsTypes        = new int   [numberOfComponents] ;
  //MED_FIELD_DRIVER<T>::_ptrField->_componentsNames        = new string[numberOfComponents] ;
  //MED_FIELD_DRIVER<T>::_ptrField->_componentsUnits        = new UNIT  [numberOfComponents] ;
  //MED_FIELD_DRIVER<T>::_ptrField->_componentsDescriptions = new string[numberOfComponents] ;
  //MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits     = new string[numberOfComponents] ;
  MED_FIELD_DRIVER<T>::_ptrField->_componentsTypes.resize(numberOfComponents);
  MED_FIELD_DRIVER<T>::_ptrField->_componentsNames.resize(numberOfComponents);
  MED_FIELD_DRIVER<T>::_ptrField->_componentsUnits.resize(numberOfComponents);
  MED_FIELD_DRIVER<T>::_ptrField->_componentsDescriptions.resize(numberOfComponents);
  MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits.resize(numberOfComponents);
  for (int i=0; i<numberOfComponents; i++) {
      MED_FIELD_DRIVER<T>::_ptrField->_componentsTypes[i]    = 1 ;
      MED_FIELD_DRIVER<T>::_ptrField->_componentsNames[i]    = string(componentName+i*MED_TAILLE_PNOM22,MED_TAILLE_PNOM22) ;
      MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits[i] = string(unitName+i*MED_TAILLE_PNOM22,MED_TAILLE_PNOM22) ;
      SCRUTE_MED(MED_FIELD_DRIVER<T>::_ptrField->_componentsNames[i]);
      SCRUTE_MED(MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits[i]);
  }
  delete[] componentName;
  delete[] unitName;

  int NumberOfTypes                       = mySupport->getNumberOfTypes() ;
  const MED_EN::medGeometryElement *types = mySupport->getTypes() ;
  T * myValues = new T[totalNumberOfElWg*numberOfComponents];
  const int * nbOfElOfType = mySupport->getNumberOfElements() ;
  bool anyProfil = false;
  int  pflSize=0,index=0;
  // Le vecteur de profil est dimensionné par rapport aux nombres de types
  // géométriques du champ même si le champ n'a pas de profil MED FICHIER sur
  // tous ses types géométriques car dans MEDMEM si onAllElement 
  // du SUPPORT est false il faut positionner un profil pour tous les types géométriques 
  // du SUPPORT
  int profilSizeC = 0;
  vector < int   >                     profilSize    (NumberOfTypes,0);
  vector < string >                    profilNameList(NumberOfTypes);
  vector < vector<med_2_3::med_int>  > profilList    (NumberOfTypes);      // IPAL13481
  vector < vector<med_2_3::med_int>  > profilListFromFile (NumberOfTypes); // IPAL13481
  char *                               profilName = new char[MED_TAILLE_NOM+1];

  MESSAGE_MED ("NumberOfTypes      : "<< NumberOfTypes);
  MED_FIELD_DRIVER<T>::_ptrField->_numberOfValues=0 ;
 
  // PAL16681 (Read no interlace field from file) ->
  // use medModeSwitch of a field in MEDMEMchampLire() if there is one geometric type
  // to exclude array conversion
  med_2_3::med_mode_switch modswt = med_2_3::MED_FULL_INTERLACE;
  // NOTE: field can be either of 3 medModeSwitch'es, MED_NO_INTERLACE_BY_TYPE added (PAL17011)
  if ( NumberOfTypes == 1 && !isFullInterlace || isNoInterlaceByType )
    modswt = med_2_3::MED_NO_INTERLACE;

  for (int typeNo=0; typeNo<NumberOfTypes; typeNo++) {

    int numberOfValuesWc= nbOfElOfType[typeNo]*numberOfGaussPoint[typeNo+1]*numberOfComponents;
    char * gaussModelName = new char[MED_TAILLE_NOM+1];

    MESSAGE_MED ("FIELD_NAME         : "<< fieldName.c_str());
    MESSAGE_MED ("MESH_NAME          : "<< meshName.c_str());
    MESSAGE_MED ("MED_ENTITE         : "<< MED_EN::entNames[entityType]);
    MESSAGE_MED ("MED_GEOM           : "<< MED_EN::geoNames[types[typeNo]]);
    MESSAGE_MED ("Iteration          : "<< MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber());
    MESSAGE_MED ("Order              : "<< MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber());
    MESSAGE_MED ("Time               : "<< MED_FIELD_DRIVER<T>::_ptrField->getTime());
    MESSAGE_MED ("NumberOfElements   : "<< nbOfElOfType[typeNo]);
    MESSAGE_MED ("NumberOfComponents : "<< numberOfComponents);
    MESSAGE_MED ("NumberOfGaussPts   : "<< numberOfGaussPoint[typeNo+1]);
    MESSAGE_MED ("NumberOfValuesWg   : "<< nbOfElOfType[typeNo]*numberOfGaussPoint[typeNo+1]);
    MESSAGE_MED ("NumberOfValuesWgWc : "<< numberOfValuesWc);
    MESSAGE_MED ("Index              : "<< index);
    med_2_3::med_err ret=-1;

    med_2_3::med_int * myValuesTmp=0;
    unsigned char* ptrTmp=0;
    if (needConversionToDouble || needConversionToInt64 ) {
      myValuesTmp = new med_2_3::med_int[numberOfValuesWc];
      ptrTmp = (unsigned char*) myValuesTmp;
    } else
      ptrTmp = (unsigned char*) &myValues[index];

    //VERIFIER LE NBRE
//     med_2_3::med_entite_maillage medfile_entity;
//     if (entityType==MED_NODE) 
//       medfile_entity= (med_2_3::med_entite_maillage)MED_NODE;
//     else 
//       medfile_entity= (med_2_3::med_entite_maillage)MED_CELL;
    ret=med_2_3::MEDchampLire(id,const_cast <char*> (meshName.c_str() ),
                              const_cast <char*> (fieldName.c_str()),
                              (unsigned char*) ptrTmp,
                              modswt /*med_2_3::MED_FULL_INTERLACE*/, // PAL16681,17011
                              MED_ALL,
                              gaussModelName,
                              profilName,
                              med_2_3::MED_COMPACT,
                              (med_2_3::med_entite_maillage) fieldMedFileEntity,
                              (med_2_3::med_geometrie_element)types[typeNo],
                              MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber(),
                              MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber()
                              );

      if (needConversionToDouble || needConversionToInt64 ) {

      if (needConversionToInt64 ) //utiliser un trait
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
	//delete[] MED_FIELD_DRIVER<T>::_ptrField->_componentsTypes ;
	//delete[] MED_FIELD_DRIVER<T>::_ptrField->_componentsNames ;
	//delete[] MED_FIELD_DRIVER<T>::_ptrField->_componentsUnits ;
	//delete[] MED_FIELD_DRIVER<T>::_ptrField->_componentsDescriptions ;
	//delete[] MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits ;
	//MED_FIELD_DRIVER<T>::_ptrField->_componentsTypes = NULL ;
	//MED_FIELD_DRIVER<T>::_ptrField->_componentsNames = NULL ;
	//MED_FIELD_DRIVER<T>::_ptrField->_componentsUnits = NULL ;
	//MED_FIELD_DRIVER<T>::_ptrField->_componentsDescriptions = NULL ;
	//MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits = NULL ;
	MED_FIELD_DRIVER<T>::_ptrField->_componentsTypes.clear();
	MED_FIELD_DRIVER<T>::_ptrField->_componentsNames.clear();
	MED_FIELD_DRIVER<T>::_ptrField->_componentsUnits.clear();
	MED_FIELD_DRIVER<T>::_ptrField->_componentsDescriptions.clear();
	MED_FIELD_DRIVER<T>::_ptrField->_MEDComponentsUnits.clear();
	MED_FIELD_DRIVER<T>::_fieldNum = MED_INVALID ; // we have not found right field, so reset the field number
	throw MEDEXCEPTION( LOCALIZED( STRING(LOC) <<": ERROR while reading values")) ;
      }

    index += numberOfValuesWc;
    // Le support prend en compte le nombre de valeurs lié aux profils
    MED_FIELD_DRIVER<T>::_ptrField->_numberOfValues+=
      nbOfElOfType[typeNo];// Ne doit pas prendre en compte les points de Gauss

    // second et troisième test lié à un bug medfichier
    if ( strcmp(gaussModelName,MED_NOGAUSS) && strcmp(gaussModelName,string(MED_TAILLE_NOM,' ').c_str() )
	 && strcmp(gaussModelName,string(16,' ').c_str() )  ) {
 
	int type_geo = (int) types[typeNo];
	int t1       = (type_geo%100)*(type_geo/100);
	int ngauss   = numberOfGaussPoint[typeNo+1];
	int t2       = ngauss*(type_geo/100);
	med_2_3::med_float * refcoo = new med_2_3::med_float[t1];
	med_2_3::med_float * gscoo  = new med_2_3::med_float[t2];
	med_2_3::med_float * wg     = new med_2_3::med_float[ngauss];

	if (MEDgaussLire(id, refcoo, gscoo, wg, modswt, gaussModelName ) < 0)
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
      pflSize = med_2_3::MEDnValProfil(id,profilName);
      if ( pflSize  <= 0)
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Error while reading the profil size of |"
				     << profilName << "|" ));

      profilSize[typeNo]=pflSize;
      profilList[typeNo].resize(pflSize);
      profilListFromFile[typeNo].resize(pflSize);
      ret = med_2_3::MEDprofilLire(id,&profilList[typeNo][0],profilName); // cf item 16 Effective STL // IPAL13481
      profilListFromFile[typeNo] = profilList[typeNo];
      profilNameList[typeNo]= healName(profilName);
    }
  }

  delete[] profilName;

  //MESSAGE_MED ("Index              : "<< index);
  assert(index == totalNumberOfElWg*numberOfComponents);
  assert(MED_FIELD_DRIVER<T>::_ptrField->_numberOfValues == mySupport->getNumberOfElements(MED_ALL_ELEMENTS));

  if (anyProfil)
  {
    for (int typeNo=0; typeNo < NumberOfTypes; typeNo++)
    {
      //MED_FILE uses MED_NONE as a geometricType to describe MED_NODE
      //MEDMEM uses MED_POINT1
      MED_EN::medGeometryElement geomType = types[typeNo];
      if (geomType == MED_EN::MED_NONE)
        geomType = MED_EN::MED_POINT1;

      // Trouve l'index du type géométrique dans la liste des types géométriques du maillage
      // correspondant au type géométrique du champ traité
      vector<MED_EN::medGeometryElement>::iterator meshTypeNoIt =
	find(meshGeoType.begin(),meshGeoType.end(),geomType); //Gérer l'exception
      if (meshTypeNoIt ==  meshGeoType.end())
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<": Can't find "<< MED_EN::geoNames[geomType]
                                     << " on entity " << MED_EN::entNames[entityType]
                                     << " in geometric type list of mesh " << meshName));
      int meshTypeNo = meshTypeNoIt - meshGeoType.begin();

      if (! profilList[typeNo].empty() )
      {
// 	for (int j =0 ; j< meshGeoType.size();++j)
// 	  cout << "--MeshTypeNo : "<<meshTypeNo<<"-> meshNbOfElOfTypeC["<<j<<"]="<<meshNbOfElOfTypeC[j]<<endl;
// 	cout << "--typeNo--" << typeNo << endl;
// 	cout << "meshNbOfElOfTypeC["<<meshTypeNo<<"]=" << meshNbOfElOfTypeC[meshTypeNo] <<endl;

	// Transformer les numéros locaux d'entités medfichier en numéro global medmémoire
        for (int i = 0; i < profilList[typeNo].size(); i++) {
          // Les numéros des entités commencent à 1 dans MEDfichier comme dans MEDmémoire
          // meshNbOfElOfTypeC[0]=0 ...meshNbOfEltOfTypeC[meshTypeNo]=
          // meshNbOfElOfTypeC[meshTypeNo-1]+nbrOfElem of meshTypeNo type
          // rem1 : Si le meshTypeNo trouvé est 0 (premier type géométrique du maillage
          // il ne faut pas décaler les numéros du profils qui commencent à 1 dans MEDFICHIER
          // rem2 : meshNbOfElOfTypeC[NumberOfTypes] ne devrait jamais être utilisé
          profilList[typeNo][i]+=meshNbOfElOfTypeC[meshTypeNo];
	}
      } else {
	// Créer le profil <MED_ALL> pour ce type géométrique
	// uniquement pour renseigner le tableau skyline avec des accesseurs directs
	// par type géométriques
	// REM : Une conséquence est qu'à la réecriture le fichier contiendra des
	// profils sur certains types géométriques alors qu'à la lecture il n'y en avait pas !
	// Solution : Stocker les noms des profils et les utiliser pour savoir si il y avait ou non
	//            un profil
        int pflSize = meshNbOfElOfType[meshTypeNo];
	// profil    = new int[pflSize];

	profilList[typeNo].resize(pflSize);
	profilSize[typeNo] = pflSize;

	for (int j = 1; j <= pflSize; j++) {
          profilList[typeNo][j-1] = meshNbOfElOfTypeC[meshTypeNo] + j ; // index MEDMEM commence à 1
	}
	profilNameList[typeNo] = MED_NOPFL; //Information a utiliser pour la sauvegarde : PLUTOT MED_ALL
      }
      profilSizeC += profilList[typeNo].size();
    }

    MEDSKYLINEARRAY * skyLine = new MEDSKYLINEARRAY(profilList.size(), profilSizeC );
    vector<int> index(NumberOfTypes+1,0);
    index[0]=1;
    for( int typeNo=0; typeNo < NumberOfTypes; typeNo++ )
      index[typeNo+1]=index[typeNo]+profilSize[typeNo];
    skyLine->setIndex(&index[0]);
    for (int i=1; i <= profilList.size() ; i++) {
      vector<int> aTmp(profilList[i-1].size()); // IPAL13481
      for (int j=0; j < profilList[i-1].size(); j++)
	aTmp[j] = (int) profilList[i-1][j];
      skyLine->setI(i,&aTmp[0]);
      //skyLine->setI(i,&profilList[i-1][0]);
    }

    MEDSKYLINEARRAY * skyLineFromFile = new MEDSKYLINEARRAY(profilListFromFile.size(), profilSizeC );
    skyLineFromFile->setIndex(&index[0]);
    for (int i=1; i <= profilListFromFile.size() ; i++) {
      vector<int> aTmp(profilListFromFile[i-1].size()); // IPAL13481
      for (int j=0; j < profilListFromFile[i-1].size(); j++)
	aTmp[j] = (int) profilListFromFile[i-1][j];
      skyLineFromFile->setI(i,&aTmp[0]);
      //skyLineFromFile->setI(i,&profilListFromFile[i-1][0]);
    }

    mySupport->setAll(false);
    mySupport->setpartial(skyLine,true);
    mySupport->setpartial_fromfile(skyLineFromFile,true);
    mySupport->setProfilNames(profilNameList);
//    cout << "Valeurs du skyline du SUPPORT partiel crée : " << *skyLine << endl;
  }

  // Créer un driver spécifique pour les modes MED_FULL_INTERLACE et MED_NO_INTERLACE
  // serait plus efficace.
  bool anyGauss = (numberOfGaussPoint != vector<int>(numberOfGaussPoint.size(),1));
  SCRUTE_MED(anyGauss);
  MEDMEM_Array_ * Values;
  if (anyGauss) {
    SCRUTE_MED(mySupport->getNumberOfElements(MED_ALL_ELEMENTS) );
    SCRUTE_MED(NumberOfTypes);
    SCRUTE_MED(numberOfElementsOfTypeC[NumberOfTypes]-1);
    assert(mySupport->getNumberOfElements(MED_ALL_ELEMENTS) == (numberOfElementsOfTypeC[NumberOfTypes]-1) );
    // PAL16681. If NumberOfTypes == 1 then myValues is what should be
    // in a field value, inspite of InterlacingType
    if ( NumberOfTypes == 1 && modswt == med_2_3::MED_NO_INTERLACE )
      Values = new ArrayNoWg(myValues,
                             numberOfComponents,
                             numberOfElementsOfTypeC[NumberOfTypes]-1,
                             NumberOfTypes,
                             &numberOfElementsOfTypeC[0],
                             &numberOfGaussPoint[0],
                             true,true);
    else if ( isNoInterlaceByType ) // PAL17011 (MEDMEM : no_interlace_by_type fields)
      Values = new ArrayByTypeWg(myValues,
                                 numberOfComponents,
                                 numberOfElementsOfTypeC[NumberOfTypes]-1,
                                 NumberOfTypes,
                                 &numberOfElementsOfTypeC[0],
                                 &numberOfGaussPoint[0],
                                 true,true);
    else
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
//     cout << "Valeurs du ArrayFullWg crée : " << endl <<
//       *(static_cast<ArrayFullWg*>(Values))  << endl;
  }
  else {
    // PAL16681. If NumberOfTypes == 1 then myValues is what should be
    // in a field value, inspite of InterlacingType
    if ( NumberOfTypes == 1 && interlacingType == MED_EN::MED_NO_INTERLACE )
      Values = new ArrayNo(myValues,numberOfComponents,totalNumberOfElWg,
                           true,true);
    else if ( isNoInterlaceByType ) // PAL17011 (MEDMEM : no_interlace_by_type fields)
      Values = new ArrayByType(myValues,numberOfComponents,totalNumberOfElWg,
                               NumberOfTypes, &numberOfElementsOfTypeC[0], true,true);
    else
      Values = new ArrayFull(myValues,numberOfComponents,totalNumberOfElWg,
                             true,true);
  }
  if (MED_FIELD_DRIVER<T>::_ptrField->_value != NULL)
    delete MED_FIELD_DRIVER<T>::_ptrField->_value;

  if ( NumberOfTypes != 1 &&  // PAL16681
       interlacingType == MED_EN::MED_NO_INTERLACE )
  {
    // Convert MED_FULL_INTERLACE -> MED_NO_INTERLACE
    if (Values->getGaussPresence())
      MED_FIELD_DRIVER<T>::_ptrField->_value=ArrayConvert(*static_cast<ArrayFullWg*>(Values));
    else
      MED_FIELD_DRIVER<T>::_ptrField->_value=ArrayConvert(*static_cast<ArrayFull*  >(Values));
    delete Values;
  }
  else
  {
    MED_FIELD_DRIVER<T>::_ptrField->_value=Values;
  }

  MED_FIELD_DRIVER<T>::_ptrField->_isRead = true ;

  bool isFound = false;
  MESH* aMesh = MED_FIELD_DRIVER<T>::_ptrField->_mesh;
  if (!haveSupport && aMesh && anyProfil)
  {
    int it = -1;
    for (int typeNo = 0; (typeNo < NumberOfTypes) && (it == -1); typeNo++) {
      if (strcmp(profilNameList[typeNo].c_str(), MED_NOPFL) != 0)
        it = typeNo;
    }
    // IMP 0019953: link between fields and families for MED 2.2 read driver
    string aPN = profilNameList[it];
    MED_EN::medGeometryElement aPT = types[it];

    ostringstream typestr;
    typestr << "_type" << aPT;
    string aSuff = typestr.str();

    //- If the field profile name is toto_PFL and a family toto exists,
    //  the field will point to the corresponding FAMILY object.
    const vector<FAMILY*> aFams = aMesh->getFamilies(entityType);
    for (int fi = 0; fi < aFams.size() && !isFound; fi++) {
      FAMILY* aF = aFams[fi];
      string aFN_suff = aF->getName() + aSuff;
      if (aPN == aFN_suff) {
        isFound = true;
        //family found
        MED_FIELD_DRIVER<T>::_ptrField->_support = aF; //Prévenir l'utilisateur ?
      }
    }
    if (!isFound) {
      // - If no family was found, lookup the groups and if a group toto
      //   exists, the field will point to the corresponding GROUP object.
      const vector<GROUP*> aGrps = aMesh->getGroups(entityType);
      for (int gi = 0; gi < aGrps.size() && !isFound; gi++) {
        GROUP* aG = aGrps[gi];
        string aGN_suff = aG->getName() + aSuff;
        if (aPN == aGN_suff) {
          isFound = true;
          //group found
          MED_FIELD_DRIVER<T>::_ptrField->_support = aG; //Prévenir l'utilisateur ?
        }
      }
    }
    if (!isFound) {
      // - If no family or group was found and the
      //   profile name is xxx_PFL, throw an exception
      int pos = aPN.rfind(aSuff);
      if (pos + aSuff.length() - 1 == aPN.length())
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)
                                     << ": Can't find appropriate support (GROUP or FAMILY)"
                                     << " in mesh " << meshName << " for field " << fieldName
                                     << ", while one of its profiles " << aPN
                                     << " was generated from a FAMILY or a GROUP"));
    }
    else {
      // - Check that the found support has correct types
      //   and number of elements. If not, throw an exception
      const SUPPORT* aSupp = MED_FIELD_DRIVER<T>::_ptrField->_support;
      if (aSupp->getNumberOfTypes() != NumberOfTypes)
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << ": Invalid support (GROUP or FAMILY) found in mesh "
                                     << meshName << " for field " << fieldName << " by name of profile "
                                     << aPN << ": different number of types in found support |"
                                     << aSupp->getNumberOfTypes() << "| and in required |"
                                     << NumberOfTypes << "|"));

      const MED_EN::medGeometryElement* aTypes = aSupp->getTypes();
      for (int it = 0; it < NumberOfTypes && isFound; it++)
      {
        MED_EN::medGeometryElement aType = aTypes[it];
        if (aType != types[it])
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << ": Invalid support (GROUP or FAMILY) found in mesh "
                                       << meshName << " for field " << fieldName << " by name of profile "
                                       << aPN << ": geometric type in found support |" << aType
                                       << "| differs from required type |" << types[it] << "|"));

        if (aSupp->getNumberOfElements(aType) != nbOfElOfType[it])
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << ": Invalid support (GROUP or FAMILY) found in mesh "
                                       << meshName << " for field " << fieldName << " by name of profile "
                                       << aPN << ": number of elements of type " << aType
                                       << " in found support |" << aSupp->getNumberOfElements(aType)
                                       << "| differs from required |" << nbOfElOfType[it] << "|"));
      }
    }
  }

  if (!isFound) {
    // No corresponding support (family or group)
    // found in the mesh, use the newly created one
    MED_FIELD_DRIVER<T>::_ptrField->_support = mySupport; //Prévenir l'utilisateur ?
  }

  END_OF_MED(LOC);
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
  BEGIN_OF_MED(LOC);
  typedef typename MEDMEM_ArrayInterface<T,NoInterlace,NoGauss>::Array   ArrayNo;
  typedef typename MEDMEM_ArrayInterface<T,NoInterlace,Gauss>::Array     ArrayNoWg;
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,NoGauss>::Array ArrayFull;
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,Gauss>::Array   ArrayFullWg;

  typedef map<MED_EN::medGeometryElement,GAUSS_LOCALIZATION<FullInterlace>*> locMapFull;
  typedef map<MED_EN::medGeometryElement,GAUSS_LOCALIZATION<NoInterlace>*>   locMapNo;
  typedef map<MED_EN::medGeometryElement,GAUSS_LOCALIZATION_*>   locMap;

  med_2_3::med_idt id = MED_FIELD_DRIVER22<T>::_medIdt;

  if (MED_FIELD_DRIVER<T>::_status!=MED_OPENED)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": Method open must be called before method write.")) ;

  string fieldName;
  if ( ( MED_FIELD_DRIVER<T>::_fieldName.empty()       ) &&
       ( MED_FIELD_DRIVER<T>::_ptrField->_name.empty() )    )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)
				 <<" neither <fieldName> is set in driver nor in object FIELD.")) ;

  // If _fieldName is not set in driver, try to use _ptrfield->_fieldName
  if ( ( MED_FIELD_DRIVER<T>::_fieldName.empty()       ) &&
       ( !MED_FIELD_DRIVER<T>::_ptrField->_name.empty() )    )
    fieldName = healName( MED_FIELD_DRIVER<T>::_ptrField->_name );
  else
    fieldName = healName( MED_FIELD_DRIVER<T>::_fieldName );

  //if ( ! MED_FIELD_DRIVER<T>::_ptrField->_isRead )
  //  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)
  //			 <<" FIELD |"<<fieldName<<"| was not read but is being written"));

  SCRUTE_MED(fieldName);
  if ( fieldName.size() > MED_TAILLE_NOM ) {
    fieldName.substr(0,MED_TAILLE_NOM);
    MESSAGE_MED( "Be careful <fieldName> size must not be > MED_TAILLE_NOM, using fieldName : |"<< fieldName <<"|." );
  }

  const SUPPORT * mySupport = MED_FIELD_DRIVER<T>::_ptrField->getSupport() ;
  if ( ! mySupport )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)
				 <<" There is no SUPPORT associated with FIELD : "
				 << fieldName << "."));

  bool onAll = mySupport->isOnAllElements();
  const locMap & gaussModel = MED_FIELD_DRIVER<T>::_ptrField->_gaussModel;


  string meshName = healName( mySupport->getMeshName() );
  SCRUTE_MED(meshName);
  if ( meshName.size() > MED_TAILLE_NOM ) {
    meshName = meshName.substr(0,MED_TAILLE_NOM);
    MESSAGE_MED( "Be careful <meshName> size must not be > MED_TAILLE_NOM, using meshName : |"<< meshName <<"|." );
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
  if ( ! listcomponent_name || ! listcomponent_unit )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<" Udefined components of FIELD : "
				 << fieldName << "."));
  int length ;
  for (int i=0; i < component_count ; i++) {
    length = min(MED_TAILLE_PNOM22,(int)listcomponent_name[i].size());
    component_name.replace(i*MED_TAILLE_PNOM22,length,
			   listcomponent_name[i],0,length);
    length = min(MED_TAILLE_PNOM22,(int)listcomponent_unit[i].size());
    component_unit.replace(i*MED_TAILLE_PNOM22,length,
			   listcomponent_unit[i],0,length);
  }

  MESSAGE_MED("using component_name=|"<<component_name<<"|");
  MESSAGE_MED("using component_unit=|"<<component_unit<<"|");

  MED_EN::med_type_champ ValueType=MED_FIELD_DRIVER<T>::_ptrField->getValueType() ;

  MESSAGE_MED("Template Type =|"<<ValueType<<"|");

  // Vérifier si le champ existe déjà
  char   champName[MED_TAILLE_NOM+1];
  char * compName, * compUnit ;
  med_2_3::med_type_champ type ;
  bool Find = false ;
  int n = med_2_3::MEDnChamp(id,0);
  int nbComp = 0;
  for (int i=1; i<=n; i++) {
    nbComp   = med_2_3::MEDnChamp(id,i);
    compName = new char[MED_TAILLE_PNOM22*nbComp+1];
    compUnit = new char[MED_TAILLE_PNOM22*nbComp+1];
    err = med_2_3::MEDchampInfo(id,i,champName,&type,compName,compUnit,nbComp);
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
    SCRUTE_MED(nbComp);
    MESSAGE_MED(LOC<<" Component name in file   : "<<compName);
    MESSAGE_MED(LOC<<" Component name in memory : "<<component_name);
    MESSAGE_MED(LOC<<" Component unit in file   : "<<compUnit);
    MESSAGE_MED(LOC<<" Component unit in memory : "<<component_unit);
    delete[] compName ;
    delete[] compUnit ;

  } else {
    // Verify the field isn't yet created

    string dataGroupName =  "/CHA/";
    dataGroupName        += fieldName;
    MESSAGE_MED(LOC << "|" << dataGroupName << "|" );
    med_2_3::med_idt gid =  H5Gopen(id, dataGroupName.c_str() );

    if ( gid < 0 ) {
      // create field :
      err=med_2_3::MEDchampCr(id,
			      const_cast <char*> (fieldName.c_str()),
			      (med_2_3::med_type_champ) ValueType,
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
  MED_EN::medModeSwitch interlacingType = MED_FIELD_DRIVER<T>::_ptrField->getInterlacingType();
  bool isFullInterlace     = ( interlacingType == MED_EN::MED_FULL_INTERLACE );
  bool isNoInterlaceByType = ( interlacingType == MED_EN::MED_NO_INTERLACE_BY_TYPE );//PAL17011
  med_2_3::med_mode_switch modswt = med_2_3::MED_FULL_INTERLACE;

  if ( isFullInterlace ) {
    myField = MED_FIELD_DRIVER<T>::_ptrField;
  }
  else if ( isNoInterlaceByType ) {
    // PAL17011, no need to convert, that is what this improvement is needed for
    modswt = med_2_3::MED_NO_INTERLACE;
  }
  else {
    myField = FieldConvert( *((FIELD<T,NoInterlace>*) MED_FIELD_DRIVER<T>::_ptrField ));
  }

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
  med_2_3::med_int fileHasMesh=0;

  if (!onAll) {

    number = mySupport->getNumber(MED_ALL_ELEMENTS);
    numberIndex = mySupport->getNumberIndex();
    profilNameList=mySupport->getProfilNames();

    fileHasMesh = ( med_2_3::MEDdimLire(id, const_cast<char *>(meshName.c_str())) > 0);
    MESH * meshPtr = mySupport->getMesh();
//     if(!meshPtr)
//       throw MEDEXCEPTION( LOCALIZED (STRING(LOC)
//                                      <<": Mesh in support is null"
//                                      )
//                           );

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

  //converting MEDMEM type to MEDfile type 
  if (entityType != MED_EN::MED_NODE)
    entityType = MED_EN::MED_CELL;

  // on boucle sur tout les types pour ecrire les tableaux de valeur
  for (int typeNo=0;typeNo<numberOfTypes;typeNo++) {

    int numberOfElements = mySupport->getNumberOfElements(types[typeNo]) ;
    //UP : prend en compte les profils, pas les points de Gauss

    //value = MED_FIELD_DRIVER<T>::_ptrField->getRow(index) ;
    // rem 1 : le getRow du Array est différent de celui du FIELD si le SUPPORT contient
    //         des profils (les indices des valeurs ne se suivent pas forcément)
    // rem 2 : Afin de respecter la norme MEDFICHIER, les indices contenus dans les
    //         profils doivent être croissant
    if (onAll) {

      if ( isNoInterlaceByType ) { //PAL17011
        value = MED_FIELD_DRIVER<T>::_ptrField->getValueByType(typeNo+1);
          //((ArrayNoByType *)MED_FIELD_DRIVER<T>::_ptrField->getArray())->getValueByType(i+1);
      }
      else {
        value = myField->getRow(index);
      }
      profilName=MED_NOPFL;
      numberOfElForMED = numberOfElements;

    } else {

      if ( isNoInterlaceByType ) { //PAL17011
        value = MED_FIELD_DRIVER<T>::_ptrField->getValueByType(typeNo+1);
      }
      else {
        value = myField->getRow(number[index-1]);
      }
      // PAL16854(Partial support on nodes) ->
      //profilName = (profilNameList.size()>typeNo) ? profilNameList[typeNo].substr(0,MED_TAILLE_NOM) : MED_NOPFL;
      if (profilNameList[typeNo].size()>MED_TAILLE_NOM)
        profilName = healName( profilNameList[typeNo].substr(0,MED_TAILLE_NOM) );
      else
        profilName = healName( profilNameList[typeNo] );

      // Rem : Si le SUPPORT n'est pas onAll mais que pour un type géométrique donné le nom
      // du profil associé est MED_NOPFL alors le profil n'est pas écrit dans le fichier MED.
      // Car en MEDMEMOIRE si le champ repose sur des éléments de deux types géométriques
      // différents et est défini sur tous les éléments d'un type géométrique
      // mais pas de l'autre, il existe tout de même des profils sur les deux types géométriques.
      // Ce n'est pas le cas en MEDFICHIER.
      vector<med_2_3::med_int> profil(&number[index-1],&(number[index-1])+numberOfElements);

      int meshTypeNo=0;
      if ( entityType != MED_EN::MED_NODE ) // PAL16854(Partial support on nodes)
      {
        // Trouve l'index du type géométrique dans la liste des types géométriques du maillage
        // correspondant au type géométrique du champ en cours de traitement
        vector<MED_EN::medGeometryElement>::iterator meshTypeNoIt =
          find(meshGeoType.begin(),meshGeoType.end(),types[typeNo]);
        if ( meshTypeNoIt ==  meshGeoType.end() )
          throw MEDEXCEPTION(LOCALIZED( STRING(LOC) <<": Can't find "<< MED_EN::geoNames[types[typeNo]]
                                        << " on entity " << MED_EN::entNames[entityType]
                                        << " in geometric type list of mesh " << meshName
                                        )
                             );
        meshTypeNo = meshTypeNoIt -  meshGeoType.begin();
      }

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

      //REM : Ce n'est pas évident, mais lorsqu'il y a un profil, le nombre de valeurs
      //      que l'on indique à MEDchampEcr est le nombre de valeurs sans profil, d'où
      //      le nombre d'éléments du maillage sur le type géométrique courant.
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

      if ( med_2_3::MEDprofilEcr(id,
				 &profil[0],
				 numberOfElements,
				 const_cast<char *>(profilName.c_str())) < 0)

	throw MEDEXCEPTION(LOCALIZED( STRING(LOC) <<": Error while writing "<< numberOfElements
				      << " values for MED profil "<< profilName
				    )
			 );
    }

    bool anyGauss = MED_FIELD_DRIVER<T>::_ptrField->getGaussPresence();
    string locName=MED_NOGAUSS;
    if ( anyGauss ) {
//       cout << endl << "Nombre de points de Gauss à l'écriture de " << fieldName
// 	   << " pour le type géométrique : " << MED_EN::geoNames[types[typeNo]]
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
	locName=healName( loc.getName() );
	err=med_2_3::MEDgaussEcr(id,
			       (med_2_3::med_geometrie_element) loc.getType(),
			       (med_2_3::med_float *)           loc.getRefCoo().getPtr(),
			                                        med_2_3::MED_FULL_INTERLACE,
			       (med_2_3::med_int)               ngauss,
			       (med_2_3::med_float *)           loc.getGsCoo().getPtr(),
			       (med_2_3::med_float *)           (&loc.getWeight()[0]),
			       const_cast<char *>               (locName.c_str())
			       );
      } else {
	const GAUSS_LOCALIZATION<NoInterlace> & loc=*(static_cast<const GAUSS_LOCALIZATION<NoInterlace> * >(locPtr));
	ngauss = loc.getNbGauss();
	locName=healName( loc.getName() );
	err=med_2_3::MEDgaussEcr(id,
			       (med_2_3::med_geometrie_element) loc.getType(),
			       (med_2_3::med_float *)           loc.getRefCoo().getPtr(),
				                                med_2_3::MED_NO_INTERLACE,
			       (med_2_3::med_int)               ngauss,
			       (med_2_3::med_float *)           loc.getGsCoo().getPtr(),
			       (med_2_3::med_float *)           (&loc.getWeight()[0]),
			       const_cast<char *>               (locName.c_str())
			       );

      };

      if ( err != 0 )
	throw MEDEXCEPTION(LOCALIZED( STRING(LOC) <<": Error while writing Gauss Model for FIELD "<< fieldName
				      << " on entity " << MED_EN::entNames[entityType]
				      << " and geometric type " << MED_EN::geoNames[types[typeNo]] 
			    )
			 );

	//numberOfElForMED *= mySupport->getNumberOfGaussPoints(types[typeNo]); //Deplacer la méthode dans FIELD
	numberOfElForMED *= ngauss;
    }

    MESSAGE_MED("MED_FIELD_DRIVER22<T>::_medIdt                       : "<<id);
    MESSAGE_MED("meshName.c_str()                : "<<meshName.c_str());
    MESSAGE_MED("MED_FIELD_DRIVER<T>::_ptrField->getName()            : "<<MED_FIELD_DRIVER<T>::_ptrField->getName());
    MESSAGE_MED("MED_FIELD_DRIVER<T>::_fieldName                      : "<<MED_FIELD_DRIVER<T>::_fieldName);
    MESSAGE_MED("value                           : "<<value);
    MESSAGE_MED("numberOfElements                : "<<numberOfElements);
    MESSAGE_MED("numberOfElForMED                : "<<numberOfElForMED);
    MESSAGE_MED("entityType                      : "<<MED_EN::entNames[entityType]);
    MESSAGE_MED("types[i]                        : "<<MED_EN::geoNames[types[typeNo]]);
    if (myField) //myField may be NULL (PAL17011)
      MESSAGE_MED("NumberOfGaussPoint[i]           : "<<myField->getNumberOfGaussPoints(types[typeNo]));
    MESSAGE_MED("MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber() : "<<MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber());
    MESSAGE_MED("MED_FIELD_DRIVER<T>::_ptrField->getTime()            : "<<MED_FIELD_DRIVER<T>::_ptrField->getTime());
    MESSAGE_MED("MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber()     : "<<MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber());

    // Rem 1 : le nombre d'éléments passé à MEDchampEcr ne doit pas tenir compte de la taille
    //         des profils : c'est la taille du champ sans profil.
    err=med_2_3::MEDchampEcr(id,
			     const_cast <char*> ( meshName.c_str()) ,
			     const_cast <char*> ( fieldName.c_str()),
			     (unsigned char*)value, modswt,//med_2_3::MED_FULL_INTERLACE,- PAL17011
			     numberOfElForMED,
			     //UP : prend en compte le nombre de points de Gauss mais 
			     //     pas le nombre de composantes
			     const_cast <char*> ( locName.c_str()),
			     MED_ALL,
			     const_cast <char *> (profilName.c_str()), med_2_3::MED_COMPACT,
			     (med_2_3::med_entite_maillage)entityType,
			     (med_2_3::med_geometrie_element)types[typeNo],
			     MED_FIELD_DRIVER<T>::_ptrField->getIterationNumber(),
			     "        ",                     // A FAIRE : IMPLEMENTER L'UNITE DU PAS DE TEMPS!
			     MED_FIELD_DRIVER<T>::_ptrField->getTime(),
			     MED_FIELD_DRIVER<T>::_ptrField->getOrderNumber()
			     );

    if (err < MED_VALID ) {
      if ( !isFullInterlace )
        delete myField;
      throw MEDEXCEPTION(LOCALIZED( STRING(LOC) <<": Error while writing "<< numberOfElements
                                    << " values for FIELD "<< fieldName 
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
  if ( !isFullInterlace ) delete myField;


  END_OF_MED(LOC);
}

/*--------------------- RDWR PART -------------------------------*/

template <class T> GENDRIVER * MED_FIELD_RDWR_DRIVER22<T>::copy(void) const
{
  return new MED_FIELD_RDWR_DRIVER22<T>(*this);
}

template <class T> void MED_FIELD_RDWR_DRIVER22<T>::write(void) const
  throw (MEDEXCEPTION)
{
  const char* LOC = "MED_FIELD_RDWR_DRIVER22::write(void)";
  BEGIN_OF_MED(LOC);
  MED_FIELD_WRONLY_DRIVER22<T>::write();
  END_OF_MED(LOC);
}

template <class T> void MED_FIELD_RDWR_DRIVER22<T>::read (void)
  throw (MEDEXCEPTION)
{
  const char* LOC = "MED_FIELD_RDWR_DRIVER22::read(void)";
  BEGIN_OF_MED(LOC);
  MED_FIELD_RDONLY_DRIVER22<T>::read();
  END_OF_MED(LOC);
}

} //End namespace MEDMEM
/*-----------------------------------------------------------------*/

#endif /* MED_FIELD_DRIVER_HXX */
