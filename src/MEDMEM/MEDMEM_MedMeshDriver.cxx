#include "MEDMEM_MedMeshDriver.hxx"

#include "MEDMEM_DriversDef.hxx"

#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_CellModel.hxx"

extern "C" {
  extern med_idt _MEDdatagroupOuvrir(med_idt pid, char *nom);
  extern med_err _MEDdatagroupFermer(med_idt id);
}
using namespace MED_FR;

// Every memory allocation made in the MedDriver members function are desallocated in the Mesh destructor 

void    MED_MESH_DRIVER::setMeshName(const string & meshName) { _meshName = meshName; };
string  MED_MESH_DRIVER::getMeshName() const { return _meshName; };

//---------------------------------- RDONLY PART -------------------------------------------------------------

//A FAIRE UTILISER LES MAPS...
const MED_FR::med_geometrie_element  MED_MESH_DRIVER::all_cell_type[MED_NBR_GEOMETRIE_MAILLE]=
  { MED_FR::MED_POINT1,MED_FR::MED_SEG2,MED_FR::MED_SEG3,MED_FR::MED_TRIA3,MED_FR::MED_QUAD4,MED_FR::MED_TRIA6,MED_FR::MED_QUAD8,
    MED_FR::MED_TETRA4,MED_FR::MED_PYRA5,MED_FR::MED_PENTA6,MED_FR::MED_HEXA8,MED_FR::MED_TETRA10,MED_FR::MED_PYRA13,
    MED_FR::MED_PENTA15, MED_FR::MED_HEXA20};

const char * const MED_MESH_DRIVER::all_cell_type_tab [MED_NBR_GEOMETRIE_MAILLE]=
  { "MED_POINT1","MED_SEG2","MED_SEG3","MED_TRIA3","MED_QUAD4","MED_TRIA6","MED_QUAD8",
    "MED_TETRA4","MED_PYRA5","MED_PENTA6","MED_HEXA8","MED_TETRA10","MED_PYRA13",
    "MED_PENTA15","MED_HEXA20"};




void MED_MESH_RDONLY_DRIVER::read(void)
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER::read() : " ;
  BEGIN_OF(LOC);
  if (_status!=MED_OPENED)
    throw MEDEXCEPTION(" ");
    //throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "The _idt of file " << _fileName << " is : " << _medIdt <<  " (the file is not opened)." )) ;

  _ptrMesh->_name =  _meshName;
  
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
  _ptrMesh->_numberOfNodesGroups = _ptrMesh->_groupNode.size() ;
  // on cell
  buildAllGroups(_ptrMesh->_groupCell,_ptrMesh->_familyCell) ;
  _ptrMesh->_numberOfCellsGroups = _ptrMesh->_groupCell.size() ;
  // on face
  buildAllGroups(_ptrMesh->_groupFace,_ptrMesh->_familyFace) ;
  _ptrMesh->_numberOfFacesGroups = _ptrMesh->_groupFace.size() ;
  // on edge
  buildAllGroups(_ptrMesh->_groupEdge,_ptrMesh->_familyEdge) ;
  _ptrMesh->_numberOfEdgesGroups = _ptrMesh->_groupEdge.size() ;

  END_OF(LOC);
}


// A FAIRE : RENVOYER DU VOID
int  MED_MESH_RDONLY_DRIVER::getCOORDINATE()
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER::getCOORDINATE() : " ;
  BEGIN_OF(LOC);

  if (_status==MED_OPENED)
    {
      int err ;
      
      // Read the dimension of the space for the mesh <_meshName>
      // to be able to create a COORDINATE object
      int SpaceDimension = MEDdimLire(_medIdt,const_cast <char *> (_meshName.c_str())) ;
      if ( SpaceDimension  <= MED_VALID ) 
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"The space dimension |" << SpaceDimension << "| seems to be incorrect "
                                     << "for the mesh : |" << _meshName << "|")) ;
      _ptrMesh->_spaceDimension = SpaceDimension ;

      

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
      _ptrMesh->_coordinate = new COORDINATE(MED_EN::MED_FULL_INTERLACE, SpaceDimension, NumberOfNodes);
      
      MED_FR::med_repere rep ; // ATTENTION ---> DOIT ETRE INTEGRE DS MESH EF: FAIT NON?
      string tmp_nom_coord (MED_TAILLE_PNOM*(_ptrMesh->_spaceDimension)+1,'\0');
      string tmp_unit_coord(MED_TAILLE_PNOM*(_ptrMesh->_spaceDimension)+1,'\0');
      char * tmp_nom = (const_cast <char *> ( tmp_nom_coord.c_str())  ) ;
      char * tmp_unit= (const_cast <char *> ( tmp_unit_coord.c_str()) ) ;

      err=MEDcoordLire(_medIdt,
                       const_cast <char *> (_ptrMesh->_name.c_str()),
		       _ptrMesh->_spaceDimension,
		       const_cast <double *> ( _ptrMesh->_coordinate->_coordinate->get(MED_EN::MED_FULL_INTERLACE) ),
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
	// PG : What about blank !!!!!
	_ptrMesh->_coordinate->_coordinateName[i]=string(tmp_nom,i*MED_TAILLE_PNOM,MED_TAILLE_PNOM) ;
	_ptrMesh->_coordinate->_coordinateUnit[i]=string(tmp_unit,i*MED_TAILLE_PNOM,MED_TAILLE_PNOM);
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

      // Read the unused optionnal node Names
      char * tmp_node_name = new char[NumberOfNodes*MED_TAILLE_PNOM+1];
      tmp_node_name[NumberOfNodes]='\0' ;
      err=MEDnomLire(_medIdt,const_cast <char*> (_ptrMesh->_name.c_str()),
		     tmp_node_name,NumberOfNodes*MED_TAILLE_PNOM,MED_FR::MED_NOEUD,
		     (MED_FR::med_geometrie_element) MED_NONE);
      if (err == MED_VALID) 
        INFOS(LOC<<"MED_MESH_RDONLY_DRIVER::getNoeuds() : WARNING : Nodes have names but we do not read them !");
      delete[] tmp_node_name ;


      // ??? Read the unused optionnal node Numbers ???
      int * tmp_node_number = new int[NumberOfNodes] ;
      err=MEDnumLire(_medIdt,const_cast <char*> (_ptrMesh->_name.c_str()),
		     tmp_node_number,NumberOfNodes,MED_NOEUD,(MED_FR::med_geometrie_element)0);
      if (err == MED_VALID) {
        INFOS(LOC<<"WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING");
        INFOS(LOC<<"MED_MESH_RDONLY_DRIVER::getNoeuds() : WARNING : Nodes have numbers but we do not take care of them !");
        INFOS(LOC<<"WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING");
	_ptrMesh->_coordinate->_nodeNumber = tmp_node_number ;
      } else
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
      for (int i=1;i<MED_NBR_GEOMETRIE_MAILLE;i++) {                       // EF :ON SCANNE DES GEOMETRIES INUTILES, UTILISER LES MAPS
	tmp_cells_count[i]=MEDnEntMaa(_medIdt,(const_cast <char *> (_ptrMesh->_name.c_str())),
				      MED_FR::MED_CONN,(MED_FR::med_entite_maillage) Entity,
                                      all_cell_type[i],MED_FR::MED_NOD); 

        // We suppose there is no cells used as faces, this is forbidden !!!

	// Only in version 2.2.x of MED, but before, it's right :-(

	if (tmp_cells_count[i]>0) { 
	  Connectivity->_entityDimension=all_cell_type[i]/100;  
	  Connectivity->_numberOfTypes++;
	}
      }
      

      // well if there is nothing, we quit !
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

      // we get version number
//        string medVersion(version_med);
//        int firstNumber = 
      int * tmp_edges_count = new int[MED_NBR_GEOMETRIE_MAILLE] ;
      int numberOfEdgesTypes = 0;
      int * tmp_faces_count = new int[MED_NBR_GEOMETRIE_MAILLE] ;
      int numberOfFacesTypes = 0;
      if ((version_med != "2.2")&(Entity==MED_MAILLE)) {
	Connectivity->_numberOfTypes=0;
	for (int i=1;i<MED_NBR_GEOMETRIE_MAILLE;i++) {
	  tmp_faces_count[i]=0;
	  tmp_edges_count[i]=0;
	  if (tmp_cells_count[i]!=0) {
	    int dimension = all_cell_type[i]/100 ;
	    if (Connectivity->_entityDimension==dimension) 
	      Connectivity->_numberOfTypes++ ;
	    
	    if (dimension == 2)
	      if (Connectivity->_entityDimension==3) {
		tmp_faces_count[i]=tmp_cells_count[i] ;
		tmp_cells_count[i]=0 ;
		numberOfFacesTypes++;
	      }
	    if (dimension == 1)
	      if (Connectivity->_entityDimension>dimension) {
		tmp_edges_count[i]=tmp_cells_count[i] ;
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
	int TypeNumber=1 ;
	for (int i=1;i<MED_NBR_GEOMETRIE_MAILLE;i++)  { // no point1 cell type (?)
	  if (tmp_cells_count[i]>0) {
	    
	    Connectivity->_count[TypeNumber]=Connectivity->_count[TypeNumber-1]+tmp_cells_count[i];
	    
	    CELLMODEL t( (MED_EN::medGeometryElement) MED_MESH_DRIVER::all_cell_type[i]) ;
	    Connectivity->_type[TypeNumber-1]=t ;
	    
	    Connectivity->_geometricTypes[TypeNumber-1]=( MED_EN::medGeometryElement) MED_MESH_DRIVER::all_cell_type[i] ;
	    
	    // probleme avec les mailles de dimension < a dimension du maillage :
	    // Il faut oter le zero a la lecture est le remettre a l'ecriture : ce n'est pas fait !!!!! On interdit ce cas pour l'instant !!!

	      
	    size+=tmp_cells_count[i]*((MED_MESH_DRIVER::all_cell_type[i])%100) ;
	    
	    MESSAGE(LOC
		    << Connectivity->_count[TypeNumber]-1 << " cells of type " 
		    << all_cell_type_tab[i] ); 
	    TypeNumber++;
	  }
	}
	
	// Creation of the MEDSKYLINEARRAY
	Connectivity->_nodal = new MEDSKYLINEARRAY(Connectivity->_count[Connectivity->_numberOfTypes]-1,size) ; 
	int * NodalIndex = Connectivity->_nodal->getIndex() ;
	NodalIndex[0]=1 ;
	
	// Fill the MEDSKYLINEARRAY by reading the MED file.
	for (int i=0;i<Connectivity->_numberOfTypes;i++) {
	  int multi = 0 ;
	  MED_FR::med_geometrie_element med_type = (MED_FR::med_geometrie_element) Connectivity->_type[i].getType() ;
//  	  if ( Connectivity->_type[i].getDimension() < Connectivity->_entityDimension) 
	  if (Connectivity->_entity == MED_CELL)
	    if ( Connectivity->_type[i].getDimension() < _ptrMesh->_spaceDimension) 
	      multi=1;
	  
	  //	  int NumberOfCell = Connectivity->_count[i+1]-Connectivity->_count[i] ;
	  int NumberOfNodeByCell = Connectivity->_type[i].getNumberOfNodes() ;
	  
	  // initialise index
	  for (int j=Connectivity->_count[i]; j<Connectivity->_count[i+1];j++)
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
	    delete[] tmp_faces_count;
	    delete[] tmp_edges_count;
	    MESSAGE(LOC<<": MEDconnLire returns "<<err) ;
	    return MED_ERROR ;
	  }
	  int * ConnectivityArray = Connectivity->_nodal->getI(Connectivity->_count[i]) ;
	  for (int j=0; j<tmp_numberOfCells; j++)
	    for (int k=0; k<NumberOfNodeByCell; k++) 
	      ConnectivityArray[j*NumberOfNodeByCell+k]=tmp_ConnectivityArray[j*(NumberOfNodeByCell+multi)+k] ;

	  delete[] tmp_ConnectivityArray;
  
	}
      } // end of bloc to read CELL

      delete[] tmp_cells_count; 

      // get Face if any
      // ===============

      if (numberOfFacesTypes!=0) {
	CONNECTIVITY * constituent = new CONNECTIVITY(numberOfFacesTypes,MED_EN::MED_FACE) ;
	constituent->_entityDimension = 2 ;
	constituent->_count[0]=1 ;

	int size = 0 ; 
	int size_more_one = 0 ; 
	int TypeNumber=1 ;
	for (int i=1;i<MED_NBR_GEOMETRIE_MAILLE;i++)  { // no point1 cell type (?)
	  if (tmp_faces_count[i]>0) {
	    
	    constituent->_count[TypeNumber]=constituent->_count[TypeNumber-1]+tmp_faces_count[i];
	    CELLMODEL t( (MED_EN::medGeometryElement) MED_MESH_DRIVER::all_cell_type[i]) ;
	    constituent->_type[TypeNumber-1]=t ;
	    
	    constituent->_geometricTypes[TypeNumber-1]=( MED_EN::medGeometryElement) MED_MESH_DRIVER::all_cell_type[i] ;
	    
	    size+=tmp_faces_count[i]*((MED_MESH_DRIVER::all_cell_type[i])%100) ;
	    TypeNumber++;
	  }
	}
	
	// Creation of the MEDSKYLINEARRAY
	constituent->_nodal = new MEDSKYLINEARRAY(constituent->_count[constituent->_numberOfTypes]-1,size) ; 
	int * NodalIndex = constituent->_nodal->getIndex() ;
	NodalIndex[0]=1 ;
	
	// Fill the MEDSKYLINEARRAY by reading the MED file.
	for (int i=0;i<constituent->_numberOfTypes;i++) {
	  MED_FR::med_geometrie_element med_type = (MED_FR::med_geometrie_element) constituent->_type[i].getType() ;

	  int NumberOfNodeByFace = constituent->_type[i].getNumberOfNodes() ;
	  
	  // initialise index
	  for (int j=constituent->_count[i]; j<constituent->_count[i+1];j++)
	    NodalIndex[j]=NodalIndex[j-1]+NumberOfNodeByFace ; 
	  
	  int tmp_numberOfFaces = constituent->_count[i+1]-constituent->_count[i] ;
	  // Il faut ajouter 1 pour le zero a la lecture !!!
	  int * tmp_constituentArray = new int[(NumberOfNodeByFace+1)*tmp_numberOfFaces] ;
	  
	  int err=MEDconnLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
			      Connectivity->_entityDimension,tmp_constituentArray,
			      MED_FR::MED_FULL_INTERLACE,NULL,0,MED_FR::MED_MAILLE,med_type,MED_FR::MED_NOD);
	  if ( err != MED_VALID) {
	    MESSAGE(LOC<<": MEDconnLire returns "<<err) ;
	    delete constituent ;
	    delete[] tmp_constituentArray;
	    delete[] tmp_faces_count;
	    delete[] tmp_edges_count;
	    return MED_ERROR ;
	  }

	  int * constituentArray = constituent->_nodal->getI(constituent->_count[i]) ;
	  for (int j=0; j<tmp_numberOfFaces; j++)
	    for (int k=0; k<NumberOfNodeByFace; k++)
	      constituentArray[j*NumberOfNodeByFace+k]=tmp_constituentArray[j*(NumberOfNodeByFace+1)+k] ;
	  
	  delete[] tmp_constituentArray;
	}
	
	Connectivity->_constituent = constituent ;
      }

      delete[] tmp_faces_count;

      // get Edge if any
      // ===============
      if (numberOfEdgesTypes!=0) {
	CONNECTIVITY * constituent = new CONNECTIVITY(numberOfEdgesTypes,MED_EDGE) ;
	constituent->_entityDimension = 2 ;
	constituent->_count[0]=1 ;

	int size = 0 ; 
	int size_more_one = 0 ; 
	int TypeNumber=1 ;
	for (int i=1;i<MED_NBR_GEOMETRIE_MAILLE;i++)  { // no point1 cell type (?)
	  if (tmp_edges_count[i]>0) {
	    
	    constituent->_count[TypeNumber]=constituent->_count[TypeNumber-1]+tmp_edges_count[i];
	    CELLMODEL t( (MED_EN::medGeometryElement) MED_MESH_DRIVER::all_cell_type[i]) ;
	    constituent->_type[TypeNumber-1]=t ;
	    
	    constituent->_geometricTypes[TypeNumber-1]=( MED_EN::medGeometryElement) MED_MESH_DRIVER::all_cell_type[i] ;
	    
	    size+=tmp_edges_count[i]*((MED_MESH_DRIVER::all_cell_type[i])%100) ;
	    TypeNumber++;
	  }
	}
	
	// Creation of the MEDSKYLINEARRAY
	constituent->_nodal = new MEDSKYLINEARRAY(constituent->_count[constituent->_numberOfTypes]-1,size) ; 
	int * NodalIndex = constituent->_nodal->getIndex() ;
	NodalIndex[0]=1 ;
	
	// Fill the MEDSKYLINEARRAY by reading the MED file.
	for (int i=0;i<constituent->_numberOfTypes;i++) {
	  MED_FR::med_geometrie_element med_type = (MED_FR::med_geometrie_element) constituent->_type[i].getType() ;

	  int NumberOfNodeByEdge = constituent->_type[i].getNumberOfNodes() ;
	  
	  // initialise index
	  for (int j=constituent->_count[i]; j<constituent->_count[i+1];j++)
	    NodalIndex[j]=NodalIndex[j-1]+NumberOfNodeByEdge ; 
	  
	  int tmp_numberOfEdges = constituent->_count[i+1]-constituent->_count[i] ;
	  // Il faut ajouter 1 pour le zero a la lecture !!!
	  int * tmp_constituentArray = new int[(NumberOfNodeByEdge+1)*tmp_numberOfEdges] ;
	  
	  int err=MEDconnLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
			      Connectivity->_entityDimension,tmp_constituentArray,
			      MED_FR::MED_FULL_INTERLACE,NULL,0,MED_FR::MED_MAILLE,med_type,MED_FR::MED_NOD);
	  if ( err != MED_VALID) {
	    MESSAGE(LOC<<": MEDconnLire returns "<<err) ;
	    delete constituent ;
	    delete[] tmp_constituentArray;
	    delete[] tmp_edges_count;
	    return MED_ERROR ;
	  }

	  int * constituentArray = constituent->_nodal->getI(constituent->_count[i]) ;
	  for (int j=0; j<tmp_numberOfEdges; j++)
	    for (int k=0; k<NumberOfNodeByEdge; k++)
	      constituentArray[j*NumberOfNodeByEdge+k]=tmp_constituentArray[j*(NumberOfNodeByEdge+1)+k] ;
	  
	  delete[] tmp_constituentArray;
	}

	if (Connectivity->_entityDimension == 3) {
	  if (Connectivity->_constituent==NULL)
	    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Edges are defined but there are no Faces !"));
	  Connectivity->_constituent->_constituent = constituent ;
	} else 
	  Connectivity->_constituent = constituent ;
      }

      delete[] tmp_edges_count; 
      
      return MED_VALID;
    }
  return MED_ERROR;
}

int MED_MESH_RDONLY_DRIVER::getDescendingConnectivity(CONNECTIVITY * Connectivity) 
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER::getDescendingConnectivity " ;
  if (_status==MED_OPENED)
    {
      MESSAGE(LOC<<" Not implemented !");
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

  if (_status==MED_OPENED) {
    int err = 0 ;
    // read number :
    // NODE :
    err = getNodesFamiliesNumber() ; // error only if (_status!=MED_OPENED), other case exeception !
    // CELL
    _ptrMesh->_MEDArrayCellFamily = new (int*)[_ptrMesh->_connectivity->_numberOfTypes] ; // ET SI IL N'Y A PAS DE CELLS ?
    getCellsFamiliesNumber(_ptrMesh->_MEDArrayCellFamily,_ptrMesh->_connectivity) ;
    if (_ptrMesh->_connectivity->_constituent != NULL) {
      if (_ptrMesh->_connectivity->_constituent->_entity == MED_EN::MED_FACE) {
	// FACE
	_ptrMesh->_MEDArrayFaceFamily = new (int*)[_ptrMesh->_connectivity->_constituent->_numberOfTypes] ;
	getCellsFamiliesNumber(_ptrMesh->_MEDArrayFaceFamily,_ptrMesh->_connectivity->_constituent) ;
      } else {
	// EDGE in 2D
	_ptrMesh->_MEDArrayEdgeFamily = new (int*)[_ptrMesh->_connectivity->_constituent->_numberOfTypes] ;
	getCellsFamiliesNumber(_ptrMesh->_MEDArrayEdgeFamily,_ptrMesh->_connectivity->_constituent) ;
      }
      // EDGE in 3D
      if (_ptrMesh->_connectivity->_constituent->_constituent != NULL) {
	_ptrMesh->_MEDArrayEdgeFamily = new (int*)[_ptrMesh->_connectivity->_constituent->_constituent->_numberOfTypes] ;
	getCellsFamiliesNumber(_ptrMesh->_MEDArrayEdgeFamily,_ptrMesh->_connectivity->_constituent) ; // we are in 3D !
      }
    }

    // Creation of the families
    int NumberOfFamilies = MEDnFam(_medIdt,const_cast <char *> (_meshName.c_str()),0,MED_FR::MED_FAMILLE) ;
    if ( NumberOfFamilies < 1 ) // at least family 0 must exist 
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"There is no FAMILY, FAMILY 0 must exists" ));

    vector<FAMILY*> &NodeFamilyVector = _ptrMesh->_familyNode ;
    vector<FAMILY*> &CellFamilyVector = _ptrMesh->_familyCell ;
    vector<FAMILY*> &FaceFamilyVector = _ptrMesh->_familyFace ;
    vector<FAMILY*> &EdgeFamilyVector = _ptrMesh->_familyEdge ;

    int numberOfNodesFamilies = 0 ;
    int numberOfCellsFamilies = 0 ;
    int numberOfFacesFamilies = 0 ;
    int numberOfEdgesFamilies = 0 ;

    for (int i=0;i<NumberOfFamilies;i++) {
      
      int NumberOfAttributes = MEDnFam(_medIdt,const_cast <char *> (_meshName.c_str()),i+1,MED_FR::MED_ATTR) ;
      if (NumberOfAttributes < 0) 
	throw MEDEXCEPTION("MED_MESH_RDONLY_DRIVER::getFAMILY() : NumberOfAttributes" );
    
      int NumberOfGroups = MEDnFam(_medIdt,const_cast <char *> (_meshName.c_str()),i+1,MED_FR::MED_GROUPE) ;
      if (NumberOfGroups < 0)
	throw MEDEXCEPTION("MED_MESH_RDONLY_DRIVER::getFAMILY() : NumberOfGroups" );
      
      int FamilyIdentifier ;
      string FamilyName(MED_TAILLE_NOM,' ');
      int *  AttributesIdentifier = new int[NumberOfAttributes] ;
      int *  AttributesValues     = new int[NumberOfAttributes] ;
      string AttributesDescription(MED_TAILLE_DESC*NumberOfAttributes,' ') ;
      string GroupsNames(MED_TAILLE_LNOM*NumberOfGroups,' ') ;
      err = MEDfamInfo(_medIdt,const_cast <char *> (_meshName.c_str()),
		       i+1,const_cast <char *> (FamilyName.c_str()),
		       &FamilyIdentifier,AttributesIdentifier,AttributesValues,
		       const_cast <char *> (AttributesDescription.c_str()),
		       &NumberOfAttributes,
		       const_cast <char *> (GroupsNames.c_str()),&NumberOfGroups
		       );
      //SCRUTE(GroupsNames);
      if (err != MED_VALID)
	throw MEDEXCEPTION("MED_MESH_RDONLY_DRIVER::getFAMILY() : ERROR when get FAMILY informations" );
      if (FamilyIdentifier != 0 ) {
	FAMILY * Family = new FAMILY(_ptrMesh,FamilyIdentifier,FamilyName,
                                     NumberOfAttributes,AttributesIdentifier,
				     AttributesValues,AttributesDescription,
                                     NumberOfGroups,GroupsNames) ;
	switch (Family->getEntity()) {
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
      }
    }
    _ptrMesh->_numberOfNodesFamilies = numberOfNodesFamilies ;
    _ptrMesh->_numberOfCellsFamilies = numberOfCellsFamilies ;
    _ptrMesh->_numberOfFacesFamilies = numberOfFacesFamilies ;
    _ptrMesh->_numberOfEdgesFamilies = numberOfEdgesFamilies ;
    
    END_OF(LOC);
    return MED_VALID ;
  }
  return MED_ERROR;
}

int  MED_MESH_RDONLY_DRIVER::getNodesFamiliesNumber() 
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER::getNodesFamiliesNumber() : " ;
  BEGIN_OF(LOC);
  if (_status==MED_OPENED) {
    int err = 0 ;
    int * tmp_NodesFamilies = new int[_ptrMesh->getNumberOfNodes()] ;
    err = MEDfamLire(_medIdt,(const_cast <char *> (_ptrMesh->_name.c_str())), 
		     tmp_NodesFamilies, _ptrMesh->getNumberOfNodes(),
		     MED_NOEUD,(enum MED_FR::med_geometrie_element) MED_NONE);
    if ( err != MED_VALID) {
      delete[] tmp_NodesFamilies ;
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "There is no family for the |"<< _ptrMesh->getNumberOfNodes() 
                                   << "| nodes in mesh |" 
                                   << _ptrMesh->_name.c_str() << "|" ));
    }
    _ptrMesh->_MEDArrayNodeFamily = tmp_NodesFamilies ;
    END_OF(LOC);
    return MED_VALID;
  }
  return MED_ERROR;
}

int  MED_MESH_RDONLY_DRIVER::getCellsFamiliesNumber(int **MEDArrayFamily,CONNECTIVITY *Connectivity) 
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER::getCellsFamiliesNumber " ;
  BEGIN_OF(LOC);

  if (_status==MED_OPENED) {
    int i, err = 0 ;
    for (i=0;i<Connectivity->_numberOfTypes;i++)	{
      int NumberOfCell = Connectivity->_count[i+1]-Connectivity->_count[i] ;
      int * fam = new int[NumberOfCell] ;

      err=MEDfamLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
                     fam,NumberOfCell,
                     (MED_FR::med_entite_maillage) Connectivity->_entity,
                     (MED_FR::med_geometrie_element) Connectivity->_geometricTypes[i]);

      // provisoire : si les faces ou les aretes sont des mailles !!!
      if (err != MED_VALID) {
	MESSAGE(LOC<<"search face/edge family on cell !!!");
	err=MEDfamLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
		       fam,NumberOfCell,
		       MED_FR::MED_MAILLE,
		       (MED_FR::med_geometrie_element) Connectivity->_geometricTypes[i]);
      }

      MEDArrayFamily[i]=fam ;
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
    //    GROUP * myGroup = new GROUP((*currentGroup).first,(*currentGroup).second) ;
    GROUP * myGroup = new GROUP() ;
    myGroup->setName((*currentGroup).first);
    SCRUTE(myGroup->getName());
    //myGroup->setMesh(_ptrMesh);
    myGroup->init((*currentGroup).second);
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


void MED_MESH_RDONLY_DRIVER::write( void ) const { INFOS("MED_MESH_RDONLY_DRIVER::write : Can't write with a RDONLY driver !");}

/*--------------------- RDWR PART -------------------------------*/

void MED_MESH_RDWR_DRIVER::write(void) const { MED_MESH_WRONLY_DRIVER::write(); } 
void MED_MESH_RDWR_DRIVER::read (void)       { MED_MESH_RDONLY_DRIVER::read(); } 


/*--------------------- WRONLY PART -------------------------------*/
void MED_MESH_WRONLY_DRIVER::read (void)       {   INFOS("MED_MESH_WRONLY_DRIVER::write : Can't read with a WRONLY driver !");} 

void MED_MESH_WRONLY_DRIVER::write(void) const { 
  const char * LOC = "void MED_MESH_WRONLY_DRIVER::write(void) const : ";
  BEGIN_OF(LOC);

  // we must first create mesh !!
  MESSAGE(LOC<< "MeshName : "<< _meshName <<"FileName : "<<_fileName<<" MedIdt : "<< _medIdt);

  if (writeCoordinates()!=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeCoordinates()"  )) ;

  if (writeConnectivities(MED_EN::MED_CELL)!=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeConnectivities(MED_CELL)"  )) ;
  if (writeConnectivities(MED_EN::MED_FACE)!=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeConnectivities(MED_FACE)"  )) ;
  if (writeConnectivities(MED_EN::MED_EDGE)!=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeConnectivities(MED_EDGE)"  )) ;

  if (writeFamilyNumbers() !=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeFamilyNumbers()"  )) ;
 
  // well we must first write zero family :
  if (_status==MED_OPENED) {
    int err ;
    // test if the family already exists (HDF trick waiting a MED evolution to be replaced)
    string dataGroupFam = "/ENS_MAA/"+_meshName+"/FAS/FAMILLE_0/";  
    SCRUTE("|"<<dataGroupFam<<"|");
    if ( err =_MEDdatagroupOuvrir(_medIdt,const_cast <char *> (dataGroupFam.c_str()) ) < MED_VALID ) {
      SCRUTE(err);
      
      err = MEDfamCr( _medIdt, const_cast <char *> ( _meshName.c_str() ),
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


int MED_MESH_WRONLY_DRIVER::writeCoordinates() const {
 
  const char * LOC = "int MED_MESH_WRONLY_DRIVER::writeCoordinates() const : ";
  BEGIN_OF(LOC);
  if (_status==MED_OPENED) {

    MED_FR::med_err err = MED_ERROR;
    MED_FR::med_repere rep;
    string tmp_name,tmp_unit;
    
    // Recompose the <_spaceDimension> strings in 1 string 
    for (int i=0;i<_ptrMesh->_spaceDimension;i++) {
      tmp_name += _ptrMesh->_coordinate->_coordinateName[i]; 
      tmp_unit += _ptrMesh->_coordinate->_coordinateUnit[i]; 

    }

    // Test if the mesh <_meshName> already exists
    // If it doesn't exists create it
    // If it already exists verify if its dimension is the same as <_ptrMesh->_spaceDimension>
    // rem : <_meshName> is the driver meshName not <ptrMesh->_meshName>
    int dim = MEDdimLire(_medIdt, const_cast <char *> (_meshName.c_str()) );
    if (dim < MED_VALID) 
      if (MEDmaaCr(_medIdt,const_cast <char *> (_meshName.c_str()),_ptrMesh->_spaceDimension) != 0 )
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Unable to create Mesh"));
      else 
	{
	  MESSAGE(LOC<<"Mesh "<<_meshName<<" created in file "<<_fileName<<" !");
	}
    else if (dim != _ptrMesh->_spaceDimension) 
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Mesh |" << _meshName.c_str() << "| already exists in file |" << _fileName
                                   << "| with dimension |" << dim << "| but the dimension of the mesh we want to write is |"
                                   << _ptrMesh->_spaceDimension <<"|" )) ;
    
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
      
    err = MEDcoordEcr(_medIdt, const_cast <char *> (_meshName.c_str()),
                      _ptrMesh->_spaceDimension, 
                      const_cast <double *> ( _ptrMesh->_coordinate->_coordinate->get(MED_EN::MED_FULL_INTERLACE) ), 
                      MED_FR::MED_FULL_INTERLACE, 
                      _ptrMesh->_numberOfNodes,                 //  _ptrMesh->_coordinate->_numberOfNodes
                      MED_FR::MED_REMP,    
                      rep,
                      const_cast <char *> (tmp_name.c_str()), 
                      const_cast <char *> (tmp_unit.c_str()) 
                      );  

    if (err<0) 
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write coordinates of mesh |" << _meshName.c_str() << "| in file |" << _fileName
                                   << "| with dimension |"  << _ptrMesh->_spaceDimension <<"| and" 
                                   << " with units names |"  << tmp_name
                                   << "| and units |"       << tmp_unit
                                   << " |")) ;    
    
    END_OF(LOC);
    
    return MED_VALID;
  }

  MESSAGE (LOC<<" Not open !!!");
  return MED_ERROR;
}




int MED_MESH_WRONLY_DRIVER::writeConnectivities(medEntityMesh entity) const {
  
  const char * LOC="int MED_MESH_WRONLY_DRIVER::writeConnectivities() const : ";
  BEGIN_OF(LOC);

  MED_FR::med_err err;
  
  if (_status==MED_OPENED) {
    
    // REM SI LA METHODE EST APPELEE DIRECTEMENT ET QUE LE MAILLAGE N'EST PAS CREE IL Y A PB !
    
    // A FAIRE : A tester surtout dans les methodes de MESH.
    //    if ( _ptrMesh->_connectivity == (CONNECTIVITY *) MED_INVALID )
    if ( _ptrMesh->_connectivity == (CONNECTIVITY *) NULL )
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "The connectivity is not defined in the MESH object ")) ;
    
    if   ( _ptrMesh->existConnectivity(MED_NODAL,entity) ) { 

      int numberOfTypes           = _ptrMesh->getNumberOfTypes (entity) ;
      medGeometryElement  * types = _ptrMesh->getTypes         (entity) ;
    
      for (int i=0; i<numberOfTypes; i++) {
      
        int    numberOfElements = _ptrMesh->getNumberOfElements (entity,types[i]);
        int * connectivity      = _ptrMesh->getConnectivity     (MED_EN::MED_FULL_INTERLACE, 
                                                                 MED_NODAL, entity, types[i]); // ?? et SI MED_NODAL n'existe pas, recalcul auto ??
      
        // Pour l'instant la class utilise le multi.....
	int multi = 0 ;
	if (entity==MED_EN::MED_CELL)
	  if ( (types[i]/ 100) < _ptrMesh->_spaceDimension) 
	    multi=1 ;
	int numberOfNodes = types[i]%100 ;
	int * connectivityArray = new int[numberOfElements*(numberOfNodes+multi)];
	for (int j=0 ; j<numberOfElements; j++) {
	  for (int k=0; k<numberOfNodes; k++)
	    connectivityArray[j*(numberOfNodes+multi)+k]=connectivity[j*numberOfNodes+k] ;
	  connectivityArray[j*(numberOfNodes+multi)+numberOfNodes]=0;
	}
        err = MEDconnEcr( _medIdt, const_cast <char *> ( _meshName.c_str()), _ptrMesh->_spaceDimension,
                          connectivityArray, MED_FR::MED_FULL_INTERLACE , numberOfElements,
                          MED_FR::MED_REMP,
                          (MED_FR::med_entite_maillage  ) entity, 
                          (MED_FR::med_geometrie_element) types[i], MED_NOD );
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
      medGeometryElement  * types = _ptrMesh->getTypes         (entity) ;
      
      for (int i=0; i<numberOfTypes; i++) {
	
        int    numberOfElements = _ptrMesh->getNumberOfElements (entity,types[i]);
        int * connectivity = _ptrMesh->getConnectivity(MED_EN::MED_FULL_INTERLACE, MED_DESCENDING, entity, types[i]); 
      
        // Pour l'instant la class utilise le multi.....
        err = MEDconnEcr( _medIdt, const_cast <char *> ( _meshName.c_str()), _ptrMesh->_spaceDimension,
                          connectivity, MED_FR::MED_FULL_INTERLACE , numberOfElements,
                          MED_FR::MED_REMP,
                          (MED_FR::med_entite_maillage  ) entity, 
                          (MED_FR::med_geometrie_element) types[i], MED_DESC );
	
        if (err<0) // ETENDRE LES EXPLICATIONS
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write connectivities of mesh |" << _meshName.c_str() << "| in file |" << _fileName
                                       << "| with dimension |"  << _ptrMesh->_spaceDimension <<"| and" 
				       )) ;
      }
    }
    // Connctivity descending :
    if   ( _ptrMesh->existConnectivity(MED_DESCENDING,entity) ) { 
      
      int numberOfTypes           = _ptrMesh->getNumberOfTypes (entity) ;
      medGeometryElement  * types = _ptrMesh->getTypes         (entity) ;
      
      for (int i=0; i<numberOfTypes; i++) {
	
        int    numberOfElements = _ptrMesh->getNumberOfElements (entity,types[i]);
        int * connectivity = _ptrMesh->getConnectivity(MED_EN::MED_FULL_INTERLACE, MED_DESCENDING, entity, types[i]); 
      
        // Pour l'instant la class utilise le multi.....
        err = MEDconnEcr( _medIdt, const_cast <char *> ( _meshName.c_str()), _ptrMesh->_spaceDimension,
                          connectivity, MED_FR::MED_FULL_INTERLACE , numberOfElements,
                          MED_FR::MED_REMP,
                          (MED_FR::med_entite_maillage  ) entity, 
                          (MED_FR::med_geometrie_element) types[i], MED_DESC );
	
        if (err<0) // ETENDRE LES EXPLICATIONS
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write connectivities of mesh |" << _meshName.c_str() << "| in file |" << _fileName
                                       << "| with dimension |"  << _ptrMesh->_spaceDimension <<"| and" 
				       )) ;
            
      }
    }
    END_OF(LOC);
    return MED_VALID;
  }
  return MED_ERROR;
}

int MED_MESH_WRONLY_DRIVER::writeFamilyNumbers() const {
  
  const char * LOC="int MED_MESH_WRONLY_DRIVER::writeFamilyNumbers() const : ";
  BEGIN_OF(LOC);

  MED_FR::med_err err;
  
  if (_status==MED_OPENED) {

    // SOLUTION TEMPORAIRE CAR _ptrMesh->_MEDArrayNodeFamily DOIT ETRE ENLEVER DE LA CLASSE MESH
    err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
                    _ptrMesh->_MEDArrayNodeFamily, _ptrMesh->getNumberOfNodes(),MED_FR::MED_REMP ,
                    MED_NOEUD,(enum MED_FR::med_geometrie_element) MED_NONE); 
    if ( err != MED_VALID) 
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't write node family for the |"<< _ptrMesh->getNumberOfNodes() 
                                   << "| nodes in mesh |" 
                                   << _ptrMesh->_name.c_str() << "|" ));   


    { // CELLS RELATED BLOCK
      medEntityMesh entity=MED_EN::MED_CELL;
      // SOLUTION TEMPORAIRE CAR _ptrMesh->_MEDArray____Family DOIT ETRE ENLEVER DE LA CLASSE MESH
      if  ( ( _ptrMesh->existConnectivity(MED_NODAL,entity) )|( _ptrMesh->existConnectivity(MED_DESCENDING,entity) ) ) { 

        int numberOfTypes           = _ptrMesh->getNumberOfTypes (entity) ;
        medGeometryElement  * types = _ptrMesh->getTypes         (entity) ;
      
        for (int i=0; i<numberOfTypes; i++) {

          err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
                          _ptrMesh->_MEDArrayCellFamily[i], _ptrMesh->getNumberOfElements(entity, types[i]),
                          MED_FR::MED_REMP ,
                          (MED_FR::med_entite_maillage) entity,
                          (MED_FR::med_geometrie_element) types[i]); 
        
          if ( err != MED_VALID) 
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't write family for the |"<< _ptrMesh->getNumberOfElements(entity, types[i])
                                         << "| cells of geometric type |" << geoNames[ (MED_FR::med_geometrie_element) types[i]] <<"|in mesh |"      
                                         << _ptrMesh->_name.c_str() << "|" ));   
        }
      }
    }

    { // FACE RELATED BLOCK
      medEntityMesh entity=MED_EN::MED_FACE;
      // SOLUTION TEMPORAIRE CAR _ptrMesh->_MEDArray____Family DOIT ETRE ENLEVER DE LA CLASSE MESH
      if  ( ( _ptrMesh->existConnectivity(MED_NODAL,entity) )|( _ptrMesh->existConnectivity(MED_DESCENDING,entity) ) ) { 

        int numberOfTypes           = _ptrMesh->getNumberOfTypes (entity) ;
        medGeometryElement  * types = _ptrMesh->getTypes         (entity) ;
      
	int numberOfElements = _ptrMesh->getNumberOfElements(entity, MED_ALL_ELEMENTS) ;
	int * familyArray = new int[numberOfElements] ;
	for (int i=0;i<numberOfElements;i++)
	  familyArray[i]=0;

	int numberOfFamilies = _ptrMesh->getNumberOfFamilies(entity) ;
	vector<FAMILY*> myFamilies = _ptrMesh->getFamilies(entity) ;
	for (int i=0;i<numberOfFamilies;i++) {
	  int familyNumber = myFamilies[i]->getIdentifier() ;
	  int numberOfFamilyElements = myFamilies[i]->getNumberOfElements(MED_ALL_ELEMENTS) ;
	  int * myFamilyElements = myFamilies[i]->getNumber(MED_ALL_ELEMENTS) ;
	  for (int ii=0;ii<numberOfFamilyElements;ii++)
	      familyArray[myFamilyElements[ii]-1]=familyNumber;
	}

	int * typeCount = _ptrMesh->getGlobalNumberingIndex(entity) ;

        for (int i=0; i<numberOfTypes; i++) {

	  int typeNumberOfElements = typeCount[i+1] - typeCount[i] ;
          err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
                          familyArray+typeCount[i]-1, typeNumberOfElements,
                          MED_FR::MED_REMP ,
                          (MED_FR::med_entite_maillage) entity,
                          (MED_FR::med_geometrie_element) types[i]); 

          if ( err != MED_VALID) 
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't write family for the |"<< _ptrMesh->getNumberOfElements(entity, types[i])
                                         << "| faces of geometric type |" << geoNames[ (MED_FR::med_geometrie_element) types[i]] <<"|in mesh |"      
                                         << _ptrMesh->_name.c_str() << "|" ));   
        }
	delete[] familyArray ;
      }
    }

    { // EDGE RELATED BLOCK
      //medEntityMesh entity=MED_EN::MED_FACE;
      medEntityMesh entity=MED_EN::MED_EDGE;
      // SOLUTION TEMPORAIRE CAR _ptrMesh->_MEDArray____Family DOIT ETRE ENLEVER DE LA CLASSE MESH
      if  ( ( _ptrMesh->existConnectivity(MED_NODAL,entity) )|( _ptrMesh->existConnectivity(MED_DESCENDING,entity) ) ) { 

        int numberOfTypes           = _ptrMesh->getNumberOfTypes (entity) ;
        medGeometryElement  * types = _ptrMesh->getTypes         (entity) ;
      
	int numberOfElements = _ptrMesh->getNumberOfElements(entity, MED_ALL_ELEMENTS) ;
	int * familyArray = new int[numberOfElements] ;
	for (int i=0;i<numberOfElements;i++)
	  familyArray[i]=0;

	int numberOfFamilies = _ptrMesh->getNumberOfFamilies(entity) ;
	vector<FAMILY*> myFamilies = _ptrMesh->getFamilies(entity) ;
	for (int i=0;i<numberOfFamilies;i++) {
	  int familyNumber = myFamilies[i]->getIdentifier() ;
	  int numberOfFamilyElements = myFamilies[i]->getNumberOfElements(MED_ALL_ELEMENTS) ;
	  int * myFamilyElements = myFamilies[i]->getNumber(MED_ALL_ELEMENTS) ;
	  for (int ii=0;ii<numberOfFamilyElements;ii++)
	      familyArray[myFamilyElements[ii]-1]=familyNumber;
	}

	int * typeCount = _ptrMesh->getGlobalNumberingIndex(entity) ;

        for (int i=0; i<numberOfTypes; i++) {

	  int typeNumberOfElements = typeCount[i+1] - typeCount[i] ;
          err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
                          familyArray+typeCount[i]-1, typeNumberOfElements,
                          MED_FR::MED_REMP ,
                          (MED_FR::med_entite_maillage) entity,
                          (MED_FR::med_geometrie_element) types[i]); 
        
          if ( err != MED_VALID) 
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't write family for the |"<< _ptrMesh->getNumberOfElements(entity, types[i])
                                         << "| edges of geometric type |" << geoNames[ (MED_FR::med_geometrie_element) types[i]] <<"|in mesh |"      
                                         << _ptrMesh->_name.c_str() << "|" ));   
        }
      }
    }
    
    END_OF(LOC);
    return MED_VALID;
  }
  return MED_ERROR;
}


int MED_MESH_WRONLY_DRIVER::writeFamilies(vector<FAMILY*> & families ) const {
  
  const char * LOC="int MED_MESH_WRONLY_DRIVER::writeFamilies(vector<FAMILY*> families) const : ";
  BEGIN_OF(LOC);

  MED_FR::med_err err;
  
  if (_status==MED_OPENED) {

    MESSAGE(LOC<<" families.size() :"<<families.size());

    for (int i=0; i< families.size(); i++) {

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
      if ( err =_MEDdatagroupOuvrir(_medIdt,const_cast <char *> (dataGroupFam.c_str()) ) < MED_VALID ) {
        SCRUTE(err);

	MESSAGE(LOC<<"families[i]->getName().c_str() : "<<families[i]->getName().c_str());
	MESSAGE(LOC<<"_meshName.c_str() : "<<_meshName.c_str());
	MESSAGE(LOC<<"families[i]->getIdentifier() : "<<families[i]->getIdentifier());
	MESSAGE(LOC<<"families[i]->getAttributesIdentifiers() : "<<families[i]->getAttributesIdentifiers()[0]);
	MESSAGE(LOC<<"families[i]->getAttributesValues() : "<<families[i]->getAttributesValues()[0]);
	MESSAGE(LOC<<"attributesDescriptions.c_str() : "<<attributesDescriptions.c_str());
	MESSAGE(LOC<<"numberOfAttributes : "<<numberOfAttributes);
	MESSAGE(LOC<<"groupsNames.c_str() : "<<groupsNames.c_str());
	MESSAGE(LOC<<"numberOfGroups : "<<numberOfGroups);

        err = MEDfamCr( _medIdt, 
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
  return MED_ERROR;
}


// A FAIRE POUR LES NOEUDS ET LES ELEMENTS ret = MEDfamEcr(
