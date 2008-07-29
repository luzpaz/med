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
# include <string> 
# include <math.h>

# include "MEDMEM_Med.hxx"
# include "MEDMEM_DriverFactory.hxx" 
# include "MEDMEM_STRING.hxx"

# include "MEDMEM_Mesh.hxx"
# include "MEDMEM_Grid.hxx"
# include "MEDMEM_Field.hxx"
# include "MEDMEM_Group.hxx"
# include "MEDMEM_Family.hxx"

# include "MEDMEM_Exception.hxx"
# include "MEDMEM_Utilities.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

#define MED_NOPDT -1

/*!

\defgroup MED_constructors Constructors

\defgroup MED_query Query methods
These methods enable the user to retrieve information
about a MED file structure, i.e. the meshes, supports
and fields that it contains.

\defgroup MED_io File I/O methods
These methods are similar to their MESH counterparts. 
Reading should be achieved thrfough the MED constructor, while writing 
is performed thanks to two calls : one attaching a driver to the MED
object, the other one writing the file.

*/
MED::MED() {
  MESSAGE("MED::MED()");
};

/*! \if MEDMEM_ug 
\addtogroup MED_constructors
@{
\endif
*/

/*!
This constructor constructs the Med object from the file \a filename.
The driver type can specify whether the file is opened in read, write or
read/write mode. Specifying MED_DRIVER as a \a driverType opens the file in read/write mode.
It is also possible to use VTK_DRIVER to open a VTK ascii file.
*/
MED::MED(driverTypes driverType, const string & fileName)
{
  //const char * LOC = "MED::MED(driverTypes driverType, const string & fileName) : ";
  BEGIN_OF("MED::MED(driverTypes driverType, const string & fileName) : ");

  MESSAGE(__LOC << "driverType = " << driverType);

  int current = addDriver(driverType,fileName,RDONLY);

  _drivers[current]->open();
  _drivers[current]->readFileStruct();
  _drivers[current]->close();

  END_OF();
};

/*!\if MEDMEM_ug @} \endif */


/*!\ifnot MEDMEM_ug
  Destructor.
\endif
*/
MED::~MED()
{
  //const char * LOC = "MED::~MED() : ";
  BEGIN_OF("MED::~MED() : ");

  // Analysis of the object MED

  int index;

  map<FIELD_ *, MESH_NAME_>::const_iterator currentField;
  index = 0;
  for ( currentField=_meshName.begin();currentField != _meshName.end(); currentField++ ) {
    if ( (*currentField).first != NULL) index++;
  }

  MESSAGE(__LOC << " there is(are) " << index << " field(s):");
  for ( currentField=_meshName.begin();currentField != _meshName.end(); currentField++ ) {
    if ( (*currentField).first != NULL) MESSAGE("             " << ((*currentField).first)->getName().c_str());
  }

  map<MESH_NAME_, map<MED_EN::medEntityMesh,SUPPORT *> >::iterator itSupportOnMesh ;
  index = 0;
  for ( itSupportOnMesh=_support.begin();itSupportOnMesh != _support.end(); itSupportOnMesh++ ) {
    map<MED_EN::medEntityMesh,SUPPORT *>::iterator itSupport ;
    for ( itSupport=(*itSupportOnMesh).second.begin();itSupport!=(*itSupportOnMesh).second.end();itSupport++)
      index++;
  }

  MESSAGE(__LOC << " there is(are) " << index << " support(s):");

  map<MESH_NAME_,MESH*>::const_iterator  currentMesh;
  index =0;
  for ( currentMesh=_meshes.begin();currentMesh != _meshes.end(); currentMesh++ ) {
    if ( (*currentMesh).second != NULL)
      index++;
  }

  MESSAGE(__LOC << " there is(are) " << index << " meshe(s):");
//   for ( currentMesh=_meshes.begin();currentMesh != _meshes.end(); currentMesh++ ) {
//     if ( (*currentMesh).second != NULL)
//       {
// 	SCRUTE((*currentMesh).second);

// 	string meshName = ((*currentMesh).second)->getName();

// 	MESSAGE("             " << meshName);
//       }
//   }

  // delete all ? : PG : YES !
  //  map<FIELD_ *, MESH_NAME_>::const_iterator currentField;
  for ( currentField=_meshName.begin();currentField != _meshName.end(); currentField++ ) {
    if ( (*currentField).first != NULL) {
      // cast in right type to delete it !
      switch ((*currentField).first->getValueType()) {
      case MED_INT32 : 
	delete (FIELD<int>*) (*currentField).first ;
	break ;
      case MED_REEL64 :
	delete (FIELD<double>*) (*currentField).first ;
	break ;
      default : 
	MESSAGE(__LOC << "Field has type different of int or double, could not destroy its values array !") ;
	delete (*currentField).first;
      }
    }
  }
  //  map<MESH_NAME_, map<MED_EN::medEntityMesh,SUPPORT *> >::iterator itSupportOnMesh ;
  for ( itSupportOnMesh=_support.begin();itSupportOnMesh != _support.end(); itSupportOnMesh++ ) {
    map<MED_EN::medEntityMesh,SUPPORT *>::iterator itSupport ;
    for ( itSupport=(*itSupportOnMesh).second.begin();itSupport!=(*itSupportOnMesh).second.end();itSupport++)
      if (! dynamic_cast<GROUP*>( (*itSupport).second ) &&
          ! dynamic_cast<FAMILY*>( (*itSupport).second ) )
	delete (*itSupport).second ;
  }

  //  map<MESH_NAME_,MESH*>::const_iterator  currentMesh;
  for ( currentMesh=_meshes.begin();currentMesh != _meshes.end(); currentMesh++ ) {
    if ( (*currentMesh).second != NULL)
      {
	if (!((*currentMesh).second)->getIsAGrid())
	  delete (*currentMesh).second;
	else
	  delete (GRID *) (*currentMesh).second;
      }
  }

  index =_drivers.size();

  MESSAGE(__LOC << "In this object MED there is(are) " << index << " driver(s):");

  for (unsigned int ind=0; ind < _drivers.size(); ind++ )
    {
      SCRUTE(_drivers[ind]);
      if ( _drivers[ind] != NULL) delete _drivers[ind];
    }



  END_OF();
} ;

/*! \if MEDMEM_ug
\addtogroup MED_io
@{
\endif
*/

/*!
  Create the specified driver and return its index reference to path to 
  read or write methods.
*/
int MED::addDriver(driverTypes driverType,
		   const string & fileName="Default File Name.med",
		   MED_EN::med_mode_acces access)
{
  //const char * LOC = "MED::addDriver(driverTypes driverType, const string & fileName=\"Default File Name.med\") : ";

  BEGIN_OF("MED::addDriver(driverTypes driverType, const string & fileName=\"Default File Name.med\") : ");

  MESSAGE(__LOC << " the file name is " << fileName);

  SCRUTE(driverType);
  SCRUTE(access);

  GENDRIVER *driver = DRIVERFACTORY::buildDriverForMed(driverType,fileName,
						       this,access);

  _drivers.push_back(driver);

  int current = _drivers.size()-1;

  driver->setId(current); 

  END_OF();

  return current;
}

/*!
  Duplicate the given driver and return its index reference to path to 
  read or write methods.
*/
int  MED::addDriver(GENDRIVER & driver) {
  //const char * LOC = "MED::addDriver(GENDRIVER &) : ";
  int current;

  BEGIN_OF("MED::addDriver(GENDRIVER &) : ");
  
  SCRUTE(_drivers.size());

  _drivers.push_back(&driver);

  SCRUTE(_drivers.size());

  SCRUTE(_drivers[0]);
  SCRUTE(driver);

  current = _drivers.size()-1;
  SCRUTE(current);
  driver.setId(current); 

  END_OF();

  return current;
  
}
/*! \if MEDMEM_ug @} \endif */

/*!
  Remove the driver referenced by its index.
*/
void MED::rmDriver (int index/*=0*/)
  throw (MED_EXCEPTION)
{
  const char * LOC = "MED::rmDriver (int index=0): ";
  BEGIN_OF(LOC);

  if (index >= 0 && index < _drivers.size() && _drivers[index]) {
    delete _drivers[index];
    _drivers[index] = 0;
    //_drivers.erase(&_drivers[index]); 
  }
  else
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "The index given is invalid, index must be between 0 and |" 
                                     << _drivers.size()
                                     )
                          );   
  END_OF();
}

/*!
  ??? to do comment ???
*/
void MED::writeFrom (int index/*=0*/)
  throw (MED_EXCEPTION)
{
  const char * LOC = "MED::write (int index=0): ";
  BEGIN_OF(LOC);

  if (index >= 0 && index < _drivers.size() && _drivers[index]) {
    // open and close are made by all objects !
    _drivers[index]->writeFrom();
  }
  else {
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "The index given is invalid, index must be between 0 and |" 
                                     << _drivers.size()
                                     )
                          ); 
  }
  END_OF();
}; 

/*!\if MEDMEM_ug 
\addtogroup MED_io
@{
\endif
*/

/*!
  Write all objects with the driver given by its index.
*/
void MED::write (int index/*=0*/)
  throw (MED_EXCEPTION)
{
  const char * LOC = "MED::write (int index=0): ";
  BEGIN_OF(LOC);

  if (_drivers[index]) {
    // open and close are made by the subsequent objects !
    _drivers[index]->write(); 
  }
  else
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "The index given is invalid, index must be between  0 and |" 
                                     << _drivers.size()
                                     )
                          ); 
  END_OF();
}; 
/*!\if MEDMEM_ug @} \endif */

/*!
  Parse all the file and generate empty object.

  All object must be read explicitly later with their own method read 
  or use MED::read to read all.

  This method is automatically called by constructor with driver information.
*/
void MED::readFileStruct (int index/*=0*/)
  throw (MED_EXCEPTION)
{
  const char * LOC = "MED::readFileStruct (int index=0): ";
  BEGIN_OF(LOC);
  
  if (_drivers[index]) {
    _drivers[index]->open(); 
    _drivers[index]->readFileStruct(); 
    _drivers[index]->close(); 
  }
  else
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "The index given is invalid, index must be between 0 and |" 
                                     << _drivers.size()
                                     )
                          );   
}

/*!
  Read all objects in the file specified in the driver given by its index.
*/
void MED::read  (int index/*=0*/)
  throw (MED_EXCEPTION)
{
  const char * LOC = "MED::read (int index=0): ";
  BEGIN_OF(LOC);
  
  SCRUTE(index);

  if (_drivers[index]) {
    // open and close are made by all objects !
    SCRUTE(index);
    SCRUTE(_drivers[index]);
    SCRUTE(&_drivers[index]);
    //    _drivers[index]->open();
    _drivers[index]->read();
    //    _drivers[index]->close();
  }
  else
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
				     << "The index given is invalid, index must be between  >0 and < |" 
				     << _drivers.size()-1 
				     )
			  );  
  END_OF();
  
};

// ------- End Of Drivers Management Part

/*! \if MEDMEM_ug
\addtogroup MED_query
@{
\endif
*/

/*!
  Gets the number of MESH objects.
*/
int      MED::getNumberOfMeshes ( void ) const
{
  //const char * LOC = "MED::getNumberOfMeshes ( void ) const : ";
  BEGIN_OF("MED::getNumberOfMeshes ( void ) const : ");

  return _meshes.size();
};   
    
/*!
  Gets the number of FIELD objects.
*/
int      MED::getNumberOfFields ( void ) const
{
  //const char * LOC = "MED::getNumberOfFields ( void ) const : ";
  BEGIN_OF("MED::getNumberOfFields ( void ) const : ");

  return _fields.size(); // we get number of field with different name
};       

/*!
  Gets the names of all MESH objects.

  meshNames is an in/out argument.

  It is a string array of size the
  number of MESH objects. It must be allocated before calling
  this method. All names are put in it.
*/
void MED::getMeshNames      ( string * meshNames ) const
  throw (MED_EXCEPTION)
{
  //const char * LOC = "MED::getMeshNames ( string * ) const : ";
  BEGIN_OF("MED::getMeshNames ( string * ) const : ");

  // REM : ALLOCATION D'UN TABLEAU DE POINTEURS SUR STRING FAITE PAR LE CLIENT
  map<MESH_NAME_,MESH*>::const_iterator  currentMesh; // ??ITERATEUR CONST SUR UN OBJET NON CONST ??

  int meshNamesIndex = 0;

  for ( currentMesh=_meshes.begin();currentMesh != _meshes.end(); currentMesh++ ) {
    meshNames[meshNamesIndex]=(*currentMesh).first;
    meshNamesIndex++;                               // CF OPTIMISATION
  }

  END_OF();
};

/*!
  Gets the names of all MESH objects.

  Returns a deque<string> object which contain the name of all MESH objects.
*/
deque<string> MED::getMeshNames      () const
{
  //const char * LOC = "MED::getMeshNames () const : ";
  BEGIN_OF("MED::getMeshNames () const : ");

  deque<string> meshNames(_meshes.size());
  
  map<MESH_NAME_,MESH*>::const_iterator  currentMesh; // ??ITERATEUR CONST SUR UN OBJET NON CONST ??

  int meshNamesIndex = 0;

  for ( currentMesh=_meshes.begin();currentMesh != _meshes.end(); currentMesh++ ) {
    meshNames[meshNamesIndex]=(*currentMesh).first;
    meshNamesIndex++;                               // CF OPTIMISATION
  }

  END_OF();
  return meshNames ;
};


/*!
  Returns a reference to the MESH object named meshName.
*/
MESH   * MED::getMesh           ( const string & meshName )  const
  throw (MED_EXCEPTION)
{

  const char * LOC = "MED::getMesh ( const string & meshName ) const : ";
  BEGIN_OF(LOC);

  map<MESH_NAME_,MESH*>::const_iterator itMeshes =  _meshes.find(meshName);

  if ( itMeshes == _meshes.end() )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "There is no known mesh named |" 
                                     << meshName << "|"
                                     )
                          );
  
  return (*itMeshes).second;
  
  END_OF();
}

/*!
 \internal Returns a reference to the MESH object associated with 
 field argument.
*/
MESH   * MED::getMesh           (const FIELD_ * const field ) const
  throw (MED_EXCEPTION)
{
 
  const char * LOC = "MED::getMesh ( const FIELD * field ) const : ";
  BEGIN_OF(LOC);

  FIELD_ * f = const_cast< FIELD_* > (field);     //  Comment faire mieux ?
  map<FIELD_ *, MESH_NAME_>::const_iterator itMeshName = _meshName.find(f);

  if ( itMeshName  == _meshName.end() )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "There is no known mesh associated with |" 
                                     << field << "| pointer"
                                     )
                          );

  string meshName = (*itMeshName).second;
  map<MESH_NAME_,MESH*>::const_iterator itMeshes =  _meshes.find(meshName);
  if ( itMeshes == _meshes.end() )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC)
                                     << "There is no known mesh named |"
                                     << meshName << " while it's associated with the found field |"
                                     << field << "| pointer"
                                     )
                          );

  END_OF();

  return (*itMeshes).second;
};


/*!
  Gets the names of all FIELD objects.

  fieldNames is an in/out argument.

  It is an array of string of size the
  number of FIELD objects. It must be allocated before calling
  this method. All names are put in it.
*/
void MED::getFieldNames     ( string * fieldNames ) const
  throw (MED_EXCEPTION)
{
  //const char * LOC = "MED::getFieldNames ( string * ) const : ";
  BEGIN_OF("MED::getFieldNames ( string * ) const : ");

//  unsigned int fieldNamesSize =  sizeof(fieldNames) / sizeof(string *);
 
//  if ( fieldNamesSize != _fields.size() )
//    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
//                                     << "Size of parameter fieldNames is |" 
//                                     << fieldNamesSize     << "| and should be |" 
//                                     << _fields.size() << "| and should be |" 
//                                     )
//                         );   
  
  // REM : ALLOCATION D'UN TABLEAU DE POINTEURS SUR STRING FAITE PAR LE CLIENT
  map<FIELD_NAME_,MAP_DT_IT_>::const_iterator  currentField;

  int fieldNamesIndex = 0;
  for ( currentField=_fields.begin();currentField != _fields.end(); currentField++ ) {
    fieldNames[fieldNamesIndex]=(*currentField).first;
    fieldNamesIndex++;                               // CF OPTIMISATION
  }

  END_OF();

};

/*!
  Gets the names of all FIELD objects.

  Returns a deque<string> object which contain the name of all FIELD objects.
*/
deque<string> MED::getFieldNames     () const
{
  //const char * LOC = "MED::getFieldNames ( ) const : ";
  BEGIN_OF("MED::getFieldNames ( ) const : ");

  deque<string> fieldNames(_fields.size());

  map<FIELD_NAME_,MAP_DT_IT_>::const_iterator  currentField;

  int fieldNamesIndex = 0;

  for ( currentField=_fields.begin();currentField != _fields.end(); currentField++ ) {
    fieldNames[fieldNamesIndex]=(*currentField).first;
    fieldNamesIndex++;                               // CF OPTIMISATION
  }

  END_OF();
  return fieldNames ;
};

/*!
  Returns a deque<DT_IT_> which contain all iteration step for the FIELD 
  identified by its name. DT_IT_ definition is 
\verbatim
typedef struct { int dt; int it; } DT_IT_;
\endverbatim

\a dt represents the time iteration number, while \a it represents
the inner iteration number.

*/
deque<DT_IT_> MED::getFieldIteration (const string & fieldName) const
  throw (MED_EXCEPTION)
{

  const char * LOC = "MED::getFieldIteration ( const string & ) const : ";
  BEGIN_OF(LOC);

  map<FIELD_NAME_,MAP_DT_IT_>::const_iterator itFields = _fields.find(fieldName);
  
  if ( itFields == _fields.end() ) 
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "There is no known field named |" 
                                     << fieldName << "|"
                                     )
                          );   
  //  const MAP_DT_IT_ & myIterationMap =  const_cast<const MAP_DT_IT_ & > ((*itFields).second);
  const MAP_DT_IT_ & myIterationMap =  (*itFields).second ;
  MAP_DT_IT_::const_iterator currentIterator ;

  int iterationSize = myIterationMap.size();
  
  SCRUTE(iterationSize);

  deque<DT_IT_> Iteration(iterationSize);
  
  int iterationIndex = 0;
  
  for (currentIterator = myIterationMap.begin();
       currentIterator != myIterationMap.end(); currentIterator++ )
    {
      SCRUTE(((*currentIterator).first).dt);
      SCRUTE(((*currentIterator).first).it);

      Iteration[iterationIndex].dt = ((*currentIterator).first).dt;
      Iteration[iterationIndex].it = ((*currentIterator).first).it;

      //      Iteration[iterationIndex]=(*currentIterator).first;
      SCRUTE(Iteration[iterationIndex].dt);
      SCRUTE(Iteration[iterationIndex].it);
      iterationIndex++;                               // CF OPTIMISATION
    }

  END_OF();
  return Iteration ;
};

/*!
 Returns a reference to the FIELD object named fieldName with 
 time step number dt and order number it.
*/
FIELD_  * MED::getField          ( const string & fieldName, const int dt=MED_NOPDT, const int it=MED_NOPDT ) const
  throw (MED_EXCEPTION)
{

  const char * LOC = "MED::getField ( const string &, const int, const int ) const : ";
  BEGIN_OF(LOC);

  MESSAGE(LOC << "fieldName = "<<fieldName<<", dt ="<<dt<<", it = "<<it);

  DT_IT_ dtIt;

  dtIt.dt= dt;
  dtIt.it= it;

  map<FIELD_NAME_,MAP_DT_IT_>::const_iterator itFields = _fields.find(fieldName);

  if ( itFields == _fields.end() ) 
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "There is no known field named |" 
                                     << fieldName << "|"
                                     )
                          );   

  const MAP_DT_IT_ & map_dtIt =  const_cast<const MAP_DT_IT_ & > ((*itFields).second);
  MAP_DT_IT_::const_iterator itMap_dtIt =  map_dtIt.find(dtIt);

  if ( itMap_dtIt == map_dtIt.end() )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "There is no (dt,it) |("
                                     << dt << "," << it << ")| associated with the found field |" 
                                     << fieldName << "|"
                                     )
                          );   

  END_OF();

  //return _fields[fieldName][dtIt];
  return (*itMap_dtIt).second;
  
};
/*!\if MEDMEM_ug
 @}
 \endif 
*/


/*!
 Returns a reference to the FIELD object named fieldName with 
 time and iteration nb it.
*/
FIELD_  *MED::getField2(const string & fieldName, double time, int it) const throw (MEDEXCEPTION)
{
  const char * LOC = "MED::getField2 ( const string &, const int, const int ) const : ";
  const double eps=1e-9;
  map<FIELD_NAME_,MAP_DT_IT_>::const_iterator itFields=_fields.find(fieldName);
  if ( itFields == _fields.end() ) 
    throw MED_EXCEPTION (LOCALIZED( STRING(LOC) << "There is no known field named |" << fieldName << "|"));
  MAP_DT_IT_::const_iterator iters=(*itFields).second.begin();
  bool found=false;
  FIELD_  *ret;
  for(;iters!=(*itFields).second.end() && !found;iters++)
    if(fabs((*iters).second->getTime()-time)<eps)
      //if(it==(*iters).first.it)
	{
	  ret=(*iters).second;
	  found=true;
	}
  if(!found)
    throw MED_EXCEPTION(LOCALIZED( STRING(LOC) << "There is no known field existing at specified time and iteration !!! "));
  return ret;
  //END_OF();
}

// fiend ostream & MED::operator<<(ostream &os,const MED & med) const {
//   return os;
// };


/*!
  Returns a map<MED_EN::medEntityMesh,SUPPORT*> which contain 
  foreach entity, a reference to the SUPPORT on all elements.
*/
const map<MED_EN::medEntityMesh,SUPPORT*> & MED::getSupports(const string & meshName) const
  throw (MED_EXCEPTION)
{
  const char * LOC = "MED::getSupports ( const string ) const : ";
  BEGIN_OF(LOC);

  map<MESH_NAME_, map<MED_EN::medEntityMesh,SUPPORT *> >::const_iterator itSupportOnMesh = _support.find(meshName) ;
  
  if ( itSupportOnMesh == _support.end() )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "There is no support on mesh named |" 
                                     << meshName << "|"
                                     )
                          );
  END_OF();
  return (*itSupportOnMesh).second ;
}


/*! \if MEDMEM_ug
\addtogroup MED_query
@{
\endif
*/

/*!
  Returns a reference to the SUPPORT object on all elements of entity 
  for the MESH named meshName.
*/
SUPPORT *  MED::getSupport (const string & meshName,MED_EN::medEntityMesh entity) const 
  throw (MED_EXCEPTION)
{
  const char * LOC = "MED::getSupport ( const string, MED_EN::medEntityMesh ) const : ";
  BEGIN_OF(LOC);

  int index = 0;
  map<MESH_NAME_, map<MED_EN::medEntityMesh,SUPPORT *> >::const_iterator const_itSupportOnMesh;

  for (const_itSupportOnMesh=_support.begin(); const_itSupportOnMesh != _support.end();
       const_itSupportOnMesh++ )
    {
      map<MED_EN::medEntityMesh,SUPPORT *>::const_iterator const_itSupport ;
      for (const_itSupport=(*const_itSupportOnMesh).second.begin();
	   const_itSupport!=(*const_itSupportOnMesh).second.end();const_itSupport++) index++;
    }

  MESSAGE(LOC << "In this MED object there is(are) " << index << " support(s):");

  for (const_itSupportOnMesh=_support.begin();const_itSupportOnMesh != _support.end(); const_itSupportOnMesh++ )
    {
      map<MED_EN::medEntityMesh,SUPPORT *>::const_iterator const_itSupport ;
      for (const_itSupport=(*const_itSupportOnMesh).second.begin();
	   const_itSupport!=(*const_itSupportOnMesh).second.end();const_itSupport++)
	{
	  MESSAGE(LOC << "Support on mesh " << (*const_itSupportOnMesh).first << " on entity " << (*const_itSupport).first << " : " << *((*const_itSupport).second));
	}
  }

  const_itSupportOnMesh = _support.find(meshName) ;
  
  if ( const_itSupportOnMesh == _support.end() )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "There is no support on mesh named |" 
                                     << meshName << "|"
                                     )
                          );
 
//   map<MED_EN::medEntityMesh,SUPPORT *> & SupportOnMesh = (map<MED_EN::medEntityMesh,SUPPORT *>&) ((*itSupportOnMesh).second) ;
//   map<MED_EN::medEntityMesh,SUPPORT *>::const_iterator itSupport = SupportOnMesh.find(entity) ;
  
//   if (itSupport == SupportOnMesh.end() )
//     throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
//                                      << "There is no support on entity "
// 				     << entity << " in mesh named |" 
//                                      << meshName << "|"
//                                      )
//                           );


  map<MED_EN::medEntityMesh,SUPPORT *> SupportOnMesh = ((*const_itSupportOnMesh).second);

  map<MED_EN::medEntityMesh,SUPPORT *>::const_iterator itSupport = SupportOnMesh.find(entity) ;
  
  if (itSupport == SupportOnMesh.end() )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "There is no support on entity "
				     << entity << " in mesh named |" 
                                     << meshName << "|"
                                     )
                          );
  END_OF();
  return (*itSupport).second ;
};
/*!\if MEDMEM_ug @} \endif */



/*!
The need for this method arises from the following situation.
When loading a mesh, Medmem reads the constituent elements
in the Med file. It is possible at this stage that not all
constituent elements are stored in memory (For instance, reading 
a 2D mesh, not all the edges are present, because only 
the boundaries were stored in the file). 
When computing descending connectivities, Medmem stores all the 
faces and has a corresponding numbering. This introduces a
discrepancy between the support numbering which was determined at
file loading and the new numbering. The following method
synchronizes the two numberings.
*/
void MED::updateSupport ()
{
  //const char * LOC = "MED::updateSupport () : ";
  BEGIN_OF("MED::updateSupport () : ");

  map<MESH_NAME_, map<MED_EN::medEntityMesh,SUPPORT *> >::iterator itSupportOnMesh ;
  for ( itSupportOnMesh=_support.begin();itSupportOnMesh != _support.end(); itSupportOnMesh++ ) {
    map<MED_EN::medEntityMesh,SUPPORT *>& anEntity2Support = (*itSupportOnMesh).second;
    map<MED_EN::medEntityMesh,SUPPORT *> anEntity2SupportTmp;
    map<MED_EN::medEntityMesh,SUPPORT *>::iterator itSupport ;
    for ( itSupport=anEntity2Support.begin();itSupport!=anEntity2Support.end();itSupport++)
    {
      MED_EN::medEntityMesh aKey = (*itSupport).first;
      SUPPORT* aData = (*itSupport).second;
      try {
        aData->update() ;
        anEntity2SupportTmp[aKey] = aData;
      }
      catch (MEDEXCEPTION & ex) {
	// entity not defined in mesh -> we remove support on it !
	MESSAGE(__LOC<<ex.what());
	delete (*itSupport).second ;
	//(*itSupportOnMesh).second.erase(itSupport) ; // that's right ????
	//itSupport-- ;
        map<MED_EN::medEntityMesh,SUPPORT *>::iterator itSupportCurr = itSupport;
        itSupport--; // decrement before erase()
	(*itSupportOnMesh).second.erase(itSupportCurr);
      }
    }

    // some entities has not defined in mesh -> we should remove their supports!
    anEntity2Support.swap( anEntity2SupportTmp );

    for ( itSupport=anEntity2SupportTmp.begin();itSupport!=anEntity2SupportTmp.end();itSupport++)
    {
      MED_EN::medEntityMesh aKey = (*itSupport).first;
      SUPPORT* aData = (*itSupport).second;
      if( anEntity2Support.find( aKey ) == anEntity2Support.end() )
        delete aData;
    }
  }

  END_OF();
}

/*!
  Adds the given MESH object. MED object control it,
  and destroy it, so you must not destroy it after.

  The meshName is given by the MESH object.
*/
void MED::addMesh( MESH * const ptrMesh)
  throw (MED_EXCEPTION)
{
  const char * LOC = "MED::addMesh(const MESH * ptrMesh): ";
  BEGIN_OF(LOC);

  if ( ! ptrMesh ) 
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "ptrMesh must not be NULL !"));
 
  string meshName;
  if ( ! ( meshName = ptrMesh->getName()).size() ) 
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "ptrMesh->_name must not be NULL !"));

//   MESH * meshToMed = new MESH(*ptrMesh); DO WE HAVE TO COPY THE ENTRY MESH OR NOT ????? (NB)

  _meshes[meshName] = ptrMesh; // if ptrMesh->meshName already exists it is modified

//   _meshes[meshName] = meshToMed;

  END_OF();
}

/*!
  Add the given FIELD object. MED object control it,
  and destroy it, so you must not destroy it after.

  The fieldName is given by the FIELD object.
*/
void MED::addField( FIELD_ * const ptrField)
  throw (MED_EXCEPTION)
{
  const char * LOC = "MED::addField(const FIELD_ * const ptrField): ";
  BEGIN_OF(LOC);
  
  if ( ! ptrField ) 
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "ptrField must not be NULL !"));

  string fieldName;
  if ( ! (fieldName = ptrField->getName()).size() ) 
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "ptrField->_name must not be NULL !"));
  
  SUPPORT * ptrSupport;
  if ( ! ( ptrSupport = (SUPPORT * ) ptrField->getSupport()) ) 
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "ptrField->_support must not be NULL !"));
  
  MESH * ptrMesh;
  if ( ! ( ptrMesh = ptrSupport->getMesh()) ) 
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "ptrField->_support->_mesh must not be NULL !"));

  string meshName;
  if ( ! ( meshName = ptrMesh->getName()).size() ) 
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "ptrField->_support->_mesh->_name must not be NULL !"));

  DT_IT_ dtIt;
  dtIt.dt  = ptrField->getIterationNumber();
  dtIt.it  = ptrField->getOrderNumber();
		
  _fields   [fieldName][dtIt] = ptrField; // if it already exists it is replaced
  _meshName [ptrField]        = meshName; // if it already exists it is replaced
  _meshes   [meshName]        = ptrMesh;  // if it already exists it is replaced

  //  int  numberOfTypes = ptrSupport->getNumberOfTypes(); !! UNUSED VARIABLE !!
  _support  [meshName][ptrSupport->getEntity()] = ptrSupport;// if it already exists it is replaced


  END_OF();
}
