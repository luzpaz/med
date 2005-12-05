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

/*
 * - Nom de la fonction : _MEDattrStringEcrire
 * - Description : ecriture d'un attribut chaine de caracteres
 * - Parametres :
 *     - pere (IN)     : l'ID de l'objet HDF pere ou placer l'attribut
 *     - nom  (IN)     : le nom de l'attribut 
 *     - longueur (IN) : strlen(val)
 *     - val  (IN)     : la valeur de l'attribut
 * - Resultat : 0 en cas de succes, -1 sinon
 */ 

namespace med_2_1{

med_err 
_MEDattrStringEcrire(med_idt pere,char *nom,int longueur,char *val, med_mode_acces mode)
{
  med_idt aid,attr, datatype;
  med_err ret;

  if ((aid = H5Screate(H5S_SCALAR)) < 0)
    return -1;
  if((datatype = H5Tcopy(H5T_C_S1)) < 0)
    return -1;
  if((ret = H5Tset_size(datatype,longueur+1)) < 0)
    return -1;

  if ( ((attr = H5Aopen_name(pere,nom)) > 0)
       && (mode != MED_REMP) )
    return -1;
  else
    if ( attr < 0)
      if ((attr = H5Acreate(pere,nom,datatype,aid,H5P_DEFAULT)) < 0) return -1;
  
  if ((ret = H5Awrite(attr, datatype, val)) < 0)
    return -1;

  if ((ret = H5Sclose(aid)) < 0)
    return -1;
  if ((ret = H5Tclose(datatype)) < 0)
    return -1;
  if ((ret = H5Aclose(attr)) < 0)
    return -1;

  return 0;
}

}
