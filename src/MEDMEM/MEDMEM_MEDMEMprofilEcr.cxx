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

#include "MEDMEM_MEDMEMprofilEcr.hxx"
/*
 * En attendant une correction de la gestion du mode d'accès au fichier dans MEDfichier
 * on intègre la correction ici.
 */

namespace med_2_3 {
  extern "C" {

# define ICI                    {                       \
      fflush(stdout);                                                   \
      fprintf(stderr, "%s [%d] : " , __FILE__ , __LINE__ ) ;            \
      fflush(stderr) ;                                                  \
    }

# define ISCRUTE_MED(entier)        {           \
      ICI ;                                                             \
      fprintf(stderr,"%s = %d\n",#entier,entier) ;                      \
      fflush(stderr) ;                                                  \
    }

# define SSCRUTE_MED(chaine)        {           \
      ICI ;                                                             \
      fprintf(stderr,"%s = \"%s\"\n",#chaine,chaine) ;                  \
      fflush(stderr) ;                                                  \
    }
# define MESSAGE_MED(chaine)        {           \
      ICI ;                                                             \
      fprintf(stderr,"%s\n",chaine) ;                                   \
      fflush(stderr) ;                                                  \
    }

    extern void _MEDmodeErreurVerrouiller(void);

#include <med.h>
#include <med_outils.h>

#include <string.h>
#include <stdlib.h>

    med_err
    MEDMEMprofilEcr(med_idt fid,med_int *pflval,med_int n,char *profilname)
    {
      med_idt gid, chid;
      med_size dimd[1];
      med_err ret;
      char chemin[MED_TAILLE_PROFILS+1];

      /*
       * On inhibe le gestionnaire d'erreur HDF 5
       */
      _MEDmodeErreurVerrouiller();

      /*
       * Si le groupe "PROFILS" n'existe pas, on le cree
       */
      strncpy(chemin,MED_PROFILS,MED_TAILLE_PROFILS-1);
      chemin[MED_TAILLE_PROFILS-1] = '\0';
      if ((gid = _MEDdatagroupOuvrir(fid,chemin)) < 0)
        if ((gid = _MEDdatagroupCreer(fid,chemin)) < 0) {
          MESSAGE_MED("Impossible de creer le groupe MED_PROFILS : ");
          SSCRUTE_MED(chemin); goto ERROR;
        }

      /*
       * Si le groupe "profilname" n'existe pas, on le cree
       * Sinon => erreur
       */
      if ((chid = _MEDdatagroupOuvrir(gid,profilname)) >= 0) {
        if ( false )//MED_MODE_ACCES != MED_LECTURE_ECRITURE )
  {
          MESSAGE_MED("Le profil existe déjà : ");
          SSCRUTE_MED(profilname); goto ERROR;
        }
      } else
        if ((chid = _MEDdatagroupCreer(gid,profilname)) < 0)
          goto ERROR;

      /*
       * On stocke "n" sous forme d'attribut
       */
      if ((ret = _MEDattrEntierEcrire(chid,MED_NOM_NBR,&n)) < 0) {
        MESSAGE_MED("Erreur à l'écriture de l'attribut MED_NOM_NBR : ");
        ISCRUTE_MED(n); goto ERROR;
      };

      /*
       * On stocke le profil dans un dataset
       */
      dimd[0] = n;
#if defined(F77INT64)
      ret =  _MEDdatasetNumEcrire(chid,MED_NOM_PFL,MED_INT64,MED_NO_INTERLACE,MED_DIM1,MED_ALL,MED_NOPF,MED_NO_PFLMOD,0,MED_NOPG,dimd,
                                  (unsigned char*) pflval);
#else
      ret =  _MEDdatasetNumEcrire(chid,MED_NOM_PFL,MED_INT32,MED_NO_INTERLACE,MED_DIM1,MED_ALL,MED_NOPF,MED_NO_PFLMOD,0,MED_NOPG,dimd,
                                  (unsigned char*) pflval);
#endif
      if (ret < 0 ) {
        MESSAGE_MED("Impossible d'ecrire le dataset pflval de taille  : ");
        ISCRUTE_MED(n); goto ERROR;
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




