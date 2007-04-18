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
#include "MEDMEM_EnsightMeshDriver.hxx"

#include <sstream>
#include <strstream>
#include <iomanip>
#include <libgen.h>

#include "MEDMEM_define.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_Grid.hxx"

#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_MedMeshDriver.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

#define MED_NULL     NULL

ENSIGHT_MESH_DRIVER::ENSIGHT_MESH_DRIVER(): GENDRIVER(), 
				    _ptrMesh((MESH *)MED_NULL)
{
}

ENSIGHT_MESH_DRIVER::ENSIGHT_MESH_DRIVER(const string & fileName,
				 MESH * ptrMesh) :
  GENDRIVER(fileName,MED_EN::MED_RDWR), _ptrMesh(ptrMesh)
{
}

ENSIGHT_MESH_DRIVER::ENSIGHT_MESH_DRIVER(const string & fileName,
			       MESH * ptrMesh,
			       MED_EN::med_mode_acces accessMode):
  GENDRIVER(fileName,accessMode), _ptrMesh(ptrMesh)
{
}

ENSIGHT_MESH_DRIVER::ENSIGHT_MESH_DRIVER(const ENSIGHT_MESH_DRIVER & driver): 
  GENDRIVER(driver),
  _ptrMesh(driver._ptrMesh),
  _meshName(driver._meshName)
{
}

ENSIGHT_MESH_DRIVER::~ENSIGHT_MESH_DRIVER()
{
  MESSAGE("ENSIGHT_MESH_DRIVER::~ENSIGHT_MESH_DRIVER() has been destroyed");
}

void    ENSIGHT_MESH_DRIVER::setMeshName(const string & meshName) { _meshName = meshName; };

string  ENSIGHT_MESH_DRIVER::getMeshName() const { return _meshName; };

void ENSIGHT_MESH_DRIVER::open() {
  openConst() ;
}

void ENSIGHT_MESH_DRIVER::close() {
  closeConst() ;
}

ENSIGHT_MESH_WRONLY_DRIVER::ENSIGHT_MESH_WRONLY_DRIVER() : ENSIGHT_MESH_DRIVER()
{
  _ensightFile = new ofstream();
}

ENSIGHT_MESH_WRONLY_DRIVER::ENSIGHT_MESH_WRONLY_DRIVER(const string & fileName,  MESH * ptrMesh) : ENSIGHT_MESH_DRIVER(fileName,ptrMesh)
{
  _ensightFile = new ofstream();
}

ENSIGHT_MESH_WRONLY_DRIVER::ENSIGHT_MESH_WRONLY_DRIVER(const ENSIGHT_MESH_WRONLY_DRIVER & driver) : ENSIGHT_MESH_DRIVER(driver),_support(driver._support)
{
  _ensightFile = new ofstream();
}

ENSIGHT_MESH_WRONLY_DRIVER::~ENSIGHT_MESH_WRONLY_DRIVER()
{
  delete _ensightFile ;
}

GENDRIVER * ENSIGHT_MESH_WRONLY_DRIVER::copy() const
{
  return new ENSIGHT_MESH_WRONLY_DRIVER(*this) ;
}

void ENSIGHT_MESH_WRONLY_DRIVER::openConst() const
{
  const char * LOC = "ENSIGHT_MESH_WRONLY_DRIVER::open()" ;

  BEGIN_OF(LOC);

  if ( _fileName == "" )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "_fileName is |\"\"|, please set a correct fileName before calling open()"
                                     )
                          );

  if (!(*_ensightFile).is_open())
    (*_ensightFile).open(_fileName.c_str()) ; 

  if (!(*_ensightFile))
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not open main Ensight file "
				     << _fileName)
			  );

  END_OF(LOC);
}

void ENSIGHT_MESH_WRONLY_DRIVER::closeConst() const {

  const char * LOC = "ENSIGHT_MESH_WRONLY_DRIVER::close() : ";
  BEGIN_OF(LOC);
  
  (*_ensightFile).close();
 
  if (!(*_ensightFile))
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not close main Ensight file "
				     << _fileName)
			  );
  END_OF(LOC);
}

void ENSIGHT_MESH_WRONLY_DRIVER::read() throw (MEDEXCEPTION) {
  throw MEDEXCEPTION("ENSIGHT_MESH_WRONLY_DRIVER::read : Can't read with a WRONLY driver !");
}

void ENSIGHT_MESH_WRONLY_DRIVER::write() const throw (MEDEXCEPTION)
{ 

  const char * LOC = "ENSIGHT_MESH_WRONLY_DRIVER::write() : ";
  BEGIN_OF(LOC);

  // Well we must open ensight file first, because there are
  // no other driver than MESH for ENSIGHT that do it !
  openConst() ;

  // Ensight case organization requires a main file (filename.case) which defines organization
  // We also need a geom file (filemane.geo) to store the meshs
  // We also need a data file (filemane.data) to store the fields
   
  string MeshName = _ptrMesh->getName() ;
  // In fact, we must take care of all supports 
  // We restrict Field on all nodes or cells
	       
  cout << "-> creating the Ensight case file " << _fileName <<  endl ;
  (*_ensightFile) << "FORMAT" << endl ;
  (*_ensightFile) << "type:   ensight" << endl ;
  (*_ensightFile) << endl ;
  (*_ensightFile) << "GEOMETRY" << endl ;
  int len       = _fileName.size() ;
  string prefix = _fileName.substr(0,len-5); // extraction de .case
  string ensight_geomf = prefix + ".geom" ;
  string basen = basename((char*)ensight_geomf.c_str());
  (*_ensightFile) << "# Mesh detected with name = " << MeshName << endl ;
  (*_ensightFile) << "model: 1 " << basen << endl ;
  (*_ensightFile) << endl ;

  ofstream ensightGeomFile(ensight_geomf.c_str(),ios::out); 
  cout << "-> creating the Ensight geometry file " << ensight_geomf << endl ;

  // ------------ create the Ensight file associated to this meshe
  ensightGeomFile << "Ensight Geometry File : " <<  endl 
		  << "Meshing from MedMemory"  << endl ;
//   ensightGeomFile << "node id given " << endl ;
//   ensightGeomFile << "element id given " << endl ;
  ensightGeomFile << "node id assign " << endl ;
  ensightGeomFile << "element id assign " << endl ;
  ensightGeomFile << "coordinates" << endl ;
  // put points (all point are in 3D, so if we are in 1D or 2D, we complete by zero !
  int SpaceDimension = _ptrMesh->getSpaceDimension() ;
  int NumberOfNodes  = _ptrMesh->getNumberOfNodes() ;
  ensightGeomFile << NumberOfNodes << endl ;
  const double *coordinate = _ptrMesh->getCoordinates(MED_FULL_INTERLACE) ;
  ensightGeomFile.setf(ios::scientific);	
  ensightGeomFile.precision(5);	
  for (int i=0;i<NumberOfNodes;i++) {
//     ensightGeomFile << setw(8) << i+1 ;
    for (int j=0;j<SpaceDimension;j++)
      ensightGeomFile << setw(12) << coordinate[i*SpaceDimension+j] ;
    if (SpaceDimension==1) 
      ensightGeomFile << "0       0" ;
    if (SpaceDimension==2) 
      ensightGeomFile << "0" ;
    ensightGeomFile << endl ;
  }

  // we put connectivity
  // how many cells and how many value in connectivity :
  int cells_types_count = _ptrMesh->getNumberOfTypes(MED_CELL) ;
  const CELLMODEL * cells_type =_ptrMesh->getCellsTypes(MED_CELL) ;
  ensightGeomFile << "part 1 " << endl ;
  ensightGeomFile << "elements are following " << endl ;

  // we put connectivity
  for (int i=0;i<cells_types_count;i++) {
    int numberOfCell = _ptrMesh->getNumberOfElements(MED_CELL,cells_type[i].getType()) ;
    int *filter = (int*) NULL ; 
    switch (cells_type[i].getType())
      {
      case MED_POINT1  : {
        ensightGeomFile << "point" << endl ;
        ensightGeomFile << setw(8) << numberOfCell << endl ;
	filter = new int[1] ;
	filter[0] = 0 ;
        break ;
      }
      case MED_SEG2    : {
        ensightGeomFile << "bar2" << endl ;
        ensightGeomFile << setw(8) << numberOfCell << endl ;
        filter = new int[2] ;
	filter[0] = 0 ;
        filter[1] = 1 ;
        break ;
      }
      case MED_SEG3    : {  
        filter = new int[3] ;
	filter[0] = 0 ;
        filter[1] = 2 ;
        filter[2] = 1 ;
        break ;
      }
      case MED_TRIA3   : {
        ensightGeomFile << "tria3" << endl ;
        ensightGeomFile << setw(8) << numberOfCell << endl ;
        filter = new int[3] ;
        filter[0] = 0 ;
        filter[1] = 2 ;
 	filter[2] = 1 ;
        break ;
      }
      case MED_QUAD4   : {
        ensightGeomFile << "quad4" << endl ;
        ensightGeomFile << setw(8) << numberOfCell << endl ;
        filter = new int[4] ;
        filter[0] = 0 ;
        filter[1] = 3 ;
        filter[2] = 2 ;
  	filter[3] = 1 ;
        break ;
      }
      case MED_TRIA6   : {
        ensightGeomFile << "tria6" << endl ;
        ensightGeomFile << setw(8) << numberOfCell << endl ;
        filter = new int[6] ;
        filter[0] = 0 ;
        filter[1] = 2 ;
 	filter[2] = 1 ;
        filter[3] = 5 ;
        filter[4] = 4 ;
 	filter[5] = 3 ;
        break ;
      }
      case MED_QUAD8   : {
        ensightGeomFile << "quad8" << endl ;
        ensightGeomFile << setw(8) << numberOfCell << endl ;
        filter = new int[8] ;
        filter[0] = 0 ;
        filter[1] = 3 ;
 	filter[2] = 2 ;
        filter[3] = 1 ;
        filter[4] = 7 ;
 	filter[5] = 6 ;
        filter[6] = 5 ;
 	filter[7] = 4 ;
        break ;
      }
      case MED_TETRA4  : {
        ensightGeomFile << "tetra4" << endl ;
        ensightGeomFile << setw(8) << numberOfCell << endl ;
        filter = new int[4] ;
        filter[0] = 0 ;
        filter[1] = 1 ;
        filter[2] = 3 ;  // 3td element in med are 4th in vtk (array begin at 0 !)
        filter[3] = 2 ;  // 4th element in med are 3rd in vtk (array begin at 0 !)
        break ;
      }
      case MED_PYRA5   : {
        ensightGeomFile << "pyramid5" << endl ;
        ensightGeomFile << setw(8) << numberOfCell << endl ;
        filter = new int[5] ;
        filter[0] = 0 ;
        filter[1] = 3 ;  // 2nd element in med are 4th in vtk (array begin at 0 !)
        filter[2] = 2 ;
        filter[3] = 1 ;  // 4th element in med are 2nd in vtk (array begin at 0 !)
  	filter[4] = 4 ;
        break ;
      }
      case MED_PENTA6  : {
        ensightGeomFile << "penta6" << endl ;
        ensightGeomFile << setw(8) << numberOfCell << endl ;
        filter = new int[6] ;
        filter[0] = 0 ;
        filter[1] = 2 ;
        filter[2] = 1 ;
        filter[3] = 3 ;
        filter[4] = 4 ;
  	filter[5] = 5 ;
	break ;
      }
      case MED_HEXA8   : {
        ensightGeomFile << "hexa8" << endl ;
        ensightGeomFile << setw(8) << numberOfCell << endl ;
        filter = new int[8] ;
        filter[0] = 0 ;
        filter[1] = 3 ;
        filter[2] = 2 ;
        filter[3] = 1 ;
        filter[4] = 4 ;
        filter[5] = 7 ;
  	filter[6] = 6 ;
	filter[7] = 5 ;
        break ;
      }
      case MED_TETRA10 : {
        ensightGeomFile << "tetra10" << endl ;
        ensightGeomFile << setw(8) << numberOfCell << endl ;
        filter = new int[10] ;
        filter[0] = 0 ;
        filter[1] = 2 ;
        filter[2] = 1 ;  
        filter[3] = 3 ;  
        filter[4] = 6 ;
        filter[5] = 5 ;
        filter[6] = 4 ;  
        filter[7] = 7 ;  
        filter[8] = 9 ;  
        filter[9] = 8 ;  
        break ;
      }
      case MED_PYRA13  : {
        ensightGeomFile << "pyramid13" << endl ;
        ensightGeomFile << setw(8) << numberOfCell << endl ;
        break ;
      }
      case MED_PENTA15 : {
        ensightGeomFile << "penta15" << endl ;
        ensightGeomFile << setw(8) << numberOfCell << endl ;
        break ;
      }
      case MED_HEXA20  : {
        ensightGeomFile << "hexa20" << endl ;
        ensightGeomFile << setw(8) << numberOfCell << endl ;
        break ;
      }
      default : { 
        break ;
      }
      }
    if (filter==NULL) 
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": MED element type not supported yet : " << cells_type[i].getName() ) ) ;
    int nodes_cell = cells_type[i].getNumberOfNodes();
    const int * connectivityArray = _ptrMesh->getConnectivity(MED_FULL_INTERLACE,MED_NODAL,MED_CELL,cells_type[i].getType());
    for (int j=0;j<numberOfCell;j++) {
      for (int k=0;k<nodes_cell;k++)
	ensightGeomFile << setw(8) << connectivityArray[j*nodes_cell+filter[k]] ;
      ensightGeomFile << endl ;
    }
    if (filter != NULL)
      delete[] filter ;
  }

  for(int i=0;i<_support.size();i++){
    // we put connectivity
    // how many cells and how many value in connectivity :
    int nbTypes = _support[i]->getNumberOfTypes() ;
    const medGeometryElement * geo_type = _support[i]->getTypes() ;

    ensightGeomFile << "part " << i+2 << endl;
    ensightGeomFile << "connectivities description" << endl;

    int nodes_cell;
    // we put connectivity
    for (int i=0;i<nbTypes;i++) {

      int numberOfCell = _support[i]->getNumberOfElements(geo_type[i]) ;
      int *filter = (int*) NULL ; // index in ensight connectivity
      switch (geo_type[i]){
      case MED_POINT1  : {
	nodes_cell=1;
	ensightGeomFile << "point" << endl ;
	ensightGeomFile << setw(8) << numberOfCell << endl ;
	filter = new int[1] ;
	filter[0] = 0 ;
	break ;
      }
      case MED_SEG2    : {
	nodes_cell=2;
	ensightGeomFile << "bar2" << endl ;
	ensightGeomFile << setw(8) << numberOfCell << endl ;
	filter = new int[2] ;
	filter[0] = 0 ;
	filter[1] = 1 ;
	break ;
      }
      case MED_SEG3    : {  
	nodes_cell=3;
	ensightGeomFile << "bar3" << endl ;
	ensightGeomFile << setw(8) << numberOfCell << endl ;
        filter = new int[3] ;
	filter[0] = 0 ;
        filter[1] = 2 ;
        filter[2] = 1 ;
	break ;
      }
      case MED_TRIA3   : {
	nodes_cell=3;
	ensightGeomFile << "tria3" << endl ;
	ensightGeomFile << setw(8) << numberOfCell << endl ;
	filter = new int[3] ;
	filter[0] = 0 ;
	filter[1] = 2 ;
	filter[2] = 1 ;
	break ;
      }
      case MED_QUAD4   : {
	nodes_cell=4;
	ensightGeomFile << "quad4" << endl ;
	ensightGeomFile << setw(8) << numberOfCell << endl ;
	filter = new int[4] ;
	filter[0] = 0 ;
	filter[1] = 3 ;
	filter[2] = 2 ;
	filter[3] = 1 ;
	break ;
      }
      case MED_TRIA6   : {
	nodes_cell=6;
	ensightGeomFile << "tria6" << endl ;
	ensightGeomFile << setw(8) << numberOfCell << endl ;
	filter = new int[6] ;
	filter[0] = 0 ;
	filter[1] = 2 ;
	filter[2] = 1 ;
	filter[3] = 5 ;
	filter[4] = 4 ;
	filter[5] = 3 ;
	break ;
      }
      case MED_QUAD8   : {
	nodes_cell=8;
	ensightGeomFile << "quad8" << endl ;
	ensightGeomFile << setw(8) << numberOfCell << endl ;
	filter = new int[8] ;
	filter[0] = 0 ;
	filter[1] = 3 ;
	filter[2] = 2 ;
	filter[3] = 1 ;
	filter[4] = 7 ;
	filter[5] = 6 ;
	filter[6] = 5 ;
	filter[7] = 4 ;
	break ;
      }
      case MED_TETRA4  : {
	nodes_cell=4;
	ensightGeomFile << "tetra4" << endl ;
	ensightGeomFile << setw(8) << numberOfCell << endl ;
	filter = new int[4] ;
	filter[0] = 0 ;
	filter[1] = 1 ;
	filter[2] = 3 ;  // 3td element in med are 4th in ensight (array begin at 0 !)
	filter[3] = 2 ;  // 4th element in med are 3rd in ensight (array begin at 0 !)
	break ;
      }
      case MED_PYRA5   : {
	nodes_cell=5;
	ensightGeomFile << "pyramid5" << endl ;
	ensightGeomFile << setw(8) << numberOfCell << endl ;
	filter = new int[5] ;
	filter[0] = 0 ;
	filter[1] = 3 ;  // 2nd element in med are 4th in ensight (array begin at 0 !)
	filter[2] = 2 ;
	filter[3] = 1 ;  // 4th element in med are 2nd in ensight (array begin at 0 !)
	filter[4] = 4 ;
	break ;
      }
      case MED_PENTA6  : {
	nodes_cell=6;
	ensightGeomFile << "penta6" << endl ;
	ensightGeomFile << setw(8) << numberOfCell << endl ;
	filter = new int[6] ;
	filter[0] = 0 ;
	filter[1] = 2 ;
	filter[2] = 1 ;
	filter[3] = 3 ;
	filter[4] = 5 ;
	filter[5] = 4 ;
	break ;
      }
      case MED_HEXA8   : {
	nodes_cell=8;
	ensightGeomFile << "hexa8" << endl ;
	ensightGeomFile << setw(8) << numberOfCell << endl ;
	filter = new int[8] ;
	filter[0] = 0 ;
	filter[1] = 3 ;
	filter[2] = 2 ;
	filter[3] = 1 ;
	filter[4] = 4 ;
	filter[5] = 7 ;
	filter[6] = 6 ;
	filter[7] = 5 ;
	break ;
      }
      case MED_TETRA10 : {
	nodes_cell=10;
	ensightGeomFile << "tetra10" << endl ;
	ensightGeomFile << setw(8) << numberOfCell << endl ;
	filter = new int[10] ;
	filter[0] = 0 ;
	filter[1] = 2 ;
	filter[2] = 1 ;  
	filter[3] = 3 ;  
	filter[4] = 6 ;
	filter[5] = 5 ;
	filter[6] = 4 ; 
	filter[7] = 7 ; 
	filter[8] = 9 ;
	filter[9] = 8 ;
	break ;
      }
      case MED_PYRA13  : {
	nodes_cell=13;
	ensightGeomFile << "pyramid13" << endl ;
	ensightGeomFile << setw(8) << numberOfCell << endl ;
	break ;
      }
      case MED_PENTA15 : {
	nodes_cell=15;
	ensightGeomFile << "penta15" << endl ;
	ensightGeomFile << setw(8) << numberOfCell << endl ;
	break ;
      }
      case MED_HEXA20  : {
	nodes_cell=20;
	ensightGeomFile << "hexa20" << endl ;
	ensightGeomFile << setw(8) << numberOfCell << endl ;
	break ;
      }
      default : { 
	break ;
      }
      }
      if (filter==NULL) 
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": MED element type not supported yet : " << cells_type[i].getName() ) ) ;

      const int * connectivityArray = _ptrMesh->getConnectivity(MED_FULL_INTERLACE,MED_NODAL,_support[i]->getEntity(),geo_type[i]);
      const int *type = _support[i]->getNumber(geo_type[i]);

      for (int j=0;j<numberOfCell;j++) {
	for (int k=0;k<nodes_cell;k++){
	  ensightGeomFile << setw(8) << connectivityArray[(type[j]-1)*nodes_cell+filter[k]];
	}
	ensightGeomFile << endl ;
      }
      if (filter != NULL)
	delete[] filter ;

    }

  }

  ensightGeomFile << endl ;
  return ;

  END_OF(LOC);
}

void ENSIGHT_MESH_WRONLY_DRIVER::addSupport(SUPPORT *sup)
{
  _support.push_back(sup);
}

// void ENSIGHT_MESH_WRONLY_DRIVER::writeSupport(SUPPORT * mySupport) const {
//   const char * LOC = "ENSIGHT_MESH_WRONLY_DRIVER::writeSupport(SUPPORT *) : " ;
//   BEGIN_OF(LOC) ;
//   MESSAGE(LOC << "Not yet implemented, acting on the object " << *mySupport);
//   END_OF(LOC) ;
// }

ENSIGHT_MESH_RDONLY_DRIVER::ENSIGHT_MESH_RDONLY_DRIVER() : ENSIGHT_MESH_DRIVER()
{
  _ensightFile = new ifstream();
}

ENSIGHT_MESH_RDONLY_DRIVER::ENSIGHT_MESH_RDONLY_DRIVER(const string & fileName,  MESH * ptrMesh) : ENSIGHT_MESH_DRIVER(fileName,ptrMesh)
{
  _ensightFile = new ifstream();
}

ENSIGHT_MESH_RDONLY_DRIVER::ENSIGHT_MESH_RDONLY_DRIVER(const ENSIGHT_MESH_RDONLY_DRIVER & driver) : ENSIGHT_MESH_DRIVER(driver)
{
  _ensightFile = new ifstream();
}

ENSIGHT_MESH_RDONLY_DRIVER::~ENSIGHT_MESH_RDONLY_DRIVER()
{
  delete _ensightFile ;
}

GENDRIVER * ENSIGHT_MESH_RDONLY_DRIVER::copy() const
{
  return new ENSIGHT_MESH_RDONLY_DRIVER(*this) ;
}

void ENSIGHT_MESH_RDONLY_DRIVER::openConst() const {

  const char * LOC ="ENSIGHT_MESH_RDONLY_DRIVER::open() : ";

  BEGIN_OF(LOC);

  if ( _fileName == "" )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "_fileName is |\"\"|, please set a correct fileName before calling open()"
                                     )
                          );

  if (!(*_ensightFile).is_open())
    (*_ensightFile).open(_fileName.c_str()) ; 

  if (!(*_ensightFile))
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not open main Ensight file "
				     << _fileName)
			  );
  END_OF(LOC);
}

void ENSIGHT_MESH_RDONLY_DRIVER::closeConst() const {

  const char * LOC = "ENSIGHT_MESH_RDONLY_DRIVER::close() : ";
  BEGIN_OF(LOC);
  
  (*_ensightFile).close();
 
  if (!(*_ensightFile))
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not close main Ensight file "
				     << _fileName)
			  );
  END_OF(LOC);
}

void ENSIGHT_MESH_RDONLY_DRIVER::write() const throw (MEDEXCEPTION) {
  throw MEDEXCEPTION("ENSIGHT_MESH_RDONLY_DRIVER::write : Can't write with a RDONLY driver !");
}

void ENSIGHT_MESH_RDONLY_DRIVER::read() {

  const char * LOC = "ENSIGHT_MESH_RDONLY_DRIVER::read() : " ;
  BEGIN_OF(LOC);

  openConst() ;

  string type_Ensight[15] = {
    "point" , "bar2", "bar3" , "tria3" , "tria6" , "quad4" , "quad8" , "tetra4" , "tetra10" , "pyramid5" ,
    "pyramid13" , "hexa8" , "hexa20" , "penta6" , "penta15" };
  int nb_edge[15] = { 1,2,3,3,6,4,8,4,10,5,13,8,20,6,15 };

  vector<string> type_read  ;
  vector<int> nbcell_read   ;
  vector< vector <int> > total_conn ;
  vector<double> var ;

  int number_of_geom ;
  string geom_namefile ;
  string field_namefile ;
  string mot_lu ;
  int geom_given = 0 ;
  int num_coordinate ;
  string type_cell ;
  int number_of_cell ;
  int conn ;
  //----- ?
  int SpaceDimension = 3 ;
  //----- ? 

  int NumberOfTypes  = 0 ;
  int NumberOfNodes ;
  int iType ;

  string mesh_read_name = "EnsightMesh"; // defaut name for the mesh

  // recuperation des arguments du fichier ensight case
  // --------------------------------------------------

  // In this release, the following options are available :
  // For GEOMETRY -> model:

  cout << "****************** READ **************** starting " << endl ;

  ifstream ensightCaseFile(_fileName.c_str(),ios::in);
  cout << "Ensight case file name to read " << _fileName << endl ;
  string diren = dirname((char*)_fileName.c_str());

  if (ensightCaseFile.is_open() )
    { 
      while ( ensightCaseFile >> mot_lu )
	{
	  if ( mot_lu == "GEOMETRY" ) {
	    cout << "geometry detected" << endl ;
	    while ( ensightCaseFile >> mot_lu ){
	      if ( mot_lu == "model:" ) {
// 		ensightCaseFile >> number_of_geom ;
// 		cout << "number of geometries " << number_of_geom << endl ;
		ensightCaseFile >> mot_lu ;
		geom_namefile = mot_lu;
		cout << "name of geometry file : " << geom_namefile << endl ;
		break ;
	      }
	    }	    
	  }
	}
    }
  else
    {
      cout << "Error :  requested file " << ensightCaseFile << " not found " << endl;
      exit( EXIT_FAILURE );
    }
 
  // chargement des noeuds et connectivites necessaires depuis le fichier ensight geom
  // ---------------------------------------------------------------------------------

  _ptrMesh->_name = mesh_read_name ;

  string cgeom_namefile;
  if( diren.length() > 0 )
    cgeom_namefile = diren + '/' + geom_namefile;
  else
    cgeom_namefile = geom_namefile;
  cout << "-> Entering into the geometry file " << geom_namefile << endl  ;
  ifstream ensightGeomFile(cgeom_namefile.c_str(),ios::in);
  if (ensightGeomFile.is_open() )
    {
      while ( ensightGeomFile >> mot_lu ){
	if ( mot_lu == "given" ) geom_given=1 ;
	if ( mot_lu == "coordinates" ) {
//---------------- Nodes part --------------------------------------------------
	  ensightGeomFile >> NumberOfNodes ;
	  cout << "-> loading " << NumberOfNodes << " coordinates " << endl ;
	  int NumberOfCoordinates = NumberOfNodes*SpaceDimension ;
	  double* Coordinates = new double[NumberOfCoordinates];
	  int iCoord = 0 ;
	  //cout << "-> geom given " << geom_given << endl ;
	  for ( int i=0 ; i < NumberOfNodes ; i++) {
	    if( geom_given) ensightGeomFile >> setw(8) >> num_coordinate ;
	    ensightGeomFile >> setw(12) >> Coordinates[iCoord]   ;
	    ensightGeomFile >> setw(12) >> Coordinates[iCoord+1] ; 
	    ensightGeomFile >> setw(12) >> Coordinates[iCoord+2] ;
// 	    cout << "coordinate read " << num_coordinate << " : x = " << Coordinates[iCoord] << " y = " << Coordinates[iCoord+1] << " z = " << Coordinates[iCoord+2] << endl ;
	    iCoord+=3 ;
	  }
	  _ptrMesh->_spaceDimension = SpaceDimension;
	  _ptrMesh->_numberOfNodes = NumberOfNodes;
	  _ptrMesh->_coordinate =  new COORDINATE(SpaceDimension,NumberOfNodes,MED_EN::MED_FULL_INTERLACE);
	  _ptrMesh->_coordinate->setCoordinates(MED_EN::MED_FULL_INTERLACE,Coordinates);
	  _ptrMesh->_coordinate->setCoordinatesSystem("CARTESIAN");
	  delete [] Coordinates;
	}
	else if  ( mot_lu == "part" ) {
//---------------- Connectivities part --------------------------------------------
	  while ( ensightGeomFile >> mot_lu ){
	    for ( int j = 0 ; j < 15 ; j++){
	      if( mot_lu == type_Ensight[j] ) {
		NumberOfTypes+=1;
		iType=NumberOfTypes-1 ;
		total_conn.resize(NumberOfTypes) ;
		type_read.push_back(mot_lu) ;
		ensightGeomFile >> number_of_cell ;
		nbcell_read.push_back(number_of_cell) ;
		total_conn[iType].resize(nb_edge[j]*number_of_cell);
		cout << "-> loading " << number_of_cell << " cells connectivities of type " << type_Ensight[j] << " (" << nb_edge[j]*number_of_cell << ") values " << endl ;
		for ( int k=0 ; k < nb_edge[j]*number_of_cell ; k++ ) {
		  ensightGeomFile >> setw(8) >> conn ;
		  total_conn[iType][k]=conn ;
		  // cout << " connectivitie " << k << " read = " << total_conn[iType][k] << endl ; 
		}
	      }    
	    }
	  }
	}	  
      }
    }

    medGeometryElement classicalTypesCell[NumberOfTypes];
    int nbOfClassicalTypesCell[NumberOfTypes];

    int ind=0 ;
    for (int k=0 ; k<NumberOfTypes ; k++){
      for (int j=0 ; j<15 ; j++)
	if(type_read[k] == type_Ensight[j] ){
	  switch ( j+1 )
	    {
	    case 1  : {classicalTypesCell[ind] = MED_EN::MED_POINT1  ; nbOfClassicalTypesCell[ind] = nbcell_read[k] ; ind++ ; break ;}
	    case 2  : {classicalTypesCell[ind] = MED_EN::MED_SEG2    ; nbOfClassicalTypesCell[ind] = nbcell_read[k] ; ind++ ; break ;}
	    case 3  : {classicalTypesCell[ind] = MED_EN::MED_SEG3    ; nbOfClassicalTypesCell[ind] = nbcell_read[k] ; ind++ ; break ;}
	    case 4  : {classicalTypesCell[ind] = MED_EN::MED_TRIA3   ; nbOfClassicalTypesCell[ind] = nbcell_read[k] ; ind++ ; break ;}
	    case 5  : {classicalTypesCell[ind] = MED_EN::MED_TRIA6   ; nbOfClassicalTypesCell[ind] = nbcell_read[k] ; ind++ ; break ;}
	    case 6  : {classicalTypesCell[ind] = MED_EN::MED_QUAD4   ; nbOfClassicalTypesCell[ind] = nbcell_read[k] ; ind++ ; break ;}
	    case 7  : {classicalTypesCell[ind] = MED_EN::MED_QUAD8   ; nbOfClassicalTypesCell[ind] = nbcell_read[k] ; ind++ ; break ;}
	    case 8  : {classicalTypesCell[ind] = MED_EN::MED_TETRA4  ; nbOfClassicalTypesCell[ind] = nbcell_read[k] ; ind++ ; break ;}
	    case 9  : {classicalTypesCell[ind] = MED_EN::MED_TETRA10 ; nbOfClassicalTypesCell[ind] = nbcell_read[k] ; ind++ ; break ;}
	    case 10 : {classicalTypesCell[ind] = MED_EN::MED_PYRA5   ; nbOfClassicalTypesCell[ind] = nbcell_read[k] ; ind++ ; break ;}
	    case 11 : {classicalTypesCell[ind] = MED_EN::MED_PYRA13  ; nbOfClassicalTypesCell[ind] = nbcell_read[k] ; ind++ ; break ;}
	    case 12 : {classicalTypesCell[ind] = MED_EN::MED_HEXA8   ; nbOfClassicalTypesCell[ind] = nbcell_read[k] ; ind++ ; break ;}
	    case 13 : {classicalTypesCell[ind] = MED_EN::MED_HEXA20  ; nbOfClassicalTypesCell[ind] = nbcell_read[k] ; ind++ ; break ;}
	    case 14 : {classicalTypesCell[ind] = MED_EN::MED_PENTA6  ; nbOfClassicalTypesCell[ind] = nbcell_read[k] ; ind++ ; break ;}
	    case 15 : {classicalTypesCell[ind] = MED_EN::MED_PENTA15 ; nbOfClassicalTypesCell[ind] = nbcell_read[k] ; ind++ ; break ;}
	    default : break ;
	    }
	}
    }

    _ptrMesh->_connectivity = new CONNECTIVITY(NumberOfTypes,MED_EN::MED_CELL);
    _ptrMesh->_connectivity->setGeometricTypes(classicalTypesCell,MED_EN::MED_CELL);
    int * Count = new int[NumberOfTypes+1] ;
    Count[0]=1 ;
    for (int i=0; i<NumberOfTypes; i++)
      Count[i+1]=Count[i]+nbOfClassicalTypesCell[i] ;
    _ptrMesh->_connectivity->setCount(Count,MED_EN::MED_CELL) ;
    delete[] Count ;
    int MeshDimension ;
    MeshDimension = 2 ;
    for (int k=0 ; k<NumberOfTypes ; k++){
      for (int j=0 ; j<15 ; j++)
	if(type_read[k] == type_Ensight[j] && j>6 ) MeshDimension = 3 ; 
    }
    _ptrMesh->_meshDimension = MeshDimension;
    _ptrMesh->_connectivity->setEntityDimension(MeshDimension);

    for (int k = 0 ; k < NumberOfTypes ; k++) {
      int nb_connectivities = total_conn[k].size();
      int *connectivities = new int[nb_connectivities];
      for (int itab=0 ; itab < nb_connectivities ; itab++) connectivities[itab]=total_conn[k][itab] ;
      for (int j=0 ; j<15 ; j++) {
	if( type_read[k] == type_Ensight[j] ) {
	  switch ( j+1 )
	    {
	    case 1  : {_ptrMesh->_connectivity->setNodal(connectivities,MED_EN::MED_CELL,MED_EN::MED_POINT1)  ; break ;}
	    case 2  : {_ptrMesh->_connectivity->setNodal(connectivities,MED_EN::MED_CELL,MED_EN::MED_SEG2)    ; break ;}
	    case 3  : {_ptrMesh->_connectivity->setNodal(connectivities,MED_EN::MED_CELL,MED_EN::MED_SEG3)    ; break ;}
	    case 4  : {_ptrMesh->_connectivity->setNodal(connectivities,MED_EN::MED_CELL,MED_EN::MED_TRIA3)   ; break ;}
	    case 5  : {_ptrMesh->_connectivity->setNodal(connectivities,MED_EN::MED_CELL,MED_EN::MED_TRIA6)   ; break ;}
	    case 6  : {_ptrMesh->_connectivity->setNodal(connectivities,MED_EN::MED_CELL,MED_EN::MED_QUAD4)   ; break ;}
	    case 7  : {_ptrMesh->_connectivity->setNodal(connectivities,MED_EN::MED_CELL,MED_EN::MED_QUAD8)   ; break ;}
	    case 8  : {_ptrMesh->_connectivity->setNodal(connectivities,MED_EN::MED_CELL,MED_EN::MED_TETRA4)  ; break ;}
	    case 9  : {_ptrMesh->_connectivity->setNodal(connectivities,MED_EN::MED_CELL,MED_EN::MED_TETRA10) ; break ;}
	    case 10 : {_ptrMesh->_connectivity->setNodal(connectivities,MED_EN::MED_CELL,MED_EN::MED_PYRA5)   ; break ;}
	    case 11 : {_ptrMesh->_connectivity->setNodal(connectivities,MED_EN::MED_CELL,MED_EN::MED_PYRA13)  ; break ;}
	    case 12 : {_ptrMesh->_connectivity->setNodal(connectivities,MED_EN::MED_CELL,MED_EN::MED_HEXA8)   ; break ;}
	    case 13 : {_ptrMesh->_connectivity->setNodal(connectivities,MED_EN::MED_CELL,MED_EN::MED_HEXA20)  ; break ;}
	    case 14 : {_ptrMesh->_connectivity->setNodal(connectivities,MED_EN::MED_CELL,MED_EN::MED_PENTA6)  ; break ;}
	    case 15 : {_ptrMesh->_connectivity->setNodal(connectivities,MED_EN::MED_CELL,MED_EN::MED_PENTA15) ; break ;}
	    default : break ;
	    }
	}
      }
      delete [] connectivities;
    }

//     cout << "Impression de _ptrmesh dans EnsightMeshDriver: " << endl;
//     cout << *_ptrMesh ;

  END_OF(LOC);
}

