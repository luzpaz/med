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
#ifndef ENSIGHT_MED_DRIVER_HXX
#define ENSIGHT_MED_DRIVER_HXX

#include <string>
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_Exception.hxx"

#include "MEDMEM_GenDriver.hxx"

#include <fstream>

using namespace std ;


namespace MEDMEM {
class MESH;
class SUPPORT;
class FIELD_;
class MED;
// This driver pilots within a ENSIGHT class read/write accesses of fields/meshes
class ENSIGHT_MED_DRIVER : public GENDRIVER
{
protected:
  
  MED * const       _ptrMed ;              // Store 'ENSIGHT_DRIVER (0..n)----(1) ENSIGHT' associations
 
  virtual void openConst()  const =0;
  virtual void closeConst() const =0;

public :
  ENSIGHT_MED_DRIVER();
  ENSIGHT_MED_DRIVER(const string & fileName, MED * const ptrMed);
  ENSIGHT_MED_DRIVER(const string & fileName,  MED * const ptrMed,
		 MED_EN::med_mode_acces accessMode);
  ENSIGHT_MED_DRIVER(const ENSIGHT_MED_DRIVER & driver);
  virtual ~ENSIGHT_MED_DRIVER();
  // OPERATEUR DE RECOPIE AVEC _ensightFile ??

  void open();
  void close();
  virtual void write( void ) const = 0 ;
  virtual void read( void ) = 0 ;
  virtual GENDRIVER * copy (void ) const = 0;

};

class ENSIGHT_MED_RDONLY_DRIVER : public virtual ENSIGHT_MED_DRIVER
{
public :
  ENSIGHT_MED_RDONLY_DRIVER();
  ENSIGHT_MED_RDONLY_DRIVER(const string & fileName,  MED * const ptrMed);
  ENSIGHT_MED_RDONLY_DRIVER(const ENSIGHT_MED_RDONLY_DRIVER & driver);
  virtual ~ENSIGHT_MED_RDONLY_DRIVER();
  void openConst()  const;
  void closeConst() const;
  void write          ( void ) const throw (MEDEXCEPTION) ;
  virtual void read           ( void ) ;
//   virtual void readFileStruct ( void ) ;
private:
  ifstream *        _ensightFile;         // The main _ensightFile used to read geom and data _filename
  GENDRIVER * copy ( void ) const;
};

class ENSIGHT_MED_WRONLY_DRIVER : public virtual ENSIGHT_MED_DRIVER
{

public :
  ENSIGHT_MED_WRONLY_DRIVER();
  ENSIGHT_MED_WRONLY_DRIVER(const string & fileName,  MED * const ptrMed);
  ENSIGHT_MED_WRONLY_DRIVER(const ENSIGHT_MED_WRONLY_DRIVER & driver);
  virtual ~ENSIGHT_MED_WRONLY_DRIVER();
  void openConst()  const;
  void closeConst() const;
  void write          ( void ) const throw (MEDEXCEPTION) ;
//   void writeFrom      ( void ) const throw (MEDEXCEPTION) ;
  virtual void read           ( void ) throw (MEDEXCEPTION) ;
//   virtual void readFileStruct ( void ) throw (MEDEXCEPTION) ;
private:
  ofstream *        _ensightFile;         // The main _ensightFile used to write geom and data _filename
  GENDRIVER * copy ( void ) const;
  void writeMesh(MESH * myMesh) const ;
  void writeSupport(SUPPORT * mySupport) const ;
  void writeField(FIELD_ * myField,string name) const ;
};

};


#endif /* ENSIGTH_MED_DRIVER_HXX */
