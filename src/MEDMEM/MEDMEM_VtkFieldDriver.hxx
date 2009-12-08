//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef VTK_FIELD_DRIVER_HXX
#define VTK_FIELD_DRIVER_HXX

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

  Driver Med for FIELD.

  Generic part : implement open and close methods.

*/

namespace MEDMEM {
template <class T> class VTK_FIELD_DRIVER : public GENDRIVER
{
protected:
  
  FIELD<T> *     _ptrField;
  mutable ofstream *        _vtkFile ;
  string         _fieldName;
  int            _fieldNum;

public :

  // all MED cell type ?? Classe de Définition ??
  //   static const medGeometryElement all_cell_type[MED_NBR_GEOMETRIE_MAILLE];
  
  //   static const char * const all_cell_type_tab [MED_NBR_GEOMETRIE_MAILLE];
  
  /*!
    Constructor.
  */
  template <class INTERLACING_TAG>
  VTK_FIELD_DRIVER():GENDRIVER(),
                     _ptrField((FIELD<T> *) 0), _fieldName(""),
                     _fieldNum(MED_INVALID)
  {
    const char * LOC = "VTK_FIELD_DRIVER::VTK_FIELD_DRIVER() ";
    BEGIN_OF_MED(LOC);

    _vtkFile = new ofstream();

  END_OF_MED(LOC);
  }
  /*!
    Constructor.
  */
  template <class INTERLACING_TAG>
  VTK_FIELD_DRIVER(const string & fileName,
                   FIELD<T, INTERLACING_TAG> * ptrField):
    GENDRIVER(fileName, MED_EN::WRONLY, VTK_DRIVER),
    _ptrField((FIELD<T> *) ptrField),
    _fieldName(fileName),_fieldNum(MED_INVALID) 
  {
  const char* LOC = "VTK_FIELD_DRIVER::VTK_FIELD_DRIVER(const string & fileName, FIELD<T> * ptrField) ";
  BEGIN_OF_MED(LOC);

    _vtkFile = new ofstream();

  END_OF_MED(LOC);
  }

  /*!
    Copy constructor.
  */
  VTK_FIELD_DRIVER(const VTK_FIELD_DRIVER & fieldDriver):
    GENDRIVER(fieldDriver),
    _ptrField(fieldDriver._ptrField),
    _fieldName(fieldDriver._fieldName),
    _fieldNum(fieldDriver._fieldNum) 
  {
    _ptrField->addDriver(*this);
    _vtkFile = new ofstream();
  }

  /*!
    Destructor.
  */
  ~VTK_FIELD_DRIVER()
  {
  const char* LOC = "VTK_FIELD_DRIVER::~VTK_FIELD_DRIVER()";
  BEGIN_OF_MED(LOC);

    close();

    SCRUTE_MED(_vtkFile);

    delete _vtkFile ;

    SCRUTE_MED(_vtkFile);

  END_OF_MED(LOC);
  }

  void openConst() const throw (MEDEXCEPTION)
  {
    const char * LOC = "VTK_FIELD_DRIVER::openConst()" ;
    BEGIN_OF_MED(LOC);

    MESSAGE_MED(LOC<<" : _fileName.c_str : "<< _fileName.c_str()<<",mode : "<< _accessMode);

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


    SCRUTE_MED((*_vtkFile).is_open());
    SCRUTE_MED(_vtkFile);



    if (!(*_vtkFile))
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not open file "
                                       << _fileName)
                            );
  END_OF_MED(LOC);
  }

  void openConstAppend() const throw (MEDEXCEPTION)
  {
    const char * LOC = "VTK_FIELD_DRIVER::openConstAppend()" ;
    BEGIN_OF_MED(LOC);

    MESSAGE_MED(LOC<<" : _fileName.c_str : "<< _fileName.c_str()<<",mode : "<< _accessMode);

    if ( _fileName == "" )
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                       << "_fileName is |\"\"|, please set a correct fileName before calling open()"
                                       )
                            );

    SCRUTE_MED((*_vtkFile).is_open());

    if (!(*_vtkFile).is_open())
      {
        MESSAGE_MED(LOC<<"The file is already close and it is opened with the right option");
        (*_vtkFile).open(_fileName.c_str(), ofstream::out | ofstream::app) ; 
      }
    else
      {
        MESSAGE_MED(LOC<<"The file is still open, it is closed to make sure that it will be opened with the right option");
        //      closeConst();


        (*_vtkFile).close() ;

        _vtkFile->open(_fileName.c_str(), ofstream::out | ofstream::app) ; 
      }
//    if (*_vtkFile)
//      _status = MED_OPENED ;
//    else


    SCRUTE_MED((*_vtkFile).is_open());
    SCRUTE_MED(_vtkFile);



    if (!(*_vtkFile))
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not open file "
                                       << _fileName)
                            );
  END_OF_MED(LOC);
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
    const char * LOC = "VTK_FIELD_DRIVER::closeConst() " ;
    BEGIN_OF_MED(LOC);

    SCRUTE_MED(_vtkFile);
    SCRUTE_MED(*_vtkFile);


    if ((*_vtkFile).is_open())
      (*_vtkFile).close();
  
//    if (*_vtkFile)
//      _status = MED_CLOSED ;
//    else

    SCRUTE_MED(_vtkFile);
    SCRUTE_MED(*_vtkFile);

    if ( (*_vtkFile) && _vtkFile->is_open() )
      //if (!(*_vtkFile))
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not close file "
                                       << _fileName)
                            );

  END_OF_MED(LOC);
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

template <class T> void VTK_FIELD_DRIVER<T>::setFieldName(const string & fieldName)
{
  _fieldName = fieldName; 
}

template <class T> string  VTK_FIELD_DRIVER<T>::getFieldName() const
{
  return _fieldName;
}

template <class T> GENDRIVER * VTK_FIELD_DRIVER<T>::copy(void) const
{
  VTK_FIELD_DRIVER<T> * myDriver = new VTK_FIELD_DRIVER<T>(*this);

  return myDriver ;
}

template <class T> void VTK_FIELD_DRIVER<T>::read (void)
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("VTK_FIELD_DRIVER::read : Can't read with a VTK driver because it is write only driver !");
}

template <class T> void VTK_FIELD_DRIVER<T>::write(void) const
  throw (MEDEXCEPTION)
{
  const char * LOC = "VTK_FIELD_DRIVER::write(void) const " ;
  BEGIN_OF_MED(LOC);

  // we get the Support and its associated Mesh

  const SUPPORT * supportField = _ptrField->getSupport();
  MESH * meshField = supportField->getMesh();
  if (! meshField )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": mesh was not read before writing")) ;

  // Well we must open vtk file first, because there are
  // no other driver than MED for VTK that do it !
  openConst() ;

  // could we put more than one Mesh ?????
  (*_vtkFile) << "# vtk DataFile Version 2.0" << endl 
           << "maillage from MedMemory"  << endl ;
  // only ASCII for the moment (binary came later :-)
  (*_vtkFile) << "ASCII" << endl ;

  (*_vtkFile) << "DATASET UNSTRUCTURED_GRID" << endl ;
  // put points (all point are in 3D, so if we are in 1D or 2D, we complete by zero !
  int SpaceDimension = meshField->getSpaceDimension() ;
  int NumberOfNodes = meshField->getNumberOfNodes() ;
  (*_vtkFile) << "POINTS " << NumberOfNodes << " float" << endl ;
  const double *coordinate = meshField->getCoordinates(MED_EN::MED_FULL_INTERLACE) ;
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
  int cells_types_count = meshField->getNumberOfTypes(MED_EN::MED_CELL) ;
  //  int * cells_count = meshField->get_cells_count() ;
  //  int cells_sum = cells_count[cells_types_count] ;
  int cells_sum = meshField->getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS) ;
  const CELLMODEL * cells_type = meshField->getCellsTypes(MED_EN::MED_CELL) ;
  //  int connectivity_sum = 0 ;

  //const int * connectivity = meshField->getConnectivity(MED_FULL_INTERLACE,MED_NODAL,MED_CELL,MED_ALL_ELEMENTS) ; !! UNUSED VARIABLE !!
  const int * connectivityIndex = meshField->getConnectivityIndex(MED_EN::MED_NODAL,MED_EN::MED_CELL) ;

  int connectivity_sum =  connectivityIndex[cells_sum]-1 ;

  (*_vtkFile) << "CELLS " << cells_sum << " " << connectivity_sum+cells_sum << endl ;
  // we put connectivity
  for (int i=0;i<cells_types_count;i++) {
    int *filter = (int*) NULL ; // index in vtk connectivity
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
        break ;
      }
      case MED_EN::MED_TRIA3   : {
        filter = new int[3] ;
        filter[0] = 0 ;
        filter[1] = 1 ;
        filter[2] = 2 ;
        break ;
      }
      case MED_EN::MED_QUAD4   : {
        filter = new int[4] ;
        filter[0] = 0 ;
        filter[1] = 1 ;
        filter[2] = 2 ;
        filter[3] = 3 ;
        break ;
      }
      case MED_EN::MED_TRIA6   : {
        break ;
      }
      case MED_EN::MED_QUAD8   : {
        break ;
      }
      case MED_EN::MED_TETRA4  : {
        filter = new int[4] ;
        filter[0] = 0 ;
        filter[1] = 1 ;
        filter[2] = 3 ;  // 3td element in med are 4th in vtk (array begin at 0 !)
        filter[3] = 2 ;  // 4th element in med are 3rd in vtk (array begin at 0 !)
        break ;
      }
      case MED_EN::MED_PYRA5   : {
        filter = new int[5] ;
        filter[0] = 0 ;
        filter[1] = 3 ;  // 2nd element in med are 4th in vtk (array begin at 0 !)
        filter[2] = 2 ;
        filter[3] = 1 ;  // 4th element in med are 2nd in vtk (array begin at 0 !)
        filter[4] = 4 ;
        break ;
      }
      case MED_EN::MED_PENTA6  : {
        filter = new int[6] ;
        filter[0] = 0 ;
        filter[1] = 1 ;
        filter[2] = 2 ;
        filter[3] = 3 ;
        filter[4] = 4 ;
        filter[5] = 5 ;
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
      case MED_EN::MED_POINT1  : {
        vtkType = 1 ;
        break ;
      }
      case MED_EN::MED_SEG2    : {
        vtkType = 3 ;
        break ;
      }
      case MED_EN::MED_SEG3    : {  
        vtkType = 0 ;
        break ;
      }
      case MED_EN::MED_TRIA3   : {
        vtkType = 5 ;
        break ;
      }
      case MED_EN::MED_QUAD4   : {
        vtkType = 9 ;
        break ;
      }
      case MED_EN::MED_TRIA6   : {
        vtkType = 0 ;
        break ;
      }
      case MED_EN::MED_QUAD8   : {
        vtkType = 0 ;
        break ;
      }
      case MED_EN::MED_TETRA4  : {
        vtkType = 10 ;
        break ;
      }
      case MED_EN::MED_PYRA5   : {
        vtkType = 14 ;
        break ;
      }
      case MED_EN::MED_PENTA6  : {
        vtkType = 13 ;
        break ;
      }
      case MED_EN::MED_HEXA8   : {
        vtkType = 12 ;
        break ;
      }
      case MED_EN::MED_TETRA10 : {
        vtkType = 0 ;
        break ;
      }
      case MED_EN::MED_PYRA13  : {
        vtkType = 0 ;
        break ;
      }
      case MED_EN::MED_PENTA15 : {
        vtkType = 0 ;
        break ;
      }
      case MED_EN::MED_HEXA20  : {
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
    int numberOfCell = meshField->getNumberOfElements(MED_EN::MED_CELL,cells_type[i].getType()) ;
    for (int j=0;j<numberOfCell;j++)
      (*_vtkFile) << vtkType << endl ;
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
    (*_vtkFile) << "POINT_DATA " << meshField->getNumberOfNodes() << endl ;
  else if (entitySupport == MED_EN::MED_CELL)
    (*_vtkFile) << "CELL_DATA " << meshField->getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS) << endl ;
  else
    throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Could not write field "<<_ptrField->getName()<<" which is not on all nodes or cells but it's on !" << entitySupport));

  int NomberOfValue = supportField->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS) ;
  int NomberOfComponents =  _ptrField->getNumberOfComponents() ;

  MED_EN::med_type_champ fieldType = _ptrField->getValueType() ;

  SCRUTE_MED(name.str());
  SCRUTE_MED(fieldType);

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
    (*_vtkFile) << "VECTORS " << name.str() << " int" << endl ;
  else if (NomberOfComponents<=4)
    {
      (*_vtkFile) << "SCALARS " << name.str() << " int " << NomberOfComponents << endl ;
      (*_vtkFile) << "LOOKUP_TABLE default" << endl ;
    }
  else
    throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Could not write field "<<_ptrField->getName()<<" there are more than 4 components !"));

  // IL EST POSSIBLE D'AVOIR LES DEUX BOUCLES D'ECRITURE POUR EVITER
  // DE CONVERTIR LE CHAMP DANS LE BON TYPE D'ENTRELACEMENT
  const T * value;
  MEDMEM_Array_ * tmpArray = 0;
  if ( _ptrField->getInterlacingType() == MED_EN::MED_NO_INTERLACE )
    value = _ptrField->getValue();
  else if ( _ptrField->getInterlacingType() == MED_EN::MED_NO_INTERLACE_BY_TYPE ) {
    MEDMEM_Array_ * ptrArray = _ptrField->getArray();
    MEDMEM_Array<T,NoInterlaceByTypeNoGaussPolicy> * temp = dynamic_cast<MEDMEM_Array<T,NoInterlaceByTypeNoGaussPolicy> * >  ( ptrArray );
    MEDMEM_Array<T,NoInterlaceNoGaussPolicy> * array = ArrayConvert2No( *temp );
    tmpArray = array;
    value = array->getPtr();
  }
  else {
    MEDMEM_Array_ * ptrArray = _ptrField->getArray();
    MEDMEM_Array<T,FullInterlaceNoGaussPolicy> * temp = dynamic_cast<MEDMEM_Array<T,FullInterlaceNoGaussPolicy> * >  ( ptrArray );
    MEDMEM_Array<T,NoInterlaceNoGaussPolicy> * array = ArrayConvert( *temp );
    tmpArray = array;
    value = array->getPtr();
  }

  for (int i=0; i<NomberOfValue; i++)
    {
      for(int j=0; j<NomberOfComponents; j++)
        (*_vtkFile) << value[j*NomberOfValue+i] << " " ;
      (*_vtkFile) << endl ;
    }
  
  if ( _ptrField->getInterlacingType() != MED_EN::MED_NO_INTERLACE )
    delete tmpArray;
  END_OF_MED(LOC);
}

template <class T> void VTK_FIELD_DRIVER<T>::writeAppend(void) const
  throw (MEDEXCEPTION)
{
  const char * LOC = "VTK_FIELD_DRIVER::writeAppend(void) const " ;
  BEGIN_OF_MED(LOC);

  // we get the Support and its associated Mesh

  const SUPPORT * supportField = _ptrField->getSupport();
  MESH * meshField = supportField->getMesh();
  if (! meshField )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": mesh was not read before writing")) ;

  // Well we must open vtk file first, because there are
  // no other driver than MED for VTK that do it !
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
    (*_vtkFile) << "POINT_DATA " << meshField->getNumberOfNodes() << endl ;
  else if (entitySupport == MED_EN::MED_CELL)
    (*_vtkFile) << "CELL_DATA " << meshField->getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS) << endl ;
  else
    throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Could not write field "<<_ptrField->getName()<<" which is not on all nodes or cells but it's on !" << entitySupport));

  int NomberOfValue = supportField->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS) ;
  int NomberOfComponents =  _ptrField->getNumberOfComponents() ;

  MED_EN::med_type_champ fieldType = _ptrField->getValueType() ;

  SCRUTE_MED(name.str());
  SCRUTE_MED(fieldType);
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
    (*_vtkFile) << "VECTORS " << name.str() << " int" << endl ;
  else if (NomberOfComponents<=4)
    {
      (*_vtkFile) << "SCALARS " << name.str() << " int " << NomberOfComponents << endl ;
      (*_vtkFile) << "LOOKUP_TABLE default" << endl ;
    }
  else
    throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Could not write field "<<_ptrField->getName()<<" there are more than 4 components !"));

  const T * value ;
  MEDMEM_Array_ * tmpArray = 0;
  if ( _ptrField->getInterlacingType() == MED_EN::MED_NO_INTERLACE )
    value = _ptrField->getValue();
  else if ( _ptrField->getInterlacingType() == MED_EN::MED_NO_INTERLACE_BY_TYPE ) {
    MEDMEM_Array_ * ptrArray = _ptrField->getArray();
    MEDMEM_Array<T,NoInterlaceByTypeNoGaussPolicy> * temp = dynamic_cast<MEDMEM_Array<T,NoInterlaceByTypeNoGaussPolicy> * >  ( ptrArray );
    MEDMEM_Array<T,NoInterlaceNoGaussPolicy> * array = ArrayConvert2No( *temp );
    tmpArray = array;
    value = array->getPtr();
  }
  else {
    MEDMEM_Array_ * ptrArray = _ptrField->getArray();
    MEDMEM_Array<T,FullInterlaceNoGaussPolicy> * temp = dynamic_cast<MEDMEM_Array<T,FullInterlaceNoGaussPolicy> * >  ( ptrArray );
    MEDMEM_Array<T,NoInterlaceNoGaussPolicy> * array = ArrayConvert( *temp );
    tmpArray = array;
    value = array->getPtr();
  }

  for (int i=0; i<NomberOfValue; i++)
    {
      for(int j=0; j<NomberOfComponents; j++)
        (*_vtkFile) << value[j*NomberOfValue+i] << " " ;
      (*_vtkFile) << endl ;
    }

  if ( _ptrField->getInterlacingType() != MED_EN::MED_NO_INTERLACE )
    delete tmpArray;

  END_OF_MED(LOC);
}
}//End namespace MEDMEM

#endif /* VTK_FIELD_DRIVER_HXX */
