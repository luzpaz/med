// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "MEDMEM_convert.hxx"
#include "Utils_CorbaException.hxx"
#include "UtilClient.hxx"
#include "CONNECTIVITYClient.hxx"

#include "ReceiverFactory.hxx"
using namespace MEDMEM;
//=============================================================================
/*!
 * Constructeur
 */
//=============================================================================

CONNECTIVITYClient::CONNECTIVITYClient(const SALOME_MED::MESH_ptr m,
                                       medEntityMesh Entity) :
  CONNECTIVITY(m->getNumberOfTypes(Entity), Entity),
  _numberOfElements_client(0),
  _complete(false),
  IOR_Mesh(SALOME_MED::MESH::_duplicate(m))
{
  ASSERT(m);

  blankCopy();
}

//=============================================================================
/*!
 * Destructeur
 */
//=============================================================================
CONNECTIVITYClient::~CONNECTIVITYClient()
{
  if (_numberOfElements_client)
    delete [] _numberOfElements_client;
}

//=============================================================================
/*!
 * Remplit les informations nécessaires
 * 1 seul appel Corba
 */
//=============================================================================
void CONNECTIVITYClient::blankCopy()
{
   SALOME_MED::MESH::connectivityInfos_var all;
   medEntityMesh Entity = getEntity();
   try
   {
        all= IOR_Mesh->getConnectGlobal(Entity);
   }
   catch (const exception & ex)
   {
        MESSAGE("Unable to acces Global information");
        THROW_SALOME_CORBA_EXCEPTION(ex.what() ,SALOME::INTERNAL_ERROR);
   }

  _numberOfNodes = all->numberOfNodes;
  _entityDimension = all->entityDimension;
  medGeometryElement * Types;

  long iT, nT;
  convertCorbaArray<MED_EN::medGeometryElement,SALOME_MED::medGeometryElement_array *, long>
    (Types, nT, &all->meshTypes);

  ASSERT(nT == (int) getNumberOfTypes(Entity));
  SCRUTE(nT);
  
  setGeometricTypes(Types, Entity);

  _totalNumberOfElements_client = 0L;
  if (_numberOfElements_client)
    delete [] _numberOfElements_client;
  _numberOfElements_client = new long[nT];
  for (iT=0; iT<nT; iT++) 
   {
    _numberOfElements_client[iT] =  all->numberOfElements[iT];
    _totalNumberOfElements_client += _numberOfElements_client[iT];
    SCRUTE(iT);
    SCRUTE(_numberOfElements_client[iT]);
  }

  if(Types)
    delete [] Types;

  // create a constituent (PAL10556)
// The consequence is that, if the remote server
// has not calculated nodal connectivity of dimension d-1, heavy method
// (CPU and memory) calculateDecsendingConnectivity is called on this
// server for a potentially useless information for client side . (by Anthony GEAY)
   if ( Entity == MED_CELL ) {
     Entity = ( IOR_Mesh->getMeshDimension() == 3 ? MED_FACE : MED_EDGE );
     if(_constituent)
       delete _constituent;
     _constituent = new CONNECTIVITYClient( IOR_Mesh, Entity );
   }

  _complete = false;
}

//=============================================================================
/*!
 */
//=============================================================================
// template< class T>
// void dumpArray(const T* array, int size, const char* msg)
// {
//   if ( msg )
//     std::cout << msg << " " << std::endl;
//   std::cout << "Size: " << size << std::endl;
//   for ( int i = 0; i < size; i++ )
//     std::cout << " " << array[ i ];
//   std::cout << endl;
// }

void CONNECTIVITYClient::fillCopy()
{
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
      SCRUTE(Count[iT+1]-Count[iT]);
      
      SALOME::SenderInt_var senderForConnectivity=IOR_Mesh->getSenderForConnectivity(MED_NODAL, Entity, T[iT]);
      pC=ReceiverFactory::getValue(senderForConnectivity,nC);
      SCRUTE(nC);
      ASSERT(nC == (T[iT]%100) * (Count[iT+1]-Count[iT]));

      int *index = 0;
      if ( T[iT] == MED_EN::MED_POLYGON || T[iT] == MED_EN::MED_POLYHEDRA )
      {
        SALOME::SenderInt_var senderForIndex=IOR_Mesh->getSenderForConnectivityIndex(MED_NODAL, Entity, T[iT]);
        index=ReceiverFactory::getValue(senderForIndex,nC);
        ASSERT(nC == (Count[iT+1]-Count[iT]+1));
      }

      setNodal(pC, Entity, T[iT], index);
      delete [] pC;
      delete [] index;
    }

    delete[] Count;
    if (_constituent)
      ((CONNECTIVITYClient *)_constituent)->fillCopy();
    _complete = true;
  }
}

//=============================================================================
/*!
 */
//=============================================================================
int CONNECTIVITYClient::getNumberOf(medEntityMesh Entity, 
                                    medGeometryElement Type) const
{
  int n = 0;

  SCRUTE(Type);
  SCRUTE(Entity);
  
  if (!_complete) {
    
    if (Entity == _entity) {
      
      if (Type==MED_ALL_ELEMENTS)
        n = _totalNumberOfElements_client;
      
      for (int i=0; i<_numberOfTypes; i++) {
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
  return n;
}

//=============================================================================
/*!
 */
//=============================================================================
const int * CONNECTIVITYClient::getConnectivity (medConnectivity ConnectivityType, 
                                                 medEntityMesh Entity,
                                                 medGeometryElement Type) const
{
  if (!_complete)
    (const_cast<CONNECTIVITYClient *>(this))->fillCopy();

  const int * c = CONNECTIVITY::getConnectivity (ConnectivityType, Entity, Type);

  return c;
}

int CONNECTIVITYClient::getConnectivityLength(medConnectivity    ConnectivityType,
                                              medEntityMesh      Entity,
                                              medGeometryElement Type) const
{
  if (!_complete)
    (const_cast<CONNECTIVITYClient *>(this))->fillCopy();

  const int l = CONNECTIVITY::getConnectivityLength (ConnectivityType, Entity, Type);

  return l;
}

//=============================================================================
/*!
 */
//=============================================================================
const int * CONNECTIVITYClient::getConnectivityIndex (medConnectivity ConnectivityType,
                                                      medEntityMesh Entity) const
{
  if (!_complete)
    (const_cast<CONNECTIVITYClient *>(this))->fillCopy();

  const int *c = CONNECTIVITY::getConnectivityIndex
    (ConnectivityType, Entity);

  return c;
}

//=============================================================================
/*!
 */
//=============================================================================
void CONNECTIVITYClient::calculateConnectivity (medConnectivity connectivityType, 
                                                medEntityMesh Entity)
{
  if (!_complete)
    fillCopy();

  CONNECTIVITY::calculateConnectivity(connectivityType, Entity);
}

//=============================================================================
/*!
 */
//=============================================================================
void  CONNECTIVITYClient::updateFamily (vector<FAMILY*> myFamilies)
{
  if (!_complete)
    fillCopy();

  CONNECTIVITY::updateFamily(myFamilies);
}

//=============================================================================
/*!
 */
//=============================================================================
const int * CONNECTIVITYClient::getGlobalNumberingIndex (medEntityMesh Entity) const
  throw (MEDEXCEPTION)
{
  if (!_complete)
    (const_cast<CONNECTIVITYClient *>(this))->fillCopy();

  const int * index = CONNECTIVITY::getGlobalNumberingIndex(Entity);

  return index;
}

//=============================================================================
/*!
 */
//=============================================================================
bool CONNECTIVITYClient::existConnectivity(medConnectivity ConnectivityType, 
                                           medEntityMesh Entity) const
{ 
  if (!_complete)
    (const_cast<CONNECTIVITYClient *>(this))->fillCopy();

  bool b = CONNECTIVITY::existConnectivity(ConnectivityType, Entity);

  return b;
}

//=============================================================================
/*!
 */
//=============================================================================
const int * CONNECTIVITYClient::getReverseConnectivity (medConnectivity ConnectivityType, 
                                                        medEntityMesh Entity) const
  throw (MEDEXCEPTION)
{
  if (!_complete)
    (const_cast<CONNECTIVITYClient *>(this))->fillCopy();

  const int *c = CONNECTIVITY::getReverseConnectivity
    (ConnectivityType, Entity);

  return c;
}

//=============================================================================
/*!
 */
//=============================================================================
const int * CONNECTIVITYClient::getReverseConnectivityIndex (medConnectivity ConnectivityType,
                                                             medEntityMesh Entity) const
  throw (MEDEXCEPTION)
{
  if (!_complete)
    (const_cast<CONNECTIVITYClient *>(this))->fillCopy();

  const int *c =  CONNECTIVITY::getReverseConnectivityIndex
    (ConnectivityType, Entity);

  return c;
}

//=============================================================================
/*!
 */
//=============================================================================
const int* CONNECTIVITYClient::getValue(medConnectivity TypeConnectivity, 
                                        medGeometryElement Type) const
{
  if (!_complete)
    (const_cast<CONNECTIVITYClient *>(this))->fillCopy();

  const int * c =  CONNECTIVITY::getValue(TypeConnectivity, Type);

  return c;
}

//=============================================================================
/*!
 */
//=============================================================================
const int* CONNECTIVITYClient::getValueIndex(medConnectivity TypeConnectivity) const
{
  if (!_complete)
    (const_cast<CONNECTIVITYClient *>(this))->fillCopy();

  const int * c =  CONNECTIVITY::getValueIndex(TypeConnectivity);

  return c;
}

//=============================================================================
/*!
 */
//=============================================================================
const int* CONNECTIVITYClient::getNeighbourhood() const
{
  if (!_complete)
    (const_cast<CONNECTIVITYClient *>(this))->fillCopy();

  const int * c =  CONNECTIVITY::getNeighbourhood();

  return c;
}
