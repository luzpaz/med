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
  med_2_2::med_idt fid22;

  med_2_1::med_err ret21;
  med_2_2::med_err ret22;

  med_2_1::med_int major21;
  med_2_1::med_int minor21;
  med_2_1::med_int release21;

  med_2_2::med_int major22;
  med_2_2::med_int minor22;
  med_2_2::med_int release22;

  med_2_2::med_mode_acces access22 = med_2_2::MED_LECTURE;
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
      MESSAGE("getMedFileVersion the file may have been produced by a version 2.1.x x<5");
      minor21 = 1;
      release21 = 5;
    }

  ret21 = med_2_1::MEDfermer(fid21);

  if (ret21 < 0)
    throw MEDEXCEPTION("Problem in getMedFileVersion(const string &) Med file V2.1 file closing");

  /*
    Med Version 2.2 access to the file
  */

  fid22 = med_2_2::MEDouvrir((const_cast <char *> (fileName.c_str())),
			     access22);

  if (fid22 < 0)
    throw MEDEXCEPTION("Problem in getMedFileVersion(const string &) Med file V2.2 access");

  ret22 = med_2_2::MEDversionLire(fid22,&major22,&minor22,&release22);

  if (ret22 < 0)
    throw MEDEXCEPTION("Problem in getMedFileVersion(const string &) Med file V2.2 version numbers reading");

  if ((minor22 == -1) || (release22 == -1))
    {
      MESSAGE("getMedFileVersion the file may have been produced by a version 2.1.x x<5");
      minor22 = 1;
      release22 = 5;
    }

  ret22 = med_2_2::MEDfermer(fid22);

  if (ret22 < 0)
    throw MEDEXCEPTION("Problem in getMedFileVersion(const string &) Med file V2.2 file closing");

  if ((major21 != major22) || (minor21 != minor22) || (release21 != release22))
    throw MEDEXCEPTION("Problem in getMedFileVersion(const string &) Med file V21 and V22 version numbers are different");

  MESSAGE("getMedFileVersion: status version 21 of the file major " << major21 << " minor " << minor21 << " release " << release21);

  MESSAGE("getMedFileVersion: status version 22 of the file major " << major22 << " minor " << minor22 << " release " << release22);

  if (major21 == 2)
    {
      if (minor21 == 1)
	version = V21;
      else if (minor21 > 1)
	version = V22;
    }
  else
    version = V22;

  MESSAGE("getMedFileVersion the version of the file is " << version);

  return version;
}
