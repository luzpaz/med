//  Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "MEDMEM_MedMedDriver.hxx"
#include "MEDMEM_Compatibility21_22.hxx"
#include "MEDMEM_MedMedDriver21.hxx"
#include "MEDMEM_MedMedDriver22.hxx"
#include "MEDMEM_DriverFactory.hxx" 
#include "MEDMEM_define.hxx"

using namespace std;
using namespace MED_EN;
using namespace MEDMEM;

MED_MED_DRIVER::MED_MED_DRIVER(): GENDRIVER(MED_DRIVER), 
                                  _ptrMed(0),
                                  _concreteMedDrv(0)
{}

MED_MED_DRIVER::MED_MED_DRIVER(const string & fileName,  MED * const ptrMed):
  GENDRIVER(fileName, MED_EN::RDWR, MED_DRIVER),
  _ptrMed(ptrMed),
  _concreteMedDrv(0)
{
  //_ptrMed->addDriver(*this); // The specific MED driver id is set within the addDriver method.
}

MED_MED_DRIVER::MED_MED_DRIVER(const string & fileName,
                               MED * const ptrMed,
                               MED_EN::med_mode_acces accessMode):
  GENDRIVER(fileName, accessMode, MED_DRIVER),
  _ptrMed(ptrMed),
  _concreteMedDrv(0)
{
}
//REM :  As t'on besoin du champ _status :  _medIdt <-> _status  ?  Oui

MED_MED_DRIVER::MED_MED_DRIVER(const MED_MED_DRIVER & driver):
  GENDRIVER(driver),
  _ptrMed(driver._ptrMed)
  //  _concreteMedDrv(driver._concreteMedDrv->copy())
{
  if ( driver._concreteMedDrv )
    _concreteMedDrv = driver._concreteMedDrv->copy();
  else 
    _concreteMedDrv=0;
}

MED_MED_DRIVER::~MED_MED_DRIVER()
{
  MESSAGE_MED("MED_MED_DRIVER::~MED_MED_DRIVER() has been destroyed");
  if (_concreteMedDrv) delete _concreteMedDrv;
  _concreteMedDrv=0;
}

// ------------- Read Only Part --------------

IMED_MED_RDONLY_DRIVER::IMED_MED_RDONLY_DRIVER():MED_MED_DRIVER(),_fileStructIsRead(false)
{
}

IMED_MED_RDONLY_DRIVER::IMED_MED_RDONLY_DRIVER(const string & fileName,  MED * const ptrMed):
  MED_MED_DRIVER(fileName,ptrMed,MED_EN::RDONLY),_fileStructIsRead(false)
{
  MESSAGE_MED("MED_MED_RDONLY_DRIVER::MED_MED_RDONLY_DRIVER(const string & fileName,  MED * const ptrMed) Constructeur read only");
}

IMED_MED_RDONLY_DRIVER::IMED_MED_RDONLY_DRIVER(const IMED_MED_RDONLY_DRIVER & driver):
  MED_MED_DRIVER(driver),_fileStructIsRead(false)
{
}

IMED_MED_RDONLY_DRIVER::~IMED_MED_RDONLY_DRIVER()
{
  MESSAGE_MED("MED_MED_RDONLY_DRIVER::~MED_MED_RDONLY_DRIVER() has been destroyed");
} 

void IMED_MED_RDONLY_DRIVER::write(void) const
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("MED_MED_RDONLY_DRIVER::write : Can't write with a RDONLY driver !");
}

void IMED_MED_RDONLY_DRIVER::writeFrom(void) const
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("MED_MED_RDONLY_DRIVER::writeFrom : Can't write with a RDONLY driver !");
}

// ------------- Write Only Part --------------

IMED_MED_WRONLY_DRIVER::IMED_MED_WRONLY_DRIVER()
{
}

IMED_MED_WRONLY_DRIVER::IMED_MED_WRONLY_DRIVER(const string & fileName,  MED * const ptrMed):
  MED_MED_DRIVER(fileName,ptrMed,MED_EN::WRONLY)
{}

IMED_MED_WRONLY_DRIVER::IMED_MED_WRONLY_DRIVER(const IMED_MED_WRONLY_DRIVER & driver):
  MED_MED_DRIVER(driver)
{}

IMED_MED_WRONLY_DRIVER::~IMED_MED_WRONLY_DRIVER()
{
  MESSAGE_MED("MED_MED_WRONLY_DRIVER::~MED_MED_WRONLY_DRIVER() has been destroyed");
} 

void IMED_MED_WRONLY_DRIVER::read(void)
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("MED_MED_WRONLY_DRIVER::read : Can't read with a WRONLY driver !");
}

void IMED_MED_WRONLY_DRIVER::readFileStruct(void)
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("MED_MED_WRONLY_DRIVER::read : Can't read with a WRONLY driver !");
}

// ------------- Read Write Part --------------

IMED_MED_RDWR_DRIVER::IMED_MED_RDWR_DRIVER()
{}

IMED_MED_RDWR_DRIVER::IMED_MED_RDWR_DRIVER(const string & fileName,  MED * const ptrMed):
  IMED_MED_RDONLY_DRIVER(fileName,ptrMed),IMED_MED_WRONLY_DRIVER(fileName,ptrMed),MED_MED_DRIVER(fileName,ptrMed,RDWR)
{}

IMED_MED_RDWR_DRIVER::IMED_MED_RDWR_DRIVER(const IMED_MED_RDWR_DRIVER & driver):
  IMED_MED_RDONLY_DRIVER(driver),IMED_MED_WRONLY_DRIVER(driver),MED_MED_DRIVER(driver)
{}

IMED_MED_RDWR_DRIVER::~IMED_MED_RDWR_DRIVER() { 
  MESSAGE_MED("MED_MED_RDWR_DRIVER::~MED_MED_RDWR_DRIVER() has been destroyed");
}

MED_MED_RDONLY_DRIVER::MED_MED_RDONLY_DRIVER()
{
  MESSAGE_MED("You are using the default constructor of the Med read only Driver and it is 2.1 one");
  _concreteMedDrv = new MED_MED_RDONLY_DRIVER21();
}

MED_MED_RDONLY_DRIVER::MED_MED_RDONLY_DRIVER(const string & fileName,  MED * const ptrMed):IMED_MED_RDONLY_DRIVER(fileName,ptrMed),MED_MED_DRIVER(fileName,ptrMed,RDONLY)
{
  _concreteMedDrv = DRIVERFACTORY::buildMedDriverFromFile(fileName,ptrMed,RDONLY);
}

MED_MED_RDONLY_DRIVER::MED_MED_RDONLY_DRIVER(const MED_MED_RDONLY_DRIVER & driver):MED_MED_DRIVER(driver)
{
}

MED_MED_RDONLY_DRIVER::~MED_MED_RDONLY_DRIVER()
{
}

void MED_MED_RDONLY_DRIVER::write          ( void ) const throw (MEDEXCEPTION)
{
  _concreteMedDrv->write();
}

void MED_MED_RDONLY_DRIVER::writeFrom      ( void ) const throw (MEDEXCEPTION)
{
  _concreteMedDrv->writeFrom();
}

void MED_MED_RDONLY_DRIVER::open() throw (MEDEXCEPTION)
{
  _concreteMedDrv->open();
}

void MED_MED_RDONLY_DRIVER::close()
{
  _concreteMedDrv->close();
}
void MED_MED_RDONLY_DRIVER::read           ( void )
{
  _concreteMedDrv->read();
}

void MED_MED_RDONLY_DRIVER::readFileStruct ( void )
{
  _concreteMedDrv->readFileStruct();
}

GENDRIVER * MED_MED_RDONLY_DRIVER::copy ( void ) const
{
  return new MED_MED_RDONLY_DRIVER(*this);
}

MED_MED_WRONLY_DRIVER::MED_MED_WRONLY_DRIVER()
{
  MESSAGE_MED("You are using the default constructor of the Med write only Driver and it is 2.1 one");

  _concreteMedDrv = new MED_MED_WRONLY_DRIVER21();
}

MED_MED_WRONLY_DRIVER::MED_MED_WRONLY_DRIVER(const string & fileName,  MED * const ptrMed):IMED_MED_WRONLY_DRIVER(fileName,ptrMed),MED_MED_DRIVER(fileName,ptrMed,WRONLY)
{
  _concreteMedDrv = DRIVERFACTORY::buildMedDriverFromFile(fileName,ptrMed,WRONLY);
}

MED_MED_WRONLY_DRIVER::MED_MED_WRONLY_DRIVER(const MED_MED_WRONLY_DRIVER & driver):MED_MED_DRIVER(driver)
{
}

MED_MED_WRONLY_DRIVER::~MED_MED_WRONLY_DRIVER()
{
}

void MED_MED_WRONLY_DRIVER::write          ( void ) const throw (MEDEXCEPTION)
{
  _concreteMedDrv->write();
}

void MED_MED_WRONLY_DRIVER::writeFrom      ( void ) const throw (MEDEXCEPTION)
{
  _concreteMedDrv->writeFrom();
}

void MED_MED_WRONLY_DRIVER::open() throw (MEDEXCEPTION)
{
  _concreteMedDrv->open();
}

void MED_MED_WRONLY_DRIVER::close()
{
  _concreteMedDrv->close();
}

void MED_MED_WRONLY_DRIVER::read           ( void ) throw (MEDEXCEPTION) 
{
  _concreteMedDrv->read();
}

void MED_MED_WRONLY_DRIVER::readFileStruct ( void ) throw (MEDEXCEPTION) 
{
  _concreteMedDrv->readFileStruct();
}

GENDRIVER * MED_MED_WRONLY_DRIVER::copy ( void ) const
{
  return _concreteMedDrv->copy();
}

MED_MED_RDWR_DRIVER::MED_MED_RDWR_DRIVER()
{
  MESSAGE_MED("You are using the default constructor of the Med read write Driver and it is 2.1 one");
  _concreteMedDrv = new MED_MED_RDWR_DRIVER21();
}

MED_MED_RDWR_DRIVER::MED_MED_RDWR_DRIVER(const string & fileName,  MED * const ptrMed):IMED_MED_RDWR_DRIVER(fileName,ptrMed),MED_MED_DRIVER(fileName,ptrMed,RDWR)
{
  _concreteMedDrv = DRIVERFACTORY::buildMedDriverFromFile(fileName,ptrMed,RDWR);
}

MED_MED_RDWR_DRIVER::MED_MED_RDWR_DRIVER(const MED_MED_RDWR_DRIVER & driver):
  MED_MED_DRIVER(driver)
{
}

MED_MED_RDWR_DRIVER::~MED_MED_RDWR_DRIVER()
{
}

void MED_MED_RDWR_DRIVER::write          ( void ) const throw (MEDEXCEPTION)
{
  _concreteMedDrv->write();
}

void MED_MED_RDWR_DRIVER::writeFrom      ( void ) const throw (MEDEXCEPTION)
{
  _concreteMedDrv->writeFrom();
}

void MED_MED_RDWR_DRIVER::open() throw (MEDEXCEPTION)
{
  _concreteMedDrv->open();
}

void MED_MED_RDWR_DRIVER::close()
{
  _concreteMedDrv->close();
}

void MED_MED_RDWR_DRIVER::read           ( void ) throw (MEDEXCEPTION) 
{
  _concreteMedDrv->read();
}

void MED_MED_RDWR_DRIVER::readFileStruct ( void ) throw (MEDEXCEPTION) 
{
  _concreteMedDrv->readFileStruct();
}

GENDRIVER * MED_MED_RDWR_DRIVER::copy ( void ) const
{
  return _concreteMedDrv->copy();
}
