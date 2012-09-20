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

#ifndef MED_PROTO_H
#define MED_PROTO_H

#ifdef WIN32
 #if defined MEDWRAPPER_V2_1_CORE_EXPORTS || defined med_V2_1_EXPORTS
  #if defined WIN32
   #define MEDWRAPPER_V2_1_CORE_EXPORT __declspec( dllexport )
  #else
   #define MEDWRAPPER_V2_1_CORE_EXPORT
  #endif
 #else
  #if defined WIN32
   #define MEDWRAPPER_V2_1_CORE_EXPORT __declspec( dllimport )
  #else
   #define MEDWRAPPER_V2_1_CORE_EXPORT
  #endif
 #endif
#else
 #define MEDWRAPPER_V2_1_CORE_EXPORT
#endif

/* Interface de l'API MED */
namespace med_2_1{
/* Fichier */
MEDWRAPPER_V2_1_CORE_EXPORT extern med_idt
MEDouvrir(char *nom, med_mode_acces mode_acces);
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDfermer(med_idt fid); 
MEDWRAPPER_V2_1_CORE_EXPORT extern med_int
MEDlFichDes(med_idt fid);
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDfichEntete(med_idt fid, med_fich_info quoi, char str[]); 
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDfichDesEcr(med_idt fid, char *des, med_mode_acces mode); 
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err
MEDunvCr(med_idt fid, char *maa);
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err
MEDunvLire(med_idt fid, char *maa,char *nomu);
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err
MEDformatConforme(const char * nomfich);
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err
MEDversionConforme(const char *nom);
MEDWRAPPER_V2_1_CORE_EXPORT extern void
MEDversionDonner(med_int *majeur, med_int *mineur, med_int *release);
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDversionLire(med_idt fid, med_int *majeur, med_int *mineur, med_int *release);



/* Maillage */
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDmaaCr(med_idt fid, char *maillage, med_int dim);
MEDWRAPPER_V2_1_CORE_EXPORT extern med_int
MEDdimLire(med_idt fid, char *maillage); 
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDmaaInfo(med_idt fid, int indice, char *maillage, med_int *dim); 
MEDWRAPPER_V2_1_CORE_EXPORT extern med_int 
MEDnMaa(med_idt fid);
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDnbnosoEcr(med_idt fid, char *nom_maillage,med_int n);
MEDWRAPPER_V2_1_CORE_EXPORT extern med_int
MEDnbnosoLire(med_idt fid,char *nom_maillage);
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDnbnoisEcr(med_idt fid, char *nom_maillage,med_int n);
MEDWRAPPER_V2_1_CORE_EXPORT extern med_int
MEDnbnoisLire(med_idt fid,char *nom_maillage);
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err
MEDnbnomaEcr(med_idt fid, char *nom_maillage,med_int n);
MEDWRAPPER_V2_1_CORE_EXPORT extern med_int
MEDnbnomaLire(med_idt fid,char *nom_maillage);

/* EntMaillage */
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDconnEcr(med_idt fid,char *maa, med_int mdim, med_int *connectivite,med_mode_switch mode_switch,
           med_int nbre,med_mode_acces mode,med_entite_maillage type_ent,
           med_geometrie_element type_geo,med_connectivite type_conn);

MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDconnLire(med_idt fid,char *maa,med_int mdim,med_int *connectivite,med_mode_switch mode_switch,
            med_int * pfltab, med_size psize,
            med_entite_maillage type_ent, med_geometrie_element type_geo,med_connectivite type_conn);
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDnomEcr(med_idt fid,char *maa, char *nom, med_int n, med_mode_acces mode,
          med_entite_maillage type_ent,med_geometrie_element type_geo); 
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDnomLire(med_idt fid,char *maa, char *nom, med_int n, 
           med_entite_maillage type_ent,med_geometrie_element type_geo); 
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDnumLire(med_idt fid,char *maa, med_int *num, med_int n, 
           med_entite_maillage type_ent,med_geometrie_element type_geo); 
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDnumEcr(med_idt fid,char *maa, med_int *num, med_int n, med_mode_acces mode,
          med_entite_maillage type_ent,med_geometrie_element type_geo);
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDcoordEcr(med_idt fid, char *maa, med_int mdim, med_float *coo, 
            med_mode_switch mode_coo,med_int n,
            med_mode_acces mode, med_repere type_rep, char *nom, char *unit);
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDcoordLire(med_idt fid, char *maa, med_int mdim, med_float *coo,
             med_mode_switch mode_coo,med_int numco,
             med_int * pfltab, med_size psize, med_repere *type_rep, char *nom, char *unit);

MEDWRAPPER_V2_1_CORE_EXPORT extern med_int
MEDnEntMaa(med_idt fid, char *maa, med_table quoi, med_entite_maillage type_ent, 
           med_geometrie_element type_geo, med_connectivite type_conn); 


/* Resultat */
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err MEDchampCr(med_idt fid, char *champ, med_type_champ type, char *comp,
                   char *unit,med_int ncomp);

MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDchampEcr(med_idt fid, char *maa, char *cha,unsigned char *val,med_mode_switch interlace,med_int nbelem,med_int ngauss, 
            med_int numco, char * profil, med_mode_acces mode, med_entite_maillage type_ent, 
            med_geometrie_element type_geo, med_int numdt,char * dt_unit, med_float dt, med_int numo);

MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDchampLire(med_idt fid,char *maa, char *cha, unsigned char *val,med_mode_switch interlace,med_int numco,
             char *profil,med_entite_maillage type_ent, med_geometrie_element type_geo,
             med_int numdt, med_int numo);
 
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err
MEDchampInfo(med_idt fid,int indice,char *champ,
                     med_type_champ *type,char *comp,char *unit, 
                     med_int ncomp);

MEDWRAPPER_V2_1_CORE_EXPORT extern med_int 
MEDnChamp(med_idt fid, int indice); 

MEDWRAPPER_V2_1_CORE_EXPORT extern med_int
MEDnVal(med_idt fid, char *champ, med_entite_maillage typ_ent, 
        med_geometrie_element typ_geo,med_int numdt, med_int numo);


/* Famille  */
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDfamEcr(med_idt fid,char *maa, med_int *fam, med_int n, med_mode_acces mode,
          med_entite_maillage type_ent, med_geometrie_element type_geo); 
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDfamLire(med_idt fid,char *maa, med_int *fam, med_int n, 
           med_entite_maillage type_ent,med_geometrie_element type_geo);
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDfamCr(med_idt fid,char* maa,char *famille,med_int numero, 
         med_int *attr_ident, med_int *attr_val,char *attr_desc,med_int n_attr,
         char *groupe , med_int n_groupe);
MEDWRAPPER_V2_1_CORE_EXPORT extern med_int 
MEDnFam(med_idt fid,char *maa, int indice, med_dim_famille quoi);
 
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDfamInfo(med_idt fid,char *maa,int indice, char *famille, 
           med_int *numero,
           med_int *attr_ident, med_int *attr_val, char *attr_desc,
           med_int *n_attr,char *groupe ,med_int *n_groupe); 
 
/* Equivalence    */
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDequivCr(med_idt fid,char *maa, char *eq, char *desc); 
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDequivLire(med_idt fid, char *maa, char *eq, med_int *corr, med_int n,
            med_entite_maillage typ_ent,med_geometrie_element typ_geo); 
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDequivEcr(med_idt fid, char *maa, char *eq, med_int *corr, med_int n, 
            med_mode_acces mode, med_entite_maillage typ_ent, med_geometrie_element typ_geo); 
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDequivInfo(med_idt fid, char *maa, int ind, char *eq, char *des);
MEDWRAPPER_V2_1_CORE_EXPORT extern med_int 
MEDnEquiv(med_idt fid, char *maa);
MEDWRAPPER_V2_1_CORE_EXPORT extern med_int 
MEDnCorres(med_idt fid,char *maa,char *eq,med_entite_maillage typ_ent,
           med_geometrie_element typ_geo); 


/* Routines de niveau intermediaire */
MEDWRAPPER_V2_1_CORE_EXPORT extern med_int
MEDnEntites(med_idt fid,char *maa,med_entite_maillage typ_ent, 
            med_connectivite typ_con);

MEDWRAPPER_V2_1_CORE_EXPORT extern med_err
MEDnoeudsLire(med_idt fid,char *maa,med_int mdim, med_float *coord,
              med_mode_switch mode_coo,
              med_repere *repere,char *nomcoo, char *unicoo,char *nom,
              med_booleen *inom,med_int *num,med_booleen *inum,med_int *fam,
              med_int nnoeuds);

MEDWRAPPER_V2_1_CORE_EXPORT extern med_err
MEDnoeudsEcr(med_idt fid,char *maa,med_int mdim,med_float *coord,
             med_mode_switch mode_coo,
             med_repere repere,char *nomcoo, char *unicoo,char *nom,
             med_booleen inom,med_int *num,med_booleen inum,med_int *fam,
             med_int nnoeuds,med_mode_acces mode);
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err
MEDelementsEcr(med_idt fid,char *maa,med_int mdim,med_int *connectivite,med_mode_switch mode_switch,
               char *nom,med_booleen inom,med_int *num,med_booleen inum,
               med_int *fam,med_int nele,med_entite_maillage typ_ent, 
               med_geometrie_element typ_geo,med_connectivite typ_conn,med_mode_acces mode);
MEDWRAPPER_V2_1_CORE_EXPORT extern med_err
MEDelementsLire(med_idt fid,char *maa,med_int mdim,med_int *connectivite,med_mode_switch mode_switch,
               char *nom,med_booleen *inom,med_int *num,med_booleen *inum,
               med_int *fam,med_int nele,med_entite_maillage typ_ent, 
               med_geometrie_element typ_geo,med_connectivite typ_conn);

/* Routines de haut niveau    */

/*(? On enlève le reste ?)*/

MEDWRAPPER_V2_1_CORE_EXPORT extern med_err
MEDfamMaaInfo(med_idt fid,char *maa,med_int *nfam,med_int *nattc,
              med_int *ngroc);

MEDWRAPPER_V2_1_CORE_EXPORT extern med_err
MEDfamMaaLire(med_idt fid,char *maa,
              med_int *numfam,med_int *attide,
              med_int *attval,char *attdes,int *indatt,char *gro,int *indgro,
              med_int nfamilles);

MEDWRAPPER_V2_1_CORE_EXPORT extern med_err
MEDfamMaaCr(med_idt fid,char *maa,
            med_int *numfam,med_int *attide,
            med_int *attval,char *attdes,int *indatt,char *gro,int *indgro,
            med_int nfamilles);

/* Routines concernant les profils */

MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDprofilInfo(med_idt fid, int indice, char *profil, med_int *n); 

MEDWRAPPER_V2_1_CORE_EXPORT extern med_int 
MEDnProfil(med_idt fid);

MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDprofilEcr(med_idt fid,med_int *pflval,med_int n,char *nom);

MEDWRAPPER_V2_1_CORE_EXPORT extern med_int 
MEDnValProfil(med_idt fid, char *nom);

MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDprofilLire(med_idt fid, med_int *pflval, char *nom);

/* Routines concernant les pas de temps/ numéros d'ordre */

MEDWRAPPER_V2_1_CORE_EXPORT extern med_int
MEDnPasdetemps(med_idt fid,char *cha,med_entite_maillage type_ent, 
               med_geometrie_element type_geo);

MEDWRAPPER_V2_1_CORE_EXPORT extern med_err 
MEDpasdetempsInfo(med_idt fid,char *champ
                  ,med_entite_maillage type_ent, med_geometrie_element type_geo,
                  int indice, char *maa, med_int * ngauss, med_int * numdt, char * dt_unit, med_float * dt, 
                  med_int * numo);

/* Grilles */

MEDWRAPPER_V2_1_CORE_EXPORT med_int MEDnGrid(med_idt fid, char *maa, med_grid n);
MEDWRAPPER_V2_1_CORE_EXPORT med_err MEDgridCr(med_idt fid, char *maillage, med_int dim, med_grid_type typ);
MEDWRAPPER_V2_1_CORE_EXPORT med_err MEDgridInfo(med_idt fid, int indice, med_int *isAGrid, med_grid_type *typ);
MEDWRAPPER_V2_1_CORE_EXPORT med_err MEDgridEcr(
        med_idt fid, char *maa, med_int mdim, med_float *coo, med_int nb, med_int dim, med_mode_switch mode_coo,
        med_repere repere, char *nomcoo, char *unicoo, med_mode_acces mode );
MEDWRAPPER_V2_1_CORE_EXPORT med_err MEDgridLire(
        med_idt fid, char *maa, med_int mdim, med_float *coo, med_int dim, med_mode_switch mode_coo,
        med_repere *repere, char *nomcoo, char *unicoo );
MEDWRAPPER_V2_1_CORE_EXPORT med_err MEDfamGridEcr(med_idt fid, char *maa, med_int *fam, med_int n, med_mode_acces mode, med_entite_maillage type_ent);
MEDWRAPPER_V2_1_CORE_EXPORT med_err MEDfamGridLire(med_idt fid, char *maa, med_int *fam, med_int n, med_entite_maillage type_ent);
MEDWRAPPER_V2_1_CORE_EXPORT med_err MEDbodyFittedEcr(
        med_idt fid, char *maa, med_int mdim, med_float *coo, med_int *nbr, med_mode_switch mode_coo,
        med_repere repere, char *nomcoo, char *unicoo, med_int *fam, med_int nnoeuds, med_mode_acces mode );
MEDWRAPPER_V2_1_CORE_EXPORT med_err MEDbodyFittedLire(
        med_idt fid, char *maa, med_int mdim, med_float *coo, med_mode_switch mode_coo,
        med_repere *repere, char *nomcoo, char *unicoo, med_int *fam, med_int nnoeuds );
}

#endif /* MED_PROTO_H */




