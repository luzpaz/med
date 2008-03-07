// Copyright (C) 2005  OPEN CASCADE, CEA, EDF R&D, LEG
//           PRINCIPIA R&D, EADS CCR, Lip6, BV, CEDRAT
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
#include "MEDMEM_MEDMEMprofilEcr.hxx"
/*
 * En attendant une correction de la gestion du mode d'accès au fichier dans MEDfichier
 * on intègre la correction ici.
 */

namespace med_2_3 {
  extern "C" {

# define ICI                    {			\
      fflush(stdout);							\
      fprintf(stderr, "%s [%d] : " , __FILE__ , __LINE__ ) ;		\
      fflush(stderr) ;							\
    }

# define ISCRUTE(entier)        {		\
      ICI ;								\
      fprintf(stderr,"%s = %d\n",#entier,entier) ;			\
      fflush(stderr) ;							\
    }

# define SSCRUTE(chaine)        {		\
      ICI ;								\
      fprintf(stderr,"%s = \"%s\"\n",#chaine,chaine) ;			\
      fflush(stderr) ;							\
    }
# define MESSAGE(chaine)        {		\
      ICI ;								\
      fprintf(stderr,"%s\n",chaine) ;					\
      fflush(stderr) ;							\
    }

    extern void _MEDmodeErreurVerrouiller(void);

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
	  MESSAGE("Impossible de creer le groupe MED_PROFILS : ");
	  SSCRUTE(chemin); goto ERROR;
	}

      /*
       * Si le groupe "profilname" n'existe pas, on le cree
       * Sinon => erreur
       */
      if ((chid = _MEDdatagroupOuvrir(gid,profilname)) >= 0) {
	if ( false )//MED_MODE_ACCES != MED_LECTURE_ECRITURE )
  {
	  MESSAGE("Le profil existe déjà : ");
	  SSCRUTE(profilname); goto ERROR;
	}
      } else
	if ((chid = _MEDdatagroupCreer(gid,profilname)) < 0)
	  goto ERROR;

      /*
       * On stocke "n" sous forme d'attribut
       */
      if ((ret = _MEDattrEntierEcrire(chid,MED_NOM_NBR,&n)) < 0) {
	MESSAGE("Erreur à l'écriture de l'attribut MED_NOM_NBR : ");
	ISCRUTE(n); goto ERROR;
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
	MESSAGE("Impossible d'ecrire le dataset pflval de taille  : ");
	ISCRUTE(n); goto ERROR;
      }

      ret = 0;
    ERROR:
      /*
       * On ferme tout
       */
      if (chid>0)     if (_MEDdatagroupFermer(chid) < 0) {
	MESSAGE("Impossible de fermer le datagroup : ");
	ISCRUTE(chid); ret = -1;
      }

      if (gid>0)     if (_MEDdatagroupFermer(gid) < 0) {
	MESSAGE("Impossible de fermer le datagroup : ");
	ISCRUTE(gid); ret = -1;
      }

      return ret;
    }

#undef MESSAGE
#undef SSCRUTE
#undef ISCRUTE

  }
}




