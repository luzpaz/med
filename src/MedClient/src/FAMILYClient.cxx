#include "FAMILYClient.hxx"
#include "MESHClient.hxx"
#include "UtilClient.hxx"

FAMILYClient::FAMILYClient(const SALOME_MED::FAMILY_ptr F,
			   MESH * M) 
  : SUPPORTClient(F, M), 
    FAMILY(), 
    IOR_Family(SALOME_MED::FAMILY::_duplicate(F))
{
  BEGIN_OF("FAMILYClient::FAMILYClient(SALOME_MED::FAMILY_ptr m)");

  SCRUTE(F);
  SCRUTE(M);

  blankCopy(false);

  END_OF("FAMILYClient::FAMILYClient(SALOME_MED::FAMILY_ptr m)");
}

void FAMILYClient::blankCopy(bool blankSupport)
{
  BEGIN_OF("FAMILYClient::blankCopy()");

  if (blankSupport)
    SUPPORTClient::blankCopy();

  MESSAGE("");

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


FAMILYClient::~FAMILYClient()
{
  BEGIN_OF("FAMILYClient::~FAMILYClient()");
  END_OF("FAMILYClient::~FAMILYClient()");
}
