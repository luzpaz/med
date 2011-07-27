//  Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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
  IOR_Mesh(SALOME_MED::MESH::_duplicate(m)),
  _numberOfElements_client(0),
  _complete(false)
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

  long iT, nTwithPoly, nT=getNumberOfTypes(Entity);
  convertCorbaArray<MED_EN::medGeometryElement,SALOME_MED::medGeometryElement_array *, long>
    (Types, nTwithPoly, &all->meshTypes);

  //ASSERT(nT == (int) getNumberOfTypes(Entity)); FALSE with POLY
  SCRUTE(nT);
  
  setGeometricTypes(Types, Entity);

  _totalNumberOfElements_client = 0L;
  if (_numberOfElements_client)
    delete [] _numberOfElements_client;
  _numberOfElements_client = new long[nTwithPoly];
  for (iT=0; iT<nT; iT++) 
   {
    _numberOfElements_client[iT] =  all->numberOfElements[iT];
    _totalNumberOfElements_client += _numberOfElements_client[iT];
    SCRUTE(iT);
    SCRUTE(_numberOfElements_client[iT]);
  }

  if ( nT != nTwithPoly )
  {
    _numberOfElements_client[iT] = all->numberOfElements[iT];
    _polyType_client = Types[iT];
  }
  else
  {
    _polyType_client = MED_EN::MED_NONE;
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
      
      SALOME::SenderInt_var senderForConnectivity=IOR_Mesh->getSenderForConnectivity(MED_FULL_INTERLACE, MED_NODAL, Entity, T[iT]);
      pC=ReceiverFactory::getValue(senderForConnectivity,nC);
      SCRUTE(nC);
      ASSERT(nC == (T[iT]%100) * (Count[iT+1]-Count[iT]));
      
      setNodal(pC, Entity, T[iT]);
      delete [] pC;
    }

    if ( _polyType_client == MED_POLYGON )
    {
      const medConnectivity ConType = MED_NODAL;
      SALOME::SenderInt_var senderForConnectivity=IOR_Mesh->getSenderForPolygonsConnectivity (ConType, Entity);
      SALOME::SenderInt_var senderForConnectivityIndex=IOR_Mesh->getSenderForPolygonsConnectivityIndex (ConType, Entity);

      pC=ReceiverFactory::getValue(senderForConnectivity,nC);
      long nP;
      int * pCI=ReceiverFactory::getValue(senderForConnectivityIndex,nP);
//       dumpArray( pC, nC, "POLYGON Connectivity: ");
//       dumpArray( pCI, nP, "POLYGON ConnectivityIndex: ");

      setPolygonsConnectivity(ConType, Entity, pC, pCI, nC, nP-1);
      delete [] pC;
      delete [] pCI;
    }

    if ( _polyType_client == MED_POLYHEDRA )
    {
      const medConnectivity ConType = MED_NODAL;
      SALOME::SenderInt_var senderForConnectivity=IOR_Mesh->getSenderForPolyhedronConnectivity (ConType);
      SALOME::SenderInt_var senderForPolyhedronIndex=IOR_Mesh->getSenderForPolyhedronIndex (ConType);
      SALOME::SenderInt_var senderForPolyhedronFacesIndex=IOR_Mesh->getSenderForPolyhedronFacesIndex ();

      pC=ReceiverFactory::getValue(senderForConnectivity,nC);
      long nP, nF;
      int * pPI=ReceiverFactory::getValue(senderForPolyhedronIndex,nP);
      int * pFI=ReceiverFactory::getValue(senderForPolyhedronFacesIndex,nF);

//       dumpArray( pC, nC, "POLYHedron Connectivity: ");
//       dumpArray( pFI, nF, "POLYHedron Face Index: ");
//       dumpArray( pPI, nP, "POLYHedron Index: ");

      setPolyhedronConnectivity(ConType, pC, pPI, nC, nP-1, pFI, nF-1);
      delete [] pC;
      delete [] pPI;
      delete [] pFI;
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
const int * CONNECTIVITYClient::getConnectivity
                      (medConnectivity ConnectivityType, 
                       medEntityMesh Entity,
                       medGeometryElement Type) const
{
  if (!_complete)
    (const_cast<CONNECTIVITYClient *>(this))->fillCopy();

  const int * c = CONNECTIVITY::getConnectivity
    (ConnectivityType, Entity, Type);

  return c;
}

//=============================================================================
/*!
 */
//=============================================================================
const int * CONNECTIVITYClient::getConnectivityIndex
                      (medConnectivity ConnectivityType,
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
void CONNECTIVITYClient::calculateConnectivity
                      (medConnectivity connectivityType, 
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
const int * CONNECTIVITYClient::getGlobalNumberingIndex 
                      (medEntityMesh Entity) const throw (MEDEXCEPTION)
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
const int * CONNECTIVITYClient::getReverseConnectivity
                      (medConnectivity ConnectivityType, 
                       medEntityMesh Entity) const throw (MEDEXCEPTION)
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
const int * CONNECTIVITYClient::getReverseConnectivityIndex
                      (medConnectivity ConnectivityType,
                       medEntityMesh Entity) const throw (MEDEXCEPTION)
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

//=======================================================================
//function : existPolygonsConnectivity
//purpose  : 
//=======================================================================

bool CONNECTIVITYClient::existPolygonsConnectivity(medConnectivity connectivityType,
                                                   medEntityMesh   Entity) const
{
  if (!_complete)
    (const_cast<CONNECTIVITYClient *>(this))->fillCopy();

  bool b = CONNECTIVITY::existPolygonsConnectivity( connectivityType, Entity );

  return b;
}

//=======================================================================
//function : existPolyhedronConnectivity
//purpose  : 
//=======================================================================

bool CONNECTIVITYClient::existPolyhedronConnectivity(medConnectivity connectivityType,
                                                     medEntityMesh   Entity) const
{
  if (!_complete)
    (const_cast<CONNECTIVITYClient *>(this))->fillCopy();

  bool b = CONNECTIVITY::existPolyhedronConnectivity( connectivityType, Entity );

  return b;
}

//=======================================================================
//function : getPolygonsConnectivity
//purpose  : 
//=======================================================================

const int* CONNECTIVITYClient::getPolygonsConnectivity(medConnectivity ConnectivityType,
                                                       medEntityMesh Entity) const
{
  if (!_complete)
    (const_cast<CONNECTIVITYClient *>(this))->fillCopy();

  const int * c = CONNECTIVITY::getPolygonsConnectivity (ConnectivityType, Entity);

  return c;
}

//=======================================================================
//function : getPolygonsConnectivityIndex
//purpose  : 
//=======================================================================

const int* CONNECTIVITYClient::getPolygonsConnectivityIndex(medConnectivity ConnectivityType,
                                                            medEntityMesh Entity) const
{
  if (!_complete)
    (const_cast<CONNECTIVITYClient *>(this))->fillCopy();

  const int * c = CONNECTIVITY::getPolygonsConnectivityIndex (ConnectivityType, Entity);

  return c;
}

//=======================================================================
//function : getPolyhedronConnectivity
//purpose  : 
//=======================================================================

const int* CONNECTIVITYClient::getPolyhedronConnectivity(medConnectivity ConnectivityType) const
{
  if (!_complete)
    (const_cast<CONNECTIVITYClient *>(this))->fillCopy();

  const int * c = CONNECTIVITY::getPolyhedronConnectivity (ConnectivityType);

  return c;
}

//=======================================================================
//function : getPolyhedronFacesIndex
//purpose  : 
//=======================================================================

const int* CONNECTIVITYClient::getPolyhedronFacesIndex() const
{
  if (!_complete)
    (const_cast<CONNECTIVITYClient *>(this))->fillCopy();

  const int * c = CONNECTIVITY::getPolyhedronFacesIndex();

  return c;
}

//=======================================================================
//function : getPolyhedronIndex
//purpose  : 
//=======================================================================

const int* CONNECTIVITYClient::getPolyhedronIndex(medConnectivity ConnectivityType) const
{
  if (!_complete)
    (const_cast<CONNECTIVITYClient *>(this))->fillCopy();

  const int * c = CONNECTIVITY::getPolyhedronIndex (ConnectivityType);

  return c;
}

//=======================================================================
//function : getNumberOfPolygons
//purpose  : 
//=======================================================================

int CONNECTIVITYClient::getNumberOfPolygons() const
{
  int n = 0;

  if (!_complete) {
    if ( _polyType_client == MED_POLYGON )
      n = _numberOfElements_client[ getNumberOfTypes( _entity )];
  }
  else
    n = CONNECTIVITY::getNumberOfPolygons();

  SCRUTE(n);

  return n;
}

//=======================================================================
//function : getNumberOfPolyhedronFaces
//purpose  : 
//=======================================================================

int CONNECTIVITYClient::getNumberOfPolyhedronFaces() const
{
  if (!_complete)
    (const_cast<CONNECTIVITYClient *>(this))->fillCopy();

  int n = CONNECTIVITY::getNumberOfPolyhedronFaces ();

  SCRUTE(n);

  return n;
}

//=======================================================================
//function : getNumberOfPolyhedron
//purpose  : 
//=======================================================================

int CONNECTIVITYClient::getNumberOfPolyhedron() const
{
  int n = 0;

  if (!_complete) {
    if ( _polyType_client == MED_POLYHEDRA )
      n = _numberOfElements_client[ getNumberOfTypes( _entity )];
  }
  else
    n = CONNECTIVITY::getNumberOfPolyhedron();

  SCRUTE(n);

  return n;
}

