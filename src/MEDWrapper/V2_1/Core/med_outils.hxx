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

#ifndef MED_OUTILS_H
#define MED_OUTILS_H
#include "med.hxx"

#define MED_NOM_MAJEUR (char*)"MAJ"
#define MED_NOM_MINEUR (char*)"MIN"
#define MED_NOM_RELEASE (char*)"REL"

#define MED_NUM_MAJEUR 2
#define MED_NUM_MINEUR 1
#define MED_NUM_RELEASE 6

#define MED_NOM_INFOS (char*)"INFOS_GENERALES"

#define MED_NOM_DESCRIPTEUR "descripteur de fichier"
#define MED_VERSION_ACTUELLE "2.1.6"
#define HDF_VERSION_ACTUELLE "5-1.4.4"

/* Noms des data sets ou attributs correspondant a des entites MED */
#define MED_TAILLE_NOM_ENTITE 3
#define MED_NOM_NUM (char*)"NUM"
#define MED_NOM_NBR (char*)"NBR"
#define MED_NOM_NOM (char*)"NOM"
#define MED_NOM_UNV (char*)"UNV"
#define MED_NOM_NNS (char*)"NNS"
#define MED_NOM_NNM (char*)"NNM"
#define MED_NOM_NNI (char*)"NNI"
#define MED_NOM_GRO (char*)"GRO"
#define MED_NOM_ATT (char*)"ATT"
#define MED_NOM_NCO (char*)"NCO"
#define MED_NOM_DIM (char*)"DIM"
#define MED_NOM_FAM (char*)"FAM"
#define MED_NOM_IDE (char*)"IDE"
#define MED_NOM_VAL (char*)"VAL"
#define MED_NOM_DES (char*)"DES"
#define MED_NOM_COR (char*)"COR"
#define MED_NOM_DIM (char*)"DIM"
#define MED_NOM_NOE (char*)"NOE"
#define MED_NOM_COO (char*)"COO"
#define MED_NOM_REP (char*)"REP"
#define MED_NOM_UNI (char*)"UNI"
#define MED_NOM_NOD (char*)"NOD"
#define MED_NOM_TYP (char*)"TYP"
#define MED_NOM_CO (char*)"CO"
#define MED_NOM_NCW (char*)"NCW"
#define MED_NOM_TYW (char*)"TYW"
#define MED_NOM_MAI (char*)"MAI"
#define MED_NOM_FAC (char*)"FAC"
#define MED_NOM_ARE (char*)"ARE"
#define MED_NOM_PO1 (char*)"PO1"
#define MED_NOM_SE2 (char*)"SE2"
#define MED_NOM_SE3 (char*)"SE3"
#define MED_NOM_TR3 (char*)"TR3"
#define MED_NOM_TR6 (char*)"TR6"
#define MED_NOM_QU4 (char*)"QU4"
#define MED_NOM_QU8 (char*)"QU8"
#define MED_NOM_TE4 (char*)"TE4"
#define MED_NOM_T10 (char*)"T10"
#define MED_NOM_HE8 (char*)"HE8"
#define MED_NOM_H20 (char*)"H20"
#define MED_NOM_PE6 (char*)"PE6"
#define MED_NOM_P15 (char*)"P15"
#define MED_NOM_PY5 (char*)"PY5"
#define MED_NOM_P13 (char*)"P13"  

#define MED_NOM_GEO (char*)"GEO"
#define MED_NOM_GAU (char*)"GAU"
#define MED_NOM_NGA (char*)"NGA"
#define MED_NOM_N (char*)"N"
#define MED_NOM_PFL (char*)"PFL"
#define MED_NOM_NDT (char*)"NDT"
#define MED_NOM_PDT (char*)"PDT"
#define MED_NOM_NOR (char*)"NOR"

/* Integration des developpements OCC */
#define MED_NOM_GRD (char*)"GRD"
#define MED_NOM_BOF (char*)"BOF"
#define MED_NOM_IN1 (char*)"IN1"
#define MED_NOM_IN2 (char*)"IN2"
#define MED_NOM_IN3 (char*)"IN3"

/* Nom du DATA GROUP CONTENANT TOUS LES MAILLAGES DU FICHIER HDF */
#define MED_MAA (char*)"/ENS_MAA/"
#define MED_TAILLE_MAA 9

/* Nom du data group ou ranger les champs solution */
#define MED_CHA (char*)"/CHA/"
#define MED_TAILLE_CHA 5

/* Nom du data group ou ranger les familles */
#define MED_FAS (char*)"/FAS/"
#define MED_TAILLE_FAS 5

/* Nom du data group ou ranger les equivalences */
#define MED_EQS (char*)"/EQS/"
#define MED_TAILLE_EQS 5

/* Nom du data groupe contenant les profils */
#define MED_PROFILS (char*)"/PROFILS/"
#define MED_TAILLE_PROFILS 9 

/*Pour eviter le bug solaris*/
#include <malloc.h>

/* Interface des routines du composant tools */
#include "med_misc.hxx"
#include "med_hdfi.hxx"
#include "med_utils.hxx"
#endif /* MED_OUTILS_H */
