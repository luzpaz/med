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
#include <strstream>

#include "MEDMEM_define.hxx"
#include "MEDMEM_Med.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_CellModel.hxx"

#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_MedFieldDriver.hxx"
#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_Group.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

ENSIGHT_MED_DRIVER::ENSIGHT_MED_DRIVER(): GENDRIVER(), 
                                  _ptrMed((MED * const)MED_NULL)
{}

ENSIGHT_MED_DRIVER::ENSIGHT_MED_DRIVER(const string & fileName,  MED * const ptrMed):
  GENDRIVER(fileName,MED_EN::MED_RDWR), _ptrMed(ptrMed)
{
}

ENSIGHT_MED_DRIVER::ENSIGHT_MED_DRIVER(const string & fileName,
			       MED * const ptrMed,
			       MED_EN::med_mode_acces accessMode):
  GENDRIVER(fileName,accessMode), _ptrMed(ptrMed)
{
}

ENSIGHT_MED_DRIVER::ENSIGHT_MED_DRIVER(const ENSIGHT_MED_DRIVER & driver):
  GENDRIVER(driver),
  _ptrMed(driver._ptrMed)
{
}

ENSIGHT_MED_DRIVER::~ENSIGHT_MED_DRIVER()
{
  MESSAGE("ENSIGHT_MED_DRIVER::~ENSIGHT_MED_DRIVER() has been destroyed");
}

void ENSIGHT_MED_DRIVER::open() {
  openConst() ;
}

void ENSIGHT_MED_DRIVER::close() {
  closeConst() ;
}

ENSIGHT_MED_WRONLY_DRIVER::ENSIGHT_MED_WRONLY_DRIVER() : ENSIGHT_MED_DRIVER()
{
  _ensightFile = new ofstream();
}

ENSIGHT_MED_WRONLY_DRIVER::ENSIGHT_MED_WRONLY_DRIVER(const string & fileName,  MED * const ptrMed) : ENSIGHT_MED_DRIVER(fileName,ptrMed)
{
  _ensightFile = new ofstream();
}

ENSIGHT_MED_WRONLY_DRIVER::ENSIGHT_MED_WRONLY_DRIVER(const ENSIGHT_MED_WRONLY_DRIVER & driver) : ENSIGHT_MED_DRIVER(driver)
{
  _ensightFile = new ofstream();
}

ENSIGHT_MED_WRONLY_DRIVER::~ENSIGHT_MED_WRONLY_DRIVER()
{
  delete _ensightFile ;
}

GENDRIVER * ENSIGHT_MED_WRONLY_DRIVER::copy() const
{
  return new ENSIGHT_MED_WRONLY_DRIVER(*this) ;
}

void ENSIGHT_MED_WRONLY_DRIVER::openConst() const {

  const char * LOC ="ENSIGHT_MED_WRONLY_DRIVER::open() : ";

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

void ENSIGHT_MED_WRONLY_DRIVER::closeConst() const {

  const char * LOC = "ENSIGHT_MED_WRONLY_DRIVER::close() : ";
  BEGIN_OF(LOC);
  
  (*_ensightFile).close();
 
  if (!(*_ensightFile))
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not close main Ensight file "
				     << _fileName)
			  );
  END_OF(LOC);
}

void ENSIGHT_MED_WRONLY_DRIVER::read() throw (MEDEXCEPTION) {
  throw MEDEXCEPTION("ENSIGHT_MED_WRONLY_DRIVER::read : Can't read with a WRONLY driver !");
}

void ENSIGHT_MED_WRONLY_DRIVER::write() const throw (MEDEXCEPTION) {

  const char * LOC = "ENSIGHT_MED_WRONLY_DRIVER::write() : ";
  BEGIN_OF(LOC);

  // Well we must open ensight file first, because there are
  // no other driver than MED for ENSIGHT that do it !
  openConst() ;

  // Ensight case organization requires a main file (filename.case) which defines organization
  // We also need a geom file (filemane.geo) to store the meshs
  // We also need a data file (filemane.data) to store the fields
   
  int NumberOfMeshes = _ptrMed->getNumberOfMeshes() ;
  deque<string> MeshNames = _ptrMed->getMeshNames() ;
  // In fact, we must take care of all supports 
  // We restrict Field on all nodes or cells

  int NumberOfFields = _ptrMed->getNumberOfFields() ;
  deque<string> FieldNames = _ptrMed->getFieldNames() ;
	       
  cout << "-> creating the Ensight case file " << _fileName <<  endl ;
  (*_ensightFile) << "FORMAT" << endl ;
  (*_ensightFile) << "type:   ensight" << endl ;
  (*_ensightFile) << endl ;
  if (NumberOfMeshes){
    (*_ensightFile) << "GEOMETRY" << endl ;
    for (int i=0; i<NumberOfMeshes; i++) {
      int len       = _fileName.size() ;
      string prefix = _fileName.substr(0,len-5); // extraction de .case
      (*_ensightFile) << "# Mesh " << i+1 << " detected with name = " << MeshNames[i] << endl ;
      (*_ensightFile) << "model: " << i+1 << " " << prefix << "-" << i+1 << ".geom"  << endl ;
    }
  }
  (*_ensightFile) << endl ;
  if (NumberOfFields) (*_ensightFile) << "VARIABLE" << endl ;

  for (int i=0; i<NumberOfMeshes; i++) {
    MESH * myMesh = _ptrMed->getMesh(MeshNames[i]) ;
    writeMesh(myMesh, i) ;
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
	(*_ensightFile) << "# -Field "<< j+1 << " detected with name = " << name.str() << endl ;
	if( MeshNames[i] == myField->getSupport()->getMesh()->getName() ) { 
	  // rigth in all case : better compare pointeur ?
	  if (MED_NODE == myField->getSupport()->getEntity())
	    if (myField->getSupport()->isOnAllElements()) {
	      int NumberOfComponents =  myField->getNumberOfComponents() ;
	      ostringstream name ; 
	      name << myField->getName() << "_" << dt << "_" << it ;
	      if (NumberOfComponents==3)      (*_ensightFile) << "vector per node: " << name.str() << " " << name.str() << ".sol" << endl ;
	      else if (NumberOfComponents<=4) (*_ensightFile) << "scalar per node: " << name.str() << " " << name.str() << ".sol" << endl ;
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
	(*_ensightFile) << "# --Field "<< j+1 << " detected for " << name.str() << endl ;
	if( MeshNames[i] == myField->getSupport()->getMesh()->getName() ) { 
	  // rigth in all case : better compare pointeur ?
	  if (MED_CELL == myField->getSupport()->getEntity())
	    if (myField->getSupport()->isOnAllElements()) {
	      int NumberOfComponents =  myField->getNumberOfComponents() ;
	      ostringstream name ; 
	      name << myField->getName() << "_" << dt << "_" << it ;
	      if (NumberOfComponents==3)      (*_ensightFile) << "vector per element: " << name.str() << " " << name.str() << ".sol" << endl ;
	      else if (NumberOfComponents<=4) (*_ensightFile) << "scalar per element: " << name.str() << " " << name.str() << ".sol" << endl ;
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

void ENSIGHT_MED_WRONLY_DRIVER::writeMesh(MESH * myMesh , int imesh) const {

  const char * LOC = "ENSIGHT_MED_DRIVER::writeMesh() : ";
  BEGIN_OF(LOC);

  int len       = _fileName.size() ;
  string prefix = _fileName.substr(0,len-5); // extraction de .case
  strstream w_mesh ;
  w_mesh << prefix << "-" << imesh+1 << ".geom" << ends ;
  string ensight_geomf = w_mesh.str()  ;

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
  int SpaceDimension = myMesh->getSpaceDimension() ;
  int NumberOfNodes = myMesh->getNumberOfNodes() ;
  ensightGeomFile << NumberOfNodes << endl ;
  const double *coordinate = myMesh->getCoordinates(MED_FULL_INTERLACE) ;
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
  int cells_types_count = myMesh->getNumberOfTypes(MED_CELL) ;
//   int cells_sum = myMesh->getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS) ;
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
    const int * connectivityArray = myMesh->getConnectivity(MED_FULL_INTERLACE,MED_NODAL,MED_CELL,cells_type[i].getType());
    for (int j=0;j<numberOfCell;j++) {
      for (int k=0;k<nodes_cell;k++)
	ensightGeomFile << setw(8) << connectivityArray[j*nodes_cell+filter[k]] ;
      ensightGeomFile << endl ;
    }
    if (filter != NULL)
      delete[] filter ;
  }

  ensightGeomFile << endl ;
  return ;

  END_OF(LOC);
}

void ENSIGHT_MED_WRONLY_DRIVER::writeField(FIELD_ * myField,string name) const {

  const char * LOC = "ENSIGHT_MED_WRONLY_DRIVER::writeField() : ";
  BEGIN_OF(LOC);
  
  typedef MEDMEM_ArrayInterface<int,NoInterlace,NoGauss>::Array ArrayIntNo;
  typedef MEDMEM_ArrayInterface<double,NoInterlace,NoGauss>::Array ArrayDoubleNo;

  int NumberOfValue = myField->getSupport()->getNumberOfElements(MED_ALL_ELEMENTS) ;
  int NumberOfComponents =  myField->getNumberOfComponents() ;

  int is_node    = 0 ;
  int is_element = 0 ;
	
  string dataname = name += ".sol" ;
  ofstream ensightDataFile(name.c_str(),ios::out) ;  
  cout << "-> creating the Ensight data file " << name << endl ;
  ensightDataFile.setf(ios::scientific);	
  ensightDataFile.precision(5);	

  med_type_champ type = myField->getValueType() ;
  SCRUTE(name);
  SCRUTE(type);

  if ( myField->getSupport()->getEntity() == 0 ) is_element = 1 ;
  else if ( myField->getSupport()->getEntity() == 3 ) is_node = 1 ;

  switch (type)

    {
    case MED_INT32 : {
      MESSAGE("MED_INT32");
      if (NumberOfComponents==3) {
	if (is_node)    ensightDataFile << "vector per node integer 32 mode for " << name << " following " << endl ;
	if (is_element) ensightDataFile << "vector per element integer 32 mode for " << name << " following " << endl ;
      } else if (NumberOfComponents<=4) {
	if (is_node)    ensightDataFile << "scalar per node integer 32 mode for " << name << " following " << endl ;
	if (is_element) ensightDataFile << "scalar per element integer 32 mode for " << name << " following " << endl ;
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
	  ensightDataFile << setw(12) << value[j*NumberOfValue+i] ;
     }
      if ( myField->getInterlacingType() == MED_FULL_INTERLACE )
	delete[] myArray;
      break ;
    }
    case MED_REEL64 : {
      MESSAGE("MED_REEL64");
      if (NumberOfComponents==3) {
	if (is_node)    ensightDataFile << "vector per node real 64 mode for " << name << " following " << endl ;
	if (is_element) ensightDataFile << "vector per element real 64 mode for " << name << " following " << endl ;
      } else if (NumberOfComponents<=4) {
	if (is_node)    ensightDataFile << "scalar per node real 64 mode for " << name << " following " << endl ;
	if (is_element) ensightDataFile << "scalar per element real 64 mode for " << name << " following " << endl ;
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
      //cout << "-> number of values " << NumberOfValue << " number of components " << NumberOfComponents << endl ;
      for (int i=0; i<NumberOfValue; i++) {
	for(int j=0; j<NumberOfComponents; j++) {
	  ensightDataFile << setw(12) << value[j*NumberOfValue+i] ;
	  mypoint+=1;
	  if (mypoint == 6) {
	    ensightDataFile << endl ;
	    mypoint=0;
	  }
	}
      }
      ensightDataFile << endl ;

// genere un core  !?!?
//       if ( myField->getInterlacingType() == MED_FULL_INTERLACE )
// 	delete[] myArray;

      break ;
    }
    default : { 
      MESSAGE(LOC << "Could not write field "<<name<<" the type is not int or double !");
    }
    }

  ensightDataFile.close();  	
  
  END_OF(LOC);
}

void ENSIGHT_MED_WRONLY_DRIVER::writeSupport(SUPPORT * mySupport) const {
  const char * LOC = "ENSIGHT_MED_WRONLY_DRIVER::writeSupport(SUPPORT *) : " ;
  BEGIN_OF(LOC) ;
  MESSAGE(LOC << "Not yet implemented, acting on the object " << *mySupport);
  END_OF(LOC) ;
}

ENSIGHT_MED_RDONLY_DRIVER::ENSIGHT_MED_RDONLY_DRIVER() : ENSIGHT_MED_DRIVER()
{
  _ensightFile = new ifstream();
}

ENSIGHT_MED_RDONLY_DRIVER::ENSIGHT_MED_RDONLY_DRIVER(const string & fileName,  MED * const ptrMed) : ENSIGHT_MED_DRIVER(fileName,ptrMed)
{
  _ensightFile = new ifstream();
}

ENSIGHT_MED_RDONLY_DRIVER::ENSIGHT_MED_RDONLY_DRIVER(const ENSIGHT_MED_RDONLY_DRIVER & driver) : ENSIGHT_MED_DRIVER(driver)
{
  _ensightFile = new ifstream();
}

ENSIGHT_MED_RDONLY_DRIVER::~ENSIGHT_MED_RDONLY_DRIVER()
{
  delete _ensightFile ;
}

GENDRIVER * ENSIGHT_MED_RDONLY_DRIVER::copy() const
{
  return new ENSIGHT_MED_RDONLY_DRIVER(*this) ;
}

void ENSIGHT_MED_RDONLY_DRIVER::openConst() const {

  const char * LOC ="ENSIGHT_MED_RDONLY_DRIVER::open() : ";

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

void ENSIGHT_MED_RDONLY_DRIVER::closeConst() const {

  const char * LOC = "ENSIGHT_MED_RDONLY_DRIVER::close() : ";
  BEGIN_OF(LOC);
  
  (*_ensightFile).close();
 
  if (!(*_ensightFile))
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not close main Ensight file "
				     << _fileName)
			  );
  END_OF(LOC);
}

void ENSIGHT_MED_RDONLY_DRIVER::write() const throw (MEDEXCEPTION) {
  throw MEDEXCEPTION("ENSIGHT_MED_RDONLY_DRIVER::write : Can't write with a RDONLY driver !");
}

void ENSIGHT_MED_RDONLY_DRIVER::read() {

  const char * LOC = "ENSIGHT_MED_RDONLY_DRIVER::read() : " ;
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

  char ligne[80]; 
  int number_of_geom ;
  string geom_namefile ;
//   vector<string> field_namefile ;
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

  int is_scalar=0 ;
  int is_vector=0;
  int is_node=0;
  int is_element=0;
  int my_field=0 ;
  int is_doublon=0 ;
  int type_doublon=0 ;
//   strstream w_field ;

  MESHING* MyMeshing = new MESHING();
 
  MyMeshing->setName( "MeshFromEnsight" );

  // recuperation des arguments du fichier ensight case
  // --------------------------------------------------

  // In this release, the following options are available :
  // For GEOMETRY -> model:
  // For VARIABLE -> scalar per node:
  //              -> vector per node:
  //              -> scalar per element:
  //              -> vector per element:

  cout << "****************** READ **************** starting " << endl ;

  ifstream ensightCaseFile(_fileName.c_str(),ios::in);
  cout << "Ensight case file name to read " << _fileName << endl ;
  string diren = getDirName((char*)_fileName.c_str());

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
		cout << "name of geometrie : " << geom_namefile << endl ;
		break ;
	      }
	    }	    
	  }
	  else if ( mot_lu == "VARIABLE" ) {
	    cout << "field detected" << endl ;
            while ( ensightCaseFile >> mot_lu ){
              if ( mot_lu == "scalar" || mot_lu == "vector" ) {
                if ( mot_lu == "scalar" ) is_scalar = 1 ;
                if ( mot_lu == "vector" ) is_vector = 1 ;
		ensightCaseFile >> mot_lu ; //per
                ensightCaseFile >> mot_lu ; //node
		if ( mot_lu == "node:" )    is_node    = 1 ;
		if ( mot_lu == "element:" ) is_element = 1 ;
               else {
                  ensightCaseFile >> mot_lu ; //description
                  ensightCaseFile >> mot_lu ;
                  field_namefile = mot_lu ;
		  my_field +=1 ;
                  cout << "name of field : " << my_field << " = " << field_namefile << endl ;
                  break ; // couple la boucle sur les champs
                }
	      }
	    }
	    // cout << " scalar " << is_scalar << " vector " << is_vector << " node " << is_node << " element " << is_element << endl ;
	    if ( !(is_scalar+is_vector) ) {
	      cout << " the field ist not a scalar and is not a vector " << endl ;
	      break ;
	    }
	    else if ( !(is_node+is_element) ) {
	      cout << " the field ist not on nodes and is not on elements " << endl ;
	      break ;
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

  string cgeom_namefile;
  if( diren.length() > 0 )
    cgeom_namefile = diren + '/' + geom_namefile;
  else
    cgeom_namefile = geom_namefile;
  cout << "-> Entering into the geometry file " << cgeom_namefile << endl;

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
	  cout << "-> geom given " << geom_given << endl ;
	  for ( int i=0 ; i < NumberOfNodes ; i++) {
	    if( geom_given) ensightGeomFile >> setw(8) >> num_coordinate ;
	    ensightGeomFile >> setw(12) >> Coordinates[iCoord]   ;
	    ensightGeomFile >> setw(12) >> Coordinates[iCoord+1] ; 
	    ensightGeomFile >> setw(12) >> Coordinates[iCoord+2] ;
// 	    cout << "coordinate " << i+1 << " read : x = " << Coordinates[iCoord] << " y = " << Coordinates[iCoord+1] << " z = " << Coordinates[iCoord+2] << endl ;
// 	    cout << "coordinate read " << num_coordinate << " : x = " << Coordinates[iCoord] << " y = " << Coordinates[iCoord+1] << " z = " << Coordinates[iCoord+2] << endl ;
	    iCoord+=3 ;
	  }

	  MyMeshing->setSpaceDimension(SpaceDimension);
	  MyMeshing->setNumberOfNodes(NumberOfNodes);
	  MyMeshing->setCoordinates(SpaceDimension,NumberOfNodes,Coordinates,"CARTESIAN",MED_EN::MED_FULL_INTERLACE);
	  string coordname[3] = { "x", "y", "z" } ;
	  MyMeshing->setCoordinatesNames(coordname) ;
	  delete [] Coordinates;
	}
	else if  ( mot_lu == "part" ) {
//---------------- Connectivities part --------------------------------------------

	  while ( ensightGeomFile >> mot_lu ){
	    for ( int j = 0 ; j < 15 ; j++){
	      if( mot_lu == type_Ensight[j] ) {
		is_doublon=0 ;
		for ( int ltype = 0 ; ltype < NumberOfTypes ; ltype++ ){
		  if ( mot_lu == type_read[ltype] ) {
		    is_doublon=1 ;
		    type_doublon=ltype ;
		  }
		}
		if ( is_doublon ) {
		  // type_read already stored
		  ensightGeomFile >> number_of_cell ;
		  int last_number_of_cell = nbcell_read[type_doublon] ;
		  int total_number_of_cell = number_of_cell + last_number_of_cell ;
		  nbcell_read[type_doublon] = total_number_of_cell ;
		  total_conn[type_doublon].resize(nb_edge[j]*total_number_of_cell);
		  cout << "-> loading again by adding " << number_of_cell << " to " << last_number_of_cell << " cells connectivities of type " << type_Ensight[j] << " existing" << endl << "-> new results for " << type_Ensight[j] << " are : number of cells connectivities = " << total_number_of_cell << " and total number of values = " << nb_edge[j]*total_number_of_cell << endl ;
		  for ( int k=0 ; k < nb_edge[j]*number_of_cell ; k++ ) {
		    ensightGeomFile >> setw(8) >> conn ;
		    int last_dim = last_number_of_cell*nb_edge[j] ;
		    total_conn[type_doublon][last_dim+k]=conn ;
// 		    cout << " connectivitie " << last_number_of_cell+k+1 << " read = " << total_conn[type_doublon][last_dim+k] << endl ; 
		  }
		}
		else {
		  NumberOfTypes+=1;
		  iType=NumberOfTypes-1 ;
		  total_conn.resize(NumberOfTypes) ;
		  type_read.push_back(mot_lu) ;
		  ensightGeomFile >> number_of_cell ;
		  nbcell_read.push_back(number_of_cell) ;
		  total_conn[iType].resize(nb_edge[j]*number_of_cell);
		  cout << "-> loading " << number_of_cell << " cell connectivities of type " << type_Ensight[j] << " (total number of values = " << nb_edge[j]*number_of_cell << ") " << endl ;
		  for ( int k=0 ; k < nb_edge[j]*number_of_cell ; k++ ) {
		    ensightGeomFile >> setw(8) >> conn ;
		    total_conn[iType][k]=conn ;
// 		    cout << " connectivitie " << k+1 << " read = " << total_conn[iType][k] << endl ; 
		  }
		}
	      }
	    }	  
	  }



	}
      }
    }
    // for compilation on WNT
#ifndef WNT
    medGeometryElement classicalTypesCell[NumberOfTypes];
    int nbOfClassicalTypesCell[NumberOfTypes];
#else // massive with zero size can't exist on Win32
    medGeometryElement* classicalTypesCell = new medGeometryElement(NumberOfTypes);
    int* nbOfClassicalTypesCell = new int(NumberOfTypes);
#endif

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

    MyMeshing->setNumberOfTypes(NumberOfTypes, MED_EN::MED_CELL) ;
    MyMeshing->setTypes(classicalTypesCell, MED_EN::MED_CELL) ;
    MyMeshing->setNumberOfElements(nbOfClassicalTypesCell, MED_EN::MED_CELL);
    int MeshDimension ;
    MeshDimension = 2 ;
    for (int k=0 ; k<NumberOfTypes ; k++){
      for (int j=0 ; j<15 ; j++) 
	if(type_read[k] == type_Ensight[j] && j>6 ) MeshDimension = 3 ;
    }
    MyMeshing->setMeshDimension(MeshDimension);

    for (int k = 0 ; k < NumberOfTypes ; k++) {
      int nb_connectivities = total_conn[k].size();
      cout << "-> type = " << k+1 << " has for total number of values of connectivities : " <<  nb_connectivities << endl ;
      int *connectivities = new int[nb_connectivities];
      for (int itab=0 ; itab < nb_connectivities ; itab++) connectivities[itab]=total_conn[k][itab] ;
      for (int j=0 ; j<15 ; j++) {
	if( type_read[k] == type_Ensight[j] ) {
	  cout << "-> type read = " << type_read[k] << endl ;
	  switch ( j+1 )
	    {
	    case 1  : {MyMeshing->setConnectivity(connectivities,MED_EN::MED_CELL,MED_EN::MED_POINT1)  ; break ;}
	    case 2  : {MyMeshing->setConnectivity(connectivities,MED_EN::MED_CELL,MED_EN::MED_SEG2)    ; break ;}
	    case 3  : {MyMeshing->setConnectivity(connectivities,MED_EN::MED_CELL,MED_EN::MED_SEG3)    ; break ;}
	    case 4  : {MyMeshing->setConnectivity(connectivities,MED_EN::MED_CELL,MED_EN::MED_TRIA3)   ; break ;}
	    case 5  : {MyMeshing->setConnectivity(connectivities,MED_EN::MED_CELL,MED_EN::MED_TRIA6)   ; break ;}
	    case 6  : {MyMeshing->setConnectivity(connectivities,MED_EN::MED_CELL,MED_EN::MED_QUAD4)   ; break ;}
	    case 7  : {MyMeshing->setConnectivity(connectivities,MED_EN::MED_CELL,MED_EN::MED_QUAD8)   ; break ;}
	    case 8  : {MyMeshing->setConnectivity(connectivities,MED_EN::MED_CELL,MED_EN::MED_TETRA4)  ; break ;}
	    case 9  : {MyMeshing->setConnectivity(connectivities,MED_EN::MED_CELL,MED_EN::MED_TETRA10) ; break ;}
	    case 10 : {MyMeshing->setConnectivity(connectivities,MED_EN::MED_CELL,MED_EN::MED_PYRA5)   ; break ;}
	    case 11 : {MyMeshing->setConnectivity(connectivities,MED_EN::MED_CELL,MED_EN::MED_PYRA13)  ; break ;}
	    case 12 : {MyMeshing->setConnectivity(connectivities,MED_EN::MED_CELL,MED_EN::MED_HEXA8)   ; break ;}
	    case 13 : {MyMeshing->setConnectivity(connectivities,MED_EN::MED_CELL,MED_EN::MED_HEXA20)  ; break ;}
	    case 14 : {MyMeshing->setConnectivity(connectivities,MED_EN::MED_CELL,MED_EN::MED_PENTA6)  ; break ;}
	    case 15 : {MyMeshing->setConnectivity(connectivities,MED_EN::MED_CELL,MED_EN::MED_PENTA15) ; break ;}
	    default : break ;
	    }
	}
      }
      delete [] connectivities;
    }
    cout << "-> connectivities OK " << endl ;

//   cout << "-> Writing the mesh into med file" << endl ;
//   int id = MyMeshing->addDriver(MED_DRIVER,med_filename,MyMeshing->getName());
//   MyMeshing->write(id) ;
    cout << "-> Writing the mesh into the med object" << endl ;
    _ptrMed->addMesh(MyMeshing);

  // chargement des variables depuis le fichier ensight data
  // -------------------------------------------------------
  //  ! faire le distinguo champs aux noeuds et aux mailles OK
  //  ! faire le cas des champs multiples

//   for (int ifield=0 ; ifield < my_field ; ifield++) {
  string cfield_namefile;
  if( diren.length() > 0 )
    cfield_namefile = diren + '/' + field_namefile;
  else
    cfield_namefile = field_namefile;
  cout << "-> Entering into the field file " << cfield_namefile << endl  ;

    ifstream ensightFieldFile(cfield_namefile.c_str(),ios::in);
    if (ensightFieldFile.is_open() )
      {
	ensightFieldFile.getline(ligne, 80, '\n') ; // read the first line of comment

	medEntityMesh Entity ; 
	if ( is_node ) {
	  cout << "-> loading " << NumberOfNodes << " variables on nodes " << endl ;
	  var.resize(NumberOfNodes);
	  for (int k = 0 ; k < NumberOfNodes ; k++) ensightFieldFile >> setw(12) >> var[k] ;
	  Entity = MED_EN::MED_NODE ;
	}
	else if ( is_element ) {
	  int NumberOfElements = 0 ;
	  for ( int i = 0 ; i < NumberOfTypes ; i++ ) {
	    NumberOfElements = NumberOfElements + nbcell_read[i] ;
	  }
	  cout << "-> loading " << NumberOfElements << " variables on elements " << endl ;
	  var.resize(NumberOfElements);
	  for (int k = 0 ; k < NumberOfElements ; k++) ensightFieldFile >> setw(12) >> var[k] ;
	  Entity = MED_EN::MED_CELL ;
	}

	SUPPORT * MySupport = new SUPPORT(MyMeshing,"Support",Entity);

	int NumberOfComponents = 1 ;
	if ( is_vector ) NumberOfComponents = 3 ;
	FIELD<double> *MyField = new FIELD<double>(MySupport,NumberOfComponents);

// 	w_field << "FieldFromEnsight" << ifield << ends ;
// 	const string FieldName = "FieldFromEnsight" ;
// 	const string FieldName = w_field.str() ;

	const string FieldName = "FieldFromEnsight" ;
	MyField->setName(FieldName) ;
	string * ComponentsNames = new string[NumberOfComponents] ;
	if ( NumberOfComponents == 1 ) ComponentsNames[0] = "scalar comp" ;
	else if ( NumberOfComponents == 3 ) {
	  ComponentsNames[0] = "X comp" ;
	  ComponentsNames[1] = "Y comp" ;
	  ComponentsNames[2] = "Z comp" ;
	}
	
	MyField->setComponentsNames(ComponentsNames) ;
	const int NumberOfValues = MySupport->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS) ;
	MyField->setNumberOfValues(NumberOfValues) ;

	int NumberOf = NumberOfValues / NumberOfComponents;
	for ( int i = 1 ; i <= NumberOf ; i++) {
	  for ( int j = 1 ; j <= NumberOfComponents ; j++) {
	    MyField->setValueIJ(i,j,var[NumberOfComponents*(i-1)+j-1]) ;
	  }
	}
	cout << "-> Writing the field into the med object" << endl ;
	_ptrMed->addField(MyField);

// 	cout << "-> Writing the field into the med file" << endl ;
// 	int id = MyField.addDriver(MED_DRIVER,med_filename,MyField.getName());
// 	MyField.write(id) ;
      }
//   }


  cout << "****************** READ **************** ending " << endl ;

}
