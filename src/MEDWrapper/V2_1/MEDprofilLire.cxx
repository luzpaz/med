// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
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
// See http://www.salome-platform.org/
//
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

#include "med.hxx"
#include "med_outils.hxx"

#include <string.h>
#include <stdlib.h>

namespace med_2_1{

med_err 
MEDprofilLire(med_idt fid,med_int *pflval, char *nom)
{
  med_err ret = 0;
  med_idt pid,did;
  char chemin[MED_TAILLE_PROFILS+MED_TAILLE_NOM+1]; 

  /*
   * On inhibe le gestionnaire d'erreur HDF 5
   */
  _MEDmodeErreurVerrouiller();

  /* 
   * ouverture du groupe /PROFILS/"nom"
   */  
  strcpy(chemin,MED_PROFILS);
  strcat(chemin,nom); 
  if ((pid = _MEDdatagroupOuvrir(fid,chemin)) < 0)
    return -1;

  /*
   * Lecture du profil
   */
#if defined(IRIX64)||defined(OSF1)
  if ((ret =  _MEDdatasetNumLire(pid,MED_NOM_PFL,MED_INT64,
				 MED_NO_INTERLACE,1,MED_ALL,
				 MED_NOPF,0,MED_NOPG,
				 (unsigned char *) pflval)) < 0)
    return -1;
#else
  if ((ret =  _MEDdatasetNumLire(pid,MED_NOM_PFL,MED_INT32,
				 MED_NO_INTERLACE,1,MED_ALL,
				 MED_NOPF,0,MED_NOPG,
				 (unsigned char *) pflval)) < 0)
    return -1;
#endif

  /*
   * On ferme tout
   */
  if ((ret = _MEDdatagroupFermer(pid)) < 0)
    return -1; 

  return ret;
}

}
