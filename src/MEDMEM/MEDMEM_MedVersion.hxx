#ifndef MED_VERSION_HXX
#define MED_VERSION_HXX

#include <string>
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

namespace MEDMEM {

  MED_EN::medFileVersion getMedFileVersion(const string & fileName)
    throw (MEDEXCEPTION);
}

#endif /* MED_VERSION_HXX */
