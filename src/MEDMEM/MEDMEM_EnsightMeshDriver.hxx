//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
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
#ifndef ENSIGHT_MESH_DRIVER_HXX
#define ENSIGHT_MESH_DRIVER_HXX

#include <string>
#include <vector>
#include "MEDMEM_define.hxx"
#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_Support.hxx"

#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Utilities.hxx"

#include <fstream>

using namespace std ;

/*!

  Driver Ensight for MESH ( writing and reading ).

  Generic part : implement open and close methods.

*/
namespace MEDMEM {
class MESH;
class FAMILY;
class GROUP;
class CONNECTIVITY;
class ENSIGHT_MESH_DRIVER : public GENDRIVER
{
protected:

  MESH *  _ptrMesh;
  string   _meshName;

  virtual void openConst()  const =0;
  virtual void closeConst() const =0;

public :
  ENSIGHT_MESH_DRIVER() ;
  ENSIGHT_MESH_DRIVER(const string & fileName,  MESH * ptrMesh) ;
  ENSIGHT_MESH_DRIVER(const string & fileName,  MESH * ptrMesh,
		 MED_EN::med_mode_acces accessMode);
  ENSIGHT_MESH_DRIVER(const ENSIGHT_MESH_DRIVER & driver) ;
  virtual ~ENSIGHT_MESH_DRIVER() ;

  void open();
  void close();
  virtual void write( void ) const = 0 ;
  virtual void read( void ) = 0 ;
  virtual GENDRIVER * copy (void ) const = 0;

  void   setMeshName(const string & meshName) ;
  string getMeshName() const ;

};

class ENSIGHT_MESH_RDONLY_DRIVER : public virtual ENSIGHT_MESH_DRIVER
{
public :
  ENSIGHT_MESH_RDONLY_DRIVER();
  ENSIGHT_MESH_RDONLY_DRIVER(const string & fileName,  MESH * ptrMesh);
  ENSIGHT_MESH_RDONLY_DRIVER(const ENSIGHT_MESH_RDONLY_DRIVER & driver);
  virtual ~ENSIGHT_MESH_RDONLY_DRIVER();
  void openConst()  const;
  void closeConst() const;
  void write          ( void ) const throw (MEDEXCEPTION) ;
  void read           ( void ) ;
private:
  ifstream *        _ensightFile;         // The main _ensightFile used to read geom and data _filename
  GENDRIVER * copy ( void ) const;
};

class ENSIGHT_MESH_WRONLY_DRIVER : public virtual ENSIGHT_MESH_DRIVER
{
public :
  ENSIGHT_MESH_WRONLY_DRIVER();
  ENSIGHT_MESH_WRONLY_DRIVER(const string & fileName,  MESH * ptrMesh);
  ENSIGHT_MESH_WRONLY_DRIVER(const ENSIGHT_MESH_WRONLY_DRIVER & driver);
  virtual ~ENSIGHT_MESH_WRONLY_DRIVER();
  void openConst()  const;
  void closeConst() const;
  void write          ( void ) const throw (MEDEXCEPTION) ;
//   void writeFrom      ( void ) const throw (MEDEXCEPTION) ;
  void read           ( void ) throw (MEDEXCEPTION) ;
//   virtual void readFileStruct ( void ) throw (MEDEXCEPTION) ;
  // add one support
  void addSupport(SUPPORT *sup);

private:
  ofstream *        _ensightFile;         // The main _ensightFile used to write geom and data _filename
  GENDRIVER * copy ( void ) const;
  vector<SUPPORT*> _support;

//   void writeMesh(MESH * myMesh,int imesh) const ;
//   void writeSupport(SUPPORT * mySupport)  const ;
};

};

#endif /* ENSIGHT_MESH_DRIVER_HXX */

