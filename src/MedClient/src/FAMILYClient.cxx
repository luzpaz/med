#include "FAMILYClient.hxx"
#include "MESHClient.hxx"
#include "UtilClient.hxx"
using namespace MEDMEM;
//=============================================================================
/*!
 * Constructeur
 */
//=============================================================================
FAMILYClient::FAMILYClient(const SALOME_MED::FAMILY_ptr S,
			     MESH * M) 
  : SUPPORTClient(S, M), 
    FAMILY(), 
    IOR_Family(SALOME_MED::FAMILY::_duplicate(S))
{
  BEGIN_OF("FAMILYClient::FAMILYClient(SALOME_MED::FAMILY_ptr m)");

  SCRUTE(S);
  SCRUTE(M);

  blankCopy(false);

  END_OF("FAMILYClient::FAMILYClient(SALOME_MED::FAMILY_ptr m)");
}
//=============================================================================
/*!
 * Remplit les informations générales
 */
//=============================================================================
void FAMILYClient::blankCopy(bool blankSupport)
{
  BEGIN_OF("FAMILYClient::blankCopy()");

  if (blankSupport)
    SUPPORTClient::blankCopy();

//   setIdentifier(IOR_Family->getIdentifier());

//   int nAttr, nGr, n;
//   int *i;
//   std::string *s;

//   nAttr = IOR_Family->getNumberOfAttributes();
//   setNumberOfAttributes(nAttr);

//   convertCorbaArray(i, n, IOR_Family->getAttributesIdentifiers());
//   ASSERT(n == nAttr);
//   setAttributesIdentifiers(i);

//   convertCorbaArray(i, n, IOR_Family->getAttributesValues());
//   ASSERT(n == _numberOfAttribute);
//   setAttributesValues(i);

//   convertCorbaArray(s, n, IOR_Family->getAttributesDescriptions());
//   ASSERT(n == _numberOfAttribute);
//   setAttributesDescriptions(s);

//   nGr = IOR_Family->getNumberOfGroups();
//   setNumberOfGroups(nGr);

//   convertCorbaArray(s, n, IOR_Family->getGroupsNames());
//   ASSERT(n == _numberOfAttribute);
//   setGroupsNames(s);

  _complete = false;

  END_OF("FAMILYClient::blankCopy()");
}

//=============================================================================
/*!
 * Remplit les informations générales
 */
//=============================================================================
void FAMILYClient::fillCopy(bool fillSupport)
{
  BEGIN_OF("FAMILYClient::fillCopy()");

  if (!_complete) {

    if (fillSupport)
      SUPPORTClient::fillCopy();
    
    _complete = true;
  }

  END_OF("FAMILYClient::fillCopy()");
}

//=============================================================================
/*!
 * Destructeur
 */
//=============================================================================

FAMILYClient::~FAMILYClient()
{
  BEGIN_OF("FAMILYClient::~FAMILYClient()");
  END_OF("FAMILYClient::~FAMILYClient()");
}
