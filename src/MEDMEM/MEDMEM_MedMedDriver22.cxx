//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "MEDMEM_Compatibility21_22.hxx"
# include "MEDMEM_MedMedDriver22.hxx"
# include "MEDMEM_MedMeshDriver.hxx"
# include "MEDMEM_DriversDef.hxx"

# include "MEDMEM_Mesh.hxx"
# include "MEDMEM_Grid.hxx"
# include "MEDMEM_Field.hxx"
// EN ATTENDANT L'utilisation de MedFieldDriver.hxx ds Field.hxx
# include "MEDMEM_MedFieldDriver.hxx"
# include "MEDMEM_Med.hxx"
# include "MEDMEM_define.hxx"

using namespace std;
using namespace MED_EN;
using namespace MEDMEM;

MED_MED_DRIVER22::MED_MED_DRIVER22(): _medIdt(MED_INVALID) 
{}

MED_MED_DRIVER22::MED_MED_DRIVER22(const string & fileName,  MED * const ptrMed):
  MED_MED_DRIVER(fileName,ptrMed), _medIdt(MED_INVALID)
{
  //_ptrMed->addDriver(*this); // The specific MED driver id is set within the addDriver method.
}

MED_MED_DRIVER22::MED_MED_DRIVER22(const string & fileName,
                                   MED * const ptrMed,
                                   MED_EN::med_mode_acces accessMode):
  MED_MED_DRIVER(fileName,ptrMed,accessMode), _medIdt(MED_INVALID)
{
}
//REM :  As t'on besoin du champ _status :  _medIdt <-> _status  ?  Oui

MED_MED_DRIVER22::MED_MED_DRIVER22(const MED_MED_DRIVER22 & driver):
  MED_MED_DRIVER(driver), _medIdt(MED_INVALID)
{
}

MED_MED_DRIVER22::~MED_MED_DRIVER22()
{
  MESSAGE_MED("MED_MED_DRIVER22::~MED_MED_DRIVER22() has been destroyed");
}

//  GENDRIVER * MED_MED_DRIVER::copy(void) const
//  {
//    return new MED_MED_DRIVER(*this) ;
//  }
void MED_MED_DRIVER22::read()
{
}
void MED_MED_DRIVER22::readFileStruct()
{
}
void MED_MED_DRIVER22::write() const
{
}
GENDRIVER * MED_MED_DRIVER22::copy(void) const
{
  return new MED_MED_DRIVER22(*this) ;
}
void MED_MED_DRIVER22::writeFrom() const
{
}



void MED_MED_DRIVER22::open()
  throw (MEDEXCEPTION)
{

  const char * LOC ="MED_MED_DRIVER22::open() : ";
  BEGIN_OF_MED(LOC);

  // REFLECHIR SUR CE TEST PAR RAPPORT A L'OUVERTURE/FERMETURE
//    if ( _medIdt != MED_INVALID ) 
//      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
//                                       << "_medIdt is already in use, please close the file |" 
//                                       << _fileName << "| before calling open()"
//                                       )
//                            );   
  
//    if ( _status != MED_CLOSED ) 
//      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
//                                       << "_status is closed, please close the file |"
//                                       << _fileName << "| before calling open()"
//                                       )
//                            );
  
  if ( _fileName == "" )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "_fileName is |\"\"|, please set a correct fileName before calling open()"
                                     )
                          );

  int accessMode = getMedAccessMode( _accessMode, MED_EN::V22 );
  MESSAGE_MED(LOC<<"_fileName.c_str : "<< _fileName.c_str()<<",mode : "<< accessMode);

  _medIdt = med_2_3::MEDouvrir( (const_cast <char *> (_fileName.c_str())), (med_2_3::med_mode_acces) accessMode);
  MESSAGE_MED(LOC<<" _medIdt = "<<_medIdt);
  
  if (_medIdt > 0) 
    _status=MED_OPENED; 
  else {
    _status = MED_CLOSED;
    _medIdt = MED_INVALID;
    throw MED_EXCEPTION (LOCALIZED( STRING(LOC) 
                                    << "Can't open |"  << _fileName 
                                    << "|, _medIdt : " << _medIdt
                                    )
                         );
  }

  END_OF_MED(LOC);
}


void MED_MED_DRIVER22::close()
{
  med_2_3::med_int err = 0;
  
  
//    if ( _status == MED_CLOSED)
//      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << ": the file |" 
//                                       << _fileName << "| is already closed"
//                                       )
//                            );
   
//    if ( _medIdt == MED_INVALID ) 
//      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "_medIdt invalid, but the file |" 
//                                       << _fileName << "| seems to be openned !"
//                                       )
//                            );   

  if ( _medIdt != MED_INVALID )
    err=med_2_3::MEDfermer(_medIdt);
  
//    if (err != MED_VALID) 
//      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "the file |" 
//                                       << _fileName << "| couldn't be closed"
//                                       )
//                            );   
  
  _status = MED_CLOSED;
  _medIdt = MED_INVALID;
    
  //END_OF_MED();
}


// ------------- Read Only Part --------------

MED_MED_RDONLY_DRIVER22::MED_MED_RDONLY_DRIVER22()
{
  this->GENDRIVER::_accessMode = MED_EN::RDONLY;
}

MED_MED_RDONLY_DRIVER22::MED_MED_RDONLY_DRIVER22(const string & fileName,  MED * const ptrMed):
  IMED_MED_RDONLY_DRIVER(fileName,ptrMed),
  MED_MED_DRIVER22(fileName,ptrMed,MED_EN::RDONLY),
  MED_MED_DRIVER(fileName,ptrMed,MED_EN::RDONLY)
{
  MESSAGE_MED("MED_MED_RDONLY_DRIVER22::MED_MED_RDONLY_DRIVER22(const string & fileName,  MED * const ptrMed) Constructeur read only");
}

MED_MED_RDONLY_DRIVER22::MED_MED_RDONLY_DRIVER22(const MED_MED_RDONLY_DRIVER22 & driver):
  IMED_MED_RDONLY_DRIVER(driver) ,MED_MED_DRIVER22(driver),MED_MED_DRIVER(driver)
{
}

MED_MED_RDONLY_DRIVER22::~MED_MED_RDONLY_DRIVER22()
{
  MESSAGE_MED("MED_MED_RDONLY_DRIVER22::~MED_MED_RDONLY_DRIVER22() has been destroyed");
} 

GENDRIVER * MED_MED_RDONLY_DRIVER22::copy(void) const
{
  return new MED_MED_RDONLY_DRIVER22(*this) ;
}

void MED_MED_RDONLY_DRIVER22::write          ( void ) const throw (MEDEXCEPTION)
{
  IMED_MED_RDONLY_DRIVER::write();
}

void MED_MED_RDONLY_DRIVER22::writeFrom      ( void ) const throw (MEDEXCEPTION)
{
  IMED_MED_RDONLY_DRIVER::writeFrom();
}

void MED_MED_RDONLY_DRIVER22::readFileStruct( void ) 
  throw (MEDEXCEPTION)
{
  const char * LOC = "MED_MED_DRIVER22::readFileStruct() : ";
  int          err,i,j;

  // PAL12192
  if ( IMED_MED_RDONLY_DRIVER::_fileStructIsRead )
    return;
  else
    IMED_MED_RDONLY_DRIVER::_fileStructIsRead = true;

  BEGIN_OF_MED(LOC);

  if ( _medIdt == MED_INVALID ) 
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "_medIdt is invalid, please open the file |" 
                                     << _fileName << "| before calling readFileStruct()"
                                     )
                          );   
  // Read number of meshes and their associated name
  {
    int          numberOfMeshes;
    char         meshName[MED_TAILLE_NOM+1]="";
    char         meshDescription[MED_TAILLE_DESC+1]="";
//CCRT    int          meshDim;
    med_2_3::med_int          meshDim;
    med_2_3::med_maillage meshType;

    MESH *       ptrMesh;
    //    MED_MESH_RDWR_DRIVER * ptrDriver; !! UNUSED VARIABLE !!
    
    numberOfMeshes = med_2_3::MEDnMaa(_medIdt) ;
    if ( numberOfMeshes <= 0 ) 
      MESSAGE_MED(LOC << "Be careful there is no mesh in file |"<<_fileName<<"| !");

    MESH_ENTITIES::const_iterator currentEntity; 
    for (i=1;i<=numberOfMeshes;i++)
      {
        //get infoamation on the i^th mesh

        err = MEDmaaInfo(_medIdt, i ,meshName, &meshDim, &meshType,
                         meshDescription) ;

        if (err != MED_VALID) 
          throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << ": can't get information about the mesh n°" << i << " of the file |" << _fileName << "| !"));

        switch (meshType)
          {
          case med_2_3::MED_STRUCTURE:
            MESSAGE_MED(LOC<<": Mesh n°"<< i <<" nammed "<< meshName << " with the description " << meshDescription << " is structured");

            med_2_3::med_type_grille type;

            err = MEDnatureGrilleLire(_medIdt, meshName, &type);

            if (err != MED_VALID)
              throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << ": can't get the nature of the grid which is the mesh n°" << i << " of the file |" << _fileName << "| !"));

            ptrMesh = new GRID((MED_EN::med_grid_type) type);
            break;
          case med_2_3::MED_NON_STRUCTURE:
            MESSAGE_MED(LOC<<": Mesh n°"<< i <<" nammed "<< meshName << " with the description " << meshDescription << " is not structured");

            ptrMesh = new MESH();
            break;
          default:
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Bad file mesh type !"));
          }
      
        bool isAGrid = ptrMesh->getIsAGrid();
      

      MED_EN::med_mode_acces myMode = getAccessMode();
//       MED_MESH_DRIVER * ptrDriver ;
//       switch (myMode) {
//       case MED_EN::MED_LECT: // V2_1->V2_2
//      ptrDriver = new MED_MESH_RDONLY_DRIVER(_fileName, ptrMesh);
//      break ;
//       case MED_EN::MED_REMP: // V2_1->V2_2
//      ptrDriver = new MED_MESH_RDWR_DRIVER(_fileName, ptrMesh);
//      break ;
//       case MED_EN::MED_ECRI: // should never append !! V2_1->V2_2
//      ptrDriver = new MED_MESH_WRONLY_DRIVER(_fileName, ptrMesh);
//      break;
//       default:
//      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Bad file mode access !"));
//       }

      GENDRIVER * ptrDriver =
        DRIVERFACTORY::buildConcreteMedDriverForMesh(_fileName, ptrMesh,
                                                     meshName, myMode, V22);

      ptrDriver->setId       ( getId() );
      ptrDriver->setMeshName ( meshName );
      ptrMesh->addDriver(*ptrDriver);
      delete ptrDriver ;

      if (isAGrid)
        _ptrMed->_meshes[meshName] = (MESH *) ptrMesh;
      else
        _ptrMed->_meshes[meshName] = ptrMesh;

      //        _ptrMed->_meshes[meshName] = ptrMesh;

        ptrMesh->setName( healName( meshName ));

        ptrMesh->setDescription(meshDescription);

        // add by B. Secher for filter module
        ptrMesh->setMeshDimension(meshDim);

        SCRUTE_MED(ptrMesh);

        MESSAGE_MED(LOC<<"is" << (isAGrid ? "" : " NOT") << " a GRID and its name is "<<ptrMesh->getName());

      // we create all global support (for each entity type :
      int index = 0;
        for (currentEntity=meshEntities.begin();currentEntity != meshEntities.end(); currentEntity++) {
        string supportName="SupportOnAll_" ;
          supportName+=entNames[(MED_EN::medEntityMesh)(*currentEntity).first] ;
        SUPPORT* mySupport = new SUPPORT() ;
        mySupport->setName(supportName);
        mySupport->setMesh(ptrMesh);
        mySupport->setEntity((MED_EN::medEntityMesh) (*currentEntity).first);
        mySupport->setAll(true);
        (_ptrMed->_support)[meshName][(MED_EN::medEntityMesh)(*currentEntity).first] = mySupport ;
        MESSAGE_MED(LOC<< "The support " << supportName.c_str() << " on entity " << (*currentEntity).first << " is built");
        index++;
      }
        MESSAGE_MED(LOC <<"The mesh " <<ptrMesh->getName() << " has " << index << " support(s)");
      }

    map<MESH_NAME_, map<MED_EN::medEntityMesh,SUPPORT *> >::iterator itSupportOnMesh ;

    int index = 0;

    vector<SUPPORT *> vectSupp;
    for (itSupportOnMesh=_ptrMed->_support.begin();
         itSupportOnMesh != _ptrMed->_support.end();
         itSupportOnMesh++ )
      {
        map<MED_EN::medEntityMesh,SUPPORT *>::iterator itSupport ;
        for (itSupport=(*itSupportOnMesh).second.begin();
             itSupport!=(*itSupportOnMesh).second.end();
             itSupport++)
          {
            index++;
          }
      }

    MESSAGE_MED(LOC << "In this MED object there is(are) " << index << " support(s):");

    vectSupp.resize(index);

    map<MESH_NAME_, map<MED_EN::medEntityMesh,SUPPORT *> >::const_iterator const_itSupportOnMesh ;

    index = 0;
    for (const_itSupportOnMesh=_ptrMed->_support.begin(); const_itSupportOnMesh != _ptrMed->_support.end();
         const_itSupportOnMesh++ )
      {
        map<MED_EN::medEntityMesh,SUPPORT *>::const_iterator const_itSupport ;
        for (const_itSupport=(*const_itSupportOnMesh).second.begin();
             const_itSupport!=(*const_itSupportOnMesh).second.end();const_itSupport++)
          {
            vectSupp[index] = (*const_itSupport).second;
            SCRUTE_MED(vectSupp[index]);
            MESSAGE_MED(LOC << "Support number " << index << " is "<< *vectSupp[index]);
            index++;
          }
      }

  }
  
  
  // Read number of fields, their associated name and their list of (timeStepNumber,iterationNumber)
  {
    int                           numberOfFields              = 0;      //MED_INVALID
    //    char                          fieldName[MED_TAILLE_NOM+1] = "";
    char                          fieldName[MED_TAILLE_LNOM+1] ; // to avoid a crash if the field name is longer than MED_TAILLE_NOM
    int                           numberOfComponents           = 0;
    char                          * componentName              = (char *) MED_NULL;
    char                          * unitName                   =  (char *) MED_NULL;
    //    char                          meshName[MED_TAILLE_NOM+1]  = "";
    char                          meshName[MED_TAILLE_NOM+1]  ;
    med_2_3::med_type_champ        type;
    MESH                          * ptrMesh                        = (MESH  *) MED_NULL;
    FIELD_                        * ptrField                       = (FIELD_ *) MED_NULL;
    //MED_FIELD_RDWR_DRIVER         * ptrDriver                      = (MED_FIELD_RDWR_DRIVER * ) MED_NULL;
    GENDRIVER                     * ptrDriver                      = (GENDRIVER * ) MED_NULL;
    SUPPORT                       * ptrSupport                     = (SUPPORT *   ) MED_NULL;
    MESH_ENTITIES::const_iterator currentEntity; 
    list<MED_EN::medGeometryElement>::const_iterator currentGeometry;
    med_2_3::med_int                           NbOfGaussPts                 =  0;
    int                           numberOfTimeSteps            =  -1;
    med_2_3::med_int                           timeStepNumber               =  -1;
    //    char                          timeStepUnit[MED_TAILLE_PNOM]= "";
    char                          timeStepUnit[MED_TAILLE_PNOM22+1] ;
    double                        timeStep                     = 0.0;
//CCRT    int                           orderNumber                  =  -1;                           //???init?????
    med_2_3::med_int                           orderNumber                  =  -1;                           //???init?????
//CCRT    int                           numberOfRefMesh = 0;
    med_2_3::med_int                           numberOfRefMesh = 0;
    med_2_3::med_booleen           meshLink;
    map<MESH_NAME_,MESH*>      & _meshes   =  _ptrMed->_meshes; 
    map<FIELD_NAME_,MAP_DT_IT_> & _fields   =  _ptrMed->_fields; 
    map<FIELD_ *, MESH_NAME_>  & _meshName =  _ptrMed->_meshName; 
    map<MESH_NAME_, map<MED_EN::medEntityMesh,SUPPORT *> > & support = _ptrMed->_support;

    numberOfFields = med_2_3::MEDnChamp(_medIdt,0) ;
    if ( numberOfFields <= 0 ) 
      MESSAGE_MED(LOC << "Be careful there is no field in file |"<<
              _fileName<<"| !");

    for (i=1;i<=numberOfFields;i++)
      {
        numberOfComponents = med_2_3::MEDnChamp(_medIdt,i) ;

        if ( numberOfComponents <= 0 ) 
          if (err != MED_VALID)
            throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Be careful there is no compound for field n°" << i << "in file |"<<_fileName<<"| !"));

        componentName = new char[numberOfComponents*MED_TAILLE_PNOM22+1] ;
        unitName      = new char[numberOfComponents*MED_TAILLE_PNOM22+1] ;   
      
        err = MEDchampInfo(_medIdt, i, fieldName, &type, componentName, 
                           unitName, numberOfComponents) ;

        MESSAGE_MED("Field n°" << i << " nammed " << fieldName << endl
                << "Name real length = " << strlen(fieldName)
                << ", while reserved " << MED_TAILLE_NOM);

        if (err != MED_VALID)
          throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << ": can't get information about the field n°" << i <<" of the file |" << _fileName << "| !")); 

        MESSAGE_MED(LOC << "Field n°"<<i<<" nammed "<< fieldName 
                << " ,component(s)  : " << componentName 
                <<" ,unit(s) : "        << unitName);

        // Loop on all (entity type,geometry type) until you find an existing
        // one then get the (n°dt,n°it) pairs list for the current
        // (field,entity type,geometry type) We suppose there is the same list
        // whatever the existing (entity type,geometry type) pair 

        // support map :

        for (currentEntity=meshEntities.begin();currentEntity != meshEntities.end(); currentEntity++)
          { 
            //         numberOfTimeSteps  MUST be given by MEDchampInfo !!!!!
            for (currentGeometry  = (*currentEntity).second.begin();
                 currentGeometry != (*currentEntity).second.end();
                 currentGeometry++)
              {
                MESSAGE_MED("Field information with Entity,Geom = "<<
                        (*currentEntity).first<<","<<(*currentGeometry));

                numberOfTimeSteps =
                  MEDnPasdetemps(_medIdt, fieldName,
                                 (med_2_3::med_entite_maillage)(*currentEntity).first,
                                 (med_2_3::med_geometrie_element) (*currentGeometry) );

                MESSAGE_MED("Field information 2 : NumberOfTimeStep :"<<
                        numberOfTimeSteps);
/*
          if ( numberOfTimeSteps > MED_VALID ) 
                  break ;
                // There are value for some med_geometrie_element of this
                // med_entite_maillage.
        }
        if (numberOfTimeSteps>0) // we have at least one
          
              for (currentGeometry = (*currentEntity).second.begin();
                   currentGeometry != (*currentEntity).second.end();
                   currentGeometry++)
                {*/
                MESSAGE_MED("Field information 3 : Geom : "<<(*currentGeometry));
              
                for (j=1;j <= numberOfTimeSteps; j++)
                    {
                      MESSAGE_MED("Field information 4 : time step j = "<<j);
                
                      // err = MEDpasdetempsInfo( _medIdt, fieldName,
                      // (med_2_3::med_entite_maillage) (*currentEntity).first, 
                      // (*currentGeometry),j, meshName,
                      // &NbOfGaussPts, &timeStepNumber,
                      // timeStepUnit, &timeStep, &orderNumber);

                      err = MEDpasdetempsInfo(_medIdt, fieldName,
                                              (med_2_3::med_entite_maillage)
                                              (*currentEntity).first,
                                              (med_2_3::med_geometrie_element)
                                              (*currentGeometry),j,
                                              &NbOfGaussPts, &timeStepNumber,
                                              &orderNumber, timeStepUnit,
                                              &timeStep, meshName, &meshLink,
                                              &numberOfRefMesh);

                      if (err == MED_VALID)
                        { // we have found for (*currentEntity).first and
                          // (*currentGeometry)
                          MESSAGE_MED("Field information 5 ;: NumberOfGaussPoint : " << NbOfGaussPts << ", timeStepNumber : " << timeStepNumber << ", orderNumber : " << orderNumber);
                          // CORRECT a bug in MEDpasdetempsInfo :
                          // we get a value n'importe quoi in NbOfGaussPts !!!!
                
                          if (timeStepNumber<0)  timeStepNumber=-1 ;
                          if ((numberOfRefMesh != 1) ||
                              (meshLink != med_2_3::MED_VRAI) ||
                              (NbOfGaussPts != 1))
                            {
                              NbOfGaussPts = 1;
                              numberOfRefMesh=1;
                              MESSAGE_MED("This field is Med Memory compliant because NumberOfGaussPoint : " << NbOfGaussPts << ", or  numberOfRefMesh : " << numberOfRefMesh << ", or meshLink : " << meshLink);
                            }
                
                          // ATTENTION TRAITER L'EXCEPTION CI DESSUS !!!!!!!!
                
                          // Il faudra traiter le cas d'un champ qui utilise
                          // +sieurs (entity,geom) voir le travail de patrick
                          // Il faudra traiter le cas des profils...
                          //             ptrField = new FIELD();
                          // _ptrDriver = new MED_FIELD_RDWR_DRIVER(_fileName,
                          //                                        ptrField);
                          //             ptrField->addDriver(_ptrDriver);
                          //             _fields[fieldName]=ptrField;
                
                          map<MESH_NAME_,MESH*>::iterator _meshesIt =
                            _meshes.find(meshName);

                          if ( _meshesIt == _meshes.end() ) {
                            MESSAGE_MED(LOC << "There is no mesh |" << meshName <<
                                    "| in the file |" << _fileName <<
                                    "|, but  |" << meshName <<
                                    "| is referenced by field |" <<
                                    fieldName <<"|, entity : |" <<
                                    entNames[ (med_2_3::med_entite_maillage)
                                               (*currentEntity).first] <<
                                    "|, geometric element of type |"  <<
                                    geoNames [(*currentGeometry)]     <<"|"); 
                          }
                          // POURQUOI SI JE NE MET PAS DE BLOCK J'AI UN PARSE
                          // ERROR : PG : c'est la macro MESSAGE_MED qui fait ca !
                          else 
                            ptrMesh = _meshes[meshName];
                
                          ptrSupport     =  support[meshName][(MED_EN::medEntityMesh) (*currentEntity).first];
                          if (NbOfGaussPts != 1)
                            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Number of Gauss Point must be equal to 1 for instance")) ;

                          // init to null to prevent some error if not
                          // correctly allocated !
                
                          ptrField = (FIELD_*)NULL ;
                          ptrDriver = (GENDRIVER*)NULL ;

                          switch ( type) {
                          case med_2_3::MED_INT64 :
                            if ( sizeof(med_2_3::med_int) != 8 )
                              throw MED_EXCEPTION(LOCALIZED(STRING(LOC) <<" The Field type of |" << fieldName <<"|, entity : |" << entNames[(med_2_3::med_entite_maillage) (*currentEntity).first] <<"|, geometric element of type |" << geoNames [(*currentGeometry)] << "| is  MED_INT64 but size of med_int is not equal to 8 bytes !"));
                            break;
                          case med_2_3::MED_INT32 : {
                            ptrField =  new FIELD<int> ( );
                            // Les valeurs du champ ne doivent pas être lue
                            // pour l'instant
                            ((FIELD<int>*)
                             ptrField)->setSupport(ptrSupport);
                            ((FIELD<int>*)
                             ptrField)->setNumberOfComponents(numberOfComponents);
                            ((FIELD<int>*)
                             ptrField)->setName(fieldName) ;
                            //provisoire, pour debug
                            MESSAGE_MED("#### SET NAME in FIELD : "<<fieldName);

                            MED_EN::med_mode_acces myMode = getAccessMode();
//                          switch (myMode) {
//                          case MED_EN::MED_LECT: // V2_1->V2_2
//                            ptrDriver = new
//                              MED_FIELD_RDONLY_DRIVER<int>
//                              (_fileName, (FIELD<int> *)
//                               ptrField);
//                            break ;
//                          case MED_EN::MED_REMP:      // V2_1->V2_2
//                            ptrDriver = new
//                              MED_FIELD_RDWR_DRIVER<int>
//                              (_fileName, (FIELD<int> *)
//                               ptrField);
//                            break ;
//                          case MED_EN::MED_ECRI: // should never append !! V2_1->V2_2
//                            ptrDriver = new
//                              MED_FIELD_WRONLY_DRIVER<int>
//                              (_fileName, (FIELD<int> *)
//                               ptrField);
//                            break;
//                          default:
//                            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Bad file mode access !"));
//                          }

                            ptrDriver = DRIVERFACTORY::buildConcreteMedDriverForField<int>(_fileName, (FIELD<int> *)ptrField, myMode, V22);

                            SCRUTE_MED(ptrDriver);


                            break;
                          }
                          case MED_EN::MED_REEL64 : {
                            ptrField = new FIELD<double> ( );
                            // Les valeurs du champ ne doivent pas être lue
                            // pour l'instant
                            ((FIELD<double>*)
                             ptrField)->setSupport(ptrSupport);
                            ((FIELD<double>*)
                             ptrField)->setNumberOfComponents(numberOfComponents);
                            ((FIELD<double>*)
                             ptrField)->setName(fieldName) ;
                            //provisoire, pour debug
                            MESSAGE_MED("#### SET NAME in FIELD : "<<fieldName);

                            MED_EN::med_mode_acces myMode = getAccessMode();
//                          switch (myMode) {
//                          case MED_EN::MED_LECT: // V2_1->V2_2
//                            ptrDriver = new
//                              MED_FIELD_RDONLY_DRIVER<double>
//                              (_fileName, (FIELD<double> *)
//                               ptrField);
//                            break ;
//                          case MED_EN::MED_REMP:      // V2_1->V2_2
//                            ptrDriver = new
//                              MED_FIELD_RDWR_DRIVER<double>
//                              (_fileName, (FIELD<double> *)
//                               ptrField);
//                            break ;
//                          case MED_EN::MED_ECRI: // should never append !! V2_1->V2_2
//                            ptrDriver = new
//                              MED_FIELD_WRONLY_DRIVER<double>
//                              (_fileName, (FIELD<double> *)
//                               ptrField);
//                            break;
//                          default:
//                            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Bad file mode access !"));
//                          }

                            ptrDriver = DRIVERFACTORY::buildConcreteMedDriverForField<double>(_fileName, (FIELD<double> *)ptrField, myMode, V22);
                            SCRUTE_MED(ptrDriver);

                            break;
                          }
                          default : {
                            if ( numberOfTimeSteps > 1) 
                              throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << " The Field type of |" << fieldName <<"|, entity : |" << entNames[(med_2_3::med_entite_maillage) (*currentEntity).first] <<"|, geometric element of type |" << geoNames [(*currentGeometry)] <<"| is neither MED_INT, MED_INT32, MED_INT64 nor MED_REEL64 !"));
                            break ;
                          }
                          }
                          // need to write field !
                
                          MESSAGE_MED("timeStepNumber :"<<timeStepNumber<<
                                  ",orderNumber :"<<orderNumber);
                          ptrField->setIterationNumber ( timeStepNumber);
                          // A ajouter dans la classe FIELD
                          ptrField->setOrderNumber     ( orderNumber); 
                          ptrField->setTime            ( timeStep); 
                
                          // Create a driver for this (field n°dt,n°it)
                          ptrDriver->setId            ( getId() );
                          MESSAGE_MED("###### ptrDriver->setFieldName : #"<<
                                  fieldName<<"#");
                          ptrDriver->setFieldName(fieldName);
                          ptrField->addDriver(*ptrDriver);
                          // driver is duplicated : remove it
                          delete ptrDriver;

                          DT_IT_ dtIt;
                          dtIt.dt  = timeStepNumber;
                          dtIt.it  = orderNumber;

                          //020582:[CEA 368] MEDMEM don't work with a same field on NODES and CELLS
                          //(_fields  [fieldName])[dtIt] = ptrField;
                          MAP_DT_IT_::iterator dtit_field =
                            _fields  [fieldName].insert(make_pair(dtIt, ptrField)).first;
                          if ( ptrField != dtit_field->second )
                          {
                            if ( ptrField->getSupport()->getEntity() !=
                                 dtit_field->second->getSupport()->getEntity())
                            {
                              INFOS_MED("Can't store field |"<<fieldName<<"| with (timeStepNumber="<<
                                        timeStepNumber<<",orderNumber="<<orderNumber<<
                                        ") on "<<entNames[ ptrField->getSupport()->getEntity() ]<<
                                        " since its part on " <<
                                        entNames[ dtit_field->second->getSupport()->getEntity() ]<<
                                        " has been already stored.\n Use constructor "
                                        "FIELD(SUPPORT,MED_DRIVER,...) to get the part on "<<
                                        entNames[ ptrField->getSupport()->getEntity() ]);
                            }
                            delete ptrField;
                          }
                          else
                          {
                            _meshName[ptrField ]       = meshName;
                          }
                        }
                    }
                }
          }
        delete[] componentName ;
        delete[] unitName ;
      }
  }
  
  // read profil count and their names
  //  int support_count_= 0 ; !! UNUSED VARIABLE !!
  // il faut lire les champs pour avoir les profils stockes !!!
  // il faudrait implémenter la lecture des profils dans med !!!
  
  END_OF_MED(LOC);
  
}

// This method ask the drivers of all MESH/FIELD objects created from this MED driver
// to read themselves
void MED_MED_RDONLY_DRIVER22::read( void )
  throw (MEDEXCEPTION) // from objects method read !
{
 
  const char* LOC = "MED_MED_DRIVER22::read() : ";
  BEGIN_OF_MED(LOC);

  // For PAL12192: assure that file structure is already read
  this->open();
  this->readFileStruct();
  this->close();

  const map<MESH_NAME_, MESH*> & _meshes = const_cast<const map<MESH_NAME_, MESH*>& > (_ptrMed->_meshes); 
  map<MESH_NAME_,MESH*>::const_iterator  currentMesh;

  const map<FIELD_ *, MESH_NAME_> & _meshName = const_cast<const map<FIELD_ *, MESH_NAME_>& > (_ptrMed->_meshName);
  map<FIELD_ *, MESH_NAME_>::const_iterator currentField;
  
  for ( currentMesh=_meshes.begin();currentMesh != _meshes.end(); currentMesh++ ) 
    //(*currentMesh).second->read(*this); -> crash in MEDMEMTest_TopLevel.cxx
    (*currentMesh).second->read(); // default reader, from readFileStruct
    
  // PROVISOIRE
  _ptrMed->updateSupport() ;

  for ( currentField =_meshName.begin(); currentField != _meshName.end(); currentField++ )
    //(*currentField).first->read(*this);
    (*currentField).first->read(); // default reader, from readFileStruct

  END_OF_MED(LOC);
}

// ------------- Write Only Part --------------

MED_MED_WRONLY_DRIVER22::MED_MED_WRONLY_DRIVER22()
{
  this->GENDRIVER::_accessMode = MED_EN::WRONLY;
}

MED_MED_WRONLY_DRIVER22::MED_MED_WRONLY_DRIVER22(const string & fileName,  MED * const ptrMed):
  IMED_MED_WRONLY_DRIVER(fileName,ptrMed),
  MED_MED_DRIVER22(fileName,ptrMed),
  MED_MED_DRIVER(fileName,ptrMed,MED_EN::WRONLY)
{}

MED_MED_WRONLY_DRIVER22::MED_MED_WRONLY_DRIVER22(const MED_MED_WRONLY_DRIVER22 & driver):
  IMED_MED_WRONLY_DRIVER(driver),
  MED_MED_DRIVER(driver),
  MED_MED_DRIVER22(driver)
{}

MED_MED_WRONLY_DRIVER22::~MED_MED_WRONLY_DRIVER22()
{
  MESSAGE_MED("MED_MED_WRONLY_DRIVER22::~MED_MED_WRONLY_DRIVER22() has been destroyed");
} 

GENDRIVER * MED_MED_WRONLY_DRIVER22::copy(void) const
{
  return new MED_MED_WRONLY_DRIVER22(*this) ;
}

void MED_MED_WRONLY_DRIVER22::read(void)
  throw (MEDEXCEPTION)
{
  IMED_MED_WRONLY_DRIVER::read();
}

void MED_MED_WRONLY_DRIVER22::readFileStruct(void)
  throw (MEDEXCEPTION)
{
  IMED_MED_WRONLY_DRIVER::readFileStruct();
}

// This method ask the drivers of all MESH/FIELD objects created from this MED driver
// to write themselves
void MED_MED_WRONLY_DRIVER22::writeFrom( void) const
  throw (MEDEXCEPTION) //from object method write !
{

  const char* LOC = "MED_MED_DRIVER22::writeFrom() : ";
  BEGIN_OF_MED(LOC);

  const map<MESH_NAME_, MESH*> & _meshes = const_cast<const map<MESH_NAME_, MESH*>& > (_ptrMed->_meshes); 
  map<MESH_NAME_,MESH*>::const_iterator  currentMesh;

  const map<FIELD_ *, MESH_NAME_> & _meshName = const_cast<const map<FIELD_ *, MESH_NAME_>& > (_ptrMed->_meshName);
  map<FIELD_ *, MESH_NAME_>::const_iterator currentField;
  
  for ( currentMesh=_meshes.begin();currentMesh != _meshes.end(); currentMesh++ ) {
    try {
      (*currentMesh).second->write(*this); 
      // On utilise pour les objects MESH ET FIELD le write(GENDRIVER *) et le == ds GENDRIVER avec eventuellement 1 id 
    }
    catch ( const MED_DRIVER_NOT_FOUND_EXCEPTION & ) {
      continue;
    }
  }

  for ( currentField=_meshName.begin();currentField != _meshName.end(); currentField++ ) {
    try {
      (*currentField).first->write(*this);
    }
    catch ( const MED_DRIVER_NOT_FOUND_EXCEPTION & ) {
      continue;
    }
  }

  END_OF_MED(LOC);

}

void MED_MED_WRONLY_DRIVER22::write(void ) const
  throw (MEDEXCEPTION) // from object method write !
{
  int current;

  const char* LOC = "MED_MED_DRIVER22::write() : ";
  BEGIN_OF_MED(LOC);

  // BCLE SUR LES OBJETS AVEC AJOUT DE DRIVER ET APPELS write

  const map<MESH_NAME_, MESH*> & _meshes = const_cast<const map<MESH_NAME_, MESH*>& > (_ptrMed->_meshes); 
  map<MESH_NAME_,MESH*>::const_iterator  currentMesh;

  const map<FIELD_ *, MESH_NAME_> & _meshName = const_cast<const map<FIELD_ *, MESH_NAME_>& > (_ptrMed->_meshName);
  map<FIELD_ *, MESH_NAME_>::const_iterator currentField;
  
  for ( currentMesh=_meshes.begin();currentMesh != _meshes.end(); currentMesh++ ) {
    //current = (*currentMesh).second->addDriver(MED_DRIVER,_fileName);
    MESSAGE_MED((*currentMesh).second->getName());
    current = (*currentMesh).second->addDriver(MED_DRIVER,_fileName,(*currentMesh).second->getName());
    // put right _id in Mesh driver (same as this._id)
    (*currentMesh).second->_drivers[current]->setId( getId() );
    (*currentMesh).second->write(current) ;
  }

  for ( currentField=_meshName.begin();currentField != _meshName.end(); currentField++ ) {
    //current = (*currentField).first->addDriver(MED_DRIVER,_fileName);getName())
    MESSAGE_MED((*currentField).first->getName());
    FIELD_* myField = (*currentField).first;
    current = (*currentField).first->addDriver(MED_DRIVER,_fileName,myField->getName());
    // put right _id in Field driver (same as this._id)
    (*currentField).first->_drivers[current]->setId( getId() );
    (*currentField).first->write(current) ;
  }

  // that's work, but it is more efficenty to write directly when we had driver, no ?
  //writeFrom();
  
  END_OF_MED(LOC);

}

// ------------- Read Write Part --------------

MED_MED_RDWR_DRIVER22::MED_MED_RDWR_DRIVER22()
{
  this->GENDRIVER::_accessMode = MED_EN::RDWR;
}

MED_MED_RDWR_DRIVER22::MED_MED_RDWR_DRIVER22(const string & fileName,  MED * const ptrMed):
  MED_MED_RDONLY_DRIVER22(fileName,ptrMed),
  MED_MED_WRONLY_DRIVER22(fileName,ptrMed),
  IMED_MED_RDWR_DRIVER(fileName,ptrMed),
  MED_MED_DRIVER22(fileName,ptrMed,RDWR),
  IMED_MED_WRONLY_DRIVER(fileName,ptrMed),
  IMED_MED_RDONLY_DRIVER(fileName,ptrMed),
  MED_MED_DRIVER(fileName,ptrMed,RDWR)
{}

MED_MED_RDWR_DRIVER22::MED_MED_RDWR_DRIVER22(const MED_MED_RDWR_DRIVER22 & driver):
  MED_MED_RDONLY_DRIVER22(driver),
  MED_MED_WRONLY_DRIVER22(driver),
  IMED_MED_RDWR_DRIVER(driver),
  MED_MED_DRIVER22(driver),
  IMED_MED_WRONLY_DRIVER(driver),
  IMED_MED_RDONLY_DRIVER(driver),
  MED_MED_DRIVER(driver)
{}

MED_MED_RDWR_DRIVER22::~MED_MED_RDWR_DRIVER22() { 
  MESSAGE_MED("MED_MED_RDWR_DRIVER22::~MED_MED_RDWR_DRIVER22() has been destroyed");
}

GENDRIVER * MED_MED_RDWR_DRIVER22::copy(void) const
{
  return new MED_MED_RDWR_DRIVER22(*this) ;
}

void MED_MED_RDWR_DRIVER22::read(void)
  throw (MEDEXCEPTION) // from MED_MED_RDONLY_DRIVER::read()
{
  const char* LOC = "MED_MED_RDWR_DRIVER22::read(void)";
  BEGIN_OF_MED(LOC);
  MED_MED_RDONLY_DRIVER22::read();
  END_OF_MED(LOC);
}

void MED_MED_RDWR_DRIVER22::readFileStruct(void)
  throw (MEDEXCEPTION) // from MED_MED_RDONLY_DRIVER::readFileStruct()
{
  const char* LOC = "MED_MED_RDWR_DRIVER22::readFileStruct(void)";
  BEGIN_OF_MED(LOC);
  MED_MED_RDONLY_DRIVER22::readFileStruct();
  END_OF_MED(LOC);
}

void MED_MED_RDWR_DRIVER22::write(void) const
  throw (MEDEXCEPTION) // from MED_MED_WRONLY_DRIVER::write()
{
  const char* LOC = "MED_MED_RDWR_DRIVER22::write(void) const";
  BEGIN_OF_MED(LOC);
  MED_MED_WRONLY_DRIVER22::write();
  END_OF_MED(LOC);
}

void MED_MED_RDWR_DRIVER22::writeFrom(void) const
  throw (MEDEXCEPTION) // from MED_MED_WRONLY_DRIVER::writeFrom();
{
  const char* LOC = "MED_MED_RDWR_DRIVER22::writeFrom(void) const";
  BEGIN_OF_MED(LOC);
  MED_MED_WRONLY_DRIVER22::writeFrom();
  END_OF_MED(LOC);
}