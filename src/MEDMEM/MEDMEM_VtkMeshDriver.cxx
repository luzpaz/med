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
#include "MEDMEM_VtkMeshDriver.hxx"

#include "MEDMEM_DriversDef.hxx"

#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_Grid.hxx"

#include <sstream>

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

#define MED_NULL     NULL

VTK_MESH_DRIVER::VTK_MESH_DRIVER(): GENDRIVER(), 
				    _ptrMesh((MESH * const)MED_NULL)
{
  _vtkFile = new ofstream();
  // What about _id in Gendriver ?
  // _driverType ???
}

VTK_MESH_DRIVER::VTK_MESH_DRIVER(const string & fileName,
				 MESH * ptrMesh) :
  GENDRIVER(fileName,MED_WRONLY),
  _ptrMesh(ptrMesh)
{
  const char * LOC = "VTK_MESH_DRIVER::VTK_MESH_DRIVER(const string & fileName, MESH * ptrMesh) : " ;

  // Send an exception because a VTK_MESH_DRIVER object cannot be instantied
  // from a file and there is no read for that kind of driver

  //  throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "This driver is only used to write in VTK format So thie object can not be instantied using a file!"));

  //  _ptrMesh->addDriver(*this); // OU RECUPERER L'ID.
  MESSAGE(LOC << "WARNING this driver is only used to write in VTK format So the object can not be instantied using a file!");

  _vtkFile = new ofstream(); 
}

VTK_MESH_DRIVER::VTK_MESH_DRIVER(const VTK_MESH_DRIVER & driver): 
  GENDRIVER(driver),
  _ptrMesh(driver._ptrMesh),
  _meshName(driver._meshName)
{
  // next string commented by skl for bug NPAL14840
  //_ptrMesh->addDriver(*this);
  _vtkFile = new ofstream();
}

VTK_MESH_DRIVER::~VTK_MESH_DRIVER()
{
  const char * LOC ="VTK_MESH_DRIVER::~VTK_MESH_DRIVER()";
  BEGIN_OF(LOC);

  close();

  SCRUTE(_vtkFile);

  delete _vtkFile ;

  SCRUTE(_vtkFile);

  END_OF(LOC);
}

void VTK_MESH_DRIVER::openConst() const throw (MEDEXCEPTION)
{
  const char * LOC = "VTK_MESH_DRIVER::openConst()" ;
  BEGIN_OF(LOC);

  MESSAGE(LOC<<" : _fileName.c_str : "<< _fileName.c_str()<<",mode : "<< _accessMode);

  if ( _fileName == "" )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "_fileName is |\"\"|, please set a correct fileName before calling open()"
                                     )
                          );

  if (!(*_vtkFile).is_open())
    (*_vtkFile).open(_fileName.c_str()) ; 
//    if (*_vtkFile)
//      _status = MED_OPENED ;
//    else


  SCRUTE((*_vtkFile).is_open());
  SCRUTE(_vtkFile);



  if (!(*_vtkFile))
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not open file "
				     << _fileName)
			  );
  END_OF(LOC);
}

void VTK_MESH_DRIVER::open() {
  openConst() ;
}

void VTK_MESH_DRIVER::closeConst() const throw (MEDEXCEPTION)
{
  const char * LOC = "VTK_MESH_DRIVER::closeConst() " ;
  BEGIN_OF(LOC);

  SCRUTE(_vtkFile);
  SCRUTE(*_vtkFile);


  if ((*_vtkFile).is_open())
    (*_vtkFile).close();
  
//    if (*_vtkFile)
//      _status = MED_CLOSED ;
//    else

  SCRUTE(_vtkFile);
  SCRUTE(*_vtkFile);

  if (!(*_vtkFile))
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not close file "
				     << _fileName)
			  );

  END_OF(LOC);
}

void VTK_MESH_DRIVER::close() {
  closeConst() ;
}

void    VTK_MESH_DRIVER::setMeshName(const string & meshName) { _meshName = meshName; };
string  VTK_MESH_DRIVER::getMeshName() const { return _meshName; };

void VTK_MESH_DRIVER::read(void) throw (MEDEXCEPTION)
{
  const char * LOC = "VTK_MESH_DRIVER::read() : " ;
  BEGIN_OF(LOC);

  // Send an exception

  throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "This driver is only used to write in VTK format !"));

  END_OF(LOC);
}


void VTK_MESH_DRIVER::write(void) const
  throw (MEDEXCEPTION)
{ 
  const char * LOC = "void VTK_MESH_DRIVER::write(void) const : ";
  BEGIN_OF(LOC);

  // Well we must open vtk file first, because there are
  // no other driver than MED for VTK that do it !

  openConst();

  (*_vtkFile) << "# vtk DataFile Version 2.0" << endl 
	   << "maillage from MedMemory"  << endl ;
  // only ASCII for the moment (binary came later :-)
  (*_vtkFile) << "ASCII" << endl ;


  (*_vtkFile) << "DATASET UNSTRUCTURED_GRID" << endl ;
  // put points (all point are in 3D, so if we are in 1D or 2D, we complete by zero !
  int SpaceDimension = _ptrMesh->getSpaceDimension() ;
  int NumberOfNodes = _ptrMesh->getNumberOfNodes() ;
  (*_vtkFile) << "POINTS " << NumberOfNodes << " float" << endl ;
  const double *coordinate = _ptrMesh->getCoordinates(MED_FULL_INTERLACE) ;
  for (int i=0;i<NumberOfNodes;i++) {
    for (int j=0;j<SpaceDimension;j++)
      (*_vtkFile) << coordinate[i*SpaceDimension+j] << " " ;
    if (SpaceDimension==1) 
      (*_vtkFile) << "0 0" ;
    if (SpaceDimension==2) 
      (*_vtkFile) << "0" ;
    (*_vtkFile) << endl ;
  }

  // we put connectivity
  // how many cells and how many value in connectivity :
  int cells_types_count = _ptrMesh->getNumberOfTypes(MED_CELL) ;
  //  int * cells_count = _ptrMesh->get_cells_count() ;
  //  int cells_sum = cells_count[cells_types_count] ;
  int cells_sum = _ptrMesh->getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS) ;
  const CELLMODEL * cells_type = _ptrMesh->getCellsTypes(MED_CELL) ;
  //  int connectivity_sum = 0 ;

  //const int * connectivity = _ptrMesh->getConnectivity(MED_FULL_INTERLACE,MED_NODAL,MED_CELL,MED_ALL_ELEMENTS) ; !! UNUSED VARIABLE !!
  const int * connectivityIndex = _ptrMesh->getConnectivityIndex(MED_NODAL,MED_CELL) ;

  int connectivity_sum =  connectivityIndex[cells_sum]-1 ;

  (*_vtkFile) << "CELLS " << cells_sum << " " << connectivity_sum+cells_sum << endl ;
  // we put connectivity
  for (int i=0;i<cells_types_count;i++) {
    int *filter = (int*) NULL ; // index in vtk connectivity
    switch (cells_type[i].getType())
      {
      case MED_POINT1  : {
	filter = new int[1] ;
	filter[0] = 0 ;
        break ;
      }
      case MED_SEG2    : {
        filter = new int[2] ;
	filter[0] = 0 ;
        filter[1] = 1 ;
        break ;
      }
      case MED_SEG3    : {  
        break ;
      }
      case MED_TRIA3   : {
        filter = new int[3] ;
        filter[0] = 0 ;
        filter[1] = 1 ;
 	filter[2] = 2 ;
        break ;
      }
      case MED_QUAD4   : {
        filter = new int[4] ;
        filter[0] = 0 ;
        filter[1] = 1 ;
        filter[2] = 2 ;
  	filter[3] = 3 ;
        break ;
      }
      case MED_TRIA6   : {
        break ;
      }
      case MED_QUAD8   : {
        break ;
      }
      case MED_TETRA4  : {
        filter = new int[4] ;
        filter[0] = 0 ;
        filter[1] = 1 ;
        filter[2] = 3 ;  // 3td element in med are 4th in vtk (array begin at 0 !)
        filter[3] = 2 ;  // 4th element in med are 3rd in vtk (array begin at 0 !)
        break ;
      }
      case MED_PYRA5   : {
        filter = new int[5] ;
        filter[0] = 0 ;
        filter[1] = 3 ;  // 2nd element in med are 4th in vtk (array begin at 0 !)
        filter[2] = 2 ;
        filter[3] = 1 ;  // 4th element in med are 2nd in vtk (array begin at 0 !)
  	filter[4] = 4 ;
        break ;
      }
      case MED_PENTA6  : {
        filter = new int[6] ;
        filter[0] = 0 ;
        filter[1] = 1 ;
        filter[2] = 2 ;
        filter[3] = 3 ;
        filter[4] = 4 ;
  	filter[5] = 5 ;
	break ;
      }
      case MED_HEXA8   : {
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
        break ;
      }
      case MED_PYRA13  : {
        break ;
      }
      case MED_PENTA15 : {
        break ;
      }
      case MED_HEXA20  : {
        break ;
      }
      default : { 
        break ;
      }
      }
    if (filter==NULL) 
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": MED element type not supported yet : " << cells_type[i].getName() ) ) ;
    int nodes_cell = cells_type[i].getNumberOfNodes();
    int numberOfCell = _ptrMesh->getNumberOfElements(MED_CELL,cells_type[i].getType()) ;
    const int * connectivityArray = _ptrMesh->getConnectivity(MED_FULL_INTERLACE,MED_NODAL,MED_CELL,cells_type[i].getType());
    for (int j=0;j<numberOfCell;j++) {
      (*_vtkFile) << nodes_cell << " " ;
      for (int k=0;k<nodes_cell;k++)
	(*_vtkFile) << connectivityArray[j*nodes_cell+filter[k]] - 1 << " " ;
      (*_vtkFile) << endl ;
    }
    if (filter != NULL)
      delete[] filter ;
  }
  (*_vtkFile) << endl ;
  // we put cells type
  (*_vtkFile) << "CELL_TYPES " << cells_sum << endl ;
  for (int i=0;i<cells_types_count;i++) {
    int vtkType = 0 ;
    switch (cells_type[i].getType())
      {
      case MED_POINT1  : {
	vtkType = 1 ;
        break ;
      }
      case MED_SEG2    : {
	vtkType = 3 ;
        break ;
      }
      case MED_SEG3    : {  
	vtkType = 0 ;
        break ;
      }
      case MED_TRIA3   : {
	vtkType = 5 ;
        break ;
      }
      case MED_QUAD4   : {
	vtkType = 9 ;
        break ;
      }
      case MED_TRIA6   : {
	vtkType = 0 ;
        break ;
      }
      case MED_QUAD8   : {
	vtkType = 0 ;
        break ;
      }
      case MED_TETRA4  : {
	vtkType = 10 ;
        break ;
      }
      case MED_PYRA5   : {
	vtkType = 14 ;
        break ;
      }
      case MED_PENTA6  : {
	vtkType = 13 ;
	break ;
      }
      case MED_HEXA8   : {
	vtkType = 12 ;
        break ;
      }
      case MED_TETRA10 : {
	vtkType = 0 ;
        break ;
      }
      case MED_PYRA13  : {
	vtkType = 0 ;
        break ;
      }
      case MED_PENTA15 : {
	vtkType = 0 ;
        break ;
      }
      case MED_HEXA20  : {
	vtkType = 0 ;
        break ;
      }
      default : { 
	vtkType = 0 ;
        break ;
      }
      }
    if (vtkType == 0)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": MED element type not supported yet : " << cells_type[i].getType() ) ) ;
    int numberOfCell = _ptrMesh->getNumberOfElements(MED_CELL,cells_type[i].getType()) ;
    for (int j=0;j<numberOfCell;j++)
      (*_vtkFile) << vtkType << endl ;
  }

  END_OF(LOC);
} 

GENDRIVER * VTK_MESH_DRIVER::copy(void) const
{
  return new VTK_MESH_DRIVER(*this);
}
