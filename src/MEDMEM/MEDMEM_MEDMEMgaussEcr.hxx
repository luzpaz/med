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

#ifndef  MEDMEM_MEDMEMGAUSSECR_HXX
#define  MEDMEM_MEDMEMGAUSSECR_HXX

#include "MEDMEM.hxx"

/*
 * En attendant une correction de la gestion du mode d'accès au fichier dans MEDfichier
 * on intègre la correction ici.
 */
namespace med_2_3 {
  extern "C" {

# define __UTILITES_H__
#include <med_outils.h>
# undef __UTILITES_H__

#include <string.h>
#include <stdlib.h>

MEDMEM_EXPORT med_err
MEDMEMgaussEcr(med_idt fid, med_geometrie_element type_geo, med_float *refcoo, med_mode_switch mode_coo,
            med_int ngauss, med_float *gscoo, med_float * wg, char * locname );

  }
}

#endif
