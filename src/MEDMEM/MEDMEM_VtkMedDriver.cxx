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
#include "MEDMEM_VtkMedDriver.hxx"

#include <sstream>

#include "MEDMEM_define.hxx"
#include "MEDMEM_Med.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_CellModel.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

VTK_MED_DRIVER::VTK_MED_DRIVER(): GENDRIVER(), 
                                  _ptrMed((MED * const)MED_NULL)
{
  _vtkFile = new ofstream();
  // What about _id in Gendriver ?
  // _driverType ???
}


VTK_MED_DRIVER::VTK_MED_DRIVER(const string & fileName,  MED * const ptrMed):
  GENDRIVER(fileName,MED_EN::MED_RDWR), _ptrMed(ptrMed)
{
  _vtkFile = new ofstream();
}

VTK_MED_DRIVER::VTK_MED_DRIVER(const VTK_MED_DRIVER & driver):
  GENDRIVER(driver), 
  _ptrMed(driver._ptrMed)
{
  _ptrMed->addDriver(*this); // OU RECUPERER L'ID.
  _vtkFile = new ofstream(); 
  // What about _id in Gendriver ?
  // _driverType ???
}

VTK_MED_DRIVER::~VTK_MED_DRIVER()
{
  const char * LOC ="VTK_MED_DRIVER::~VTK_MED_DRIVER()";
  BEGIN_OF(LOC);

  close();
  delete _vtkFile ;

  END_OF(LOC);
}

GENDRIVER * VTK_MED_DRIVER::copy() const
{
  return new VTK_MED_DRIVER(*this) ;
}

//REM :  As t'on besoin du champ _status :  _vtkFile <-> _status  ?  Oui

void VTK_MED_DRIVER::openConst() const {

  const char * LOC ="VTK_MED_DRIVER::open() : ";
  BEGIN_OF(LOC);

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
  if (!(*_vtkFile))
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not open file "
				     << _fileName)
			  );
  END_OF(LOC);
}

void VTK_MED_DRIVER::open() {
  openConst() ;
}

void VTK_MED_DRIVER::closeConst() const {

  const char * LOC = "VTK_MED_DRIVER::close() : ";
  BEGIN_OF(LOC);
  
  (*_vtkFile).close();
  
//    if (*_vtkFile)
//      _status = MED_CLOSED ;
//    else
  if (!(*_vtkFile))
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not close file "
				     << _fileName)
			  );
  END_OF(LOC);
}

void VTK_MED_DRIVER::close() {
  closeConst() ;
}


void VTK_MED_DRIVER::write() const {

  const char * LOC = "VTK_MED_DRIVER::write() : ";
  BEGIN_OF(LOC);

  // Well we must open vtk file first, because there are
  // no other driver than MED for VTK that do it !
  openConst() ;

  // could we put more than one Mesh ?????
  (*_vtkFile) << "# vtk DataFile Version 2.0" << endl 
	   << "maillage from MedMemory"  << endl ;
  // only ASCII for the moment (binary came later :-)
  (*_vtkFile) << "ASCII" << endl ;

  int NumberOfMeshes = _ptrMed->getNumberOfMeshes() ;
  deque<string> MeshNames = _ptrMed->getMeshNames() ;
  //deque<string>::const_iterator  currentMesh ; !! UNUSED VARIABLE !!
  // In fact, we must take care of all supports 
  // We restrict Field on all nodes or cells

  int NumberOfFields = _ptrMed->getNumberOfFields() ;
  deque<string> FieldNames = _ptrMed->getFieldNames() ;
  //deque<string>::const_iterator  currentField ; !! UNUSED VARIABLE !!

  //  for ( currentMesh=MeshName.begin();currentMesh != MeshName.end(); currentMesh++) {
  for (int i=0; i<NumberOfMeshes; i++) {
    MESH * myMesh = _ptrMed->getMesh(MeshNames[i]) ;
    writeMesh(myMesh) ;
    // get all field which values are on this mesh => revoir api de Med !!!
    // first : field on node
    // fields is on all node !
    (*_vtkFile) << "POINT_DATA " << myMesh->getNumberOfNodes() << endl ;
    for (int j=0; j<NumberOfFields; j++) {
      deque<DT_IT_> timeStep = _ptrMed->getFieldIteration(FieldNames[j]) ;
      deque<DT_IT_>::const_iterator currentTimeStep ;
      for ( currentTimeStep=timeStep.begin(); currentTimeStep!=timeStep.end(); currentTimeStep++) {
	int dt = (*currentTimeStep).dt ;
	int it = (*currentTimeStep).it ;
	FIELD_ * myField = _ptrMed->getField(FieldNames[j],dt,it) ;
	if( MeshNames[i] == myField->getSupport()->getMesh()->getName() ) { 
	  // rigth in all case : better compare pointeur ?
	  if (MED_NODE == myField->getSupport()->getEntity())
	    if (myField->getSupport()->isOnAllElements()) {
	      ostringstream name ; 
	      name << myField->getName() << "_" << dt << "_" << it ;
	      writeField(myField,name.str()) ;
	    } else
	      MESSAGE(LOC << "Could not write field "<<myField->getName()<<" which is not on all nodes !");
	}
      }
    }

    (*_vtkFile) << "CELL_DATA " << myMesh->getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS) << endl ;
    // second : field on cell
    for (int j=0; j<NumberOfFields; j++) {
      deque<DT_IT_> timeStep = _ptrMed->getFieldIteration(FieldNames[j]) ;
      deque<DT_IT_>::const_iterator currentTimeStep ;
      for ( currentTimeStep=timeStep.begin(); currentTimeStep!=timeStep.end(); currentTimeStep++) {
	int dt = (*currentTimeStep).dt;
	int it = (*currentTimeStep).it;
	FIELD_ * myField = _ptrMed->getField(FieldNames[j],dt,it) ;
	if( MeshNames[i] == myField->getSupport()->getMesh()->getName() ) { 
	  // rigth in all case : better compare pointeur ?
	  if (MED_CELL == myField->getSupport()->getEntity())
	    if (myField->getSupport()->isOnAllElements()) {
	      ostringstream name ; 
	      name << myField->getName() << "_" << dt << "_" << it ;
	      writeField(myField,name.str()) ;
	    } else
	      MESSAGE(LOC << "Could not write field "<<myField->getName()<<" which is not on all cells !");
	}
      }
    }
    
  }

  // Well we must close vtk file first, because there are
  // no other driver than MED for VTK that do it !
  //  closeConst() ;
  
  END_OF(LOC);
}

void VTK_MED_DRIVER::writeMesh(MESH * myMesh) const {

  const char * LOC = "VTK_MED_DRIVER::writeMesh() : ";
  BEGIN_OF(LOC);

  (*_vtkFile) << "DATASET UNSTRUCTURED_GRID" << endl ;
  // put points (all point are in 3D, so if we are in 1D or 2D, we complete by zero !
  int SpaceDimension = myMesh->getSpaceDimension() ;
  int NumberOfNodes = myMesh->getNumberOfNodes() ;
  (*_vtkFile) << "POINTS " << NumberOfNodes << " float" << endl ;
  const double *coordinate = myMesh->getCoordinates(MED_FULL_INTERLACE) ;
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
  int cells_types_count = myMesh->getNumberOfTypes(MED_CELL) ;
  //  int * cells_count = myMesh->get_cells_count() ;
  //  int cells_sum = cells_count[cells_types_count] ;
  int cells_sum = myMesh->getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS) ;
  const CELLMODEL * cells_type = myMesh->getCellsTypes(MED_CELL) ;
  //  int connectivity_sum = 0 ;

  //const int * connectivity = myMesh->getConnectivity(MED_FULL_INTERLACE,MED_NODAL,MED_CELL,MED_ALL_ELEMENTS) ; !! UNUSED VARIABLE !!
  const int * connectivityIndex = myMesh->getConnectivityIndex(MED_NODAL,MED_CELL) ;

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
    int numberOfCell = myMesh->getNumberOfElements(MED_CELL,cells_type[i].getType()) ;
    const int * connectivityArray = myMesh->getConnectivity(MED_FULL_INTERLACE,MED_NODAL,MED_CELL,cells_type[i].getType());
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
    int numberOfCell = myMesh->getNumberOfElements(MED_CELL,cells_type[i].getType()) ;
    for (int j=0;j<numberOfCell;j++)
      (*_vtkFile) << vtkType << endl ;
  }

  // add a constant field on all node to test !
  //    _vtkFile << "POINT_DATA " << NumberOfNodes << endl ;
  //    _vtkFile << "SCALARS example_scalaire float 1" << endl ;
  //    _vtkFile << "LOOKUP_TABLE default" << endl ;
  //    for (int i=0;i<NumberOfNodes;i++)
  //      _vtkFile << i << endl ;
  
  return ;


  END_OF(LOC);
}

void VTK_MED_DRIVER::writeField(FIELD_ * myField,string name) const {

  const char * LOC = "VTK_MED_DRIVER::writeField() : ";
  BEGIN_OF(LOC);
  
  typedef MEDMEM_ArrayInterface<int,NoInterlace,NoGauss>::Array ArrayIntNo;
  typedef MEDMEM_ArrayInterface<double,NoInterlace,NoGauss>::Array ArrayDoubleNo;

  int NumberOfValue = myField->getSupport()->getNumberOfElements(MED_ALL_ELEMENTS) ;
  int NumberOfComponents =  myField->getNumberOfComponents() ;

  med_type_champ type = myField->getValueType() ;
  SCRUTE(name);
  SCRUTE(type);
  switch (type)
    {
    case MED_INT32 : {
      MESSAGE("MED_INT32");
      if (NumberOfComponents==3) {
	(*_vtkFile) << "VECTORS " << name << " int" << endl ;
      } else if (NumberOfComponents<=4) {
	(*_vtkFile) << "SCALARS " << name << " int " << NumberOfComponents << endl ;
	(*_vtkFile) << "LOOKUP_TABLE default" << endl ;
      } else {
	MESSAGE(LOC << "Could not write field "<<myField->getName()<<" there are more than 4 components !");
	return ;
      }

      const int * value;
      ArrayIntNo * myArray;
      if ( myField->getInterlacingType() == MED_FULL_INTERLACE ) {
	myArray = ArrayConvert( *( dynamic_cast< FIELD<int,FullInterlace>* >
				   (myField)->getArrayNoGauss() 
				   )
				);
	value = myArray->getPtr();
      } else {
	value = ((FIELD<int>*)myField)->getValue() ;
      }

      for (int i=0; i<NumberOfValue; i++) {
	for(int j=0; j<NumberOfComponents; j++)
	  (*_vtkFile) << value[j*NumberOfValue+i] << " " ;
	(*_vtkFile) << endl ;
      }
      // mkr : PAL13994 (commented the code below)
      //if ( myField->getInterlacingType() == MED_FULL_INTERLACE )
      //delete[] myArray;
      break ;
    }
    case MED_REEL64 : {
      MESSAGE("MED_REEL64");
      if (NumberOfComponents==3) {
	(*_vtkFile) << "VECTORS " << name << " float" << endl ;
      } else if (NumberOfComponents<=4) {
	(*_vtkFile) << "SCALARS " << name << " float " << NumberOfComponents << endl ;
	(*_vtkFile) << "LOOKUP_TABLE default" << endl ;
      } else {
	MESSAGE(LOC << "Could not write field "<<myField->getName()<<" there are more than 4 components !");
	return ;
      }

      const double * value;
      ArrayDoubleNo * myArray;
      if ( myField->getInterlacingType() == MED_FULL_INTERLACE ) {
	myArray = ArrayConvert( *( dynamic_cast< FIELD<double,FullInterlace>* >
				   (myField)->getArrayNoGauss()
				   )
				);
	value = myArray->getPtr();
      } else {
	value = ((FIELD<double>*)myField)->getValue() ;
      }

      for (int i=0; i<NumberOfValue; i++) {
	for(int j=0; j<NumberOfComponents; j++)
	  (*_vtkFile) << value[j*NumberOfValue+i] << " " ;
	(*_vtkFile) << endl ;
      }
      // mkr : PAL13994 (commented the code below)
      //if ( myField->getInterlacingType() == MED_FULL_INTERLACE )
      //delete[] myArray;
      break ;
    }
    default : { 
      MESSAGE(LOC << "Could not write field "<<name<<" the type is not int or double !");
    }
    }
  
  END_OF(LOC);
}

void VTK_MED_DRIVER::writeSupport(SUPPORT * mySupport) const {
  const char * LOC = "VTK_MED_DRIVER::writeSupport(SUPPORT *) : " ;
  BEGIN_OF(LOC) ;
  MESSAGE(LOC << "Not yet implemented, acting on the object " << *mySupport);
  END_OF(LOC) ;
}
