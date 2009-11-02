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

#include <algorithm>
#include <queue>
#include <list>

#include "MEDMEM_GibiMeshDriver.hxx"

#include "MEDMEM_DriversDef.hxx"

#include "MEDMEM_Med.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_DriverTools.hxx"

class MEDMEM::FIELD_;

#include <Basics_Utils.hxx>

#include <stdio.h>
#include <fcntl.h>
#ifdef WNT
#include <io.h>
#else
#include <unistd.h>
#endif

#include <float.h>

/////
using namespace std;
using namespace MED_EN;
using namespace MEDMEM;
/////

// read or not non-named fields
//#define GIBI_READ_ONLY_NAMED_FIELD

// to throw an exception when try to write a name longer than 8 or non-unique encounters
//#define THROW_ON_BAD_NAME

// to see full dump of RESULTATS STRUCTURE INTERMEDIAIRES
#ifdef _DEBUG_
// #undef MESSAGE_MED
// #define MESSAGE_MED(txt) std::cout << txt << endl;
// #undef INFOS_MED
// #define INFOS_MED(txt) std::cout << txt << endl;
#endif

// Every memory allocation made in the MedDriver members function are desallocated in the Mesh destructor

/////
//const size_t GIBI_MESH_DRIVER::nb_geometrie_gibi;

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

//=======================================================================
//function : gibi2medGeom
//purpose  :
//=======================================================================

medGeometryElement GIBI_MESH_DRIVER::gibi2medGeom( size_t gibiTypeNb )
{
  if ( gibiTypeNb < 1 || gibiTypeNb > 47 )
    return MED_NONE;

  return geomGIBItoMED[ gibiTypeNb - 1 ];
}

//=======================================================================
//function : med2gibiGeom
//purpose  :
//=======================================================================

int GIBI_MESH_DRIVER::med2gibiGeom( medGeometryElement medGeomType )
{
  for ( int i = 0; i < nb_geometrie_gibi; i++ )
    if ( geomGIBItoMED[ i ] == medGeomType )
      return i + 1;

  return -1;
}

//=======================================================================
//function : getGroupId
//purpose  :
//=======================================================================

static int getGroupId(const vector<int>& support_ids, _intermediateMED*  medi)
{
  int group_id = 0;
  vector<int>::const_iterator sb = support_ids.begin(), se = support_ids.end();
  if (support_ids.size() == 1 || // one or equal support ids
      *std::max_element( sb, se ) == *std::min_element( sb, se ))
  {
    group_id = support_ids[0] - 1;
  }
  else
  {
    // try to find an existing group with the same sub-groups
    set<int> sup_set;
    sup_set.insert( sb, se );

    for ( group_id = 0; group_id < (int)medi->groupes.size(); ++group_id )
    {
      if (sup_set.size() == medi->groupes[ group_id ].groupes.size() &&
          std::equal (sup_set.begin(), sup_set.end(),
                      medi->groupes[ group_id ].groupes.begin()))
        break;
    }
    if ( group_id == medi->groupes.size() )
    {
      // no such a group, add a new one
      medi->groupes.push_back( _groupe() );
      _groupe& new_grp = medi->groupes.back();
      //new_grp.nom = string( group_id % 10 + 1, 'G' );
      new_grp.groupes.reserve( sup_set.size() );
      for ( set<int>::iterator it = sup_set.begin(); it != sup_set.end(); it++ ) {
        new_grp.groupes.push_back( *it );
        //new_grp.nom += "_" + medi->groupes[ *it - 1 ].nom;
      }
    }
  }
  return group_id;
}

//=======================================================================
//function : isNamedObject
//purpose  :
//=======================================================================

//#ifdef GIBI_READ_ONLY_NAMED_FIELD
static inline bool isNamedObject( int obj_index, const vector<int>& indices_objets_nommes )
{
  return ( std::find( indices_objets_nommes.begin(), indices_objets_nommes.end(), obj_index)
           != indices_objets_nommes.end() );
}
//#endif

//=======================================================================
//function : setFieldNames
//purpose  : set field names
//=======================================================================

static void setFieldNames(const vector< _fieldBase* >& fields,
                          const vector<string>&        objets_nommes,
                          const vector<int>&           indices_objets_nommes)
{
  // set field names
  int i;
  set<string> fnames;
  for ( i = 0; i < indices_objets_nommes.size(); ++i ) {
    int fieldIndex = indices_objets_nommes[ i ];
    fnames.insert( objets_nommes[ i ]);
    if ( fields[ fieldIndex - 1 ] )
      fields[ fieldIndex - 1 ]->_name = objets_nommes[ i ];
  }
  int noNameIndex = 0;
  for ( int i = 0; i < fields.size(); ++i ) {
    if ( !fields[ i ] ) {
      if ( !isNamedObject( i+1, indices_objets_nommes ))
        ++noNameIndex;
    }
    else if ( fields[ i ]->_name.empty() ) {
      do {
        fields[ i ]->_name = STRING("F_") << ++noNameIndex;
      } while ( !fnames.insert( fields[ i ]->_name ).second );
    }
  }
}

//=======================================================================
//function : read
//purpose  :
//=======================================================================

#define GIBI_EQUAL(var_str, stat_str) \
  (strncmp (var_str, stat_str, strlen(stat_str)) == 0)
#define DUMP_LINE_NB " on line " << _lineNb

bool GIBI_MESH_RDONLY_DRIVER::readFile (_intermediateMED* medi, bool readFields )
{
  const char* LOC = "GIBI_MESH_RDONLY_DRIVER::readFile() : ";
  BEGIN_OF_MED(LOC);

  // LECTURE DES DONNEES DS FICHIER GIBI

  enum Readable_Piles {
    PILE_SOUS_MAILLAGE=1,
    PILE_NODES_FIELD  =2,
    PILE_TABLES       =10,
    PILE_STRINGS      =27,
    PILE_NOEUDS       =32,
    PILE_COORDONNEES  =33,
    PILE_FIELD        =39,
    PILE_LAST_READABLE=39
  };
  Readable_Piles readable_Piles [] = {
    PILE_SOUS_MAILLAGE,
    PILE_NODES_FIELD,
    PILE_TABLES,
    PILE_STRINGS,
    PILE_NOEUDS,
    PILE_COORDONNEES,
    PILE_FIELD,
    PILE_LAST_READABLE
  };
  // other known piles:
  // PILE NUMERO  26 - Integers
  // PILE NUMERO  25 - Floats

  char* ligne; // pour lire une ligne
  const char* enregistrement_type=" ENREGISTREMENT DE TYPE";
  vector<int> numero_noeuds; // tableau de travail (indices)
  set<int> donePiles; // already read piles
  unsigned space_dimension = 0;

  // IMP 0020434: mapping GIBI names to MED names
  list<nameGIBItoMED> listGIBItoMED_mail; // to be read from PILE_TABLES, from table "MED_MAIL"
  list<nameGIBItoMED> listGIBItoMED_cham; // to be read from PILE_TABLES, from table "MED_CHAM"
  list<nameGIBItoMED> listGIBItoMED_comp; // to be read from PILE_TABLES, from table "MED_COMP"
  map<int,string> mapStrings; // to be read from PILE_STRINGS
  vector<_fieldBase*> node_fields;
  vector<_fieldBase*> cell_fields;

  while ( getNextLine(ligne, false)) // boucle externe de recherche de "ENREGISTREMENT DE TYPE"
  {
    if ( !GIBI_EQUAL( ligne, enregistrement_type ))
      continue; // "ENREGISTREMENT DE TYPE" non trouvé -> on lit la ligne suivante

    // lecture du numéro d'enregistrement
    int numero_enregistrement = atoi( ligne + strlen(enregistrement_type) + 1 );

    enum { ENREG_TYPE_2=2, ENREG_TYPE_4=4}; // énumération des types d'enregistrement traités
    int numero_pile, nb_objets_nommes, nb_objets, nb_indices;
    vector<int> indices_objets_nommes;
    vector<string> objets_nommes;

    if (numero_enregistrement == ENREG_TYPE_4)
    {
      getNextLine(ligne);
      const char* s = " NIVEAU  15 NIVEAU ERREUR   0 DIMENSION";
      space_dimension = atoi( ligne + strlen( s ) + 1 );
      if ( !GIBI_EQUAL( ligne, " NIVEAU" ) || space_dimension < 1 ) {
        INFOS_MED( " Could not read file: syntax error in type 4 record" << DUMP_LINE_NB );
        return false;
      }
    }
    else if (numero_enregistrement == ENREG_TYPE_2 )
    {
      if ( space_dimension == 0 ) {
        INFOS_MED( "Missing ENREGISTREMENT DE TYPE   4" << DUMP_LINE_NB );
        return false;
      }
      // FORMAT(' PILE NUMERO',I4,'NBRE OBJETS NOMMES',I8,'NBRE OBJETS',I8)
      getNextLine(ligne);
      const char *s1 = " PILE NUMERO", *s2 = "NBRE OBJETS NOMMES", *s3 = "NBRE OBJETS";
      if ( ! GIBI_EQUAL( ligne, s1 ) ) {
        INFOS_MED( " Could not read file: error in type 2 record. " << ligne << DUMP_LINE_NB );
        return false;
      }
      ligne = ligne + strlen(s1);
      numero_pile = atoi( ligne );
      ligne = ligne + 4 + strlen(s2);
      nb_objets_nommes = atoi( ligne );
      ligne = ligne + 8 + strlen(s3);
      nb_objets = atoi( ligne );
      if ( nb_objets_nommes<0 || nb_objets<0  ) {
        INFOS_MED(" Could not read file: " << nb_objets << " " <<nb_objets_nommes << DUMP_LINE_NB );
        return false;
      }
      if ( !donePiles.insert( numero_pile ).second ) // piles may repeat
        continue;

      if ( numero_pile > PILE_LAST_READABLE )
        break; // stop file reading

      // skip not readable piles
      int i = -1;
      while ( readable_Piles[ ++i ] != PILE_LAST_READABLE )
        if ( readable_Piles[ i ] == numero_pile )
          break;
      if ( readable_Piles[ i ] != numero_pile )
        continue;

      // lecture des objets nommés et de leurs indices
      objets_nommes.resize(nb_objets_nommes);
      indices_objets_nommes.resize(nb_objets_nommes);
      //indices_objets_nommes.resize(nb_objets);
      for ( initNameReading( nb_objets_nommes ); more(); next() ) {
        objets_nommes[ index() ] = getName();
      }
      for ( initIntReading( nb_objets_nommes ); more(); next() )
      //for ( initIntReading( nb_objets ); more(); next() )
        indices_objets_nommes[ index() ] = getInt();

      // boucle interne : lecture de la pile

      MESSAGE_MED(PREFIX_MED << "---- Traitement pile " << numero_pile);

      // -----------------------------------
      //                        MESH GROUPS
      // -----------------------------------

      if (numero_pile == PILE_SOUS_MAILLAGE ) // PILE NUMERO   1
      {
        map<int,int> strangeGroupType;
        medi->groupes.reserve(nb_objets*2); // fields may add some groups
        map< int , int > nbElemsByGeomType;

        for (int objet=0; objet!=nb_objets; ++objet) // pour chaque groupe
        {
          initIntReading( 5 );
          unsigned type_geom_castem = getInt(); next();
          unsigned nb_sous_maillage = getInt(); next();
          unsigned nb_reference     = getInt(); next();
          unsigned nb_noeud         = getInt(); next();
          unsigned nb_elements      = getInt();

          // le cas type_geom_castem=0 correspond aux maillages composites
          if (type_geom_castem<0) {
            INFOS_MED(" Error while reading file, bad geometric type:" <<
                  type_geom_castem << DUMP_LINE_NB );
            return false;
          }

          medi->groupes.push_back(_groupe());
          _groupe & groupe = medi->groupes.back();

          // si le groupe se compose de sous-maillages (ie groupe composite)
          if (type_geom_castem==0 && nb_sous_maillage>0)
          {
            // lecture des indices des sous-maillages, stockage.
            // les mailles correspondant a ces sous_maillages seront inserees a la fin du case
            groupe.groupes.resize( nb_sous_maillage );
            for ( initIntReading( nb_sous_maillage ); more(); next() ) {
              groupe.groupes[ index() ] = getInt();
            }
            if ( readFields )
              std::sort( groupe.groupes.begin(), groupe.groupes.end() );
          }
          // lecture des references (non utilisé pour MED)
          for ( i = 0; i < (int)nb_reference; i += 10 ) {// FORMAT(10I8)
            getNextLine(ligne);
          }
          // lecture des couleurs (non utilisé pour MED)
          for ( i = 0; i < (int)nb_elements; i += 10 ) {
            getNextLine(ligne);
          }
          // not a composit group
          if (type_geom_castem>0 && nb_sous_maillage==0)
          {
            medGeometryElement medType = gibi2medGeom(type_geom_castem);

            initIntReading( nb_elements * nb_noeud );
            if ( medType == MED_NONE ) { // look for group end
              while ( more() )
                next();
              strangeGroupType.insert( make_pair( objet, type_geom_castem ));
              continue;
            }
            if ( nbElemsByGeomType.find( medType ) == nbElemsByGeomType.end())
              nbElemsByGeomType[ medType ] = 0;
            int & order = nbElemsByGeomType[ medType ];

            pair<set<_maille>::iterator,bool> p;
            pair<map<int,_noeud>::iterator,bool> p_no;
            _noeud no;
            no.coord.resize(space_dimension);
            _maille ma( medType, nb_noeud );
            ma.sommets.resize(nb_noeud);
            groupe.mailles.resize( nb_elements );

            // lecture pour chaque maille des sommets et insertions
            for ( i = 0; i < nb_elements; ++i )
            {
              for (unsigned n = 0; n < nb_noeud; ++n, next() )
              {
                if ( !more() ) {
                  INFOS_MED( " Error while reading elem nodes ");
                  return false;
                }
                no.number = getInt();
                p_no=medi->points.insert(make_pair(no.number, no));
                ma.sommets[n]=p_no.first;
              }
              ma.setOrdre( ++order );
              // on stocke dans le groupe un iterateur sur la maille
              groupe.mailles[i] = medi->insert(ma);
            }
          }
        } // loop on groups

        // set group names
        for (i=0; i!=nb_objets_nommes; ++i) {
          int grpID = indices_objets_nommes[i];
          _groupe & grp = medi->groupes[ grpID-1 ];
          if ( !grp.nom.empty() ) // a group has several names
          { // create a group with subgroup grp and named grp.nom
            medi->groupes.push_back(_groupe());
            medi->groupes.back().groupes.push_back( grpID );
            medi->groupes.back().nom = grp.nom;
          }
          grp.nom=objets_nommes[i];
          map<int,int>::iterator it = strangeGroupType.find( grpID - 1 );
          if ( it != strangeGroupType.end() ) {
#ifdef _DEBUG
            cout << "Skip " << grp.nom << " of not supported CASTEM type: " << it->second << endl;
#endif
            //INFOS_MED( "Skip " << grp.nom << " of not supported CASTEM type: " << it->second );
          }
        }

      }// Fin case PILE_SOUS_MAILLAGE

      // ---------------------------------
      //                            NODES
      // ---------------------------------

      else if ( numero_pile == PILE_NOEUDS ) // PILE NUMERO  32
      {
        getNextLine( ligne );
        std::vector<int> place_noeuds;
        nb_indices = atoi ( ligne );
        if (nb_indices != nb_objets)
        {
          INFOS_MED("Erreur de lecture dans enregistrement de pile " <<
                PILE_NOEUDS << DUMP_LINE_NB );
          return false;
        }

        place_noeuds.resize(nb_objets);
        for ( initIntReading( nb_objets ); more(); next() )
          place_noeuds[ index() ] = getInt();
        int max=(* std::max_element(place_noeuds.begin(),place_noeuds.end()));

        // numero_noeuds contient pour chacun des max noeuds qu'on va lire dans le case PILE_COORDONNEES
        // son indice dans la connectivite du maillage. Cet indice correspond egalement a la cle du map
        // medi->points ou l'on stocke les noeuds.
        numero_noeuds.resize(max,-1);
        for (unsigned i=0; i!=place_noeuds.size(); ++i)
          numero_noeuds[place_noeuds[i]-1]=i+1;
      }

      // ---------------------------------------
      //                            COORDINATES
      // ---------------------------------------

      else if ( numero_pile == PILE_COORDONNEES )// PILE NUMERO  33
      {
        getNextLine( ligne );
        unsigned nb_reels = atoi( ligne );
        // PROVISOIRE : certains fichier gibi n`ont
        if (nb_reels < numero_noeuds.size()*(space_dimension)) {
          INFOS_MED("Erreur de lecture dans enregistrement de pile " <<
                PILE_COORDONNEES << DUMP_LINE_NB );
          return false;
        }
        initDoubleReading( nb_reels );
        map< int, _noeud >::iterator pIt;
        for (unsigned i=0; i!=numero_noeuds.size(); ++i)
        {
          // si le noeud est utilisé dans le maillage,
          //on lit ses coordonnées et on les stocke dans la structure
          if (( numero_noeuds[i] != -1 ) &&
              (( pIt = medi->points.find(numero_noeuds[i])) != medi->points.end()))
          {
            for (unsigned j=0; j!=space_dimension; ++j, next())
              pIt->second.coord[j] = getDouble();
            next(); // on ne conserve pas la densite
          }
          else // sinon, on passe au noeud suivant
          {
            for (unsigned j=0; j!=space_dimension+1; ++j)
              next();
          }
        }
      }

      // ---------------------------------------
      //                            NODE FIELDS
      // ---------------------------------------

      else if ( numero_pile == PILE_NODES_FIELD && readFields ) // PILE NUMERO   2
      {
        //vector< _fieldBase* > fields( nb_objets );
        node_fields.resize(nb_objets, (_fieldBase*)0);
        for (int objet=0; objet!=nb_objets; ++objet) // pour chaque field
        {
          bool ignoreField = false;
#ifdef GIBI_READ_ONLY_NAMED_FIELD
          ignoreField = !isNamedObject( objet+1, indices_objets_nommes );
          if ( ignoreField )
            INFOS_MED("Skip non-named field " << objet+1 << DUMP_LINE_NB);
#endif

          // EXAMPLE ( with no values )

          // (1)       4       7       2       1
          // (2)     -88       0       3     -89       0       1     -90       0       2     -91
          // (2)       0       1
          // (3) FX   FY   FZ   FZ   FX   FY   FLX
          // (4)       0       0       0       0       0       0       0
          // (5)           cree  par  muc pri
          // (6)
          // (7)       2

          // (1): nb subcomponents, nb components(total), IFOUR, nb attributes
          initIntReading( 4 );
          int i_sub, nb_sub         = getInt(); next();
          int i_comp, total_nb_comp = getInt(); next();
          next(); // ignore IFOUR
          int nb_attr = getInt();
          if ( nb_sub < 0 || total_nb_comp < 0 || nb_attr < 0 ) {
            INFOS_MED("Error of field reading: wrong nb of components "
                  << nb_sub << " " << total_nb_comp << DUMP_LINE_NB);
            return false;
          }
          // (2) loop on subcomponents of a field, for each read
          // (a) support, (b) number of values and (c) number of components
          vector<int>     support_ids( nb_sub );
          vector<int>     nb_values  ( nb_sub );
          vector<int>     nb_comps   ( nb_sub );
          int total_nb_values = 0;
          initIntReading( nb_sub * 3 );
          for ( i_sub = 0; i_sub < nb_sub; ++i_sub )
          {
            support_ids[ i_sub ] = -getInt(); next(); // (a) reference to support
            if ( support_ids[ i_sub ] < 1 || support_ids[ i_sub ] > medi->groupes.size() ) {
              INFOS_MED("Error of field reading: wrong mesh reference "<<
                    support_ids[ i_sub ] << DUMP_LINE_NB );
              return false;
            }
            nb_values[ i_sub ] = getInt(); next();    // (b) nb points
            total_nb_values += nb_values[ i_sub ];
            if ( nb_values[ i_sub ] < 0 ) {
              INFOS_MED(" Wrong nb of points: " << nb_values[ i_sub ]  << DUMP_LINE_NB );
              return false;
            }
            nb_comps[ i_sub ] = getInt(); next();     // (c) nb of components in i_sub
          }
          // create a field if there are values
          _field<double>* fdouble = 0;
          if ( total_nb_values > 0 && !ignoreField )
          {
            fdouble = new _field<double>( MED_REEL64, nb_sub, total_nb_comp );
            medi->fields.push_back( node_fields[ objet ] = fdouble );
          }
          // (3) component names
          initNameReading( total_nb_comp, 4 );
          for ( i_sub = 0; i_sub < nb_sub; ++i_sub )
          {
            // store support id and nb components of a sub
            if ( fdouble )
              fdouble->_sub[ i_sub ].setData( nb_comps[ i_sub ], support_ids[ i_sub ] );
            for ( i_comp = 0; i_comp < nb_comps[ i_sub ]; ++i_comp, next() )
            {
              ASSERT_MED( more() );
              // store component name
              if ( fdouble )
                fdouble->_sub[ i_sub ].compName( i_comp ) = getName();
            }
          }
          // (4) nb harmonics ( ignored )
          for ( initIntReading( total_nb_comp ); more(); next() )
            ;
          // (5) TYPE ( ignored )
          getNextLine( ligne );
          // (6) TITRE ( ignored )
          getNextLine( ligne );
          // (7) attributes ( ignored )
          for ( initIntReading( nb_attr ); more(); next() )
            ;

          for ( i_sub = 0; i_sub < nb_sub; ++i_sub )
          {
            // loop on components: read values
            initDoubleReading( nb_values[ i_sub ] * nb_comps[ i_sub ] );
            for ( i_comp = 0; i_comp < nb_comps[ i_sub ]; ++i_comp )
            {
              vector<double>* vals = 0;
              if ( fdouble ) vals = & fdouble->addComponent( nb_values[ i_sub ] );
              for ( int i = 0; more() && i < nb_values[ i_sub ]; next(), ++i ) {
                if ( vals ) (*vals)[ i ] = getDouble();
              }
            }
          } // loop on subcomponents of a field

          // set id of a group including all subs supports but only
          // if all subs have the same components
          if ( fdouble && fdouble->hasSameComponentsBySupport() )
            fdouble->_group_id = getGroupId( support_ids, medi );

        } // end loop on field objects

        // set field names
        setFieldNames( node_fields, objets_nommes, indices_objets_nommes );

      }  // Fin numero_pile == PILE_NODES_FIELD

      // -------------------------------------------------
      //                                           FIELDS
      // -------------------------------------------------

      else if ( numero_pile == PILE_FIELD && readFields ) // PILE NUMERO  39
      {
        // REAL EXAMPLE

        // (1)        1       2       6      16
        // (2)                                                         CARACTERISTIQUES
        // (3)      -15  317773       4       0       0       0      -2       0       3
        // (4)             317581
        // (5)  0
        // (6)   317767  317761  317755  317815
        // (7)  YOUN     NU       H        SIGY
        // (8)  REAL*8            REAL*8            REAL*8            REAL*8
        // (9)        1       1       0       0
        // (10)  2.00000000000000E+05
        // (11)       1       1       0       0
        // (12)  3.30000000000000E-01
        // (13)       1       1       0       0
        // (14)  1.00000000000000E+04
        // (15)       6     706       0       0
        // (16)  1.00000000000000E+02  1.00000000000000E+02  1.00000000000000E+02
        // (17)  1.00000000000000E+02  1.00000000000000E+02  1.00000000000000E+02
        // (18)  ...

        //vector< _fieldBase* > fields( nb_objets, (_fieldBase*)0 );
        cell_fields.resize(nb_objets, (_fieldBase*)0);
        for (int objet=0; objet!=nb_objets; ++objet) // pour chaque field
        {
          bool ignoreField = false;
#ifdef GIBI_READ_ONLY_NAMED_FIELD
          ignoreField = !isNamedObject( objet+1, indices_objets_nommes );
          if ( ignoreField )
            INFOS_MED("Skip non-named field " << objet+1 << DUMP_LINE_NB);
#endif
          initIntReading( 4 );
          int i_sub, nb_sub = getInt(); // (1) <nb_sub> 2 6 <title length>
          if ( nb_sub < 1 ) {
            INFOS_MED("Error of field reading: wrong nb of subcomponents " <<
                  nb_sub << DUMP_LINE_NB );
            return false;
          }
          next(); next(); next(); // skip (1) <nb_sub> 2 6
          int title_length = getInt(); // <title length>
          string description;
          if ( title_length ) {
            getNextLine( ligne ); // (2) title
            const int len = 72; // line length
            // title is right justified
            description = string(ligne + len - title_length, title_length);
          }
          // look for a line starting with '-' : <reference to support>
          do {
            initIntReading( nb_sub * 9 );
          } while ( getInt() >= 0 );

          int total_nb_comp = 0;
          vector<int> support_ids( nb_sub ), nb_comp( nb_sub );
          for ( i_sub = 0; i_sub < nb_sub; ++i_sub )
          {                                           // (3)
            support_ids[ i_sub ] = -getInt(); next(); // <reference to support>
            next();                                   // ignore <address>
            nb_comp    [ i_sub ] =  getInt(); next(); // <nb of components in the sub>
            for ( i = 0; i < 6; ++i )                 // ignore 6 ints, in example 0 0 0 -2 0 3
              next();
            if ( support_ids[ i_sub ] < 1 || support_ids[ i_sub ] > medi->groupes.size() ) {
              INFOS_MED("Error of field reading: wrong mesh reference "<<
                    support_ids[ i_sub ] << DUMP_LINE_NB );
              return false;
            }
            if ( nb_comp[ i_sub ] < 0 /*1*/ ) { // [SALOME platform 0019886]
              INFOS_MED("Error of field reading: wrong nb of components " <<
                    nb_comp[ i_sub ] << DUMP_LINE_NB );
              return false;
            }
            total_nb_comp += nb_comp[ i_sub ];
          }
          for ( initNameReading( nb_sub, 17 ); more(); next() )
            ; // (4) dummy strings
          for ( initNameReading( nb_sub ); more(); next() )
            ; // (5) dummy strings

          // loop on subcomponents of a field, each of which refers to
          // a certain support and has its own number of components;
          // read component values
          _field<double>* fdouble = 0;
          _field<int>*    fint    = 0;
          _fieldBase *    fbase   = 0;
          for ( i_sub = 0; i_sub < nb_sub; ++ i_sub )
          {
            vector<string> comp_names( nb_comp[ i_sub ]), comp_type( nb_comp[ i_sub ]);
            for ( initIntReading( nb_comp[ i_sub ] ); more(); next() )
              ;  // (6) nb_comp addresses of MELVAL structure

            // (7) component names
            for ( initNameReading( nb_comp[ i_sub ] ); more(); next() )
              comp_names[ index() ] = getName();

            // (8) component type
            for ( initNameReading( nb_comp[ i_sub ], 17 ); more(); next() ) { // 17 is name width
              comp_type[ index() ] = getName();
              // component types must be the same
              if ( index() > 0 && comp_type[ index() ] != comp_type[ index() - 1] ) {
                INFOS_MED( "Error of field reading: diff component types <"
                      << comp_type[ index() ] << "> != <" << comp_type[ index() - 1 ]
                       << ">" << DUMP_LINE_NB );
                return false;
              }
            }
            // now type is known, create a field, one for all subs
            bool isReal = (nb_comp[i_sub] > 0) ? (comp_type[0] == "REAL*8") : true;
            if ( !ignoreField && !fbase && total_nb_comp ) {
              if ( !isReal ) {
                fbase = fint = new _field<int>( MED_INT32, nb_sub, total_nb_comp );
                INFOS_MED( "Warning: read NOT REAL field, type <" << comp_type[0] << ">"
                      << DUMP_LINE_NB);
              }
              else
                fbase = fdouble = new _field<double>( MED_REEL64, nb_sub, total_nb_comp );
              medi->fields.push_back( cell_fields[ objet ] = fbase ); // medi->fields is a std::list
            }
            // store support id and nb components of a sub
            if ( fbase ) {
              fbase->_sub[ i_sub ].setData( nb_comp[ i_sub ], support_ids[ i_sub ]);
              fbase->_description = description;
            }
            // loop on components: read values
            for ( int i_comp = 0; i_comp < nb_comp[ i_sub ]; ++i_comp )
            {
              // (9) nb of values
              initIntReading( 2 );
              int nb_val_by_elem = getInt(); next();
              int nb_values      = getInt();
              fbase->_sub[ i_sub ]._nb_gauss[ i_comp ] = nb_val_by_elem;
              // (10) values
              nb_values *= nb_val_by_elem;
              if ( fbase ) {
                if ( isReal ) {
                  vector<double> & vals = fdouble->addComponent( nb_values );
                  for ( initDoubleReading( nb_values ); more(); next()) {
                    vals[ index() ] = getDouble();
                  }
                }
                else {
                  vector<int> & vals = fint->addComponent( nb_values );
                  for ( initIntReading( nb_values ); more(); next() ) {
                    vals[ index() ] = getInt();
                  }
                }
                // store component name
                fbase->_sub[ i_sub ].compName( i_comp ) = comp_names[ i_comp ];
              }
              else {
                for ( isReal ? initDoubleReading( nb_values ) : initIntReading( nb_values );
                      more();
                      next() ) ;
              }
            }
          } // loop on subcomponents of a field

          // set id of a group including all sub supports but only
          // if all subs have the same nb of components
          if ( fbase && fbase->hasSameComponentsBySupport() )
            fbase->_group_id = getGroupId( support_ids, medi );

        } // end loop on field objects

        // set field names
        setFieldNames( cell_fields, objets_nommes, indices_objets_nommes );

      } // numero_pile == PILE_FIELD && readFields

      // ---------------------------------------
      //                            MED NAMES
      // ---------------------------------------

      // IMP 0020434: mapping GIBI names to MED names
      else if (numero_pile == PILE_TABLES)
      {
        const char * table_med_mail = "MED_MAIL";
        const char * table_med_cham = "MED_CHAM";
        const char * table_med_comp = "MED_COMP";

        int table_med_mail_id = -1;
        int table_med_cham_id = -1;
        int table_med_comp_id = -1;

        for (int iname = 0; iname < nb_objets_nommes; iname++) {
          if (strcmp(objets_nommes[iname].c_str(), table_med_mail) == 0) {
            table_med_mail_id = indices_objets_nommes[iname];
          }
          else if (strcmp(objets_nommes[iname].c_str(), table_med_cham) == 0) {
            table_med_cham_id = indices_objets_nommes[iname];
          }
          else if (strcmp(objets_nommes[iname].c_str(), table_med_comp) == 0) {
            table_med_comp_id = indices_objets_nommes[iname];
          }
          else {
          }
        }
        //if (noms_med_id < 0) continue;
        if (table_med_mail_id < 0 && table_med_cham_id < 0 && table_med_comp_id < 0)
          continue;

        for (int itable = 1; itable <= nb_objets; itable++) {
          // read tables "MED_MAIL", "MED_CHAM" and "MED_COMP", that keeps correspondence
          // between GIBI names (8 symbols) and MED names (possibly longer)
          getNextLine( ligne );
          int nb_table_vals = atoi( ligne );
          if (nb_table_vals < 0) {
            INFOS_MED("Erreur de lecture dans enregistrement de pile " <<
                      PILE_TABLES << DUMP_LINE_NB );
            return false;
          }

          //int name_i_val_pile;
          int name_i_med_pile;
          initIntReading(nb_table_vals);
          for (int i = 0; i < nb_table_vals/4; i++)
          {
            if (itable == table_med_mail_id ||
                itable == table_med_cham_id ||
                itable == table_med_comp_id)
            {
              nameGIBItoMED name_i;
              //name_i.key_pile = getInt(); next();
              //name_i.key_id   = getInt(); next();
              //name_i_val_pile = getInt(); next();
              //name_i.val_id   = getInt(); next();
              name_i_med_pile  = getInt(); next();
              name_i.med_id    = getInt(); next();
              name_i.gibi_pile = getInt(); next();
              name_i.gibi_id   = getInt(); next();

              if (name_i_med_pile != PILE_STRINGS) {
                // error: med(long) names are always kept in PILE_STRINGS
              }

              if (itable == table_med_mail_id) {
                if (name_i.gibi_pile != PILE_SOUS_MAILLAGE) {
                  // error: must be PILE_SOUS_MAILLAGE
                }
                listGIBItoMED_mail.push_back(name_i);
              }
              else if (itable == table_med_cham_id) {
                if (name_i.gibi_pile != PILE_FIELD &&
                    name_i.gibi_pile != PILE_NODES_FIELD) {
                  // error: must be PILE_FIELD or PILE_NODES_FIELD
                }
                listGIBItoMED_cham.push_back(name_i);
              }
              else if (itable == table_med_comp_id) {
                if (name_i.gibi_pile != PILE_STRINGS) {
                  // error: gibi(short) names of components are kept in PILE_STRINGS
                }
                listGIBItoMED_comp.push_back(name_i);
              }
              else {
              }
            }
            else
            {
              // pass table
              next(); next(); next(); next();
            }
          }
        } // for (int itable = 0; itable < nb_objets; itable++)
      } // numero_pile == PILE_TABLES

      // ---------------------------------------
      //                            MED NAMES
      // ---------------------------------------

      // IMP 0020434: mapping GIBI names to MED names
      else if (numero_pile == PILE_STRINGS)
      {
        initIntReading(2);
        int stringLen = getInt(); next();
        int nbSubStrings = getInt(); next();
        if (nbSubStrings != nb_objets) {
          // error
        }
        string aWholeString;
        string aCurrLine;
        const int fixedLength = 71;
        while (aWholeString.length() < stringLen) {
          getNextLine( ligne );
          int remainLen = stringLen - aWholeString.length();
          if ( remainLen > fixedLength )
          {
            aWholeString += ligne + 1;
          }
          else
          {
            aWholeString += ligne + ( 72 - remainLen );
          }
        }
        int prevOffset = 0;
        int currOffset = 0;
        initIntReading(nbSubStrings);
        for (int istr = 1; istr <= nbSubStrings; istr++, next()) {
          currOffset = getInt();
          // fill mapStrings
          mapStrings[istr] = aWholeString.substr(prevOffset, currOffset - prevOffset);
          prevOffset = currOffset;
        }
      }

      else if ( numero_pile >= PILE_LAST_READABLE )
        break; // stop file reading

    } // Fin case ENREG_TYPE_2
  } //  fin de la boucle while de lecture externe

  // IMP 0020434: mapping GIBI names to MED names
  // set med names to objects (mesh, fields, support, group or other)
  if (listGIBItoMED_mail.size() > 0) {
    list<nameGIBItoMED>::iterator itGIBItoMED = listGIBItoMED_mail.begin();
    for (; itGIBItoMED != listGIBItoMED_mail.end(); itGIBItoMED++) {
      _groupe & grp = medi->groupes[itGIBItoMED->gibi_id - 1];
      grp.nom = mapStrings[itGIBItoMED->med_id];
    } // iterate on listGIBItoMED_mail
  }
  if (listGIBItoMED_cham.size() > 0) {
    list<nameGIBItoMED>::iterator itGIBItoMED = listGIBItoMED_cham.begin();
    for (; itGIBItoMED != listGIBItoMED_cham.end(); itGIBItoMED++) {
      if (itGIBItoMED->gibi_pile == PILE_FIELD) {
        cell_fields[itGIBItoMED->gibi_id - 1]->_name = mapStrings[itGIBItoMED->med_id];
      }
      else if (itGIBItoMED->gibi_pile == PILE_NODES_FIELD) {
        node_fields[itGIBItoMED->gibi_id - 1]->_name = mapStrings[itGIBItoMED->med_id];
      }
      else {
      }
    } // iterate on listGIBItoMED_cham
  }
  if (listGIBItoMED_comp.size() > 0) {
    list<nameGIBItoMED>::iterator itGIBItoMED = listGIBItoMED_comp.begin();
    for (; itGIBItoMED != listGIBItoMED_comp.end(); itGIBItoMED++)
    {
      string medName  = mapStrings[itGIBItoMED->med_id];
      string gibiName = mapStrings[itGIBItoMED->gibi_id];

      int posDot = medName.find(".");
      string medNameField = medName.substr(0, posDot);
      string medNameCompo = medName.substr(posDot + 1);

      int nbFields = cell_fields.size();
      for (int ifi = 0; ifi < nbFields; ifi++) {
        if (cell_fields[ifi]->_name == medNameField) {
          std::vector<_fieldBase::_sub_data>& aSubDs = cell_fields[ifi]->_sub;
          int nbSub = aSubDs.size();
          for (int isu = 0; isu < nbSub; isu++) {
            for (int ico = 0; ico < aSubDs[isu].nbComponents(); ico++) {
              if (aSubDs[isu].compName(ico) == gibiName) {
                cell_fields[ifi]->_sub[isu].compName(ico) = medNameCompo;
              }
            }
          }
        }
      }
    } // iterate on listGIBItoMED_comp
  }

  // check if all needed piles present
  if ( donePiles.find( PILE_SOUS_MAILLAGE ) != donePiles.end() )
  {
    if (donePiles.find( PILE_NOEUDS ) == donePiles.end() ) {
      INFOS_MED( " Missing pile " << PILE_NOEUDS );
      return false;
    }
    if (donePiles.find( PILE_COORDONNEES ) == donePiles.end()) {
      INFOS_MED( " Missing pile " << PILE_COORDONNEES );
      return false;
    }
  }

  END_OF_MED(LOC);
  return true;
}

GIBI_MESH_DRIVER::GIBI_MESH_DRIVER():
       GENDRIVER(GIBI_DRIVER),
       _ptrMesh(( MESH *) NULL),
       // A VOIR _medIdt(MED_INVALID),
       _meshName("")
{
  MESSAGE_MED("GIBI_MESH_DRIVER()");
}

GIBI_MESH_DRIVER::GIBI_MESH_DRIVER(const string & fileName,
                                   MESH * ptrMesh,
                                   MED_EN::med_mode_acces accessMode):
  GENDRIVER(fileName, accessMode, GIBI_DRIVER),
  _ptrMesh(ptrMesh)
  // A VOIR _medIdt(MED_INVALID),
{
  MESSAGE_MED( "GIBI_MESH_DRIVER(" << fileName <<","<<accessMode );
//   _meshName=fileName.substr(0,fileName.rfind("."));
    // mesh name construction from fileName
    const string ext=".sauve"; // expected extension
    string::size_type pos=fileName.find(ext,0);
    string::size_type pos1=fileName.rfind('/');
    _meshName = string(fileName,pos1+1,pos-pos1-1); //get rid of directory & extension
    SCRUTE_MED(_meshName);
}

GIBI_MESH_DRIVER::GIBI_MESH_DRIVER(const GIBI_MESH_DRIVER & driver):
  GENDRIVER(driver),
  _ptrMesh(driver._ptrMesh),
  // A VOIR _medIdt(MED_INVALID),
  _meshName(driver._meshName)
{
  MESSAGE_MED("GIBI_MESH_DRIVER(const GIBI_MESH_DRIVER & driver)");
}

GIBI_MESH_DRIVER::~GIBI_MESH_DRIVER()
{
  MESSAGE_MED("~GIBI_MESH_DRIVER()");
}
void    GIBI_MESH_DRIVER::setMeshName(const string & meshName) { _meshName = meshName; };
string  GIBI_MESH_DRIVER::getMeshName() const { return _meshName; };


//---------------------------------- RDONLY PART -------------------------------------------------------------

GIBI_MESH_RDONLY_DRIVER::GIBI_MESH_RDONLY_DRIVER():
       GIBI_MESH_DRIVER(),
       _File (-1),_start(0L),_ptr  (0L),_eptr (0L)
{
}
GIBI_MESH_RDONLY_DRIVER::GIBI_MESH_RDONLY_DRIVER(const string & fileName,MESH * ptrMesh):
       GIBI_MESH_DRIVER(fileName,ptrMesh,RDONLY),
       _File (-1),_start(0L),_ptr  (0L),_eptr (0L)
{
    MESSAGE_MED("GIBI_MESH_RDONLY_DRIVER::GIBI_MESH_RDONLY_DRIVER"
            "(const string & fileName, MESH * ptrMesh) has been created, "
            << fileName << ", " << RDONLY);
}
GIBI_MESH_RDONLY_DRIVER::GIBI_MESH_RDONLY_DRIVER(const GIBI_MESH_RDONLY_DRIVER & driver):
GIBI_MESH_DRIVER(driver)
{
}
GIBI_MESH_RDONLY_DRIVER::~GIBI_MESH_RDONLY_DRIVER()
{
  const char* LOC = "~GIBI_MESH_RDONLY_DRIVER()";
  BEGIN_OF_MED(LOC);
  if (_File >= 0)
  {
    ::close (_File);
    if (_start != 0L)
      delete [] _start;
  }
  MESSAGE_MED("GIBI_MESH_RDONLY_DRIVER::~GIBI_MESH_RDONLY_DRIVER() has been destroyed");
}
GENDRIVER * GIBI_MESH_RDONLY_DRIVER::copy(void) const
{
  return new GIBI_MESH_RDONLY_DRIVER(*this);
}

//=======================================================================
//function : open
//purpose  :
//=======================================================================

const int GIBI_MaxOutputLen   = 150;
const int GIBI_BufferSize     = 16184; // for non-stream input

void GIBI_MESH_RDONLY_DRIVER::open()
  //     throw (MEDEXCEPTION)
{
  if( _status == MED_OPENED )
    return;

  const char * LOC = "GIBI_MESH_RDONLY_DRIVER::open()";
  BEGIN_OF_MED(LOC);

//   MED_EN::med_mode_acces aMode = getAccessMode();
//   if ( aMode != MED_EN::MED_LECT && aMode != MED_EN::MED_REMP )
//     throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << " Bad file mode access ! " << aMode));

#ifdef WNT
  _File = ::_open (_fileName.c_str(), _O_RDONLY|_O_BINARY);
#else
  _File = ::open (_fileName.c_str(), O_RDONLY);
#endif
  if (_File >= 0)
  {
    _start = new char [GIBI_BufferSize];
    _ptr   = _start;
    _eptr  = _start;
    _status = MED_OPENED;
    _lineNb = 0;
  }
  else
  {
    _status = MED_CLOSED;
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Could not open file "<<_fileName
                                 << " fd: " << _File));
  }
  END_OF_MED(LOC);
}

//=======================================================================
//function : close
//purpose  :
//=======================================================================

void GIBI_MESH_RDONLY_DRIVER::close()
{
  const char* LOC = "GIBI_MESH_DRIVER::close() ";
  BEGIN_OF_MED(LOC);
  if ( _status == MED_OPENED) {
    if (_File >= 0) {
      ::close (_File);
      if (_start != 0L)
        delete [] _start;
      _File = -1;
    }
    _status = MED_CLOSED;
  }
  END_OF_MED(LOC);
}

//=======================================================================
//function : getLine
//purpose  :
//=======================================================================

bool GIBI_MESH_RDONLY_DRIVER::getLine(char* & aLine)
{
  bool aResult = true;
  // Check the state of the buffer;
  // if there is too little left, read the next portion of data
  int nBytesRest = _eptr - _ptr;
  if (nBytesRest < GIBI_MaxOutputLen)
  {
    if (nBytesRest > 0) {
      //memcpy (_start, _ptr, nBytesRest);
      char* tmpBuf = new char [nBytesRest];
      memcpy (tmpBuf, _ptr, nBytesRest);
      memcpy (_start, tmpBuf, nBytesRest);
      delete [] tmpBuf;
    } else
      nBytesRest = 0;
    _ptr = _start;
    const int nBytesRead = ::read (_File,
                                   &_start [nBytesRest],
                                   GIBI_BufferSize - nBytesRest);
    nBytesRest += nBytesRead;
    _eptr = &_start [nBytesRest];
  }
  // Check the buffer for the end-of-line
  char * ptr = _ptr;
  while (~0)
  {
    // Check for end-of-the-buffer, the ultimate criterion for termination
    if (ptr >= _eptr)
    {
      if (nBytesRest <= 0)
        aResult = false;
      else
        _eptr[-1] = '\0';
      break;
    }
    // seek the line-feed character
    if (ptr[0] == '\n')
    {
      if (ptr[-1] == '\r')
        ptr[-1] = '\0';
      ptr[0] = '\0';
      ++ptr;
      break;
    }
    ++ptr;
  }
  // Output the result
  aLine = _ptr;
  _ptr = ptr;
  _lineNb++;

  return aResult;
}

//=======================================================================
//function : init
//purpose  :
//=======================================================================

void GIBI_MESH_RDONLY_DRIVER::init( int nbToRead, int nbPosInLine, int width, int shift )
{
  _nbToRead = nbToRead;
  _nbPosInLine = nbPosInLine;
  _width = width;
  _shift = shift;
  _iPos = _iRead = 0;
  if ( _nbToRead ) {
    getNextLine( _curPos );
    _curPos = _curPos + _shift;
  }
  else
    _curPos = 0;
}

//=======================================================================
//function : next
//purpose  : line getting
//=======================================================================

void GIBI_MESH_RDONLY_DRIVER::next()
{
  if ( !more() ) throw MEDEXCEPTION(LOCALIZED("!more()"));
  ++_iRead;
  ++_iPos;
  if ( _iRead < _nbToRead ) {
    if ( _iPos >= _nbPosInLine ) {
      getNextLine( _curPos );
      _curPos = _curPos + _shift;
      _iPos = 0;
    }
    else
      _curPos = _curPos + _width + _shift;
  }
  else
    _curPos = 0;
}

//=======================================================================
//function : getName
//purpose  : names reading
//=======================================================================

string GIBI_MESH_RDONLY_DRIVER::getName() const
{
  int len = _width;
  while (( _curPos[len-1] == ' ' || _curPos[len-1] == 0) && len > 0 )
    len--;
  return string( _curPos, len );
}

//=======================================================================
//function : getDouble
//purpose  : double reading
//=======================================================================

double GIBI_MESH_RDONLY_DRIVER::getDouble() const
{
  //return atof(str());

  std::string aStr (str());

  // Correction 1: add missing 'E' specifier
  int aPosStart = aStr.find_first_not_of(" \t");
  if (aPosStart < aStr.length()) {
    int aPosSign = aStr.find_first_of("+-", aPosStart + 1); // pass the leading symbol, as it can be a sign
    if (aPosSign < aStr.length()) {
      if (aStr[aPosSign - 1] != 'e' && aStr[aPosSign - 1] != 'E')
        aStr.insert(aPosSign, "E", 1);
    }
  }

  // Correction 2: set "C" numeric locale to read numbers
  // with dot decimal point separator, as it is in SAUVE files
  Kernel_Utils::Localizer loc;
  double ret = atof(aStr.data());

  return ret;
}

//=======================================================================
//function : read
//purpose  :
//=======================================================================

void GIBI_MESH_RDONLY_DRIVER::read(void) throw (MEDEXCEPTION)
{
  const char * LOC = "_GIBI_RDONLY_DRIVER::read() : ";
  BEGIN_OF_MED(LOC);

  if (_status!=MED_OPENED)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "The _idt of file " << _fileName << " is : "
                                 <<  " (the file is not opened)." ));
  if ( ! _ptrMesh->isEmpty() )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Mesh object not empty : can't fill it!"));

  _intermediateMED medi;
  try {
    if ( readFile( &medi, false )) {
      // impression résultats
      MESSAGE_MED(LOC << "GIBI_MESH_RDONLY_DRIVER::read : RESULTATS STRUCTURE INTERMEDIAIRES : ");
      MESSAGE_MED(LOC <<  medi );

      fillMesh( &medi );
      updateSupports(); // create families from groups etc.
    }
  }
  catch (MEDEXCEPTION &ex)
  {
    INFOS_MED( ex.what() );
  }
  END_OF_MED(LOC);
}

//=======================================================================
//function : getReverseVector
//purpose  :
//=======================================================================

static void getReverseVector (const medGeometryElement type,
                              vector<pair<int,int> > & swapVec )
{
  const char* LOC = "void getReverseVector()";
  BEGIN_OF_MED(LOC);
  swapVec.clear();

  switch ( type ) {
  case MED_TETRA4:
    swapVec.resize(1);
    swapVec[0] = make_pair( 1, 2 );
    break;
  case MED_PYRA5:
    swapVec.resize(1);
    swapVec[0] = make_pair( 1, 3 );
    break;
  case MED_PENTA6:
    swapVec.resize(2);
    swapVec[0] = make_pair( 1, 2 );
    swapVec[1] = make_pair( 4, 5 );
    break;
  case MED_HEXA8:
    swapVec.resize(2);
    swapVec[0] = make_pair( 1, 3 );
    swapVec[1] = make_pair( 5, 7 );
    break;
  case MED_TETRA10:
    swapVec.resize(3);
    swapVec[0] = make_pair( 1, 2 );
    swapVec[1] = make_pair( 4, 5 );
    swapVec[2] = make_pair( 8, 9 );
    break;
  case MED_PYRA13:
    swapVec.resize(4);
    swapVec[0] = make_pair( 1, 3 );
    swapVec[1] = make_pair( 5, 8 );
    swapVec[2] = make_pair( 6, 7 );
    swapVec[3] = make_pair( 10, 12 );
    break;
  case MED_PENTA15:
    swapVec.resize(4);
    swapVec[0] = make_pair( 1, 2 );
    swapVec[1] = make_pair( 4, 5 );
    swapVec[2] = make_pair( 6, 8 );
    swapVec[3] = make_pair( 9, 11 );
    break;
  case MED_HEXA20:
    swapVec.resize(7);
    swapVec[0] = make_pair( 1, 3 );
    swapVec[1] = make_pair( 5, 7 );
    swapVec[2] = make_pair( 8, 11 );
    swapVec[3] = make_pair( 9, 10 );
    swapVec[4] = make_pair( 12, 15 );
    swapVec[5] = make_pair( 13, 14 );
    swapVec[6] = make_pair( 17, 19 );
    break;
//   case MED_SEG3: no need to reverse edges
//     swapVec.resize(1);
//     swapVec[0] = make_pair( 1, 2 );
//     break;
  case MED_TRIA6:
    swapVec.resize(2);
    swapVec[0] = make_pair( 1, 2 );
    swapVec[1] = make_pair( 3, 5 );
    break;
  case MED_QUAD8:
    swapVec.resize(3);
    swapVec[0] = make_pair( 1, 3 );
    swapVec[1] = make_pair( 4, 7 );
    swapVec[2] = make_pair( 5, 6 );
    break;
  default:;
  }
  END_OF_MED(LOC);
}

//=======================================================================
//function : reverse
//purpose  : inverse element orientation using vector of indices to swap
//=======================================================================

static void reverse(const _maille & aMaille, const vector<pair<int,int> > & swapVec )
{
  _maille* ma = (_maille*) & aMaille;
  for ( int i = 0; i < swapVec.size(); ++i ) {
    std::swap( ma->sommets[ swapVec[i].first ],
               ma->sommets[ swapVec[i].second ]);
  }
  if ( swapVec.empty() )
    ma->reverse = true;
  else
    ma->reverse = false;
}

//=======================================================================
//function : getGibi2MedConnectivity
//purpose  : return array of indices to transform GIBI connectivity to MED one
//=======================================================================

static const int * getGibi2MedConnectivity( const medGeometryElement type )
{
  static vector<const int*> conn;
  static int hexa20 [] = {0,6,4,2, 12,18,16,14, 7,5,3,1, 19,17,15,13, 8,11,10,9};
  static int penta15[] = {0,2,4, 9,11,13, 1,3,5, 10,12,14, 6,7,3};
  static int pyra13 [] = {0,2,4,6, 12, 1,3,5,7, 8,9,10,11};
  static int tetra10[] = {0,2,4, 9, 1,3,5, 6,7,8};
  static int quad8  [] = {0,2,4,6, 1,3,5,7};
  static int tria6  [] = {0,2,4, 1,3,5};
  static int seg3   [] = {0,2,1};
  if ( conn.empty() ) {
    conn.resize( MED_HEXA20 + 1, 0 );
    conn[ MED_HEXA20 ] = hexa20;
    conn[ MED_PENTA15] = penta15;
    conn[ MED_PYRA13 ] = pyra13;
    conn[ MED_TETRA10] = tetra10;
    conn[ MED_SEG3   ] = seg3;
    conn[ MED_TRIA6  ] = tria6;
    conn[ MED_QUAD8  ] = quad8;
  }
  return conn[ type ];
}

//=======================================================================
//function : fixConnectivity
//purpose  : GIBI connectivity -> MED one
//=======================================================================

static inline void fixConnectivity(const _maille & aMaille )
{
  if ( const int * conn = getGibi2MedConnectivity( aMaille.geometricType )) {
    _maille* ma = (_maille*) & aMaille;
    //cout << "###### BEFORE fixConnectivity() " << *ma << endl;
    vector< _maille::TNoeud > newSommets( ma->sommets.size() );
    for ( int i = 0; i < newSommets.size(); ++i )
      newSommets[ i ] = ma->sommets[ conn[ i ]];
    ma->sommets = newSommets;
    //cout << "###### AFTER fixConnectivity() " << *ma << endl;
  }
}

//=======================================================================
//function : orientElements
//purpose  :
//=======================================================================

static void orientElements( _intermediateMED& medi )
{
  MESSAGE_MED("orientElements()");

  set<_maille>::const_iterator elemIt, elemEnd;

  int type = -100;
  vector< pair<int,int> > swapVec;

  bool isQuadratic = false;
  _maillageByDimIterator maIt(medi);
  while ( !isQuadratic && maIt.nextType() )
    isQuadratic = getGibi2MedConnectivity( maIt.type() );

  if ( medi.points.begin()->second.coord.size() == 2 ) { // space dimension

    // fix connectivity of quadratic edges
    if ( isQuadratic ) {
      if ( const set<_maille > * edges = _maillageByDimIterator( medi, 1 ).nextType() ) {
        elemIt = edges->begin(), elemEnd = edges->end();
        for ( ; elemIt != elemEnd; ++elemIt )
          fixConnectivity( *elemIt );
      }
    }
    // --------------------------
    // Orient 2D faces clockwise
    // --------------------------

    _maillageByDimIterator faceMailIt( medi, 2 );
    while ( const set<_maille > * faces = faceMailIt.nextType() )
    {
      elemIt = faces->begin(), elemEnd = faces->end();
      for ( ; elemIt != elemEnd; elemIt++ )
      {
        // fix connectivity of quadratic faces
        if ( isQuadratic )
          fixConnectivity( *elemIt );

        // look for index of the most left node
        int iLeft = 0, iNode, nbNodes = elemIt->sommets.size();
        if ( nbNodes > 4 ) // quadratic face
          nbNodes /= 2;
        double minX = elemIt->sommets[0]->second.coord[0];
        for ( iNode = 1; iNode < nbNodes; ++iNode )
        {
          if ( minX > elemIt->sommets[ iNode ]->second.coord[ 0 ]) {
            minX = elemIt->sommets[ iNode ]->second.coord[ 0 ];
            iLeft = iNode;
          }
        }
        // indeces of the nodes neighboring the most left one
        int iPrev = ( iLeft - 1 < 0 ) ? nbNodes - 1 : iLeft - 1;
        int iNext = ( iLeft + 1 == nbNodes ) ? 0 : iLeft + 1;
        // find components of prev-left and left-next vectors
        double xP = elemIt->sommets[ iPrev ]->second.coord[ 0 ];
        double yP = elemIt->sommets[ iPrev ]->second.coord[ 1 ];
        double xN = elemIt->sommets[ iNext ]->second.coord[ 0 ];
        double yN = elemIt->sommets[ iNext ]->second.coord[ 1 ];
        double xL = elemIt->sommets[ iLeft ]->second.coord[ 0 ];
        double yL = elemIt->sommets[ iLeft ]->second.coord[ 1 ];
        double xPL = xL - xP, yPL = yL - yP; // components of prev-left vector
        double xLN = xN - xL, yLN = yN - yL; // components of left-next vector
        // normalise y of the vectors
        double modPL = sqrt ( xPL * xPL + yPL * yPL );
        double modLN = sqrt ( xLN * xLN + yLN * yLN );
        if ( modLN > DBL_MIN && modPL > DBL_MIN )
        {
          yPL /= modPL;
          yLN /= modLN;
          // summury direction of neighboring links must be positive
          bool clockwise = ( yPL + yLN > 0 );
          if ( !clockwise ) {
            if ( elemIt->geometricType != type ) {
              type = elemIt->geometricType;
              getReverseVector( type, swapVec );
            }
            reverse( *elemIt, swapVec );
          }
        }
      }
    }
  }
  else {

    // --------------------------------------
    // orient equally all connected 3D faces
    // --------------------------------------
    // connectivity of quadratic faces will be fixed by fixConnectivity()
    // in the next loop on elements

    // fill map of links and their faces
    set<const _maille*> faces;
    map<const _maille*, _groupe*> fgm;
    map<_link, list<const _maille*> > linkFacesMap;
    map<_link, list<const _maille*> >::iterator lfIt, lfIt2;

    medi.treatGroupes(); // erase groupes that wont be converted
    for (unsigned int i=0; i!=medi.groupes.size(); ++i)
    {
      _groupe& grp = medi.groupes[i];
      _groupe::TMailleIter maIt=grp.mailles.begin();
      if ( maIt==grp.mailles.end() || (*maIt)->dimensionWithPoly() != 2 )
        continue;
      for(; maIt!=grp.mailles.end(); ++maIt) {
        if ( faces.insert( &(**maIt )).second ) {
          for ( int j = 0; j < (int)(*maIt)->sommets.size(); ++j )
            linkFacesMap[ (*maIt)->link( j ) ].push_back( &(**maIt) );
          fgm.insert( make_pair( &(**maIt), &grp ));
        }
      }
    }
    // dump linkFacesMap
//     for ( lfIt = linkFacesMap.begin(); lfIt!=linkFacesMap.end(); lfIt++) {
//       cout<< "LINK: " << lfIt->first.first << "-" << lfIt->first.second << endl;
//       list<const _maille*> & fList = lfIt->second;
//       list<const _maille*>::iterator fIt = fList.begin();
//       for ( ; fIt != fList.end(); fIt++ )
//         cout << "\t" << **fIt << fgm[*fIt]->nom << endl;
//     }

    // Each oriented link must appear in one face only, else a face is reversed.

    queue<const _maille*> faceQueue; // the queue contains well oriented faces
    // whose neighbors orientation is to be checked

    bool manifold = true;
    while ( !linkFacesMap.empty() )
    {
      if ( faceQueue.empty() ) {
        ASSERT_MED( !linkFacesMap.begin()->second.empty() );
        faceQueue.push( linkFacesMap.begin()->second.front() );
      }
      while ( !faceQueue.empty() )
      {
        const _maille* face = faceQueue.front();
        faceQueue.pop();

        // loop on links of <face>
        for ( int i = 0; i < (int)face->sommets.size(); ++i ) {
          _link link = face->link( i );
          // find the neighbor faces
          lfIt = linkFacesMap.find( link );
          int nbFaceByLink = 0;
          list< const _maille* > ml;
          if ( lfIt != linkFacesMap.end() )
          {
            list<const _maille*> & fList = lfIt->second;
            list<const _maille*>::iterator fIt = fList.begin();
            ASSERT_MED( fIt != fList.end() );
            for ( ; fIt != fList.end(); fIt++, nbFaceByLink++ ) {
              ml.push_back( *fIt );
              if ( *fIt != face ) // wrongly oriented neighbor face
              {
                const _maille* badFace = *fIt;
                // reverse and remove badFace from linkFacesMap
                for ( int j = 0; j < (int)badFace->sommets.size(); ++j ) {
                  _link badlink = badFace->link( j );
                  if ( badlink == link ) continue;
                  lfIt2 = linkFacesMap.find( badlink );
                  if ( lfIt2 != linkFacesMap.end() ) {
                    list<const _maille*> & ff = lfIt2->second;
                    ff.erase( find( ff.begin(), ff.end(), badFace ));
                        if ( ff.empty() )
                          linkFacesMap.erase( lfIt2 );
                  }
                }
                badFace->reverse = true; // reverse
                //INFOS_MED( "REVERSE " << *badFace );
                faceQueue.push( badFace );
              }
            }
            linkFacesMap.erase( lfIt );
          }
          // add good neighbors to the queue
          _link revLink( link.second, link.first );
          lfIt = linkFacesMap.find( revLink );
          if ( lfIt != linkFacesMap.end() )
          {
            list<const _maille*> & fList = lfIt->second;
            list<const _maille*>::iterator fIt = fList.begin();
            for ( ; fIt != fList.end(); fIt++, nbFaceByLink++ ) {
              ml.push_back( *fIt );
              if ( *fIt != face )
                faceQueue.push( *fIt );
            }
            linkFacesMap.erase( lfIt );
          }
          if ( nbFaceByLink > 2 ) {
            if ( manifold ) {
              list<const _maille*>::iterator i = ml.begin();
              INFOS_MED(nbFaceByLink << " faces by 1 link:");
              for( ; i!= ml.end(); i++ ) {
                INFOS_MED("in object " << fgm[ *i ]->nom);
                INFOS_MED( **i );
              }
            }
            manifold = false;
          }
        } // loop on links of the being checked face
      } // loop on the face queue
    } // while ( !linkFacesMap.empty() )

    if ( !manifold )
      INFOS_MED(" -> Non manifold mesh, faces orientation may be incorrect");


    // ---------------------------------------------------
    // fix connectivity of quadratic elements
    // ---------------------------------------------------

    for ( int dim = 1; dim <= 3; ++dim )
    {
      _maillageByDimIterator mailIt( medi, dim );
      while ( const set<_maille > * elems = mailIt.nextType() )
      {
        elemIt = elems->begin(), elemEnd = elems->end();
        for ( ; elemIt != elemEnd; elemIt++ )
        {
          // GIBI connectivity -> MED one
          if ( isQuadratic )
            fixConnectivity( *elemIt );

          // reverse quadratic faces
          if ( elemIt->reverse ) {
            if ( elemIt->geometricType != type ) {
              type = elemIt->geometricType;
              getReverseVector( type, swapVec );
            }
            reverse ( *elemIt, swapVec );
          }
        }
      }
    }

    // ---------------------------------------------------
    // Orient volumes according to MED conventions:
    // normal of a bottom (first) face should be downward
    // ---------------------------------------------------

    _maillageByDimIterator mailIt( medi, 3 );
    while ( const set<_maille > * elems = mailIt.nextType() )
    {
      elemIt = elems->begin(), elemEnd = elems->end();
      int nbBottomNodes = 0;
      switch ( elemIt->geometricType ) {
      case MED_TETRA4:
      case MED_TETRA10:
      case MED_PENTA6:
      case MED_PENTA15:
        nbBottomNodes = 3; break;
      case MED_PYRA5:
      case MED_PYRA13:
      case MED_HEXA8:
      case MED_HEXA20:
        nbBottomNodes = 4; break;
      default: continue;
      }
      // find a normal to the bottom face
      const _noeud* n[4] = {
        &elemIt->sommets[0]->second, // 3 bottom nodes
        &elemIt->sommets[1]->second,
        &elemIt->sommets[2]->second,
        &elemIt->sommets[nbBottomNodes]->second };// a top node
      double vec01 [3] = { // vector n[0]-n[1]
        n[1]->coord[0] - n[0]->coord[0],
        n[1]->coord[1] - n[0]->coord[1],
        n[1]->coord[2] - n[0]->coord[2], };
      double vec02 [3] = { // vector n[0]-n[2]
        n[2]->coord[0] - n[0]->coord[0],
        n[2]->coord[1] - n[0]->coord[1],
        n[2]->coord[2] - n[0]->coord[2] };
      double normal [3] = { // vec01 ^ vec02
        vec01[1] * vec02[2] - vec01[2] * vec02[1],
        vec01[2] * vec02[0] - vec01[0] * vec02[2],
        vec01[0] * vec02[1] - vec01[1] * vec02[0] };
      // check if the 102 angle is convex
      if ( nbBottomNodes > 3 ) {
        const _noeud* n3 = &elemIt->sommets[nbBottomNodes-1]->second;// last bottom node
        double vec03 [3] = { // vector n[0]-n3
          n3->coord[0] - n[0]->coord[0],
          n3->coord[1] - n[0]->coord[1],
          n3->coord[2] - n[0]->coord[2], };
        if ( fabs( normal[0]+normal[1]+normal[2] ) <= DBL_MIN ) { // vec01 || vec02
          normal[0] = vec01[1] * vec03[2] - vec01[2] * vec03[1]; // vec01 ^ vec03
          normal[1] = vec01[2] * vec03[0] - vec01[0] * vec03[2];
          normal[2] = vec01[0] * vec03[1] - vec01[1] * vec03[0];
        }
        else {
          double vec [3] = { // normal ^ vec01
            normal[1] * vec01[2] - normal[2] * vec01[1],
            normal[2] * vec01[0] - normal[0] * vec01[2],
            normal[0] * vec01[1] - normal[1] * vec01[0] };
          double dot2 = vec[0]*vec03[0] + vec[1]*vec03[1] + vec[2]*vec03[2]; // vec*vec03
          if ( dot2 < 0 ) { // concave -> reverse normal
            normal[0] *= -1;
            normal[1] *= -1;
            normal[2] *= -1;
          }
        }
      }
      // direction from top to bottom
      vector<double> tbDir(3);
      tbDir[0] = n[0]->coord[0] - n[3]->coord[0];
      tbDir[1] = n[0]->coord[1] - n[3]->coord[1];
      tbDir[2] = n[0]->coord[2] - n[3]->coord[2];
      // compare 2 directions: normal and top-bottom
      double dot = normal[0]*tbDir[0] + normal[1]*tbDir[1] + normal[2]*tbDir[2];
      if ( dot < 0. ) // need reverse
      {
        getReverseVector( elemIt->geometricType, swapVec );
        for ( ; elemIt != elemEnd; elemIt++ )
          reverse( *elemIt, swapVec );
      }
    } // loop on volumes of one geometry

  } // space dimension == 3
}

//=======================================================================
//function : fillMesh
//purpose  : load data from medi to mesh
//=======================================================================

void GIBI_MESH_RDONLY_DRIVER::fillMesh(_intermediateMED* _ptrMedi)
{
  const char* LOC = "GIBI_MESH_RDONLY_DRIVER::fillMesh(_intermediateMED* _ptrMedi) : ";
  BEGIN_OF_MED(LOC);

  _ptrMesh->_name = _meshName;

  if (_ptrMedi)
  {
    if (!_maillageByDimIterator(*_ptrMedi).nextType() ||
        _ptrMedi->groupes.empty() ||
        _ptrMedi->points.empty()) {
      INFOS_MED(" Error while reading file: the data read are not completed " );
      return;
    }
    // fix element orientation
    orientElements( *_ptrMedi );

    _ptrMesh->_spaceDimension = _ptrMedi->points.begin()->second.coord.size();
    _ptrMesh->_meshDimension  = _ptrMedi->getMeshDimension();
    _ptrMesh->_numberOfNodes  = _ptrMedi->points.size();
    _ptrMesh->_isAGrid        = 0;
    _ptrMesh->_coordinate     = _ptrMedi->getCoordinate();

    //Construction des groupes
    _ptrMedi->getGroups(_ptrMesh->_groupCell,
                        _ptrMesh->_groupFace,
                        _ptrMesh->_groupEdge,
                        _ptrMesh->_groupNode, _ptrMesh);

    _ptrMesh->_connectivity = _ptrMedi->getConnectivity();
  }
  END_OF_MED(LOC);
}

//================================================================================
/*!
 * \brief Create families from groups etc.
 */
//================================================================================

void GIBI_MESH_RDONLY_DRIVER::updateSupports()
{
  _ptrMesh->createFamilies();

  // add attributes to families
  set<string> famNames;
  for (medEntityMesh entity=MED_CELL; entity<MED_ALL_ENTITIES; ++entity)
  {
    int i, nb = _ptrMesh->getNumberOfFamilies(entity);
    for ( i = 1; i <= nb; ++i ) {
      FAMILY* f = const_cast<FAMILY*>( _ptrMesh->getFamily( entity, i ));
      f->setNumberOfAttributes( 1 );
      int* attIDs = new int[1];
      attIDs[0] = 1;
      f->setAttributesIdentifiers( attIDs );
      int* attVals = new int[1];
      attVals[0] = 1;
      f->setAttributesValues( attVals );
      string* attDescr = new string[1];
      attDescr[0] = "med_family";
      f->setAttributesDescriptions( attDescr );
      // limit a name length
      if ( f->getName().length() > 31 ) {
        ostringstream name;
        name << "FAM" << f->getIdentifier();
        f->setName( name.str());
      }
      // check if family is on the whole mesh entity
      if (_ptrMesh->getNumberOfElements( entity, MED_ALL_ELEMENTS ) ==
          f->getNumberOfElements( MED_ALL_ELEMENTS ))
        f->setAll( true );
    }
    // setAll() for groups
    nb = _ptrMesh->getNumberOfGroups(entity);
    for ( i = 1; i <= nb; ++i ) {
      GROUP * g = const_cast<GROUP*>( _ptrMesh->getGroup( entity, i ));
      if (_ptrMesh->getNumberOfElements( entity, MED_ALL_ELEMENTS ) ==
          g->getNumberOfElements( MED_ALL_ELEMENTS ))
        g->setAll( true );
    }
  }
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
  GIBI_MESH_DRIVER(fileName,ptrMesh,WRONLY)
{
  MESSAGE_MED("GIBI_MESH_WRONLY_DRIVER::GIBI_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}
GIBI_MESH_WRONLY_DRIVER::GIBI_MESH_WRONLY_DRIVER(const GIBI_MESH_WRONLY_DRIVER & driver):
  GIBI_MESH_DRIVER(driver)
{
}
GIBI_MESH_WRONLY_DRIVER::~GIBI_MESH_WRONLY_DRIVER()
{
  //MESSAGE_MED("GIBI_MESH_WRONLY_DRIVER::GIBI_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been destroyed");
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

//=======================================================================
//function : open
//purpose  :
//=======================================================================

void GIBI_MESH_WRONLY_DRIVER::open()
  //     throw (MEDEXCEPTION)
{
  if( _status == MED_OPENED )
    return;

  const char * LOC = "GIBI_MESH_DRIVER::open()";
  BEGIN_OF_MED(LOC);

  MED_EN::med_mode_acces aMode = getAccessMode();
  switch (aMode) {
  case MED_EN::RDWR:
  case MED_EN::WRONLY: // should never append !!
    _gibi.open(_fileName.c_str(), ios::out);
    break;
  default:
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Bad file mode access ! " << aMode));
  }
  //change for windows compilation
  if ( !_gibi ||
#ifdef WNT
      !_gibi.is_open()
#else
      !_gibi.rdbuf()->is_open()
#endif
      )
  {
    _status = MED_CLOSED;
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Could not open file "<<_fileName));
  }
  else
  {
    _status = MED_OPENED;
  }
  END_OF_MED(LOC);
}

//=======================================================================
//function : close
//purpose  :
//=======================================================================

void GIBI_MESH_WRONLY_DRIVER::close()
  //  throw (MEDEXCEPTION)
{
  const char* LOC = "GIBI_MESH_DRIVER::close() ";
  BEGIN_OF_MED(LOC);
    if ( _status == MED_OPENED)
    {
        _gibi.close();
        _status = MED_CLOSED;
    }
  END_OF_MED(LOC);
}

//=======================================================================
//function : write
//purpose  :
//=======================================================================

void GIBI_MESH_WRONLY_DRIVER::write(void) const
  throw (MEDEXCEPTION)
{
  const char* LOC = "void GIBI_MESH_WRONLY_DRIVER::write(void) const : ";
  BEGIN_OF_MED(LOC);

  // we are going to modify the _gibi field
  GIBI_MESH_WRONLY_DRIVER * me = const_cast<GIBI_MESH_WRONLY_DRIVER *>(this);
//  try {
    // IMP 0020434: mapping GIBI names to MED names
    list<nameGIBItoMED> listGIBItoMED_mail;
    list<nameGIBItoMED> listGIBItoMED_cham;
    list<nameGIBItoMED> listGIBItoMED_comp;
    me->writeSupportsAndMesh(listGIBItoMED_mail);
    me->writeMEDNames(listGIBItoMED_mail, listGIBItoMED_cham, listGIBItoMED_comp);
    me->writeLastRecord();
//   }
//   catch (MEDEXCEPTION &ex)
//   {
//     INFOS_MED( ex.what() );
//   }

  END_OF_MED(LOC);
}

//=======================================================================
//function : getName
//purpose  : return cleaned up support name
//=======================================================================

static string cleanName( const string& theName )
{
  string name = theName;
  if ( !name.empty() ) {
    /*
    // find a name string end
    int i, len = name.length();
    for ( i = 0; i < len; ++i ) {
      if ( name[i] == 0 )
        break;
    }
    // cut off trailing white spaces
    while ( i > 0 && name[i-1] == ' ' )
      i--;
    if ( i != len ) {
      name = name.substr( 0, i );
      len = i;
    }
    */
    // cut off leading white spaces
    int firstChar = name.find_first_not_of(" \t");
    if (firstChar < name.length())
    {
      name = name.substr(firstChar);
    }
    else
    {
      name = ""; // only whitespaces there - remove them
    }
    // cut off trailing white spaces
    int lastChar = name.find_last_not_of(" \t");
    if (lastChar < name.length()) {
      name = name.substr(0, lastChar + 1);
    }
  }
  return name;
}

//=======================================================================
//function : addSupport
//purpose  :
//=======================================================================

bool GIBI_MESH_WRONLY_DRIVER::addSupport( const SUPPORT * support )
{
  if ( !support )
    return false;
  map<const SUPPORT*,supportData>::iterator su = _supports.find( support );
  if ( su != _supports.end() )
    return ( su->second.getNumberOfTypes() > 0 );

  if ( support->getMesh() != _ptrMesh )
    throw MEDEXCEPTION(LOCALIZED(STRING("cant write support of other mesh" )));

  // get sub-supports and define a support type name
  string supType;
  list<const SUPPORT*> sList;
  const GROUP* group = dynamic_cast< const GROUP* >(support);
  if ( group )
  {
    if ( group->getNumberOfTypes() > 0 || group->isOnAllElements() )
      sList.push_back( group );
    else {
      int iFam, nbFam = group->getNumberOfFamilies();
      for ( iFam = 1; iFam <= nbFam; ++iFam )
        sList.push_back( group->getFamily( iFam ));
    }
    supType = "group";
  }
  else
  {
    sList.push_back( support );
    supType = dynamic_cast< const FAMILY* >(support) ? "family" : "support";
  }

  supportData & data = _supports[ support ];
  data._cleanName = cleanName( support->getName() );

  // check if it is a writtable support, i.e.
  // nodal connectivity for a support entity exists
  medEntityMesh entity = support->getEntity();
  if ( entity != MED_NODE && !_ptrMesh->existConnectivity( MED_NODAL, entity )) {
    INFOS_MED("Do not save " << supType << " of entity " << entity
          << " named <" << data._cleanName << "> nodal connectivity not defined");
    return false;
  }

  // fill supportData
  list<const SUPPORT*>::iterator sIt = sList.begin();
  for ( ; sIt != sList.end(); sIt++ )
  {
    bool onAll = (*sIt)->isOnAllElements();
    int nbTypes = 0;
    if ( !onAll )
      nbTypes = (*sIt)->getNumberOfTypes();
    else
      nbTypes = _ptrMesh->getNumberOfTypes( entity );
    if ( nbTypes == 0 )
      continue;
    const medGeometryElement* types = 0;
    if ( !onAll )
      types = (*sIt)->getTypes();
    else if ( entity != MED_NODE )
      types = _ptrMesh->getTypes( entity );
    for ( int iType = 0; iType < nbTypes; ++iType )
    {
      if ( types && types[ iType ] > MED_HEXA20 )
        continue; // poly
      medGeometryElement geomType = types ? types[ iType ] : MED_ALL_ELEMENTS;
      const int * ptrElemIDs = 0;
      int elemID1 = 0, nbElems = 0;
      if ( onAll ) {
        nbElems = _ptrMesh->getNumberOfElements( entity, geomType );
        elemID1 = (entity == MED_NODE) ? 1 : _ptrMesh->getGlobalNumberingIndex (entity)[ iType ];
      }
      else {
        nbElems = (*sIt)->getNumberOfElements( geomType );
        ptrElemIDs = (*sIt)->getNumber( geomType );
      }
      if ( geomType == 0 || ( entity == MED_NODE ))
        geomType = MED_POINT1;

      data.addTypeData( geomType, nbElems, ptrElemIDs, elemID1 );
    }
  }

  if ( data.getNumberOfTypes() == 0 ) {
    INFOS_MED("Do not save " << supType << " of entity " << entity
          << " named <" << data._cleanName << "> no geometric types");
    return false;
  }

  return true;
}

//=======================================================================
//function : getSupportIndex
//purpose  :
//=======================================================================

int GIBI_MESH_WRONLY_DRIVER::getSubMeshIdAndSize(const SUPPORT *        support,
                                                 list<pair<int,int> > & idsAndSizes) const
{
  idsAndSizes.clear();
  map<const SUPPORT*,supportData>::const_iterator su = _supports.find( support );
  if ( su == _supports.end() )
    return 0;

  supportData * data = const_cast<supportData *>( & su->second );
  int id = data->_id;
  if ( data->getNumberObjects() > data->getNumberOfTypes() )
    id++;
  supportData::typeIterator tIt = data->_types.begin();
  for ( ; tIt != data->_types.end(); ++tIt )
  {
    int size = 0;
    list< typeData >& td = tIt->second;
    list< typeData >::iterator tdIt = td.begin();
    for ( ; tdIt != td.end(); ++tdIt )
      size += tdIt->_nbElems;
    idsAndSizes.push_back( make_pair( id++, size ));
  }
  return idsAndSizes.size();
}

// ============================================================
// the class writes endl to the file as soon as <limit> fields
// have been written after the last endl
// ============================================================

class TFieldCounter
{
  fstream& _file;
  int _count, _limit;
 public:
  TFieldCounter(fstream& f, int limit=0): _file(f), _limit(limit) { init(); }
  void init(int limit=0) // init, is done by stop() as well
  { if (limit) _limit = limit; _count = 0; }
  void operator++(int) // next
  { if ( ++_count == _limit ) { _file << endl; init(); }}
  void stop() // init() and write endl if there was no endl after the last written field
  { if ( _count ) _file << endl; init(); }
};

//=======================================================================
//function : writeElements
//purpose  : ptrElemIDs and elemID1 provide two alternative ways of giving
//           elements to write.
//           If elemSet != 0 then an element is
//           ( addElemInSet ? <written and added to elemSet> : <ignored if id is in elemSet>)
//=======================================================================

void GIBI_MESH_WRONLY_DRIVER::writeElements (medGeometryElement geomType,
                                             list< typeData >&  typeDataList,
                                             const int *        nodalConnect,
                                             const int *        nodalConnectIndex)
{
  // ITYPEL : type de l'鬩ment 1=point, 2=segment ?eux noeuds...
  // NBSOUS : nombre de sous parties dans cet objet,
  //          une sous partie par type d'鬩ments le composant.
  // NBREF : nombre de sous r馩rences. Une r馩rence est par exemple le contour
  // NBNOEL : nombre de noeuds par 鬩ment
  // NBEL : nombre d'鬩ments

  int castemType = GIBI_MESH_DRIVER::med2gibiGeom( geomType );
  const char* zeroI8 = "       0"; // FORMAT(I8)
  int nbElemNodes = geomType % 100;

  // indices to transform MED connectivity to GIBI one
  vector< int > toGibiConn;
  toGibiConn.reserve( nbElemNodes );
  if ( const int * toMedConn = getGibi2MedConnectivity( geomType )) {
    toGibiConn.resize( nbElemNodes );
    for ( int i = 0; i < nbElemNodes; ++i )
      toGibiConn[ toMedConn[ i ]] = i;
  }
  else {
    while ( toGibiConn.size() < nbElemNodes )
      toGibiConn.push_back( toGibiConn.size() );
  }

  // count total nb of elements
  int nbElements = 0;
  list< typeData >::iterator td = typeDataList.begin();
  for ( ; td != typeDataList.end(); td++ )
    nbElements += td->_nbElems;

  _gibi << setw(8) << castemType <<  // ITYPE
    zeroI8 <<                       // NBSOUS
      zeroI8 <<                     // NBREF
        setw(8) << nbElemNodes <<   // NBNOEL
          setw(8) << nbElements <<  // NBEL
            endl;

  MESSAGE_MED("writeElements(): geomType=" << geomType << " nbElements= " << nbElements)

  // L 'enregistrement donnant le num? de la couleur des 鬩ments.
  // * 8000 FORMAT(10I8)
  TFieldCounter fcount( _gibi, 10 );
  int iElem = 0;
  for ( ; iElem < nbElements; ++iElem, fcount++ )
    _gibi << zeroI8;
  fcount.stop();

  // Tableau des connectivit鳮 Description du premier 鬩ment puis du deuxi譥...
  // ATTENTION il ne s'agit pas de la num?tation vraie,
  // il faut la faire passer par le filtre du dernier tableau de la pile num? 32.
  //int nbSkipped = 0;

  for ( td = typeDataList.begin(); td != typeDataList.end(); td++ )
  {
    for ( int i = 0; i < td->_nbElems; i++ )
    {
      iElem = td->_ptrElemIDs ? td->_ptrElemIDs[ i ] : td->_elemID1 + i;
      if ( geomType == MED_POINT1 )
      {
        _gibi << setw(8) << iElem;
        fcount++;
      }
      else
      {
        int nodeId = nodalConnectIndex[ iElem - 1 ] - 1;
        for ( int iNode = 0; iNode < nbElemNodes; ++iNode, fcount++ ) {
          _gibi << setw(8) << nodalConnect[ nodeId + toGibiConn[ iNode ]];
        }
      }
    }
  }

  fcount.stop();
}

//=======================================================================
//function : addName
//purpose  : make name uppercase and shorter than 9, add it to nameNbMap,
//           raise if not unique
//=======================================================================
/*
void GIBI_MESH_WRONLY_DRIVER::addName(map<string,int>& nameMap,
                                      string&          theName,
                                      int              index,
                                      string           prefix)
{
  string name = cleanName( theName );
  if ( !name.empty() ) {
    int len = name.length();
#ifdef THROW_ON_BAD_NAME
    if ( len > 8 )
      throw MEDEXCEPTION(STRING("Can't write name longer than 8: ") << name );

    for ( int i = 0; i < len; ++i )
      name[i] = toupper( name[i] );
    if ( ! nameMap.insert( make_pair( name, index )).second )
      throw MEDEXCEPTION(STRING("Can't write not unique name: ") << name );
#else
    bool ok = ( len <= 8 && len > 0 );
    if ( ok ) {
      for ( int i = 0; i < len; ++i )
        name[i] = toupper( name[i] );
      ok = nameMap.insert( make_pair( name, index )).second;
    }
    if ( !ok ) {
      char *str=new char[ prefix.size() + 13 ];
      int j = 1;
      do {
        sprintf( str, "%s_%d", prefix.c_str(), nameMap.size()+j );
        ok = nameMap.insert( make_pair( str, index )).second;
        j++;
      } while ( !ok );
      INFOS_MED( "Save <" << name << "> as <" << str << ">");
      delete [] str;
    }
#endif
  }
}
*/

// Converts names like:
// MED:
//   TEMPERATURE_FLUIDE
//   TEMPERATURE_SOLIDE
//   PRESSION
//   NU
//   VOLUM001
//   VOLUMOFOBJECT
//   VOLUM002
//
// GIBI:
//   TEMPE001
//   TEMPE002
//   PRESSION
//   NU
//   VOLUM001
//   VOLUM003
//   VOLUM002
void GIBI_MESH_WRONLY_DRIVER::addName (map<string,int>& nameMap,
                                       map<string,int>& namePrefixesMap,
                                       string&          theName,
                                       int              index)
{
  string name = cleanName(theName);
  int ind = index;

  if (!name.empty()) {
    int len = name.length();
    for (int i = 0; i < len; ++i)
      name[i] = toupper(name[i]);

    bool doResave = false; // only for tracing

    // I. Save a short name as it is
    if (len <= 8) {
      INFOS_MED("Save <" << theName << "> as <" << name << ">");

      map<string,int>::iterator it = nameMap.find(name);
      if (it != nameMap.end()) {
        // There is already such name in the map.

        // a. Replace in the map the old pair by the current one
        int old_ind = nameMap[name];
        nameMap[name] = ind;
        // b. Rebuild the old pair (which was in the map,
        //    it seems to be built automatically by step II)
        ind = old_ind;
        // continue with step II
        doResave = true; // only for tracing
      }
      else {
        // Save in the map
        nameMap.insert(make_pair(name, ind));

        // Update loc_index for this name (if last free characters represents a number)
        // to avoid conflicts with long names, same in first 5 characters
        if (len == 8) {
          int new_loc_index = atoi(name.c_str() + 5);
          if (new_loc_index > 0) {
            // prefix
            char str [6];
            strncpy(str, name.c_str(), 5);
            str[5] = '\0';

            if (namePrefixesMap.find(str) != namePrefixesMap.end()) {
              int old_loc_index = namePrefixesMap[str];
              if (new_loc_index < old_loc_index) new_loc_index = old_loc_index;
            }
            namePrefixesMap[str] = new_loc_index;
          }
        }
        return;
      }
    } // if (len <= 8)

    // II. Cut long name and add a numeric suffix

    // first 5 or less characters of the name
    if (len > 5) len = 5;
    char str [9];
    str[8] = '\0';
    int addr = 0;
    strncpy(str, name.c_str(), len);
    addr = len;
    str[addr] = '\0';

    // numeric suffix
    int loc_index = 1;
    if (namePrefixesMap.find(str) != namePrefixesMap.end())
      loc_index = namePrefixesMap[str] + 1;
    namePrefixesMap[str] = loc_index;

    if (loc_index > 999)
      throw MEDEXCEPTION(STRING("Can't write not unique name: ") << name);

    if (loc_index < 100) {
      str[addr] = '0';
      addr++;
    }
    if (loc_index < 10) {
      str[addr] = '0';
      addr++;
    }
    sprintf(str + addr, "%d", loc_index);

    nameMap.insert(make_pair(str, ind));
    if (doResave) {
      INFOS_MED("Resave previous <" << name << "> as <" << str << ">");
    }
    else {
      INFOS_MED("Save <" << theName << "> as <" << str << ">");
    }
  }
}

//=======================================================================
//function : writeNames
//purpose  :
//=======================================================================

void GIBI_MESH_WRONLY_DRIVER::writeNames( map<string,int>& nameNbMap )
{
  // La pile num? 1 est celle des objets de type maillage.
  // La ligne suivante donne le nom des objets maillages sauv鳮
  // * 8001       FORMAT(8(1X,A8))
  if ( !nameNbMap.empty() )
  {
    TFieldCounter fcount( _gibi, 8 );
    _gibi << left;
    map<string,int>::iterator nameNbIt = nameNbMap.begin();
    for ( ; nameNbIt != nameNbMap.end(); nameNbIt++, fcount++ ) {
      _gibi << " " << setw(8) << nameNbIt->first;
    }
    fcount.stop();
    _gibi << right;
    // La ligne suivante donne les num?s d'ordre, dans la pile,
    // des objets nomm?cit?pr飩demment.
    // *  8000 FORMAT(10I8)
    nameNbIt = nameNbMap.begin();
    for ( fcount.init(10); nameNbIt != nameNbMap.end(); nameNbIt++, fcount++ )
      _gibi << setw(8) << nameNbIt->second;
    fcount.stop();
  }
}

//=======================================================================
//function : writeSupportsAndMesh
//purpose  :
//=======================================================================

void GIBI_MESH_WRONLY_DRIVER::writeSupportsAndMesh(list<nameGIBItoMED>& listGIBItoMED_mail)
{
  const char * LOC = "void GIBI_MESH_WRONLY_DRIVER::writeSupportsAndMesh() ";
  BEGIN_OF_MED(LOC);

  if (_status!=MED_OPENED)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "file " << _fileName<<  " is not opened." ));
  if (!_ptrMesh)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "can't write a NULL mesh" ));
  if (!_ptrMesh->getConnectivityptr())
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "can't write a mesh with NULL connectivity" ));

  // fill _supports with families and groups
  medEntityMesh entity;
  for (entity=MED_CELL; entity<MED_ALL_ENTITIES; ++entity)
  {
    int i, nb = _ptrMesh->getNumberOfGroups(entity);
    for ( i = 1; i <= nb; ++i )
      addSupport( _ptrMesh->getGroup( entity, i ));
//     nb = _ptrMesh->getNumberOfFamilies(entity);
//     for ( i = 1; i <= nb; ++i )
//       addSupport( _ptrMesh->getFamily( entity, i ));
  }

  // --------------------------------------------------------------------
  // Count total nb of objects: an object per an element type in support
  // plus an object per an element type not used in _supports.
  // Collect object names
  // --------------------------------------------------------------------

  vector<int> nbSuppElemsByType(MED_HEXA20+1,0);
  map<string,int> nameNbMap;
  map<string,int> namePrefixMap;
  map<const SUPPORT*,supportData>::iterator supIt = _supports.begin();
  int i, nb_objects = 0;
  for ( ; supIt != _supports.end(); supIt++ )
  {
    supportData & data = supIt->second;
    int nbSupObj = data.getNumberObjects();
    if ( nbSupObj == 0 )
      continue;
    data._id = nb_objects + 1;
    nb_objects += nbSupObj;

    //addName( nameNbMap, data._cleanName, data._id, "C" );
    addName(nameNbMap, namePrefixMap, data._cleanName, data._id);

    // IMP 0020434: mapping GIBI names to MED names
    nameGIBItoMED aMEDName;
    aMEDName.gibi_pile = 1; // PILE_SOUS_MAILLAGE
    aMEDName.gibi_id = data._id;
    aMEDName.med_name = data._cleanName;
    listGIBItoMED_mail.push_back(aMEDName);

    MESSAGE_MED( "obj " << data._id << " " << data._cleanName);

    // count elements: take into account supports on all elements and families only
    const SUPPORT* support = supIt->first;
    if ( support->isOnAllElements() || dynamic_cast< const FAMILY* >( support ))
    {
      supportData::typeIterator tIt = data._types.begin();
      for ( ; tIt != data._types.end(); ++tIt )
        if ( support->isOnAllElements() )
        {
          nbSuppElemsByType[ tIt->first ] = INT_MAX / 100;
        }
        else
        {
          list< typeData >& td = tIt->second;
          list< typeData >::iterator tdIt = td.begin();
          for ( ; tdIt != td.end(); ++tdIt )
            nbSuppElemsByType[ tIt->first] += tdIt->_nbElems;
        }
    }
  }

  // count types of mesh elements that are not all in _supports
  int iType, nbTypes;
  entity = _ptrMesh->getConnectivityptr()->getEntity();
  for ( ; entity < MED_NODE; entity++ )
  {
    nbTypes = _ptrMesh->getNumberOfTypes( entity );
    if ( nbTypes == 0 || !_ptrMesh->existConnectivity( MED_NODAL, entity ))
      continue;
    const medGeometryElement* types = _ptrMesh->getTypes( entity );
    for ( iType = 0; iType < nbTypes; ++iType )
    {
      int nbElemInSups = nbSuppElemsByType[ types[ iType ]];
      int nbElemInMesh = _ptrMesh->getNumberOfElements(entity, types[ iType ]);
      if ( nbElemInSups < nbElemInMesh ) {
        nb_objects++;
        nbSuppElemsByType[ types[ iType ]] = -1; // to keep written elements of _supports
      }
    }
  }

  // ------------
  // Write file
  // ------------

  // Premier paquet dont le nombre de lignes ne varie pas.
  // On y trouve des indications g鮩rales.
  const int dim = _ptrMesh->getSpaceDimension();
  _gibi << " ENREGISTREMENT DE TYPE   4" << endl;
  _gibi << " NIVEAU  15 NIVEAU ERREUR   0 DIMENSION   " << dim <<endl;
  _gibi << " DENSITE  .00000E+00" << endl;
  _gibi << " ENREGISTREMENT DE TYPE   7" << endl;
  _gibi << " NOMBRE INFO CASTEM2000   8" <<endl;
  _gibi << " IFOUR  -1 NIFOUR   0 IFOMOD  -1 IECHO   1 IIMPI   0 IOSPI   0 ISOTYP   1" << endl;
  _gibi << " NSDPGE     0" << endl;

  // Deuxi譥 paquet qui d馩nit toutes les piles
  // (une pile par type d'objet et certaines piles en plus).
  // Un enregistrement de type 2 pr鶩ent de l'飲iture d'une nouvelle pile,
  // celui de type 5 pr鶩ent de la fin.
  // * 800   FORMAT (' ENREGISTREMENT DE TYPE', I4)
  _gibi << " ENREGISTREMENT DE TYPE   2" << endl;
  // * 801     FORMAT(' PILE NUMERO',I4,'NBRE OBJETS NOMMES',I8,'NBRE OBJETS',I8)
  _gibi << " PILE NUMERO   1NBRE OBJETS NOMMES" << setw(8) << nameNbMap.size() <<
    "NBRE OBJETS" << setw(8) << nb_objects <<endl;

  writeNames( nameNbMap );

  // Passage ?a description des objets les uns apr賠les autres.
  // Le premier enregistrement de chaque objet est compos頤e 5 nombres repr鳥ntant :
  // ITYPEL : type de l'鬩ment 1=point, 2=segment ?eux noeuds...
  // NBSOUS : nombre de sous parties dans cet objet,
  //          une sous partie par type d'鬩ments le composant.
  // NBREF : nombre de sous r馩rences. Une r馩rence est par exemple le contour
  // NBNOEL : nombre de noeuds par 鬩ment
  // NBEL : nombre d'鬩ments
  // Si ITYPEL=0 alors NBSOUS diff鲥nt de z?. Dans ce cas on lira la liste des positions,
  // dans la pile des objets, des sous parties le composant.
  // Si NBSOUS=0, NBNOEL et NBEL sont diff鲥nts de z?, on trouve, au besoin,
  // la liste des r馩rences , les num?s des couleurs puis les connectivit鳮

  TFieldCounter fcount( _gibi, 10 );
  const char* zeroI8 = "       0"; // FORMAT(I8)
  for ( supIt = _supports.begin(); supIt != _supports.end(); supIt++ )
  {
    supportData & data = supIt->second;
    int nbSupObj = data.getNumberObjects();
    if ( nbSupObj == 0 )
      continue;
    MESSAGE_MED("support " << data._id << "<" << data._cleanName << ">");

    // write a compound object
    int nbTypes = data.getNumberOfTypes();
    if ( nbSupObj > nbTypes )
    {
      _gibi << zeroI8 << setw(8) << nbTypes << zeroI8 << zeroI8 << zeroI8 << endl;
      for ( int i_sub = 1; i_sub <= nbTypes; ++i_sub, fcount++ )
        _gibi << setw(8) << ( data._id + i_sub );
      fcount.stop();
    }

    // write components
    entity = supIt->first->getEntity();
    const int * nodalConnect = 0, * nodalConnectIndex = 0;
    if ( entity != MED_NODE ) {
      nodalConnect = _ptrMesh->getConnectivity (MED_FULL_INTERLACE,
                                                MED_NODAL,
                                                entity,
                                                MED_ALL_ELEMENTS);
      nodalConnectIndex = _ptrMesh->getConnectivityIndex (MED_NODAL,entity);
    }
    supportData::typeIterator tIt = data._types.begin();
    for ( ; tIt != data._types.end(); ++tIt )
    {
      writeElements (tIt->first,
                     tIt->second,
                     nodalConnect,
                     nodalConnectIndex);
    }
  }  // loop on _supports

  // Write elements that are not in _supports

  supportData data;
  entity = _ptrMesh->getConnectivityptr()->getEntity();
  for ( ; entity < MED_NODE; entity++ )
  {
    int nbTypes = _ptrMesh->getNumberOfTypes( entity );
    if ( nbTypes == 0 || !_ptrMesh->existConnectivity( MED_NODAL, entity ))
      continue;
    const medGeometryElement* types = _ptrMesh->getTypes( entity );
    const int * nbIndex = _ptrMesh->getGlobalNumberingIndex (entity);
    const int * nodalConnect = 0, * nodalConnectIndex = 0;
    nodalConnect = _ptrMesh->getConnectivity (MED_FULL_INTERLACE,
                                              MED_NODAL,
                                              entity,
                                              MED_ALL_ELEMENTS);
    nodalConnectIndex = _ptrMesh->getConnectivityIndex (MED_NODAL,entity);

    for ( int iType = 1; iType <= nbTypes; ++iType )
    {
      int nbElements = nbIndex[ iType ] - nbIndex[ iType - 1 ];
      medGeometryElement geomType = types[ iType - 1 ];
      if ( nbSuppElemsByType[ geomType ] >= nbElements )
        continue; // all elements are written with _supports

      int elemId1 = nbIndex[ iType - 1 ];
      data.addTypeData( geomType, nbElements, 0, elemId1 );

      writeElements (geomType,
                     data._types[ geomType ],
                     nodalConnect,
                     nodalConnectIndex);
    }
  }

  // D颵t de la pile 32 (celle des points)

  int nbNodes = _ptrMesh->getNumberOfNodes();
  _gibi << " ENREGISTREMENT DE TYPE   2" << endl;
  _gibi << " PILE NUMERO  32NBRE OBJETS NOMMES       0" <<
    "NBRE OBJETS" << setw(8) << nbNodes << endl;
  // Liste des noms de points
  // * 8001       FORMAT(8(1X,A8))
  // No named nodes
  // suit le nombre de noeuds
  _gibi << setw(8) << nbNodes << endl;
  // Le tableau suivant donne le filtre pour avoir le vrai num? des noeuds
  // appartenant aux 鬩ments d飲its. Par exemple, si un 鬩ment, d飲it
  // dans la pile 1, fait r馩rence ?n num? de noeud 駡l ? il faut le
  // mettre 駡l ?2
  // * 8000 FORMAT(10I8)
  for ( i = 0; i < nbNodes; ++i, fcount++ )
    _gibi << setw(8) << i + 1;
  fcount.stop();

  // D颵t de pile 33 (celle des configurations (coordonn?))
  _gibi << " ENREGISTREMENT DE TYPE   2" << endl;
  _gibi << " PILE NUMERO  33NBRE OBJETS NOMMES       0NBRE OBJETS       1" << endl;
  // Suit le nombre de points dont on donne les coordonn?
  int nbValues = nbNodes * ( dim + 1 );
  _gibi << setw(8) << nbValues << endl;
  // Les coordonn? sont donn? par noeuds. D'abord le premier puis le deuxi譥...
  // Pour chaque noeuds, on donne les 2 ou 3 coordonn? plus la densit頣ourante
  // au moment de sa cr顴ion.
  // * 8003   FORMAT(1P,3E22.14)
  _gibi.precision(14);
  _gibi.setf( ios_base::scientific, ios_base::floatfield );
  _gibi.setf( ios_base::uppercase );
  const double * coords = _ptrMesh->getCoordinates(MED_FULL_INTERLACE);
  int j = 0;
  const double precision = 1.e-99; // PAL12077
  for ( fcount.init(3),i = 0; i < nbNodes; ++i, j += dim )
  {
    for ( int iCoord = 0; iCoord < dim; ++iCoord, fcount++ ) {
      double coo = coords[ j + iCoord ];
      bool  zero = ( -precision < coo && coo < precision );
      _gibi << setw(22) << ( zero ? 0.0 : coo );
    }
    _gibi << setw(22) << 0.0; // densite
    fcount++;
  }
  fcount.stop();

  END_OF_MED(LOC);
}

//=======================================================================
//function : writeMEDNames
//purpose  :
//=======================================================================
void GIBI_MESH_WRONLY_DRIVER::writeMEDNames (const list<nameGIBItoMED>& listGIBItoMED_mail,
                                             const list<nameGIBItoMED>& listGIBItoMED_cham,
                                             const list<nameGIBItoMED>& listGIBItoMED_comp)
{
  // IMP 0020434: mapping GIBI names to MED names
  // Store correspondence between GIBI and MED names
  // as one PILE_STRINGS and one PILE_TABLES
  // (in three tables: MED_MAIL, MED_CHAM and MED_COMP)

  int nbNames_mail = listGIBItoMED_mail.size();
  int nbNames_cham = listGIBItoMED_cham.size();
  int nbNames_comp = listGIBItoMED_comp.size();

  int nbTables = 0;
  if (nbNames_mail) nbTables++;
  if (nbNames_cham) nbTables++;
  if (nbNames_comp) nbTables++;

  if (!nbTables)
    return;

  // The whole string (concatenated names)
  string theWholeString;
  list<int> theOffsets;
  int currOffset = 0;

  // The TABLE PILE
  // * 800   FORMAT (' ENREGISTREMENT DE TYPE', I4)
  _gibi << " ENREGISTREMENT DE TYPE   2" << endl;
  // * 801     FORMAT(' PILE NUMERO',I4,'NBRE OBJETS NOMMES',I8,'NBRE OBJETS',I8)
  _gibi << " PILE NUMERO  10NBRE OBJETS NOMMES" << setw(8) << nbTables <<
    "NBRE OBJETS" << setw(8) << nbTables << endl; // <nbTables> named tables
  // Table names
  if (nbNames_mail)
    _gibi << " MED_MAIL";
  if (nbNames_cham)
    _gibi << " MED_CHAM";
  if (nbNames_comp)
    _gibi << " MED_COMP";
  _gibi << endl;
  // Table indices
  _gibi << setw(8) << 1;
  if (nbTables > 1) _gibi << setw(8) << 2;
  if (nbTables > 2) _gibi << setw(8) << 3;
  _gibi << endl;

  int istr = 1;

  // Table MED_MAIL
  if (nbNames_mail) {
    // Provide unique MED names, to exclude conflicts on reading saved files
    // (use case: read fra.med, save it to GIBI, read it from GIBI,
    // save to MED again -> this new MED file is not readable)
    set<string> medUniqueNames;

    _gibi << setw(8) << nbNames_mail*4 << endl; // Nb of table values

    TFieldCounter fcount1 (_gibi, 10);
    _gibi << right;
    list<nameGIBItoMED>::const_iterator itGIBItoMED = listGIBItoMED_mail.begin();
    for (; itGIBItoMED != listGIBItoMED_mail.end(); itGIBItoMED++, istr++) {
      // PILE of i-th key(med name)
      _gibi << setw(8) << 27; fcount1++; // PILE_STRINGS number is 27
      // ID of i-th key(med name)
      _gibi << setw(8) << istr; fcount1++;

      // PILE of i-th value(gibi name)
      _gibi << setw(8) << itGIBItoMED->gibi_pile; fcount1++; // PILE_SOUS_MAILLAGE number is 1
      // ID of i-th value(gibi name)
      _gibi << setw(8) << itGIBItoMED->gibi_id; fcount1++;

      // check MED name to be unique
      string aMedName = itGIBItoMED->med_name;
      if (!medUniqueNames.insert(aMedName).second) {
        string aMedNameNew;
        int ind = 1;
        char strInd [32];
        do {
          sprintf(strInd, "_%d", ind++);
          aMedNameNew = aMedName + strInd;
        } while (!medUniqueNames.insert(aMedNameNew).second);
        aMedName = aMedNameNew;
      }

      // add to the string
      theWholeString += aMedName; // MED name

      // add an offset
      currOffset += aMedName.length();
      theOffsets.push_back(currOffset);
    }
    fcount1.stop();
  }

  // Table MED_CHAM
  if (nbNames_cham) {
    // Provide unique MED names, to exclude conflicts on reading saved files
    // (use case: read fra.med, save it to GIBI, read it from GIBI,
    // save to MED again -> this new MED file is not readable)
    set<string> medUniqueNames;

    _gibi << setw(8) << nbNames_cham*4 << endl; // Nb of table values

    TFieldCounter fcount1 (_gibi, 10);
    _gibi << right;
    list<nameGIBItoMED>::const_iterator itGIBItoMED = listGIBItoMED_cham.begin();
    for (; itGIBItoMED != listGIBItoMED_cham.end(); itGIBItoMED++, istr++) {
      // PILE of i-th key(med name)
      _gibi << setw(8) << 27; fcount1++; // PILE_STRINGS number is 27
      // ID of i-th key(med name)
      _gibi << setw(8) << istr; fcount1++;

      // PILE of i-th value(gibi name)
      // PILE_NODES_FIELD number is 2, PILE_FIELD number is 39
      _gibi << setw(8) << itGIBItoMED->gibi_pile; fcount1++;
      // ID of i-th value(gibi name)
      _gibi << setw(8) << itGIBItoMED->gibi_id; fcount1++;

      // check MED name to be unique
      string aMedName = itGIBItoMED->med_name;
      if (!medUniqueNames.insert(aMedName).second) {
        string aMedNameNew;
        int ind = 1;
        char strInd [32];
        do {
          sprintf(strInd, "_%d", ind++);
          aMedNameNew = aMedName + strInd;
        } while (!medUniqueNames.insert(aMedNameNew).second);
        aMedName = aMedNameNew;
      }

      // add to the string
      theWholeString += aMedName; // MED name

      // add an offset
      currOffset += aMedName.length();
      theOffsets.push_back(currOffset);
    }
    fcount1.stop();
  }

  // Table MED_COMP
  if (nbNames_comp) {
    // for components, both key and value (long and short name) is in the STRING PILE

    _gibi << setw(8) << nbNames_comp*4 << endl; // Nb of table values

    TFieldCounter fcount1 (_gibi, 10);
    _gibi << right;
    list<nameGIBItoMED>::const_iterator itGIBItoMED = listGIBItoMED_comp.begin();
    for (; itGIBItoMED != listGIBItoMED_comp.end(); itGIBItoMED++, istr+=2) {
      // PILE of i-th key(med name)
      _gibi << setw(8) << 27; fcount1++; // PILE_STRINGS number is 27
      // ID of i-th key(med name)
      _gibi << setw(8) << istr; fcount1++;

      // PILE of i-th value(gibi name)
      _gibi << setw(8) << 27; fcount1++; // PILE_STRINGS number is 27
      // ID of i-th value(gibi name)
      _gibi << setw(8) << istr + 1; fcount1++;

      // add to the string
      string aMedName  = itGIBItoMED->med_name;
      string aGibiName = itGIBItoMED->gibi_name;
      theWholeString += aMedName; // MED name
      theWholeString += aGibiName; // GIBI name

      // add offsets
      currOffset += aMedName.length();
      theOffsets.push_back(currOffset);
      currOffset += aGibiName.length();
      theOffsets.push_back(currOffset);
    }
    fcount1.stop();
  }

  int nbNames = nbNames_mail + nbNames_cham + 2 * nbNames_comp; // tmp

  // The STRING PILE
  // * 800   FORMAT (' ENREGISTREMENT DE TYPE', I4)
  _gibi << " ENREGISTREMENT DE TYPE   2" << endl;
  // * 801     FORMAT(' PILE NUMERO',I4,'NBRE OBJETS NOMMES',I8,'NBRE OBJETS',I8)
  _gibi << " PILE NUMERO  27NBRE OBJETS NOMMES" << setw(8) << 0 << // No named strings
    "NBRE OBJETS" << setw(8) << nbNames << endl;

  // Write IDs
  //TFieldCounter fcount2 (_gibi, 10);
  //_gibi << right;
  //for (int jj = 1; jj <= nbNames; jj++) {
  //  _gibi << setw(8) << jj; fcount2++;
  //}
  //fcount2.stop();

  // <LENGTH>     <NB>
  _gibi << setw(8) << theWholeString.length()
        << setw(8) << nbNames << endl;

  // Write the whole string
  const int fixedLength = 71;
  int aPos = 0;
  int aLen = theWholeString.length();
  for (; aPos < aLen; aPos += fixedLength) {
    _gibi << setw(72) << theWholeString.substr(aPos, fixedLength) << endl;
  } while (aPos < aLen);

  // Write the offsets
  TFieldCounter fcount3 (_gibi, 10);
  _gibi << right;
  list<int>::iterator offsetIt = theOffsets.begin();
  for (; offsetIt != theOffsets.end(); offsetIt++) {
    _gibi << setw(8) << (*offsetIt); fcount3++;
  }
  fcount3.stop();
}

//=======================================================================
//function : writeLastRecord
//purpose  :
//=======================================================================

void GIBI_MESH_WRONLY_DRIVER::writeLastRecord()
{
  _gibi << " ENREGISTREMENT DE TYPE   5" << endl;
  _gibi << "LABEL AUTOMATIQUE :   1" << endl;
}

/*--------------------- RDWR PART -------------------------------*/

GIBI_MESH_RDWR_DRIVER::GIBI_MESH_RDWR_DRIVER():GIBI_MESH_DRIVER()
{
}
GIBI_MESH_RDWR_DRIVER::GIBI_MESH_RDWR_DRIVER(const string & fileName,
                                             MESH * ptrMesh):
       GIBI_MESH_DRIVER(fileName,ptrMesh,RDWR)
{
  MESSAGE_MED("GIBI_MESH_RDWR_DRIVER::GIBI_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}
GIBI_MESH_RDWR_DRIVER::GIBI_MESH_RDWR_DRIVER(const GIBI_MESH_RDWR_DRIVER & driver):
  GIBI_MESH_RDONLY_DRIVER::GIBI_MESH_DRIVER(driver)
{
  MESSAGE_MED("GIBI_MESH_RDWR_DRIVER::GIBI_MESH_RDWR_DRIVER(driver) has been created");
}
GIBI_MESH_RDWR_DRIVER::~GIBI_MESH_RDWR_DRIVER() {
  MESSAGE_MED("GIBI_MESH_RDWR_DRIVER::GIBI_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) has been destroyed");
}
GENDRIVER * GIBI_MESH_RDWR_DRIVER::copy(void) const
{
  const char* LOC = "GIBI_MESH_RDWR_DRIVER::copy()";
  BEGIN_OF_MED(LOC);
  GENDRIVER * driver = new GIBI_MESH_RDWR_DRIVER(*this);
  END_OF_MED(LOC);
  return driver;
}
void GIBI_MESH_RDWR_DRIVER::write(void) const
  throw (MEDEXCEPTION)
{
  GIBI_MESH_RDWR_DRIVER * me = const_cast<GIBI_MESH_RDWR_DRIVER *>(this);
  me->GIBI_MESH_WRONLY_DRIVER::open();
  me->GIBI_MESH_WRONLY_DRIVER::write();
  me->GIBI_MESH_WRONLY_DRIVER::close();
}
void GIBI_MESH_RDWR_DRIVER::read (void)
  throw (MEDEXCEPTION)
{
  const char* LOC = "GIBI_MESH_RDWR_DRIVER::read()";
  BEGIN_OF_MED(LOC);
  GIBI_MESH_RDONLY_DRIVER::open();
  GIBI_MESH_RDONLY_DRIVER::read();
  GIBI_MESH_RDONLY_DRIVER::close();
  END_OF_MED(LOC);
}
void GIBI_MESH_RDWR_DRIVER::open()
  // throw (MEDEXCEPTION)
{
}
void GIBI_MESH_RDWR_DRIVER::close()
  // throw (MEDEXCEPTION)
{
}

//============================== ====================================================
//============================== FIELD Reading Driver ==============================
//============================== ====================================================

GIBI_MED_RDONLY_DRIVER::GIBI_MED_RDONLY_DRIVER():GIBI_MESH_RDONLY_DRIVER()
{
}
GIBI_MED_RDONLY_DRIVER::GIBI_MED_RDONLY_DRIVER(const string & fileName, MED * ptrMed):
       GIBI_MESH_RDONLY_DRIVER(fileName,NULL), _med( ptrMed )
{
  MESSAGE_MED("GIBI_MED_RDONLY_DRIVER(const string & fileName, MED * ptrMed) has been created");
  _fileName = fileName;
  _accessMode = RDONLY;
}
GIBI_MED_RDONLY_DRIVER::GIBI_MED_RDONLY_DRIVER(const GIBI_MED_RDONLY_DRIVER & driver)
{
}
 GIBI_MED_RDONLY_DRIVER::~GIBI_MED_RDONLY_DRIVER()
{
}
GENDRIVER * GIBI_MED_RDONLY_DRIVER::copy ( void ) const
{
  return new GIBI_MED_RDONLY_DRIVER(*this);
}

//=======================================================================
//function : read
//purpose  :
//=======================================================================

void GIBI_MED_RDONLY_DRIVER::read ( void ) throw (MEDEXCEPTION)
{
  const char * LOC = "GIBI_MED_RDONLY_DRIVER::read() : ";
  BEGIN_OF_MED(LOC);

  if (_status!=MED_OPENED)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "file " << _fileName<<" is not opened." ));

  _ptrMesh = new MESH();

  _intermediateMED medi;
  try {
    if ( !readFile( &medi, true ) )
      return;

    //MESSAGE_MED(LOC <<  medi );
    fillMesh( &medi );
    MESSAGE_MED(LOC << "GIBI_MED_RDONLY_DRIVER::read : RESULTATS STRUCTURE INTERMEDIAIRES : ");
    MESSAGE_MED(LOC <<  medi );

    list< FIELD_* > fields;
    medi.getFields( fields );
    updateSupports(); // create families from groups etc.
    MESSAGE_MED( "nb fields: " << fields.size() );

    if ( _ptrMesh->getName().empty() )
      _ptrMesh->setName( "MESH" );

    _med->addMesh( _ptrMesh );

    list< FIELD_* >::iterator it = fields.begin();
    for ( ; it != fields.end(); it++ )
    {
      FIELD_* fld = *it;
      int nbComponents = fld->getNumberOfComponents();
      if(nbComponents>0) {
        UNIT* compoUnits = new UNIT[nbComponents];
        string* MEDcompoUnits = new string[nbComponents];
        for(int l = 0; l<nbComponents; l++) {
          compoUnits[l] = UNIT("", "");
          MEDcompoUnits[l] = "";
        }
        fld->setComponentsUnits(compoUnits);
        fld->setMEDComponentsUnits(MEDcompoUnits);
      }
      // 0020466: [CEA] sauv2med : bad conversion
      // Provide profile names for a partial field
      const SUPPORT* sup = fld->getSupport();
      if ( sup && !sup->isOnAllElements() )
      {
        vector<string> prof_names( sup->getNumberOfTypes() );
        for (int itype=0; itype < prof_names.size(); itype++)
          prof_names[itype]=STRING( sup->getName())<<"_type_"<<sup->getTypes()[itype];
        ((SUPPORT*) sup)->setProfilNames( prof_names );
      }
      _med->addField( *it );
    }
  }
  catch (MEDEXCEPTION &ex)
  {
    INFOS_MED( ex.what() );
  }

  END_OF_MED(LOC);
}

//============================== ====================================================
//============================== FIELD Writting Driver ==============================
//============================== ====================================================

GIBI_MED_WRONLY_DRIVER::GIBI_MED_WRONLY_DRIVER():GIBI_MESH_WRONLY_DRIVER()
{
}
GIBI_MED_WRONLY_DRIVER::GIBI_MED_WRONLY_DRIVER(const string & fileName,
                                                   MED * ptrMed,
                                                   MESH * ptrMesh)
     :GIBI_MESH_WRONLY_DRIVER(fileName,ptrMesh), _med( ptrMed )
{
  const char * LOC =
    "GIBI_MED_WRONLY_DRIVER(const string & fileName, MED * ptrMed, MESH * ptrMesh)";
  BEGIN_OF_MED(LOC);

  _fileName = fileName;
  _accessMode = WRONLY;
  _ptrMesh = ptrMesh;
  if ( !_med || !_ptrMesh )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << " Bad params " << ptrMed << " " << ptrMesh ));
}
GIBI_MED_WRONLY_DRIVER::GIBI_MED_WRONLY_DRIVER(const GIBI_MED_WRONLY_DRIVER & driver)
{
}
GIBI_MED_WRONLY_DRIVER::~GIBI_MED_WRONLY_DRIVER()
{
}
GENDRIVER * GIBI_MED_WRONLY_DRIVER::copy ( void ) const
{
  return new GIBI_MED_WRONLY_DRIVER(*this);
}

//=======================================================================
//function : writeDataSection
//purpose  :
//=======================================================================

template< class T, class INTERLACING_TAG>
static void writeDataSection (fstream&                    file,
                              FIELD<T, INTERLACING_TAG> * field,
                              const int                   id1,
                              const int                   id2) throw (MEDEXCEPTION)
{
  const char * LOC="writeDataSection (.....) :";
  BEGIN_OF_MED(LOC);

  int nbGauss, nbComp = field->getNumberOfComponents();

  typedef typename MEDMEM_ArrayInterface<T,INTERLACING_TAG,NoGauss>::Array ArrayNoGauss;
  typedef typename MEDMEM_ArrayInterface<T,INTERLACING_TAG,Gauss>::Array   ArrayGauss;

  MEDMEM_Array_ * array        = field->getArray();
  ArrayNoGauss  * arrayNoGauss = 0;
  ArrayGauss    * arrayGauss   = 0;

  if ( !array )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Field |"<< field->getName()
                                 << "| not allocated"));

  if ( array->getGaussPresence() ) {
    arrayGauss = field->getArrayGauss();
    nbGauss    = arrayGauss->getNbGauss( id1 );
  }
  else {
    arrayNoGauss = field->getArrayNoGauss();
    nbGauss      = 1;
  }

  TFieldCounter fcount( file, 3 ); // 3 values on a line

#ifdef CASTEM_FULL_INTERLACE
  const int gauss_step = field->getInterlacingType() == MED_EN::MED_FULL_INTERLACE ? nbComp : 1;
#endif

  for ( int iComp = 1; iComp <= nbComp; ++iComp )
  {
    file << setw(8) << nbGauss       // nb scalar values by element
         << setw(8) << ( id2 - id1 ) // total nb of scalar values
         << setw(8) << 0
         << setw(8) << 0
         << endl;
    // * 8003   FORMAT(1P,3E22.14)
    if ( arrayNoGauss ) {
      for (int id = id1; id < id2; id++, fcount++ )
        file << setw(22) << arrayNoGauss->getIJ( id, iComp );
    }
    else {
#ifdef CASTEM_FULL_INTERLACE
      for (int id = id1; id < id2; id++ ) {
        const T* val = & arrayGauss->getIJK( id, iComp, 1 );
        const T* valEnd = val + nbGauss * gauss_step;
        for ( ; val < valEnd; val += gauss_step, fcount++ )
          file << setw(22) << *val;
      }
#else
      for ( int iGauss = 1; iGauss <= nbGauss; ++iGauss ) {
        for (int id = id1; id < id2; id++, fcount++  )
          file << setw(22) << arrayGauss->getIJK( id, iComp, iGauss );
      }
#endif
    }
    fcount.stop();
  }
  END_OF_MED(LOC);
}

//=======================================================================
//function : write
//purpose  :
//=======================================================================

void GIBI_MED_WRONLY_DRIVER::write( void ) const throw (MEDEXCEPTION)
{
  const char* LOC = "void GIBI_MED_WRONLY_DRIVER::write(void) const : ";
  BEGIN_OF_MED(LOC);

  // we are going to modify the _gibi field
  GIBI_MED_WRONLY_DRIVER * me = const_cast<GIBI_MED_WRONLY_DRIVER *>(this);

  // get all fields on _ptrMesh and add their support to be written
  list<FIELD_*> fields;
  int iField, nbFields = _med->getNumberOfFields();
  int nb_obj = 0;
  list<int> nb_sub_list;
  map<string,int> nameNbMap;
  map<string,int> namePrefixMap;

  list<pair<int,int> >           subIdSizeList; // pair( <submesh id>, <submesh size> );
  list<pair<int,int> >::iterator idsize;

  // IMP 0020434: mapping GIBI names to MED names
  list<nameGIBItoMED> listGIBItoMED_mail;
  list<nameGIBItoMED> listGIBItoMED_cham;
  list<nameGIBItoMED> listGIBItoMED_comp;

  string *names=new string[ nbFields ];
  _med->getFieldNames( names );
  for ( iField = 0; iField < nbFields; ++iField )
  {
    int nb_sub = 0;
    deque<DT_IT_> dtit = _med->getFieldIteration( names[ iField ]);
    deque<DT_IT_>::iterator fIt = dtit.begin();
    for ( ; fIt != dtit.end(); fIt++ )
    {
      FIELD_ * f = _med->getField( names[ iField ], fIt->dt, fIt->it );
      if ( f->getValueType() != MED_EN::MED_REEL64 )
      {
        MESSAGE_MED("GIBI_MED_WRONLY_DRIVER::write( FIELD< int > ) not implemented");
        continue;
      }
      const SUPPORT * sup = f->getSupport();
      if ( me->addSupport( sup ) ) {
        fields.push_back( f );
        nb_sub += getSubMeshIdAndSize( sup, subIdSizeList );
      }
    }
    if ( nb_sub ) {
      //addName( nameNbMap, names[ iField ], ++nb_obj, "F" );
      addName(nameNbMap, namePrefixMap, names[ iField ], ++nb_obj);

      // IMP 0020434: mapping GIBI names to MED names
      nameGIBItoMED aMEDName;
      aMEDName.gibi_pile = 39; // PILE_FIELD
      aMEDName.gibi_id = nb_obj;
      aMEDName.med_name = names[ iField ];
      listGIBItoMED_cham.push_back(aMEDName);

      nb_sub_list.push_back( nb_sub );
    }
  }

  // write mesh

  //try {
  me->writeSupportsAndMesh(listGIBItoMED_mail);
//   }
//   catch (MEDEXCEPTION &ex)
//   {
//     INFOS_MED( ex.what() );
//     return;
//   }

  // write fields

  if ( !fields.empty() ) {

    fstream & gibi = me->_gibi;

    TFieldCounter fcount( gibi, 10 );

    gibi << " ENREGISTREMENT DE TYPE   2" << endl;
    gibi << " PILE NUMERO  39NBRE OBJETS NOMMES" << setw(8) << nameNbMap.size()
      << "NBRE OBJETS" << setw(8) << nb_obj << endl;

    me->writeNames( nameNbMap );

    list<FIELD_*>::iterator itF = fields.begin();
    list<int>::iterator itNbSub = nb_sub_list.begin();
    int nb_sub = 0, cur_nb_sub = 0;
    for ( iField = 0; itF != fields.end(); itF++, iField++ )
    {
      if ( cur_nb_sub == nb_sub && itNbSub != nb_sub_list.end() ) {
        // start the next field writting
        nb_sub = *(itNbSub++);
        string description = (*itF)->getDescription();
        if ( description.size() > 72 )
          description = description.substr(0,72);
        gibi << setw(8) << nb_sub
             << setw(8) << -1
             << setw(8) << 6
             << setw(8) << 72 /*description.size()*/ << endl; // PAL19100
        if ( !description.empty() )
          gibi << setw(72) << description << endl;
        else
          gibi << setw(72) << "Field" << endl;
        gibi << setw(72) << " " << endl;

        // Sub Components section
        list<FIELD_*>::iterator itF2 = itF;
        vector<int> vals( 9, 0 );
        vals[8] = 2;
        fcount.init(10);
        cur_nb_sub = 0;
        while ( itF2 != fields.end() && cur_nb_sub < nb_sub )
        {
          FIELD_* f = *itF2++;
          vals[2] = f->getNumberOfComponents();
          getSubMeshIdAndSize( f->getSupport(), subIdSizeList );
          for ( idsize = subIdSizeList.begin(); idsize != subIdSizeList.end(); idsize++ )
          {
            ++cur_nb_sub;
            vals[0] = -idsize->first; // support id
            for ( int i = 0; i < (int)vals.size(); ++i, fcount++ )
              gibi << setw(8) << vals[ i ];
          }
        }
        fcount.stop();
        cur_nb_sub = 0;
        // dummy strings
        int i_sub;
        for ( fcount.init(4), i_sub = 0; i_sub < nb_sub; ++i_sub, fcount++ )
          gibi << "                  ";
        fcount.stop();
        for ( fcount.init(8), i_sub = 0; i_sub < nb_sub; ++i_sub, fcount++ )
          gibi << "         ";
        fcount.stop();
      }

      FIELD_* f = *itF;
      int id1 = 1;
      int iComp, nbComp = f->getNumberOfComponents();

      // IMP 0020434: mapping GIBI names to MED names
      int compIndex = 1;
      map<string, string> mapMedToGibi;
      for (int ico = 0; ico < nbComp; ico++) {
        string compMedName = f->getComponentName(ico + 1);
        compMedName = cleanName(compMedName);
        mapMedToGibi[compMedName] = compMedName;
      }
      map<string, string>::iterator namesIt = mapMedToGibi.begin();
      for (; namesIt != mapMedToGibi.end(); namesIt++) {
        string compMedName = (*namesIt).first;
        string compGibiName = compMedName;
        if (compGibiName.size() > 4) {
          // use new name in form "CXXX", where "XXX" is a number
          do {
            char strCGN [32];
            strCGN[0] = 'C';
            int pos = 1;
            if (compIndex < 100) strCGN[pos++] = '0';
            if (compIndex < 10 ) strCGN[pos++] = '0';
            sprintf(strCGN + pos, "%d", compIndex++);
            compGibiName = strCGN;
          } while (mapMedToGibi.count(compGibiName) > 0); // real component name could be CXXX

          mapMedToGibi[compMedName] = compGibiName;
        }

        compMedName = names[iField] + "." + compMedName;
        nameGIBItoMED aMEDName;
        aMEDName.med_name = compMedName;
        aMEDName.gibi_pile = 27; // PILE_STRINGS
        aMEDName.gibi_name = compGibiName;
        listGIBItoMED_comp.push_back(aMEDName);
      } // IMP 0020434

      // loop on sub-components
      getSubMeshIdAndSize( f->getSupport(), subIdSizeList );
      for ( idsize = subIdSizeList.begin(); idsize != subIdSizeList.end(); idsize++ )
      {
        cur_nb_sub++;
        // component addresses
        for ( fcount.init(10), iComp = 0; iComp < nbComp; ++iComp, fcount++ )
          gibi << setw(8) << 777; // a good number
        fcount.stop();
        // component names
        gibi << left;
        for ( fcount.init(8), iComp = 0; iComp < nbComp; ++iComp, fcount++ ) {
          string compMedName = cleanName(f->getComponentName(iComp + 1));
          string compName = mapMedToGibi[compMedName];
          //if ( compName.size() > 8 )
          //  compName = compName.substr(0, 8);
          gibi << " "  << setw(8) << compName;
        }
        fcount.stop();
        // component types
        for ( fcount.init(4), iComp = 0; iComp < nbComp; ++iComp, fcount++ )
          gibi << " "  << setw(17) << "REAL*8";
        fcount.stop();
        gibi << right;

        // Data section
        int id2 = id1 + idsize->second;

        // PAL11040
        //if  (f->getGaussPresence() )
        //throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "GibiDriver don't support Field with Gauss point"));

        if ( f->getInterlacingType() == MED_NO_INTERLACE )
          writeDataSection( gibi, static_cast<FIELD<double,NoInterlace  > * >(f), id1, id2 );
        else if ( f->getInterlacingType() == MED_FULL_INTERLACE )
          writeDataSection( gibi, static_cast<FIELD<double,FullInterlace> * >(f), id1, id2 );
        else
          writeDataSection( gibi, static_cast<FIELD<double,NoInterlaceByType> * >(f), id1, id2 );

        id1 = id2;
      }
    } // loop on fields
  }

  // IMP 0020434: mapping GIBI names to MED names
  me->writeMEDNames(listGIBItoMED_mail, listGIBItoMED_cham, listGIBItoMED_comp);

  me->writeLastRecord();
  delete [] names;
  END_OF_MED(LOC);
}
