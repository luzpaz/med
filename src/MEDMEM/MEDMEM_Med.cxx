# include "MEDMEM_Med.hxx"
 
# include "MEDMEM_STRING.hxx"

# include "MEDMEM_Mesh.hxx"
# include "MEDMEM_Field.hxx"

# include "MEDMEM_Exception.hxx"
# include "utilities.h"

// MED constructors
MED::MED() {
  const char * LOC = "MED::MED() : ";
 
  MESSAGE(LOC << "Construction...");

};

MED::MED(driverTypes driverType, const string & fileName) {
  const char * LOC = "MED::MED(driverTypes driverType, const string & fileName) : ";
  int current;

  BEGIN_OF(STRING(LOC));
  current = addDriver(driverType,fileName);
  _drivers[current]->open();
  _drivers[current]->readFileStruct();
  _drivers[current]->close();
  END_OF(STRING(LOC));
};

MED::~MED()
{
} ;

// ------- Drivers Management Part

MED::INSTANCE_DE<MED_MED_DRIVER> MED::inst_med ;
//MED::INSTANCE_DE<VTK_DRIVER> MED::inst_vtk ;

const MED::INSTANCE * const MED::instances[] = { &MED::inst_med }; //, &MED::inst_vtk } ;

int MED::addDriver(driverTypes driverType, const string & fileName="Default File Name.med") {

  const char * LOC = "MED::addDriver(driverTypes driverType, const string & fileName=\"Default File Name.med\") : ";
  GENDRIVER * driver;
  int current;

  BEGIN_OF(LOC);

  driver = instances[driverType]->run(fileName, this) ;
  _drivers.push_back(driver);
  current = _drivers.size()-1;
  // _drivers[current]->set... 

  END_OF(LOC);
  return current;
}


void MED::rmDriver (int index=0) {
  const char * LOC = "MED::rmDriver (int index=0): ";
  BEGIN_OF(LOC);

  if (_drivers[index])
    //_drivers.erase(&_drivers[index]); 
    {}
  else
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "The index given is invalid, index must be between 0 and |" 
                                     << _drivers.size()
                                     )
                          );   
  END_OF(LOC);
}


void MED::writeFrom (int index=0)
{
  const char * LOC = "MED::write (int index=0): ";
  BEGIN_OF(LOC);

  if (_drivers[index]) {
    _drivers[index]->open(); 
    _drivers[index]->writeFrom(); 
    _drivers[index]->close(); 
  }
  throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                   << "The index given is invalid, index must be between 0 and |" 
                                   << _drivers.size()
                                   )
                        ); 
  END_OF(LOC);
}; 


void MED::write (int index=0)
{
  const char * LOC = "MED::writeAll (int index=0): ";
  BEGIN_OF(LOC);

  if (_drivers[index]) {
    _drivers[index]->open(); 
    _drivers[index]->write(); 
    _drivers[index]->close(); 
  }
  else
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "The index given is invalid, index must be between  0 and |" 
                                     << _drivers.size()
                                     )
                          ); 
  END_OF(LOC);
}; 


void MED::readFileStruct (int index=0) {
  const char * LOC = "MED::readFileStruct (int index=0): ";
  BEGIN_OF(LOC);
  
  if (_drivers[index]) {
    _drivers[index]->open(); 
    _drivers[index]->readFileStruct(); 
    _drivers[index]->close(); 
  }
  else
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "The index given is invalid, index must be between 0 and |" 
                                     << _drivers.size()
                                     )
                          );   
  END_OF(LOC);
}

// void MED::read  (int index=0)
// {
//   const char * LOC = "MED::read (int index=0): ";
//   BEGIN_OF(LOC);
  
//   if (_drivers[index])  
//     throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
//                                      << "The index given is invalid, index must be between  >0 and < |" 
//                                      << _drivers.size()-1 
//                                      )
//                           );   
//   // GERER LE 0
//   _drivers[index]->open(); 
//   _drivers[index]->read(); 
//   _drivers[index]->close(); 

//   END_OF(LOC);

// };

// ------- End Of Drivers Management Part

int      MED::getNumberOfMeshes ( void ) const {

  const char * LOC = "MED::getNumberOfMeshes ( void ) const : ";
  BEGIN_OF(LOC);

  return _meshes.size();

  END_OF(LOC);
};   
    
int      MED::getNumberOfFields ( void ) const {

  const char * LOC = "MED::getNumberOfFields ( void ) const : ";
  BEGIN_OF(LOC);

  //  return _meshName.size();
  return _fields.size(); // we get number of field with different name

  END_OF(LOC);
};       

void MED::getMeshNames      ( string * meshNames ) const {

  const char * LOC = "MED::getMeshNames ( string * ) const : ";
  BEGIN_OF(LOC);
  int meshNamesSize;
  
  if (  ( meshNamesSize = sizeof(meshNames) / sizeof(string *) )
       != _meshes.size() )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "Size of parameter meshNames is |" 
                                     << meshNamesSize    << "| and should be |" 
                                     << _meshes.size() << "| and should be |" 
                                     )
                          );   
  
  // REM : ALLOCATION D'UN TABLEAU DE POINTEURS SUR STRING FAITE PAR LE CLIENT
  map<MESH_NAME_,MESH*>::const_iterator  currentMesh; // ??ITERATEUR CONST SUR UN OBJET NON CONST ??

  int meshNamesIndex = 0;

  for ( currentMesh=_meshes.begin();currentMesh != _meshes.end(); currentMesh++ ) {
    meshNames[meshNamesIndex]=(*currentMesh).first;
    meshNamesIndex++;                               // CF OPTIMISATION
  }

  END_OF(LOC);
};

deque<string> MED::getMeshNames      () const {
  
  const char * LOC = "MED::getMeshNames () const : ";
  BEGIN_OF(LOC);

  deque<string> meshNames(_meshes.size());
  
  map<MESH_NAME_,MESH*>::const_iterator  currentMesh; // ??ITERATEUR CONST SUR UN OBJET NON CONST ??

  int meshNamesIndex = 0;

  for ( currentMesh=_meshes.begin();currentMesh != _meshes.end(); currentMesh++ ) {
    meshNames[meshNamesIndex]=(*currentMesh).first;
    meshNamesIndex++;                               // CF OPTIMISATION
  }

  END_OF(LOC);
  return meshNames ;
};

MESH   * MED::getMesh           ( const string & meshName )  const {

  const char * LOC = "MED::getMesh ( const string & meshName ) const : ";
  BEGIN_OF(LOC);

  map<MESH_NAME_,MESH*>::const_iterator itMeshes =  _meshes.find(meshName);

  //  if ( itMeshes == _meshName.end() )
  if ( itMeshes == _meshes.end() )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "There is no known mesh named |" 
                                     << meshName << "|"
                                     )
                          );
  
  return (*itMeshes).second;
  
  END_OF(LOC);
}

MESH   * MED::getMesh           (const FIELD_ * const field ) const {
 
  const char * LOC = "MED::getMesh ( const FIELD * field ) const : ";
  BEGIN_OF(LOC);

  FIELD_ * f = const_cast< FIELD_* > (field);     //  Comment faire mieux ?
  map<FIELD_ *, MESH_NAME_>::const_iterator itMeshName = _meshName.find(f);

  if ( itMeshName  == _meshName.end() )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "There is no known field associated with |" 
                                     << field << "| pointer"
                                     )
                          );   
  
  string meshName = (*itMeshName).second;
  map<MESH_NAME_,MESH*>::const_iterator itMeshes =  _meshes.find(meshName);
  if ( itMeshes == _meshes.end() )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "There is no known mesh named |"
                                     << meshName << " while it's associated with the found field |" 
                                     << field << "| pointer"
                                     )
                          );   
  
  return (*itMeshes).second;
  
  END_OF(LOC);
};


void MED::getFieldNames     ( string * fieldNames ) const {

  const char * LOC = "MED::getFieldNames ( string * ) const : ";
  BEGIN_OF(LOC);

  int fieldNamesSize =  sizeof(fieldNames) / sizeof(string *);
 
  if ( fieldNamesSize != _fields.size() )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "Size of parameter fieldNames is |" 
                                     << fieldNamesSize     << "| and should be |" 
                                     << _fields.size() << "| and should be |" 
                                     )
                          );   
  
  // REM : ALLOCATION D'UN TABLEAU DE POINTEURS SUR STRING FAITE PAR LE CLIENT
  map<FIELD_NAME_,MAP_DT_IT_>::const_iterator  currentField;

  int fieldNamesIndex = 0;
  for ( currentField=_fields.begin();currentField != _fields.end(); currentField++ ) {
    fieldNames[fieldNamesIndex]=(*currentField).first;
    fieldNamesIndex++;                               // CF OPTIMISATION
  }

  END_OF(LOC);

};

deque<string> MED::getFieldNames     () const {

  const char * LOC = "MED::getFieldNames ( ) const : ";
  BEGIN_OF(LOC);

  deque<string> fieldNames(_fields.size());

  map<FIELD_NAME_,MAP_DT_IT_>::const_iterator  currentField;

  int fieldNamesIndex = 0;

  for ( currentField=_fields.begin();currentField != _fields.end(); currentField++ ) {
    fieldNames[fieldNamesIndex]=(*currentField).first;
    fieldNamesIndex++;                               // CF OPTIMISATION
  }

  END_OF(LOC);
  return fieldNames ;
};

deque<DT_IT_> MED::getFieldIteration (const string & fieldName) const {

  const char * LOC = "MED::getFieldIteration ( const string & ) const : ";
  BEGIN_OF(LOC);

  map<FIELD_NAME_,MAP_DT_IT_>::const_iterator itFields = _fields.find(fieldName);
  
  if ( itFields == _fields.end() ) 
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "There is no known field named |" 
                                     << fieldName << "|"
                                     )
                          );   
  //  const MAP_DT_IT_ & myIterationMap =  const_cast<const MAP_DT_IT_ & > ((*itFields).second);
  const MAP_DT_IT_ & myIterationMap =  (*itFields).second ;
  MAP_DT_IT_::const_iterator currentIterator ;
  
  deque<DT_IT_> Iteration(myIterationMap.size());
  
  int iterationIndex = 0;
  
  for ( currentIterator=myIterationMap.begin();currentIterator != myIterationMap.end(); currentIterator++ ) {
    Iteration[iterationIndex]=(*currentIterator).first;
    iterationIndex++;                               // CF OPTIMISATION
  }

  END_OF(LOC);
  return Iteration ;
};

FIELD_  * MED::getField          ( const string & fieldName, const int dt=MED_NOPDT, const int it=MED_NOPDT ) const {

  const char * LOC = "MED::getField ( const string &, const int, const int ) const : ";
  BEGIN_OF(LOC);

  MESSAGE(LOC << "fieldName = "<<fieldName<<", dt ="<<dt<<", it = "<<it);

  DT_IT_ dtIt;

  dtIt.dt= dt;
  dtIt.it= it;

  map<FIELD_NAME_,MAP_DT_IT_>::const_iterator itFields = _fields.find(fieldName);

  if ( itFields == _fields.end() ) 
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "There is no known field named |" 
                                     << fieldName << "|"
                                     )
                          );   

  const MAP_DT_IT_ & map_dtIt =  const_cast<const MAP_DT_IT_ & > ((*itFields).second);
  MAP_DT_IT_::const_iterator itMap_dtIt =  map_dtIt.find(dtIt);

  if ( itMap_dtIt == map_dtIt.end() )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "There is no (dt,it) |("
                                     << dt << "," << it << ")| associated with the found field |" 
                                     << fieldName << "|"
                                     )
                          );   

  END_OF(LOC);

  //return _fields[fieldName][dtIt];
  return (*itMap_dtIt).second;
  
};


// fiend ostream & MED::operator<<(ostream &os,const MED & med) const {
//   return os;
// };


const map<MED_FR::med_entite_maillage,SUPPORT*> & MED::getSupports(const string & meshName) const
{
  const char * LOC = "MED::getSupports ( const string ) const : ";
  BEGIN_OF(LOC);

  map<MESH_NAME_, map<MED_FR::med_entite_maillage,SUPPORT *> >::const_iterator itSupportOnMesh = _support.find(meshName) ;
  
  if ( itSupportOnMesh == _support.end() )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "There is no support on mesh named |" 
                                     << meshName << "|"
                                     )
                          );
  END_OF(LOC);
  return (*itSupportOnMesh).second ;
}

SUPPORT *  MED::getSupport (const string & meshName,MED_FR::med_entite_maillage entity) const 
{
  const char * LOC = "MED::getSupport ( const string, MED_FR::med_entite_maillage ) const : ";
  BEGIN_OF(LOC);

  map<MESH_NAME_, map<MED_FR::med_entite_maillage,SUPPORT *> >::const_iterator itSupportOnMesh = _support.find(meshName) ;
  
  if ( itSupportOnMesh == _support.end() )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "There is no support on mesh named |" 
                                     << meshName << "|"
                                     )
                          );
 
  map<MED_FR::med_entite_maillage,SUPPORT *> & SupportOnMesh = (map<MED_FR::med_entite_maillage,SUPPORT *>&) ((*itSupportOnMesh).second) ;
  map<MED_FR::med_entite_maillage,SUPPORT *>::const_iterator itSupport = SupportOnMesh.find(entity) ;
  
  if (itSupport == SupportOnMesh.end() )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "There is no support on entity "
				     << entity << " in mesh named |" 
                                     << meshName << "|"
                                     )
                          );
  END_OF(LOC);
  return (*itSupport).second ;
};

void MED::updateSupport ()
{
 
  const char * LOC = "MED::updateSupport () : ";
  BEGIN_OF(LOC);

  map<MESH_NAME_, map<MED_FR::med_entite_maillage,SUPPORT *> >::iterator itSupportOnMesh ;
  for ( itSupportOnMesh=_support.begin();itSupportOnMesh != _support.end(); itSupportOnMesh++ ) {
    map<MED_FR::med_entite_maillage,SUPPORT *>::iterator itSupport ;
    for ( itSupport=(*itSupportOnMesh).second.begin();itSupport!=(*itSupportOnMesh).second.end();itSupport++)
      try {
	(*itSupport).second->update() ;
      }
      catch (MEDEXCEPTION & ex) {
	// entity not defined in mesh -> we remove support on it !
	delete (*itSupport).second ;
	(*itSupportOnMesh).second.erase(itSupport) ; // that's rigth ????
      }
  }

  END_OF(LOC);
}

void MED::addMesh(const MESH * ptrMesh) throw (MEDEXCEPTION)
{
  const char * LOC = "MED::addMesh(const MESH * ptrMesh): ";

  throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "WARNING method not yet implemented"));
}

void MED::addField(const FIELD_ * const ptrField) throw (MEDEXCEPTION)
{
  const char * LOC = "MED::addField(const FIELD_ * const ptrField): ";

  throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "WARNING method not yet implemented"));
}
