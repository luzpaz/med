// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "MEDMEM_TypeMeshDriver.hxx"
#include "MEDMEM_DriversDef.hxx"

#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_CellModel.hxx"

using namespace std;
using namespace MEDMEM;

// Every memory allocation made in the MedDriver members function are desallocated in the Mesh destructor 

TYPE_MESH_DRIVER::TYPE_MESH_DRIVER():
  GENDRIVER(),
  _ptrMesh(( MESH *)MED_NULL),
  // A VOIR _medIdt(MED_INVALID),
  _meshName("")
{
}

TYPE_MESH_DRIVER::TYPE_MESH_DRIVER(const std::string & fileName,
                                   MESH * ptrMesh,
                                   MED_EN::med_mode_acces accessMode): 
  GENDRIVER(fileName,accessMode),
  _ptrMesh(ptrMesh),
  // A VOIR _medIdt(MED_INVALID), 
  _meshName("")
{
}
  
TYPE_MESH_DRIVER::TYPE_MESH_DRIVER(const TYPE_MESH_DRIVER & driver): 
  GENDRIVER(driver),
  _ptrMesh(driver._ptrMesh),
  // A VOIR _medIdt(MED_INVALID), 
  _meshName(driver._meshName)
{
}

TYPE_MESH_DRIVER::~TYPE_MESH_DRIVER()
{
}

void TYPE_MESH_DRIVER::open()
  throw (MEDEXCEPTION)
{
  const char * LOC = "TYPE_MESH_DRIVER::open()" ;
  BEGIN_OF(LOC);

//   if (_medIdt > 0) 
//     _status = MED_OPENED; 
//   else {
//     _medIdt = MED_INVALID;
//     _status = MED_CLOSED;
//     throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Could not open file "<<_fileName<<" in mode "<<_accessMode));
//   }
  
  END_OF();
}
  
void TYPE_MESH_DRIVER::close()
  throw (MEDEXCEPTION)
{
  const char * LOC = "TYPE_MESH_DRIVER::close() " ;
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

  END_OF();
}

void    TYPE_MESH_DRIVER::setMeshName(const string & meshName) { _meshName = meshName; };
string  TYPE_MESH_DRIVER::getMeshName() const { return _meshName; };


//---------------------------------- RDONLY PART -------------------------------------------------------------

TYPE_MESH_RDONLY_DRIVER::TYPE_MESH_RDONLY_DRIVER(): TYPE_MESH_DRIVER()
{
}
  
TYPE_MESH_RDONLY_DRIVER::TYPE_MESH_RDONLY_DRIVER(const string & fileName,
                                                 MESH * ptrMesh):
  TYPE_MESH_DRIVER(fileName,ptrMesh,MED_RDONLY)
{ 
  MESSAGE("TYPE_MESH_RDONLY_DRIVER::TYPE_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}
  
TYPE_MESH_RDONLY_DRIVER::TYPE_MESH_RDONLY_DRIVER(const TYPE_MESH_RDONLY_DRIVER & driver): 
  TYPE_MESH_DRIVER(driver)
{
}

TYPE_MESH_RDONLY_DRIVER::~TYPE_MESH_RDONLY_DRIVER()
{
  //MESSAGE("TYPE_MESH_RDONLY_DRIVER::~TYPE_MESH_RDONLY_DRIVER() has been destroyed");
}
  
GENDRIVER * TYPE_MESH_RDONLY_DRIVER::copy(void) const
{
  return new TYPE_MESH_RDONLY_DRIVER(*this);
}

void TYPE_MESH_RDONLY_DRIVER::read(void)
  throw (MEDEXCEPTION)
{
  const char * LOC = "TYPE_MESH_RDONLY_DRIVER::read() : " ;
  BEGIN_OF(LOC);
 
  if (_status!=MED_OPENED)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "The _idt of file " << _fileName << " is : "  <<  " (the file is not opened)." )) ;

  _ptrMesh->_name =  _meshName;
  
 
  END_OF();
}

void TYPE_MESH_RDONLY_DRIVER::write( void ) const
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("TYPE_MESH_RDONLY_DRIVER::write : Can't write with a RDONLY driver !");
}


/*--------------------- WRONLY PART -------------------------------*/

TYPE_MESH_WRONLY_DRIVER::TYPE_MESH_WRONLY_DRIVER():TYPE_MESH_DRIVER()
{
}
  
TYPE_MESH_WRONLY_DRIVER::TYPE_MESH_WRONLY_DRIVER(const string & fileName,
                                                 MESH * ptrMesh):
  TYPE_MESH_DRIVER(fileName,ptrMesh,MED_WRONLY)
{
  MESSAGE("TYPE_MESH_WRONLY_DRIVER::TYPE_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}

TYPE_MESH_WRONLY_DRIVER::TYPE_MESH_WRONLY_DRIVER(const TYPE_MESH_WRONLY_DRIVER & driver): 
  TYPE_MESH_DRIVER(driver)
{
}

TYPE_MESH_WRONLY_DRIVER::~TYPE_MESH_WRONLY_DRIVER()
{
  //MESSAGE("TYPE_MESH_WRONLY_DRIVER::TYPE_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been destroyed");
}

GENDRIVER * TYPE_MESH_WRONLY_DRIVER::copy(void) const
{
  return new TYPE_MESH_WRONLY_DRIVER(*this);
}

void TYPE_MESH_WRONLY_DRIVER::read (void)
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("TYPE_MESH_WRONLY_DRIVER::read : Can't read with a WRONLY driver !");
}

void TYPE_MESH_WRONLY_DRIVER::write(void) const
  throw (MEDEXCEPTION)
{ 
  const char * LOC = "void TYPE_MESH_WRONLY_DRIVER::write(void) const : ";
  BEGIN_OF(LOC);

  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<< "Write Driver isn\'t implemented"));

  END_OF();
} 



/*--------------------- RDWR PART -------------------------------*/

TYPE_MESH_RDWR_DRIVER::TYPE_MESH_RDWR_DRIVER():TYPE_MESH_DRIVER()
{
}

TYPE_MESH_RDWR_DRIVER::TYPE_MESH_RDWR_DRIVER(const string & fileName,
					   MESH * ptrMesh):
  TYPE_MESH_DRIVER(fileName,ptrMesh,MED_RDWR)
{
  MESSAGE("TYPE_MESH_RDWR_DRIVER::TYPE_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}

TYPE_MESH_RDWR_DRIVER::TYPE_MESH_RDWR_DRIVER(const TYPE_MESH_RDWR_DRIVER & driver): 
  TYPE_MESH_RDONLY_DRIVER::TYPE_MESH_DRIVER(driver)
{
}

TYPE_MESH_RDWR_DRIVER::~TYPE_MESH_RDWR_DRIVER() {
  //MESSAGE("TYPE_MESH_RDWR_DRIVER::TYPE_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) has been destroyed");
} 
  
GENDRIVER * TYPE_MESH_RDWR_DRIVER::copy(void) const
{
  return new TYPE_MESH_RDWR_DRIVER(*this);
}

void TYPE_MESH_RDWR_DRIVER::write(void) const
  throw (MEDEXCEPTION)
{
  TYPE_MESH_WRONLY_DRIVER::write();
}
void TYPE_MESH_RDWR_DRIVER::read (void)
  throw (MEDEXCEPTION)
{
  TYPE_MESH_RDONLY_DRIVER::read();
}
