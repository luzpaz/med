#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
using namespace std;
using namespace MEDMEM;

GENDRIVER::GENDRIVER(): _id(MED_INVALID),
                        _fileName(""),
                        _accessMode( (med_mode_acces) MED_INVALID ),
                        _status(MED_INVALID),
                        _driverType(NO_DRIVER) {}

GENDRIVER::GENDRIVER(const string & fileName,
                     med_mode_acces accessMode=(med_mode_acces) MED_INVALID): _id(MED_INVALID),
                                                                              _fileName(fileName),
                                                                              _accessMode(accessMode),
                                                                              _status(MED_CLOSED),
                                                                              _driverType(NO_DRIVER) {}

GENDRIVER::GENDRIVER(const GENDRIVER & genDriver):
  //_id(MED_INVALID), 
  _id(genDriver._id),
  _fileName(genDriver._fileName),
  _accessMode(genDriver._accessMode),
  _status(genDriver._status),
  _driverType(genDriver._driverType) 
{}

GENDRIVER::~GENDRIVER() {}


GENDRIVER & MEDMEM::GENDRIVER::operator=(const GENDRIVER &  genDriver) 
{
  const char * LOC = " GENDRIVER & GENDRIVER::operator=(const GENDRIVER &  genDriver)  : ";
  
  BEGIN_OF(LOC);
  _fileName    = genDriver._fileName;
  _accessMode  = genDriver._accessMode;
  _status      = genDriver._status;
  _id          = genDriver._id;
  return *this;

//CCRT  END_OF(LOC);
}

void GENDRIVER::writeFrom      ( void ) {};
void GENDRIVER::readFileStruct ( void ) {};

void GENDRIVER::setMeshName    (const string & ) {};
void GENDRIVER::setFieldName   (const string & ) {};

void GENDRIVER::openAppend ( void ) {};
void GENDRIVER::writeAppend ( void ) const {};

void GENDRIVER::setId ( int id ) {
  const char * LOC = "void GENDRIVER::setId ( int id ) : ";

  BEGIN_OF(LOC);

  if ( id >= 0 ) _id=id; else _id = MED_INVALID ;

  END_OF(LOC);
};

int GENDRIVER::getId ( void) const {
  const char * LOC = "int GENDRIVER::getId ( void) const ";

  BEGIN_OF(LOC);

  return _id ;

//CCRT  END_OF(LOC);
};

string GENDRIVER::getFileName() const {

  const char * LOC = "string GENDRIVER::getFileName() const : ";
  BEGIN_OF(LOC);
  
  return _fileName;

//CCRT  END_OF(LOC);
}
    

void GENDRIVER::setFileName(const string & fileName)  {

  const char * LOC = "void GENDRIVER::setFileName(const string & fileName) : ";
  BEGIN_OF(LOC);

  if ( _status == MED_OPENED )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<" File |" << _fileName 
                                 << "| is still openned, close it before openning : | " << fileName << "|"));
  else
    _fileName = fileName; 

  END_OF(LOC);
}
       


med_mode_acces GENDRIVER::getAccessMode() const {

  const char * LOC = "med_mode_acces GENDRIVER::getAccessMode() const : ";

  BEGIN_OF(LOC);

  return _accessMode;

//CCRT  END_OF(LOC);
}

ostream & MEDMEM::operator<<(ostream &os,const GENDRIVER & drv)
{
  switch (drv._accessMode)
    {
    case MED_RDONLY : 
      os<<"C'est un IO de READ"<<endl;
      break;
    case MED_RDWR :
      os<<"C'est un IO d'READ/WRITE"<<endl;
      break;
    case MED_REMP :
      os <<"C'est un IO de remplacement"<<endl;
      break;
    }
  switch (drv._status)
    {
    case MED_OPENED :
      os<<"L'IO_Mesh_MED est open"<<endl;
      break;
    case MED_CLOSED :
      os<<"L'IO_Mesh_MED est fermé"<<endl;
      break;
    case MED_INVALID :
      os<<"L'IO_Mesh_MED est non-valide"<<endl;
      break;
    }
  return os;
}

// Test if this driver has been created from  MED driver
bool MEDMEM::GENDRIVER::operator ==(const GENDRIVER &genDriver) const {
  
  const char * LOC = "bool GENDRIVER::operator ==(const GENDRIVER &genDriver) const : ";

  MESSAGE(LOC);

  return ( _id == genDriver._id )  &&
    ( _driverType == genDriver._driverType );
  
};

