#ifndef GENDRIVER_HXX
#define GENDRIVER_HXX

#include <string>

#include "MEDMEM_define.hxx"
#include "utilities.h"

/* Generic Read/Write Driver Class for Meshes & Fields */

using namespace std;

/*!

  Virtual class GENDRIVER use by all driver.

*/
namespace MEDMEM {

/* Modify the following line to add a new driver type (step 1) */
typedef enum { MED_DRIVER = 0, GIBI_DRIVER = 1, PORFLOW_DRIVER = 2, VTK_DRIVER = 254, ASCII_DRIVER = 3, NO_DRIVER = 255 } driverTypes;
class GENDRIVER {

protected :

  int            _id;         // MED_INVALID : if the driver hasn't been created by a MedMedDriver
                              // the MedMedDriver index of the driver vector in the MED object where it has been created
  /*File related part*/
  string         _fileName;   // The name of the file currently in use.
  MED_EN::med_mode_acces _accessMode; // The file access mode set by the adequate construtor.
  int            _status;     // The file status {MED_INVALID, MED_OPENED, MED_CLOSED } 
  driverTypes    _driverType; //  A FAIRE LE POSITIONNER DS TOUTES LES SS CLASSES !!


public:
  /*!
    Constructor.
  */
  GENDRIVER();
  /*!
    Constructor.
  */
  GENDRIVER(const string & fileName,MED_EN::med_mode_acces accessMode);
  /*!
    Copy constructor.
  */
  GENDRIVER(const GENDRIVER & genDriver);

  /*!
    Destructor.
  */
  virtual ~GENDRIVER();

  /*!
    Operator = : duplicate the given genDriver.
  */
  GENDRIVER & operator=(const GENDRIVER & genDriver);

  /*!
    Operator << : put GENDRIVER object information to the given stream
  */
  friend ostream & operator<<(ostream &os,const GENDRIVER &genDriver);

  bool operator ==(const GENDRIVER &genDriver) const;
  /*!
    Open file.
  */
  virtual void open ( void ) = 0;
  /*!
    Open file with the append option. This method is used especially
    on ASCII drivers (e.g. VTK_DRIVER).
  */
  virtual void openAppend ( void );
  /*!
    Close file.
  */
  virtual void close( void ) = 0;
  /*!
    Write object in opened file.
  */
  virtual void write( void ) const = 0;
  /*!
    Write object in opened file. This method is used especially
    on ASCII drivers (e.g. VTK_DRIVER).
  */
  virtual void writeAppend( void ) const;
  /*!
    Read object in opened file.
  */
  virtual void read ( void ) = 0;
  
  // needed to duplicate arrays 
  virtual GENDRIVER * copy ( void ) const = 0 ;

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
  MED_EN::med_mode_acces getAccessMode() const;
};
};


#endif /* GENDRIVER_HXX */
