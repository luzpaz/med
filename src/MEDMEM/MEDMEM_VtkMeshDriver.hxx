#ifndef VTK_MESH_DRIVER_HXX
#define VTK_MESH_DRIVER_HXX

#include <string>
#include <vector>
#include "MEDMEM_define.hxx"
#include "MEDMEM_GenDriver.hxx"

#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "utilities.h"

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
class VTK_MESH_DRIVER : public GENDRIVER
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
};

#endif /* VTK_MESH_DRIVER_HXX */
