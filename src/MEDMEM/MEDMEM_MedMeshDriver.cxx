using namespace std;
#include "MEDMEM_MedMeshDriver.hxx"

#include "MEDMEM_DriversDef.hxx"

#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_Grid.hxx"

using namespace MEDMEM;
extern "C" {
  extern med_idt _MEDdatagroupOuvrir(med_idt pid, char *nom);
  extern med_err _MEDdatagroupFermer(med_idt id);
}

// Every memory allocation made in the MedDriver members function are desallocated in the Mesh destructor 

MED_MESH_DRIVER::MED_MESH_DRIVER():
  GENDRIVER(),
  _ptrMesh(( MESH *)MED_NULL),
  _medIdt(MED_INVALID),
  _meshName(""),
  _meshNum(MED_INVALID)
{
}

MED_MESH_DRIVER::MED_MESH_DRIVER(const string & fileName,
				 MESH * ptrMesh,
				 MED_EN::med_mode_acces accessMode): 
  GENDRIVER(fileName,accessMode),
  _ptrMesh(ptrMesh),
  _medIdt(MED_INVALID), 
  _meshName(""),
  _meshNum(MED_INVALID) 
{
}
  
MED_MESH_DRIVER::MED_MESH_DRIVER(const MED_MESH_DRIVER & driver): 
  GENDRIVER(driver),
  _ptrMesh(driver._ptrMesh),
  _medIdt(MED_INVALID), 
  _meshName(driver._meshName),
  _meshNum(driver._meshNum) 
{
}

MED_MESH_DRIVER::~MED_MESH_DRIVER()
{
}

void MED_MESH_DRIVER::open()
  throw (MEDEXCEPTION)
{
  const char * LOC = "MED_MESH_DRIVER::open()" ;
  BEGIN_OF(LOC);
  MESSAGE(LOC<<" : _fileName.c_str : "<< _fileName.c_str()<<",mode : "<< _accessMode);
  _medIdt = MED_FR::MEDouvrir( (const_cast <char *> (_fileName.c_str())),(MED_FR::med_mode_acces) _accessMode);
  MESSAGE(LOC<<" _medIdt : "<< _medIdt );
  if (_medIdt > 0) 
    _status = MED_OPENED; 
  else {
    _medIdt = MED_INVALID;
    _status = MED_CLOSED;
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Could not open file "<<_fileName<<" in mode "<<_accessMode));
  }
  
  END_OF(LOC);
}
  
void MED_MESH_DRIVER::close()
  throw (MEDEXCEPTION)
{
  const char * LOC = "MED_MESH_DRIVER::close() " ;
  BEGIN_OF(LOC);
  int err = 0;
  if ( _status == MED_OPENED) {
    err=MED_FR::MEDfermer(_medIdt);
    H5close(); // If we call H5close() all the files are closed.
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
  END_OF(LOC);
}

void    MED_MESH_DRIVER::setMeshName(const string & meshName) { _meshName = meshName; };
string  MED_MESH_DRIVER::getMeshName() const { return _meshName; };

//A FAIRE UTILISER LES MAPS...
const MED_FR::med_geometrie_element  MED_MESH_DRIVER::all_cell_type[MED_NBR_GEOMETRIE_MAILLE]=
  { MED_FR::MED_POINT1,MED_FR::MED_SEG2,MED_FR::MED_SEG3,MED_FR::MED_TRIA3,MED_FR::MED_QUAD4,MED_FR::MED_TRIA6,MED_FR::MED_QUAD8,
    MED_FR::MED_TETRA4,MED_FR::MED_PYRA5,MED_FR::MED_PENTA6,MED_FR::MED_HEXA8,MED_FR::MED_TETRA10,MED_FR::MED_PYRA13,
    MED_FR::MED_PENTA15, MED_FR::MED_HEXA20};

const char * const MED_MESH_DRIVER::all_cell_type_tab [MED_NBR_GEOMETRIE_MAILLE]=
  { "MED_POINT1","MED_SEG2","MED_SEG3","MED_TRIA3","MED_QUAD4","MED_TRIA6","MED_QUAD8",
    "MED_TETRA4","MED_PYRA5","MED_PENTA6","MED_HEXA8","MED_TETRA10","MED_PYRA13",
    "MED_PENTA15","MED_HEXA20"};


//---------------------------------- RDONLY PART -------------------------------------------------------------

MED_MESH_RDONLY_DRIVER::MED_MESH_RDONLY_DRIVER(): MED_MESH_DRIVER()
{
}
  
MED_MESH_RDONLY_DRIVER::MED_MESH_RDONLY_DRIVER(const string & fileName,
					       MESH * ptrMesh):
  MED_MESH_DRIVER(fileName,ptrMesh,MED_RDONLY)
{ 
  MESSAGE("MED_MESH_RDONLY_DRIVER::MED_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}
  
MED_MESH_RDONLY_DRIVER::MED_MESH_RDONLY_DRIVER(const MED_MESH_RDONLY_DRIVER & driver): 
  MED_MESH_DRIVER(driver)
{
}

MED_MESH_RDONLY_DRIVER::~MED_MESH_RDONLY_DRIVER()
{
  //MESSAGE("MED_MESH_RDONLY_DRIVER::~MED_MESH_RDONLY_DRIVER() has been destroyed");
}
  
GENDRIVER * MED_MESH_RDONLY_DRIVER::copy(void) const
{
  return new MED_MESH_RDONLY_DRIVER(*this);
}

void MED_MESH_RDONLY_DRIVER::read(void)
  throw (MEDEXCEPTION)
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER::read() : " ;
  BEGIN_OF(LOC);
  if (_status!=MED_OPENED)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "The _idt of file " << _fileName << " is : " << _medIdt <<  " (the file is not opened)." )) ;

  _ptrMesh->_name =  _meshName;

  SCRUTE(_ptrMesh->getIsAGrid());

  if (_ptrMesh->getIsAGrid())
    {
      getGRID( );

      // always call getFAMILY : families are requiered !!!!

//        int nbFam = MEDnFam(_medIdt,
//  			  const_cast <char *> (_meshName.c_str()),
//  			  0,
//  			  MED_FR::MED_FAMILLE);
//        if (nbFam > 0)
	{
// 	  getFAMILY();
    
	  if (getFAMILY()!=MED_VALID)
	    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERREUR in getFAMILY when the mesh is a grid")) ;

	  buildAllGroups(_ptrMesh->_groupNode,_ptrMesh->_familyNode) ;
	}

      END_OF(LOC);
      return;
    }

  if (getCOORDINATE()!=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERREUR in getCOORDINATE"  )) ;
 
  if (getCONNECTIVITY()!=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERREUR in getCOONECTIVITY")) ;
  
  if (getFAMILY()!=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERREUR in getFAMILY"      )) ;
  
  updateFamily();

  // we build all groups
  // on node
  buildAllGroups(_ptrMesh->_groupNode,_ptrMesh->_familyNode) ;
  // on cell
  buildAllGroups(_ptrMesh->_groupCell,_ptrMesh->_familyCell) ;
  // on face
  buildAllGroups(_ptrMesh->_groupFace,_ptrMesh->_familyFace) ;
  // on edge
  buildAllGroups(_ptrMesh->_groupEdge,_ptrMesh->_familyEdge) ;

//   MESSAGE(LOC<<"Checking of CellModel !!!!!!!");

//   int nbOfTypes =  _ptrMesh->_connectivity->_numberOfTypes;
//    for(int i=0;i<nbOfTypes;i++)
//      {
//        MESSAGE(LOC << _ptrMesh->_connectivity->_type[i]) ;
//      }

  END_OF(LOC);
}

//=======================================================================
//function : getGRID
//purpose  : 
//=======================================================================

void MED_MESH_RDONLY_DRIVER::getGRID()
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER::getGRID() : " ;
  BEGIN_OF(LOC);
  
  if (_status!=MED_OPENED)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "med file is not opened"));
  
  GRID * ptrGrid = (GRID *) _ptrMesh;
    
  SCRUTE(ptrGrid);

  int err, i;

  // Read the dimension of the mesh <_meshName>
  int MeshDimension = MED_FR::MEDdimLire(_medIdt, const_cast <char *>
					 (_meshName.c_str())) ;

  if (MeshDimension == MED_INVALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "The mesh dimension |" <<
				 MeshDimension << "| seems to be incorrect " <<
				 "for the mesh : |" << _meshName << "|")) ;

  _ptrMesh->_meshDimension = MeshDimension;

  // Read or get the dimension of the space for the mesh <_meshName>
  int SpaceDimension = MeshDimension;

  int SpaceDimensionRead = MED_FR::MEDdimEspaceLire(_medIdt,
						    const_cast <char *>
						    (_meshName.c_str())) ;

  if (SpaceDimensionRead != MED_INVALID) SpaceDimension = SpaceDimensionRead;

  _ptrMesh->_spaceDimension = SpaceDimension;

  MED_FR::med_repere rep ;
  string tmp_nom_coord (MED_TAILLE_PNOM*(_ptrMesh->_spaceDimension)+1,' ');
  string tmp_unit_coord(MED_TAILLE_PNOM*(_ptrMesh->_spaceDimension)+1,' ');
  char * tmp_nom = (const_cast <char *> ( tmp_nom_coord.c_str())  ) ;
  char * tmp_unit= (const_cast <char *> ( tmp_unit_coord.c_str()) ) ;

  // Read Array length
  int * ArrayLen[] = { & ptrGrid->_iArrayLength,
                       & ptrGrid->_jArrayLength,
                       & ptrGrid->_kArrayLength  };

  med_type_grille gridType = ptrGrid->getGridType();

  int NumberOfNodes;

  if (gridType == MED_EN::MED_GRILLE_STANDARD)
    {
      NumberOfNodes = MED_FR::MEDnEntMaa(_medIdt,
					 const_cast <char *>
					 (_ptrMesh->_name.c_str()),
					 MED_FR::MED_COOR,MED_FR::MED_NOEUD,
					 MED_FR::MED_NONE,MED_FR::MED_NOD);

      if ( NumberOfNodes <= MED_VALID )
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"The number of nodes |" <<
				     NumberOfNodes <<
				     "| seems to be incorrect "
				     << "for the mesh : |" <<
				     _meshName << "|" )) ;

      _ptrMesh->_numberOfNodes = NumberOfNodes ;

      // create coordinates and its structure
      _ptrMesh->_coordinate = new COORDINATE(SpaceDimension,NumberOfNodes,
					     MED_EN::MED_FULL_INTERLACE);

      int * structure = new int[MeshDimension];

      err = MED_FR::MEDstructureCoordLire(_medIdt,
					  const_cast <char *>
					  (_ptrMesh->_name.c_str()),
					  MeshDimension,structure);

      if (err != MED_VALID)
	throw MEDEXCEPTION(STRING(LOC) <<"Error in reading the structure of grid : |" << _meshName << "|" ) ;
      
      for (int idim = 0; idim < MeshDimension; idim++)
	ArrayLen [idim][0] = structure[idim];
      
      delete [] structure;

      err = MEDcoordLire(_medIdt,
			 const_cast <char *> (_ptrMesh->_name.c_str()),
			 _ptrMesh->_spaceDimension,
			 //const_cast <double *> ( _ptrMesh->_coordinate->_coordinate->get(MED_EN::MED_FULL_INTERLACE) ),
			 const_cast <double *> ( _ptrMesh->_coordinate->_coordinate.get(MED_EN::MED_FULL_INTERLACE) ),
			 MED_FR::MED_FULL_INTERLACE,
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
  else if ((gridType == MED_EN::MED_GRILLE_CARTESIENNE) ||
	   (gridType == MED_EN::MED_GRILLE_POLAIRE))
    {
      NumberOfNodes = 1;

      double * Array[] = { (double*) 0, (double*) 0, (double*) 0 };

      for (int idim = 0; idim < _ptrMesh->_meshDimension; ++idim)
	{
	  MED_FR::med_table table;
	  if (idim == 0) table = MED_FR::MED_COOR_IND1;
	  else if (idim == 1) table = MED_FR::MED_COOR_IND2;
	  else if (idim == 2) table = MED_FR::MED_COOR_IND3;

	  int length = MED_FR::MEDnEntMaa(_medIdt,
					  const_cast <char *> (_ptrMesh->_name.c_str()),
					  table,MED_FR::MED_NOEUD,
					  MED_FR::MED_NONE,
					  MED_FR::MED_NOD);
	  if ( length <= MED_VALID )
	    throw MEDEXCEPTION(STRING(LOC) <<"The number of nodes |" << length <<
			       "| seems to be incorrect "
			       << "for the mesh : |" << _meshName << "|" ) ;
	  
	  ArrayLen [idim][0] = length;
	  NumberOfNodes *= length;

	  Array [idim] = new double [ length ];

	  err = MED_FR::MEDindicesCoordLire(_medIdt, const_cast <char *>
					    (_ptrMesh->_name.c_str()),
					    _ptrMesh->_meshDimension,
					    Array [idim], length, (idim+1),
					    tmp_nom+(idim*MED_TAILLE_PNOM),
					    tmp_unit+(idim*MED_TAILLE_PNOM));

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

      if (gridType == MED_EN::MED_GRILLE_CARTESIENNE)
	rep = MED_FR::MED_CART;
      else if (gridType == MED_EN::MED_GRILLE_POLAIRE)
	{
	  if (SpaceDimension == 2) rep = MED_FR::MED_CYL;
	  else if (SpaceDimension == 3) rep = MED_FR::MED_SPHER;
	}
    }
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<" bad grid type : " << gridType));

  // set coordinate names

  for (i=0; i<_ptrMesh->_spaceDimension; ++i ) {
    string myStringName(tmp_nom,i*MED_TAILLE_PNOM,MED_TAILLE_PNOM) ;
    string myStringUnit(tmp_unit,i*MED_TAILLE_PNOM,MED_TAILLE_PNOM) ;
    // suppress space at the end
    int j ;
    for(j=MED_TAILLE_PNOM-1;j>=0;j--)
      if (myStringName[j] != ' ') break ;
    _ptrMesh->_coordinate->_coordinateName[i]=string(myStringName,0,j+1);
    for(j=MED_TAILLE_PNOM-1;j>=0;j--)
      if (myStringUnit[j] != ' ') break ;
    _ptrMesh->_coordinate->_coordinateUnit[i]=string(myStringUnit,0,j+1);
  }

  string coordinateSystem = "UNDEFINED";

  if( rep == MED_FR::MED_CART) coordinateSystem = "CARTESIAN";
  else if ( rep == MED_FR::MED_CYL) coordinateSystem = "CYLINDRICAL";
  else if ( rep == MED_FR::MED_SPHER) coordinateSystem = "SPHERICAL";

  _ptrMesh->_coordinate->setCoordinatesSystem(coordinateSystem);

  END_OF(LOC);
}

//=======================================================================
//function : getCOORDINATE
// A FAIRE : RENVOYER DU VOID
//=======================================================================
int  MED_MESH_RDONLY_DRIVER::getCOORDINATE()
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER::getCOORDINATE() : " ;

  BEGIN_OF(LOC);

  if (_status==MED_OPENED)
    {
      int err ;
      
      // Read the dimension of the mesh <_meshName>
      int MeshDimension = MED_FR::MEDdimLire(_medIdt, const_cast <char *>
					     (_meshName.c_str())) ;

      if ( MeshDimension == MED_INVALID ) 
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "The mesh dimension |" <<
				     MeshDimension <<
				     "| seems to be incorrect " <<
				     "for the mesh : |" << _meshName << "|")) ;

      _ptrMesh->_meshDimension = MeshDimension;

      // Read or get the dimension of the space for the mesh <_meshName>
      int SpaceDimension = MeshDimension;

      int SpaceDimensionRead = MED_FR::MEDdimEspaceLire(_medIdt,
							const_cast <char *>
							(_meshName.c_str())) ;

      if (SpaceDimensionRead  != MED_INVALID)
	SpaceDimension = SpaceDimensionRead;

      _ptrMesh->_spaceDimension = SpaceDimension;

      // Read the number of nodes used in the mesh <_meshName>
      // to be able to create a COORDINATE object
      int NumberOfNodes=MEDnEntMaa(_medIdt,
                                   const_cast <char *> (_meshName.c_str()),
                                   MED_FR::MED_COOR,
                                   MED_FR::MED_NOEUD,
                                   (MED_FR::med_geometrie_element) MED_NONE,
                                   (MED_FR::med_connectivite)      MED_NONE);
      if ( NumberOfNodes <= MED_VALID )
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"The number of nodes |" << NumberOfNodes << "| seems to be incorrect "
                                     << "for the mesh : |" << _meshName << "|" )) ;
      _ptrMesh->_numberOfNodes = NumberOfNodes ;

      // create a COORDINATE object
      _ptrMesh->_coordinate = new COORDINATE(SpaceDimension, NumberOfNodes, MED_EN::MED_FULL_INTERLACE);
      
      MED_FR::med_repere rep ; // ATTENTION ---> DOIT ETRE INTEGRE DS MESH EF: FAIT NON?
      string tmp_nom_coord (MED_TAILLE_PNOM*(_ptrMesh->_spaceDimension)+1,'\0');
      string tmp_unit_coord(MED_TAILLE_PNOM*(_ptrMesh->_spaceDimension)+1,'\0');
      char * tmp_nom = (const_cast <char *> ( tmp_nom_coord.c_str())  ) ;
      char * tmp_unit= (const_cast <char *> ( tmp_unit_coord.c_str()) ) ;

      err=MEDcoordLire(_medIdt,
                       const_cast <char *> (_ptrMesh->_name.c_str()),
		       _ptrMesh->_spaceDimension,
		       //const_cast <double *> ( _ptrMesh->_coordinate->_coordinate->get(MED_EN::MED_FULL_INTERLACE) ),
		       const_cast <double *> ( _ptrMesh->_coordinate->_coordinate.get(MED_EN::MED_FULL_INTERLACE) ),
                       MED_FR::MED_FULL_INTERLACE,
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
	string myStringName(tmp_nom,i*MED_TAILLE_PNOM,MED_TAILLE_PNOM) ;
	string myStringUnit(tmp_unit,i*MED_TAILLE_PNOM,MED_TAILLE_PNOM) ;
	// suppress space at the end
	int j ;
	for(j=MED_TAILLE_PNOM-1;j>=0;j--)
	  if (myStringName[j] != ' ') break ;
	_ptrMesh->_coordinate->_coordinateName[i]=string(myStringName,0,j+1);
	for(j=MED_TAILLE_PNOM-1;j>=0;j--)
	  if (myStringUnit[j] != ' ') break ;
	_ptrMesh->_coordinate->_coordinateUnit[i]=string(myStringUnit,0,j+1);
      }

      // Pourquoi le stocker sous forme de chaîne ?
      switch (rep)
	{
	case MED_FR::MED_CART : 
	  {
	    _ptrMesh->_coordinate->_coordinateSystem = "CARTESIAN";
	    break ;
	  }
	case MED_FR::MED_CYL :
	  {
	    _ptrMesh->_coordinate->_coordinateSystem = "CYLINDRICAL";
	    break ;
	  }
	case MED_FR::MED_SPHER :
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
      char * tmp_node_name = new char[NumberOfNodes*MED_TAILLE_PNOM+1];
      tmp_node_name[NumberOfNodes]='\0' ;
      err=MEDnomLire(_medIdt,const_cast <char*> (_ptrMesh->_name.c_str()),
		     tmp_node_name,NumberOfNodes*MED_TAILLE_PNOM,MED_FR::MED_NOEUD,
		     (MED_FR::med_geometrie_element) MED_NONE);
      if (err == MED_VALID) 
        MESSAGE(LOC<<"MED_MESH_RDONLY_DRIVER::getNoeuds() : WARNING : Nodes have names but we do not read them !");
      delete[] tmp_node_name ;


      // ??? Read the unused optional node Numbers ???
      int * tmp_node_number = new int[NumberOfNodes] ;
      err=MEDnumLire(_medIdt,const_cast <char*> (_ptrMesh->_name.c_str()),
		     tmp_node_number,NumberOfNodes,MED_FR::MED_NOEUD,(MED_FR::med_geometrie_element)0);
      if (err == MED_VALID) {
        // INFOS(LOC<<"WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING");
        // INFOS(LOC<<"MED_MESH_RDONLY_DRIVER::getNoeuds() : WARNING : Nodes have numbers but we do not take care of them !");
        // INFOS(LOC<<"WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING");
	MESSAGE(LOC<<"MED_MESH_RDONLY_DRIVER::getNoeuds() : Nodes have numbers, we DO TAKE care of them !");
	_ptrMesh->_coordinate->_nodeNumber.set(NumberOfNodes) ; 
	memcpy((int*)_ptrMesh->_coordinate->_nodeNumber,tmp_node_number,sizeof(int)*NumberOfNodes) ;
	
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
      
      END_OF(LOC);
      return MED_VALID;
    }
  return MED_ERROR;
}


int MED_MESH_RDONLY_DRIVER::getCONNECTIVITY() 
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER::getCONNECTIVITY : " ;
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
      if (err!=MED_VALID) {
	Connectivity->_typeConnectivity = MED_DESCENDING ;
	err = getDescendingConnectivity(Connectivity) ;
      } else 
	getDescendingConnectivity(Connectivity) ; // we read it if there is one
      
      if (err!=MED_VALID) {
	delete Connectivity ;
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "We could not read any Connectivity")) ;
      }

      //      _ptrMesh->_meshDimension = Connectivity->_entityDimension ; 

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
	  Connectivity->_constituent=ConnectivityFace ; 
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
      
      END_OF(LOC);
      return MED_VALID;
    }
  return MED_ERROR;
}

int MED_MESH_RDONLY_DRIVER::getNodalConnectivity(CONNECTIVITY * Connectivity) 
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER::getNodalConnectivity : " ;
  BEGIN_OF(LOC);
  if (_status==MED_OPENED)
    {
      // Get the type of entity to work on (previously set in the Connectivity Object)
      MED_FR::med_entite_maillage Entity = (MED_FR::med_entite_maillage) Connectivity->getEntity();

      // Get the number of cells of each type & store it in <tmp_cells_count>.
      int * tmp_cells_count = new int[MED_NBR_GEOMETRIE_MAILLE] ;
      int i;
      for (i=1;i<MED_NBR_GEOMETRIE_MAILLE;i++) {                       // EF :ON SCANNE DES GEOMETRIES INUTILES, UTILISER LES MAPS
	tmp_cells_count[i]=MEDnEntMaa(_medIdt,(const_cast <char *> (_ptrMesh->_name.c_str())),
				      MED_FR::MED_CONN,(MED_FR::med_entite_maillage) Entity,
                                      all_cell_type[i],MED_FR::MED_NOD); 


        // Get the greatest dimension of the cells : Connectivity->_entityDimension
        // We suppose there is no cells used as faces in MED 2.2.x , this is forbidden !!!
	// In version prior to 2.2.x, it is possible
	if (tmp_cells_count[i]>0) { 
	  Connectivity->_entityDimension=all_cell_type[i]/100;  
	  Connectivity->_numberOfTypes++;
	}
      }
      

      // If there is no nodal connectivity, we quit !
      if ( Connectivity->_numberOfTypes == 0 ) {
	delete[] tmp_cells_count ;
	return MED_ERROR ;
      }

      // if MED version < 2.2.x, we read only entity with dimention = Connectivity->_entityDimension. Lesser dimension are face or edge !

      char version_med[10] ;
      if ( MEDfichEntete(_medIdt,MED_FR::MED_VERSION,version_med) != 0 ){
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
  
      if ((version_med != "2.2")&(Entity==MED_FR::MED_MAILLE)) {
	
        Connectivity->_numberOfTypes=0;
	
        for ( i=1;i<MED_NBR_GEOMETRIE_MAILLE;i++) {
	  tmpFaceCount[i]=0;
	  tmpEdgeCount[i]=0;
	  if (tmp_cells_count[i]!=0) {
	    int dimension = all_cell_type[i]/100 ;
	    if (Connectivity->_entityDimension==dimension) 
	      Connectivity->_numberOfTypes++ ;
	    
	    if (dimension == 2)
	      if (Connectivity->_entityDimension==3) {
		tmpFaceCount[i]=tmp_cells_count[i] ;
		tmp_cells_count[i]=0 ;
		numberOfFacesTypes++;
	      }
	    if (dimension == 1)
	      if (Connectivity->_entityDimension>dimension) {
		tmpEdgeCount[i]=tmp_cells_count[i] ;
		tmp_cells_count[i]=0;
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
	for ( i=1;i<MED_NBR_GEOMETRIE_MAILLE;i++)  { // no point1 cell type (?)
	  if (tmp_cells_count[i]>0) {
	    Connectivity->_count[typeNumber]=Connectivity->_count[typeNumber-1]+tmp_cells_count[i];

	    CELLMODEL t( (MED_EN::medGeometryElement) MED_MESH_DRIVER::all_cell_type[i]) ;
	    Connectivity->_type[typeNumber-1]=t ;
	    
	    Connectivity->_geometricTypes[typeNumber-1]=( MED_EN::medGeometryElement) MED_MESH_DRIVER::all_cell_type[i] ;
	    
	    // probleme avec les mailles de dimension < a dimension du maillage :
	    // Il faut oter le zero a la lecture est le remettre a l'ecriture : ce n'est pas fait !!!!! On interdit ce cas pour l'instant !!!

	      
	    size+=tmp_cells_count[i]*((MED_MESH_DRIVER::all_cell_type[i])%100) ;
	    
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
	for ( i=0;i<Connectivity->_numberOfTypes;i++) {
	  int multi = 0 ;
	  MED_FR::med_geometrie_element med_type = (MED_FR::med_geometrie_element) Connectivity->_type[i].getType() ;
//  	  if ( Connectivity->_type[i].getDimension() < Connectivity->_entityDimension) 
	  if (Connectivity->_entity == MED_CELL)
	    if ( Connectivity->_type[i].getDimension() < _ptrMesh->_spaceDimension) 
	      multi=1;
	  
	  //	  int NumberOfCell = Connectivity->_count[i+1]-Connectivity->_count[i] ;
	  int NumberOfNodeByCell = Connectivity->_type[i].getNumberOfNodes() ;
	  
	  // initialise index
	  for ( j=Connectivity->_count[i]; j<Connectivity->_count[i+1];j++)
	    NodalIndex[j]=NodalIndex[j-1]+NumberOfNodeByCell ; 

	  int tmp_numberOfCells = Connectivity->_count[i+1]-Connectivity->_count[i] ;
	  int * tmp_ConnectivityArray = new int[(NumberOfNodeByCell+multi)*tmp_numberOfCells];
	  
//  	  int err=MEDconnLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
//  			      Connectivity->_entityDimension,tmp_ConnectivityArray,
//  			      MED_FR::MED_FULL_INTERLACE,NULL,0,Entity,med_type,MED_FR::MED_NOD);
	  int err=MEDconnLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
			      _ptrMesh->_spaceDimension,tmp_ConnectivityArray,
			      MED_FR::MED_FULL_INTERLACE,NULL,0,Entity,med_type,MED_FR::MED_NOD);

	  if ( err != MED_VALID) {
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

	//////////////////////////////////////////////////////////////////////////////////////
  	///  Modification pour prise en compte de la numérotation optionnelle des noeuds   ///
  	//////////////////////////////////////////////////////////////////////////////////////
  	///
	/// Rénumérote le tableau temporaire tmp_ConnectivityArray en utilisant _optionnalToCanonicNodesNumbers
	/// Le traitement est identique à la version originelle s'il n'y a pas de numérotation optionnelle
	
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
  	
	//////////////////////////////////////////////////////////////////////////////////////
	
	
	  delete[] tmp_ConnectivityArray;
  
	}

	Connectivity->_nodal = new MEDSKYLINEARRAY(Connectivity->_count[Connectivity->_numberOfTypes]-1,
						   size,
						   NodalIndex,
						   NodalValue) ; 
	delete[] NodalIndex;
	delete[] NodalValue;
	
      } // end of bloc to read CELL

      delete[] tmp_cells_count; 



      // Get Face if any
      // ===============
     
      if (numberOfFacesTypes!=0) {

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
	for ( i=1; i < MED_NBR_GEOMETRIE_MAILLE; i++)  { // no point1 cell type (?)
	  if (tmpFaceCount[i]>0) {
	    
	    constituent->_count[typeNumber] = constituent->_count[typeNumber-1] + tmpFaceCount[i];
	    CELLMODEL t( (MED_EN::medGeometryElement) MED_MESH_DRIVER::all_cell_type[i]) ;
	    constituent->_type[typeNumber-1]=t ;
	    
	    constituent->_geometricTypes[typeNumber-1]=( MED_EN::medGeometryElement) MED_MESH_DRIVER::all_cell_type[i] ;
	    
	    size+=tmpFaceCount[i]*((MED_MESH_DRIVER::all_cell_type[i])%100) ;
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
	for ( i=0; i<constituent->_numberOfTypes; i++) {
	  MED_FR::med_geometrie_element med_type = (MED_FR::med_geometrie_element) constituent->_type[i].getType() ;

	  int NumberOfNodeByFace = constituent->_type[i].getNumberOfNodes() ;
	  
	  // initialise NodalIndex
	  for (int j=constituent->_count[i]; j<constituent->_count[i+1];j++)
	    NodalIndex[j]=NodalIndex[j-1]+NumberOfNodeByFace ; 
	  
	  int tmp_numberOfFaces = constituent->_count[i+1]-constituent->_count[i] ;
	  // Il faut ajouter 1 pour le zero a la lecture !!!
          // ATTENTION UNIQUEMENT POUR MED < 2.2.x
          int * tmp_constituentArray = NULL;
          if (version_med != "2.2") 
            tmp_constituentArray = new int[(NumberOfNodeByFace+1)*tmp_numberOfFaces] ;
          else {
            tmp_constituentArray = new int[NumberOfNodeByFace*tmp_numberOfFaces] ;
            MESSAGE(LOC<<": WE ARE USING MED2.2 so there is no +1 for calculating the size of  tmp_constituentArray !") ;
	  }
	  int err=MEDconnLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
			      Connectivity->_entityDimension,tmp_constituentArray,
			      MED_FR::MED_FULL_INTERLACE,NULL,0,MED_FR::MED_MAILLE,med_type,MED_FR::MED_NOD);

	  if ( err != MED_VALID) {
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
	  for (int j=0; j<tmp_numberOfFaces; j++)
	    for (int k=0; k<NumberOfNodeByFace; k++)
	      constituentArray[j*NumberOfNodeByFace+k]=tmp_constituentArray[j*(NumberOfNodeByFace+1)+k] ;

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
      if (numberOfEdgesTypes!=0) {
	CONNECTIVITY * constituent = new CONNECTIVITY(numberOfEdgesTypes,MED_EDGE) ;
	constituent->_entityDimension = 1 ;
	constituent->_count[0]=1 ;

	int size = 0 ; 
	int typeNumber=1 ;
        // if you declare a variable <i> in two <for> initialization statement, 
        // compiler gcc2.95.3 says nothing but there are two <i> variables in the same block 
        //and the value you get in the common block seems to be the value of the first variable !
        int i; 

	for ( i=1; i<MED_NBR_GEOMETRIE_MAILLE; i++)  { // no point1 cell type (?)
	  if (tmpEdgeCount[i]>0) {
	    
	    constituent->_count[typeNumber]=constituent->_count[typeNumber-1]+tmpEdgeCount[i];
	    CELLMODEL t( (MED_EN::medGeometryElement) MED_MESH_DRIVER::all_cell_type[i]) ;
	    constituent->_type[typeNumber-1]=t ;
	    
	    constituent->_geometricTypes[typeNumber-1]=( MED_EN::medGeometryElement) MED_MESH_DRIVER::all_cell_type[i] ;
	    
	    size+=tmpEdgeCount[i]*((MED_MESH_DRIVER::all_cell_type[i])%100) ;
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
	for ( i=0; i<constituent->_numberOfTypes; i++) {
	  MED_FR::med_geometrie_element med_type = (MED_FR::med_geometrie_element) constituent->_type[i].getType() ;

	  int NumberOfNodeByEdge = constituent->_type[i].getNumberOfNodes() ;
	  
	  // initialise index
	  for (int j=constituent->_count[i]; j<constituent->_count[i+1];j++)
	    NodalIndex[j]=NodalIndex[j-1]+NumberOfNodeByEdge ; 
	  
	  int tmp_numberOfEdges = constituent->_count[i+1]-constituent->_count[i] ;
	  // Il faut ajouter 1 pour le zero a la lecture !!!

          // ATTENTION UNIQUEMENT POUR MED < 2.2.x
          int * tmp_constituentArray = NULL;
          if (version_med != "2.2") 
	    tmp_constituentArray = new int[(NumberOfNodeByEdge+1)*tmp_numberOfEdges] ;
          else {
	    tmp_constituentArray = new int[NumberOfNodeByEdge*tmp_numberOfEdges] ;
            MESSAGE(LOC<<": WE ARE USING MED2.2 so there is no +1 for calculating the size of  tmp_constituentArray !") ;
          }
	  
	  int err=MEDconnLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
			      _ptrMesh->_spaceDimension,tmp_constituentArray,
			      MED_FR::MED_FULL_INTERLACE,NULL,0,MED_FR::MED_MAILLE,med_type,MED_FR::MED_NOD);
	  if ( err != MED_VALID) {
	    MESSAGE(LOC<<": MEDconnLire returns "<<err) ;
	    delete constituent ;
	    delete[] tmp_constituentArray;
	    delete[] tmpEdgeCount;
	    return MED_ERROR ;
	  }

	  int * constituentArray = NodalValue + NodalIndex[constituent->_count[i]-1]-1 ;
	  
	  // version originale sans prise en compte des numéros optionnels	
	  //
	  for (int j=0; j<tmp_numberOfEdges; j++)
	    for (int k=0; k<NumberOfNodeByEdge; k++)
	      constituentArray[j*NumberOfNodeByEdge+k]=tmp_constituentArray[j*(NumberOfNodeByEdge+1)+k] ;

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

	if (Connectivity->_entityDimension == 3) {
	  if (Connectivity->_constituent==NULL)
	    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Edges are defined but there are no Faces !"));
	  Connectivity->_constituent->_constituent = constituent ;
	} else 
	  Connectivity->_constituent = constituent ;
      }

      delete[] tmpEdgeCount; 
      
      return MED_VALID;
    }

  return MED_ERROR;
}

int MED_MESH_RDONLY_DRIVER::getDescendingConnectivity(CONNECTIVITY * Connectivity) 
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER::getDescendingConnectivity : " ;
  if (_status==MED_OPENED)
    {
      MESSAGE(LOC<<"call on the object " << Connectivity);
      MESSAGE(LOC<<"Not yet implemented !");
    }
  return MED_ERROR;
}

//  int  MED_MESH_RDONLY_DRIVER::getElementFamilies(CONNECTIVITY * Connectivity)
//  {
//    int err = 0 ;
//    int NumberOfTypes = Connectivity->_numberOfTypes ;
//    int * Count = Connectivity->_count ;
//    medGeometryElement * GeometricTypes= Connectivity->_geometricTypes ;
//    int ** tmp_array = new int*[NumberOfTypes] ;
//    for (int i=0; i<NumberOfTypes; i++) 
//      tmp_array[i]=NULL ;
//    for (int i=0; i<NumberOfTypes; i++) {
//      int NumberOfElements = Count[i+1]-Count[i] ;
//      int * tmp_families_number = new int[NumberOfElements] ;
//      err = MEDfamLire(_medIdt,const_cast <char*> (_ptrMesh->_name.c_str()),
//  		     tmp_families_number,NumberOfElements,
//  		     Connectivity->_entity,GeometricTypes[i]);
//      tmp_array[i]=tmp_families_number ;
//      if (err != MED_VALID) {
//        for (int j=0; j<NumberOfTypes; j++)
//  	if (tmp_array[j] != NULL)
//  	  delete[] tmp_array[j] ;
//        delete[] tmp_array ;
//        throw MEDEXCEPTION("MED_MESH_RDONLY_DRIVER::getElementFamilies : No Family in element GeometricTypes[i]");
//      }
//    }
  
//    if (Connectivity->_entity == MED_CELL)
//      _ptrMesh->_MEDArrayCellFamily = tmp_array ;
//    else if (Connectivity->_entity == MED_FACE)
//      _ptrMesh->_MEDArrayFaceFamily = tmp_array ;
//    else if (Connectivity->_entity == MED_EDGE)
//      _ptrMesh->_MEDArrayEdgeFamily = tmp_array ;
  
//    return MED_VALID ;
//  }

int  MED_MESH_RDONLY_DRIVER::getFAMILY() 
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER::getFAMILY() : " ;
  BEGIN_OF(LOC);

  if (_status==MED_OPENED)
    {
      int err = 0 ;

      int * MEDArrayNodeFamily = NULL ;
      int ** MEDArrayCellFamily = NULL ;
      int ** MEDArrayFaceFamily = NULL ;
      int ** MEDArrayEdgeFamily = NULL ;

//     if ( !_ptrMesh->getIsAGrid() )
//       {
    // read number :
    // NODE :
      MEDArrayNodeFamily = new int[_ptrMesh->getNumberOfNodes()] ;

      err = getNodesFamiliesNumber(MEDArrayNodeFamily) ;
      // error only if (_status!=MED_OPENED), other case exeception !
      // CELL

      MESSAGE(LOC << "error returned from getNodesFamiliesNumber " << err);

      MEDArrayCellFamily = new (int*)[_ptrMesh->getNumberOfTypes(MED_CELL)] ;
      // ET SI IL N'Y A PAS DE CELLS ?

      const medGeometryElement * myTypes = _ptrMesh->getTypes(MED_CELL);
      for (int i=0;i<_ptrMesh->getNumberOfTypes(MED_CELL);i++)
	MEDArrayCellFamily[i] = new
	  int[_ptrMesh->getNumberOfElements(MED_CELL,myTypes[i])] ;

      err = getCellsFamiliesNumber(MEDArrayCellFamily,
				   _ptrMesh->_connectivity) ;

      MESSAGE(LOC << "error returned from getCellsFamiliesNumber for Cells " << err);

    if (_ptrMesh->_connectivity->_constituent != NULL)
      {
	if (_ptrMesh->_connectivity->_constituent->_entity == MED_EN::MED_FACE)
	  {
	    // FACE
	    MEDArrayFaceFamily = new
	      (int*)[_ptrMesh->getNumberOfTypes(MED_FACE)] ;

	    myTypes = _ptrMesh->getTypes(MED_FACE);
	    for (int i=0;i<_ptrMesh->getNumberOfTypes(MED_FACE);i++)
	      MEDArrayFaceFamily[i] = new
		int[_ptrMesh->getNumberOfElements(MED_FACE,myTypes[i])] ;
	
	    err =
	      getCellsFamiliesNumber(MEDArrayFaceFamily,
				     _ptrMesh->_connectivity->_constituent) ;

	    MESSAGE(LOC << "error returned from getCellsFamiliesNumber for Faces " << err);
	  }
	else
	  {
	    // EDGE in 2D
	    MEDArrayEdgeFamily = new
	      (int*)[_ptrMesh->getNumberOfTypes(MED_EDGE)] ;

	    myTypes = _ptrMesh->getTypes(MED_EDGE);
	    for (int i=0;i<_ptrMesh->getNumberOfTypes(MED_EDGE);i++)
	      MEDArrayEdgeFamily[i] = new
		int[_ptrMesh->getNumberOfElements(MED_EDGE,myTypes[i])] ;

	    err =
	      getCellsFamiliesNumber(MEDArrayEdgeFamily,
				     _ptrMesh->_connectivity->_constituent) ;
	  
	    MESSAGE(LOC << "error returned from getCellsFamiliesNumber for Edges in 2D " << err);
	  }
	// EDGE in 3D
	if (_ptrMesh->_connectivity->_constituent->_constituent != NULL)
	  {
	    MEDArrayEdgeFamily = new
	      (int*)[_ptrMesh->getNumberOfTypes(MED_EDGE)] ;

	    myTypes = _ptrMesh->getTypes(MED_EDGE);
	    for (int i=0;i<_ptrMesh->getNumberOfTypes(MED_EDGE);i++)
	      MEDArrayEdgeFamily[i] = new
		int[_ptrMesh->getNumberOfElements(MED_EDGE,myTypes[i])] ;

	    err =
	      getCellsFamiliesNumber(MEDArrayEdgeFamily,
				     _ptrMesh->_connectivity->_constituent->_constituent);
	    // we are in 3D !
	
	    MESSAGE(LOC << "error returned from getCellsFamiliesNumber for Edges in 3D " << err);

	  }
      }
//       }
//     else
//       {
// 	// node 
// 	int NumberOfNodes =  _ptrMesh->getNumberOfNodes() ;
// 	MEDArrayNodeFamily = new int[ NumberOfNodes ];
// 	err = MED_FR::MEDfamGridLire (_medIdt,
// 				      const_cast <char *> (_ptrMesh->_name.c_str()),
// 				      MEDArrayNodeFamily,
// 				      NumberOfNodes,
// 				      MED_FR::MED_NOEUD);

// 	// what about cell face and edge ?
//       }

    // Creation of the families
//     int NumberOfFamilies = MEDnFam(_medIdt,const_cast <char *> (_meshName.c_str()),0,MED_FR::MED_FAMILLE) ;

    int NumberOfFamilies = MED_FR::MEDnFam(_medIdt, const_cast <char *>
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
	// int NumberOfAttributes = MEDnFam(_medIdt,const_cast <char *>
	//       (_meshName.c_str()),i+1,MED_FR::MED_ATTR) ;

	int NumberOfAttributes = MED_FR::MEDnAttribut(_medIdt,
						      const_cast <char *>
						      (_meshName.c_str()),
						      (i+1));

	if (NumberOfAttributes < 0) 
	  throw MEDEXCEPTION("MED_MESH_RDONLY_DRIVER::getFAMILY() : NumberOfAttributes" );
    
	// int NumberOfGroups = MEDnFam(_medIdt,const_cast <char *>
	//    (_meshName.c_str()),i+1,MED_FR::MED_GROUPE) ;

	int NumberOfGroups = MED_FR::MEDnGroupe(_medIdt, const_cast <char *>
						(_meshName.c_str()),(i+1)) ;

	if (NumberOfGroups < 0)
	  throw MEDEXCEPTION("MED_MESH_RDONLY_DRIVER::getFAMILY() : NumberOfGroups" );
      
	int FamilyIdentifier ;
	string FamilyName(MED_TAILLE_NOM,'\0');
	int *  AttributesIdentifier = new int[NumberOfAttributes] ;
	int *  AttributesValues     = new int[NumberOfAttributes] ;
	string AttributesDescription(MED_TAILLE_DESC*NumberOfAttributes,' ') ;
	string GroupsNames(MED_TAILLE_LNOM*NumberOfGroups+1,'\0') ;
	err = MED_FR::MEDfamInfo(_medIdt,const_cast <char *>
				 (_meshName.c_str()),
				 (i+1),const_cast <char *>
				 (FamilyName.c_str()), &FamilyIdentifier,
				 AttributesIdentifier,AttributesValues,
				 const_cast <char *>
				 (AttributesDescription.c_str()),
				 &NumberOfAttributes, const_cast <char *>
				 (GroupsNames.c_str()),&NumberOfGroups);


	SCRUTE(GroupsNames);
	SCRUTE(FamilyName);
	SCRUTE(err);
	SCRUTE(i);

	if (err != MED_VALID)
	  throw MEDEXCEPTION("MED_MESH_RDONLY_DRIVER::getFAMILY() : ERROR when get FAMILY informations" );

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


	    //MESSAGE(LOC << " Well is that OK now ?? " << (*Family));

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
	for (int i=0;i<_ptrMesh->getNumberOfTypes(MED_CELL);i++)
	  delete[] MEDArrayCellFamily[i] ;
	delete[] MEDArrayCellFamily ;
      }

    if (MEDArrayFaceFamily != NULL)
      {
	for (int i=0;i<_ptrMesh->getNumberOfTypes(MED_FACE);i++)
	  delete[] MEDArrayFaceFamily[i] ;
	delete[] MEDArrayFaceFamily ;
      }

    if (MEDArrayEdgeFamily != NULL)
      {
	for (int i=0;i<_ptrMesh->getNumberOfTypes(MED_EDGE);i++)
	  delete[] MEDArrayEdgeFamily[i] ;
	delete[] MEDArrayEdgeFamily ;
      }

    END_OF(LOC);
    return MED_VALID ;
    }

  return MED_ERROR;
}

int  MED_MESH_RDONLY_DRIVER::getNodesFamiliesNumber(int * MEDArrayNodeFamily) 
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER::getNodesFamiliesNumber() : " ;

  BEGIN_OF(LOC);

  if (_status==MED_OPENED)
    {
      int err = 0 ;

      err = MEDfamLire(_medIdt, const_cast <char *>
		       (_ptrMesh->_name.c_str()), MEDArrayNodeFamily,
		       _ptrMesh->getNumberOfNodes(), MED_FR::MED_NOEUD,
		       (enum MED_FR::med_geometrie_element) MED_NONE);

      if ( err != MED_VALID)
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "There is no family for the |"<< _ptrMesh->getNumberOfNodes() << "| nodes in mesh |" << _ptrMesh->_name.c_str() << "|"));

      END_OF(LOC);
      return MED_VALID;
    }

  return MED_ERROR;
}

int  MED_MESH_RDONLY_DRIVER::getCellsFamiliesNumber(int **MEDArrayFamily,CONNECTIVITY *Connectivity) 
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER::getCellsFamiliesNumber " ;

  BEGIN_OF(LOC);

  if (_status==MED_OPENED)
    {
      int i, err = 0 ;

      SCRUTE(Connectivity->_numberOfTypes);

      for (i=0;i<Connectivity->_numberOfTypes;i++)
	{
	  int NumberOfCell = Connectivity->_count[i+1]-Connectivity->_count[i];

	  SCRUTE(NumberOfCell);

	  err=MEDfamLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
			 MEDArrayFamily[i],NumberOfCell,
			 (MED_FR::med_entite_maillage) Connectivity->_entity,
			 (MED_FR::med_geometrie_element)
			 Connectivity->_geometricTypes[i]);

	  // provisoire : si les faces ou les aretes sont des mailles !!!
	  if (err != MED_VALID)
	    {
	      MESSAGE(LOC<<"search face/edge family on cell !!!");
	      err=MEDfamLire(_medIdt,const_cast <char *>
			     (_ptrMesh->_name.c_str()),
			     MEDArrayFamily[i],NumberOfCell,
			     MED_FR::MED_MAILLE,
			     (MED_FR::med_geometrie_element)
			     Connectivity->_geometricTypes[i]);
	    }

	  if (err != MED_VALID) 
	    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Family not found for entity "<<Connectivity->_entity<<" and geometric type "<<Connectivity->_geometricTypes[i]));
	}
      return MED_VALID;
    }
  return MED_ERROR;  
}

void MED_MESH_RDONLY_DRIVER::buildAllGroups(vector<GROUP*> & Groups, vector<FAMILY*> & Families) 
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER::buildAllGroups " ;
  BEGIN_OF(LOC);

  int numberOfFamilies = Families.size() ;
  //SCRUTE(numberOfFamilies);
  map< string,list<FAMILY*> > groupsNames ;
  for(int i=0; i<numberOfFamilies; i++) {
    FAMILY * myFamily = Families[i] ;
    int numberOfGroups_ = myFamily->getNumberOfGroups();
    //SCRUTE(i);
    //SCRUTE(numberOfGroups_);
    for (int j=0; j<numberOfGroups_; j++) {
      //SCRUTE(j);
      //SCRUTE(myFamily->getGroupName(j+1));
      groupsNames[myFamily->getGroupName(j+1)].push_back(myFamily);
    }
  }
  int numberOfGroups = groupsNames.size() ;
  SCRUTE(numberOfGroups);
  Groups.resize(numberOfGroups);
  map< string,list<FAMILY*> >::const_iterator currentGroup ;
  int it = 0 ;
  for(currentGroup=groupsNames.begin();currentGroup!=groupsNames.end();currentGroup++) {
    GROUP * myGroup = new GROUP((*currentGroup).first,(*currentGroup).second) ;
//     GROUP * myGroup = new GROUP() ;
//     myGroup->setName((*currentGroup).first);
//     SCRUTE(myGroup->getName());
//     //myGroup->setMesh(_ptrMesh);
//     myGroup->init((*currentGroup).second);
    Groups[it]=myGroup;
    //SCRUTE(it);
    it++;
  }

  END_OF(LOC);
}

void MED_MESH_RDONLY_DRIVER::updateFamily()
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER::updateFamily() " ;
  BEGIN_OF(LOC);

  // we need to update family on constituent if we have constituent, but no 
  // descending connectivity, so, we must calculate all constituent and
  // numbering correctly family !
  _ptrMesh->_connectivity->updateFamily(_ptrMesh->_familyFace) ; // in 2d, do nothing
  _ptrMesh->_connectivity->updateFamily(_ptrMesh->_familyEdge) ; // in 3d, do nothing

  END_OF(LOC);
}


void MED_MESH_RDONLY_DRIVER::write( void ) const
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("MED_MESH_RDONLY_DRIVER::write : Can't write with a RDONLY driver !");
}

/*--------------------- WRONLY PART -------------------------------*/

MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER():MED_MESH_DRIVER()
{
}
  
MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER(const string & fileName,
					       MESH * ptrMesh):
  MED_MESH_DRIVER(fileName,ptrMesh,MED_WRONLY)
{
  MESSAGE("MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}

MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER(const MED_MESH_WRONLY_DRIVER & driver): 
  MED_MESH_DRIVER(driver)
{
}

MED_MESH_WRONLY_DRIVER::~MED_MESH_WRONLY_DRIVER()
{
  //MESSAGE("MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been destroyed");
}

GENDRIVER * MED_MESH_WRONLY_DRIVER::copy(void) const
{
  return new MED_MESH_WRONLY_DRIVER(*this);
}

void MED_MESH_WRONLY_DRIVER::read (void)
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("MED_MESH_WRONLY_DRIVER::read : Can't read with a WRONLY driver !");
}

void MED_MESH_WRONLY_DRIVER::write(void) const
  throw (MEDEXCEPTION)
{ 
  const char * LOC = "void MED_MESH_WRONLY_DRIVER::write(void) const : ";
  BEGIN_OF(LOC);

  // we must first create mesh !!
  MESSAGE(LOC << "MeshName : |" << _meshName << "| FileName : |"<<_fileName<<"| MedIdt : | "<< _medIdt << "|");

  if (_status!=MED_OPENED)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "File "<<_fileName<<" is not open. Open it before write !"));

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
    string dataGroupFam = "/ENS_MAA/"+_meshName+"/FAS/FAMILLE_0/";  
    MESSAGE("|"<<dataGroupFam<<"|");
    err =_MEDdatagroupOuvrir(_medIdt,const_cast <char *> (dataGroupFam.c_str()) );
    if ( err < MED_VALID ) {
      SCRUTE(err);
      
      err = MED_FR::MEDfamCr( _medIdt,
			      const_cast <char *> ( _meshName.c_str() ),
			      "FAMILLE_0", 0,
			      (int*)NULL, (int*)NULL, (char*)NULL, 0,
			      (char*)NULL, 0);
      
      if ( err != MED_VALID) 
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't create family |FAMILLE_0| with identifier |0| groups names || and  attributes descriptions ||")) ;
    }
    else
      _MEDdatagroupFermer(_medIdt);
     
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

  END_OF(LOC);
} 

//=======================================================================
//function : writeGRID
//purpose  : 
//=======================================================================

int MED_MESH_WRONLY_DRIVER::writeGRID() const
{
  const char * LOC = "MED_MESH_WRONLY_DRIVER::writeGRID() : " ;
  BEGIN_OF(LOC);
  
  if (_status!=MED_OPENED)
  {
    MESSAGE (LOC<<" Not open !!!");
    return MED_ERROR;
  }
  GRID * ptrGrid = (GRID*) _ptrMesh;
  
  MED_FR::med_err err = MED_ERROR;
  MED_FR::med_repere rep;
  string tmp_name(_ptrMesh->_spaceDimension*MED_TAILLE_PNOM,' ');
  string tmp_unit(_ptrMesh->_spaceDimension*MED_TAILLE_PNOM,' ');

  // Test if the mesh <_meshName> already exists
  // If it doesn't exists create it
  // If it already exists verify if its space and mesh dimensions are the same
  // as <_ptrMesh->_spaceDimension>, <_ptrMesh->_meshDimension> respectively
  // rem : <_meshName> is the driver meshName not <ptrMesh->_meshName>

  int spaceDimension = MED_FR::MEDdimEspaceLire(_medIdt, const_cast <char *>
						(_meshName.c_str()) );

  int meshDimension = MED_FR::MEDdimLire(_medIdt, const_cast <char *>
					 (_meshName.c_str()) );

  if ((spaceDimension != MED_VALID) && (meshDimension != MED_VALID))
    {
      err = MEDmaaCr(_medIdt,
		     const_cast <char *> (_meshName.c_str()),
		     _ptrMesh->_meshDimension,MED_FR::MED_STRUCTURE,
		     const_cast <char *> (_ptrMesh->_description.c_str()));

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

  // Recompose the <_spaceDimension> strings in 1 string 
  int lengthString ;
  string valueString ;
  for (int i=0;i<_ptrMesh->_spaceDimension;i++) {
    SCRUTE(i);
    valueString = _ptrMesh->_coordinate->_coordinateName[i] ;
    lengthString = (MED_TAILLE_PNOM<valueString.size())?MED_TAILLE_PNOM:valueString.size() ;
    tmp_name.replace(i*MED_TAILLE_PNOM,i*MED_TAILLE_PNOM+lengthString,valueString,0,lengthString);
    valueString = _ptrMesh->_coordinate->_coordinateUnit[i];
    lengthString = (MED_TAILLE_PNOM<valueString.size())?MED_TAILLE_PNOM:valueString.size() ;
    tmp_unit.replace(i*MED_TAILLE_PNOM,i*MED_TAILLE_PNOM+lengthString,valueString,0,lengthString);
  }

  // Pourquoi le stocker sous forme de chaîne ?
  const string & coordinateSystem = _ptrMesh->_coordinate->_coordinateSystem;
  if      (coordinateSystem  == "CARTESIAN") 
    rep = MED_FR::MED_CART;
  else if ( coordinateSystem == "CYLINDRICAL")
    rep = MED_FR::MED_CYL;
  else if ( coordinateSystem == "SPHERICAL" )
    rep = MED_FR::MED_SPHER;
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Grid |" << _meshName.c_str() <<
                                 "| doesn't have a valid coordinate system : |" 
                                 << _ptrMesh->_coordinate->_coordinateSystem
                                 << "|" )) ;  

  int ArrayLen[] = { ptrGrid->_iArrayLength,
                     ptrGrid->_jArrayLength,
                     ptrGrid->_kArrayLength  };
  
  med_type_grille gridType = ptrGrid->getGridType();

  // Write node coordinates for MED_BODY_FITTED grid
  if (gridType == MED_EN::MED_GRILLE_STANDARD)
    {
      // Write Coordinates and families
      double * coo = const_cast <double *>
	(_ptrMesh->_coordinate->getCoordinates(MED_EN::MED_FULL_INTERLACE));
    
      int* structure = new int [meshDimension];

      for (int idim = 0; idim < meshDimension; ++idim)
	structure[idim] = ArrayLen [idim];
 

      err = MED_FR::MEDstructureCoordEcr(_medIdt, const_cast <char *>
					 (_meshName.c_str()), meshDimension,
					 structure);

      if (err != MED_VALID)
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"error in writing the structure of the grid |" << _meshName.c_str()));

      delete structure;

      err = MEDcoordEcr(_medIdt, const_cast <char *> (_meshName.c_str()),
			_ptrMesh->_spaceDimension, 
			//const_cast <double *> ( _ptrMesh->_coordinate->_coordinate->get(MED_EN::MED_FULL_INTERLACE) ), 
			const_cast <double *> ( _ptrMesh->_coordinate->_coordinate.get(MED_EN::MED_FULL_INTERLACE) ), 
			MED_FR::MED_FULL_INTERLACE, _ptrMesh->_numberOfNodes,
			//  _ptrMesh->_coordinate->_numberOfNodes
			rep, const_cast <char *> (tmp_name.c_str()), 
			const_cast <char *> (tmp_unit.c_str()));

      if (err != MED_VALID) 
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write coordinates of the grid |" << _meshName.c_str() << "| in file |" << _fileName
				     << "| with dimension |"  << _ptrMesh->_spaceDimension <<"| and" 
				     << " with units names |"  << tmp_name
				     << "| and units |"       << tmp_unit
				     << " |")) ;
    }
  else if ((gridType == MED_EN::MED_GRILLE_CARTESIENNE) ||
	   (gridType == MED_EN::MED_GRILLE_POLAIRE))
    {
      // Write Arrays of Cartesian or Polar Grid

      double * Array[] = { ptrGrid->_iArray,
			   ptrGrid->_jArray,
			   ptrGrid->_kArray };

      for (int idim = 0; idim < _ptrMesh->_meshDimension; ++idim)
	{
	  string str_name = string (tmp_name,idim*MED_TAILLE_PNOM,
				    MED_TAILLE_PNOM);
	  string str_unit = string (tmp_unit,idim*MED_TAILLE_PNOM,
				    MED_TAILLE_PNOM);

	  err = MED_FR::MEDindicesCoordEcr(_medIdt, const_cast <char *>
					   (_ptrMesh->_name.c_str()),
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

  END_OF(LOC);
  return MED_VALID;
}

//=======================================================================
//function : writeCoordinates
//purpose  : 
//=======================================================================

int MED_MESH_WRONLY_DRIVER::writeCoordinates() const {
 
  const char * LOC = "int MED_MESH_WRONLY_DRIVER::writeCoordinates() const : ";
  BEGIN_OF(LOC);

  MED_FR::med_err err = MED_ERROR;
  MED_FR::med_repere rep;
  string tmp_name(_ptrMesh->_spaceDimension*MED_TAILLE_PNOM,' ');
  string tmp_unit(_ptrMesh->_spaceDimension*MED_TAILLE_PNOM,' ');
    
  // Recompose the <_spaceDimension> strings in 1 string 
  int lengthString ;
  string valueString ;
  for (int i=0;i<_ptrMesh->_spaceDimension;i++) {
    valueString = _ptrMesh->_coordinate->_coordinateName[i] ;
    lengthString = (MED_TAILLE_PNOM<valueString.size())?MED_TAILLE_PNOM:valueString.size() ;
    tmp_name.replace(i*MED_TAILLE_PNOM,i*MED_TAILLE_PNOM+lengthString,valueString,0,lengthString);
    valueString = _ptrMesh->_coordinate->_coordinateUnit[i];
    lengthString = (MED_TAILLE_PNOM<valueString.size())?MED_TAILLE_PNOM:valueString.size() ;
    tmp_unit.replace(i*MED_TAILLE_PNOM,i*MED_TAILLE_PNOM+lengthString,valueString,0,lengthString);
  }

  // Test if the mesh <_meshName> already exists
  // If it doesn't exists create it
  // If it already exists verify if its space and mesh dimensions are the same
  // as <_ptrMesh->_spaceDimension>, <_ptrMesh->_meshDimension> respectively
  // rem : <_meshName> is the driver meshName not <ptrMesh->_meshName>

  int spaceDimension = MED_FR::MEDdimEspaceLire(_medIdt, const_cast <char *>
						(_meshName.c_str()));

  int meshDimension = MED_FR::MEDdimLire(_medIdt, const_cast <char *>
					 (_meshName.c_str()) );

  if ((spaceDimension != MED_VALID) && (meshDimension != MED_VALID))
    {
      err = MEDmaaCr(_medIdt, const_cast <char *> (_meshName.c_str()),
		     _ptrMesh->_meshDimension, MED_FR::MED_NON_STRUCTURE,
		     const_cast <char *> (_ptrMesh->_description.c_str()));

      if (err != MED_VALID)
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Unable to create Mesh : |" << _meshName << "|"));
      else 
	MESSAGE(LOC<<"Mesh "<<_meshName<<" created in file "<<_fileName<<" !");
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
    rep = MED_FR::MED_CART;
  else if ( coordinateSystem == "CYLINDRICAL")
    rep = MED_FR::MED_CYL;
  else if ( coordinateSystem == "SPHERICAL" )
    rep = MED_FR::MED_SPHER;
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
		    MED_FR::MED_FULL_INTERLACE, _ptrMesh->_numberOfNodes,
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
        
        err =  MEDnumEcr(_medIdt, const_cast <char *> (_meshName.c_str()),
                         const_cast <med_int *> (_ptrMesh->_coordinate->getNodesNumbers() ), 
                         _ptrMesh->_numberOfNodes, MED_FR::MED_NOEUD,
			 MED_FR::med_geometrie_element(0) );

        if (err != MED_VALID) 
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write optionnal numbers of mesh |" << 
				       _meshName.c_str() << "| in file |" <<
				       _fileName  << " |")) ;
      }
      //////////////////////////////////////////////////////////////////////////////////////

  END_OF(LOC);
    
  return MED_VALID;
}




int MED_MESH_WRONLY_DRIVER::writeConnectivities(medEntityMesh entity) const {
  
  const char * LOC="int MED_MESH_WRONLY_DRIVER::writeConnectivities() const : ";
  BEGIN_OF(LOC);

  MED_FR::med_err err;
  
  // REM SI LA METHODE EST APPELEE DIRECTEMENT ET QUE LE MAILLAGE N'EST PAS CREE IL Y A PB !
  // PG : IMPOSSIBLE : LA METHODE EST PRIVEE !
    
    // A FAIRE : A tester surtout dans les methodes de MESH.
    //    if ( _ptrMesh->_connectivity == (CONNECTIVITY *) MED_INVALID )
  if ( _ptrMesh->_connectivity == (CONNECTIVITY *) NULL )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "The connectivity is not defined in the MESH object ")) ;
    
  if   ( _ptrMesh->existConnectivity(MED_NODAL,entity) ) { 

    int numberOfTypes           = _ptrMesh->getNumberOfTypes (entity) ;
    const medGeometryElement  * types = _ptrMesh->getTypes         (entity) ;
    
    for (int i=0; i<numberOfTypes; i++) {
      
      int    numberOfElements = _ptrMesh->getNumberOfElements (entity,types[i]);
      const int * connectivity      = _ptrMesh->getConnectivity     (MED_EN::MED_FULL_INTERLACE, 
								     MED_NODAL, entity, types[i]); // ?? et SI MED_NODAL n'existe pas, recalcul auto ??
      
      // Pour l'instant la class utilise le multi.....
      int multi = 0 ;
      if (entity==MED_EN::MED_CELL)
	if ( (types[i]/ 100) < _ptrMesh->_spaceDimension) 
	  multi=1 ;
      int numberOfNodes = types[i]%100 ;
      int * connectivityArray = new int[numberOfElements*(numberOfNodes+multi)];
      
      // version originale sans prise en compte des numéros optionnels 
      //     
      for (int j=0 ; j<numberOfElements; j++) 
       	{
	  for (int k=0; k<numberOfNodes; k++)
	    connectivityArray[j*(numberOfNodes+multi)+k]=connectivity[j*numberOfNodes+k] ;

	  if (multi>0) connectivityArray[j*(numberOfNodes+multi)+numberOfNodes]=0;
	}
      
      //////////////////////////////////////////////////////////////////////////////////////
      ///  Modification pour prise en compte de la numérotation optionnelle des noeuds   ///
      //////////////////////////////////////////////////////////////////////////////////////
      ///
      /// Dénumérote les sommets des mailles pour leur rendre leurs numéros optionnels
      /// Le traitement est identique à la version originelle s'il n'y a pas de numérotation optionnelle

//  	if (_ptrMesh->_arePresentOptionnalNodesNumbers==1) 
// 		{
// 		const int * nodesNumbers = _ptrMesh->_coordinate->getNodesNumbers();
//       		for (int j=0 ; j<numberOfElements; j++) 
// 			{
// 			for (int k=0; k<numberOfNodes; k++) connectivityArray[j*(numberOfNodes+multi)+k]=nodesNumbers[connectivity[j*numberOfNodes+k]-1] ;
// 			if (multi>0) connectivityArray[j*(numberOfNodes+multi)+numberOfNodes]=0;
// 			}
// 		}
// 	else
// 		{
//       		for (int j=0 ; j<numberOfElements; j++) 
// 			{
// 			for (int k=0; k<numberOfNodes; k++) connectivityArray[j*(numberOfNodes+multi)+k]=connectivity[j*numberOfNodes+k] ;
// 			if (multi>0) connectivityArray[j*(numberOfNodes+multi)+numberOfNodes]=0;
// 			}
// 		}

      //////////////////////////////////////////////////////////////////////////////////////
      
//       err = MEDconnEcr( _medIdt, const_cast <char *> ( _meshName.c_str()), _ptrMesh->_spaceDimension,
// 			connectivityArray, MED_FR::MED_FULL_INTERLACE , numberOfElements,
// 			MED_FR::MED_LECTURE_ECRITURE,
// 			(MED_FR::med_entite_maillage  ) entity, 
// 			(MED_FR::med_geometrie_element) types[i], MED_FR::MED_NOD );

	  err = MEDconnEcr(_medIdt, const_cast <char *> ( _meshName.c_str()),
			   _ptrMesh->_spaceDimension, connectivityArray,
			   MED_FR::MED_FULL_INTERLACE , numberOfElements,
			   (MED_FR::med_entite_maillage  ) entity, 
			   (MED_FR::med_geometrie_element) types[i],
			   MED_FR::MED_NOD);

      delete[] connectivityArray ;

      if (err<0) // ETENDRE LES EXPLICATIONS
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write connectivities of mesh |" << _meshName.c_str() << "| in file |" << _fileName
				     << "| with dimension |"  << _ptrMesh->_spaceDimension <<"| and" 
				     )) ;
    }
  }
  // Connctivity descending :
  if   ( _ptrMesh->existConnectivity(MED_DESCENDING,entity) ) { 
      
    int numberOfTypes           = _ptrMesh->getNumberOfTypes (entity) ;
    const medGeometryElement  * types = _ptrMesh->getTypes         (entity) ;
      
    for (int i=0; i<numberOfTypes; i++) {
	
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

      err = MED_FR::MEDconnEcr(_medIdt,
			       const_cast <char *> ( _meshName.c_str()),
			       _ptrMesh->_spaceDimension,
			       const_cast <int *> (connectivity),
			       MED_FR::MED_FULL_INTERLACE,
			       numberOfElements,
			       (MED_FR::med_entite_maillage  ) entity, 
			       (MED_FR::med_geometrie_element) types[i],
			       MED_FR::MED_DESC );
	
      if (err<0) // ETENDRE LES EXPLICATIONS
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write connectivities of mesh |" << _meshName.c_str() << "| in file |" << _fileName
				     << "| with dimension |"  << _ptrMesh->_spaceDimension <<"| and" 
				     )) ;
            
    }
  }
  END_OF(LOC);
  return MED_VALID;
}

int MED_MESH_WRONLY_DRIVER::writeFamilyNumbers() const {
  
  const char * LOC="int MED_MESH_WRONLY_DRIVER::writeFamilyNumbers() const : ";
  BEGIN_OF(LOC);

  MED_FR::med_err err;
  
  // SOLUTION TEMPORAIRE CAR _ptrMesh->_MEDArrayNodeFamily DOIT ETRE ENLEVER DE LA CLASSE MESH

  { // Node related block
    
    // We build the array from the families list objects :
    int NumberOfNodes = _ptrMesh->getNumberOfNodes() ;
    int * MEDArrayNodeFamily = new int[NumberOfNodes] ;
    // family 0 by default
    for (int i=0; i<NumberOfNodes; i++)
      MEDArrayNodeFamily[i]=0;
    //vector<FAMILY*> myFamilies = _ptrMesh->getFamilies(MED_NODE);
    vector<FAMILY*> * myFamilies = &_ptrMesh->_familyNode;
    int NumberOfNodesFamilies = myFamilies->size() ;
    //bool ToDestroy = false;
    if (0 == NumberOfNodesFamilies) {
      //ToDestroy = true ;
      vector<GROUP*> myGroups = _ptrMesh->getGroups(MED_NODE);
      int NumberOfGroups = myGroups.size() ;
      // build families from groups
      for (int i=0; i<NumberOfGroups; i++) {
	SUPPORT * mySupport = myGroups[i] ;
	FAMILY* myFamily = new FAMILY(*mySupport);
	myFamily->setIdentifier(i+1);
	myFamilies->push_back(myFamily);
      }
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

    for(int j=0; j<NumberOfNodes; j++) {
      SCRUTE(MEDArrayNodeFamily[j]);
    }

//     if ( !_ptrMesh->getIsAGrid() )

    err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
		    MEDArrayNodeFamily, NumberOfNodes, MED_FR::MED_NOEUD,
		    (enum MED_FR::med_geometrie_element) MED_NONE);

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
    //	  delete myFamilies[i];
  }
    
  { // CELLS RELATED BLOCK
    medEntityMesh entity=MED_EN::MED_CELL;
    // SOLUTION TEMPORAIRE CAR _ptrMesh->_MEDArray____Family DOIT ETRE ENLEVER DE LA CLASSE MESH
    if  ( ( _ptrMesh->existConnectivity(MED_NODAL,entity) )|( _ptrMesh->existConnectivity(MED_DESCENDING,entity) ) ) { 

      int numberOfTypes           = _ptrMesh->getNumberOfTypes (entity) ;
      const medGeometryElement  * types = _ptrMesh->getTypes         (entity) ;

      // We build the array from the families list objects :
      int NumberOfElements = _ptrMesh->getNumberOfElements(entity, MED_ALL_ELEMENTS);
      int * MEDArrayFamily = new int[NumberOfElements] ;
      // family 0 by default
      for (int i=0; i<NumberOfElements; i++)
	MEDArrayFamily[i]=0;
      //vector<FAMILY*> myFamilies = _ptrMesh->getFamilies(entity);
      vector<FAMILY*> * myFamilies = &_ptrMesh->_familyCell ;
      int NumberOfFamilies = myFamilies->size() ;
      //bool ToDestroy = false;
      if (0 == NumberOfFamilies) {
	//ToDestroy = true ;
	vector<GROUP*> myGroups = _ptrMesh->getGroups(entity);
	int NumberOfGroups = myGroups.size() ;
	// build families from groups
	for (int i=0; i<NumberOfGroups; i++) {
	  SCRUTE( myGroups[i]->getName() );
	  SUPPORT * mySupport = myGroups[i] ;
	  FAMILY* myFamily = new FAMILY(*mySupport);
	  myFamily->setIdentifier(-i-1);
	  myFamilies->push_back(myFamily);
	}
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

      const int * typeCount = _ptrMesh->getGlobalNumberingIndex(entity) ;

      for (int i=0; i<numberOfTypes; i++) {

	err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
			(MEDArrayFamily+typeCount[i]-1),
			(typeCount[i+1]-typeCount[i]),
			(MED_FR::med_entite_maillage) entity,
			(MED_FR::med_geometrie_element) types[i]); 

	MESSAGE("OK "<<i);
	if ( err != MED_VALID) 
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't write family for the |"<< _ptrMesh->getNumberOfElements(entity, types[i])
				       << "| cells of geometric type |" << MED_FR::geoNames[ (MED_FR::med_geometrie_element) types[i]] <<"|in mesh |"      
				       << _ptrMesh->_name.c_str() << "|" ));   
      }
      delete[] MEDArrayFamily ;
      //if (true == ToDestroy) {
      //  int NumberOfFamilies = myFamilies->size();
      //  for (int i=0; i<NumberOfFamilies; i++)
      //    delete myFamilies[i];
      //}
    }
  }

  { // FACE RELATED BLOCK
    medEntityMesh entity=MED_EN::MED_FACE;
    // SOLUTION TEMPORAIRE CAR _ptrMesh->_MEDArray____Family DOIT ETRE ENLEVER DE LA CLASSE MESH
    if  ( ( _ptrMesh->existConnectivity(MED_NODAL,entity) )|( _ptrMesh->existConnectivity(MED_DESCENDING,entity) ) ) { 

      int numberOfTypes           = _ptrMesh->getNumberOfTypes (entity) ;
      const medGeometryElement  * types = _ptrMesh->getTypes         (entity) ;
      SCRUTE(numberOfTypes);
      
      int numberOfElements = _ptrMesh->getNumberOfElements(entity, MED_ALL_ELEMENTS) ;
      int * familyArray = new int[numberOfElements] ;
      for (int i=0;i<numberOfElements;i++)
	familyArray[i]=0;

      int numberOfFamilies = _ptrMesh->getNumberOfFamilies(entity) ;
      //vector<FAMILY*> myFamilies = _ptrMesh->getFamilies(entity) ;
      vector<FAMILY*> * myFamilies = &_ptrMesh->_familyFace ;
      //bool ToDestroy = false;
      if (0 == numberOfFamilies) {
	//ToDestroy = true ;
	vector<GROUP*> myGroups = _ptrMesh->getGroups(entity);
	int NumberOfGroups = myGroups.size() ;
	// build families from groups
	for (int i=0; i<NumberOfGroups; i++) {
	  SCRUTE( myGroups[i]->getName() );
	  SUPPORT * mySupport = myGroups[i] ;
	  FAMILY* myFamily = new FAMILY(*mySupport);
	  myFamily->setIdentifier(-i-1000);
	  myFamilies->push_back(myFamily);
	}
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

      const int * typeCount = _ptrMesh->getGlobalNumberingIndex(entity) ;

      for (int i=0; i<numberOfTypes; i++) {
	int typeNumberOfElements = typeCount[i+1] - typeCount[i] ;

	err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
			familyArray+typeCount[i]-1, typeNumberOfElements,
			(MED_FR::med_entite_maillage) entity,
			(MED_FR::med_geometrie_element) types[i]); 

	if ( err != MED_VALID) 
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't write family for the |"<< _ptrMesh->getNumberOfElements(entity, types[i])
				       << "| faces of geometric type |" << MED_FR::geoNames[ (MED_FR::med_geometrie_element) types[i]] <<"|in mesh |"      
				       << _ptrMesh->_name.c_str() << "|" ));   
      }
      delete[] familyArray ;
      //if (true == ToDestroy) {
      //  int NumberOfFamilies = myFamilies->size();
      //    for (int i=0; i<NumberOfFamilies; i++)
      //      delete myFamilies[i];
      //}
    }
  }

  { // EDGE RELATED BLOCK
    //medEntityMesh entity=MED_EN::MED_FACE;
    medEntityMesh entity=MED_EN::MED_EDGE;
    // SOLUTION TEMPORAIRE CAR _ptrMesh->_MEDArray____Family DOIT ETRE ENLEVER DE LA CLASSE MESH
    if  ( ( _ptrMesh->existConnectivity(MED_NODAL,entity) )|( _ptrMesh->existConnectivity(MED_DESCENDING,entity) ) ) { 

      int numberOfTypes           = _ptrMesh->getNumberOfTypes (entity) ;
      const medGeometryElement  * types = _ptrMesh->getTypes         (entity) ;
      
      int numberOfElements = _ptrMesh->getNumberOfElements(entity, MED_ALL_ELEMENTS) ;
      int * familyArray = new int[numberOfElements] ;
      for (int i=0;i<numberOfElements;i++)
	familyArray[i]=0;

      int numberOfFamilies = _ptrMesh->getNumberOfFamilies(entity) ;
      //vector<FAMILY*> myFamilies = _ptrMesh->getFamilies(entity) ;
      vector<FAMILY*> * myFamilies = &_ptrMesh->_familyEdge ;
      //bool ToDestroy = false;
      if (0 == numberOfFamilies) {
	//ToDestroy = true ;
	vector<GROUP*> myGroups = _ptrMesh->getGroups(entity);
	int NumberOfGroups = myGroups.size() ;
	// build families from groups
	for (int i=0; i<NumberOfGroups; i++) {
	  SCRUTE( myGroups[i]->getName() );
	  SUPPORT * mySupport = myGroups[i] ;
	  FAMILY* myFamily = new FAMILY(*mySupport);
	  myFamily->setIdentifier(-i-2000);
	  myFamilies->push_back(myFamily);
	}
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

      const int * typeCount = _ptrMesh->getGlobalNumberingIndex(entity) ;

      for (int i=0; i<numberOfTypes; i++) {
	int typeNumberOfElements = typeCount[i+1] - typeCount[i] ;

	err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
			familyArray+typeCount[i]-1, typeNumberOfElements,
			(MED_FR::med_entite_maillage) entity,
			(MED_FR::med_geometrie_element) types[i]); 

	if ( err != MED_VALID) 
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't write family for the |"<< _ptrMesh->getNumberOfElements(entity, types[i])
				       << "| edges of geometric type |" << MED_FR::geoNames[ (MED_FR::med_geometrie_element) types[i]] <<"|in mesh |"      
				       << _ptrMesh->_name.c_str() << "|" ));   
      }
      delete[] familyArray ;
      //if (true == ToDestroy) {
      //  int NumberOfFamilies = myFamilies->size();
      //  for (int i=0; i<NumberOfFamilies; i++)
      //    delete myFamilies[i];
      //}
    }
  }
    
  END_OF(LOC);
  return MED_VALID;
}

int MED_MESH_WRONLY_DRIVER::writeFamilies(vector<FAMILY*> & families ) const {
  
  const char * LOC="int MED_MESH_WRONLY_DRIVER::writeFamilies(vector<FAMILY*> families) const : ";
  BEGIN_OF(LOC);

  MED_FR::med_err err;
  
  MESSAGE(LOC<<" families.size() :"<<families.size());

  for (unsigned int i=0; i< families.size(); i++) {

    int      numberOfAttributes         = families[i]->getNumberOfAttributes ();
    string   attributesDescriptions     = "";

    // Recompose the attributes descriptions arg for MED
    for (int j=0; j < numberOfAttributes; j++) {
        
      string attributeDescription = families[i]->getAttributeDescription(j+1);
        
      if ( attributeDescription.size() > MED_TAILLE_DESC )
	throw MEDEXCEPTION( LOCALIZED(STRING(LOC) << "The size of the attribute description n° |" << j+1 << "| of the family |" << families[i]->getName()
				      << "| with identifier |" << families[i]->getIdentifier()  << "| is |" 
				      <<  attributeDescription.size()  <<"| and is more than |" <<  MED_TAILLE_DESC << "|")) ;
        
      attributesDescriptions += attributeDescription;
    }
      

    int      numberOfGroups  = families[i]->getNumberOfGroups();
    string   groupsNames(numberOfGroups*MED_TAILLE_LNOM,'\0') ;
    // Recompose the groups names arg for MED
    for (int j=0; j < numberOfGroups; j++) {

      string groupName = families[i]->getGroupName(j+1);
       
      if ( groupName.size() > MED_TAILLE_LNOM )
	throw MEDEXCEPTION( LOCALIZED(STRING(LOC) << "The size of the group name  n° |" << j+1 << "| of the family |" << families[i]->getName()
				      << "| with identifier |" << families[i]->getIdentifier()  << "| is |" 
				      <<  groupName.size()  <<"| and is more than |" << MED_TAILLE_LNOM << "|")) ;
        

      int length = min(MED_TAILLE_LNOM,(int)groupName.size());
      groupsNames.replace(j*MED_TAILLE_LNOM,length, groupName,0,length);
      
    }

    // test if the family already exists (HDF trick waiting a MED evolution to be replaced)
    string dataGroupFam = "/ENS_MAA/"+_meshName+"/FAS/"+families[i]->getName()+"/";  
    SCRUTE("|"<<dataGroupFam<<"|");
    err =_MEDdatagroupOuvrir(_medIdt,const_cast <char *> (dataGroupFam.c_str()) ) ;
    if ( err < MED_VALID ) {
      SCRUTE(err);

      MESSAGE(LOC<<"families[i]->getName().c_str() : "<<families[i]->getName().c_str());
      MESSAGE(LOC<<"_meshName.c_str() : "<<_meshName.c_str());
      MESSAGE(LOC<<"families[i]->getIdentifier() : "<<families[i]->getIdentifier());
      MESSAGE(LOC<<"numberOfAttributes : "<<numberOfAttributes);
	
      //MESSAGE(LOC<<"families[i]->getAttributesIdentifiers() : "<<families[i]->getAttributesIdentifiers()[0]);
      //MESSAGE(LOC<<"families[i]->getAttributesValues() : "<<families[i]->getAttributesValues()[0]);
      MESSAGE(LOC<<"attributesDescriptions.c_str() : "<<attributesDescriptions.c_str());
      MESSAGE(LOC<<"numberOfGroups : "<<numberOfGroups);
      MESSAGE(LOC<<"groupsNames.c_str() : "<<groupsNames.c_str());

      err = MED_FR::MEDfamCr( _medIdt, 
			      const_cast <char *> ( _meshName.c_str() ),
			      const_cast <char *> ( families[i]->getName().c_str() ),
			      families[i]->getIdentifier(), 
			      families[i]->getAttributesIdentifiers(),
			      families[i]->getAttributesValues(),
			      const_cast <char *> (attributesDescriptions.c_str()), 
			      numberOfAttributes,  
			      const_cast <char *> (groupsNames.c_str()), 
			      numberOfGroups);
      SCRUTE(err);
      if ( err != MED_VALID) 
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't create family |" << families[i]->getName()
				     << "| with identifier |" << families[i]->getIdentifier()  << "| groups names |" 
				     << groupsNames  <<"| and  attributes descriptions |" << attributesDescriptions << "|")) ;
    }
    else
      _MEDdatagroupFermer(_medIdt);


  }

  END_OF(LOC);
    
  return MED_VALID;
}


// A FAIRE POUR LES NOEUDS ET LES ELEMENTS ret = MEDfamEcr(



/*--------------------- RDWR PART -------------------------------*/

MED_MESH_RDWR_DRIVER::MED_MESH_RDWR_DRIVER():MED_MESH_DRIVER()
{
}

MED_MESH_RDWR_DRIVER::MED_MESH_RDWR_DRIVER(const string & fileName,
					   MESH * ptrMesh):
  MED_MESH_DRIVER(fileName,ptrMesh,MED_RDWR)
{
  MESSAGE("MED_MESH_RDWR_DRIVER::MED_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}

MED_MESH_RDWR_DRIVER::MED_MESH_RDWR_DRIVER(const MED_MESH_RDWR_DRIVER & driver): 
  MED_MESH_RDONLY_DRIVER::MED_MESH_DRIVER(driver)
{
}

MED_MESH_RDWR_DRIVER::~MED_MESH_RDWR_DRIVER() {
  //MESSAGE("MED_MESH_RDWR_DRIVER::MED_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) has been destroyed");
} 
  
GENDRIVER * MED_MESH_RDWR_DRIVER::copy(void) const
{
  return new MED_MESH_RDWR_DRIVER(*this);
}

void MED_MESH_RDWR_DRIVER::write(void) const
  throw (MEDEXCEPTION)
{
  MED_MESH_WRONLY_DRIVER::write();
}
void MED_MESH_RDWR_DRIVER::read (void)
  throw (MEDEXCEPTION)
{
  MED_MESH_RDONLY_DRIVER::read();
}

