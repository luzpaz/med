#include "GROUPClient.hxx"
#include "MESHClient.hxx"

GROUPClient::GROUPClient(const SALOME_MED::GROUP_ptr G,
			 MESH * M) 
  : SUPPORTClient(G, M),
    GROUP(),
    IOR_Group(SALOME_MED::GROUP::_duplicate(G))
{
  BEGIN_OF("GROUPClient::GROUPClient()");

  SCRUTE(G);
  SCRUTE(M);

  END_OF("GROUPClient::GROUPClient()");
}


void GROUPClient::blankCopy()
{
  BEGIN_OF("GROUPClient::blankCopy()");

  SUPPORTClient::blankCopy();
  _complete = false;

  END_OF("GROUPClient::blankCopy()");
}

void GROUPClient::fillCopy()
{
  BEGIN_OF("GROUPClient::fillCopy()");

  SUPPORTClient::fillCopy();
  _complete = true;

  END_OF("GROUPClient::fillCopy()");
}


GROUPClient::~GROUPClient()
{
  BEGIN_OF("GROUPClient::~GROUPClient()");
  END_OF("GROUPClient::~GROUPClient()");
}
