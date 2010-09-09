//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "MEDMEM_MedFileBrowser.hxx"

#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_MedVersion.hxx"

#include <set>

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;


/*!
Class giving access to names of meshes and fields contained in the med file  

\defgroup MED_constructors Initialization

\defgroup MED_query Query methods
These methods enable the user to retrieve information
about a MED file structure, i.e. the meshes and fields it contains.

*/
namespace 
{
  /*!
   * \brief Structure to open a med file and to close it at destruction
   * (especially useful when we throw an exception)
   */
  struct MED_FILE
  {
    med_2_3::med_idt _id;
    MED_FILE(const string fileName)
    {
      _id = med_2_3::MEDouvrir( (const_cast <char *> (fileName.c_str())),med_2_3::MED_LECTURE );
    }
    ~MED_FILE()
    {
      if ( _id > 0 )
        med_2_3::MEDfermer(_id);
    }
     operator med_2_3::med_idt() const { return _id; }
  };
}
/*! \if MEDMEM_ug 
\addtogroup MED_constructors
@{
\endif
*/
//================================================================================
/*!
 * \brief Constructor of an empty MEDFILEBROWSER to be filled with \ref readFileStruct()
 */
//================================================================================

MEDFILEBROWSER::MEDFILEBROWSER()
{
}

//================================================================================
/*!
 * \brief This constructor constructs the MEDFILEBROWSER object from the file \a filename.
 * Names of meshes and fields are read.
 */
//================================================================================

MEDFILEBROWSER::MEDFILEBROWSER(const string & fileName) throw (MED_EXCEPTION)
{
  readFileStruct( fileName );
}

//================================================================================
/*!
 * \brief Read names of meshes and fields from the file \a fileName
 */
//================================================================================

void MEDFILEBROWSER::readFileStruct(const string & fileName) throw (MEDEXCEPTION)
{
  const char * LOC = "MEDFILEBROWSER::readFileStruct() : ";
  BEGIN_OF_MED(LOC);

  _fileName = fileName;
  _meshes.clear();
  _fields.clear();

  // =================
  // 1. Open the file
  // =================

  if ( _fileName.empty() )
    throw MED_EXCEPTION
      ( LOCALIZED( STRING(LOC) << "_fileName is |\"\"|, please provide a correct fileName"));

  MED_EN::medFileVersion version = getMedFileVersion(_fileName);
  if ( version == MED_EN::V21 )
    throw MED_EXCEPTION
      ( LOCALIZED( STRING(LOC) << _fileName << " is med-2.1 file which is of no longer supported version"));

  MED_FILE medIdt(_fileName);

  if (medIdt < 0)
    throw MED_EXCEPTION
      (LOCALIZED( STRING(LOC)<< "Can't open |"  << _fileName<< "|, medIdt : " << medIdt));

  int err,i,j;

  // =========================================
  // 2. Read number of meshes and their names
  // =========================================
  {
    char                  meshName[MED_TAILLE_NOM+1]="";
    char                  meshDescription[MED_TAILLE_DESC+1]="";
    med_2_3::med_int      meshDim;
    med_2_3::med_maillage meshType;

    int numberOfMeshes = med_2_3::MEDnMaa(medIdt) ;
    if ( numberOfMeshes <= 0 ) 
      MESSAGE_MED(LOC << "Be careful there is no mesh in file |"<<_fileName<<"| !");

    for (i=1;i<=numberOfMeshes;i++)
    {
      //get information on the i^th mesh
      err = med_2_3::MEDmaaInfo(medIdt, i, meshName, &meshDim, &meshType, meshDescription) ;

      if (err != MED_VALID)
        throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << ": can't get information about the mesh #" << i << " of the file |" << _fileName << "| !"));

      // Don't want to prevent the user from reading other correct meshes
//       if (meshType != med_2_3::MED_STRUCTURE &&
//           meshType != med_2_3::MED_NON_STRUCTURE )
//         throw MEDEXCEPTION
//           (LOCALIZED(STRING(LOC) << "Bad mesh type of mesh #"<<i <<" in file |"<<_fileName<<"|"));
      _meshes.insert( make_pair( string(meshName), meshType == med_2_3::MED_STRUCTURE ));
    }
  }

  // ===================================================================
  // 3. Read number of fields and their (timeStepNumber,iterationNumber)
  // ===================================================================
  {
    //    char                          fieldName[MED_TAILLE_NOM+1] = "";
    char                          fieldName[MED_TAILLE_LNOM+1] ; // to avoid a crash if the field name is longer than MED_TAILLE_NOM
    char                          * componentName              = (char *) MED_NULL;
    char                          * unitName                   = (char *) MED_NULL;
    char                          meshName[MED_TAILLE_NOM+1]  ;
    med_2_3::med_type_champ       type;
    MESH_ENTITIES::const_iterator currentEntity; 
    list<MED_EN::medGeometryElement>::const_iterator currentGeometry;
    med_2_3::med_int              NbOfGaussPts                 =  0;
    med_2_3::med_int              timeStepNumber               =  -1;
    char                          timeStepUnit[MED_TAILLE_PNOM+1] ;
    double                        timeStep                     = 0.0;
    med_2_3::med_int              orderNumber                  =  -1;
    med_2_3::med_int              numberOfRefMesh = 0;
    med_2_3::med_booleen          meshLink;

    int numberOfFields = med_2_3::MEDnChamp(medIdt,0) ;

    for (i=1;i<=numberOfFields;i++)
    {
      int numberOfComponents = med_2_3::MEDnChamp(medIdt,i) ;

      componentName = new char[numberOfComponents*MED_TAILLE_PNOM+1] ;
      unitName      = new char[numberOfComponents*MED_TAILLE_PNOM+1] ;   

      err = MEDchampInfo(medIdt, i, fieldName, &type, componentName, 
                         unitName, numberOfComponents) ;
      delete[] componentName ;
      delete[] unitName ;

      MESSAGE_MED("Field #" << i << " is named " << fieldName);

      if (err != MED_VALID)
        throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << ": can't get information about the field #" << i <<" of the file |" << _fileName << "| !"));

      map< string, FIELD_DATA_ >::iterator name2data =
        _fields.insert( make_pair( fieldName, FIELD_DATA_() )).first;

      FIELD_DATA_& fieldData = name2data->second;
      fieldData._type = MED_EN::med_type_champ( type );

      // find all dtid of this field
      set<DT_IT_, LT_DT_IT_> set_dtit;

      for (currentEntity=meshEntities.begin();currentEntity != meshEntities.end(); currentEntity++)
      {
        for (currentGeometry  = (*currentEntity).second.begin();
             currentGeometry != (*currentEntity).second.end();
             currentGeometry++)
        {
          int numberOfTimeSteps =
            MEDnPasdetemps(medIdt, fieldName,
                           (med_2_3::med_entite_maillage)(*currentEntity).first,
                           (med_2_3::med_geometrie_element) (*currentGeometry) );

          for (j=1;j <= numberOfTimeSteps; j++)
          {
            err = MEDpasdetempsInfo(medIdt, fieldName,
                                    (med_2_3::med_entite_maillage) (*currentEntity).first,
                                    (med_2_3::med_geometrie_element) (*currentGeometry),
                                    j,
                                    &NbOfGaussPts,
                                    &timeStepNumber, &orderNumber, timeStepUnit, &timeStep,
                                    meshName, &meshLink, &numberOfRefMesh);

            if (err == MED_VALID) // we have found for (*currentEntity).first and (*currentGeometry)
            {
              DT_IT_ dtIt;
              if (timeStepNumber<0)  timeStepNumber=-1 ;
              dtIt.dt  = timeStepNumber;
              dtIt.it  = orderNumber;

              set_dtit.insert( dtIt );
            }
          }
        }
      }
      fieldData._meshName = meshName;
      fieldData._vec_dtit.assign( set_dtit.begin(), set_dtit.end() );

    } // loop on numberOfFields
  }

  END_OF_MED(LOC);
}
/*! \if MEDMEM_ug @} \endif */

/*! \if MEDMEM_ug
\addtogroup MED_query
@{
\endif
*/

//================================================================================
/*!
 * \brief Returns the name of a browsed file
 */
//================================================================================

std::string MEDFILEBROWSER::getFileName() const
{
  return _fileName;
}

//================================================================================
/*!
  \brief Gets the number of meshes in med file.
*/
//================================================================================

int MEDFILEBROWSER::getNumberOfMeshes ( void ) const
{
  return _meshes.size();
}   
    
//================================================================================
/*!
  \brief Gets the number of fields in med file.
*/
//================================================================================

int MEDFILEBROWSER::getNumberOfFields ( void ) const
{
  return _fields.size(); // we get number of field with different name
}       

//================================================================================
/*!
  \brief Gets the names of all meshes.

  meshNames is an in/out argument.

  It is a string array of size the
  number of meshes. It must be allocated before calling
  this method. All names are put in it.
*/
//================================================================================

void MEDFILEBROWSER::getMeshNames      ( string * meshNames ) const
{
  map< string, bool >::const_iterator name_isstruct = _meshes.begin();
  for ( int i=0; name_isstruct != _meshes.end(); ++i, ++name_isstruct )
    meshNames[i] = name_isstruct->first;
}

//================================================================================
/*!
  \brief Gets the names of all MESH objects.

  Returns a vector<string> object which contain the name of all MESH objects.
*/
//================================================================================

vector<string> MEDFILEBROWSER::getMeshNames      () const
{
  vector<string> meshNames( _meshes.size() );
  map< string, bool >::const_iterator name_isstruct = _meshes.begin();
  for ( int i=0; name_isstruct != _meshes.end(); ++i, ++name_isstruct )
    meshNames[i] = name_isstruct->first;
  return meshNames;
}

//================================================================================
/*!
  \brief Gets the names of all fields.

  fieldNames is an in/out argument.

  It is an array of string of size the
  number of fields. It must be allocated before calling
  this method. All names are put in it.
*/
//================================================================================

void MEDFILEBROWSER::getFieldNames     ( string * fieldNames ) const
{
  map< string, FIELD_DATA_ >::const_iterator name_data = _fields.begin();
  for( int i = 0; name_data != _fields.end(); ++name_data, ++i )
    fieldNames[i] = name_data->first;
}

//================================================================================
/*!
  \brief Gets the names of all fields.
*/
//================================================================================

vector<string> MEDFILEBROWSER::getFieldNames     () const
{
  vector<string> fieldNames( _fields.size() );
  map< string, FIELD_DATA_ >::const_iterator name_data = _fields.begin();
  for( int i = 0; name_data != _fields.end(); ++name_data, ++i )
    fieldNames[i] = name_data->first;
  return fieldNames;
}

//================================================================================
/*!
 * \brief Return true if mesh \a meshName is a structured one
 */
//================================================================================

bool MEDFILEBROWSER::isStructuredMesh(const string & meshName) const throw (MEDEXCEPTION)
{
  const char* LOC="MEDFILEBROWSER::isStructuredMesh(meshName)";

  map< std::string, bool >::const_iterator name_isstruct = _meshes.find(meshName);
  if ( name_isstruct == _meshes.end() )
    throw MEDEXCEPTION
      ( LOCALIZED( STRING(LOC)<< "There is no known mesh named |"<< meshName
                   << "| in file |" << _fileName <<"|"));

  return name_isstruct->second;
}

//================================================================================
/*!
 * \brief Return type of values of FIELD named \a fieldName 
 */
//================================================================================

med_type_champ MEDFILEBROWSER::getFieldType (const string & fieldName) const throw (MEDEXCEPTION)
{
  const char* LOC="MEDFILEBROWSER::getFieldIteration (fieldName)";

  std::map< std::string, FIELD_DATA_ >::const_iterator name_data = _fields.find(fieldName);

  if ( name_data == _fields.end() )
    throw MEDEXCEPTION
      ( LOCALIZED( STRING(LOC)<< "There is no known field named |"<< fieldName
                   << "| in file |" << _fileName <<"|"));

  return name_data->second._type;
}

//================================================================================
/*!
 * \brief Return name of mesh supporting the FIELD named \a fieldName 
 */
//================================================================================

std::string MEDFILEBROWSER::getMeshName (const string & fieldName) const throw (MEDEXCEPTION)
{
  const char* LOC="MEDFILEBROWSER::getMeshName (fieldName)";

  std::map< std::string, FIELD_DATA_ >::const_iterator name_data = _fields.find(fieldName);

  if ( name_data == _fields.end() )
    throw MEDEXCEPTION
      ( LOCALIZED( STRING(LOC)<< "There is no known field named |"<< fieldName
                   << "| in file |" << _fileName <<"|"));

  return name_data->second._meshName;
}

//================================================================================
/*!
  \brief Returns a vector<DT_IT_> which contain all iteration step for the FIELD 
  identified by its name.

  DT_IT_ definition is 
\verbatim
typedef struct { int dt; int it; } DT_IT_;
\endverbatim

\a dt represents the time iteration number, while \a it represents
the inner iteration number.
*/
//================================================================================

vector<DT_IT_> MEDFILEBROWSER::getFieldIteration (const string & fieldName) const
  throw (MED_EXCEPTION)
{
  const char* LOC="MEDFILEBROWSER::getFieldIteration (fieldName)";

  std::map< std::string, FIELD_DATA_ >::const_iterator name_data = _fields.find(fieldName);

  if ( name_data == _fields.end() )
    throw MEDEXCEPTION
      ( LOCALIZED( STRING(LOC)<< "There is no known field named |"<< fieldName
                   << "| in file |" << _fileName <<"|"));

  return name_data->second._vec_dtit;
}
/*!\if MEDMEM_ug @} \endif */
