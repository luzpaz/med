#include <convert.hxx>
#include "UtilClient.hxx"
#include "CONNECTIVITYClient.hxx"

CONNECTIVITYClient::CONNECTIVITYClient(const SALOME_MED::MESH_ptr m,
				       medEntityMesh Entity) :
  CONNECTIVITY(m->getNumberOfTypes(Entity), Entity),
  IOR_Mesh(SALOME_MED::MESH::_duplicate(m)),
  _numberOfElements_client(0),
  _complete(false)
{
  BEGIN_OF("CONNECTIVITYClient::CONNECTIVITYClient()");

  ASSERT(m);

  blankCopy();
  
  END_OF("CONNECTIVITYClient::CONNECTIVITYClient()");
}

CONNECTIVITYClient::~CONNECTIVITYClient()
{
  BEGIN_OF("CONNECTIVITYClient::~CONNECTIVITYClient()");

  if (_numberOfElements_client)
    delete [] _numberOfElements_client;

  END_OF("CONNECTIVITYClient::~CONNECTIVITYClient()");
}

void CONNECTIVITYClient::blankCopy()
{
  BEGIN_OF("CONNECTIVITYClient::blankCopy()");

  _numberOfNodes = IOR_Mesh->getNumberOfNodes();
  
  medEntityMesh Entity = getEntity();
  medGeometryElement * Types;

  long iT, nT;
  convertCorbaArray<SALOME_MED::medGeometryElement>
    (Types, nT, IOR_Mesh->getTypes(Entity));

  ASSERT(nT == (int) getNumberOfTypes(Entity));
  SCRUTE(nT);
  
  setGeometricTypes(Types, Entity);
  
  _totalNumberOfElements_client = 0L;
  _numberOfElements_client = new long[nT];
  for (iT=0; iT<nT; iT++) {
    _numberOfElements_client[iT] 
      = IOR_Mesh->getNumberOfElements(Entity, Types[iT]);
    _totalNumberOfElements_client += _numberOfElements_client[iT];
    SCRUTE(iT);
    SCRUTE(_numberOfElements_client[iT]);
  }

  _complete = false;

  END_OF("CONNECTIVITYClient::blankCopy()");
}


void CONNECTIVITYClient::fillCopy()
{
  BEGIN_OF("void CONNECTIVITYClient::fillCopy()");


  if (!_complete) {

    int *pC;
    long nC;
    
    medEntityMesh Entity = getEntity();
    int iT, nT = getNumberOfTypes(Entity);
    SCRUTE(nT);
    const medGeometryElement * T = getGeometricTypes(Entity);
    
    int * Count = new int[nT+1] ;
    Count[0]=1 ;
    SCRUTE(Count[0]);
    for (iT=0; iT<nT; iT++) {
      Count[iT+1]=Count[iT] + _numberOfElements_client[iT];
      SCRUTE(Count[iT+1]);
    }
    setCount(Count, Entity) ;
    
    for (iT=0; iT<nT; iT++) {
      
      SCRUTE(iT);
      int kT = Count[iT+1]-Count[iT];
      SCRUTE(kT);
      
      convertCorbaArray(pC, nC, IOR_Mesh->getConnectivity
			(MED_FULL_INTERLACE, MED_NODAL, Entity, T[iT]));
      SCRUTE(nC);
      ASSERT(nC == (T[iT]%100) * kT);
      
      setNodal(pC, Entity, T[iT]);
      delete [] pC;
    }
    
    delete[] Count;
    
    _complete = true;
  }

  END_OF("void CONNECTIVITYClient::fillCopy()");
}

med_int CONNECTIVITYClient::getNumberOf(medEntityMesh Entity, 
					medGeometryElement Type) const
{
  BEGIN_OF("void CONNECTIVITYClient::getNumberOf()");

  med_int n = 0;

  SCRUTE(Type);
  SCRUTE(Entity);
  
  if (!_complete) {
    
    if (Entity == _entity) {
      
      if (Type==MED_ALL_ELEMENTS)
	n = _totalNumberOfElements_client;
      
      for (med_int i=0; i<_numberOfTypes; i++) {
	SCRUTE(_geometricTypes[i]);
	if (_geometricTypes[i] == Type) {
	  n = _numberOfElements_client[i];
	  break;
	}
      }
    } 
    else if (_constituent != NULL)
      n = _constituent->getNumberOf(Entity,Type);
    
  }
  else
    n = CONNECTIVITY::getNumberOf(Entity, Type);

  SCRUTE(n);
  END_OF("void CONNECTIVITYClient::getNumberOf()");
  return n;
}

const med_int * CONNECTIVITYClient::getConnectivity
                      (medConnectivity ConnectivityType, 
		       medEntityMesh Entity,
		       medGeometryElement Type)
{
  BEGIN_OF("void CONNECTIVITYClient::getConnectivity()");

  if (!_complete)
    fillCopy();

  const med_int * c = CONNECTIVITY::getConnectivity
    (ConnectivityType, Entity, Type);

  END_OF("void CONNECTIVITYClient::getConnectivity()");
  return c;
}

const med_int * CONNECTIVITYClient::getConnectivityIndex
                      (medConnectivity ConnectivityType,
		       medEntityMesh Entity)
{
  BEGIN_OF("void CONNECTIVITYClient::getConnectivityIndex()");

  if (!_complete)
    fillCopy();

  const med_int *c = CONNECTIVITY::getConnectivityIndex
    (ConnectivityType, Entity);

  END_OF("void CONNECTIVITYClient::getConnectivityIndex()");
  return c;
}

void CONNECTIVITYClient::calculateConnectivity
                      (medConnectivity connectivityType, 
		       medEntityMesh Entity)
{
  BEGIN_OF("void CONNECTIVITYClient::calculateConnectivity()");

  if (!_complete)
    fillCopy();

  CONNECTIVITY::calculateConnectivity(connectivityType, Entity);

  END_OF("void CONNECTIVITYClient::calculateConnectivity()");
}

void  CONNECTIVITYClient::updateFamily (vector<FAMILY*> myFamilies)
{
  BEGIN_OF("void CONNECTIVITYClient::updateFamily()");

  if (!_complete)
    fillCopy();

  CONNECTIVITY::updateFamily(myFamilies);

  END_OF("void CONNECTIVITYClient::updateFamily()");
}

const int * CONNECTIVITYClient::getGlobalNumberingIndex 
                      (medEntityMesh Entity) const throw (MEDEXCEPTION)
{
  BEGIN_OF("void CONNECTIVITYClient::getGlobalNumberingIndex()");

  if (!_complete)
    (const_cast<CONNECTIVITYClient *>(this))->fillCopy();

  CONNECTIVITY::getGlobalNumberingIndex(Entity);

  END_OF("void CONNECTIVITYClient::getGlobalNumberingIndex()");
}

bool CONNECTIVITYClient::existConnectivity(medConnectivity ConnectivityType, 
					   medEntityMesh Entity) const
{ 
  BEGIN_OF("void CONNECTIVITYClient::existConnectivity()");

  if (!_complete)
    (const_cast<CONNECTIVITYClient *>(this))->fillCopy();

  bool b = CONNECTIVITY::existConnectivity(ConnectivityType, Entity);

  END_OF("void CONNECTIVITYClient::existConnectivity()");

  return b;
}

const med_int * CONNECTIVITYClient::getReverseConnectivity
                      (medConnectivity ConnectivityType, 
		       medEntityMesh Entity) throw (MEDEXCEPTION)
{
  BEGIN_OF("void CONNECTIVITYClient::getReverseConnectivity()");
  
  if (!_complete)
    fillCopy();

  const med_int *c = CONNECTIVITY::getReverseConnectivity
    (ConnectivityType, Entity);

  END_OF("void CONNECTIVITYClient::getReverseConnectivity()");

  return c;
}

const med_int * CONNECTIVITYClient::getReverseConnectivityIndex
                      (medConnectivity ConnectivityType,
		       medEntityMesh Entity) throw (MEDEXCEPTION)
{
  BEGIN_OF("void CONNECTIVITYClient::getReverseConnectivityIndex()");
  
  if (!_complete)
    fillCopy();

  const med_int *c =  CONNECTIVITY::getReverseConnectivityIndex
    (ConnectivityType, Entity);

  END_OF("void CONNECTIVITYClient::getReverseConnectivityIndex()");

  return c;
}

const med_int* CONNECTIVITYClient::getValue(medConnectivity TypeConnectivity, 
			medGeometryElement Type)
{
  BEGIN_OF("void CONNECTIVITYClient::getValue()");

  if (!_complete)
    fillCopy();

  const med_int * c =  CONNECTIVITY::getValue(TypeConnectivity, Type);

  END_OF("void CONNECTIVITYClient::()");

  return c;
}

const med_int* CONNECTIVITYClient::getValueIndex(medConnectivity TypeConnectivity)
{
  BEGIN_OF("void CONNECTIVITYClient::getValueIndex()");

  if (!_complete)
    fillCopy();

  const med_int * c =  CONNECTIVITY::getValueIndex(TypeConnectivity);

  END_OF("void CONNECTIVITYClient::getValueIndex()");

  return c;
}

const med_int* CONNECTIVITYClient::getNeighbourhood() const
{
  BEGIN_OF("void CONNECTIVITYClient::getNeighbourhood()");

  if (!_complete)
    (const_cast<CONNECTIVITYClient *>(this))->fillCopy();

  const med_int * c =  CONNECTIVITY::getNeighbourhood();

  END_OF("void CONNECTIVITYClient::getNeighbourhood()");

  return c;
}

