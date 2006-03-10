#ifndef  MEDMEM_MEDMEM_PROFILECR_HXX
#define  MEDMEM_MEDMEM_PROFILECR_HXX
/*
 * En attendant une correction de la gestion du mode d'accès au fichier dans MEDfichier
 * on intègre la correction ici.
 */
namespace med_2_2 {
  extern "C" {

# define __UTILITES_H__
#include <med_outils.h>
# undef __UTILITES_H__

#include <string.h>
#include <stdlib.h>

    med_err
    MEDMEMprofilEcr(med_idt fid,med_int *pflval,med_int n,char *profilname);

  }
}

#endif
