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
/*************************************************************************
* COPYRIGHT (C) 1999 - 2003  EDF R&D
* THIS LIBRARY IS FREE SOFTWARE; YOU CAN REDISTRIBUTE IT AND/OR MODIFY
* IT UNDER THE TERMS OF THE GNU LESSER GENERAL PUBLIC LICENSE 
* AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION; 
* EITHER VERSION 2.1 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
*  
* THIS LIBRARY IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL, BUT
* WITHOUT ANY WARRANTY; WITHOUT EVEN THE IMPLIED WARRANTY OF
* MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. SEE THE GNU
* LESSER GENERAL PUBLIC LICENSE FOR MORE DETAILS.
*
* YOU SHOULD HAVE RECEIVED A COPY OF THE GNU LESSER GENERAL PUBLIC LICENSE
* ALONG WITH THIS LIBRARY; IF NOT, WRITE TO THE FREE SOFTWARE FOUNDATION,
* INC., 59 TEMPLE PLACE, SUITE 330, BOSTON, MA 02111-1307 USA
*
*************************************************************************/
#ifndef IMPORT_HXX
#define IMPORT_HXX

#include <MEDMEM.hxx>

#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
// #include "MEDMEM_define.hxx"


/*
  Med File V 2.2 attributes in the med_2_2 namespace
*/

#undef MED_H

namespace med_2_2 {
  extern "C" { 
#include <med.h>
#include <med_proto.h>
#include <med_hdfi.h>

    /*
      Med File V 2.2 attributes more addings to encapsulate the Med File V2_2
      medimport utilitary
    */

#define MED_NOM_INFOS "INFOS_GENERALES"
#define MED_NOM_MAJEUR "MAJ"
#define MED_NOM_MINEUR "MIN"
#define MED_NOM_RELEASE "REL"
#define MED_NOM_NOE "NOE"
#define MED_NOM_NBR "NBR"
#define MED_NOM_REP "REP"
#define MED_NOM_NOM "NOM"
#define MED_NOM_UNI "UNI"
#define MED_NOM_COO "COO"
#define MED_TAILLE_NOM_ENTITE 3
#define MED_NOM_MAI "MAI"
#define MED_NOM_NOD "NOD"
#define MED_TAILLE_FAS 5
#define MED_NOM_NUM "NUM"
#define MED_TAILLE_MAA 9
#define MED_MAA "/ENS_MAA/"
#define MED_NOM_DIM "DIM"
#define MED_NOM_DES "DES"
#define MED_NOM_TYP "TYP"
#define MED_NOM_PO1 "PO1"
#define MED_NOM_SE2 "SE2"
#define MED_NOM_SE3 "SE3"
#define MED_NOM_TR3 "TR3"
#define MED_NOM_TR6 "TR6"
#define MED_NOM_QU4 "QU4"
#define MED_NOM_QU8 "QU8"
#define MED_NOM_TE4 "TE4"
#define MED_NOM_T10 "T10"
#define MED_NOM_HE8 "HE8"
#define MED_NOM_H20 "H20"
#define MED_NOM_PE6 "PE6"
#define MED_NOM_P15 "P15"
#define MED_NOM_PY5 "PY5"
#define MED_NOM_P13 "P13"
#define MED_CHA "/CHA/"
#define MED_TAILLE_CHA 5
#define MED_NOM_NCO "NCO"
#define MED_NOM_PDT "PDT"
#define MED_NOM_NGA "NGA"
#define MED_NOM_GAU "GAU"
#define MED_NOM_PFL "PFL"
#define MED_NOM_CO  "CO"
#define MED_PROFILS "/PROFILS/"
#define MED_TAILLE_PROFILS 9 
#define MED_LIENS "/LIENS/"
#define MED_TAILLE_LIENS 7 
  }
}

// namespace med_2_2 {
//   extern "C" {
// #include <med_hdfi.h>
// #include <med_misc.h>

// #define MED_NOM_MAJEUR "MAJ"
// #define MED_NOM_MINEUR "MIN"
// #define MED_NOM_RELEASE "REL"

// #define MED_NUM_MAJEUR 2
// #define MED_NUM_MINEUR 2
// #define MED_NUM_RELEASE 1

// #define MED_NOM_INFOS "INFOS_GENERALES"

// #define MED_NOM_DESCRIPTEUR "descripteur de fichier"
// #define MED_VERSION_ACTUELLE "2.2.1"
// #define HDF_VERSION_ACTUELLE "5-1.4.4"

// /* Noms des data sets ou attributs correspondant a des entites MED */
// #define MED_TAILLE_NOM_ENTITE 3
// #define MED_NOM_NUM "NUM"
// #define MED_NOM_NBR "NBR"
// #define MED_NOM_NOM "NOM"
// #define MED_NOM_UNV "UNV"
// #define MED_NOM_NNS "NNS"
// #define MED_NOM_NNM "NNM"
// #define MED_NOM_NNI "NNI"
// #define MED_NOM_GRO "GRO"
// #define MED_NOM_ATT "ATT"
// #define MED_NOM_NCO "NCO"
// #define MED_NOM_DIM "DIM"
// #define MED_NOM_ESP "ESP"
// #define MED_NOM_FAM "FAM"
// #define MED_NOM_IDE "IDE"
// #define MED_NOM_VAL "VAL"
// #define MED_NOM_DES "DES"
// #define MED_NOM_COR "COR"
// #define MED_NOM_DIM "DIM"
// #define MED_NOM_NOE "NOE"
// #define MED_NOM_COO "COO"
// #define MED_NOM_IN1 "IN1"
// #define MED_NOM_IN2 "IN2"
// #define MED_NOM_IN3 "IN3"
// #define MED_NOM_REP "REP"
// #define MED_NOM_UNI "UNI"
// #define MED_NOM_NOD "NOD"
// #define MED_NOM_TYP "TYP"
// #define MED_NOM_GTY "GTY"
// #define MED_NOM_CO  "CO"
// #define MED_NOM_NCW "NCW"
// #define MED_NOM_TYW "TYW"

// #define MED_NOM_MAI "MAI"
// #define MED_NOM_FAC "FAC"
// #define MED_NOM_ARE "ARE"

// #define MED_NOM_PO1 "PO1"
// #define MED_NOM_SE2 "SE2"
// #define MED_NOM_SE3 "SE3"
// #define MED_NOM_TR3 "TR3"
// #define MED_NOM_TR6 "TR6"
// #define MED_NOM_QU4 "QU4"
// #define MED_NOM_QU8 "QU8"
// #define MED_NOM_TE4 "TE4"
// #define MED_NOM_T10 "T10"
// #define MED_NOM_HE8 "HE8"
// #define MED_NOM_H20 "H20"
// #define MED_NOM_PE6 "PE6"
// #define MED_NOM_P15 "P15"
// #define MED_NOM_PY5 "PY5"
// #define MED_NOM_P13 "P13"

// #define MED_NOM_TAI "TAI"
// #define MED_NOM_TTI "TTI"
// #define MED_NOM_IFN "IFN"
// #define MED_NOM_IFD "IFD"
// #define MED_NOM_INN "INN"
// #define MED_NOM_IND "IND"
// #define MED_NOM_POG "POG"
// #define MED_NOM_POE "POE"

// #define MED_NOM_GEO "GEO"
// #define MED_NOM_GAU "GAU"
// #define MED_NOM_NGA "NGA"
// #define MED_NOM_PFL "PFL"
// #define MED_NOM_NDT "NDT"
// #define MED_NOM_PDT "PDT"
// #define MED_NOM_NOR "NOR"
// #define MED_NOM_LIE "LIE"

// /* Nom du DATA GROUP CONTENANT TOUS LES MAILLAGES DU FICHIER HDF */
// #define MED_MAA "/ENS_MAA/"
// #define MED_TAILLE_MAA 9

// /* Nom du data group ou ranger les champs solution */
// #define MED_CHA "/CHA/"
// #define MED_TAILLE_CHA 5

// /* Nom du data group point de montage */
// #define MED_MNT "/MNT/"
// #define MED_TAILLE_MNT 5

// /* Nom du data group ou ranger les familles */
// #define FAMILLE_ZERO "FAMILLE_ZERO"
// #define MED_FAS "/FAS/"
// #define MED_TAILLE_FAS 5
// #define MED_FAS_NOEUD "/NOEUD/"
// #define MED_FAS_NOEUD_NOM "NOEUD"
// #define MED_FAS_ELEME "/ELEME/"
// #define MED_FAS_ELEME_NOM "ELEME"
// #define MED_TAILLE_FAS_ENTITE 7


// /* Nom du data group ou ranger les equivalences */
// #define MED_EQS "/EQS/"
// #define MED_TAILLE_EQS 5

// /* Nom du data group contenant les profils */
// #define MED_PROFILS "/PROFILS/"
// #define MED_TAILLE_PROFILS 9 

// /* Nom du data group contenant la localisation des points de GAUSS */
// #define MED_GAUSS "/GAUSS/"
// #define MED_TAILLE_GAUSS 7 

// /* Nom du groupe HDF ou sont rangees les valeurs numeriques scalaires 
//    non liees au modele de donnees
// */
// #define MED_NUM_DATA  "/NUM_DATA/"
// #define MED_TAILLE_NUM_DATA 10

// /* Nom du data group contenant les liens */
// #define MED_LIENS "/LIENS/"
// #define MED_TAILLE_LIENS 7 

//   }
// }

#include <stdio.h>
#include <string.h>

#define ANCIEN_MED_TAILLE_PNOM 8
/*                              12345678 */
#define ANCIEN_MED_BLANC_PNOM  "        "

namespace med_2_2 {

MEDMEM_EXPORT med_err OLD_MEDattrNumLire(med_idt pere, med_type_champ type, char *nom,
			   unsigned char *val, hid_t hdf_file);

MEDMEM_EXPORT med_err OLD_MEDdatasetNumLire(med_idt pere, char *nom, med_type_champ type,
			      med_mode_switch interlace, med_size nbdim,
			      med_size fixdim, med_size psize,
			      med_ssize * pfltab, med_int ngauss,
			      unsigned char *val,hid_t hdf_file);

MEDMEM_EXPORT void conversion_chaine(char *ancienne_chaine, char *nouvelle_chaine,
		       med_int n);

MEDMEM_EXPORT void MAJ_version(med_idt fid) throw (MEDMEM::MEDEXCEPTION) ;

MEDMEM_EXPORT void MAJ_noeuds_maillage(med_idt mid, med_int dimension) throw (MEDMEM::MEDEXCEPTION) ;

MEDMEM_EXPORT void MAJ_elements_maillage(med_idt mid, med_int dimension)
  throw (MEDMEM::MEDEXCEPTION);

MEDMEM_EXPORT void MAJ_familles_maillage(med_idt mid) throw (MEDMEM::MEDEXCEPTION) ;

MEDMEM_EXPORT void MAJ_maillages(med_idt fid) throw (MEDMEM::MEDEXCEPTION) ;

MEDMEM_EXPORT void MAJ_localisation_Gauss(med_idt fid,char *nom_modele,med_int ngauss)
  throw (MEDMEM::MEDEXCEPTION) ;

MEDMEM_EXPORT void MAJ_champs(med_idt fid) throw (MEDMEM::MEDEXCEPTION) ;

MEDMEM_EXPORT void MAJ_profils(med_idt fid,med_int nprofil) throw (MEDMEM::MEDEXCEPTION) ;
}

namespace MEDMEM {

/*!
  This method is supposed to be the same as the med File V2.2 medimport
  utility. fileNameIn is .med V2.1 file supplied by the user including
  all the directory path. fileNameOut is .med V2.2 file produced by this
  command in the directory $HOME/TMP_Med2_2Files/
*/

MEDMEM_EXPORT char * med2_1_To_med2_2(char * fileNameIn) throw (MEDMEM::MEDEXCEPTION) ;
}

#endif /* IMPORT_HXX */
