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
#ifndef VTK_MESH_DRIVER_HXX
#define VTK_MESH_DRIVER_HXX

#include "MEDMEM.hxx"

#include <string>
#include <vector>
#include "MEDMEM_define.hxx"
#include "MEDMEM_GenDriver.hxx"

#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Utilities.hxx"

#include <fstream>


/*!

  Driver Vtk for MESH (only for writing).

  Generic part : implement open and close methods.

*/
namespace MEDMEM {
class MESH;
class FAMILY;
class GROUP;
class CONNECTIVITY;
class MEDMEM_EXPORT VTK_MESH_DRIVER : public GENDRIVER
{
protected:

  MESH *   _ptrMesh;
  ofstream *        _vtkFile ;     // The _vtkFile used to write Meshes and Fields to _filename
  string         _meshName;

public :

  /*!
    Constructor.
  */
  VTK_MESH_DRIVER() ;
  /*!
    Constructor.
  */
  VTK_MESH_DRIVER(const string & fileName,  MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  VTK_MESH_DRIVER(const VTK_MESH_DRIVER & driver) ;

  /*!
    Destructor.
  */
  ~VTK_MESH_DRIVER() ;

  void open() ;
  void close() ;

  void openConst() const throw (MEDEXCEPTION);
  void closeConst() const throw (MEDEXCEPTION);

  void write( void ) const throw (MEDEXCEPTION) ;
  void read ( void ) throw (MEDEXCEPTION) ;

  /*!
    Set the name of the MESH asked in file.

    It could be different than the name of the MESH object.
  */
  void   setMeshName(const string & meshName) ;
  /*!
    Get the name of the MESH asked in file.
  */
  string getMeshName() const ;

private:
  GENDRIVER * copy ( void ) const;

};
}

#endif /* VTK_MESH_DRIVER_HXX */
