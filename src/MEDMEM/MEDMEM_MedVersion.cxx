//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
#include "MEDMEM_MedVersion.hxx"
#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_Compatibility21_22.hxx"

using namespace MEDMEM;
using namespace MED_EN;

medFileVersion MEDMEM::getMedFileVersion(const string & fileName)
  throw (MEDEXCEPTION)
{
  medFileVersion version;

  med_2_1::med_idt fid21;
  med_2_3::med_idt fid22;

  med_2_1::med_err ret21;
  med_2_3::med_err ret22;

  med_2_1::med_int major21;
  med_2_1::med_int minor21;
  med_2_1::med_int release21;

  med_2_3::med_int major22;
  med_2_3::med_int minor22;
  med_2_3::med_int release22;

  med_2_3::med_mode_acces access22 = med_2_3::MED_LECTURE;
  med_2_1::med_mode_acces access21 = med_2_1::MED_LECT;

  /*
    Med Version 2.1 access to the file
  */

  fid21 = med_2_1::MEDouvrir((const_cast <char *> (fileName.c_str())),
			     access21);

  if (fid21 < 0)
    throw MEDEXCEPTION("Problem in getMedFileVersion(const string &) Med file V2.1 access");

  ret21 = med_2_1::MEDversionLire(fid21,&major21,&minor21,&release21);

  if (ret21 < 0)
    throw MEDEXCEPTION("Problem in getMedFileVersion(const string &) Med file V2.1 version numbers reading");

  if ((minor21 == -1) || (release21 == -1))
    {
      MESSAGE_MED("getMedFileVersion the file may have been produced by a version 2.1.x x<5");
      minor21 = 1;
      release21 = 5;
    }

  ret21 = med_2_1::MEDfermer(fid21);

  if (ret21 < 0)
    throw MEDEXCEPTION("Problem in getMedFileVersion(const string &) Med file V2.1 file closing");

  /*
    Med Version 2.3 access to the file
  */

  fid22 = med_2_3::MEDouvrir((const_cast <char *> (fileName.c_str())),
			     access22);

  if (fid22 < 0)
    throw MEDEXCEPTION("Problem in getMedFileVersion(const string &) Med file V2.2 access");

  ret22 = med_2_3::MEDversionLire(fid22,&major22,&minor22,&release22);

  if (ret22 < 0)
    throw MEDEXCEPTION("Problem in getMedFileVersion(const string &) Med file V2.2 version numbers reading");

  if ((minor22 == -1) || (release22 == -1))
    {
      MESSAGE_MED("getMedFileVersion the file may have been produced by a version 2.1.x x<5");
      minor22 = 1;
      release22 = 5;
    }

  ret22 = med_2_3::MEDfermer(fid22);

  if (ret22 < 0)
    throw MEDEXCEPTION("Problem in getMedFileVersion(const string &) Med file V2.2 file closing");

  if ((major21 != major22) || (minor21 != minor22) || (release21 != release22))
    throw MEDEXCEPTION("Problem in getMedFileVersion(const string &) Med file V21 and V22 version numbers are different");

  MESSAGE_MED("getMedFileVersion: status version 21 of the file major " << major21 << " minor " << minor21 << " release " << release21);

  MESSAGE_MED("getMedFileVersion: status version 22 of the file major " << major22 << " minor " << minor22 << " release " << release22);

  if (major21 == 2)
    {
      if (minor21 == 1)
	version = V21;
      else if (minor21 > 1)
	version = V22;
    }
  else
    version = V22;

  MESSAGE_MED("getMedFileVersion the version of the file is " << version);

  return version;
}

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

//int MEDMEM::getMedAccessMode(MED_EN::med_mode_acces mode,
//                             MED_EN::medFileVersion medVersion)
//  throw (MEDEXCEPTION)
//{
//  switch ( medVersion ) {
//  case V21:
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
//    switch ( mode ) {
//    case MED_EN::MED_LECT: return med_2_1::MED_LECT;
//    case MED_EN::MED_ECRI: return med_2_1::MED_ECRI;
//    case MED_EN::MED_REMP: return med_2_1::MED_ECRI;
//    default:
//      throw MEDEXCEPTION("getMedAccessMode(): Wrong access mode");
//    }
//  case V22:
/*
  from med.h:
   MED_LECTURE          : Ouverture en lecture seule
   MED_LECTURE_ECRITURE : Ouverture en lecture/ecriture, si un élément existe il est écrasé
   MED_LECTURE_AJOUT    : Ouverture en lecture/ecriture, si un élément existe une erreur est générée
   MED_CREATION         : Créer le fichier s'il n'existe pas, l'écrase sinon
*/
//    switch ( mode ) {
//    case MED_EN::MED_LECT: return med_2_3::MED_LECTURE;
//    case MED_EN::MED_ECRI: return med_2_3::MED_LECTURE_ECRITURE;
//    case MED_EN::MED_REMP: return med_2_3::MED_LECTURE_ECRITURE;
//		case MED_EN::MED_CREA: return med_2_3::MED_CREATION;
//    default:
//      throw MEDEXCEPTION("getMedAccessMode(): Wrong access mode");
//    }
//  default:;
//  }
//  throw MEDEXCEPTION("getMedAccessMode(): Unknown med version");
//}
