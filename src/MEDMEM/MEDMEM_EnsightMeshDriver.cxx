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
#include <iomanip>
#include "MEDMEM_DriversDef.hxx"

#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_Grid.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

#define MED_NULL     NULL

ENSIGHT_MESH_DRIVER::ENSIGHT_MESH_DRIVER(): GENDRIVER(), 
				    _ptrMesh((MESH * const)MED_NULL)
{
  _ensightFile = new ofstream();
  // What about _id in Gendriver ?
  // _driverType ???
}

ENSIGHT_MESH_DRIVER::ENSIGHT_MESH_DRIVER(const string & fileName,
				 MESH * ptrMesh) :
  GENDRIVER(fileName,MED_WRONLY),
  _ptrMesh(ptrMesh)
{
  const char * LOC = "ENSIGHT_MESH_DRIVER::ENSIGHT_MESH_DRIVER(const string & fileName, MESH * ptrMesh) : " ;

  //  _ptrMesh->addDriver(*this); // OU RECUPERER L'ID.
  MESSAGE(LOC << "WARNING this driver is only used to write in Ensight format So the object can not be instantied using a file!");

  _ensightFile = new ofstream(); 
}

ENSIGHT_MESH_DRIVER::ENSIGHT_MESH_DRIVER(const ENSIGHT_MESH_DRIVER & driver): 
  GENDRIVER(driver),
  _ptrMesh(driver._ptrMesh),
  _meshName(driver._meshName),
  _support(driver._support)
{
  _ensightFile = new ofstream();
}

ENSIGHT_MESH_DRIVER::~ENSIGHT_MESH_DRIVER()
{
  const char * LOC ="ENSIGHT_MESH_DRIVER::~ENSIGHT_MESH_DRIVER()";
  BEGIN_OF(LOC);

  close();

  SCRUTE(_ensightFile);

  delete _ensightFile ;

  SCRUTE(_ensightFile);

  END_OF(LOC);
}

void ENSIGHT_MESH_DRIVER::openConst() const throw (MEDEXCEPTION)
{
  const char * LOC = "ENSIGHT_MESH_DRIVER::openConst()" ;
  BEGIN_OF(LOC);

  MESSAGE(LOC<<" : _fileName.c_str : "<< _fileName.c_str()<<",mode : "<< _accessMode);

  if ( _fileName == "" )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "_fileName is |\"\"|, please set a correct fileName before calling open()"
                                     )
                          );

  if (!(*_ensightFile).is_open())
    (*_ensightFile).open(_fileName.c_str()) ; 
//    if (*_ensightFile)
//      _status = MED_OPENED ;
//    else


  SCRUTE((*_ensightFile).is_open());
  SCRUTE(_ensightFile);



  if (!(*_ensightFile))
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not open file "
				     << _fileName)
			  );
  END_OF(LOC);
}

void ENSIGHT_MESH_DRIVER::open() {
  openConst() ;
}

void ENSIGHT_MESH_DRIVER::closeConst() const throw (MEDEXCEPTION)
{
  const char * LOC = "ENSIGHT_MESH_DRIVER::closeConst() " ;
  BEGIN_OF(LOC);

  SCRUTE(_ensightFile);
  SCRUTE(*_ensightFile);


  if ((*_ensightFile).is_open())
    (*_ensightFile).close();
  
//    if (*_ensightFile)
//      _status = MED_CLOSED ;
//    else

  SCRUTE(_ensightFile);
  SCRUTE(*_ensightFile);

  if (!(*_ensightFile))
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not close file "
				     << _fileName)
			  );

  END_OF(LOC);
}

void ENSIGHT_MESH_DRIVER::close() {
  closeConst() ;
}

void    ENSIGHT_MESH_DRIVER::setMeshName(const string & meshName) { _meshName = meshName; };
string  ENSIGHT_MESH_DRIVER::getMeshName() const { return _meshName; };

void ENSIGHT_MESH_DRIVER::read(void) throw (MEDEXCEPTION)
{
  const char * LOC = "ENSIGHT_MESH_DRIVER::read() : " ;
  BEGIN_OF(LOC);

  // Send an exception

  throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "This driver is only used to write in Ensight format !"));

  END_OF(LOC);
}


void ENSIGHT_MESH_DRIVER::write(void) const
  throw (MEDEXCEPTION)
{ 
  const char * LOC = "void ENSIGHT_MESH_DRIVER::write(void) const : ";
  BEGIN_OF(LOC);

  // ------------ create the main Ensight file 
  string ensight_case(ENSIGHT_MESH_DRIVER::ENSIGHT_MESH_DRIVER::_fileName) ;
  replace(ensight_case.begin(),ensight_case.end(),'.','\0');
  string ensight_casef(ensight_case.c_str()) ;
  ensight_casef  = ensight_casef +=".case" ;
  ofstream ensightCaseFile(ensight_casef.c_str(),ios::out);
  ensightCaseFile << "FORMAT" << endl ;
  ensightCaseFile << "type:   ensight" << endl ;
  ensightCaseFile << endl ;
  ensightCaseFile << "GEOMETRY" << endl ;
  ensightCaseFile << "# Mesh " << 1 << " detected with name = " << _ptrMesh->getName() << endl;
  ensightCaseFile << "model:" << 1 << " " << _fileName << endl;
  ensightCaseFile << endl ;

  // Well we must open ensight file first, because there are
  // no other driver than MED for Ensight that do it !

  openConst();

  (*_ensightFile) << "# Ensight6 case DataFile Version" << endl 
		  << "maillage from MedMemory" << endl ;
  (*_ensightFile) << "node id assign" << endl ;
  (*_ensightFile) << "element id assign" << endl ;


  (*_ensightFile) << "coordinates" << endl ;
  // put points (all point are in 3D, so if we are in 1D or 2D, we complete by zero !
  int SpaceDimension = _ptrMesh->getSpaceDimension() ;
  int NumberOfNodes = _ptrMesh->getNumberOfNodes() ;
  (*_ensightFile) << NumberOfNodes << endl ;
  const double *coordinate = _ptrMesh->getCoordinates(MED_FULL_INTERLACE) ;
  for (int i=0;i<NumberOfNodes;i++) {
//     (*_ensightFile) << setw(8) << i+1  << " " ;
    for (int j=0;j<SpaceDimension;j++)
      (*_ensightFile) << setw(8) << coordinate[i*SpaceDimension+j] << " " ;
    if (SpaceDimension==1) 
      (*_ensightFile) << "0 0" ;
    if (SpaceDimension==2) 
      (*_ensightFile) << "0" ;
    (*_ensightFile) << endl ;
  }

  // we put connectivity
  // how many cells and how many value in connectivity :
  int cells_types_count = _ptrMesh->getNumberOfTypes(MED_CELL) ;
  int cells_sum = _ptrMesh->getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS) ;
  const CELLMODEL * cells_type = _ptrMesh->getCellsTypes(MED_CELL) ;

//   const int * connectivityIndex = _ptrMesh->getConnectivityIndex(MED_NODAL,MED_CELL) ;

  (*_ensightFile) << "part 1" << endl;
  (*_ensightFile) << "connectivities description" << endl;
  // we put connectivity
  for (int i=0;i<cells_types_count;i++) {
    int numberOfCell = _ptrMesh->getNumberOfElements(MED_CELL,cells_type[i].getType()) ;
    int *filter = (int*) NULL ; // index in ensight connectivity
    switch (cells_type[i].getType()){
      case MED_POINT1  : {
        (*_ensightFile) << "point" << endl ;
        (*_ensightFile) << setw(8) << numberOfCell << endl ;
	filter = new int[1] ;
	filter[0] = 0 ;
        break ;
      }
      case MED_SEG2    : {
        (*_ensightFile) << "bar2" << endl ;
        (*_ensightFile) << setw(8) << numberOfCell << endl ;
        filter = new int[2] ;
	filter[0] = 0 ;
        filter[1] = 1 ;
        break ;
      }
      case MED_SEG3    : {  
        (*_ensightFile) << "bar3" << endl ;
        (*_ensightFile) << setw(8) << numberOfCell << endl ;
        filter = new int[3] ;
	filter[0] = 0 ;
        filter[1] = 2 ;
        filter[2] = 1 ;
        break ;
      }
      case MED_TRIA3   : {
        (*_ensightFile) << "tria3" << endl ;
        (*_ensightFile) << setw(8) << numberOfCell << endl ;
        filter = new int[3] ;
        filter[0] = 0 ;
        filter[1] = 2 ;
 	filter[2] = 1 ;
        break ;
      }
      case MED_QUAD4   : {
        (*_ensightFile) << "quad4" << endl ;
        (*_ensightFile) << setw(8) << numberOfCell << endl ;
        filter = new int[4] ;
        filter[0] = 0 ;
        filter[1] = 3 ;
        filter[2] = 2 ;
  	filter[3] = 1 ;
        break ;
      }
      case MED_TRIA6   : {
        (*_ensightFile) << "tria6" << endl ;
        (*_ensightFile) << setw(8) << numberOfCell << endl ;
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
        (*_ensightFile) << "quad8" << endl ;
        (*_ensightFile) << setw(8) << numberOfCell << endl ;
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
        (*_ensightFile) << "tetra4" << endl ;
        (*_ensightFile) << setw(8) << numberOfCell << endl ;
        filter = new int[4] ;
        filter[0] = 0 ;
        filter[1] = 1 ;
        filter[2] = 3 ;  // 3td element in med are 4th in ensight (array begin at 0 !)
        filter[3] = 2 ;  // 4th element in med are 3rd in ensight (array begin at 0 !)
        break ;
      }
      case MED_PYRA5   : {
        (*_ensightFile) << "pyramid5" << endl ;
        (*_ensightFile) << setw(8) << numberOfCell << endl ;
        filter = new int[5] ;
        filter[0] = 0 ;
        filter[1] = 3 ;  // 2nd element in med are 4th in ensight (array begin at 0 !)
        filter[2] = 2 ;
        filter[3] = 1 ;  // 4th element in med are 2nd in ensight (array begin at 0 !)
  	filter[4] = 4 ;
        break ;
      }
      case MED_PENTA6  : {
        (*_ensightFile) << "penta6" << endl ;
        (*_ensightFile) << setw(8) << numberOfCell << endl ;
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
        (*_ensightFile) << "hexa8" << endl ;
        (*_ensightFile) << setw(8) << numberOfCell << endl ;
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
        (*_ensightFile) << "tetra10" << endl ;
        (*_ensightFile) << setw(8) << numberOfCell << endl ;
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
        (*_ensightFile) << "pyramid13" << endl ;
        (*_ensightFile) << setw(8) << numberOfCell << endl ;
        break ;
      }
      case MED_PENTA15 : {
        (*_ensightFile) << "penta15" << endl ;
        (*_ensightFile) << setw(8) << numberOfCell << endl ;
        break ;
      }
      case MED_HEXA20  : {
        (*_ensightFile) << "hexa20" << endl ;
        (*_ensightFile) << setw(8) << numberOfCell << endl ;
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
	(*_ensightFile) << setw(8) << connectivityArray[j*nodes_cell+filter[k]]  << " " ;
      (*_ensightFile) << endl ;
    }
    if (filter != NULL)
      delete[] filter ;
  }

  for(int i=0;i<_support.size();i++){
    // we put connectivity
    // how many cells and how many value in connectivity :
    int cells_types_count = _support[i]->getNumberOfTypes() ;
    int cells_sum = _support[i]->getNumberOfElements(MED_ALL_ELEMENTS) ;
    const medGeometryElement * geo_type = _support[i]->getTypes() ;

//     const int * connectivityIndex = _ptrMesh->getConnectivityIndex(MED_NODAL,MED_CELL) ;

    (*_ensightFile) << "part " << i+2 << endl;
    (*_ensightFile) << "connectivities description" << endl;

    int nodes_cell;
    // we put connectivity
    for (int i=0;i<cells_types_count;i++) {
      int numberOfCell = _support[i]->getNumberOfElements(geo_type[i]) ;
      int *filter = (int*) NULL ; // index in ensight connectivity
      switch (geo_type[i]){
      case MED_POINT1  : {
	nodes_cell=1;
	(*_ensightFile) << "point" << endl ;
	(*_ensightFile) << setw(8) << numberOfCell << endl ;
	filter = new int[1] ;
	filter[0] = 0 ;
	break ;
      }
      case MED_SEG2    : {
	nodes_cell=2;
	(*_ensightFile) << "bar2" << endl ;
	(*_ensightFile) << setw(8) << numberOfCell << endl ;
	filter = new int[2] ;
	filter[0] = 0 ;
	filter[1] = 1 ;
	break ;
      }
      case MED_SEG3    : {  
	nodes_cell=3;
	(*_ensightFile) << "bar3" << endl ;
	(*_ensightFile) << setw(8) << numberOfCell << endl ;
        filter = new int[3] ;
	filter[0] = 0 ;
        filter[1] = 2 ;
        filter[2] = 1 ;
	break ;
      }
      case MED_TRIA3   : {
	nodes_cell=3;
	(*_ensightFile) << "tria3" << endl ;
	(*_ensightFile) << setw(8) << numberOfCell << endl ;
	filter = new int[3] ;
	filter[0] = 0 ;
	filter[1] = 2 ;
	filter[2] = 1 ;
	break ;
      }
      case MED_QUAD4   : {
	nodes_cell=4;
	(*_ensightFile) << "quad4" << endl ;
	(*_ensightFile) << setw(8) << numberOfCell << endl ;
	filter = new int[4] ;
	filter[0] = 0 ;
	filter[1] = 3 ;
	filter[2] = 2 ;
	filter[3] = 1 ;
	break ;
      }
      case MED_TRIA6   : {
	nodes_cell=6;
	(*_ensightFile) << "tria6" << endl ;
	(*_ensightFile) << setw(8) << numberOfCell << endl ;
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
	(*_ensightFile) << "quad8" << endl ;
	(*_ensightFile) << setw(8) << numberOfCell << endl ;
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
	(*_ensightFile) << "tetra4" << endl ;
	(*_ensightFile) << setw(8) << numberOfCell << endl ;
	filter = new int[4] ;
	filter[0] = 0 ;
	filter[1] = 1 ;
	filter[2] = 3 ;  // 3td element in med are 4th in ensight (array begin at 0 !)
	filter[3] = 2 ;  // 4th element in med are 3rd in ensight (array begin at 0 !)
	break ;
      }
      case MED_PYRA5   : {
	nodes_cell=5;
	(*_ensightFile) << "pyramid5" << endl ;
	(*_ensightFile) << setw(8) << numberOfCell << endl ;
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
	(*_ensightFile) << "penta6" << endl ;
	(*_ensightFile) << setw(8) << numberOfCell << endl ;
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
	(*_ensightFile) << "hexa8" << endl ;
	(*_ensightFile) << setw(8) << numberOfCell << endl ;
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
	(*_ensightFile) << "tetra10" << endl ;
	(*_ensightFile) << setw(8) << numberOfCell << endl ;
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
	(*_ensightFile) << "pyramid13" << endl ;
	(*_ensightFile) << setw(8) << numberOfCell << endl ;
	break ;
      }
      case MED_PENTA15 : {
	nodes_cell=15;
	(*_ensightFile) << "penta15" << endl ;
	(*_ensightFile) << setw(8) << numberOfCell << endl ;
	break ;
      }
      case MED_HEXA20  : {
	nodes_cell=20;
	(*_ensightFile) << "hexa20" << endl ;
	(*_ensightFile) << setw(8) << numberOfCell << endl ;
	break ;
      }
      default : { 
	break ;
      }
      }
      if (filter==NULL) 
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": MED element type not supported yet : " << cells_type[i].getName() ) ) ;

      const int * connectivityArray = _ptrMesh->getConnectivity(MED_FULL_INTERLACE,MED_NODAL,MED_FACE,geo_type[i]);
      for (int j=0;j<numberOfCell;j++) {
	for (int k=0;k<nodes_cell;k++)
	  (*_ensightFile) << setw(8) << connectivityArray[j*nodes_cell+filter[k]]  << " " ;
	(*_ensightFile) << endl ;
      }
      if (filter != NULL)
	delete[] filter ;

    }

  }
    
  (*_ensightFile) << endl ;

  END_OF(LOC);
} 

GENDRIVER * ENSIGHT_MESH_DRIVER::copy(void) const
{
  return new ENSIGHT_MESH_DRIVER(*this);
}

void ENSIGHT_MESH_DRIVER::addSupport(SUPPORT *sup)
{
  _support.push_back(sup);
}
