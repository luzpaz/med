#include "MEDMEM_MedMedDriver.hxx"
#include "MEDMEM_Compatibility21_22.hxx"
#include "MEDMEM_MedMedDriver21.hxx"
#include "MEDMEM_MedMedDriver22.hxx"
#include "MEDMEM_DriverFactory.hxx" 
#include "MEDMEM_define.hxx"

using namespace std;
using namespace MED_EN;
using namespace MEDMEM;

MED_MED_DRIVER::MED_MED_DRIVER(): GENDRIVER(), 
                                  _ptrMed((MED * const)MED_NULL),
				  _concreteMedDrv((GENDRIVER *) MED_NULL)
{}

MED_MED_DRIVER::MED_MED_DRIVER(const string & fileName,  MED * const ptrMed):
  GENDRIVER(fileName,MED_EN::MED_RDWR), _ptrMed(ptrMed),
  _concreteMedDrv((GENDRIVER *) MED_NULL)
{
  //_ptrMed->addDriver(*this); // The specific MED driver id is set within the addDriver method.
}

MED_MED_DRIVER::MED_MED_DRIVER(const string & fileName,
			       MED * const ptrMed,
			       MED_EN::med_mode_acces accessMode):
  GENDRIVER(fileName,accessMode), _ptrMed(ptrMed),
  _concreteMedDrv((GENDRIVER *) MED_NULL)
{
}
//REM :  As t'on besoin du champ _status :  _medIdt <-> _status  ?  Oui

MED_MED_DRIVER::MED_MED_DRIVER(const MED_MED_DRIVER & driver):
  GENDRIVER(driver),
  _ptrMed(driver._ptrMed),
  _concreteMedDrv(driver._concreteMedDrv->copy())
{
}

MED_MED_DRIVER::~MED_MED_DRIVER()
{
  MESSAGE("MED_MED_DRIVER::~MED_MED_DRIVER() has been destroyed");
  if (_concreteMedDrv) delete _concreteMedDrv;
}

// ------------- Read Only Part --------------

IMED_MED_RDONLY_DRIVER::IMED_MED_RDONLY_DRIVER():MED_MED_DRIVER()
{
}

IMED_MED_RDONLY_DRIVER::IMED_MED_RDONLY_DRIVER(const string & fileName,  MED * const ptrMed):
  MED_MED_DRIVER(fileName,ptrMed,MED_EN::MED_RDONLY)
{
  MESSAGE("MED_MED_RDONLY_DRIVER::MED_MED_RDONLY_DRIVER(const string & fileName,  MED * const ptrMed) Constructeur read only");
}

IMED_MED_RDONLY_DRIVER::IMED_MED_RDONLY_DRIVER(const IMED_MED_RDONLY_DRIVER & driver):
  MED_MED_DRIVER(driver)
{
}

IMED_MED_RDONLY_DRIVER::~IMED_MED_RDONLY_DRIVER()
{
  MESSAGE("MED_MED_RDONLY_DRIVER::~MED_MED_RDONLY_DRIVER() has been destroyed");
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
  MED_MED_DRIVER(fileName,ptrMed,MED_EN::MED_ECRI)
{}

IMED_MED_WRONLY_DRIVER::IMED_MED_WRONLY_DRIVER(const IMED_MED_WRONLY_DRIVER & driver):
  MED_MED_DRIVER(driver)
{}

IMED_MED_WRONLY_DRIVER::~IMED_MED_WRONLY_DRIVER()
{
  MESSAGE("MED_MED_WRONLY_DRIVER::~MED_MED_WRONLY_DRIVER() has been destroyed");
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
  IMED_MED_RDONLY_DRIVER(fileName,ptrMed),IMED_MED_WRONLY_DRIVER(fileName,ptrMed),MED_MED_DRIVER(fileName,ptrMed,MED_REMP)
{}

IMED_MED_RDWR_DRIVER::IMED_MED_RDWR_DRIVER(const IMED_MED_RDWR_DRIVER & driver):
  IMED_MED_RDONLY_DRIVER(driver),IMED_MED_WRONLY_DRIVER(driver),MED_MED_DRIVER(driver)
{}

IMED_MED_RDWR_DRIVER::~IMED_MED_RDWR_DRIVER() { 
  MESSAGE("MED_MED_RDWR_DRIVER::~MED_MED_RDWR_DRIVER() has been destroyed");
}

MED_MED_RDONLY_DRIVER::MED_MED_RDONLY_DRIVER()
{
  MESSAGE("You are using the default constructor of the Med read only Driver and it is 2.1 one");
  _concreteMedDrv = new MED_MED_RDONLY_DRIVER21();
}

MED_MED_RDONLY_DRIVER::MED_MED_RDONLY_DRIVER(const string & fileName,  MED * const ptrMed)
{
  _concreteMedDrv = DRIVERFACTORY::buildMedDriverFromFile(fileName,ptrMed,MED_LECT);
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
  MESSAGE("You are using the default constructor of the Med write only Driver and it is 2.1 one");

  _concreteMedDrv = new MED_MED_WRONLY_DRIVER21();
}

MED_MED_WRONLY_DRIVER::MED_MED_WRONLY_DRIVER(const string & fileName,  MED * const ptrMed)
{
  _concreteMedDrv = DRIVERFACTORY::buildMedDriverFromFile(fileName,ptrMed,MED_ECRI);
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
  MESSAGE("You are using the default constructor of the Med read write Driver and it is 2.1 one");
  _concreteMedDrv = new MED_MED_RDWR_DRIVER21();
}

MED_MED_RDWR_DRIVER::MED_MED_RDWR_DRIVER(const string & fileName,  MED * const ptrMed)
{
  _concreteMedDrv = DRIVERFACTORY::buildMedDriverFromFile(fileName,ptrMed,MED_REMP);
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
