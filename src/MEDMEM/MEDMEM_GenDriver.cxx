#include "MEDMEM_GenDriver.hxx"

GENDRIVER::GENDRIVER(): _fileName(""),_accessMode((med_mode_acces)MED_INVALID),_status(MED_INVALID),_driverType(NO_DRIVER) {}

GENDRIVER::GENDRIVER(const string & fileName,
                     med_mode_acces accessMode=(med_mode_acces) MED_INVALID): _fileName(fileName),
                                                                              _accessMode(accessMode),
                                                                              _status(MED_CLOSED),
                                                                              _driverType(NO_DRIVER) {}

GENDRIVER::GENDRIVER(const GENDRIVER & genDriver):_fileName(genDriver._fileName),
                                                  _accessMode(genDriver._accessMode),
                                                  _status(genDriver._status),_driverType(NO_DRIVER) {}
GENDRIVER::~GENDRIVER() {}


GENDRIVER & GENDRIVER::operator=(const GENDRIVER &  genDriver) {
  _fileName    = genDriver._fileName;
  _accessMode  = genDriver._accessMode;
  _status      = genDriver._status;
  
  return *this;
}

void GENDRIVER::writeFrom      ( void ) {};
void GENDRIVER::readFileStruct ( void ) {};

void GENDRIVER::setMeshName    (const string & meshName) {};
void GENDRIVER::setFieldName   (const string & fieldName) {};

string GENDRIVER::getFileName() const {
  return _fileName;
}
  
void GENDRIVER::setFileName(const string & fileName)  {
  _fileName = fileName; 
}

med_mode_acces GENDRIVER::getAccessMode() const {
  return _accessMode;
}

ostream & operator<<(ostream &os,const GENDRIVER & drv)
{
  switch (drv._accessMode)
    {
    case MED_RDONLY : 
      os<<"C'est un IO de READ"<<endl;
      break;
    case MED_RDWR :
      os<<"C'est un IO d'READ/WRITE"<<endl;
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

bool GENDRIVER::operator ==(const GENDRIVER &genDriver) const {
  
  return ( _id == genDriver._id )  &&
    ( _fileName   == genDriver._fileName   ) && 
    ( _driverType == genDriver._driverType );
  
};
