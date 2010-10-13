//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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

using namespace MEDMEM;
using namespace MED_EN;

medFileVersion MEDMEM::getMedFileVersion(const string & fileName)
  throw (MEDEXCEPTION)
{
  medFileVersion version;

  med_2_3::med_idt fid22;

  med_2_3::med_err ret22;

  med_2_3::med_int major22;
  med_2_3::med_int minor22;
  med_2_3::med_int release22;

  med_2_3::med_mode_acces access22 = med_2_3::MED_LECTURE;

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

  if (major22 == 2)
    {
      if (minor22 == 1)
        version = V21;
      else if (minor22 > 1)
        version = V22;
    }
  else
    version = V22;

  MESSAGE_MED("getMedFileVersion the version of the file is " << version);

  return version;
}

med_2_3::med_mode_acces MEDMEM::getMedAccessMode(MED_EN::med_mode_acces mode)
{
  switch ( mode ) {
  case MED_EN::RDONLY: return med_2_3::MED_LECTURE;
  case MED_EN::WRONLY: return med_2_3::MED_CREATION;
  case MED_EN::RDWR:   return med_2_3::MED_LECTURE_ECRITURE;
  default:
    return med_2_3::med_mode_acces( mode );
  }
}
