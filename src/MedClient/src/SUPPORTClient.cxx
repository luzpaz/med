#include "UtilClient.hxx"
#include "SUPPORTClient.hxx"
#include "MESHClient.hxx"

SUPPORTClient::SUPPORTClient(const SALOME_MED::SUPPORT_ptr S,
			     MESH * M) : 
  SUPPORT(), 
  IOR_Support(SALOME_MED::SUPPORT::_duplicate(S))
{
  BEGIN_OF("SUPPORTClient::SUPPORTClient(SALOME_MED::SUPPORT_ptr m)");

  SCRUTE(S);
  SCRUTE(M);

  setMesh(M ? M : new MESHClient(IOR_Support->getMesh()));

  blankCopy();

  END_OF("SUPPORTClient::SUPPORTClient(SALOME_MED::SUPPORT_ptr m)");
}

void SUPPORTClient::blankCopy()
{
  BEGIN_OF("SUPPORTClient::blankCopy");

  setName(IOR_Support->getName());
  setDescription(IOR_Support->getDescription());
  setAll(IOR_Support->isOnAllElements());
  setEntity(IOR_Support->getEntity());

  convertCorbaArray(_geometricType, _numberOfGeometricType,
		    IOR_Support->getTypes());

  int *nE = new int[_numberOfGeometricType];
  int i;
  for (i=0; i<_numberOfGeometricType; i++)
    nE[i] = IOR_Support->getNumberOfElements(_geometricType[i]);
  setNumberOfElements(nE);
  SCRUTE(_totalNumberOfElements);

  SCRUTE(_name);
  SCRUTE(_description);

  _complete_support = false;

  END_OF("SUPPORTClient::blankCopy");

}

void SUPPORTClient::fillCopy()
{
  BEGIN_OF("SUPPORTClient::fillCopy");

  int * index, * value;
  long n_index, n_value;
  
  MESSAGE("");
  try {
    convertCorbaArray(value, n_value, 
		      IOR_Support->getNumber(MED_ALL_ELEMENTS));
  }
  catch (...) {
    std::cerr << "erreur ...." << std::endl;
    exit(-1);
  }

  SCRUTE(n_value);
  SCRUTE(value[0]);
  SCRUTE(value[1]);
  MESSAGE("");

  try {
    convertCorbaArray(index, n_index, 
		      IOR_Support->getNumberIndex());
  }
  catch (...) {
    std::cerr << "erreur ...." << std::endl;
    exit(-1);
  }

  SCRUTE(n_index);
  SCRUTE(n_value);
  setNumber(index, value);
  
  delete [] index;
  delete [] value;
  
  _complete_support = true;

  END_OF("SUPPORTClient::fillCopy");
}


SUPPORTClient::~SUPPORTClient()
{
  BEGIN_OF("SUPPORTClient::~SUPPORTClient");

  END_OF("SUPPORTClient::~SUPPORTClient");
}

MEDSKYLINEARRAY *  SUPPORTClient::getnumber() const throw (MEDEXCEPTION)
{
  BEGIN_OF("SUPPORTClient::getnumber()");

  if (!_complete_support) (const_cast<SUPPORTClient *>(this))->fillCopy();
  MEDSKYLINEARRAY *m = SUPPORT::getnumber();

  END_OF("SUPPORTClient::getnumber()");
  return m;
}

const int *  SUPPORTClient::getNumber(medGeometryElement GeometricType) 
    const throw (MEDEXCEPTION)
{
  BEGIN_OF("SUPPORTClient::getnumber(medGeometryElement)");

  if (!_complete_support) (const_cast<SUPPORTClient *>(this))->fillCopy();
  const int *n = SUPPORT::getNumber(GeometricType);

  END_OF("SUPPORTClient::getnumber(medGeometryElement)");
  return n;
}

const int *  SUPPORTClient::getNumberIndex() const throw (MEDEXCEPTION) 
{
  BEGIN_OF("SUPPORTClient::getnumberIndex()");

  if (!_complete_support) (const_cast<SUPPORTClient *>(this))->fillCopy();
  const int * n = SUPPORT::getNumberIndex();

  END_OF("SUPPORTClient::getnumberIndex()");
  return n;
}

