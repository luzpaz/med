#include <algorithm>
#include <queue>

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


// Every memory allocation made in the MedDriver members function are desallocated in the Mesh destructor 

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
    group_id = support_ids[0] - 1;
  else {
    // try to find an existing group with the same sub-groups
    set<int> sup_set;
    for ( ; sb != se; sb++ )
      sup_set.insert( *sb );
    for ( group_id = 0; group_id < medi->groupes.size(); ++group_id ) {
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
//function : read
//purpose  : 
//=======================================================================

#define GIBI_EQUAL(var_str, stat_str) \
  (strncmp (var_str, stat_str, sizeof(stat_str)-1) == 0)
#define DUMP_LINE_NB " on line " << _lineNb

bool GIBI_MESH_RDONLY_DRIVER::readFile (_intermediateMED* medi, bool readFields )
{
  const char * LOC = "GIBI_MESH_RDONLY_DRIVER::readFile() : " ;
  BEGIN_OF(LOC);

  // LECTURE DES DONNEES DS FICHIER GIBI

  enum Readable_Piles {
    PILE_SOUS_MAILLAGE=1,
    PILE_NODES_FIELD  =2,
    PILE_NOEUDS       =32,
    PILE_COORDONNEES  =33,
    PILE_FIELD        =39,
    PILE_LAST_READABLE=39
    };
  Readable_Piles readable_Piles [] = {
    PILE_SOUS_MAILLAGE,
    PILE_NODES_FIELD,
    PILE_NOEUDS,
    PILE_COORDONNEES,
    PILE_FIELD,
    PILE_LAST_READABLE
    };
  char* ligne; // pour lire une ligne
  const char* enregistrement_type=" ENREGISTREMENT DE TYPE";
  vector<int> numero_noeuds; // tableau de travail (indices)
  set<int> donePiles; // already read piles
  unsigned space_dimension = 0;

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
        INFOS( " Could not read file: syntax error in type 4 record");
        return false;
      }
    }
    else if (numero_enregistrement == ENREG_TYPE_2 )
    {
      if ( space_dimension == 0 ) {
        INFOS( "Missing ENREGISTREMENT DE TYPE   4");
        return false;
      }
      // FORMAT(' PILE NUMERO',I4,'NBRE OBJETS NOMMES',I8,'NBRE OBJETS',I8)
      getNextLine(ligne);
      const char *s1 = " PILE NUMERO", *s2 = "NBRE OBJETS NOMMES", *s3 = "NBRE OBJETS";
      if ( ! GIBI_EQUAL( ligne, s1 ) ) {
        INFOS( " Could not read file: error in type 2 record. " << ligne);
        return false;
      }
      ligne = ligne + strlen(s1);
      numero_pile = atoi( ligne );
      ligne = ligne + 4 + strlen(s2);
      nb_objets_nommes = atoi( ligne );
      ligne = ligne + 8 + strlen(s3);
      nb_objets = atoi( ligne );
      if ( nb_objets_nommes<0 || nb_objets<0  ) {
        INFOS(" Could not read file: " << nb_objets << " " <<nb_objets_nommes);
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
      for ( initNameReading( nb_objets_nommes ); more(); next() ) {
        objets_nommes[ index() ] = getName();
      }
      for ( initIntReading( nb_objets_nommes ); more(); next() )
        indices_objets_nommes[ index() ] = getInt();

      // boucle interne : lecture de la pile

      MESSAGE(LOC << "---- Traitement pile " << numero_pile);

      // -----------------------------------
      //                        MESH GROUPS
      // -----------------------------------

      if (numero_pile == PILE_SOUS_MAILLAGE )
      {
        map<int,int> strangeGroupType;
        medi->groupes.reserve(nb_objets*2); // fields may add some groups
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
            INFOS(" Error while reading file, bad geometric type:" << type_geom_castem);
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
          for ( i = 0; i < nb_reference; i += 10 ) {// FORMAT(10I8)
            getNextLine(ligne);
          }
          // lecture des couleurs (non utilisé pour MED)
          for ( i = 0; i < nb_elements; i += 10 ) {
            getNextLine(ligne);
          }
          // not a composit group
          if (type_geom_castem>0 && nb_sous_maillage==0) 
          {
            medGeometryElement medType = gibi2medGeom(type_geom_castem);
            bool goodType = ( medType!=MED_NONE );
            if ( !goodType )
              strangeGroupType.insert( make_pair( objet, type_geom_castem ));

            pair<set<_maille>::iterator,bool> p;
            pair<map<int,_noeud>::iterator,bool> p_no;
            _noeud no;
            no.coord.resize(space_dimension);
            _maille ma( medType, nb_noeud );
            ma.sommets.resize(nb_noeud);
            if ( goodType )
              groupe.mailles.resize( nb_elements );

            // lecture pour chaque maille des sommets et insertions
            initIntReading( nb_elements * nb_noeud );
            if ( !goodType ) {
              while ( more() )
                next();
            }
            else {
              for ( i = 0; i < nb_elements; ++i )
              {
                for (unsigned n = 0; n < nb_noeud; ++n, next() )
                {
                  if ( !more() ) {
                    INFOS( " Error while reading elem nodes ");
                    return false;
                  }
                  no.number = getInt();
                  p_no=medi->points.insert(make_pair(no.number, no));
                  ma.sommets[n]=p_no.first;
                }
                p=medi->maillage.insert(ma);
                groupe.mailles[i] = p.first; // on stocke dans le groupe un iterateur sur la maille
              }
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
            //INFOS( "Skip " << grp.nom << " of not supported CASTEM type: " << it->second );
          }
        }

      }// Fin case PILE_SOUS_MAILLAGE

      // ---------------------------------
      //                            NODES
      // ---------------------------------

      else if ( numero_pile == PILE_NOEUDS )
      {
        getNextLine( ligne );
        std::vector<int> place_noeuds;
        nb_indices = atoi ( ligne );
        if (nb_indices != nb_objets)
        {
          INFOS("Erreur de lecture dans enregistrement de pile " << PILE_NOEUDS);
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

      else if ( numero_pile == PILE_COORDONNEES )
      {
        getNextLine( ligne );
        unsigned nb_reels = atoi( ligne );
        // PROVISOIRE : certains fichier gibi n`ont 
        if (nb_reels < numero_noeuds.size()*(space_dimension)) {
          INFOS("Erreur de lecture dans enregistrement de pile " << PILE_COORDONNEES);
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

      else if ( numero_pile == PILE_NODES_FIELD && readFields )
      {
        vector< _fieldBase* > fields( nb_objets );
        for (int objet=0; objet!=nb_objets; ++objet) // pour chaque field
        {
          initIntReading( 4 ); // nb subcomponents, nb components, IFOUR, nb attributes
          int i_sub,  nb_sub  = getInt(); next();
          int i_comp, nb_comp = getInt(); next(); next();
          int nb_attr = getInt();
          if ( nb_sub < 0 || nb_comp < 0 ) {
            INFOS("Error of field reading: wrong nb of components "
                  << nb_sub << " " << nb_comp << DUMP_LINE_NB);
            return false;
          }
          _field<double>* fdouble = 0;
          if ( nb_sub > 0 && nb_comp > 0 ) {
            fdouble = new _field<double>(MED_REEL64);
            medi->fields.push_back( fdouble );
            fdouble->nb_subcomponents = nb_sub;
            fdouble->nb_components    = nb_comp;
            fdouble->comp_names.resize( nb_comp );
          }
          fields[ objet ] = fdouble;

          // loop on subcomponents of a field, each of which refers to
          // a certain support and has the same number of components
          vector<int>     support_ids( nb_sub );
          vector<int>     nb_values( nb_sub );
          vector<string>  comp_names( nb_comp );
          initIntReading( nb_sub * 3 );
          for ( i_sub = 0; i_sub < nb_sub; ++i_sub )
          {
            support_ids[ i_sub ] = -getInt(); next(); // reference to support
            if ( support_ids[ i_sub ] < 1 || support_ids[ i_sub ] > medi->groupes.size() ) {
              INFOS("Error of field reading: wrong mesh reference "<< support_ids[ i_sub ]);
              return false;
            }
            nb_values[ i_sub ] = getInt(); next();    // nb points
            if ( nb_values[ i_sub ] < 0 ) {
              INFOS(" Wrong nb of points: " << nb_values[ i_sub ] );
              return false;
            }
            next(); // skip nb of comp names equal to nb_comp
          }
          initNameReading( nb_sub * nb_comp, 4 );
          for ( i_sub = 0; i_sub < nb_sub; ++i_sub )
          {
            for ( i_comp = 0; i_comp < nb_comp; ++i_comp, next() ) {
              ASSERT( more() );
              fdouble->comp_names[ i_comp ] = getName();// component names
            }
          }
          
          for ( initIntReading( nb_sub ); more(); next() )// nb harmonics ( ignored )
            ;

          getNextLine( ligne );                             // TYPE ( ignored )
          getNextLine( ligne );                             // TITRE ( ignored )
          for ( initIntReading( nb_attr ); more(); next() ) // attributes ( ignored )
            ;

          for ( i_sub = 0; i_sub < nb_sub; ++i_sub )
          {
            // loop on components: read values
            initDoubleReading( nb_values[ i_sub ] * nb_comp );
            for ( i_comp = 0; i_comp < nb_comp; ++i_comp ) {
              vector<double> & vals =
                fdouble->addComponent( support_ids[ i_sub ], nb_values[ i_sub ] );
              for ( int i = 0; more() && i < nb_values[ i_sub ]; next(), ++i ) {
                vals[ i ] = getDouble();
              }
            }
          } // loop on subcomponents of a field

          // supporting group id
          if ( fdouble )
            fdouble->group_id = getGroupId( support_ids, medi );

        } // end loop on field objects

        // set field names
        for ( i = 0; i < nb_objets_nommes; ++i ) {
          int fieldIndex = indices_objets_nommes[ i ];
          if ( fields[ fieldIndex - 1 ] )
            fields[ fieldIndex - 1 ]->name = objets_nommes[ i ];
        }

      }  // Fin numero_pile == PILE_NODES_FIELD
      
      // -------------------------------------------------
      //                                           FIELDS
      // -------------------------------------------------

      else if ( numero_pile == PILE_FIELD && readFields )
      {
        vector< _fieldBase* > fields( nb_objets, (_fieldBase*)NULL );
        for (int objet=0; objet!=nb_objets; ++objet) // pour chaque field
        {
          initIntReading( 1 );
          int i_sub, nb_sub = getInt(); // <nb_sub> -1 6 <title length>
          if ( nb_sub < 1 ) {
            INFOS("Error of field reading: wrong nb of subcomponents " << nb_sub);
            return false;
          }
          getNextLine( ligne ); // title
          // look for a line starting with '-' : <reference to support>
          do {
            initIntReading( nb_sub * 9 );
          } while ( getInt() >= 0 );

          int i_comp, nb_comp = 0;
          vector<int> support_ids( nb_sub );
          for ( i_sub = 0; i_sub < nb_sub; ++i_sub )
          {
            support_ids[ i_sub ] = -getInt(); next(); // <reference to support>
            next();                                   // skip <address>
            nb_comp = getInt(); next();               // <nb of components in the field>
            for ( i = 0; i < 6; ++i )                 //  0 0 0 -1 0 2
              next();
            if ( support_ids[ i_sub ] < 1 || support_ids[ i_sub ] > medi->groupes.size() ) {
              INFOS("Error of field reading: wrong mesh reference "<< support_ids[ i_sub ]);
              return false;
            }
            if ( nb_comp < 1 ) {
              INFOS("Error of field reading: wrong nb of components " << nb_comp);
              return false;
            }
          }
          for ( initNameReading( nb_sub, 17 ); more(); next() )
            ; // dummy strings
          for ( initNameReading( nb_sub ); more(); next() )
            ; // dummy strings

          // loop on subcomponents of a field, each of which refers to
          // a certain support and has the same number of components
          _field<double>* fdouble = 0;
          _field<int>*    fint    = 0;
          vector<string> comp_names( nb_comp ), comp_type( nb_comp );
          for ( i_sub = 0; i_sub < nb_sub; ++ i_sub )
          {
            getNextLine( ligne );  // skip nb_comp addresses of MELVAL structure

            // component names
            for ( initNameReading( nb_comp ); more(); next() )
              comp_names[ index() ] = getName();

            // component type
            for ( initNameReading( nb_comp, 17 ); more(); next() ) { // 17 is name width
              comp_type[ index() ] = getName();
              // component types must be the same
              if ( index() > 0 && comp_type[ index() ] != comp_type[ index() - 1] ) {
                INFOS( "Error of field reading: diff component types <"
                      << comp_type[ index() ] << "> != <" << comp_type[ index() - 1 ] << ">");
                return false;
              }
            }
            // now type is known, create a field
            bool isReal = ( comp_type[0] == "REAL*8" );
            if ( !fdouble && !fint ) {
              _fieldBase * fbase;
              if ( !isReal ) {
                fbase = fint = new _field<int>(MED_INT32);
                INFOS( "Warning: read NOT REAL field, type <" << comp_type[0] << ">"
                      << DUMP_LINE_NB);
              }
              else
                fbase = fdouble = new _field<double>(MED_REEL64);
              medi->fields.push_back( fbase ); // medi->fields is a std::list
              fields[ objet ]         = fbase;
              fbase->nb_subcomponents = nb_sub;
              fbase->nb_components    = nb_comp;
              fbase->comp_names       = comp_names;
              fbase->group_id         = getGroupId( support_ids, medi );
            }

            // loop on components: read values
            for ( i_comp = 0; i_comp < nb_comp; ++i_comp ) {
              // nb of values
              initIntReading( 2 );
              int nb_val_by_elem = getInt(); next();
              int nb_values      = getInt();
              if ( nb_val_by_elem != 1 ) {
                INFOS("Error of field reading: nb of values by element "
                      << " != 1 : " << nb_val_by_elem << DUMP_LINE_NB );
                return false;
              }
              if ( isReal ) {
                vector<double> & vals =
                  fdouble->addComponent( support_ids[ i_sub ], nb_values );
                for ( initDoubleReading( nb_values ); more(); next()) {
                  vals[ index() ] = getDouble();
                }
              }
              else {
                vector<int> & vals =
                  fint->addComponent( support_ids[ i_sub ], nb_values );
                for ( initIntReading( nb_values ); more(); next() ) {
                  vals[ index() ] = getInt();
                }
              }
            }
          } // loop on subcomponents of a field

        } // end loop on field objects

        // set field names
        for ( i = 0; i < nb_objets_nommes; ++i ) {
          int fieldIndex = indices_objets_nommes[ i ];
          fields[ fieldIndex - 1 ]->name = objets_nommes[ i ];
        }

      } // numero_pile == PILE_FIELD && readFields

      else if ( numero_pile >= PILE_LAST_READABLE )
        break; // stop file reading

    } // Fin case ENREG_TYPE_2
  } //  fin de la boucle while de lecture externe

  // check if all needed piles present
  if ( donePiles.find( PILE_SOUS_MAILLAGE ) != donePiles.end() )
  {
    if (donePiles.find( PILE_NOEUDS ) == donePiles.end() ) {
      INFOS( " Missing pile " << PILE_NOEUDS );
      return false;
    }
    if (donePiles.find( PILE_COORDONNEES ) == donePiles.end()) {
      INFOS( " Missing pile " << PILE_COORDONNEES );
      return false;
    }
  }

  END_OF(LOC);
  return true;
}

GIBI_MESH_DRIVER::GIBI_MESH_DRIVER():
       GENDRIVER(),
       _ptrMesh(( MESH *)MED_NULL),
       // A VOIR _medIdt(MED_INVALID),
       _meshName("")
{
  MESSAGE("GIBI_MESH_DRIVER()");
}

GIBI_MESH_DRIVER::GIBI_MESH_DRIVER(const string & fileName,
                                   MESH * ptrMesh,
                                   MED_EN::med_mode_acces accessMode): 
  GENDRIVER(fileName,accessMode),
  _ptrMesh(ptrMesh)
  // A VOIR _medIdt(MED_INVALID), 
{
  MESSAGE( "GIBI_MESH_DRIVER(" << fileName <<","<<accessMode );
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
  MESSAGE("GIBI_MESH_DRIVER(const GIBI_MESH_DRIVER & driver)");
}

GIBI_MESH_DRIVER::~GIBI_MESH_DRIVER()
{
  MESSAGE("~GIBI_MESH_DRIVER()");
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
       GIBI_MESH_DRIVER(fileName,ptrMesh,MED_RDONLY),
       _File (-1),_start(0L),_ptr  (0L),_eptr (0L)
{ 
    MESSAGE("GIBI_MESH_RDONLY_DRIVER::GIBI_MESH_RDONLY_DRIVER"
            "(const string & fileName, MESH * ptrMesh) has been created, "
            << fileName << ", " << MED_RDONLY);
}
GIBI_MESH_RDONLY_DRIVER::GIBI_MESH_RDONLY_DRIVER(const GIBI_MESH_RDONLY_DRIVER & driver): 
GIBI_MESH_DRIVER(driver)
{
}
GIBI_MESH_RDONLY_DRIVER::~GIBI_MESH_RDONLY_DRIVER()
{
  BEGIN_OF( "~GIBI_MESH_RDONLY_DRIVER()");
  if (_File >= 0)
  {
    ::close (_File);
    if (_start != 0L)
      delete [] _start;
  }
  MESSAGE("GIBI_MESH_RDONLY_DRIVER::~GIBI_MESH_RDONLY_DRIVER() has been destroyed");
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
  const char * LOC = "GIBI_MESH_RDONLY_DRIVER::open()" ;
  BEGIN_OF(LOC);

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
  END_OF(LOC);
}

//=======================================================================
//function : close
//purpose  : 
//=======================================================================

void GIBI_MESH_RDONLY_DRIVER::close()
  //  throw (MEDEXCEPTION)
{
    const char * LOC = "GIBI_MESH_DRIVER::close() " ;
    BEGIN_OF(LOC);
    if ( _status == MED_OPENED) 
    {
      if (_File >= 0)
      {
        ::close (_File);
        if (_start != 0L)
          delete [] _start;
        _File = -1;
      }
      _status = MED_CLOSED;
    }
    END_OF(LOC);
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
      memcpy (_start, _ptr, nBytesRest);
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
//function : read
//purpose  :
//=======================================================================

void GIBI_MESH_RDONLY_DRIVER::read(void) throw (MEDEXCEPTION)
{
  const char * LOC = "_GIBI_RDONLY_DRIVER::read() : " ;

  if (_status!=MED_OPENED)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "The _idt of file " << _fileName << " is : "  
                                 <<  " (the file is not opened)." )) ;
  if ( ! _ptrMesh->isEmpty() )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Mesh object not empty : can't fill it!"));

  _intermediateMED medi;
  try {
    if ( readFile( &medi, false )) {
      // impression résultats
      MESSAGE(LOC << "GIBI_MESH_RDONLY_DRIVER::read : RESULTATS STRUCTURE INTERMEDIAIRES : ");
      MESSAGE(LOC <<  medi );

      fillMesh( &medi );
    }
  }
  catch (MEDEXCEPTION &ex)
  {
    INFOS( ex.what() );
  }

}

//=======================================================================
//function : getReverseVector
//purpose  : 
//=======================================================================

static void getReverseVector (const medGeometryElement type,
                              vector<pair<int,int> > & swapVec )
{
  BEGIN_OF("void getReverseVector()");
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
  default:;
  }
  END_OF("void getReverseVector()");
}

//=======================================================================
//function : orientElements
//purpose  : 
//=======================================================================

static void orientElements( _intermediateMED& medi )
{
  MESSAGE("orientElements()");
  set<_maille>::iterator elemIt = medi.maillage.begin();

  if ( elemIt->sommets[0]->second.coord.size() == 2 ) { // space dimension

    // --------------------------
    // Orient 2D faces clockwise
    // --------------------------

    for ( ; elemIt != medi.maillage.end(); elemIt++ )
      if ( elemIt->dimension() == 2 )
      {
        // look for index of the most left node 
        int iLeft = 0, iNode, nbNodes = elemIt->sommets.size();
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
          elemIt->reverse = ( !clockwise );
        }
      }
  }
  else {

    int type = -100;
    vector< pair<int,int> > swapVec;
    for ( ; elemIt != medi.maillage.end(); elemIt++ ) {
      if ( elemIt->dimension() == 3 )
      {
        // ---------------------------------------------------
        // Orient volumes according to MED conventions:
        // normal of a bottom (first) face should be downward
        // ---------------------------------------------------

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
        bool reverse = ( dot < 0. );
        if ( reverse ) {
          if ( elemIt->geometricType != type ) {
            type = elemIt->geometricType;
            getReverseVector( type, swapVec );
//             INFOS("vec01: " <<vec01[0] << " " <<vec01[1] << " " << vec01[2]);
//             INFOS("vec02: " <<vec02[0] << " " <<vec02[1] << " " << vec02[2]);
//             INFOS("normal: " <<normal[0] << " " <<normal[1] << " " << normal[2]);
//             INFOS("tb: " << tbDir[0] << " " <<tbDir[1] << " " << tbDir[2]);
//             INFOS( *elemIt );
//             for ( vector< _maille::iter >::const_iterator si = elemIt->sommets.begin();
//                  si != elemIt->sommets.end(); si++ )
//               INFOS( (*si)->second );
          }
          _maille* ma = (_maille*) & (*elemIt);
          for ( int i = 0; i < swapVec.size(); ++i ) {
            _maille::iter tmp = ma->sommets[ swapVec[i].first ];
            ma->sommets[ swapVec[i].first ] = ma->sommets[ swapVec[i].second ];
            ma->sommets[ swapVec[i].second ] = tmp;
          }
        }
      } // dimension() == 3
    } // loop on maillage
    
    // --------------------------------------
    // orient equally all connected 3D faces
    // --------------------------------------

    // fill map of links and their faces
    set<const _maille*> faces;
    map<const _maille*, _groupe*> fgm;
    map<_link, list<const _maille*> > linkFacesMap;
    map<_link, list<const _maille*> >::iterator lfIt, lfIt2;

    medi.treatGroupes(); // erase groupes that wont be converted
    for (unsigned int i=0; i!=medi.groupes.size(); ++i)
    {
      _groupe& grp = medi.groupes[i];
      _groupe::mailleIter maIt=grp.mailles.begin();
      if ( maIt==grp.mailles.end() || (*maIt)->dimension() != 2 )
        continue;
      for(; maIt!=grp.mailles.end(); ++maIt) {
        if ( faces.insert( &(**maIt )).second ) {
          for ( int j = 0; j < (*maIt)->sommets.size(); ++j )
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
        ASSERT( !linkFacesMap.begin()->second.empty() );
        faceQueue.push( linkFacesMap.begin()->second.front() );
      }
      while ( !faceQueue.empty() )
      {
        const _maille* face = faceQueue.front();
        faceQueue.pop();

        // loop on links of <face>
        for ( int i = 0; i < face->sommets.size(); ++i ) {
          _link link = face->link( i );
          // find the neighbor faces
          lfIt = linkFacesMap.find( link );
          int nbFaceByLink = 0;
          list< const _maille* > ml;
          if ( lfIt != linkFacesMap.end() )
          {
            list<const _maille*> & fList = lfIt->second;
            list<const _maille*>::iterator fIt = fList.begin();
            ASSERT( fIt != fList.end() );
            for ( ; fIt != fList.end(); fIt++, nbFaceByLink++ ) {
              ml.push_back( *fIt );
              if ( *fIt != face ) // wrongly oriented neighbor face
              {
                const _maille* badFace = *fIt;
                // reverse and remove badFace from linkFacesMap
                for ( int j = 0; j < badFace->sommets.size(); ++j ) {
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
                //INFOS( "REVERSE " << *badFace );
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
              INFOS(nbFaceByLink << " faces by 1 link:");
              for( ; i!= ml.end(); i++ ) {
                INFOS("in object " << fgm[ *i ]->nom);
                INFOS( **i );
              }
            }
            manifold = false;
          }
        } // loop on links of the being checked face
      } // loop on the face queue
    } // while ( !linkFacesMap.empty() )

    if ( !manifold )
      INFOS(" -> Non manifold mesh, faces orientation may be incorrect");

  } // space dimension == 3
}

//=======================================================================
//function : fillMesh
//purpose  : load data from medi to mesh
//=======================================================================

void GIBI_MESH_RDONLY_DRIVER::fillMesh(_intermediateMED* _ptrMedi)
{
  const char * LOC = "GIBI_MESH_RDONLY_DRIVER::fillMesh(_intermediateMED* _ptrMedi) : " ;
  BEGIN_OF(LOC);

  _ptrMesh->_name = _meshName;

  if (_ptrMedi)
  {
    if (_ptrMedi->maillage.size()==0 ||
        _ptrMedi->groupes.size()==0 ||
        _ptrMedi->points.size()==0) {
      INFOS(" Error while reading file: the data read are not completed " ) ;
      return;
    }
    // fix element orientation
    orientElements( *_ptrMedi );

    _ptrMesh->_spaceDimension = _ptrMedi->points.begin()->second.coord.size();
    _ptrMesh->_meshDimension = _ptrMedi->maillage.rbegin()->dimension();
    _ptrMesh->_numberOfNodes = _ptrMedi->points.size();
    _ptrMesh->_isAGrid = 0;
    _ptrMesh->_coordinate = _ptrMedi->getCoordinate();

    //Construction des groupes
    _ptrMedi->getGroups(_ptrMesh->_groupCell,
                        _ptrMesh->_groupFace,
                        _ptrMesh->_groupEdge,
                        _ptrMesh->_groupNode, _ptrMesh);

    _ptrMesh->_connectivity = _ptrMedi->getConnectivity(); 

    // calcul de la connectivite d-1 complete, avec renumerotation des groupes
    //if (_ptrMesh->_spaceDimension==3)
    //    _ptrMesh->_connectivity->updateGroup(_ptrMesh->_groupFace) ;
    //else if (_ptrMesh->_spaceDimension==2)
    //    _ptrMesh->_connectivity->updateGroup(_ptrMesh->_groupEdge) ;

    // Creation des familles à partir des groupes
    // NC : Cet appel pourra être différé quand la gestion de la cohérence famille/groupes sera assurée
    _ptrMesh->createFamilies();
    // TAKE CARE OF ELEMENTS ORDER IN GROUPS AFTER THEIR SPLITING INTO FAMILIES !!!!
    // _ptrMesh->createFamilies() breaks the order
//     _ptrMedi->getFamilies(_ptrMesh->_familyCell,
//                           _ptrMesh->_familyFace,
//                           _ptrMesh->_familyEdge,
//                           _ptrMesh->_familyNode, _ptrMesh);

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

//=======================================================================
//function : open
//purpose  : 
//=======================================================================

void GIBI_MESH_WRONLY_DRIVER::open()
  //     throw (MEDEXCEPTION)
{
  const char * LOC = "GIBI_MESH_DRIVER::open()" ;
  BEGIN_OF(LOC);

  MED_EN::med_mode_acces aMode = getAccessMode();
  switch (aMode) {
  case MED_EN::MED_REMP:	
  case MED_EN::MED_ECRI: // should never append !!
    _gibi.open(_fileName.c_str(), ios::out);
    break;
  default:
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Bad file mode access ! " << aMode));
  }
  if (_gibi &&
#ifdef WNT
      _gibi.is_open()
#else
      _gibi.rdbuf()->is_open()
#endif
      )
    _status = MED_OPENED;
  else
  {
    _status = MED_CLOSED;
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Could not open file "<<_fileName));
  }
  END_OF(LOC);
}

//=======================================================================
//function : close
//purpose  : 
//=======================================================================

void GIBI_MESH_WRONLY_DRIVER::close()
  //  throw (MEDEXCEPTION)
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

//=======================================================================
//function : write
//purpose  : 
//=======================================================================

void GIBI_MESH_WRONLY_DRIVER::write(void) const
  throw (MEDEXCEPTION)
{ 
  const char * LOC = "void GIBI_MESH_WRONLY_DRIVER::write(void) const : ";
  BEGIN_OF(LOC);

  // we are going to modify the _gibi field
  GIBI_MESH_WRONLY_DRIVER * me = const_cast<GIBI_MESH_WRONLY_DRIVER *>(this);
  try {
    me->writeSupportsAndMesh();
    me->writeLastRecord();
  }
  catch (MEDEXCEPTION &ex)
  {
    INFOS( ex.what() );
  }

  END_OF(LOC);
}

//=======================================================================
//function : getName
//purpose  : return cleaned up support name
//=======================================================================

static string cleanName( const string& theName )
{
  string name = theName;
  if ( !name.empty() ) {
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
    INFOS("Do not save " << supType << " of entity " << entity
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
      if ( geomType == 0 )
        geomType = MED_POINT1;

      data.addTypeData( geomType, nbElems, ptrElemIDs, elemID1 );
    }
  }

  if ( data.getNumberOfTypes() == 0 ) {
    INFOS("Do not save " << supType << " of entity " << entity
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
  char* zeroI8 = "       0"; // FORMAT(I8)
  int nbElemNodes = geomType % 100;
        
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

  MESSAGE("writeElements(): geomType=" << geomType << " nbElements= " << nbElements)

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
          _gibi << setw(8) << nodalConnect[ nodeId++ ];
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

void GIBI_MESH_WRONLY_DRIVER::addName(map<string,int>& nameMap,
                                      string&          theName,
                                      int              index,
                                      string           prefix)
{
  string name = cleanName( theName );
  if ( !name.empty() ) {
    int len = name.length();
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
      INFOS( "Save <" << name << "> as <" << str << ">");
      delete [] str;
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

void GIBI_MESH_WRONLY_DRIVER::writeSupportsAndMesh()
{
  const char * LOC = "void GIBI_MESH_WRONLY_DRIVER::writeSupportsAndMesh() ";
  BEGIN_OF(LOC);

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

  vector<int> nbSuppElemsByType(MED_HEXA20,0);
  map<string,int> nameNbMap;
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

    addName( nameNbMap, data._cleanName, data._id, "C" );
    MESSAGE( "obj " << data._id << " " << data._cleanName);

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
  char* zeroI8 = "       0"; // FORMAT(I8)
  for ( supIt = _supports.begin(); supIt != _supports.end(); supIt++ )
  {
    supportData & data = supIt->second;
    int nbSupObj = data.getNumberObjects();
    if ( nbSupObj == 0 )
      continue;
    MESSAGE("support " << data._id << "<" << data._cleanName << ">");

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
  for ( fcount.init(3),i = 0; i < nbNodes; ++i, j += dim )
  {
    for ( int iCoord = 0; iCoord < dim; ++iCoord, fcount++ )
      _gibi << setw(22) << coords[ j + iCoord ];
    _gibi << setw(22) << 0.0; // densit銠   fcount++;
  }
  fcount.stop();

  END_OF(LOC);
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
       GIBI_MESH_DRIVER(fileName,ptrMesh,MED_RDWR)
{
  MESSAGE("GIBI_MESH_RDWR_DRIVER::GIBI_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}
GIBI_MESH_RDWR_DRIVER::GIBI_MESH_RDWR_DRIVER(const GIBI_MESH_RDWR_DRIVER & driver): 
  GIBI_MESH_RDONLY_DRIVER::GIBI_MESH_DRIVER(driver)
{
  MESSAGE("GIBI_MESH_RDWR_DRIVER::GIBI_MESH_RDWR_DRIVER(driver) has been created");
}
GIBI_MESH_RDWR_DRIVER::~GIBI_MESH_RDWR_DRIVER() {
  MESSAGE("GIBI_MESH_RDWR_DRIVER::GIBI_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) has been destroyed");
} 
GENDRIVER * GIBI_MESH_RDWR_DRIVER::copy(void) const
{
  BEGIN_OF( "GIBI_MESH_RDWR_DRIVER::copy()");
  GENDRIVER * driver = new GIBI_MESH_RDWR_DRIVER(*this);
  END_OF( "GIBI_MESH_RDWR_DRIVER::copy()");
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
  BEGIN_OF( "GIBI_MESH_RDWR_DRIVER::read()");
  GIBI_MESH_RDONLY_DRIVER::open();
  GIBI_MESH_RDONLY_DRIVER::read();
  GIBI_MESH_RDONLY_DRIVER::close();
  END_OF( "GIBI_MESH_RDWR_DRIVER::read()");
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
  MESSAGE("GIBI_MED_RDONLY_DRIVER(const string & fileName, MED * ptrMed) has been created");
  _fileName = fileName;
  _accessMode = MED_RDONLY;
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
  const char * LOC = "GIBI_MED_RDONLY_DRIVER::read() : " ;
  BEGIN_OF(LOC);

  if (_status!=MED_OPENED)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "file " << _fileName<<" is not opened." ));

  _ptrMesh = new MESH();

  _intermediateMED medi;
  try {
    if ( !readFile( &medi, true ) )
      return;

    // set name of field and its support if it is empty
    set<string> fnames;
    list< _fieldBase* >::iterator fIt = medi.fields.begin();
    for ( ; fIt != medi.fields.end(); fIt++ )
      fnames.insert( (*fIt)->name );
    int i = 0;
    for (fIt = medi.fields.begin(); fIt != medi.fields.end(); fIt++ ) {
      _fieldBase* f = *fIt;
      if ( f->name.empty() ) {
        do {
          ostringstream name;
          name << "F_" << ++i;
          f->name = name.str();
        } while ( !fnames.insert( f->name ).second );
      }
      _groupe& g = medi.groupes[ f->group_id ];
      if ( g.nom.empty() )
        g.nom = "GRP_" + f->name;
    }
    //MESSAGE(LOC <<  medi );
    fillMesh( &medi );
    MESSAGE(LOC << "GIBI_MED_RDONLY_DRIVER::read : RESULTATS STRUCTURE INTERMEDIAIRES : ");
    MESSAGE(LOC <<  medi );

    list< FIELD_* > fields;
    medi.getFields( fields );
    MESSAGE( "nb fields: " << fields.size() );

    if ( _ptrMesh->getName().empty() )
      _ptrMesh->setName( "MESH" );

    _med->addMesh( _ptrMesh );

    list< FIELD_* >::iterator it = fields.begin();
    for ( ; it != fields.end(); it++ )
      _med->addField( *it );
  }
  catch (MEDEXCEPTION &ex)
  {
    INFOS( ex.what() );
  }

  END_OF(LOC);
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
    "GIBI_MED_WRONLY_DRIVER(const string & fileName, MED * ptrMed, MESH * ptrMesh)" ;
  BEGIN_OF(LOC);
  
  _fileName = fileName;
  _accessMode = MED_WRONLY;
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

template< class T >
  static void writeDataSection (fstream& file,
                                FIELD_*  field,
                                int      id1,
                                int      id2)
{
  FIELD<T>* f = dynamic_cast<FIELD<T>*>( field );
  if (!f) return;
  MEDARRAY<T>* array = f->getvalue();
  int ld = array->getLeadingValue();
  //SCRUTE( array->getLengthValue() );
  for ( int iComp = 0; iComp < ld; ++iComp )
  {
    file << setw(8) << 1          // nb scalar values by element
      << setw(8) << ( id2 - id1 ) // total nb of scalar values
        << setw(8) << 0
          << setw(8) << 0 << endl;
    // * 8003   FORMAT(1P,3E22.14)
    int id = id1;
    while ( id < id2 )
    {
      for ( int i = 0; id < id2 && i < 3; ++i )
        file << setw(22) << array->getIJ( id++, iComp + 1);
      file << endl;
    }
  }
}

//=======================================================================
//function : getSupports
//purpose  : 
//=======================================================================

// int GIBI_MED_WRONLY_DRIVER::getSupports(const FIELD_*         field,
//                                         list<const SUPPORT*>& supList) const
// {
//   int nbSup = 0;
//   const SUPPORT* sup = field->getSupport();
//   const GROUP* group = dynamic_cast<const GROUP*>( sup );
//   if ( group ) {
//     int nbFam = group->getNumberOfFamilies();
//     for ( int iFam = 1; iFam <= nbFam; ++iFam ) {
//       const FAMILY* fam = group->getFamily( iFam );
//       if ( getSupportIndex( fam ) >= 0 ) {
//         supList.push_back( fam );
//         nbSup++;
//       }
//     }
//   }
//   else {
//     supList.push_back( sup );
//     nbSup++;
//   }
//   return nbSup;
// }

//=======================================================================
//function : write
//purpose  : 
//=======================================================================

void GIBI_MED_WRONLY_DRIVER::write( void ) const throw (MEDEXCEPTION)
{
  const char * LOC = "void GIBI_MED_WRONLY_DRIVER::write(void) const : ";
  BEGIN_OF(LOC);

  // we are going to modify the _gibi field
  GIBI_MED_WRONLY_DRIVER * me = const_cast<GIBI_MED_WRONLY_DRIVER *>(this);

  // get all fields on _ptrMesh and add their support to be written
  list<FIELD_*> fields;
  int iField, nbFileds = _med->getNumberOfFields();
  int nb_obj = 0;
  list<int> nb_sub_list;
  map<string,int> nameNbMap;

  list<pair<int,int> >           subIdSizeList; // pair( <submesh id>, <submesh size> );
  list<pair<int,int> >::iterator idsize;

  string *names=new string[ nbFileds ];
  _med->getFieldNames( names );
  for ( iField = 0; iField < nbFileds; ++iField )
  {
    int nb_sub = 0;
    deque<DT_IT_> dtit = _med->getFieldIteration( names[ iField ]);
    deque<DT_IT_>::iterator fIt = dtit.begin();
    for ( ; fIt != dtit.end(); fIt++ )
    {
      FIELD_ * f = _med->getField( names[ iField ], fIt->dt, fIt->it );
      if ( !dynamic_cast< FIELD<double >* >( f ))
      {
        MESSAGE("GIBI_MED_WRONLY_DRIVER::write( FIELD< int > ) not implemented"); 
        continue;
      }
      const SUPPORT * sup = f->getSupport();
      if ( me->addSupport( sup ) ) {
        fields.push_back( f );
        nb_sub += getSubMeshIdAndSize( sup, subIdSizeList );
      }
    }
    if ( nb_sub ) {
      addName( nameNbMap, names[ iField ], ++nb_obj, "F" );
      nb_sub_list.push_back( nb_sub );
    }
  }

  // write mesh

  try {
    me->writeSupportsAndMesh();
  }
  catch (MEDEXCEPTION &ex)
  {
    INFOS( ex.what() );
    return;
  }

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
    for ( ; itF != fields.end(); itF++ )
    {
      if ( cur_nb_sub == nb_sub && itNbSub != nb_sub_list.end() ) {
        // start the next field writting
        nb_sub = *(itNbSub++);
        gibi << setw(8) << nb_sub << "      -1       6      72" << endl;
        gibi << left;
        gibi << setw(72) << " Field" << endl;
        gibi << right;
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
            for ( int i = 0; i < vals.size(); ++i, fcount++ )
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
        for ( fcount.init(8), iComp = 0; iComp < nbComp; ++iComp, fcount++ )
          gibi << " "  << setw(8) << f->getComponentName( iComp + 1 );
        fcount.stop();
        // component types
        for ( fcount.init(4), iComp = 0; iComp < nbComp; ++iComp, fcount++ )
          gibi << " "  << setw(17) << "REAL*8";
        fcount.stop();
        gibi << right;

        // Data section
        int id2 = id1 + idsize->second;
        writeDataSection<double>( gibi, f, id1, id2 );
        id1 = id2;
      }
    } // loop on fields
  }
  me->writeLastRecord();
  delete [] names;
  END_OF(LOC);
}
