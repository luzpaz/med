using namespace std;

#include <algorithm>

#include "MEDMEM_GibiMeshDriver.hxx"

#include "MEDMEM_DriversDef.hxx"

#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_DriverTools.hxx"

/////
using namespace MED_EN;
using namespace MEDMEM;
/////


/////
const size_t GIBI_MESH_DRIVER::nb_geometrie_gibi;

const medGeometryElement GIBI_MESH_DRIVER::geomGIBItoMED[nb_geometrie_gibi] =
     {   /*1 */ MED_POINT1 ,/*2 */ MED_SEG2   ,/*3 */ MED_SEG3   ,/*4 */ MED_TRIA3  ,/*5 */ MED_NONE   ,
       /*6 */ MED_TRIA6  ,/*7 */ MED_NONE   ,/*8 */ MED_QUAD4  ,/*9 */ MED_NONE   ,/*10*/ MED_QUAD8  ,
       /*11*/ MED_NONE   ,/*12*/ MED_NONE   ,/*13*/ MED_NONE   ,/*14*/ MED_HEXA8  ,/*15*/ MED_HEXA20 ,
       /*16*/ MED_PENTA6 ,/*17*/ MED_PENTA15,/*18*/ MED_NONE   ,/*19*/ MED_NONE   ,/*20*/ MED_NONE   ,
       /*21*/ MED_NONE   ,/*22*/ MED_NONE   ,/*23*/ MED_TETRA4 ,/*24*/ MED_TETRA10,/*25*/ MED_PYRA5  ,
       /*26*/ MED_PYRA13 ,/*27*/ MED_NONE   ,/*28*/ MED_NONE   ,/*29*/ MED_NONE   ,/*30*/ MED_NONE   ,
       /*31*/ MED_NONE   ,/*32*/ MED_NONE   ,/*33*/ MED_NONE   ,/*34*/ MED_NONE   ,/*35*/ MED_NONE   ,
       /*36*/ MED_NONE   ,/*37*/ MED_NONE   ,/*38*/ MED_NONE   ,/*39*/ MED_NONE   ,/*40*/ MED_NONE   ,
       /*41*/ MED_NONE   ,/*42*/ MED_NONE   ,/*43*/ MED_NONE   ,/*44*/ MED_NONE   ,/*45*/ MED_NONE   ,
       /*46*/ MED_NONE   ,/*47*/ MED_NONE   };
/////

// Every memory allocation made in the MedDriver members function are desallocated in the Mesh destructor 

GIBI_MESH_DRIVER::GIBI_MESH_DRIVER():
  GENDRIVER(),
  _ptrMesh(( MESH *)MED_NULL),
  // A VOIR _medIdt(MED_INVALID),
  _meshName("")
{
}

GIBI_MESH_DRIVER::GIBI_MESH_DRIVER(const string & fileName,
                                   MESH * ptrMesh,
                                   MED_EN::med_mode_acces accessMode): 
  GENDRIVER(fileName,accessMode),
  _ptrMesh(ptrMesh)
  // A VOIR _medIdt(MED_INVALID), 
{
//   _meshName=fileName.substr(0,fileName.rfind("."));
    // mesh name construction from fileName
    const string ext=".sauve"; // expected extension
    string::size_type pos=fileName.find(ext,0);
    string::size_type pos1=fileName.rfind('/');
    _meshName = string(fileName,pos1+1,pos-pos1-1); //get rid of directory & extension
    SCRUTE(_meshName);
}
  
GIBI_MESH_DRIVER::GIBI_MESH_DRIVER(const GIBI_MESH_DRIVER & driver): 
  GENDRIVER(driver),
  _ptrMesh(driver._ptrMesh),
  // A VOIR _medIdt(MED_INVALID), 
  _meshName(driver._meshName)
{
}

GIBI_MESH_DRIVER::~GIBI_MESH_DRIVER()
{
}

void GIBI_MESH_DRIVER::open()
  throw (MEDEXCEPTION)
{
    const char * LOC = "GIBI_MESH_DRIVER::open()" ;
    BEGIN_OF(LOC);

    _gibi.open(_fileName.c_str(), ios::in);
    if(_gibi)
	_status = MED_OPENED;
    else
    {
	_status = MED_CLOSED;
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Could not open file "<<_fileName<<" in mode ios::in"));
    }
  END_OF(LOC);
}
  
void GIBI_MESH_DRIVER::close()
  throw (MEDEXCEPTION)
{
    const char * LOC = "GIBI_MESH_DRIVER::close() " ;
    BEGIN_OF(LOC);
    if ( _status == MED_OPENED) 
    {
	_gibi.close();
	_status = MED_CLOSED;
    }
    END_OF(LOC);
}

void    GIBI_MESH_DRIVER::setMeshName(const string & meshName) { _meshName = meshName; };
string  GIBI_MESH_DRIVER::getMeshName() const { return _meshName; };


//---------------------------------- RDONLY PART -------------------------------------------------------------

GIBI_MESH_RDONLY_DRIVER::GIBI_MESH_RDONLY_DRIVER(): GIBI_MESH_DRIVER()
{
}

GIBI_MESH_RDONLY_DRIVER::GIBI_MESH_RDONLY_DRIVER(const string & fileName,
	MESH * ptrMesh):
GIBI_MESH_DRIVER(fileName,ptrMesh,MED_RDONLY)
{ 
    MESSAGE("GIBI_MESH_RDONLY_DRIVER::GIBI_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}

    GIBI_MESH_RDONLY_DRIVER::GIBI_MESH_RDONLY_DRIVER(const GIBI_MESH_RDONLY_DRIVER & driver): 
GIBI_MESH_DRIVER(driver)
{
}

GIBI_MESH_RDONLY_DRIVER::~GIBI_MESH_RDONLY_DRIVER()
{
    //MESSAGE("GIBI_MESH_RDONLY_DRIVER::~GIBI_MESH_RDONLY_DRIVER() has been destroyed");
}
  
GENDRIVER * GIBI_MESH_RDONLY_DRIVER::copy(void) const
{
    return new GIBI_MESH_RDONLY_DRIVER(*this);
}

void GIBI_MESH_RDONLY_DRIVER::read(void) throw (MEDEXCEPTION)
{
    const char * LOC = "GIBI_MESH_RDONLY_DRIVER::read() : " ;
    BEGIN_OF(LOC);

    if (_status!=MED_OPENED)
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "The _idt of file " << _fileName << " is : "  
		                                 <<  " (the file is not opened)." )) ;

    // LECTURE DES DONNEES DS FICHIER GIBI

    _intermediateMED medi; // structure de données intermédiaire pour conversion gibi->med
    string buf_ligne; // pour lire une ligne
    const char* enregistrement_type="ENREGISTREMENT DE TYPE";
    std::vector<int> numero_noeuds; // tableau de travail (indices)


    while ( getline(_gibi, buf_ligne) ) // boucle externe de recherche de "ENREGISTREMENT DE TYPE"
    {
	string::size_type pos = buf_ligne.find(enregistrement_type);
	if ( pos==string::npos )
	    continue; // "ENREGISTREMENT DE TYPE" non trouvé -> on lit la ligne suivante

	// lecture du numéro d'enregistrement
	int numero_enregistrement;
	istringstream buf(buf_ligne.c_str()+strlen(enregistrement_type)+1);
	buf >> numero_enregistrement;

	enum { ENREG_TYPE_2=2, ENREG_TYPE_4=4}; // énumération des types d'enregistrement traités
	int niveau, niveau_erreur;
	unsigned space_dimension,nb_reels;
	int numero_pile, nb_objets_nommes, nb_objets, nb_indices;
	string s1,s2,s3,s4,s5,s6,s7; // temporary strings
	int i1; //temporary int
	double d1; //temporary double
	vector<int> indices_objets_nommes;
	vector<string> objets_nommes;

	switch (numero_enregistrement)
	  {
	  case ENREG_TYPE_4:
	    MESSAGE(LOC << "---- Traitement enregistrement de type 4");
	    _gibi >> s1 >> niveau >> s2 >> s3 >> niveau_erreur >> s4 >> space_dimension;
	    if ( !_gibi || s1!="NIVEAU" || s3!="ERREUR" || s4!="DIMENSION" ) // verification mots -cles
	      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << " Could not read file " << _fileName 
					   << " : syntax error in type 4 record"));
	    break;

	  case ENREG_TYPE_2:
	    {
	      MESSAGE(LOC << "---- Traitement enregistrement de type 2");
	      _gibi >> s1 >> s2 >> numero_pile >> s3 >> s4 >> s5 >> nb_objets_nommes >> s6 >> s7 >> nb_objets;
	      if ( !_gibi || s1!="PILE"   || s2!="NUMERO"       || s3!="NBRE"  // verification mots -cles
		   || s4!="OBJETS" || s5!="NOMMES"       || s6!="NBRE"   
		   || s7!="OBJETS" || nb_objets_nommes<0 || nb_objets<0  )
		{
		  throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << " Could not read file " << _fileName 
					       << " : error in type 2 record"));
		}

	      // lecture des objets nommés et de leurs indices
	      objets_nommes.resize(nb_objets_nommes);
	      indices_objets_nommes.resize(nb_objets_nommes);
	      for (int i=0; i!=nb_objets_nommes; ++i)
		_gibi >> objets_nommes[i];

	      for (int i=0; i!=nb_objets_nommes; ++i)
		_gibi >> indices_objets_nommes[i];
		
	      // boucle interne : lecture de la pile
	      enum {PILE_SOUS_MAILLAGE=1, PILE_NOEUDS=32, PILE_COORDONNEES=33};
	      switch(numero_pile)
		{
		case PILE_SOUS_MAILLAGE:
		  {
		    medi.groupes.reserve(nb_objets);
		    for (int objet=0; objet!=nb_objets; ++objet) // pour chaque groupe
		      {
			unsigned type_geom_castem, nb_reference,nb_noeud,nb_elements, nb_sous_maillage;
			_gibi >> type_geom_castem >> nb_sous_maillage >> nb_reference >> nb_noeud >> nb_elements;
			
			// le cas type_geom_castem=0 correspond aux maillages composites
			if (type_geom_castem<0 || (type_geom_castem>0 && geomGIBItoMED[type_geom_castem-1]==MED_NONE) )
			  throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << " Error while reading file " << _fileName 
						       << "\nCastem geometric type " << type_geom_castem 
						       << " does not have a correspondant MED geometric type!" ));

			// lecture des references (non utilisé pour MED)
			for( unsigned i=0; i!=nb_reference; ++i)
			  _gibi >> i1;
			
			// lecture des couleurs (non utilisé pour MED)
			for( unsigned i=0; i!=nb_elements; ++i)
			  _gibi >> i1;
			
			_groupe groupe;
			// si le groupe se compose de sous-maillages (ie groupe composite) 
			if (type_geom_castem==0 && nb_sous_maillage>0) 
			  {
			    // lecture des indices des sous-maillages, stockage.
			    // les mailles correspondant a ces sous_maillages seront inserees a la fin du case
			    for (unsigned i=0; i!=nb_sous_maillage; ++i)
			      {
				_gibi >> i1;
				groupe.groupes.push_back(i1);
			      }
			  }
			else
			  {
			    pair<set<_maille>::iterator,bool> p;
			    pair<map<int,_noeud>::iterator,bool> p_no;
			    _noeud no;
			    no.coord.reserve(space_dimension);
			    no.coord.resize(space_dimension);
			    _maille ma(geomGIBItoMED[type_geom_castem-1], nb_noeud);
			    ma.sommets.resize(nb_noeud);

			    // lecture pour chaque maille des sommets et insertions
			    for( unsigned i=0; i!=nb_elements; ++i)
			      {
				for (unsigned n=0; n!=nb_noeud; ++n)
				  {
				    _gibi >> i1;
				    no.number=i1;
				    p_no=medi.points.insert(make_pair(i1, no));
				    ma.sommets[n]=p_no.first;
				  }

				p=medi.maillage.insert(ma);
				groupe.mailles.insert(p.first); // on stocke dans le groupe un iterateur sur la maille

//				    cout << "   " << p.second << ": ";
//				    for (unsigned n=0; n!=nb_noeud; ++n)
//					cout <<  ma.sommets[n]->second.number << " ";
//				    cout << endl;
				
			      }
			  }
			medi.groupes.push_back(groupe);
		      }

		    for (int i=0; i!=nb_objets_nommes; ++i)
		      medi.groupes[indices_objets_nommes[i]-1].nom=objets_nommes[i];
		    
		    // scanne les groupes à la recherche de groupes composites
		    for( std::vector<_groupe>::iterator i=medi.groupes.begin(); i!=medi.groupes.end(); ++i)
		      {
			if( i->groupes.size() ) // le groupe i contient des sous-maillages
			  {
			    for( std::vector<int>::iterator j=i->groupes.begin(); j!=i->groupes.end(); ++j)
			      {
				// pour chacun des sous-maillages j, on recupere les iterateurs *k sur les  maille 
				// contenues et on les insere dans le groupe i
				std::set< std::set<_maille>::iterator >::const_iterator k=medi.groupes[*j-1].mailles.begin();
				for( ; k!=medi.groupes[*j-1].mailles.end(); ++k)
				  i->mailles.insert(*k);
			      }
			    i->groupes.clear(); // après avoir inséré leur mailles, on efface les groupes composites
			  }
		      }
		    
		    break;
		  }// Fin case PILE_SOUS_MAILLAGE
			
		case PILE_NOEUDS:
		  {
		    std::vector<int> place_noeuds;
		    _gibi >> nb_indices;
		    if (nb_indices != nb_objets)
		      {
			throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << " Could not read file " << _fileName 
						     << "Erreur de lecture dans enregistrement de type " << ENREG_TYPE_2 
						     << " (pile " << PILE_NOEUDS << ")" ));
		      }
		    
		    place_noeuds.resize(nb_objets);
		    for (unsigned i=0; i!=place_noeuds.size(); ++i)
		      _gibi >> place_noeuds[i];
		    int max=(* std::max_element(place_noeuds.begin(),place_noeuds.end()));

		    // numero_noeuds contient pour chacun des max noeuds qu'on va lire dans le case PILE_COORDONNEES
		    // son indice dans la connectivite du maillage. Cet indice correspond egalement a la cle du map
		    // medi.points ou l'on stocke les noeuds.
		    numero_noeuds.resize(max,-1);
		    for (unsigned i=0; i!=place_noeuds.size(); ++i)
		      numero_noeuds[place_noeuds[i]-1]=i+1;
		    break;
		  }
		  
		case PILE_COORDONNEES:
		  _gibi >> nb_reels;
		  // PROVISOIRE : certains fichier gibi n'ont 
		  if (nb_reels < numero_noeuds.size()*(space_dimension))
		    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << " Could not read file " << _fileName 
						 << "Erreur de lecture dans enregistrement de type " << ENREG_TYPE_2 
						 << " (pile " << PILE_COORDONNEES << ")" ));

		  for (unsigned i=0; i!=numero_noeuds.size(); ++i)
		    {
		      // si le noeud est utilisé dans le maillage, on lit ses coordonnées et on les stocke dans la structure
		      if ( (numero_noeuds[i] != -1) && (medi.points.find(numero_noeuds[i])!=medi.points.end()) ) 
			{
			  for (unsigned j=0; j!=space_dimension; ++j)
			    _gibi >> medi.points[numero_noeuds[i]].coord[j];
			  _gibi >> d1; // on ne conserve pas la densite
			}
		      else // sinon, on passe au noeud suivant
			{
			  for (unsigned j=0; j!=space_dimension+1; ++j)
			    _gibi >> d1;
			}
		    }
		  break;

		} // Fin switch numero_pile
	      break;
	    } // Fin case ENREG_TYPE_2
	  }

    } //  fin de la boucle while de lecture externe

    // impression résultats
    MESSAGE(LOC << "GIBI_MESH_RDONLY_DRIVER::read : RESULTATS STRUCTURE INTERMEDIAIRES : ");
    MESSAGE(LOC <<  medi );

	    // TRANSFORMATION EN STRUCTURES MED
    if ( ! _ptrMesh->isEmpty() )
    {
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Mesh object not empty : can't fill it!"));
    }
    else if ( medi.maillage.size()==0 || medi.groupes.size()==0 || medi.points.size()==0)
    {
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << " Error while reading file " << _fileName 
		    << " The data read are not completed " ) ) ;
    }
    else
    {
	_ptrMesh->_name = _meshName;
	_ptrMesh->_spaceDimension = medi.points.begin()->second.coord.size();
	_ptrMesh->_meshDimension = medi.maillage.rbegin()->dimension();
	_ptrMesh->_numberOfNodes = medi.points.size();
	_ptrMesh->_isAGrid = 0;
	_ptrMesh->_coordinate = medi.getCoordinate();

	//Construction des groupes
	vector<GROUP *> groupCell, groupFace, groupEdge, groupNode;
	medi.getGroups(groupCell, groupFace, groupEdge, groupNode, _ptrMesh);
	_ptrMesh->_groupCell = groupCell;
	_ptrMesh->_groupFace = groupFace;
	_ptrMesh->_groupEdge = groupEdge;
	_ptrMesh->_groupNode = groupNode;

	// appele en dernier car cette fonction detruit le maillage intermediaire!
	_ptrMesh->_connectivity = medi.getConnectivity(); 

	// calcul de la connectivite d-1 complete, avec renumerotation des groupes
	//if (_ptrMesh->_spaceDimension==3)
	//    _ptrMesh->_connectivity->updateGroup(_ptrMesh->_groupFace) ;
	//else if (_ptrMesh->_spaceDimension==2)
	//    _ptrMesh->_connectivity->updateGroup(_ptrMesh->_groupEdge) ;
	
	// Creation des familles à partir des groupes
	// NC : Cet appel pourra être différé quand la gestion de la cohérence famille/groupes sera assurée
	_ptrMesh->createFamilies();
    }



    END_OF(LOC);
}

void GIBI_MESH_RDONLY_DRIVER::write( void ) const
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("GIBI_MESH_RDONLY_DRIVER::write : Can't write with a RDONLY driver !");
}


/*--------------------- WRONLY PART -------------------------------*/

GIBI_MESH_WRONLY_DRIVER::GIBI_MESH_WRONLY_DRIVER():GIBI_MESH_DRIVER()
{
}
  
GIBI_MESH_WRONLY_DRIVER::GIBI_MESH_WRONLY_DRIVER(const string & fileName,
                                                 MESH * ptrMesh):
  GIBI_MESH_DRIVER(fileName,ptrMesh,MED_WRONLY)
{
  MESSAGE("GIBI_MESH_WRONLY_DRIVER::GIBI_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}

GIBI_MESH_WRONLY_DRIVER::GIBI_MESH_WRONLY_DRIVER(const GIBI_MESH_WRONLY_DRIVER & driver): 
  GIBI_MESH_DRIVER(driver)
{
}

GIBI_MESH_WRONLY_DRIVER::~GIBI_MESH_WRONLY_DRIVER()
{
  //MESSAGE("GIBI_MESH_WRONLY_DRIVER::GIBI_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been destroyed");
}

GENDRIVER * GIBI_MESH_WRONLY_DRIVER::copy(void) const
{
  return new GIBI_MESH_WRONLY_DRIVER(*this);
}

void GIBI_MESH_WRONLY_DRIVER::read (void)
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("GIBI_MESH_WRONLY_DRIVER::read : Can't read with a WRONLY driver !");
}

void GIBI_MESH_WRONLY_DRIVER::write(void) const
  throw (MEDEXCEPTION)
{ 
  const char * LOC = "void GIBI_MESH_WRONLY_DRIVER::write(void) const : ";
  BEGIN_OF(LOC);

  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<< "Write Driver isn\'t implemented"));

  END_OF(LOC);
} 



/*--------------------- RDWR PART -------------------------------*/

GIBI_MESH_RDWR_DRIVER::GIBI_MESH_RDWR_DRIVER():GIBI_MESH_DRIVER()
{
}

GIBI_MESH_RDWR_DRIVER::GIBI_MESH_RDWR_DRIVER(const string & fileName,
					   MESH * ptrMesh):
  GIBI_MESH_DRIVER(fileName,ptrMesh,MED_RDWR)
{
  MESSAGE("GIBI_MESH_RDWR_DRIVER::GIBI_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}

GIBI_MESH_RDWR_DRIVER::GIBI_MESH_RDWR_DRIVER(const GIBI_MESH_RDWR_DRIVER & driver): 
  GIBI_MESH_RDONLY_DRIVER::GIBI_MESH_DRIVER(driver)
{
}

GIBI_MESH_RDWR_DRIVER::~GIBI_MESH_RDWR_DRIVER() {
  //MESSAGE("GIBI_MESH_RDWR_DRIVER::GIBI_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) has been destroyed");
} 
  
GENDRIVER * GIBI_MESH_RDWR_DRIVER::copy(void) const
{
  return new GIBI_MESH_RDWR_DRIVER(*this);
}

void GIBI_MESH_RDWR_DRIVER::write(void) const
  throw (MEDEXCEPTION)
{
  GIBI_MESH_WRONLY_DRIVER::write();
}
void GIBI_MESH_RDWR_DRIVER::read (void)
  throw (MEDEXCEPTION)
{
  GIBI_MESH_RDONLY_DRIVER::read();
}
