//  MED MEDMEM : MED files in memory
//
//  Copyright (C) 2003  CEA/DEN, EDF R&D
//
//
//
//  File   : MEDMEM_GibiMeshDriver.cxx
//  Module : MED

using namespace std;
#include "MEDMEM_GibiMeshDriver.hxx"

#include "MEDMEM_DriversDef.hxx"

#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_CellModel.hxx"

// Every memory allocation made in the MedDriver members function are desallocated in the Mesh destructor 

GIBI_MESH_DRIVER::GIBI_MESH_DRIVER():
  GENDRIVER(),
  _ptrMesh(( MESH *)MED_NULL),
  // A VOIR _medIdt(MED_INVALID),
  _meshName("")
{
}

GIBI_MESH_DRIVER::GIBI_MESH_DRIVER(const string & fileName,
                                   MESH * ptrMesh,
                                   MED_EN::med_mode_acces accessMode): 
  GENDRIVER(fileName,accessMode),
  _ptrMesh(ptrMesh),
  // A VOIR _medIdt(MED_INVALID), 
  _meshName("")
{
}
  
GIBI_MESH_DRIVER::GIBI_MESH_DRIVER(const GIBI_MESH_DRIVER & driver): 
  GENDRIVER(driver),
  _ptrMesh(driver._ptrMesh),
  // A VOIR _medIdt(MED_INVALID), 
  _meshName(driver._meshName)
{
}

GIBI_MESH_DRIVER::~GIBI_MESH_DRIVER()
{
}

void GIBI_MESH_DRIVER::open()
  throw (MEDEXCEPTION)
{
  const char * LOC = "GIBI_MESH_DRIVER::open()" ;
  BEGIN_OF(LOC);

//   if (_medIdt > 0) 
//     _status = MED_OPENED; 
//   else {
//     _medIdt = MED_INVALID;
//     _status = MED_CLOSED;
//     throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Could not open file "<<_fileName<<" in mode "<<_accessMode));
//   }
  
  END_OF(LOC);
}
  
void GIBI_MESH_DRIVER::close()
  throw (MEDEXCEPTION)
{
  const char * LOC = "GIBI_MESH_DRIVER::close() " ;
  BEGIN_OF(LOC);

//   int err = 0;
//   if ( _status == MED_OPENED) {
//     err=MED_FR::MEDfermer(_medIdt);
//     H5close(); // If we call H5close() all the files are closed.
//     if (err != 0)
//       throw MEDEXCEPTION( LOCALIZED(STRING(LOC)
// 				    <<" Error when closing file !"
// 				    )
// 			  );
//     _status = MED_CLOSED;
//     _medIdt = MED_INVALID;
//   }

  END_OF(LOC);
}

void    GIBI_MESH_DRIVER::setMeshName(const string & meshName) { _meshName = meshName; };
string  GIBI_MESH_DRIVER::getMeshName() const { return _meshName; };


//---------------------------------- RDONLY PART -------------------------------------------------------------

GIBI_MESH_RDONLY_DRIVER::GIBI_MESH_RDONLY_DRIVER(): GIBI_MESH_DRIVER()
{
}
  
GIBI_MESH_RDONLY_DRIVER::GIBI_MESH_RDONLY_DRIVER(const string & fileName,
                                                 MESH * ptrMesh):
  GIBI_MESH_DRIVER(fileName,ptrMesh,MED_RDONLY)
{ 
  MESSAGE("GIBI_MESH_RDONLY_DRIVER::GIBI_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}
  
GIBI_MESH_RDONLY_DRIVER::GIBI_MESH_RDONLY_DRIVER(const GIBI_MESH_RDONLY_DRIVER & driver): 
  GIBI_MESH_DRIVER(driver)
{
}

GIBI_MESH_RDONLY_DRIVER::~GIBI_MESH_RDONLY_DRIVER()
{
  //MESSAGE("GIBI_MESH_RDONLY_DRIVER::~GIBI_MESH_RDONLY_DRIVER() has been destroyed");
}
  
GENDRIVER * GIBI_MESH_RDONLY_DRIVER::copy(void) const
{
  return new GIBI_MESH_RDONLY_DRIVER(*this);
}

void GIBI_MESH_RDONLY_DRIVER::read(void)
  throw (MEDEXCEPTION)
{
  const char * LOC = "GIBI_MESH_RDONLY_DRIVER::read() : " ;
  BEGIN_OF(LOC);
 
  if (_status!=MED_OPENED)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "The _idt of file " << _fileName << " is : "  <<  " (the file is not opened)." )) ;

  _ptrMesh->_name =  _meshName;
  
 
  END_OF(LOC);
}

void GIBI_MESH_RDONLY_DRIVER::write( void ) const
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("GIBI_MESH_RDONLY_DRIVER::write : Can't write with a RDONLY driver !");
}


/*--------------------- WRONLY PART -------------------------------*/

GIBI_MESH_WRONLY_DRIVER::GIBI_MESH_WRONLY_DRIVER():GIBI_MESH_DRIVER()
{
}
  
GIBI_MESH_WRONLY_DRIVER::GIBI_MESH_WRONLY_DRIVER(const string & fileName,
                                                 MESH * ptrMesh):
  GIBI_MESH_DRIVER(fileName,ptrMesh,MED_WRONLY)
{
  MESSAGE("GIBI_MESH_WRONLY_DRIVER::GIBI_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}

GIBI_MESH_WRONLY_DRIVER::GIBI_MESH_WRONLY_DRIVER(const GIBI_MESH_WRONLY_DRIVER & driver): 
  GIBI_MESH_DRIVER(driver)
{
}

GIBI_MESH_WRONLY_DRIVER::~GIBI_MESH_WRONLY_DRIVER()
{
  //MESSAGE("GIBI_MESH_WRONLY_DRIVER::GIBI_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been destroyed");
}

GENDRIVER * GIBI_MESH_WRONLY_DRIVER::copy(void) const
{
  return new GIBI_MESH_WRONLY_DRIVER(*this);
}

void GIBI_MESH_WRONLY_DRIVER::read (void)
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("GIBI_MESH_WRONLY_DRIVER::read : Can't read with a WRONLY driver !");
}

void GIBI_MESH_WRONLY_DRIVER::write(void) const
  throw (MEDEXCEPTION)
{ 
  const char * LOC = "void GIBI_MESH_WRONLY_DRIVER::write(void) const : ";
  BEGIN_OF(LOC);

  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<< "Write Driver isn\'t implemented"));

  END_OF(LOC);
} 



/*--------------------- RDWR PART -------------------------------*/

GIBI_MESH_RDWR_DRIVER::GIBI_MESH_RDWR_DRIVER():GIBI_MESH_DRIVER()
{
}

GIBI_MESH_RDWR_DRIVER::GIBI_MESH_RDWR_DRIVER(const string & fileName,
					   MESH * ptrMesh):
  GIBI_MESH_DRIVER(fileName,ptrMesh,MED_RDWR)
{
  MESSAGE("GIBI_MESH_RDWR_DRIVER::GIBI_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}

GIBI_MESH_RDWR_DRIVER::GIBI_MESH_RDWR_DRIVER(const GIBI_MESH_RDWR_DRIVER & driver): 
  GIBI_MESH_RDONLY_DRIVER::GIBI_MESH_DRIVER(driver)
{
}

GIBI_MESH_RDWR_DRIVER::~GIBI_MESH_RDWR_DRIVER() {
  //MESSAGE("GIBI_MESH_RDWR_DRIVER::GIBI_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) has been destroyed");
} 
  
GENDRIVER * GIBI_MESH_RDWR_DRIVER::copy(void) const
{
  return new GIBI_MESH_RDWR_DRIVER(*this);
}

void GIBI_MESH_RDWR_DRIVER::write(void) const
  throw (MEDEXCEPTION)
{
  GIBI_MESH_WRONLY_DRIVER::write();
}
void GIBI_MESH_RDWR_DRIVER::read (void)
  throw (MEDEXCEPTION)
{
  GIBI_MESH_RDONLY_DRIVER::read();
}
