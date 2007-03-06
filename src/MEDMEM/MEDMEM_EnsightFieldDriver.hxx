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
#ifndef ENSIGHT_FIELD_DRIVER_HXX
#define ENSIGHT_FIELD_DRIVER_HXX

#include <string>
#include <fstream>
#include <sstream>

#include "MEDMEM_define.hxx"

#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_Utilities.hxx"

#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Unit.hxx"
#include "MEDMEM_nArray.hxx"
#include "MEDMEM_ArrayConvert.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_CellModel.hxx"


/*!

  Driver Ensight for FIELD.

  Generic part : implement open and close methods.

*/

namespace MEDMEM {
template <class T> class ENSIGHT_FIELD_DRIVER : public GENDRIVER
{
protected:
  
  FIELD<T> *     _ptrField;
  mutable ofstream *        _ensightFile ;
  string         _fieldName;
  int            _fieldNum;

public :
  
  /*!
    Constructor.
  */
  template <class INTERLACING_TAG>
  ENSIGHT_FIELD_DRIVER():GENDRIVER(),
                     _ptrField((FIELD<T> *) 0), _fieldName(""),
		     _fieldNum(MED_INVALID)
  {
    const char * LOC = "ENSIGHT_FIELD_DRIVER::ENSIGHT_FIELD_DRIVER() ";
    BEGIN_OF(LOC);

    _ensightFile = new ofstream();

    END_OF(LOC);
  }
  /*!
    Constructor.
  */
  template <class INTERLACING_TAG>
  ENSIGHT_FIELD_DRIVER(const string & fileName,
		   FIELD<T, INTERLACING_TAG> * ptrField):
    GENDRIVER(fileName,MED_EN::MED_WRONLY),
    _ptrField((FIELD<T> *) ptrField),
    _fieldNum(MED_INVALID) 
  {
    const char * LOC = "ENSIGHT_FIELD_DRIVER::ENSIGHT_FIELD_DRIVER(const string & fileName, FIELD<T> * ptrField) ";
    BEGIN_OF(LOC);
    _fieldName = ptrField->getName();
    _ensightFile = new ofstream();

    END_OF(LOC);
  }

  /*!
    Copy constructor.
  */
  ENSIGHT_FIELD_DRIVER(const ENSIGHT_FIELD_DRIVER & fieldDriver):
    GENDRIVER(fieldDriver),
    _ptrField(fieldDriver._ptrField),
    _fieldName(fieldDriver._fieldName),
    _fieldNum(fieldDriver._fieldNum) 
  {
    _ensightFile = new ofstream();
  }

  /*!
    Destructor.
  */
  ~ENSIGHT_FIELD_DRIVER()
  {
  const char * LOC ="ENSIGHT_FIELD_DRIVER::~ENSIGHT_FIELD_DRIVER()";
  BEGIN_OF(LOC);

  close();

  SCRUTE(_ensightFile);

  delete _ensightFile ;

  SCRUTE(_ensightFile);

  END_OF(LOC);
  }

  void openConst() const throw (MEDEXCEPTION)
  {
    const char * LOC = "ENSIGHT_FIELD_DRIVER::openConst()" ;
    BEGIN_OF(LOC);

    MESSAGE(LOC<<" : _fileName.c_str : "<< _fileName.c_str()<<",mode : "<< _accessMode);

    if ( _fileName == "" )
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
				       << "_fileName is |\"\"|, please set a correct fileName before calling open()"
				       )
			    );

    if (!(*_ensightFile).is_open())
      (*_ensightFile).open(_fileName.c_str()) ; 

    SCRUTE((*_ensightFile).is_open());
    SCRUTE(_ensightFile);

    if (!(*_ensightFile))
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not open file "
				       << _fileName)
			    );
    END_OF(LOC);
  }

  void openConstAppend() const throw (MEDEXCEPTION)
  {
    const char * LOC = "ENSIGHT_FIELD_DRIVER::openConstAppend()" ;
    BEGIN_OF(LOC);

    MESSAGE(LOC<<" : _fileName.c_str : "<< _fileName.c_str()<<",mode : "<< _accessMode);

    if ( _fileName == "" )
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
				       << "_fileName is |\"\"|, please set a correct fileName before calling open()"
				       )
			    );

    SCRUTE((*_ensightFile).is_open());

    if (!(*_ensightFile).is_open())
      {
	MESSAGE(LOC<<"The file is already close and it is opened with the right option");
	(*_ensightFile).open(_fileName.c_str(), ofstream::out | ofstream::app) ; 
      }
    else
      {
	MESSAGE(LOC<<"The file is still open, it is closed to make sure that it will be opened with the right option");

	(*_ensightFile).close() ;

	_ensightFile->open(_fileName.c_str(), ofstream::out | ofstream::app) ; 
      }

    SCRUTE((*_ensightFile).is_open());
    SCRUTE(_ensightFile);

    if (!(*_ensightFile))
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not open file "
				       << _fileName)
			    );
    END_OF(LOC);
  }

  void open() throw (MEDEXCEPTION)
  {
    openConst() ;
  }

  void openAppend() throw (MEDEXCEPTION)
  {
    openConstAppend() ;
  }

  void closeConst() const throw (MEDEXCEPTION)
  {
    const char * LOC = "ENSIGHT_FIELD_DRIVER::closeConst() " ;
    BEGIN_OF(LOC);

    SCRUTE(_ensightFile);
    SCRUTE(*_ensightFile);


    if ((*_ensightFile).is_open())
      (*_ensightFile).close();
  
    SCRUTE(_ensightFile);
    SCRUTE(*_ensightFile);

    if (!(*_ensightFile))
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not close file "
				       << _fileName)
			    );

    END_OF(LOC);
  }

  void close() {
    closeConst() ;
  }

  /*!
    Set the name of the FIELD asked in file.

    It could be different than the name of the FIELD object.
  */
  void   setFieldName(const string & fieldName) ;

  /*!
    Get the name of the FIELD asked in file.
  */
  string getFieldName() const ;

  /*!
    Return a MEDEXCEPTION : it is the write-only driver.
  */
  void read ( void ) throw (MEDEXCEPTION) ;

  /*!
    Write FIELD in the specified file, with its mesh through its support
    which has to be on all entities (excluding the faces in 3d and edges
    in 2d).
  */
  void write( void ) const throw (MEDEXCEPTION) ;

  /*!
    Write FIELD in the specified file, the mesh is supposed to be
    written in this file. The field support has to be on all entities
    (excluding the faces in 3d and edges in 2d).
  */
  void writeAppend( void ) const throw (MEDEXCEPTION);

private:
  GENDRIVER * copy ( void ) const ;

};

/*-------------------------*/
/* template implementation */
/*-------------------------*/

/*--------------------- DRIVER PART -------------------------------*/

template <class T> void ENSIGHT_FIELD_DRIVER<T>::setFieldName(const string & fieldName)
{
  _fieldName = fieldName; 
}

template <class T> string  ENSIGHT_FIELD_DRIVER<T>::getFieldName() const
{
  return _fieldName;
}

template <class T> GENDRIVER * ENSIGHT_FIELD_DRIVER<T>::copy(void) const
{
  ENSIGHT_FIELD_DRIVER<T> * myDriver = new ENSIGHT_FIELD_DRIVER<T>(*this);

  return myDriver ;
}

template <class T> void ENSIGHT_FIELD_DRIVER<T>::read (void)
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("ENSIGHT_FIELD_DRIVER::read : Can't read with a ENSIGHT driver because it is write only driver !");
}

template <class T> void ENSIGHT_FIELD_DRIVER<T>::write(void) const
  throw (MEDEXCEPTION)
{
  const char * LOC = "ENSIGHT_FIELD_DRIVER::write(void) const " ;
  BEGIN_OF(LOC);

  // ------------ open the main Ensight file 
  string ensight_case(ENSIGHT_FIELD_DRIVER::ENSIGHT_FIELD_DRIVER::_fileName) ;
  replace(ensight_case.begin(),ensight_case.end(),'.','\0');
  string ensight_casef(ensight_case.c_str()) ;
  ensight_casef  = ensight_casef +=".case" ;
  ofstream ensightCaseFile(ensight_casef.c_str(),ios::app);
  ensightCaseFile << "VARIABLE" << endl ;
  ensightCaseFile << "# Field "<< 1 << " detected with name = " << _fieldName << endl ;
  ensightCaseFile << "scalar per node: " << _ptrField->getName() << " " << ENSIGHT_FIELD_DRIVER::ENSIGHT_FIELD_DRIVER::_fileName << endl ;

  // we get the Support and its associated Mesh

  const SUPPORT * supportField = _ptrField->getSupport();
  MESH * meshField = supportField->getMesh();

  // Well we must open ensight file first, because there are
  // no other driver than MED for ENSIGHT that do it !
  openConst() ;

  // could we put more than one Mesh ?????
  (*_ensightFile) << "# ensight DataFile Version 2.01" << endl 
	   << "maillage from MedMemory"  << endl ;
  // only ASCII for the moment (binary came later :-)
  (*_ensightFile) << "ASCII" << endl ;

  (*_ensightFile) << "DATASET UNSTRUCTURED_GRID" << endl ;
  // put points (all point are in 3D, so if we are in 1D or 2D, we complete by zero !
  int SpaceDimension = meshField->getSpaceDimension() ;
  int NumberOfNodes = meshField->getNumberOfNodes() ;
  (*_ensightFile) << "POINTS " << NumberOfNodes << " float" << endl ;
  const double *coordinate = meshField->getCoordinates(MED_EN::MED_FULL_INTERLACE) ;
  for (int i=0;i<NumberOfNodes;i++) {
    for (int j=0;j<SpaceDimension;j++)
      (*_ensightFile) << coordinate[i*SpaceDimension+j] << " " ;
    if (SpaceDimension==1) 
      (*_ensightFile) << "0 0" ;
    if (SpaceDimension==2) 
      (*_ensightFile) << "0" ;
    (*_ensightFile) << endl ;
  }

  // we put connectivity
  // how many cells and how many value in connectivity :
  int cells_types_count = meshField->getNumberOfTypes(MED_EN::MED_CELL) ;
  int cells_sum = meshField->getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS) ;
  const CELLMODEL * cells_type = meshField->getCellsTypes(MED_EN::MED_CELL) ;

  const int * connectivityIndex = meshField->getConnectivityIndex(MED_EN::MED_NODAL,MED_EN::MED_CELL) ;

  int connectivity_sum =  connectivityIndex[cells_sum]-1 ;

  (*_ensightFile) << "CELLS " << cells_sum << " " << connectivity_sum+cells_sum << endl ;
  // we put connectivity
  for (int i=0;i<cells_types_count;i++) {
    int *filter = (int*) NULL ; // index in ensight connectivity
    switch (cells_type[i].getType())
      {
      case MED_EN::MED_POINT1  : {
	filter = new int[1] ;
	filter[0] = 0 ;
        break ;
      }
      case MED_EN::MED_SEG2    : {
        filter = new int[2] ;
	filter[0] = 0 ;
        filter[1] = 1 ;
        break ;
      }
      case MED_EN::MED_SEG3    : {  
        filter = new int[3] ;
	filter[0] = 0 ;
        filter[1] = 2 ;
        filter[2] = 1 ;
        break ;
      }
      case MED_EN::MED_TRIA3   : {
        filter = new int[3] ;
        filter[0] = 0 ;
        filter[1] = 2 ;
 	filter[2] = 1 ;
        break ;
      }
      case MED_EN::MED_QUAD4   : {
        filter = new int[4] ;
        filter[0] = 0 ;
        filter[1] = 3 ;
        filter[2] = 2 ;
  	filter[3] = 1 ;
        break ;
      }
      case MED_EN::MED_TRIA6   : {
        filter = new int[6] ;
        filter[0] = 0 ;
        filter[1] = 2 ;
 	filter[2] = 1 ;
        filter[3] = 5 ;
        filter[4] = 4 ;
 	filter[5] = 3 ;
        break ;
      }
      case MED_EN::MED_QUAD8   : {
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
      case MED_EN::MED_TETRA4  : {
        filter = new int[4] ;
        filter[0] = 0 ;
        filter[1] = 1 ;
        filter[2] = 3 ;  // 3td element in med are 4th in ensight (array begin at 0 !)
        filter[3] = 2 ;  // 4th element in med are 3rd in ensight (array begin at 0 !)
        break ;
      }
      case MED_EN::MED_PYRA5   : {
        filter = new int[5] ;
        filter[0] = 0 ;
        filter[1] = 3 ;  // 2nd element in med are 4th in ensight (array begin at 0 !)
        filter[2] = 2 ;
        filter[3] = 1 ;  // 4th element in med are 2nd in ensight (array begin at 0 !)
  	filter[4] = 4 ;
        break ;
      }
      case MED_EN::MED_PENTA6  : {
        filter = new int[6] ;
        filter[0] = 0 ;
        filter[1] = 2 ;
        filter[2] = 1 ;
        filter[3] = 3 ;
        filter[4] = 5 ;
  	filter[5] = 4 ;
	break ;
      }
      case MED_EN::MED_HEXA8   : {
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
      case MED_EN::MED_TETRA10 : {
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
      case MED_EN::MED_PYRA13  : {
        break ;
      }
      case MED_EN::MED_PENTA15 : {
        break ;
      }
      case MED_EN::MED_HEXA20  : {
        break ;
      }
      default : { 
        break ;
      }
      }
    if (filter==NULL) 
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": MED element type not supported yet : " << cells_type[i].getName() ) ) ;
    int nodes_cell = cells_type[i].getNumberOfNodes();
    int numberOfCell = meshField->getNumberOfElements(MED_EN::MED_CELL,cells_type[i].getType()) ;
    const int * connectivityArray = meshField->getConnectivity(MED_EN::MED_FULL_INTERLACE,MED_EN::MED_NODAL,MED_EN::MED_CELL,cells_type[i].getType());
    for (int j=0;j<numberOfCell;j++) {
      (*_ensightFile) << nodes_cell << " " ;
      for (int k=0;k<nodes_cell;k++)
	(*_ensightFile) << connectivityArray[j*nodes_cell+filter[k]] - 1 << " " ;
      (*_ensightFile) << endl ;
    }
    if (filter != NULL)
      delete[] filter ;
  }
  (*_ensightFile) << endl ;
  // we put cells type
  (*_ensightFile) << "CELL_TYPES " << cells_sum << endl ;
  for (int i=0;i<cells_types_count;i++) {
    int ensightType = 0 ;
    switch (cells_type[i].getType())
      {
      case MED_EN::MED_POINT1  : {
	ensightType = 1 ;
        break ;
      }
      case MED_EN::MED_SEG2    : {
	ensightType = 3 ;
        break ;
      }
      case MED_EN::MED_SEG3    : {  
	ensightType = 0 ;
        break ;
      }
      case MED_EN::MED_TRIA3   : {
	ensightType = 5 ;
        break ;
      }
      case MED_EN::MED_QUAD4   : {
	ensightType = 9 ;
        break ;
      }
      case MED_EN::MED_TRIA6   : {
	ensightType = 0 ;
        break ;
      }
      case MED_EN::MED_QUAD8   : {
	ensightType = 0 ;
        break ;
      }
      case MED_EN::MED_TETRA4  : {
	ensightType = 10 ;
        break ;
      }
      case MED_EN::MED_PYRA5   : {
	ensightType = 14 ;
        break ;
      }
      case MED_EN::MED_PENTA6  : {
	ensightType = 13 ;
	break ;
      }
      case MED_EN::MED_HEXA8   : {
	ensightType = 12 ;
        break ;
      }
      case MED_EN::MED_TETRA10 : {
	ensightType = 15 ;
        break ;
      }
      case MED_EN::MED_PYRA13  : {
	ensightType = 0 ;
        break ;
      }
      case MED_EN::MED_PENTA15 : {
	ensightType = 0 ;
        break ;
      }
      case MED_EN::MED_HEXA20  : {
	ensightType = 0 ;
        break ;
      }
      default : { 
	ensightType = 0 ;
        break ;
      }
      }
    if (ensightType == 0)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": MED element type not supported yet : " << cells_type[i].getType() ) ) ;
    int numberOfCell = meshField->getNumberOfElements(MED_EN::MED_CELL,cells_type[i].getType()) ;
    for (int j=0;j<numberOfCell;j++)
      (*_ensightFile) << ensightType << endl ;
  }

  // first : field on node
  // fields is on all node !

  // second : field on cell
  // fields is on all cell !

  int dt = _ptrField->getIterationNumber();
  int it = _ptrField->getOrderNumber();

  ostringstream name ;
  string nameField = _ptrField->getName();
  MED_EN::medEntityMesh entitySupport = supportField->getEntity();
  name << nameField << "_" << dt << "_" << it ;

  if ( _ptrField->getGaussPresence() )
    throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Could not write field "<<_ptrField->getName()<<" which use Gauss Points !" << entitySupport));

  if (!(supportField->isOnAllElements()))
    throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Could not write field "<<_ptrField->getName()<<" which is not on all entities of the mesh !" << entitySupport));

  if (entitySupport == MED_EN::MED_NODE)
    (*_ensightFile) << "POINT_DATA " << meshField->getNumberOfNodes() << endl ;
  else if (entitySupport == MED_EN::MED_CELL)
    (*_ensightFile) << "CELL_DATA " << meshField->getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS) << endl ;
  else
    throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Could not write field "<<_ptrField->getName()<<" which is not on all nodes or cells but it's on !" << entitySupport));

  int NomberOfValue = supportField->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS) ;
  int NomberOfComponents =  _ptrField->getNumberOfComponents() ;

  MED_EN::med_type_champ fieldType = _ptrField->getValueType() ;

  SCRUTE(name.str());
  SCRUTE(fieldType);

  switch (fieldType)
    {
    case MED_EN::MED_INT32 :
      {
	break ;
      }
    case MED_EN::MED_REEL64 :
      {
	break ;
      }
    default :
      { 
	throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Could not write field "<< name.str() <<" the type is not int or double !"));
      }
    }

  if (NomberOfComponents==3)
    (*_ensightFile) << "VECTORS " << name.str() << " int" << endl ;
  else if (NomberOfComponents<=4)
    {
      (*_ensightFile) << "SCALARS " << name.str() << " int " << NomberOfComponents << endl ;
      (*_ensightFile) << "LOOKUP_TABLE default" << endl ;
    }
  else
    throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Could not write field "<<_ptrField->getName()<<" there are more than 4 components !"));

  // IL EST POSSIBLE D'AVOIR LES DEUX BOUCLES D'ECRITURE POUR EVITER
  // DE CONVERTIR LE CHAMP DANS LE BON TYPE D'ENTRELACEMENT
  const T * value;
  if ( _ptrField->getInterlacingType() == MED_EN::MED_NO_INTERLACE )
    value = _ptrField->getValue();
  else {
    MEDMEM_Array_ * ptrArray = _ptrField->getArray();
    MEDMEM_Array<T,FullInterlaceNoGaussPolicy> * temp = dynamic_cast<MEDMEM_Array<T,FullInterlaceNoGaussPolicy> * >  ( ptrArray );
    MEDMEM_Array<T,NoInterlaceNoGaussPolicy> * array = ArrayConvert( *temp );
    value = array->getPtr();
  }

  for (int i=0; i<NomberOfValue; i++)
    {
      for(int j=0; j<NomberOfComponents; j++)
	(*_ensightFile) << value[j*NomberOfValue+i] << " " ;
      (*_ensightFile) << endl ;
    }
  
  if ( _ptrField->getInterlacingType() == MED_EN::MED_FULL_INTERLACE )
    delete value;
  END_OF(LOC);
}

template <class T> void ENSIGHT_FIELD_DRIVER<T>::writeAppend(void) const
  throw (MEDEXCEPTION)
{
  const char * LOC = "ENSIGHT_FIELD_DRIVER::writeAppend(void) const " ;
  BEGIN_OF(LOC);

  // we get the Support and its associated Mesh

  const SUPPORT * supportField = _ptrField->getSupport();
  MESH * meshField = supportField->getMesh();

  // Well we must open ensight file first, because there are
  // no other driver than MED for ENSIGHT that do it !
  openConstAppend() ;

  // first : field on node
  // fields is on all node !

  // second : field on cell
  // fields is on all cell !

  int dt = _ptrField->getIterationNumber();
  int it = _ptrField->getOrderNumber();

  ostringstream name ;
  string nameField = _ptrField->getName();
  MED_EN::medEntityMesh entitySupport = supportField->getEntity();
  name << nameField << "_" << dt << "_" << it ;

  if ( _ptrField->getGaussPresence() )
    throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Could not write field "<<_ptrField->getName()<<" which use Gauss Points !" << entitySupport));

  if (!(supportField->isOnAllElements()))
    throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Could not write field "<<_ptrField->getName()<<" which is not on all entities of the mesh !" << entitySupport));

  if (entitySupport == MED_EN::MED_NODE)
    (*_ensightFile) << "POINT_DATA " << meshField->getNumberOfNodes() << endl ;
  else if (entitySupport == MED_EN::MED_CELL)
    (*_ensightFile) << "CELL_DATA " << meshField->getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS) << endl ;
  else
    throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Could not write field "<<_ptrField->getName()<<" which is not on all nodes or cells but it's on !" << entitySupport));

  int NomberOfValue = supportField->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS) ;
  int NomberOfComponents =  _ptrField->getNumberOfComponents() ;

  MED_EN::med_type_champ fieldType = _ptrField->getValueType() ;

  SCRUTE(name.str());
  SCRUTE(fieldType);
  switch (fieldType)
    {
    case MED_EN::MED_INT32 :
      {
	break ;
      }
    case MED_EN::MED_REEL64 :
      {
	break ;
      }
    default :
      { 
	throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Could not write field "<< name.str() <<" the type is not int or double !"));
      }
    }

  if (NomberOfComponents==3)
    (*_ensightFile) << "VECTORS " << name.str() << " int" << endl ;
  else if (NomberOfComponents<=4)
    {
      (*_ensightFile) << "SCALARS " << name.str() << " int " << NomberOfComponents << endl ;
      (*_ensightFile) << "LOOKUP_TABLE default" << endl ;
    }
  else
    throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Could not write field "<<_ptrField->getName()<<" there are more than 4 components !"));

  const T * value ;
  if ( _ptrField->getInterlacingType() == MED_EN::MED_NO_INTERLACE )
    value = _ptrField->getValue();
  else {
    MEDMEM_Array_ * ptrArray = _ptrField->getArray();
    MEDMEM_Array<T,FullInterlaceNoGaussPolicy> * temp = dynamic_cast<MEDMEM_Array<T,FullInterlaceNoGaussPolicy> * >  ( ptrArray );
    MEDMEM_Array<T,NoInterlaceNoGaussPolicy> * array = ArrayConvert( *temp );
    value = array->getPtr();
  }

  for (int i=0; i<NomberOfValue; i++)
    {
      for(int j=0; j<NomberOfComponents; j++)
	(*_ensightFile) << value[j*NomberOfValue+i] << " " ;
      (*_ensightFile) << endl ;
    }

  if ( _ptrField->getInterlacingType() == MED_EN::MED_FULL_INTERLACE )
    delete value;

  END_OF(LOC);
}
}//End namespace MEDMEM

#endif /* ENSIGHT_FIELD_DRIVER_HXX */
