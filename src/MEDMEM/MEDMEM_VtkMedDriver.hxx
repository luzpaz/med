#ifndef VTK_MED_DRIVER_HXX
#define VTK_MED_DRIVER_HXX

#include <string>
#include "MEDMEM_STRING.hxx"
#include "utilities.h"
#include "MEDMEM_Exception.hxx"

#include "MEDMEM_GenDriver.hxx"


class MESH;
class FIELD_;
class MED;

// This driver pilots within a VTK class read/write accesses of fields/meshes
class VTK_VTK_DRIVER : public GENDRIVER
{
protected:
  
  MED * const       _ptrMED ;     // Store 'VTK_DRIVER (0..n)----(1) VTK' associations
  ofstream          _vtkFile ;     // The _vtkFile used to write Meshes and Fields to _filename
  
private:
  VTK_MED_DRIVER();

public :
  VTK_MED_DRIVER(const string & fileName,  MED * const ptrMed);
  // OPERATEUR DE RECOPIE AVEC _vtkFile ??

  void open();
  void close();

  virtual void write          ( void ) const ;
  //  virtual void writeFrom      ( void ) const ;
  //  virtual void read           ( void ) ;

protected :
  void writeMesh(MESH * myMesh) ;
  void writeField(FIELD_ * myField) ;

};


#endif /* VTK_MED_DRIVER_HXX */
