#ifndef MED_MESH_DRIVER_HXX
#define MED_MESH_DRIVER_HXX

#include <string>
#include <vector>
#include "MEDMEM_define.hxx"
#include "MEDMEM_GenDriver.hxx"

#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "utilities.h"

class MESH;
class FAMILY;
class GROUP;
class CONNECTIVITY;

class MED_MESH_DRIVER : public GENDRIVER
{
protected:
  
  MESH *   _ptrMesh;
  MED_FR::med_idt        _medIdt;
  string         _meshName;    // const ?
  int            _meshNum;     // INUTILE ?
  
public :

  // all MED cell type
  static const MED_FR::med_geometrie_element all_cell_type[MED_NBR_GEOMETRIE_MAILLE];
  
  static const char * const all_cell_type_tab [MED_NBR_GEOMETRIE_MAILLE];
 
  MED_MESH_DRIVER():GENDRIVER(),
                    _ptrMesh(( MESH *)MED_NULL),_medIdt(MED_INVALID),
                    _meshName(""),_meshNum(MED_INVALID)
  {
  }
  MED_MESH_DRIVER(const string & fileName,  MESH * ptrMesh, med_mode_acces accessMode): 
    GENDRIVER(fileName,accessMode),
    _ptrMesh(ptrMesh),_medIdt(MED_INVALID), 
    _meshName(""),_meshNum(MED_INVALID) 
  {
  }
  
  void open() {
    BEGIN_OF("MED_MESH_DRIVER::open()");
    _medIdt = MED_FR::MEDouvrir( (const_cast <char *> (_fileName.c_str())),(MED_FR::med_mode_acces) _accessMode);
    MESSAGE("MED_MESH_DRIVER::open() _medIdt : "<< _medIdt );
    if (_medIdt > 0) _status=MED_OPENED; else {
      MESSAGE(" MED_MESH__DRIVER::open() : MEDouvrir : _medIdt= " << _medIdt );
      _status = MED_INVALID;
    }
    END_OF("MED_MESH_DRIVER::open()");
  }
  
  void close() {
    const char * LOC = "MED_MESH_DRIVER::close() " ;
    BEGIN_OF(LOC);
    med_int err = 0;
    if ( _status == MED_OPENED) {
      err=MED_FR::MEDfermer(_medIdt);
      H5close();
      if (err != 0)
	throw MEDEXCEPTION( LOCALIZED(STRING(LOC)
				      <<" Error when closing file !"
				      )
			    );
      MESSAGE(LOC <<": _medIdt= " << _medIdt );
      MESSAGE(LOC<<": MEDfermer : err    = " << err );
      _status = MED_CLOSED;
      _medIdt = MED_INVALID;
    }
    END_OF(LOC);
  }

  virtual void write( void ) const = 0 ;
  virtual void read ( void ) = 0 ;
 
  void   setMeshName(const string & meshName) ;
  string getMeshName() const ;

};

class MED_MESH_RDONLY_DRIVER : public virtual MED_MESH_DRIVER
{
 
public :
  
  MED_MESH_RDONLY_DRIVER():MED_MESH_DRIVER() {};
  
  MED_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh):
    MED_MESH_DRIVER(fileName,ptrMesh,MED_RDONLY) { 
    MESSAGE("MED_MESH_RDONLY_DRIVER::MED_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
  }
  
  ~MED_MESH_RDONLY_DRIVER() { 
    MESSAGE("MED_MESH_RDONLY_DRIVER::~MED_MESH_RDONLY_DRIVER() has been destroyed");
  } 
  
  // CREER UNE METHODE POUR LIRE LA LISTE DES MAILLAGES .....
  int getCOORDINATE();
  int getCONNECTIVITY();
  int getFAMILY();
  void write( void ) const ;
  void read ( void ) ;

private:
  int getNodalConnectivity(CONNECTIVITY * Connectivity) ;
  int getDescendingConnectivity(CONNECTIVITY * Connectivity) ;
  int getNodesFamiliesNumber() ;
  int getCellsFamiliesNumber(int** Arrays, CONNECTIVITY* Connectivity) ;
  void updateFamily() ;
  void buildAllGroups(vector<GROUP*> & Groups, vector<FAMILY*> & Families) ;
  
};

class MED_MESH_WRONLY_DRIVER : public virtual MED_MESH_DRIVER {
  
public :
  
  MED_MESH_WRONLY_DRIVER():MED_MESH_DRIVER() {}
  
  MED_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh):
    MED_MESH_DRIVER(fileName,ptrMesh,MED_WRONLY)
  {
    MESSAGE("MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
  };

  ~MED_MESH_WRONLY_DRIVER() {  
    MESSAGE("MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been destroyed");
  }

  void write( void ) const ;
  void read ( void ) ;

  int writeCoordinates    ()                         const;
  int writeConnectivities (medEntityMesh entity)     const;
  int writeFamilyNumbers  ()                         const;
  int writeFamilies       (vector<FAMILY*> & families) const;
};


class MED_MESH_RDWR_DRIVER : public MED_MESH_RDONLY_DRIVER, public MED_MESH_WRONLY_DRIVER {

public :

  MED_MESH_RDWR_DRIVER():MED_MESH_DRIVER() {}

  MED_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh):
    MED_MESH_DRIVER(fileName,ptrMesh,MED_RDWR)
  {
    MESSAGE("MED_MESH_RDWR_DRIVER::MED_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
  };

  ~MED_MESH_RDWR_DRIVER() {
    MESSAGE("MED_MESH_RDWR_DRIVER::MED_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) has been destroyed");
  } 
  
  void write(void) const ;
  void read (void)       ;
};


#endif /* MED_MESH_DRIVER_HXX */
