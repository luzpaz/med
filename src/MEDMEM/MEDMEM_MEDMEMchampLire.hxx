#ifndef  MEDMEM_MEDMEM_CHAMPLIRE_HXX
#define  MEDMEM_MEDMEM_CHAMPLIRE_HXX
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
MEDMEMchampLire(med_idt fid,char *maa, char *cha, unsigned char *val,med_mode_switch interlace,med_int numco,
	     char * locname, char *profil, med_mode_profil pflmod, 
	     med_entite_maillage type_ent, med_geometrie_element type_geo,
	     med_int numdt, med_int numo);


  }
}

#endif
