//  MED MEDMEM : MED files in memory
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : MEDMEM_GenDriver.hxx
//  Module : MED

#ifndef GENDRIVER_HXX
#define GENDRIVER_HXX

#include <string>

#include "MEDMEM_define.hxx"
#include "utilities.h"

/* Generic Read/Write Driver Class for Meshes & Fields */

/* Modify the following line to add a new driver type (step 1) */
typedef enum { MED_DRIVER = 0, GIBI_DRIVER = 1, VTK_DRIVER = 254, NO_DRIVER = 255 } driverTypes;

using namespace MED_EN;
using namespace std;

/*!

  Virtual class GENDRIVER use by all driver.

*/

class GENDRIVER {

protected :

  int            _id;         // MED_INVALID : if the driver hasn't been created by a MedMedDriver
                              // the MedMedDriver index of the driver vector in the MED object where it has been created
  /*File related part*/
  string         _fileName;   // The name of the file currently in use.
  med_mode_acces _accessMode; // The file access mode set by the adequate construtor.
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
  GENDRIVER(const string & fileName,med_mode_acces accessMode);
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
    Close file.
  */
  virtual void close( void ) = 0;
  /*!
    Write object in opened file.
  */
  virtual void write( void ) const = 0;
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
  med_mode_acces getAccessMode() const;
};



#endif /* GENDRIVER_HXX */
