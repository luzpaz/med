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
#include <iomanip>

#include "MEDMEM_define.hxx"

#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_Utilities.hxx"

#include "MEDMEM_ArrayInterface.hxx"

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
  string         _fieldName;
  int            _fieldNum;

public :
  
  /*!
    Constructor.
  */
  ENSIGHT_FIELD_DRIVER():GENDRIVER(),
                     _ptrField((FIELD<T> *) 0), _fieldName(""),
		     _fieldNum(MED_INVALID)
  {}
  /*!
    Constructor.
  */
  template<class INTERLACING_TAG>
  ENSIGHT_FIELD_DRIVER(const string & fileName,
		       FIELD<T,INTERLACING_TAG> * ptrField,
		   MED_EN::med_mode_acces accessMode):
    GENDRIVER(fileName, accessMode, MED_DRIVER),
    _ptrField((FIELD<T> *) ptrField),
    _fieldNum(MED_INVALID) 
  {
    _fieldName = ptrField->getName();
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
  }

  /*!
    Destructor.
  */
  virtual ~ENSIGHT_FIELD_DRIVER()
  {
  }
  virtual void open() throw (MEDEXCEPTION) = 0;
  virtual void close() =0;
  virtual void write( void ) const = 0 ;
  virtual void read ( void ) = 0 ;

  /*!
    Set the name of the FIELD asked in file.

    It could be different than the name of the FIELD object.
  */
  virtual void   setFieldName(const string & fieldName) { _fieldName = fieldName; }
  /*!
    Get the name of the FIELD asked in file.
  */
  virtual string getFieldName() const { return _fieldName; }
protected:
  virtual GENDRIVER * copy ( void ) const = 0 ;

};



template <class T> class ENSIGHT_FIELD_RDONLY_DRIVER : public ENSIGHT_FIELD_DRIVER<T>
{

public :
  
  /*!
    Constructor.
  */
  ENSIGHT_FIELD_RDONLY_DRIVER():ENSIGHT_FIELD_DRIVER<T>()
  {
    const char * LOC = "ENSIGHT_FIELD_RDONLY_DRIVER::ENSIGHT_FIELD_RDONLY_DRIVER() ";
    BEGIN_OF(LOC);

    _ensightFile = new ifstream();

  END_OF(LOC);
  }
  /*!
    Constructor.
  */
  template<class INTERLACING_TAG>
  ENSIGHT_FIELD_RDONLY_DRIVER<T>(const string & fileName,
				 FIELD<T,INTERLACING_TAG> * ptrField):ENSIGHT_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::RDONLY)
  {
  const char* LOC = "ENSIGHT_FIELD_RDONLY_DRIVER::ENSIGHT_FIELD_RDONLY_DRIVER(const string & fileName, FIELD<T> * ptrField) ";
  BEGIN_OF(LOC);
    _ensightFile = new ifstream();

  END_OF(LOC);
  }

  /*!
    Copy constructor.
  */
  ENSIGHT_FIELD_RDONLY_DRIVER(const ENSIGHT_FIELD_RDONLY_DRIVER & fieldDriver):ENSIGHT_FIELD_DRIVER<T>(fieldDriver)
  {
    _ensightFile = new ifstream();
  }

  /*!
    Destructor.
  */
  ~ENSIGHT_FIELD_RDONLY_DRIVER()
  {
  const char* LOC = "ENSIGHT_FIELD_RDONLY_DRIVER::~ENSIGHT_FIELD_RDONLY_DRIVER()";
  BEGIN_OF(LOC);

    close();

    SCRUTE(_ensightFile);

    delete _ensightFile ;

    SCRUTE(_ensightFile);

  END_OF(LOC);
  }

  void openConst() const throw (MEDEXCEPTION)
  {
    const char * LOC = "ENSIGHT_FIELD_RDONLY_DRIVER::openConst()" ;
    BEGIN_OF(LOC);

    MESSAGE(LOC<<" : _fileName.c_str : "<< ENSIGHT_FIELD_DRIVER<T>::_fileName.c_str()<<",mode : "<< ENSIGHT_FIELD_DRIVER<T>::_accessMode);

    if ( ENSIGHT_FIELD_DRIVER<T>::_fileName == "" )
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
				       << "_fileName is |\"\"|, please set a correct fileName before calling open()"
				       )
			    );

    if (!(*_ensightFile).is_open())
      (*_ensightFile).open(ENSIGHT_FIELD_DRIVER<T>::_fileName.c_str()) ; 

    SCRUTE((*_ensightFile).is_open());
    SCRUTE(_ensightFile);

    if (!(*_ensightFile))
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not open file "
				       << ENSIGHT_FIELD_DRIVER<T>::_fileName)
			    );
//     _ensightFile.setf(ios::scientific);	
//     _ensightFile.precision(5);	
  END_OF(LOC);
  }

  void openConstAppend() const throw (MEDEXCEPTION)
  {
    const char * LOC = "ENSIGHT_FIELD_DRIVER::openConstAppend()" ;
    BEGIN_OF(LOC);

    MESSAGE(LOC<<" : _fileName.c_str : "<< ENSIGHT_FIELD_DRIVER<T>::_fileName.c_str()<<",mode : "<< ENSIGHT_FIELD_DRIVER<T>::_accessMode);

    if ( ENSIGHT_FIELD_DRIVER<T>::_fileName == "" )
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
				       << "_fileName is |\"\"|, please set a correct fileName before calling open()"
				       )
			    );

    SCRUTE((*_ensightFile).is_open());

    if (!(*_ensightFile).is_open())
      {
	MESSAGE(LOC<<"The file is already close and it is opened with the right option");
	(*_ensightFile).open(ENSIGHT_FIELD_DRIVER<T>::_fileName.c_str(), ofstream::out | ofstream::app) ; 
      }
    else
      {
	MESSAGE(LOC<<"The file is still open, it is closed to make sure that it will be opened with the right option");

	(*_ensightFile).close() ;

	_ensightFile->open(ENSIGHT_FIELD_DRIVER<T>::_fileName.c_str(), ofstream::out | ofstream::app) ; 
      }

    SCRUTE((*_ensightFile).is_open());
    SCRUTE(_ensightFile);

    if (!(*_ensightFile))
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not open file "
				       << ENSIGHT_FIELD_DRIVER<T>::_fileName)
			    );
//     _ensightFile.setf(ios::scientific);	
//     _ensightFile.precision(5);	
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
				       << ENSIGHT_FIELD_DRIVER<T>::_fileName)
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
  ifstream *        _ensightFile;         // The main _ensightFile used to read
  GENDRIVER * copy ( void ) const ;

};

template <class T> class ENSIGHT_FIELD_WRONLY_DRIVER : public ENSIGHT_FIELD_DRIVER<T>
{

public :
  
  /*!
    Constructor.
  */
  ENSIGHT_FIELD_WRONLY_DRIVER():ENSIGHT_FIELD_DRIVER<T>()
  {
    const char * LOC = "ENSIGHT_FIELD_WRONLY_DRIVER::ENSIGHT_FIELD_WRONLY_DRIVER() ";
    BEGIN_OF(LOC);

    _ensightFile = new ifstream();

  END_OF(LOC);
  }
  /*!
    Constructor.
  */
  template<class INTERLACING_TAG>
  ENSIGHT_FIELD_WRONLY_DRIVER(const string & fileName,
			      FIELD<T,INTERLACING_TAG> * ptrField):
    ENSIGHT_FIELD_DRIVER<T>(fileName,ptrField,MED_EN::WRONLY)
  {
  const char* LOC = "ENSIGHT_FIELD_WRONLY_DRIVER::ENSIGHT_FIELD_WRONLY_DRIVER(const string & fileName, FIELD<T> * ptrField) ";
  BEGIN_OF(LOC);
    _ensightFile = new ifstream();

  END_OF(LOC);
  }

  /*!
    Copy constructor.
  */
  ENSIGHT_FIELD_WRONLY_DRIVER(const ENSIGHT_FIELD_WRONLY_DRIVER & fieldDriver):ENSIGHT_FIELD_DRIVER<T>(fieldDriver)
  {
    _ensightFile = new ifstream();
  }

  /*!
    Destructor.
  */
  ~ENSIGHT_FIELD_WRONLY_DRIVER()
  {
  const char* LOC = "ENSIGHT_FIELD_WRONLY_DRIVER::~ENSIGHT_FIELD_WRONLY_DRIVER()";
  BEGIN_OF(LOC);

    close();

    SCRUTE(_ensightFile);

    delete _ensightFile ;

    SCRUTE(_ensightFile);

  END_OF(LOC);
  }

  void openConst() const throw (MEDEXCEPTION)
  {
    const char * LOC = "ENSIGHT_FIELD_WRONLY_DRIVER::openConst()" ;
    BEGIN_OF(LOC);

    MESSAGE(LOC<<" : _fileName.c_str : "<< ENSIGHT_FIELD_DRIVER<T>::_fileName.c_str()<<",mode : "<< ENSIGHT_FIELD_DRIVER<T>::_accessMode);

    if ( ENSIGHT_FIELD_DRIVER<T>::_fileName == "" )
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
				       << "_fileName is |\"\"|, please set a correct fileName before calling open()"
				       )
			    );

    if (!(*_ensightFile).is_open())
      (*_ensightFile).open(ENSIGHT_FIELD_DRIVER<T>::_fileName.c_str()) ; 

    SCRUTE((*_ensightFile).is_open());
    SCRUTE(_ensightFile);

    if (!(*_ensightFile))
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not open file "
				       << ENSIGHT_FIELD_DRIVER<T>::_fileName)
			    );
//     _ensightFile.setf(ios::scientific);	
//     _ensightFile.precision(5);	
  END_OF(LOC);
  }

  void openConstAppend() const throw (MEDEXCEPTION)
  {
    const char * LOC = "ENSIGHT_FIELD_WRONLY_DRIVER::openConstAppend()" ;
    BEGIN_OF(LOC);

    MESSAGE(LOC<<" : _fileName.c_str : "<< ENSIGHT_FIELD_DRIVER<T>::_fileName.c_str()<<",mode : "<< ENSIGHT_FIELD_DRIVER<T>::_accessMode);

    if ( ENSIGHT_FIELD_DRIVER<T>::_fileName == "" )
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
				       << "_fileName is |\"\"|, please set a correct fileName before calling open()"
				       )
			    );

    SCRUTE((*_ensightFile).is_open());

    if (!(*_ensightFile).is_open())
      {
	MESSAGE(LOC<<"The file is already close and it is opened with the right option");
	(*_ensightFile).open(ENSIGHT_FIELD_DRIVER<T>::_fileName.c_str(), ofstream::out | ofstream::app) ; 
      }
    else
      {
	MESSAGE(LOC<<"The file is still open, it is closed to make sure that it will be opened with the right option");

	(*_ensightFile).close() ;

	_ensightFile->open(ENSIGHT_FIELD_DRIVER<T>::_fileName.c_str(), ofstream::out | ofstream::app) ; 
      }

    SCRUTE((*_ensightFile).is_open());
    SCRUTE(_ensightFile);

    if (!(*_ensightFile))
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not open file "
				       << ENSIGHT_FIELD_DRIVER<T>::_fileName)
			    );
//     _ensightFile.setf(ios::scientific);	
//     _ensightFile.precision(5);	
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
    const char * LOC = "ENSIGHT_FIELD_WRONLY_DRIVER::closeConst() " ;
    BEGIN_OF(LOC);

    SCRUTE(_ensightFile);
    SCRUTE(*_ensightFile);


    if ((*_ensightFile).is_open())
      (*_ensightFile).close();
  
    SCRUTE(_ensightFile);
    SCRUTE(*_ensightFile);

    if (!(*_ensightFile))
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not close file "
				       << ENSIGHT_FIELD_DRIVER<T>::_fileName)
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
  ifstream *        _ensightFile;         // The main _ensightFile used to read
  GENDRIVER * copy ( void ) const ;

};

/*-------------------------*/
/* template implementation */
/*-------------------------*/

/*--------------------- DRIVER PART -------------------------------*/

template <class T> void ENSIGHT_FIELD_RDONLY_DRIVER<T>::setFieldName(const string & fieldName)
{
  GENDRIVER::setFieldName(fieldName); 
}

template <class T> string  ENSIGHT_FIELD_RDONLY_DRIVER<T>::getFieldName() const
{
  return ENSIGHT_FIELD_DRIVER<T>::_fieldName;
}

template <class T> GENDRIVER * ENSIGHT_FIELD_RDONLY_DRIVER<T>::copy(void) const
{
  ENSIGHT_FIELD_RDONLY_DRIVER<T> * myDriver = new ENSIGHT_FIELD_RDONLY_DRIVER<T>(*this);

  return myDriver ;
}

template <class T> void ENSIGHT_FIELD_RDONLY_DRIVER<T>::read (void)
  throw (MEDEXCEPTION)
{

  vector<double> var ;
  double myVar ;
  char ligne[80]; 
  string mot_lu ;

  string field_namefile ;
  string field_read_name ;

  int is_scalar=0 ;
  int is_vector=0;
  int is_node=0;
  int is_element=0;

  // recuperation des valeurs du champ  

  ifstream ensightCaseFile(ENSIGHT_FIELD_DRIVER<T>::_fileName.c_str(),ios::in);
  cout << "-> Ensight case file name to read " << ENSIGHT_FIELD_DRIVER<T>::_fileName << endl ;

  field_read_name = "EnsightField"; // defaut name for the field

  if (ensightCaseFile.is_open() )
    {
      while ( ensightCaseFile >> mot_lu )
	{
	  if ( mot_lu == "VARIABLE" ) {
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
                  cout << "name of field : " << field_namefile << endl ;
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

  ifstream ensightFieldFile(field_namefile.c_str(),ios::in);
  cout << "-> Entering into the field file " << field_namefile << endl  ;

  if ( ensightFieldFile.is_open() )
    {
      ensightFieldFile.getline(ligne, 80, '\n') ; // read the first line of comment

//       while ( ensightFieldFile.eof() == 0 )
 
      while ( ensightFieldFile >> setw(12) >> myVar ) 
	{
	var.push_back(myVar) ;	
	}
    }
  int NumberOfValues = var.size() ;

  // construction med

  ENSIGHT_FIELD_DRIVER<T>::_ptrField->setName(field_read_name) ;

  int NumberOfComponents = 1 ;
  if ( is_vector ) NumberOfComponents = 3 ;
  ENSIGHT_FIELD_DRIVER<T>::_ptrField->setNumberOfComponents(NumberOfComponents) ;
  string * componentsNames = new string[NumberOfComponents] ;
  if ( NumberOfComponents == 1 ) componentsNames[0] = "scalar comp" ;
  else if ( NumberOfComponents == 3 ) {
    componentsNames[0] = "X comp" ;
    componentsNames[1] = "Y comp" ;
    componentsNames[2] = "Z comp" ;
  }
  ENSIGHT_FIELD_DRIVER<T>::_ptrField->setComponentsNames(componentsNames);
  delete[] componentsNames ;

  ENSIGHT_FIELD_DRIVER<T>::_ptrField->setNumberOfValues(NumberOfValues);

  int NumberOf = NumberOfValues / NumberOfComponents;
  cout << "-> Loading " << NumberOf << " NumberOf values " << endl  ;
  ENSIGHT_FIELD_DRIVER<T>::_ptrField->allocValue(NumberOfComponents);
  for ( int i = 1 ; i <= NumberOf ; i++) {
    for ( int j = 1 ; j <= NumberOfComponents ; j++) {
//       cout << "-> EnsightFieldDriver : write v = " << var[NumberOfComponents*(i-1)+j-1] << endl  ;
      ENSIGHT_FIELD_DRIVER<T>::_ptrField->setValueIJ(i,j,(T)var[NumberOfComponents*(i-1)+j-1]);
    }
  }
  cout << "NOM DU MAILLAGE : " << ENSIGHT_FIELD_DRIVER<T>::_ptrField->getSupport()->getMesh()->getName() << endl;
	
}

template <class T> void ENSIGHT_FIELD_RDONLY_DRIVER<T>::write(void) const
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("ENSIGHT_FIELD_RDONLY_DRIVER::write : Can't write with a RDONLY driver !");
}

template <class T> void ENSIGHT_FIELD_RDONLY_DRIVER<T>::writeAppend(void) const
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("ENSIGHT_FIELD_RDONLY_DRIVER::write : Can't write with a RDONLY driver !");
}

template <class T> void ENSIGHT_FIELD_WRONLY_DRIVER<T>::setFieldName(const string & fieldName)
{
  ENSIGHT_FIELD_DRIVER<T>::_fieldName = fieldName; 
}

template <class T> string  ENSIGHT_FIELD_WRONLY_DRIVER<T>::getFieldName() const
{
  return ENSIGHT_FIELD_DRIVER<T>::_fieldName;
}

template <class T> GENDRIVER * ENSIGHT_FIELD_WRONLY_DRIVER<T>::copy(void) const
{
  ENSIGHT_FIELD_WRONLY_DRIVER<T> * myDriver = new ENSIGHT_FIELD_WRONLY_DRIVER<T>(*this);

  return myDriver ;
}

template <class T> void ENSIGHT_FIELD_WRONLY_DRIVER<T>::read (void)
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("ENSIGHT_FIELD_WRONLY_DRIVER::read : Can't read with a ENSIGHT driver because it is write only driver !");
}

template <class T> void ENSIGHT_FIELD_WRONLY_DRIVER<T>::write(void) const
  throw (MEDEXCEPTION)
{

  const char * LOC = "ENSIGHT_FIELD_WRONLY_DRIVER::write(void) const " ;
  BEGIN_OF(LOC);

  string mot_lu ;
  int is_node     = 0 ;
  int is_element  = 0 ;
  int is_variable = 0 ;

//   FIELD_ * myField = ENSIGHT_FIELD_DRIVER<T>::_ptrField ;
//   MESH * meshField = supportField->getMesh();

  const SUPPORT * supportField = ENSIGHT_FIELD_DRIVER<T>::_ptrField->getSupport();

  int dt = ENSIGHT_FIELD_DRIVER<T>::_ptrField->getIterationNumber();
  int it = ENSIGHT_FIELD_DRIVER<T>::_ptrField->getOrderNumber();

  ostringstream name ;
  string nameField = ENSIGHT_FIELD_DRIVER<T>::_ptrField->getName();
  MED_EN::medEntityMesh entitySupport = supportField->getEntity();
  name << nameField << "_" << dt << "_" << it << ".sol" ;

  if ( ENSIGHT_FIELD_DRIVER<T>::_ptrField->getGaussPresence() )
    throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Could not write field "<<ENSIGHT_FIELD_DRIVER<T>::_ptrField->getName()<<" which use Gauss Points !" << entitySupport));

  if (!(supportField->isOnAllElements()))
    throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Could not write field "<<ENSIGHT_FIELD_DRIVER<T>::_ptrField->getName()<<" which is not on all entities of the mesh !" << entitySupport));

//   if (entitySupport == MED_EN::MED_NODE) int NumberOfNodes = ENSIGHT_FIELD_DRIVER<T>::_ptrField->getNumberOfElements(MED_EN::MED_NODE,MED_EN::MED_ALL_ELEMENTS) ;
//   else if (entitySupport == MED_EN::MED_CELL) int NumberOfElements = ENSIGHT_FIELD_DRIVER<T>::_ptrField->getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS)  ;
//   else
//     throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Could not write field "<<ENSIGHT_FIELD_DRIVER<T>::_ptrField->getName()<<" which is not on all nodes or cells but it's on !" << entitySupport));

  int NumberOfValue = ENSIGHT_FIELD_DRIVER<T>::_ptrField->getSupport()->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS) ;
  int NumberOfComponents =  ENSIGHT_FIELD_DRIVER<T>::_ptrField->getNumberOfComponents() ;

//   MED_EN::med_type_champ fieldType = ENSIGHT_FIELD_DRIVER<T>::_ptrField->getValueType() ;

// Case file part
//---------------

  ifstream ensightCaseFile(ENSIGHT_FIELD_DRIVER<T>::_fileName.c_str(),ios::in);
  cout << "-> Creating the Ensight case file " << ENSIGHT_FIELD_DRIVER<T>::_fileName << endl  ;

  if (ensightCaseFile.is_open() ) // test si le mot clé VARIABLE est déjà dans le case file
    { 
      while ( ensightCaseFile.eof() == 0 ) 
	{
	  ensightCaseFile >> mot_lu ;
	  if ( mot_lu == "VARIABLE" ) is_variable = 1 ;
	}
    }
//   else
//     cout << "-> Cannot access to the Ensight case file : " << ENSIGHT_FIELD_DRIVER<T>::_fileName << endl ;
  ensightCaseFile.close();

  MED_EN::med_type_champ type =  ENSIGHT_FIELD_DRIVER<T>::_ptrField->getValueType() ;
  SCRUTE(name);
  SCRUTE(type);

  if ( ENSIGHT_FIELD_DRIVER<T>::_ptrField->getSupport()->getEntity() == 0 ) is_element = 1 ;
  else if ( ENSIGHT_FIELD_DRIVER<T>::_ptrField->getSupport()->getEntity() == 3 ) is_node = 1 ;

  ofstream NensightCaseFile(ENSIGHT_FIELD_DRIVER<T>::_fileName.c_str(),ios::out|ios::app);
  NensightCaseFile << endl ;
  if (!is_variable) NensightCaseFile << "VARIABLE" << endl ; // mot clé VARIABLE non présent dans le case file, on l'ajoute

  int len       = ENSIGHT_FIELD_DRIVER<T>::_fileName.size() ;
  string prefix = ENSIGHT_FIELD_DRIVER<T>::_fileName.substr(0,len-5); // extraction de .case
  string dataname = prefix + ".sol" ;

  std::string aBaseName = getBaseName( (char*)dataname.c_str() );
  NensightCaseFile << "# --Field  detected for " << aBaseName << endl ;
  if ( NumberOfComponents > 1 ) {
    if (is_node)    NensightCaseFile << "vector per node: " << nameField << " " << aBaseName << endl ;
    if (is_element) NensightCaseFile << "vector per element: " << nameField << " " << aBaseName << endl ;
  } else {
    if (is_node)    NensightCaseFile << "scalar per node: " << nameField << " " << aBaseName << endl ;
    if (is_element) NensightCaseFile << "scalar per element: " << nameField << " " << aBaseName << endl ;
  }
  NensightCaseFile.close();

// Field file part
//----------------

  ofstream ensightDataFile(dataname.c_str(),ios::out) ;  
  cout << "-> Creating the Ensight data file " << dataname << endl ;
  ensightDataFile.setf(ios::scientific);	
  ensightDataFile.precision(5);	

  switch (type)

    {
    case MED_EN::MED_INT32 : {
      MESSAGE("MED_INT32");
      if (NumberOfComponents==3) {
	if (is_node)    ensightDataFile << "vector per node integer 32 mode for " << nameField << " following " << endl ;
	if (is_element) ensightDataFile << "vector per element integer 32 mode for " << nameField << "following " << endl ;
      } else if (NumberOfComponents<=4) {
	if (is_node)    ensightDataFile << "scalar per node integer 32 mode for " << nameField << " following " << endl ;
	if (is_element) ensightDataFile << "scalar per element integer 32 mode for " << nameField << " following " << endl ;
      } else {
	MESSAGE(LOC << "Could not write field "<< nameField << " there are more than 4 components !");
	return ;
      }

      const T * value;
      if ( ENSIGHT_FIELD_DRIVER<T>::_ptrField->getInterlacingType() == MED_EN::MED_NO_INTERLACE ) {
	value = ENSIGHT_FIELD_DRIVER<T>::_ptrField->getValue(); 
      }
      else if ( ENSIGHT_FIELD_DRIVER<T>::_ptrField->getInterlacingType() == MED_EN::MED_NO_INTERLACE_BY_TYPE ) {
	MEDMEM_Array_ * ptrArray = ENSIGHT_FIELD_DRIVER<T>::_ptrField->getArray();
	MEDMEM_Array<T,NoInterlaceByTypeNoGaussPolicy> * temp = dynamic_cast<MEDMEM_Array<T,NoInterlaceByTypeNoGaussPolicy> * >  ( ptrArray );
	MEDMEM_Array<T,NoInterlaceNoGaussPolicy> * array = ArrayConvert2No( *temp );
	value = array->getPtr();
      }
      else {
	MEDMEM_Array_ * ptrArray = ENSIGHT_FIELD_DRIVER<T>::_ptrField->getArray();
	MEDMEM_Array<T,FullInterlaceNoGaussPolicy> * temp = dynamic_cast<MEDMEM_Array<T,FullInterlaceNoGaussPolicy> * >  ( ptrArray );
	MEDMEM_Array<T,NoInterlaceNoGaussPolicy> * array = ArrayConvert( *temp );
	value = array->getPtr();
      }

      int mypoint=0;
      for (int i=0; i<NumberOfValue; i++) {
	for(int j=0; j<NumberOfComponents; j++) {
	  ensightDataFile << setw(12) << (double)value[j*NumberOfValue+i] ;
	  mypoint+=1;
	  if (mypoint == 6) {
	    ensightDataFile << endl ;
	    mypoint=0;
	  }
	}
      }
  
      if ( ENSIGHT_FIELD_DRIVER<T>::_ptrField->getInterlacingType() != MED_EN::MED_NO_INTERLACE ) delete value;
      break ;
    }

    case MED_EN::MED_REEL64 : {
      MESSAGE("MED_REEL64");
      if (NumberOfComponents==3) {
	if (is_node)    ensightDataFile << "vector per node real 64 mode for " << nameField << " following " << endl ;
	if (is_element) ensightDataFile << "vector per element real 64 mode for " << nameField << " following " << endl ;
      } else if (NumberOfComponents<=4) {
	if (is_node)    ensightDataFile << "scalar per node real 64 mode for " << nameField << " following " << endl ;
	if (is_element) ensightDataFile << "scalar per element real 64 mode for " << nameField << " following " << endl ;
      } else {
	MESSAGE(LOC << "Could not write field "<<ENSIGHT_FIELD_DRIVER<T>::_ptrField->getName()<<" there are more than 4 components !");
	return ;
      }

      const T * value;
      if ( ENSIGHT_FIELD_DRIVER<T>::_ptrField->getInterlacingType() == MED_EN::MED_NO_INTERLACE )
	value = ENSIGHT_FIELD_DRIVER<T>::_ptrField->getValue();
      else if ( ENSIGHT_FIELD_DRIVER<T>::_ptrField->getInterlacingType() == MED_EN::MED_NO_INTERLACE_BY_TYPE ) {
	MEDMEM_Array_ * ptrArray = ENSIGHT_FIELD_DRIVER<T>::_ptrField->getArray();
	MEDMEM_Array<T,NoInterlaceByTypeNoGaussPolicy> * temp = dynamic_cast<MEDMEM_Array<T,NoInterlaceByTypeNoGaussPolicy> * >  ( ptrArray );
	MEDMEM_Array<T,NoInterlaceNoGaussPolicy> * array = ArrayConvert2No( *temp ); // ici ca pond un p'tain de core //  
	value = array->getPtr();
      }
      else {
	MEDMEM_Array_ * ptrArray = ENSIGHT_FIELD_DRIVER<T>::_ptrField->getArray();
	MEDMEM_Array<T,FullInterlaceNoGaussPolicy> * temp = dynamic_cast<MEDMEM_Array<T,FullInterlaceNoGaussPolicy> * >  ( ptrArray );
	MEDMEM_Array<T,NoInterlaceNoGaussPolicy> * array = ArrayConvert( *temp ); // ici ca pond un p'tain de core //  
	value = array->getPtr();
      }

      int mypoint=0;
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
 
      if ( ENSIGHT_FIELD_DRIVER<T>::_ptrField->getInterlacingType() != MED_EN::MED_NO_INTERLACE ) delete value;
      ensightDataFile << endl ;
      break ;
    }
    default : { 
      MESSAGE(LOC << "Could not write field "<<name<<" the type is not int or double !");
    }
    }

  ensightDataFile.close();  

}	

template <class T> void ENSIGHT_FIELD_WRONLY_DRIVER<T>::writeAppend(void) const
  throw (MEDEXCEPTION)
{
  const char* LOC = "ENSIGHT_FIELD_DRIVER::writeAppend(void) const ";
  BEGIN_OF(LOC);

// redondant avec write()

  END_OF(LOC);
}

}//End namespace MEDMEM

#endif /* ENSIGHT_FIELD_DRIVER_HXX */
