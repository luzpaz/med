#ifndef GENDRIVER_HXX
#define GENDRIVER_HXX

#include <string>

#include "MEDMEM_define.hxx"
#include "utilities.h"

/* Generic Read/Write Driver Class for Meshes & Fields */

typedef enum { MED_DRIVER = 0, VTK_DRIVER = 1, NO_DRIVER = 255 } driverTypes;

using namespace MED_EN;
using namespace std;

class GENDRIVER {

protected :

  int            _id;         // MED_INVALID : if the driver hasn't been created by a MedMedDriver
                              // the MedMedDriver index of the driver vector in the MED object where it has been created
  /*File related part*/
  string         _fileName;
  med_mode_acces _accessMode; 
  int            _status;
  driverTypes    _driverType; //  A FAIRE LE POSITIONNER DS TOUTES LES SS CLASSES !!

public:
  GENDRIVER();
  
  GENDRIVER(const string & fileName,med_mode_acces accessMode);
  GENDRIVER(const GENDRIVER & genDriver);
  ~GENDRIVER();

  GENDRIVER & operator=(const GENDRIVER & genDriver);

  friend ostream & operator<<(ostream &os,const GENDRIVER &genDriver);

  bool operator ==(const GENDRIVER &genDriver) const;
 
  virtual void open ( void ) = 0;
  virtual void close( void ) = 0;
  virtual void write( void ) const = 0;
  virtual void read ( void ) = 0;
  
  // MED  related part
  virtual void writeFrom      ( void );
  virtual void readFileStruct ( void );
  // MESH related Part
  virtual void setMeshName    ( const string & meshName);
  // FIELD related Part
  virtual void setFieldName   ( const string & fieldName);

  void   setId       ( int id = MED_INVALID );
  int    getId       ( void ) const ;
  string getFileName () const;
  void   setFileName ( const string & fileName);
  med_mode_acces getAccessMode() const;
};



#endif /* GENDRIVER_HXX */
