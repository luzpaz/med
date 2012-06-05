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

#include "MEDMEM_MEDMEMgaussEcr.hxx"
/*
 * En attendant une correction de la gestion du mode d'accès au fichier dans MEDfichier
 * on intègre la correction ici.
 */

namespace med_2_3 {
  extern "C" {
# define ICI                    {\
                                        fflush(stdout);\
                                        fprintf(stderr, "%s [%d] : " , __FILE__ , __LINE__ ) ;\
                                        fflush(stderr) ;\
                                }

# define ISCRUTE_MED(entier)        {\
                                        ICI ;\
                                        fprintf(stderr,"%s = %d\n",#entier,entier) ;\
                                        fflush(stderr) ;\
                                }

# define SSCRUTE_MED(chaine)        {\
                                        ICI ;\
                                        fprintf(stderr,"%s = \"%s\"\n",#chaine,chaine) ;\
                                        fflush(stderr) ;\
                                }
# define MESSAGE_MED(chaine)        {\
                                        ICI ;\
                                        fprintf(stderr,"%s\n",chaine) ;\
                                        fflush(stderr) ;\
                                }

    extern void _MEDmodeErreurVerrouiller(void);

    /*
     * - Nom de la fonction : MEDgaussEcr
     * - Description : Itérateur renvoyant (n°pdt,n°or), le nbre de point de GAUSS pour le type d'élément,
     *                 et le  maillage par défaut avec son eventuel lien à un autre fichier.
     * - Parametres :
     *     - fid      (IN) : ID du fichier HDF courant
     *     - type_geo (IN) : Le type géométrique de l'entité concerné {MED_POINT,MED_SEG2 ......}
     *     - refcoo   (IN) : Les coordonnées des noeuds de l'élément de référence (tableau de taille(typegeo%100)*(typegeo/100) )
     *     - mode_coo (IN) : Choix du type d'entrelacement utilisé en mémoire pour refcoo et gscoo
     *                        { MED_FULL_INTERLACE(x1,y1,z1,x2,...)) , MED_NO_INTERLACE(x1,x2,y1,y2,z1,z2) }
     *     - ngauss   (IN) : Nombre de points de Gauss pour l'élément de référence considéré
     *     - gscoo    (IN) : Les coordonnées des points de Gauss pour l'élément de référence (tableau de taille ngauss*type_geo/100)
     *     - wg       (IN) : Poids à appliquer aux points d'intégration (tableau de taille ngauss)
     *     - locname (IN) : Nom à associer à la localisation (MED_TAILLe_NOM)

     REM :
     <locname> est à passer en paramètre de MEDchampEcrire.
    */

    med_err
    MEDMEMgaussEcr(med_idt fid, med_geometrie_element type_geo, med_float *refcoo, med_mode_switch mode_coo,
                   med_int ngauss, med_float *gscoo, med_float * wg, char * locname )
    {
      med_idt gid=0, chid=0;
      med_size dimd[1];
      med_err ret = -1;
      med_int typegeo = -1;
      char chemin[MED_TAILLE_GAUSS+1]="";

      /*
       * On inhibe le gestionnaire d'erreur HDF 5
       */
      _MEDmodeErreurVerrouiller();

      /*
       * Si le groupe "GAUSS" n'existe pas, on le cree
       */
      strncpy(chemin,MED_GAUSS,MED_TAILLE_GAUSS-1);
      chemin[MED_TAILLE_GAUSS-1] = '\0';
      if ((gid = _MEDdatagroupOuvrir(fid,chemin)) < 0)
        if ((gid = _MEDdatagroupCreer(fid,chemin)) < 0) {
          MESSAGE_MED("Impossible de creer le groupe MED_GAUSS : ");
          SSCRUTE_MED(chemin); goto ERROR;
        }

      /*
       * Si le groupe <locname> n'existe pas, on le cree
       * Sinon => erreur
       */
      if ((chid = _MEDdatagroupOuvrir(gid,locname)) >= 0) {
        if ( false )//MED_MODE_ACCES != MED_LECTURE_ECRITURE )
  {
          MESSAGE_MED("Le nom de localisation existe déjà : ");
          SSCRUTE_MED(locname); goto ERROR;
        }
      } else
        if ((chid = _MEDdatagroupCreer(gid,locname)) < 0)
          goto ERROR;

      /*
       * On stocke <ngauss> sous forme d'attribut
       */
      if (_MEDattrEntierEcrire(chid,MED_NOM_NBR,&ngauss) < 0) {
        MESSAGE_MED("Erreur à l'écriture de l'attribut MED_NOM_NBR : ");
        ISCRUTE_MED(ngauss);goto ERROR;
      };

      /*
       * On stocke <type_geo> sous forme d'attribut
       */
      typegeo = (med_int) type_geo;
      /* sizeof(enum) tjrs = sizeof(int) en C, or
         sur machines 64 bits par défaut med_int==long,
         du coup sur  machines 64 bits _MEDattrEntierEcrire utilise
         le type hdf NATIVE_LONG, ce qui pose un problème qd on passe
         un enum.
      */
      if (_MEDattrEntierEcrire(chid,MED_NOM_GEO,&typegeo) < 0) {
        MESSAGE_MED("Erreur à l'écriture de l'attribut MED_NOM_GEO : ");
        ISCRUTE_MED(type_geo);goto ERROR;
      };


      /*
       * On stocke les coordonnées de référence dans un dataset
       */

      dimd[0] = (type_geo%100)*(type_geo/100);
      if ( _MEDdatasetNumEcrire(chid,MED_NOM_COO,MED_FLOAT64,mode_coo,(type_geo/100),MED_ALL,MED_NOPF,MED_NO_PFLMOD,0,MED_NOPG,dimd,
                                (unsigned char*) refcoo)  < 0 ) {
        MESSAGE_MED("Impossible d'ecrire le dataset : ");SSCRUTE_MED(MED_NOM_COO);
        ISCRUTE_MED(dimd); goto ERROR;
      }

      /*
       * On stocke les points d'intégration dans un dataset
       */

      dimd[0] = ngauss*(type_geo/100);
      if ( _MEDdatasetNumEcrire(chid,MED_NOM_GAU,MED_FLOAT64,mode_coo,(type_geo/100),MED_ALL,MED_NOPF,MED_NO_PFLMOD,0,MED_NOPG,dimd,
                                (unsigned char*) gscoo)  < 0 ) {
        MESSAGE_MED("Impossible d'ecrire le dataset : ");SSCRUTE_MED(MED_NOM_GAU);
        ISCRUTE_MED(dimd); goto ERROR;
      }

      /*
       * On stocke les poids dans un dataset
       */

      dimd[0] = ngauss;
      if ( _MEDdatasetNumEcrire(chid,MED_NOM_VAL,MED_FLOAT64,mode_coo,1,MED_ALL,MED_NOPF,MED_NO_PFLMOD,0,MED_NOPG,dimd,
                                (unsigned char*) wg)  < 0 ) {
        MESSAGE_MED("Impossible d'ecrire le dataset : ");SSCRUTE_MED(MED_NOM_VAL);
        ISCRUTE_MED(dimd); goto ERROR;
      }


      ret = 0;

    ERROR:

      /*
       * On ferme tout
       */

      if (chid>0)     if (_MEDdatagroupFermer(chid) < 0) {
        MESSAGE_MED("Impossible de fermer le datagroup : ");
        ISCRUTE_MED(chid); ret = -1;
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
