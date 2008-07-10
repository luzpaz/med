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
#include "MEDMEM_DriverTools.hxx"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Field.hxx"
#include <iomanip>
#include <algorithm>

using namespace std;
using namespace MED_EN;

#define DUMP_LINES_LIMIT 20

namespace MEDMEM {

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

_link _maille::link(int i) const
{
  ASSERT ( i >= 0 && i < sommets.size() );
  int i2 = ( i + 1 == sommets.size() ) ? 0 : i + 1;
  if ( reverse )
    return make_pair( sommets[i2]->first, sommets[i]->first );
  else
    return make_pair( sommets[i]->first, sommets[i2]->first );
}

// retourne l'entité d'une maille en fonction de la dimension du maillage.
MED_EN::medEntityMesh _maille::getEntity(const int meshDimension) const throw (MEDEXCEPTION)
{
  const char * LOC = "_maille::getEntity(const int meshDimension)" ;
//   BEGIN_OF(LOC);

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

//END_OF(LOC);
};

std::ostream& operator << (std::ostream& os, const _maille& ma)
{
    os << "maille " << ma.ordre << " (" << ma.geometricType << ") : < ";
    std::vector< std::map<int,_noeud>::iterator >::const_iterator i=ma.sommets.begin();
    os << (*i++)->second.number ;
    for( ; i!=ma.sommets.end(); ++i)
	os << ", " << (*i)->second.number;
    os << " >";
    return os;
}

std::ostream& operator << (std::ostream& os, const _groupe& gr)
{
    os << "--- Groupe " << gr.nom << " --- " << std::endl ;
    os << " -> liste des sous-groupes : ";
    for( std::vector<int>::const_iterator i=gr.groupes.begin(); i!=gr.groupes.end(); ++i)
	    os << *i << " ";
    
    os << std::endl << " -> liste des "<< gr.mailles.size() << " mailles : " << std::endl;
    
    _groupe::mailleIter i1=gr.mailles.begin();
    int l;
    for(l = 0; l < DUMP_LINES_LIMIT && i1!=gr.mailles.end(); i1++, l++)
	    os << setw(3) << l+1 << " " << *(*i1) << std::endl;
    
    if ( l == DUMP_LINES_LIMIT )
      os << "   ... skip " << gr.mailles.size() - l << " mailles" << endl;
    
    os << " relocMap, size=" << gr.relocMap.size() << endl;
    map<const _maille*,int>::const_iterator it = gr.relocMap.begin();
    for ( l = 0; l < DUMP_LINES_LIMIT && it != gr.relocMap.end(); ++it, ++l )
      os << " (" << it->first << "," << it->second << ")";
    if ( gr.relocMap.size() > 0 )
      os << endl;
    return os;
}

std::ostream& operator << (std::ostream& os, const _noeud& no)
{
    os << "noeud " << no.number << " : < ";
    std::vector<double>::const_iterator i=no.coord.begin();
    os << *i++ ;
    for( ; i!=no.coord.end(); ++i)
	os << ", " << *i;
    os << " >";
    return os;
}

void MEDMEM::_fieldBase::dump(std::ostream& os) const
{
  os << "field " << "<" << _name << ">" << endl <<
    "  nb sub: " << _sub.size() << endl <<
    "  group index: " << _group_id << endl <<
    "  type: " << _type << endl;
  os << "  subcomponents:" << endl;
  vector< _sub_data >::const_iterator sub_data = _sub.begin();
  for ( ; sub_data != _sub.end(); ++sub_data ) {
    os << "    group index: " << sub_data->_supp_id <<
      ", " << sub_data->nbComponents() << " comp names: ";
    for ( int i_comp = 0; i_comp < sub_data->nbComponents(); ++i_comp )
      os << " |" << sub_data->_comp_names[ i_comp ] << "|";
    os << endl;
  }
}

std::ostream& operator << (std::ostream& os, const _fieldBase * f)
{
  f->dump( os );
  return os;
}

std::ostream& operator << (std::ostream& os, const _intermediateMED& mi)
{
    os << "Set des " << mi.maillage.size() << " mailles : " << std::endl;
    std::set<_maille>::const_iterator i=mi.maillage.begin();
    int l;
    for( l = 0; l < DUMP_LINES_LIMIT && i!=mi.maillage.end(); ++i, ++l)
	os << setw(3) << l+1 << " " <<*i << std::endl;
    if ( l == DUMP_LINES_LIMIT )
      os << "   ... skip " << mi.maillage.size() - l << " mailles" << endl;
    
    os << std::endl << "Vector des " << mi.groupes.size() << " groupes : " << std::endl;
    for (unsigned int k=0; k!=mi.groupes.size(); k++)
      os << std::setw(3) << k << " " << mi.groupes[k] << std::endl;
    
    os << std::endl << "map des " << mi.points.size() << " noeuds : " << std::endl;
    std::map<int,_noeud>::const_iterator j=mi.points.begin();
    for( l = 0; l < DUMP_LINES_LIMIT && j!=mi.points.end(); ++j, ++l)
	os << j->second << std::endl;
    if ( l == DUMP_LINES_LIMIT )
      os << "   ... skip " << mi.points.size() - l << " noeuds" << endl;

    os << endl << mi.fields.size() << " fields:" << endl;
    std::list<_fieldBase* >::const_iterator fIt = mi.fields.begin();
    for ( l = 0; fIt != mi.fields.end(); ++fIt, ++l )
      os << " - " << l+1 << " " << *fIt << endl;

    return os;
}


//=======================================================================
//function : treatGroupes
//purpose  : detect groupes of mixed dimension and erase groupes that
//           won't be converted
//=======================================================================

void _intermediateMED::treatGroupes()
{
  const char * LOC = "_intermediateMED::treatGroupes() : ";
  BEGIN_OF(LOC);

  if ( myGroupsTreated )
    return;
  myGroupsTreated = true;
  
  // --------------------
  // erase useless group
  // --------------------

  // decrease hierarchical depth of subgroups
  vector<int>::iterator j;
  for (unsigned int i=0; i!=this->groupes.size(); ++i)
  {
    _groupe& grp = groupes[i];
    //INFOS( i << " " << grp.nom );
    j = grp.groupes.begin();
    while( j!=grp.groupes.end() ) {
      int grpInd = *j-1;
      if ( grpInd < 0 || grpInd >= (int)groupes.size() ) {
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Bad subgroup index: " << grpInd <<
                                     ", in " << i << " groupe.nom=" << grp.nom));
      }
      _groupe & sub_grp = groupes[ grpInd ];
      if ( !sub_grp.groupes.empty() ) {
        MESSAGE("High hierarchical depth of subgroups in group " << i );
        *j = sub_grp.groupes[0]; // replace j with its 1st subgroup
        // push back the rest subs
        for ( int k = 1; k < (int)sub_grp.groupes.size(); ++k )
          grp.groupes.push_back( sub_grp.groupes[ k ]);
        // vector maybe is reallocated: restart iterator
        j = grp.groupes.begin();
      }
      else
        j++;
    }
    // remove empty sub-groupes
    j = grp.groupes.begin();
    while ( j!=grp.groupes.end() ) {
      if ( groupes[*j-1].empty() ) {
        grp.groupes.erase( j );
        j = grp.groupes.begin();
      }
      else
        j++;
    }
  }
  // get indices of groups that are field support -
  // do not erase them and their subgroups
  std::set<int> groups2convert;
  std::list< _fieldBase* >::const_iterator fIt = fields.begin();
  for ( ; fIt != fields.end(); ++fIt )
    (*fIt)->getGroupIds( groups2convert, true );

  // keep named groups and their subgroups
  for (unsigned int i=0; i!=this->groupes.size(); ++i)
  {
    _groupe& grp = groupes[i];
    if ( grp.empty() || grp.nom.empty() )
      continue;
    groups2convert.insert( i );
    for( j = grp.groupes.begin(); j!=grp.groupes.end(); ++j)
      groups2convert.insert( *j-1 );
  }
  // erase groups that are not in groups2convert
  for (unsigned int i=0; i!=this->groupes.size(); ++i)
  {
    if ( groups2convert.find( i ) == groups2convert.end() ) {
      _groupe& grp = groupes[i];
      grp.mailles.clear();
      grp.groupes.clear();
      MESSAGE( "Erase " << i << "-th group " << grp.nom );
    }
  }

  // ---------------------------------------------------
  // define if there are groups with mixed entity types
  // ---------------------------------------------------

  hasMixedCells = false;  
  for (unsigned int i=0; i!=this->groupes.size(); ++i)
  {
    _groupe& grp = groupes[i];
    if ( grp.groupes.empty() )
      continue;

    // check if sub-groups have different dimension
    j = grp.groupes.begin();
    int dim = groupes[*j-1].mailles[0]->dimension();
    for( j++; /*!hasMixedCells &&*/ j!=grp.groupes.end(); ++j) {
      int dim2 = groupes[*j-1].mailles[0]->dimension();
      if ( dim != dim2 ) {
        if ( dim == 0 || dim2 == 0 ) {
          // cant create a group of nodes plus anything else
          grp.mailles.clear();
          grp.groupes.clear();
          MESSAGE( "Erase mixed dim group with nodes:" << i << "-th group " << grp.nom );
        }
        else {
          hasMixedCells = true;
          MESSAGE( "Mixed dim group: " << i << "-th " << grp.nom <<
                   "  dim1 = " << dim << " dim2 = " << dim2 );
        }
        break;
      }
    }
  }

//   if ( hasMixedCells )
//     INFOS( "There will be groups of mixed dimention" );
  END_OF(LOC);
}

void _intermediateMED::numerotationMaillage()
{
  const char * LOC = "_intermediateMED::numerotationMaillage() : ";
  BEGIN_OF(LOC);

  treatGroupes();

  // numerotation mailles of type MED_POINT1 by node number
  std::set<_maille>::iterator i=maillage.begin();
  if ( i->geometricType == MED_POINT1 ) {
    numerotationPoints();
    while ( i!=maillage.end() && i->geometricType == MED_POINT1 ) {
      i->ordre = i->sommets[0]->second.number;
      i++;
    }
  }

  // check if numeration is needed
  if ( i->ordre  > 0 && maillage.rbegin()->ordre > 0 )
  {
    // already numerated, check numeration
    bool ok = true;
    const int maxNbTypes = 20;
    pair< unsigned, unsigned > minMaxOrder[ maxNbTypes ];
    int nbElems[ maxNbTypes ];
    std::set<_maille>::iterator j=i;

    do { // loop on elements of one entity
      int iType = 0;
      minMaxOrder[ iType ] = make_pair( maillage.size(), 0 );
      int i_maille = 0, dimension = j->dimension(), type = j->geometricType;

      std::set<_maille>::iterator k=j;
      for ( ; j!=maillage.end() && (hasMixedCells || dimension==j->dimension()); ++j)
      {
        if (type != j->geometricType) // si changement de type geometrique
        {
          nbElems[ iType ] = i_maille;
          i_maille=0;
          type = j->geometricType;
          iType++;
          minMaxOrder[ iType ] = make_pair( maillage.size(), 0 );
        }
        ++i_maille;
        if ( j->ordre < minMaxOrder[ iType ].first )
          minMaxOrder[ iType ].first = j->ordre;
        if ( j->ordre > minMaxOrder[ iType ].second )
          minMaxOrder[ iType ].second = j->ordre;
      }
      nbElems[ iType ] = i_maille;

      bool renumEntity = false; 
      for ( int t = 0; t <= iType; ++t ) {
        int orderRange = minMaxOrder[ t ].second - minMaxOrder[ t ].first;
        if ( nbElems[ t ] != orderRange + 1 )
          ok = false;
        if ( t > 0 ) {
          if ( minMaxOrder[ t ].first == 1 )
            renumEntity = true;
          else if ( minMaxOrder[ t-1 ].second+1 != minMaxOrder[ t ].first )
            ok = false;
        }
      }
      if ( ok && renumEntity ) { // each geom type was numerated separately
        int iType = 0, i_shift = 0;
        type = k->geometricType;
        for ( ; k != j; ++k ) {
          if (type != k->geometricType) { // si changement de type geometrique
            i_shift += minMaxOrder[ iType++ ].second;
            type = k->geometricType;
          }
          k->ordre += i_shift;
        }
      }

    } while ( ok && j != maillage.end() );

    if ( ok ) return; // renumeration not needed
  }

  // numerotation des mailles par entité
  int i_maille=0;
  int dimension=i->dimension();
  for( ; i!=maillage.end(); ++i)
  {
    if ( !hasMixedCells && dimension != i->dimension() ) // on change d'entite
    {
      MESSAGE( "NB dim " << dimension << " entities: " << i_maille);
      dimension=i->dimension();
      i_maille=0;
    }
    i->ordre=++i_maille;
  }
  END_OF(LOC);
}

void _intermediateMED::numerotationPoints()
{
//   if ( myPointsNumerated )
//     return;
//   myPointsNumerated = true;
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
_intermediateMED::getCoordinate(const string & coordinateSystem)
{
    const medModeSwitch mode=MED_FULL_INTERLACE;
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
CONNECTIVITY * _intermediateMED::getConnectivity()
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
    //int dimension_maillage_moin_2=maillage.rbegin()->dimension() - 2;

    medGeometryElement type=0; // variables de travail
    int nbtype=0;
    int dimension=0;
    bool first = true;

    std::vector<medGeometryElement> vtype; // tableau de travail : stockage des types geometriques pour UNE entite
    std::vector<int> vcount; // tableau de travail : nombre d'elements pour chaque type geometrique de vtype

    std::set<_maille>::const_iterator i, j; // iterateurs sur les mailles

    // min and max element nb for each geom type
    const int maxNbTypes = 20;
    vector< pair< unsigned, unsigned > > minMaxOrder( maxNbTypes );

    // skip nodes and elements of <dimension_maillage - 2> or less dimension
    // Unfortunately, it is impossible because of MESH::createFamilies() that requires
    // presence of connectivity even for nodes!
    for ( i = maillage.begin(); i != maillage.end(); ++i )
      //if ( i->geometricType != MED_POINT1 && i->dimension() > dimension_maillage_moin_2 )
        break;
    j = i;

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

        int iType=0;
        minMaxOrder[ iType ] = make_pair( maillage.size(), 0 );

        // if hasMixedCells, store POINT1 elems as MED_NODE and
        // elems of all the rest types as MED_CELL, i.e. do not break the loop
        // when dimension changes
        bool ignoreDimChange = hasMixedCells && dimension > 0;
	for( ; i!=maillage.end() && ( ignoreDimChange || dimension==i->dimension()) ; ++i)
	{
	    if (type != i->geometricType) // si changement de type geometrique
	    {
		vcount.push_back(nbtype); // stocke le nombre d'elements de type nbtype
		nbtype=0;
		type=i->geometricType;
		vtype.push_back(type); // stocke le nouveau type geometrique rencontre
                dimension=i->dimension();
                iType++;
                minMaxOrder[ iType ] = make_pair( maillage.size(), 0 );
	    }
            if ( i->ordre < minMaxOrder[ iType ].first )
              minMaxOrder[ iType ].first = i->ordre;
            if ( i->ordre > minMaxOrder[ iType ].second )
              minMaxOrder[ iType ].second = i->ordre;
	    ++nbtype;
	}
	vcount.push_back(dimension ? nbtype : numberOfNodes); // n'a pas été stocké dans la boucle
	
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
	SCRUTE( entity );
	SCRUTE(numberOfTypes);
	SCRUTE(count[numberOfTypes]-1);
	delete [] count;

	for (int k=0; k!=numberOfTypes; ++k )
	  {
            int orderShift = 1;
            if ( minMaxOrder[ k ].first > 1 ) // min elem number != 1
              orderShift += minMaxOrder[ k-1 ].second; // max elem number in previous type
	    // pour chaque type géometrique k, copie des sommets dans connectivity et set dans Connectivity
	    int nbSommetsParMaille = j->sommets.size();
	    int n, nbSommets = vcount[k] * nbSommetsParMaille;
	    connectivity = new int[ nbSommets ];
	    for (int l=0; l!=vcount[k]; ++l) // loop on elements of geom type
	    {
                if ( entity==MED_NODE )
                  connectivity[l] = l+1;
                else
                {
                  int index0 = nbSommetsParMaille * ( j->ordre - orderShift );
                  for ( n=0; n != nbSommetsParMaille; ++n) {
		    //connectivity[nbSommetsParMaille*l+n] =
                    connectivity[ index0 + n ] =
                      j->sommets[ j->reverse ? nbSommetsParMaille-n-1 : n ]->second.number;
                  }
                // DO NOT ERASE, maillage will be used while fields construction
		//maillage.erase(j);    ; // dangereux, mais optimise la mémoire consommée!
                  j++;
                }
	    }	   

	    Connectivity->setNodal  (connectivity, entity, vtype[k]);
	    delete [] connectivity;
	  }

        if ( entity==MED_NODE )
          j = i;		
	else if (i!=j)
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
_intermediateMED::getGroups(vector<GROUP *> & _groupCell,
                            vector<GROUP *> & _groupFace,
                            vector<GROUP *> & _groupEdge,
                            vector<GROUP *> & _groupNode, MESH * _ptrMesh)
{
  const char * LOC = "_intermediateMED::getGroups() : ";
  BEGIN_OF(LOC);

  medGroupes.resize( groupes.size(), 0 );
  if (maillage.size() == 0) {
    INFOS( "Erreur : il n'y a plus de mailles");
    return;
  }

  // get indices of groups that are field support - do not skip them
  std::set<int> support_groups;
  std::list< _fieldBase* >::const_iterator fIt = fields.begin();
  for ( ; fIt != fields.end(); ++fIt )
    (*fIt)->getGroupIds( support_groups, false );

  numerotationMaillage(); // Renumerotation des mailles par entite

  int dimension_maillage=maillage.rbegin()->dimension();

  for (unsigned int i=0; i!=this->groupes.size(); ++i)
  {
    _groupe& grp = groupes[i];
    if ( medGroupes[ i ] )
      continue; // grp already converted into a MED group

    bool isFieldSupport = ( support_groups.find( i ) != support_groups.end() );
    // si le groupe est vide, ou si le groupe n'est pas nommé : on passe au suivant
    if ( grp.empty() || ( grp.nom.empty() && !isFieldSupport )) {
      if ( !grp.nom.empty() ) {
        INFOS("Skip group " << i << grp.nom);
//       } else {
//         MESSAGE("Skip group " << i << " <" << grp.nom << "> " << ( grp.empty() ? ": empty" : ""));
      }
      continue;
    }

    // Build a set of mailles: sort mailles by type and exclude maille doubling
    typedef set< set<_maille>::iterator, _mailleIteratorCompare > TMailleSet;
    TMailleSet mailleSet;
    if( grp.groupes.size() ) {// le groupe i contient des sous-maillages
      int nb_elem = 0;
      for( vector<int>::iterator j=grp.groupes.begin(); j!=grp.groupes.end(); ++j)
      {
        nb_elem += groupes[*j-1].mailles.size();
        _groupe::mailleIter maIt=groupes[*j-1].mailles.begin();
        for( ; maIt!=groupes[*j-1].mailles.end(); ++maIt) {
//           TMailleSet::const_iterator ma_it = mailleSet.find( *maIt );
//           if ( ma_it != mailleSet.end() ) {
//             MESSAGE("EQUAL ELEMS: " << *ma_it << " AND " << *maIt);
//           }
//           else
            mailleSet.insert( *maIt );
        }
      }
      if ( nb_elem != mailleSet.size() ) { // Self intersecting compound group
        INFOS("Self intersecting group: " << i << " <" << grp.nom << ">"
              << ", mailleSet.size = " << mailleSet.size() << ", sum nb elems = " << nb_elem);
        for( vector<int>::iterator j=grp.groupes.begin(); j!=grp.groupes.end(); ++j)
          INFOS(" in sub-group "<<  *j << " <" << groupes[*j-1].nom << "> "
                << groupes[*j-1].mailles.size() << " mailles of type "
                << groupes[*j-1].mailles[0]->geometricType);
        // if a group serve as a support to a field, then the _field is to be converted
        // into several MEDMEM::FIELDs, one per sub-group; if we already skipped some sub-groups,
        // we roll back the loop on groups to create MED groups for skipped ones
        if ( isFieldSupport ) {
          if ( grp.nom.empty() ) // clear group existing for the sake of field only
            grp.groupes.clear();
          std::set<int> sub_grps;
          for ( fIt = fields.begin(); fIt != fields.end(); ++fIt ) {
            _fieldBase * field = (*fIt);
            if ( field->_group_id == i ) {
              field->_group_id = -1; // -> a field by support
              field->getGroupIds( sub_grps, false );
            }
          }
          if ( (int)i > *sub_grps.begin() ) { // roll back
            support_groups.erase( i );
            support_groups.insert( sub_grps.begin(), sub_grps.end() ); 
            i = *sub_grps.begin() - 1;
            continue;
          }
        }
      }
    }
    else {
      _groupe::mailleIter maIt=grp.mailles.begin();
      for(; maIt!=grp.mailles.end(); ++maIt)
        mailleSet.insert( *maIt );
      if ( grp.mailles.size() != mailleSet.size() )
        INFOS( "Self intersecting group: " << i << " <" << grp.nom << ">"
               << ", mailleSet.size = " << mailleSet.size()
               << ", nb elems = " << grp.mailles.size());
    }
    // MEDMEM does not allow constituents of <mesh_dim>-2 and less dimension
    // but do not skip node group
    int group_min_dim = (**mailleSet.begin()).dimension();
    int group_max_dim = (**(--mailleSet.end())).dimension();
    if ( group_max_dim != 0 && group_min_dim <= dimension_maillage - 2  ) {
      MESSAGE("Skip group: " << i << " <" << grp.nom << "> - too small dimension: "
              << group_min_dim);
      continue;
    }

    // 1. Build a map _maille* -> index in MEDMEM::GROUP.getNumber(MED_ALL_ELEMENTS).
    // It is used while fields building.
    // 2. make mailles know the groups they belong to, that is used in getFamilies()
    TMailleSet::iterator maIt = mailleSet.begin();
    int iMa;
    for ( iMa = 0; maIt != mailleSet.end(); maIt++ ) {
      grp.relocMap.insert( make_pair( &(**maIt), ++iMa ));
      (*maIt)->groupes.push_back( i );
    }
    ASSERT( iMa == grp.relocMap.size() );

    int nb_geometric_types=1;
    TMailleSet::iterator j=mailleSet.begin(); 
    // initialise groupe_entity a l'entite de la premiere maille du groupe
    medEntityMesh groupe_entity = (**mailleSet.rbegin()).getEntity(dimension_maillage);
    if ( hasMixedCells && group_min_dim > 0 )
      groupe_entity = MED_CELL;
    medGeometryElement geometrictype=(**j).geometricType;

    //Parcours des mailles (a partir de la deuxieme) pour compter les types geometriques
    for ( ++j ; j!=mailleSet.end(); ++j )
    {
      //Compte nombre de types geometriques
      if ( (**j).geometricType != geometrictype ) // si on change de type geometrique
      {
        nb_geometric_types++;
        geometrictype=(**j).geometricType;
      }
    }

    MED_EN::medGeometryElement * tab_types_geometriques = new MED_EN::medGeometryElement[nb_geometric_types];
    int * tab_index_types_geometriques = new int[nb_geometric_types+1];
    int * tab_numeros_elements = new int[mailleSet.size()];
    int * tab_nombres_elements = new int[nb_geometric_types];

    //Remplit tableaux entree des methodes set
    int indice_mailles=0/*, maxOrdre = -1*/;
    j=mailleSet.begin();
    geometrictype=(**j).geometricType;
    tab_index_types_geometriques[0]=1;
    int indice_types_geometriques=1;
    tab_types_geometriques[0]=geometrictype;
    //parcours des mailles du groupe
    for (  ; j!=mailleSet.end(); ++j , ++indice_mailles)
    {
      const _maille& ma = **j;
      tab_numeros_elements[indice_mailles]= ma.ordre;
//       if ( maxOrdre < tab_numeros_elements[indice_mailles] )
//         maxOrdre = tab_numeros_elements[indice_mailles];
      if (ma.geometricType != geometrictype)
      {
        tab_index_types_geometriques[indice_types_geometriques]=indice_mailles+1;
        geometrictype=ma.geometricType;
        tab_types_geometriques[indice_types_geometriques]=geometrictype;
        ++indice_types_geometriques;
      }
    }
    tab_index_types_geometriques[indice_types_geometriques]=indice_mailles+1;
    for (int k=0; k != nb_geometric_types; ++k)
    {
      tab_nombres_elements[k] = tab_index_types_geometriques[k+1]-tab_index_types_geometriques[k];
    }
    //INFOS( "MAX ORDRE in grp " << grp.nom << " entity " << groupe_entity << " : " << maxOrdre);

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
    medGroupes[ i ] = new_group;
    //Appel methodes set
    new_group->setTotalNumberOfElements(mailleSet.size());
    new_group->setName(grp.nom);
    new_group->setMesh(_ptrMesh);
    new_group->setNumberOfGeometricType(nb_geometric_types);
    new_group->setGeometricType(tab_types_geometriques);
    new_group->setNumberOfElements(tab_nombres_elements);
    new_group->setNumber(tab_index_types_geometriques,tab_numeros_elements);
    new_group->setEntity(groupe_entity);
    new_group->setAll(mailleSet.size() == maillage.size());

    vect_group->push_back(new_group);

    delete [] tab_types_geometriques;
    delete [] tab_index_types_geometriques;
    delete [] tab_numeros_elements;
    delete [] tab_nombres_elements;
  }

  END_OF(LOC);
}

//=======================================================================
//function : getFamilies
//purpose  : create families like MESH::createFamilies() but preserves
//           the order of elements in GROUPs defined by constituent families
//           order. Call it after getGroups()
//=======================================================================

void _intermediateMED::getFamilies(std::vector<FAMILY *> & _famCell,
                                   std::vector<FAMILY *> & _famFace, 
                                   std::vector<FAMILY *> & _famEdge,
                                   std::vector<FAMILY *> & _famNode, MESH * _ptrMesh)
{
  const char * LOC = "_intermediateMED::getFamilies() : ";
  BEGIN_OF(LOC);
  
  int nbElemFam = 0, nbNodeFam = 0;
  std::map< GROUP*, vector< FAMILY * > > grpFamsMap;
  int dimension_maillage=maillage.rbegin()->dimension();

  std::set<_maille>::const_iterator i=maillage.begin(); // iterateurs sur les mailles
  std::set<_maille>::const_iterator j=maillage.begin();

  do
  {
    // make a family containing mailles shared by the same set of groups
    std::list<unsigned>&  grList = i->groupes;  // to define the family end
    int           dimension = i->dimension();        // to define the entity end
    medGeometryElement type = i->geometricType;
    medEntityMesh    entity = i->getEntity( dimension_maillage );

    std::vector<medGeometryElement> tab_types_geometriques;
    std::vector<int> tab_index_types_geometriques;
    std::vector<int> tab_nombres_elements;
    std::vector<int> tab_numeros_elements;

    int iMa = 1, nbtype = 0;
    tab_types_geometriques.push_back( type );
    tab_index_types_geometriques.push_back( iMa );

    // scan family cells and fill the tab that are needed by the create a MED FAMILY
    while (i != maillage.end() &&
           i->groupes == grList &&
           i->dimension() == dimension)
    {
      if (type != i->geometricType) // si changement de type geometrique
      {
        tab_index_types_geometriques.push_back(iMa);
        tab_nombres_elements.push_back(nbtype);
        nbtype=0;
        type=i->geometricType;
        tab_types_geometriques.push_back(type); // stocke le nouveau type geometrique rencontre
      }
      ++nbtype;
      ++iMa;
      ++i;
    }
    tab_index_types_geometriques.push_back(iMa);
    tab_nombres_elements.push_back(nbtype); // n'a pas été stocké dans la boucle

    tab_numeros_elements.resize( iMa - 1 );
    for ( iMa = 0; j != i; j++, iMa++ )
      tab_numeros_elements[ iMa ] = j->ordre;

    int id = ( entity == MED_NODE ? ++nbNodeFam : -(++nbElemFam) );

    ostringstream name;
    name << "FAM_" << id;

    // create a empty MED FAMILY and fill it with the tabs we constructed
    FAMILY* newFam = new FAMILY();
    newFam->setTotalNumberOfElements( iMa );
    newFam->setName( name.str() );
    newFam->setMesh( _ptrMesh );
    newFam->setNumberOfGeometricType( tab_types_geometriques.size() );
    newFam->setGeometricType( &tab_types_geometriques[0] ); // we know the tab is not empy
    newFam->setNumberOfElements( &tab_nombres_elements[0] );
    newFam->setNumber( &tab_index_types_geometriques[0], &tab_numeros_elements[0] );
    newFam->setEntity( entity );
    newFam->setAll( false );
    newFam->setIdentifier( id );
    newFam->setNumberOfGroups( grList.size() );

    // Update links between families and groups
    if ( ! grList.empty() )
    {
      std::string * groupNames = new string[ grList.size() ];
      std::list<unsigned>::iterator g = grList.begin();
      for ( int i = 0; g != grList.end(); ++g, ++i ) {
        GROUP * medGROUP = getGroup( *g );
        groupNames[ i ] = medGROUP->getName();
        grpFamsMap[ medGROUP ].push_back( newFam );
      }
      newFam->setGroupsNames(groupNames);
    }
    // store newFam
    std::vector<FAMILY*>* families = 0;
    switch ( entity )
    {
    case MED_CELL :
      families = & _famCell; break;
    case MED_FACE :
      families = & _famFace; break;
    case MED_EDGE :
      families = & _famEdge; break;
    case MED_NODE :
      families = & _famNode; break;
    }
    if ( families )
      families->push_back( newFam );

  } while ( i != maillage.end() );

  // update references in groups
  std::map< GROUP*, vector< FAMILY * > >::iterator gf = grpFamsMap.begin();
  for ( ; gf != grpFamsMap.end(); ++gf )
  {
    gf->first->setNumberOfFamilies( gf->second.size() );
    gf->first->setFamilies( gf->second );
  }
}

//=======================================================================
//function : getGroup
//purpose  : 
//=======================================================================

GROUP * _intermediateMED::getGroup( int i )
{
  if ( i <(int) medGroupes.size() )
    return medGroupes[ i ];
  throw MEDEXCEPTION
    (LOCALIZED(STRING("_intermediateMED::getGroup(): WRONG GROUP INDEX: ")
               << medGroupes.size() << " <= " << i ));
}

//=======================================================================
//function : getFields
//purpose  : 
//=======================================================================

void _intermediateMED::getFields(std::list< FIELD_* >& theFields)
{
  const char * LOC = "_intermediateMED::getFields() : ";
  BEGIN_OF(LOC);

  std::list< _fieldBase* >::const_iterator fIt = fields.begin();
  for ( ; fIt != fields.end(); fIt++ )
  {
    const _fieldBase* fb = *fIt;
    list<pair< FIELD_*, int> >  ff = fb->getField(groupes, medGroupes);
    list<pair< FIELD_*, int> >::iterator f_sup = ff.begin();
    for (int j = 1 ; f_sup != ff.end(); f_sup++, ++j )
    {
      FIELD_    * f = f_sup->first;
      SUPPORT * sup = getGroup( f_sup->second );
      if ( !sup )
        throw MEDEXCEPTION
          (LOCALIZED(STRING(LOC) <<"_intermediateMED::getFields(), NULL field support: "
                     << " group index: " << fb->_group_id));
      int nb_elems = sup->getNumberOfElements( MED_ALL_ELEMENTS );
      if ( nb_elems != f->getNumberOfValues() )
        throw MEDEXCEPTION
          (LOCALIZED(STRING("_intermediateMED::getFields(), field support size (")
                     << nb_elems  << ") != NumberOfValues (" << f->getNumberOfValues()));
      theFields.push_back( f );
      if ( sup->getName().empty() ) {
        ostringstream name;
        name << "GRP_" << f->getName() << "_" << j;
        sup->setName( name.str() );
      }
      f->setSupport( sup );
      //f->setIterationNumber( j );
      f->setOrderNumber( j );
    }
  }
  END_OF(LOC);
}

_intermediateMED::~_intermediateMED()
{
  MESSAGE( "~_intermediateMED()");
  std::list< _fieldBase* >::const_iterator fIt = fields.begin();
  for ( ; fIt != fields.end(); fIt++ )
    delete *fIt;
}

//=======================================================================
//function : getGroupIds
//purpose  : return ids of main and/or sub- groups
//=======================================================================

void _fieldBase::getGroupIds( std::set<int> & ids, bool all ) const
{
  if ( hasCommonSupport() )
    ids.insert( _group_id );
  if ( all || !hasCommonSupport() ) {
    vector< _sub_data >::const_iterator sub = _sub.begin();
    for ( ; sub != _sub.end(); ++sub )
      ids.insert( sub->_supp_id );
  }
}

//=======================================================================
//function : hasSameComponentsBySupport
//purpose  : 
//=======================================================================

bool _fieldBase::hasSameComponentsBySupport() const
{
  vector< _sub_data >::const_iterator sub_data = _sub.begin();
  const _sub_data& first_sub_data = *sub_data;
  for ( ++sub_data ; sub_data != _sub.end(); ++sub_data )
  {
    if ( first_sub_data._comp_names != sub_data->_comp_names )
      return false; // diff names of components

    if ( first_sub_data._nb_gauss != sub_data->_nb_gauss )
      return false; // diff nb of gauss points
  }
  return true;
}

}

/////
