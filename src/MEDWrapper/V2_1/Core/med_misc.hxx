/*************************************************************************
* COPYRIGHT (C) 1999 - 2002  EDF R&D
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

#ifndef MED_MISC_H
#define MED_MISC_H
#include "med.hxx"


/* CHAINES DE CARACTERES FORTRAN => C */
namespace med_2_1{

MEDWRAPPER_V2_1_CORE_EXPORT extern
char *_MED1cstring(char *chaine,int longueur_reelle,int longueur_fixee);

MEDWRAPPER_V2_1_CORE_EXPORT extern
char *_MED2cstring(char *chaine, int longueur);

MEDWRAPPER_V2_1_CORE_EXPORT extern
med_err _MEDcstringFree(char *chaine);

MEDWRAPPER_V2_1_CORE_EXPORT extern
med_err _MEDfstring(char *chaine, med_int longueur_fixee);

/* Noms associes aux objets MED */
MEDWRAPPER_V2_1_CORE_EXPORT extern
med_err _MEDnomEntite(char *nom_ent,med_entite_maillage type_ent);

MEDWRAPPER_V2_1_CORE_EXPORT extern
med_err _MEDnomGeometrie(char *nom_geo,med_geometrie_element type_geo);

MEDWRAPPER_V2_1_CORE_EXPORT extern
med_err _MEDparametresGeometrie(med_entite_maillage type_ent, 
			       med_geometrie_element type_geo, int *dim, int *nnoe,
			       int *ndes);
MEDWRAPPER_V2_1_CORE_EXPORT extern
med_err _MEDnomDataset(char *nom_dataset,med_table quoi,
		       med_connectivite type_conn);

/* Geometrie des objets MED */
MEDWRAPPER_V2_1_CORE_EXPORT extern 
med_err _MEDGeometrieElement(med_geometrie_element typ_geo[],
			     med_entite_maillage typ_ent);

}

#endif /* MED_MISC_H */

