//  Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "MEDMEM_Compatibility21_22.hxx"

using namespace MEDMEM;
using namespace MED_EN;

//================================================================================
/*!
 * \brief return file mode access corresponding to MED_EN::med_mode_acces in given med version
  * \param mode - either MED_LECT, MED_ECRI or MED_REMP
  * \param medVersion - V21 or V22 or ??
  * \retval int - file mode access
  *
  * To be used in MEDouvrir() call
 */
//================================================================================

int MEDMEM::getMedAccessMode(MED_EN::med_mode_acces mode,
                             MED_EN::medFileVersion medVersion)
  throw (MEDEXCEPTION)
{
  switch ( medVersion ) {
  case V21:
/*
  from MEDouvrir.c:
     switch(mode_acces)
    {
    case MED_LECT :
      if (access(nom,F_OK))
        return -1;
      else 
        if ((fid = _MEDfichierOuvrir(nom,mode_acces)) < 0)
          return -1;
      break;

    case MED_ECRI :
      if (access(nom,F_OK))
        {
          if ((fid = _MEDfichierCreer(nom)) < 0)
            return -1;
        }
      else
        if ((fid = _MEDfichierOuvrir(nom,mode_acces)) < 0)
          return -1;
      break;

    case MED_REMP :
      if ((fid = _MEDfichierCreer(nom)) < 0)
        return -1;
      break;
*/
    switch ( mode ) {
    case MED_EN::WRONLY: return med_2_1::MED_ECRI;
    case MED_EN::RDONLY: return med_2_1::MED_LECT;
    case MED_EN::RDWR:   return med_2_1::MED_ECRI; //REMP; -- pb with RDWR drivers
    default:
      throw MEDEXCEPTION("getMedAccessMode(): Wrong access mode");
    }
  case V22:
/*
  from med.h:
   MED_LECTURE          : Ouverture en lecture seule
   MED_LECTURE_ECRITURE : Ouverture en lecture/ecriture, si un élément existe il est écrasé
   MED_LECTURE_AJOUT    : Ouverture en lecture/ecriture, si un élément existe une erreur est générée
   MED_CREATION         : Créer le fichier s'il n'existe pas, l'écrase sinon
*/
    switch ( mode ) {
    case MED_EN::WRONLY: return med_2_3::MED_LECTURE_ECRITURE;// be coherent with V21 /*MED_CREATION;*/
    case MED_EN::RDONLY: return med_2_3::MED_LECTURE;
    case MED_EN::RDWR:   return med_2_3::MED_LECTURE_ECRITURE;
    default:
      throw MEDEXCEPTION("getMedAccessMode(): Wrong access mode");
    }
  default:;
  }
  throw MEDEXCEPTION("getMedAccessMode(): Unknown med version");
}
