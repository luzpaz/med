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
#include "MEDMEM_EnsightMedDriver.hxx"

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

ENSIGHT_MED_DRIVER::ENSIGHT_MED_DRIVER(): GENDRIVER(), 
                                  _ptrMed((MED * const)MED_NULL)
{
  _ensightFile     = new ofstream();
  // What about _id in Gendriver ?
  // _driverType ???
}


ENSIGHT_MED_DRIVER::ENSIGHT_MED_DRIVER(const string & fileName,  MED * const ptrMed):
  GENDRIVER(fileName,MED_EN::MED_RDWR), _ptrMed(ptrMed)
{
  _ensightFile     = new ofstream();
}

ENSIGHT_MED_DRIVER::ENSIGHT_MED_DRIVER(const ENSIGHT_MED_DRIVER & driver):
  GENDRIVER(driver), 
  _ptrMed(driver._ptrMed)
{
  _ptrMed->addDriver(*this); // OU RECUPERER L'ID.
  _ensightFile     = new ofstream(); 
  // What about _id in Gendriver ?
  // _driverType ???
}

ENSIGHT_MED_DRIVER::~ENSIGHT_MED_DRIVER()
{
  const char * LOC ="ENSIGHT_MED_DRIVER::~ENSIGHT_MED_DRIVER()";
  BEGIN_OF(LOC);

  close();
  delete _ensightFile ;

  END_OF(LOC);
}

GENDRIVER * ENSIGHT_MED_DRIVER::copy() const
{
  return new ENSIGHT_MED_DRIVER(*this) ;
}

//REM :  A t'on besoin du champ _status :  _ensightFile <-> _status  ?  Oui

void ENSIGHT_MED_DRIVER::openConst() const {

  const char * LOC ="ENSIGHT_MED_DRIVER::open() : ";

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

void ENSIGHT_MED_DRIVER::open() {
  openConst() ;
}

void ENSIGHT_MED_DRIVER::closeConst() const {

  const char * LOC = "ENSIGHT_MED_DRIVER::close() : ";
  BEGIN_OF(LOC);
  
  (*_ensightFile).close();
 
  if (!(*_ensightFile))
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not close main Ensight file "
				     << _fileName)
			  );
  END_OF(LOC);
}

void ENSIGHT_MED_DRIVER::close() {
  closeConst() ;
}


void ENSIGHT_MED_DRIVER::write() const {

  const char * LOC = "ENSIGHT_MED_DRIVER::write() : ";
  BEGIN_OF(LOC);

  // Well we must open ensight file first, because there are
  // no other driver than MED for ENSIGHT that do it !
  openConst() ;

  // Ensight case organization requires a main file (filename.case) which defines organization
  // We also need a geom file (filemane.geo) to store the meshs
  // We also need a data file (filemane.data) to store the fields

  // main Ensight file  
  // ofstream ensightCaseFile("test.case", ios::out);
   
  // could we put more than one Mesh ?????

  int NumberOfMeshes = _ptrMed->getNumberOfMeshes() ;
  deque<string> MeshNames = _ptrMed->getMeshNames() ;
  // In fact, we must take care of all supports 
  // We restrict Field on all nodes or cells

  int NumberOfFields = _ptrMed->getNumberOfFields() ;
  deque<string> FieldNames = _ptrMed->getFieldNames() ;

  string ensight_case(ENSIGHT_MED_DRIVER::ENSIGHT_MED_DRIVER::_fileName) ;

  //------------------------------------------------------------------------
  (*_ensightFile) << "Driver Ensight for " << ensight_case << " case" << endl << endl ;
  (*_ensightFile) << "Result files are : " << endl ;
  (*_ensightFile) << "-------------------" << endl ;
  (*_ensightFile) << "Main file     = " << ensight_case << ".case" << endl ;
  //------------------------------------------------------------------------

  string ensight_casef  = ensight_case ;
  ensight_casef  = ensight_casef +=".case" ;
  // ------------ create the main Ensight file 
  ofstream ensightCaseFile(ensight_casef.c_str(),ios::out);
  ensightCaseFile << "FORMAT" << endl ;
  ensightCaseFile << "type:   ensight" << endl ;
  ensightCaseFile << endl ;
  if (NumberOfMeshes){
    ensightCaseFile << "GEOMETRY" << endl ;
    for (int i=0; i<NumberOfMeshes; i++) {
      ensightCaseFile << "# Mesh " << i+1 << " detected with name = " << MeshNames[i] << endl ;
      ensightCaseFile << "model: " << i+1 << " " << ensight_case << "-" << i+1 << ".geom"  << endl ;
      (*_ensightFile) << "Mesh     file = " << ensight_case << "-" << i+1 << ".geom" << endl ;
    }
  }
  ensightCaseFile << endl ;
  if (NumberOfFields) ensightCaseFile << "VARIABLE" << endl ;

  for (int i=0; i<NumberOfMeshes; i++) {
    MESH * myMesh = _ptrMed->getMesh(MeshNames[i]) ;
    writeMesh(myMesh) ;
    // get all field which values are on this mesh => revoir api de Med !!!
    // first : field on node
    // fields is on all node !
    for (int j=0; j<NumberOfFields; j++) {
      deque<DT_IT_> timeStep = _ptrMed->getFieldIteration(FieldNames[j]) ;
      deque<DT_IT_>::const_iterator currentTimeStep ;
      for ( currentTimeStep=timeStep.begin(); currentTimeStep!=timeStep.end(); currentTimeStep++) {
	int dt = (*currentTimeStep).dt ;
	int it = (*currentTimeStep).it ;
	FIELD_ * myField = _ptrMed->getField(FieldNames[j],dt,it) ;
	ostringstream name ; 
	name << myField->getName() << "_" << dt << "_" << it ;
	ensightCaseFile << "# Field "<< j+1 << " detected with name = " << name.str() << endl ;
	if( MeshNames[i] == myField->getSupport()->getMesh()->getName() ) { 
	  // rigth in all case : better compare pointeur ?
	  if (MED_NODE == myField->getSupport()->getEntity())
	    if (myField->getSupport()->isOnAllElements()) {
	      int NumberOfComponents =  myField->getNumberOfComponents() ;
	      ostringstream name ; 
	      name << myField->getName() << "_" << dt << "_" << it ;
	      if (NumberOfComponents==3) ensightCaseFile << "vector per node: " << name.str() << " " << name.str() << ".data" << endl ;
	      else if (NumberOfComponents<=4) ensightCaseFile << "scalar per node: " << name.str() << " " << name.str() << ".data" << endl ;
	      (*_ensightFile) << "Field    file = " << name.str() << ".data" << endl ;
	      writeField(myField,name.str()) ;
	    } else
	      MESSAGE(LOC << "Could not write field "<<myField->getName()<<" which is not on all nodes !");
	}
      }
    }

    for (int j=0; j<NumberOfFields; j++) {
      deque<DT_IT_> timeStep = _ptrMed->getFieldIteration(FieldNames[j]) ;
      deque<DT_IT_>::const_iterator currentTimeStep ;
      for ( currentTimeStep=timeStep.begin(); currentTimeStep!=timeStep.end(); currentTimeStep++) {
	int dt = (*currentTimeStep).dt;
	int it = (*currentTimeStep).it;
	FIELD_ * myField = _ptrMed->getField(FieldNames[j],dt,it) ;
	ostringstream name ; 
	name << myField->getName() << "_" << dt << "_" << it ;
	ensightCaseFile << "# Field "<< j+1 << " detected for " << name.str() << endl ;
	if( MeshNames[i] == myField->getSupport()->getMesh()->getName() ) { 
	  // rigth in all case : better compare pointeur ?
	  if (MED_CELL == myField->getSupport()->getEntity())
	    if (myField->getSupport()->isOnAllElements()) {
	      int NumberOfComponents =  myField->getNumberOfComponents() ;
	      ostringstream name ; 
	      name << myField->getName() << "_" << dt << "_" << it ;
	      if (NumberOfComponents==3) ensightCaseFile << "vector per node: " << name.str() << " " << name.str() << ".data" << endl ;
	      else if (NumberOfComponents<=4) ensightCaseFile << "scalar per node: " << name.str() << " " << name.str() << ".data" << endl ;
	      (*_ensightFile) << "Field    file = " << name.str() << ".data" << endl ;
	      writeField(myField,name.str()) ;
	    } else
	      MESSAGE(LOC << "Could not write field "<<myField->getName()<<" which is not on all cells !");
	}
      }
    }
    
  }

  // Well we must close ensight file first, because there are
  // no other driver than MED for ENSIGHT that do it !
  //  closeConst() ;
  
  END_OF(LOC);
}

void ENSIGHT_MED_DRIVER::writeMesh(MESH * myMesh) const {

  const char * LOC = "ENSIGHT_MED_DRIVER::writeMesh() : ";
  BEGIN_OF(LOC);

  string ensight_geomf(ENSIGHT_MED_DRIVER::ENSIGHT_MED_DRIVER::_fileName) ;
  ensight_geomf  += "-1"  ;
  ensight_geomf  += ".geom"  ;

  // ------------ create the Ensight file associated to this meshe
  ofstream ensightGeomFile(ensight_geomf.c_str(),ios::out);  
  ensightGeomFile << "Ensight Geometry File : " <<  endl 
	   << "Meshing from MedMemory"  << endl ;
  // only ASCII for the moment (binary came later :-)
  ensightGeomFile << "node id given " << endl ;
  ensightGeomFile << "element id given " << endl ;

  ensightGeomFile << "coordinates" << endl ;
  // put points (all point are in 3D, so if we are in 1D or 2D, we complete by zero !
  int SpaceDimension = myMesh->getSpaceDimension() ;
  int NumberOfNodes = myMesh->getNumberOfNodes() ;
  // (ensightGeomFile) << "how many points =  " << NumberOfNodes << " float" << endl ;
  ensightGeomFile << NumberOfNodes << endl ;
  const double *coordinate = myMesh->getCoordinates(MED_FULL_INTERLACE) ;
  for (int i=0;i<NumberOfNodes;i++) {
    ensightGeomFile << setw(8) << i+1  << " " ;
    for (int j=0;j<SpaceDimension;j++)
      ensightGeomFile << setw(8) << coordinate[i*SpaceDimension+j] << " " ;
    if (SpaceDimension==1) 
      ensightGeomFile << "0 0" ;
    if (SpaceDimension==2) 
      ensightGeomFile << "0" ;
    ensightGeomFile << endl ;
  }

  // we put connectivity
  // how many cells and how many value in connectivity :
  int cells_types_count = myMesh->getNumberOfTypes(MED_CELL) ;
  int cells_sum = myMesh->getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS) ;
  const CELLMODEL * cells_type = myMesh->getCellsTypes(MED_CELL) ;
  ensightGeomFile << "part 1 " << endl ;
  ensightGeomFile << "elements are following " << endl ;
  // we put connectivity
  for (int i=0;i<cells_types_count;i++) {
    int numberOfCell = myMesh->getNumberOfElements(MED_CELL,cells_type[i].getType()) ;
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
        ensightGeomFile << "bar3" << endl ;
        ensightGeomFile << setw(8) << numberOfCell << endl ;
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
        filter[2] = 3 ;  // 3td element in med are 4th in ensight (array begin at 0 !)
        filter[3] = 2 ;  // 4th element in med are 3rd in ensight (array begin at 0 !)
        break ;
      }
      case MED_PYRA5   : {
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
    const int * connectivityArray = myMesh->getConnectivity(MED_FULL_INTERLACE,MED_NODAL,MED_CELL,cells_type[i].getType());
    for (int j=0;j<numberOfCell;j++) {
      for (int k=0;k<nodes_cell;k++)
	ensightGeomFile << setw(8) << connectivityArray[j*nodes_cell+filter[k]]  << " " ;
      ensightGeomFile << endl ;
    }
    if (filter != NULL)
      delete[] filter ;
  }

  ensightGeomFile << endl ;
  return ;

  END_OF(LOC);
}

void ENSIGHT_MED_DRIVER::writeField(FIELD_ * myField,string name) const {

  const char * LOC = "ENSIGHT_MED_DRIVER::writeField() : ";
  BEGIN_OF(LOC);
  
  typedef MEDMEM_ArrayInterface<int,NoInterlace,NoGauss>::Array ArrayIntNo;
  typedef MEDMEM_ArrayInterface<double,NoInterlace,NoGauss>::Array ArrayDoubleNo;

  int NumberOfValue = myField->getSupport()->getNumberOfElements(MED_ALL_ELEMENTS) ;
  int NumberOfComponents =  myField->getNumberOfComponents() ;
	
  string dataname = name += ".data" ;
  ofstream ensightDataFile(name.c_str(),ios::out) ;  
  ensightDataFile.setf(ios::scientific);	
  ensightDataFile.precision(5);	

  med_type_champ type = myField->getValueType() ;
  SCRUTE(name);
  SCRUTE(type);
  switch (type)
    {
    case MED_INT32 : {
      MESSAGE("MED_INT32");
      if (NumberOfComponents==3) {
	ensightDataFile << "vector per node integer 32 mode for " << name << " following " << endl ;
      } else if (NumberOfComponents<=4) {
	ensightDataFile << "scalar per node integer 32 mode for " << name << " following " << endl ;
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
	  ensightDataFile << setw(12) << value[j*NumberOfValue+i] << " " ;
     }
      if ( myField->getInterlacingType() == MED_FULL_INTERLACE )
	delete[] myArray;
      break ;
    }
    case MED_REEL64 : {
      MESSAGE("MED_REEL64");
      if (NumberOfComponents==3) {
	ensightDataFile << "vector per node real 64 mode for " << name << " following " << endl ;
      } else if (NumberOfComponents<=4) {
	ensightDataFile << "scalar per element real 64 mode for " << name << " following " << endl ;
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

      int mypoint=0;
      for (int i=0; i<NumberOfValue; i++) {
	for(int j=0; j<NumberOfComponents; j++)
	  ensightDataFile << setw(12) << value[j*NumberOfValue+i] ;
	  mypoint+=1;
	  if (mypoint == 6) {
	    ensightDataFile << endl ;
	    mypoint=0;
	  }
      }
	ensightDataFile << endl ;
      if ( myField->getInterlacingType() == MED_FULL_INTERLACE )
	delete[] myArray;
      break ;
    }
    default : { 
      MESSAGE(LOC << "Could not write field "<<name<<" the type is not int or double !");
    }
    }

  ensightDataFile.close();  	
  
  END_OF(LOC);
}

void ENSIGHT_MED_DRIVER::writeSupport(SUPPORT * mySupport) const {
  const char * LOC = "ENSIGHT_MED_DRIVER::writeSupport(SUPPORT *) : " ;
  BEGIN_OF(LOC) ;
  MESSAGE(LOC << "Not yet implemented, acting on the object " << *mySupport);
  END_OF(LOC) ;
}
