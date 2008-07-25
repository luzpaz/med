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
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_MedMeshDriver22.hxx"
#include "MEDMEM_DriversDef.hxx"

#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_Grid.hxx"
#include "MEDMEM_MedVersion.hxx"

using namespace std;
using namespace MED_EN;
using namespace MEDMEM;

namespace med_2_3 {
  extern "C" {
    extern med_idt _MEDdatagroupOuvrir(med_idt pid, char *nom);
    extern med_err _MEDdatagroupFermer(med_idt id);
  }
}

// Every memory allocation made in the MedDriver members function are desallocated in the Mesh destructor

MED_MESH_DRIVER22::MED_MESH_DRIVER22():  _medIdt(MED_INVALID)
{
}

MED_MESH_DRIVER22::MED_MESH_DRIVER22(const string & fileName,
				     MESH * ptrMesh,
				     MED_EN::med_mode_acces accessMode):
  _medIdt(MED_INVALID), MED_MESH_DRIVER(fileName,ptrMesh,accessMode)
{
}

MED_MESH_DRIVER22::MED_MESH_DRIVER22(const MED_MESH_DRIVER22 & driver):
  MED_MESH_DRIVER(driver),_medIdt(driver._medIdt)
{
}

MED_MESH_DRIVER22::~MED_MESH_DRIVER22()
{
}

void MED_MESH_DRIVER22::open()
{
  const char * LOC = "MED_MESH_DRIVER22::open()" ;
  BEGIN_OF(LOC);

  int accessMode = getMedAccessMode( _accessMode, MED_EN::V22 );
  MESSAGE(LOC<<" : _fileName.c_str : "<< _fileName.c_str()<<",mode : "<< accessMode);
  _medIdt = med_2_3::MEDouvrir( (const_cast <char *> (_fileName.c_str())),(med_2_3::med_mode_acces) accessMode);
  MESSAGE(LOC<<" _medIdt : "<< _medIdt );
  if (_medIdt > 0) 
    _status = MED_OPENED; 
  else {
    _medIdt = MED_INVALID;
    _status = MED_CLOSED;
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Could not open file "<<_fileName<<" in mode "<<_accessMode));
  }
  
  END_OF();
}
  
void MED_MESH_DRIVER22::close()
{
  const char * LOC = "MED_MESH_DRIVER22::close() " ;
  BEGIN_OF(LOC);
  int err = 0;
  if ( _status == MED_OPENED) {
    err = med_2_3::MEDfermer(_medIdt);
    // san -- MED5873 : Calling H5close() here leads to failure of SALOMEDS::StudyManager_i::_SaveAs()
    // method during study saving process. MEDfermer() seems sufficient for closing a file.
    //H5close(); // If we call H5close() all the files are closed.
    if (err != 0)
      throw MEDEXCEPTION( LOCALIZED(STRING(LOC)
				    <<" Error when closing file !"
				    )
			  );
    MESSAGE(LOC <<": _medIdt= " << _medIdt );
    MESSAGE(LOC<<": MEDfermer : err    = " << err );
    _status = MED_CLOSED;
    _medIdt = MED_INVALID;
  }
  END_OF();
}

//A FAIRE UTILISER LES MAPS...
const med_2_3::med_geometrie_element  MED_MESH_DRIVER22::all_cell_type[MED_NBR_GEOMETRIE_MAILLE]=
  { med_2_3::MED_POINT1,med_2_3::MED_SEG2,med_2_3::MED_SEG3,med_2_3::MED_TRIA3,med_2_3::MED_QUAD4,med_2_3::MED_TRIA6,med_2_3::MED_QUAD8,
    med_2_3::MED_TETRA4,med_2_3::MED_PYRA5,med_2_3::MED_PENTA6,med_2_3::MED_HEXA8,med_2_3::MED_TETRA10,med_2_3::MED_PYRA13,
    med_2_3::MED_PENTA15, med_2_3::MED_HEXA20};

const char * const MED_MESH_DRIVER22::all_cell_type_tab [MED_NBR_GEOMETRIE_MAILLE]=
  { "MED_POINT1","MED_SEG2","MED_SEG3","MED_TRIA3","MED_QUAD4","MED_TRIA6","MED_QUAD8",
    "MED_TETRA4","MED_PYRA5","MED_PENTA6","MED_HEXA8","MED_TETRA10","MED_PYRA13",
    "MED_PENTA15","MED_HEXA20"};


//---------------------------------- RDONLY PART -------------------------------------------------------------

MED_MESH_RDONLY_DRIVER22::MED_MESH_RDONLY_DRIVER22():_computeFaces(true)
{
  this->GENDRIVER::_accessMode = MED_EN::RDONLY;
}
  
MED_MESH_RDONLY_DRIVER22::MED_MESH_RDONLY_DRIVER22(const string & fileName,
						   MESH * ptrMesh):
  IMED_MESH_RDONLY_DRIVER(fileName,ptrMesh),
  MED_MESH_DRIVER22(fileName,ptrMesh,RDONLY),
  MED_MESH_DRIVER(fileName,ptrMesh,RDONLY),
  _computeFaces(true)
{ 
  MESSAGE("MED_MESH_RDONLY_DRIVER22::MED_MESH_RDONLY_DRIVER22(const string & fileName, MESH * ptrMesh) has been created");
}
  
MED_MESH_RDONLY_DRIVER22::MED_MESH_RDONLY_DRIVER22(const MED_MESH_RDONLY_DRIVER22 & driver): 
   IMED_MESH_RDONLY_DRIVER(driver),
   MED_MESH_DRIVER22(driver),
   MED_MESH_DRIVER(driver),
   _computeFaces(driver._computeFaces)
{
}

MED_MESH_RDONLY_DRIVER22::~MED_MESH_RDONLY_DRIVER22()
{
  //MESSAGE("MED_MESH_RDONLY_DRIVER22::~MED_MESH_RDONLY_DRIVER() has been destroyed");
}

GENDRIVER * MED_MESH_RDONLY_DRIVER22::copy(void) const
{
  return new MED_MESH_RDONLY_DRIVER22(*this);
}

void MED_MESH_RDONLY_DRIVER22::read(void)
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER22::read() : " ;
  BEGIN_OF(LOC);
  if (_status!=MED_OPENED)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "The _idt of file " << _fileName << " is : " << _medIdt <<  " (the file is not opened)." )) ;

  if ( ( _meshName.empty() ) && ( _ptrMesh->_name.empty() )    )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)
				 <<" neither <meshName> is set in driver nor in object MESH.")) ;

  // If _meshName is not set in driver, try to use _ptrMesh->_name
  if ( ( _meshName.empty() ) && ( !_ptrMesh->_name.empty() )    )
    _meshName=_ptrMesh->_name;

  if ( _meshName.size() > MED_TAILLE_NOM )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)
				 <<" <meshName> size in object driver MESH is > MED_TAILLE_NOM ."));


  _ptrMesh->_name =  _meshName;

  SCRUTE(_ptrMesh->getIsAGrid());

  if (_ptrMesh->getIsAGrid())
  {
    getGRID( );
    {
      if (getFAMILY()!=MED_VALID)
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERREUR in getFAMILY when the mesh is a grid")) ;
      buildAllGroups(_ptrMesh->_groupNode,_ptrMesh->_familyNode) ;
    }
    END_OF();
    return;
  }
  else // check that the mesh is really unstructured (PAL14113)
  {
    char                  meshName[MED_TAILLE_NOM+1]="";
    char                  meshDescription[MED_TAILLE_DESC+1]="";
    med_2_3::med_int      meshDim;
    med_2_3::med_maillage meshType;
    int numberOfMeshes = med_2_3::MEDnMaa(_medIdt);
    for (int i=1;i<=numberOfMeshes;i++)
    {
      MEDmaaInfo(_medIdt, i ,meshName, &meshDim, &meshType, meshDescription);
      if (_meshName == string(meshName)) {
        if ( meshType == med_2_3::MED_STRUCTURE ) {
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<
                                       "class GRID must be used for a structured mesh"));
        }
        else {
          break;
        }
      }
    }
  }

  if (getCOORDINATE()!=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERREUR in getCOORDINATE"  )) ;
 
  if (getCONNECTIVITY()!=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERREUR in getCOONECTIVITY")) ;
  
  if (getFAMILY()!=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERREUR in getFAMILY"      )) ;

  if (_computeFaces)
    updateFamily();

  // we build all groups
  // on node
  buildAllGroups(_ptrMesh->_groupNode,_ptrMesh->_familyNode) ;
  // on cell
  buildAllGroups(_ptrMesh->_groupCell,_ptrMesh->_familyCell) ;

  if (_ptrMesh->_meshDimension == 3)
    // on face
    buildAllGroups(_ptrMesh->_groupFace,_ptrMesh->_familyFace) ;
//   else if (_ptrMesh->_meshDimension == 2) -- PAL13414
  if (_ptrMesh->_meshDimension > 1)
    // on edge
    buildAllGroups(_ptrMesh->_groupEdge,_ptrMesh->_familyEdge) ;

//   MESSAGE(LOC<<"Checking of CellModel !!!!!!!");

//   int nbOfTypes =  _ptrMesh->_connectivity->_numberOfTypes;
//    for(int i=0;i<nbOfTypes;i++)
//      {
//        MESSAGE(LOC << _ptrMesh->_connectivity->_type[i]) ;
//      }

  END_OF();
}

//=======================================================================
//function : getGRID
//purpose  : 
//=======================================================================

void MED_MESH_RDONLY_DRIVER22::getGRID()
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER22::getGRID() : " ;
  BEGIN_OF(LOC);
  
  if (_status!=MED_OPENED)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "med file is not opened"));
  
  GRID * ptrGrid = (GRID *) _ptrMesh;
    
  SCRUTE(ptrGrid);

  int err, i;

  int numberOfMeshesInFile = med_2_3::MEDnMaa(_medIdt);

  if (numberOfMeshesInFile == MED_INVALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Problem in File where the mesh " << _meshName << " is supposed to be stored"));

  for (int index = 0; index < numberOfMeshesInFile; index++)
    {
      char meshName[MED_TAILLE_NOM+1]="";
      char meshDescription[MED_TAILLE_DESC+1]="";
//CCRT      int meshDim;
      med_2_3::med_int meshDim;
      med_2_3::med_maillage meshType;

      err = med_2_3::MEDmaaInfo(_medIdt, (index+1),meshName, &meshDim,
			       &meshType, meshDescription) ;

      MESSAGE(LOC<<": Mesh n°"<< (index+1) <<" nammed "<< meshName << " with the description " << meshDescription << " is structured");

      if (_meshName == string(meshName))
	{
	  _ptrMesh->_description = meshDescription;
	  _ptrMesh->_name = meshName;
	}
    }

  MED_EN::med_grid_type gridType = ptrGrid->getGridType();
  if ( ptrGrid->_is_default_gridType )
  {
    med_2_3::med_type_grille type;
    err = med_2_3::MEDnatureGrilleLire(_medIdt,
                                       const_cast <char *>(_meshName.c_str()),
                                       &type);
    if (err != MED_VALID)
      throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << ": can't get the nature of the grid which is the mesh n°" << i << " of the file |" << _fileName << "| !"));

    gridType = ptrGrid->_gridType = (MED_EN::med_grid_type) type;
    ptrGrid->_is_default_gridType = false;
  }

  MESSAGE(LOC<<": Mesh processed is nammed "<< _ptrMesh->_name << " with the description " << _ptrMesh->_description << " is structured with the type " << gridType);

  // Read the dimension of the mesh <_meshName>
  int MeshDimension = med_2_3::MEDdimLire(_medIdt, const_cast <char *>
					 (_meshName.c_str())) ;

  if (MeshDimension == MED_INVALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "The mesh dimension |" <<
				 MeshDimension << "| seems to be incorrect " <<
				 "for the mesh : |" << _meshName << "|")) ;

  _ptrMesh->_meshDimension = MeshDimension;

  // Read or get the dimension of the space for the mesh <_meshName>
  int SpaceDimension = MeshDimension;

  int SpaceDimensionRead = med_2_3::MEDdimEspaceLire(_medIdt,
						    const_cast <char *>
						    (_meshName.c_str())) ;

  if (SpaceDimensionRead != MED_INVALID) SpaceDimension = SpaceDimensionRead;

  _ptrMesh->_spaceDimension = SpaceDimension;

  med_2_3::med_repere rep ;
  string tmp_nom_coord (MED_TAILLE_PNOM22*(_ptrMesh->_spaceDimension)+1,' ');
  string tmp_unit_coord(MED_TAILLE_PNOM22*(_ptrMesh->_spaceDimension)+1,' ');
  char * tmp_nom = (const_cast <char *> ( tmp_nom_coord.c_str())  ) ;
  char * tmp_unit= (const_cast <char *> ( tmp_unit_coord.c_str()) ) ;

  // Read Array length
  int * ArrayLen[] = { & ptrGrid->_iArrayLength,
                       & ptrGrid->_jArrayLength,
                       & ptrGrid->_kArrayLength  };

  MESSAGE(LOC << "The mesh is a " << _ptrMesh->_meshDimension << "D mesh on a " << _ptrMesh->_spaceDimension << "D space");

  int NumberOfNodes;

  //  if (gridType == MED_EN::MED_GRILLE_STANDARD)
  if (gridType == MED_EN::MED_BODY_FITTED)
    {
//CCRT      int * structure = new int[MeshDimension];
      med_2_3::med_int * structure = new med_2_3::med_int[MeshDimension];

      err = med_2_3::MEDstructureCoordLire(_medIdt,
					  const_cast <char *>
					  (_ptrMesh->_name.c_str()),
					  MeshDimension,structure);

      if (err != MED_VALID)
	throw MEDEXCEPTION(STRING(LOC) <<"Error in reading the structure of grid : |" << _meshName << "|" ) ;
      
      NumberOfNodes = 1;

      for (int idim = 0; idim < MeshDimension; idim++)
	{
	  MESSAGE(LOC<<"structure dim " << idim << " " << structure[idim]);

	  ArrayLen [idim][0] = structure[idim];
	  NumberOfNodes = NumberOfNodes*structure[idim];
	}
      
      delete [] structure;

//       NumberOfNodes = MED_FR::MEDnEntMaa(_medIdt,
// 					 const_cast <char *>
// 					 (_ptrMesh->_name.c_str()),
// 					 MED_FR::MED_COOR,MED_FR::MED_NOEUD,
// 					 (MED_FR::med_geometrie_element) 0,
// 					 (MED_FR::med_connectivite) 0);

      if ( NumberOfNodes == MED_INVALID )
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"The number of nodes |" <<
				     NumberOfNodes <<
				     "| seems to be incorrect "
				     << "for the mesh : |" <<
				     _meshName << "|" )) ;

      _ptrMesh->_numberOfNodes = NumberOfNodes ;

      // create coordinates and its structure
      _ptrMesh->_coordinate = new COORDINATE(SpaceDimension,NumberOfNodes,
					     MED_EN::MED_FULL_INTERLACE);

      err = MEDcoordLire(_medIdt,
			 const_cast <char *> (_ptrMesh->_name.c_str()),
			 _ptrMesh->_spaceDimension,
			 //const_cast <double *> ( _ptrMesh->_coordinate->_coordinate->get(MED_EN::MED_FULL_INTERLACE) ),
			 const_cast <double *> ( _ptrMesh->_coordinate->_coordinate.get(MED_EN::MED_FULL_INTERLACE) ),
			 med_2_3::MED_FULL_INTERLACE,
			 MED_ALL, // we read all the coordinates
			 NULL,    // we don't use a profile
			 0,       // so the profile's size is 0
			 &rep,tmp_nom,tmp_unit);

      if (err != MED_VALID)
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't read coordinates of the |" <<
				     NumberOfNodes << "| nodes for the mesh : |" <<
				     _meshName << "| of space dimension |" <<
				     SpaceDimension << "| with units names |" <<
				     tmp_nom << "| and units |" <<
				     tmp_unit << " |")) ;

      ptrGrid->_is_coordinates_filled = true;
    }
  else if ((gridType == MED_EN::MED_CARTESIAN) ||
	   (gridType == MED_EN::MED_POLAR))
    {
      NumberOfNodes = 1;

      double * Array[] = { (double*) 0, (double*) 0, (double*) 0 };

      for (int idim = 0; idim < _ptrMesh->_meshDimension; ++idim)
	{
	  med_2_3::med_table table;
	  if (idim == 0) table = med_2_3::MED_COOR_IND1;
	  else if (idim == 1) table = med_2_3::MED_COOR_IND2;
	  else if (idim == 2) table = med_2_3::MED_COOR_IND3;

	  int length = med_2_3::MEDnEntMaa(_medIdt,
					  const_cast <char *> (_ptrMesh->_name.c_str()),
					  table,med_2_3::MED_NOEUD,
					  med_2_3::MED_NONE,
					  med_2_3::MED_NOD);
	  if ( length <= MED_VALID )
	    throw MEDEXCEPTION(STRING(LOC) <<"The number of nodes |" << length <<
			       "| seems to be incorrect "
			       << "for the mesh : |" << _meshName << "|" ) ;
	  
	  ArrayLen [idim][0] = length;
	  NumberOfNodes *= length;

	  Array [idim] = new double [ length ];

	  err = med_2_3::MEDindicesCoordLire(_medIdt, const_cast <char *>
					    (_ptrMesh->_name.c_str()),
					    _ptrMesh->_meshDimension,
					    Array [idim], length, (idim+1),
					    tmp_nom+(idim*MED_TAILLE_PNOM22),
					    tmp_unit+(idim*MED_TAILLE_PNOM22));

	  if (err != MED_VALID)
	    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Error in reading coordinates indices " <<
					 idim << "of the grid : |" <<
					 _meshName << "|" )) ;
	}

      ptrGrid->_iArray = Array[0];
      ptrGrid->_jArray = Array[1];
      ptrGrid->_kArray = Array[2];

      _ptrMesh->_numberOfNodes = NumberOfNodes ;
    
      // create coordinates
      _ptrMesh->_coordinate = new COORDINATE(SpaceDimension,NumberOfNodes,
					     MED_EN::MED_FULL_INTERLACE);

      if (gridType == MED_EN::MED_CARTESIAN)
	rep = med_2_3::MED_CART;
      else if (gridType == MED_EN::MED_POLAR)
	{
	  if (SpaceDimension == 2) rep = med_2_3::MED_CYL;
	  else if (SpaceDimension == 3) rep = med_2_3::MED_SPHER;
	}
    }
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<" bad grid type : " << gridType));

  // set coordinate names

  for (i=0; i<_ptrMesh->_spaceDimension; ++i ) {
    string myStringName(tmp_nom_coord,i*MED_TAILLE_PNOM22,MED_TAILLE_PNOM22) ;
    string myStringUnit(tmp_unit_coord,i*MED_TAILLE_PNOM22,MED_TAILLE_PNOM22) ;
    // suppress space at the end
    int j ;
    for(j=MED_TAILLE_PNOM22-1;j>=0;j--)
      if (myStringName[j] != ' ') break ;
    _ptrMesh->_coordinate->_coordinateName[i]=string(myStringName,0,j+1);
    for(j=MED_TAILLE_PNOM22-1;j>=0;j--)
      if (myStringUnit[j] != ' ') break ;
    _ptrMesh->_coordinate->_coordinateUnit[i]=string(myStringUnit,0,j+1);
  }

  string coordinateSystem = "UNDEFINED";

  if( rep == med_2_3::MED_CART) coordinateSystem = "CARTESIAN";
  else if ( rep == med_2_3::MED_CYL) coordinateSystem = "CYLINDRICAL";
  else if ( rep == med_2_3::MED_SPHER) coordinateSystem = "SPHERICAL";

  _ptrMesh->_coordinate->setCoordinatesSystem(coordinateSystem);

  END_OF();
}

//=======================================================================
//function : getCOORDINATE
// A FAIRE : RENVOYER DU VOID
//=======================================================================
int  MED_MESH_RDONLY_DRIVER22::getCOORDINATE()
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER22::getCOORDINATE() : " ;

  BEGIN_OF(LOC);

  if (_status==MED_OPENED)
    {
      int err ;
      
  int numberOfMeshesInFile = med_2_3::MEDnMaa(_medIdt);

  if (numberOfMeshesInFile == MED_INVALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Problem in File where the mesh " << _meshName << " is supposed to be stored"));

  for (int index = 0; index < numberOfMeshesInFile; index++)
    {
      char meshName[MED_TAILLE_NOM+1]="";
      char meshDescription[MED_TAILLE_DESC+1]="";
//CCRT      int meshDim;
      med_2_3::med_int meshDim;
      med_2_3::med_maillage meshType;

      err = med_2_3::MEDmaaInfo(_medIdt, (index+1), meshName, &meshDim,
			       &meshType, meshDescription) ;

      MESSAGE(LOC<<": Mesh n°"<< (index+1) <<" nammed "<< meshName << " with the description " << meshDescription << " is structured");

      if (_meshName == string(meshName))
	{
	  _ptrMesh->_description = meshDescription;
	}
    }


      // Read the dimension of the mesh <_meshName>
      int MeshDimension = med_2_3::MEDdimLire(_medIdt, const_cast <char *>
					     (_meshName.c_str())) ;

      if ( MeshDimension == MED_INVALID ) 
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "The mesh dimension |" <<
				     MeshDimension <<
				     "| seems to be incorrect " <<
				     "for the mesh : |" << _meshName << "|")) ;

      _ptrMesh->_meshDimension = MeshDimension;

      // Read or get the dimension of the space for the mesh <_meshName>
      int SpaceDimension = MeshDimension;

      int SpaceDimensionRead = med_2_3::MEDdimEspaceLire(_medIdt,
							const_cast <char *>
							(_meshName.c_str())) ;

      if (SpaceDimensionRead  != MED_INVALID)
	SpaceDimension = SpaceDimensionRead;

      _ptrMesh->_spaceDimension = SpaceDimension;

      // Read the number of nodes used in the mesh <_meshName>
      // to be able to create a COORDINATE object
      int NumberOfNodes=MEDnEntMaa(_medIdt,
                                   const_cast <char *> (_meshName.c_str()),
                                   med_2_3::MED_COOR,
                                   med_2_3::MED_NOEUD,
                                   (med_2_3::med_geometrie_element) MED_NONE,
                                   (med_2_3::med_connectivite)      MED_NONE);
      if ( NumberOfNodes <= MED_VALID )
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"The number of nodes |" << NumberOfNodes << "| seems to be incorrect "
                                     << "for the mesh : |" << _meshName << "|" )) ;
      _ptrMesh->_numberOfNodes = NumberOfNodes ;

      // create a COORDINATE object
      if (_ptrMesh->_coordinate)
        delete _ptrMesh->_coordinate;
      _ptrMesh->_coordinate = new COORDINATE(SpaceDimension, NumberOfNodes, MED_EN::MED_FULL_INTERLACE);
      
      med_2_3::med_repere rep ; // ATTENTION ---> DOIT ETRE INTEGRE DS MESH EF: FAIT NON?
      string tmp_nom_coord (MED_TAILLE_PNOM22*(_ptrMesh->_spaceDimension)+1,'\0');
      string tmp_unit_coord(MED_TAILLE_PNOM22*(_ptrMesh->_spaceDimension)+1,'\0');
      char * tmp_nom = (const_cast <char *> ( tmp_nom_coord.c_str())  ) ;
      char * tmp_unit= (const_cast <char *> ( tmp_unit_coord.c_str()) ) ;

      err=MEDcoordLire(_medIdt,
                       const_cast <char *> (_ptrMesh->_name.c_str()),
		       _ptrMesh->_spaceDimension,
		       //const_cast <double *> ( _ptrMesh->_coordinate->_coordinate->get(MED_EN::MED_FULL_INTERLACE) ),
		       const_cast <double *> ( _ptrMesh->_coordinate->_coordinate.get(MED_EN::MED_FULL_INTERLACE) ),
                       med_2_3::MED_FULL_INTERLACE,
                       MED_ALL,                      // we read all the coordinates
                       NULL,                         // we don't use a profile
                       0,                            // so the profile's size is 0
                       &rep,tmp_nom,tmp_unit);
      if (err != MED_VALID)
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't read coordinates of the |" << NumberOfNodes << "| nodes "
                                     << "for the mesh : |" << _meshName 
                                     << "| of space dimension |" << SpaceDimension 
                                     << "| with units names |"   << tmp_nom
                                     << "| and units |"          << tmp_unit
                                     << " |")) ;
      

      for (int i=0;i<_ptrMesh->_spaceDimension;i++) {
	string myStringName(tmp_nom_coord,i*MED_TAILLE_PNOM22,MED_TAILLE_PNOM22) ;
	string myStringUnit(tmp_unit_coord,i*MED_TAILLE_PNOM22,MED_TAILLE_PNOM22) ;
	// suppress space at the end
	int j ;
	for(j=MED_TAILLE_PNOM22-1;j>=0;j--)
	  if (myStringName[j] != ' ') break ;
	_ptrMesh->_coordinate->_coordinateName[i]=string(myStringName,0,j+1);
	for(j=MED_TAILLE_PNOM22-1;j>=0;j--)
	  if (myStringUnit[j] != ' ') break ;
	_ptrMesh->_coordinate->_coordinateUnit[i]=string(myStringUnit,0,j+1);
      }

      // Pourquoi le stocker sous forme de chaîne ?
      switch (rep)
	{
	case med_2_3::MED_CART : 
	  {
	    _ptrMesh->_coordinate->_coordinateSystem = "CARTESIAN";
	    break ;
	  }
	case med_2_3::MED_CYL :
	  {
	    _ptrMesh->_coordinate->_coordinateSystem = "CYLINDRICAL";
	    break ;
	  }
	case med_2_3::MED_SPHER :
	  {
	    _ptrMesh->_coordinate->_coordinateSystem = "SPHERICAL";
	    break ;
	  }
	default :
	  {
	    _ptrMesh->_coordinate->_coordinateSystem = "UNDEFINED"; // ?Erreur ?
	    break ;
	  }
	}

      // Read the unused optional node Names
      char * tmp_node_name = new char[NumberOfNodes*MED_TAILLE_PNOM22+1];
      tmp_node_name[NumberOfNodes]='\0' ;
      err=MEDnomLire(_medIdt,const_cast <char*> (_ptrMesh->_name.c_str()),
		     tmp_node_name,NumberOfNodes*MED_TAILLE_PNOM22,med_2_3::MED_NOEUD,
		     (med_2_3::med_geometrie_element) MED_NONE);
      if (err == MED_VALID) 
        MESSAGE(LOC<<"MED_MESH_RDONLY_DRIVER::getNoeuds() : WARNING : Nodes have names but we do not read them !");
      delete[] tmp_node_name ;


      // ??? Read the unused optional node Numbers ???
      med_2_3::med_int * tmp_node_number = new med_2_3::med_int[NumberOfNodes] ;
      err=MEDnumLire(_medIdt,const_cast <char*> (_ptrMesh->_name.c_str()),
		     tmp_node_number,NumberOfNodes,med_2_3::MED_NOEUD,(med_2_3::med_geometrie_element)0);
      if (err == MED_VALID) {
        // INFOS(LOC<<"WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING");
        // INFOS(LOC<<"MED_MESH_RDONLY_DRIVER::getNoeuds() : WARNING : Nodes have numbers but we do not take care of them !");
        // INFOS(LOC<<"WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING");
	MESSAGE(LOC<<"MED_MESH_RDONLY_DRIVER::getNoeuds() : Nodes have numbers, we DO TAKE care of them !");
	_ptrMesh->_coordinate->_nodeNumber.set(NumberOfNodes) ; 
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
	for(med_2_3::med_int i2=0;i2<NumberOfNodes;i2++)
	  _ptrMesh->_coordinate->_nodeNumber[i2]=(int)(tmp_node_number[i2]);
#else
	memcpy((int*)_ptrMesh->_coordinate->_nodeNumber,tmp_node_number,sizeof(int)*NumberOfNodes) ;
#endif
	
	//////////////////////////////////////////////////////////////////////////////////////
  	///  Modification pour prise en compte de la numérotation optionnelle des noeuds   ///
  	//////////////////////////////////////////////////////////////////////////////////////
  	///
	/// Calcule _optionnalToCanonicNodesNumbers de telle sorte que _optionnalToCanonicNodesNumbers[OptionnalNumber]==CanonicNumber
	
// 	_ptrMesh->_arePresentOptionnalNodesNumbers=1;
//   	for (int canonicNumber=1;canonicNumber<=NumberOfNodes;canonicNumber++) _ptrMesh->_optionnalToCanonicNodesNumbers[tmp_node_number[canonicNumber-1]]=canonicNumber;
// ICI RETOUR A LA NORMALE::: AUCUNE PRISE EN COMPTE D'UN NUMEROTATION OPTIONNEL
	_ptrMesh->_arePresentOptionnalNodesNumbers=0;
      } 
      else _ptrMesh->_arePresentOptionnalNodesNumbers=0;

  	//////////////////////////////////////////////////////////////////////////////////////

      delete[] tmp_node_number ;
      
      END_OF();
      return MED_VALID;
    }
  return MED_ERROR;
}


int MED_MESH_RDONLY_DRIVER22::getCONNECTIVITY() 
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER22::getCONNECTIVITY : " ;
  BEGIN_OF(LOC);

  if (_status==MED_OPENED)
    {

      int err = 0 ;
      // read MED_CELL connectivity
      CONNECTIVITY * Connectivity     = new CONNECTIVITY(MED_CELL) ;
      Connectivity->_numberOfNodes    = _ptrMesh->_numberOfNodes ;   // EF : Pourquoi cet attribut est-il dans MESH et non dans COORDINATE ?

      // Try to read nodal connectivity of the cells <Connectivity->_nodal>
      // then try to read descending connectivity    <Connectivity->_descending>
      // if neither nodal nor descending connectivity exists
      // throw an exception.
      err = getNodalConnectivity(Connectivity) ;
      if (err!=MED_VALID)
	{
	  Connectivity->_typeConnectivity = MED_DESCENDING ;
	  err = getDescendingConnectivity(Connectivity) ;
	}
      else 
	getDescendingConnectivity(Connectivity) ; // we read it if there is one
      
      if (err!=MED_VALID)
	{
	  delete Connectivity ;
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "We could not read " <<
				       "any Connectivity")) ;
	}

      if (Connectivity->_entityDimension != _ptrMesh->_meshDimension)
	MESSAGE(LOC << "Small mesh dimension problem on the med file mounted in memory : diim stored " << _ptrMesh->_meshDimension << " dim computed using the connectivity " << Connectivity->_entityDimension);

      _ptrMesh->_meshDimension = Connectivity->_entityDimension ; 

      // At this point Connectivity->_typeConnectivity is either NODAL or DESCENDING
      // If both connectivities are found Connectivity->_typeConnectivity is NODAL
      // If space dimension is 3 
      // try to read the nodal connectivity of the faces <ConnectivityFace->_nodal> then
      // try to read the descending connectivity <ConnectivityFace->_descending>
      // if there is no descending connectivity and the CELLS are
      // defined in descending mode then throw an exception

      // PROVISOIRE : if we have some face or edge in MED_MAILLE, we don't read more. There could not be have face or edge !!!!

      if(Connectivity->_constituent==NULL) {

      SCRUTE(_ptrMesh->_meshDimension);
      SCRUTE(Connectivity->_entityDimension);
      if (_ptrMesh->_meshDimension == 3) {
        MESSAGE(LOC<<" ESSAI DE LECTURE DE LA CONNECTIVITE DES FACES..." );
        CONNECTIVITY * ConnectivityFace = new CONNECTIVITY(MED_EN::MED_FACE) ;
	ConnectivityFace->_typeConnectivity = Connectivity->_typeConnectivity ; // NODAL or DESCENDING
	SCRUTE(ConnectivityFace->_typeConnectivity);
	if (Connectivity->_typeConnectivity == MED_DESCENDING) {
          MESSAGE(LOC<<" ESSAI DE LECTURE DE LA CONNECTIVITE DESCENDANTE DES FACES" );
	  err = getDescendingConnectivity(ConnectivityFace) ;
	  if (err!=MED_VALID)
	    throw MEDEXCEPTION ( LOCALIZED(STRING(LOC) << "No FACE in descending connectivity")) ;
	  getNodalConnectivity(ConnectivityFace) ; // if any !
	} else {
          MESSAGE(LOC<<" ESSAI DE LECTURE DE LA CONNECTIVITE NODALE DES FACES" );
	  err = getNodalConnectivity(ConnectivityFace) ;
	  if (err!=MED_VALID) { // or error ????? we are in NODAL mode.
	    err = getDescendingConnectivity(ConnectivityFace) ;
	  } else
	    getDescendingConnectivity(ConnectivityFace); // if any !
	}
        if (err!=MED_VALID) {
	  delete ConnectivityFace ;
	  MESSAGE(LOC<<"No FACE defined.") ;
	} else {
          MESSAGE(LOC<<" SAUVEGARDE DE LA CONNECTIVITE DES FACES DANS L'OBJET CONNECTIVITY" );
          delete Connectivity->_constituent;
	  Connectivity->_constituent=ConnectivityFace;
        }
      }

      // read MED_EDGE connectivity
      if (_ptrMesh->_meshDimension > 1) { // we are in 3 or 2D 
        MESSAGE(LOC<<" ESSAI DE LECTURE DE LA CONNECTIVITE DES ARRETES...." );
	CONNECTIVITY * ConnectivityEdge = new CONNECTIVITY(MED_EDGE) ;
	ConnectivityEdge->_typeConnectivity = Connectivity->_typeConnectivity ;
	if (Connectivity->_typeConnectivity == MED_DESCENDING) {
          MESSAGE(LOC<<" ESSAI DE LECTURE DE LA CONNECTIVITE DESCENDANTE DES ARRETES" );
	  err = getDescendingConnectivity(ConnectivityEdge) ;
	  if (err!=MED_VALID)
	    throw MEDEXCEPTION ( LOCALIZED(STRING(LOC) << "No EDGE in descending connectivity")) ;
	  getNodalConnectivity(ConnectivityEdge) ; // if any !
	} else {
          MESSAGE(LOC<<" ESSAI DE LECTURE DE LA CONNECTIVITE NODALE DES ARRETES" );
	  err = getNodalConnectivity(ConnectivityEdge) ;
	  if (err!=MED_VALID) { // or error ????? we are in NODAL mode.
	    err = getDescendingConnectivity(ConnectivityEdge) ;
	  } else
	    getDescendingConnectivity(ConnectivityEdge) ; // if any !
	}
	if (err!=MED_VALID) {
	  delete ConnectivityEdge ;
	  MESSAGE(LOC<<"No EDGE defined.") ;
	} else {
	  if (_ptrMesh->_meshDimension == 3)
	    if (Connectivity->_constituent != NULL)
	      Connectivity->_constituent->_constituent=ConnectivityEdge ;
	    else
	      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<< "EDGE defined but there are no FACE !")) ;
	  else { // IN 2D
            MESSAGE(LOC<<" SAUVEGARDE DE LA CONNECTIVITE DES ARETES DANS L'OBJET CONNECTIVITY" );
	    Connectivity->_constituent=ConnectivityEdge ;
          }
        }
      }
      }
      if (_ptrMesh->_connectivity)
        delete _ptrMesh->_connectivity;
      _ptrMesh->_connectivity  = Connectivity ;                                      

      // all right !

      // we have read all connectivity in file, now we must build descending connectivity if necessary !

      // If connectivity descending is defined, we have nothing to do, all constituent are defined !
      // If connectivity is only nodal, we must rebuild descending if we have some contituent !

      //A FAIRE !!!!
//        if (Connectivity->_descending == NULL)
//  	if (Connectivity->_constituent != NULL){
//  	  // update Connectivity->_constituent
//  	  CONNECTIVITY * myConstituentOld = Connectivity->_constituent ;
//  	  Connectivity->_constituent = (CONNECTIVITY *)NULL ;
//  	  Connectivity->calculateDescendingConnectivity() ;
	  
//  	}
      
      END_OF();
      return MED_VALID;
    }
  return MED_ERROR;
}

int MED_MESH_RDONLY_DRIVER22::getNodalConnectivity(CONNECTIVITY * Connectivity)
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER22::getNodalConnectivity : " ;
  BEGIN_OF(LOC);

  if (_status==MED_OPENED)
    {
      int spaceDimension = _ptrMesh->_spaceDimension;

      // Get the type of entity to work on (previously set in the Connectivity Object)
      med_2_3::med_entite_maillage Entity = (med_2_3::med_entite_maillage) Connectivity->getEntity();
      
      // Get the number of cells of each type & store it in <tmp_cells_count>.
      int * tmp_cells_count = new int[MED_NBR_GEOMETRIE_MAILLE] ;
      int i;
      for (i=1;i<MED_NBR_GEOMETRIE_MAILLE;i++) 
      {                       // EF :ON SCANNE DES GEOMETRIES INUTILES, UTILISER LES MAPS
        tmp_cells_count[i]=MEDnEntMaa(_medIdt,(const_cast <char *> (_ptrMesh->_name.c_str())),
                                      med_2_3::MED_CONN,(med_2_3::med_entite_maillage) Entity,
                                      all_cell_type[i],med_2_3::MED_NOD); 


        // Get the greatest dimension of the cells : Connectivity->_entityDimension
        // We suppose there is no cells used as faces in MED 2.2.x , this is forbidden !!!
        // In version prior to 2.2.x, it is possible
        if (tmp_cells_count[i]>0) 
        {
          Connectivity->_entityDimension=all_cell_type[i]/100;
          Connectivity->_numberOfTypes++;
        }
      }

      {
        // VBD fix to be able reading polygons and polyhedrons
        // "in the case when the space dimension is 3 and the mesh dimension 2"

        //checking for the number of polygons/polyhedra to get an appropriate mesh dimension
        int nbpolygons=MEDnEntMaa(_medIdt,(const_cast <char *> (_ptrMesh->_name.c_str())),
                                  med_2_3::MED_CONN,(med_2_3::med_entite_maillage) Entity,
                                  med_2_3::MED_POLYGONE,med_2_3::MED_NOD);
        if (nbpolygons>0)
        {
          if (Connectivity->_entityDimension<2)
            Connectivity->_entityDimension=2;
        }
        int nbpolyhedra=MEDnEntMaa(_medIdt,(const_cast <char *> (_ptrMesh->_name.c_str())),
                                   med_2_3::MED_CONN,(med_2_3::med_entite_maillage) Entity,
                                   med_2_3::MED_POLYEDRE,med_2_3::MED_NOD);
        if (nbpolyhedra>0)
          Connectivity->_entityDimension=3;

        //setting a correct mesh dimension as being the dimnsion corresponding to
        // the highest dimension  element
        if (Entity==med_2_3::MED_MAILLE)
          _ptrMesh->_meshDimension = Connectivity->_entityDimension;
      }

      // begin classic geometric types
      if (Connectivity->_numberOfTypes > 0)
	{
	  // if MED version < 2.2.x, we read only entity with dimention = Connectivity->_entityDimension. Lesser dimension are face or edge !

          med_2_3::med_int major, minor, release;

          if ( med_2_3::MEDversionLire(_medIdt, &major, &minor, &release) != 0 )
	    {
	      // error : we suppose we have not a good med file !
	      delete[] tmp_cells_count ;
	      return MED_ERROR ;
	    }

	  // we get MED version number
	  // If MED version is < 2.2 then the cells which dimension
	  // is lesser than the main dimension ( Connectivity->_entityDimension )
	  // are either faces or edges

	  //        string medVersion(version_med);
	  //        int firstNumber = 
	  int * tmpEdgeCount = new int[MED_NBR_GEOMETRIE_MAILLE] ;
	  tmpEdgeCount[0]     = 0 ;
	  int numberOfEdgesTypes = 0;
	  int * tmpFaceCount = new int[MED_NBR_GEOMETRIE_MAILLE] ;
	  tmpFaceCount[0]     = 0 ;
	  int numberOfFacesTypes = 0;
  
//       if ((version_med != "2.2")&(Entity==MED_FR::MED_MAILLE))
// 	{
// 	  Connectivity->_numberOfTypes=0;
	
// 	  for ( i=1;i<MED_NBR_GEOMETRIE_MAILLE;i++)
// 	    {
// 	      tmpFaceCount[i]=0;
// 	      tmpEdgeCount[i]=0;
// 	      if (tmp_cells_count[i]!=0)
// 		{
// 		  int dimension = all_cell_type[i]/100 ;
// 		  if (Connectivity->_entityDimension==dimension) 
// 		    Connectivity->_numberOfTypes++ ;
	    
// 		  if (dimension == 2)
// 		    if (Connectivity->_entityDimension==3)
// 		      {
// 			tmpFaceCount[i]=tmp_cells_count[i] ;
// 			tmp_cells_count[i]=0 ;
// 			numberOfFacesTypes++;
// 		      }
// 		  if (dimension == 1)
// 		    if (Connectivity->_entityDimension>dimension)
// 		      {
// 			tmpEdgeCount[i]=tmp_cells_count[i] ;
// 			tmp_cells_count[i]=0;
// 			numberOfEdgesTypes++ ;
// 		      }
// 		}
// 	    }
// 	}
	
      if (Entity==med_2_3::MED_MAILLE)
	{
	      Connectivity->_numberOfTypes=0;
	
	      for ( i=1;i<MED_NBR_GEOMETRIE_MAILLE;i++) 
		{
		  tmpFaceCount[i]=0;
		  tmpEdgeCount[i]=0;
		  if (tmp_cells_count[i]!=0) 
		    {
		      int dimension = all_cell_type[i]/100 ;
		      if (Connectivity->_entityDimension==dimension) 
			Connectivity->_numberOfTypes++ ;
		
		      if (dimension == 2)
			if (Connectivity->_entityDimension==3) 
			  {
			    tmpFaceCount[i]=tmp_cells_count[i] ;
			//tmp_cells_count[i]=0 ;
			//Connectivity->_numberOfTypes++ ;
			    numberOfFacesTypes++;
			  }
		      if (dimension == 1)
			if (Connectivity->_entityDimension>dimension) 
			  {
			    tmpEdgeCount[i]=tmp_cells_count[i] ;
			//tmp_cells_count[i]=0;
			//Connectivity->_numberOfTypes++ ;
			    numberOfEdgesTypes++ ;
			  }
		    }
		}
	    }

	  // bloc to read CELL :
	  {
	    // Prepare an array of indexes on the different cell types to create a MEDSKYLINEARRAY
	    // We use <tmp_cells_count> to calculate <Connectivity->_count> then we release it
	    Connectivity->_geometricTypes = new MED_EN::medGeometryElement [Connectivity->_numberOfTypes]   ;  // Double emploi pour des raisons pratiques 
	    Connectivity->_type           = new CELLMODEL                  [Connectivity->_numberOfTypes]   ;  //
	    Connectivity->_count          = new int                        [Connectivity->_numberOfTypes+1] ;
	    Connectivity->_count[0]       = 1;
	    
	    int size = 0 ; 
	    int typeNumber=1 ;
	    int i;
	    for ( i=1;i<MED_NBR_GEOMETRIE_MAILLE;i++)  
	      { // no point1 cell type (?)
	    int dimension = all_cell_type[i]/100 ;
	    if ((tmp_cells_count[i]>0) && (Connectivity->_entityDimension == dimension))
		  {
		    Connectivity->_count[typeNumber]=Connectivity->_count[typeNumber-1]+tmp_cells_count[i];

		    CELLMODEL t( (MED_EN::medGeometryElement) MED_MESH_DRIVER22::all_cell_type[i]) ;

		Connectivity->_type[typeNumber-1] = t ;
	    
		    Connectivity->_geometricTypes[typeNumber-1]=( MED_EN::medGeometryElement) MED_MESH_DRIVER22::all_cell_type[i] ;
	    
		    // probleme avec les mailles de dimension < a dimension du maillage :
		    // Il faut oter le zero a la lecture est le remettre a l'ecriture : ce n'est pas fait !!!!! On interdit ce cas pour l'instant !!!

	      
		    size+=tmp_cells_count[i]*((MED_MESH_DRIVER22::all_cell_type[i])%100) ;
	    
		    MESSAGE(LOC
			    << Connectivity->_count[typeNumber]-1 << " cells of type " 
			    << all_cell_type_tab[i] ); 
		    typeNumber++;
		  }
	      }
	
	    // Creation of the MEDSKYLINEARRAY
	    //Connectivity->_nodal = new MEDSKYLINEARRAY(Connectivity->_count[Connectivity->_numberOfTypes]-1,size) ; 
	    //int * NodalIndex = Connectivity->_nodal->getIndex() ;
	    int * NodalValue = new int[size] ;
	    int * NodalIndex = new int[Connectivity->_count[Connectivity->_numberOfTypes]] ;
	    NodalIndex[0]=1 ;
	
	    // Fill the MEDSKYLINEARRAY by reading the MED file.
	    int j=0;
	    for ( i=0;i<Connectivity->_numberOfTypes;i++) 
	      {
		int multi = 0 ;
		med_2_3::med_geometrie_element med_type = (med_2_3::med_geometrie_element) Connectivity->_type[i].getType() ;
		//  	  if ( Connectivity->_type[i].getDimension() < Connectivity->_entityDimension) 
		// 	  if (Connectivity->_entity == MED_CELL)
		// 	    if ( Connectivity->_type[i].getDimension() < _ptrMesh->_spaceDimension) 
		// 	      multi=1;
	  
		//	  int NumberOfCell = Connectivity->_count[i+1]-Connectivity->_count[i] ;
		int NumberOfNodeByCell = Connectivity->_type[i].getNumberOfNodes() ;
	  
		// initialise index
		for ( j=Connectivity->_count[i]; j<Connectivity->_count[i+1];j++)
		  NodalIndex[j]=NodalIndex[j-1]+NumberOfNodeByCell ; 

	    int tmp_numberOfCells = Connectivity->_count[i+1]-Connectivity->_count[i] ;
//CCRT		int * tmp_ConnectivityArray = new int[(NumberOfNodeByCell+multi)*tmp_numberOfCells];
		med_2_3::med_int * tmp_ConnectivityArray = new med_2_3::med_int[(NumberOfNodeByCell+multi)*tmp_numberOfCells];
	  
		//  	  int err=MEDconnLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
	    //                    Connectivity->_entityDimension,tmp_ConnectivityArray,
		//  			      MED_FR::MED_FULL_INTERLACE,NULL,0,Entity,med_type,MED_FR::MED_NOD);
	    int err=MEDconnLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
				spaceDimension,tmp_ConnectivityArray,
				    med_2_3::MED_FULL_INTERLACE,NULL,0,Entity,med_type,med_2_3::MED_NOD);

	    if ( err != MED_VALID)
	      {
		delete[] tmp_ConnectivityArray;
		delete[] tmp_cells_count;
		delete[] tmpFaceCount;
		delete[] tmpEdgeCount;
		MESSAGE(LOC<<": MEDconnLire returns "<<err) ;
		return MED_ERROR ;
	      }

	    int * ConnectivityArray = NodalValue + NodalIndex[Connectivity->_count[i]-1]-1 ;

	    // version originale sans prise en compte des numéros optionnels
	    //
	    for ( j=0; j<tmp_numberOfCells; j++) for (int k=0; k<NumberOfNodeByCell; k++) 
		  ConnectivityArray[j*NumberOfNodeByCell+k]=tmp_ConnectivityArray[j*(NumberOfNodeByCell+multi)+k] ;

	    //////////////////////////////////////////////////////////////////////////////
	    // Modification pour prise en compte de la numérotation optionnelle des noeuds ///
	    //////////////////////////////////////////////////////////////////////////////
	    //
	    // Rénumérote le tableau temporaire tmp_ConnectivityArray en utilisant _optionnalToCanonicNodesNumbers
	    // Le traitement est identique à la version originelle s'il n'y a pas de numérotation optionnelle
						     
						       // 	if (_ptrMesh->_arePresentOptionnalNodesNumbers==1) 
						       // 		{
						       // 	 	for ( j=0; j<tmp_numberOfCells; j++) for (int k=0; k<NumberOfNodeByCell; k++) 
						       // 	 		ConnectivityArray[j*NumberOfNodeByCell+k]=_ptrMesh->_optionnalToCanonicNodesNumbers[tmp_ConnectivityArray[j*(NumberOfNodeByCell+multi)+k]] ;
						       // 		}
						       // 	else
						       // 		{
						       // 	 	for ( j=0; j<tmp_numberOfCells; j++) for (int k=0; k<NumberOfNodeByCell; k++) 
						       // 	 		ConnectivityArray[j*NumberOfNodeByCell+k]=tmp_ConnectivityArray[j*(NumberOfNodeByCell+multi)+k] ;
						       // 		}
	    ////////////////////////////////////////////////////////////////////////////
	
						       delete[] tmp_ConnectivityArray;
  
	      }

	    Connectivity->_nodal = new MEDSKYLINEARRAY(Connectivity->_count[Connectivity->_numberOfTypes]-1,
						       size,
						       NodalIndex,
						       NodalValue) ; 

	    delete[] NodalIndex;
	    delete[] NodalValue;

	  } // end of bloc to read CELL


	  // Get Face if any
	  // ===============
     
	  if (numberOfFacesTypes!=0) 
	    {

	      // Create a CONNECTIVITY constituent to put in the top level CONNECTIVITY recursive class
	      CONNECTIVITY * constituent = new CONNECTIVITY(numberOfFacesTypes,MED_EN::MED_FACE) ;
	      constituent->_entityDimension = 2 ;
	      constituent->_count[0]=1 ;

	      // In order to create the MEDSKYLINEARRAY of the constituent object we need :
	      // 1:
	      // To initialize the _count array of the constituent object (containning cumulated face count by geometric type)
	      // _count[0]=1 and _count[_numberOfTypes] give the size of NodalIndex
	      // 2:
	      // To calculate the total number of face nodes whatever the geometric type is.
	      // The result is the size of the array containning all the nodes : NodalValue
	      // 3 :
	      // To calculate the starting indexes of the different face types in NodalValue, 
	      // this is the NodalIndex array.
        
	      int size       = 0 ; 
	      int typeNumber = 1 ;
	      int i;
	      for ( i=1; i < MED_NBR_GEOMETRIE_MAILLE; i++)
		{ // no point1 cell type (?)
		  if (tmpFaceCount[i]>0)
		    {
		      constituent->_count[typeNumber] = constituent->_count[typeNumber-1] + tmpFaceCount[i];
		      CELLMODEL t( (MED_EN::medGeometryElement) MED_MESH_DRIVER22::all_cell_type[i]) ;
		      constituent->_type[typeNumber-1]=t ;
	    
		      constituent->_geometricTypes[typeNumber-1]=( MED_EN::medGeometryElement) MED_MESH_DRIVER22::all_cell_type[i] ;
	    
		      size+=tmpFaceCount[i]*((MED_MESH_DRIVER22::all_cell_type[i])%100) ;
		      typeNumber++;
		    }
		}
	
	      // Creation of the MEDSKYLINEARRAY
	      //constituent->_nodal = new MEDSKYLINEARRAY(constituent->_count[constituent->_numberOfTypes]-1,size) ; 
	      //int * NodalIndex = constituent->_nodal->getIndex() ;
	      int * NodalValue = new int[size] ;
	      int * NodalIndex = new int[constituent->_count[constituent->_numberOfTypes]] ;
	      NodalIndex[0]=1 ;
	
	      // Fill the MEDSKYLINEARRAY by reading the MED file.
	      for ( i=0; i<constituent->_numberOfTypes; i++) 
		{
		  med_2_3::med_geometrie_element med_type = (med_2_3::med_geometrie_element) constituent->_type[i].getType() ;

		  int NumberOfNodeByFace = constituent->_type[i].getNumberOfNodes() ;
	  
		  // initialise NodalIndex
		  for (int j=constituent->_count[i]; j<constituent->_count[i+1];j++)
		    NodalIndex[j]=NodalIndex[j-1]+NumberOfNodeByFace ; 
	  
	  int tmp_numberOfFaces = constituent->_count[i+1]-constituent->_count[i] ;
	  // Il faut ajouter 1 pour le zero a la lecture !!!
          // ATTENTION UNIQUEMENT POUR MED < 2.2.x
//CCRT		  int * tmp_constituentArray = NULL;
		  med_2_3::med_int * tmp_constituentArray = NULL;

	    MESSAGE(LOC << "Med file version used here " << major << " " << minor << " " << release);

	    if ((major == 2) && (minor <= 1))
//CCRT		    tmp_constituentArray = new int[(NumberOfNodeByFace+1)*tmp_numberOfFaces] ;
		    tmp_constituentArray = new med_2_3::med_int[(NumberOfNodeByFace+1)*tmp_numberOfFaces] ;
	    else if ((major == 2) && (minor >= 2))
		    {
//CCRT		      tmp_constituentArray = new int[NumberOfNodeByFace*tmp_numberOfFaces] ;
		      tmp_constituentArray = new med_2_3::med_int[NumberOfNodeByFace*tmp_numberOfFaces] ;
            MESSAGE(LOC<<": WE ARE USING MED2.2 so there is no +1 for calculating the size of  tmp_constituentArray !") ;
	  }

		  int err=MEDconnLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
				      Connectivity->_entityDimension,tmp_constituentArray,
				      med_2_3::MED_FULL_INTERLACE,NULL,0,med_2_3::MED_MAILLE,med_type,med_2_3::MED_NOD);

		  if ( err != MED_VALID) 
		    {
		      MESSAGE(LOC<<": MEDconnLire returns "<<err) ;
		      delete constituent ;
		      delete[] tmp_constituentArray;
		      delete[] tmpFaceCount;
		      delete[] tmpEdgeCount;
		      return MED_ERROR ;
		    }

		  int * constituentArray = NodalValue + NodalIndex[constituent->_count[i]-1]-1 ;
	  
	  // version originale sans prise en compte des numéros optionnels
	  //	
		  int multi = 0; // quand est-ce que multi vaut 1 ?? en MED-fichier < 2.2 ??
	    MESSAGE(LOC << "Med file version used here " << major << " " << minor << " " << release);

	    if ((major == 2) && (minor <= 1))
	      for (int j=0; j<tmp_numberOfFaces; j++)
		for (int k=0; k<NumberOfNodeByFace; k++)
		  constituentArray[j*NumberOfNodeByFace+k]=tmp_constituentArray[j*(NumberOfNodeByFace+1)+k] ;
	    else if ((major == 2) && (minor >= 2))
		  for (int j=0; j<tmp_numberOfFaces; j++)
		    for (int k=0; k<NumberOfNodeByFace; k++)
		  constituentArray[j*NumberOfNodeByFace+k]=tmp_constituentArray[j*(NumberOfNodeByFace+multi)+k] ;

		  //////////////////////////////////////////////////////////////////////////////////////
		  ///  Modification pour prise en compte de la numérotation optionnelle des noeuds   ///
		      //////////////////////////////////////////////////////////////////////////////////////
		      ///
		      /// Rénumérote le tableau temporaire tmp_constituentArray en utilisant _optionnalToCanonicNodesNumbers
		      /// Le traitement est identique à la version originelle s'il n'y a pas de numérotation optionnelle
	
		      // 	if (_ptrMesh->_arePresentOptionnalNodesNumbers) 
		      // 		{
		      // 		for (int j=0; j<tmp_numberOfFaces; j++) for (int k=0; k<NumberOfNodeByFace; k++)
		      // 			constituentArray[j*NumberOfNodeByFace+k]=_ptrMesh->_optionnalToCanonicNodesNumbers[tmp_constituentArray[j*(NumberOfNodeByFace+1)+k]] ;
		      // 		}
		      // 	else
		      // 		{
		      // 		for (int j=0; j<tmp_numberOfFaces; j++) for (int k=0; k<NumberOfNodeByFace; k++)
		      // 			constituentArray[j*NumberOfNodeByFace+k]=tmp_constituentArray[j*(NumberOfNodeByFace+1)+k] ;
		      // 		}
  	
		      //////////////////////////////////////////////////////////////////////////////////////
	  
		      delete[] tmp_constituentArray;
		}
	
	      constituent->_nodal = new MEDSKYLINEARRAY(constituent->_count[constituent->_numberOfTypes]-1,
							size,
							NodalIndex,
							NodalValue) ;
	      delete[] NodalIndex ;
	      delete[] NodalValue ;


	      Connectivity->_constituent = constituent ;
	    }

	  delete[] tmpFaceCount;


	  // get Edge if any
	  // ===============
	  if (numberOfEdgesTypes!=0) 
	    {
	      CONNECTIVITY * constituent = new CONNECTIVITY(numberOfEdgesTypes,MED_EDGE) ;
	      constituent->_entityDimension = 1 ;
	      constituent->_count[0]=1 ;

	      int size = 0 ; 
	      int typeNumber=1 ;
	      // if you declare a variable <i> in two <for> initialization statement, 
	      // compiler gcc2.95.3 says nothing but there are two <i> variables in the same block 
	      //and the value you get in the common block seems to be the value of the first variable !
	      int i; 

	      for ( i=1; i<MED_NBR_GEOMETRIE_MAILLE; i++)  
		{ // no point1 cell type (?)
		  if (tmpEdgeCount[i]>0) 
		    {
	    
		      constituent->_count[typeNumber]=constituent->_count[typeNumber-1]+tmpEdgeCount[i];
		      CELLMODEL t( (MED_EN::medGeometryElement) MED_MESH_DRIVER22::all_cell_type[i]) ;
		      constituent->_type[typeNumber-1]=t ;
	    
		      constituent->_geometricTypes[typeNumber-1]=( MED_EN::medGeometryElement) MED_MESH_DRIVER22::all_cell_type[i] ;
	    
		      size+=tmpEdgeCount[i]*((MED_MESH_DRIVER22::all_cell_type[i])%100) ;
		      typeNumber++;
		    }
		}
	
	      // Creation of the MEDSKYLINEARRAY
	      //constituent->_nodal = new MEDSKYLINEARRAY(constituent->_count[constituent->_numberOfTypes]-1,size) ; 
	      //int * NodalIndex = constituent->_nodal->getIndex() ;
	      int * NodalValue = new int[size] ;
	      int * NodalIndex = new int[constituent->_count[constituent->_numberOfTypes]] ;
	      NodalIndex[0]=1 ;
	
	      // Fill the MEDSKYLINEARRAY by reading the MED file.
	      for ( i=0; i<constituent->_numberOfTypes; i++) 
		{
		  med_2_3::med_geometrie_element med_type = (med_2_3::med_geometrie_element) constituent->_type[i].getType() ;

		  int NumberOfNodeByEdge = constituent->_type[i].getNumberOfNodes() ;
	  
		  // initialise index
		  for (int j=constituent->_count[i]; j<constituent->_count[i+1];j++)
		    NodalIndex[j]=NodalIndex[j-1]+NumberOfNodeByEdge ; 
	  
		  int tmp_numberOfEdges = constituent->_count[i+1]-constituent->_count[i] ;
		  // Il faut ajouter 1 pour le zero a la lecture !!!

		  // ATTENTION UNIQUEMENT POUR MED < 2.2.x
//CCRT		  int * tmp_constituentArray = NULL;
		  med_2_3::med_int * tmp_constituentArray = NULL;

	      MESSAGE(LOC << "Med file version used here " << major << " " << minor << " " << release);

	      if ((major == 2) && (minor <= 1))
//CCRT		    tmp_constituentArray = new int[(NumberOfNodeByEdge+1)*tmp_numberOfEdges] ;
		    tmp_constituentArray = new med_2_3::med_int[(NumberOfNodeByEdge+1)*tmp_numberOfEdges] ;
	      else if ((major == 2) && (minor >= 2))
		    {
//CCRT		      tmp_constituentArray = new int[NumberOfNodeByEdge*tmp_numberOfEdges] ;
		      tmp_constituentArray = new med_2_3::med_int[NumberOfNodeByEdge*tmp_numberOfEdges] ;
            MESSAGE(LOC<<": WE ARE USING MED2.2 so there is no +1 for calculating the size of  tmp_constituentArray !") ;
          }
	  
		  int err=MEDconnLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
				      spaceDimension,tmp_constituentArray,
				      med_2_3::MED_FULL_INTERLACE,NULL,0,med_2_3::MED_MAILLE,med_type,med_2_3::MED_NOD);
		  if ( err != MED_VALID) 
		    {
		      MESSAGE(LOC<<": MEDconnLire returns "<<err) ;
		      delete constituent ;
		      delete[] tmp_constituentArray;
		      delete[] tmpEdgeCount;
		      return MED_ERROR ;
		    }

		  int * constituentArray = NodalValue + NodalIndex[constituent->_count[i]-1]-1 ;
	  
	  // version originale sans prise en compte des numéros optionnels	
	  //
		  int multi = 0; // quand est-ce que multi vaut 1 ?? en MED-fichier < 2.2 ??
	      MESSAGE(LOC << "Med file version used here " << major << " " << minor << " " << release);

	      if ((major == 2) && (minor <= 1))
		  for (int j=0; j<tmp_numberOfEdges; j++)
		    for (int k=0; k<NumberOfNodeByEdge; k++)
		    constituentArray[j*NumberOfNodeByEdge+k]=tmp_constituentArray[j*(NumberOfNodeByEdge+multi)+k] ;
	      else if ((major == 2) && (minor >= 2))
		for (int j=0; j<tmp_numberOfEdges; j++)
		  for (int k=0; k<NumberOfNodeByEdge; k++)
		    constituentArray[j*NumberOfNodeByEdge+k]=tmp_constituentArray[j*(NumberOfNodeByEdge)+k] ;

		  //////////////////////////////////////////////////////////////////////////////////////
		  ///  Modification pour prise en compte de la numérotation optionnelle des noeuds   ///
		      //////////////////////////////////////////////////////////////////////////////////////
		      ///
		      /// Rénumérote le tableau temporaire tmp_constituentArray en utilisant _optionnalToCanonicNodesNumbers
		      /// Le traitement est identique à la version originelle s'il n'y a pas de numérotation optionnelle
	
		      // 	if (_ptrMesh->_arePresentOptionnalNodesNumbers) 
		      // 		{
		      // 		for (int j=0; j<tmp_numberOfEdges; j++) for (int k=0; k<NumberOfNodeByEdge; k++)
		      // 			constituentArray[j*NumberOfNodeByEdge+k]=_ptrMesh->_optionnalToCanonicNodesNumbers[tmp_constituentArray[j*(NumberOfNodeByEdge+1)+k]] ;
		      // 		}
		      // 	else
		      // 		{
		      // 		for (int j=0; j<tmp_numberOfEdges; j++) for (int k=0; k<NumberOfNodeByEdge; k++)
		      // 			constituentArray[j*NumberOfNodeByEdge+k]=tmp_constituentArray[j*(NumberOfNodeByEdge+1)+k] ;
		      // 		}
  	
		      //////////////////////////////////////////////////////////////////////////////////////

		      delete[] tmp_constituentArray;
		}

	      constituent->_nodal = new MEDSKYLINEARRAY(constituent->_count[constituent->_numberOfTypes]-1,
							size,
							NodalIndex,
							NodalValue) ;

	      delete[] NodalIndex ;
	      delete[] NodalValue ;

	      if (Connectivity->_entityDimension == 3) 
		{
		  if (Connectivity->_constituent==NULL)
		    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Edges are defined but there are no Faces !"));
		  Connectivity->_constituent->_constituent = constituent ;
		} else 
		  Connectivity->_constituent = constituent ;
	    }

	  delete[] tmpEdgeCount; 

      
	} // end classic geometric types
      else
        {//Polyg/Polh only...
          delete [] Connectivity->_count;
          Connectivity->_count=new int[1];
          Connectivity->_count[0]=1;
        }
      delete[] tmp_cells_count;

         // Lecture des polygones MED_CELL
      int NumberOfPolygons = MEDnEntMaa(_medIdt,
                                      const_cast <char *> (_ptrMesh->_name.c_str()),
                                      med_2_3::MED_CONN,
                                      med_2_3::MED_MAILLE,
                                      med_2_3::MED_POLYGONE,
                                      med_2_3::MED_NOD);

        // Correction to permit the loading of mesh dimensionned at 2 even
        // if it has only MED_POLYGON elements

        if (NumberOfPolygons > 0)
        {
          if (Connectivity->_entityDimension < 2) Connectivity->_entityDimension = 2;
        }

        if (NumberOfPolygons > 0)
        {
          // By consequence this exception will never occur
          if (Connectivity->_entityDimension == 1)
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"In a 2D mesh, polygons need at least one 2D cell of a classic geometric type !"));
          med_2_3::med_int ConnectivitySize;
          med_2_3::med_err err1 = MEDpolygoneInfo(_medIdt,
                                                  const_cast <char *> (_ptrMesh->_name.c_str()),
                                                  med_2_3::MED_MAILLE,
                                                  med_2_3::MED_NOD,
                                                  &ConnectivitySize);
          if (err1 != MED_VALID)
          {
            MESSAGE(LOC<<": MEDpolygoneInfo returns "<<err1);
            return MED_ERROR;
          }

          med_2_3::med_int* PolygonsConnectivity = new med_2_3::med_int[ConnectivitySize];
          med_2_3::med_int* PolygonsConnectivityIndex = new med_2_3::med_int[NumberOfPolygons+1];

          med_2_3::med_err err2 = MEDpolygoneConnLire(_medIdt,
                                                      const_cast <char *> (_ptrMesh->_name.c_str()),
                                                      PolygonsConnectivityIndex,
                                                      NumberOfPolygons+1,
                                                      PolygonsConnectivity,
                                                      med_2_3::MED_MAILLE,
                                                      med_2_3::MED_NOD);
          if (err2 != MED_VALID)
          {
            MESSAGE(LOC<<": MEDpolygoneConnLire returns "<<err2);
            return MED_ERROR;
          }

          if (Connectivity->_entityDimension == 2) // 2D mesh : polygons in Connectivity
          {
//CCRT
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
            int* tmp_PolygonsConnectivity = new int[ConnectivitySize];
            int i ;
            for ( i = 0 ; i < ConnectivitySize ; i++ )
              tmp_PolygonsConnectivity[i] = PolygonsConnectivity[i] ;

            int* tmp_PolygonsConnectivityIndex = new int[NumberOfPolygons+1];
            for ( i = 0 ; i < NumberOfPolygons+1 ; i++ )
              tmp_PolygonsConnectivityIndex[i] = PolygonsConnectivityIndex[i] ;

            Connectivity->setPolygonsConnectivity
              (MED_NODAL, (medEntityMesh) Entity, tmp_PolygonsConnectivity,
               tmp_PolygonsConnectivityIndex, ConnectivitySize, NumberOfPolygons);

            delete [] tmp_PolygonsConnectivity ;
            delete [] tmp_PolygonsConnectivityIndex ;
#else
            Connectivity->setPolygonsConnectivity
              (MED_NODAL, (medEntityMesh) Entity, PolygonsConnectivity,
               PolygonsConnectivityIndex, ConnectivitySize, NumberOfPolygons);
#endif
          }
          else if (Connectivity->_entityDimension == 3)
          {
            if (Connectivity->_constituent == NULL) // 3D mesh : polygons in Connectivity->_constituent
              Connectivity->_constituent = new CONNECTIVITY(MED_FACE);
//CCRT
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
            int* tmp_PolygonsConnectivity = new int[ConnectivitySize];
            int i ;
            for ( i = 0 ; i < ConnectivitySize ; i++ )
              tmp_PolygonsConnectivity[i] = PolygonsConnectivity[i] ;

            int* tmp_PolygonsConnectivityIndex = new int[NumberOfPolygons+1];
            for ( i = 0 ; i < NumberOfPolygons+1 ; i++ )
              tmp_PolygonsConnectivityIndex[i] = PolygonsConnectivityIndex[i] ;

            Connectivity->_constituent->setPolygonsConnectivity
              (MED_NODAL, MED_FACE, tmp_PolygonsConnectivity,
               tmp_PolygonsConnectivityIndex, ConnectivitySize, NumberOfPolygons);

            delete [] tmp_PolygonsConnectivity ;
            delete [] tmp_PolygonsConnectivityIndex ;
#else
            Connectivity->_constituent->setPolygonsConnectivity
              (MED_NODAL, MED_FACE, PolygonsConnectivity,
               PolygonsConnectivityIndex, ConnectivitySize, NumberOfPolygons);
#endif
          }

          delete[] PolygonsConnectivity;
          delete[] PolygonsConnectivityIndex;
        } // end polygons


      // Lecture des polyedres MED_CELL
      med_int NumberOfPolyhedron = MEDnEntMaa(_medIdt,
					      const_cast <char *> (_ptrMesh->_name.c_str()),
					      med_2_3::MED_CONN,
					      Entity,
					      med_2_3::MED_POLYEDRE,
					      med_2_3::MED_NOD);

      // Correction to permit the loading of mesh dimensionned at 3 even
      // if it has only MED_POLYEDRE elements

      if (NumberOfPolyhedron > 0)
	{
	  Connectivity->_entityDimension = 3;
	}

      if (NumberOfPolyhedron > 0)
	{
	  // By consequence this exception will never occur
	  if (Connectivity->_entityDimension == 2 || Connectivity->_entityDimension == 1)
	    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"In a 3D mesh, polyhedron need at least one 3D cell of a classic geometric type !"));
	  med_2_3::med_int FacesIndexSize, NumberOfNodes, NumberOfFaces;
	  med_2_3::med_err err3 = MEDpolyedreInfo(_medIdt,
						  const_cast <char *> (_ptrMesh->_name.c_str()),
						  med_2_3::MED_NOD,
						  &FacesIndexSize,
						  &NumberOfNodes);
	  NumberOfFaces = FacesIndexSize-1;
	  if (err3 != MED_VALID)
	    {
	      MESSAGE(LOC<<": MEDpolyhedreInfo returns "<<err3);
	      return MED_ERROR;
	    }
      
	  med_2_3::med_int* Nodes = new med_2_3::med_int[NumberOfNodes];
	  med_2_3::med_int* FacesIndex = new med_2_3::med_int[NumberOfFaces+1];
	  med_2_3::med_int* PolyhedronIndex = new med_2_3::med_int[NumberOfPolyhedron+1];
	  
	  med_2_3::med_err err4 = MEDpolyedreConnLire(_medIdt,
						      const_cast <char *> (_ptrMesh->_name.c_str()),
						      PolyhedronIndex,
						      NumberOfPolyhedron+1,
						      FacesIndex,
						      NumberOfFaces+1,
						      Nodes,
						      med_2_3::MED_NOD);
	  if (err4 != MED_VALID)
	    {
	      MESSAGE(LOC<<": MEDpolyedreConnLire returns "<<err4);
	      return MED_ERROR;
	    }
      
//CCRT
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
	  int* tmp_Nodes = new int[NumberOfNodes];
          int i ;
          for ( i = 0 ; i < NumberOfNodes ; i++ )
             tmp_Nodes[i] = Nodes[i] ;
	  int* tmp_FacesIndex = new int[NumberOfFaces+1];
          for ( i = 0 ; i < NumberOfFaces+1 ; i++ )
             tmp_FacesIndex[i] = FacesIndex[i] ;
	  int* tmp_PolyhedronIndex = new int[NumberOfPolyhedron+1];
          for ( i = 0 ; i < NumberOfPolyhedron+1 ; i++ )
             tmp_PolyhedronIndex[i] = PolyhedronIndex[i] ;
	  Connectivity->setPolyhedronConnectivity(MED_NODAL,tmp_Nodes,tmp_PolyhedronIndex,NumberOfNodes,NumberOfPolyhedron,tmp_FacesIndex,NumberOfFaces);
	  delete[] tmp_Nodes;
	  delete[] tmp_FacesIndex;
	  delete[] tmp_PolyhedronIndex;
#else
	  Connectivity->setPolyhedronConnectivity(MED_NODAL,Nodes,PolyhedronIndex,NumberOfNodes,NumberOfPolyhedron,FacesIndex,NumberOfFaces);
#endif
	  
	  delete[] Nodes;
	  delete[] FacesIndex;
	  delete[] PolyhedronIndex;
	} // end polyhedron



      // If there is no nodal connectivity, we return MED_ERROR !
      if (Connectivity->_numberOfTypes == 0 && NumberOfPolygons == 0 && NumberOfPolyhedron == 0)
	return MED_ERROR;
      else
	return MED_VALID;

    }
  return MED_ERROR;
}

int  MED_MESH_RDONLY_DRIVER22::getFAMILY() 
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER22::getFAMILY() : " ;
  BEGIN_OF(LOC);

  if (_status==MED_OPENED)
    {
      int err = 0 ;

      int * MEDArrayNodeFamily = NULL ;
      int ** MEDArrayCellFamily = NULL ;
      int ** MEDArrayFaceFamily = NULL ;
      int ** MEDArrayEdgeFamily = NULL ;

    // NODE :
      MEDArrayNodeFamily = new int[_ptrMesh->getNumberOfNodes()] ;

      err = getNodesFamiliesNumber(MEDArrayNodeFamily) ;
      // error only if (_status!=MED_OPENED), other case exeception !
      // CELL

      MESSAGE(LOC << "error returned from getNodesFamiliesNumber " << err);

      MEDArrayCellFamily = new int* [_ptrMesh->getNumberOfTypesWithPoly(MED_CELL)] ;
      // ET SI IL N'Y A PAS DE CELLS ?

      medGeometryElement * myTypes = _ptrMesh->getTypesWithPoly(MED_CELL);
      for (int i=0;i<_ptrMesh->getNumberOfTypesWithPoly(MED_CELL);i++)
	MEDArrayCellFamily[i] = new
	  int[_ptrMesh->getNumberOfElementsWithPoly(MED_CELL,myTypes[i])] ;

      // assure connectivety exists in case of GRID
      _ptrMesh->getConnectivityptr(); //PAL14113

      err = getCellsFamiliesNumber(MEDArrayCellFamily,
				   _ptrMesh->_connectivity,MED_CELL) ;
      delete [] myTypes;

      MESSAGE(LOC << "error returned from getCellsFamiliesNumber for Cells " << err);

    if (_ptrMesh->_connectivity->_constituent != NULL)
      {
	if (_ptrMesh->_connectivity->_constituent->_entity == MED_EN::MED_FACE)
	  {
	    // FACE
	    MEDArrayFaceFamily = new
	      int* [_ptrMesh->getNumberOfTypesWithPoly(MED_FACE)] ;

	    myTypes = _ptrMesh->getTypesWithPoly(MED_FACE);
	    for (int i=0;i<_ptrMesh->getNumberOfTypesWithPoly(MED_FACE);i++)
	      MEDArrayFaceFamily[i] = new
		int[_ptrMesh->getNumberOfElementsWithPoly(MED_FACE,myTypes[i])] ;

	    err =
	      getCellsFamiliesNumber(MEDArrayFaceFamily,
				     _ptrMesh->_connectivity->_constituent,MED_FACE) ;
	    delete [] myTypes;
	    MESSAGE(LOC << "error returned from getCellsFamiliesNumber for Faces " << err);
	  }
	else
	  {
	    // EDGE in 2D
	    MEDArrayEdgeFamily = new
	      int* [_ptrMesh->getNumberOfTypes(MED_EDGE)] ;

	    const medGeometryElement *myTypes2 = _ptrMesh->getTypes(MED_EDGE);
	    for (int i=0;i<_ptrMesh->getNumberOfTypes(MED_EDGE);i++)
	      MEDArrayEdgeFamily[i] = new
		int[_ptrMesh->getNumberOfElements(MED_EDGE,myTypes2[i])] ;

	    err =
	      getCellsFamiliesNumber(MEDArrayEdgeFamily,
				     _ptrMesh->_connectivity->_constituent,MED_FACE) ;
	  
	    MESSAGE(LOC << "error returned from getCellsFamiliesNumber for Edges in 2D " << err);
	  }
	// EDGE in 3D
	if (_ptrMesh->_connectivity->_constituent->_constituent != NULL)
	  {
	    MEDArrayEdgeFamily = new
	      int* [_ptrMesh->getNumberOfTypes(MED_EDGE)] ;

	    const medGeometryElement *myTypes2 = _ptrMesh->getTypes(MED_EDGE);
	    for (int i=0;i<_ptrMesh->getNumberOfTypes(MED_EDGE);i++)
	      MEDArrayEdgeFamily[i] = new
		int[_ptrMesh->getNumberOfElements(MED_EDGE,myTypes2[i])] ;

	    err =
	      getCellsFamiliesNumber(MEDArrayEdgeFamily,
				     _ptrMesh->_connectivity->_constituent->_constituent,MED_EDGE);
	    // we are in 3D !
	
	    MESSAGE(LOC << "error returned from getCellsFamiliesNumber for Edges in 3D " << err);

	  }
      }

    int NumberOfFamilies = med_2_3::MEDnFam(_medIdt, const_cast <char *>
					   (_meshName.c_str())) ;

    if ( NumberOfFamilies < 1 ) // at least family 0 must exist 
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"There is no FAMILY, FAMILY 0 must exists" ));

    SCRUTE(NumberOfFamilies);

    vector<FAMILY*> &NodeFamilyVector = _ptrMesh->_familyNode ;
    vector<FAMILY*> &CellFamilyVector = _ptrMesh->_familyCell ;
    vector<FAMILY*> &FaceFamilyVector = _ptrMesh->_familyFace ;
    vector<FAMILY*> &EdgeFamilyVector = _ptrMesh->_familyEdge ;

    int numberOfNodesFamilies = 0 ;
    int numberOfCellsFamilies = 0 ;
    int numberOfFacesFamilies = 0 ;
    int numberOfEdgesFamilies = 0 ;

    for (int i=0;i<NumberOfFamilies;i++)
      {
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
	med_2_3::med_int tmp_NumberOfAttributes = med_2_3::MEDnAttribut(_medIdt,
									const_cast <char *>(_meshName.c_str()),
									(i+1));
        med_2_3::med_int NumberOfAttributes = tmp_NumberOfAttributes ;
#else
	int NumberOfAttributes = med_2_3::MEDnAttribut(_medIdt,
						      const_cast <char *>
						      (_meshName.c_str()),
						      (i+1));
#endif
      
      if (NumberOfAttributes < 0) 
	throw MEDEXCEPTION("MED_MESH_RDONLY_DRIVER22::getFAMILY() : NumberOfAttributes" );

#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
      med_2_3::med_int tmp_NumberOfGroups = med_2_3::MEDnGroupe(_medIdt, 
								 const_cast <char *>(_meshName.c_str()),
								(i+1)) ;
      int NumberOfGroups = tmp_NumberOfGroups ;
#else
      int NumberOfGroups = med_2_3::MEDnGroupe(_medIdt, 
					       const_cast <char *>(_meshName.c_str()),
					       (i+1)) ;
#endif

	if (NumberOfGroups < 0)
	  throw MEDEXCEPTION("MED_MESH_RDONLY_DRIVER22::getFAMILY() : NumberOfGroups" );
      
	int FamilyIdentifier ;
	string FamilyName(MED_TAILLE_NOM,'\0');
	int *  AttributesIdentifier = new int[NumberOfAttributes] ;
	int *  AttributesValues     = new int[NumberOfAttributes] ;
	string AttributesDescription(MED_TAILLE_DESC*NumberOfAttributes,' ') ;
	string GroupsNames(MED_TAILLE_LNOM*NumberOfGroups+1,'\0') ;
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
	med_2_3::med_int tmp_FamilyIdentifier ;
	med_2_3::med_int *  tmp_AttributesIdentifier = new med_2_3::med_int[NumberOfAttributes] ;
	med_2_3::med_int *  tmp_AttributesValues     = new med_2_3::med_int[NumberOfAttributes] ;
	err = med_2_3::MEDfamInfo(_medIdt,
				  const_cast <char *>(_meshName.c_str()),
				  (i+1),
				  const_cast <char *>(FamilyName.c_str()), 
				  &tmp_FamilyIdentifier,
				  tmp_AttributesIdentifier,
				  tmp_AttributesValues,
				  const_cast <char *>(AttributesDescription.c_str()),
				  &tmp_NumberOfAttributes, 
				  const_cast <char *>(GroupsNames.c_str()),
				  &tmp_NumberOfGroups);
        FamilyIdentifier = tmp_FamilyIdentifier ;
        int ii ;
        for ( ii = 0 ; ii < NumberOfAttributes ; ii++ ) {
           AttributesIdentifier[ii] = tmp_AttributesIdentifier[ii] ;
           AttributesValues[ii] = tmp_AttributesValues[ii] ;
        }
        NumberOfAttributes = tmp_NumberOfAttributes ;
	NumberOfGroups = tmp_NumberOfGroups ;
        delete [] tmp_AttributesIdentifier ;
        delete [] tmp_AttributesValues ;
#else
	err = med_2_3::MEDfamInfo(_medIdt,const_cast <char *>
				 (_meshName.c_str()),
				 (i+1),const_cast <char *>
				 (FamilyName.c_str()), &FamilyIdentifier,
				 AttributesIdentifier,AttributesValues,
				 const_cast <char *>
				 (AttributesDescription.c_str()),
				 &NumberOfAttributes, const_cast <char *>
				 (GroupsNames.c_str()),&NumberOfGroups);
#endif


	SCRUTE(GroupsNames);
	SCRUTE(FamilyName);
	SCRUTE(err);
	SCRUTE(i);

      if (err != MED_VALID)
	throw MEDEXCEPTION("MED_MESH_RDONLY_DRIVER22::getFAMILY() : ERROR when get FAMILY informations" );

	if (FamilyIdentifier != 0 )
	  {
	    FAMILY * Family = new FAMILY(_ptrMesh,FamilyIdentifier,FamilyName,
					 NumberOfAttributes,
					 AttributesIdentifier,
					 AttributesValues,
					 AttributesDescription,
					 NumberOfGroups,GroupsNames,
					 MEDArrayNodeFamily,
					 MEDArrayCellFamily,
					 MEDArrayFaceFamily,
					 MEDArrayEdgeFamily) ;

	    // All good ?
	    // if nothing found, delete Family

			/*! \todo TODO : once unit tests are ready, check that there is no side effect
				and let the driver read empty families by removing the if case of the
				few following lines */

	    if (Family->getNumberOfTypes() == 0)
	      {
		MESSAGE(LOC<<"Nothing found for family "<<FamilyName<<
			" : skip");
		delete Family;
	      }
	    else
	      switch (Family->getEntity())
		{
		case MED_EN::MED_NODE :
		  NodeFamilyVector.push_back(Family) ;
		  numberOfNodesFamilies++ ;
		  break ;
		case MED_EN::MED_CELL :
		  CellFamilyVector.push_back(Family) ;
		  numberOfCellsFamilies++ ;
		  break ;
		case MED_EN::MED_FACE :
		  FaceFamilyVector.push_back(Family) ;
		  numberOfFacesFamilies++ ;
		  break ;
		case MED_EN::MED_EDGE :
		  EdgeFamilyVector.push_back(Family) ;
		  numberOfEdgesFamilies++ ;
		  break ;
		}
	    //	MESSAGE(LOC << (*Family));
	  }

	delete [] AttributesIdentifier ;
	delete [] AttributesValues ;
      }

    if (MEDArrayNodeFamily != NULL)
      delete[] MEDArrayNodeFamily ;

    if (MEDArrayCellFamily != NULL)
      {
	for (int i=0; i<_ptrMesh->getNumberOfTypesWithPoly(MED_CELL); i++)
	  delete[] MEDArrayCellFamily[i] ;
	delete[] MEDArrayCellFamily ;
      }

    if (MEDArrayFaceFamily != NULL)
      {
	for (int i=0; i<_ptrMesh->getNumberOfTypesWithPoly(MED_FACE); i++)
	  delete[] MEDArrayFaceFamily[i] ;
	delete[] MEDArrayFaceFamily ;
      }

    if (MEDArrayEdgeFamily != NULL)
      {
	for (int i=0; i<_ptrMesh->getNumberOfTypesWithPoly(MED_EDGE); i++)
	  delete[] MEDArrayEdgeFamily[i] ;
	delete[] MEDArrayEdgeFamily ;
      }

    END_OF();
    return MED_VALID ;
    }

  return MED_ERROR;
}

int  MED_MESH_RDONLY_DRIVER22::getNodesFamiliesNumber(int * MEDArrayNodeFamily)
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER22::getNodesFamiliesNumber() : " ;

  BEGIN_OF(LOC);

  if (_status==MED_OPENED)
    {
    int err = 0 ;

#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
      med_2_3::med_int * tmp_MEDArrayNodeFamily = new med_2_3::med_int[_ptrMesh->getNumberOfNodes()] ;
      err = MEDfamLire(_medIdt, const_cast <char *>
		       (_ptrMesh->_name.c_str()), tmp_MEDArrayNodeFamily,
		       _ptrMesh->getNumberOfNodes(), med_2_3::MED_NOEUD,
		       (med_2_3::med_geometrie_element) MED_NONE);
      int i ;
      for ( i = 0 ; i < _ptrMesh->getNumberOfNodes() ; i++ )
         MEDArrayNodeFamily[i] = tmp_MEDArrayNodeFamily[i] ;
#else
      err = MEDfamLire(_medIdt, const_cast <char *>
		       (_ptrMesh->_name.c_str()), MEDArrayNodeFamily,
		       _ptrMesh->getNumberOfNodes(), med_2_3::MED_NOEUD,
		       (med_2_3::med_geometrie_element) MED_NONE);
#endif

      if ( err != MED_VALID)
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "There is no family for the |"<< _ptrMesh->getNumberOfNodes() << "| nodes in mesh |" << _ptrMesh->_name.c_str() << "|"));

      END_OF();
      return MED_VALID;
    }

  return MED_ERROR;
}

int  MED_MESH_RDONLY_DRIVER22::getCellsFamiliesNumber(int **MEDArrayFamily,
						      CONNECTIVITY *Connectivity,
						      MED_EN::medEntityMesh entity) 
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER22::getCellsFamiliesNumber " ;

  BEGIN_OF(LOC);

  if (_status==MED_OPENED)
    {
    int i, err = 0 ;
    MED_EN::medGeometryElement *types=Connectivity->getGeometricTypesWithPoly(Connectivity->_entity);
    for (i=0;i<Connectivity->getNumberOfTypesWithPoly(Connectivity->_entity);i++)
      {
	int NumberOfCell=Connectivity->getNumberOfElementsWithPoly(Connectivity->_entity,types[i]);
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
        med_2_3::med_int * tmp_MEDArrayFamily = new med_2_3::med_int[NumberOfCell] ;
	err=MEDfamLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
		       tmp_MEDArrayFamily,NumberOfCell,
		       (med_2_3::med_entite_maillage) Connectivity->_entity,
		       (med_2_3::med_geometrie_element)types[i]);
	if (err != MED_VALID)
	  {
	    err=MEDfamLire(_medIdt,const_cast <char *>
			   (_ptrMesh->_name.c_str()),
			   tmp_MEDArrayFamily,NumberOfCell,
			   med_2_3::MED_MAILLE,
			   (med_2_3::med_geometrie_element)types[i]);
	    if (err != MED_VALID
                && !_ptrMesh->getIsAGrid() ) // (skl for IPAL14260)
	      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Family not found for entity "<<Connectivity->_entity<<" and geometric type "<<types[i]));
	  }
	if (err == MED_VALID) {
	  int ii ;
	  for ( ii = 0 ; ii < NumberOfCell ; ii++ )
	    MEDArrayFamily[i][ii] = tmp_MEDArrayFamily[ii] ;
	}
        delete [] tmp_MEDArrayFamily ;
#else
	err=MEDfamLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
		       MEDArrayFamily[i],NumberOfCell,
		       (med_2_3::med_entite_maillage) Connectivity->_entity,
		       (med_2_3::med_geometrie_element)types[i]);
	if (err != MED_VALID)
	  {
	    err=MEDfamLire(_medIdt,const_cast <char *>
			   (_ptrMesh->_name.c_str()),
			   MEDArrayFamily[i],NumberOfCell,
			   med_2_3::MED_MAILLE,
			   (med_2_3::med_geometrie_element)types[i]);
	    if (err != MED_VALID
                && !_ptrMesh->getIsAGrid() ) // it's normal for a grid (PAL14113)
	      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Family not found for entity "<<Connectivity->_entity<<" and geometric type "<<types[i]));
	  }
#endif
      }
    delete [] types;
    return MED_VALID;
    }
  return MED_ERROR;
}

/*--------------------- WRONLY PART -------------------------------*/

MED_MESH_WRONLY_DRIVER22::MED_MESH_WRONLY_DRIVER22()
{
  this->GENDRIVER::_accessMode = MED_EN::WRONLY;
}
  
MED_MESH_WRONLY_DRIVER22::MED_MESH_WRONLY_DRIVER22(const string & fileName,
																									 MESH * ptrMesh, 
																									 MED_EN::med_mode_acces access):
  MED_MESH_DRIVER22(fileName,ptrMesh,access),IMED_MESH_WRONLY_DRIVER(fileName,ptrMesh),MED_MESH_DRIVER(fileName,ptrMesh,access)
{
  MESSAGE("MED_MESH_WRONLY_DRIVER22::MED_MESH_WRONLY_DRIVER22(const string & fileName, MESH * ptrMesh) has been created");
}

MED_MESH_WRONLY_DRIVER22::MED_MESH_WRONLY_DRIVER22(const MED_MESH_WRONLY_DRIVER22 & driver):
  IMED_MESH_WRONLY_DRIVER(driver),MED_MESH_DRIVER22(driver),MED_MESH_DRIVER(driver)
{

}

MED_MESH_WRONLY_DRIVER22::~MED_MESH_WRONLY_DRIVER22()
{
  //MESSAGE("MED_MESH_WRONLY_DRIVER22::MED_MESH_WRONLY_DRIVER22(const string & fileName, MESH * ptrMesh) has been destroyed");
 
}

GENDRIVER * MED_MESH_WRONLY_DRIVER22::copy(void) const
{
  return new MED_MESH_WRONLY_DRIVER22(*this);
}

void MED_MESH_WRONLY_DRIVER22::write(void) const
{ 
  
  if (_ptrMesh==NULL || _ptrMesh->getCoordinateptr()==NULL)
    throw MEDEXCEPTION("Error trying to write an empty mesh");

  const char * LOC = "void MED_MESH_WRONLY_DRIVER22::write(void) const : ";
  BEGIN_OF(LOC);

  // we must first create mesh !!
  MESSAGE(LOC << "MeshName : |" << _meshName << "| FileName : |"<<_fileName<<"| MedIdt : | "<< _medIdt << "|");

  if (_status!=MED_OPENED)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "File "<<_fileName<<" is not open. Open it before write !"));

 
  string fieldName;
  if ( ( _meshName.empty() ) && ( _ptrMesh->_name.empty() )    )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)
				 <<" neither <meshName> is set in driver nor in object MESH.")) ;

  // If _meshName is not set in driver, try to use _ptrmesh->_meshName
  if ( ( _meshName.empty() ) && ( !_ptrMesh->_name.empty() )    )
    _meshName = healName(_ptrMesh->_name );

  if ( _meshName.size() > MED_TAILLE_NOM )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)
				 <<" <meshName> size in object driver MESH is > MED_TAILLE_NOM ."));


  if (_ptrMesh->getIsAGrid())
  {
    if ( writeGRID() != MED_VALID )
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeGRID()"  )) ;
  }
  else
  {
    if (writeCoordinates()!=MED_VALID)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeCoordinates()"  )) ;

    if (writeConnectivities(MED_EN::MED_CELL)!=MED_VALID)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeConnectivities(MED_CELL)"  )) ;
    if (writeConnectivities(MED_EN::MED_FACE)!=MED_VALID)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeConnectivities(MED_FACE)"  )) ;
    if (writeConnectivities(MED_EN::MED_EDGE)!=MED_VALID)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeConnectivities(MED_EDGE)"  )) ;
  }

  if (writeFamilyNumbers() !=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeFamilyNumbers()"  )) ;
  

  // well we must first write zero family :
  if (_status==MED_OPENED) {
    int err ;
    // test if the family already exists (HDF trick waiting a MED evolution to be replaced)
    string dataGroupFam = "/ENS_MAA/"+_meshName+"/FAS/FAMILLE_ZERO/";  
    MESSAGE("|"<<dataGroupFam<<"|");
    err =med_2_3::_MEDdatagroupOuvrir(_medIdt,const_cast <char *> (dataGroupFam.c_str()) );
    if ( err < MED_VALID ) {
      SCRUTE(err);

      char familyName[MED_TAILLE_NOM+1];
      //      strcpy(familyName,"FAMILLE_ZERO");
      err = med_2_3::MEDfamCr( _medIdt,
			      const_cast <char *> ( _meshName.c_str() ),
			      familyName, 0,
//CCRT			      (int*)NULL, (int*)NULL, (char*)NULL, 0,
			      (med_2_3::med_int*)NULL, (med_2_3::med_int*)NULL, (char*)NULL, 0,
			      (char*)NULL, 0);

      SCRUTE(familyName);
      
      if ( err != MED_VALID) 
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't create family |FAMILLE_ZERO| with identifier |0| groups names || and  attributes descriptions ||")) ;
    }
    else
      med_2_3::_MEDdatagroupFermer(_medIdt);
     
  }

  MESSAGE(LOC<<"writeFamilies(_ptrMesh->_familyNode)");
  if (writeFamilies(_ptrMesh->_familyNode) !=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeFamilies(_ptrMesh->_familyNode)"  )) ;

  MESSAGE(LOC<<"writeFamilies(_ptrMesh->_familyCell)");
  if (writeFamilies(_ptrMesh->_familyCell) !=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeFamilies(_ptrMesh->_familyCell)"  )) ;

  MESSAGE(LOC<<"writeFamilies(_ptrMesh->_familyFace)");
  if (writeFamilies(_ptrMesh->_familyFace) !=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeFamilies(_ptrMesh->_familyFace)"  )) ;

  MESSAGE(LOC<<"writeFamilies(_ptrMesh->_familyEdge)");
  if (writeFamilies(_ptrMesh->_familyEdge) !=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeFamilies(_ptrMesh->_familyEdge)"  )) ;
       
  END_OF();
} 

//=======================================================================
//function : writeGRID
//purpose  : 
//=======================================================================

int MED_MESH_WRONLY_DRIVER22::writeGRID() const
{
  const char * LOC = "MED_MESH_WRONLY_DRIVER22::writeGRID() : " ;
  BEGIN_OF(LOC);
  
  if (_status!=MED_OPENED)
  {
    MESSAGE (LOC<<" Not open !!!");
    return MED_ERROR;
  }
  GRID * ptrGrid = (GRID*) _ptrMesh;
  
  med_2_3::med_err err = MED_ERROR;
  med_2_3::med_repere rep;
  string tmp_name(_ptrMesh->_spaceDimension*MED_TAILLE_PNOM22,' ');
  string tmp_unit(_ptrMesh->_spaceDimension*MED_TAILLE_PNOM22,' ');

  // Test if the mesh <_meshName> already exists
  // If it doesn't exists create it
  // If it already exists verify if its space and mesh dimensions are the same
  // as <_ptrMesh->_spaceDimension>, <_ptrMesh->_meshDimension> respectively
  // rem : <_meshName> is the driver meshName not <ptrMesh->_meshName>

  int spaceDimension = med_2_3::MEDdimEspaceLire(_medIdt, const_cast <char *>
						(_meshName.c_str()) );

  int meshDimension = med_2_3::MEDdimLire(_medIdt, const_cast <char *>
					 (_meshName.c_str()) );


  MESSAGE(LOC << "spacedim " << spaceDimension << " meshdim " << meshDimension);

  MESSAGE(LOC << "From the mesh spacedim " << _ptrMesh->_spaceDimension << " meshdim " << _ptrMesh->_meshDimension);

  if ((spaceDimension <= MED_VALID) && (meshDimension <= MED_VALID))
    {
      err = MEDmaaCr(_medIdt,
		     const_cast <char *> (_meshName.c_str()),
		     _ptrMesh->_meshDimension,med_2_3::MED_STRUCTURE,
		     const_cast <char *> (_ptrMesh->_description.c_str()));

      meshDimension =  _ptrMesh->_meshDimension;

      if (err != MED_VALID)
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Unable to create Grid"));
      else 
	MESSAGE(LOC<<"Grid "<<_meshName<<" created in file "<<_fileName<<" !");
    }
  else if ((spaceDimension != _ptrMesh->_spaceDimension)  &&
	   (meshDimension != _ptrMesh->_meshDimension))
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Grid |" << _meshName.c_str() <<
                                 "| already exists in file |" << _fileName <<
                                 "| with space dimension |" << spaceDimension <<
                                 "| and mesh dimension |" << meshDimension <<
                                 "| but the space dimension and the mesh dimension of the mesh we want to write are respectively |"
                                 << _ptrMesh->_spaceDimension <<"|" <<
				 _ptrMesh->_meshDimension <<"|" )) ;

  MED_EN::med_grid_type gridType = ptrGrid->getGridType();

  err = med_2_3::MEDnatureGrilleEcr(_medIdt,
				   const_cast <char *> (_meshName.c_str()),
				   (med_2_3::med_type_grille) gridType);

  if (err != MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Unable to write the type of the Grid"));

  // Recompose the <_spaceDimension> strings in 1 string 
  int lengthString ;
  string valueString ;
  for (int i=0;i<_ptrMesh->_spaceDimension;i++) {
    SCRUTE(i);
    valueString = _ptrMesh->_coordinate->_coordinateName[i] ;
    lengthString = (MED_TAILLE_PNOM22<valueString.size())?MED_TAILLE_PNOM22:valueString.size() ;
    tmp_name.replace(i*MED_TAILLE_PNOM22,i*MED_TAILLE_PNOM22+lengthString,valueString,0,lengthString);
    valueString = _ptrMesh->_coordinate->_coordinateUnit[i];
    lengthString = (MED_TAILLE_PNOM22<valueString.size())?MED_TAILLE_PNOM22:valueString.size() ;
    tmp_unit.replace(i*MED_TAILLE_PNOM22,i*MED_TAILLE_PNOM22+lengthString,valueString,0,lengthString);
  }

  // Pourquoi le stocker sous forme de chaîne ?
  const string & coordinateSystem = _ptrMesh->_coordinate->_coordinateSystem;
  if      (coordinateSystem  == "CARTESIAN") 
    rep = med_2_3::MED_CART;
  else if ( coordinateSystem == "CYLINDRICAL")
    rep = med_2_3::MED_CYL;
  else if ( coordinateSystem == "SPHERICAL" )
    rep = med_2_3::MED_SPHER;
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Grid |" << _meshName.c_str() <<
                                 "| doesn't have a valid coordinate system : |" 
                                 << _ptrMesh->_coordinate->_coordinateSystem
                                 << "|" )) ;  

  med_2_3::med_int ArrayLen[] = { (med_2_3::med_int) ptrGrid->_iArrayLength,
                     (med_2_3::med_int) ptrGrid->_jArrayLength,
                     (med_2_3::med_int) ptrGrid->_kArrayLength  };
  
  // Write node coordinates for MED_BODY_FITTED grid
  if (gridType == MED_EN::MED_BODY_FITTED)
    {
      // Write Coordinates and families
//       double * coo = const_cast <double *>
// 	(_ptrMesh->_coordinate->getCoordinates(MED_EN::MED_FULL_INTERLACE));

      err = MEDcoordEcr(_medIdt, const_cast <char *> (_meshName.c_str()),
			_ptrMesh->_spaceDimension, 
			//const_cast <double *> ( _ptrMesh->_coordinate->_coordinate->get(MED_EN::MED_FULL_INTERLACE) ), 
			const_cast <double *> ( _ptrMesh->_coordinate->_coordinate.get(MED_EN::MED_FULL_INTERLACE) ), 
			med_2_3::MED_FULL_INTERLACE, _ptrMesh->_numberOfNodes,
			//  _ptrMesh->_coordinate->_numberOfNodes
			rep, const_cast <char *> (tmp_name.c_str()), 
			const_cast <char *> (tmp_unit.c_str()));

      if (err != MED_VALID) 
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write coordinates of the grid |" << _meshName.c_str() << "| in file |" << _fileName
				     << "| with dimension |"  << _ptrMesh->_spaceDimension <<"| and" 
				     << " with units names |"  << tmp_name
				     << "| and units |"       << tmp_unit
				     << " |")) ;

//CCRT      int* structure = new int [meshDimension];
      med_2_3::med_int* structure = new med_2_3::med_int [meshDimension];

      for (int idim = 0; idim < meshDimension; ++idim)
	structure[idim] = ArrayLen [idim];
 

      err = med_2_3::MEDstructureCoordEcr(_medIdt, const_cast <char *>
					 (_meshName.c_str()), meshDimension,
					 structure);

      if (err != MED_VALID)
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"error in writing the structure of the grid |" << _meshName.c_str()));

      delete structure;
    }
  else if ((gridType == MED_EN::MED_CARTESIAN) ||
	   (gridType == MED_EN::MED_POLAR))
    {
      // Write Arrays of Cartesian or Polar Grid

      double * Array[] = { ptrGrid->_iArray,
			   ptrGrid->_jArray,
			   ptrGrid->_kArray };

      for (int idim = 0; idim < _ptrMesh->_meshDimension; ++idim)
	{
	  string str_name = string (tmp_name,idim*MED_TAILLE_PNOM22,
				    MED_TAILLE_PNOM22);
	  string str_unit = string (tmp_unit,idim*MED_TAILLE_PNOM22,
				    MED_TAILLE_PNOM22);

	  err = med_2_3::MEDindicesCoordEcr(_medIdt, const_cast <char *>
					   (_meshName.c_str()),
					   _ptrMesh->_meshDimension,
					   Array[idim], ArrayLen[idim],
					   (idim+1), const_cast <char *>
					   (str_name.c_str()),
					   const_cast <char *>
					   (str_unit.c_str()));

	  if (err != MED_VALID)
	    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<
					 "Can't write grid coordinates for " <<
					 idim << "-th dimention"));
	}
  } // end Write  Cartesian or Polar Grid

  END_OF();
  return MED_VALID;
}

//=======================================================================
//function : writeCoordinates
//purpose  : 
//=======================================================================

int MED_MESH_WRONLY_DRIVER22::writeCoordinates() const {
 
  const char * LOC = "int MED_MESH_WRONLY_DRIVER22::writeCoordinates() const : ";
  BEGIN_OF(LOC);

  med_2_3::med_err err = MED_ERROR;
  med_2_3::med_repere rep;
  string tmp_name(_ptrMesh->_spaceDimension*MED_TAILLE_PNOM22,' ');
  string tmp_unit(_ptrMesh->_spaceDimension*MED_TAILLE_PNOM22,' ');
    
  // Recompose the <_spaceDimension> strings in 1 string 
  int lengthString ;
  string valueString ;
  for (int i=0;i<_ptrMesh->_spaceDimension;i++) {
    valueString = _ptrMesh->_coordinate->_coordinateName[i] ;
    lengthString = (MED_TAILLE_PNOM22<valueString.size())?MED_TAILLE_PNOM22:valueString.size() ;
    tmp_name.replace(i*MED_TAILLE_PNOM22,i*MED_TAILLE_PNOM22+lengthString,valueString,0,lengthString);
    valueString = _ptrMesh->_coordinate->_coordinateUnit[i];
    lengthString = (MED_TAILLE_PNOM22<valueString.size())?MED_TAILLE_PNOM22:valueString.size() ;
    tmp_unit.replace(i*MED_TAILLE_PNOM22,i*MED_TAILLE_PNOM22+lengthString,valueString,0,lengthString);
  }

  // Test if the mesh <_meshName> already exists
  // If it doesn't exists create it
  // If it already exists verify if its space and mesh dimensions are the same
  // as <_ptrMesh->_spaceDimension>, <_ptrMesh->_meshDimension> respectively
  // rem : <_meshName> is the driver meshName not <ptrMesh->_meshName>

  int spaceDimension = med_2_3::MEDdimEspaceLire(_medIdt, const_cast <char *>
						(_meshName.c_str()));

  int meshDimension = med_2_3::MEDdimLire(_medIdt, const_cast <char *>
					 (_meshName.c_str()) );

  SCRUTE(spaceDimension);
  SCRUTE(meshDimension);
  SCRUTE(_ptrMesh->_spaceDimension);
  SCRUTE(_ptrMesh->_meshDimension);

  if ((spaceDimension != MED_VALID) && (meshDimension < MED_VALID))
    {
      err = MEDmaaCr(_medIdt, const_cast <char *> (_meshName.c_str()),
		     _ptrMesh->_meshDimension, med_2_3::MED_NON_STRUCTURE,
		     const_cast <char *> (_ptrMesh->_description.c_str()));

      if (err < MED_VALID)
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Unable to create Mesh : |" << _meshName << "|"));
      else 
	MESSAGE(LOC<<"Mesh "<<_meshName<<" created in file "<<_fileName<<" !");

      // PAL14544. Write _spaceDimension if _spaceDimension != _meshDimension
      if ( _ptrMesh->_spaceDimension != _ptrMesh->_meshDimension )
      {
        err = med_2_3::MEDdimEspaceCr(_medIdt, const_cast <char *> (_meshName.c_str()),
                                      _ptrMesh->_spaceDimension);
        if (err < MED_VALID)
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't write spaceDimension of Mesh : |"
                                       << _meshName << "|"));
      }
    }
  else if ((spaceDimension != _ptrMesh->_spaceDimension)  &&
	   (meshDimension != _ptrMesh->_meshDimension))
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Mesh |" << _meshName.c_str() <<
				 "| already exists in file |" << _fileName <<
				 "| with space dimension |" << spaceDimension <<
				 "| and mesh dimension |" << meshDimension <<
				 "| but the space dimension and the mesh dimension of the mesh we want to write are respectively |"
				 << _ptrMesh->_spaceDimension <<"|" <<
				 _ptrMesh->_meshDimension << "|")) ;
    
  // Pourquoi le stocker sous forme de chaîne ?
  const string & coordinateSystem = _ptrMesh->_coordinate->_coordinateSystem;
  if      (coordinateSystem  == "CARTESIAN") 
    rep = med_2_3::MED_CART;
  else if ( coordinateSystem == "CYLINDRICAL")
    rep = med_2_3::MED_CYL;
  else if ( coordinateSystem == "SPHERICAL" )
    rep = med_2_3::MED_SPHER;
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Mesh |" << _meshName.c_str() << "| doesn't have a valid coordinate system : |" 
				 << _ptrMesh->_coordinate->_coordinateSystem
				 << "|" )) ;  
      
//   err = MEDcoordEcr(_medIdt, const_cast <char *> (_meshName.c_str()),
// 		    _ptrMesh->_spaceDimension, 
// 		    //const_cast <double *> ( _ptrMesh->_coordinate->_coordinate->get(MED_EN::MED_FULL_INTERLACE) ), 
// 		    const_cast <double *> ( _ptrMesh->_coordinate->_coordinate.get(MED_EN::MED_FULL_INTERLACE) ), 
// 		    MED_FR::MED_FULL_INTERLACE, 
// 		    _ptrMesh->_numberOfNodes,                 //  _ptrMesh->_coordinate->_numberOfNodes
// 		    MED_FR::MED_LECTURE_ECRITURE,    
// 		    rep,
// 		    const_cast <char *> (tmp_name.c_str()), 
// 		    const_cast <char *> (tmp_unit.c_str()) 
// 		    );

  err = MEDcoordEcr(_medIdt, const_cast <char *> (_meshName.c_str()),
		    _ptrMesh->_spaceDimension, 
		    //const_cast <double *> ( _ptrMesh->_coordinate->_coordinate->get(MED_EN::MED_FULL_INTERLACE) ), 
		    const_cast <double *> ( _ptrMesh->_coordinate->_coordinate.get(MED_EN::MED_FULL_INTERLACE) ), 
		    med_2_3::MED_FULL_INTERLACE, _ptrMesh->_numberOfNodes,
		    //  _ptrMesh->_coordinate->_numberOfNodes
		    rep, const_cast <char *> (tmp_name.c_str()), 
		    const_cast <char *> (tmp_unit.c_str()));

  if (err<0) 
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write coordinates of mesh |" << _meshName.c_str() << "| in file |" << _fileName
				 << "| with dimension |"  << _ptrMesh->_spaceDimension <<"| and" 
				 << " with units names |"  << tmp_name
				 << "| and units |"       << tmp_unit
				 << " |")) ;    
    

       //////////////////////////////////////////////////////////////////////////////////////
       ///  Modification pour prise en compte de la numérotation optionnelle des noeuds   ///
       //////////////////////////////////////////////////////////////////////////////////////
       ///
       /// Ecrit les numéros optionnels des noeuds
       /// Le traitement est identique à la version originelle s'il n'y a pas de numérotation optionnelle


      if (_ptrMesh->_arePresentOptionnalNodesNumbers==1) {
        
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
        const int * NodesNumbers = _ptrMesh->_coordinate->getNodesNumbers() ;
        med_2_3::med_int * tmp_NodesNumbers = new med_2_3::med_int[_ptrMesh->_numberOfNodes] ;
        int ii ;
        for ( ii = 0 ; ii < _ptrMesh->_numberOfNodes ; ii++ )
           tmp_NodesNumbers[ii] = NodesNumbers[ii] ;
        err =  MEDnumEcr(_medIdt, const_cast <char *> (_meshName.c_str()),
                         tmp_NodesNumbers , 
                         _ptrMesh->_numberOfNodes, med_2_3::MED_NOEUD,
			 med_2_3::med_geometrie_element(0) );
        delete [] tmp_NodesNumbers ;
#else
        err =  MEDnumEcr(_medIdt, const_cast <char *> (_meshName.c_str()),
                         const_cast <med_int *> (_ptrMesh->_coordinate->getNodesNumbers() ), 
                         _ptrMesh->_numberOfNodes, med_2_3::MED_NOEUD,
			 med_2_3::med_geometrie_element(0) );
#endif

        if (err != MED_VALID) 
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write optionnal numbers of mesh |" << 
				       _meshName.c_str() << "| in file |" <<
				       _fileName  << " |")) ;
      }
      //////////////////////////////////////////////////////////////////////////////////////

  END_OF();
    
  return MED_VALID;
}




int MED_MESH_WRONLY_DRIVER22::writeConnectivities(medEntityMesh entity) const 
{
  const char * LOC="int MED_MESH_WRONLY_DRIVER22::writeConnectivities() const : ";
  BEGIN_OF(LOC);

  med_2_3::med_err err;
  
  // REM SI LA METHODE EST APPELEE DIRECTEMENT ET QUE LE MAILLAGE N'EST PAS CREE IL Y A PB !
  // PG : IMPOSSIBLE : LA METHODE EST PRIVEE !
    
  // A FAIRE : A tester surtout dans les methodes de MESH.
  //    if ( _ptrMesh->_connectivity == (CONNECTIVITY *) MED_INVALID )
  if ( _ptrMesh->_connectivity == (CONNECTIVITY *) NULL )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "The connectivity is not defined in the MESH object ")) ;


  // Nodal connectivity for classic geometric types
  if ( _ptrMesh->existConnectivity(MED_NODAL,entity) )
    {
      int numberOfTypes = _ptrMesh->getNumberOfTypes(entity);
      const medGeometryElement * types = _ptrMesh->getTypes(entity);

      for (int i=0; i<numberOfTypes; i++)
	{
	  int numberOfElements = _ptrMesh->getNumberOfElements(entity,types[i]);
	  const int * connectivity = _ptrMesh->getConnectivity(MED_EN::MED_FULL_INTERLACE,
                                                               MED_NODAL, entity, types[i]); // ?? et SI MED_NODAL n'existe pas, recalcul auto ??

	  // Pour l'instant la class utilise le multi.....
	  int multi = 0;
	  //	  if (entity==MED_EN::MED_CELL)
	  //	    if ( (types[i]/ 100) < _ptrMesh->_spaceDimension)
	  //	      multi=1;
	  int numberOfNodes = types[i]%100;
//CCRT	  int * connectivityArray = new int[numberOfElements*(numberOfNodes+multi)];
	  med_2_3::med_int * connectivityArray = new med_2_3::med_int[numberOfElements*(numberOfNodes+multi)];

	  // version originale sans prise en compte des numéros optionnels
	  //
	  for (int j=0 ; j<numberOfElements; j++)
	    {
	      for (int k=0; k<numberOfNodes; k++)
		connectivityArray[j*(numberOfNodes+multi)+k] = connectivity[j*numberOfNodes+k];

	      if (multi>0) connectivityArray[j*(numberOfNodes+multi)+numberOfNodes] = 0;
	    }

	  //////////////////////////////////////////////////////////////////////////////////////
	  ///  Modification pour prise en compte de la numérotation optionnelle des noeuds  ///
          //////////////////////////////////////////////////////////////////////////////////////
          ///
          /// Dénumérote les sommets des mailles pour leur rendre leurs numéros optionnels
          /// Le traitement est identique à la version originelle s'il n'y a pas de numérotation optionnelle

          //if (_ptrMesh->_arePresentOptionnalNodesNumbers==1)
          //{
          //  const int * nodesNumbers = _ptrMesh->_coordinate->getNodesNumbers();
          //  for (int j=0 ; j<numberOfElements; j++) 
          //  {
          //    for (int k=0; k<numberOfNodes; k++)
          //      connectivityArray[j*(numberOfNodes+multi)+k] =
          //        nodesNumbers[connectivity[j*numberOfNodes+k]-1];
          //    if (multi>0) connectivityArray[j*(numberOfNodes+multi)+numberOfNodes] = 0;
          //  }
          //}
          //else
          //{
          //  for (int j=0 ; j<numberOfElements; j++)
          //  {
          //    for (int k=0; k<numberOfNodes; k++)
          //      connectivityArray[j*(numberOfNodes+multi)+k] = connectivity[j*numberOfNodes+k];
          //    if (multi>0) connectivityArray[j*(numberOfNodes+multi)+numberOfNodes]=0;
          //  }
          //}

          //////////////////////////////////////////////////////////////////////////////////////

          //err = MEDconnEcr(_medIdt, const_cast <char *> (_meshName.c_str()),
          //                 _ptrMesh->_spaceDimension, connectivityArray,
          //                 MED_FR::MED_FULL_INTERLACE, numberOfElements,
          //                 MED_FR::MED_LECTURE_ECRITURE,
          //                 (MED_FR::med_entite_maillage  ) entity,
          //                 (MED_FR::med_geometrie_element) types[i], MED_FR::MED_NOD);

          //err = MEDconnEcr(_medIdt, const_cast <char *> (_meshName.c_str()),
          //                 _ptrMesh->_spaceDimension, connectivityArray,
          //                 MED_FR::MED_FULL_INTERLACE, numberOfElements,
          //                 (MED_FR::med_entite_maillage  ) entity,
          //                 (MED_FR::med_geometrie_element) types[i], med_2_3::MED_NOD);

          err = MEDconnEcr(_medIdt, const_cast <char *> (_meshName.c_str()),
                           _ptrMesh->_spaceDimension, connectivityArray,
                           med_2_3::MED_FULL_INTERLACE, numberOfElements,
                           //(med_2_3::med_entite_maillage) entity, because Med Memory works only in Nodal connectivity
                           (med_2_3::med_entite_maillage  ) MED_CELL,
                           (med_2_3::med_geometrie_element) types[i], med_2_3::MED_NOD);
          err = med_2_3::MEDdimEspaceCr
            (_medIdt, const_cast<char *>(_meshName.c_str()), _ptrMesh->_spaceDimension);

          delete[] connectivityArray;

          if (err<0) // ETENDRE LES EXPLICATIONS
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write connectivities of mesh |"
                                         << _meshName.c_str() << "| in file |" << _fileName
                                         << "| with dimension |"  << _ptrMesh->_spaceDimension <<"| and" 
                                         ));
	}

    }


  // Polygons writing
  if (_ptrMesh->existPolygonsConnectivity(MED_NODAL,entity))
    {
      int nbPolygons = _ptrMesh->getNumberOfPolygons(entity);
      const int * PolygonsConnectivityIndex = _ptrMesh->getPolygonsConnectivityIndex(MED_NODAL,entity);
      const int * PolygonsConnectivity = _ptrMesh->getPolygonsConnectivity(MED_NODAL, entity);

#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
      int ii, polygonsConnectivityLength = _ptrMesh->getPolygonsConnectivityLength(MED_NODAL, entity);

      med_2_3::med_int * tmp_PolygonsConnectivityIndex = new med_2_3::med_int[nbPolygons + 1];
      for (ii = 0; ii < nbPolygons + 1; ii++)
        tmp_PolygonsConnectivityIndex[ii] = PolygonsConnectivityIndex[ii];

      med_2_3::med_int * tmp_PolygonsConnectivity = new med_2_3::med_int[polygonsConnectivityLength];
      for (ii = 0; ii < polygonsConnectivityLength; ii++)
        tmp_PolygonsConnectivity[ii] = PolygonsConnectivity[ii];

      err = MEDpolygoneConnEcr(_medIdt,
			       const_cast <char *> (_meshName.c_str()),
			       tmp_PolygonsConnectivityIndex,
			       nbPolygons + 1,
			       tmp_PolygonsConnectivity,
 			       //(med_2_3::med_entite_maillage) entity,
                               //because Med Memory works only in Nodal connectivity
			       (med_2_3::med_entite_maillage) MED_CELL,
			       med_2_3::MED_NOD);

      delete [] tmp_PolygonsConnectivityIndex;
      delete [] tmp_PolygonsConnectivity;
#else
      err = MEDpolygoneConnEcr(_medIdt,
			       const_cast <char *> (_meshName.c_str()),
			       const_cast <med_int*> (PolygonsConnectivityIndex),
			       nbPolygons + 1,
			       const_cast <med_int*> (PolygonsConnectivity),
 			       //(med_2_3::med_entite_maillage) entity,
                               //because Med Memory works only in Nodal connectivity
			       (med_2_3::med_entite_maillage) MED_CELL,
			       med_2_3::MED_NOD);
#endif

      if (err<0)
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write polygons nodal connectivities of mesh |"
                                     <<  _meshName.c_str() << "| in file |" << _fileName
                                     << "| with dimension |"  << _ptrMesh->_spaceDimension <<"| and"
                                     ));
    }


  // Polyhedron writing
  if (_ptrMesh->existPolyhedronConnectivity(MED_NODAL,entity))
    {
      //int nbPolyhedron = _ptrMesh->getNumberOfPolyhedron(entity);
      int nbPolyhedron = _ptrMesh->getNumberOfPolyhedron();
      int nbPolyhedronFaces = _ptrMesh->getNumberOfPolyhedronFaces();
      const int * PolyhedronIndex = _ptrMesh->getPolyhedronIndex(MED_NODAL);
      const int * PolyhedronFacesIndex = _ptrMesh->getPolyhedronFacesIndex();
      const int * PolyhedronConnectivity = _ptrMesh->getPolyhedronConnectivity(MED_NODAL);

#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
      int ii, polyhedronConnectivityLength = _ptrMesh->getPolyhedronConnectivityLength(MED_NODAL);

      med_2_3::med_int * tmp_PolyhedronIndex = new med_2_3::med_int[nbPolyhedron + 1];
      for (ii = 0; ii < nbPolyhedron + 1; ii++)
         tmp_PolyhedronIndex[ii] = PolyhedronIndex[ii];

      med_2_3::med_int * tmp_PolyhedronFacesIndex = new med_2_3::med_int[nbPolyhedronFaces + 1];
      for (ii = 0; ii < nbPolyhedronFaces + 1; ii++)
         tmp_PolyhedronFacesIndex[ii] = PolyhedronFacesIndex[ii];

      med_2_3::med_int * tmp_PolyhedronConnectivity = new med_2_3::med_int[polyhedronConnectivityLength];
      for (ii = 0; ii < polyhedronConnectivityLength; ii++)
         tmp_PolyhedronConnectivity[ii] = PolyhedronConnectivity[ii];

      err = MEDpolyedreConnEcr(_medIdt,
			       const_cast <char *> (_meshName.c_str()),
			       tmp_PolyhedronIndex,
			       nbPolyhedron + 1,
			       tmp_PolyhedronFacesIndex,
			       nbPolyhedronFaces + 1,
			       tmp_PolyhedronConnectivity,
			       med_2_3::MED_NOD);

      delete [] tmp_PolyhedronIndex;
      delete [] tmp_PolyhedronFacesIndex;
      delete [] tmp_PolyhedronConnectivity;
#else
      err = MEDpolyedreConnEcr(_medIdt,
			       const_cast <char *> (_meshName.c_str()),
			       const_cast <med_int*> (PolyhedronIndex),
			       nbPolyhedron + 1,
			       const_cast <med_int*> (PolyhedronFacesIndex),
			       nbPolyhedronFaces + 1,
			       const_cast <med_int*> (PolyhedronConnectivity),
			       med_2_3::MED_NOD);
#endif

      if (err<0)
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write polyhedron nodal connectivities of mesh |"
                                     << _meshName.c_str() << "| in file |" << _fileName
				     << "| with dimension |"  << _ptrMesh->_spaceDimension <<"| and"
                                     ));
    }


  // Descending connectivity for classic geometric types
  if ( _ptrMesh->existConnectivity(MED_DESCENDING,entity) )
    {
      int numberOfTypes                = _ptrMesh->getNumberOfTypes (entity) ;
      const medGeometryElement * types = _ptrMesh->getTypes         (entity) ;
      
      for (int i=0; i<numberOfTypes; i++)
	{
          int    numberOfElements = _ptrMesh->getNumberOfElements (entity,types[i]);
	  const int * connectivity = _ptrMesh->getConnectivity(MED_EN::MED_FULL_INTERLACE, MED_DESCENDING, entity, types[i]); 
      
          // Pour l'instant la class utilise le multi.....
	  //       err = MED_FR::MEDconnEcr( _medIdt,
	  // 				const_cast <char *> ( _meshName.c_str()),
	  // 				_ptrMesh->_spaceDimension,
	  // 				const_cast <int *> (connectivity),
	  // 				MED_FR::MED_FULL_INTERLACE,
	  // 				numberOfElements,
	  // 				MED_FR::MED_LECTURE_ECRITURE,
	  // 				(MED_FR::med_entite_maillage  ) entity, 
	  // 				(MED_FR::med_geometrie_element) types[i],
	  // 				MED_FR::MED_DESC );

#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
          int lgth = _ptrMesh->getConnectivityLength(MED_EN::MED_FULL_INTERLACE, MED_DESCENDING, entity, types[i]);
          med_2_3::med_int * tmp_Connectivity = new med_2_3::med_int[lgth] ;
          int ii ;
          for ( ii = 0 ; ii < lgth ; ii++ )
          tmp_Connectivity[ii] = connectivity[ii] ;
	  err = med_2_3::MEDconnEcr(_medIdt,
				    const_cast <char *> ( _meshName.c_str()),
				    _ptrMesh->_spaceDimension,
				    tmp_Connectivity,
				    med_2_3::MED_FULL_INTERLACE,
				    numberOfElements,
 				    //(med_2_3::med_entite_maillage  ) entity,
                                    //because Med Memory works only in Nodal connectivity
				    (med_2_3::med_entite_maillage  ) MED_CELL,
				    (med_2_3::med_geometrie_element) types[i],
				    med_2_3::MED_DESC);
          delete [] tmp_Connectivity ;
#else
	  err = med_2_3::MEDconnEcr(_medIdt,
				    const_cast <char *> ( _meshName.c_str()),
				    _ptrMesh->_spaceDimension,
				    const_cast <int *> (connectivity),
				    med_2_3::MED_FULL_INTERLACE,
				    numberOfElements,
 				    //(med_2_3::med_entite_maillage  ) entity,
                                    //because Med Memory works only in Nodal connectivity
				    (med_2_3::med_entite_maillage  ) MED_CELL, 
				    (med_2_3::med_geometrie_element) types[i],
				    med_2_3::MED_DESC);
#endif
	
	  if (err<0) // ETENDRE LES EXPLICATIONS
	    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write connectivities of mesh |"
                                         << _meshName.c_str() << "| in file |" << _fileName
					 << "| with dimension |"  << _ptrMesh->_spaceDimension <<"| and" 
					 )) ;
            
	}
    }


  // Polygons writing
  if (_ptrMesh->existPolygonsConnectivity(MED_DESCENDING,entity))
    {
      int nbPolygons = _ptrMesh->getNumberOfPolygons(entity);
      // ??? //int nbPolygons = _ptrMesh->getNumberOfPolygons();
      const int * PolygonsConnectivityIndex = _ptrMesh->getPolygonsConnectivityIndex(MED_DESCENDING, entity);
      const int * PolygonsConnectivity = _ptrMesh->getPolygonsConnectivity(MED_DESCENDING, entity);

#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
      int ii, polygonsConnectivityLength = _ptrMesh->getPolygonsConnectivityLength(MED_DESCENDING, entity);

      med_2_3::med_int * tmp_PolygonsConnectivityIndex = new med_2_3::med_int[nbPolygons + 1];
      for (ii = 0; ii < nbPolygons + 1; ii++)
        tmp_PolygonsConnectivityIndex[ii] = PolygonsConnectivityIndex[ii];

      med_2_3::med_int * tmp_PolygonsConnectivity = new med_2_3::med_int[polygonsConnectivityLength];
      for (ii = 0; ii < polygonsConnectivityLength; ii++)
	tmp_PolygonsConnectivity[ii] = PolygonsConnectivity[ii];

      err = MEDpolygoneConnEcr(_medIdt,
			       const_cast <char *> (_meshName.c_str()),
			       tmp_PolygonsConnectivityIndex,
			       nbPolygons + 1,
			       tmp_PolygonsConnectivity,
 			       //(med_2_3::med_entite_maillage) entity,
                               //because Med Memory works only in Nodal connectivity
			       (med_2_3::med_entite_maillage) MED_CELL,
			       med_2_3::MED_DESC);

      delete [] tmp_PolygonsConnectivityIndex;
      delete [] tmp_PolygonsConnectivity;
#else
      err = MEDpolygoneConnEcr(_medIdt,
			       const_cast <char *> (_meshName.c_str()),
			       const_cast <med_int*> (PolygonsConnectivityIndex),
			       nbPolygons + 1,
			       const_cast <med_int*> (PolygonsConnectivity),
 			       //(med_2_3::med_entite_maillage) entity,
                               //because Med Memory works only in Nodal connectivity
			       (med_2_3::med_entite_maillage) entity,
			       med_2_3::MED_DESC);
#endif

      if (err<0)
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write polygons descending connectivities of mesh |"
                                     <<  _meshName.c_str() << "| in file |" << _fileName
                                     << "| with dimension |"  << _ptrMesh->_spaceDimension <<"| and"
                                     ));
    }


  // Polyhedron writing
  if (_ptrMesh->existPolyhedronConnectivity(MED_DESCENDING, entity))
    {
      med_2_3::med_int nbPolyhedron = _ptrMesh->getNumberOfPolyhedron();
      med_2_3::med_int NumberOfFaces = _ptrMesh->getNumberOfPolyhedronFaces();
      // ??? //_ptrMesh->getPolyhedronIndex(MED_DESCENDING)[nbPolyhedron] - 1;
      // by default all polyhedron faces are polygons
      vector<med_2_3::med_int> FacesGeometricTypes (NumberOfFaces, MED_POLYGON);
      const int * PolyhedronIndex = _ptrMesh->getPolyhedronIndex(MED_DESCENDING);
      const int * PolyhedronConnectivity = _ptrMesh->getPolyhedronConnectivity(MED_DESCENDING);

#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
      int ii, polyhedronConnectivityLength = _ptrMesh->getPolyhedronConnectivityLength(MED_DESCENDING);

      med_2_3::med_int * tmp_PolyhedronIndex = new med_2_3::med_int[nbPolyhedron + 1];
      for (ii = 0; ii < nbPolyhedron + 1; ii++)
         tmp_PolyhedronIndex[ii] = PolyhedronIndex[ii];

      med_2_3::med_int * tmp_PolyhedronConnectivity = new med_2_3::med_int[polyhedronConnectivityLength];
      for (ii = 0; ii < polyhedronConnectivityLength; ii++)
         tmp_PolyhedronConnectivity[ii] = PolyhedronConnectivity[ii];

      err = MEDpolyedreConnEcr(_medIdt,
			       const_cast <char *> (_meshName.c_str()),
			       tmp_PolyhedronIndex,
			       nbPolyhedron + 1,
			       &FacesGeometricTypes[0],
			       NumberOfFaces,
			       tmp_PolyhedronConnectivity,
			       med_2_3::MED_DESC);

      delete [] tmp_PolyhedronIndex;
      delete [] tmp_PolyhedronConnectivity;
#else
      err = MEDpolyedreConnEcr(_medIdt,
			       const_cast <char *> (_meshName.c_str()),
			       const_cast <med_int*> (PolyhedronIndex),
			       nbPolyhedron + 1,
			       &FacesGeometricTypes[0],
			       NumberOfFaces,
			       const_cast <med_int*> (PolyhedronConnectivity),
			       med_2_3::MED_DESC);
#endif

      if (err<0)
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write polyhedron descending connectivities of mesh |"
                                     << _meshName.c_str() << "| in file |" << _fileName
                                     << "| with dimension |"  << _ptrMesh->_spaceDimension <<"| and" 
                                     ));
    }


  END_OF();
  return MED_VALID;
}

/*!creates a vector of families from a vector of groups
 * 
 * \param myGroups input : vector of groups
 * \param myFamilies output vector of families
 * 
 * Routine is meant to be called to reconstruct families from existing groups
 * typically for writing meshes created by MEDMEM::MESHING containing overlapping groups 
 * 
 * if the groups do not overlap, the vector of families will have the same size
 * as the vector of groups
 * otherwise, the size of the family vector will be larger
*/	

void MED_MESH_WRONLY_DRIVER22::groupFamilyConverter(const vector <GROUP*>& myGroups, vector <FAMILY*>& myFamilies ) const 
{

	BEGIN_OF("MED_MESH_WRONLY_DRIVER::groupFamilyConverter");
  if (myGroups.empty()) return;
	
//	if (!myFamilies.empty())
//      throw MEDEXCEPTION(LOCALIZED("Family vector must be empty on call "
//      << "to groupFamilyConverter"));
// 
 //mapping elements to all the groups containing it
 	std::multimap <int,int> elem2groups;
 	for (int igroup=0; igroup< myGroups.size(); igroup++)
 	{
 		// if the support is on all Mesh elements
 		//all the items from 1 to nb are in the group
 		if (myGroups[igroup]->isOnAllElements())
 		{
 			for (int ielem = 0; ielem< myGroups[igroup]->getNumberOfElements(MED_ALL_ELEMENTS); ielem++)
 			{
 				elem2groups.insert(make_pair(ielem+1, igroup));
 			}
 		}
 		//otherwise getNumber() gives the appropriate number of items
 		else
 		{
 			const int*Number = myGroups[igroup]->getNumber(MED_ALL_ELEMENTS) ;
 			for (int ielem = 0; ielem< myGroups[igroup]->getNumberOfElements(MED_ALL_ELEMENTS); ielem++)
 			{
 				elem2groups.insert(make_pair(Number[ielem], igroup));
 			}
 		}
 	}
 	
 	//creating a set of signatures for the groups intersections
 	std::multimap<vector<int>,int> signatures;
 	
 	typedef multimap<int,int>::iterator MI;
 	MI iter=elem2groups.begin();
 	
 	while (iter!=elem2groups.end())
 	{
 		vector<int> sign (1, iter -> second );
 		int key = iter -> first;
 		iter ++;
 		while (iter!=elem2groups.end()&&iter-> first == key)
 		{
 			sign.push_back(iter -> second);
 			iter++;
 		}
 		signatures.insert(make_pair(sign,key));
 	}
 	
 	elem2groups.clear();
 	
 	//creating the families from the signatures mapping
 	//each signature will correspond to a new family
 	std::multimap<vector<int>,int>::const_iterator iter_signatures = signatures.begin();
 	
 	//retrieving the entity type (all the groups have the same)
 	// node families are numbered above 0
 	// cell families are numbered from  -1 to -MAX_NB_GROUPS
 	// face falimies are numbered from -MAX_NB_GROUPS-1 to -2*MAX_NB_GROUPS
 	// edge families are numbered from -2*MAX_NB_GROUPS to -3*MAX_NB_GROUPS
 	// MAX_NB_GROUPS is defined in MEDMEM_define.hxx
 	
 	medEntityMesh entity = myGroups[0]->getEntity();
 	MESH* mesh=myGroups[0]->getMesh();
 	
 	int ifamily;
 	switch (entity)
 	{
 		case MED_NODE:
 			ifamily=1;
 			break;
 		case MED_CELL:
 			ifamily=-MAX_NB_GROUP;
 			break;
 		case MED_FACE:
 			ifamily=-2*MAX_NB_GROUP;
 			break;
 		case MED_EDGE:
 			 ifamily=-3*MAX_NB_GROUP;
 			break;
 	}

	//browsing signatures to build all the families
	//each signature corresponds to a new family

 	while (iter_signatures!= signatures.end())
 	{
 		const vector<int>& key= iter_signatures->first;
 		int size = signatures.count(key);
 		
 		list<int> numbers;

 		for (int i=0; i< size; i++)
 		{
 			numbers.push_back(iter_signatures->second);
 			iter_signatures++;
 		}
 		
 		//TODO : see if build SupportOnElementFromElementList could not be built from another container

		// for nodes, the family is built directly from the node list
		// for elements, it is built from the buildSupportOnElementsFromElementList
		//               which allocates a support
		FAMILY* myFamily;
 		if (entity!=MED_NODE)
		  {	
		    SUPPORT* support;
		    support=mesh->buildSupportOnElementsFromElementList(numbers, entity);
		    myFamily=new FAMILY(*support);
        delete support;
		  }
	 	else
		  {
		    myFamily= new FAMILY();
		    myFamily->setMesh(mesh);
		    myFamily->fillFromNodeList(numbers);
		  }
        
        
		// the identifier and the groups are set
 		myFamily->setIdentifier(ifamily);
 		myFamily->setNumberOfGroups(key.size());
		char family_name[MED_TAILLE_NOM];

		//if the family has one group to which only one family
		//is associated, the name of the family underlying the group
		//is given back to the family
		if (key.size()==1 && myGroups[key[0]]->getNumberOfFamilies()==0)
			{
				vector<FAMILY*> families;
				families.push_back(myFamily);
				myGroups[key[0]]->setFamilies(families);
				//it is necessary to use strncpy because group and family
				//have different name sizes
				strncpy(family_name,myGroups[key[0]]->getName().c_str(),MED_TAILLE_NOM);
				family_name[MED_TAILLE_NOM-1]='\0';
			}
		else
			sprintf(family_name,"family%d",ifamily);
	
		myFamily->setName( healName( family_name ));

 		string* groupnames=new string[key.size()];
 		//myFamily->getGroupsNames();
 		//groupnames.set(key.size());

 		for (int igroup=0; igroup<key.size(); igroup++)
 		{
		  groupnames[igroup]=myGroups[key[igroup]]->getName();
 		}	
 		
		myFamily->setGroupsNames(groupnames,true);
	  
 		myFamilies.push_back(myFamily);
 		ifamily++;
 	}

	//adding empty families corresponding to empty groups
	for (int i=0; i<myGroups.size(); i++)
		{
			if (myGroups[i]->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)==0)
				{
					FAMILY* myFamily=new FAMILY(*(myGroups[i]));
					char family_name[MED_TAILLE_NOM];
					//it is necessary to use strncpy because group and family
					//have different name sizes
					strncpy(family_name,myGroups[i]->getName().c_str(),MED_TAILLE_NOM);
					family_name[MED_TAILLE_NOM-1]='\0';
					myFamily->setName( healName( family_name ));
					myFamily->setIdentifier(ifamily);
					ifamily++;
					vector<string> groupnames;
					groupnames.push_back(myGroups[i]->getName());
					myFamily->setGroupsNames(&groupnames[0]);
					myFamilies.push_back(myFamily);
				}
		}
	END_OF();
}

int MED_MESH_WRONLY_DRIVER22::writeFamilyNumbers() const {
  
  const char * LOC="int MED_MESH_WRONLY_DRIVER22::writeFamilyNumbers() const : ";
  BEGIN_OF(LOC);

  med_2_3::med_err err;
  
  // SOLUTION TEMPORAIRE CAR _ptrMesh->_MEDArrayNodeFamily DOIT ETRE ENLEVER DE LA CLASSE MESH

  { // Node related block
    
    // We build the array from the families list objects :
    int NumberOfNodes = _ptrMesh->getNumberOfNodes() ;
//CCRT    int * MEDArrayNodeFamily = new int[NumberOfNodes] ;
    med_2_3::med_int * MEDArrayNodeFamily = new med_2_3::med_int[NumberOfNodes] ;
    // family 0 by default
    for (int i=0; i<NumberOfNodes; i++)
      MEDArrayNodeFamily[i]=0;
    //vector<FAMILY*> myFamilies = _ptrMesh->getFamilies(MED_NODE);
    vector<FAMILY*> * myFamilies = &_ptrMesh->_familyNode;
    int NumberOfNodesFamilies = myFamilies->size() ;
   // bool ToDestroy = false;
    if (0 == NumberOfNodesFamilies) {
   //   ToDestroy = true ;
      vector<GROUP*> myGroups = _ptrMesh->getGroups(MED_NODE);
      
      groupFamilyConverter(myGroups,*myFamilies);
		
      NumberOfNodesFamilies=myFamilies->size() ;
    }
    for (int i=0 ; i<NumberOfNodesFamilies; i++) {
      //SCRUTE(i);
      //SCRUTE(myFamilies[i]->getName());
      int FamilyIdentifier = (*myFamilies)[i]->getIdentifier() ;
      int TotalNumber = (*myFamilies)[i]->getNumberOfElements(MED_ALL_ELEMENTS) ;
      if ((*myFamilies)[i]->isOnAllElements())
	for (int j=0; j<TotalNumber; j++)
	  MEDArrayNodeFamily[j]=FamilyIdentifier;
      else {
	const int * Number = (*myFamilies)[i]->getNumber(MED_ALL_ELEMENTS) ;
	for (int j=0; j<TotalNumber; j++)
	  MEDArrayNodeFamily[Number[j]-1]=FamilyIdentifier ;
      }
    }

//     for(int j=0; j<NumberOfNodes; j++) {
//       SCRUTE(MEDArrayNodeFamily[j]);
//     }

//     if ( !_ptrMesh->getIsAGrid() )

      err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
		    MEDArrayNodeFamily, NumberOfNodes, med_2_3::MED_NOEUD,
		    (med_2_3::med_geometrie_element) MED_NONE);

//     else
//       err = MEDfamGridEcr(_medIdt,
// 			  const_cast <char *> (_ptrMesh->_name.c_str()),
// 			  MEDArrayNodeFamily,
// 			  NumberOfNodes,
// 			  MED_FR::MED_LECTURE_ECRITURE,
// 			  MED_FR::MED_NOEUD);

    if ( err != MED_VALID) 
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't write node family for the |"<< NumberOfNodes
				   << "| nodes in mesh |" 
				   << _ptrMesh->_name.c_str() << "|" ));
    delete[] MEDArrayNodeFamily;
//if (true == ToDestroy)
//  for (int i=0; i<NumberOfNodesFamilies; i++)
//	  delete (*myFamilies)[i];
  }
    
  { // CELLS RELATED BLOCK
    medEntityMesh entity=MED_EN::MED_CELL;
    // SOLUTION TEMPORAIRE CAR _ptrMesh->_MEDArray____Family DOIT ETRE ENLEVER DE LA CLASSE MESH
    if ((_ptrMesh->getIsAGrid()) || // PAL18712, GRID::existConnectivity() calls GRID::fillConnectivity() but it is not needed for writing GRID
        (_ptrMesh->existConnectivityWithPoly(MED_NODAL,entity)) ||
        (_ptrMesh->existConnectivityWithPoly(MED_DESCENDING,entity)))
    {
      int numberOfTypes           = _ptrMesh->getNumberOfTypesWithPoly(entity) ;
      medGeometryElement  * types = _ptrMesh->getTypesWithPoly(entity) ;

      // We build the array from the families list objects :
      //int NumberOfElements = _ptrMesh->getNumberOfElements(entity, MED_ALL_ELEMENTS);
      int NumberOfElements = _ptrMesh->getNumberOfElementsWithPoly(entity, MED_ALL_ELEMENTS);
      int * MEDArrayFamily = new int[NumberOfElements] ;
      // family 0 by default
      for (int i=0; i<NumberOfElements; i++)
	MEDArrayFamily[i]=0;
      //vector<FAMILY*> myFamilies = _ptrMesh->getFamilies(entity);
      vector<FAMILY*> * myFamilies = &_ptrMesh->_familyCell ;
      int NumberOfFamilies = myFamilies->size() ;
     // bool ToDestroy = false;
      if (0 == NumberOfFamilies) {
//	ToDestroy = true ;
	vector<GROUP*> myGroups = _ptrMesh->getGroups(entity);
	groupFamilyConverter(myGroups,*myFamilies);
	NumberOfFamilies=myFamilies->size() ;
      }
      for (int i=0 ; i<NumberOfFamilies; i++) {
	int FamilyIdentifier = (*myFamilies)[i]->getIdentifier() ;
	int TotalNumber = (*myFamilies)[i]->getNumberOfElements(MED_ALL_ELEMENTS) ;
	if ((*myFamilies)[i]->isOnAllElements())
	  for (int ii=0; ii<TotalNumber; ii++)
	    MEDArrayFamily[ii]=FamilyIdentifier;
	else {
	  const int * Number = (*myFamilies)[i]->getNumber(MED_ALL_ELEMENTS) ;
	  for (int ii=0; ii<TotalNumber; ii++)
	    MEDArrayFamily[Number[ii]-1]=FamilyIdentifier ;
	}
      }



//       for (int i=0; i<NumberOfElements; i++)
// 	SCRUTE(MEDArrayFamily[i]);


      //const int * typeCount = _ptrMesh->getGlobalNumberingIndex(entity);
      int offset=0;
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
	int lgth=NumberOfElements;
	med_2_3::med_int *temp=new med_2_3::med_int[lgth];
	for(int i2=0;i2<lgth;i2++)
	  temp[i2]=(med_2_3::med_int) (MEDArrayFamily[i2]);
#endif
      for (int i=0; i<numberOfTypes; i++) {
        int typeNumberOfElements=_ptrMesh->getNumberOfElementsWithPoly(entity,types[i]);
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
	err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
			//(temp+offset),(typeCount[i+1]-typeCount[i]),
			(temp+offset), typeNumberOfElements,
			//CCRT			med_2_3::MED_REMP ,
// 			(med_2_3::med_entite_maillage) entity, because Med Memory works only in Nodal connectivity
			(med_2_3::med_entite_maillage) MED_CELL,
			(med_2_3::med_geometrie_element) types[i]);
#else
	err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
			(MEDArrayFamily+offset),
			typeNumberOfElements,
// 			(med_2_3::med_entite_maillage) entity, because Med Memory works only in Nodal connectivity
			(med_2_3::med_entite_maillage) MED_CELL,
			(med_2_3::med_geometrie_element) types[i]);
#endif
	MESSAGE("OK "<<i);
	if ( err != MED_VALID)
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't write family for the |"<< typeNumberOfElements
				       << "| cells of geometric type |" << geoNames[types[i]] <<"|in mesh |"
				       << _ptrMesh->_name.c_str() << "|" ));
	offset+=typeNumberOfElements;
      }
//CCRT Clutter
//CCRT#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
//CCRT      delete [] temp;
//CCRT#endif
      delete[] MEDArrayFamily ;
      delete[] types;
//      if (true == ToDestroy) {
//        int NumberOfFamilies = myFamilies->size();
//        for (int i=0; i<NumberOfFamilies; i++)
//          delete (*myFamilies)[i];
//      }
    }
  }

  { // FACE RELATED BLOCK
    medEntityMesh entity=MED_EN::MED_FACE;
    // SOLUTION TEMPORAIRE CAR _ptrMesh->_MEDArray____Family DOIT ETRE ENLEVER DE LA CLASSE MESH
    if ((_ptrMesh->getIsAGrid()) || // PAL18712, GRID::existConnectivity() calls GRID::fillConnectivity() but it is not needed for writing GRID
        (_ptrMesh->existConnectivityWithPoly(MED_NODAL,entity)) ||
        (_ptrMesh->existConnectivityWithPoly(MED_DESCENDING,entity)))
    {
      int numberOfTypes           = _ptrMesh->getNumberOfTypesWithPoly(entity) ;
      medGeometryElement  * types = _ptrMesh->getTypesWithPoly(entity) ;
      SCRUTE(numberOfTypes);
      
      int numberOfElements = _ptrMesh->getNumberOfElementsWithPoly(entity, MED_ALL_ELEMENTS) ;
      int * familyArray = new int[numberOfElements] ;
      for (int i=0;i<numberOfElements;i++)
	familyArray[i]=0;

      int numberOfFamilies = _ptrMesh->getNumberOfFamilies(entity) ;
      //vector<FAMILY*> myFamilies = _ptrMesh->getFamilies(entity) ;
      vector<FAMILY*> * myFamilies = &_ptrMesh->_familyFace ;
     // bool ToDestroy = false;
      if (0 == numberOfFamilies) {
	//ToDestroy = true ;
	vector<GROUP*> myGroups = _ptrMesh->getGroups(entity);

	groupFamilyConverter(myGroups,*myFamilies);
	
	numberOfFamilies=myFamilies->size() ;
      }
      for (int i=0;i<numberOfFamilies;i++) {
	int familyNumber = (*myFamilies)[i]->getIdentifier() ;
	int numberOfFamilyElements = (*myFamilies)[i]->getNumberOfElements(MED_ALL_ELEMENTS) ;
	if ((*myFamilies)[i]->isOnAllElements())
	  for (int ii=0; ii<numberOfFamilyElements; ii++)
	    familyArray[ii]=familyNumber;
	else {
	  const int * myFamilyElements = (*myFamilies)[i]->getNumber(MED_ALL_ELEMENTS) ;
	  for (int ii=0;ii<numberOfFamilyElements;ii++)
	    familyArray[myFamilyElements[ii]-1]=familyNumber;
	}
      }

      for (int i=0;i<numberOfElements;i++)
	SCRUTE(familyArray[i]);

//CCRT Clutter
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
      int lgth=numberOfElements;
      med_2_3::med_int *temp=new med_2_3::med_int[lgth];
      for(int i2=0;i2<lgth;i2++)
	temp[i2]=(med_2_3::med_int) (familyArray[i2]);
#endif
      SCRUTE(numberOfTypes);
      int offset=0;
      for (int i=0; i<numberOfTypes; i++) {
	int typeNumberOfElements = _ptrMesh->getNumberOfElementsWithPoly(entity,types[i]) ;
	SCRUTE(typeNumberOfElements);
	SCRUTE(offset);
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
	err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
			(temp+offset), typeNumberOfElements,
// 			(med_2_3::med_entite_maillage) entity, because Med Memory works only in Nodal connectivity
			(med_2_3::med_entite_maillage) MED_CELL,
			(med_2_3::med_geometrie_element) types[i]); 
#else
	MESSAGE("On est bien la !!! entity = " << entity << " type " << types[i]);

	err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
			(familyArray+offset), typeNumberOfElements,
// 			(med_2_3::med_entite_maillage) entity, because Med Memory works only in Nodal connectivity
			(med_2_3::med_entite_maillage) MED_CELL,
			(med_2_3::med_geometrie_element) types[i]);
#endif
	if ( err != MED_VALID)
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't write family for the |"<< typeNumberOfElements
				       << "| faces of geometric type |" << geoNames[types[i]] <<"|in mesh |"
				       << _ptrMesh->_name.c_str() << "|" ));
	offset+=typeNumberOfElements;
      }
//CCRT there was "temp" and "familyArray" for OSF, but only "familyArray" for Linux ...
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
      delete [] temp;
//CCRT#endif
#endif
      delete[] familyArray ;
      delete[] types;
//      if (true == ToDestroy) {
//        int NumberOfFamilies = myFamilies->size();
//          for (int i=0; i<NumberOfFamilies; i++)
//            delete (*myFamilies)[i];
//      }
    }
  }

  { // EDGE RELATED BLOCK
    //medEntityMesh entity=MED_EN::MED_FACE;
    medEntityMesh entity=MED_EN::MED_EDGE;
    // SOLUTION TEMPORAIRE CAR _ptrMesh->_MEDArray____Family DOIT ETRE ENLEVER DE LA CLASSE MESH
    if  ((_ptrMesh->getIsAGrid()) || // PAL18712, GRID::existConnectivity() calls GRID::fillConnectivity() but it is not needed for writing GRID
         ( _ptrMesh->existConnectivity(MED_NODAL,entity) ) ||
         ( _ptrMesh->existConnectivity(MED_DESCENDING,entity) ) ) { 

      int numberOfTypes           = _ptrMesh->getNumberOfTypes (entity) ;
      const medGeometryElement  * types = _ptrMesh->getTypes         (entity) ;
      
      int numberOfElements = _ptrMesh->getNumberOfElements(entity, MED_ALL_ELEMENTS) ;
      int * familyArray = new int[numberOfElements] ;
      //      med_2_3::med_int * familyArray = new int[numberOfElements] ;
      for (int i=0;i<numberOfElements;i++)
	familyArray[i]=0;

      int numberOfFamilies = _ptrMesh->getNumberOfFamilies(entity) ;
      //vector<FAMILY*> myFamilies = _ptrMesh->getFamilies(entity) ;
      vector<FAMILY*> * myFamilies = &_ptrMesh->_familyEdge ;
      //bool ToDestroy = false;
      if (0 == numberOfFamilies) {
	//ToDestroy = true ;
	vector<GROUP*> myGroups = _ptrMesh->getGroups(entity);

	groupFamilyConverter(myGroups,*myFamilies);
	
	numberOfFamilies=myFamilies->size() ;

      }

      for (int i=0;i<numberOfFamilies;i++) {
	int familyNumber = (*myFamilies)[i]->getIdentifier() ;
	int numberOfFamilyElements = (*myFamilies)[i]->getNumberOfElements(MED_ALL_ELEMENTS) ;
	if ((*myFamilies)[i]->isOnAllElements())
	  for (int ii=0; ii<numberOfFamilyElements; ii++)
	    familyArray[ii]=familyNumber;
	else {
	  const int * myFamilyElements = (*myFamilies)[i]->getNumber(MED_ALL_ELEMENTS) ;
	  for (int ii=0;ii<numberOfFamilyElements;ii++)
	    familyArray[myFamilyElements[ii]-1]=familyNumber;
	}
      }


      for (int i=0;i<numberOfElements;i++)
	SCRUTE(familyArray[i]);


      const int * typeCount = _ptrMesh->getGlobalNumberingIndex(entity) ;
//CCRT : clutter :
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
      int lgth=numberOfElements;
      med_2_3::med_int *temp=new med_2_3::med_int[lgth];
      for(int i2=0;i2<lgth;i2++)
	temp[i2]=(med_2_3::med_int) (familyArray[i2]);
#endif
      SCRUTE(numberOfTypes);

      for (int i=0; i<numberOfTypes; i++) {
	int typeNumberOfElements = typeCount[i+1] - typeCount[i] ;
	SCRUTE(typeNumberOfElements);
	SCRUTE(typeCount[i+1]);
	SCRUTE(typeCount[i]);
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
	err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
			(temp+(typeCount[i]-1)), typeNumberOfElements,
// 			(med_2_3::med_entite_maillage) entity, because Med Memory works only in Nodal connectivity
			(med_2_3::med_entite_maillage) MED_CELL,
			(med_2_3::med_geometrie_element) types[i]); 
#else
	err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
			(familyArray+(typeCount[i]-1)), typeNumberOfElements,
// 			(med_2_3::med_entite_maillage) entity, because Med Memory works only in Nodal connectivity
			(med_2_3::med_entite_maillage) MED_CELL,
			(med_2_3::med_geometrie_element) types[i]);
#endif
	if ( err != MED_VALID)
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't write family for the |"<< _ptrMesh->getNumberOfElements(entity, types[i])
				       << "| edges of geometric type |" << geoNames[types[i]] <<"|in mesh |"
				       << _ptrMesh->_name.c_str() << "|" ));
      }

#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
      delete [] temp;
#endif
      delete[] familyArray ;
//      if (true == ToDestroy) {
//        int NumberOfFamilies = myFamilies->size();
//        for (int i=0; i<NumberOfFamilies; i++)
//          delete (*myFamilies)[i];
//      }
    }
  }
    
  END_OF();
  return MED_VALID;
}

int MED_MESH_WRONLY_DRIVER22::writeFamilies(vector<FAMILY*> & families ) const
{

  const char * LOC="int MED_MESH_WRONLY_DRIVER22::writeFamilies(vector<FAMILY*> families) const : ";
  BEGIN_OF(LOC);

  med_2_3::med_err err;

  MESSAGE(LOC<<" families.size() :"<<families.size());

  for (unsigned int i=0; i< families.size(); i++) {

    int      numberOfAttributes = families[i]->getNumberOfAttributes ();
    string   attributesDescriptions (numberOfAttributes*MED_TAILLE_DESC,'\0');

    // Recompose the attributes descriptions arg for MED
    for (int j=0; j < numberOfAttributes; j++)
    {
      string attributeDescription = families[i]->getAttributeDescription(j+1);

      if ( attributeDescription.size() > MED_TAILLE_DESC )
	throw MEDEXCEPTION( LOCALIZED(STRING(LOC) << "The size of the attribute description n° |"
                                      << j+1 << "| of the family |" << families[i]->getName()
				      << "| with identifier |" << families[i]->getIdentifier()
                                      << "| is |" << attributeDescription.size()
                                      <<"| and is more than |" <<  MED_TAILLE_DESC << "|"));

      int length = min(MED_TAILLE_LNOM,(int)attributeDescription.size());
      attributesDescriptions.replace(j*MED_TAILLE_DESC,length, attributeDescription,0,length);
    }


    int      numberOfGroups  = families[i]->getNumberOfGroups();
    string   groupsNames(numberOfGroups*MED_TAILLE_LNOM,'\0');

    // Recompose the groups names arg for MED
    for (int j=0; j < numberOfGroups; j++)
    {
      string groupName = families[i]->getGroupName(j+1);

      if ( groupName.size() > MED_TAILLE_LNOM )
	throw MEDEXCEPTION( LOCALIZED(STRING(LOC) << "The size of the group name  n° |"
                                      << j+1 << "| of the family |" << families[i]->getName()
				      << "| with identifier |" << families[i]->getIdentifier()
                                      << "| is |" << groupName.size()
                                      <<"| and is more than |" << MED_TAILLE_LNOM << "|"));

      int length = min(MED_TAILLE_LNOM,(int)groupName.size());
      groupsNames.replace(j*MED_TAILLE_LNOM,length, groupName,0,length);
    }

    // test if the family already exists (HDF trick waiting a MED evolution to be replaced)

    string dataGroupFam;
    if (families[i]->getEntity() == MED_NODE)
      dataGroupFam = "/ENS_MAA/"+_meshName+"/FAS/NOEUD/"+families[i]->getName()+"/";  
    else
      dataGroupFam = "/ENS_MAA/"+_meshName+"/FAS/ELEME/"+families[i]->getName()+"/"; 

    SCRUTE("|"<<dataGroupFam<<"|");
    err = med_2_3::_MEDdatagroupOuvrir(_medIdt,const_cast <char *> (dataGroupFam.c_str()) ) ;
    if ( err < MED_VALID ) {
      SCRUTE(err);
      if ( families[i]->getName().size() > MED_TAILLE_NOM )
	throw MEDEXCEPTION
          ( LOCALIZED(STRING(LOC) << "The size of the name of the family |" << i+1
                      << "| |" << families[i]->getName()
                      << "| with identifier |" << families[i]->getIdentifier()  << "| is |" 
                      <<  families[i]->getName().size()  <<"| and is more than |"
                      << MED_TAILLE_NOM << "|")) ;

      MESSAGE(LOC<<"families[i]->getName().c_str() : "<<families[i]->getName().c_str());
      MESSAGE(LOC<<"_meshName.c_str() : "<<_meshName.c_str());
      MESSAGE(LOC<<"families[i]->getIdentifier() : "<<families[i]->getIdentifier());
      MESSAGE(LOC<<"numberOfAttributes : "<<numberOfAttributes);
	
      //MESSAGE(LOC<<"families[i]->getAttributesIdentifiers() : "<<families[i]->getAttributesIdentifiers()[0]);
      //MESSAGE(LOC<<"families[i]->getAttributesValues() : "<<families[i]->getAttributesValues()[0]);
      MESSAGE(LOC<<"attributesDescriptions.c_str() : "<<attributesDescriptions.c_str());
      MESSAGE(LOC<<"numberOfGroups : "<<numberOfGroups);
      MESSAGE(LOC<<"groupsNames.c_str() : "<<groupsNames.c_str());
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
      int lgth=families[i]->getNumberOfAttributes();
      med_2_3::med_int *  AttributesIdentifier2 = new med_2_3::med_int[lgth] ;
      med_2_3::med_int *  AttributesValues2     = new med_2_3::med_int[lgth] ;
      for(med_2_3::med_int i2=0;i2<lgth;i2++)
	{
	  AttributesIdentifier2[i2]=(med_2_3::med_int)(families[i]->getAttributesIdentifiers()[i2]);
	  AttributesValues2[i2]=(med_2_3::med_int)(families[i]->getAttributesValues()[i2]);
	}
      err = med_2_3::MEDfamCr( _medIdt, 
			      const_cast <char *> ( _meshName.c_str() ),
			      const_cast <char *> ( healName(families[i]->getName()).c_str() ),
			      families[i]->getIdentifier(), 
			      AttributesIdentifier2,
			      AttributesValues2,
			      const_cast <char *> (attributesDescriptions.c_str()), 
			      numberOfAttributes,  
			      const_cast <char *> (groupsNames.c_str()), 
			      numberOfGroups);
      delete [] AttributesIdentifier2;
      delete [] AttributesValues2;
#else
      err = med_2_3::MEDfamCr( _medIdt, 
			      const_cast <char *> ( _meshName.c_str() ),
			      const_cast <char *> ( healName(families[i]->getName()).c_str() ),
			      families[i]->getIdentifier(), 
			      (med_2_3::med_int*)families[i]->getAttributesIdentifiers(),
			      (med_2_3::med_int*)families[i]->getAttributesValues(),
			      const_cast <char *> (attributesDescriptions.c_str()), 
			      numberOfAttributes,  
			      const_cast <char *> (groupsNames.c_str()), 
			      numberOfGroups);
#endif
      SCRUTE(err);
      if (err != MED_VALID) 
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't create family |" << families[i]->getName()
				     << "| with identifier |" << families[i]->getIdentifier()
                                     << "| groups names |" << groupsNames
                                     << "| and  attributes descriptions |" << attributesDescriptions << "|"));
    }
    else
      med_2_3::_MEDdatagroupFermer(_medIdt);

  }

  END_OF();

  return MED_VALID;
}


// A FAIRE POUR LES NOEUDS ET LES ELEMENTS ret = MEDfamEcr(



/*--------------------- RDWR PART -------------------------------*/

MED_MESH_RDWR_DRIVER22::MED_MESH_RDWR_DRIVER22()
{
  this->GENDRIVER::_accessMode = MED_EN::RDWR;
}

MED_MESH_RDWR_DRIVER22::MED_MESH_RDWR_DRIVER22(const string & fileName,
					       MESH * ptrMesh):
  MED_MESH_DRIVER(fileName,ptrMesh,RDWR),
  IMED_MESH_RDONLY_DRIVER(fileName,ptrMesh),
  IMED_MESH_WRONLY_DRIVER(fileName,ptrMesh),
  IMED_MESH_RDWR_DRIVER(fileName,ptrMesh),
  MED_MESH_DRIVER22(fileName,ptrMesh,RDWR),
  MED_MESH_RDONLY_DRIVER22(fileName,ptrMesh),
  MED_MESH_WRONLY_DRIVER22(fileName,ptrMesh)
{
  
  MESSAGE("MED_MESH_RDWR_DRIVER22::MED_MESH_RDWR_DRIVER22(const string & fileName, MESH * ptrMesh) has been created");
}

MED_MESH_RDWR_DRIVER22::MED_MESH_RDWR_DRIVER22(const MED_MESH_RDWR_DRIVER22 & driver):
  MED_MESH_DRIVER(driver),
  IMED_MESH_RDONLY_DRIVER(driver),
  IMED_MESH_WRONLY_DRIVER(driver),
  MED_MESH_DRIVER22(driver),
  MED_MESH_RDONLY_DRIVER22(driver),
  MED_MESH_WRONLY_DRIVER22(driver),
  IMED_MESH_RDWR_DRIVER(driver)

{
}

MED_MESH_RDWR_DRIVER22::~MED_MESH_RDWR_DRIVER22() {
  //MESSAGE("MED_MESH_RDWR_DRIVER22::MED_MESH_RDWR_DRIVER22(const string & fileName, MESH * ptrMesh) has been destroyed");
}

GENDRIVER * MED_MESH_RDWR_DRIVER22::copy(void) const
{
  return new MED_MESH_RDWR_DRIVER22(*this);
}

void MED_MESH_RDWR_DRIVER22::write(void) const
{
  MED_MESH_WRONLY_DRIVER22::write();
}
void MED_MESH_RDWR_DRIVER22::read (void)
{
  MED_MESH_RDONLY_DRIVER22::read();
}
