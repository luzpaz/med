#ifndef DEFINE_HXX
#define DEFINE_HXX	

#include <stdio.h>
#include <iostream>
#include <string>
#include <assert.h>

// UTILE AUX DEUX NAMESPACES
#include <hdf5.h>

// Arrondis pour le diff entre PCLINUX et ALPHA_OSF
#define DIFF_PC_ALPHA 0
// Trace des valeurs hexadecimales des double
#define TRACE_DBLE_HEXA 0
typedef union { double d ;
                int i[2] ; } DI ;
inline double Arrondi( double avalret ) {
  DI valret ;
  valret.d = avalret ;
  DI val ;
  DI valeps ;
  DI valeps1 ;
  val.d = valret.d ;
  bool kneg = false ;
  if ( valret.i[1] == 0x80000000 && valret.i[0] == 0x00000000 ) {
    valret.d = 0.0 ;
  }
  if ( valret.d < 0 ) {
    kneg = true ;
    valret.d = - valret.d ;
  }
  if ( valret.d != 0 ) {
    valeps.i[1] = valret.i[1] & 0xfff00000 ;
    valeps.i[0] = 1 ;
    valeps1.i[1] = valret.i[1] & 0xfff00000 ;
    valeps1.i[0] = 0 ;
    valeps.d = valeps.d - valeps1.d ;
    valret.d = valret.d + valeps.d ;
    valret.i[0] = valret.i[0] & 0xfffffff0 ;
    if ( kneg ) {
      valret.d = - valret.d ;
    }
  }
//#if TRACE_DBLE_HEXA
//  printf( "testMedMemGeneral %0#8x%0#8x Arrondi : %0#8x%0#8x \n" , val.i[1] , val.i[0] , valret.i[1] , valret.i[0] ) ;
//#endif
  return valret.d ;
}

namespace MED_FR {
  extern "C" { 
# include <med.h>
# include <med_proto.h>
  }
}


namespace MED_EN {
  extern "C" { 
  
#define MED_NULL       (void *) NULL
#define MED_MAX_PARA     20

#define MED_TAILLE_DESC 200
#define MED_TAILLE_IDENT  8
#define MED_TAILLE_NOM   32
#define MED_TAILLE_LNOM  80    
#define MED_TAILLE_PNOM   8 


    typedef long medGeometryElement;
    const medGeometryElement MED_NONE = 0;
    const medGeometryElement MED_POINT1 = 1;
    const medGeometryElement MED_SEG2 = 102;
    const medGeometryElement MED_SEG3 = 103;
    const medGeometryElement MED_TRIA3 = 203;
    const medGeometryElement MED_QUAD4 = 204;
    const medGeometryElement MED_TRIA6 = 206;
    const medGeometryElement MED_QUAD8 = 208;
    const medGeometryElement MED_TETRA4 = 304;
    const medGeometryElement MED_PYRA5 = 305;
    const medGeometryElement MED_PENTA6 = 306;
    const medGeometryElement MED_HEXA8 = 308;
    const medGeometryElement MED_TETRA10 = 310;
    const medGeometryElement MED_PYRA13 = 313;
    const medGeometryElement MED_PENTA15 = 315;
    const medGeometryElement MED_HEXA20 = 320;
    const medGeometryElement MED_ALL_ELEMENTS = 999;

    typedef long medEntityMesh;
    const medEntityMesh MED_CELL = 0;
    const medEntityMesh MED_FACE = 1;
    const medEntityMesh MED_EDGE = 2;
    const medEntityMesh MED_NODE = 3;
    const medEntityMesh MED_ALL_ENTITIES = 4;
    
    typedef long medModeSwitch;
    const medModeSwitch MED_FULL_INTERLACE = 0;
    const medModeSwitch MED_NO_INTERLACE = 1;
    
    typedef long medConnectivity;
    const medConnectivity MED_NODAL = 0;
    const medConnectivity MED_DESCENDING = 1;
    
    typedef enum {MED_CARTESIAN, MED_POLAR, MED_BODY_FITTED} med_grid_type;
    
    typedef enum {MED_LECT,MED_ECRI,MED_REMP} med_mode_acces; 
    
    typedef enum {MED_COOR, MED_CONN, MED_NOM, MED_NUM, MED_FAM} med_table;
    
    typedef enum {MED_REEL64=6, MED_INT32=24,MED_INT64=26, MED_INT} med_type_champ;

#define MED_NBR_GEOMETRIE_MAILLE 15
#define MED_NBR_GEOMETRIE_FACE 4
#define MED_NBR_GEOMETRIE_ARETE 2

typedef enum {MED_CART, MED_CYL, MED_SPHER} med_repere; 

typedef enum {MED_FAUX, MED_VRAI} med_booleen ; 

typedef enum {MED_GROUPE, MED_ATTR, MED_FAMILLE} med_dim_famille; 

typedef enum {MED_COMP, MED_DTYPE} med_dim_champ; 

typedef enum {MED_HDF_VERSION, MED_VERSION, MED_FICH_DES} med_fich_info; 

#define MED_NOPG   1                   /* -> pas de point de Gauss                    */
#define MED_NOPFL  ""                  /* -> pas de profils utilisateur               */
#define MED_NOPFLi "                                "  /* Variable Interne                      */
#define MED_NOPF   0                   /* -> pas de profils pour _MEDdataseNnumEcrire */
#define MED_NOPDT -1                   /* rem: pas de pas de temps negatifs           */
#define MED_NONOR -1                   /* rem: pas de n°ordre negatif                 */
#define MED_DIM1   1                   /* PAS */
  //#define MED_ALL    0 !!!!! NB: WARNING MED_ALL deja utilise dans l'enum medGeometryElement !!!!!!!!
#define MED_ALL    0

//CCRT#if defined(SUN4SOL2) || defined(PCLINUX) || defined(OSF1) || defined(IRIX64_32) || defined(RS6000)
// Copy of med.h with OSF1 instead of OSF1_32 :
// mismatch between definitions of MEDMEM and med-2.1.6 !!! ...
#if defined(SUN4SOL2) || defined(PCLINUX) || defined(OSF1_32) || defined(IRIX64_32) || defined(RS6000)
/* interface C/FORTRAN */
/* this true only with g77 and gcc : we must change it to use directly NOMF_... and INT32 or INT64 - it will be more simple to understand and to use ! */
#define NOMF_POST_UNDERSCORE 

/* correspondance des types avec HDF 5 */
typedef hsize_t        med_size;
typedef hssize_t       med_ssize;
typedef hid_t          med_idt;
typedef herr_t         med_err;

/* types elementaires */
typedef int            med_int;
typedef double         med_float;
#endif

#if defined(HP9000)
/* correspondance des types avec HDF 5 */
typedef hsize_t        med_size;
typedef hssize_t       med_ssize;
typedef hid_t          med_idt;
typedef herr_t         med_err;

/* types elementaires */
typedef int            med_int;
typedef double         med_float;
#endif

// Copy of med.h without OSF1 :
// mismatch between definitions of MEDMEM and med-2.1.6 !!! ...
#if defined(IRIX64) || defined(OSF1)
#define NOMF_POST_UNDERSCORE

/* correspondance des types avec HDF 5 */
typedef hsize_t        med_size;
typedef hssize_t       med_ssize;
typedef hid_t          med_idt;
typedef herr_t         med_err;

/* types elementaires */
typedef long           med_int;
typedef double         med_float;
#endif


#if defined(PPRO_NT) 
/* correspondance des types avec HDF 5 */
typedef hsize_t        med_size;
typedef hssize_t       med_ssize;
typedef hid_t          med_idt;
typedef herr_t         med_err;

/* types elementaires */
typedef int	       med_int;
typedef double         med_float;
#endif


#if defined(NOMF_PRE_UNDERSCORE) && defined(NOMF_POST_UNDERSCORE)
#   define NOMF(x)     _##x##_
#endif
#if defined(NOMF_PRE_UNDERSCORE) && !defined(NOMF_POST_UNDERSCORE)
#   define NOMF(x)     _##x
#endif
#if !defined(NOMF_PRE_UNDERSCORE) && defined(NOMF_POST_UNDERSCORE)
#   define NOMF(x)     x##_
#endif
#if !defined(NOMF_PRE_UNDERSCORE) && !defined(NOMF_POST_UNDERSCORE)
#   define NOMF(x)     x
#endif

  //#include "med_proto.h"

  }

// valeurs des algos de connexites
#define FRACT_ALGO 10
#define MAX_LOOP 200
// fin

// Valeurs de IO_MED
//#define READ 0
//#define WRITE 1


#define MED_CLOSED   0
#define MED_OPENED   1
#define MED_INVALID -1
#define MED_ERROR   -1
#define MED_VALID    0
//#define MED_NULL     NULL

#define MED_RDONLY MED_LECT
  //   rem: MED_WRONLY=MED_ECR n'empêche malheureusement pas de lire le fichier
  //   mais permet de conserver l'existant. Attention à la création d'objet 
  //   ne prenant pas de paramètre de type mode d'accès il faut tester si il
  //   est déjà présent dans le fichier. Si  MED_WRONLY=MED_REMP le fichier est
  //   réinitialisé. Cf une évolution de MED.
#define MED_WRONLY MED_ECRI
#define MED_RDWR   MED_ECRI
// Fin

#define FAUX 0
#define VRAI 1
#define longueur_string 100
#define PRECISION 0.0000001
// #define undefined -1
#define UNDEFINED -1

// type des connectivites
//#define CONN_NOD 0
//#define CONN_DESC_PART 1
//#define CONN_DESC_TOT 2

// valeurs de type_interpolation
#define P1                 0
#define pseudo_P2          1
#define trilineaire        2
#define P2_adapte          3
#define VF_conservatif     4
#define VF_non_conservatif 5
// valeurs de type_mapping
#define connexite   0
#define conn_octree 1
#define brutal      2
// valeurs des type_support
#define support_noeud 0
#define support_maille 1
// retour des fonctions
#define OK 0
#define KO 1
// fin

}
// End Of NameSpace MED_EN


#endif /* DEFINE_HXX */

