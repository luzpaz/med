#ifndef  MEDMEM_MEDMEM_GAUSSECR_HXX
#define  MEDMEM_MEDMEM_GAUSSECR_HXX
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
MEDMEMgaussEcr(med_idt fid, med_geometrie_element type_geo, med_float *refcoo, med_mode_switch mode_coo,
	    med_int ngauss, med_float *gscoo, med_float * wg, char * locname );

  }
}

#endif
