// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
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

#include "MEDMEM_MEDMEMchampLire.hxx"
/*
 * En attendant une correction de la gestion du mode d'accès au fichier dans MEDfichier
 * on intègre la correction ici.
 */

namespace med_2_3 {
  extern "C" {

# define ICI                    {                                       \
      fflush(stdout);                                                   \
      fprintf(stderr, "%s [%d] : " , __FILE__ , __LINE__ ) ;            \
      fflush(stderr) ;                                                  \
    }

# define ISCRUTE_MED(entier)        {                                   \
      ICI ;                                                             \
      fprintf(stderr,"%s = %d\n",#entier,entier) ;                      \
      fflush(stderr) ;                                                  \
    }

# define SSCRUTE_MED(chaine)        {                                   \
      ICI ;                                                             \
      fprintf(stderr,"%s = \"%s\"\n",#chaine,chaine) ;                  \
      fflush(stderr) ;                                                  \
    }
# define MESSAGE_MED(chaine)        {                                   \
      ICI ;                                                             \
      fprintf(stderr,"%s\n",chaine) ;                                   \
      fflush(stderr) ;                                                  \
    }

    extern void _MEDmodeErreurVerrouiller(void);

#include <med.h>
#include <med_outils.h>

#include <string.h>
#include <stdlib.h>

    /*
     * - Nom de la fonction : MEDchampLire
     * - Description : Lecture d'un Champ Résultat
     * - Parametres :
     *     - fid      (IN)  : ID du fichier HDF courant
     *     - maa      (IN)  : le nom du maillage sur lequel s'applique le champ
     *     - cha      (IN)  : le nom du champ
     *     - val      (OUT) : valeurs du champ à lire
     *     - interlace(IN)  : entrelacement voulu en mémoire {MED_FULL_INTERLACE,MED_NO_INTERLACE}
     *     - numco    (IN)  : n° de la composante à lire (MED_ALL si toutes)
     *     - profil   (OUT) : nom du profil utilisé (MED_NOPFL si inutilisé)
     *     - pflmod   (IN)  : Indique comment lire les informations en mémoire { MED_COMPACT, MED_GLOBALE }.
     *     - type_ent (IN)  : entité concerné par le champ {MED_NOEUD,MED_ARETE,MED_FACE,MED_MAILLE}
     *     - type_geo (IN)  : type géométrique de l'entité concerné {MED_POINT,MED_SEG2 ......}
     *     - numdt    (IN)  : n° du pas de temps (MED_NOPDT si aucun)
     *     - numo     (IN)  : n° d'ordre utilisé MED_NONOR si inutile
     * - Resultat : 0 en cas de succes, -1 sinon
     */

    /* REM : La taille de val allouée par l'utilisateur doit prendre en compte le nbre de points de gauss et le nbre de composantes*/

    med_err
    MEDMEMchampLire(med_idt fid,char *maa, char *cha, unsigned char *val,med_mode_switch interlace,med_int numco,
                 char * locname, char *profil, med_mode_profil pflmod,
                 med_entite_maillage type_ent, med_geometrie_element type_geo,
                 med_int numdt, med_int numo)

    {
      med_err ret=-1;
      med_idt gid=0, datagroup1=0, datagroup2=0,datagroup3=0;
      med_int ncomp=0, chtype=0, ngauss=0, i=0, pfluse=0;
      char nomdatagroup1[2*MED_TAILLE_NOM_ENTITE+2]="",nomdatagroup2[2*MED_MAX_PARA+1]="";
      char tmp1[MED_TAILLE_NOM_ENTITE+1]="", pfltmp[MED_TAILLE_NOM+1]="";
      char chemin[MED_TAILLE_CHA+MED_TAILLE_NOM+1]="";
      med_size   psize=0;
      med_int   *pfltabtmp=0;
      med_ssize *pfltab=0;

      /*
       * On inhibe le gestionnaire d'erreur HDF 5
       */
      _MEDmodeErreurVerrouiller();

      /*
       * Si le Data Group cha n'existe pas => erreur
       */
      strcpy(chemin,MED_CHA);
      strcat(chemin,cha);
      if ((gid = _MEDdatagroupOuvrir(fid,chemin)) < 0)
        goto ERROR;

      /* Lecture du nbre de composantes du champ */

      if (_MEDattrEntierLire(gid,MED_NOM_NCO,&ncomp) < 0)
        goto ERROR;

      /*
       * Si le Data Group  de niveau 1 <type_ent>[.<type_geo>] n'existe pas => erreur
       */
      if (_MEDnomEntite(nomdatagroup1,type_ent) < 0)
        goto ERROR;
      if ((type_ent != MED_NOEUD))
        {
          if (_MEDnomGeometrie(tmp1,type_geo) < 0)
            goto ERROR;
          strcat(nomdatagroup1,".");
          strcat(nomdatagroup1,tmp1);
        }
      datagroup1 = 0;
      if ( (datagroup1 = _MEDdatagroupOuvrir(gid,nomdatagroup1)) < 0 )
        goto ERROR;

      /*
       * Si le Data Group de niveau 2 <numdt>.<numoo> n'existe pas => erreur
       */
      sprintf(nomdatagroup2,"%*li%*li",MED_MAX_PARA,(long ) numdt,MED_MAX_PARA,(long ) numo);

      datagroup2 = 0;
      if ( (datagroup2 = _MEDdatagroupOuvrir(datagroup1,nomdatagroup2)) < 0)
        goto ERROR;

      /*
       * Ouvre le datagroup de niveau 3 <maa>
       */

      if ( ! strcmp(maa,MED_NOREF) )
        if (_MEDattrStringLire(datagroup2,MED_NOM_MAI,MED_TAILLE_NOM,maa) < 0)
          goto ERROR;

      datagroup3 = 0;
      if ( (datagroup3 = _MEDdatagroupOuvrir(datagroup2,maa)) < 0 )
        goto ERROR;

      /* Gestion des profils*/

      /*
       * Lire le profil
       */

      if (_MEDattrStringLire(datagroup3,MED_NOM_PFL,MED_TAILLE_NOM,pfltmp) < 0)
        goto ERROR;

      if ( pfluse = (strcmp(pfltmp,MED_NOPFLi) && strcmp(pfltmp,"")) ) /* le test MED_NOPFLi pour des raisons de compatibilité */
        {
          strcpy(profil,pfltmp);
          if ( (i = MEDnValProfil(fid,profil)) < 0 )
            goto ERROR;
          else
            psize = i;

          pfltabtmp = (med_int *)   malloc (sizeof(med_int)*psize);
          pfltab = (med_ssize *) malloc (sizeof(med_ssize)*psize);
          if (MEDprofilLire(fid,pfltabtmp,profil) < 0)
            goto ERROR;
          for (i=0;i<psize;i++)
            pfltab[i] = (med_ssize) pfltabtmp[i];

        }
      else {
        psize = MED_NOPF;
        strcpy(profil,MED_NOPFL);
      }


      /* Lire le nbre des points de GAUSS*/
      if (_MEDattrEntierLire(datagroup3,MED_NOM_NGA,&ngauss) < 0) {
        MESSAGE_MED("Erreur à la lecture de l'attribut MED_NOM_NGA : ");
        ISCRUTE_MED(ngauss);goto ERROR;
      };

      /* Lire l'identificateur de localisation des points de GAUSS*/
      if ( _MEDattrStringLire(datagroup3,MED_NOM_GAU,MED_TAILLE_NOM,locname) < 0) {
        MESSAGE_MED("Erreur à la lecture de l'attribut MED_NOM_GAU : ");
        SSCRUTE_MED(locname); goto ERROR;
      }

      /*
       * Lecture du champ
       */

      if (_MEDattrEntierLire(gid,MED_NOM_TYP,&chtype) < 0)
        goto ERROR;

      switch(chtype)
        {
        case MED_FLOAT64 :
          if ( _MEDdatasetNumLire(datagroup3,MED_NOM_CO,MED_FLOAT64,
                                  interlace,ncomp,numco,
                                  psize,pflmod,pfltab,ngauss,val)< 0)
            goto ERROR;
          break;

        case MED_INT32 :
#if defined(F77INT64)
          if ( _MEDdatasetNumLire(datagroup3,MED_NOM_CO,MED_INT64,
                                  interlace,ncomp,numco,
                                  psize,pflmod,pfltab,ngauss,val)< 0)
            goto ERROR;
#else
          if ( _MEDdatasetNumLire(datagroup3,MED_NOM_CO,MED_INT32,
                                  interlace,ncomp,numco,
                                  psize,pflmod,pfltab,ngauss,val)< 0)
            goto ERROR;
#endif
          break;

        case MED_INT64 :
#if defined(F77INT64)
          if ( _MEDdatasetNumLire(datagroup3,MED_NOM_CO,MED_INT64,
                                  interlace,ncomp,numco,
                                  psize,pflmod,pfltab,ngauss,val)< 0)
            goto ERROR;
#else
          goto ERROR;
#endif
          break;

        default :
          goto ERROR;
        }

      /*
       * On ferme tout
       */

      ret = 0;

    ERROR:

      if ( pfluse ) { free(pfltab); free(pfltabtmp);}

      if (datagroup3>0)     if (_MEDdatagroupFermer(datagroup3) < 0) {
        MESSAGE_MED("Impossible de fermer le datagroup : ");
        ISCRUTE_MED(datagroup3); ret = -1;
      }

      if (datagroup2>0)     if (_MEDdatagroupFermer(datagroup2) < 0) {
        MESSAGE_MED("Impossible de fermer le datagroup : ");
        ISCRUTE_MED(datagroup2); ret = -1;
      }

      if (datagroup1>0)     if (_MEDdatagroupFermer(datagroup1) < 0) {
        MESSAGE_MED("Impossible de fermer le datagroup : ");
        ISCRUTE_MED(datagroup1); ret = -1;
      }

      if (gid>0)     if (_MEDdatagroupFermer(gid) < 0) {
        MESSAGE_MED("Impossible de fermer le datagroup : ");
        ISCRUTE_MED(gid); ret = -1;
      }

      return ret;
    }


#undef MESSAGE_MED
#undef SSCRUTE_MED
#undef ISCRUTE_MED

  }
}


