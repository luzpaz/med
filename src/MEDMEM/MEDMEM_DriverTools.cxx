using namespace std;
#include "MEDMEM_DriverTools.hxx"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Group.hxx"
#include <algorithm>
using namespace MEDMEM;


// Cet opérateur permet d'ordonner les mailles dans un set suivant l'ordre requis par MED
bool _maille::operator < (const _maille& ma) const
{
    // si le type géométrique differe, la comparaison est basée dessus
    // sinon on se base sur une comparaison des numéros de sommets 
    if(geometricType==ma.geometricType)
    {
	// construction de deux vecteur temporaire contenant les numeros de sommets
	// pour faire le tri et les comparaisons
	size_t l=sommets.size();
	std::vector<int> v1(l);
	std::vector<int> v2(l);
	for (unsigned int i=0; i!=l; ++i)
	  {
	    v1[i]=sommets[i]->second.number;
	    v2[i]=ma.sommets[i]->second.number;
	  }
	std::sort(v1.begin(), v1.end());
	std::sort(v2.begin(), v2.end());
	for(std::vector<int>::const_iterator i1=v1.begin(), i2=v2.begin(); i1!=v1.end(); ++i1, ++i2)
	    if(*i1 != *i2)
		return *i1 < *i2;
	return false; // cas d'égalité
    }
    else
	return geometricType<ma.geometricType;
};


// retourne l'entité d'une maille en fonction de la dimension du maillage.
MED_EN::medEntityMesh _maille::getEntity(const int meshDimension) const throw (MEDEXCEPTION)
{
  const char * LOC = "_maille::getEntity(const int meshDimension)" ;
  BEGIN_OF(LOC);

  int mailleDimension = this->dimension();
  medEntityMesh entity;
  if (mailleDimension == meshDimension)
    entity = MED_CELL;
  else
    switch (mailleDimension)
      {
      case 0 :
	entity = MED_NODE;
	break;
      case 1 :
	entity = MED_EDGE;
	break;
      case 2 :
	entity = MED_FACE;
	break;
      default :
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Impossible de determiner l'entite de la maille."));
      }
return entity;

END_OF(LOC);
};

std::ostream& MEDMEM::operator << (std::ostream& os, const _maille& ma)
{
    os << "maille " << ma.ordre << " (" << ma.geometricType << ") : < ";
    std::vector< std::map<int,_noeud>::iterator >::const_iterator i=ma.sommets.begin();
    os << (*i++)->second.number ;
    for( ; i!=ma.sommets.end(); ++i)
	os << ", " << (*i)->second.number;
    os << " >";
    return os;
}

std::ostream& MEDMEM::operator << (std::ostream& os, const _groupe& gr)
{
    os << "--- Groupe " << gr.nom << " --- " << std::endl ;
    os << " -> liste des sous-groupes : ";
    for( std::list<int>::const_iterator i=gr.groupes.begin(); i!=gr.groupes.end(); ++i)
	os << *i << " ";
    os << std::endl << " -> liste des mailles : " << std::endl;
    for( _groupe::mailleIter i=gr.mailles.begin(); i!=gr.mailles.end(); i++)
	os << "    " << *(*i) << std::endl;
    return os;
}

std::ostream& MEDMEM::operator << (std::ostream& os, const _noeud& no)
{
    os << "noeud " << no.number << " : < ";
    std::vector<double>::const_iterator i=no.coord.begin();
    os << *i++ ;
    for( ; i!=no.coord.end(); ++i)
	os << ", " << *i;
    os << " >";
    return os;
}

std::ostream& MEDMEM::operator << (std::ostream& os, const _intermediateMED& mi)
{
    os << "Set des mailles : " << std::endl;
    for( std::set<_maille>::const_iterator i=mi.maillage.begin(); i!=mi.maillage.end(); ++i)
	os << *i << std::endl;
    
    os << std::endl << "Vector des groupes : " << std::endl;
   // for( std::vector<_groupe>::const_iterator i=mi.groupes.begin(); i!=mi.groupes.end(); ++i)
    for (unsigned int i=0; i!=mi.groupes.size(); i++)
      os << std::setw(3) << i << " " << mi.groupes[i] << std::endl;
    
    os << std::endl << "map des noeuds : " << std::endl;
    for( std::map<int,_noeud>::const_iterator i=mi.points.begin(); i!=mi.points.end(); ++i)
	os << i->second << std::endl;
    return os;
}

void _intermediateMED::numerotationMaillage()
{
    // numerotation des mailles par entité
    int i_maille=0;
    std::set<_maille>::iterator i=maillage.begin();
    int dimension=i->dimension();
    for( ; i!=maillage.end(); ++i)
    {
	if ( dimension!=i->dimension() ) // on change d'entite
	{
	    dimension=i->dimension();
	    i_maille=0;
	}
	(*i).ordre=++i_maille;
    }
}

void _intermediateMED::numerotationPoints()
{
    // Fonction de renumerotation des noeuds (necessaire quand il y a des trous dans la numerotation.
    int i_noeud=0;
    for( std::map<int,_noeud>::const_iterator i=points.begin(); i!=points.end(); ++i)
	i->second.number = ++i_noeud ;
}
    

/*!
 * \if developper
 * create a MED COORDINATE from the intermediate structure.
 * \endif
 */
COORDINATE *
_intermediateMED::getCoordinate()
{
    const medModeSwitch mode=MED_FULL_INTERLACE;
    const string coordinateSystem="CARTESIAN";

    int spaceDimension=points.begin()->second.coord.size();
    int numberOfNodes=points.size();


    // creation du tableau des coordonnees en mode MED_FULL_INTERLACE
    double * coord = new double[spaceDimension*numberOfNodes];
    int k=0;
    for( std::map<int,_noeud>::const_iterator i=points.begin(); i!=points.end(); ++i, ++k)
	std::copy(i->second.coord.begin(), i->second.coord.end(), coord+k*spaceDimension);

    // creation de l'objet COORDINATE
    COORDINATE * coordinate = new COORDINATE(spaceDimension, numberOfNodes, mode);
    coordinate->setCoordinates(mode,coord);
    delete [] coord;
    coordinate->setCoordinatesSystem(coordinateSystem);
    return coordinate;
}


  /*!
   * \if developper
   * create a MED CONNECTIVITY from the intermediate structure.
   * for memory optimisation, clear the intermediate structure (the "maillage" set )
   * \endif
   */
CONNECTIVITY * 
_intermediateMED::getConnectivity()
{
    const char * LOC = "_intermediateMED::getConnectivity() : ";
    BEGIN_OF(LOC);
    int numberOfNodes=points.size(); // number of nodes in the mesh
    int numberOfTypes=0;
    medEntityMesh entity;
    medGeometryElement * types=NULL; // pointeurs pour allouer les structures MED
    int * count=NULL;
    int * connectivity=NULL;
    CONNECTIVITY *Connectivity, *Constituent;

    medGeometryElement type=0; // variables de travail
    int nbtype=0;
    int dimension=0;
    bool first = true;

    std::vector<medGeometryElement> vtype; // tableau de travail : stockage des types geometriques pour UNE entite
    std::vector<int> vcount; // tableau de travail : nombre d'elements pour chaque type geometrique de vtype

    std::set<_maille>::const_iterator i=maillage.begin(); // iterateurs sur les mailles
    std::set<_maille>::const_iterator j=maillage.begin();

    // renumerote les points de 1 a n (pour le cas ou certains points ne sont pas presents dans le maillage d'origine) 
    numerotationPoints(); 

    do 
    {
	// boucle sur les entites de la structure maillage :
	//   - on parcourt une premiere fois avec i les mailles d'une entite pour récupérer 
	//     des infos sur les types geometriques, leur nombre et le nombre d'elements.
	//   - on alloue la connectivite
	//   - on parcourt une deuxieme fois avec j pour lire les noeuds.


	type=i->geometricType; // init boucle for
	dimension=i->dimension();
	nbtype=0;
	vtype.push_back(type);
	// Boucle sur i de parcours des mailles d'une entite
	// Une entite se termine lorsqu'on atteint la fin de maillage ou lorsque la dimension des mailles change
	for( ; i!=maillage.end() && dimension==i->dimension() ; ++i)
	{
	    if (type != i->geometricType) // si changement de type geometrique
	    {
		vcount.push_back(nbtype); // stocke le nombre d'elements de type nbtype
		nbtype=0;
		type=i->geometricType;
		vtype.push_back(type); // stocke le nouveau type geometrique rencontre
	    }

	    ++nbtype;
	}
	vcount.push_back(nbtype); // n'a pas été stocké dans la boucle

	
	// Pour l'entite qu'on vient de parcourir, allocation des tableau et creation de la connectivite
//	cout << "Dimension = " << dimension << endl;
//	cout << "Nombre de type geometriques : " << vtype.size() << endl;
//	for (unsigned k=0; k!=vtype.size(); ++k )
//	    cout << "  -> " << vtype[k] << " : " << vcount[k] << endl;

	numberOfTypes=vtype.size(); // nombre de types de l'entite
	
	if ( i==maillage.end() ) // cas de l'entite de plus haut niveau
	    entity=MED_CELL;
	else if (dimension==2 )
	    entity=MED_FACE;
	else if (dimension==1 )
	    entity=MED_EDGE;
	else if (dimension==0 )
	    entity=MED_NODE;

	Connectivity = new CONNECTIVITY(numberOfTypes,entity);
	Connectivity->setEntityDimension(dimension);
	Connectivity->setNumberOfNodes(numberOfNodes);
	
	types = new medGeometryElement[numberOfTypes];
	std::copy(vtype.begin(),vtype.end(),types);
	Connectivity->setGeometricTypes(types,entity);
	delete [] types;

	count = new int[numberOfTypes+1];
	count[0]=1;
	for (unsigned int k=0; k!=vcount.size(); ++k )
	  count[k+1]=count[k]+vcount[k];
	Connectivity->setCount (count, entity);
	delete [] count;

	for (int k=0; k!=numberOfTypes; ++k )
	  {
	    // pour chaque type geometrique k, copie des sommets dans connectivity et set dans Connectivity
	    int nbSommetsParMaille = j->sommets.size();
	    int nbSommets = vcount[k] * j->sommets.size();
	    connectivity = new int[ nbSommets ];
	    for (int l=0; l!=vcount[k]; ++l, ++j)
	    {
		for ( unsigned n=0; n != j->sommets.size(); ++n)
		    connectivity[nbSommetsParMaille*l+n] = j->sommets[n]->second.number;
		maillage.erase(j);    ; // dangereux, mais optimise la mémoire consommée!
	    }

	    Connectivity->setNodal  (connectivity, entity, vtype[k]);
	    delete [] connectivity;
	  }
		
	if (i!=j)
	    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Erreur de lecture des mailles ."));

	if ( ! first)
	    Connectivity->setConstituent (Constituent);
	else
	    first = false;
	Constituent = Connectivity; // stocke Connectivity pour utilisation dans setConstituent lors de la boucle suivante

	vtype.clear();
	vcount.clear();

    }
    while ( i != maillage.end() );

    END_OF(LOC);
    return Connectivity;
}


  /*!
   * \if developper
   * fill the arguments vector of groups from the intermediate structure.
   * This function must be called before getConnectivity()
   * \endif
   */
void
_intermediateMED::getGroups(std::vector<GROUP *> & _groupCell, std::vector<GROUP *> & _groupFace, std::vector<GROUP *> & _groupEdge, std::vector<GROUP *> & _groupNode, MESH * _ptrMesh)
{
    const char * LOC = "_intermediateMED::getGroups() : ";
    BEGIN_OF(LOC);
    if (maillage.size() == 0)
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Erreur : il n'y a plus de mailles (appeler getConnectivity apres getGroups!)."));

    int dimension_maillage=maillage.rbegin()->dimension();

    numerotationMaillage(); // Renumerotation des mailles par entite

    for (unsigned int i=0; i!=this->groupes.size(); ++i)
    {
	if (groupes[i].mailles.size()==0)
	    continue; // si le groupe est vide, on passe au suivant

	int nb_geometric_types=1;
	_groupe::mailleIter j=groupes[i].mailles.begin(); 
	// initialise groupe_entity a l'entite de la premiere maille du groupe
	medEntityMesh groupe_entity = (**j).getEntity(dimension_maillage);
	medGeometryElement geometrictype=(**j).geometricType;

	//Parcours des mailles (a partir de la deuxieme) pour compter les types geometriques
	for ( ++j ; j!=groupes[i].mailles.end(); ++j )
	{
	    //Compte nombre de types geometriques
	    if ( (**j).geometricType != geometrictype ) // si on change de type geometrique
	    {
		nb_geometric_types++;
		geometrictype=(**j).geometricType;
	    }

	    //Test si groupe valide : le groupe doit pointer vers des entites de meme dimension
	    if ((**j).dimension() != dimension_maillage)
		continue;
	}

	// le groupe est valide -> on le traite
	MED_EN::medGeometryElement * tab_types_geometriques = new MED_EN::medGeometryElement[nb_geometric_types];
	int * tab_index_types_geometriques = new int[nb_geometric_types+1];
	int * tab_numeros_elements = new int[groupes[i].mailles.size()];
	int * tab_nombres_elements = new int[nb_geometric_types];

	//Remplit tableaux entree des methodes set
	int indice_mailles=0;
	j=groupes[i].mailles.begin();
	geometrictype=(**j).geometricType;
	tab_index_types_geometriques[0]=1;
	int indice_types_geometriques=1;
	tab_types_geometriques[0]=geometrictype;
	//parcours des mailles du groupe
	for (  ; j!=groupes[i].mailles.end(); ++j , ++indice_mailles)
	{
	    tab_numeros_elements[indice_mailles]=((**j).ordre);
	    if ((**j).geometricType != geometrictype)
	    {
		tab_index_types_geometriques[indice_types_geometriques]=indice_mailles+1;
		geometrictype=(**j).geometricType;
		tab_types_geometriques[indice_types_geometriques]=geometrictype;
		++indice_types_geometriques;
	    }
	}
	tab_index_types_geometriques[indice_types_geometriques]=indice_mailles+1;
	for (int k=0; k != nb_geometric_types; ++k)
	{
	    tab_nombres_elements[k] = tab_index_types_geometriques[k+1]-tab_index_types_geometriques[k];
	}

	//Determination type entite du groupe
	vector <GROUP *> * vect_group;
	switch ( groupe_entity )
	{
	    case MED_CELL :
		vect_group= & _groupCell;
		break;
	    case MED_FACE :
		vect_group= & _groupFace;
		break;
	    case MED_EDGE :
		vect_group= & _groupEdge;
		break;
	    case MED_NODE :
		vect_group= & _groupNode;
		break;
	}
	//Creation nouveau groupe MED
	GROUP * new_group = new GROUP();
	//Appel methodes set
	new_group->setTotalNumberOfElements(groupes[i].mailles.size());
	new_group->setName(groupes[i].nom);
	new_group->setMesh(_ptrMesh);
	new_group->setNumberOfGeometricType(nb_geometric_types);
	new_group->setGeometricType(tab_types_geometriques);
	new_group->setNumberOfElements(tab_nombres_elements);
	new_group->setNumber(tab_index_types_geometriques,tab_numeros_elements);
	new_group->setEntity(groupe_entity);
	new_group->setAll(groupes[i].mailles.size() == maillage.size());
	vect_group->push_back(new_group);
	delete [] tab_types_geometriques;
	delete [] tab_index_types_geometriques;
	delete [] tab_numeros_elements;
	delete [] tab_nombres_elements;
    }

    END_OF(LOC);
}

/////
