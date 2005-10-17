#ifndef VTK_MED_DRIVER_HXX
#define VTK_MED_DRIVER_HXX

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
// This driver pilots within a VTK class read/write accesses of fields/meshes
class VTK_MED_DRIVER : public GENDRIVER
{
protected:
  
  MED * const       _ptrMed ;     // Store 'VTK_DRIVER (0..n)----(1) VTK' associations
  ofstream *        _vtkFile ;     // The _vtkFile used to write Meshes and Fields to _filename
  
private:
  VTK_MED_DRIVER();

  //writeField(FIELD * myField) ;
  //writeMesh(MESH * myMesh) ;

  void open()   ;
  void close()  ;
  void openConst()  const ;
  void closeConst() const ;

public :
  VTK_MED_DRIVER(const string & fileName,  MED * const ptrMed);
  VTK_MED_DRIVER(const VTK_MED_DRIVER & driver);
  ~VTK_MED_DRIVER();
  // OPERATEUR DE RECOPIE AVEC _vtkFile ??

  //virtual void write          ( void )  ;
  void write                 ( void ) const ;
  virtual void read          ( void )  {} ;
  //  virtual void writeFrom      ( void ) const ;
  //  virtual void read           ( void ) ;
  GENDRIVER * copy (void ) const ;

private :
  void writeMesh(MESH * myMesh) const ;
  void writeSupport(SUPPORT * mySupport) const ;
  void writeField(FIELD_ * myField,string name) const ;

};
};


#endif /* VTK_MED_DRIVER_HXX */
